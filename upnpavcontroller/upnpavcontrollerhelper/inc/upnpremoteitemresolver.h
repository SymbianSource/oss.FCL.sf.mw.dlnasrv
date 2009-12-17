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
* Description:      Resolver for remote items
*
*/






#ifndef UPNPREMOTEITEMRESOLVER_H
#define UPNPREMOTEITEMRESOLVER_H

//  INCLUDES
#include <e32base.h>
#include "upnpitemresolver.h" // base class
#include "upnpabstractbrowsingsessionobserver.h" // base class

// FORWARD DECLARATIONS
class MUPnPAVBrowsingSession;
class CUpnpItem;
class CUpnpElement;
class MUPnPResourceSelector;

// CLASS DECLARATION

/**
* Resolves remote upnp items from a plain item id.
* This resolving is done by executing a upnp metadata browse, or
* possibly several ones of those.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPRemoteItemResolver
    : public CUPnPAbstractBrowsingSessionObserver
    , public MUPnPItemResolver
    {
public: // construction/destruction

    /**
     * static constructor.
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     * @return RemoteItemResolver instance 
     */
    static CUPnPRemoteItemResolver* NewL(
        const TDesC8& aItemId,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );

    /**
     * destructor
     * @since Series 60 3.2
     */
    virtual ~CUPnPRemoteItemResolver();

private: // private construction part

    /**
     * default constructor.
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     */
    CUPnPRemoteItemResolver(
        const TDesC8& aItemId,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );

    /**
     * 2nd phase constructor.
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     */
    void ConstructL(
        const TDesC8& aItemId,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );


public: // the interface

    /**
     * see UPnPItemResolver
     */
    void ResolveL(
        MUPnPItemResolverObserver& aObserver );

    /**
     * see UPnPItemResolver
     */
    const CUpnpItem& Item() const;


    /**
     * see UPnPItemResolver
     */
    const CUpnpElement& Resource() const;


protected: // MUPnPAVBrowsingSessionObserver

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void BrowseResponse(
        const TDesC8& aBrowseResponse,
        TInt aError,
        TInt aMatches,
        TInt aTotalCount,
        const TDesC8& aUpdateId
        );


private: // private methods

    /**
     * after receiving the result object, processes it:
     * either continues browsing recursively or calls back
     * notifying the object is ready to be fetched.
     */
    void ProcessResultObjectL( const CUpnpObject* aResult );

    /**
     * Completes ongoing async operation
     */
    void Complete( TInt aError );

    /**
     * Clean up all resources
     */
    void Cleanup();

    /**
     * internal state machinery
     */
    enum TInternalState {
        EStateIdle,    // no data, ready for starting
        EStateActive,  // browsing for data
        EStateReady    // Data exists
    };

    /**
     * Handles the parsing of the browse response.
     *
     * @since S60 3.2
     * @param aBrowseResponse (const TDesC8&) the browse response
     */
    void BrowseResponseL( const TDesC8& aBrowseResponse );

private: // members

    // item to resolve (Owned)
    HBufC8* iItemId;

    // the host browsing session
    MUPnPAVBrowsingSession& iBrowsingSession;

    // selector interface to use
    MUPnPResourceSelector& iSelector;

    // filter used in browse
    const TDesC8& iBrowseFilter;

    // observer for the resolver. NOT OWNED
    MUPnPItemResolverObserver* iObserver;

    // state of the class
    TInternalState iState;

    // counter for browse resursion
    TInt iRecursionDepth;

    // the first level browse result item (Owned).
    CUpnpItem* iFirstLevelItem;

    // optional: last level browse result item, if first level
    // is a pointer object (Owned). 
    CUpnpItem* iLastLevelItem;

    // The selected resource within the last level item. NOT OWNED
    const CUpnpElement* iResource;

    };


#endif  // UPNPREMOTEITEMRESOLVER_H

// End of File
