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
* Description:  provides playlist handling services for UPnP framework
*
*/


// INCLUDES
// System
#include <escapeutils.h> // for unicode conversion

// upnp stack api
#include <upnpitem.h> // s60 upnp stack
#include <upnpelement.h> // s60 upnp stack

// MPX framework api
#include <mpxcollectionutility.h>
#include <mpxcollectionframeworkdefs.h> // main attribute keys
#include <mpxmediageneraldefs.h> // commonly used attribute keys
#include <mpxmediacontainerdefs.h> // container-specific attribute keys
#include <mpxmediamusicdefs.h> // music-specific attribute keys
#include <mpxcollectionplugin.hrh> // collection plugin types
#include <mpxmedia.h>
#include <mpxmediaarray.h>

// upnpframework / avcontroller helper api
#include "upnpitemutility.h" // for FindElementByName & ResourceFromItemL
#include "upnpconstantdefs.h" // for element names

// upnpframework / internal api's
#include "upnpcdsreselementutility.h" // for UriFromItemL 

// music adapter internal
#include "upnpmpxhelper.h" // for some mpx operations
#include "upnpplaylistservices.h" // ourselves

// upnpframework / settings engine api
#include "upnpsettingsengine.h"     // CUPnPSettingsEngine

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"


// constant definitions
const TUid KHomeConnectUid = { 0x10208A0A };
const TUid KMpxLocalCollectionUid = { 0x101FFC3A };


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPPlaylistServices* CUPnPPlaylistServices::NewL()
    {
    __LOG( "CUPnPPlaylistServices::NewL" );

    CUPnPPlaylistServices* self = new(ELeave) CUPnPPlaylistServices();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::CUPnPPlaylistServices
// Default constructor.
// --------------------------------------------------------------------------
//    
CUPnPPlaylistServices::CUPnPPlaylistServices()
    {
    }


// --------------------------------------------------------------------------
// CUPnPPlaylistServices::CUPnPPlaylistServices
// 2nd phase constructor
// --------------------------------------------------------------------------
//    
void CUPnPPlaylistServices::ConstructL()
    {
    __LOG( "CUPnPPlaylistServices::ConstructL" );
    iStatus = KErrNone;
    
    iCollectionUtility = MMPXCollectionUtility::NewL(
        (MMPXCollectionObserver*)0, KMcModePlaylist );
            
    // Initialize/Merge music collection databases
    RArray<TUid> uid;
    CleanupClosePushL( uid );
    uid.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TUid collection = iCollectionUtility->CollectionIDL( uid.Array() );
    TRAP( iStatus, iCollectionUtility->Collection().CommandL(
        EMcCmdCollectionInit,  collection.iUid ) );
    CleanupStack::PopAndDestroy( &uid );

    iMpxHelper = CUPnPMpxHelper::NewL( KHomeConnectUid );
    
    
    __LOG1( "CUPnPPlaylistServices::ConstructL end iStatus %d",
         iStatus );
    }


// --------------------------------------------------------------------------
// CUPnPPlaylistServices::~CUPnPPlaylistServices
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPPlaylistServices::~CUPnPPlaylistServices()
    {
    __LOG( "CUPnPPlaylistServices ~()" );
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = 0;
        }

    delete iMpxHelper;
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::ListPlaylistsL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::ListPlaylistsL(
    CDesCArray& aPlaylistIds,
    CDesCArray& aPlaylistNames )
    {
    __LOG( "ListPlaylistsL()" );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::ListPlaylistsL iStatus %d return",
         iStatus );
        return;
        }
        
    CMPXMedia* result = FindItemL( EPlaylists,
        KNullDesC, &KMPXMediaGeneralId, &KMPXMediaGeneralTitle );
    if ( result != 0 )
        {
        CleanupStack::PushL( result );
        __LOG("getting media array");
        const CMPXMediaArray* medias = result->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
        TInt count = medias->Count();
        for( TInt i = 0; i < count; ++i )
            {
            const CMPXMedia* entry = (*medias)[i];
            TMPXItemId id = *entry->Value<TMPXItemId>(
                KMPXMediaGeneralId );
            const TDesC& title = entry->ValueText(
                KMPXMediaGeneralTitle );
            __LOG3( "media array %d/%d [%S]",
                i, count, &title );
            aPlaylistIds.AppendL( Id2Desc( id ) );
            aPlaylistNames.AppendL( title );
            }
        CleanupStack::PopAndDestroy( result );
        }
    }


