/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      AVController interface
*
*/






#ifndef M_UPNPAVCONTROLLER_H
#define M_UPNPAVCONTROLLER_H

// INCLUDE FILES
#include <e32std.h>
#include "upnpavcontrollerglobals.h"

// FORWARD DECLARATIONS
class MUPnPAVDeviceObserver;
class CUpnpAVDeviceList;
class CUpnpAVDevice;
class MUPnPAVBrowsingSession;
class MUPnPAVRenderingSession;
class MUPnPFileUploadSession;
class MUPnPFileDownloadSession;

/**
 *  Defines the AV Controller interface
 *
 *  This class defines the AV Controller interface. Provides methods to
 *  get devices and to start and stop rendering and browsing sessions.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class MUPnPAVController
    {

public:

    /**
     * Sets Device observer instance
     *
     * @since Series 60 3.1     
     * @param aInstance MUPnPAVDeviceObserver instance of the class that 
     *        implements the IF
     * @return TInt status
     */
    virtual void SetDeviceObserver( MUPnPAVDeviceObserver& aObserver ) = 0;

    /**
     * Removes Device observer instance
     *
     * @since Series 60 3.1     
     * @param None
     * @return None
     */
    virtual void RemoveDeviceObserver() = 0;
    
    /**
     * Returns Device observer instance
     *
     * @since Series 60 3.1     
     * @param None
     * @return observer pointer
     */
    virtual MUPnPAVDeviceObserver* DeviceObserver() = 0;

    /**
     * Start a browsing session with a media server
     *
     * @since Series 60 3.1
     * @param aDevice media server
     * @return session reference
     */    
    virtual MUPnPAVBrowsingSession& StartBrowsingSessionL(
        const CUpnpAVDevice& aDevice ) = 0;

    /**
     * Stop a browsing session with a media server
     *
     * @since Series 60 3.1
     * @param aSession session reference
     * @return None
     */    
    virtual void StopBrowsingSession( MUPnPAVBrowsingSession& aSession ) = 0;
    
    /**
     * Start a rendering session with a media renderer
     *
     * @since Series 60 3.1
     * @param aDevice media renderer
     * @return session reference
     */    
    virtual MUPnPAVRenderingSession& StartRenderingSessionL(
        const CUpnpAVDevice& aDevice ) = 0;
    
    /**
     * Stop a rendering session with a media server
     *
     * @since Series 60 3.1
     * @param aSession session reference
     * @return None
     */    
    virtual void StopRenderingSession(
        MUPnPAVRenderingSession& aSession ) = 0;

    /**
     * Start an upload session with a media server
     *
     * @since Series 60 3.1
     * @param aDevice media renderer
     * @return session reference
     */    
    virtual MUPnPFileUploadSession& StartUploadSessionL(
        const CUpnpAVDevice& aDevice ) = 0;
    
    /**
     * Stop an upload session
     *
     * @since Series 60 3.1
     * @param aSession session reference
     * @return None
     */    
    virtual void StopUploadSession(
        MUPnPFileUploadSession& aSession ) = 0;    

    /**
     * Start a download session with a media server
     *
     * @since Series 60 3.1
     * @param aDevice media renderer
     * @return session reference
     */    
    virtual MUPnPFileDownloadSession& StartDownloadSessionL(
        const CUpnpAVDevice& aDevice ) = 0;
    
    /**
     * Stop a download session
     *
     * @since Series 60 3.1
     * @param aSession session reference
     * @return None
     */    
    virtual void StopDownloadSession(
        MUPnPFileDownloadSession& aSession ) = 0;    

// =======================================================
// DEVICE CONTROL METHODS
// =======================================================
public:

    /**
     * Gets the list of online Media Server devices.
     *
     * The client must release the memory allocated for the items 
     * in the array.
     *
     * @since Series 60 3.1
     * @param none
     * @return device list
     */
    virtual CUpnpAVDeviceList* GetMediaServersL() = 0;

    /**
     * Gets the list of online Media Renderer devices.
     *
     * The client must release the memory allocated for the items 
     * in the array.
     *
     * @since Series 60 3.1
     * @param none
     * @return device list
     */
    virtual CUpnpAVDeviceList* GetMediaRenderersL() = 0;
    
public: // Destructor    

    /**
     * Destructor
     * DEPRACATED, usage of Release() recommended. Will be removed in the
     * future. Safe deletion is not guaranteed.
     *
     * @since Series 60 3.1
     */   
    virtual ~MUPnPAVController() {};
    
    /**
     * Destructor. Ensures safe deletion (also during WLAN disconnection).
     *
     * @since Series 60 3.2
     */    
    virtual void Release() = 0;
    };


#endif // M_UPNPAVCONTROLLER_H
