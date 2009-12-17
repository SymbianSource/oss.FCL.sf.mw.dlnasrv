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
* Description:      Capsulating sql items
*
*/






#include <e32std.h>
#include "cmsqlaudioitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlAudioItem* CCmSqlAudioItem::NewL()
    {   
    CCmSqlAudioItem* self = CCmSqlAudioItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlAudioItem* CCmSqlAudioItem::NewLC()
    {    
    CCmSqlAudioItem* self = new ( ELeave ) CCmSqlAudioItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlAudioItem::~CCmSqlAudioItem()
    {
    delete iAlbumArtUri;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetArtistId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetArtistId( const TInt64 aArtistId )
    {
    iArtistId = aArtistId;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetAlbumId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetAlbumId( const TInt64 aAlbumId )
    {
    iAlbumId = aAlbumId;
    }    

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetGenreId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetGenreId( const TInt64 aGenreId )
    {
    iGenreId = aGenreId;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetDuration
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetDuration( const TInt aDuration )
    {
    iDuration = aDuration;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetBitrate
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetBitrate( const TInt aBitrate )
    {
    iBitrate = aBitrate;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetTrackNumber
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetTrackNumber( const TInt aTrackNumber )
    {
    iTrackNumber = aTrackNumber;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlAudioItem::SetAlbumArtUriL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlAudioItem::SetAlbumArtUriL( const TDesC8& aAlbumArtUri )
    {
    delete iAlbumArtUri;
    iAlbumArtUri = NULL;

    if( &aAlbumArtUri )
        {
        iAlbumArtUri = aAlbumArtUri.AllocL();
        }
    else
        {
        iAlbumArtUri = KNullDesC8().AllocL();
        }     
    }
    
// ---------------------------------------------------------------------------
// CCmSqlAudioItem::ArtistId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlAudioItem::ArtistId() const
    {
    return iArtistId;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::AlbumId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlAudioItem::AlbumId() const
    {
    return iAlbumId;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::GenreId
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt64 CCmSqlAudioItem::GenreId() const
    {
    return iGenreId;
    }

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::Duration
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CCmSqlAudioItem::Duration() const
    {
    return iDuration;
    } 

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::Bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlAudioItem::Bitrate() const
    {
    return iBitrate;
    }        

// ---------------------------------------------------------------------------
// CCmSqlAudioItem::TrackNumber
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlAudioItem::TrackNumber() const
    {
    return iTrackNumber;
    }
  
// ---------------------------------------------------------------------------
// CCmSqlAudioItem::AlbumArtUri
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmSqlAudioItem::AlbumArtUri() const
    {
    return *iAlbumArtUri;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlAudioItem::CCmSqlAudioItem()
    {                
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlAudioItem::ConstructL()
    {
    }    

// End of file

