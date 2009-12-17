/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpAVCPEngine
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "upnpavcpengine.h"
#include "upnpavcpenginesession.h"
#include "upnpavcpmanager.h"
#include "upnpavcpenginecommon.h"
#include "upnpmdebug.h"

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::CUpnpAVCPEngine
// constructor.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngine::CUpnpAVCPEngine( TInt aPriority )
    : CServer2( aPriority )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::~CUpnpAVCPEngine
// destructor.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngine::~CUpnpAVCPEngine()
    {   
    delete iAVCPManger;
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngine* CUpnpAVCPEngine::NewLC()
    {
    CUpnpAVCPEngine* engine = new (ELeave) CUpnpAVCPEngine( EPriorityNormal );
    CleanupStack::PushL( engine );
    engine->ConstructL() ;
    return engine;
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::ConstructL()
    {
     DEBUGSTRING(("CUpnpAVCPEngine::ConstructL "));
    iAVCPManger = CUpnpAVCPManager::NewL();
    StartL( KAVCPEngineName );
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CUpnpAVCPEngine::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
    {
    // Check it's the right version
    if ( !User::QueryVersionSupported( TVersion( 
            KAVCPEngineMajorVersionNumber,
            KAVCPEngineMinorVersionNumber,
            KAVCPEngineBuildVersionNumber ),
            aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

	RThread client;
	aMessage.Client(client); 
    return CUpnpAVCPEngineSession::NewL( client, *const_cast<CUpnpAVCPEngine*> ( this ), *const_cast<CUpnpAVCPManager*> (iAVCPManger) );
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::IncrementSessions
// Increment sessions.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::IncrementSessions(CUpnpAVCPEngineSession* /*aObserver*/)
    {    
    iSessionCount++;   
    DEBUGSTRING(("IncrementSessions (%d)", iSessionCount));
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::DecrementSessions
// Decrement sessions.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::DecrementSessions( CUpnpAVCPEngineSession* aObserver)
    {
    TRAP_IGNORE( iAVCPManger->CleanupSessionL(aObserver) );
    iSessionCount--; 
    DEBUGSTRING(("DecrementSessions (%d)", iSessionCount));    
    if ( iSessionCount <= 0 )
		{
		CActiveScheduler::Stop();
		}
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpAVCPEngine::RunError( TInt aError )
    {
    DEBUGSTRING(("CUpnpAVCPEngine::RunError (%d)", aError));    
    
    if ( aError == KErrBadDescriptor )
        {
        // A bad descriptor error implies a badly programmed client, so panic it;
        // otherwise report the error to the client
        PanicClient( Message(), /*EMessageHandlerBadDescriptor*/EAVCPEngineGenerealPanic );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::PanicClient( const RMessage2& aMessage, 
                                   TAVCPEnginePanic aPanic )
    {
    aMessage.Panic( KAVCPEngine, aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::PanicServer
// Panic server.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::PanicServer( TAVCPEnginePanic aPanic )
    {
    User::Panic( KAVCPEngine, aPanic );
    }


// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::ThreadFunctionL
// Create and start the server.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngine::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

	User::RenameThread( KAVCPEngineName );
    // Construct our server
    CUpnpAVCPEngine* engine = CUpnpAVCPEngine::NewLC();    // Anonymous

    RProcess::Rendezvous(KErrNone);

	// Start handling requests
	CActiveScheduler::Start();	

    CleanupStack::PopAndDestroy( engine );
	CleanupStack::PopAndDestroy( activeScheduler );

    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngine::ThreadFunctionL
// Create and start the server.
// -----------------------------------------------------------------------------
//
TInt CUpnpAVCPEngine::ThreadFunction( TAny* /*aNone*/ )
    {
    __UHEAP_MARK;
    
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !(cleanupStack) )
        {
        PanicServer( EAVCPEngineGenerealPanic );
        }

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        PanicServer( EAVCPEngineGenerealPanic );
        }

    delete cleanupStack;
    cleanupStack = NULL;

    __UHEAP_MARKEND;

    return KErrNone;
    }


// ============================= LOCAL FUNCTIONS ===============================

TInt E32Main()
    {
	return CUpnpAVCPEngine::ThreadFunction(NULL);
    }

// End of File
