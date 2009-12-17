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
* Description:  Fills MPX playlist by browsing media server content
*
*/


// INCLUDE FILES
// upnpframework / xmlparser api
#include "upnpobjectlite.h"

// musicadapter internal
#include "upnpliteselectionfiller.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// CONSTANTS
const TInt KItemListGranularity = 20;

// METHODS

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::NewL
//---------------------------------------------------------------------------
EXPORT_C CUPnPLiteSelectionFiller* CUPnPLiteSelectionFiller::NewL(
    const RPointerArray<CUpnpObjectLite>& aObjectList,
    const CUpnpAVDevice& aMediaServer )
    {
    CUPnPLiteSelectionFiller* filler = 
        new(ELeave)CUPnPLiteSelectionFiller();
    CleanupStack::PushL( filler );
    filler->ConstructL( aObjectList, aMediaServer );
    CleanupStack::Pop( filler );
    return filler;
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::FillL
// Fill track data into the playlist
// --------------------------------------------------------------------------
//
void CUPnPLiteSelectionFiller::FillL(
    CUPnPMusicAdapter& aHost,
    CMPXMediaArray& aPlaylist )
    {
    __LOG( "CUPnPLiteSelectionFiller::FillL" );
    SetHost( aHost );
    
    TInt count = iObjectList.Count();
    for( TInt i = 0; i < count; i++ )
        {
        // Convert to mpx media and add to the playlist.
    	FillItemMpxPlaylistL( aPlaylist, *iObjectList[i], 0 );
        }

    DoComplete();
    __LOG( "CUPnPLiteSelectionFiller::FillL - End" );
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::CancelFill
// --------------------------------------------------------------------------
//
void CUPnPLiteSelectionFiller::CancelFill()
    {
    // no implementation required
    // - this class does not have async nature
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::PlaylistSize
// Returns the total playlist size
// --------------------------------------------------------------------------
//
TInt CUPnPLiteSelectionFiller::PlaylistSize()
    {
    return iObjectList.Count();
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::~CUPnPLiteSelectionFiller
//---------------------------------------------------------------------------
EXPORT_C CUPnPLiteSelectionFiller::~CUPnPLiteSelectionFiller()
    {
    iObjectList.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::CUPnPLiteSelectionFiller
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPLiteSelectionFiller::CUPnPLiteSelectionFiller()
    : iObjectList( KItemListGranularity )
    {
    }

// --------------------------------------------------------------------------
// CUPnPLiteSelectionFiller::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
// 
void CUPnPLiteSelectionFiller::ConstructL( 
    const RPointerArray<CUpnpObjectLite>& aObjectList,
    const CUpnpAVDevice& aMediaServer )
    {
    __LOG( "CUPnPLiteSelectionFiller::ConstructL" );
    
    // Set object list
    TInt count = aObjectList.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iObjectList.AppendL( aObjectList[i] );
        }

    // Set device uid
    SetSourceDeviceL( aMediaServer );
    }


