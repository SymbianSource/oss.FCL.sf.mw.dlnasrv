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
* Description:      Class for retrieving and selecting media renderers
*
*/






// INCLUDES
#include <mpxplaybackplugin.h>
#include <mpxplaybackpluginobserver.h>
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"
#include "upnpitemutility.h" // for UPnPDurationToMilliseconds
#include "upnpmusicplayer.h"
#include "upnptrack.h" // for TrackDuration
#include "upnpvaluestatemachine.h"
#include "upnppluginserrortranslation.h"

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// CONSTANT
const TInt KDefaultMaxVolume = 100;
const TInt KDefaultMinVolume = 0;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPValueStateMachine* CUPnPValueStateMachine::NewL(
    CUPnPMusicPlayer& aParent,
    MUPnPAVRenderingSession& aRenderingSession )
    {
    __LOG( "ValueStateMachine: NewL" );
    CUPnPValueStateMachine* self = new(ELeave)
        CUPnPValueStateMachine( aParent, aRenderingSession );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::CUPnPValueStateMachine
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPValueStateMachine::CUPnPValueStateMachine( 
    CUPnPMusicPlayer& aParent,
    MUPnPAVRenderingSession& aRenderingSession ) :
        iParent( aParent ),
        iRendererSession( aRenderingSession ),
        iCurrentOperation( EOperationNone ),
        iRendererVolume( KErrNotFound ),
        iRendererMuted( KErrNotFound )
    {
    }


// --------------------------------------------------------------------------
// CUPnPValueStateMachine::~CUPnPValueStateMachine
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPValueStateMachine::~CUPnPValueStateMachine()
    {
    __LOG( "ValueStateMachine: destructor" );
    iOperationQueue.Close();
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::CopyValues
// copies renderer-specific cached values from another instance
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::CopyValues(
    const CUPnPValueStateMachine& aOther )
    {
    iRendererVolume = aOther.iRendererVolume;
    iRendererMuted = aOther.iRendererMuted;
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::ValidatePropertyInState
// defines the rules of getting / setting in different states
// --------------------------------------------------------------------------
//
CUPnPValueStateMachine::TValidationResult
    CUPnPValueStateMachine::ValidatePropertyInState(
        TMPXPlaybackProperty aProperty,
        TBool aSetting,
        TInt aPlayerState,
        TBool aIsReady )
    {
    switch( aProperty )
        {
        case EPbPropertyMaxVolume: // fall through
        case EPbPropertyRemote: // fall through
        case EPbPropertySupportedFeatures:
            {
            // allow getting in all states,
            // trying to set will result to error
            if ( !aSetting )
                {
                return EHandleStatic;
                }
            else
                {
                return EErrorNotSupported;
                }
            }
        case EPbPropertyVolume: // fall through
        case EPbPropertyMute:
            {            
            // allow volume get/set after initialisation when remote device
            // is ready. In other states respond with error
            if ( aPlayerState ==
                CUPnPMusicPlayer::EStateActive )
                {
                if( !aIsReady )
                    {
                    return EErrorNotReady;
                    }
                else
                    {
                    return EHandle;
                    }
                }
            else if ( aPlayerState ==
                CUPnPMusicPlayer::EStatePreInitialized ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateWaiting ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateActiveForceInitialise )
                {
                return EHandle;
                }
            else
                {
                return EErrorNotReady;
                }
            }
        case EPbPropertyDuration:
            {
            // duration setting is not allowed
            // getting is allowed after initialised, but will result
            // to returning plain zero in other states except active
            // when it is actually asked from the device
            if ( aSetting )
                {
                return EErrorNotSupported;
                }
            else if ( aPlayerState ==
                CUPnPMusicPlayer::EStateActive )
                {
                if( aIsReady )
                    {
                    return EHandle;
                    }
                else
                    {
                    // On active state but device is not ready ->
                    // Use initial values.
                    return EHandleStatic;
                    }
                }
            else if ( aPlayerState ==
                CUPnPMusicPlayer::EStatePreInitialized ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateWaiting ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateActiveForceInitialise )
                {
                return EHandleStatic;
                }
            else
                {
                return EErrorNotReady;
                }
            }
        case EPbPropertyPosition:
            {
            // position get/set is allowed after init when remote device 
            // is ready. Only active state it will result to real action
            if ( aPlayerState ==
                CUPnPMusicPlayer::EStateActive )
                {
                if( aIsReady )
                    {
                    return EHandle;
                    }
                else
                    {
                    // On active state but device is not ready ->
                    // Use initial values.
                    return EHandleStatic;
                    }
                }
            else if ( aPlayerState ==
                CUPnPMusicPlayer::EStatePreInitialized ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateWaiting ||
                aPlayerState ==
                CUPnPMusicPlayer::EStateActiveForceInitialise )
                {
                return EHandleStatic;
                }
            else
                {
                return EErrorNotReady;
                }
            }
        default:
            {
            // unsupported property
            return EErrorNotSupported;
            }
        }
    }


// --------------------------------------------------------------------------
// CUPnPValueStateMachine::SetL
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::SetL( TMPXPlaybackProperty aProperty,
    TInt aValue )
    {
    // Check if volume or mute is supported by device
    if( ( aProperty == EPbPropertyVolume && 
          !iParent.UsedRendererDevice().VolumeCapability() ) ||
        ( aProperty == EPbPropertyMute && 
          !iParent.UsedRendererDevice().MuteCapability() )      
      ) 
        {
        __LOG( "ValueStateMachine::SetL - \
        Request is not supported by device" );
        iParent.Observer().HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
           (TInt)aProperty, KErrNotSupported );
            
        return;
        }

    // Check from queue if alredy requested.
    if( FoundFromQueue( aProperty ) )
        {
        __LOG( "ValueStateMachine::SetL: Volume query Ignored!" );
        // Ignore
        return;
        }
    
    if ( !iCurrentOperation.None() )
        {
        __LOG( "ValueStateMachine::SetL - Append to queue" );
        iOperationQueue.AppendL( TOperation( aProperty, aValue ) );
        return;
        }

    switch( aProperty )
        {
        case EPbPropertyVolume:
            {               
            __LOG1( "ValueStateMachine: SetVolume(%d)",
                aValue );
            // Set the volume, do a sanity check
            if( aValue < KDefaultMinVolume || aValue > KDefaultMaxVolume )
                {
                // Given value out of range
                CheckOperationInQueueL();
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPSetComplete,
                    aProperty, KErrArgument );
                }
            else if ( aValue == iRendererVolume )
                {
                // re-setting the same volume
                if ( iRendererMuted )
                    {
                    MuteRequestL( 0 );
                    }
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPSetComplete,
                    aProperty, KErrNone );
                }
            else
                {
                iCurrentOperation.Set( aProperty, aValue );
                iRendererSession.SetVolumeL( aValue );
                if ( iRendererMuted )
                    {
                    // unmute after volume has been adjusted
                    MuteRequestL( 0 );
                    }
                }        
            break;
            }
        case EPbPropertyMute:
            {
            __LOG1( "ValueStateMachine: SetMute(%d)",
                aValue );
            if( aValue != 0 && iRendererMuted != 1 )
                {
                // set mute on
                iCurrentOperation.Set( aProperty, 1 );
                iRendererSession.SetMuteL( ETrue );
                }
            else if ( aValue == 0 && iRendererMuted != 0 )
                {
                // set mute off
                iCurrentOperation.Set( aProperty, 0 );
                iRendererSession.SetMuteL( EFalse );
                }
            else
                {
                // mute is already in requested state
                CheckOperationInQueueL();
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPSetComplete,
                    aProperty, KErrNone );
                }
            break;
            }
        default: // Given property is not supported
            {
            __LOG( "ValueStateMachine: Set - default" );
            CheckOperationInQueueL();
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPSetComplete,
                aProperty, KErrNotSupported );
            break;
            }    
        }
 
    }
    
