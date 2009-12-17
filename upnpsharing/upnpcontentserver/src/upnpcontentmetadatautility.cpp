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

// upnp stack api
#include <upnpitem.h>
#include <upnpcontainer.h>
#include <upnpstring.h>

// upnpframework / avcontroller helper api
#include "upnpfileutility.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"
#include "upnpmetadatautility.h"

#include "upnpdlnaprofiler.h"
#include "upnpcdsreselementutility.h"

// homeconnect internal
#include "upnpcontentmetadatautility.h"
#include "upnpcustomgrouper.h"
#include "upnppostfilter.h"

#include "upnpcontentserverdefs.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KUPnPFileListSeparator, "\t" );
const TInt KMediaTypeArrGranularity(1);

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS ============================
// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::CUpnpContentMetadataUtility()
// Default constructor
// --------------------------------------------------------------------------
//
CUpnpContentMetadataUtility::CUpnpContentMetadataUtility()
    : iRefreshOngoing( ETrue )
    {
    }

void CUpnpContentMetadataUtility::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // Create Content Listing Engine and a list model
    iEngine = ContentListingFactory::NewContentListingEngineLC();
    CleanupStack::Pop();   // iEngine
    iMusicModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop();    // iMusicModel

    iImageModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop();    // iImageModel

    iVideoModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop();    // iVideoModel

    iCollectionModel = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop();    // iCollectionModel
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

    // Set Collection media type filter to CLF
    RArray<TInt> collectionArray( KMediaTypeArrGranularity );

    CleanupClosePushL( collectionArray );
    collectionArray.AppendL( ECLFMediaTypeCollection );
    iCollectionModel->SetWantedMediaTypesL( collectionArray.Array() );
    CleanupStack::PopAndDestroy( &collectionArray );

    // Group items by collection name
    iCustomGrouper = CUpnpCustomGrouper::NewL( ECLFFieldIdCollectionName );
    iCollectionModel->SetCustomGrouper( iCustomGrouper );

    // Start to refresh the music files (HandleOperationEventL
    // callback comes when finished)
    iMusicModel->RefreshL();
    
    // Create metadata utility
    iMetaDataUtility = CUPnPMetaDataUtility::NewL();

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
    CleanupStack::Pop();
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
    delete iCollectionModel;
    delete iPostFilter;
    delete iCustomGrouper;
    delete iEngine;
    delete iMetaDataUtility;
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
        switch ( iRefreshCounter )
            {
        case 0 :
            {
            iVideoModel->RefreshL();
            break;
            }
        case 1 :
            {
            iImageModel->RefreshL();
            break;
            }
        case 2 :
            {
            iCollectionModel->RefreshL();
            break;
            }
        case 3 :
            {

            iRefreshOngoing = EFalse;
            iRefreshCounter = 0;
            if ( iHandler )
                {
                iHandler->RefreshDoneL();
                }
            __LOG("CUpnpContentMetadataUtility::HandleOperationEventL: \
Refresh done");
            break;

            }
        default :
            {
            __LOG("CUpnpContentMetadataUtility::HandleOperationEventL: \
default: error");
            break;
            }

            }
        if ( iRefreshOngoing )
            {
            iRefreshCounter++;
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::HandleItemChangeL
// CLF content is changed --> model needs to be refreshed
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::HandleItemChangeL(
    const TArray<TCLFItemId>& /*aItemIDArray*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // Start to refresh the music files (HandleOperationEventL
    // callback comes when finished)
    iRefreshOngoing = ETrue;
    iMusicModel->RefreshL();
    iImageModel->RefreshL();
    iVideoModel->RefreshL();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::HandleError
// Method is used to handle errors in changed item event.
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::HandleError( TInt /*aError*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::MusicFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel& CUpnpContentMetadataUtility::MusicFiles() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    return *iMusicModel;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::ImageFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel& CUpnpContentMetadataUtility::ImageFiles() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    return *iImageModel;
    }


// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::VideoFiles
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel& CUpnpContentMetadataUtility::VideoFiles() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    return *iVideoModel;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::Collections
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
const MCLFItemListModel& CUpnpContentMetadataUtility::Collections() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    return *iCollectionModel;
    }



// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::GetCollectionFileNamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::GetCollectionFileNamesL( 
    CDesCArray& aFileArray,
    const TDesC& aFiles ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt collectionLength( aFiles.Length() );
    TInt position( 0 );
    TInt dataPos( 0 );
    do
        {
        // Put filenames from ":" separeted row to aFileArray
        TPtrC data( aFiles.Right( collectionLength - position ) );
        dataPos = data.Find( KUPnPFileListSeparator );
        if( dataPos > 0 )
            {
            ++position; // skip KUPnPFileListSeparator
            position += dataPos;
            aFileArray.AppendL( data.Left( dataPos ) );
            }
        } while ( dataPos > 0 );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::CollectionItemsL()
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentMetadataUtility::CollectionItemsL( 
    const TDesC& aNameOfCollection )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG1( "CUpnpContentMetadataUtility: collection: %S",
                  &aNameOfCollection );

    //clear previous filtering
    iCollectionModel->SetPostFilter( NULL );

    // Delete old post filter if any

    delete iPostFilter;
    iPostFilter = NULL;

    // Create and activate a post filter for collection filtering
    // so that the model will contain only files
    // from selected collection
    iPostFilter = CUpnpPostFilter::NewL( ECLFFieldIdCollectionName,
                                            aNameOfCollection, EFalse );
    iCollectionModel->SetPostFilter( iPostFilter );

    iCollectionModel->RefreshL( ECLFRefreshPostFilter );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::CreateItemL
// Create the item with mandatory fields
// --------------------------------------------------------------------------
//
CUpnpItem* CUpnpContentMetadataUtility::CreateItemL(
    const MCLFItem& aCLFItem,
    const TDesC8& aParentId ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TPtrC fullFileName;
    TInt err( aCLFItem.GetField( ECLFFieldIdFileNameAndPath,
                                 fullFileName ));
    
    TInt32 filesize( 0 );
    TInt err1( aCLFItem.GetField( ECLFFieldIdFileSize, filesize ) );    
 
    CUpnpItem* newItem( NULL );
    
    if ( !err && !err1 && filesize )
        {
        newItem = CreateItemL( fullFileName, aParentId );
        }
    else
        {
        __LOG8_1( "MCLFItem ECLFFieldIdFileNameAndPath err= %d", err );
        __LOG8_1( "MCLFItem ECLFFieldIdFileSize err= %d", err1 );
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return newItem;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::CreateItemL
// Update the basic fields, based on list of filenames
// --------------------------------------------------------------------------
//
CUpnpItem* CUpnpContentMetadataUtility::CreateItemL(
    const TDesC& aFullFilename,
    const TDesC8& aParentId
    ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUpnpItem* newItem( NULL );
    
    TFileName path( aFullFilename );
    TBool isProtected( EFalse );
    TRAPD( err, 
           isProtected = UPnPFileUtility::IsFileProtectedL( path ) );

    if ( !isProtected && !err )
        {
        // title
        HBufC8* itemName( NULL );
        iMetaDataUtility->LoadTitleL( path );
        if( iMetaDataUtility->Title().Length() > 0 )
            {
            itemName = UpnpString::FromUnicodeL( 
                    iMetaDataUtility->Title() );
            }
        else
            {
            // If does not find the title, using filename instead
            TParse fileParser;
            fileParser.Set( path, NULL, NULL );
            itemName = UpnpString::FromUnicodeL( fileParser.Name() );
            }
        
        if ( itemName )
            {
            CleanupStack::PushL( itemName );
            
            newItem = CUpnpItem::NewL();
            CleanupStack::PushL( newItem );            
            newItem->SetTitleL( *itemName );            
            newItem->SetObjectClassL( KClassItem );
            newItem->SetParentIdL( aParentId );
            CleanupStack::Pop( newItem );

            CleanupStack::PopAndDestroy( itemName );
            }
        }
    else
        {
        __LOG8_1( "UPnPFileUtility::IsFileProtectedL err= %d", err );
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return newItem;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::RefreshOngoing() const
// Used to check if refresh is ongoing.
// --------------------------------------------------------------------------
//

TBool CUpnpContentMetadataUtility::RefreshOngoing() const
    {
    return iRefreshOngoing;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::UpdateMetaDataL
// Updates meta data for the item
// --------------------------------------------------------------------------
//
TBool CUpnpContentMetadataUtility::UpdateMetadataL(
    const TUpnpMediaType& aMediaType, 
    CUpnpItem* aItem, 
    const TDesC& aFileName )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // 1st use correct model according to item type
    MCLFItemListModel* model = NULL;
    switch( aMediaType )
        {
    case EMusicFile :
        {
        model = iMusicModel;
        break;
        }
    case EVideoFile :
        {
        model = iVideoModel;
        break;
        }
    case EPhotoFile :
        {
        model = iImageModel;
        break;
        }
    default:
        {
        break;
        }
        }
    // Then find the CLF item and update data from it
    TBool found = EFalse;
    TBool end = EFalse;
    TInt beginLoop = iClfIndex;

    // increment. If passed item count, start from beginning.
        iClfIndex = 0;
        beginLoop = -1;

    while ( model->ItemCount() && 
            !found && 
            !end )
        {
        // Get the item
        const MCLFItem& myItem = model->Item( iClfIndex );
        TPtrC fileName;
        TInt error = myItem.GetField( ECLFFieldIdFileNameAndPath,
                                      fileName );

        // if there was no error and file name matched
        if ( !error && aFileName.CompareF( fileName ) == 0 )
            {
            found = ETrue;

            // Get the mediatype
            TInt32 mediaType;
            TInt errorType = myItem.GetField( ECLFFieldIdMediaType,
                                              mediaType );
            // If it is music file, fill the meta data for it
            if ( !errorType && mediaType == ECLFMediaTypeMusic )
                {
                // Get title, artist, album & genre tag info of the item
                // Test update the class as it is in 1.x
                aItem->SetObjectClassL( 
                    KClassAudioMusicTrack );

                TPtrC songArtist;
                TInt errorArtist( myItem.GetField( ECLFFieldIdArtist,
                                                   songArtist ) );
                if ( !errorArtist )
                    {
                    CUpnpElement* elArtist = CUpnpElement::NewL( 
                        KElementArtist );
                    CleanupStack::PushL( elArtist );
                    HBufC8* artist = UpnpString::FromUnicodeL( 
                        songArtist );
                    CleanupStack::PushL( artist );
                    elArtist->SetValueL( *artist );
                    // UPnP stack needs filepath
                    elArtist->SetFilePathL( aFileName );
                    CleanupStack::PopAndDestroy( artist );
                    aItem->AddElementL( elArtist ); // transfer own..
                    CleanupStack::Pop( elArtist );
                    }
                TPtrC songAlbum;
                TInt errorAlbum( myItem.GetField( ECLFFieldIdAlbum,
                                                  songAlbum ) );
                if ( !errorAlbum )
                    {
                    CUpnpElement* elAlbum = CUpnpElement::NewL( 
                        KElementAlbum );
                    CleanupStack::PushL( elAlbum );
                    HBufC8* album = UpnpString::FromUnicodeL( songAlbum );

                    CleanupStack::PushL( album );
                    elAlbum->SetValueL( *album );
                    // UPnP stack needs filepath
                    elAlbum->SetFilePathL( aFileName );
                    CleanupStack::PopAndDestroy( album );
                    aItem->AddElementL( elAlbum ); // transfer own..
                    CleanupStack::Pop( elAlbum );
                    }
                TPtrC songGenre;
                TInt errorGenre( myItem.GetField( ECLFFieldIdGenre,
                                                  songGenre ) );
                if ( !errorGenre )
                    {
                    CUpnpElement* elGenre = CUpnpElement::NewL( 
                        KElementGenre );
                    CleanupStack::PushL( elGenre );
                    HBufC8* genre = UpnpString::FromUnicodeL( songGenre );

                    CleanupStack::PushL( genre );
                    elGenre->SetValueL( *genre );
                    // UPnP stack needs filepath
                    elGenre->SetFilePathL( aFileName );
                    CleanupStack::PopAndDestroy( genre );
                    aItem->AddElementL( elGenre ); // transfer own..
                    CleanupStack::Pop( elGenre );

                    }
                }
            else if ( !errorType && mediaType == ECLFMediaTypeImage )
                {
                // Just set correct object class
                aItem->SetObjectClassL( KImageItemObjectClass );
                }
            else if ( !errorType && mediaType == ECLFMediaTypeVideo )
                {
                // Just set correct object class
                aItem->SetObjectClassL( KVideoItemObjectClass );
                }

            // Below this happens to ALL media types
            TTime dateTime;
            TInt errorDate( myItem.GetField( ECLFFieldIdFileDate,
                                             dateTime ) );

        if ( !errorDate )
                {
                HBufC* date = NULL;
                TRAP( errorDate, date = 
                    UPnPCommonUtils::TTimeToUPnPDateL(dateTime));
                
                
                if(date && errorDate == KErrNone)
                    {
                    CleanupStack::PushL( date );
                    
                    CUpnpElement* elDate = CUpnpElement::NewL( 
                    KElementDate );
                    CleanupStack::PushL( elDate );
                
                    
                    HBufC8* date8 = UpnpString::FromUnicodeL( *date );
                    CleanupStack::PushL( date8 );
                    elDate->SetValueL( *date8 );
              
                    CleanupStack::PopAndDestroy( date8 );
                    
                    elDate->SetFilePathL( aFileName );
                    aItem->AddElementL( elDate ); // transfer own..
                
                    CleanupStack::Pop( elDate );
                    CleanupStack::Pop( date );
                    }
                    
               
                if(date)
                    {
                    delete date; 
                    date = NULL;
                    }
                }

            }
        else
            {
            // The item was not found
            if ( iClfIndex != beginLoop )
                {
                if ( ++iClfIndex >= model->ItemCount() )
                    {
                    iClfIndex = 0;
                    end = ETrue;
                    }
                }
            else
                {
                end = ETrue;
                __LOG("end=ETRue");
                }
            }
        } // while
    if( found )
        {
        __LOG("CUpnpContentMetadataUtility:: item found");
        }
    else
        {
        __LOG("CUpnpContentMetadataUtility:: item not found");
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return found;
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::ClearPostFiltersL
// ( other items are commented in header )
// --------------------------------------------------------------------------
void CUpnpContentMetadataUtility::ClearPostFiltersL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    //clear previous filtering
    iCollectionModel->SetPostFilter( NULL );

    // Delete old post filter if any

    delete iPostFilter;
    iPostFilter = NULL;
    
    iCollectionModel->RefreshL( ECLFRefreshPostFilter );

   // Set the default postfilter
    iPostFilter = CUpnpPostFilter::NewL( ECLFFieldIdCollectionName,
                                         KNullDesC, ETrue );
    iCollectionModel->SetPostFilter( iPostFilter );

    iCollectionModel->RefreshL( ECLFRefreshPostFilter );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentMetadataUtility::SetCallback
// ( other items are commented in header )
// --------------------------------------------------------------------------
void CUpnpContentMetadataUtility::SetCallback( 
    MUpnpMetadataObserver* aObserver )
    {
    iHandler = aObserver;

    }

//  End of File
