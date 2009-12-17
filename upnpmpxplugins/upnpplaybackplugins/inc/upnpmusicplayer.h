/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Plugin for playing files in a remote upnp renderer
*
*/






#ifndef __CUPNPMUSICPLAYER_H__
#define __CUPNPMUSICPLAYER_H__

// INCLUDES
#include <badesca.h>
#include <mpxplaybackplugin.h> // base class
#include "upnpavrenderingsessionobserver.h" // Avcontrol callback
#include "upnprendererselectorobserver.h" // for r.s.observer
#include "upnptrackobserver.h" // for track observer
#include "upnpaudiopolicy.h" // for audio policy observer

// FORWARD DECLARATIONS
class MUPnPAVController;
class CUPnPSingleton;
class MUPnPAVRenderingSession;
class CUpnpAVDevice;
class CUPnPTrack;
class CUPnPPlaybackStateMachine;
class CUPnPValueStateMachine;

// CONSTANTS
const TInt KMaxDeviceFriendlyName = 256; // from DLNA

/**
 * Plugin for playing files in a remote upnp renderer.
 *
 * This class implements a CPlayerPlugin that plays
 * both local and remote files in a remote UPnP
 * renderer. The class uses UPnP framework's AVControl
 * resource to execute UPnP commands to achieve this.
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPMusicPlayer : public CMPXPlaybackPlugin,
                         public MUPnPAVRenderingSessionObserver,
                         public MUPnPRendererSelectorObserver,
                         public MUPnPTrackObserver,
                         public MUPnPAudioPolicyObserver
    {

public:
    
    /**
     * Static 1st phase constructor.
     * 
     * @since Series 60 3.1
     * @param aObs reference to call back interface
     * @return A new player instance
     */   
    static CUPnPMusicPlayer* NewL( MMPXPlaybackPluginObserver& aObs );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */   
    ~CUPnPMusicPlayer();
    
    /**
     * Compares renderers names
     *
     * @since Series 60 3.1
     * @param aRenderer current renderer to be compared
     * @return TBool ETrue if match EFalse if not
     */ 
    TBool MatchRenderer( const CUpnpAVDevice* aRenderer ) const;
    
    /**
     * Returns currently used renderer device
     *
     * @since Series 60 3.1
     * @return CUpnpAVDevice currently used device
     */ 
    CUpnpAVDevice& UsedRendererDevice() const;
    
    /**
     * Set Plugin state to active
     *
     * @param aForceInitialise
     * @since Series 60 3.1
     */
    void SetActive( TBool aForceInitialise );

    /**
     * Public access to the plugin's observer
     */
    MMPXPlaybackPluginObserver& Observer() const;

    /**
     * Public access to the playback state machine
     */
    const CUPnPPlaybackStateMachine& PlaybackStateMachine() const;

    /**
     * Public access to the value state machine
     */
    const CUPnPValueStateMachine& ValueStateMachine() const;

    /**
     * Public access to the track
     */
    CUPnPTrack& Track() const;

    /**
     * State handling when playback has started.
     * Initiates song duration query.
     */
    void HandlePlayStarted();

    /**
     * State handling when playback is complete.
     * If there is another instance of plugin waiting, activates it.
     */
    void HandlePlayComplete();


public: // datatypes

    /**
     * Defines internal player state
     * 
     * EStateUninitialised          - The plugin is in initial state and
     *                                can not be used
     * EStateUninitialising         - Uninitialise called.
     * EStateInitializing           - Initialise for track is called
     * EStateActive                 - Initialise is complete, plugin is
     *                                readyto play
     * EStatePreInitializing        - Plugin is doing initialisation to 
     *                                the renderer by using
     *                                SetNextAVTransportURI command.
     * EStatePreInitialized         - The plugin pre-initialisation phase
     *                                iscomplete, but plugin is not yet 
     *                                ready to play
     * EStateWaiting                - Another instance of the plugin is
     *                                active. This plugin is just waiting
     *                                for activation
     * EStateActiveForceInitialise  - An instance that was waiting is
     *                                activated, and is indicating that
     *                                is in working state. However when
     *                                it receives Play command, it will
     *                                do normal initialisation
     *
     * EStatePauseInActiveForceInitialise
     *                              - Plugin is on
     *                                EStateActiveForceInitialise state
     *                                and pause is called. This case 
     *                                happens when user skip to the next
     *                                track when playback is paused.
     *
     * EStateError                  - connection failure. In this state
     *                                the plugin has resources freed and
     *                                does not accept any requests or
     *                                notifications. It will ignore all
     *                                other commands except close
     *                                
     * @since Series 60 3.1
     */   
    enum TPlayerState
        {
        EStateNone = 0,
        EStateUninitialised,
        EStateUninitialising,
        EStateInitializing,
        EStateActive,
        EStatePreInitializing,
        EStatePreInitialized,
        EStateWaiting,
        EStateActiveForceInitialise,
        EStatePauseInActiveForceInitialise,
        EStateError
        };
        
