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


#ifndef C_UPNPPLAYLISTSERVICE_H
#define C_UPNPPLAYLISTSERVICE_H

// INCLUDES
#include <e32base.h>
#include <mpxcommonframeworkdefs.h> // for TMPXItemId

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class TMPXAttributeData;
class CMPXMedia;
class CUpnpItem;
class CUPnPMpxHelper;

// constants
const TInt KMaxPlaylistId = 32; // Maximum playlist ID length
const TInt KTempBufferSize = 255; // from DLNA

/**
 * A class providing abstract playlist handling services.
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPPlaylistServices
    :public CBase
    {

public: // construction / destruction

    /**
     * Static constructor
     */
    IMPORT_C static CUPnPPlaylistServices* NewL();

    /**
     * desctuctor
     */
    virtual ~CUPnPPlaylistServices();

public: // component services

    /**
     * Lists all playlists in the music library.
     * Returns a list of playlist ID's and a names.
     * ID can be used to open or modify the playlist.
     * @param aPlaylistIds array to receive playlist ID's
     * @param aPlaylistNames array to receive playlist names
     */
    IMPORT_C void ListPlaylistsL(
        CDesCArray& aPlaylistIds,
        CDesCArray& aPlaylistNames );

    /**
     * Retrieves contents of given playlist.
     * @param aPlaylistId ID of the playlist
     * @param aContentMedia array to receive the content file paths
     */
    IMPORT_C void OpenPlaylistL(
        const TDesC& aPlaylistId,
        CDesCArray& aContentMedia );

    /**
     * Creates a record of a music track into the music library.
     * The media is assumed to exist already.
     * @param aTrackPath path to the track media
     * @param aTrackMetadata the upnp item for track metadata
     */
    IMPORT_C void CreateTrackL(
        const TDesC& aTrackPath,
        const CUpnpItem& aTrackMetadata );

    /**
     * Creates a playlist by given data into the music library.
     * The tracks should be introduced to MPX using CreateTrackL
     * prior to calling this method.
     * @param aPlaylistName the name of the playlist
     * @param aTrackPaths array of playlist content file paths
     * @param aPlaylistId receives the playlist ID (KMaxPlaylistId)
     *                    Can also be NULL, if ID is not needed.
     */
    IMPORT_C void CreatePlaylistL(
        const TDesC& aPlaylistName,
        const MDesCArray& aTrackPaths,
        TDes* aPlaylistId );

    /**
     * Adds a file into an existing playlist.
     * The track should be introduced to MPX using CreateTrackL
     * prior to calling this method.
     * @param aPlaylistId ID of the playlist
     * @param aTrackPath path to the track media
     */
    IMPORT_C void AddMediaToPlaylistL(
        const TDesC& aPlaylistId,
        const TDesC& aTrackPath );

    /**
     * Deletes a record of a music track from the music library.
     * Note that this does not delete the actual media.
     * @param aTrackPath the actual file path
     */
    IMPORT_C void DeleteTrackL(
        const TDesC& aTrackPath );

    /**
     * Deletes a record of a playlist from the music library.
     * Note that this does not delete the actual media.
     * @param aPlaylistId ID of the playlist
     */
    IMPORT_C void DeletePlaylistL(
        const TDesC& aPlaylistId );

    /**
     * Checks if the given track exists in the music library.
     * Note: does not check the existence of the actual media
     * @param aTrackPath the actual file path
     */
    IMPORT_C TBool IsValidTrackL(
        const TDesC& aTrackPath );

    /**
     * Checks if the given playlist exists in the music library.
     * Note: does not check the existence of the actual media
     * @param aPlaylistId ID of the playlist
     */
    IMPORT_C TBool IsValidPlaylistL(
        const TDesC& aPlaylistName );


protected: // private methods

    /**
     * category to be used in FindItemL
     */
    enum TFindCategory {
        ESongs,
        ESongByUri,
        EPlaylists,
        EPlaylistById,
        EPlaylistByTitle
    };

    /**
     * Finds a playlist according to given criteria.
     * @param find category (as in TMPXGeneralCategory)
     * @param aKey find key, depends on the category
     * @param aAttribute1 attribute to get (optional)
     * @param aAttribute2 attribute to get (optional)
     * @param aAttribute3 attribute to get (optional)
     * @return the playlist media, or NULL if not found
     */
    CMPXMedia* FindItemL(
        TFindCategory aCategory,
        const TDesC& aKey,
        const TMPXAttributeData* aAttribute1 = 0,
        const TMPXAttributeData* aAttribute2 = 0,
        const TMPXAttributeData* aAttribute3 = 0 );

    /**
     * Fetch a playlist content by id
     * @param aId playlist id
     * @return the playlist media, or NULL if not found
     */
    CMPXMedia* FetchPlaylistContentL( const TDesC& aId );

    /**
     * Converts an ID from TMPXItemId form to descriptor form.
     * conversion is guaranteed symmetric to Desc2Id()
     * @param aId the id in TMPXItemId form
     * @return the id string representation
     */
    const TDesC& Id2Desc( const TMPXItemId& aId );

    /**
     * Converts an ID from descriptor form to TMPXItemId form.
     * conversion is guaranteed symmetric to Id2Desc()
     * @param aDesc the id as a descriptor
     * @return the id extracted from the string
     */
    TMPXItemId Desc2Id( const TDesC& aDesc );

    /**
     * Finds an element within CUpnpItem
     * returns the element value converted to unicode,
     * pushed to cleanup stack.
     * @param aSource the source CUpnpItem
     * @param aSourceField the element name within source
     * @return the vield value, or 0 if element not found.
     */
    HBufC16* GetElementLC( const CUpnpItem& aSource,
        const TDesC8& aSourceField ) const;

private:

    /**
     * default constructor
     */
    CUPnPPlaylistServices();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: // data

    /**
     * Collection utility of MPX framework
     */
    MMPXCollectionUtility*      iCollectionUtility;

    /**
     * internal helper for some MPX operations
     */
    CUPnPMpxHelper*             iMpxHelper;


    /**
     * temp buffer used in Id2Desc
     */
    TBuf<KTempBufferSize>       iTempBuffer;

    /**
     * ID of item currently processed asynchronously
     */
    TMPXItemId                  iCurrentId;

    /**
     * tells if collection plugin exists
     */
    TInt                        iStatus;
    };


#endif // C_UPNPPLAYLISTSERVICE_H
