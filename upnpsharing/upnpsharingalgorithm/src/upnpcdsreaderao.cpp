/**
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  CUpnpCdsReaderAO class implementation
 *
 */

// INCLUDES
#include <upnpfilesharing.h>
#include <upnpobject.h>

#include "upnpcdsreaderao.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"
#include "upnpcommonutils.h"
#include "upnpcdsliteobjectarray.h"
#include "upnpbrowsecriteria.h"
#include "upnpcontainerlist.h"
#include "upnpitemlist.h"

const TInt KDefaultBrowseReqCount( 50 );
const TInt KLoopLimit( 3 );

// --------------------------------------------------------------------------
// CUpnpSecAccessController::NewL
// --------------------------------------------------------------------------
//
CUpnpCdsReaderAO* CUpnpCdsReaderAO::NewL(
    CUpnpCdsLiteObjectArray& aUpnpCdsLiteObjects )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::NewL" );

    CUpnpCdsReaderAO* self = new ( ELeave ) CUpnpCdsReaderAO(
        aUpnpCdsLiteObjects );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::ConstructL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::ConstructL" );

    if ( !CActiveScheduler::Current() )
        {
        iScheduler = new ( ELeave ) CActiveScheduler;
        CActiveScheduler::Install( iScheduler );
        }
    // Add this active object to the active scheduler.
    CActiveScheduler::Add( this );

    iFileSharing = CUpnpFileSharing::NewL();
    iWaitRequest = new ( ELeave ) CActiveSchedulerWait();
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::~CUpnpCdsReaderAO
// --------------------------------------------------------------------------
//
CUpnpCdsReaderAO::~CUpnpCdsReaderAO()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::~CUpnpCdsReaderAO" );

    delete iFileSharing;
    delete iWaitRequest;

    if ( CActiveScheduler::Current() == iScheduler )
        {
        CActiveScheduler::Install( NULL );
        }
    delete iScheduler;
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::ReadCdsStructureL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::ReadCdsStructureL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::ReadCdsStructureL" );

    // Start reading from root container
    GetContainersL( KRootContainer );
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::GetContainersL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::GetContainersL( const TDesC8& aContainerId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::GetContainersL" );

    // Get all items of container with id aContainerId
    GetItemsL( aContainerId );

    // Number of found items. Count is initialized as KDefaultBrowseReqCount
    // to get into the while loop. Loop ends, when GetSharedItemListL
    // request does not get new objects
    TInt count( KDefaultBrowseReqCount );
    TInt startingIndex( 0 );
    TInt loopCounter( 0 );

    while ( startingIndex < count )
        {
        count = 0;

        CUpnpContainerList* containerList = CUpnpContainerList::NewL();
        CleanupStack::PushL( containerList );

        CUpnpBrowseCriteria* browseCriteria = CUpnpBrowseCriteria::NewLC();
        browseCriteria->SetRequestedCount( KDefaultBrowseReqCount );
        browseCriteria->SetStartingIndex( startingIndex );
        browseCriteria->SetFilterL( KBrowseCriteriaFilter );

        // Query all items and containers with container id aContainerId
        iFileSharing->GetSharedContainerListL(
            UPnPCommonUtils::DesC8ToInt( aContainerId ),
            *browseCriteria,
            *containerList,
            count,
            iStatus );

        CleanupStack::PopAndDestroy( browseCriteria );

        // Wait for the request to complete
        CompleteRequestL();

        // Leave, if GetSharedContainerListL gives continuously zero items
        if ( !containerList->ObjectCount() )
            {
            loopCounter += 1;
            if ( loopCounter >= KLoopLimit )
                {
                User::Leave( KErrGeneral );
                }
            }
        else
            {
            loopCounter = 0;
            }

        for ( TInt index( 0 ); index < containerList->ObjectCount(); index++ )
            {
            // Append container to the objects array
            iUpnpCdsLiteObjects.AppendL( ( *containerList )[ index ] );
            // Call object search recursively for each found container
            GetContainersL( ( *containerList )[ index ]->Id() );
            }

        startingIndex += containerList->ObjectCount();

        CleanupStack::PopAndDestroy( containerList );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::GetItemsL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::GetItemsL( const TDesC8& aContainerId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::GetItemsL" );

    // Number of found items. Count is initialized as KDefaultBrowseReqCount
    // to get into the while loop. Loop ends, when GetSharedItemListL
    // request does not get new objects
    TInt count( KDefaultBrowseReqCount );
    TInt startingIndex( 0 );
    TInt loopCounter( 0 );

    while ( startingIndex < count )
        {
        count = 0;

        CUpnpItemList* itemList = CUpnpItemList::NewL();
        CleanupStack::PushL( itemList );

        CUpnpBrowseCriteria* browseCriteria = CUpnpBrowseCriteria::NewLC();
        browseCriteria->SetRequestedCount( KDefaultBrowseReqCount );
        browseCriteria->SetStartingIndex( startingIndex );
        browseCriteria->SetFilterL( KBrowseCriteriaFilter );

        // Query all items of container with container id aContainerId
        iFileSharing->GetSharedItemListL(
            UPnPCommonUtils::DesC8ToInt( aContainerId ),
            *browseCriteria,
            *itemList,
            count,
            iStatus );

        CleanupStack::PopAndDestroy( browseCriteria );

        // Wait for the request to complete
        CompleteRequestL();

        // Leave, if GetSharedContainerListL gives continuously zero items
        if ( !itemList->ObjectCount() )
            {
            loopCounter += 1;
            if ( loopCounter >= KLoopLimit )
                {
                User::Leave( KErrGeneral );
                }
            }
        else
            {
            loopCounter = 0;
            }

        // Append files to the objects array
        for ( TInt index( 0 ); index < itemList->ObjectCount(); index++ )
            {
            // Local media applications store temporary items under
            // root container. Therefore items whose parent container
            // is root, are not appended to the objects array.
            if ( ( *itemList )[ index ]->ParentId().Match( KRootContainer )
                == KErrNotFound )
                {
                iUpnpCdsLiteObjects.AppendL( ( *itemList )[ index ] );
                }
            }

        startingIndex += itemList->ObjectCount();

        CleanupStack::PopAndDestroy( itemList );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::RunL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::RunL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::RunL" );

    // Request completed - no need to wait anymore
    // Let ao caller to continue
    StopWaitingRequestComplete();
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::DoCancel
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::DoCancel()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::DoCancel" );

    Cancel();
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::WaitRequestToComplete
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::WaitRequestToComplete()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::\
WaitRequestToComplete" );

    // Wait for a request to complete
    if ( iWaitRequest && !iWaitRequest->IsStarted() )
        {
        iWaitRequest->Start();
        }
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::StopWaitingRequestComplete
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::StopWaitingRequestComplete()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::\
StopWaitingRequestComplete" );

    // Stop waiting process after query has been completed
    if ( iWaitRequest && iWaitRequest->IsStarted() )
        {
        iWaitRequest->AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::CompleteRequestL
// --------------------------------------------------------------------------
//
void CUpnpCdsReaderAO::CompleteRequestL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsReaderAO::\
CompleteRequestL" );

    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        User::Leave( KErrInUse );
        }

    // Wait for request to complete
    WaitRequestToComplete();

    // Leave if error happens
    if ( iStatus.Int() != KErrNone &&
         iStatus.Int() != KErrNotFound )
        {
        User::Leave( iStatus.Int() );
        }
    }

//  End of File
