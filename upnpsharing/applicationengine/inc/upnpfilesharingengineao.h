/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      file sharing engine active object class definition
*
*/






#ifndef __UPNPFILESHARINGENGINEAO_H__
#define __UPNPFILESHARINGENGINEAO_H__


// Include Files
#include <e32base.h>
#include <badesca.h>  //for CDesCArray
#include <upnpmediaserverclient.h>
#include "upnpcontentserverclient.h"

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;
class CUPnPIconConfigurator;

// CLASS DECLARATION
/**
 *  Active object class for file sharing
 *  Handles starting and stopping of filesharing and upload listener
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CUPnPFileSharingEngineAO ): public CActive
    {

public: // Constructors and destructor

    /**
     * C++ default constructor.
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aContentServer Handle to upnpcontentserver
     */
    CUPnPFileSharingEngineAO( 
        CUPnPFileSharingEngine& aEngine, 
        RUpnpContentServerClient& aContentServer );

    /**
     * Destructor.
     */
    virtual ~CUPnPFileSharingEngineAO();

private: // From CActive

    /**
     * Function is called when active request is ready
     * @since S60 3.1
     */
    void RunL();

    /**
     * Handles a leave occurring in the request completion event handler
     * @since S60 3.1
     * @param aError The leave code
     */
    TInt RunError( TInt aError );

    /**
     * Cancels active request
     * @since S60 3.1
     */
    void DoCancel();

public: // new functions

    /**
     * Starts file sharing process
     * @since S60 3.1
     * @param aState ETrue to enable sharing, otherwise EFalse
     */
    void SetSharingStateL( TBool aState );

    /**
     * Gets file sharing state from local MediaServer
     * @since S60 3.1
     * @return ETrue if sharing is ON otherwise EFalse
     */
    TBool SharingStateL();

    /**
     * Set the internal state of class in case connection is lost
     * @since S60 3.1
     */
    void ConnectionLost();

    /**
     * Check that there is enough disk space in c drive
     * @since S60 3.1
     * @return EFalse if there is no disk space
     */
    TBool HasDiskSpace();

    /**
     * Inform that settings of mediaserver icon is complete
     * @since S60 3.2
     * @param aError Error code of the operation
     */
    void SetIconComplete( TInt aError );

private:

    /**
     * Open connection to media server
     * @since S60 3.1
     */
    TInt OpenMediaServer();

    /**
     *Enables upnp security
     */
    void EnableUpnpSecurityL();

    /**
     * Checks whether mediaserver icons need to be configured
     */
    void CheckMediaserverIconsL();

private: // Data

    /**
     * file sharing engine
     */
    CUPnPFileSharingEngine& iEngine;

    /**
     * Content server session handle
     */
    RUpnpContentServerClient& iContentServer;

    /**
     * mediaserver session handle
     */
    RUpnpMediaServerClient iMediaServer;

    /**
     * Internal state
     */
    enum TState
        {
        ENotActive,
        EStartSharing,
        EStopSharing,
        ECheckIcon,
        EWaitSetConMonStateTrue,
        EWaitSetConMonStateFalse
        };
    TState  iState;

    /**
     * Cached value of sharing state
     */
    enum TUpnpVisibility
        {
        EUnknown = 0,
        EOnline,
        EOffline
        };
    TUpnpVisibility iVisibility;

    /**
     * handle add icons
     * owned
     */
    CUPnPIconConfigurator* iIconConfigurator;

    };

#endif  // __UPNPFILESHARINGENGINEAO_H__

// End of file
