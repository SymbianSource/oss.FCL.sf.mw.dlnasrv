/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      Server core
*
*/






#include    <e32base.h>

#include    "mdhserver.h"
#include    "mdhcommon.h"
#include    "mdhserversession.h"
#include    "mdhmediaservercontainer.h"

#include    "msdebug.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCmMdhServer::CCmMdhServer
// constructor
// ---------------------------------------------------------------------------
//
CCmMdhServer::CCmMdhServer()
    : CPolicyServer( CActive::EPriorityHigh, KCmMdhServerPolicy )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::CCmMdhServer"));
    iServerState = ECmMdhServerStateIdle;
    }
    
// ---------------------------------------------------------------------------
// CCmMdhServer::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCmMdhServer::ConstructL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::ConstructL"));
    
    iSessionCount = 0;
    
    StartL( KCmMdhServerName );
    
    LOG(_L("[CmMdh Server]\t CCmMdhServer::ConstructL end"));
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::NewLC
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCmMdhServer* CCmMdhServer::NewLC()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::NewLC"));
    CCmMdhServer *self = new (ELeave) CCmMdhServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::~CCmMdhServer
// destructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCmMdhServer::~CCmMdhServer()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::~CCmMdhServer"));
    delete iMediaserverContainer;
    LOG(_L("[CmMdh Server]\t CCmMdhServer::~CCmMdhServer end"));
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhServer::CreateMediaserverContainerL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::CreateMediaserverContainerL"));
    
    if ( !iMediaserverContainer )
        {
        iMediaserverContainer = 
            CCmMdhMediaserverContainer::NewL( *this );
        LOG(_L("[CmMdh Server]\t MediaserverContainer created"));
        }
    
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::DestroyMediaserverContainer
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhServer::DestroyMediaserverContainer() 
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::DestroyMediaserverContainer"));
    
    if ( iMediaserverContainer )
        {
        delete iMediaserverContainer; iMediaserverContainer = NULL;
        LOG(_L("[CmMdh Server]\t MediaserverContainer destroyed"));
        }
    }
    
// ---------------------------------------------------------------------------
// CCmMdhServer::MediaserverContainer
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCmMdhMediaserverContainer& CCmMdhServer::MediaserverContainer() 
    {
    return *iMediaserverContainer;
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::NewSessionL
// Creates a new session to the server if version information is correct.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CSession2* CCmMdhServer::NewSessionL( const TVersion & aVersion, 
                                      const RMessage2 &/*aMessage*/) const
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::NewSessionL"));

#ifdef _DEBUG
    
    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE( Print(
        _L("[CmMdh Server]\t NewSessionL allocsize %d, size %d, cells %d"), 
        alloc, 
        size, 
        cells ) );
 
#endif
    
    // check we're the right version
    TVersion version( KCmMdhServerMajor, 
                      KCmMdhServerMinor, 
                      KCmMdhServerBuild);
                      
    if( !User::QueryVersionSupported( version, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    // make new session
    return CCmMdhSession::NewL( iMediaserverContainer, 
        *const_cast<CCmMdhServer*>(this) );
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::SetServerStateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhServer::SetServerStateL( TCmMdhServerState aServerState, 
                                     TInt aErrCode /* = KErrNone */)
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::SetServerState"));
    TRACE( Print( _L("[CmMdh Server]\t Setting server state from %d to %d"), 
               iServerState, aServerState));

    if ( iServerState == aServerState ) 
        {
        return;
        }
    
    TBool validTransition = EFalse;
    
    switch ( iServerState )
        {
        case ECmMdhServerStateIdle:
            if ( aServerState ) // search or harvest started
                {
                validTransition = ETrue;
                }
            break;
        case ECmMdhServerStateSearching:
            if ( aServerState == ECmMdhServerStateIdle ) // search complete
                {
                if ( aErrCode )
                    {
                    //DestroyMediaserverContainer();
                    }
                iCurrentAsyncMsg.Complete( aErrCode );
                validTransition = ETrue;    
                }
            break;
        case ECmMdhServerStateHarvesting:
            if ( aServerState == ECmMdhServerStateIdle ) // harvest complete
                {
                //DestroyMediaserverContainer();
                LOG(_L("[CmMdh Server]\t Completing..."));
                iCurrentAsyncMsg.Complete( aErrCode );
                LOG(_L("[CmMdh Server]\t Completed..."));
                validTransition = ETrue;
                if ( iSessionCount <= 0)
                    {
                    // shutdown server 
                    LOG(_L("[CmMdh Server]\t Harvest complete, \
                        shutting down.."));
                    CActiveScheduler::Stop();
                    }
                }
            break;
         default:
            // nothing
            break;
        }
    if (validTransition) 
        {
        iServerState = aServerState;   
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// CCmMdhServer::DecrementSessions
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhServer::DecrementSessions()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::DecrementSessions"));
    
    if ( --( iSessionCount) <= 0 
      && iServerState!= ECmMdhServerStateHarvesting ) 
      {
      LOG(_L("[CmMdh Server]\t Destroying media server container..."));
      DestroyMediaserverContainer();
      // shutdown server
      LOG(_L("[CmMdh Server]\t No sessions left, shutting down.."));
      CActiveScheduler::Stop();
      }
#ifdef _DEBUG
    
    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE( Print(
        _L(
        "[CmMdh Server]\t DecrementSessions allocsize %d, size %d, cells %d"
          ),
        alloc, 
        size, 
        cells )); 
#endif      
    }

// --------------------------------------------------------------------------
// CCmMdhServer::IncrementSessions
// --------------------------------------------------------------------------
void CCmMdhServer::IncrementSessions() 
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::IncrementSessions"))
    ++iSessionCount;    
    }
    
// ---------------------------------------------------------------------------
// CCmMdhServer::ServerState
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TCmMdhServerState CCmMdhServer::ServerState()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhServer::ServerState"));
    return iServerState;
    }
    
// ---------------------------------------------------------------------------
// CCmMdhServer::SetCurrentAsyncMsg
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhServer::SetCurrentAsyncMsg( const RMessage2& aMessage )
    {
    LOG(_L("[CmMdh Server]\t CCmServer::SetCurrentAsyncMsg"));
    iCurrentAsyncMsg = aMessage;
    }


// ========================== OTHER EXPORTED FUNCTIONS =======================

// ---------------------------------------------------------------------------
// PanicServer
// Server panic handler
// Panic our own thread
// ---------------------------------------------------------------------------
//
void PanicServer(TInt aPanic)
    {
    TRACE(Print(_L("[CmMdh Server]\t PanicServer: Reason = %d\n"), aPanic));
    User::Panic( KCmMdhServerPanic, aPanic );
    }

// ---------------------------------------------------------------------------
// PanicClient
// Client panic handler
// RMessage2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void PanicClient( const RMessage2& aMessage, TInt aPanic )
    {
    TRACE(Print(_L("[CmMdh Server]\t PanicClient: Reason = %d\n"), aPanic));
    aMessage.Panic( KCmMdhServerPanic, aPanic );
    }

// End of File
