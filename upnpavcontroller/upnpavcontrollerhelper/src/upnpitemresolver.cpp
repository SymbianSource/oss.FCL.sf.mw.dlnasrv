/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Tool for resolving local or remote items
*
*/






// INCLUDE FILES
#include "upnpremoteitemresolver.h"
#include "upnplocalitemresolver.h"
#include "upnpdownloaditemresolver.h"

#include "upnpitemresolver.h"

// CONSTANTS

// METHODS

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewRemoteItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* MUPnPItemResolver::NewRemoteItemResolverL(
    const TDesC8& aItemId,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    {
    return CUPnPRemoteItemResolver::NewL(
            aItemId, aHostSession, aSelector, aBrowseFilter );
    }

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewRemoteItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* MUPnPItemResolver::NewLocalItemResolverL(
    const TDesC& aFilePath,
    MUPnPAVController& aAvController,
    MUPnPResourceSelector& aSelector )
    {
    return CUPnPLocalItemResolver::NewL(
            aFilePath, aAvController, aSelector );
    }

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewDownloadItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* MUPnPItemResolver::NewDownloadItemResolverL(
    const TDesC8& aItemId,
    MUPnPAVController& aAvController,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    {
    return CUPnPDownloadItemResolver::NewL(
            aItemId, aAvController, aHostSession, aSelector, aBrowseFilter );
    }

// --------------------------------------------------------------------------
// MUPnPItemResolver::~MUPnPItemResolver
// destructor
//---------------------------------------------------------------------------
MUPnPItemResolver::~MUPnPItemResolver()
    {
    }

