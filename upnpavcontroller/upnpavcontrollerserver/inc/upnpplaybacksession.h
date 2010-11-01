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
* Description:      implements playback state machinery for media renderer
*
*/

#ifndef C_UPNPPLAYBACKSESSION_H
#define C_UPNPPLAYBACKSESSION_H

// INCLUDE FILES
// System
#include <e32base.h>

// dlnasrv / avcontroller api
#include "upnpavcontroller.h"
#include "upnpavrenderingsessionobserver.h"
#include "upnpavtevent.h"

// dlnasrv / avcontroller server internal
#include "upnpavtimer.h"
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpObject;
class CUpnpDevice;
class CUpnpAVRequest;
class CUpnpAVDeviceExtended;
class TUnsolicitedEventC;
class CUPnPXMLEventParser;
class CUpnpAVControllerServer;
class CUpnpAttribute;

/**
 * Implements the server side playback session. Provides functionality to
 * Set URI, control playback and to get position info.
 *
 * @since S60 v3.1
 */
class CUPnPPlaybackSession : public CBase,
                             private MUpnpAVControlPointObserver,
                             private MUPnPAVTimerCallback
    {

private:

    /**
     * Defines current playback state.
     */
    enum TPlaybackState
        {
        EUninitialized = -1,
        ENoMedia,
        EStopped,
        EPlaying,
        EPaused,
        EHalted // Go to this state if rendering is interrupted by another CP
        };
  
    /**
     * Defines the event we are expecting to get.
     */
    enum TExpectedEvent
        {
        EEventNone = 0,
        EEventStopped,
        EEventAVTransportUri,
        EEventPlaying,
        EEventPaused
        };
    
    /**
     * Defines current mute state.
     */
    enum TMuteState
        {
        EUnknown    = -1,
        ENotMuted   = 0,
        EMuted      = 1
        };    

public:

    /**
     * Static 1st phase constructor.
     *
     * @param aControlPoint AV Control Point reference
     * @param aDispatcher observer callback disparcher reference
     * @param aRepository device repository
     * @param aSessionId session id
     * @param aUuid device Uuid
     * @return new instance
     */
    static CUPnPPlaybackSession* NewL
        (
        CUpnpAVControllerServer& aServer,
        TInt aSessionId,   
        const TDesC8& aUuid
        );
    
    /**
     * Destructor
     */
    virtual ~CUPnPPlaybackSession();
    
private:

    /**
     * Private constructor.
     *
     * @param aControlPoint AV Control Point reference
     * @param aDispatcher observer callback disparcher reference
     * @param aRepository device repository
     * @param aSessionId session id
     * @param aUuid device Uuid
     */
    CUPnPPlaybackSession
        (
        CUpnpAVControllerServer& aServer,
        TInt aSessionId
        );    
    
    /**
     * 2nd phase construct
     *
     * @param aUuid device Uuid
     */
    void ConstructL( const TDesC8& aUuid );
  
private: // From MUpnpAVControlPointObserver

    void ActionResponseL(CUpnpAction* aAction );
    void StateUpdatedL(CUpnpService* aService);
    void HttpResponseL(CUpnpHttpMessage* aMessage);
    void DeviceDiscoveredL(CUpnpDevice* aDevice);
    void DeviceDisappearedL(CUpnpDevice* aDevice);

private: // network event handling

    /**
     * Handles response for RenderingControl SetVolume command
     */
    void RcSetVolumeResponse(
        TInt aErr, 
        const TDesC8& aDesiredVolume );

    /**
     * Handles response for RenderingControl GetVolume command
     */
    void RcVolumeResponse(
        TInt aErr, 
        const TDesC8& aCurrentVolume);

    /**
     * Handles response for RenderingControl SetMute command
     */
    void RcSetMuteResponse(
        TInt aErr, 
        const TDesC8& aDesiredMute );

    /**
     * Handles response for RenderingControl GetMute command
     */
    void RcMuteResponse(
        TInt aErr, 
        const TDesC8& aCurrentMute );

    /**
     * Handles response for AVTransport SetTransportURI command
     */
    void AvtSetTransportUriResponse(
        TInt aErr );

    /**
     * Handles response for AVTransport GetMediaInfo command
     */
    void AvtGetMediaInfoResponse(
        TInt aErr,
        const TDesC8& aCurrentURI );
    
    /**
     * Handles response for AVTransport GetTransportInfo command
     */
    void AvtGetTransportInfoResponse(
        TInt aErr,
        const TDesC8& aCurrenTransportState );

    /**
     * Handles response for AVTransport GetPositionInfo command
     */
    void AvtPositionInfoResponse(
        TInt aErr,
        const TDesC8& aTrackDuration,
        const TDesC8& aRelTime );

    /**
     * Handles response for AVTransport Stop command
     */
    void AvtStopResponse(
        TInt aErr );

    /**
     * Handles response for AVTransport Play command
     */
    void AvtPlayResponse(
        TInt aErr );

    /**
     * Handles response for AVTransport Pause command
     */
    void AvtPauseResponse(
        TInt aErr );

    /**
     * Handles response for AVTransport Seek command
     */
    void AvtSeekResponse(
        TInt aErr );

    /**
     * Handles response for ConnectionManager Prepare command
     */
    void CmPrepareResponse(
        TInt aErr,
        TInt aConnection,
        TInt aTransport,
        TInt aRsc );

    /**
     * Processes a state change in RenderingControl
     */
    void RcLastChangeEvent(
        const TDesC8& aLastChange );

    /**
     * Processes a state change in AVTransport
     */
    void AvtLastChangeEvent(
        const TDesC8& aLastChange );


private: // From MUPnPAVTimerCallback
    
    // see upnpavtimer.h
    void UPnPAVTimerCallback( CUPnPAVTimer::TAVTimerType aType );
    

public: // New functions
    
    /**
     * Device disappeared callback
     *
     * @param aDevice disappeared device
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );
    
    /**
     * Sets Uuid of the local Media Server
     *
     * @param aUuid device Uuid
     */
    void SetLocalMSUuidL( const TDesC8& aUuid );

    /**
     * Returns session id
     *
     * @return session id
     */
    TInt SessionId() const;
        
    /**
     * See upnpavcontrollerimpl.h
     */
    void EventRequestL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelEventRequestL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void SetURIL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelSetURIL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void SetNextURIL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelSetNextURIL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void PlayL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelPlayL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void StopL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelStopL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void PauseL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelPauseL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void SetVolumeL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelSetVolumeL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetVolumeL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetVolumeL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void SetMuteL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelSetMuteL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetMuteL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetMuteL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetPositionInfoL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetPositionInfoL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void SeekRelTimeL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelSeekRelTimeL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetRendererStateL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void DeviceDisappearedRequestL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDeviceDisappearedRequestL();        
    
    /**
     * Returns device Uuid
     */
    const TDesC8& Uuid() const;     
    
private:

    /**
     * Encodes given xml-document
     *
     * @param aResult xml-document
     * @return encoded xml-document
     */
    HBufC8* EncodeXmlL( const TDesC8& aResult );
    
    /**
     * Reset function
     */
    void ResetL();
    
    /**
     * Reads object from a message
     *
     * @param aMessage message from client
     * @param aSlot message slot numer
     * @param aObj object pointer
     */
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpObject* aObj );        

    /**
     * Reads request from a message
     *
     * @param aMessage message from client
     * @param aSlot message slot numer
     * @param aReq request pointer
     */
    void ReadReqFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpAVRequest* aReq );
        
    /**
     * Validates protocolinfo.
     *
     * @param aProtocolInfo
     */
    void ValidateProtocolInfoL( const CUpnpAttribute& aProtocolInfo );        
        
    /**
     * Checks if we have a valid connection with Media Renderer
     *
     * @param aProtocolInfo items protocolinfo
     * @return ETrue if we have a valid connection
     */
    TBool CheckConnectionL( const TDesC8& aProtocolInfo );
    
    /**
     * Sends AVTransportUri-action
     */
    void SendAVTransportUriActionL();
    
    /**
     * removes DNLA.ORG* flags profile id etc. from protocol info of 
     * res element
     */
    void RemoveDlnaFlagsFromResElementsL();
    
    /**
     * removes dlna profile id (DLNA.ORG_PN=*) from protocol info of 
     * res element
     */
    void RemoveDlnaProfileFromResElementsL();

    /**
     * Sends GetMediaInfo-action
     */
    void SendGetMediaInfoActionL();
    
    /**
     * Sends GetAVTransportUri-action
     */
    void SendGetTransportInfoActionL();
        
    /**
     * Handles "PLAYING" event
     */
    void PlayingEventReceived();
    
    /**
     * Handles "STOPPED" event
     */
    void StoppedEventReceived();
    
    /**
     * Handles "PAUSED" event
     */
    void PausedEventReceived();
    
    /**
     * Handles "NO_MEDIA_PRESENT" event
     */
    void NoMediaEventReceived();
    
    /**
     * Handles AVTransportUri event
     */
    void AVTransportUriEventReceived( const TDesC8& aUri,
        CUPnPAVTEvent::TTransportState aTransportState );
    
    /**
     * Propagates an event to the client (complete or queu).
     */
    void PropagateEvent( TUnsolicitedEventC event );
    
    /**
     * Stops possible playback.
     */
    void EmergencyStopL();
    
    /**
     * Returns CUpnpDevice from control point.
     */
    const CUpnpDevice* CpDeviceL();
    
    /**
     * Propagates (transition) state to clients.
     */
    void PropagateState( CUPnPAVTEvent::TTransportState aTransportState );
    
    /**
     * Responds to requested stop command.
     */
    void RespondToStopRequest();
    
