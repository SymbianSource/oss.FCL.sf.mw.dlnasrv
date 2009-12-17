/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Security manager server / common definitions for cliet and server
 *
 */

#ifndef UPNPSECURITYMANAGERCLIENTSERVERCOMMON_H
#define UPNPSECURITYMANAGERCLIENTSERVERCOMMON_H

// CONSTANTS
// the name of the server, used by client
_LIT( KUpnpSecurityManagerFilename, "upnpsecuritymanagerserver");

//server's name
_LIT( KUpnpSecurityManagerName, "upnpsecuritymanager");
_LIT( KUpnpSecurityManagerThreadName, "upnpsecuritymanager_MainThread");

const TUid KServerUid3 =
    {
    0x2001B2BE
    };

// the server version
// A version must be specified when creating a session with the server
const TUint KUpnpSecurityManagerMajorVersionNumber = 0;
const TUint KUpnpSecurityManagerMinorVersionNumber = 1;
const TUint KUpnpSecurityManagerBuildVersionNumber = 1;

// ENUMS
// opcodes used in message passing between client and server
enum TUpnpSecManagerRqst
    {
    EAddFile = 1,
    ERemoveFile,
    EAddAddress,
    ERemoveAddress,
    EResetFileList,
    EResetAddressList,
    ECheckAuthorization,
    EQueryAuthorisation
    };

#endif  // UPNPSECURITYMANAGERCLIENTSERVERCOMMON_H
// End of File
