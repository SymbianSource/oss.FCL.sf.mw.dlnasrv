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
* Description:      Common header for Common component
*
*/






#ifndef CMCOMMONTYPES_H
#define CMCOMMONTYPES_H



/// Fill rule method types

enum TFillRuleMethodTypes
    {
    ECMRandom = 0,
    ECMLatest,
    ECMOldest
    };

enum TCmLimitType
    {
    EUnlimited = 0,
    EPieces,
    EMbits
    };

enum TCmMediaType
    {
    ECmAll   = -1,
    ECmVideo = 0,
    ECmImage,
    ECmAudio,
    ECmAudioBroadCast,
    ECmVideoBroadCast,
    ECmOtherVideo,
    ECmOtherImage
    };
    
enum TCmFillMethod
    {
    ECmRandom = 0,
    ECmLatest,
    ECmOldest,
    ECmRandomTrack,
    ECmRandomAlbum
    };

enum TCmFillRuleStatus
    {
    ECmUnSelected = 0,
    ECmSelected,
    ECmDeleted 
    };
    
enum TCmMetadataField
    {
    ECmArtist = 0,
    ECmAlbum,
    ECmGenre,
    ECmDuration,
    ECmDate,
    ECmTitle,
    ECmUpnpClass,
    ECmContentURI,
    ECmFileSize,
    ECmBitrate,
    ECmResolution,
    ECmFreeText,
    ECmProfileId,
    ECmNone
    };

enum TCmOperatorType
    {
    ECmEquals,
    ECmGreater,
    ECmSmaller,
    ECmConsistOf,
    ECmBeginsWith,
    ECmEndsWith
    };                           

enum TCmRuleType
    {
    ECmFmFillRule = 1,
    ECmFmStoreRule
    };        
#endif // CMCOMMONTYPES_H

// end of file
