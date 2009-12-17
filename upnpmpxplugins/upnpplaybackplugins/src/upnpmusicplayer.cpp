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






// INCLUDES
#include "upnpavcontrollerfactory.h"
#include "upnpavcontroller.h"
#include "upnpfileutility.h"
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"
#include <mpxplaybackpluginobserver.h>

#include "upnpmusicplayer.h"
#include "upnpsingleton.h"
#include "upnptrack.h"
#include "upnpplaybackstatemachine.h"
#include "upnpvaluestatemachine.h"
#include "upnppluginserrortranslation.h"


// CONSTANTS
// ** a message to music player to complete an immediate exit
const TInt KMusicPluginMessageExit = 3001;
const TUid KMusicPlayerUid = { 0x200075D8 };

_LIT( KStateUninitialized, "Uninitialised" );
_LIT( KStateUninitialising, "Uninitialising" );
_LIT( KStateInitializing, "Initializing" );
_LIT( KStateActive, "Active" );
_LIT( KStatePreInitializing, "PreInitializing" );
_LIT( KStatePreInitialized, "PreInitialized" );
_LIT( KStateWaiting, "Waiting" );
_LIT( KStateActiveForceInit, "ActiveForceInit" );
_LIT( KStateError, "Error" );
_LIT( KStateUnknown, "Unknown" );


_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// LOCAL FUNCTIONS

    /*
     * a template helper function to safely delete a pointer
     * (make sure pointer becomes NULL before delete is called)
     * 1. first copy the given pointer to a local temp variable
     * 2. then allocate a local member, then nullify the given pointer
     * 3. last, delete the temporary pointer
     */
    template <class T>
    inline void SafeDelete( T*& aPointer )
        {
        // first, store the given object into a local temp variable
        T* tempPointer = aPointer;
        // nullify the given pointer
        aPointer = 0;
        // last, delete the given object
        delete tempPointer;
        }

// --------------------------------------------------------------------------
// Static members of CUPnPMusicPlayer
// --------------------------------------------------------------------------
//

// Pointer to the plugin that is active at the time
CUPnPMusicPlayer* CUPnPMusicPlayer::iActivePlugIn;

