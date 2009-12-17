/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common header for store manager
*
*/


#ifndef __CMSMCOMMON_H
#define __CMSMCOMMON_H

// Used in store manager panics
_LIT( KCmSmManager, "CmStoreManager" );

/// Status codes used in Store Manager
// Transfer engine call back statuses
enum TCmSmTransferStatus
    {
    ECmSmTransferCompleted = 0,
    ECmSmAborted,
    ECmSmTransferCanceled,
    ECmSmFileTransferred,
    ECmSmFileTransferFailed,
    ECmSmWlanLost,
    ECmSmTransferContinued
    };

// Store file processing call back statuses  
enum TCmSmFileProcessingStatus 
    {
    ECmSmStoreRulesLoaded = 20,
    ECmSmProcessingFilesStarted,
    ECmSmProcessingFilesReady,
    ECmSmAllLoaded,
    ECmSmStoreRulesReady,
    ECmSmAllRefreshed,
    ECmSmMediaTypesSelected,
    ECmSmFilesLoaded,
    ECmSmStartItemFiltering,
    ECmSmContinueFiltering,
    ECmSmFilteringReady,
    ECmSmProcessingCanceled,
    ECmSmRefreshError,
    ECmSmNoStoreRulesSelected,
    ECmSmNoDevicesAvailable,
    ECmSmPreProcessingStarted,
    ECmSmLoadFilledAndStored,
    ECmSmPreProcessingReady,
    ECmSmFileAdded,
    ECmSmFileRemoved,
    ECmSmFileRenamed,
    ECmSmEventProcessed,
    ECmSmProcessAlbumList,
    ECmSmAlbumFilteringReady,
    ECmSmAlbumProcessingReady,
    ECmSmQueryCompleted,
    ECmSmQueryFailed,
    ECmSmContentChanged,
    ECmSmNone
    };    

// Store manager main statuses 
enum TCmSmMainSequencer
    {
    ECmSmGetMediaServers = 40,
    ECmSmStartFileProcessing,
    ECmSmStartFileFiltering,
    ECmSmStartCopying,
    ECmSmContinueCopying,
    ECmSmManageFiles,
    ECmSmFinishCanceling,
    ECmSmFinishing
    };    

// Transfer engine states    
enum TCmSmTransferEngineState
    {
    ECmSmIdle = 60,
    ECmSmCopying,
    ECmSmNextFile,
    ECmSmWaitingForCopyStarted,
    ECmSmTransferComplete,
    ECmSmTransferAborted,
    ECmSmStartingTransfer,
    ECmSmCanceling,
    ECmSmReserveLocalMs,
    ECmSmReserveLocalMsFailed
    };

#endif // __CMSMCOMMON_H

// end of file