// --------------------------------------------------------------------------
// CUPnPPlaylistServices::OpenPlaylistL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::OpenPlaylistL(
    const TDesC& aPlaylistId,
    CDesCArray& aContentMedia )
    {
    __LOG1( "OpenPlaylistL(%S)", &aPlaylistId );

    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::OpenPlaylistL iStatus %d return",
         iStatus );
        return;
        }
        
    CMPXMedia* result = FindItemL( EPlaylistById, aPlaylistId,
        &KMPXMediaArrayContents, &KMPXMediaGeneralUri );
    if ( result != 0 )
        {
        CleanupStack::PushL( result );
        const CMPXMediaArray* resultMedias =
            result->Value<CMPXMediaArray>( KMPXMediaArrayContents );

        if ( resultMedias->Count() == 1 )
            {
            // fetch content 
            CMPXMedia* playlist = FetchPlaylistContentL( aPlaylistId );              
            if( playlist != 0 )
                {
                CleanupStack::PushL( playlist );
                const CMPXMediaArray* playlistMedias =
                playlist->Value<CMPXMediaArray>( KMPXMediaArrayContents );
                if ( playlistMedias )
                    {
                    TInt count = playlistMedias->Count();
                    for( TInt i = 0; i < count; ++i )
                        {
                        __LOG2( "accessing mediaList: item %d of %d",
                            i, count );
                        const CMPXMedia* entry = (*playlistMedias)[i];
                        aContentMedia.AppendL( entry->ValueText(
                            KMPXMediaGeneralUri ) );
                        }
                    }
                CleanupStack::PopAndDestroy( playlist );
                }
            }
        else
            {
            __LOG1("Unknown result medias count: %d",
                resultMedias->Count() );
            }
        CleanupStack::PopAndDestroy( result );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    // now ready to return
    }


