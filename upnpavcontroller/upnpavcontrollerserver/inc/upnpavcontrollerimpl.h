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
* Description:      AV Controller Implementation
*
*/







#ifndef C_UPNPAVCONTROLLERIMPL_H
#define C_UPNPAVCONTROLLERIMPL_H

// INDLUDE FILES
#include <e32base.h>
#include <upnpavcontrolpointobserver.h>
#include <upnpmediaserverclient.h>
#include "upnpavcontrollerglobals.h"
#include "upnpconnectionmonitorobserver.h"

// FORWARD DECLARATIONS
class MUPnPAVDeviceObserver;
class MUPnPAVMediaObserver;
class MUPnPAVFileObserver;
class CUPnPPlaybackSession;
class CUpnpAVControllerServer;
class CUpnpAVDevice;
class CUpnpHttpMessage;
class CUpnpDevice;
class CUpnpAVDeviceExtended;
class CUPnPBrowsingSession;
class CUpnpDeviceDiscoveryMessage;

class CUPnPUploadSession;
class CUPnPDownloadSession;

/**
 *  AV Controller server side implementation. Impmements the base session. 
 *  Rendering and browsing sessions are created from this session.
 *  Handles device discovery and fetching of device listst.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVControllerImpl :   public CBase,
                                public MUPnPConnectionMonitorObserver
                                
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aControlPoint control point reference
     * @param aClient media server client reference
     * @param aDispatcher callback dispatcher reference
     * @param aServer server class reference
     */
    static CUPnPAVControllerImpl* NewL
        (
        RUpnpMediaServerClient& aClient,
        CUpnpAVControllerServer& aServer
        );
    
    /**
     * Destructor
     */
    virtual ~CUPnPAVControllerImpl();
    
private:

    /**
     * Private constructor
     *
     * @param aControlPoint control point reference
     * @param aClient media server client reference
     * @param aDispatcher callback dispatcher reference
     * @param aServer server class reference          
     */
    CUPnPAVControllerImpl
    (
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer
    );    
    
    /**
     * Destructor
     */
    void ConstructL();    

public: // From MUPnPConnectionMonitorObserver
    
    /**
     * See upnpconnectionmonitorobserver.h
     */
    void ConnectionLost();
    
