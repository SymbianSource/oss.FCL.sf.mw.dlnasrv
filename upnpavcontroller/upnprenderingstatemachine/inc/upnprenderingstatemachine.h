/*
* Copyright (c) 2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Generic upnp remote rendering state machine
*
*/

#ifndef C_UPNPRENDERINGSTATEMACHINE_H
#define C_UPNPRENDERINGSTATEMACHINE_H

// INCLUDES
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h" // TUPnPAVInteractOperation
#include "upnpresourceselector.h" // TUPnPSelectDefaultResource
#include "upnprenderingstatemachineconstants.h" // types
#include "upnprenderingoperationexecutor.h"

// FORWARD DECLARATIONS
class MUPnPAVRenderingSession;
class MUpnpRenderingStateMachineObserver;
class TUpnpRenderingOperation;
class CUpnpRenderingPlaytimeCalculator;
class CUpnpRenderingStateMachineStates;
class CUpnpRenderingOperationQueue;
class MUPnPResourceSelector;
class CUpnpItem;
class CPeriodic;

// CONSTANTS
const TInt KUriBuffer = 256;

/**
 * Class for handling renderer states
 *
 * @lib upnprenderingstatemachine.lib
 */
class CUpnpRenderingStateMachine
    : public CBase
    , public MUpnpRenderingOperationExecutor
    {
    
public: // construction / destruction

    /**
     * static constructor
     *
     * @param aSession session where to send volume events
     */
    IMPORT_C static CUpnpRenderingStateMachine* NewL(
        MUPnPAVRenderingSession& aSession );

    /**
     * Destructor
     */
    IMPORT_C virtual ~CUpnpRenderingStateMachine();

private: // construction, private part

    /**
     * static constructor
     */
    CUpnpRenderingStateMachine(
        MUPnPAVRenderingSession& aSession );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

public: // the interface

    /**
     * sets the state machine options (future extension)
     * @param aOptions options bit mask
     */
    IMPORT_C void SetOptions( TInt aOptions );

    /**
     * returns current options
     * @return the options bitmask
     */
    IMPORT_C TInt Options() const;

    /**
     * sets the state machine observer
     * @param aObserver the observer to set
     */
    IMPORT_C void SetObserver( MUpnpRenderingStateMachineObserver& aObserver );

    /**
     * removes the current observer
     */
    IMPORT_C void RemoveObserver();

    /**
     * sets the resource selector for media to play
     * if not called, TUPnPSelectDefaultResource will be used.
     */
    IMPORT_C void SetResourceSelector( MUPnPResourceSelector& aSelector );

    /**
     * Synchronises the state machine with the remote renderer
     * (updates the state)
     * this is an aynchronous operation and calls back RendererSyncReady()
     */
    IMPORT_C void SyncL();

    /**
     * Forces the state machine off sync
     * (sets the cached state to OffSync)
     */
    IMPORT_C void SetOffSync();

    /**
     * Checks if the state machine is synchronised
     * with the remote renderer ie. in operation
     * @return ETrue if state machine is operational
     */
    IMPORT_C TBool IsInSync() const;

    /**
     * Executes a command on the renderer.
     * If a command is currently running, the command is queued.
     * see Upnp::TCommand on upnprenderingstatemachineconstants.h
     * @param aCommand the command to execute
     * @param aCommandParameter command-specific parameter
     * @param aMedia the media to be played (specific to PLAY command)
     */
    IMPORT_C void CommandL(
        Upnp::TCommand aCommand,
        TInt aCommandParameter = 0,
        const CUpnpItem* aMedia = NULL
        );

    /**
     * Checks if the state machine is busy executing a command
     * @return ETrue if the state machine is busy
     */
    IMPORT_C TBool IsBusy() const;

    /**
     * Cancels currently ongoing operation and cleans up the
     * queue of operations
     */
    IMPORT_C void Cancel();

    /**
     * returns the current renderer state
     * Note: this is not a network operation - returns a cached value
     * @return the renderer state
     */
    IMPORT_C Upnp::TState State() const;

    /**
     * returns current track duration.
     * Note: this is NOT a network operation - returns a cached value.
     * NOTE ALSO: The method only works in case ERenderingOptionAutoSyncPos
     * option is in use or ERenderingCommandSyncPos has been called to
     * synchronise the position/duration.
     * @return the current track duration, or -1 if it is not available
     */
    IMPORT_C TInt Duration() const;

    /**
     * returns current position within the track.
     * Note: this is NOT a network operation - returns the position within an
     * internal position timer. The timer can be calibrated with the renderer
     * position using ECalibrate command, after which the Position values are
     * again more accurate.
     * @return the current estimated position, or -1 if it is not available
     */
    IMPORT_C TInt Position() const;

    /**
     * Tests if this renderer has pause capability.
     * Requesting PAUSE for a renderer that does not have this capability
     * wil result into an error.
     * @return ETrue if device has volume control capability.
     */
    IMPORT_C TBool HasPauseCapability() const;

public: // methods like in MUPnPRenderingSessionObserver

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void InteractOperationComplete(
        TInt aError, TUPnPAVInteractOperation aOperation );

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void PositionInfoResult( 
        TInt aError, const TDesC8& aPosition, const TDesC8& aLength );

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void SetURIResult( TInt aError );

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void SetNextURIResult( TInt aError );

public: // internal methods for the state algorithms

    /**
     * Reports SynReady to observer
     */
    void ReportSyncReady( TInt aError,
        Upnp::TState aNewState );

    /**
     * Reports StateChanged to observers
     */
    void ReportStateChanged( TInt aError,
        Upnp::TState aNewState,
        TBool aActionResponse = ETrue,
        TInt aStateParam = 0 );

    /**
     * Reports PositionSync to observers
     */
    void ReportPositionSync( TInt aError,
        Upnp::TPositionMode aMode,
        TInt aDuration, TInt aPosition );

private: // data

    // Rendering session
    MUPnPAVRenderingSession&            iSession;

    // the observer (no ownership!)
    MUpnpRenderingStateMachineObserver* iObserver;

    // the current state
    Upnp::TState                        iState;

    // rendering state machine option flags
    TInt                                iOptions;

    // playtime calculator
    CUpnpRenderingPlaytimeCalculator*   iPlaytimeCalculator;
    
    // state when buffering started with 'set uri' request
    Upnp::TState                        iStateBeforeSetUri;

    
/**************************
 ** State machine states **
 **************************/
    
public: // Operation execution methods

    /**
     * from MUpnpRenderingOperationExecutor
     */
    TResult ExecuteOperationL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );

    /**
     * from MUpnpRenderingOperationExecutor
     */
    TResult ProcessOperationL(
        const TUpnpRenderingOperation& aOperation, TInt aEvent,
        TInt aError, const TAny* aParam1, const TAny* aParam2 );

    /**
     * from MUpnpRenderingOperationExecutor
     */
    void HandleAsyncError(
        const TUpnpRenderingOperation& aOperation, TInt aError );

    /**
     * processing events
     * used in context of the operation queue
     */
    enum TOperationEvents
        {
        EInteractOperationComplete,
        EPositionInfo,
        ESetUriResult,
        ETimeout
        };
    
