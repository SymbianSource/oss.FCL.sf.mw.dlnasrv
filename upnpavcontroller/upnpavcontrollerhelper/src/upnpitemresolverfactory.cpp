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
* Description:      Tool for resolving local or remote items - factory
*
*/






// INCLUDE FILES
// upnpframework / avcontroller helper api
#include "upnpitemresolverfactory.h"

// avcontrollerhelper internal
#include "upnpremoteitemresolver.h"
#include "upnplocalitemresolver.h"
#include "upnpdownloaditemresolver.h"

// CONSTANTS

// METHODS

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewRemoteItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* UPnPItemResolverFactory::NewRemoteItemResolverL(
    const TDesC8& aItemId,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter,
    TInt /*aOptimisationFlags*/ )
    {
    return CUPnPRemoteItemResolver::NewL(
        aItemId, aHostSession, aSelector, aBrowseFilter );
    }

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewRemoteItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* UPnPItemResolverFactory::NewLocalItemResolverL(
    const TDesC& aFilePath,
    MUPnPAVController& aAvController,
    MUPnPResourceSelector& aSelector,
    TInt aOptimisationFlags )
    {
    return CUPnPLocalItemResolver::NewL(
        aFilePath, aAvController, aSelector, aOptimisationFlags );
    }

// --------------------------------------------------------------------------
// MUPnPItemResolver::NewDownloadItemResolverL
// factory for implementation
//---------------------------------------------------------------------------
EXPORT_C MUPnPItemResolver* UPnPItemResolverFactory::NewDownloadItemResolverL(
    const TDesC8& aItemId,
    MUPnPAVController& aAvController,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter,
    TInt /*aOptimisationFlags*/ )
    {
    return CUPnPDownloadItemResolver::NewL(
        aItemId, aAvController, aHostSession, aSelector, aBrowseFilter );
    }

