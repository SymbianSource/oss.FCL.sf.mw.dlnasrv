/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Session implementation class for Metadata Harvester Server
*
*/






// INCLUDE FILES
#include    <e32base.h>
#include    <s32mem.h>
#include    "mdhserversession.h"
#include    "mdhcommon.h"
#include    "mdhserver.h"
#include    "mdhmediaservercontainer.h"

#include    "msdebug.h"


// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
//
CCmMdhSession* CCmMdhSession::NewL( 
    CCmMdhMediaserverContainer* aMediaserverContainer, CCmMdhServer& aServer )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhSession::NewL"));
    CCmMdhSession* self = 
        new (ELeave) CCmMdhSession( aMediaserverContainer, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCmMdhSession::CCmMdhSession( 
    CCmMdhMediaserverContainer* aMediaserverContainer, 
    CCmMdhServer& aServer ): iServer( aServer )
    {
        iMediaserverContainer = aMediaserverContainer;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
void CCmMdhSession::ConstructL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhSession::ConstructL"))
    iServer.IncrementSessions();
    }
            
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmMdhSession::~CCmMdhSession()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhSession::~CCmMdhSession"));
    iServer.DecrementSessions();
    }

// ---------------------------------------------------------------------------
// CCmMdhSession::Server
// Returns a reference to server
// ---------------------------------------------------------------------------
//
CCmMdhServer& CCmMdhSession::Server()
    {
    return *static_cast<CCmMdhServer*>
        (const_cast<CServer2*>(CSession2::Server()));
    }

// ---------------------------------------------------------------------------
// CCmMdhSession::CompleteIfBusy
// Completes message with busy error code if server is busy
// ---------------------------------------------------------------------------
TBool CCmMdhSession::CompleteIfBusy(const RMessage2& aMessage)
    {
    if (iServer.ServerState()) // operation already in progress
        {
        aMessage.Complete(KErrInUse);
        return ETrue;
        }
    return EFalse;    
    }

// ---------------------------------------------------------------------------
// CCmMdhSession::ServiceL
// Implements CSession2 -derived ServiceL -method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmMdhSession::ServiceL(const RMessage2& aMessage)
    {
    LOG(_L("[CmMdh Server]\t CCmMdhSession::ServiceL"));
    
    switch ( aMessage.Function() )
        {
        case ECmMdhSearchMediaservers:
            {
            LOG(_L("[CmMdh Server]\t ECmMdhSearchMediaServers request"));
            if ( !CompleteIfBusy(aMessage) ) 
                {
                iServer.SetCurrentAsyncMsg( aMessage );
                iServer.CreateMediaserverContainerL();
                iServer.MediaserverContainer().SearchMediaserversL();        
                }
            break;    
            }
        case ECmMdhHarvest:
            {
            LOG(_L("[CmMdh Server]\t ECmMdhHarvest request"));
            iServer.SetCurrentAsyncMsg( aMessage );
            iServer.CreateMediaserverContainerL();
            iServer.MediaserverContainer().HarvestMediaserversL();
            break;
            }
        case ECmMdhCancel:
            {
            LOG(_L("[CmMdh Server]\t ECmMdhCancel request"));
            switch ( iServer.ServerState() )
                {
                case ECmMdhServerStateSearching:
                    {
                    iServer.MediaserverContainer().CancelSearchL();
                    iServer.DestroyMediaserverContainer();
                
                    aMessage.Complete( KErrNone );
                    break;
                    }
                case ECmMdhServerStateHarvesting:
                    {
                    iServer.MediaserverContainer().CancelHarvestL();
                    iServer.DestroyMediaserverContainer();
                
                    aMessage.Complete( KErrNone );
                    break;
                    }
                default: 
                    {
                    aMessage.Complete( KErrNotReady );
                    break;
                    }
                }
            break;
            }
        default:
            {
            // Unknown opcode --> panic client
            PanicClient( aMessage, ECmMdhClientBadRequest );
            break;
            }
        }    
    }

// End of File
