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






#ifndef __CMSQLAUDIOITEM_H
#define __CMSQLAUDIOITEM_H

// INCLUDES
#include <e32base.h>
#include "cmsqlgenericitem.h"

// FORWARD DECLARATIONS

/**
 *  CCmSqlAudioItem class
 *  Capsulating sql items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlAudioItem : public CCmSqlGenericItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlAudioItem class.
     * @param None
     * @return  pointer to CCmSqlAudioItem class
     */
    IMPORT_C static CCmSqlAudioItem* NewL();
    
    /**
     * Creates new CCmSqlAudioItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlAudioItem class
     */
    IMPORT_C static CCmSqlAudioItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlAudioItem();
    
public:

    /**
    * Sets artist id
    * @since Series 60 3.1
    * @param aArtistId, artist Id
    * @return None
    */  
    IMPORT_C void SetArtistId( const TInt64 aArtistId );

    /**
    * Sets album id
    * @since Series 60 3.1
    * @param aAlbumId, Album Id
    * @return None
    */     
    IMPORT_C void SetAlbumId( const TInt64 aAlbumId );
    
    /**
    * Sets genre id
    * @since Series 60 3.1
    * @param aGenreId, Genre Id
    * @return None
    */     
    IMPORT_C void SetGenreId( const TInt64 aGenreId );
    
    /**
    * Sets duration
    * @since Series 60 3.1
    * @param aDuration, duration
    * @return None
    */     
    IMPORT_C void SetDuration( const TInt aDuration );
    
    /**
    * Sets bitrate
    * @since Series 60 3.1
    * @param aBitrate, Bitrate
    * @return None
    */     
    IMPORT_C void SetBitrate( const TInt aBitrate );
    
    /**
    * Sets track number
    * @since Series 60 3.1
    * @param aTrackNumber, Track number
    * @return None
    */     
    IMPORT_C void SetTrackNumber( const TInt aTrackNumber );
    
    /**
    * Sets albumarturi number
    * @since Series 60 3.1
    * @param aAlbumArtUri, album art uri
    * @return None
    */     
    IMPORT_C void SetAlbumArtUriL( const TDesC8& aAlbumArtUri );

    /**
    * Gets artist id
    * @since Series 60 3.1
    * @param None
    * @return artist id
    */    
    IMPORT_C TInt64 ArtistId() const;

    /**
    * Gets album id
    * @since Series 60 3.1
    * @param None
    * @return album id
    */     
    IMPORT_C TInt64 AlbumId() const;

    /**
    * Gets genre id
    * @since Series 60 3.1
    * @param None
    * @return genre id
    */     
    IMPORT_C TInt64 GenreId() const;

    /**
    * Gets duration of the item ( in seconds )
    * @since Series 60 3.1
    * @param None
    * @return duration
    */     
    IMPORT_C TInt Duration() const;

    /**
    * Gets bitrate of the item ( bits/sec )
    * @since Series 60 3.1
    * @param None
    * @return bitrate
    */     
    IMPORT_C TInt Bitrate() const;
   
    /**
    * Gets track number
    * @since Series 60 3.1
    * @param None
    * @return track number
    */     
    IMPORT_C TInt TrackNumber() const;
    
    /**
    * Gets album art uri of the item
    * @since Series 60 3.1
    * @return Album art uri
    */    
    IMPORT_C TDesC8& AlbumArtUri() const;    
    
protected:

    /**
     * Constructor.
     */
    CCmSqlAudioItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    // Artist id
    TInt64 iArtistId;
    
    // Album id
    TInt64 iAlbumId;
    
    // Genre id
    TInt64 iGenreId;
    
    // Duration in seconds
    TInt iDuration;
    
    // Bitrate in bits/sec
    TInt iBitrate;
    
    // Track number
    TInt iTrackNumber;
    
    // Album art uri
    HBufC8* iAlbumArtUri; // owned    
    
    };

#endif //  __CMSQLAUDIOITEM_H