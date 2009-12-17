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


#ifndef C_UPNPPLAYLISTFILLER_H
#define C_UPNPPLAYLISTFILLER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class CUpnpObject;
class CUPnPMusicAdapter;
class CMPXCollectionPlaylist;
class CMPXMediaArray;

// CONSTANTS
const TInt KMaxUriSize = 520; // see upnpmusicadapter design

/**
 *  Defines the playlistfiller interface
 *
 *  @lib upnpmusicadapter.lib
 *  @since S60 v3.2
 */
class MUPnPPlaylistFiller
    {

public:

    /**
     * Fill track data into the playlist.
     * Calling method will fulfill the purpose of this object, and
     * therefore this object will be deleted.
     *
     * @param aHost the host adapter in which context the filler operates
     * @param aPlaylist playlist to be filled
     * @since Series 60 3.1
     */
    virtual void FillL(
        CUPnPMusicAdapter& aHost,
        CMPXMediaArray& aPlaylist ) = 0;

    /**
     * Cancels an ongoing Fill operation.
     */
    virtual void CancelFill() = 0;

    /**
     * Size for which the playlist is to be created.
     * Default implementation returns number of items added in the cache.
     */
    virtual TInt PlaylistSize() = 0;
    
    /**
     * Index of selected item (where playback should start)
     *
     * @return the selected index
     */
    virtual TInt SelectedIndex() const = 0;
    
    public: // Destructor    

    virtual ~MUPnPPlaylistFiller() {};
    };

/**
 * A base class providing services for filling MPX playlist
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPPlayListFiller : public CBase,
                            public MUPnPPlaylistFiller
                            
    {

public:

    /**
     * 1st phase constructor
     *
     * @since Series 60 3.1
     * @return a new CUPnPPlayListFiller instance
     */
    IMPORT_C static CUPnPPlayListFiller* NewL();

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    IMPORT_C virtual ~CUPnPPlayListFiller();

    /**
     * Inserts an initial object to be filled in the playlist
     * should be called after construction, but before use.
     * Note that the class refers to the object, so it should be
     * persistent in memory when Fill() is called.
     *
     * @param aMediaServer External Media Server device
     * @param aObject reference to the object.
     * @since Series 60 3.1
     */
    IMPORT_C void InsertObjectL(
        const CUpnpAVDevice& aMediaServer,
        const CUpnpObject& aObject );

    /**
     * Sets the index (starting from zero = the first inserted item)
     * of selected item. Playback will begin from the selected item.
     * If no item is selected, the first item is selected by default.
     * Note that this method must be called AFTER inserting the objects.
     *
     * @param aSelectedIndex index of the item selected
     * @since Series 60 3.1
     */
    IMPORT_C void SetSelectedIndex( TInt aSelectedIndex );

    /**
     * Fill track data into the playlist.
     * Calling method will fulfill the purpose of this object, and
     * therefore this object will be deleted.
     *
     * @param aHost the host adapter in which context the filler operates
     * @param aPlaylist playlist to be filled
     * @since Series 60 3.1
     */
    void FillL( CUPnPMusicAdapter& aHost, CMPXMediaArray& aPlaylist );

    /**
     * Cancels an ongoing Fill operation.
     */
    void CancelFill();

    /**
     * Size for which the playlist is to be created.
     * Default implementation returns number of items added in the cache.
     */
    TInt PlaylistSize();

    /**
     * Index of selected item (where playback should start)
     *
     * @return the selected index
     */
    TInt SelectedIndex() const;

    /**
     * Returns a non-modifiable reference to the object array
     *
     * @since Series 60 3.1
     * @return an object array
     */
    const RPointerArray<CUpnpObject>& ObjectArray();

    /**
     * The source device for the playlist
     *
     * @since Series 60 3.1
     * @return device for the playlist
     */
    const CUpnpAVDevice& SourceDevice();

protected:
    
    /**
     * default constructor
     */
    CUPnPPlayListFiller();

    /**
     * Fills the initial items into the playlist
     *
     * @param aPlaylist the MPX playlist
     * @param aSource the source list of objects
     * @param aStartIndex playlist index of items being added
     * @return number of items filled
     */
    TInt DoFillL( CMPXMediaArray& aPlaylist,
        const RPointerArray<CUpnpObject>& aSource,
        TInt aStartIndex = 0 );

    /**
     * Fills a single object with given index into a playlist.
     * if item is not an audio item, does not fill. Checks current
     * selected index and adjusts it if necessary.
     *
     * @param aPlaylist the MPX playlist
     * @param aObject the object to fill
     * @param aIndex playlist index of the item being added
     * @return void
     */
    void DoFillObjectL( CMPXMediaArray& aPlaylist,
        const CUpnpObject& aObject, TInt aIndex );
    
    /**
     * Convert UPnP item to playlist item URI string
     * URI string format is upnp:<device_id>*<object_id>
     * 
     * @param aDeviceUid the UID of the device, in string form
     * @param aItem browsed UPnP item to be converted
     * @param aBuffer the buffer for result URI
     * @return playlist item descriptor
     */
    void ConvertToURI( const TDesC& aDeviceUid,
        const CUpnpObject& aItem, TDes& aBuffer );

    /**
     * Completes the filler, resulting it to be deleted
     */    
    void DoComplete();

    /**
     * Sets the host for this filler
     *
     * @param aHost pointer to the music adapter in which context the
     * filler is running
     */
    void SetHost( CUPnPMusicAdapter& aHost );

    /**
     * The music adapter that hosts this filler
     *
     * @return the host music adapter
     */
    CUPnPMusicAdapter& Host() const;
    	
private:

    /**
     * The host music adapter in which context the
     * filler is running (not owned)
     */
    CUPnPMusicAdapter*                    iHost;

    /**
     * The initial set of items that we are about to
     * set into the playlist
     * Note: NO OWNERSHIP ON THE CONTENT
     */
    RPointerArray<CUpnpObject>            iItemList;

    /**
     * index of selected item
     */
    TInt                                  iSelectedIndex;

    /**
     * The media server we are playing on
     */
    const CUpnpAVDevice*                  iSourceDevice;

    /**
     * Media server device id
     * Own
     */
    HBufC*                                iDeviceUid;

    /**
     * Temporary buffer for 8->16-bit conversion
     */
    TBuf<KMaxUriSize>                     iTempBuf;
    

    };

#endif // C_UPNPPLAYLISTFILLER_H

