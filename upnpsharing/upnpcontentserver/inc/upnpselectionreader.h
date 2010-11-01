/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUpnpSelectionReader class definition
 *
*/






#ifndef UPNPSELECTIONREADER_H
#define UPNPSELECTIONREADER_H

// INCLUDES
#include <badesca.h> 
#include "upnpcontentserverdefs.h"

using namespace UpnpContentServer;

// CLASS DECLARATION

/**
 * CUpnpSelectionReader class
 * Reads the selections user has made
 * @since S60 3.1
 */
class CUpnpSelectionReader: public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @since S60 3.1
     */
    IMPORT_C static CUpnpSelectionReader* NewL();

    /**
     * Destructor.
     */
    virtual ~CUpnpSelectionReader();

    /**
     * Fetch collections from Media Gallery
     * @param aSettingsTextArray, The array of collection names
     * @since S60 3.1
     */
    IMPORT_C void FetchCollectionsL( CDesCArray* aSettingsTextArray );

    /**
     * Fetch playlists from Media Gallery
     * @param aSettingsTextArray, The array of playlist names
     * @since S60 3.1
     */
    IMPORT_C void FetchPlaylistsL( CDesCArray* aSettingsTextArray );

    /**
     * Checks if item is alredy shared
     * @param aItemName, Name of the item
     * @param aIdArray, Array of Ids to search from
     * @return TBool, True is shared, false otherwise
     * @since S60 3.1
     */
    IMPORT_C TBool IsItemShared(const TDesC& aItemName,
                                const CDesCArray& aIDArray ) const;

    /**
     * Gets the indexes of users previous selections relative to the available
     * playlists or albums
     * @since S60 3.1
     * @param aSelections, Array of selections
     * @param aType, Selections type needed
     */
    IMPORT_C void GetSelectionIndexesL(
        RArray<TInt>& aSelections,
        const TUpnpMediaType& aType );

    /**
     * Gets the sharing state of Images and videos
     * @since S60 3.1
     * @param aShareAllState, The variable to store the state
     * @return Status of the operation
     */
    TInt GetVisualSharingStateL( TInt& aShareAllState );

    /**
     * Gets the sharing state of music
     * @since S60 3.1
     * @param aShareAllState, The variable to store the state
     * @return Status of the operation
     */
    TInt GetMusicSharingStateL( TInt& aShareAllState );

    /**
     * Get the number of shared containers
     * @since S60 3.1
     * @return number of shared containers
     */
    TInt GetContainerCount() const;

    /**
     * Get the playlist ids
     * @since S60 3.1
     * @param aPlaylistIds Array containing the playlist ids
     * @return error code
     */
    TInt PlayListIdsL( CDesCArray& aPlaylistIds,
        CDesCArray& aPlaylistNames );

    /**
     * Get the collection ids
     * @since S60 3.1
     * @param aCollectionIds Array containing the collection ids
     * @return error code
     */
    TInt CollectionIdsL( CDesCArray& aCollectionIds, 
        CDesCArray& aCollectionNames );

    /**
     * Get the collection content. Filenames includes also path.
     * @since S60 5.2
     * @param aSelectionIndex Selected index
     * @param aFilenames Array containing filenames of the collection
     * @return Collection id (if not found, empty string is returned)
     */
    TPtrC GetCollectionItemsL( const TInt aSelectionIndex, 
        CDesCArray& aFilenames );
    /**
     * Get the playlist content. Filenames includes also path.
     * @since S60 5.2
     * @param aSelectionIndex Selected index
     * @param aFilenames Array containing filenames of the playlist
     * @return Playlist id (if not found, empty string is returned)
     */
    TPtrC GetPlaylistItemsL( const TInt aSelectionIndex, 
        CDesCArray& aFilenames );

    /**
     * Get the playlist content. Filenames includes also path.
     * @since S60 5.2
     * @param aType Mediatype (EImageAndVideo or EPlaylist)
     * @param aClfId Album/Playlist id in the clf structure
     * @return Selection index (if not found, KErrNotFound is returned)
     */
    TInt GetSelectionIndexL( const TUpnpMediaType& aType,
            TPtrC aClfId );

protected:  // New functions
    /**
     * Two-phased constructor.
     * @since S60 3.1
     */
    void ConstructL();

    /**
     * Searches playlist files
     * @since S60 3.1
     */
    TBool SearchPlaylistFilesL();

    /**
     * Searches collections
     * @since S60 3.1
     */
    TBool SearchCollectionsL();

    /**
     * Saves sharing state to central repository
     * @since S60 3.1
     * @return TInt, error code
     */
    TInt SaveSharingState();


private:
    /**
     * C++ default constructor.
     */
    CUpnpSelectionReader();

    /**
     * Reads container IDs from file
     * @since S60 3.1
     * @param aArray, array where desciptors are read
     * @param aContainerType, type of container
     * @return TInt, error code
     */
    TInt ReadSharedContainerIDsL(
        CDesCArray* aArray,
        const TUpnpMediaType& aContainerType ) const;


private: //data

    /**
     *  Mediatype selected from listbox
     */
    TUpnpMediaType iMediaType;

    /**
     * Selection value for images and videos
     */
    TInt iVisualSharingSelection;

    /**
     * Selection value for playlists
     */
    TInt iMusicSharingSelection;

    /** 
     * selected image and video indexes
     * owned
     */
    RArray<TInt>* iSelectedImages;

    /**
     * selected music indexes
     * owned
     */
    RArray<TInt>* iSelectedMusic;

    /** 
     * array for playlists
     * owned
     */
    CDesCArrayFlat* iPlIdArray;

    /** 
     * array for collection Ids
     * owned
     */
    CDesCArrayFlat* iCollIdArray;

    /**
     * array for image and video containers shown in ui
     * owned
     */
    CDesCArrayFlat* iImageContainers;

    /**
     * array for playlists shown in ui
     * owned
     */
    CDesCArrayFlat* iPlaylistNames;

    /**
     * array for collections shown in ui
     * owned
     */
    CDesCArrayFlat* iCollectionNames;

    /**
     * number of ui items
     */
    TInt iContainerCount;

    };

#endif // UPNPSELECTIONREADER_H

// End of File