// --------------------------------------------------------------------------
// CUPnPPlaylistServices::CreateTrackL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::CreateTrackL(
    const TDesC& aTrackPath,
    const CUpnpItem& aTrackMetadata )
    {
    __LOG1( "CreateTrackL(%S)", &aTrackPath );

    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::CreateTrackL iStatus %d return",
         iStatus );
        return;
        }
        
    CMPXMedia* item = CMPXMedia::NewL();
    CleanupStack::PushL( item );
    item->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    item->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    item->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong );
    item->SetTextValueL(
        KMPXMediaGeneralUri, aTrackPath );
    // Insert metadata
    HBufC16* buf = NULL;
    // title
    buf = EscapeUtils::ConvertToUnicodeFromUtf8L( aTrackMetadata.Title() );
    CleanupStack::PushL( buf );
    item->SetTextValueL(
        KMPXMediaGeneralTitle, *buf );
    CleanupStack::PopAndDestroy( buf );
    // artist
    if ( ( buf = GetElementLC( aTrackMetadata, KElementArtist ) ) != 0 )
        {
        item->SetTextValueL(
            KMPXMediaMusicArtist, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    else if ( ( buf = GetElementLC( aTrackMetadata, KElementCreator ) ) != 0 )
        {
        item->SetTextValueL(
            KMPXMediaMusicArtist, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    // album
    if ( ( buf = GetElementLC( aTrackMetadata, KElementAlbum ) ) != 0 )
        {
        item->SetTextValueL(
            KMPXMediaMusicAlbum, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    // genre
    if ( ( buf = GetElementLC( aTrackMetadata, KElementGenre ) ) != 0 )
        {
        item->SetTextValueL(
            KMPXMediaMusicGenre, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    // date and year
    const CUpnpElement* elem = UPnPItemUtility::FindElementByName(
        aTrackMetadata, KElementDate );
    if ( elem != 0 )
        {
        TTime timestamp;
        TInt err = 
            UPnPItemUtility::UPnPDateAsTTime( elem->Value(), timestamp );
        if( err == KErrNone )
            {
            item->SetTObjectValueL<TInt64>( 
                KMPXMediaGeneralDate, timestamp.Int64() );
            
            item->SetTObjectValueL<TInt64>( 
                KMPXMediaMusicYear, timestamp.Int64() );
            }
        }
        
    // duration
    const CUpnpElement* trackResource = 
        &UPnPItemUtility::ResourceFromItemL( aTrackMetadata );
     
    if( trackResource != 0 )
        {
        const CUpnpAttribute* attr = UPnPItemUtility
            ::FindAttributeByName( *trackResource, KAttributeDuration );

        if ( attr != 0 )
            {
            TInt ms = 0;
            UPnPItemUtility
                ::UPnPDurationAsMilliseconds( attr->Value(), ms );
            
            item->SetTObjectValueL<TInt>(
                KMPXMediaGeneralDuration, ms );
            }
        }

    // track number
    if ( ( buf = GetElementLC( aTrackMetadata, KElementTrackNumber ) ) != 0 )
        {
        item->SetTextValueL(
            KMPXMediaMusicAlbumTrack, *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    __LOG("...about to add track...");
    iMpxHelper->AddTrackL( item );

    CleanupStack::PopAndDestroy( item );
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::GetElement
// --------------------------------------------------------------------------
//
HBufC16* CUPnPPlaylistServices::GetElementLC(
    const CUpnpItem& aSource, const TDesC8& aSourceField ) const
    {
    HBufC16* result = 0;
    const CUpnpElement* elem = UPnPItemUtility::FindElementByName(
        aSource, aSourceField );
    if ( elem != 0 )
        {
        result = EscapeUtils::ConvertToUnicodeFromUtf8L( elem->Value() );
        CleanupStack::PushL( result );
        }
    return result;
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::CreatePlaylistL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::CreatePlaylistL(
    const TDesC& aPlaylistName,
    const MDesCArray& aTrackPaths,
    TDes* aPlaylistId )
    {
    __LOG1( "CreatePlaylistL(%S)", &aPlaylistName );

    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::CreatePlaylistL iStatus %d return",
         iStatus );
        return;
        }
        
    // create a media list
    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    CleanupStack::PushL( medias );

    // add track info into the media list
    for( TInt i = 0; i < aTrackPaths.MdcaCount(); ++i )
        {
        TPtrC16 trackUri  = aTrackPaths.MdcaPoint(i);
        CMPXMedia* entry = CMPXMedia::NewL();
        CleanupStack::PushL( entry );
        entry->SetTObjectValueL<TUid>(
            KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
        entry->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXItem );
        entry->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXSong );
        entry->SetTextValueL(
            KMPXMediaGeneralUri, trackUri );
        medias->AppendL( entry );
        CleanupStack::Pop( entry );
        }

    // create a media item for the playlist
    CMPXMedia* playlist = CMPXMedia::NewL();
    CleanupStack::PushL( playlist );

    CUPnPSettingsEngine* engine = CUPnPSettingsEngine::NewL();;
    CleanupStack::PushL( engine );
    
    HBufC* location = HBufC::NewLC( KMaxFileName );
    TPtr locationPtr( location->Des() );
    TBool isPhoneMemory;
    engine->GetCopyLocationL( locationPtr, isPhoneMemory );

    // add playlist info into the playlist media
    playlist->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    playlist->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    playlist->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXPlaylist );
    playlist->SetTextValueL(
        KMPXMediaGeneralTitle, aPlaylistName );
    playlist->SetTextValueL(
        KMPXMediaGeneralUri, locationPtr );
    playlist->SetCObjectValueL(
        KMPXMediaArrayContents, medias );
    playlist->SetTObjectValueL<TInt>(
        KMPXMediaArrayCount, medias->Count() );

    // add in collection
    __LOG("...about to add playlist...");
    iMpxHelper->AddPlaylistL( playlist );
    
    iCurrentId = *playlist->Value<TMPXItemId>(
        KMPXMediaGeneralId );

    CleanupStack::PopAndDestroy( location );
    CleanupStack::PopAndDestroy( engine );
    CleanupStack::PopAndDestroy( playlist );
    CleanupStack::PopAndDestroy( medias );

    // Find out playlist ID
    if ( aPlaylistId )
        {
        __LOG("...about to retrieve playlist id...");
        aPlaylistId->Copy( Id2Desc( iCurrentId ) );
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::AddMediaToPlaylistL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::AddMediaToPlaylistL(
    const TDesC& aPlaylistId,
    const TDesC& aTrackPath )
    {
    __LOG2( "AddMediaToPlaylistL(%S,%S)",
        &aPlaylistId, &aTrackPath );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::AddMediaToPlaylistL iStatus %d return",
         iStatus );
        return;
        }
        
    // create a media list
    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    CleanupStack::PushL( medias );

    // add new track to media list
    CMPXMedia* entry = CMPXMedia::NewL();
    CleanupStack::PushL( entry );
    entry->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    entry->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    entry->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong );
    entry->SetTextValueL(
        KMPXMediaGeneralUri, aTrackPath );
    medias->AppendL( entry );
    CleanupStack::Pop( entry );

    // create the playlist delta
    CMPXMedia* playlistDelta = CMPXMedia::NewL();
    CleanupStack::PushL( playlistDelta );

    // add playlist info into the playlist delta media
    playlistDelta->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    playlistDelta->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    playlistDelta->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXPlaylist );
    playlistDelta->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralId, Desc2Id( aPlaylistId ) );
    playlistDelta->SetCObjectValueL(
        KMPXMediaArrayContents, medias ); // new content
    playlistDelta->SetTObjectValueL<TInt>(
        KMPXMediaArrayCount, medias->Count() );

    // add in collection
    __LOG("Add playlist delta to collection");
    iMpxHelper->AddPlaylistL( playlistDelta );

    CleanupStack::PopAndDestroy( playlistDelta );
    CleanupStack::PopAndDestroy( medias );
    }


// --------------------------------------------------------------------------
// CUPnPPlaylistServices::DeleteTrackL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::DeleteTrackL(
    const TDesC& aContentFile )
    {
    __LOG1( "DeleteTrackL(%S)", &aContentFile );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::DeleteTrackL iStatus %d return",
         iStatus );
        return;
        }
        
    // item to delete
    CMPXMedia* item = CMPXMedia::NewL();
    CleanupStack::PushL( item );
    item->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    item->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    item->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong );
    item->SetTextValueL(
        KMPXMediaGeneralUri, aContentFile );
    iCollectionUtility->Collection().RemoveL( *item );
    CleanupStack::PopAndDestroy( item );
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::DeletePlaylistL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlaylistServices::DeletePlaylistL(
    const TDesC& aPlaylistId )
    {
    __LOG1( "DeletePlaylistL(%S)", &aPlaylistId );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::DeletePlaylistL iStatus %d return",
         iStatus );
        return;
        }
        
    // item to delete
    CMPXMedia* playlist = CMPXMedia::NewL();
    CleanupStack::PushL( playlist );
    playlist->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    playlist->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    playlist->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXPlaylist );
    playlist->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralId, Desc2Id( aPlaylistId ) );
    iCollectionUtility->Collection().RemoveL( *playlist );
    CleanupStack::PopAndDestroy( playlist );
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::IsValidTrackL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUPnPPlaylistServices::IsValidTrackL(
    const TDesC& aContentFile )
    {
    __LOG1( "IsValidTrackL(%S)", &aContentFile );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::IsValidTrackL iStatus %d return",
         iStatus );
        return EFalse;
        }
        
    TBool found = EFalse;
    CMPXMedia* track = FindItemL( ESongByUri, aContentFile );
    if ( track != 0 )
        {
        CleanupStack::PushL( track );
        TInt count = *track->Value<TInt>( KMPXMediaArrayCount );
        if ( count >= 0 )
            {
            found = ETrue;
            }
        CleanupStack::PopAndDestroy( track );
        }

    return found;
    }

