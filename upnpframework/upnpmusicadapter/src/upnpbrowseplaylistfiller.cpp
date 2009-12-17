/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fills MPX playlist by browsing media server content
*
*/


// INCLUDE FILES
// upnp stack api
#include <upnpobject.h>

// MPX framework api
#include <mpxcollectionutility.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"
#include "upnpavdevice.h"
#include "upnpavbrowsingsession.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for browse filters

// upnpframework / xmlparser api
#include "upnpxmlparser.h"

// musicadapter internal
#include "upnpmusicadapter.h"
#include "upnpbrowseplaylistfiller.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// CONSTANTS
const TInt KBrowseRequestedCount = 100;
const TUid KHostAppUid = { 0x10208A0A };  // Host app UID
const TInt KMaxPlaylistSize = 500; // MAX container size to play
const TInt KBrowsePeriod = 5000000; // 5 seconds

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::NewL
// static constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPBrowsePlaylistFiller* CUPnPBrowsePlaylistFiller::NewL( 
    const TDesC8& aContainerId,
    TInt aCachedStartIndex,
    TInt aTotalContainerSize )
    {
    __LOG( "CUPnPBrowsePlaylistFiller::NewL" );
    
    CUPnPBrowsePlaylistFiller* self = new(ELeave)
        CUPnPBrowsePlaylistFiller();
    CleanupStack::PushL( self );
    self->ConstructL( aContainerId, aCachedStartIndex, aTotalContainerSize );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::CUPnPBrowsePlaylistFiller
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPBrowsePlaylistFiller::CUPnPBrowsePlaylistFiller()
    : CUPnPPlayListFiller()
    {
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
// 
void CUPnPBrowsePlaylistFiller::ConstructL(
    const TDesC8& aContainerId,
    TInt aCachedStartIndex,
    TInt aTotalContainerSize )
    {
    // Create timer for continue browse after short period.
    iBrowseTimer = CUPnPBrowseTimer::NewL( *this, KBrowsePeriod );
    iBlocksize = KBrowseRequestedCount;
    
    iContainerId = HBufC8::NewL( aContainerId.Length() );
    iContainerId->Des().Copy( aContainerId );    
    iCachedStartIndex = aCachedStartIndex;

    iPlaylistInitialSize = KErrUnknown;
    if ( aTotalContainerSize >= 0 )
        {
        iPlaylistInitialSize = Min(
            aTotalContainerSize - iCachedStartIndex,
            KMaxPlaylistSize );
        }
    iPlaylistActualSize = 0;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::~CUPnPBrowsePlaylistFiller
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPBrowsePlaylistFiller::~CUPnPBrowsePlaylistFiller()
    {
    delete iBrowseTimer;
    iBrowseTimer = 0;

    __LOG( "CUPnPBrowsePlaylistFiller::~CUPnPBrowsePlaylistFiller" );
    if ( iBrowsingSession )
        {
        Host().AVController().StopBrowsingSession( *iBrowsingSession );
        iBrowsingSession = 0;
        }
    delete iContainerId;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::FillL
// Fill track data into the playlist and continue browsing
// --------------------------------------------------------------------------
//    
void CUPnPBrowsePlaylistFiller::FillL(
    CUPnPMusicAdapter& aHost,
    CMPXMediaArray& aPlaylist )
    {
    __LOG( "CUPnPBrowsePlaylistFiller::Fill" );
    
    SetHost( aHost );

    // fill the initial items, increase the index
    iCurrentIndex = iCachedStartIndex;
    iPlaylistActualSize = DoFillL( aPlaylist, ObjectArray() );
    iCurrentIndex += ObjectArray().Count();

    if ( iPlaylistActualSize < iPlaylistInitialSize )
        {
        // Start doing more browse requests after short waiting period
        iBrowseTimer->Start();
        }
    else
        {
        __LOG( "CUPnPBrowsePlaylistFiller::Fill - \
        No need to browse" );
        DoComplete();
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::CancelFill
// Fill track data into the playlist and continue browsing
// --------------------------------------------------------------------------
//    
void CUPnPBrowsePlaylistFiller::CancelFill()
    {
    if ( iBrowsingSession )
        {
        iBrowsingSession->CancelBrowse();
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::PlaylistSize
// Returns the total playlist size
// --------------------------------------------------------------------------
//
TInt CUPnPBrowsePlaylistFiller::PlaylistSize()
    {
    if ( iPlaylistInitialSize >= 0 )
        {
        return iPlaylistInitialSize;
        }
    else
        {
        return ObjectArray().Count();
        }
    }

// --------------------------------------------------------------------------
// Methods from MUPnPAVBrowsingSessionObserver
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::BrowseResponse
// --------------------------------------------------------------------------
// 
void CUPnPBrowsePlaylistFiller::BrowseResponse(
    const TDesC8& aBrowseResponse, TInt aError, TInt /*aMatches*/,
    TInt aTotalCount, const TDesC8& /*aUpdateId*/ )
    {
    __LOG( "CUPnPBrowsePlaylistFiller::BrowseResponse" );
    TBool continueBrowse = ETrue;

    // Stop browsing if any error in previous browse 
    if( aError != KErrNone )
        {
        // Browse stops to an error - do not continue playlist.
        __LOG( "CUPnPBrowsePlaylistFiller::BrowseResponse ->error!" );
        continueBrowse = EFalse;
        DoComplete();
        }
    else 
        {
        __LOG( "CUPnPBrowsePlaylistFiller::BrowseResponse - \
            Error None" );

        TInt error = KErrNone;
        TRAP( error, AddMediaToCollectionL( aBrowseResponse ) );
        if( error != KErrNone )
            {
            __LOG( "CUPnPBrowsePlaylistFiller::BrowseResponse - \
            AddMediaToCollectionL leaves!" );
            continueBrowse = EFalse;
            DoComplete();
            }
        else
            {
            if ( iPlaylistActualSize >= iPlaylistInitialSize )
                {
                // browse is complete
                __LOG( "CUPnPBrowsePlaylistFiller::BrowseResult - \
                    ->playlist size reached." );
                continueBrowse = EFalse;
                DoComplete();
                }

            if ( iCurrentIndex >= aTotalCount )
                {
                // browse is complete
                __LOG( "CUPnPBrowsePlaylistFiller::BrowseResult - \
                    ->end of data." );
                continueBrowse = EFalse;
                DoComplete();
                }
            }     
        }

    if ( continueBrowse )
        {
        // Calculate item count to be browsed.
        iBlocksize = KBrowseRequestedCount;
        if ( iPlaylistActualSize + iBlocksize > iPlaylistInitialSize )
            {
            iBlocksize = iPlaylistInitialSize - iPlaylistActualSize;
            }

        // Continue the browse request            
        iBrowseTimer->Continue();
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::AddMediaToCollectionL
// Adds media to collection
// --------------------------------------------------------------------------
//    
void CUPnPBrowsePlaylistFiller::AddMediaToCollectionL( 
    const TDesC8& aBrowseResponse )
    {
    CUPnPXMLParser* parser = NULL;
    parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    RPointerArray<CUpnpObject> array;
    parser->ParseResultDataL( array, aBrowseResponse );
       
    // Update playlist counter
    iCurrentIndex += KBrowseRequestedCount;
  
    TInt count = array.Count();    
    if( count > 0 )
        {
        // Create new media object
        CMPXMedia* media = CMPXMedia::NewL(); 
        CleanupStack::PushL( media );
        // Set collection Uid
        media->SetTObjectValueL<TUid>( 
        KMPXMediaGeneralCollectionId,
        Host().PlaylistCollectionId() );
        // Set type
        media->SetTObjectValueL<TMPXGeneralType>( 
        KMPXMediaGeneralType, EMPXItem );
        // Set category
        media->SetTObjectValueL<TMPXGeneralCategory>( 
        KMPXMediaGeneralCategory, EMPXPlaylist );
        // Set playlist ID
        media->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralId, KHostAppUid.iUid );
        // Add playlist into media object
        CMPXMediaArray* playlist = CMPXMediaArray::NewL();
        CleanupStack::PushL( playlist ); 
        iPlaylistActualSize += DoFillL( *playlist, array );
        media->SetCObjectValueL( 
        KMPXMediaArrayContents, playlist );
        // Add count of items in playlist
        media->SetTObjectValueL( 
        KMPXMediaArrayCount,
        playlist->Count() );
        CleanupStack::PopAndDestroy( playlist );

        __LOG( "CUPnPBrowsePlaylistFiller::AddMediaToCollectionL - \
        Collection->SetSyncL" );
        // Add new playlist to the collection
        Host().CollectionUtility().Collection().SetSyncL( *media );
        CleanupStack::PopAndDestroy( media );
        }
    else
        {
        __LOG( "CUPnPBrowsePlaylistFiller::AddMediaToCollectionL - \
        No items received!" );
        User::Leave( KErrNotFound );
        }
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( parser );
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::MediaServerDisappeared
// The media server has vanished
// --------------------------------------------------------------------------
//    
void CUPnPBrowsePlaylistFiller::MediaServerDisappeared( 
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    __LOG( "CUPnPBrowsePlaylistFiller::MediaServerDisappeared" );
    DoComplete();
    }

// --------------------------------------------------------------------------
// Unused callback methods from the interface
// --------------------------------------------------------------------------
//    
 
void CUPnPBrowsePlaylistFiller::SearchResponse( 
    const TDesC8& /*aSearchResponse*/, TInt /*aError*/, TInt /*aMatches*/,
    TInt /*aTotalCount*/, const TDesC8& /*aUpdateId*/ ) {}
void CUPnPBrowsePlaylistFiller::SearchCapabilitiesResponse( TInt /*aError*/,
    const TDesC8& /*aSearchCapabilities*/ ) {}
void CUPnPBrowsePlaylistFiller::CreateContainerResponse( TInt /*aError*/, 
    const TDesC8& /*aObjectId*/ ) {}
void CUPnPBrowsePlaylistFiller::DeleteObjectResponse( TInt /*aError*/ ) {}
void CUPnPBrowsePlaylistFiller::ReserveLocalMSServicesCompleted( TInt 
    /*aError*/ ) {}

// --------------------------------------------------------------------------
// CUPnPBrowsePlaylistFiller::HandlePeriod
// Action when timer has expired.
// --------------------------------------------------------------------------
// 
void CUPnPBrowsePlaylistFiller::HandlePeriod()
    {
    __LOG( "CUPnPBrowsePlaylistFiller::HandlePeriod" );

    TRAP_IGNORE( 
    // start a browsing session if not already started
    if( iBrowsingSession == 0 )
        {
        __LOG( "Start a browsing session" );
        iBrowsingSession = &Host().AVController()
            .StartBrowsingSessionL( SourceDevice() );
        iBrowsingSession->SetObserver( *this );
        }

    __LOG( "Start doing more browse requests" );
    // Start doing more browse requests
    iBrowsingSession->BrowseL( *iContainerId, KFilterMin,
        MUPnPAVBrowsingSession::EDirectChildren, iCurrentIndex,
        iBlocksize, KSortNone );
        );
    }