private: // internal state machinery

    // these are all called from the operation queue.
    // in all below methods, aOperation is the currently ongoing operation.

    // executes an operation
    TResult ExecuteOperationSyncL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationPlayL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationStopL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationPauseL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationResumeL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationRestartL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationSeekL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationCalibrateL(
        const TUpnpRenderingOperation& aOperation,
        const TUpnpRenderingOperation& aCurrent );
    TResult ExecuteOperationSetUriL(
            const TUpnpRenderingOperation& aOperation,
            const TUpnpRenderingOperation& aCurrent );

    // handles network events
    TResult ProcessPositionInfo(
        const TUpnpRenderingOperation& aOperation,
        TInt aError, const TDesC8& aPosition, const TDesC8& aLength );
    TResult ProcessSetURIResultL(
        const TUpnpRenderingOperation& aOperation, TInt aError );
    TResult ProcessAVEvents( 
            const TUpnpRenderingOperation& aOperation, 
            const TUPnPAVInteractOperation aIntOp,
            TInt aError );
    
protected: // internal

    // retrieves a resource from given media using the media selector
    const CUpnpElement& ResourceFromL( const CUpnpItem& aMedia );
    

    // user-defined resource selector (NOT OWNED!)
    MUPnPResourceSelector*              iSelector;

    // the default resource selector (used if not explicitly set by user)
    TUPnPSelectDefaultResource          iDefaultSelector;

    // currently playing HTTP URI
    TBuf8<KUriBuffer>                   iCurrentUri;
    
    
/***********
 ** Queue **
 ***********/
public: // methods

    /**
     * Count of operations in the queue
     * @return count of operations in the queue
     */
    TInt Size() const;

    /**
     * Checks if the queue is free for new operations
     * @return ETrue if new operation can be run immediately
     */
    TBool IsFree() const;

    /**
     * push operation to queue.
     * Don't even try to execute it at this stage.
     * @param aOperation the operation to be added
     */
    void PushL(
        const TUpnpRenderingOperation& aOperation );

    /**
     * push operation to queue and execute
     * @param aOperation the operation to be added
     */
    void PushAndExecuteL(
        const TUpnpRenderingOperation& aOperation );

    /**
     * executes the first operation in the queue
     * note: non-leaving version.
     * if the operation completes, continues recursively to next
     * until queue is empty
     */
    void Execute();

    /**
     * Processes an item in the queue with given data
     * if the operation completes, continues recursively to next
     * until queue is empty
     */
    void Process( TInt aEvent,
        TInt aError=0, const TAny* aParam1=NULL, const TAny* aParam2=NULL );
    
    /**
     * Finds and removes deprecated commands from queue.
     * E.g. If the last command is stop, then all other except the current 
     * (ongoing) and the stop command are removed.
     */
    void CompressQueueL();
    
    /**
     * removes all dup. commands but not the latest one and ongoing
     * 
     * @param aCommand commands to be removed from queue
     */
    void RemoveAllDublicateCommands( Upnp::TCommand aCommand );

public: // own methods

    /**
     * Returns currently ongoing operation. ENone, if queue empty
     * @return operation at first position
     */
    TUpnpRenderingOperation Current() const;


    /**
     * executes the current operation recursively
     * if the operation completes, continues recursively to next
     * until queue is empty
     */
    void ExecuteRecursivelyL();

    /**
     * executes an operation parallel to an ongoing operation
     */
    void ExecuteParallelL(
        const TUpnpRenderingOperation& aOperation );
    
private:
    
    void SetUriL( const TUpnpRenderingOperation& aOperation );
    
private:
    
    // the array that implements the queue
    RArray<TUpnpRenderingOperation>  iQueue;
    
    // Seek position to be used for playbacktimecalculator 
    // when seek is completed 
    TInt                             iSeekPostion;

    // Start timestamp for getting position; it is used to add 
    // half of the delay when get position request is made and 
    // when the response is received to avoid lag with the renderer  
    TTime                            iGetPositionStartMark;
    
    };


#endif // C_UPNPRENDERINGSTATEMACHINE_H

