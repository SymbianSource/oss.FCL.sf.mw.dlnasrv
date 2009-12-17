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
* Description:  Fills MPX playlist by browsing media server content
*
*/


#ifndef C_UPNPBROWSEPLAYLISTFILLER_H
#define C_UPNPBROWSEPLAYLISTFILLER_H

// INCLUDES
// System
#include <e32base.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsessionobserver.h"

// musicadapter internal
#include "upnpplaylistfiller.h"
#include "upnpbrowsetimer.h"
#include "upnpbrowseplaylistfiller.h"

// FORWARD DECLARATIONS
class MUPnPAVBrowsingSession;
class CUpnpObject;
class CMPXCollectionPlaylist;
class CMPXMediaArray;
class MMPXCollectionUtility;
class CUPnPBrowseTimer;

/**
 * This class continues browse made in browse dialog
 * and places the items in an MPX playlist.
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPBrowsePlaylistFiller : public CUPnPPlayListFiller,
                                  public MUPnPAVBrowsingSessionObserver,
                                  public MUPnPBrowseTimerObserver
    {

public:

    /**
     * 1st phase constructor
     *
     * @param aContainerId the container where to play
     * @param aCachedStartIndex container index of first pre-cached item
     * @param aTotalContainerSize total number of items in this container
     * @since Series 60 3.1
     * @return a new CUPnPBrowsePlaylistFiller instance
     */
    IMPORT_C static CUPnPBrowsePlaylistFiller* NewL(
        const TDesC8& aContainerId,
        TInt aCachedStartIndex,
        TInt aTotalContainerSize );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    IMPORT_C virtual ~CUPnPBrowsePlaylistFiller();

    /**
     * Fill track data into the playlist and continue browsing
     *
     * @param aHost the host adapter in which context the filler operates
     * @param aPlaylist playlist to be filled
     * @since Series 60 3.1
     */
    void FillL(
        CUPnPMusicAdapter& aHost,
        CMPXMediaArray& aPlaylist );

    /**
     * Cancels an ongoing Fill operation.
     */
    void CancelFill();

    /**
     * Size for which the playlist is to be created.
     * Default implementation returns number of items added in the cache.
     */
    TInt PlaylistSize();

protected: // From MUPnPAVBrowsingSessionObserver

    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void BrowseResponse(
                        const TDesC8& aBrowseResponse,
                        TInt aError,
                        TInt aMatches,
                        TInt aTotalCount,
                        const TDesC8& aUpdateId
                        );

    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void SearchResponse( 
                        const TDesC8& aSearchResponse,
                        TInt aError,
                        TInt aMatches,
                        TInt aTotalCount,
                        const TDesC8& aUpdateId
                        );

    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void SearchCapabilitiesResponse( TInt aError,
                                     const TDesC8& aSearchCapabilities 
                                     );

    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void CreateContainerResponse( TInt aError,
                                  const TDesC8& aObjectId );

    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void DeleteObjectResponse( TInt aError );
            
    /**
     * See MUPnPAVBrowsingSessionObserver
     */ 
    void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );
    
    /**
     * See MUPnPAVBrowsingSessionObserver
     */
    void ReserveLocalMSServicesCompleted( TInt aError );
    
protected: // From MUPnPBrowseTimerObserver

    /**
     * See MUPnPBrowseTimerObserver
     */
    void HandlePeriod();

private:

    /**
     * Default constructor
     *
     * @since Series 60 3.1
     */
    CUPnPBrowsePlaylistFiller();

    /**
     * 2nd phase constructor
     *
     * @param aContainerId the container where to browse
     * @param aCachedStartIndex container index of first pre-cached item
     * @param aTotalContainerSize total number of items in this container
     * @since Series 60 3.1
     */
    void ConstructL(
        const TDesC8& aContainerId,
        TInt aCachedStartIndex,
        TInt aTotalContainerSize );
    
    /**
     * Adds received media to collection
     *
     * @param reveived media
     * @since Series 60 3.1
     */
    void AddMediaToCollectionL( const TDesC8& aBrowseResponse );

private: // data

    /**
     * Browsing session
     */
    MUPnPAVBrowsingSession*             iBrowsingSession;

    /**
     * Container id where to browse
     */ 
    HBufC8*                             iContainerId;

    /**
     * Index of the first item in PlaylistFiller's pre-cached list
     */
    TInt                                iCachedStartIndex;

    /**
     * Index of next item to fetch from remote mediaserver
     */
    TInt                                iCurrentIndex;

    /**
     * Estimated playlist size in creation phase
     */
    TInt                                iPlaylistInitialSize;

    /**
     * Playlist size as it items are being dynamically fetched
     */
    TInt                                iPlaylistActualSize;

    /**
     * Timer support
     * Owned.
     */
    CUPnPBrowseTimer*                   iBrowseTimer;
    
    /**
     * Item count to be browsed
     */
    TInt                                iBlocksize;

    };


#endif // C_UPNPBROWSEPLAYLISTFILLER_H
