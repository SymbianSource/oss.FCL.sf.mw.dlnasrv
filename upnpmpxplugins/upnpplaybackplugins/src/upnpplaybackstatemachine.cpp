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
#include "upnpavrenderingsessionobserver.h"
#include "upnpavdevice.h"

#include "upnpmusicplayer.h" // for parent.Observer()
#include "upnptrack.h" // for TrackDuration
#include "upnpvaluestatemachine.h" //
#include "upnpplaybackstatemachine.h" // myself
#include "upnppluginserrortranslation.h"

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KStateStoppedText, "Stopped" );
_LIT( KStatePlayingText, "Playing" );
_LIT( KStatePausedText, "Paused" );
_LIT( KStateUnknownText, "Unknown" );

const TInt KDurationErrorMargin = 500; // 0.5 seconds
const TInt KPlaybackInfoTimeOut = 1000000; // 1s wait until device is ready
const TInt KPlaybackInfoTimeOutEnd = 2000000; // 2s wait check whether
    //the playing is ended

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPPlaybackStateMachine* CUPnPPlaybackStateMachine::NewL( 
    CUPnPMusicPlayer& aParent,
    MUPnPAVRenderingSession& aRenderingSession )
    {
    __LOG( "PlaybackStateMachine::NewL" );
    CUPnPPlaybackStateMachine* self =
        new(ELeave) CUPnPPlaybackStateMachine(
            aParent, aRenderingSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::CUPnPPlaybackStateMachine
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPPlaybackStateMachine::CUPnPPlaybackStateMachine( 
    CUPnPMusicPlayer& aParent,
    MUPnPAVRenderingSession& aRenderingSession )
    :iParent( aParent )
    ,iRendererSession( aRenderingSession )
    ,iState( EStateStopped )
    ,iCurrentOperation( EOperationNone )
    {
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::ConstructL()
    {
    __LOG( "CUPnPPlaybackStateMachine::ConstructL" );

    // Create timer for observing duration query time out.
    TInt64 iTime = 0;
    iPlayMark = TTime( iTime );
    iPauseMark = TTime( iTime );
    iPeriodizer = CUPnPMusicPeriodizer::NewL( *this, KPlaybackInfoTimeOut );
    iPeriodizerEnd = CUPnPMusicPeriodizer::NewL( *this, 
        KPlaybackInfoTimeOutEnd );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::~CUPnPPlaybackStateMachine
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPPlaybackStateMachine::~CUPnPPlaybackStateMachine()
    {
    __LOG( "PlaybackStateMachine destructor" );
    iOperationQueue.Close();
    delete iPeriodizer;
    delete iPeriodizerEnd;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::CommandL
// Executes a command on the selected song.
// --------------------------------------------------------------------------
// 
void CUPnPPlaybackStateMachine::CommandL( TMPXPlaybackCommand aCmd )
    {
    if ( aCmd == EPbCmdClose )
        {
        // close can be handled parallel to other commands
        HandleCloseL();
        return;
        }

    if ( !iCurrentOperation.None() )
        {
        __LOG( "PlaybackStateMachine::CommandL - Append to queue" );
        iOperationQueue.AppendL( aCmd );
        return;
        }

    switch( aCmd )
        {
        case EPbCmdPlay:
            {            
            if( iState == EStatePlaying )
                {
                // already playing - ignore
                CheckOperationInQueueL();
                }
            else
                {
                __LOG( "PlaybackStateMachine: command play" );
                iCurrentOperation = aCmd;
                iRendererSession.PlayL();
                } 
            }
            break;
        case EPbCmdPause:
            {
            // Check if pause is supported by device
            if( iParent.UsedRendererDevice().PauseCapability() )
                {
                if( iState == EStatePaused )
                    {
                    // already paused - ignore
                    CheckOperationInQueueL();
                    }
                else
                    {
                    __LOG( "PlaybackStateMachine: command pause" );
                    iCurrentOperation = aCmd;
                    iRendererSession.PauseL();
                    }
                }
            else
                {
                __LOG( "PlaybackStateMachine: Pause is not supported by \
                device!" );
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPPaused, 0,
                    KErrNotSupported );
                }
            }
            break;
        case EPbCmdStop:
            {
            if( iState == EStateStopped )
                {
                // already stopped - ignore
                CheckOperationInQueueL();
                }
            else
                {
                __LOG( "PlaybackStateMachine: command stop" );
                iCurrentOperation = aCmd;
                iRendererSession.StopL();
                }
            }
            break;
        case EPbCmdReplay:
            {
            iCurrentOperation = aCmd;
                __LOG( "PlaybackStateMachine: command replay" );
            if( iState == EStatePlaying || iState == EStatePaused )
                {
                // playing -must stop first
                iRendererSession.StopL();
                }
            else
                {
                // just play
                iRendererSession.PlayL();
                }
            }
            break;
        default: // Given command is not supported
            {
            __LOG( "PlaybackStateMachine: command default?" );
            CheckOperationInQueueL();
            User::Leave( KErrNotSupported );
            break;
            } 
        } 
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::PositionL
// Changes the position within the currently playing track
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::PositionL( TInt aPosition )
    {
    
    // Ignore if already called.
    if( iCurrentOperation == EOperationPositionToZero )
        {
        __LOG( "PlaybackStateMachine::PositionL - \
        EOperationPositionToZero called twice -> ignored" );
        return;
        }
    
    if ( !iCurrentOperation.None() )
        {
        __LOG( "PlaybackStateMachine::PositionL - Append to queue" );
        iOperationQueue.AppendL( EOperationPositionToZero );
        return;
        }
    if ( aPosition != 0 )
        {
        CheckOperationInQueueL();
        User::Leave( KErrNotSupported );
        }

    // handle Position(0)
    if ( iState == EStatePlaying )
        {
        __LOG( "PlaybackStateMachine: position(0) while playing" );
        iCurrentOperation = EOperationPositionToZero;
        iRendererSession.StopL();
        }
    else if ( iState == EStatePaused )
        {
        __LOG( "PlaybackStateMachine: PositionL(0) while paused" );
        iCurrentOperation = EOperationPositionToZeroDuringPause;
        iRendererSession.StopL();
        }
    else if ( iState == EStateStopped )
        {
        __LOG( "PlaybackStateMachine: PositionL(0) while stopped" );
        CheckOperationInQueueL();
        
        // immediate response !        
        iParent.Observer().HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPSetComplete,
            EPbPropertyPosition, KErrNone );
        
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::SilentStopL
// Stops playback, does not provide any ACK event
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::SilentStopL()
    {
    if ( !iCurrentOperation.None() )
        {
        __LOG( "PlaybackStateMachine::SilentStopL - Append to queue" );
        iOperationQueue.AppendL( EOperationSilentStop );
        return;
        }

    // handle stop
    __LOG( "PlaybackStateMachine: SilentStopL" );
    if( iState == EStateStopped )
        {
        // already stopped - ignore
        CheckOperationInQueueL();
        }
    else
        {
        iCurrentOperation = EOperationSilentStop;
        iRendererSession.StopL();
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::Cancel
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::Cancel()
    {
    // reset current operation and empty the queue -> no callbacks.
    iCurrentOperation.Reset();
    iOperationQueue.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::HandleCloseL
// Handles the close command
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::HandleCloseL()
    {
    if ( iCurrentOperation == EPbCmdStop )
        {
        // Stop pending - it can't be completed -> fake callback.
        iParent.Observer().HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPStopped,
            KErrNone, KErrNone );
        }

    iParent.Observer().HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPClosed,
        KErrNone, KErrNone );
    }


// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::InteractOperationComplete
// Response for interaction operation (play, stop, etc.).
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::InteractOperationComplete( TInt aErrorCode, 
    TUPnPAVInteractOperation aOperation )
    {
    __LOG1( "CUPnPPlaybackStateMachine::InteractOperationComplete: err:[%d]",
        aErrorCode );
    aErrorCode = TUpnpPluginsErrorTranslation::ErrorTranslate( aErrorCode );
    switch( aOperation )
        {
        case EUPnPAVPlayUser:
            {                
            __LOG1( "PlaybackStateMachine: event play user, state[%d]",iState );

            if ( aErrorCode == KErrNone )
                {
                if ( iState == EStatePaused )
                    {
                    TimeContinue();
                    ChangeState( EStatePlaying );
                    iParent.Observer().HandlePluginEvent( 
                        MMPXPlaybackPluginObserver::EPPlaying, KErrNone,
                        aErrorCode );
                    if( iCurrentOperation == EPbCmdPlay )
                        {
                        iCurrentOperation.Reset();
                        TRAP_IGNORE( CheckOperationInQueueL() );
                        }
                    }
                else if ( iState == EStatePlaying )
                    {
                    iPeriodizer->Stop();
                    iParent.HandlePlayStarted();
                    }
                else if ( iState == EStateStopped )
                    {
                    if ( iCurrentOperation == EPbCmdPlay ||
                        iCurrentOperation == EPbCmdReplay )
                        {
                        TimePlay();
                        iParent.HandlePlayStarted();
                        ChangeState( EStatePlaying );
                        iCurrentOperation.Reset();
                        TRAP_IGNORE( CheckOperationInQueueL() );
                        iParent.Observer().HandlePluginEvent( 
                            MMPXPlaybackPluginObserver::EPPlaying, KErrNone,
                            aErrorCode );
                        }
                    else
                        {
                        // spontaneous change to PLAYING state -> ignore
                        }
                    }
                    
                }
            else
                {
                ChangeState( EStateUnknown );
                }
            break;
            }
        case EUPnPAVPlay: 
            {
            if ( iState == EStatePlaying )
                {
                __LOG( "PlaybackStateMachine: event play ignored" );
                break;
                }

            __LOG( "PlaybackStateMachine: event play" );

            if ( aErrorCode == KErrNone )
                {
                if ( iState == EStateStopped )
                    {
                    TimePlay();
                    iPeriodizer->Start();
                    iPeriodizerEnd->Start();           
                    }
                else if ( iState == EStatePaused )
                    {
                    TimeContinue();
                    iPeriodizer->Stop();       
                    iPeriodizer->Start();    
                    iPeriodizerEnd->Stop();
                    iPeriodizerEnd->Start();
                    }
                ChangeState( EStatePlaying );
                }
            else
                {
                ChangeState( EStateUnknown );
                }

                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
            break;
            }
        case EUPnPAVPauseUser:
            {            
            if ( !iCurrentOperation.None() ||
                iState == EStatePaused )
                {
                __LOG( "PlaybackStateMachine: event pause user ignored" );
                break;
                }
            __LOG( "PlaybackStateMachine: event pause user" );
            
            if( aErrorCode == KErrNone )
                {
                TimePause();
                ChangeState( EStatePaused );
                iPeriodizerEnd->Stop();
                }
            else
                {
                ChangeState( EStateUnknown );
                }

            // Call callback
            iParent.Observer().HandlePluginEvent( 
                MMPXPlaybackPluginObserver::EPPaused, KErrNone,
                aErrorCode );
            break;
            }

        case EUPnPAVPause:
            {            
            __LOG( "PlaybackStateMachine: event pause" );

            TInt err = aErrorCode;
            if( aErrorCode == KErrNone )
                {
                TimePause();
                ChangeState( EStatePaused );
                iPeriodizerEnd->Stop();
                }
            else if( aErrorCode == KErrNotSupported )
                {
                // Pause is supported (pause capability checked in CommandL )
                // but device is not ready.
                err = KErrNotReady;
                }
            else
                {
                ChangeState( EStateUnknown );
                }

            if ( iCurrentOperation == EPbCmdPause )
                {
                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPPaused, KErrNone,
                    err );
                }
            else
                {
                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
                }
            break;
            }
        case EUPnPAVStopUser:
            {
            iPeriodizerEnd->Stop();
            if ( !iCurrentOperation.None() ||
                iState == EStateStopped )
                {
                __LOG( "PlaybackStateMachine: event stop user ignored" );
                break;
                }
            __LOG( "PlaybackStateMachine: event stop user" );

            TBool trackComplete = TimeStop();
            if ( aErrorCode == KErrNone )
                {
                ChangeState( EStateStopped );
                iParent.HandlePlayComplete();
                }
            else
                {
                ChangeState( EStateUnknown );
                }

            // Call callback
            if ( trackComplete )
                {
                __LOG( "PlaybackStateMachine: play complete event to user" );
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPPlayComplete, KErrNone,
                    aErrorCode );
                }
            else
                {
                __LOG( "PlaybackStateMachine: play stopped event to user" );
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPStopped, KErrNone,
                    aErrorCode );
                }
            break;
            }       
        case EUPnPAVStop:
            {
            iPeriodizerEnd->Stop();
            if ( iState == EStateStopped )
                {
                // already in this state - ignore
                break;
                }
            __LOG( "PlaybackStateMachine: event stop" );

            if ( aErrorCode == KErrNone )
                {                 
                ChangeState( EStateStopped );
                }
            else
                {
                ChangeState( EStateUnknown );
                }

            if ( iCurrentOperation == EPbCmdStop )
                {
                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
                 // Call callback
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPStopped, KErrNone,
                    aErrorCode );
                }
            else if ( iCurrentOperation == EPbCmdReplay ||
                iCurrentOperation == EOperationPositionToZero )
                {
                // continue with play
                TRAP_IGNORE( iRendererSession.PlayL() );
                }
            else if ( iCurrentOperation ==
                EOperationPositionToZeroDuringPause )
                {
                // position zero complete
                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
               
                iParent.Observer().HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPSetComplete,
                    EPbPropertyPosition, aErrorCode );
                }
            else
                {
                // Note: covers also EOperationSilentStop
                iCurrentOperation.Reset();
                TRAP_IGNORE( CheckOperationInQueueL() );
                }
            break;
            }
        default:
            {
            __LOG( "PlaybackStateMachine: event default?" );
            __PANICD( __FILE__, __LINE__ );
            break;
            }    
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::CheckOperationInQueueL
// Checks if operations are in the queue, and executes
// --------------------------------------------------------------------------
//   
void CUPnPPlaybackStateMachine::CheckOperationInQueueL()
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
        if ( op == EOperationCommand )
            {
            CommandL( op.iCmd );
            }
        else if ( op == EOperationPositionToZero )
            {
            PositionL( 0 );
            }
        else
            {
            __PANICD( __FILE__, __LINE__ );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::TimePlay
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::TimePlay()
    {
    iPlayMark.UniversalTime();
    iPausetime = 0;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::TimePause
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::TimePause()
    {
    iPauseMark.UniversalTime();
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::TimeContinue
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::TimeContinue()
    {
    TTime continueMark;
    continueMark.UniversalTime();
    TTimeIntervalMicroSeconds paused =
        continueMark.MicroSecondsFrom( iPauseMark );
    iPausetime += ( paused.Int64() / 1000 );
    }


// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::TimeStop
// --------------------------------------------------------------------------
//
TBool CUPnPPlaybackStateMachine::TimeStop()
    {
    TBool isCompleted = ETrue;
    if ( !iPlayMark.Int64() )
        {
        isCompleted = EFalse; 
        return isCompleted;  
        }
    TTime stopMark;
    stopMark.UniversalTime();
    TTimeIntervalMicroSeconds played =
        stopMark.MicroSecondsFrom( iPlayMark );

    TInt duration = iParent.Track().TrackDuration();
    TInt playtime = ( played.Int64() / 1000 ) - iPausetime;
    __LOG3("PlaybackStateMachine: playtime=%d duration=%d (pausetime=%d)",
        playtime/1000, duration/1000, iPausetime/1000 );

    if ( playtime >= 0 &&
        playtime < duration - KDurationErrorMargin )
        {
        // [0 - duration-margin]
        isCompleted= EFalse;
        }
    else if ( playtime >= duration - KDurationErrorMargin &&
        playtime <= duration + KDurationErrorMargin )
        {
        // [duration-margin - duration+margin]
        isCompleted= ETrue;
        }
    else
        {
        // position either negative or greater than duration ??
        __LOG2("Time ERROR: play=%d duration=%d ?", playtime, duration );
        isCompleted= ETrue;
        }

    return isCompleted;
    }


// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::ChangeState
// Changes the class state
// --------------------------------------------------------------------------
//
void CUPnPPlaybackStateMachine::ChangeState( TState aNewState )
    {
    __LOG2( "PlaybackStateMachine: STATE %S -> %S",
        State( iState ), State( aNewState ) );
    iState = aNewState;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::State
// --------------------------------------------------------------------------
//
const TDesC* CUPnPPlaybackStateMachine::State( TState aState )
    {
    switch( aState )
        {
        case EStateStopped:
            return &KStateStoppedText;
        case EStatePlaying:
            return &KStatePlayingText;
        case EStatePaused:
            return &KStatePausedText;
        default:
            return &KStateUnknownText;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::HandlePeriod
// Action when timer has expired.
// --------------------------------------------------------------------------
// 
void CUPnPPlaybackStateMachine::HandlePeriod()
    {
    __LOG( "CUPnPPlaybackStateMachine::HandlePeriod" );

    // Remote device is ready for give playback information.
    iPeriodizer->Stop();
    iParent.HandlePlayStarted();
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::HandlePeriodForEnd
// Action when timer has expired.
// --------------------------------------------------------------------------
// 
void CUPnPPlaybackStateMachine::HandlePeriodForEnd()
    {
    TBool trackComplete = TimeStop();
    if ( trackComplete )
        {
        iPeriodizerEnd->Stop();
        PlayOvertimeEnd();    
        }
    else
        {      
        iPeriodizerEnd->Start(); 
        }           
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackStateMachine::PlayOvertimeEnd
// Stoping the playing song.
// --------------------------------------------------------------------------
// 
 void CUPnPPlaybackStateMachine::PlayOvertimeEnd()
     {
     if ( !iCurrentOperation.None() ||
         iState == EStateStopped || iState == EStateUnknown)
        {
        return;
        }
     ChangeState( EStateStopped );  
     iParent.HandlePlayComplete();       
     iParent.Observer().HandlePluginEvent( 
          MMPXPlaybackPluginObserver::EPPlayComplete, KErrNone,
          KErrNone ); 
     }

