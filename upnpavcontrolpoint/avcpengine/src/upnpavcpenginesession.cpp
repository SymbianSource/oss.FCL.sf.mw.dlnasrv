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
* Description:  CUpnpAVCPEngineSession
*
*/


// INCLUDE FILES
#include "upnpavcpenginesession.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"
#include "upnpcommand.h"



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::CContentControlSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngineSession::CUpnpAVCPEngineSession( const RThread& /*aClient*/, 
                                                CUpnpAVCPEngine& aAVCPEngine, 
                                                CUpnpAVCPManager& aManager)
    : CSession2(),
    iAVCPEngine( aAVCPEngine ),
    iAVCPManager( aManager )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngineSession::ConstructL()
    {    
    iAVCPEngine.IncrementSessions(this);
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::~CUpnpAVCPEngineSession
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngineSession::~CUpnpAVCPEngineSession()
    {
    iAVCPEngine.DecrementSessions(this);
    iCommandList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngineSession::ServiceL( const RMessage2& aMessage )
    {
	TInt index = KErrNotFound;
    switch ( aMessage.Function() )
        {
        // getting result from previous invoked command
        case EDownloadDeviceList:  
        case EDownloadDirList:                    
        case EDownloadMetadata:   
        	DEBUGSTRING(("Command for results with id %d", aMessage.Int0()));
            index = CommandById(aMessage.Int0());
            
            if (index != KErrNotFound) 
            {
                CUpnpCommand* resultCommand =  iCommandList[index];
                CleanupStack::PushL(resultCommand);
                resultCommand->SetResultL(aMessage);
                CleanupStack::PopAndDestroy(resultCommand);
                aMessage.Complete(KErrNone);
            }
            else 
            {
            	DEBUGSTRING(("Results not found %d", aMessage.Int0()));
                aMessage.Complete(KErrNotFound);
            }
            break;
        // all else commands
        default:
            CUpnpCommand* command = CUpnpCommand::NewL(iAVCPManager, *this, aMessage);            
            if (command) 
            {            	
                iCommandList.AppendL(command);
                CleanupStack::PushL(command);
                command->ExecuteL();
                CleanupStack::Pop(command); // it will be destroyed after completion
            }
            else 
            {                            
                PanicClient( aMessage, EAVCPEngineBadRequest );
            }
            break;
        }
        
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::RemoveCommand
// remove command
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngineSession::RemoveCommand(CUpnpCommand* aCommand) 
	{
    TInt index = iCommandList.Find(aCommand); 
    if (index != KErrNotFound) 
	    {
	    DEBUGSTRING8(("CUpnpAVCPEngineSession::RemoveCommand with index %d", index));
	    iCommandList.Remove(index); 
	    }
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::CommandById
//  Index of command with given id
// -----------------------------------------------------------------------------
//
TInt CUpnpAVCPEngineSession::CommandById(TInt aId) 
	{
    TInt i = KErrNotFound;

    for ( i=0; i < iCommandList.Count() && iCommandList[i]->Id() != aId; i++ )
    {}

    if ( i != iCommandList.Count() )
	    {
        return i;
	    }
    return KErrNotFound;   
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::PanicClient
//  Panic client
// -----------------------------------------------------------------------------
//
void CUpnpAVCPEngineSession::PanicClient( const RMessage2 &aMessage, TInt aPanic ) const
	{
	aMessage.Panic( KAVCPEngine, aPanic ) ; // Note: this panics the client thread, not server
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPEngineSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAVCPEngineSession* CUpnpAVCPEngineSession::NewL( 
                                             const RThread& aClient, 
                                             CUpnpAVCPEngine& aServer, 
                                             CUpnpAVCPManager& aManager )
    {
    CUpnpAVCPEngineSession* self = new (ELeave) CUpnpAVCPEngineSession( 
        aClient, aServer, aManager);
    CleanupStack::PushL( self );
    self->ConstructL() ;
    CleanupStack::Pop( self ) ;
    return self ;
    }


// End of File