// --------------------------------------------------------------------------
// CUPnPValueStateMachine::ValueL
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::ValueL( TMPXPlaybackProperty aProperty )
    {
    // Check if volume or mute is supported by device
    if( ( aProperty == EPbPropertyVolume && 
          !iParent.UsedRendererDevice().VolumeCapability() ) ||
        ( aProperty == EPbPropertyMute && 
          !iParent.UsedRendererDevice().MuteCapability() )      
      ) 
        {
        __LOG( "ValueStateMachine::ValueL - \
        Request is not supported by device" );
        iParent.Observer().HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
           (TInt)aProperty, KErrNotSupported );
            
        return;
        }
    
    if ( !iCurrentOperation.None() )
        {
        __LOG( "ValueStateMachine::ValueL - Append to queue" );
        iOperationQueue.AppendL( TOperation( aProperty ) );
        return;
        }

    switch( aProperty )
        {
        case EPbPropertyVolume:
            {
            __LOG( "ValueStateMachine: Value(volume)" );
            iRendererSession.GetVolumeL();
            iCurrentOperation.Set( aProperty );
            break;
            }
        case EPbPropertyMute:
            {
            __LOG( "ValueStateMachine: Value(mute)" );
            iRendererSession.GetMuteL();
            iCurrentOperation.Set( aProperty );
            break;
            }
        case EPbPropertyDuration:
            {
            __LOG( "ValueStateMachine: Value(duration)" );
            if ( iParent.Track().TrackDuration() > 0 )
                {
                // duration found in cache
                CheckOperationInQueueL();
                iParent.Observer().HandleProperty( aProperty,
                    iParent.Track().TrackDuration(), KErrNone );
                }
            else
                {
                iRendererSession.GetPositionInfoL();
                iCurrentOperation.Set( aProperty );
                }
            break;
            }
        case EPbPropertyPosition:
            {
            __LOG( "ValueStateMachine: Value(position)" );
            iRendererSession.GetPositionInfoL();
            iCurrentOperation.Set( aProperty );
            break;
            }
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            break;
            }    
        }
    }


