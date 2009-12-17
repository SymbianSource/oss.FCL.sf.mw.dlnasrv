/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Common Client/Server header for Content Manager
*
*/






#ifndef CMCOMMON_H
#define CMCOMMON_H

_LIT(KCmServerExe,"cmserver.exe"); ///<EXE name

// CONSTANTS

// Server name
_LIT(KCmServerName,"CmServer");

// Panic name
_LIT(KCmServerPanic, "CmServerPanic");

/// A version must be specified when creating a session with the server
const TUint KCmServerMajor = 1;
const TUint KCmServerMinor = 0;
const TUint KCmServerBuild = 0;

const TUid KCmPropertyCat = {0x10012345};
const TInt KCmProperty = 1;

const TInt64 KCmMmcLowMemory = 10*KMega;

// Wlan scan intervals [s]
const TInt KWlanScanNetworkNever = 0;
const TInt KWlanScanNetworkInterval60 = 60;

// Cds namespace
_LIT(KCdsNamespace, "http://www.nokia.com/namespaces/metadata/cdssync/");

/**  State information for the Content Manager */
enum TCmServerState
    {
    ECmServerStateDisabled = 0,
    ECmServerStateIdle = 1,
    ECmServerStateStoring = 2,
    ECmServerStateFilling = 3,
    ECmServerStateHarvesting = 4,
    ECmServerStateGettingStoreFields = 6,
    ECmServerStateGettingFillFields = 7,
    ECmServerStatePreProcessingStore = 8,
    ECmServerStatePreProcessingFill = 9,
    ECmServerStateSearchingForServers = 12,
    ECmServerStateDeleting = 13,
    ECmServerStatePreProcessingFilllist = 14,
    ECmServerStateUpdatingFillLists = 15,
    ECmServerStateDeletingMetadata = 16
    };

/**  Content Manager services */
enum TCmService
    {
    // main service which must be enabled for anything to work
    // Note: executable service identifiers equal state identifiers
    ECmServiceNone = 0,
    ECmServiceContentManager = 1,
    ECmServiceStore = 2,
    ECmServiceFill = 3,
    ECmServiceHarvest = 4,
    ECmServiceMemoryManager = 5,
    ECmServiceGetStoreFields = 6,
    ECmServiceGetFillFields = 7,
    ECmServicePreProcessingStore = 8,
    ECmServicePreProcessingFill = 9,
    ECmServiceTransferInfoFill = 10,
    ECmServiceTransferInfoStore = 11,
    ECmServiceSearchForServers = 12,
    ECmServiceDeleteFilledFiles = 13,
    ECmServicePreProcessingFilllist = 14,
    ECmServiceUpdateFillLists = 15,
    ECmServiceDeleteMetadata = 16,
    ECmServiceAutomStoreListProcess = 17
    };

/**  Content Manager service states */
enum TCmServiceState
    {
    ECmServiceStateDisabled = 0,
    ECmServiceStateAutomatic
    };

/// opcodes used in message passing between client and server
enum TCmRequest
    {
    ECmGetServerState = 1,
    ECmSetServiceState,
    ECmGetServiceState,
    ECmExecuteService,
    ECmSetFillRules,
    ECmGetFillRules,
    ECmGetFillRulesSize,
    ECmGetMediaserversSize,
    ECmSetMediaServers,
    ECmSetStoreRules,
    ECmGetStoreRulesSize,
    ECmGetFillItemsSize,
    ECmGetStoreItemsSize,
    ECmPreProcessFillList,
    ECmGetItemCount,
    ECmGetDataBuffer,
    ECmGetDataFields,
    ECmGetDataFieldSize,
    ECmCancel,
    ECmGetAppWizardState,
    ECmSetAppWizardState,
    ECmGetSyncTime,
    ECmSetFillFileStatuses,
    ECmGetTransferInfo,
    ECmUpdateFillLists,
    ECmDeleteMetadata,
    ECmDeleteDeletedMediaServers,
    ECmSetDrives,
    ECmGetDrivesSize,
    ECmGetDriveSelectionStatus,
    ECmApplicationExit,
    ECmGetFilledFilesSize
    };

/// Panic enums
enum TCmClientPanics
    {
    ECmClientNoSessionActive,
    ECmClientBadRequest
    };

/// Panic enums
enum TCmServerPanics
    {
    EDCmServerIllegalParameter
    };

class TCmProgressInfo
    {
public: // public member variables
    TCmService iService; // Service identifier
    TInt iTotalItems;    // Total count of items
    TInt iProcessedItems;// Processed items
    TInt iItemsToTransferPerService; // Remaining items per service
    };

enum TCmListItemStatus
    {
    ECmToBeFilled = 1,
    ECmFilled = 2,
    ECmToBeRemoved = 4,
    ECmToBeShrinked = 8,
    ECmToBeSkipped = 16,
    ECmSkippedByUser = 32,
    ECmLocalCopy = 64,
    ECmStored = 128,
    ECmKeepOnDevice = 256
    };


enum TCmShrinkResolution
    {
    ECmShrinkResScreenSize = 0
    };

class TCmLowPriorityFilled
    {
public: // public member variables
    HBufC* iFullPath; // Path of the filled file
    TUint32 iSize;    // Size of the filled file
    TInt64 iListId;   // Items fill list id
    };

typedef TPckgC<TCmProgressInfo> TCmProgressInfoPckg;

#endif
// end of file
