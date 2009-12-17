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
* Description:      Internal definitions of upnp content server and client
 *
*/





#ifndef __UPNPCONTENTSERVERDEFS_H__
#define __UPNPCONTENTSERVERDEFS_H__
#include <e32debug.h>

namespace UpnpContentServer
    {
// CONSTANTS
    _LIT( KUpnpContentServer, "upnpcontentserver" );

    _LIT( KUpnpContentServerNameAndDir, "\\sys\\bin\\upnpcontentserver.exe" );

    _LIT8( KImageItemObjectClass, "object.item.imageItem.photo" );
    _LIT8( KMusicItemObjectClass, "object.item.audioItem.musicTrack" );
    _LIT8( KVideoItemObjectClass, "object.item.videoItem.movie" );

// container classes
    _LIT8( KImageVideoContainerClass, "object.container.storageFolder" );
    _LIT8( KPlaylistContainerClass, "object.container.playlistContainer" );
    _LIT8( KStorageFolderContainerClass, "object.container.storageFolder" );

// upload directories
    _LIT( KUploadDirectory, "Download\\Media\\" );

// default container names
    _LIT8( KUploadedContainerName, "Uploaded" );
    _LIT8(KImageContainerName, "Images&Videos");
    _LIT8(KMusicContainerName, "Music");

    const TInt KRootContainerId = 0;
    _LIT8( KRootContainerIdDesc8, "0" );
    const TInt KUpnpContentServerVersionMajor = 1;
    const TInt KUpnpContentServerVersionMinor = 0;
    const TInt KDefaultMessageSlots = 4;
    const TInt KTintSize = sizeof( TInt );
// There is two paths to store uploads, length of both is <25
    const TInt KMaxUploadPathLength = 25;

// File names for the selected albums
    _LIT( KMusicFile, "music.txt" );
    _LIT( KVisualFile, "visual.txt" );

/**
 * Enumeration for client server communication
 */
    enum TUpnpContentServerCmds
        {
        EStartUploadListener,
        EStopUploadListener,
        ERequestSelectionContent,
        EGetSelectionIndexes,
        EChangeShareContent,
        ERefreshShareContent,
        ERequestProgressInfo,
        ERequestObjectCount,
        EStartConnectionMonitor,
        EStopConnectionMonitor
        };

/**
 * Type for Upnp media
 */
    enum TUpnpMediaType
        {
        EImageAndVideo = 0,
        EPlaylist,
        EPhotoFile,
        EVideoFile,
        EMusicFile
        };

/**
 * Type for sharing selection
 */
    enum TUpnpSharingSelection
        {
        ENoChange = -99,
        EShareNone = 0,
        EShareMany,
        EShareAll
        };

    } // namespace UpnpContentServer


// Temporary solution until clf..extended.hrh is back begin
//
/**
 *  Content Listing Framework list model media types.
 *  Last default media type is 0x7FFFFFFF,
 *  Custom media type range 0x80000000 0xFFFFFFFF
 */
enum TCLFMediaTypeExtended
    {
    /// Gallery collections
    ECLFMediaTypeCollection     = 0x7
    };

/**
 *  S60 internal Content Listing Framework item field IDs.
 *  Last default field ID is 0x7FFFFFFF,
 *  Custom field ID range 0x80000000 - 0xFFFFFFFF
 */
enum TCLFExtendedFieldId
    {
    /// Media gallery collection name:
    /// descriptor
    ECLFFieldIdCollectionName       = 0x00002000,
    /// Media gallery collection ID:
    /// integer
    ECLFFieldIdCollectionId         = 0x00002001,
    /// Media gallery collection type:
    /// integer
    ECLFFieldIdCollectionType       = 0x00002002,
    /// Media gallery collection memory:
    /// integer
    ECLFFieldIdCollectionMemory     = 0x00002003,
    /// Media gallery collection properties:
    /// integer
    ECLFFieldIdCollectionProperties = 0x00002004,
    /// Media gallery collection files:
    /// descriptor
    ECLFFieldIdCollectionFiles      = 0x00002005,
    /// Media gallery item count of the collection:
    /// integer
    ECLFFieldIdCollectionItemCount  = 0x00002006
    };

//temp solution ends


// for Debugging
/*
#define __*LOGB __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
#define __*LOGE __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
#define __*LOGEr __LOG3( "%s: %s:%d", "Error", __FILE__, __LINE__ );
#define __*LOGEc(a) if (a) __LOG2( "%s: Error: %d", __PRETTY_FUNCTION__, a );
*/



#endif // __UPNPCONTENTSERVERDEFS_H__