// --------------------------------------------------------------------------
// CUPnPPlaylistServices::IsValidPlaylistL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUPnPPlaylistServices::IsValidPlaylistL(
    const TDesC& aPlaylistTitle )
    {
    __LOG1( "IsValidPlaylistL(%S)", &aPlaylistTitle );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPPlaylistServices::IsValidPlaylistL iStatus %d return",
         iStatus );
        return EFalse;
        }
        
    TBool found = EFalse;
    // find the playlist
    CMPXMedia* playlist = FindItemL( EPlaylistByTitle, aPlaylistTitle );
    if ( playlist != 0 )
        {
        CleanupStack::PushL( playlist );
        TInt count = *playlist->Value<TInt>( KMPXMediaArrayCount );
        if ( count >= 0 )
            {
            found = ETrue;
            }
        CleanupStack::PopAndDestroy( playlist );
        }
        
    return found;
    }


// --------------------------------------------------------------------------
// Find a playlist
// --------------------------------------------------------------------------
//
CMPXMedia* CUPnPPlaylistServices::FindItemL(
    TFindCategory aCategory,
    const TDesC& aKey,
    const TMPXAttributeData* aAttribute1,
    const TMPXAttributeData* aAttribute2,
    const TMPXAttributeData* aAttribute3 )
    {
    __LOG( "FindItemL" );
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    criteria->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );

    // set search keys
    if ( aCategory == ESongs ||
        aCategory == ESongByUri )
        {
        criteria->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXSong );
        if ( aCategory == ESongByUri )
            {
            criteria->SetTextValueL( 
                KMPXMediaGeneralUri, aKey );
            }
        }
    else if ( aCategory == EPlaylists ||
        aCategory == EPlaylistById ||
        aCategory == EPlaylistByTitle )
        {
        criteria->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXPlaylist );
        if ( aCategory == EPlaylistById )
            {
            criteria->SetTObjectValueL<TMPXItemId>(
                KMPXMediaGeneralId, Desc2Id( aKey ) );
            }
        else if ( aCategory == EPlaylistByTitle )
            {
            criteria->SetTextValueL( 
                KMPXMediaGeneralTitle, aKey );
            }
        }
    else
        {
        __PANICD( __FILE__, __LINE__ );
        }


    // define attributes fetched
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL( attributes );
    if ( aAttribute1 )
        {
        // add attribute 1
        attributes.AppendL( *aAttribute1 );
        if ( aAttribute2 )
            {
            // add attribute 2
            attributes.AppendL( *aAttribute2 );
            if ( aAttribute3 )
                {
                // add attribute 3
                attributes.AppendL( *aAttribute3 );
                }
            }
        }
    else
        {
        // add a dummy attribute because the query will fail if
        // there are no attributes
        attributes.AppendL( KMPXMediaGeneralId );
        }

    // now find
    CMPXMedia* result = 0;
    TRAPD( err, result = iCollectionUtility->Collection()
        .FindAllL( *criteria , attributes.Array() ) );
    CleanupStack::PopAndDestroy(); // attributes
    CleanupStack::PopAndDestroy( criteria );

    if ( err == KErrNotFound ||
         err == KErrPathNotFound )
        {
        __LOG1( "FindAll: err=%d - not found.", err );
        result = 0;
        }
    else if ( err != KErrNone )
        {
        __LOG1("FindAll: error=%d", err);
        User::Leave( err );
        }
    else
        {
        __LOG( "FindAll: ok" );
        }

    return result;
    }


