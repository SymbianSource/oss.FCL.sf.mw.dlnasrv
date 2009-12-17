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
* Description:  Constant definitions
*
*/


#ifndef MSCONSTANTS_H
#define MSCONSTANTS_H


// INCLUDE FILES
#include <e32std.h>


// MediaServant UIDs
const TUid KUidMediaServant = { 0x1028290B };

//const TInt KMSMainViewIdValue = 1;
const TUid KMSMainViewId = { 0x10282913 };
//const TInt KMSFillViewIdValue = 2;
const TUid KMSFillViewId = { 0x10282914 };
//const TInt KMSSettingsViewIdValue = 3;
//const TUid KMSSettingsViewId = { 0x10282915 };
//const TInt KMSFillEditViewIdValue = 4;
const TUid KMSFillEditViewId = { 0x10282915 };
//const TInt KMSStoreListViewIdValue = 5;
const TUid KMSStoreListViewId = { 0x10282916 };
//const TInt KMSStoreSettingsViewIdValue = 6;
const TUid KMSStoreSettingsViewId = { 0x10282917 };
//const TInt KMSBrowseViewIdValue = 7;
const TUid KMSBrowseViewId = { 0x10282918 };
//const TInt KMSFillBrowseViewIdValue = 8;
const TUid KMSFillBrowseViewId = { 0x10282919 };

// Listbox format strings

// Single graphic style listbox
_LIT( KSingleGraphicStyleFormatString, "%d\t%S\t%d\t" );
// Single graphic style listbox - no trailing icons
_LIT( KSingleGraphicStyleFormatStringNoTrailIcons, "%d\t%S\t\t" );
// Single graphic style listbox
_LIT( KSingleStyleFormatString, "\t%S\t\t" );
// Double large graphic style listbox
_LIT( KDoubleLargeGraphicStyleFormatString, "%d\t%S\t%S\t");
// Double large graphic style listbox (no secondary text)
_LIT(KDoubleLargeGraphicStyleFormatStringEmpty, "%d\t%S\t\t");
// Signle graphic style lis
_LIT( KItemFormatString, "1\t%S");

// Icon files
_LIT( KAvkonMifFileName, "\\resource\\apps\\avkon2.mif" );
_LIT( KMSMifFileName, "\\resource\\apps\\mediaservant.mif" );

// TITLE PANE
// CONSTANTS
const TInt KMSTitleBufLength = 256 ; // for lenght.
const TInt KMSDefaultTitleId = 0;

// Setting item secondary text delimiter
_LIT( KDelimiter, ",");

// Time format strings
_LIT(KDefaultFromDate, "19000000:000000.000000"); // 01.01.1900

// File sizes
const TInt KMSSize_0_MB = 0;
const TInt KMSSize_1_MB = KMega;
const TInt KMSSize_5_MB = 5*KMega;
const TInt KMSSize_10_MB = 10*KMega;
const TInt KMSSize_50_MB = 50*KMega;

// Track lengths [s]
const TInt KMSTrackLength_0_min = 0;
const TInt KMSTrackLength_1_min = 60;
const TInt KMSTrackLength_6_min = 360;
const TInt KMSTrackLength_8_min = 480;
const TInt KMSTrackLength_30_min = 1800;

// Listbox tabulator defitions
_LIT(KSlashT, "\t");
_LIT(KZeroSlashT, "0\t");
_LIT(KSPaceT, " ");


// Array granularities
const TInt KMetadataArrayGranularity = 5;

// Default quota size [%]
const TInt KHDDefaultQuota = 75;
const TInt KMMCDefaultQuota = 90;

// Default fill list size [MB]
const TInt KDefaultListSize50 = 50;
const TInt KDefaultListSize200 = 200;

// Server statuses
const TUint KServerNotActive = 0;
const TUint KServerActive = 1;
const TUint KServerRemoved = 2;

// Settings plugin
const TUid settinsPluginUid = { 0x10282912 };

#endif      // MSCONSTANTS_H
