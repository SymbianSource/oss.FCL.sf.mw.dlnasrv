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
#include "upnpresultcacheui.h"
#include "upnplitecontainerfiller.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// CONSTANTS
const TInt KMaxPlaylistSize = 500; // MAX container size to play

// METHODS

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::NewL
//---------------------------------------------------------------------------
EXPORT_C CUPnPLiteContainerFiller* CUPnPLiteContainerFiller::NewL(
    const CUpnpAVDevice& aMediaServer,
    const CUpnpResultCacheUi& aResultCache )
    {
    CUPnPLiteContainerFiller* filler = 
        new(ELeave)CUPnPLiteContainerFiller( aResultCache );
    CleanupStack::PushL( filler );
    filler->ConstructL( aMediaServer );
    CleanupStack::Pop( filler );
    return filler;
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::FillL
// Fill track data into the playlist
// --------------------------------------------------------------------------
//
void CUPnPLiteContainerFiller::FillL(
    CUPnPMusicAdapter& aHost,
    CMPXMediaArray& aPlaylist )
    {
    __LOG( "CUPnPLiteContainerFiller::FillL" );
    SetHost( aHost );

    TUint count = iResultCache.TotalNumberOfItems();
    for( TUint i = 0; i < count; i++ )
	    {
	    // Convert to mpx media and add to the playlist.
    	FillItemMpxPlaylistL( aPlaylist, *iResultCache.ItemAt(i), 0 );
    	iPlaylistActualSize++;
	    }

    DoComplete();
    __LOG( "CUPnPLiteContainerFiller::FillL - End" );
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::CancelFill
// --------------------------------------------------------------------------
//
void CUPnPLiteContainerFiller::CancelFill()
    {
    // no implementation required
    // - this class does not have async nature
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::PlaylistSize
// Returns the total playlist size
// --------------------------------------------------------------------------
//
TInt CUPnPLiteContainerFiller::PlaylistSize()
    {
    return iPlaylistActualSize;
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::~CUPnPLiteContainerFiller
//---------------------------------------------------------------------------
EXPORT_C CUPnPLiteContainerFiller::~CUPnPLiteContainerFiller()
    {
    // None.
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::CUPnPCachePlaylistFiller
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPLiteContainerFiller::CUPnPLiteContainerFiller( 
    const CUpnpResultCacheUi& aResultCache )
    : iResultCache( aResultCache )
    {
    // None.
    }

// --------------------------------------------------------------------------
// CUPnPLiteContainerFiller::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
// 
void CUPnPLiteContainerFiller::ConstructL( 
    const CUpnpAVDevice& aMediaServer )
    {
    __LOG( "CUPnPLiteContainerFiller::ConstructL" );
    // Set device uid
    SetSourceDeviceL( aMediaServer );
    iPlaylistActualSize = 0;
    }


