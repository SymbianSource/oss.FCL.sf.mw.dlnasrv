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
* Description:      Tool for resolving local or remote items
*
*/






#ifndef UPNPITEMRESOLVER_H
#define UPNPITEMRESOLVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class MUPnPResourceSelector;
class MUPnPItemResolverObserver;
class CUpnpItem;
class CUpnpElement;

// CLASS DECLARATION

/**
* An interface for resolving either remote or local content.
* The interface takes in some identification of the content,
* resolves the content and asynchronously notifies when the
* content is available. The content itself is provided by
* CUpnpItem, which contains item metadata, resource and URI,
* and it can be used directly in AVController interface to
* play content on remote renderer or download content.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class MUPnPItemResolver
    {

public: // construction/destruction


    /**
     * virtual destructor
     * for deleting the resolver implementation using the API pointer
     */
    virtual ~MUPnPItemResolver()  {}


public: // the interface

    /**
     * Starts resolving an object id using given session.
     * given Callback is notified when result objects are available,
     * or in case of an error.
     * @since S60 3.2
     * @param aObserver interface that will receive result callback
     * @param aSelector selects the desider resource
     */
    virtual void ResolveL(
        MUPnPItemResolverObserver& aObserver ) = 0;

    /**
     * access to the resolved item.
     * Note that if the object represents an item, it is not necessarily
     * safe to query the resource in this item, instead you should use
     * this item to query all metadata.
     * Use this method after receiving MetadataBrowseComplete( KErrNone )
     * @return reference to the item with the given ID
     */
    virtual const CUpnpItem& Item() const = 0;

    /**
     * access to the pointed item's resource.
     * The resource may be in the original or in the pointed item.
     * This is the easiest and recommended way for accessing the
     * resource.
     * example:
     * const TDesC& uri = browseUtil->Res().Value();
     * Use this method after receiving MetadataBrowseComplete( KErrNone )
     * @return reference to the upnp element of <res> type.
     */
    virtual const CUpnpElement& Resource() const = 0;

    };

#endif  // UPNPITEMRESOLVER_H

// End of File
