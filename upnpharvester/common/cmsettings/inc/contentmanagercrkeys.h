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
* Description:     
*     Definitions of internal keys related to Content Manager
*     settings in CenRep
*
*/





#ifndef CONTENTMANAGERCRKEYS_H
#define CONTENTMANAGERCRKEYS_H

#include <e32std.h>


/**
 * Content Manager UID
 *
 */
const TUid KCRUidCmContentManager = {0x10281FAB};

/**
 * KCmContentManagerState
 * Defines persistent state information of the Content Manager
 *
 */
const TUint32 KCmContentManagerState = 0x00000101;

/**
 * KCmHarvesterState
 * Defines persistent state information of the metadata harvester
 *
 */
const TUint32 KCmHarvesterState = 0x00000102;

/**
 * KCmFillState
 * Defines whether fill service state.
 *
 */
const TUint32 KCmFillState = 0x00000103;

/**
 * KCmStoreState
 * Defines whether store service state.
 *
 */
const TUint32 KCmStoreState = 0x00000104;

/**
 * KCmMemoryManagerState
 * Defines whether memory manager state.
 */
const TUint32 KCmMemoryManagerState = 0x00000105;

/**
 * KCmNextHarvestTime
 * Defines the date/time when next harvesting starts
 */
const TUint32 KCmNextHarvestTime = 0x00000106;

/**
 * KCmHarvestValue
 * Defines the harvesting period.
 */
const TUint32 KCmHarvestPeriod = 0x00000107;

/**
 * KCmNextFillTime
 * Defines the date/time when next content fill starts
 */
const TUint32 KCmNextFillTime = 0x00000108;

/**
 * KCmFillPeriod
 * Defines the content fill period.
 */
const TUint32 KCmFillPeriod = 0x00000109;

/**
 * KCmNextStoreTime
 * Defines the date/time when next content store starts
 */
const TUint32 KCmNextStoreTime = 0x0000010A;

/**
 * KCmStorePeriod
 * Defines the content store period.
 */
const TUint32 KCmStorePeriod = 0x0000010B;

/**
 * KCmDiscoveryDelay
 * Maximum time for device discovery
 */
const TUint32 KCmDiscoveryDelay = 0x0000010C;

/**
 * KCmAutoSync
 * Automatic synchronization state
 */
const TUint32 KCmAutoSync = 0x0000010D;

/**
 * KCmSearchCount
 * Item count for search
 */
const TUint32 KCmSearchCount = 0x0000010E;

/**
 * KCmSearchCount
 * Item count for database adding
 */
const TUint32 KCmAddCount = 0x0000010F;

/**
 * KCmAppWizardState
 * MediaServant wizard state
 */
const TUint32 KCmAppWizardState = 0x00000110;

#endif  // CONTENTMANAGERCRKEYS_H
