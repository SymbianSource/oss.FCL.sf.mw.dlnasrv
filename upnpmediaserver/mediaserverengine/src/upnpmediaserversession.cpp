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
* Description:  Media Server Session
*
*/


// INCLUDE FILES
#include "upnpmediaserversession.h"
#include "upnpmediaservereventhandler.h"
#include "upnpmediaserver.pan"
#include "upnpitem.h"
#include "upnpicon.h"
#include "upnpcontainer.h"
#include "upnpitemlist.h"
#include "upnpcontainerlist.h"
#include "upnpbrowsecriteria.h"
#include "upnpmediaserverclient.h"
#include <xml/xmlparsererrors.h>
#include <s32mem.h>



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::CUpnpMediaServerSession
// -----------------------------------------------------------------------------
//
CUpnpMediaServerSession::CUpnpMediaServerSession(CUpnpMediaServer* aServer)
    : CSession2(), iRespLengthPkg(iRespLength), iRespExecStatusPkg(iRespExecStatus)
    {
    iMediaServer = aServer;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ConstructL()
    {
    iEventHandler = CUpnpMediaServerEventHandler::NewL(this->iMediaServer);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::~CUpnpMediaServerSession
// -----------------------------------------------------------------------------
//
CUpnpMediaServerSession::~CUpnpMediaServerSession()
    {
    delete iResponseBuffer;
    delete iEventHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ServiceL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ServiceL( const RMessage2& aMessage )
    {
    // first of all server must be started
    if( iMediaServer->Status() == RUpnpMediaServerClient::EStopped &&
        aMessage.Function() != EMediaServerStatus &&
        aMessage.Function() != EMediaServerStart &&
        aMessage.Function() != EMediaServerStartOffline &&
        aMessage.Function() != EMediaServerSubscribeEvents &&
        aMessage.Function() != EMediaServerUnsubscribeEvents &&
        aMessage.Function() != EMediaServerListenTransferEvent &&
        aMessage.Function() != EMediaServerGetTransferEventBody &&
        aMessage.Function() != EMediaServerStatusChange &&
        aMessage.Function() != EMediaServerStatusChangeCancel &&
        aMessage.Function() != EMediaServerAddIcon &&
        aMessage.Function() != EMediaServerRemoveIcon &&
        aMessage.Function() != EMediaServerRemoveIcons &&
        aMessage.Function() != EMediaServerUpdateIcon
        )
        {
        User::Leave( KErrNotReady );
        }

    switch ( aMessage.Function() )
        {
        case EMediaServerStatus:
            EventMediaServerStatusL( aMessage );
            break;
        case EMediaServerStartOffline:
            EventMediaServerStartOfflineL(aMessage);
            break;
        case EMediaServerStart:
            EventMediaServerStartL( aMessage );
            break;
        case EMediaServerStop:
            EventMediaServerStop( aMessage );
            break;
        case EMediaServerStopSilent:
            EventMediaServerStopSilent( aMessage );
            break;
        case EMediaServerRestart:
            EventMediaServerRestartL( aMessage );
            break;
        case EMediaServerShareItem:
            EventMediaServerShareItemL( aMessage );
            break;
        case EMediaServerShareItemResult:
            EventMediaServerGetShareItemResultL( aMessage );
            break;
        case EMediaServerShareContainer:
            EventMediaServerShareContainerL( aMessage );
            break;
        case EMediaServerShareContainerResult:
            EventMediaServerGetShareContainerResultL(aMessage);
            break;
        case EMediaServerShareItemList:
            EventMediaServerShareItemListL( aMessage );
            break;
        case EMediaServerShareItemListResult:
            EventMediaServerGetShareItemListResultL( aMessage );
            break;
        case EMediaServerUnshareItem:
            EventMediaServerUnshareItemL( aMessage );
            break;
        case EMediaServerUnshareRoot:
            EventMediaServerUnshareRootL( aMessage );
            break;
        case EMediaServerUnshareContainer:
            EventMediaServerUnshareContainerL( aMessage );
            break;
        case EMediaServerGetSharedItem:
            EventMediaServerGetSharedItemL( aMessage );
            break;
        case EMediaServerGetSharedItemResult:
            EventMediaServerGetGetSharedItemResultL( aMessage );
            break;
        case EMediaServerGetSharedContainer:
            EventMediaServerGetSharedContainerL( aMessage );
            break;
        case EMediaServerGetSharedContainerResult:
            EventMediaServerGetGetSharedContainerResultL( aMessage );
            break;
        case EMediaServerUnshareItemList:
            EventMediaServerUnshareItemListL( aMessage );
            break;
        case EMediaServerGetSharedItemList:
            EventMediaServerGetSharedItemListL(aMessage);
            break;
        case EMediaServerGetSharedItemListResult:
            EventMediaServerGetGetSharedItemListResultL(aMessage);
            break;
        case EMediaServerGetSharedContainerList:
            EventMediaServerGetSharedContainerListL(aMessage);
            break;
        case EMediaServerGetSharedContainerListResult:
            EventMediaServerGetGetSharedContainerListResultL(aMessage);
            break;
        case EMediaServerSubscribeEvents:
            iEventHandler->EventMediaServerSubscribeEvents(aMessage);
            break;
        case EMediaServerUnsubscribeEvents:
            iEventHandler->EventMediaServerUnsubscribeEvents(aMessage);
            break;
        case EMediaServerListenTransferEvent:
            iEventHandler->EventMediaServerServerListenTransferEventL(aMessage);
            break;
        case EMediaServerGetTransferEventBody:
            iEventHandler->EventMediaServerGetTransferEventBodyL(aMessage);
            break;
        case EMediaServerGetIpAndPort:
            EventMediaServerGetIpAndPortL(aMessage);
            break;
        case EMediaServerSetUploadDirectory:
            EventSetUploadDirectoryL( aMessage );
            break;
        case EMediaServerAddIcon:
            EventAddIconL( aMessage );
            break;
        case EMediaServerRemoveIcon:
            EventRemoveIconL( aMessage );
            break;
        case EMediaServerRemoveIcons:
            EventRemoveIconsL( aMessage );
            break;
        case EMediaServerUpdateIcon:
            EventUpdateIconL( aMessage );
            break;
        case EMediaServerShareReference:
            EventMediaServerShareReferenceL( aMessage );
            break;
        case EMediaServerShareReferenceList:
            EventMediaServerShareReferenceListL( aMessage );
            break;
        case EMediaServerStatusChange:
            EventMediaServerStatusChange( aMessage );
            break;   
        case EMediaServerStatusChangeCancel:
            EventMediaServerStatusChangeCancel( aMessage );
            break;    
        default :
            PanicClient( aMessage, EMediaServerBadRequest );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::PanicClient
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::PanicClient( const RMessage2 &aMessage, TInt aPanic ) const
    {
    static_cast<const CUpnpSymbianServerBase*>(  Server() )->PanicClient( aMessage, aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::NewL
// -----------------------------------------------------------------------------
//
CUpnpMediaServerSession* CUpnpMediaServerSession::NewL(CUpnpMediaServer* aServer)
    {
    CUpnpMediaServerSession* self = CUpnpMediaServerSession::NewLC(aServer);
    CleanupStack::Pop( self ) ;
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::NewLC
// -----------------------------------------------------------------------------
//
CUpnpMediaServerSession* CUpnpMediaServerSession::NewLC(CUpnpMediaServer* aServer)
    {
    CUpnpMediaServerSession* self = new (ELeave) CUpnpMediaServerSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL() ;
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStartL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStatusL( const RMessage2& aMessage )
    {
    TPckgBuf<TInt> status( iMediaServer->Status() );
    aMessage.WriteL( 0, status );
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStartL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStartL( const RMessage2& aMessage )
    {
    iMediaServer->CancelAVMediaServerStop();//if stop was scheduled - cancel it

    if ( iMediaServer->Status() != RUpnpMediaServerClient::EStartedOnline )
        {
        TRAPD( err, iMediaServer->StartAVMediaServerL() );

        if ( err == KErrNotFound // files not found
            || err == KErrArgument // empty or improper one of description files
            || (err >= EXmlParserError && err <= EXmlFeatureLockedWhileParsing) // description malformed
            || err == KErrNoMemory // problem with start because of memory usage
            )
            {
            iMediaServer->InitializeL( ETrue );
            iMediaServer->RestartAVMediaServerL();
            iMediaServer->StartAVMediaServerL();
            }
        else if (err == KErrCorrupt || err == KErrEof || err == KErrNotSupported)
            {
            iMediaServer->ResetDbL();
            iMediaServer->StartAVMediaServerL();
            }
        else
            {
            User::LeaveIfError(err);
            }
        }

    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStartOfflineL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStartOfflineL( const RMessage2& aMessage )
    {
    iMediaServer->CancelAVMediaServerStop();//if stop was scheduled cancel it
    if( iMediaServer->Status() != RUpnpMediaServerClient::EStartedOffline )
        {
        TRAPD(err, iMediaServer->StartAVMediaServerOfflineL());
         if ( err == KErrNotFound // files not found
            || err == KErrArgument // empty or improper one of description files
            || (err >= EXmlParserError && err <= EXmlFeatureLockedWhileParsing) // description malformed
            || err == KErrNoMemory // problem with start because of memory usage
            )
            {
            iMediaServer->InitializeL( ETrue );
            iMediaServer->StartAVMediaServerOfflineL();
            }
        else if (err == KErrCorrupt || err == KErrEof)
            {
            iMediaServer->ResetDbL();
            iMediaServer->StartAVMediaServerOfflineL();
            }
        else
            {
            User::LeaveIfError(err);
            }

        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStop
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStop( const RMessage2& aMessage )
    {
    if ( iMediaServer->Status() != RUpnpMediaServerClient::EStopped )
        {
        iMediaServer->StopAVMediaServer(EFalse);
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStopSilent
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStopSilent( const RMessage2& aMessage )
    {
    if ( iMediaServer->Status() != RUpnpMediaServerClient::EStopped )
        {
        iMediaServer->StopAVMediaServer(ETrue);
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerRestartL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerRestartL( const RMessage2& aMessage )
    {
    iMediaServer->RestartAVMediaServerL();
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetShareItemResultL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetShareItemResultL( const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetShareContainerResultL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetShareContainerResultL( const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerShareItemL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerShareItemL( const RMessage2& aMessage )
    {
    // get object from message
    CUpnpItem* item = CUpnpItem::NewL();
    CleanupStack::PushL(item);
    ReadObjFromMessageL(aMessage, 0, item);

    // perform action
    iMediaServer->ShareObjectL(item);

    // prepare response
    iResponseBuffer = item->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(item);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetSharedItemL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetSharedItemL(
                                                    const RMessage2& aMessage )
    {
    // perform action
    CUpnpItem* item = iMediaServer->GetSharedItemL(aMessage.Int0());
    CleanupStack::PushL(item);

    // prepare response
    iResponseBuffer = item->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(item);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetGetSharedItemResultL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetGetSharedItemResultL(
                                                    const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetSharedContainerL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetSharedContainerL(
                                                    const RMessage2& aMessage )
    {
    // perform action
    CUpnpContainer* cont = iMediaServer->GetSingleContainerL(aMessage.Int0());
    CleanupStack::PushL(cont);

    // prepare response
    iResponseBuffer = cont->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(cont);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetShareContainerListResultL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetGetSharedContainerResultL(
                                                    const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerShareContainerL
// Stops the server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerShareContainerL(
                                                    const RMessage2& aMessage )
    {
    // get object from message
    CUpnpContainer* cont = CUpnpContainer::NewL();
    CleanupStack::PushL(cont);
    ReadObjFromMessageL(aMessage, 0, cont);

    // perform action
    iMediaServer->ShareObjectL(cont);

    // prepare response
    iResponseBuffer = cont->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(cont);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetShareItemListResultL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetGetSharedItemListResultL(
                                                    const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.WriteL( 1, iRespExecStatusPkg );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetGetSharedContainerListResultL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetGetSharedContainerListResultL(
                                                    const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.WriteL( 1, iRespExecStatusPkg );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetShareItemListResultL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetShareItemListResultL( const RMessage2& aMessage )
    {
    ASSERT(aMessage.GetDesMaxLength(0) >= iRespLength);

    // Write result back to client
    aMessage.WriteL( 0, *iResponseBuffer );
    aMessage.WriteL( 1, iRespExecStatusPkg );
    aMessage.Complete( KErrNone );

    // clean up
    delete iResponseBuffer;
    iResponseBuffer = NULL;
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerUnshareRootL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerUnshareRootL(
                                                    const RMessage2& aMessage )
    {
    iMediaServer->UnshareRootL();
    aMessage.Complete( KErrNone );
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerUnshareItemL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerUnshareItemL(
                                                    const RMessage2& aMessage )
    {
    iMediaServer->UnshareItemL(aMessage.Int0());
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerUnshareContainerL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerUnshareContainerL(
                                                    const RMessage2& aMessage )
    {
    iMediaServer->UnshareContainerL(aMessage.Int0());
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetSharedItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetSharedItemListL(
                                                    const RMessage2& aMessage )
    {
    // get object from message
    CUpnpBrowseCriteria* bc = CUpnpBrowseCriteria::NewL();
    CleanupStack::PushL(bc);
    ReadObjFromMessageL(aMessage, 1, bc);

    // item list
    CUpnpItemList* il = CUpnpItemList::NewL();
    CleanupStack::PushL(il);

    // perform action
    iMediaServer->GetSharedItemListL(aMessage.Int0(), bc, &iRespExecStatus, il);

    // prepare response
    iResponseBuffer = il->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(2, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(il);
    CleanupStack::PopAndDestroy(bc);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetSharedContainerListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetSharedContainerListL(
                                                    const RMessage2& aMessage )
    {
    // get object from message
    CUpnpBrowseCriteria* bc = CUpnpBrowseCriteria::NewL();
    CleanupStack::PushL(bc);
    ReadObjFromMessageL(aMessage, 1, bc);

    // item list
    CUpnpContainerList* cl = CUpnpContainerList::NewL();
    CleanupStack::PushL(cl);

    // perform action
    iMediaServer->GetSharedContainerListL(aMessage.Int0(),
                                            bc, &iRespExecStatus, cl);

    // prepare response
    iResponseBuffer = cl->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(2, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(cl);
    CleanupStack::PopAndDestroy(bc);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerShareItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerShareItemListL(
                                                    const RMessage2& aMessage )
    {
    // get object from message
    CUpnpItemList* il = CUpnpItemList::NewL();
    CleanupStack::PushL(il);
    ReadObjFromMessageL(aMessage, 0, il);

    // perform action
    iMediaServer->ShareItemListL(il, &iRespExecStatus);

    // prepare response
    iResponseBuffer = il->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(il);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerUnshareItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerUnshareItemListL(
                                                    const RMessage2& aMessage )
    {
    // get ids from message
    RArray<TInt> il;
    CleanupClosePushL(il);
    ReadIdsFromMessageL(aMessage, 0, il);

    // perform action
    iMediaServer->UnshareItemListL(il, &iRespExecStatus);

    // prepare response
    aMessage.WriteL( 1, iRespExecStatusPkg );
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(&il);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerGetIpAndPortL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerGetIpAndPortL(
                                                    const RMessage2& aMessage )
    {
    // response
    TInetAddr address = iMediaServer->GetAddress();
    if (address.Match(TInetAddr(INET_ADDR(0, 0, 0, 0))))
        {
        User::Leave( KErrNotReady );
        }
    aMessage.WriteL( 0, TPckgBuf<TInetAddr>( address ) );
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventSetUploadDirectoryL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventSetUploadDirectoryL( const RMessage2& aMessage )
    {
    HBufC* buf = HBufC::NewLC( aMessage.GetDesLength( 0 ) );
    TPtr ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 0, ptr ) );
    iMediaServer->SetUploadDirToCDL( ptr );
    CleanupStack::PopAndDestroy(buf);
    aMessage.Complete( KErrNone );
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventAddIcon
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventAddIconL( const RMessage2& aMessage)
    {
    HBufC8* buf = HBufC8::NewLC( aMessage.GetDesLength( 0 ) );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 0, ptr ) );

    iMediaServer->AddIconL(ptr);
    CleanupStack::PopAndDestroy(buf);
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventUpdateIcon
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventUpdateIconL( const RMessage2& aMessage)
    {
    HBufC8* buf0 = HBufC8::NewLC( aMessage.GetDesLength( 0 ) );
    TPtr8 ptr0( buf0->Des() );
    User::LeaveIfError( aMessage.Read( 0, ptr0 ) );

    HBufC8* buf1 = HBufC8::NewLC( aMessage.GetDesLength( 1 ) );
    TPtr8 ptr1( buf1->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr1 ) );

    iMediaServer->UpdateIconL(ptr0, ptr1);

    CleanupStack::PopAndDestroy(buf1);
    CleanupStack::PopAndDestroy(buf0);

    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventRemoveIcon
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventRemoveIconL( const RMessage2& aMessage)
    {
    //Remove icon
    HBufC8* buf = HBufC8::NewLC( aMessage.GetDesLength( 0 ) );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 0, ptr ) );

    iMediaServer->RemoveIconL(ptr);
    CleanupStack::PopAndDestroy(buf);

    aMessage.Complete( KErrNone );
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventRemoveIcons
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventRemoveIconsL( const RMessage2& aMessage )
    {
    iMediaServer->RemoveIconsL();
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ObjFromMessageL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ReadIdsFromMessageL( const RMessage2& aMessage,
                                        TInt aSlot, RArray<TInt>& aObjIds )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength(aSlot);
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aMessage.Read(aSlot, ptr));

    // read stream
    RDesReadStream stream(*buf);
    CleanupClosePushL(stream);

    // number of elements
    TInt n = stream.ReadInt32L();

    // variables used to retrieve integers
    HBufC8* tmpId;
    TLex8 lex;
    TInt tmpIdNum;
    // read elements
    while( n-- )
        {
        tmpId = HBufC8::NewLC(stream, KMaxUpnpObjStringLen);
        if ( tmpId->Des() == KNullDesC8() )
            {
            User::Leave( KErrNotFound );
            }
        lex.Assign(tmpId->Des());
        if( lex.Val( tmpIdNum ) < KErrNone )
            {
            User::Leave( KErrNotFound );
            }
        aObjIds.AppendL( tmpIdNum);
        CleanupStack::PopAndDestroy(tmpId);
        }

    // clean up
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ObjFromMessageL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ReadObjFromMessageL( const RMessage2& aMessage,
                                                TInt aSlot, CUpnpObject* aObj )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength(aSlot);
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aMessage.Read(aSlot, ptr));

    // read stream
    RDesReadStream stream(*buf);
    CleanupClosePushL(stream);

    // internalize object
    stream >> *aObj;

    // clean up
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ReadItemFromMessageL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ReadItemFromMessageL( const RMessage2& aMessage,
                                                TInt aSlot, CUpnpItem* aItem )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength(aSlot);
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aMessage.Read(aSlot, ptr));

    // read stream
    RDesReadStream stream(*buf);
    CleanupClosePushL(stream);

    // internalize object
    stream >> *aItem;

    // clean up
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ReadObjFromMessageL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ReadObjFromMessageL( const RMessage2& aMessage,
                                        TInt aSlot, CUpnpItemList* aItemList )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength(aSlot);
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aMessage.Read(aSlot, ptr));

    // read stream
    RDesReadStream stream(*buf);
    CleanupClosePushL(stream);

    // internalize object
    stream >> *aItemList;

    // clean up
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::ReadObjFromMessageL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::ReadObjFromMessageL( const RMessage2& aMessage,
                            TInt aSlot, CUpnpBrowseCriteria* aBrowseCriteria )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength(aSlot);
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aMessage.Read(aSlot, ptr));

    // read stream
    RDesReadStream stream(*buf);
    CleanupClosePushL(stream);

    // internalize object
    stream >> *aBrowseCriteria;

    // clean up
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerShareReferenceL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerShareReferenceL( const RMessage2& aMessage )
    {
    // get 'reference object' from message
    CUpnpItem* ref = CUpnpItem::NewL();
    CleanupStack::PushL(ref);
    ReadItemFromMessageL(aMessage, 0, ref);

    // perform sharing
    iMediaServer->ShareReferenceL(ref);

    // prepare response
    iResponseBuffer = ref->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(ref);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerShareReferenceListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerShareReferenceListL( const RMessage2& aMessage )
    {
    // get 'reference item' list from message
    CUpnpItemList* refList = CUpnpItemList::NewL();
    CleanupStack::PushL(refList);
    ReadObjFromMessageL(aMessage, 0, refList);

    // perform sharing
    iMediaServer->ShareReferenceListL(refList, &iRespExecStatus);

    // prepare response
    iResponseBuffer = refList->ToDes8L();
    iRespLength = iResponseBuffer->Length();
    aMessage.WriteL(1, iRespLengthPkg);
    aMessage.Complete( KErrNone );

    // clean up
    CleanupStack::PopAndDestroy(refList);
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStatusChangeL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStatusChange( const RMessage2& aMessage )
    {
    if ( !iStatusMessage.IsNull() )
        {
        aMessage.Complete( KErrInUse );
        }
    else
        {
        iStatusMessage = aMessage;
        }    
    }
    
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::EventMediaServerStatusChangeCancelL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::EventMediaServerStatusChangeCancel( const RMessage2& aMessage )
    {
    if ( !iStatusMessage.IsNull() )
        {
        iStatusMessage.Complete( KErrCancel );
        }
    aMessage.Complete( KErrNone );
    }
    
// -----------------------------------------------------------------------------
// CUpnpMediaServerSession::StatusChanged
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerSession::StatusChanged()
    {
    if ( !iStatusMessage.IsNull() )
        {
        iStatusMessage.Complete( KErrNone );
        }
    }

// End of File
