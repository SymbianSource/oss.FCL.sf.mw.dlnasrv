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
 * Description:  CUpnpSharingAO class implementation
 *
 */

// INCLUDES
#include <upnpcontainer.h>
#include <upnpfilesharing.h>
#include "upnpsharingao.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpSecAccessController::NewL
// --------------------------------------------------------------------------
//
CUpnpSharingAO* CUpnpSharingAO::NewL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::NewL" );

    CUpnpSharingAO* self = new (ELeave) CUpnpSharingAO;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::ConstructL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
ConstructL" );

    if ( !CActiveScheduler::Current() )
        {
        iScheduler = new( ELeave ) CActiveScheduler;
        CActiveScheduler::Install( iScheduler );
        }

    // Add this active object to the active scheduler.
    CActiveScheduler::Add( this );

    iFileSharing = CUpnpFileSharing::NewL();
    iWaitRequest = new ( ELeave ) CActiveSchedulerWait();

    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::~CUpnpSharingAO
// --------------------------------------------------------------------------
//
CUpnpSharingAO::~CUpnpSharingAO()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
~CUpnpSharingAO" );

    delete iFileSharing;
    delete iWaitRequest;

    if ( CActiveScheduler::Current() == iScheduler )
        {
        CActiveScheduler::Install( NULL );
        }
    delete iScheduler;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::ShareFileL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::ShareFileL( const TDesC8& aParentContainerId,
                                 CUpnpItem* aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::ShareFileL" );

    iFileSharing->ShareItemL( aParentContainerId, *aItem, iStatus );

    CompleteRequestL();
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::ShareReferenceL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::ShareReferenceL( const TInt aParentContainerId,
                                      const TInt aOriginalItemId,
                                      CUpnpItem* aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::ShareReferenceL" );

    iFileSharing->ShareReferenceL( aParentContainerId,
                                   aOriginalItemId,
                                   *aItem,
                                   iStatus );

    CompleteRequestL();
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::ShareContainerL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::ShareContainerL( CUpnpContainer* aContainer )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::ShareContainerL" );

    iFileSharing->ShareContainerL( aContainer->ParentId(),
                                   *aContainer,
                                   iStatus );

    CompleteRequestL();
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::UnShareFileL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::UnshareFileL( const TInt aItemId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
UnShareFileL" );

    iFileSharing->UnshareItemL( aItemId, iStatus );

    CompleteRequestL();
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::UnShareContainerL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::UnshareContainerL( const TInt aContainerId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
UnShareContainerL" );

    iFileSharing->UnshareContainerL( aContainerId, iStatus );

    CompleteRequestL();
    }

// --------------------------------------------------------------------------
// CUpnpCdsReaderAO::CompleteRequestL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::CompleteRequestL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
CompleteRequestL" );

    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        User::Leave( KErrInUse );
        }

    // ...and then wait request to complete
    // RunL releases wait
    WaitRequestToComplete();
    // Leave if error happens on file unsharing
    User::LeaveIfError( iStatus.Int() );
    }

// --------------------------------------------------------------------------
// CUpnpSharingAO::RunL
// --------------------------------------------------------------------------
//
void CUpnpSharingAO::CUpnpSharingAO::RunL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::RunL" );

    // Request completed - no need to wait anymore
    // Let ao caller to continue
    StopWaitingRequestComplete();
    }

// ---------------------------------------------------------------------------
// CUpnpSharingAO::DoCancel
// ---------------------------------------------------------------------------
//
void CUpnpSharingAO::CUpnpSharingAO::DoCancel()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::DoCancel" );

    Cancel();
    }

// ---------------------------------------------------------------------------
// CUpnpSharingAO::WaitRequestToComplete
// ---------------------------------------------------------------------------
//
void CUpnpSharingAO::WaitRequestToComplete()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
WaitRequestToComplete" );

    if ( iWaitRequest &&
         !iWaitRequest->IsStarted() )
        {
        iWaitRequest->Start();
        }
    }

// ---------------------------------------------------------------------------
// CUpnpSharingAO::StopWaitingRequestComplete
// ---------------------------------------------------------------------------
//
void CUpnpSharingAO::StopWaitingRequestComplete()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAO::\
StopWaitingRequestComplete" );

    if ( iWaitRequest &&
         iWaitRequest->IsStarted() )
        {
        iWaitRequest->AsyncStop();
        }
    }

//  End of File
