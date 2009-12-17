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
* Description:      UPnP Object-related constant definitions
*
*/






// System includes
#include <e32base.h>

// ***************************************************
// * Upnp interface is full of constants that come
// * directly from upnp specifications. This file
// * contains some most commonly used definitions
// * for your comfort.
// ***************************************************



// ***************************************************
// * Classes
// * 
// * Note: when comparing object class, the class
// * constant MUST be found in beginning:
// * 
// * if ( object.ObjectClass().Find( KClassContainer ) == 0 )
// *     { ...
// * 
// * 
// ***************************************************

// item. This means the object represents something
// rather than containing child objects
_LIT8( KClassItem,        "object.item" );

// the item represents an image media
_LIT8( KClassImage,       "object.item.imageItem" );

// the item represents a video media
_LIT8( KClassVideo,       "object.item.videoItem" );

// the item represents an audio media
_LIT8( KClassAudio,       "object.item.audioItem" );

// the item represents a music track
_LIT8( KClassAudioMusicTrack, "object.item.audioItem.musicTrack" );

// the object is a container that can contain child objects
_LIT8( KClassContainer,   "object.container" );

// container that represents a playlist
_LIT8( KClassPlaylist,    "object.container.playlistContainer" );

// container that represents an album of music tracks
_LIT8( KClassAlbum,       "object.container.album.musicAlbum" );

// container that represents a generic media storage
_LIT8( KClassStorage,    "object.container.storageFolder" );



// ***************************************************
// * Elements
// * 
// * Use these when finding an apropriate element
// * within an object. Useful with 2 helper utility
// * functions:
// * 
// * const CUpnpElement* e = UPnPCommonUtils::
// *     FindElementByName( object, KElementDate );
// * if ( e ) {
// *     const TDesC& d = e->Value();
// *     ...
// * 
// * const TDesC8& d = UPnPCommonUtils::
// *     FindElementByNameL( object, KElementDate ).Value();
// * 
// * The former is useful if we are not sure if the
// * element exists, the latter if the element MUST
// * exist to proceed.
// * 
// ***************************************************

// The title. Mandatory for all objects.
_LIT8( KElementTitle,           "dc:title" );

// The class. Mandatory for all objects.
_LIT8( KElementClass,           "upnp:class" );

// media timestamp. In DLNA mandatory for images and videos
_LIT8( KElementDate,            "dc:date" );

// Creator of the media. In DLNA mandatory for music tracks and albums
_LIT8( KElementCreator,         "dc:creator" );

// The artist. De facto standard in upnp, but not mandatory.
// could be understood as an alias to dc:creator
_LIT8( KElementArtist,          "upnp:artist");

// Album this media belongs to. in DLNA mandatory for audio tracks
_LIT8( KElementAlbum,           "upnp:album");

// Genre. in DLNA mandatory for audio and video
_LIT8( KElementGenre,           "upnp:genre");

// The res element. Defines the resource of the item.
_LIT8( KElementRes,             "res");

// The SortCriteria, Search by default 
_LIT8( KSortCriteria,           "+dc:title");

// The albumArtURI
_LIT8( KElementAlbumArtUri,     "upnp:albumArtURI");

// Song track number
_LIT8( KElementTrackNumber,     "upnp:originalTrackNumber");


// ***************************************************
// * Element attributes
// * 
// * Use these when finding an attribute within an
// * element.
// * 
// * const CUpnpElement& resource = UPnPCommonUtils
// *     ::ResourceFromItemL( object );
// * const TDesC8& size = UPnPCommonUtils
// *     ::FindAttributeByNameL( resource, KAttributeSize )
// *     .Value();
// * const TDesC8& duration = UPnPCommonUtils
// *     ::FindAttributeByNameL( resource, KAttributeDuration )
// *     .Value();
// * 
// ***************************************************

// attribute of RES element
// media size in bytes. Used for all media.
_LIT8( KAttributeSize,         "size" );

// attribute of RES element
// image resolution: "<XDIM>x<YDIM>", example "1024x768"
// used for images and video.
_LIT8( KAttributeResolution,   "resolution" );

// attribute of RES element
// song duration, "hh:mm:ss.yyy", example "00:03:15.000"
// used for audio and video
_LIT8( KAttributeDuration,     "duration" );

