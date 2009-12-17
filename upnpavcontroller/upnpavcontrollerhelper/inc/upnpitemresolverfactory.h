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
* Description:      Tool for resolving local or remote items - factory
*
*/






#ifndef UPNPITEMRESOLVERFACTORY_H
#define UPNPITEMRESOLVERFACTORY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MUPnPItemResolver;
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class MUPnPResourceSelector;

// CLASS DECLARATION

/**
* Factory class for constructing MUPnPItemResolver
* implementations. This class only contains static methods.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class UPnPItemResolverFactory
    {

public: // construction/destruction

    /**
     * The optimisation flags
     */
    enum TResolverOptimisationFlags
        {
        ENoOptimisation = 0x0000,
            // this is the default
        EOmitLocalMSStart = 0x0001,
            // do not start local MS (in this case it must
            // already started and kept alive the whole time
        EOmitDrmCheck     = 0x0002
            // do not check for file protection (in this case
            // protection check must have been done before)
        };

    /**
     * Creates a resolver for remote content
     * @param aItemId the item ID to resolve into an item
     * @param aHostSession the session where to resolve items
     * @param aSelector selects the desider resource
     * @param aBrowseFilter filter to be used in browse
     * @param aOptimisationFlags flags to optimise resolver behaviour
     * @return instance of RemoteItemResolver
     */
    IMPORT_C static MUPnPItemResolver* NewRemoteItemResolverL(
        const TDesC8& aItemId,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter,
        TInt aOptimisationFlags = ENoOptimisation );

    /**
     * Creates a resolver for local content
     * @param aFilePath the local file to be resolved into an item
     * @param aAvController AVController resource to reserve local media server
     * @param aSelector selects the desider resource
     * @param aOptimisationFlags flags to optimise resolver behaviour
     * @return instance of RemoteItemResolver
     */
    IMPORT_C static MUPnPItemResolver* NewLocalItemResolverL(
        const TDesC& aFilePath,
        MUPnPAVController& aAvController,
        MUPnPResourceSelector& aSelector,
        TInt aOptimisationFlags = ENoOptimisation );

    /**
     * Creates a resolver for remote content that will be resolved and
     * then downloaded to a temporary file in local file system
     * @param aItemId the item ID to resolve into an item
     * @param aAvController AVController resource to reserve local media server
     * @param aHostSession the session where to resolve items
     * @param aSelector selects the desider resource
     * @param aBrowseFilter filter to be used in browse
     * @param aOptimisationFlags flags to optimise resolver behaviour
     * @return instance of RemoteItemResolver
     */
    IMPORT_C static MUPnPItemResolver* NewDownloadItemResolverL(
        const TDesC8& aItemId,
        MUPnPAVController& aAvController,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter,
        TInt aOptimisationFlags = ENoOptimisation );

    };

#endif  // UPNPITEMRESOLVERFACTORY_H

// End of File
