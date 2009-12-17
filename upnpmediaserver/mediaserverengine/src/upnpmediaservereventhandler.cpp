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
* Description:  Media Server event handler
*
*/


// INCLUDE FILES
#include "upnpmediaservereventhandler.h"
#include "upnpmediaserversession.h"
#include "upnpfiletransfereventlist.h"
#include "upnpfiletransferevent.h"
#include "upnpmediaserver.pan"
#include <s32mem.h>

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMediaServerEventHandler* CUpnpMediaServerEventHandler::NewL(CUpnpMediaServer* aServer)
    {
    CUpnpMediaServerEventHandler* self = CUpnpMediaServerEventHandler::NewLC(aServer); 
    CleanupStack::Pop( self ) ;
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMediaServerEventHandler* CUpnpMediaServerEventHandler::NewLC(CUpnpMediaServer* aServer)
    {
    CUpnpMediaServerEventHandler* self = new (ELeave) CUpnpMediaServerEventHandler(aServer);
    CleanupStack::PushL( self );
    self->ConstructL() ;
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::EventMediaServerSubscribeEventsL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::EventMediaServerSubscribeEvents( const RMessage2& aMessage )
    {    
    iMediaServer->SubscribeEvents(this);
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::EventMediaServerUnsubscribeEventsL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::EventMediaServerUnsubscribeEvents( const RMessage2& aMessage )
    {        
    if ( iListenRequestPending )
        {
        iCurrentMessage.Complete( KErrNone );
        }
    iMediaServer->UnsubscribeEvents(this);    
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::EventMediaServerServerListenTransferEventL
// Serves event listening request
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::EventMediaServerServerListenTransferEventL( const RMessage2& aMessage )
    {    
    iCurrentMessage = aMessage;
    
    if ( iFileTransferEventList->GetPointerArray().Count() > 0 )
        {   
        ListenTransferEventCompleteL();
        }
    else
        {
        iListenRequestPending = ETrue;
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::EventMediaServerGetTransferEventBodyL
// Sends event list body to client
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::EventMediaServerGetTransferEventBodyL( const RMessage2& aMessage )
    {
    aMessage.WriteL(0, *iResponseBuffer);
    aMessage.Complete( KErrNone );
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::CUpnpMediaServerEventHandler
// Constructor.
// -----------------------------------------------------------------------------
//
CUpnpMediaServerEventHandler::CUpnpMediaServerEventHandler(CUpnpMediaServer* aServer)
    :iMediaServer(aServer), iRespLengthPkg(iRespLength)
    {
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::~CUpnpMediaServerEventHandler()
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpMediaServerEventHandler::~CUpnpMediaServerEventHandler()
    {
    if(iMediaServer)
        {        
        iMediaServer->UnsubscribeEvents(this);
        }
    delete iResponseBuffer;
    delete iFileTransferEventList;
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::ConstructL()
    {
    iFileTransferEventList = CUpnpFileTransferEventList::NewL();
	}

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::FileTransferEvent
// File transfer event callback method.(from MUpnpContentDirectoryObserver)
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::FileTransferEvent( CUpnpFileTransferEvent *aEvent )
    {
    TRAP_IGNORE( ServeEventL( aEvent ) );
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::ServeEventL
// Serves incoming transfer event
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::ServeEventL(CUpnpFileTransferEvent *aEvent) 
    {
    iFileTransferEventList->GetPointerArray().AppendL( aEvent->CloneL() );
    if (iListenRequestPending)
        {
        ListenTransferEventCompleteL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventHandler::ListenTransferEventCompleteL
// Serialize events queue and sends buffer length to client
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventHandler::ListenTransferEventCompleteL()
    {
    iResponseBuffer = iFileTransferEventList->ToDes8L(); 
    iFileTransferEventList->GetPointerArray().ResetAndDestroy();
    iRespLength = iResponseBuffer->Des().Size();
    iCurrentMessage.WriteL(0, iRespLengthPkg);
    iCurrentMessage.Complete( KErrNone );
    iListenRequestPending = EFalse;
    }


  

