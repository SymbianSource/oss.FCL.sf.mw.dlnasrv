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
* Description:  Base class for filling an MPX playlist
*
*/


// INCLUDES
// System
#include <escapeutils.h> // for unicode conversion

// upnp utility 
#include "upnpitemutility.h"

// upnp stack api
#include <upnpobject.h>

// XML framework api
#include <mpxcollectionplaylist.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h" // for the device

// upnpframeowrk / avcotnroller helper api
#include "upnpconstantdefs.h" // for KClassAudio

// musicadapter internal
#include "upnpmusicadapter.h" // for Complete(this)
#include "upnpplaylistfiller.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// CONSTANTS
_LIT16( KUPnPPrefix, "upnp:" );
_LIT16( KSeparator, "*" );
const TInt KMaxElementSize = 256; // from UPNP/DLNA
const TInt KItemListGranularity = 20;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPPlayListFiller* CUPnPPlayListFiller::NewL()
    {
    CUPnPPlayListFiller* self = new(ELeave)
        CUPnPPlayListFiller();
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::CUPnPPlayListFiller
// Default constructor.
// --------------------------------------------------------------------------
//    
CUPnPPlayListFiller::CUPnPPlayListFiller()
    : iItemList( KItemListGranularity )
    {
    iSelectedIndex = 0;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::~CUPnPPlayListFiller
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPPlayListFiller::~CUPnPPlayListFiller()
    {
    __LOG( "CUPnPPlayListFiller::~CUPnPPlayListFiller" );
    
    iItemList.Reset();
    delete iDeviceUid;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::InsertObjectL
// Fill track data into the playlist
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlayListFiller::InsertObjectL(
    const CUpnpAVDevice& aMediaServer,
    const CUpnpObject& aObject )
    {
    if ( !iSourceDevice )
        {
        iSourceDevice = &aMediaServer;        
        iDeviceUid = HBufC::NewL( aMediaServer.Uuid().Length() );
        iDeviceUid->Des().Copy( aMediaServer.Uuid() );
        }
    else
        {
        // we only support playlists from a single device.
        __ASSERTD( &aMediaServer == iSourceDevice , __FILE__, __LINE__ );
        }

    iItemList.AppendL( &aObject );
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::SetSelectedIndex
// Sets the index of pre-selected item
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPlayListFiller::SetSelectedIndex(
    TInt aSelectedIndex )
    {
    __ASSERTD( aSelectedIndex >= 0, __FILE__, __LINE__ );
    __ASSERTD( aSelectedIndex < iItemList.Count(), __FILE__, __LINE__ );
    iSelectedIndex = aSelectedIndex;
    }


// --------------------------------------------------------------------------
// CUPnPPlayListFiller::FillL
// Fill track data into the playlist
// --------------------------------------------------------------------------
//
void CUPnPPlayListFiller::FillL(
    CUPnPMusicAdapter& aHost,
    CMPXMediaArray& aPlaylist )
    {
    __LOG( "CUPnPPlayListFiller::FillL" );
    SetHost( aHost );
    DoFillL( aPlaylist, iItemList );
    DoComplete();
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::CancelFill
// --------------------------------------------------------------------------
//
void CUPnPPlayListFiller::CancelFill()
    {
    // no implementation required
    // - this class does not have async nature
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::PlaylistSize
// Returns the total playlist size
// --------------------------------------------------------------------------
//
TInt CUPnPPlayListFiller::PlaylistSize()
    {
    return iItemList.Count();
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::ObjectArray
// returns a non-modifiable reference of the object list
// --------------------------------------------------------------------------
//
const RPointerArray<CUpnpObject>& CUPnPPlayListFiller::ObjectArray()
    {
    return iItemList;
    }


// --------------------------------------------------------------------------
// CUPnPPlayListFiller::SourceDevice
// returnsthe media server the playlist is located
// --------------------------------------------------------------------------
//
const CUpnpAVDevice& CUPnPPlayListFiller::SourceDevice()
    {
    return *iSourceDevice;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::SetHost
// Assigns the host music adapter
// --------------------------------------------------------------------------
//
void CUPnPPlayListFiller::SetHost( CUPnPMusicAdapter& aHost )
    {
    iHost = &aHost;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::Host
// returns the host music adapter
// --------------------------------------------------------------------------
//
CUPnPMusicAdapter& CUPnPPlayListFiller::Host() const
    {
    __ASSERTD( iHost != 0 , __FILE__, __LINE__ );
    return *iHost;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::SelectedIndex
// Index of selected item
// --------------------------------------------------------------------------
//
TInt CUPnPPlayListFiller::SelectedIndex() const
    {
    return iSelectedIndex;
    }


// --------------------------------------------------------------------------
// CUPnPPlayListFiller::DoFillL
// Fill track data into the playlist - base class implementation
// --------------------------------------------------------------------------
//
TInt CUPnPPlayListFiller::DoFillL( CMPXMediaArray& aPlaylist,
    const RPointerArray<CUpnpObject>& aSource, TInt /*aStartIndex*/ )
    {
    __LOG( "CUPnPPlayListFiller::DoFillL" );
    
    // Add currently cached items to the playlist
  
    TInt i = 0;
    for( ; i < aSource.Count() ; ++i )
        {
        DoFillObjectL( aPlaylist, *aSource[i], i );
        }
    
    __LOG( "CUPnPPlayListFiller::DoFillL - end" );
    
    return i;
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::DoFillObjectL
// Fills a ingle object to a playlist
// --------------------------------------------------------------------------
//
void CUPnPPlayListFiller::DoFillObjectL(  CMPXMediaArray& aPlaylist,
    const CUpnpObject& aObject, TInt /*aIndex*/ )
    {
    // Create new media object for track
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    // Set type
    media->SetTObjectValueL<TMPXGeneralType>( 
        KMPXMediaGeneralType, EMPXItem );
    // Set gategory
    media->SetTObjectValueL<TMPXGeneralCategory>( 
        KMPXMediaGeneralCategory, EMPXSong );
    // URI
    if( UPnPItemUtility::BelongsToClass( aObject , KClassAudio ) )
        {
        ConvertToURI( *iDeviceUid, aObject, iTempBuf );
        }
    else // item is non audio set uri to KUPnPPrefix
    	{
    	iTempBuf.Copy( KUPnPPrefix );
    	}
    
    media->SetTextValueL( 
        KMPXMediaGeneralUri, iTempBuf );
    // Title
    HBufC16* buf = 
        EscapeUtils::ConvertToUnicodeFromUtf8L( aObject.Title() );
    CleanupStack::PushL( buf );
    media->SetTextValueL( 
        KMPXMediaGeneralTitle, *buf );
    CleanupStack::PopAndDestroy( buf );
    // Add track to the object list
    aPlaylist.AppendL( media );
    CleanupStack::Pop( media );
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::DoComplete
// Completes the object, causing it to be destroyed.
// --------------------------------------------------------------------------
//
void CUPnPPlayListFiller::DoComplete()
    {
    Host().Complete( this );
    }

// --------------------------------------------------------------------------
// CUPnPPlayListFiller::ConvertToURI
// Convert UPnP item to playlist item URI string
// --------------------------------------------------------------------------
//  
void CUPnPPlayListFiller::ConvertToURI( const TDesC& aDeviceUid,
    const CUpnpObject& aItem, TDes& aBuffer )
    {
    // Start with upnp prefix
    aBuffer.Copy( KUPnPPrefix );
    aBuffer.Append( aDeviceUid );

    aBuffer.Append( KSeparator );
    // add object id (converting 8->16 bit)
    TBuf<KMaxElementSize> id16;
    id16.Copy( aItem.Id() );
    aBuffer.Append( id16 );
    }