// attribute of RES element
// UPNP and DLNA specific technical data
_LIT8( KAttributeProtocolInfo, "protocolInfo" );

// attribute of RES element
// bitrate. used in
_LIT8( KAttributeBitrate,      "bitrate" );



// ***************************************************
// * Filters and sorting
// * 
// * Pre-defined constants for browse or search.
// * The filters determine which elements the remote
// * server is adding into the response. Querying
// * only the required elements will speed up the
// * application.
// *
// * Sort criteria defines the order of returned
// * elements.
// * 
// * iBs->BrowseL( _L("0"), KFilterFull,
// *     MUPnPAVBrowsingSession::EDirectChildren,
// *     0, 100, KSortNone );
// * 
// ***************************************************

// full filter, returns everything
_LIT8( KFilterFull, "*" );

// filter only minimum data (object id)
_LIT8( KFilterMin, "" );

// filter only object key data and title
_LIT8( KFilterTitle, "dc:title" );

// Filter most commonly used and mandatory DLNA field
_LIT8( KFilterCommon, "dc:title,dc:date,dc:creator,"
    "upnp:originalTrackNumber,"
    "upnp:album,upnp:artist,upnp:genre,upnp:albumArtURI,"
    "res,res@size,res@duration" );

// sort criteria: let server decide
_LIT8( KSortNone, "" );

// sort criteria: sort by object title
_LIT8( KSortTitle, "dc:title" );




// ***************************************************
// * Constant container ID's
// * 
// * Some container ID's are defined as constants
// * and have special meaning.
// *
// * Root container: the container that recursively
// * contains all other containers. This container
// * is not contained in any other container.
// * 
// * Any container: used when addressing a remote
// * container. Lets the remote mediaserver decide
// * an apropriate container.
// * 
// ***************************************************

// the root container
_LIT8( KContainerIdRoot, "0" );

// Anycontainer
_LIT8( KContainerIdAny,  "DLNA.ORG_AnyContainer" );



// ***************************************************
// * Constructing search criteria
// * 
// * Some useful constants that are used in generating
// * search criteria fields
// * 
// * Please see UPnP specifications for more details
// * 
// ***************************************************

// searches for elements that contain given partial string
_LIT8( KCriteriaContains, "contains" );

// searches for classes or any child classes of given class
_LIT8( KCriteriaDerivedFrom, "derivedfrom" );

// searches for classes where an element exists
_LIT8( KCriteriaExists, "exists" );

// negates the boolean operator
_LIT8( KCriteriaFalse, "false" );

// logical AND
_LIT8( KCriteriaAnd, "and" );

// logical OR
_LIT8( KCriteriaOr, "or" );

// logical comparision
_LIT8( KCriteriaEQ, "=" );
_LIT8( KCriteriaNEQ, "!=" );
_LIT8( KCriteriaGT, ">" );
_LIT8( KCriteriaLT, "<" );
_LIT8( KCriteriaGTE, ">=" );
_LIT8( KCriteriaLTE, "<=" );
_LIT8( KCriteriaOB, "(" );
_LIT8( KCriteriaCB, ")" );

// other useful for filling up the strings
_LIT8( KCriteriaQuot, "\"" );
_LIT8( KCriteriaSpace, " " );



// ***************************************************
// * DLNA Transfer modes
// * 
// * Please see DLNA specifications for more details
// ***************************************************
_LIT8( KTransferMode, "transferMode.dlna.org" );
_LIT8( KBackgroundMode, "Background" );
_LIT8( KStreamingMode, "Streaming" );
_LIT8( KInteractiveMode, "Interactive" );


// ***************************************************
// * DLNA Content headers
// * 
// * Please see DLNA specifications for more details
// ***************************************************
_LIT8( KContentType, "Content-Type" );
_LIT8( KContentFeatures, "contentFeatures.dlna.org" );


// ***************************************************
// * DLNA headers misc.
// * 
// * Please see DLNA specifications for more details
// ***************************************************
_LIT8( KExpectHeader, "Expect" );
_LIT8( K100Continue, "100-continue" );


// ***************************************************
// * Misc
// * 
// ***************************************************

// attribute value - not implemented or not supported
_LIT8( KValueNotImplemented, "NOT_IMPLEMENTED" );

