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
* Description:      An abstract browsing session observer implementation
*
*/






#ifndef UPNPABSTRACTBROWSINGSESSIONOBSERVER_H
#define UPNPABSTRACTBROWSINGSESSIONOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include "upnpavbrowsingsessionobserver.h" // base class

// FORWARD DECLARATIONS
class MUPnPAVBrowsingSession;

// CLASS DECLARATION

/**
* An abstract class for implementing a browsing session observer.
* This class is purposed as a base class for imnplementing a
* browsing observer. There are 2 main use cases:
* 1. reducing implementation code so that the actual observer will
*    only need to write the observer methods that are really used.
* 2. implementing an observer proxy that will attach itself to a session
*    and just handle some of the callbacks and forward a majority of the
*    callbacks the original session observer.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPAbstractBrowsingSessionObserver
    : public CBase
    , public MUPnPAVBrowsingSessionObserver
    {

public: // construction/destruction

    /**
     * default constructor.
     */
    IMPORT_C CUPnPAbstractBrowsingSessionObserver();

    /**
     * destructor
     */
    IMPORT_C virtual ~CUPnPAbstractBrowsingSessionObserver();


public: // the abstract observer interface

    /**
     * Sets the host session for the observer. This is the session where
     * the observer will register for callbacks. If the session already
     * has an observer, it will become the proxy observer. The default
     * implementatin for all observer methods is to forward the method to
     * the observer, if it exists.
     *
     * @since S60 3.2
     * @param aHostSession, browsing session
     */
    IMPORT_C void SetSession( MUPnPAVBrowsingSession& aHostSession );

    /**
     * Enables receiving callbacks. When enabled, the abstract observer
     * registers to the given session and thus starts receiving callbacks.
     * Note that only those callbacks that are overwritten in the derived
     * class will actually provide some value. Other will be either processed
     * in an empty method, or forwarded to the observer that previously was
     * registered as an observer (proxy mode)
     *
     * @since S60 3.2
     */
    IMPORT_C void EnableSessionObserver();

    /**
     * Disables receiving session callbacks. The observer interface is
     * deregistered, and if before observer registration there was an 
     * original observer interface (proxy mode) the original observer
     * is restored.
     *
     * @since S60 3.2
     */
    IMPORT_C void DisableSessionObserver();

    /**
     * Returns true if the observer is active (registered for events)
     *
     * @since S60 3.2
     */
    IMPORT_C TBool IsSessionObserverEnabled();

public: // MUPnPAVBrowsingSessionObserver

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void BrowseResponse(
        const TDesC8& aBrowseResponse,
        TInt aError,
        TInt aMatches,
        TInt aTotalCount,
        const TDesC8& aUpdateId );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void SearchResponse( 
        const TDesC8& aSearchResponse,
        TInt aError,
        TInt aMatches,
        TInt aTotalCount,
        const TDesC8& aUpdateId );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void SearchCapabilitiesResponse( 
        TInt aError,
        const TDesC8& aSearchCapabilities );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void CreateContainerResponse( TInt aError, 
        const TDesC8& aObjectId );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void DeleteObjectResponse( TInt aError );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void ReserveLocalMSServicesCompleted( TInt aError );

    /**
     * see MUPnPAVBrowsingSessionObserver
     */
    void MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason aReason );


private: // members

    // the host browsing session. Note: NOT OWNED!
    MUPnPAVBrowsingSession* iObservedSession;

    // The proxy observer. Note: NOT OWNED!
    MUPnPAVBrowsingSessionObserver* iProxyObserver;

    // if observer is enabled
    TBool iObserverEnabled;

    };


#endif  // UPNPABSTRACTBROWSINGSESSIONOBSERVER_H

// End of File

