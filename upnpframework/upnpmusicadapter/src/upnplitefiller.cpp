/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides services for filling MPX playlist
*
*/


// INCLUDE FILES
// System
#include <escapeutils.h> // for unicode conversion
#include <escapeutils.h> // for unicode conversion

// XML framework api
#include <mpxcollectionplaylist.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h" // for the device

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for KClassAudio

// upnpframework / xmlparser api
#include "upnpobjectlite.h"

// musicadapter internal
#include "upnpmusicadapter.h" // for Complete(this)
#include "upnplitefiller.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// CONSTANTS
_LIT16( KUPnPPrefix, "upnp:" );
_LIT16( KSeparator, "*" );
const TInt KMaxElementSize = 256; // from UPNP/DLNA

// METHODS

// --------------------------------------------------------------------------
// CUPnPLiteFiller::NewL
//---------------------------------------------------------------------------
CUPnPLiteFiller* CUPnPLiteFiller::NewL()
    {
    CUPnPLiteFiller* filler = 
        new(ELeave)CUPnPLiteFiller();
    CleanupStack::PushL( filler );
    filler->ConstructL();
    CleanupStack::Pop( filler );
    return filler;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::SetSelectedIndex
// Sets the index of pre-selected item
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPLiteFiller::SetSelectedIndex(
    TInt aSelectedIndex )
    {
    __ASSERTD( aSelectedIndex >= 0, __FILE__, __LINE__ );
    iSelectedIndex = aSelectedIndex;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::SetSourceDeviceL
// Set the media server the playlist is located
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::SetSourceDeviceL( const CUpnpAVDevice& aDevice )
    {
    if ( !iSourceDevice )
        {
        iSourceDevice = &aDevice;        
        iDeviceUid = HBufC::NewL( aDevice.Uuid().Length() );
        iDeviceUid->Des().Copy( aDevice.Uuid() );
        }
    else
        {
        // we only support playlists from a single device.
        __ASSERTD( &aDevice == iSourceDevice , __FILE__, __LINE__ );
        }
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::SetHost
// Assigns the host music adapter
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::SetHost( CUPnPMusicAdapter& aHost )
    {
    iHost = &aHost;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::Host
// returns the host music adapter
// --------------------------------------------------------------------------
//
CUPnPMusicAdapter& CUPnPLiteFiller::Host() const
    {
    __ASSERTD( iHost != 0 , __FILE__, __LINE__ );
    return *iHost;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::DoComplete
// Completes the object, causing it to be destroyed.
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::DoComplete()
    {
    Host().Complete( this );
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::FillL
// Fill track data into the playlist
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::FillL(
    CUPnPMusicAdapter& /*aHost*/,
    CMPXMediaArray& /*aPlaylist*/ )
    {
    // no implementation required
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::CancelFill
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::CancelFill()
    {
    // no implementation required
    // - this class does not have async nature
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::PlaylistSize
// Returns the total playlist size
// --------------------------------------------------------------------------
//
TInt CUPnPLiteFiller::PlaylistSize()
    {
    // no implementation required
    return 0;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::SelectedIndex
// Index of selected item
// --------------------------------------------------------------------------
//
TInt CUPnPLiteFiller::SelectedIndex() const
    {
    return iSelectedIndex;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::~CUPnPLiteFiller
//---------------------------------------------------------------------------
EXPORT_C CUPnPLiteFiller::~CUPnPLiteFiller()
    {
    delete iDeviceUid;
    iDeviceUid = 0;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::FillItemMpxPlaylistL
// Fills a single item to mpx playlist
// --------------------------------------------------------------------------
//
void CUPnPLiteFiller::FillItemMpxPlaylistL(  CMPXMediaArray& aPlaylist,
    const CUpnpObjectLite& aItem, TInt /*aIndex*/ )
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
    if( aItem.IsTypeOf( CUpnpObjectLite::EMusicItem ) )
        {
        ConvertToURI( *iDeviceUid, aItem, iTempBuf );
        }
    else // item is non audio set uri to zero
        {
        iTempBuf.Zero();
        }
    media->SetTextValueL( 
        KMPXMediaGeneralUri, iTempBuf );
    // Title        
    media->SetTextValueL( 
        KMPXMediaGeneralTitle, aItem.Title() );
    // Add track to the object list
    aPlaylist.AppendL( media );
    CleanupStack::Pop( media );
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::CUPnPLiteFiller
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPLiteFiller::CUPnPLiteFiller()
    {
    iSelectedIndex = 0;
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
// 
void CUPnPLiteFiller::ConstructL()
    {
    // None.
    }

// --------------------------------------------------------------------------
// CUPnPLiteFiller::ConvertToURI
// Convert UPnP item to playlist item URI string
// --------------------------------------------------------------------------
//  
void CUPnPLiteFiller::ConvertToURI( const TDesC& aDeviceUid,
    const CUpnpObjectLite& aItem, TDes& aBuffer )
    {
    // Start with upnp prefix
    aBuffer.Copy( KUPnPPrefix );
    aBuffer.Append( aDeviceUid );

    aBuffer.Append( KSeparator );
    // add object id (converting 8->16 bit)
    TBuf<KMaxElementSize> id16;
    id16.Copy( aItem.ObjectId() );
    aBuffer.Append( id16 );
    }


