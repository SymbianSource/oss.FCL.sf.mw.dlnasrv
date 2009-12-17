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
* Description:  Media Server notification handler
*
*/


// INCLUDE FILES
#include "upnpmediaservernotifier.h"
#include "upnpfiletransferevent.h"
#include "upnpfiletransfereventlist.h"
#include "upnpmediaserverobserver.h"


// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::CUpnpMediaServerNotifier
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpMediaServerNotifier::CUpnpMediaServerNotifier( MUpnpMediaServerObserver* aObserver) :
                          CActive(EPriorityStandard),
                          iObserver(aObserver),
                          iRespBufSizePkg(iRespBufSize),
                          iReceiveBufferPtr(0,0)
    {
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::~CUpnpMediaServerNotifier()
// Destructor
// ---------------------------------------------------------------------------
//
CUpnpMediaServerNotifier::~CUpnpMediaServerNotifier()
    {
    iMediaServerClient.UnsubscribeEvents();
    Cancel();
    iMediaServerClient.Close();
    delete iReceiveBuffer;
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::ConstructL
// Two-phase constructor
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerNotifier::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError(iMediaServerClient.Connect());
    iMediaServerClient.SubscribeEvents( iStatus );
    iPendingAction = ESubscribe;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerNotifier::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMediaServerNotifier* CUpnpMediaServerNotifier::NewLC(
                                        MUpnpMediaServerObserver* aObserver )
    {
    CUpnpMediaServerNotifier* self = new( ELeave ) CUpnpMediaServerNotifier( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMediaServerNotifier* CUpnpMediaServerNotifier::NewL(
                                        MUpnpMediaServerObserver* aObserver  )
    {
    CUpnpMediaServerNotifier* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::RunL
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerNotifier::RunL()
    {
    if (iStatus.Int() < 0)
        {
        iObserver->NotifierError(iStatus.Int());
        return;
        }

    switch ( iPendingAction )
        {
        case ESubscribe:
            {
            //starts listening for single events list,
            //this request may wait for response for a long time
            iMediaServerClient.ListenTransferEvent(iRespBufSizePkg, iStatus);
            SetActive();
            iPendingAction = EListenEvent;
            break;
            }
        case EListenEvent:
            {
            //alocates buffer using size returned by ListenTransferEvent()
            delete iReceiveBuffer;
            iReceiveBuffer = NULL;
            iReceiveBuffer = HBufC8::NewL( iRespBufSize );
            iReceiveBufferPtr.Set( iReceiveBuffer->Des() );

            //requests events list body
            iMediaServerClient.GetTransferEventBody(iReceiveBufferPtr, iStatus);
            SetActive();
            iPendingAction = EGetEventBody;
            break;
            }
        case EGetEventBody:
            {
            //internalizes events list from buffer
            CUpnpFileTransferEventList* eventList = CUpnpFileTransferEventList::NewLC();
            RDesReadStream stream( iReceiveBufferPtr );
            CleanupClosePushL( stream );
            stream >> *eventList;
            CleanupStack::PopAndDestroy( &stream );

            //listens next events
            iMediaServerClient.ListenTransferEvent(iRespBufSizePkg, iStatus);
            SetActive();
            iPendingAction = EListenEvent;

            //notifies observer
            TInt count = eventList->GetPointerArray().Count();    
            for (TInt i=0; i<count; i++ )
                {            
                iObserver->FileTransferEvent( 
                    ( CUpnpFileTransferEvent* ) eventList->GetPointerArray()[0] );
                eventList->GetPointerArray().Remove(0);
                }
                
            CleanupStack::PopAndDestroy(eventList);
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerNotifier::DoCancel( )
    {
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CUpnpMediaServerNotifier::RunError(TInt aError)
    {
    iObserver->NotifierError(aError);
    return KErrNone;
    }