// --------------------------------------------------------------------------
// CUPnPValueStateMachine::ValueStatic
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::ValueStatic(
    TMPXPlaybackProperty aProperty,
    MMPXPlaybackPluginObserver& aPlaybackObs )
    {
    switch( aProperty )
        {
        case EPbPropertyMaxVolume:
            {
            __LOG( "ValueStateMachine: ValueStatic(max volume)" );
            aPlaybackObs.HandleProperty( aProperty,
                KDefaultMaxVolume, KErrNone );
            break;
            }
         case EPbPropertyRemote:
            {
            __LOG( "ValueStateMachine: ValueStatic(remote)" );
            aPlaybackObs.HandleProperty( aProperty,
                (TInt)EFalse, KErrNone );
            break;
            }
        case EPbPropertySupportedFeatures:
            {
            __LOG( "ValueStateMachine: ValueStatic(features)" );
            // No support for any features
            TInt supportedFeatures = 0;              
            aPlaybackObs.HandleProperty( aProperty,
                supportedFeatures, KErrNone );
            break;
            }
        case EPbPropertyDuration:
            {
            __LOG( "ValueStateMachine: ValueStatic(duration)" );
            TInt initialDuration = 0;
            aPlaybackObs.HandleProperty( aProperty,
                initialDuration, KErrNone );
            break;
            }
        case EPbPropertyPosition:
            {
            __LOG( "ValueStateMachine: ValueStatic(position)" );
            TInt initialPosition = 1;
            aPlaybackObs.HandleProperty( aProperty,
                initialPosition, KErrNone );
            break;
            }
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            break;
            }    
        }
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::DurationQueryL
// --------------------------------------------------------------------------
//    
void CUPnPValueStateMachine::DurationQueryL()
    {
    // here we could check if the duration is already present in
    // iParent.track() however it was decided that we check the duration
    // from renderer anyway and trust the renderer's duration more.
    if ( !iCurrentOperation.None() )
        {
        __LOG( "ValueStateMachine::DurationQueryL - Append to queue" );
        iOperationQueue.AppendL( EOperationDurationQuery );
        return;
        }

    __LOG( "ValueStateMachine::DurationQueryL" );
    iRendererSession.GetPositionInfoL();
    iCurrentOperation = EOperationDurationQuery;
    }
    