private:

    CUpnpAVControllerServer&    iServer;
    
    TInt                        iSessionId;
    
    TInt                        iAVTInstanceId;
    
    TInt                        iRCInstanceId;
    
    TInt                        iConnectionId;
    
    TInt                        iIPSessionIdCommand;
    
    TInt                        iIPSessionIdSetting;

    TBool                       iEventingActive;
    
    RMessage2*                  iCommandMessage; // Own
    
    RMessage2*                  iSettingMessage; // Own
    
    RMessage2*                  iDeviceMessage; // Own
        
    CUpnpAVDeviceExtended*      iDevice;
    
    HBufC8*                     iLocalMediaServerUuid; // Own
        
    RMessage2*                  iEventMessage; // Own
    
    RMessage2*                  iInitialEventMsg; // Own
    
    TPlaybackState              iPlaybackState;
    
    CUPnPXMLEventParser*        iEventParser;
    
    RArray<TUnsolicitedEventC>  iEventQue; // Own
    
    TMuteState                  iMuteState;
    
    TInt                        iVolume;
    
    /**
     * Protocol info for currently played item.    
     * Own.
     */
    CUpnpDlnaProtocolInfo*      iPInfoForPrevious;
    
    /**
     * Uri for currently played item.  
     * Own.
     */
    HBufC8*                     iCurrentUri;
    
    TBool                       iCheckForHijackedRenderer;
    
    /**
     * Currently played item.
     * Own.
     */
    CUpnpItem*                  iCurrentItem;
    
    /**
     * Fail safe timer.
     * Own.
     */
    CUPnPAVTimer*               iTimer;
    
    TExpectedEvent              iExpectedEvent;
     
    TBool                       iInitialEventReceived;
    
    HBufC8*                     iUuid;
    
    CUPnPAVTEvent::TTransportState iPreviousTransportState;
    
    };

#endif // C_UPNPPLAYBACKSESSION_H