public: // New functions

    /**
     * Handles UPnP device discoveries.
     * @since Series 60 2.6
     * @param aDevice Device that is discovered.
     */
    void DeviceDiscoveredL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Handles UPnP device disappears.
     * @since Series 60 2.6
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );
    

    /**
     * Enables device discovery by storing a message to server side, which
     * is completed when a device has been discovered.
     *
     * @param aMessage message
     */
    void EnableDeviceDiscoveryL( const RMessage2& aMessage );
        
    /**
     * Disables (cancels) device discovery (and message).
     */
    void DisableDeviceDiscoveryL();
    
    /**
     * Returns a discovered/disappeared device to client side.
     *
     * @param aMessage message
     */
    void GetDeviceL( const RMessage2& aMessage );
    
    /**
     * Returns the size of device list to client side.
     *
     * @param aMessage message
     */
    void GetDeviceListSizeL( const RMessage2& aMessage );
    
    /**
     * Returns the device list to client side.
     *
     * @param aMessage message
     */
    void GetDeviceListL( const RMessage2& aMessage );
    
    /**
     * Creates a rendering session.
     *
     * @param aMessage message
     */
    void CreateRenderingSessionL( const RMessage2& aMessage );
    
    /**
     * Destroys a rendering session
     *
     * @param aMessage message
     */
    void DestroyRenderingSessionL( const RMessage2& aMessage );
    
    /**
     * Enables (unsolicited) eventing from a remote device. Stores
     * the message in to redering session to return an event.
     *
     * @param aMessage message
     */
    void EventRequestL( const RMessage2& aMessage );
    
    /**
     * Cancels eventing message.
     *
     * @param aMessage message
     */
    void CancelEventRequestL( const RMessage2& aMessage );
    
    /**
     * Sets URI
     *
     * @param aMessage message
     */
    void SetURIL( const RMessage2& aMessage );
    
    /**
     * Cancels SetURI (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelSetURIL( const RMessage2& aMessage );
    
    /**
     * Sets Next URI
     *
     * @param aMessage message
     */
    void SetNextURIL( const RMessage2& aMessage );
    
    /**
     * Cancels SetNextURI (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelSetNextURIL( const RMessage2& aMessage );
    
    /**
     * Send the play-action
     *
     * @param aMessage message
     */
    void PlayL( const RMessage2& aMessage );
    
    /**
     * Cancels the play-action (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelPlayL( const RMessage2& aMessage );

    /**
     * Send the stop-action
     *
     * @param aMessage message
     */
    void StopL( const RMessage2& aMessage );
    
    /**
     * Cancels stop (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelStopL( const RMessage2& aMessage );

    /**
     * Send the pause-action
     *
     * @param aMessage message
     */
    void PauseL( const RMessage2& aMessage );
    
    /**
     * Cancels pause (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelPauseL( const RMessage2& aMessage );

    /**
     * Send the setvolume-action
     *
     * @param aMessage message
     */
    void SetVolumeL( const RMessage2& aMessage );
    
    /**
     * Cancels setvolume (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelSetVolumeL( const RMessage2& aMessage );
    
    /**
     * Send the getvolume-action
     *
     * @param aMessage message
     */
    void GetVolumeL( const RMessage2& aMessage );
    
    /**
     * Cancels getvolume (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelGetVolumeL( const RMessage2& aMessage );
    
    /**
     * Send the setmute-action
     *
     * @param aMessage message
     */
    void SetMuteL( const RMessage2& aMessage );
    
    /**
     * Cancels setmute (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelSetMuteL( const RMessage2& aMessage );
    
    /**
     * Send the getmute-action
     *
     * @param aMessage message
     */
    void GetMuteL( const RMessage2& aMessage );
    
    /**
     * Cancels getmute (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelGetMuteL( const RMessage2& aMessage );
    
    /**
     * Send the getpositioninfo-action
     *
     * @param aMessage message
     */
    void GetPositionInfoL( const RMessage2& aMessage );
    
    /**
     * Cancels getpositioninfo (basically just ignores the result)
     *
     * @param aMessage message
     */
    void CancelGetPositionInfoL( const RMessage2& aMessage );
    
    /**
     * Create a browsing session
     *
     * @param aMessage message
     */
    void CreateBrowsingSessionL( const RMessage2& aMessage );
    
    /**
     * Destroy a browsing session
     *
     * @param aMessage message
     */
    void DestroyBrowsingSessionL( const RMessage2& aMessage );
    
    /**
     * Get browse response (return the size of it to client side)
     *
     * @param aMessage message
     */
    void GetBrowseResponseSizeL( const RMessage2& aMessage );  

    /**
     * Cancel get browse response (ignore result)
     *
     * @param aMessage message
     */
    void CancelGetBrowseResponseSizeL( const RMessage2& aMessage );

    /**
     * Return browse response to client side
     *
     * @param aMessage message
     */
    void GetBrowseResponseL( const RMessage2& aMessage );

    /**
     * Get search response (return the size of it to client side)
     *
     * @param aMessage message
     */
    void GetSearchResponseSizeL( const RMessage2& aMessage );

    /**
     * Cancel search response (ignore result)
     *
     * @param aMessage message
     */
    void CancelGetSearchResponseSizeL( const RMessage2& aMessage );

    /**
     * Return search response to client side
     *
     * @param aMessage message
     */
    void GetSearchResponseL( const RMessage2& aMessage );
    
    /**
     * Get search capabilities (return the size of it to client side)
     *
     * @param aMessage message
     */
    void GetSearchCapabitiesSizeL( const RMessage2& aMessage );
    
    /**
     * Cancel get search capabilities (ignore result)
     *
     * @param aMessage message
     */
    void CancelGetSearchCapabitiesSizeL( const RMessage2& aMessage );
    
    /**
     * Return search capabilities to client side
     *
     * @param aMessage message
     */
    void GetSearchCapabitiesL( const RMessage2& aMessage );

    /**
     * Create container action.
     *
     * @param aMessage message
     */
    void CreateContainerL( const RMessage2& aMessage );
    
    /**
     * Cancels create container.
     *
     * @param aMessage message
     */
    void CancelCreateContainerL( const RMessage2& aMessage );
    
    /**
     * Delete object action
     *
     * @param aMessage message
     */
    void DeleteObjectL( const RMessage2& aMessage );
    
    /**
     * Cancels delete object
     *
     * @param aMessage message
     */
    void CancelDeleteObjectL( const RMessage2& aMessage );

    /**
     * Stores a message in to rendering or browsing session, which is
     * completed when the session specific device has disappeared. As a
     * result the client knows that the device is no longer available and
     * the session has became absolete.
     *
     * @param aMessage message
     */
    void DeviceDisappearedRequestL( const RMessage2& aMessage );
    
    /**
     * Cancels the msg.
     *
     * @param aMessage message
     */
    void CancelDeviceDisappearedRequestL( const RMessage2& aMessage );
    
    /**
     * Stores a message in to base session, which is completed when the
     * WLAN is disconnected.
     *
     * @param aMessage message
     */
    void MonitorConnectionL( const RMessage2& aMessage );
    
    /**
     * Cancels the msg.
     *
     * @param aMessage message
     */
    void CancelMonitorConnectionL( const RMessage2& aMessage );

    void CreateDownloadSessionL( const RMessage2& aMessage );
    
    void DestroyDownloadSessionL( const RMessage2& aMessage );

    void StartDownloadL( const RMessage2& aMessage );
    
    void StartDownloadFHL( const RMessage2& aMessage );
    
    void CancelDownloadL( const RMessage2& aMessage );

    void CancelAllDownloadsL( const RMessage2& aMessage );
    
    void StartTrackingDownloadProgressL( const RMessage2& aMessage );

    void StopTrackingDownloadProgressL( const RMessage2& aMessage );

    void GetDownloadEventL( const RMessage2& aMessage );
    
    void CancelGetDownloadEventL( const RMessage2& aMessage );

    void CreateUploadSessionL( const RMessage2& aMessage );
    
    void DestroyUploadSessionL( const RMessage2& aMessage );    

    void StartUploadL( const RMessage2& aMessage );
    
    void CancelUploadL( const RMessage2& aMessage );

    void CancelAllUploadsL( const RMessage2& aMessage );
    
    void StartTrackingUploadProgressL( const RMessage2& aMessage );

    void StopTrackingUploadProgressL( const RMessage2& aMessage );

    void GetUploadEventL( const RMessage2& aMessage );
    
    void CancelGetUploadEventL( const RMessage2& aMessage );

