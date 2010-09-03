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
* Description:      This module handlers the cleaning and creating of the
 *                filesystem to mediaserver
 *
*/





#include <e32debug.h>
#include "upnpcommonutils.h"
#include "upnpcontentserverdefs.h"
#include "upnpcontainercheckerao.h"
#include "upnpunsharerao.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KDefaultBrowseReqCount( 100 );
const TInt KDefaultBrowseStartingInd( 0 );

using namespace UpnpContentServer;

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::CUpnpContainerCheckerAo()
// C++ constructor.
// --------------------------------------------------------------------------
//
CUpnpContainerCheckerAo::CUpnpContainerCheckerAo(
    MUpnpSharingCallback* aCallback ) :
    CActive( CActive::EPriorityIdle ),
    iCallback( aCallback )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iCurrentOperation = ENotActive;
    iState = EIdle;
    iTotalMatches = KErrNotFound;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::~CUpnpContainerCheckerAo()
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpContainerCheckerAo::~CUpnpContainerCheckerAo()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iFileSharing;
    iFileSharing = NULL;
    iMediaServer.Close();

    if ( IsActive() )
        {
        __LOG1("CUpnpContainerCheckerAo destructor IsActive iCurrentOp=%d",
              iCurrentOperation );
        iCurrentOperation = ENotActive;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }

    Cancel();

    Cleanup();

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::ValidateContainerStructureL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContainerCheckerAo::ValidateContainerStructureL( 
    RArray<TInt>* aIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CActiveScheduler::Add( this );
    TInt err( KErrNotFound );

    iClientIds = aIds;

    // First initialize resources
    iOperationMode = EValidateContainers;
    iCurrentOperation = EInitialize;
    SelfComplete( KErrNone );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::GetDefaultContainerIdsL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContainerCheckerAo::CheckDefaultContainersL(
    RArray<TInt>* aIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iOperationMode = ECheckDefaultContainers;
    TInt err( KErrNone );

    iClientIds = aIds;

    CActiveScheduler::Add( this );
    iCurrentOperation = EInitialize;
    SelfComplete( KErrNone );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::RequestStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::RequestStop(
    MUpnpSharingCallback::TSharingStopType aStopType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iStopRequested = aStopType;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::InitializeL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContainerCheckerAo::InitializeL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // First initialize resources
    iFileSharing = CUpnpFileSharing::NewL();
    TInt error( iMediaServer.Connect() );
    if ( error )
        {
        __LOG8_1( "iMediaServer.Connect err: %d",error );
        }
    User::LeaveIfError( error );

    if ( !error )
        {
        // start offline if not started
        TInt status( RUpnpMediaServerClient::EStopped );
        error = iMediaServer.Status( status );
        if ( !error && status == RUpnpMediaServerClient::EStopped )
            {
            error = iMediaServer.StartOffline();
            }
        }

    SelfComplete( error );
    iCurrentOperation = EQueryDefaultContainers;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return error;
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::QueryDefaultContainerIdsL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::QueryDefaultContainerIdsL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iState = EResolveDefaultContainerIds;

    CUpnpBrowseCriteria* browseCriteria = CreateBrowseCriteriaLC();

    delete iContainerList;
    iContainerList = NULL;

    iContainerList = CUpnpContainerList::NewL();

    // Get shared containers to iContainerList
    iFileSharing->GetSharedContainerListL( KRootContainerId,
                                           *browseCriteria,
                                           *iContainerList,
                                           iMatchCount,
                                           iStatus );
    iCurrentOperation = EQueryDefaultContainersResult;
    CleanupStack::PopAndDestroy( browseCriteria );
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::QueryDefaultContainerIdsResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::QueryDefaultContainerIdsResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContainer* tmpContainer( NULL );
    if ( !iContainerList )
        {
        User::Leave( KErrGeneral );
        }
    switch ( iContainerPosition )
        {
    case CUpnpContainerCheckerAo::EImageAndVideo :
        {
        tmpContainer =
            UPnPCommonUtils::FindContainerByTitle( *iContainerList,
                                                   KImageContainerName );
        if ( tmpContainer )
            {
            iImageVideoContainerId = UPnPCommonUtils::DesC8ToInt(
                tmpContainer->Id() );
            iContainerList->RemoveAndDestroy( tmpContainer->Id() );
            tmpContainer = NULL;
            iContainerPosition++;
            iCurrentOperation = EQueryDefaultContainersResult;
            SelfComplete( KErrNone );
            }
        else
            {
            iCurrentOperation = EShareContainer;
            ShareContainerL( iContainerPosition );
            }
        }
    break;
    case CUpnpContainerCheckerAo::EMusic :
        {
        tmpContainer =
            UPnPCommonUtils::FindContainerByTitle( *iContainerList,
                                                   KMusicContainerName );
        if ( tmpContainer )
            {
            iMusicContainerId = UPnPCommonUtils::DesC8ToInt(
                tmpContainer->Id() );
            iContainerList->RemoveAndDestroy( tmpContainer->Id() );
            tmpContainer = NULL;
            iContainerPosition++;
            iCurrentOperation = EQueryDefaultContainersResult;
            SelfComplete( KErrNone );
            }
        else
            {
            iCurrentOperation = EShareContainer;
            ShareContainerL( iContainerPosition );
            }
        }
    break;
    case CUpnpContainerCheckerAo::EUploaded :
        {
        tmpContainer =
            UPnPCommonUtils::FindContainerByTitle( *iContainerList,
                                                   KUploadedContainerName );
        if ( tmpContainer )
            {
            iUploadContainerId = UPnPCommonUtils::DesC8ToInt(
                tmpContainer->Id() );
            iContainerList->RemoveAndDestroy( tmpContainer->Id() );
            tmpContainer = NULL;
            iContainerPosition++;
            iCurrentOperation = EQueryDefaultContainersResult;
            SelfComplete( KErrNone );
            }
        else
            {
            iCurrentOperation = EShareContainer;
            ShareContainerL( iContainerPosition );
            }
        }
    break;
    default :
        {
        // Store IDs of possible unwanted containers
        if ( iOperationMode != ECheckDefaultContainers )
            {
            for ( TInt i(0); i< iContainerList->ObjectCount(); i++ )
                {
                tmpContainer = static_cast< CUpnpContainer* >(
                    iContainerList->operator[]( i ));
                iSharedContainerIds.AppendL(
                    UPnPCommonUtils::DesC8ToInt( tmpContainer->Id() ) );
                }


            iState = EUnshareExtraContainers;
            iCurrentOperation = EUnshareContainer;
            }
        else
            {
            iCurrentOperation = ENotActive;
            }

        // Just add the Ids to client
        if ( iClientIds )
            {
            iClientIds->Reset();
            iClientIds->Append( iImageVideoContainerId );
            iClientIds->Append( iMusicContainerId );
            iClientIds->Append( iUploadContainerId );
            iClientIds->Compress();
            }
        delete iContainerList;
        iContainerList = NULL;
        SelfComplete( KErrNone );
        }
        break;
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::RunL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::RunL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iStatus.Int() )
        {
        __LOG1( "Error: %d", iStatus.Int() );
        }
    
    if ( iStatus.Int() == KErrCorrupt )
        {
        if ( iRecoverError == ENoError )
            {
            iRecoverError = EDropAllContent;
            }
        else
            {
            iCurrentOperation = ENotActive;
            iRecoverError = ENoError;
            }
        }
    else if ( iStatus.Int() == KErrNoMemory ||
              iStatus.Int() == KErrDiskFull ||
              iStatus.Int() == KErrServerTerminated )
             {
             iCurrentOperation = ENotActive;
             SelfComplete( iStatus.Int() );
             }


    if ( iRecoverError )
        {
        switch ( iRecoverError )
            {
        case EDropAllContent :
            {
            iFileSharing->DropAllContentL( iStatus );
            iRecoverError = EDropAllContentResult;
            SetActive();
            }
            break; 
        case EDropAllContentResult :
            {
            if ( iStatus.Int() == KErrCorrupt )
                {
                iCurrentOperation = ENotActive;
                SelfComplete( iStatus.Int() );
                }
            else
                {
                iRecoverError = ENoError;
                SelfComplete( KErrNone );
                }
            }
            break;    
        default:
            __LOG1( "Error: %d", __LINE__ );
            break;
            }

        }
    else
        {

        if ( iStopRequested == MUpnpSharingCallback::ESharingPause )
            {
            delete iFileSharing;
            iFileSharing = NULL;
            iFileSharing = CUpnpFileSharing::NewL();
            TInt e = iMediaServer.Stop( 
                         RUpnpMediaServerClient::EStopSilent );
            if ( e )
                {
                __LOG1( "Error: %d", e );
                }
            e = iMediaServer.StartOffline();
            if ( e )
                {
                __LOG1( "Error: %d", e );
                }
            iStopRequested = MUpnpSharingCallback::ESharingNoStop;
            }

        switch ( iCurrentOperation )
            {
        case EInitialize :
            {
            InitializeL();
            }
            break;
        case EQueryDefaultContainers :
            {
            QueryDefaultContainerIdsL();
            }
            break;
        case EQueryDefaultContainersResult :
            {
            QueryDefaultContainerIdsResultL();
            }
            break;
        case EUnshareItemList :
            {
            UnshareItemListL();
            }
            break;
        case EQueryItemFromMediaServer :
            {
            QueryContainerItemsL();
            }
            break;
        case EQueryItemFromMediaServerResult :
            {
            QueryItemFromCdsResultL();
            }
            break;
        case EShareContainer :
            {
            __LOG1( "Error: %d", __LINE__ );
            }
            break;
        case EShareContainerResult :
            {
            ShareContainerResult();
            }
            break;
        case ENotActive :
            {
            Cleanup();
            if ( iCallback )
                {
                iCallback->CompleteSharingOperationL( iStatus.Int(), 0 );
                }
            }
            // all operations requested done
            break;
        case EUnshareContainer :
            {
            UnshareContainerL();
            }
            break;
        case EUnshareContainerResult :
            {
            UnshareContainerResultL();
            }
            break;
        default :
            {
            __LOG1( "Error: %d", __LINE__ );
            break;
            }
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::DoCancel()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // NOTE: The cancelling of mediaserver request must be made in destructor
    // because there is no cancel in CUpnpFileSharing
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::RunError
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContainerCheckerAo::RunError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( aError )
        {
        __LOG8_1( "CUpnpContainerCheckerAo::RunError", aError );
        }
    TInt ret( KErrNone );
    if ( aError == KErrNoMemory ||
         aError == KErrDiskFull ||
         aError == KErrCorrupt )
        {
        iCurrentOperation = ENotActive;
        SelfComplete( aError );
        }
    else 
        {
        
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }


// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::QueryItemFromCdsResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::QueryItemFromCdsResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt objCount( iItemList->ObjectCount() );
    if ( objCount && ( iExecStatus < iTotalMatches ) )
        {
        iCurrentOperation = EUnshareItemList;
        }
    else
        {
        if ( iState == EEmptyRootContainerItems )
            {
            iState = EEmptyUploadedContainerItems;
            iCurrentOperation = EQueryItemFromMediaServer;
            }
        else
            {
            iState = EIdle;
            iCurrentOperation = ENotActive;
            }
        iExecStatus = 0;
        iTotalMatches = KErrNotFound;

        }

    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::UnshareItemListL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::UnshareItemListL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iExecStatus += iItemList->ObjectCount();
    iFileSharing->UnshareItemListL( *iItemList, iMatchCount, iStatus );

    delete iItemList;
    iItemList = NULL;
    iCurrentOperation = EQueryItemFromMediaServer;
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::ShareContainerL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::ShareContainerL( TInt aPos )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
 /*   iTmpContainer = CUpnpContainer::NewL();
    iTmpContainer->SetParentIdL( *UPnPCommonUtils::IntToDesC8LC(
        KRootContainerId ) );
    CleanupStack::PopAndDestroy(); //UPnPCommonUtils::IntToDesC8LC
    iTmpContainer->SetObjectClassL( KStorageFolderContainerClass );

    switch ( aPos )
        {
    case CUpnpContainerCheckerAo::EImageAndVideo :
        {
        iTmpContainer->SetTitleL( KImageContainerName );
        }
        break;
    case CUpnpContainerCheckerAo::EMusic :
        {
        iTmpContainer->SetTitleL( KMusicContainerName );
        }
        break;
    case CUpnpContainerCheckerAo::EUploaded :
        {
        iTmpContainer->SetTitleL( KUploadedContainerName );
        }
        break;
    default :
        break;
        }
    if ( iTmpContainer )
        {
        iFileSharing->ShareContainerL(
            *UPnPCommonUtils::IntToDesC8LC( KRootContainerId ),
            *iTmpContainer,
            iStatus );
        iCurrentOperation = EShareContainerResult;
        CleanupStack::PopAndDestroy(); // UPnPCommonUtils::IntToDesC8LC
        iCurrentOperation = EShareContainerResult;
        SetActive();
        }*/
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::ShareContainerResult
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::ShareContainerResult( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( !iStatus.Int() )
        {
        // Store the IDs
        if ( iContainerPosition == CUpnpContainerCheckerAo::EUploaded )
            {
            iUploadContainerId = UPnPCommonUtils::DesC8ToInt(
                iTmpContainer->Id() );
            }

        else if ( iContainerPosition ==
                  CUpnpContainerCheckerAo::EImageAndVideo )
            {
            iImageVideoContainerId = UPnPCommonUtils::DesC8ToInt(
                iTmpContainer->Id() );
            }
        else if ( iContainerPosition == CUpnpContainerCheckerAo::EMusic )
            {
            iMusicContainerId = UPnPCommonUtils::DesC8ToInt(
                iTmpContainer->Id() );
            }


        }
    delete iTmpContainer;
    iTmpContainer = NULL;

    iCurrentOperation = EQueryDefaultContainersResult;
    iContainerPosition++;
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::UnshareContainerL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::UnshareContainerL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iCurrentOperation = EUnshareContainerResult;
    switch ( iState )
        {
    case EEmptyUploadedContainer :
        {
        __LOG1( "Error: %d", __LINE__ );
        }
        break;
    case EUnshareExtraContainers :
        {
        if ( iSharedContainerIds.Count() )
            {
            iSharedContainerIds.Compress();
            iFileSharing->UnshareContainerL( iSharedContainerIds[ 0 ],
                                             iStatus );
            iSharedContainerIds.Remove( 0 );
            SetActive();
            }
        else
            {
            iState = EEmptyRootContainerItems;
            QueryContainerItemsL();
            }
        }
        break;
    default :
        {
        __LOG1( "Error: %d", __LINE__ );
        }
        break;
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::UnshareContainerResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::UnshareContainerResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iState == EEmptyUploadedContainer )
        {
        __LOG1( "Error: %d", __LINE__ );
        }
    else if ( iState == EUnshareExtraContainers )
        {
        if ( iSharedContainerIds.Count() )
            {
            iCurrentOperation = EUnshareContainer;
            }
        else
            {
            iState = EEmptyRootContainerItems;
            iCurrentOperation = EQueryItemFromMediaServer;
            }
        SelfComplete( KErrNone );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::QueryContainerItemsL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::QueryContainerItemsL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iItemList;
    iItemList = NULL;
    iItemList = CUpnpItemList::NewL();
    TInt containerId( KRootContainerId );
    if ( iState == EEmptyUploadedContainerItems )
        {
        containerId = iUploadContainerId;
        }
    CUpnpBrowseCriteria* browseCriteria = CreateBrowseCriteriaLC();
    if ( iTotalMatches == KErrNotFound )
        {
        iFileSharing->GetSharedItemListL (
            containerId, *browseCriteria,
            *iItemList, iTotalMatches, iStatus );
        }
    else
        {
        TInt tmp( 0 );
        iFileSharing->GetSharedItemListL (
            containerId, *browseCriteria,
            *iItemList, tmp, iStatus );
        }
    iCurrentOperation = EQueryItemFromMediaServerResult;
    CleanupStack::PopAndDestroy( browseCriteria );
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::CreateBrowseCriteriaLC
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
CUpnpBrowseCriteria* CUpnpContainerCheckerAo::CreateBrowseCriteriaLC() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpBrowseCriteria* browseCriteria = CUpnpBrowseCriteria::NewLC();
    browseCriteria->SetRequestedCount( KDefaultBrowseReqCount );
    browseCriteria->SetStartingIndex( KDefaultBrowseStartingInd );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return browseCriteria;
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::Cleanup
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::Cleanup()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iExecStatus = 0;
    iTotalMatches = KErrNotFound;
    iImageVideoContainerId = 0;
    iMusicContainerId = 0;
    iUploadContainerId = 0;
    iContainerPosition = 0;
    iClientIds = NULL;
    delete iItemList;
    iItemList = NULL;
    delete iContainerList;
    iContainerList = NULL;
    delete iTmpContainer;
    iTmpContainer = NULL;
    iSharedContainerIds.Close();
    iState = EIdle;
    iCurrentOperation = ENotActive;
    if ( IsAdded() )
        {
        Deque();
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContainerCheckerAo::SelfComplete
// Start observing events
// --------------------------------------------------------------------------
//
void CUpnpContainerCheckerAo::SelfComplete( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    SetActive();
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aError );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// End of File
