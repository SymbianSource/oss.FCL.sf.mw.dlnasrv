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
* Description:  Header file for the CUpnpPlaylistHandler class
*
*/


#ifndef UPNP_FILE_TRANSFER_ENGINE_PLAYLIST_HANDLER_H
#define UPNP_FILE_TRANSFER_ENGINE_PLAYLIST_HANDLER_H

//  INCLUDES
#include <badesca.h>

// FORWARD DECLARATION
class CUpnpObject;
class CUpnpItem;
class CUPnPPlaylistServices;

// DATA TYPES
enum TUpnpPlaylistLocation
    {
    EPhoneMemory = 0,
    EMemorycard,
    EHarddisk,
    EUnknown
    };

// CLASS DECLARATION

/**
* CUpnpPlaylistHandler
* The class stores the playlist information, and generates MPX playlists and
* Gallery albums when requested.
*
* @since S60 3.1
*/
NONSHARABLE_CLASS( CUpnpPlaylistHandler ) : public CBase
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CUpnpPlaylistHandler* NewL();

        /**
         * Destructor.
         */
        virtual ~CUpnpPlaylistHandler();

    public: // Methods for managing the content

        /**
         * Resets the playlists content.
         *
         * @since S60 3.1
         */
        void Reset();

        /**
         * Sets the name for the playlist
         *
         * @since S60 3.1
         * @param aPlaylistName (const TDesC&) name for the playlist and
         *        image and video albums (same name will for all tree).
         */
        void SetPlaylistNameL( const TDesC& aPlaylistName );

        /**
         * Adds a audio item to the playlist (playlist will not be generated
         * yet, it has to be created by calling a separate method).
         *
         * @since S60 3.1
         * @param aFilePath (const TDesC&) filepath of the audio file
         */
        void AddAudioItemL( const TDesC& aFilePath );

        /**
         * Adds an image item to the playlist (playlist will not be generated
         * yet, it has to be created by calling a separate method).
         *
         * @since S60 3.1
         * @param aFilePath (const TDesC&) filepath of the image file
         */
        void AddImageItemL( const TDesC& aFilePath );

        /**
         * Adds a video item to the playlist (playlist will not be generated
         * yet, it has to be created by calling a separate method).
         *
         * @since S60 3.1
         * @param aFilePath (const TDesC&) filepath of the video file
         */
        void AddVideoItemL( const TDesC& aFilePath );

    public: // Methods for getting the state of the playlist
    
        /**
         * Returns the number of items in the audio item array
         *
         * @since S60 3.1
         * @return (TInt) the item count
         */
        TInt AudioItemCount();
        
        /**
         * Returns the number of items in the image item array
         *
         * @since S60 3.1
         * @return (TInt) the item count
         */
        TInt ImageItemCount();

        /**
         * Returns the number of items in the video item array
         *
         * @since S60 3.1
         * @return (TInt) the item count
         */
        TInt VideoItemCount();

    public: // Methods for creating the playlists

        /**
         * Creates a MPX playlist (if there are music items added).
         *
         * @since S60 3.1
         */
        void CreateMusicPlaylistL();

        /**
         * Creates an image album (if there are image items added).
         *
         * @since S60 3.1
         */
        void CreateImageAlbumL();

        /**
         * Creates a video album (if there are video items added).
         *
         * @since S60 3.1
         */
        void CreateVideoAlbumL();

    public: // Methods for notifying MPX

        /**
         * Notifies MPX of a new audio file.
         *
         * @since S60 3.1
         * @param aTrackPath (const TDesC&) filepath of the audio file
         * @param aMetadata (CUpnpItemt&) metadata as it was in media server
         */
        void NotifyNewAudioFileL( const TDesC& aFilePath,
                                  const CUpnpItem& aMetadata );

    private: // private business logic methods

        /**
         * Checks that Playlist location is valid
         *
         * @since S60 3.1
         * @return TBool
         */
        TBool PlaylistFileLocation();

    private: // 2nd phase constructors

        // Constructor
        CUpnpPlaylistHandler();

        /**
         * ConstructL.
         */
        void ConstructL();

    private: // Data members

        HBufC* iPlaylistName;                           // owned
        CDesCArrayFlat* iFilesForAudioPlaylist;         // owned
        RPointerArray<HBufC> iFilesForVideoAlbum;       // owned
        RPointerArray<HBufC> iFilesForImageAlbum;       // owned

        // music playlist methods encapsulation
        CUPnPPlaylistServices* iPlaylistServices;       // owned
    };

#endif // UPNP_FILE_TRANSFER_ENGINE_PLAYLIST_HANDLER_H

// End of file
