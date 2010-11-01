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
* Description:      CUpnpContentMetadataUtility class implementation
 *
*/






// INCLUDE FILES
// System
#include <e32base.h>
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItemListModel.h>
#include <CLFContentListing.hrh>
#include <MCLFItem.h>
#include <f32file.h>
// Internal
#include "upnpcontentmetadatautility.h"
#include "upnpcontentserverdefs.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KMediaTypeArrGranularity(1);

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS ============================
// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::CUpnpContentMetadataUtility()
// Default constructor
// --------------------------------------------------------------------------
//
CUpnpContentMetadataUtility::CUpnpContentMetadataUtility()
    {
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::ConstructL()
// ConstructL
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // Create iWait
    iWait = new (ELeave) CActiveSchedulerWait;
    
    // Create Content Listing Engine 
    iEngine = ContentListingFactory::NewContentListingEngineLC();
    CleanupStack::Pop(); // iEngine

    // Create list models
    iMusicModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop(); // iMusicModel
    iImageModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop(); // iImageModel
    iVideoModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop(); // iVideoModel

    // Set music media type filter to CLF
    RArray<TInt> musicArray( KMediaTypeArrGranularity );
    CleanupClosePushL( musicArray );
    musicArray.AppendL( ECLFMediaTypeMusic );
    iMusicModel->SetWantedMediaTypesL( musicArray.Array() );
    CleanupStack::PopAndDestroy( &musicArray );

    // Set image media type filter to CLF
    RArray<TInt> imageArray( KMediaTypeArrGranularity );
    CleanupClosePushL( imageArray );
    imageArray.AppendL( ECLFMediaTypeImage );
    iImageModel->SetWantedMediaTypesL( imageArray.Array() );
    CleanupStack::PopAndDestroy( &imageArray );

    // Set video media type filter to CLF
    RArray<TInt> videoArray( KMediaTypeArrGranularity );
    CleanupClosePushL( videoArray );
    videoArray.AppendL( ECLFMediaTypeVideo );
    iVideoModel->SetWantedMediaTypesL( videoArray.Array() );
    CleanupStack::PopAndDestroy( &videoArray );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::NewL()
// 2 phased constructor
// --------------------------------------------------------------------------
//
CUpnpContentMetadataUtility* CUpnpContentMetadataUtility::NewL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentMetadataUtility* self
        = new( ELeave ) CUpnpContentMetadataUtility;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::~CUpnpContentMetadataUtility()
// Default destructor
// --------------------------------------------------------------------------
//
CUpnpContentMetadataUtility::~CUpnpContentMetadataUtility()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iMusicModel;
    delete iImageModel;
    delete iVideoModel;
    delete iEngine;
    delete iWait;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::HandleOperationEventL
// Callback implementation for MCLFOperationObserver
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::HandleOperationEventL(
    TCLFOperationEvent aOperationEvent,
    TInt /*aError*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // Waiting is stopped when an event for refresh completion is received
    if( aOperationEvent == ECLFRefreshComplete )
        {
        // stop iWait
        if ( iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::MusicFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel* CUpnpContentMetadataUtility::MusicFiles()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    MCLFItemListModel* ret = iMusicModel;
    TRAPD( err, RefreshModelL( ECLFMediaTypeMusic ) );
    if ( !err )
        {
        WaitForRefreshToComplete();
        }
    else
        {
        ret = NULL;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::ImageFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel* CUpnpContentMetadataUtility::ImageFiles()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    MCLFItemListModel* ret = iImageModel;
    TRAPD( err, RefreshModelL( ECLFMediaTypeImage ) );
    if ( !err )
        {
        WaitForRefreshToComplete();
        }
    else
        {
        ret = NULL;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::VideoFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel* CUpnpContentMetadataUtility::VideoFiles()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    MCLFItemListModel* ret = iVideoModel;
    TRAPD( err, RefreshModelL( ECLFMediaTypeVideo ) );
    if ( !err )
        {
        WaitForRefreshToComplete();
        }
    else
        {
        ret = NULL;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }
    
// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::RefreshModelL
// ( other items are commented in header )
// --------------------------------------------------------------------------
void CUpnpContentMetadataUtility::RefreshModelL( 
    TCLFMediaType aCLFMediaType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    switch ( aCLFMediaType )
        {
        case ECLFMediaTypeImage:
            {
            iImageModel->RefreshL();
            break;
            }
        case ECLFMediaTypeVideo:
            {
            iVideoModel->RefreshL();
            break;
            }
        case ECLFMediaTypeMusic:
            {
            iMusicModel->RefreshL();
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::WaitForRefreshToComplete
// ( other items are commented in header )
// --------------------------------------------------------------------------
void CUpnpContentMetadataUtility::WaitForRefreshToComplete()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // start iWait
    if ( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }
    
//  End of File