private:

    /**
     * Device queu handling. Checks the queu and dequeus it if needed.
     *
     * @param aDevice av device
     * @param aType discovered/disappeared
     */
    void DequeDeviceL( const CUpnpAVDevice& aDevice,
        TAVControllerDeviceDiscovery aType );

private:
    
    RUpnpMediaServerClient&     iMediaServer; // Not own       
    
    CUpnpAVControllerServer&    iServer; // Not own       
    
    RMessage2*                  iDeviceDiscoveryMsg; // Own
    
    TBool                       iDeviceDiscoveryEnabled;
    
    RMessage2*                  iConnectionMsg; // Own
    
    HBufC8*                     iDeviceRespBuf; // Own
    
    HBufC8*                     iDeviceListRespBuf; // Own
    
    RPointerArray<CUPnPPlaybackSession> iPlaybackSessions; // Own
    
    RPointerArray<CUPnPBrowsingSession> iBrowsingSessions; // Own
    
    RPointerArray<CUPnPUploadSession>   iUploadSessions; // Own
    
    RPointerArray<CUPnPDownloadSession> iDownloadSessions; // Own
    
    
    TSglQue<CUpnpDeviceDiscoveryMessage>        iDeviceMsgQue; // Own
    TSglQueIter<CUpnpDeviceDiscoveryMessage>    iDeviceMsgQueIter;
    };

#endif // C_UPNPAVCONTROLLERIMPL_H