// Pointer to the plugini that is waiting to activate (pre-initialized)
CUPnPMusicPlayer* CUPnPMusicPlayer::iNextPlugIn;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPMusicPlayer* CUPnPMusicPlayer::NewL( MMPXPlaybackPluginObserver& aObs )
    {
    CUPnPMusicPlayer* p = new(ELeave) CUPnPMusicPlayer( aObs );
    CleanupStack::PushL( p );
    __LOG1( "CUPnPMusicPlayer::NewL [%d]", p );
    p->ConstructL();
    CleanupStack::Pop( p );
    return p;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::CUPnPMusicPlayer
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPMusicPlayer::CUPnPMusicPlayer( MMPXPlaybackPluginObserver& aObs ) :
    iSelectedRendererIndex( KErrNotFound ),
    iPlayerState( EStateUninitialised ),
    iIsReady( EFalse )
    {   
    iObs = &aObs;
    iTrack = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::ConstructL()
    {
    __LOG( "CUPnPMusicPlayer::ConstructL" );

    // get a handle to the shared singleton
    iSingleton = CUPnPSingleton::GetInstanceL();
    
    // audio policy
    iAudioPolicy = CUPnPAudioPolicy::NewL( *this );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::~CUPnPMusicPlayer
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPMusicPlayer::~CUPnPMusicPlayer()
    {
    __LOG( "CUPnPMusicPlayer::~CUPnPMusicPlayer()" );
    UninitialiseTrack( EStateNone );
            
    if( iSingleton != NULL )
        {
        CUPnPSingleton::LoseInstance( iSingleton );
        iSingleton = NULL;
        }

    delete iAudioPolicy;
    iAudioPolicy = 0;
    delete iSelectedRenderer;
    } 

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::MatchRenderer
// Compares renderers names
// --------------------------------------------------------------------------
//
TBool CUPnPMusicPlayer::MatchRenderer(
    const CUpnpAVDevice* aRenderer ) const
    {
    if( aRenderer == NULL  )
        {
        // no renderer - using default renderer, which is always
        // the same as previously selected renderer -> ALWAYS MATCH.
        return ETrue;
        }

    if( iSelectedRenderer == NULL  )
        {
        // Current renderer does not exist - this should never happen.
        __PANICD( __FILE__, __LINE__ );
        return ETrue;
        }

    // compare renderer pointer
    if ( iSelectedRenderer == aRenderer )
        {
        return ETrue;
        }

    // Compare renderer UUID
    if ( iSelectedRenderer->Uuid().Compare( aRenderer->Uuid() ) == 0 )
        {
        return ETrue;
        }
    
    return EFalse;
    }


// --------------------------------------------------------------------------
// CUPnPMusicPlayer::UsedRendererDevice
// Returns currently used renderer device
// --------------------------------------------------------------------------
//
CUpnpAVDevice& CUPnPMusicPlayer::UsedRendererDevice() const
    {
    return *iSelectedRenderer;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SetActive
// Set Plugin state to active.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::SetActive( TBool aForceInitialise )
    {
    __LOG( "CUPnPMusicPlayer::SetActive" );

    if ( iActivePlugIn != 0 &&
        iActivePlugIn->MatchRenderer( iSelectedRenderer ) )
        {
        // Continuing playback on the same renderer.
        // copy some cached values
        // 
        iValueStateMachine->CopyValues(
            iActivePlugIn->ValueStateMachine() );
        }

    if( iPlayerState == EStatePreInitialized ) 
        {
        // Preinitialize is ready. Clear pointer from iNextPlugin and 
        // sets it to iActivePlugIn.
        iActivePlugIn = this;
        iNextPlugIn = NULL;
        if( aForceInitialise )
            {
            ChangeState( EStateActiveForceInitialise );
            TRAP_IGNORE( iAudioPolicy->PlayL() );
            }
        else
            {
            ChangeState( EStateActive );
            TRAP_IGNORE( iAudioPolicy->PlayL() );
            }
        }
    else if( iPlayerState == EStateWaiting )
        {
        // Used renderer does not support SetNextURI feature. Initialise
        // track by SetURI after play command.
        iActivePlugIn = this;
        iNextPlugIn = NULL;
        ChangeState( EStateActiveForceInitialise );
        }
    else if( iPlayerState == EStateActive )
        {
        // do nothing
        iActivePlugIn = this;
        iNextPlugIn = NULL;
        ChangeState( EStateActive );
        TRAP_IGNORE( iAudioPolicy->PlayL() );
        }
    else if( iPlayerState == EStatePreInitializing )
        {
        // Do nothing.
        }
    else
        {
        __LOG1( "CUPnPMusicPlayer::SetActive in wrong state %S",
            State( iPlayerState ) );
        __PANICD( __FILE__, __LINE__ );
        }   
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::Observer
// --------------------------------------------------------------------------
//
MMPXPlaybackPluginObserver& CUPnPMusicPlayer::Observer() const
    {
    __ASSERTD( iObs != 0, __FILE__, __LINE__ );
    return *iObs;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::PlaybackStateMachine
// --------------------------------------------------------------------------
//
const CUPnPPlaybackStateMachine&
    CUPnPMusicPlayer::PlaybackStateMachine() const
    {
    __ASSERTD( iPlaybackStateMachine != 0, __FILE__, __LINE__ );
    return *iPlaybackStateMachine;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ValueStateMachine
// --------------------------------------------------------------------------
//
const CUPnPValueStateMachine&
    CUPnPMusicPlayer::ValueStateMachine() const
    {
    __ASSERTD( iValueStateMachine != 0, __FILE__, __LINE__ );
    return *iValueStateMachine;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::Track
// --------------------------------------------------------------------------
//
CUPnPTrack& CUPnPMusicPlayer::Track() const
    {
    __ASSERTD( iTrack != 0, __FILE__, __LINE__ );
    return *iTrack;
    }


// --------------------------------------------------------------------------
// CUPnPMusicPlayer::HandlePlayStarted
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::HandlePlayStarted()
    {
    __LOG( "CUPnPMusicPlayer::HandlePlayStarted" );

    // Remote device is ready for give playback information 
    // (duration, position..)
    iIsReady = ETrue;

    // Inform user that playback is started
    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlaying, 0,
        KErrNone );

    if( iValueStateMachine && iTrack )
        {
        // always query volume. Duration if not already exist or in 
        // case of remote track.
        TRAP_IGNORE(
            iValueStateMachine->VolumeQueryL();
            if( iTrack->TrackDuration() == 0 || iTrack->IsRemote() )
                {
                iValueStateMachine->DurationQueryL();
                }
            )
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::HandlePlayComplete
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::HandlePlayComplete()
    {
    __LOG( "CUPnPMusicPlayer::HandlePlayComplete" );

    iAudioPolicy->Stop();

    // If there is a next initialized plugin then active it
    if( iActivePlugIn == this && iNextPlugIn != 0 )
        {
        // Set iActivePlugIn to null.
        iNextPlugIn->SetActive( EFalse );   
        }
    }


// --------------------------------------------------------------------------
// Methods from CMPXPlaybackPlugin
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::InitialiseL( const TDesC& aSong )
// Initializes a song for playback.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::InitialiseL( const TDesC& aSong )
    {    
    // Initialise song for play back.
    InitialiseTrackL( aSong );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::InitialiseL(  RFile& aSong )
// Initializes a song for playback.
// --------------------------------------------------------------------------
//   
void CUPnPMusicPlayer::InitialiseL( RFile& aSong )
    {
    // At first get the song path from given resource file
    HBufC* tempBuf = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( tempBuf );
    TPtr ptr = tempBuf->Des();
    TInt err = aSong.FullName( ptr );

    // Leave if any error
    if( err != KErrNone )
        {
        User::Leave( KErrArgument );
        }

    // Initialise song for play back.
    InitialiseTrackL( *tempBuf );
    CleanupStack::PopAndDestroy( tempBuf );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::InitialiseTrackL
// Initializes a remote or local song for playback
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::InitialiseTrackL( const TDesC& aSong )
    {
    __LOG1( "CUPnPMusicPlayer::InitialiseTrackL [%d]", this );
    __ASSERTD( iPlayerState == EStateUninitialised, __FILE__, __LINE__ );

    // some state checks
    if ( iPlayerState != EStateUninitialised )
        {
        __LOG( "CUPnPMusicPlayer::InitialiseTrackL: In wrong state! " );
        User::Leave( KErrNotReady );
        }
    if( ( iActivePlugIn != NULL ) && ( iNextPlugIn != NULL ) )
        {
        __LOG( "CUPnPMusicPlayer::InitialiseTrackL - \
        More than two plugin initialisezed at the same time! " );
        User::Leave( KErrNotReady );
        }
    if( iSelectedRenderer == 0 )
        {
        __LOG( "CUPnPMusicPlayer::InitialiseTrackL - \
        Subplayer not selected " );
        User::Leave( KErrNotReady );
        }
    
    if( aSong.Length() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    
    __LOG( "CUPnPMusicPlayer::InitialiseTrackL: Start rendering session" );
    MUPnPAVRenderingSession* tempSession =
        &iSingleton->AVC().StartRenderingSessionL( *iSelectedRenderer );
        
    __LOG( "CUPnPMusicPlayer::InitialiseTrackL - \
        Set Rendering session observer" );
    // Set media observer
    tempSession->SetObserver( *this );

    // stop existing rendering session if exists
    if( iRendererSession )
        {
        __LOG( "CUPnPMusicPlayer::InitialiseTrackL: Stop old session" );
        // releasing local MS services is left to AVController
        iSingleton->AVC().StopRenderingSession( *iRendererSession );
        iRendererSession = 0;
        }

    iRendererSession = tempSession;
             
    // Set plugin to active and call SetURI if there is no track
    // playing currently or the track playing currently is in 
    // different renderer
    if( iActivePlugIn == NULL ) 
        {
        // There is no track playing currently
        iActivePlugIn = this; // Set Plugin to the active.
        ChangeState( EStateInitializing );
        }
    else // Track is currently playing.
        {
        // Check if track is playing in different renderer
        if( iActivePlugIn->MatchRenderer( iSelectedRenderer ) )
            {
            iNextPlugIn = this; // Set plugin to the next
            ChangeState( EStatePreInitializing );
            }
        else
            {
            // Track is playing but different renderer
            ChangeState( EStateInitializing );
            }
        } 

    // Create state machines
    iPlaybackStateMachine = CUPnPPlaybackStateMachine::NewL(
        *this, *iRendererSession );

    iValueStateMachine = CUPnPValueStateMachine::NewL(
        *this, *iRendererSession );

    // Create track object to resolving URI
    iTrack = CUPnPTrack::NewL( iSingleton->AVC() );
    iTrack->ResolveURIL( aSong, *this, CUPnPTrack::EDirectionRemote );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::UninitialiseTrack
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::UninitialiseTrack( TPlayerState aToState )
    {
    __LOG1( "CUPnPMusicPlayer::UninitialiseTrack(%S)",
        State( aToState ) );
    ChangeState( EStateUninitialising );

    if ( iAudioPolicy )
        {
        iAudioPolicy->Stop();
        }
    
    if ( iActivePlugIn == this )
        {
        iActivePlugIn = 0;
        }
    if ( iNextPlugIn == this )
        {
        iNextPlugIn = 0;
        }

    if( iPlaybackStateMachine )
        {
        delete iPlaybackStateMachine;
        iPlaybackStateMachine = 0;
        }

    if( iValueStateMachine )
        {
        delete iValueStateMachine;
        iValueStateMachine = 0;
        }

    // Rendering session is not cleared until destructor.
    if( iRendererSession && aToState == EStateNone )
        {
        // releasing local MS services is left to AVController
        iSingleton->AVC().StopRenderingSession( *iRendererSession );
        iRendererSession = 0;
        }

    iSelectedRendererIndex = KErrNotFound;
    iIsReady = EFalse;

    if ( iTrack )
        {
        // request track to delete itself
        CUPnPTrack* tempTrack = iTrack;
        iTrack = NULL;
        tempTrack->Delete();
        }

    ChangeState( aToState );
    __LOG( "CUPnPMusicPlayer::UninitialiseTrack - End" );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ResolveURIComplete
// from MUPnPTrackObserver
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::ResolveURIComplete( TInt aError )
    {
    __LOG2( "CUPnPMusicPlayer::ResolveURIComplete in state %S [%d]",
        State( iPlayerState ), this );

    if ( aError == KErrNone )
        {
        // Check is local or remote
        TRAPD( error, SetURIL() );
        if ( error != KErrNone )
            {
            __LOG1("CUPnPMusicPlayer::ResolveURIComplete: leaves with %d",
                error );
            UninitialiseTrack();
            error = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                    error );
            iObs->HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPInitialised,
                0, error );
            }
        }
    else
        {
        __LOG1("CUPnPMusicPlayer::ResolveURIComplete: error %d",
            aError );
        UninitialiseTrack();
        aError = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                aError );
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPInitialised,
            0, aError );
        }
    }


// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SetURIL
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::SetURIL()
    {
    __LOG( "CUPnPMusicPlayer::SetURIL" );

    switch( iPlayerState )
        {
        case EStateInitializing: // flow through
        case EStateActiveForceInitialise:
            {
            iRendererSession->SetURIL(
                iTrack->UpnpURI(), iTrack->UpnpItem() );
            }
            break;
        case EStatePreInitializing:
            {
            // Set NextUri if action is supported by used device
            if( iSelectedRenderer->NextAVTransportUri() )
                {
                __LOG( "CUPnPMusicPlayer::SetURIL - \
                Remote:SetNextURI" );
                
                iRendererSession->SetNextURIL( 
                    iTrack->UpnpURI(), iTrack->UpnpItem() );
                }
            else
                {
                __LOG( "CUPnPMusicPlayer::SetURI - \
                SetNextURI is not supported by used device" );

                // Used renderer does not support SetNextURI feature. 
                // Initialise track by SetURI after play is called.
                ChangeState( EStateWaiting );
                iObs->HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPInitialised,
                    iTrack->TrackDuration(), KErrNone );
                if ( iPlayerState == EStateWaiting )
                    {
                    if( iTrack->IsRemote() )
                        {
                        iTrack->SendMediaChangedEventL( *iObs );
                        }
                    }
                }
            }
            break;
        case EStatePauseInActiveForceInitialise:
            {
            // This case happens when active plugin is paused and user 
            // skips to the next track from UI.
            __LOG( "SetURI during EStatePauseInActiveForceInitialise" );
            iRendererSession->SetURIL(
                iTrack->UpnpURI(), iTrack->UpnpItem() );
            }
            break;
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            }
            break;
        }

    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::CommandL
// Executes a command on the selected song.
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::CommandL( TMPXPlaybackCommand aCmd, TInt aData )
    {
    __LOG3( "CUPnPMusicPlayer::CommandL(%d) in state %S [%d]",
       aCmd, State( iPlayerState ), this );
        
    switch( iPlayerState )
        {
        case EStateUninitialised:   // fall through
        case EStateUninitialising:  // fall through
            {
            // ignore everything
            }
        case EStateError:
            {
            // Only Stop and Close will be handled.
            // All other commands ignored.
            if ( aCmd == EPbCmdStop )
                {
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPStopped,
                    KErrNone, KErrNone );
                }
            if ( aCmd == EPbCmdClose )
                {
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPClosed,
                    KErrNone, KErrNone );
                }
            break;
            }
        case EStateInitializing:    // fall through
        case EStatePreInitializing: // fall through
        case EStatePreInitialized:  // fall through
        case EStateWaiting:         // fall through
        case EStateActiveForceInitialise:
            {
            if( aCmd == EPbCmdStop )
                {
                // answer directly to stopped message
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPStopped,
                    KErrNone, KErrNone );
                }
            if( aCmd == EPbCmdPlay &&
                ( iPlayerState == EStateActiveForceInitialise ) )
                {
                __LOG( "CUPnPMusicPlayer::Command(play)->SetURI" );
                SetURIL();
                }
            if( aCmd == EPbCmdPlay && iPlayerState == EStateWaiting )
                {                
                // This plugin has to be set active because track has forced
                // to be skipped by user.
                SetActive( ETrue );
                SetURIL();
                }
            if( aCmd == EPbCmdPlay && iPlayerState == EStatePreInitialized )
                {
                // This plugin has to be set active because track has forced
                // to be skipped by user.
                SetActive( ETrue );
                SetURIL();            
                }
            if( aCmd == EPbCmdPause && 
                ( iPlayerState == EStateActiveForceInitialise || 
                  iPlayerState == EStateWaiting ) )
                {
                // This case happens when active plugin is paused and user 
                // skips to the next track from UI -> next track must be 
                // switched to paused state.
                __LOG( "Pause command during EStateActiveForceInitialise" );
                iPlayerState = EStatePauseInActiveForceInitialise;
                SetURIL();
                }
                
            if ( aCmd == EPbCmdClose )
                {
                UninitialiseTrack();
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPClosed,
                    KErrNone, KErrNone );
                }
            break;
            }
        case EStateActive:
            {
            iPlaybackStateMachine->CommandL( aCmd );
            if( aCmd == EPbCmdStop )
                {
                if ( this == iActivePlugIn && iNextPlugIn != 0 )
                    {
                    __LOG( "CommandL(stop)->SetActive()" );
                    iNextPlugIn->SetActive( ETrue );
                    }
                }
            if( aCmd == EPbCmdPause )
                {
                if ( this == iActivePlugIn && iNextPlugIn != 0 )
                    {
                    __LOG( "CommandL(pause)->SetActive()" );
                    iNextPlugIn->SetActive( EFalse );
                    }
                }
            if( aCmd == EPbCmdClose )
                {
                if ( this == iActivePlugIn && iNextPlugIn != 0 )
                    {
                    __LOG( "CommandL(close)->SetActive()" );
                    iNextPlugIn->SetActive( ETrue );
                    }
                UninitialiseTrack();
                }
            break;
            }
        default:
            {
            __LOG( "CUPnPMusicPlayer::CommandL - Default" );
            break;
            } 
        }

    // check for messages from upnp framework (aData param)
    if ( aCmd == EPbCmdClose &&
        aData == KMusicPluginMessageExit )
        {
        __LOG( "CommandL(Close, exit) -> cleanup and terminate" );

        // cancel things that are ongoing
        CancelRequest();
        // clear resources
        UninitialiseTrack( EStateError );

        // Create event to user
        iObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPlayerUnavailable,
            0, KErrDisconnected );
        }

    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SetL
// Sets a property of the plugin.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::SetL( TMPXPlaybackProperty aProperty, TInt aValue )
    {
    __LOG2( "CUPnPMusicPlayer::SetL in state %S [%d]",
        State( iPlayerState ), this );

    CUPnPValueStateMachine::TValidationResult result =
        CUPnPValueStateMachine::ValidatePropertyInState(
        aProperty, ETrue, iPlayerState, iIsReady );

    if ( result == CUPnPValueStateMachine::EHandle )
        {
        if ( aProperty == EPbPropertyPosition )
            {
            // Position changes are handled in playback s.m.
            iPlaybackStateMachine->PositionL( aValue );
            }
        else
            {
            // all other values in value s.m.
            iValueStateMachine->SetL( aProperty, aValue );
            }
        }
    else if ( result == CUPnPValueStateMachine::EHandleStatic )
        {
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
            (TInt)aProperty, KErrNone );
        }
    else if ( result == CUPnPValueStateMachine::EErrorNotReady )
        {
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
            (TInt)aProperty, KErrNotReady );
        }
    else if ( result == CUPnPValueStateMachine::EErrorNotSupported )
        {
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
            (TInt)aProperty, KErrNotSupported );
        }
    else if ( result == CUPnPValueStateMachine::EIgnore )
        {
        // do nothing
        }
    else 
        {
        __PANICD( __FILE__, __LINE__ );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ValueL
// Gets a property of the plugin (async).
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::ValueL( TMPXPlaybackProperty aProperty ) const
    {
    __LOG2( "CUPnPMusicPlayer::ValueL in state %S [%d]",
        State( iPlayerState ), this );

    CUPnPValueStateMachine::TValidationResult result =
        CUPnPValueStateMachine::ValidatePropertyInState(
        aProperty, EFalse, iPlayerState, iIsReady );

    if ( result == CUPnPValueStateMachine::EHandle )
        {
        iValueStateMachine->ValueL( aProperty );
        }
    else if ( result == CUPnPValueStateMachine::EHandleStatic )
        {
        CUPnPValueStateMachine::ValueStatic( aProperty, *iObs );
        }
    else if ( result == CUPnPValueStateMachine::EErrorNotReady )
        {
        iObs->HandleProperty( aProperty,
            0, KErrNotReady );
        }
    else if ( result == CUPnPValueStateMachine::EErrorNotSupported )
        {
        iObs->HandleProperty( aProperty,
            0, KErrNotSupported );
        }
    else if ( result == CUPnPValueStateMachine::EIgnore )
        {
        // do nothing
        }
    else 
        {
        __PANICD( __FILE__, __LINE__ );
        }
      
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SubPlayerNamesL
// Gets a list of sub players (async).
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::SubPlayerNamesL()
    {
    __LOG1( "CUPnPMusicPlayer::SubPlayerNamesL [%d]", this );
   
    // Get media renderers from renderer selector
    iSingleton->GetRendererNamesL( *this );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SelectSubPlayerL
// Select a sub player.
// --------------------------------------------------------------------------
//  
void CUPnPMusicPlayer::SelectSubPlayerL( TInt aIndex )
    {
    __LOG1( "CUPnPMusicPlayer::SelectSubPlayerL [%d]", this );
            
    // this method can only be called in uninitialized state
    __ASSERTD( iPlayerState == EStateUninitialised ||
    aIndex == iSelectedRendererIndex, __FILE__, __LINE__ );
 
    // Select media renderer. Ignore if already selected.
    if ( iSelectedRendererIndex != aIndex )
        {
        iSelectedRendererIndex = aIndex;
        CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL( 
            *iSingleton->SelectRendererByIndexL( aIndex ) );
        delete iSelectedRenderer;
        iSelectedRenderer = tempDev;
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SubPlayerName
// Returns current sub player name
// --------------------------------------------------------------------------
// 
const TDesC& CUPnPMusicPlayer::SubPlayerName()
    {
    __LOG1( "CUPnPMusicPlayer::SubPlayerName [%d]", this );
    if ( iSelectedRenderer )
        {
        iSelectedRendererName.Copy(
            iSelectedRenderer->FriendlyName() );
        }
    else if ( iSingleton->DefaultDevice() )
        {
        iSelectedRendererName.Copy(
            iSingleton->DefaultDevice()->FriendlyName() );
        }
    else
        {
        iSelectedRendererName.Copy( KNullDesC );
        }
    return iSelectedRendererName;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SubPlayerIndex
// Return current sub player index.
// --------------------------------------------------------------------------
//    
TInt CUPnPMusicPlayer::SubPlayerIndex() const
    {
    __LOG1( "CUPnPMusicPlayer::SubPlayerIndex [%d]", this );

    // Get current sub player index
    return iSelectedRendererIndex;
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::MediaL
// Extended properties of the current file (async).
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::MediaL( const TArray<TMPXAttribute>& aAttrs )
    {
    __LOG2( "CUPnPMusicPlayer::MediaL in state %S [%d]",
        State( iPlayerState ), this );

    // Get media if track is initialised
    if( iTrack )
        {
        iTrack->GetMetaDataL( aAttrs, *iObs );
        }
    else
        {
        __LOG( "CUPnPMusicPlayer::MediaL - Wrong state!" );
        User::Leave( KErrNotReady );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::CancelRequest
// Cancel outstanding request.
// --------------------------------------------------------------------------
//   
void CUPnPMusicPlayer::CancelRequest()
    {
    __LOG2( "CUPnPMusicPlayer::CancelRequest in state %S [%d]",
        State( iPlayerState ), this );

    // If cancel is called during initialise
    if( iPlayerState == EStateInitializing || 
        iPlayerState == EStatePreInitializing || 
        iPlayerState == EStateActiveForceInitialise ||
        iPlayerState == EStateActive )
        {
        // Stop rendering session -> SetURI operation will be cancelled
        UninitialiseTrack( EStateNone );
        iPlayerState = EStateUninitialised;
        }
    else if( iPlayerState == EStateUninitialising )
        {
        // Ignore. Do not call state machines.
        return;
        }

    if ( iSingleton != 0 )
        {
        iSingleton->CancelGetRendererNames();
        }
    if ( iPlaybackStateMachine )
        {
        iPlaybackStateMachine->Cancel();
        }
    if ( iValueStateMachine )
        {
        iValueStateMachine->Cancel();
        }
    }

// --------------------------------------------------------------------------
// Methods of MUPnPAVRenderingSessionObserver
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::VolumeResult
// Response for get/set volume property commands.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::VolumeResult( TInt aError, TInt aVolumeLevel,
    TBool aActionResponse )
    {
    __LOG2( "CUPnPMusicPlayer::VolumeResult in state %S [%d]",
        State( iPlayerState ), this );

    if( iValueStateMachine )
        {
        iValueStateMachine->VolumeResult( aError, aVolumeLevel,
            aActionResponse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::MuteResult
// Response for get/set mute property commands.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::MuteResult( TInt aError, TBool aMute,
    TBool aActionResponse )
    {
    __LOG2( "CUPnPMusicPlayer::MuteResult in state %S [%d]",
        State( iPlayerState ), this );

    if( iValueStateMachine )
        {
        iValueStateMachine->MuteResult( aError, aMute, aActionResponse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::InteractOperationComplete
// Response for interaction operation (play, stop, etc.).
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::InteractOperationComplete( TInt aErrorCode, 
    TUPnPAVInteractOperation aOperation )
    {
    __LOG3( "CUPnPMusicPlayer::InteractOp.Comp(%d) in state %S [%d]",
        aOperation, State( iPlayerState ), this );

    if( iPlayerState == EStateActive )
        {
        if( iPlaybackStateMachine )
            {
            iPlaybackStateMachine->InteractOperationComplete( aErrorCode,
                aOperation );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::PositionInfoResult
// Response for position/duration requests.
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::PositionInfoResult( TInt aStatus,
    const TDesC8& aTrackPosition, const TDesC8& aTrackLength )
    {
    __LOG2( "CUPnPMusicPlayer::PositionInfoResult in state %S [%d]",
        State( iPlayerState ), this );
    
    if( iValueStateMachine )
        {
        iValueStateMachine->PositionInfoResult( aStatus, aTrackPosition, 
            aTrackLength );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SetURIResult
// Response for SetURI.
// --------------------------------------------------------------------------
//  
void CUPnPMusicPlayer::SetURIResult( TInt aError )
    {
    __LOG3( "CUPnPMusicPlayer::SetURIResult in state %S err=%d [%d]",
        State( iPlayerState ), aError, this );

    if( iPlayerState == EStateInitializing )
        {
        // "Normal" response for SetURI
        TInt duration(0);
        if( aError == KErrNone )
            {
            // Set player state to active
            ChangeState( EStateActive );
            TRAP_IGNORE( iAudioPolicy->PlayL() );
            duration = iTrack->TrackDuration();
            }
        else
            {
            UninitialiseTrack();
            }
        // if aError is KErrGeneral, the music player doesn't skip to
        //the next song .  translate to KErrCorrupt.
        if( aError == KErrGeneral)
            {
            aError = KErrCorrupt;
            }
        else
            {
            aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
            }
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPInitialised, duration, aError );
        if ( iPlayerState == EStateActive )
            {
            TRAP_IGNORE(
                if( iTrack->IsRemote() )
                    {
                    iTrack->SendMediaChangedEventL( *iObs );
                    }
                );
            }
        }
    else if( iPlayerState == EStateActiveForceInitialise )
        {
        // Response for preinitialise that is failed because of
        // used device does not support SetNextUri functionality.
        if( aError == KErrNone )
            {            
            __LOG( "SetURIResult,ActiveForceInitialise->Play" );
            // Set player state to active
            iPlayerState = EStateActive;
            TRAP_IGNORE( iAudioPolicy->PlayL() );
                            
            // Play 
            TRAPD( error, CommandL( EPbCmdPlay, TInt(0) ) );
            if ( error != KErrNone )
                {
                UninitialiseTrack();
                // Send play event with error to user. Note play event
                // because we are actually performing play command.
                error = TUpnpPluginsErrorTranslation::ErrorTranslate( error );
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPPlaying,
                    0, error );
                // Send event that device is not available.
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPPlayerUnavailable,
                    0, KErrNotFound );
                }
            }
        else
            {
            UninitialiseTrack();
            // Send play event with error to user. Note play event
            // because weare actually performing play command.
            aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
            iObs->HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPPlaying,
                0, aError );
            }
        }
    else if( iPlayerState == EStatePauseInActiveForceInitialise )
        {
        // This case happens when active plugin is paused and user 
        // skips to the next track from UI.
        __LOG( "SetUriResult during EStatePauseInActiveForceInitialise" );

        iPlayerState = EStateActive;
        if( aError == KErrNone )
            {
            // Pause
            TRAPD( error, CommandL( EPbCmdPause, TInt(0) ) );
            if ( error != KErrNone )
                {
                UninitialiseTrack();
                // Send pause event with error to user. Note pause event
                // because we are actually performing pause command.
                error = TUpnpPluginsErrorTranslation::ErrorTranslate( error );
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPPaused,
                    0, error );
                }
            }
        else
            {
            UninitialiseTrack();
            // Send pause event with error to user. Note pause event
            // because we are actually performing pause command.
            aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
            iObs->HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPPaused, 0, aError );
            }
        }
    else // Covers also if Cancel request is called during SetURI
        {
        __LOG( "CUPnPMusicPlayer::SetURIResult - Unknown state " );
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPMusicPlayer::SetNextURIResult
// Response for SetNextURI.
// --------------------------------------------------------------------------
//  
void CUPnPMusicPlayer::SetNextURIResult( TInt aError )
    {
    __LOG2( "CUPnPMusicPlayer::SetNextURIResult err=%d [%d]",
        aError, this );

    aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
    if( iPlayerState == EStatePreInitializing )
        {
        TInt duration(0);
        // Check if any error during SetNextURI call
        if( aError == KErrNone )
            {
            // Set palyer state to preinitialized
            ChangeState( EStatePreInitialized );
            duration = iTrack->TrackDuration();
            }
        else 
            {
            // Used renderer does not support SetNextURI feature. 
            // Initialise track by SetURI after play is called.
            ChangeState( EStateWaiting );
            }
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPInitialised, duration, aError );
        if ( iPlayerState == EStatePreInitialized )
            {
            TRAP_IGNORE(
                if( iTrack->IsRemote() )
                    {
                    iTrack->SendMediaChangedEventL( *iObs );
                    }
                );
            }
        }
    else
        {
        __LOG( "CUPnPMusicPlayer::SetNextURIResult - Wrong state! " );
        __PANICD( __FILE__, __LINE__ );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::MediaRendererDisappeared
// Indication in case of renderer disappear
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::MediaRendererDisappeared( 
    TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG1( "CUPnPMusicPlayer::MediaRendererDisappeared [%d]",
        this );
    
    if ( aReason == MUPnPAVSessionObserverBase::EWLANLost )
        {
        if( iTrack )
            {
            if ( iTrack->IsRemote() )
                {
                __LOG( "CUPnPMusicPlayer::MediaRendererDisappeared - \
                WLAN disappeared while playing a REMOTE track " );
                // WLAN disappeared while playing a REMOTE track
                // stop rendering session and inform player             
                // Fixes ESLX-7KQERV
                CancelRequest();
                UninitialiseTrack( EStateError );
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPPlayerUnavailable,
                    0, KErrDisconnected );
                }
            else
                {
                __LOG( "CUPnPMusicPlayer::MediaRendererDisappeared - \
                WLAN disappeared while playing a LOCAL track " );
                // WLAN disappeared while playing a LOCAL track
                // signal not available.
                UninitialiseTrack( EStateError );
                iObs->HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPPlayerUnavailable,
                    0, KErrNone );
                }
            }
        else // Track does not exist -> state is already uninitialised
            {
            iObs->HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPPlayerUnavailable,
                0, KErrNone );
            }
        }
    else
        {
        // renderer disappeared
        UninitialiseTrack( EStateUninitialised );
        iObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPlayerUnavailable,
            0, KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ReserveLocalMSServicesCompleted
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::ReserveLocalMSServicesCompleted( TInt /*aError*/ )
    {
    __LOG1( "CUPnPMusicPlayer::ReserveMSCompleted!? [%d]", this );
    // Implementation no needed.
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::HandleSubPlayerNames
// Response for GetSubplayerNames request.
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::HandleSubPlayerNames( const MDesCArray* aSubPlayers, 
    TBool aComplete, TInt aError )
    {
    __LOG2( "CUPnPMusicPlayer::HandleSubPlayerNames in state %S [%d]",
        State( iPlayerState ), this );
    
    aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
    // Return subplayer names
    iObs->HandleSubPlayerNames( KMusicPlayerUid, aSubPlayers,
        aComplete, aError );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::RendererListChanged
// Call back if reanderer list has changed.
// --------------------------------------------------------------------------
// 
void CUPnPMusicPlayer::RendererListChanged()
    {
    __LOG2( "CUPnPMusicPlayer::RendererListChanged in state %S [%d]",
        State( iPlayerState ), this );
    
    // Create EPSubPlayersChanged event
    iObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPSubPlayersChanged,
        KErrNone, KErrNone );
    }


// --------------------------------------------------------------------------
// CUPnPMusicPlayer::AudioConflict
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::AudioConflict( TInt /*aError*/ )
    {

    // stop playing
    TRAP_IGNORE( iPlaybackStateMachine->SilentStopL() );

    // notify framework        
    iObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPStopped,
        0, KErrNone );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayer::ChangeState
// --------------------------------------------------------------------------
//
void CUPnPMusicPlayer::ChangeState( TPlayerState aNewState )
    {   
    __LOG3( "CUPnPMusicPlayer: STATE %S -> %S [%d]",
        State( iPlayerState ), State( aNewState ), this );

    iPlayerState = aNewState;
    }


// --------------------------------------------------------------------------
// CUPnPMusicPlayer::State
// --------------------------------------------------------------------------
//
const TDesC* CUPnPMusicPlayer::State( TPlayerState aState ) const
    {
    switch( aState )
        {
        case EStateUninitialised:
            {
            return &KStateUninitialized;
            }
        case EStateInitializing:
            {
            return &KStateInitializing;
            }
        case EStateUninitialising:
            {
            return &KStateUninitialising;
            }
         case EStateActive:
            {
            return &KStateActive;
            }
         case EStatePreInitializing:
            {
            return &KStatePreInitializing;
            }
         case EStatePreInitialized:
            {
            return &KStatePreInitialized;
            }
        case EStateWaiting:
            {
            return &KStateWaiting;
            }
        case EStateActiveForceInitialise:
            {
            return &KStateActiveForceInit;
            }
        case EStateError:
            {
            return &KStateError;
            }
        default:
            {
            return &KStateUnknown;
            }
        }
    }

 

