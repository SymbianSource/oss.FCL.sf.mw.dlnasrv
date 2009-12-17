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
* Description:      global definitions for AVController
*
*/







#ifndef C_UPNPAVCONTROLLERGLOBALS_H
#define C_UPNPAVCONTROLLERGLOBALS_H

#include <e32std.h>


// CONSTANTS
const TUint KAVControllerMajorVersionNumber = 0;
const TUint KAVControllerMinorVersionNumber = 1;
const TUint KAVControllerBuildVersionNumber = 1;

_LIT( KAVControllerThreadName,      "AVController_MainThread" );
_LIT( KAVControllerName,            "AVController" );
_LIT( KAVControllerFilename,        "upnpavcontrollerserver" );
_LIT8( KAVControllerDefaultName,    "Nokia AV Controller" );

//_LIT8( KDownloadMemoryCard,     "e:\\Download\\Media\\" );
//_LIT8( KDownloadPhoneMemory,    "c:\\Data\\Download\\Media\\" );
//_LIT8( KDownloadRAMDrive,       "d:\\Download\\Media\\" );

const TUint KMsFriendlyNameMaxSize = 256;
const TUid KAVCServerUid3 = {0x20007566};

// ENUMERATIONS
// Opcodes used in message passing between client and server
enum TAVControllerRqst
    {
    EAVControllerSetURI = 0,                    // 0
    EAVControllerCancelSetURI,                  // 1
    EAVControllerSetNextURI,                    // 2
    EAVControllerCancelSetNextURI,              // 3
    EAVControllerCancelCopyRemoteItemFinish,    // 4
    EAVControllerCopyLocalItemStart,            // 5
    EAVControllerCancelCopyLocalItemStart,      // 6
    EAVControllerCopyLocalItemFinish,           // 7
    EAVControllerCancelCopyLocalItemFinish,     // 8
    EAVControllerCopyToPhoneStart,              // 9
    EAVControllerCancelCopyToPhoneStart,        // 10
    EAVControllerCopyToPhoneFinish,             // 11
    EAVControllerCancelCopyToPhoneFinish,       // 12
    EAVControllerGetBrowseResponseSize,         // 13
    EAVControllerCancelGetBrowseResponseSize,   // 14
    EAVControllerGetBrowseResponse,             // 15
    EAVControllerGetSearchResponseSize,         // 16
    EAVControllerCancelGetSearchResponseSize,   // 17
    EAVControllerGetSearchResponse,             // 18
    EAVControllerDeviceRequest,                 // 19
    EAVControllerGetDeviceRequest,              // 20
    EAVControllerCancelDeviceRequest,           // 21
    EAVControllerGetDeviceListRequest,          // 22
    EAVControllerGetDeviceListSizeRequest,      // 23
    EAVControllerCreateRenderingSession,        // 24
    EAVControllerDestroyRenderingSession,       // 25
    EAVControllerEventRequest,                  // 26
    EAVControllerCancelEventRequest,            // 27
    EAVControllerPlay,                          // 28
    EAVControllerCancelPlay,                    // 29
    EAVControllerStop,                          // 30
    EAVControllerCancelStop,                    // 31
    EAVControllerPause,                         // 32
    EAVControllerCancelPause,                   // 33
    EAVControllerSetVolume,                     // 34
    EAVControllerCancelSetVolume,               // 35
    EAVControllerGetVolume,                     // 36
    EAVControllerCancelGetVolume,               // 37
    EAVControllerSetMute,                       // 38
    EAVControllerCancelSetMute,                 // 39
    EAVControllerGetMute,                       // 40
    EAVControllerCancelGetMute,                 // 41
    EAVControllerGetPositionInfo,               // 42
    EAVControllerCancelGetPositionInfo,         // 43
    EAVControllerCreateBrowsingSession,         // 44
    EAVControllerDestroyBrowsingSession,        // 45
    EAVControllerGetSearchCapabilitiesSize,     // 46
    EAVControllerCancelGetSearchCapabilitiesSize, // 47   
    EAVControllerGetSearchCapabilities,         // 48
    EAVControllerCreateContainer,               // 49
    EAVControllerCancelCreateContainer,         // 50
    EAVControllerDeleteObject,                  // 51
    EAVControllerCancelDeleteObject,            // 52
    EAVControllerDeviceDisappearedRequest,      // 53
    EAVControllerCancelDeviceDisappearedRequest,// 54   
    EAVControllerStartMediaServer,              // 55
    EAVControllerCancelStartMediaServer,        // 56
    EAVControllerStopMediaServer,               // 57
    EAVControllerMonitorConnection,             // 58
    EAVControllerCancelMonitorConnection,       // 59
    EAVControllerMSServicesInUse,               // 60
    EAVControllerStartupRequest,                 // 61
    EAVControllerCancelStartupRequest,           // 62
    EAVControllerCreateDownloadSession,        // 63
    EAVControllerCreateUploadSession,           // 64
    EAVControllerDestroyDownloadSession,        // 65
    EAVControllerDestroyUploadSession,          // 66
    EAVControllerStartDownload,                 // 67
    EAVControllerCancelDownload,                // 68
    EAVControllerCancelAllDownloads,            // 69   
    EAVControllerStartUpload,                   // 70
    EAVControllerCancelUpload,                  // 71
    EAVControllerCancelAllUploads,              // 72
    EAVControllerStartTrackingUploadProgress,   // 73
    EAVControllerStartTrackingDownloadProgress, // 74
    EAVControllerGetDownloadEvent,              // 75
    EAVControllerCancelDownloadEvent,           // 77
    EAVControllerGetUploadEvent,                // 78
    EAVControllerCancelUploadEvent,             // 79
    EAVControllerStartDownloadFH                // 80
    };
    
