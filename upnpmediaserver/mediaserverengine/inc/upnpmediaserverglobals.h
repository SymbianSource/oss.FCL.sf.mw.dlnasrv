/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Media server device
*
*/



#ifndef C_UPNPMEDIASERVERGLOBALS_H
#define C_UPNPMEDIASERVERGLOBALS_H

#include <e32base.h>
#include <platform_paths.hrh>
#include <bldvariant.hrh>

// CONSTANTS
const TUint KMediaServerMajorVersionNumber = 0;
const TUint KMediaServerMinorVersionNumber = 1;
const TUint KMediaServerBuildVersionNumber = 1;


_LIT( KIconDest,  "public\\MediaServer1\\icon\\" );
_LIT( KMediaServerFilename,  "AVMediaServer" );
const TUid KServerUid3 = {0x101F977C};
_LIT( KMediaServerThreadName,    "MediaServer_MainThread" );
_LIT( KMediaServerName,  "AVMediaServer" );

// ENUMERATIONS
// Opcodes used in message passing between client and server
enum TMediaServerRqst
    {
    EMediaServerStop = 100,
    EMediaServerStart,
    EMediaServerStopSilent,
    EMediaServerStartOffline,
    EMediaServerRestart,
    EMediaServerStatus,
    EMediaServerShareItem,
    EMediaServerShareItemResult,
    EMediaServerShareContainer,
    EMediaServerShareContainerResult,
    EMediaServerShareItemList,
    EMediaServerShareItemListResult,
    EMediaServerUnshareItem,
    EMediaServerUnshareContainer,
    EMediaServerUnshareItemList,
    EMediaServerUnshareRoot,
    EMediaServerGetSharedItem,
    EMediaServerGetSharedItemResult,
    EMediaServerGetSharedItemList,
    EMediaServerGetSharedItemListResult,
    EMediaServerGetSharedContainer,
    EMediaServerGetSharedContainerResult,
    EMediaServerGetSharedContainerList,
    EMediaServerGetSharedContainerListResult,
    EMediaServerShareReference,    
    EMediaServerShareReferenceList,
    EMediaServerSubscribeEvents,
    EMediaServerUnsubscribeEvents,
    EMediaServerListenTransferEvent,
    EMediaServerGetTransferEventBody,
    EMediaServerGetIpAndPort,
    EMediaServerSetUploadDirectory,
    EMediaServerAddIcon,
    EMediaServerRemoveIcon,
    EMediaServerRemoveIcons,
    EMediaServerUpdateIcon,
    EMediaServerStatusChange,
    EMediaServerStatusChangeCancel
    };
    
#endif      // C_UPNPMEDIASERVERGLOBALS_H
            
// End of File
