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
* Description:  Provides services for filling MPX playlist
*
*/


#ifndef UPNPLITEFILLER_H
#define UPNPLITEFILLER_H

//  INCLUDES
#include <e32base.h>
#include "upnpplaylistfiller.h"

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class CUPnPMusicAdapter;
class CMPXMediaArray;
class CUpnpObjectLite;

// CLASS DECLARATION

/**
 * A base class providing services for filling MPX playlist
 *
 * @lib upnpmusicadapter.lib
 * @since S60 3.2
 */
class CUPnPLiteFiller : public CBase,
                        public MUPnPPlaylistFiller
    {

public: // static factory construction

    /**
     * 1st phase constructor
     *
     * @since Series 60 3.2
     * @return a new CUPnPLiteFiller instance
     */
     static CUPnPLiteFiller* NewL();
        
    /**
     * Sets the index (starting from zero = the first inserted item)
     * of selected item. Playback will begin from the selected item.
     * If no item is selected, the first item is selected by default.
     * Note that this method must be called AFTER inserting the objects.
     *
     * @param aSelectedIndex index of the item selected
     * @since Series 60 3.2
     */
    IMPORT_C void SetSelectedIndex( TInt aSelectedIndex );
    
    /**
     * Set Source media server. Device id is needed for creating uri
     * @param aDevice source media server
     * @since Series 60 3.2
     */
    void SetSourceDeviceL( const CUpnpAVDevice& aDevice );
    
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

    /**
     * Completes the filler, resulting it to be deleted
     */    
    void DoComplete();

    /**
     * See MUPnPPlaylistFiller
     */
    void FillL( CUPnPMusicAdapter& aHost, CMPXMediaArray& aPlaylist );

    /**
     * See MUPnPPlaylistFiller
     */
    void CancelFill();

    /**
     * See MUPnPPlaylistFiller
     */
    TInt PlaylistSize();
    
    /**
     * Index of selected item (where playback should start)
     *
     * @return the selected index
     */
    TInt SelectedIndex() const;

public: // construction/destruction

    /**
     * Destructor
     * @since Series 60 3.2
     */
    IMPORT_C virtual ~CUPnPLiteFiller();

public:
    /**
     * Fills a single item with given index into a mpx playlist.
     * if item is not an audio item, does not fill. Checks current
     * selected index and adjusts it if necessary.
     *
     * @param aPlaylist the MPX playlist
     * @param aItem the object to fill
     * @param aIndex playlist index of the item being added
     * @return void
     */
    void FillItemMpxPlaylistL( CMPXMediaArray& aPlaylist,
        const CUpnpObjectLite& aItem, TInt aIndex );

protected:

    /**
     * Default constructor
     *
     * @since Series 60 3.2
     */
    CUPnPLiteFiller();

private:
    /**
     * 2nd phase constructor
     *
     * @param aMediaServer External Media Server device
     * @param aObjectList Object to be filled
     * @since Series 60 3.2
     */
    void ConstructL();
    
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
        const CUpnpObjectLite& aItem, TDes& aBuffer );

private: // data

    /**
     * The host music adapter in which context the
     * filler is running (not owned)
     */
    CUPnPMusicAdapter*                    iHost;

    /**
     * The media server we are playing on (not owned)
     */
    const CUpnpAVDevice*                  iSourceDevice;

    /**
     * Media server device id
     * (Owned)
     */
    HBufC*                                iDeviceUid;

    /**
     * index of selected item
     */
    TInt                                  iSelectedIndex;

    /**
     * Temporary buffer for 8->16-bit conversion
     */
    TBuf<KMaxUriSize>                     iTempBuf;

    };

#endif  // UPNPLITEFILLER_H

// End of File