enum TAVControllerRsp
    {
    EAVControllerDeviceCompleted = 200,
    EAVControllerSetURICompleted,
    EAVControllerSetNextURICompleted,
    EAVControllerPlayCompleted,
    EAVControllerStopCompleted,
    EAVControllerPauseCompleted,
    EAVControllerSetVolumeCompleted,
    EAVControllerGetVolumeCompleted,
    EAVControllerSetMuteCompleted,
    EAVControllerGetMuteCompleted,
    EAVControllerPositionInfoCompleted,
    EAVControllerGetBrowseResponseSizeCompleted,
    EAVControllerGetBrowseResponseCompleted,    
    EAVControllerGetSearchResponseSizeCompleted,
    EAVControllerGetSearchResponseCompleted,
    EAVControllerGetSearchCapabilitiesSizeCompleted,
    EAVControllerGetSearchCapabilitiesCompleted,
    EAVControllerCopyRemoteItemStartCompleted,
    EAVControllerCopyRemoteItemFinishCompleted,
    EAVControllerCopyLocalItemStartCompleted,
    EAVControllerCopyLocalItemFinishCompleted,
    EAVControllerCopyToPhoneStartCompleted,
    EAVControllerCopyToPhoneFinishCompleted,
    EAVControllerCreateContainerCompleted,
    EAVControllerDeleteObjectCompleted,
    EAVControllerDeviceDisappearedRequestCompleted,
    EAVControllerStartMediaServerCompleted,
    EAVControllerConnectionLost
    };  

enum TAVControllerDeviceDiscovery
    {
    EAVDeviceDisappeared,
    EAVDeviceDiscovered
    };

enum TAVControllerDeviceListType
    {
    EAVMediaServer,
    EAVMediaRenderer
    };

/**
 * Defines unsolicited event
 */
enum TUnsolicitedEventE
    {
    EPlay = 0,
    EPause,
    EStop,
    EVolume,
    EMute
    };

/**
 * Helper class to queu unsolicited events
 */
class TUnsolicitedEventC
    {
    
public:    
    
    TUnsolicitedEventE  iEvent;
    TInt                iValue;
    };
    
#endif      // C_UPNPAVCONTROLLERGLOBALS_H
            
// End of File
