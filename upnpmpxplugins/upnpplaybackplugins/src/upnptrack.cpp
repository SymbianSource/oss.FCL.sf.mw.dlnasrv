/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Class for containing and obtaining music track-specific data
*                from media server
*
*/






// INCLUDES
#include <mpxmediaaudiodefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include <mpxattribute.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmessagegeneraldefs.h> // for messaging
#include <mpxplaybackmessagedefs.h> // for messaging

#include <escapeutils.h> // for unicode conversion

#include <upnpitem.h>
#include <upnpobject.h>
#include "upnpitemutility.h" // for FindElementByName & ResourceFromItemL
#include "upnpfileutility.h" // for IsFileProtectedL
#include "upnpconstantdefs.h" // browse filtering and item element names
#include <upnpdlnaprotocolinfo.h> // for resolving object mimetype
#include "upnpavcontroller.h"
#include "upnpavbrowsingsession.h"
#include "upnpavdevice.h"
#include "upnpavdevicelist.h"

#include "upnpitemresolverobserver.h" // MUPnPItemResolverObserver
#include "upnpitemresolverfactory.h" // factory class
#include "upnpitemresolver.h" // MUPnPItemResolver

#include "upnptrackobserver.h"
#include "upnptrack.h"

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// CONSTANTS
_LIT16( KUPnPPrefix, "upnp:" );   // Prefix for separate local/remote song
const TInt KUPnPPrefixLength = 5;
const TInt KCharCodeColon = 58;
const TInt KCharCodeSeparate = 42;
const TInt InitialTrackDuration = 1;
_LIT( KTimeFormatYearOnly, "%F%Y" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPTrack::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPTrack* CUPnPTrack::NewL( MUPnPAVController& aAvController )
    {
    __LOG( "CUPnPTrack::NewL" );
    CUPnPTrack* self = new( ELeave ) CUPnPTrack( aAvController );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::Delete
// deletor
// --------------------------------------------------------------------------
//
void CUPnPTrack::Delete()
    {
    __LOG1( "CUPnPTrack::Delete, state=%d", iState );
    if ( iState == EStateResolving )
        {
        // asynchronous delete - will be deleted after
        // asynchronous operation is complete. This is because the
        // S60 command can not be cancelled
        iState = EStateSelfDestruct;
        }
    else
        {
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CUPnPTrack::CUPnPTrack
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPTrack::CUPnPTrack( MUPnPAVController& aAvController )
    : iAvController( aAvController )
    , iQueriedAttributes()
    {
    iBrowsingSession = NULL;
    iOriginalURI = NULL;
    iMediaServer = NULL;
    iObjectId = NULL;
    iTrackObserver = NULL;
    // Use initial duration value (1). It will guarantee that in
    // case were remote device does not support duration query and playback
    // is unpaused, SetPosition is called with real value even
    // if remote player does not return real value for getposition
    // call. Note that SetPosition(0) is used when user really wants
    // to move to track beginning.
    iTrackDuration = InitialTrackDuration;
    iIsItemSolved = EFalse;
    iState = EStateIdle;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::~CUPnPTrack
// destructor.
// --------------------------------------------------------------------------
//
CUPnPTrack::~CUPnPTrack()
    {
    __LOG( "CUPnPTrack::~CUPnPTrack()" );
    StopBrowsingSession();

    // Free memory of owned members
    delete iOriginalURI;
    delete iMediaServer;        
    delete iObjectId;
    delete iItemResolver;
    iItemResolver = 0;
    __LOG( "CUPnPTrack::~CUPnPTrack End" );
    }

// --------------------------------------------------------------------------
// CUPnPTrack::ResolveURIL
// Resolve local / remote URI.
// --------------------------------------------------------------------------
//
void CUPnPTrack::ResolveURIL( const TDesC& aCodedTrackUri,
    MUPnPTrackObserver& aTrackObserver,
    TPlaybackDirection aPlaybackDirection )
    {  
    __LOG( "CUPnPTrack::ResolveURI" );

    delete iOriginalURI;
    iOriginalURI = 0;
    delete iMediaServer;
    iMediaServer = 0;
    delete iObjectId;
    iObjectId = 0;
    iTrackObserver = &aTrackObserver;

    // Either local to remote or remote to remote
    if( aPlaybackDirection == EDirectionRemote )
        {
        if( aCodedTrackUri.Find( KUPnPPrefix ) == 0 )
            {
            // track is located in remote
            iTrackLocation = ETrackLocationRemote;
            // parse and save media server id and object id
            ParsePiecesL( aCodedTrackUri );
            // start a browsing session
            StartBrowsingSessionL();
            // create resolver 
            iItemResolver =
                UPnPItemResolverFactory::NewRemoteItemResolverL(
                *iObjectId, *iBrowsingSession, 
                iDefaultSelector, KFilterCommon );
            }
        else // Local stored. Save URI to local member
            {
            // track is located in local file system
            iTrackLocation = ETrackLocationLocal;
            // store original URI
            iOriginalURI = HBufC::NewL( aCodedTrackUri.Length() );
            iOriginalURI->Des().Copy( aCodedTrackUri );
			// check for DRM protection
			if ( UPnPFileUtility::IsFileProtectedL( iOriginalURI->Des() ) )
				{
				// call back directly with an error code
				iTrackObserver->ResolveURIComplete( KErrNotSupported );
				return;
				}
			else
				{
	            // create resolver.
	            iItemResolver =
	                UPnPItemResolverFactory::NewLocalItemResolverL(
						*iOriginalURI, iAvController, iFirstSelector,
						UPnPItemResolverFactory::EOmitDrmCheck );
				}
            }
        }
    else // Remote to local direction
        {
        // track is located in remote
        iTrackLocation = ETrackLocationRemote;
        // parse and save media server id and object id
        ParsePiecesL( aCodedTrackUri );
        // start a browsing session
        StartBrowsingSessionL();
        // create resolver
        iItemResolver =
            UPnPItemResolverFactory::NewDownloadItemResolverL(
            *iObjectId, iAvController, *iBrowsingSession, 
            iFirstSelector, KFilterCommon );
        }

    // Resolve remote item. Calls back to ResolveComplete
    iState = EStateResolving;
    TRAPD( e, iItemResolver->ResolveL( *this ); )
    if ( e != KErrNone )
        {
        if ( iState == EStateSelfDestruct )
            {
            __LOG( "CUPnPTrack: self-destructing" );
            delete this;
            }
        else
            {
            iState = EStateIdle;
            User::Leave( e );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPTrack::StartBrowsingSessionL
// Finds the apropriate mediaserver and starts a browsing session
// --------------------------------------------------------------------------
//
void CUPnPTrack::StartBrowsingSessionL()
    {
    // Get list of media servers                 
    CUpnpAVDeviceList* devices = iAvController.GetMediaServersL();
    CleanupStack::PushL( devices );

    // Find out index of wanted media server from media server list 
    TInt index = KErrNotFound; 
    TInt count = devices->Count();

    for( TInt i = 0; i < count; i++ )
        {                                              
        // If media server id match.
        if( !iMediaServer->Compare( (*devices)[ i ]->Uuid() ) )
            {
            index = i;
            break;      
            }
        }

    // Leave if media server does not find by id 
    if( index == KErrNotFound )
        {
        __LOG1( "CUPnPTrack: Media server not found: %S",
            iMediaServer );
        User::Leave( KErrNotFound );    
        }

    // start a browsing session
    iBrowsingSession = &iAvController.StartBrowsingSessionL(
        *(*devices)[ index ] );
    CleanupStack::PopAndDestroy( devices );
    }

// --------------------------------------------------------------------------
// CUPnPTrack::StopBrowsingSession
// Finds the apropriate mediaserver and starts a browsing session
// --------------------------------------------------------------------------
//
void CUPnPTrack::StopBrowsingSession()
    {
    // Stop browsing session if exist.
    if( iBrowsingSession )
        {
        __LOG( "Stop browsing session" );
        iAvController.StopBrowsingSession( *iBrowsingSession );
        iBrowsingSession = 0;
        }
    }

// --------------------------------------------------------------------------
// CUPnPTrack::URI
// Return original URI of track.
// --------------------------------------------------------------------------
//  
const TDesC& CUPnPTrack::URI() const
    {
    __ASSERTD( iOriginalURI != 0,__FILE__, __LINE__ );
    return *iOriginalURI;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::UpnpURI
// Return True UPNP URI of track.
// --------------------------------------------------------------------------
//  
const TDesC8& CUPnPTrack::UpnpURI() const
    {
    __ASSERTD( iIsItemSolved,__FILE__, __LINE__ );
    return iItemResolver->Resource().Value();
    }

// --------------------------------------------------------------------------
// CUPnPTrack::UpnpItem
// Return True UPNP item representing the track
// --------------------------------------------------------------------------
//  
const CUpnpItem& CUPnPTrack::UpnpItem() const
    {
    __ASSERTD( iIsItemSolved,__FILE__, __LINE__ );
    return iItemResolver->Item();
    }
    
// --------------------------------------------------------------------------
// CUPnPTrack::FilePath
// Returns the file path of local item 
// --------------------------------------------------------------------------
//  
const TDesC& CUPnPTrack::FilePath() const
    {
    __ASSERTD( iTrackLocation == ETrackLocationRemote, __FILE__, __LINE__ );
    __ASSERTD( iIsItemSolved,__FILE__, __LINE__ );
    return iItemResolver->Resource().FilePath();
    }

// --------------------------------------------------------------------------
// CUPnPTrack::IsRemote
// Return location of track. ETrue = remote. EFalse = local
// --------------------------------------------------------------------------
//  
TBool CUPnPTrack::IsRemote() const
    {
    return iTrackLocation == ETrackLocationRemote;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::TrackDuration
// returns duration of the track
// --------------------------------------------------------------------------
// 
TInt CUPnPTrack::TrackDuration()
    {
    __LOG1( "CUPnPTrack::TrackDuration duration: %d", iTrackDuration );
    TInt ms = InitialTrackDuration;
    if ( iTrackDuration > InitialTrackDuration )
        {
        ms = iTrackDuration;
        }
    else if ( iIsItemSolved )
        {
        const CUpnpAttribute* attr = UPnPItemUtility
            ::FindAttributeByName( iItemResolver->Resource(),
             KAttributeDuration );
        if ( attr != 0 )
            {
            if ( UPnPItemUtility::UPnPDurationAsMilliseconds(
                attr->Value(), ms ) == KErrNone )
                {
                // store duration for quicker future queries
                iTrackDuration = ms;
                __LOG1( "store duration: %d", iTrackDuration );
                }
            }
        }
    return ms;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::SetTrackDuration
// overrides duration of the track
// --------------------------------------------------------------------------
// 
void CUPnPTrack::SetTrackDuration( TInt aMilliseconds )
    {
    iTrackDuration = aMilliseconds;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::GetMetaDataL
// Get metadata information for track from media server
// --------------------------------------------------------------------------
//  
void CUPnPTrack::GetMetaDataL( const TArray<TMPXAttribute>& aAttrs, 
    MMPXPlaybackPluginObserver& aObs )
    {
    // Check if remote track
    if( iTrackLocation == ETrackLocationLocal )
        {
        __LOG( "CUPnPTrack::GetMetaData - No metadata for local track!" );
        User::Leave( KErrNotSupported );
        }

    if ( iIsItemSolved )
        {
        __LOG( "CUPnPTrack::GetMetaDataL" );
        DeliverMedataL( aAttrs, aObs );
        }
    else
        {
        __LOG( "CUPnPTrack::GetMetaDataL - pending" );
        iMetadataObserver = &aObs;
        for( TInt i = 0; i < aAttrs.Count(); ++i )
            {
            iQueriedAttributes.AppendL( aAttrs[i] );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPTrack::SendMediaChangedEventL
// --------------------------------------------------------------------------
// 
void CUPnPTrack::SendMediaChangedEventL(
    MMPXPlaybackPluginObserver& aObs )
    {
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( KMPXMediaGeneralTitle );
    attrs.AppendL( KMPXMediaMusicArtist );
    attrs.AppendL( KMPXMediaMusicAlbum );
    attrs.AppendL( KMPXMediaMusicGenre );
    attrs.AppendL( KMPXMediaGeneralDate );
    attrs.AppendL( KMPXMediaMusicYear );
    attrs.AppendL( KMPXMediaGeneralComment );
    attrs.AppendL( KMPXMediaGeneralMimeType );

    // Create and fill CMPXMedia class 
    RArray<TInt> suppIds;
    CleanupClosePushL( suppIds );
    suppIds.AppendL( KMPXMediaIdMusic );
    suppIds.AppendL( KMPXMediaIdGeneral );
    suppIds.AppendL( KMPXMediaIdAudio );
    CMPXMedia* media = CMPXMedia::NewL( suppIds.Array() );
    CleanupStack::PopAndDestroy( &suppIds );
    CleanupStack::PushL( media );
    FillMediaFromItemL( *media, attrs.Array() );
    
    // create an MPX message
    CMPXMessage* msg = CMPXMessage::NewL();
    CleanupStack::PushL( msg );
    msg->SetTObjectValueL<TMPXMessageId>(
        KMPXMessageGeneralId, KMPXMessagePbMediaChanged );
    msg->SetCObjectValueL<CMPXMedia>(
        KMPXMessagePbMedia, media );

    // send message
    aObs.HandlePlaybackMessage( *msg );

    CleanupStack::PopAndDestroy( msg );
    CleanupStack::PopAndDestroy( media );
    CleanupStack::PopAndDestroy( &attrs );
    }

// --------------------------------------------------------------------------
// CUPnPTrack::DeliverMedatataL
// --------------------------------------------------------------------------
// 
void CUPnPTrack::DeliverMedataL( const TArray<TMPXAttribute>& aAttrs, 
    MMPXPlaybackPluginObserver& aObs, TInt aError )
    {
    __LOG( "CUPnPTrack::DeliverMedatata");

    // Create and fill CMPXMedia class 
    RArray<TInt> suppIds;
    CleanupClosePushL( suppIds );
    suppIds.AppendL( KMPXMediaIdMusic );
    suppIds.AppendL( KMPXMediaIdGeneral );
    suppIds.AppendL( KMPXMediaIdAudio );
    CMPXMedia* media = CMPXMedia::NewL( suppIds.Array() );
    CleanupStack::PopAndDestroy( &suppIds );        
    CleanupStack::PushL( media );

    if ( aError == KErrNone )
        {
        FillMediaFromItemL( *media, aAttrs );
        }

    // Return metadata
    aObs.HandleMedia( *media, aError );
    CleanupStack::PopAndDestroy( media );
    }

// --------------------------------------------------------------------------
// CUPnPTrack::FillMediaFromItemL
// --------------------------------------------------------------------------
// 
void CUPnPTrack::FillMediaFromItemL( CMPXMedia& aMedia,
    const TArray<TMPXAttribute>& aAttrs )
    {
    __ASSERTD( iIsItemSolved,__FILE__, __LINE__ );

    // Fill metadata 
    const CUpnpElement* elem;
    // Song title
    if( Exists( aAttrs, KMPXMediaGeneralTitle ) )
        {
        __LOG( "CUPnPTrack: Metadata: Title");
        aMedia.SetTextValueL(
            KMPXMediaGeneralTitle, *To16LC( 
            iItemResolver->Item().Title() ) );
        CleanupStack::PopAndDestroy();
        }
    // Artist
    if( Exists( aAttrs, KMPXMediaMusicArtist ) )
        {
        if ( ( elem = UPnPItemUtility::FindElementByName(
            iItemResolver->Item(), KElementArtist ) ) != 0 )
            {
            __LOG( "CUPnPTrack: Metadata: Artist" );
            aMedia.SetTextValueL(
                KMPXMediaMusicArtist, *To16LC( elem->Value() ) );
            CleanupStack::PopAndDestroy();
            }
        else if ( ( elem = UPnPItemUtility::FindElementByName(
            iItemResolver->Item(), KElementCreator ) ) != 0 )
            {
            __LOG( "CUPnPTrack: Metadata Creator" );
            aMedia.SetTextValueL(
                KMPXMediaMusicArtist, *To16LC( elem->Value() ) );
            CleanupStack::PopAndDestroy();
            }
        }
    // Album
    if( Exists( aAttrs, KMPXMediaMusicAlbum ) )
        {
        if ( ( elem = UPnPItemUtility::FindElementByName(
            iItemResolver->Item(), KElementAlbum ) ) != 0 )
            {
            __LOG( "CUPnPTrack: Metadata: Album");
            aMedia.SetTextValueL(
                KMPXMediaMusicAlbum, *To16LC( elem->Value() ) );
            CleanupStack::PopAndDestroy();
            }
        }
    // Genre
    if( Exists( aAttrs, KMPXMediaMusicGenre ) )
        {
        if ( ( elem = UPnPItemUtility::FindElementByName(
            iItemResolver->Item(), KElementGenre ) ) != 0 )
            {
            __LOG( "CUPnPTrack: Metadata: Genre" );
            aMedia.SetTextValueL(
                KMPXMediaMusicGenre, *To16LC( elem->Value() ) );
            CleanupStack::PopAndDestroy();
            }
        }
    // Date / Year
    if( ( elem = UPnPItemUtility::FindElementByName(
          iItemResolver->Item(), KElementDate ) ) != 0 )
        {
        TTime timestamp;
        TInt conversionError =
            UPnPItemUtility::UPnPDateAsTTime( elem->Value(), timestamp );
        if ( conversionError == KErrNone )
            {
            if ( Exists( aAttrs, KMPXMediaGeneralDate ) )
                {
                __LOG( "CUPnPTrack: Metadata: Date" );
                aMedia.SetTextValueL(
                    KMPXMediaGeneralDate, iTempBuf );
                }
            if ( Exists( aAttrs, KMPXMediaMusicYear ) )
                {
                __LOG( "CUPnPTrack: Metadata: Year" );
                timestamp.FormatL( iTempBuf, KTimeFormatYearOnly );
                aMedia.SetTextValueL(
                    KMPXMediaMusicYear, iTempBuf );
                }
            }
        }
    // Duration
    if( Exists( aAttrs, KMPXMediaGeneralDuration ) )
        {
        TInt duration = TrackDuration();
        if ( duration >= 0 )
            {
            aMedia.SetTObjectValueL<TInt>(
                KMPXMediaGeneralDuration, duration );
            }
        }
    // Size
    if( Exists( aAttrs, KMPXMediaGeneralSize ) )
        {
        const CUpnpAttribute* attr = UPnPItemUtility
            ::FindAttributeByName( iItemResolver->Resource(),
             KAttributeSize );
        if ( attr != 0 )
            {
            __LOG( "CUPnPTrack: Metadata: Size" );
            TInt size;
            TLex8 sizeconvert( attr->Value() );
            if ( sizeconvert.Val( size ) == KErrNone )
                {
                aMedia.SetTObjectValueL<TInt>(
                    KMPXMediaGeneralSize, size );
                }
            }
        }
    // Mimetype
    if( Exists( aAttrs, KMPXMediaGeneralMimeType ) )
        {        
        const CUpnpAttribute* attr = UPnPItemUtility
            ::FindAttributeByName( iItemResolver->Resource(),
             KAttributeProtocolInfo );
        if ( attr != 0 )
            {
            __LOG( "CUPnPTrack: Metadata: MimeType" );
            CUpnpDlnaProtocolInfo* pInfo =
                CUpnpDlnaProtocolInfo::NewL( attr->Value() );
            CleanupStack::PushL( pInfo );
            aMedia.SetTextValueL(
                KMPXMediaGeneralMimeType, *To16LC( pInfo->ThirdField() ) );
            CleanupStack::PopAndDestroy();
            CleanupStack::PopAndDestroy( pInfo );
            pInfo = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPTrack::Exists
// --------------------------------------------------------------------------
// 
TBool CUPnPTrack::Exists( const TArray<TMPXAttribute>& aAttrs,
    const TMPXAttributeData& aAttrData ) const
    {
    TBool found = EFalse;
    for( TInt i = 0; i < aAttrs.Count() && !found; ++i )
        {
        if ( aAttrs[i].ContentId() == aAttrData.iContentId &&
            aAttrs[i].AttributeId() & aAttrData.iAttributeId )
            {
            found = ETrue;
            }
        }
    return found;
    }

// --------------------------------------------------------------------------
// CUPnPTrack::To16LC
// --------------------------------------------------------------------------
// 
const HBufC16* CUPnPTrack::To16LC( const TDesC8& aText )
    {
    HBufC16* result = 0;
    result = EscapeUtils::ConvertToUnicodeFromUtf8L( aText );
    CleanupStack::PushL( result );
    return result;
    }

// --------------------------------------------------------------------------
// callbacks from MUPnPItemResolverObserver
// --------------------------------------------------------------------------
//  
void CUPnPTrack::ResolveComplete( const MUPnPItemResolver& /*aResolver*/,
    TInt aError )
    {
    __LOG1( "CUPnPTrack::ResolveComplete err: %d", aError );
    iIsItemSolved = ETrue;
    if ( iState == EStateResolving )
        {
        iState = EStateReady;

        // handle pending metadata query
        if ( iTrackLocation == ETrackLocationRemote && iMetadataObserver )
            {
            __LOG( "Handle pending metadata query");
             TRAP_IGNORE( 
                DeliverMedataL( iQueriedAttributes.Array(), *iMetadataObserver,
                aError ) );
             iMetadataObserver = 0;
             iQueriedAttributes.Reset();
            }

        // call back
        MUPnPTrackObserver& observer = *iTrackObserver;
        iTrackObserver = 0;
        observer.ResolveURIComplete( aError );
        }
    else if ( iState == EStateSelfDestruct )
        {
        __LOG( "CUPnPTrack: self-destructing" );
        delete this;
        }
    }
    
// --------------------------------------------------------------------------
// Private methods of CUPnPTrack
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPTrack::ParsePiecesL
// Parse and save media server id and object id from given descriptor
// --------------------------------------------------------------------------
//    
void CUPnPTrack::ParsePiecesL( const TDesC& aSong )
    {    
    // Delete if already exist
    if( iMediaServer )
        {
        delete iMediaServer;
        iMediaServer = 0;
        }
    if( iObjectId )
        {
        delete iObjectId;
        iObjectId = 0;
        }
    
    // Leave if argument is not valid
    if( aSong.Length() < KUPnPPrefixLength )
        {
        User::Leave( KErrArgument );
        }
        
    TInt lenght = aSong.Length();
    TInt position = 0;
    TChar colon( KCharCodeColon );
    TChar separate( KCharCodeSeparate );
        
    // At first separe "upnp:" prefix from descriptor 
    position = aSong.Locate( colon );   
    TPtrC tmp = aSong.Mid( position + 1, (lenght - KUPnPPrefixLength ) );
    
    // Get media server id
    position = tmp.Locate( separate );
    // Leave if separator character not found
    if( position == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    TPtrC mediaserverId = tmp.Left( position );
    iMediaServer = HBufC8::NewL( mediaserverId.Length() );
    iMediaServer->Des().Copy( mediaserverId );
        
    // Get object id
    TPtrC objId = tmp.Mid( position + 1, ( (tmp.Length() 
        - mediaserverId.Length() ) - 1 ) ); 
    iObjectId = HBufC8::NewL( objId.Length() );
    iObjectId->Des().Copy( objId );
    }


