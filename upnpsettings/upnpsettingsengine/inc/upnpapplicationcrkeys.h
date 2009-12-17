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
* Description:      Key definitions used in central repository
*
*/






#ifndef UPNPAPPLICATIONCRKEYS_H
#define UPNPAPPLICATIONCRKEYS_H

#include <e32std.h>

/**
 * Software Installer settings
 *
 */
const TUid KCRUidUPnPApplication = {0x20009cae};

/**
 * KUPnPAppAccessPointSetting
 * Defines the used access point setting for UPnP.
 *
 */
const TUint32 KUPnPAppAccessPointSetting = 0x00000001;

/**
 * KUPnPAppWapId
 * Defines the iap wap id for Home Network application.
 *
 */
const TUint32 KUPnPAppWapId = 0x00000002;

/**
 * KUPnPAppFirstStart
 * Defines if this is the first start of the home network
 */
const TUint32 KUPnPAppFirstStart = 0x00000003;

/**
 * KUPnPAppMediaServerIcon
 * Defines if the icon is set to mediaserver
 */
const TUint32 KUPnPAppMediaServerIcon = 0x00000004;


#endif  // UPNPAPPLICATIONCRKEYS_H
// End of file
