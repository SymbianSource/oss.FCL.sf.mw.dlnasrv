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






#ifndef C_UPNPPLAYBACKSTATEMACHINE_H
#define C_UPNPPLAYBACKSTATEMACHINE_H

// INCLUDES
#include <e32base.h>
#include "upnpmusicperiodizer.h"

// FORWARD DECLARATIONS
class CUPnPMusicPlayer;
class MUPnPPlayBackObserver;
class MUPnPAVRenderingSession;

/**
 * Class for handling playback state machine functionality
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPPlaybackStateMachine : public CBase, 
                                  public MUPnPMusicPeriodizerObserver
    {

public:

    /**
     * Static 1st phase constructor
     *
     * @since Series 60 3.1
     * @param aParent the parent plugin instance
     * @param aRenderingSession for playback commands
     */
    static CUPnPPlaybackStateMachine* NewL( 
        CUPnPMusicPlayer& aParent,
        MUPnPAVRenderingSession& aRenderingSession );

    
    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */   
    void ConstructL();
    
    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    virtual ~CUPnPPlaybackStateMachine();

    /** 
     * Executes a command on the selected song
     *
     * @param aCmd a command
     * @since Series 60 3.1
     */
    void CommandL( TMPXPlaybackCommand aCmd );

    /**
     * Sets the playback position within the current track
     * @param aPosition position in milliseconds
     */
    void PositionL( TInt aPosition );

    /**
     * Stops playback, does not provide any ACK event
     */
    void SilentStopL();

    /**
     * Cancels any ongoing operation
     * (actually it just resets the state so the result will be ignored
     * and not reported)
     */
    void Cancel();

    /**
     * Indicates that the requested interaction operation (play, stop, etc.)
     * is complete.
     *
     * @since Series 60 3.1
     * @param aErrorCode TInt error code
     * @param aOperation TInt operation (TAVInteractOperation)
     * @return None
     */
    void InteractOperationComplete( TInt aErrorCode, 
        TUPnPAVInteractOperation aOperation );

    /**
     * End any ongoing operation
     * (actually it just resets the state so the result will play next song
     */
    void PlayOvertimeEnd(); 

private:

    /**
     * Private default constructor
     *
     * @since Series 60 3.1
     * @param aParent the parent plugin instance
     * @param aRenderingSession for playback commands
     */
    CUPnPPlaybackStateMachine(
        CUPnPMusicPlayer& aParent,
        MUPnPAVRenderingSession& aRenderingSession );

public: // datatypes used in this class

    /**
     * Defines internal state
     *
     * @since Series 60 3.1
     */   
    enum TState
        {
        EStateStopped,
        EStatePlaying,
        EStatePaused,
        EStateUnknown,  // this state entered if a command fails.
                        // every state transition is accepted.
        };

    /**
     * current type of operation
     */
    enum TOperationType
        {
        EOperationNone, // no pending operations
        EOperationCommand, // executing a command
        EOperationPositionToZero, // adjusting position to zero
        EOperationPositionToZeroDuringPause, // ...during pause state
        EOperationSilentStop // stop without ack
        };

    /**
     * An internal operation class
     */
    class TOperation
        {
        public:
        /** construction */
        TOperation( TOperationType aType )
            : iType( aType ) {}
        TOperation( TMPXPlaybackCommand aCmd )
            : iType( EOperationCommand ), iCmd( aCmd ) {}
        /** setters */
        void operator=( TOperationType aType )
            {
            iType = aType;
            }
        void operator=( TMPXPlaybackCommand aCmd )
            {
            iType = EOperationCommand; iCmd = aCmd;
            }
        void Reset()
            {
            iType = EOperationNone;
            }
        /** comparision */
        TBool operator==( TOperationType aType )
            {
            return ( iType == aType );
            }
        TBool operator==( TMPXPlaybackCommand aCmd )
            {
            return ( iType == EOperationCommand && iCmd == aCmd );
            }
        TBool None()
            {
            return ( iType == EOperationNone );
            }
        /** identifies the operation type */
        TOperationType iType;
        /** in case the operation is a command, the command */
        TMPXPlaybackCommand iCmd;
        };

protected: // internal methods

    /**
     * Handles the close command internally
     */
    void HandleCloseL();

    /**
     * Checks if operations are in the queue, and executes
     */
    void CheckOperationInQueueL();

    /**
     * Handle timers when playback start
     */
    void TimePlay();

    /**
     * Handle timers when pause
     */
    void TimePause();

    /**
     * Handle timers when continuing from pause
     */
    void TimeContinue();

    /**
     * Handle timers when stopped
     * Guesses if playback was completed or stopped by user.
     * This is estimated by comparing track play time and track duration.
     * @return ETrue if we guess the track playback was completed
     */
    TBool TimeStop();

    /**
     * Changes the internal state
     * @param aNewState the state to enter
     */
    void ChangeState( TState aNewState );

    /**
     * textual representation of a state
     */
    const TDesC* State( TState aState );

protected: // From MUPnPPeriodizerObserver

    /**
     * See MUPnPMusicPeriodizerObserver
     */
    void HandlePeriod();
    
   /**
     * See MUPnPMusicPeriodizerObserver
     */ 
    void HandlePeriodForEnd();  

private: // data

    /**
     * Parent plugin instance
     */ 
    CUPnPMusicPlayer&           iParent;

    /**
     * Rendering session
     */ 
    MUPnPAVRenderingSession&    iRendererSession;

    /**
     * Playback state
     */ 
    TState                      iState;

    /**
     * currently ongoing operation
     */
    TOperation                  iCurrentOperation;

    /**
     * queue of operations pending
     */
    RArray<TOperation>          iOperationQueue;

    /**
     * time when playback started
     */
    TTime                       iPlayMark;

    /**
     * time when pause started
     */
    TTime                       iPauseMark;

    /**
     * total time spent in pause (in milliseconds)
     */
    TInt                        iPausetime;
    
    /**
     * Timer support
     * Own.
     */
    CUPnPMusicPeriodizer*       iPeriodizer;

     /**
     * Timer support
     * Own.
     */
    CUPnPMusicPeriodizer*       iPeriodizerEnd;

    };


#endif // C_UPNPPLAYBACKSTATEMACHINE_H

