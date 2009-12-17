/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the UpnpAiwProvider.
*
*/


#include <ecom/implementationproxy.h>
#include "upnpaiwprovideruids.hrh"
#include "upnpaiwenableexternalservice.h"
#include "upnpaiwcopytoexternalservice.h"
#include "upnpaiwmovetoexternalservice.h"
#include "upnpaiwplayonexternalbaseservice.h"
#include "upnpaiwplayonexternalmenuservice.h"
#include "upnpaiwopenexternalservice.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwEnableExternalMenuServiceUid,
                               CUPnPAiwEnableExternalService::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwCopyToExternalMenuServiceUid,
                               CUPnPAiwCopyToExternalService::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwMoveToExternalMenuServiceUid,
                               CUPnPAiwMoveToExternalService::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwPlayOnExternalMenuServiceUid,
                               CUPnPAiwPlayOnExternalMenuService::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwPlayOnExternalBaseServiceUid,
                               CUPnPAiwPlayOnExternalBaseService::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KUPnPAiwOpenExternalBaseServiceUid,
                               CUPnPAiwOpenExternalService::NewL)
    };

// --------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                        TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of file