private:

    /**
     * Private default constructor
     *
     * @since Series 60 3.1
     * @param aObs reference to call back interface
     */   
    CUPnPMusicPlayer( MMPXPlaybackPluginObserver& aObs );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */   
    void ConstructL();
    
    /**
     * Initializes a remote or local song for playback. 
     *
     * @since Series 60 3.1
     * @param aSong URI of initialised song
     */ 
    void InitialiseTrackL( const TDesC& aSong );

    /**
     * Uninitialises all local resources
     * @param aToState the target player state
     *        (either EStateUninitialised or EStateError)
     */
    void UninitialiseTrack( TPlayerState aToState = EStateUninitialised );

    /**
     * Set URI of track to be playing
     *
     * @since Series 60 3.1
     */
    void SetURIL();  
    
    /**
     * Changes state internally
     * @param aNewState a new plugin state
     *
     * @since Series 60 3.1
     */
    void ChangeState( TPlayerState aNewState );

    /**
     * A string representing the plugin state
     * @param aState a state
     * @return a descriptive string
     */
    const TDesC* State( TPlayerState aState ) const;

public: // From CMPXPlaybackPlugin

    /**
     * See CMPXPlaybackPlugin
     */
    void InitialiseL( const TDesC& aSong );
    
    /**
     * See CMPXPlaybackPlugin
     */
    void InitialiseL( RFile& aSong );

    /**
     * See CMPXPlaybackPlugin
     */
    void CommandL( TMPXPlaybackCommand aCmd, TInt aData );

    /**
     * See CMPXPlaybackPlugin
     */
    void SetL( TMPXPlaybackProperty aProperty, TInt aValue ); 

    /**
     * See CMPXPlaybackPlugin
     */
    void ValueL( TMPXPlaybackProperty aProperty ) const;
            
    /**
     * See CMPXPlaybackPlugin
     */
    void SubPlayerNamesL();
    
    /**
     * See CMPXPlaybackPlugin
     */
    void SelectSubPlayerL(TInt aIndex) ;
    
    /**
     * See CMPXPlaybackPlugin
     */
    const TDesC& SubPlayerName();
    
    /**
     * See CMPXPlaybackPlugin
     */
    TInt SubPlayerIndex() const;
    
    /**
     * See CMPXPlaybackPlugin
     */
    void MediaL( const TArray<TMPXAttribute>& aAttrs );
    
    /**
     * See CMPXPlaybackPlugin
     */
    void CancelRequest();

protected: // From MUPnPAVRenderingSessionObserver

    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void VolumeResult( TInt aError, TInt aVolumeLevel,
        TBool aActionResponse );

    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void MuteResult( TInt aError, TBool aMute, TBool aActionResponse );

    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void InteractOperationComplete( TInt aErrorCode, 
        TUPnPAVInteractOperation aOperation );

    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void PositionInfoResult( TInt aStatus, const TDesC8& aTrackPosition,
        const TDesC8& aTrackLength ); 
    
    /**
     * See MUPnPAVRenderingSessionObserver
     */ 
    void SetURIResult( TInt aError );
    
    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void SetNextURIResult( TInt aError );
    
    /**
     * See MUPnPAVRenderingSessionObserver
     */
    void MediaRendererDisappeared( TUPnPDeviceDisconnectedReason aReason );
    
protected: // From MUPnPAVSessionObserverBase

    /**
     * See MUPnPAVSessionObserverBase
     */    
    void ReserveLocalMSServicesCompleted( TInt aError );

protected: // From MUPnPRendererSelectorObserver

    /**
     * See MUPnPRendererSelectorObserver
     */
    void HandleSubPlayerNames( const MDesCArray* aSubPlayers, 
        TBool aComplete, TInt aError );
    
    /**
     * See MUPnPRendererSelectorObserver
     */
    void RendererListChanged();

protected: // From MUPnPTrackObserver

    /**
     * See MUPnPTrackObserver
     */
    void ResolveURIComplete( TInt aError );

protected: // From MUPnPAudioPolicyObserver

    /**
     * See MUPnPAudioPolicyObserver
     */
    void AudioConflict( TInt aError );

private:
    
    /**
     * Pointer to the plugin that is 
     * active at the time
     */
    static CUPnPMusicPlayer*            iActivePlugIn;
    
    /**
     * Pointer to the plugini that is
     * waiting to activate (pre-initialized)
     */
    static CUPnPMusicPlayer*            iNextPlugIn;
         
    /**
     * The singleton for AVController resources
     */    
    CUPnPSingleton*                     iSingleton;
    
    /**
     * Pointer to renderer session
     */
    MUPnPAVRenderingSession*            iRendererSession;
     
    /**
     * Pointer to playback state machine
     */
    CUPnPPlaybackStateMachine*          iPlaybackStateMachine;

    /**
     * Pointer to value state machine
     */
    CUPnPValueStateMachine*             iValueStateMachine;
                      
    /**
     * UPnP track
     */            
    CUPnPTrack*                         iTrack;

    /**
     * UPnP Audio policy handler
     */
    CUPnPAudioPolicy*                   iAudioPolicy;

    /**
     * The currently active device. OWNED
     */
    CUpnpAVDevice*                      iSelectedRenderer;

    /**
     * The currently active device
     */
    TBuf<KMaxDeviceFriendlyName>        iSelectedRendererName;

    /**
     * The currently active device index
     */
    TInt                                iSelectedRendererIndex;

    /**
     * Current player state
     */
    TPlayerState                        iPlayerState;
    
    /**
     * Tells is remote device ready to give playback information
     * (volume, duration and position).
     */
    TBool                               iIsReady;

    };

#endif // __CUPNPMUSICPLAYER_H__

