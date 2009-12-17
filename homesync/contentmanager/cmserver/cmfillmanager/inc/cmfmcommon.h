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
* Description:  Common header for fill manager
*  Version     : %version: tr1ido#7 % << Don't touch!
*
*/


#ifndef CMFMCOMMON_H
#define CMFMCOMMON_H

// Fill temp file
_LIT( KCmFmTempFile, "cm_temp" );


// Status codes used in Fill Manager
enum TCmFmStatus
    {
    ECmFmNULL = 0,
    ECmFmFillRuleProgessingStarted, // Rule processing started
    ECmFmFillRulesReady,            // Not used at the moment
    ECmFmMMCCheckStarted,           // memory card check starting
    ECmFmDeviceListingStarted,      // Not used at the moment
    ECmFmWrongMMC,                  // Wrong memory card found
    ECmFmMMCOk,                     // Memory ok
    ECmFmMMCCheckFailed,            // Memory checking failed
    ECmFmNoConnection,              // Not used at the moment 
    ECmFmConnectionLost,            // Not used at the moment 
    ECmFmStartUriCheck,             // Starting Uri check
    ECmFmDeleteFiles,               // Starting delete files
    ECmFmShrinkImage,               // Starting shrink images
    ECmFmBrowseReady,               // Browse to check uri ready
    ECmFmUriOK,                     // Uri ok
    ECmFmCheckItem                  // Starting check item
    };

// File transfer statuses
enum TCmFmTransferStatus
    {
    ECmFmFileFetchStarted = 20,     // File copy started
    ECmFmFileFetched                // File copied
    };  

// Uri check statuses    
enum TCmFmUriCheckResult
    {
    ECanceled = 25,                 // Operation canceled
    ENoUriAvailable,                // Uri not available
    EUriChecked                     // Uri checked
    };

// Fill list processing statuses    
enum TCmFmFillRuleProcessing 
    {
    EProcessingStarted = 60,        // Fill rule processing started
    EProcessing,                    // Processing on-going
    EProcessingReady,               // Processing ready
    EQueryCompleted,                // Metadata query completed
    EQueryFailed,                   // Metadata query failed
    ENextFillRule,                  // Processing next rule
    EFillRulesLoaded,               // Fill rules loaded
    ENoRulesSelected,               // Rules selected
    ENoObjectsSelected,             // Fill objects selected
    EProcessingCanceled,            // Canceled
    EPreProcessingStarted,          // Preprocessing started
    EFillFilesLoaded,               // Files loaded
    EProcessStatusValues,           // Processing file statuses
    ERandomizingStarted,            // Randomizing list
    EColumnDataReady,               // Property columns got
    EProcessDuplicates,             // Processing duplicate files
    EProcessReferenceValues,        // Processing reference files end
    EProcessingFailed,              // Processing failed
    EUpdatePriorities,              // Updating fill list priorities
    EDoUpdatePriorities,            // Do update priorities
    EMetadataDeleted                // Metadata deleted
    };    
    
#endif // CMFMCOMMON_H

// end of file
