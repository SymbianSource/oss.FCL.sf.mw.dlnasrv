/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UpnpSharingAlgoritm component constants
*
*/

#ifndef UPNP_SHARING_ALGORITHM_CONSTANTS_H
#define UPNP_SHARING_ALGORITHM_CONSTANTS_H

// DEBUG file name
_LIT( KComponentLogfile, "upnpsharingalgorithm.txt");

// Resource file
_LIT( KSharingAlgorithmRscFile, "\\resource\\upnpsharingalgorithm.rsc" );

// CDS Browse filter criteria
_LIT8( KBrowseCriteriaFilter, "*" );

// CDS Container names
_LIT8( KRootContainer,       "0" );

// CDS container class types
_LIT8( KMusicClassContainer,   "object.container.music" );
_LIT8( KImageClassContainer,   "object.container.image" );
_LIT8( KVideoClassContainer,   "object.container.video" );
_LIT8( KContainerClass,        "object.container" );
_LIT8( KItemClass,             "object.item" );

// Metadata container types
enum TMetadataContainerTypes
    {
    EGenreType = 0,
    EAlbumType,
    EArtistType,
    EMusicByDateType,
    EImageByDateType,
    EVideoByDateType
    };

#endif // UPNP_SHARING_ALGORITHM_CONSTANTS_H

// End of file