// --------------------------------------------------------------------------
// Fetch a playlist content
// --------------------------------------------------------------------------
//   
CMPXMedia* CUPnPPlaylistServices::FetchPlaylistContentL( const TDesC& aId )
    {
    __LOG( "FindPlaylistContentL" );
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    criteria->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXGroup );

    // set search keys
    criteria->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong );    
    criteria->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralId, Desc2Id( aId ) );
        
    // define attributes fetched
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL( attributes );
    attributes.AppendL( KMPXMediaGeneralUri );
    
    // now find
    CMPXMedia* result = 0;
    TRAPD( err, result = iCollectionUtility->Collection()
        .FindAllL( *criteria , attributes.Array() ) );
    CleanupStack::PopAndDestroy(); // attributes
    CleanupStack::PopAndDestroy( criteria );
    
    if ( err == KErrNotFound ||
         err == KErrPathNotFound )
        {
        __LOG1( "FindAll: err=%d - not found.", err );
        result = 0;
        }
    else if ( err != KErrNone )
        {
        __LOG1("FindAll: error=%d", err);
        User::Leave( err );
        }
    else
        {
        __LOG( "FindAll: ok" );
        }

    return result;
    }

// --------------------------------------------------------------------------
// Converts an ID from TMPXItemId form to descriptor form.
// --------------------------------------------------------------------------
//
const TDesC& CUPnPPlaylistServices::Id2Desc( const TMPXItemId& aId )
    {  
    iTempBuffer.Num( aId );
    return iTempBuffer;
    }

// --------------------------------------------------------------------------
// Converts an ID from descriptor form to TMPXItemId form.
// --------------------------------------------------------------------------
//
TMPXItemId CUPnPPlaylistServices::Desc2Id( const TDesC& aDesc )
    {
    TLex convert( aDesc );
    TUint temp;
    convert.Val( temp, EDecimal );
    TMPXItemId id(temp);
    return id;
    }
