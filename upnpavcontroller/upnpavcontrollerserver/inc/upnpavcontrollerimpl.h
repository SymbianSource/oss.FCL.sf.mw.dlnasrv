/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "upnpavcontrollerglobals.h"

// FORWARD DECLARATIONS

class CUPnPPlaybackSession;
class CUpnpAVControllerServer;
class CUpnpAVDevice;
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
 *  @exe upnpavcontrollerserver.exe
 *  @since S60 v3.1
 */
class CUPnPAVControllerImpl : public CBase
                                
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aClient media server client reference
     * @param aServer server class reference
     */
    static CUPnPAVControllerImpl* NewL
        (
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
     * @param aClient media server client reference
     * @param aServer server class reference          
     */
    CUPnPAVControllerImpl( CUpnpAVControllerServer& aServer );    
    
    /**
     * Destructor
     */
    void ConstructL();    

public: // New functions

    /**
     * Handles connection lost.
     */
    void ConnectionLost();
    
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
     * Handles UPnP device icon download completions.
     * @param aDevice Device that's icon was downloaded.
     */
    void DeviceIconDownloadedL( CUpnpAVDeviceExtended& aDevice );

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
     * Returns the icon to client side.
     *
     * @param aMessage message
     */
    void GetDeviceIconRequestL( const RMessage2& aMessage );

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
     * Send the seek action with unit REL_TIME.
     *
     * @param aMessage message
     */
    void SeekRelTimeL( const RMessage2& aMessage );
        
    /**
     * Get initial state of the renderer
     *
     * @param aMessage message
     */
    void GetRendererStateL( const RMessage2& aMessage );
    
    /**
     * Cancels seeking with unit REL_TIME (basically just ignores the result).
     *
     * @param aMessage message
     */
    void CancelSeekRelTimeL( const RMessage2& aMessage );
    
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
     * Cancels the connection monitoring message.
     *
     * @param aMessage message
     */
    void CancelMonitorConnectionL( const RMessage2& aMessage );
    
    /**
     * Create a download session
     *
     * @param aMessage message
     */
    void CreateDownloadSessionL( const RMessage2& aMessage );
    
    /**
     * Destroy the download session
     *
     * @param aMessage message
     */
    void DestroyDownloadSessionL( const RMessage2& aMessage );
    
    /**
     * Start download
     *
     * @param aMessage message
     */
    void StartDownloadL( const RMessage2& aMessage );
    
    /**
     * Start FHL download.
     *
     * @param aMessage message
     */
    void StartDownloadFHL( const RMessage2& aMessage );
    
    /**
     * Cancel download
     *
     * @param aMessage message
     */
    void CancelDownloadL( const RMessage2& aMessage );
    
    /**
     * Cancel all download
     *
     * @param aMessage message
     */
    void CancelAllDownloadsL( const RMessage2& aMessage );
    
    /**
     * Start tracking the download progress
     *
     * @param aMessage message
     */
    void StartTrackingDownloadProgressL( const RMessage2& aMessage );
    
    /**
     * Stop tracking the download progress
     *
     * @param aMessage message
     */
    void StopTrackingDownloadProgressL( const RMessage2& aMessage );
    
    /**
     * Get download event
     *
     * @param aMessage message
     */
    void GetDownloadEventL( const RMessage2& aMessage );
    
    /**
     * Cancel download event
     *
     * @param aMessage message
     */
    void CancelGetDownloadEventL( const RMessage2& aMessage );

    /**
     * Create upload session
     *
     * @param aMessage message
     */
    void CreateUploadSessionL( const RMessage2& aMessage );
    
    /**
     * Destroy upload session
     *
     * @param aMessage message
     */
    void DestroyUploadSessionL( const RMessage2& aMessage );    
    
    /**
     * Start upload
     *
     * @param aMessage message
     */
    void StartUploadL( const RMessage2& aMessage );
    
    /**
     * Cancel upload
     *
     * @param aMessage message
     */
    void CancelUploadL( const RMessage2& aMessage );
    
    /**
     * Cancel all upload
     *
     * @param aMessage message
     */
    void CancelAllUploadsL( const RMessage2& aMessage );
    
    /**
     * Start tracking the upload progress
     *
     * @param aMessage message
     */
    void StartTrackingUploadProgressL( const RMessage2& aMessage );
    
    /**
     * Stop tracking upload progress
     *
     * @param aMessage message
     */
    void StopTrackingUploadProgressL( const RMessage2& aMessage );
    
    /**
     * Get upload event
     *
     * @param aMessage message
     */
    void GetUploadEventL( const RMessage2& aMessage );
    
    /**
     * Cancel get upload event
     *
     * @param aMessage message
     */
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