// --------------------------------------------------------------------------
// CUPnPValueStateMachine::PositionQueryL
// --------------------------------------------------------------------------
//    
void CUPnPValueStateMachine::PositionQueryL()
    {
    __LOG( "ValueStateMachine::PositionQueryL" );
    
    if ( !iCurrentOperation.None() )
        {
        __LOG( "ValueStateMachine::PositionQueryL - Append to queue" );
        iOperationQueue.AppendL( EOperationPositionQuery );
        return;
        }
    iRendererSession.GetPositionInfoL();
    iCurrentOperation = EOperationPositionQuery;
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::VolumeQueryL
// --------------------------------------------------------------------------
//    
void CUPnPValueStateMachine::VolumeQueryL()
    {
    // Check if volume feature is supported by device.
    if( iParent.UsedRendererDevice().VolumeCapability() )
        {
        if ( iRendererVolume != KErrNotFound )
            {
            // Volume query not required - already cached.
            return;
            }

        if ( !iCurrentOperation.None() )
            {
            __LOG( "ValueStateMachine::VolumeQueryL - Append to queue" );
            iOperationQueue.AppendL( EOperationVolumeQuery );
            return;
        }

        __LOG( "ValueStateMachine::VolumeQueryL" );
        iRendererSession.GetVolumeL();
        iCurrentOperation = EOperationVolumeQuery;
        }
    else
        {
        // Get volume is not supported. Do nothing.
        }
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::MuteRequestL
// --------------------------------------------------------------------------
//    
void CUPnPValueStateMachine::MuteRequestL( TInt aMute )
    {
    if ( !iCurrentOperation.None() )
        {
        __LOG( "ValueStateMachine::MuteRequestL - Append to queue" );
        iOperationQueue.AppendL( TOperation( EOperationMute, aMute ) );
        return;
        }

    __LOG1( "ValueStateMachine: MuteRequest(%d)", aMute );
    if( aMute != 0 && iRendererMuted != 1 )
        {
        // set mute on
        iCurrentOperation = EOperationMute;
        iRendererSession.SetMuteL( ETrue );
        }
    else if ( aMute == 0 && iRendererMuted != 0 )
        {
        // set mute off
        iCurrentOperation = EOperationMute;
        iRendererSession.SetMuteL( EFalse );
        }
    else
        {
        // mute is already in requested state
        CheckOperationInQueueL();
        }
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::VolumeResult
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::VolumeResult( TInt aError, TInt aVolumeLevel,
    TBool aActionResponse )
    {
    aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
    // If response for get volume
    
    if( aActionResponse )
        {
        if (aError == KErrNone)
            {
            iRendererVolume = aVolumeLevel;
            }
        
        if( iCurrentOperation.Compare( EOperationValue, EPbPropertyVolume ) )
            {
            __LOG( "ValueStateMachine::VolumeResult: Value resp." );
            
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            iParent.Observer().HandleProperty( EPbPropertyVolume,
                iRendererVolume, aError );
            }
        else if( iCurrentOperation.Compare( EOperationSet, 
            EPbPropertyVolume ) )
            {
            __LOG( "ValueStateMachine::VolumeResult: Set resp." );
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPSetComplete,
                (TInt)EPbPropertyVolume, aError );
            }
        else if ( iCurrentOperation == EOperationVolumeQuery )
            {
            __LOG1("VolumeQueryResult: Volume query resp err=%d", aError );
            
            if ( aError == KErrNone )
                {
                iParent.Observer().HandlePluginEvent(
                    MMPXPlaybackPluginObserver::EPVolumeChanged,
                    iRendererVolume, aError );
                }
            
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            }
        else
            {         
            iCurrentOperation.Reset();
            __LOG( "ValueStateMachine::VolumeResult: no request?" );
            }
        }
    else // Volume changed from device
        {
        // Ignore volume events from device if volume control 
        // is ongoing on handset.
        if( iCurrentOperation.Compare( EOperationSet, EPbPropertyVolume ) )
            {
            return;
            }
        
        if (aError == KErrNone)
            {
            iRendererVolume = aVolumeLevel;
            }
        
        if ( !iRendererMuted )
            {
            __LOG1( "ValueStateMachine::VolumeResult: from device: (%d)",
                aVolumeLevel );
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPVolumeChanged,
                iRendererVolume, aError );
            }
        else
            {
            __LOG( "ValueStateMachine::VolumeResult: from device (muted)" );
            }
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPValueStateMachine::MuteResult
// --------------------------------------------------------------------------
//    
void CUPnPValueStateMachine::MuteResult( TInt aError, TBool aMute, 
    TBool aActionResponse )
    {
    aError = TUpnpPluginsErrorTranslation::ErrorTranslate( aError );
    // Response for set mute
    if( aActionResponse )
        {
        if( iCurrentOperation.Compare( EOperationValue, EPbPropertyMute ) )
            {
            __LOG( "ValueStateMachine::MuteResult: Value resp." );
            iRendererMuted = aMute;
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            iParent.Observer().HandleProperty( EPbPropertyMute,
                (TInt)aMute, aError );
            }
        else if ( iCurrentOperation.Compare( EOperationSet,
            EPbPropertyMute ) )
            {
            __LOG( "ValueStateMachine::MuteResult: Set resp." );
            iRendererMuted = aMute;
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPSetComplete,
                (TInt)EPbPropertyMute, aError );
            }
        else if ( iCurrentOperation == EOperationMute )
            {
            // internal operation, no event.
            iRendererMuted = aMute;
            iCurrentOperation.Reset();
            TRAP_IGNORE( CheckOperationInQueueL() );
            }
        else
            {
            iRendererMuted = aMute;
            iCurrentOperation.Reset();
            __LOG( "ValueStateMachine::MuteResult: no request?" );
            }
        }
    else // Mute changed from device
        {
        __LOG( "ValueStateMachine::MuteResult: Mute changed from device" );
        
        if( aMute )
            {
            iRendererMuted = ETrue;
            iParent.Observer().HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPVolumeChanged,
                0, aError );
            }
        else
            {
            iRendererMuted = EFalse;
            iParent.Observer().HandlePluginEvent(
                MMPXPlaybackPluginObserver::EPVolumeChanged,
                iRendererVolume, aError );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::PositionInfoResult
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::PositionInfoResult( TInt aStatus,
    const TDesC8& aTrackPosition, const TDesC8& aTrackLength )
    {                      
    // Check if position info was asked
    if( iCurrentOperation.Compare( EOperationValue, EPbPropertyPosition ) )
        {
        __LOG1( "Position response err=%d", aStatus );
        TInt ms = 0;
        if( aStatus == KErrNone )
            {
            // Convert the descriptor to integer value
            aStatus = UPnPItemUtility::UPnPDurationAsMilliseconds(
                aTrackPosition, ms );
            __LOG2( "ValueStateMachine::PositionInfoResult position=%d err=%d",
                ms, aStatus );
            }
        iCurrentOperation.Reset();
        TRAP_IGNORE( CheckOperationInQueueL() );
        
        // Use initial position value in case of errors or position 0.
        // This will guarantee that in case of unpause, SetPosition 
        // is called with real value even if remote player does not
        // return real value for getposition call. Note that 
        // SetPosition(0) is used when user really wants to move to
        // track beginning.
        if( aStatus != KErrNone )
            {
            // GetPositionInfo is not supported by used device.
            iParent.Observer().HandleProperty( EPbPropertyPosition, 1,
                KErrNotSupported );
            }
        else
            {
            if( ms == 0 )
                {
                ms = 1;
                }
            iParent.Observer().HandleProperty( EPbPropertyPosition, ms,
                aStatus );
            }
        }
    else if ( iCurrentOperation == EOperationPositionQuery )
        {
        __LOG1( "PositionQuery response err=%d", aStatus );
        TInt ms = 0;
        if( aStatus == KErrNone )
            {
            // Convert the descriptor to integer value
            aStatus = UPnPItemUtility::UPnPDurationAsMilliseconds(
                aTrackPosition, ms );
            __LOG2( "ValueStateMachine::PositionInfoResult position=%d err=%d",
                ms, aStatus );
            }
        iCurrentOperation.Reset();
        TRAP_IGNORE( CheckOperationInQueueL() );
        
        if( aStatus == KErrNone )
            {
            if( ms == 0 )
                {
                ms = 1;
                }
            //iParent.Observer().HandlePluginEvent( 
            //MMPXPlaybackPluginObserver::EPPositionChanged, ms, KErrNone );
            }
        }
    // Check if duration info was asked    
    else if( iCurrentOperation.Compare( EOperationValue, 
        EPbPropertyDuration ) )
        {
        __LOG1( "Duration response err=%d", aStatus );
        TInt ms = 0;
        if( aStatus == KErrNone )
            {
            aStatus = UPnPItemUtility::UPnPDurationAsMilliseconds(
                aTrackLength, ms );
            __LOG2( "ValueStateMachine::PositionInfoResult -\
            duration=%d err=%d", ms, aStatus );

            if ( aStatus == KErrNone )
                {
                iParent.Track().SetTrackDuration( ms );
                }
            }
        iCurrentOperation.Reset();
        TRAP_IGNORE( CheckOperationInQueueL() );
        
        if( aStatus == KErrNotSupported )
            {
            // GetDuration is not supported by used device.
            // Use initial duration value (1). It will guarantee that in
            // case of unpause, SetPosition is called with real value even
            // if remote player does not return real value for getposition
            // call. Note that SetPosition(0) is used when user really wants
            // to move to track beginning.
            iParent.Observer().HandleProperty( EPbPropertyDuration, 1,
                KErrNone );
            }
        else
            {
            iParent.Observer().HandleProperty( EPbPropertyDuration,
                ms, aStatus );
            }
        }
    else if ( iCurrentOperation == EOperationDurationQuery )
        {
        __LOG1( "DurationQuery response err=%d", aStatus );
        TInt ms = 0;
        if( aStatus == KErrNone )
            {
            aStatus = UPnPItemUtility::UPnPDurationAsMilliseconds(
                aTrackLength, ms );
            __LOG2( "ValueStateMachine::PositionInfoResult - \
            duration=%d err=%d",
                ms, aStatus );
            if ( aStatus == KErrNone )
                {
                iParent.Track().SetTrackDuration( ms );
                }
            }
        iCurrentOperation.Reset();
        TRAP_IGNORE( CheckOperationInQueueL() );

        if ( iParent.Track().TrackDuration() > 0 && aStatus == KErrNone )
            {
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPDurationChanged,
                iParent.Track().TrackDuration(), KErrNone );
            }
        }
    else
        {
        __LOG( "ValueStateMachine::PositionInfoResult: no request?" );
        }
    }



// --------------------------------------------------------------------------
// CUPnPValueStateMachine::Cancel
// --------------------------------------------------------------------------
//
void CUPnPValueStateMachine::Cancel()
    {
    // reset current operation and empty the queue -> no callbacks.
    iCurrentOperation.Reset();
    iOperationQueue.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::CheckOperationInQueueL
// Checks if operations are in the queue, and executes
// --------------------------------------------------------------------------
//   
void CUPnPValueStateMachine::CheckOperationInQueueL()
    {
    if ( !iCurrentOperation.None() )
        {
        // check operation though a current operation exists!
        __PANICD( __FILE__, __LINE__ );
        return;
        }

    if ( iOperationQueue.Count() > 0 )
        {
        TOperation op = iOperationQueue[0];
        iOperationQueue.Remove(0);
        if ( op == EOperationValue )
            {
            ValueL( op.iProperty );
            }
        else if ( op == EOperationSet )
            {
            SetL( op.iProperty, op.iValue );
            }
        else if ( op == EOperationDurationQuery )
            {
            DurationQueryL();
            }
        else if ( op == EOperationVolumeQuery )
            {
            VolumeQueryL();
            }
        else if ( op == EOperationPositionQuery )
            {
            PositionQueryL();
            }            
        else if ( op == EOperationMute )
            {
            MuteRequestL( op.iValue );
            }
        else
            {
            __PANICD( __FILE__, __LINE__ );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPValueStateMachine::FoundFromQueue
// Checks if operations are in the queue
// --------------------------------------------------------------------------
//   
TBool CUPnPValueStateMachine::FoundFromQueue(TMPXPlaybackProperty aProperty)
    {    
    TBool found = EFalse;
    TInt count = iOperationQueue.Count();

    for( TInt i = 0; i < count; i++ )
        {
        TOperation op = iOperationQueue[i];
        if( op.iProperty == aProperty )
            {
            found = ETrue;
            break;
            }
        }
    return found;
    }


