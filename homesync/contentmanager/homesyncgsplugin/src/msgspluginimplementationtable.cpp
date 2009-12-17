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
* Description:  ECOM proxy table for MediaServant plugin.
*
*/


// System includes
#include <ecom/implementationproxy.h>

// User includes
#include "mssettingsview.h"

// Constants
const TImplementationProxy KMSGSSettingsPluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x10282912, CMSSettingsView::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( KMSGSSettingsPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
    return KMSGSSettingsPluginImplementationTable;
    }

// End of File
