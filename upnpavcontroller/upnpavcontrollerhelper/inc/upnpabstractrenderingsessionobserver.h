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
* Description:      An abstract proxy for a rendering session observer
*
*/






#ifndef UPNPABSTRACTRENDERINGSESSIONOBSERVER_H
#define UPNPABSTRACTRENDERINGSESSIONOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h" // base class

// FORWARD DECLARATIONS
class MUPnPAVRenderingSession;

// CLASS DECLARATION

/**
* An abstract class for implementing a rendering session observer.
* This class is purposed as a base class for imnplementing a
* rendering session observer. There are 2 main use cases:
* 1. reducing implementation code so that the actual observer will
*    only need to write the observer methods that are really used.
* 2. implementing an observer proxy that will attach itself to a session
*    and just handle some of the callbacks and forward a majority of the
*    callbacks the original session observer.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPAbstractRenderingSessionObserver
    : public CBase
    , public MUPnPAVRenderingSessionObserver
    {

public: // construction/destruction

    /**
     * default constructor.
     */
    IMPORT_C CUPnPAbstractRenderingSessionObserver();

    /**
     * destructor
     */
    IMPORT_C virtual ~CUPnPAbstractRenderingSessionObserver();


public: // the abstract observer interface

    /**
     * Sets the host session for the observer. This is the session where
     * the observer will register for callbacks. If the session already
     * has an observer, it will become the proxy observer. The default
     * implementatin for all observer methods is to forward the method to
     * the observer, if it exists.
     *
     * @since S60 3.2
     * @param aHostSession, rendering session
     */
    IMPORT_C void SetSession( MUPnPAVRenderingSession& aHostSession );

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

public: // MUPnPAVRenderingSessionObserver

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void VolumeResult( TInt aError, TInt aVolumeLevel,
        TBool aActionResponse );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void MuteResult( TInt aError, TBool aMute,
        TBool aActionResponse );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void InteractOperationComplete( TInt aError,
        TUPnPAVInteractOperation aOperation );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void PositionInfoResult( TInt aError,
        const TDesC8& aTrackPosition, const TDesC8& aTrackLength );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void SetURIResult( TInt aError );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void SetNextURIResult( TInt aError );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void MediaRendererDisappeared(
        TUPnPDeviceDisconnectedReason aReason );

    /**
     * see MUPnPAVRenderingSessionObserver
     */
    void ReserveLocalMSServicesCompleted( TInt aError );


private: // members

    // the host rendering session. Note: NOT OWNED!
    MUPnPAVRenderingSession* iObservedSession;

    // The proxy observer. Note: NOT OWNED!
    MUPnPAVRenderingSessionObserver* iProxyObserver;

    // if observer is enabled
    TBool iObserverEnabled;

    };


#endif  // UPNPABSTRACTRENDERINGSESSIONOBSERVER_H

// End of File

