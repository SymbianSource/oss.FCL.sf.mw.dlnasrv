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
* Description:      Content sharing servers main scheduler function
 *
*/





#include <e32debug.h>
#include <e32property.h>
#include "upnpcommonutils.h"
#include "upnpcontentserverdefs.h"
#include "upnpcontentserverhandler.h"
#include "upnpselectionreader.h"
#include "upnpcontentserverclient.h"
#include "upnpcontainercheckerao.h"
#include "upnpcontentserver.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KNoProgress = 0;
const TInt KMaxProgress = 100;
const TInt KRequestBufferSize = 2;


using namespace UpnpContentServer;

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::NewL
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler* CUpnpContentServerHandler::NewL(
    CUpnpContentServer* aServer )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServerHandler* self =
        new (ELeave) CUpnpContentServerHandler( aServer );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CUpnpContentServerHandler()
// C++ constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler::CUpnpContentServerHandler(
    CUpnpContentServer* aServer ) :
    iServer( aServer ),
    iContentSharingObserver( NULL ),
    iDefaultContainerIds( 3 )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iHandlerState = ENotActive;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
 /*   static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
    static _LIT_SECURITY_POLICY_S0(KThisProcessPolicy,
                                   KUpnpContentServerCat.iUid );

    // define progress property to be a byte array, allocating mem
    TInt err = iProgressProperty.Define( KUpnpContentServerCat,
                                         ESharingProgress,
                                         RProperty::EByteArray,
                                         KAllowAllPolicy,
                                         KThisProcessPolicy,
                                         sizeof( TUpnpProgress ) );
    if ( err != KErrNone )
        {
        __LOG1( "Error: %d", err );
        }
    if ( err!=KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    iIdle = CUPnPPeriodic::NewL( CActive::EPriorityIdle );
    TCallBack cb( RefreshClfL, this );
    const TInt KRefreshDelay( 500000 );
    iIdle->Start( KRefreshDelay, KRefreshDelay, cb );
    iReader = CUpnpSelectionReader::NewL( NULL ); // metadata set later
    SetProgressL( KNoProgress );*/
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::RefreshClfL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentServerHandler::RefreshClfL( TAny* aPtr )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServerHandler* handler( NULL );
    handler = static_cast<CUpnpContentServerHandler*>( aPtr );
    handler->iMetadata = CUpnpContentMetadataUtility::NewL();
    handler->iMetadata->SetCallback( handler );
    handler->iReader->SetMetadata( handler->iMetadata );

    handler->iIdle->Cancel();
    delete handler->iIdle;
    handler->iIdle = NULL;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::~CUpnpContentServerHandler()
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler::~CUpnpContentServerHandler()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    delete iAo;
    delete iUnsharer;
    delete iContainerChecker;
    delete iMetadata;
    delete iReader;
    delete iMediaServer;

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    iDefaultContainerIds.Close();
    delete iPendingSharingReq;
    delete iMusicSharingReq;
    delete iVisualSharingReq;
    iProgressProperty.Delete( KUpnpContentServerCat, ESharingProgress );
    iProgressProperty.Close();
    iServer = NULL;
    if ( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetContentSharingObserverL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetContentSharingObserverL(
    MUpnpContentSharingObserver* aObserver )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iContentSharingObserver = aObserver; // can be NULL
    if( iContentSharingObserver )
        {
        SetProgressL( KNoProgress );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }
// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectionContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::GetSelectionContentL(
    const TInt& aContainerType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // There might be many containers -> 10 is OK for granularity
    CDesCArray* containers = new ( ELeave ) CDesCArrayFlat( 10 );
    CleanupStack::PushL( containers );
    if ( iMetadata->RefreshOngoing() )
        {
        if ( !iWait.IsStarted() )
            {
            iWait.Start();
            }
        else
            {
            __LOG1( "Error: %d", __LINE__ );
            }
        }

    switch ( aContainerType )
        {
    case EImageAndVideo :
        {
        iReader->FetchCollectionsL( containers );
        }
        break;
    case EPlaylist :
        {
        iReader->FetchPlaylistsL( containers );
        }
        break;
    default :
        {
        CleanupStack::PopAndDestroy( containers );
        __LOG1( "Error: %d", __LINE__ );
        }
        break;
        }
    if ( iContentSharingObserver )
        {
        iContentSharingObserver->CompleteSelectionContentL( *containers );
        }

    CleanupStack::PopAndDestroy( containers );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectionIndexesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::GetSelectionIndexesL(
    RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpSharingRequest* sharingReq( NULL );
    switch ( aType )
        {
    case EImageAndVideo :
        {
        if ( iVisualSharingReq )
            {
            sharingReq = iVisualSharingReq;
            }
        }
        break;
    case EPlaylist :
        {
        if ( iMusicSharingReq )
            {
            sharingReq = iMusicSharingReq;
            }
        }
        break;
    default :
        {
        __LOG1( "Error: %d", __LINE__ );
        }
        break;
        }
    if ( sharingReq )
        {
        // There is sharing ongoing, return the buffered selections
        for( TInt i(0); i < sharingReq->iSelections.Count(); i++ )
            {
            aMarkedItems.AppendL( sharingReq->iSelections[ i ] );
            }
        }
    else
        {
        if ( !iReader )
            {
            __LOG1( "Error: %d", __LINE__ );
            iReader = CUpnpSelectionReader::NewL( iMetadata );
            }
        iReader->GetSelectionIndexesL( aMarkedItems, (TUpnpMediaType)aType );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ChangeShareContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::ChangeShareContentL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    for( TInt e(0); e<aMarkedItems.Count();e++)
        {
        __LOG2("CUpnpContentServerHandler: received index[%d] = %d",
            e, aMarkedItems[e] );
        }

    switch ( iHandlerState )
        {
    case ESchedulingSharing :
        {
        if ( aType == iBufferPosition )
            {
            if ( iSharingPhase != EShare )
                {
                SetSharingRequestL( aMarkedItems, aType );
                }
            else // iSharingPhase == EShare
                {
                // sanity checks
                delete iUnsharer;
                iUnsharer = NULL;
                delete iContainerChecker;
                iContainerChecker = NULL;

                if ( iAo )
                    {
                    // iAo cannot be deleted here, request stop instead
                    iAo->RequestStop(
                        MUpnpSharingCallback::ESharingFullStop );
                    }
                // Get playlist Ids
                CDesCArray* ids = new (ELeave) CDesCArrayFlat(4);
                CleanupStack::PushL( ids );
                CDesCArray* names = new (ELeave) CDesCArrayFlat(4);
                CleanupStack::PushL( names );
                if ( aType == EImageAndVideo )
                    {
                    iReader->CollectionIdsL( *ids, *names );
                    }
                else
                    {
                    iReader->PlayListIdsL( *ids, *names );
                    }
                // pendingSharingReq will be handled from callback
                delete iPendingSharingReq;
                iPendingSharingReq = NULL;
                iPendingSharingReq = CUpnpSharingRequest::NewL( aType,
                                                                aMarkedItems,
                                                                ids,
                                                                names );
                CleanupStack::Pop( names ); // ownership transferred
                CleanupStack::Pop( ids ); // ownership transferred
                }
            }
        else
            {
            SetSharingRequestL( aMarkedItems, aType );
            }
        }
        break;
    case ENotActive :
        {

        delete iMetadata;
        iMetadata = NULL;
        iMetadata = CUpnpContentMetadataUtility::NewL();
        iMetadata->SetCallback( this );
        iReader->SetMetadata( iMetadata );


        if ( iMetadata->RefreshOngoing() )
            {
            __LOG("CUpnpContentServerHandler: Waiting refresh 3");
            iWait.Start();
            }
        SetSharingRequestL( aMarkedItems, aType );

        iBufferPosition = aType;
        // Start sharing
        iHandlerState = ESchedulingSharing;

        iSharingPhase = ECheckDefaults;
        // start the sharing
        DoShareL();

        }
        break;
    default :
        {
        __LOG1( "Error: %d", __LINE__ );
        }
        break;

        }
    SetProgressL( KNoProgress );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::RefreshShareContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::RefreshShareContentL(
    TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( ( aType == EImageAndVideo && iVisualSharingReq ) ||
         ( aType == EPlaylist && iMusicSharingReq ) )
        {
        // no refresh available if sharing is ongoing
        __LOG1( "Error: %d", __LINE__ );
        User::Leave( KErrServerBusy );
        }

    if ( iMetadata->RefreshOngoing() )
        {
        __LOG("CUpnpContentServerHandler: Waiting refresh 1");
        iWait.Start();
        }

    delete iReader;
    iReader = NULL;
    iReader = CUpnpSelectionReader::NewL( iMetadata );

    RArray<TInt> selectedItems;
    CleanupClosePushL( selectedItems );
    CDesCArray* containers = new ( ELeave ) CDesCArrayFlat( 10 );
    CleanupStack::PushL( containers );
    iReader->FetchCollectionsL( containers );
    GetSelectionIndexesL( selectedItems, aType );
    CDesCArray* ids = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( ids );
    CDesCArray* names = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( names );
    iReader->CollectionIdsL( *ids, *names );

    if ( aType == EImageAndVideo )
        {
        iVisualSharingReq = CUpnpSharingRequest::NewL( aType,
                                                       selectedItems,
                                                       ids,
                                                       names );
        }
    else if ( aType == EPlaylist )
        {
        iMusicSharingReq = CUpnpSharingRequest::NewL( aType,
                                                      selectedItems,
                                                      ids,
                                                      names );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::Pop( names ); // ownership transferred
    CleanupStack::Pop( ids ); // ownership transferred
    ChangeShareContentL( selectedItems, aType );

    selectedItems.Reset();
    containers->Reset();

    CleanupStack::PopAndDestroy( containers );
    CleanupStack::PopAndDestroy( &selectedItems );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::FillProgressInfoL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::FillProgressInfoL(
    RArray<TUpnpProgressInfo>& aArr,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpSharingRequest* sharingReq( NULL );
    if( iPendingSharingReq && iPendingSharingReq->iKind == aType )
        {
        sharingReq = iPendingSharingReq;
        }
    else
        {
        if ( aType == EImageAndVideo )
            {
            sharingReq = iVisualSharingReq;
            }
        else if ( aType == EPlaylist )
            {
            sharingReq = iMusicSharingReq;
            }
        }
    TUpnpProgressInfo progress;
    progress.iProgressKind = UpnpContentServer::TUpnpMediaType( aType );
    if ( sharingReq )
        {
        progress.iProgressType = TUpnpProgressInfo::ESharingProgress;
        if ( iBufferPosition == aType &&
             iHandlerState == ESchedulingSharing &&
             ( iAo || iUnsharer || iContainerChecker ))
            {
            // sharing ongoing for images
            TInt prog ( 0 );
            TInt progRel( 0 );
            if ( iAo && iSharingPhase == EShare )
                {
                prog = iAo->SharingProgress(
                    (TUpnpMediaType&)progress.iProgressKind );
                progRel = ( prog * KMaxProgress ) / sharingReq->iItemCount;      
                }
            if ( iUnsharer &&  iSharingPhase == EUnshare  )
                {
                prog = iUnsharer->UnSharingProgress();
                TInt itemNum = iUnsharer->TotalItemsForUnShare();
                if( itemNum > 0 )
                    {     
                    progRel = ( prog * KMaxProgress ) / itemNum;          
                    }
                }   

            // check that progress is between 0 and 100 %
            if ( progRel < 0 )
                {
                progRel = KNoProgress;
                __LOG1( "Error: %d", __LINE__ );
                }
            else if ( progRel > KMaxProgress )
                {
                progRel = KMaxProgress;
                __LOG1( "Error: %d", __LINE__ );
                }
            progress.iProgress = progRel;
            }
        else
            {
            // buffer is pending
            progress.iProgressType = TUpnpProgressInfo::ESharingProgress;
            progress.iProgress = KNoProgress;
            }
        }
    else
        {
        progress.iProgressType = TUpnpProgressInfo::EVisualStatus;
        TInt state( EShareNone );

        if ( aType == EImageAndVideo )
            {
            iReader->GetVisualSharingStateL( state );
            }
        else
            {
            iReader->GetMusicSharingStateL( state );
            }

        progress.iProgress = state;
        }
    aArr.AppendL( progress );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }



// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CanStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentServerHandler::CanStop() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( EFalse );
    if ( !( iVisualSharingReq || iMusicSharingReq  ) )
        {
        __LOG("CUpnpContentServerHandler: CanStop(): 1");
        ret = ETrue;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::RefreshDoneL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::RefreshDoneL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iMetadata->SetCallback( this );
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CompleteSharingOperationL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::CompleteSharingOperationL(
    const TInt& aErr, const TInt& aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( aErr != KErrNone )
        {
        __LOG1( "Error: %d", aErr );
        }
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    else if ( iStartupCleaning )
        {
        iStartupCleaning = EFalse;
        }

    else if ( aErr && aErr != KErrCancel )
        {
        // Cancel is handled below
        HandleError( aErr );
        }
    else
        {
        if ( iSharingPhase == ECheckDefaults )
            {
            iHandlerState = ESchedulingSharing;
            iSharingPhase = EUnshare;
            }
        else if ( iSharingPhase == EUnshare )
            {
            iHandlerState = ESchedulingSharing;
            iSharingPhase = EShare;

            }
        else if ( iSharingPhase == EShare )
            {
            switch ( aType )
                {
            case EImageAndVideo :
                {
                delete iVisualSharingReq;
                iVisualSharingReq = NULL;
                SetProgressL( KNoProgress );
                }
                break;
            case EPlaylist :
                {
                delete iMusicSharingReq;
                iMusicSharingReq = NULL;
                SetProgressL( KNoProgress );
                }
                break;
            default :
                {
                __LOG1( "Error: %d", __LINE__ );
                }
                break;
                }
            if ( aType != iBufferPosition )
                {
                __LOG1( "Error: %d", __LINE__ );
                }

            if ( aErr == KErrNone )
                {
                iErrorToClient = aErr;
                iBufferPosition = (iBufferPosition + 1)%KRequestBufferSize;
                SetProgressL( KNoProgress );
                }
            else
                {
                if ( iPendingSharingReq )
                    {
                    if ( iPendingSharingReq->iKind == EImageAndVideo )
                        {
                        delete iVisualSharingReq;
                        iVisualSharingReq = NULL;
                        iVisualSharingReq = iPendingSharingReq;
                        }
                    else
                        {
                        delete iMusicSharingReq;
                        iMusicSharingReq = NULL;
                        iMusicSharingReq = iPendingSharingReq;
                        }
                    iPendingSharingReq = NULL;
                    }
                }

            iDefaultContainerIds.Reset();
            if ( ! ( iVisualSharingReq || iMusicSharingReq ))
                {
                iHandlerState = ENotActive;
                iSharingPhase = ESharingInActive;
                if ( CanStop() && iServer && !iContentSharingObserver )
                    {
                    // client application is exited, no sharing ongoing
                    // -> stop the server
                    iServer->Stop();
                    }
                }
            else
                {
                iSharingPhase = ECheckDefaults;
                iHandlerState = ESchedulingSharing;
                }
            }
        else
            {
            __LOG1( "Error: %d", __LINE__ );
            }

        if ( iHandlerState != ENotActive )
            {
            DoShareL();
            }

        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CancelSharingOperationL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::CancelSharingOperationL(
    const TInt& /*aErr*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );


    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetProgressL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetProgressL(
    const TInt& aProgress )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpSharingRequest* sharingReq( NULL );
    if( iPendingSharingReq && iPendingSharingReq->iKind == iBufferPosition )
        {
        sharingReq = iPendingSharingReq;
        }
    else
        {
        if ( iBufferPosition == EPlaylist )
            {
            sharingReq = iMusicSharingReq;
            }
        else if ( iBufferPosition == EImageAndVideo )
            {
            sharingReq = iVisualSharingReq;
            }
        }

    if ( sharingReq )
        {
        sharingReq->iProgress = aProgress;
        }

    TProgressInfos progressArr;
    CleanupClosePushL( progressArr );

    FillProgressInfoL( progressArr, EImageAndVideo );
    FillProgressInfoL( progressArr, EPlaylist );

    TUpnpProgress finalProg;
    finalProg.iError = iErrorToClient;
    iErrorToClient = KErrNone;
    finalProg.iImageVideoProgress = progressArr[ 0 ];
    finalProg.iMusicProgress = progressArr[ 1 ];

    TPckgBuf<TUpnpProgress> progressBuf( finalProg );

    TInt err = iProgressProperty.Set( KUpnpContentServerCat,
                                      ESharingProgress,
                                      progressBuf  );
    if ( err != KErrNone )
        {
        __LOG1( "Error: %d", err );
        }
    CleanupStack::PopAndDestroy( &progressArr );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::DoShareL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::DoShareL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt err( KErrNone );

    iHandlerState = ESchedulingSharing;

    if ( iSharingPhase == ECheckDefaults )
        {
        iDefaultContainerIds.Reset();
        delete iContainerChecker;
        iContainerChecker = NULL;
        iContainerChecker = new (ELeave) CUpnpContainerCheckerAo( this );
        iContainerChecker->ValidateContainerStructureL(
            &iDefaultContainerIds );
        }
    else if ( iSharingPhase == EUnshare )
        {
        delete iContainerChecker;
        iContainerChecker = NULL;
        delete iUnsharer;
        iUnsharer = NULL;
        iUnsharer = new ( ELeave ) CUpnpUnsharerAo(
            this,
            CActive::EPriorityStandard );
        TInt id = GetContainerId( iBufferPosition );
        if ( id == KErrNotFound )
            {
            User::Leave( KErrCorrupt );
            }
        iUnsharer->EmptyContainer( id );
        }
    else if ( iSharingPhase == EShare )
        {
        delete iUnsharer;
        iUnsharer = NULL;
        delete iAo;
        iAo = NULL;
        iAo = CUpnpContentSharingAo::NewL( this, iMetadata );
        TInt id = GetContainerId( iBufferPosition );
        if ( id == KErrNotFound )
            {
            User::Leave( KErrCorrupt );
            }
        // iBufferPosition is updated in CompleteSharingOperationL
        switch( iBufferPosition )
            {
        case EImageAndVideo :
            {
            TRAP( err, iAo->InitializeL( ( TUpnpMediaType )
                                         iVisualSharingReq->iKind,
                                         iVisualSharingReq->iSelections,
                                         *iVisualSharingReq->iObjectIds,
                                         *iVisualSharingReq->iObjectNames,
                                         id ));
            // always show something in progress dialog -> add 1
            TRAP( err, iVisualSharingReq->iItemCount =
                  iAo->SelectionObjectCountL(
                      ( TUpnpMediaType )iBufferPosition ) +1 );
            iAo->ShareFiles(); // start sharing
            }
            break;
        case EPlaylist :
            {
            TRAP( err, iAo->InitializeL( ( TUpnpMediaType )
                                         iMusicSharingReq->iKind,
                                         iMusicSharingReq->iSelections,
                                         *iMusicSharingReq->iObjectIds,
                                         *iMusicSharingReq->iObjectNames,
                                         id ));
            // always show something in progress dialog -> add 1
            TRAP( err, iMusicSharingReq->iItemCount =
                  iAo->SelectionObjectCountL(
                      ( TUpnpMediaType )iBufferPosition ) +1);
            iAo->ShareFiles(); // start sharing
            }
            break;
        default:
            {
            __LOG1( "Error: %d", __LINE__ );
            }
            break;
            }
        }
    else
        {
        __LOG1( "Error: %d", __LINE__ );
        }


    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::Cleanup
// Cleanup depending on state
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::Cleanup()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if( iSharingPhase == EShare )
        {
        if( iAo )
            {
            iAo->RequestStop(
                MUpnpSharingCallback::ESharingFullStop );
            }
        }
    delete iContainerChecker;
    iContainerChecker = NULL;
    delete iUnsharer;
    iUnsharer = NULL;

    delete iVisualSharingReq;
    iVisualSharingReq = NULL;
    delete iMusicSharingReq;
    iMusicSharingReq = NULL;
    delete iPendingSharingReq;
    iPendingSharingReq = NULL;

    // reset state variables
    iHandlerState = ENotActive;
    iSharingPhase = ESharingInActive;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetContainerId
// get id from list
// --------------------------------------------------------------------------
//
TInt CUpnpContentServerHandler::GetContainerId( const TInt aType ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt id( KErrNotFound );
    if ( aType == EImageAndVideo )
        {
        id = iDefaultContainerIds[
            CUpnpContainerCheckerAo::EImageAndVideo ];
        }
    else if ( aType == EPlaylist )
        {
        id = iDefaultContainerIds[ CUpnpContainerCheckerAo::EMusic ];
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return id;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ConnectionLost
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentServerHandler::ConnectionLostL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( EFalse );
    if ( iHandlerState != ENotActive )
        {
        if ( iSharingPhase == EShare )
            {
            __LOG("iSharingPhase == EShare");
            ret = ETrue;
            iAo->RequestStop( MUpnpSharingCallback::ESharingPause );
            }
        else if ( iSharingPhase == ECheckDefaults )
            {
            __LOG("iSharingPhase == ECheckDefaults");
            iContainerChecker->RequestStop(
                MUpnpSharingCallback::ESharingPause );
            }
        else if ( iSharingPhase == EUnshare )
            {
            __LOG("iSharingPhase == EUnshare");
            iUnsharer->RequestStop( MUpnpSharingCallback::ESharingPause );
            }

        }
    else
        {
        __LOG("else ");
        iMediaServer = new (ELeave) RUpnpMediaServerClient();
        TInt err( iMediaServer->Connect() );
        TInt stat( RUpnpMediaServerClient::EStopped );
        if ( !err )
            {
            err = iMediaServer->Status( stat );
            if ( !err && stat == RUpnpMediaServerClient::EStartedOnline )
                {
                iMediaServer->Stop( RUpnpMediaServerClient::EStopSilent );
                iMediaServer->Close();
                delete iMediaServer;
                iMediaServer = NULL;
                }
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetSharingRequestL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetSharingRequestL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CDesCArray* ids = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( ids );
    CDesCArray* names = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( names );
    if ( aType == EImageAndVideo )
        {
        iReader->CollectionIdsL( *ids, *names );
        delete iVisualSharingReq;
        iVisualSharingReq = NULL;
        iVisualSharingReq = CUpnpSharingRequest::NewL( aType,
                                                       aMarkedItems,
                                                       ids,
                                                       names );
        }
    else
        {
        iReader->PlayListIdsL( *ids, *names );
        delete iMusicSharingReq;
        iMusicSharingReq = NULL;
        iMusicSharingReq = CUpnpSharingRequest::NewL( aType,
                                                      aMarkedItems,
                                                      ids,
                                                      names );
        }
    CleanupStack::Pop( names ); // ownership transferred
    CleanupStack::Pop( ids ); // ownership transferred
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ValidateDefaultContainersL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::ValidateDefaultContainersL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iSharingPhase != ECheckDefaults )
        {
        iStartupCleaning = ETrue;
        delete iContainerChecker;
        iContainerChecker = NULL;
        iContainerChecker = new (ELeave) CUpnpContainerCheckerAo( this );
        iContainerChecker->SetPriority( CActive::EPriorityUserInput );
        iContainerChecker->ValidateContainerStructureL(
            NULL );

        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::HandleError
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::HandleError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if( iHandlerState != ENotActive )
        {
        Cleanup();
        iHandlerState = ENotActive;
        iSharingPhase = ECheckDefaults;
        iErrorToClient = aError;
        TRAP_IGNORE( SetProgressL( KNoProgress ) );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// End of file
