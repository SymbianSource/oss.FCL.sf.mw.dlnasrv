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
* Description:      AO for monitoring device events
*
*/







#ifndef C_UPNPAVCONTROLLERACTIVE_H
#define C_UPNPAVCONTROLLERACTIVE_H

// INCLUDE FILES
#include <e32base.h>
#include "upnpavcontroller.h"

#include "upnpconnectionmonitorobserver.h"
#include "upnpavcontrollerclient.h"

// FORWARD DECLARATIONS
class MUPnPAVDeviceObserver;

class CUPnPAVBrowsingSessionImpl;
class CUPnPAVRenderingSessionImpl;
class CUPnPAVDeviceActive;
class CUPnPAVConnectionActive;
class MUPnPFileDownloadSession;
class MUPnPFileUploadSession;
class CUPnPFileUploadSessionImpl;
class CUPnPFileDownloadSessionImpl;

/**
 *  Implements the AV Controller interface
 *
 *  This class implements the AV Controller interface. Provides methods to
 *  get devices and to start and stop rendering and browsing sessions.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVControllerActive : public CActive,
                                public MUPnPAVController,
                                public MUPnPConnectionMonitorObserver
    {

private:

    /**
     * Release (delete) state. Declares state variables to determine if
     * deletion is safe.
     */    
    enum TReleaseState
        {
        EStateReleasable = 0,
        EStateWLANLost,
        EStateWaitingForRelease
        };

public:

    /**
     * Static constructor
     *
     * @param nonr
     * @return a new av controller instance
     */
    static CUPnPAVControllerActive* NewL();

    /**
     * Static constructor
     *
     * @param nonr
     * @return a new av controller instance
     */    
    static CUPnPAVControllerActive* NewLC();
       
    /**
     * Destructor. Ensures safe deletion (also during WLAN disconnection).
     */    
    virtual void Release();
    
    /**
     * Destructor
     * DEPRACATED, usage of Release() recommended. Will be removed in the
     * future. Safe deletion is not guaranteed.
     */    
    virtual ~CUPnPAVControllerActive();
    
private:

    /**
     * 2nd phase construct
     */    
    CUPnPAVControllerActive();    

    /**
     * 2nd phase construct
     */        
    void ConstructL();
    
protected: // From CActive

    void RunL();
    
    void DoCancel();
    
    TInt RunError( TInt aError );

protected: // From MUPnPAVController

    /**
     * See upnpavcontroller.h
     */
    void SetDeviceObserver( MUPnPAVDeviceObserver& aObserver );

    /**
     * See upnpavcontroller.h
     */
    void RemoveDeviceObserver();

    /**
     * See upnpavcontroller.h
     */
    MUPnPAVDeviceObserver* DeviceObserver();

    /**
     * See upnpavcontroller.h
     */
    CUpnpAVDeviceList* GetMediaServersL();

    /**
     * See upnpavcontroller.h
     */
    CUpnpAVDeviceList* GetMediaRenderersL();
    
    /**
     * See upnpavcontroller.h
     */
    MUPnPAVBrowsingSession& StartBrowsingSessionL(
        const CUpnpAVDevice& aDevice );

    /**
     * See upnpavcontroller.h
     */
    void StopBrowsingSession( MUPnPAVBrowsingSession& aSession );
    
    /**
     * See upnpavcontroller.h
     */
    MUPnPAVRenderingSession& StartRenderingSessionL(
        const CUpnpAVDevice& aDevice );
    
    /**
     * See upnpavcontroller.h
     */
    void StopRenderingSession( MUPnPAVRenderingSession& aSession );

    /**
     * See upnpavcontroller.h
     */
    MUPnPFileUploadSession& StartUploadSessionL(
        const CUpnpAVDevice& aDevice );
    
    /**
     * See upnpavcontroller.h
     */
    void StopUploadSession( MUPnPFileUploadSession& aSession );    

    /**
     * See upnpavcontroller.h
     */
    MUPnPFileDownloadSession& StartDownloadSessionL(
        const CUpnpAVDevice& aDevice );
    
    /**
     * See upnpavcontroller.h
     */
    void StopDownloadSession( MUPnPFileDownloadSession& aSession );
    
protected: // From CUPnPConnectionMonitorObserver

    void ConnectionLost();    

private:
    
    MUPnPAVDeviceObserver*                      iDeviceObserver; // not owned
    
    CUPnPAVConnectionActive*                    iConnectionActive; // owned
    
    RUPnPAVControllerClient                     iServer;
    
    RPointerArray<CUPnPAVBrowsingSessionImpl>   iBrowsingSessions;
    
    RPointerArray<CUPnPAVRenderingSessionImpl>  iRenderingSessions;

    RPointerArray<CUPnPFileUploadSessionImpl>   iUploadSessions;
    
    RPointerArray<CUPnPFileDownloadSessionImpl> iDownloadSessions;
    
    TInt                                        iRespBufSize;           
    TPckg<TInt>                                 iRespBufSizePkg;
    
    TAVControllerDeviceDiscovery                iDiscovered;
    TPckg<TAVControllerDeviceDiscovery>         iDiscoveredPkg;
    
    TBool iAlive;
    
    CActiveSchedulerWait                        iWait;
    
    TReleaseState                               iReleaseState;

    };

#endif // C_UPNPAVCONTROLLERACTIVE_H
