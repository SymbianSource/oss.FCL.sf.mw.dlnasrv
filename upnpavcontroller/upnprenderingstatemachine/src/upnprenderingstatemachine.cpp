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
* Description:  Implementation of generic upnp remote rendering state machine
*
*/

// INCLUDES
// dlnasrv / mediaserver api
#include <upnpitem.h>

// dlnasrv / avcontroller api
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"
#include "upnpitemutility.h"
#include "upnpconstantdefs.h"
#include "upnprenderingstatemachineconstants.h"
#include "upnprenderingstatemachineobserver.h"

// dlnasrv / component internal
#include "upnprenderingoperation.h"
#include "upnprenderingplaytimecalculator.h"
#include "upnpretraces.h"
#include "upnprenderingstatemachine.h"
#include "upnpavcontrollerglobals.h"

_LIT( KComponentLogfile, "upnprenderingstatemachine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt64 KMicrosecondsInMillisecond = 1000;
const TInt KSeekThresholdTime = 2000; //ms

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::NewL
// Static constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRenderingStateMachine* CUpnpRenderingStateMachine::NewL( 
    MUPnPAVRenderingSession& aSession )
    {
    CUpnpRenderingStateMachine* self =
        new (ELeave) CUpnpRenderingStateMachine( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::CUpnpRenderingStateMachine
// default constructor
// --------------------------------------------------------------------------
//
CUpnpRenderingStateMachine::CUpnpRenderingStateMachine( 
    MUPnPAVRenderingSession& aSession )
    : iSession( aSession )
    {
    iState = Upnp::EOffSync;
    iObserver = NULL;
    iOptions = 0;
    iSelector = 0;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ConstructL
// 2nd phase constructor
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ConstructL()
    {
    __LOG( "RenderingStateMachine::ConstructL" );
    iPlaytimeCalculator = new(ELeave) CUpnpRenderingPlaytimeCalculator();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::~CUpnpRenderingStateMachine
// destructor
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRenderingStateMachine::~CUpnpRenderingStateMachine()
    {
    __LOG( "RenderingStateMachine: ~CUpnpRenderingStateMachine" );
    delete iPlaytimeCalculator;
    iQueue.Close();
    iObserver = NULL;
    __LOG( "RenderingStateMachine: ~CUpnpRenderingStateMachine end" );
    }



// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetOptions
// sets the option flags for this state machine
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetOptions( TInt aOptions )
    {
    iOptions = aOptions;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Options
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpRenderingStateMachine::Options() const
    {
    return iOptions;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetObserver(
    MUpnpRenderingStateMachineObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::RemoveObserver()
    {
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetResourceSelector
// custom selection for resource inside item
// selects which resource is used in SetAvTransportUri
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetResourceSelector(
    MUPnPResourceSelector& aSelector )
    {
    __LOG( "RenderingStateMachine: SetResourceSelector" );
    iSelector = &aSelector;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SyncL
// synchronises the state machine with renderer
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SyncL()
    {
    __LOG( "CUpnpRenderingStateMachine::SyncL" );
    __ASSERT( !IsInSync(), __FILE__, __LINE__ );
    __ASSERT( !IsBusy(), __FILE__, __LINE__ );
    if ( IsInSync() )
        {
        User::Leave( KErrGeneral );
        }
    if ( IsBusy() )
        {
        User::Leave( KErrServerBusy );
        }
    TUpnpRenderingOperation sync(
        Upnp::ESync );
    PushAndExecuteL( sync );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetOffSync
// sets this state machine off sync
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetOffSync()
    {
    __LOG( "CUpnpRenderingStateMachine::SetOffSync" );
    __ASSERT( !IsBusy(), __FILE__, __LINE__ );
    iState = Upnp::EOffSync;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::IsInSync
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpRenderingStateMachine::IsInSync() const
    {
    return (iState & Upnp::EStateMaskInSync) != 0;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Command
// Issues a command to the renderer through the state machine
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::CommandL(
    Upnp::TCommand aCommand,
    TInt aCommandParameter,
    const CUpnpItem* aMedia )
    {
    __LOG1( "CUpnpRenderingStateMachine::CommandL cmd = %d",aCommand );
    TUpnpRenderingOperation cmd(
        aCommand, aCommandParameter, aMedia );
    PushAndExecuteL( cmd );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::IsBusy
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpRenderingStateMachine::IsBusy() const
    {
    return !IsFree();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::State
// --------------------------------------------------------------------------
//
EXPORT_C Upnp::TState CUpnpRenderingStateMachine::State() const
    {
    return iState;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Duration
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpRenderingStateMachine::Duration() const
    {
    __LOG( "CUpnpRenderingStateMachine::Duration" );
    __ASSERT( iState & Upnp::EStateMaskActive,
        __FILE__, __LINE__ );
    __ASSERT( iPlaytimeCalculator, __FILE__, __LINE__ );
    return iPlaytimeCalculator->Duration();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Position
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpRenderingStateMachine::Position() const
    {
    __LOG( "CUpnpRenderingStateMachine::Position" );
    __ASSERT( iState & Upnp::EStateMaskActive,
        __FILE__, __LINE__ );
    __ASSERT( iPlaytimeCalculator, __FILE__, __LINE__ );
    return iPlaytimeCalculator->Position();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::HasPauseCapability
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpRenderingStateMachine::HasPauseCapability() const
    {
    return iSession.Device().PauseCapability();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::InteractOperationComplete
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::InteractOperationComplete(
    TInt aError, TUPnPAVInteractOperation aOperation )
    {
    __LOG( "CUpnpRenderingStateMachine::InteractOperationComplete" );
    Process(
        CUpnpRenderingStateMachine::EInteractOperationComplete,
        aError, &aOperation );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::PositionInfoResult
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::PositionInfoResult( 
    TInt aError, const TDesC8& aPosition, const TDesC8& aLength )
    {
    __LOG( "CUpnpRenderingStateMachine::PositionInfoResult" );
    Process(
        CUpnpRenderingStateMachine::EPositionInfo,
        aError, (TAny*)&aPosition, (TAny*)&aLength );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetURIResult
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetURIResult( TInt aError )
    {
    __LOG( "CUpnpRenderingStateMachine::SetURIResult" );
    Process(
        CUpnpRenderingStateMachine::ESetUriResult,
        aError );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetNextURIResult
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRenderingStateMachine::SetNextURIResult( TInt /*aError*/ )
    {
    __LOG( "CUpnpRenderingStateMachine::SetNextURIResult" );
    __PANIC( __FILE__, __LINE__ );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ReportSyncReady
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ReportSyncReady(
    TInt aError, Upnp::TState aNewState )
    {
    __LOG3( "RenderingStateMachine: SyncReady err=%i %s->%s",
        aError, StateName( iState ), StateName( aNewState ) );
    iState = aNewState;
    if ( iObserver )
        {
        iObserver->RendererSyncReady( aError, iState );
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ReportStateChanged
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ReportStateChanged(
    TInt aError, Upnp::TState aNewState,
    TBool aActionResponse, TInt aStateParam )
    {
    __LOG3( "RenderingStateMachine: StateChanged err=%i %s->%s",
        aError, StateName( iState ), StateName( aNewState ) );
    iState = aNewState;
    if ( iObserver )
        {
        iObserver->RenderingStateChanged(
            aError, iState, aActionResponse, aStateParam );
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ReportPositionSync
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ReportPositionSync( TInt aError,
    Upnp::TPositionMode aMode,
    TInt aDuration, TInt aPosition )
    {
    __LOG1( "RenderingStateMachine: PositionSync %i", aError );
    if ( iObserver )
        {
        iObserver->PositionSync( aError, aMode, aDuration, aPosition );
        }
    }



/*****************************************
 ** State machine states implementation ** 
 *****************************************/

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationL(
    const TUpnpRenderingOperation& aOperation,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG1( "RenderingStateMachine: ExecuteOperationL (operation=%d)",
        aOperation.Command() );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    switch( aOperation.Command() )
        {
        case Upnp::ESync:
            {
            ret = ExecuteOperationSyncL( aOperation, aCurrent );
            break;
            }
        case Upnp::EPlay:
            {
            ret = ExecuteOperationPlayL( aOperation, aCurrent );
            break;
            }
        case Upnp::EStop:
            {
            ret = ExecuteOperationStopL( aOperation, aCurrent );
            break;
            }
        case Upnp::EPause:
            {
            ret = ExecuteOperationPauseL( aOperation, aCurrent );
            break;
            }
        case Upnp::EResume:
            {
            ret = ExecuteOperationResumeL( aOperation, aCurrent );
            break;
            }
        case Upnp::ERestart:
            {
            ret = ExecuteOperationRestartL( aOperation, aCurrent );
            break;
            }
        case Upnp::ESeek:
            {
            ret = ExecuteOperationSeekL( aOperation, aCurrent );
            break;
            }
        case Upnp::ECalibrate:
            {
            ret = ExecuteOperationCalibrateL( aOperation, aCurrent );
            break;
            }
        case Upnp::ESetUri:
            {
            ret = ExecuteOperationSetUriL( aOperation, aCurrent );
            break;
            }
        default:
            __PANIC( __FILE__, __LINE__ );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ProcessOperationL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ProcessOperationL(
    const TUpnpRenderingOperation& aOperation, TInt aEvent,
    TInt aError, const TAny* aParam1, const TAny* aParam2 )
    {
    __LOG2( "RenderingStateMachine: ProcessOperationL (event=%d) (aError=%d)",
            aEvent, aError );
    
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    switch( aEvent )
        {
        case EInteractOperationComplete:
            {
            TUPnPAVInteractOperation op =
                *(TUPnPAVInteractOperation*)aParam1;
            if ( op == EUPnPAVPlay 
              || op == EUPnPAVPlayUser 
              || op == EUPnPAVStop 
              || op == EUPnPAVStopUser 
              || op == EUPnPAVPause 
              || op == EUPnPAVPauseUser
              || op == EUPnPAVSeek )
                {
                ret = ProcessAVEvents( aOperation, op, aError );
                }
            else if ( op == EUPnPAVTransition )
                {
                ret = EContinue;
                __LOG1( "RenderingStateMachine: Transition %d", aError );
                }
            else
                {
                __PANIC( __FILE__, __LINE__ );
                }
            break;
            }
        case EPositionInfo: 
            {
            const TDesC8& pos = *(const TDesC8*)aParam1;
            const TDesC8& len = *(const TDesC8*)aParam2;
            ret = ProcessPositionInfo( aOperation, aError, pos, len );
            break;
            }
        case ESetUriResult:
            {
            ret = ProcessSetURIResultL( aOperation, aError );
            break;
            }
        default:
            {
            __PANIC( __FILE__, __LINE__ );
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::HandleAsyncError
// cancels the ongoing operation
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::HandleAsyncError(
    const TUpnpRenderingOperation& aOperation, TInt aError )
    {
    __LOG( "RenderingStateMachine: HandleAsyncError" );
    switch ( aOperation.Command() )
        {
        case Upnp::ESync:
            {
            ReportSyncReady( aError, State() );
            break;
            }
        case Upnp::EPlay:
            {
            if ( aOperation.IsUserOriented() )
                {
                ReportStateChanged( aError, 
                    Upnp::EStopped, ETrue );
                }
            break;
            }
        case Upnp::EStop:
            {
            if ( aOperation.IsUserOriented() )
                {
                ReportStateChanged( aError, State(), ETrue );
                }
            break;
            }
        case Upnp::EPause:
            {
            if ( aOperation.IsUserOriented() )
                {
                ReportStateChanged( aError, State(), ETrue );
                }
            break;
            }
        case Upnp::ECalibrate:
            {
            // do nothing
            break;
            }
        case Upnp::ESeek:
            {
            TInt seekCode = Upnp::EPositionChanged;
            ReportStateChanged( aError,
                    State(), ETrue, seekCode );
            break;
            }
        default:
           // __PANICD( __FILE__, __LINE__ );
            break;
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationSyncL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationSyncL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& /*aCurrent*/ )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationSyncL" );
    // GetRendererState() method to be implemented in AVController
    // 1. if renderer has already sent its first state event, it will be
    // returned immediately. 
    // 2. if not (SUBSCRIBE just sent) there will be a small wait loop to
    // wait for the initial event
    // 3. if the initial even does not arrive
    //    3.1 Request the state from the renderer (optional)
    //    3.2 Assume it is in stopped state. (maybe enough)
    
    TUPnPAVInteractOperation op = iSession.GetRendererStateL();
    __LOG1( "RenderingStateMachine: ExecuteOperationSyncL \
current op = %d" ,op );
    Upnp::TState state = Upnp::EPlaying;
    if( op == EUPnPAVStopUser )
        {
        state = Upnp::EStopped;
        }
    ReportSyncReady( KErrNone , state );
    return ECompleted;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationPlayL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationPlayL(
    const TUpnpRenderingOperation& aOperation,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationPlayL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent == Upnp::EPlay )
        {
        __LOG( "RenderingStateMachine: ExecuteOperationPlayL \
aCurrent == Upnp::EPlay " );
        // Play request repeats. Compare content.
        const CUpnpItem* item = static_cast<const CUpnpItem*>(
            aOperation.Data() );
        if ( ResourceFromL( *item ).Value() != iCurrentUri )
            {
            ret = EQueue;
            }
        }
    else if ( aCurrent != Upnp::ENone )
        {
        __LOG( "RenderingStateMachine: ExecuteOperationPlayL \
aCurrent == Upnp::ENone " );
        ret = EQueue;
        }
    else
        {
        switch( State() )
            {
            case Upnp::EPaused:
                {
                __LOG( "RenderingStateMachine: ExecuteOperationPlayL \
aCurrent == Upnp::EPaused " );
                
                const CUpnpItem* item =
                    static_cast<const CUpnpItem*>( aOperation.Data() );
                if ( item && ResourceFromL( *item ).Value() != iCurrentUri )
                    {
                    // track changed during pause -> stop paused and start 
                    // playing the new
                    TUpnpRenderingOperation stop( Upnp::EStop );
                    stop.SetUserOriented( EFalse );
                    PushL( stop );
                    TUpnpRenderingOperation play( aOperation );
                    PushL( play );
                    }
                else
                    {
                    iSession.PlayL();
                    ret = EContinue;
                    }
                break;
                }
            case Upnp::EPlaying:
            case Upnp::EStopped:
                {
                __LOG( "RenderingStateMachine: ExecuteOperationPlayL aCurrent == Upnp::EStopped " );

                // start play sequence
                const CUpnpItem* item = static_cast<const CUpnpItem*>(
                    aOperation.Data() );
                
                __LOG1( "RenderingStateMachine: ExecuteOperationPlayL item 0x%x ", item );
                
                if ( item )
                    {
                    // improvements !!!
                    // check if user requests playing the same track that was
                    // previously playing. In that case we just call Play().
                    // Much faster.
                    const TDesC8& httpuri = ResourceFromL( *item ).Value();
                    
                    __LOG( "RenderingStateMachine: ExecuteOperationPlayL current:" );
                    __LOG8( iCurrentUri );
                    __LOG( "RenderingStateMachine: ExecuteOperationPlayL new:" );
                    __LOG8( httpuri );
                    if( iCurrentUri == httpuri )
                        {
                        iSession.PlayL();
                        }
                    else
                        {
                        SetUriL( aOperation );
                        }
                    }
                else
                    {
                    // uri not available
                    User::Leave( KErrArgument );
                    }
                ret = EContinue;
                break;
                }
            case Upnp::EOffSync:  
            case Upnp::EBusy: 
            case Upnp::EDead:
            case Upnp::EBuffering:
            case Upnp::EStateMaskInSync:
            case Upnp::EStateMaskActive:
            case Upnp::EStateMaskRendering: 
            default:
                {
                User::Leave( KErrNotReady );
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationSetUriL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationSetUriL(
    const TUpnpRenderingOperation& aOperation,
    const TUpnpRenderingOperation& /*aCurrent*/ )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationSetUriL" );

    iStateBeforeSetUri = State(); 
    SetUriL( aOperation );

    return EContinue;
    }
    
// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::SetUriL
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::SetUriL( 
    const TUpnpRenderingOperation& aOperation )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationSetUriL" );
    
    const CUpnpItem* item = static_cast<const CUpnpItem*>(
        aOperation.Data() );
    
    __LOG1( "RenderingStateMachine: SetUriL item 0x%x ", item );
    
    if ( item )
        {
        const TDesC8& httpuri = ResourceFromL( *item ).Value();
        
        __LOG( "RenderingStateMachine: SetUriL current:" );
        __LOG8( iCurrentUri );
        __LOG( "RenderingStateMachine: SetUriL new:" );
        __LOG8( httpuri );

        __LOG( "RenderingStateMachine: SetUriL iSession.SetURIL" );
            
        iSession.SetURIL( httpuri, *item );
        iCurrentUri.Copy( httpuri );
        ReportStateChanged( KErrNone, Upnp::EBuffering );
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationStopL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationStopL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationStopL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent == Upnp::EStop )
        {
        // already stopping, no need to stop twice
        }
    else if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else
        {
        switch( State() )
            {
            case Upnp::EPlaying:
            case Upnp::EPaused:
                {
                iSession.StopL();
                ret = EContinue;
                break;
                }
            case Upnp::EStopped:
                {
                ReportStateChanged( KErrNone, State() );
                break;
                }
            case Upnp::EOffSync:  
            case Upnp::EBusy: 
            case Upnp::EDead:
            case Upnp::EBuffering:
            case Upnp::EStateMaskInSync:
            case Upnp::EStateMaskActive:
            case Upnp::EStateMaskRendering: 
            default:
                {
                User::Leave( KErrNotReady );
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationPauseL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationPauseL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationPauseL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent == Upnp::EPause )
        {
        // pause during pause request
        }
    else if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else
         {
         switch( State() )
             {
             case Upnp::EPlaying:
                 {
                 iSession.PauseL();
                 ret = EContinue;
                 break;
                 }
             case Upnp::EPaused:
                 {
                 ReportStateChanged( KErrNone, State() );
                 ret = ECompleted;
                 break;
                 }
             case Upnp::EOffSync:  
             case Upnp::EBusy: 
             case Upnp::EDead:
             case Upnp::EStopped:
             case Upnp::EBuffering:
             case Upnp::EStateMaskInSync:
             case Upnp::EStateMaskActive:
             case Upnp::EStateMaskRendering: 
             default:
                 {
                 User::Leave( KErrNotReady );
                 }
             }
         }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationResumeL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationResumeL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationResumeL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent == Upnp::EPlay )
        {
        // resume during play
        }
    else if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else
        {
        switch( State() )
            {
            case Upnp::EPlaying:
                {
                ReportStateChanged( KErrNone, State() );
                break;
                }
            case Upnp::EPaused:
                {
                iSession.PlayL();
                ret = EContinue;
                break;
                }
            case Upnp::EOffSync:  
            case Upnp::EBusy: 
            case Upnp::EDead:
            case Upnp::EStopped:
            case Upnp::EBuffering:
            case Upnp::EStateMaskInSync:
            case Upnp::EStateMaskActive:
            case Upnp::EStateMaskRendering: 
            default:
                {
                User::Leave( KErrNotReady );
                }
            }

        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationRestartL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationRestartL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationRestartL" );
    // handle parallel
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else
        {
        switch( State() )
            {
            case Upnp::EPlaying:
                {
                TUpnpRenderingOperation stop(
                    Upnp::EStop );
                stop.SetUserOriented( EFalse );
                PushL( stop );
                TUpnpRenderingOperation play(
                    Upnp::EPlay );
                PushL( play );
                break;
                }
            case Upnp::EPaused:
                {
                TUpnpRenderingOperation stop(
                     Upnp::EStop );
                 PushL( stop );
                break;
                }
            case Upnp::EOffSync:  
            case Upnp::EBusy: 
            case Upnp::EDead:
            case Upnp::EStopped:
            case Upnp::EBuffering:
            case Upnp::EStateMaskInSync:
            case Upnp::EStateMaskActive:
            case Upnp::EStateMaskRendering: 
            default:
                {
                User::Leave( KErrNotReady );
                }
            }

        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationSeekL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationSeekL(
    const TUpnpRenderingOperation& aOperation,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationSeekL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    
    TInt currentPosition = iPlaytimeCalculator->Position();
    TInt seekPosition = aOperation.Param();
    __LOG3( "RenderingStateMachine: state 0x%x, currentPosition %d, seekPosition %d",
            State(), currentPosition, seekPosition );
    if( seekPosition < currentPosition + KSeekThresholdTime &&
        seekPosition > currentPosition - KSeekThresholdTime )
        {
        // ignore seek
        }
    else if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else if ( State() == Upnp::EStopped ||
              State() == Upnp::EPlaying )
        {
        iSeekPostion = seekPosition;
        TTime seekTime = (TInt64)iSeekPostion * KMicrosecondsInMillisecond;
        iSession.SeekRelTimeL( seekTime );
        ret = EContinue;
        }
    else if( State() == Upnp::EPaused )
        {
        TInt lastIndex( iQueue.Count() - 1 );   
        TBool found(EFalse);
        for( TInt i(lastIndex); i > 0; i-- )
            {
            if( iQueue[i].Command() == Upnp::EPause )
                {
                break;
                }
            else if( iQueue[i].Command() == Upnp::EPlay || 
                     iQueue[i].Command() == Upnp::EResume )
                {
                __LOG( "RenderingStateMachine: ExecuteOperationSeekL \
                play or resume found adding seek to queue" );
                TUpnpRenderingOperation seek( aOperation );
                PushL( seek );
                found = ETrue;
                break;
                }
            }
        /**
         * if play or resume was not found it is 
         * illigal to execute seek in renderer 
         */
        if ( !found )
            {
            User::Leave( KErrNotReady );
            }
        }
    else
        {
        User::Leave( KErrNotReady );
        }    
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteOperationCalibrateL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult 
CUpnpRenderingStateMachine::ExecuteOperationCalibrateL(
    const TUpnpRenderingOperation& /*aOperation*/,
    const TUpnpRenderingOperation& aCurrent )
    {
    __LOG( "RenderingStateMachine: ExecuteOperationCalibrateL" );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aCurrent != Upnp::ENone )
        {
        ret = EQueue;
        }
    else if ( State() == Upnp::EPlaying ||
        State() == Upnp::EPaused )
        {
        iGetPositionStartMark.UniversalTime();
        iSession.GetPositionInfoL();
        ret = EContinue;
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ProcessEvents
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult
CUpnpRenderingStateMachine::ProcessAVEvents( 
                                   const TUpnpRenderingOperation& aOperation, 
                                    const TUPnPAVInteractOperation aIntOp,
                                    TInt aError )
    {
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if( !aError )
        {
        switch( aIntOp )
            {
            case EUPnPAVPlay:
                {
                if ( State() == Upnp::EPaused )
                    {
                    iPlaytimeCalculator->Resume();
                    }                
                else if ( !iPlaytimeCalculator->IsPlaying() )
                    {
                    iPlaytimeCalculator->Start();
                    }
                ReportStateChanged( aError,
                    Upnp::EPlaying );
                break;
                }
            case EUPnPAVPlayUser:
                {
                // spontaneous state change to playing
                if ( State() == Upnp::EStopped )
                    {
                    iPlaytimeCalculator->Start();
                    }
                else if ( State() == Upnp::EPaused )
                    {
                    iPlaytimeCalculator->Resume();
                    }
                ReportStateChanged( aError,
                    Upnp::EPlaying, EFalse );
                ret = EContinue;
                break;
                }
            case EUPnPAVStop:
                {
                iPlaytimeCalculator->Stop();
                ReportStateChanged( aError,
                    Upnp::EStopped , ETrue );
                break;
                }
            case EUPnPAVStopUser:
                {
                // spontaneous state change to stopped
                iPlaytimeCalculator->Stop();
                TInt stopcode = Upnp::ENoMedia;
                if( iPlaytimeCalculator->Duration() != KErrNotFound )
                    {
                    stopcode = ( iPlaytimeCalculator->IsTrackComplete() ?
                        Upnp::ETrackCompleted : 
                        Upnp::EStopRequested );
                    }
                ReportStateChanged( aError,
                    Upnp::EStopped, EFalse, stopcode );
                ret = EContinue;
                break;
                }
            case EUPnPAVPause:
                {
                iPlaytimeCalculator->Pause();
                ReportStateChanged( aError,
                    Upnp::EPaused );
                break;
                }
            case EUPnPAVPauseUser:
                {
                // spontaneous state change to paused
                iPlaytimeCalculator->Pause();
                ReportStateChanged( aError,
                    Upnp::EPaused, EFalse );
                ret = EContinue;
                break;
                }
            case EUPnPAVSeek:
                {
                iPlaytimeCalculator->RestartAt( iSeekPostion );
                TInt seekCode = Upnp::EPositionChanged;
                ReportStateChanged( aError,
                    State(), ETrue, seekCode );
                break;
                }
            default:
                {
                __LOG2( "RenderingStateMachine: aIntOp = %d event during %s",
                        aIntOp , CommandName(aOperation.Command()) );
                break;
                }
            }
        }
    else
        {
        ReportStateChanged( aError,
                            State() );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ProcessPositionInfo
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult
CUpnpRenderingStateMachine::ProcessPositionInfo( 
    const TUpnpRenderingOperation& aOperation,
    TInt aError, const TDesC8& aPosition, const TDesC8& aLength )
    {
    __LOG( "RenderingStateMachine: ProcessPositionInfo" );
    __ASSERT( aOperation == Upnp::ECalibrate,
        __FILE__, __LINE__ );

    TInt duration = 0;
    TInt position = 0;
    if ( aError == KErrNone )
        {
        aError = UPnPItemUtility::UPnPDurationAsMilliseconds(
            aLength, duration );
        }
    if ( aError == KErrNone )
        {
        aError = UPnPItemUtility::UPnPDurationAsMilliseconds(
            aPosition, position );
        }
     if ( aError == KErrNone )
        {
        //Incorporate the delay in receiving the response.
        TTime posStopMark; posStopMark.UniversalTime();
        TTimeIntervalMicroSeconds delay = 
                 posStopMark.MicroSecondsFrom( iGetPositionStartMark );
        TInt delayInMSec = ( delay.Int64() / KMicrosecondsInMillisecond );
        position += (delayInMSec/2); 
        if ( position > duration )
            {
            position = duration;
            }
        __LOG2( "RenderingStateMachine: ProcessPositionInfo added delay(%d) \
position(%d)", delayInMSec/2, position);
        iPlaytimeCalculator->AcknowledgePositionInfo(
            duration, position );
        }
    // response
    // trick states
    ReportPositionSync( aError,
        Upnp::EPlayingNormally,
        iPlaytimeCalculator->Duration(),
        iPlaytimeCalculator->Position() );
    return ECompleted;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ProcessSetURIResultL
// --------------------------------------------------------------------------
//
MUpnpRenderingOperationExecutor::TResult
CUpnpRenderingStateMachine::ProcessSetURIResultL(
    const TUpnpRenderingOperation& aOperation,
    TInt aError )
    {
    __LOG( "RenderingStateMachine: ProcessSetURIResultL" );
    TBool validOp( aOperation == Upnp::EPlay || aOperation == Upnp::ESetUri );
    __ASSERT( validOp, __FILE__, __LINE__ );
    MUpnpRenderingOperationExecutor::TResult ret(ECompleted);
    if ( aError == KErrNone )
        {
        // get duration from item and write to playtime calculator
        const CUpnpItem* item = static_cast<const CUpnpItem*>(
            aOperation.Data() );
        const CUpnpAttribute* attr =
            UPnPItemUtility::FindAttributeByName(
                ResourceFromL( *item ), KAttributeDuration );
        if ( attr )
            {
            TInt ms = 0;
            if ( UPnPItemUtility::UPnPDurationAsMilliseconds(
                attr->Value(), ms ) == KErrNone )
                {
                iPlaytimeCalculator->SetDuration( ms );
                }
            }
        if( aOperation.Command() == Upnp::EPlay )
            {
            iSession.PlayL();
            ret = EContinue;
            }
        else
            {
            ReportStateChanged( KErrNone, Upnp::EBuffering, EFalse, ETrue );
            iState = iStateBeforeSetUri;
            ret = ECompleted;
            }
        }
    //if seturi failed send first stop in that case!
    else if( aError == KErrAccessDenied || aError == KErrLocked )
        {
        iCurrentUri.Zero();
        iQueue.Remove( 0 ); // removes play from queue
        TUpnpRenderingOperation stop( Upnp::EStop );
        stop.SetUserOriented( EFalse );
        PushL( stop ); // add stop
        iSession.StopL();
        ret = EContinue; // continue = waits for operation to complete
        TUpnpRenderingOperation play( aOperation );
        PushL( play );
        }
    else
        {
        HandleAsyncError( aOperation, aError );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ResourceFrom
// --------------------------------------------------------------------------
//
const CUpnpElement& CUpnpRenderingStateMachine::ResourceFromL(
    const CUpnpItem& aItem )
    {
    const CUpnpElement* element( NULL );
    if ( iSelector )
        {
        element = &iSelector->SelectResourceL( aItem );
        }
    else
        {
        element = &iDefaultSelector.SelectResourceL( aItem );
        }
    return *element;
    }


/**************************
 ** Queue implementation ** 
 **************************/

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Size
// --------------------------------------------------------------------------
// 
TInt CUpnpRenderingStateMachine::Size() const
    {
    __LOG1( "CUpnpRenderingStateMachine::Size(%d)", iQueue.Count() );
    return iQueue.Count();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::IsFree
// Checks if the queue is free for new operations
// --------------------------------------------------------------------------
// 
TBool CUpnpRenderingStateMachine::IsFree() const
    {
    return Size() == 0;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::PushL
// push operation to queue. Don't try to execute it yet.
// --------------------------------------------------------------------------
// 
void CUpnpRenderingStateMachine::PushL(
    const TUpnpRenderingOperation& aOperation )
    {
    __LOG( "CUpnpRenderingStateMachine::PushL" );
    iQueue.AppendL( aOperation );
    CompressQueueL();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::PushAndExecuteL
// push operation to queue and execute it immediately
// --------------------------------------------------------------------------
// 
void CUpnpRenderingStateMachine::PushAndExecuteL(
    const TUpnpRenderingOperation& aOperation )
    {
    __LOG( "CUpnpRenderingStateMachine::PushAndExecuteL" );
    if ( IsFree() )
        {
        PushL( aOperation );
        ExecuteRecursivelyL();
        }
    else
        {
        ExecuteParallelL( aOperation );
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingAlgorithms::Execute
// Runs an operation in queue
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::Execute()
    {
    __LOG( "CUpnpRenderingStateMachine::Execute" );
    TUpnpRenderingOperation op = Current();
    if ( op.IsValid() )
        {
        __LOG1( "OperationQueue: Executing %s",
            CommandName(op.Command()) );
        MUpnpRenderingOperationExecutor::TResult result = ECompleted;
        TRAPD( leave, result = ExecuteOperationL( op,
            Upnp::ENone ) );
        __LOG1( "OperationQueue: leave code(%d) ", leave );

        if ( leave != KErrNone )
            {
            __LOG1( "OperationQueue: %s failed while executing",
                CommandName(op.Command()) );
            iQueue.Reset();
            HandleAsyncError( op, leave );
            }
        else if ( result == ECompleted )
            {
            __LOG1( "OperationQueue: %s completed while executing",
                CommandName(op.Command()) );
            if( iQueue.Count() > 0 )
                {
                iQueue.Remove( 0 );
                Execute(); // recur
                }
            }
        else if ( result == EContinue )
            {
            // operation continues
            }
        else
            {
            // illegal answer
            __PANIC( __FILE__, __LINE__ );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Process
// Abstract process of an operation
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::Process(
    TInt aEvent,
    TInt aError, const TAny* aParam1, const TAny* aParam2 )
    {
    __LOG( "CUpnpRenderingStateMachine::Process" );
    TUpnpRenderingOperation op = Current();
    MUpnpRenderingOperationExecutor::TResult result = ECompleted;
    __LOG2( "OperationQueue: Processing %s with event %d",
        CommandName(op.Command()), aEvent );
    __LOG( "CUpnpRenderingStateMachine::Process - call ProcessOperationL" );
    TRAPD( leave,
        result = ProcessOperationL( op, aEvent,
            aError, aParam1, aParam2 ) );
    if ( leave != KErrNone )
        {
        __LOG1( "OperationQueue: %s failed while processing",
            CommandName(op.Command()) );
        iQueue.Reset();
        HandleAsyncError( op, leave );
        }
    else if ( result == ECompleted )
        {
        __LOG2( 
        "OperationQueue: %s completed while processing iQueue count = %d"
         ,CommandName(op.Command()) , iQueue.Count() );
        if( iQueue.Count() > 0 )
            {
            iQueue.Remove( 0 );
            Execute(); // pop new
            }
        }
    else if ( result == EContinue )
        {
        // operation continues
        }
    else
        {
        // illegal answer
        __PANIC( __FILE__, __LINE__ );
        }
    }

void CUpnpRenderingStateMachine::CompressQueueL()
    {
    TInt queueCount( iQueue.Count() );  
    
    if( queueCount > 1 )
        {
        TInt lastIndex( queueCount - 1 );     
        
        switch( iQueue[lastIndex].Command() )
            {
            case Upnp::EStop:
                {
                /*
                
                TODO : check how to do later!! 
                 
                TInt removeStartIndex( 0 );
                
                if( iQueue[0] == Upnp::EStop )
                    {
                    // remove all except current ongoing stop command
                    removeStartIndex = lastIndex ;
                    }  
                else
                    {
                    // remove all except current ongoing command and last stop 
                    // command
                    removeStartIndex = lastIndex - 1;
                    }
                    
                for( TInt i = removeStartIndex; i > 0; i-- )
                    {
                    __LOG2( "OperationQueue: Removing %s from queue index %d",
                        CommandName( iQueue[i].Command() ), i );
                    iQueue.Remove( i );
                    } */
                    
                break;
                }        
            case Upnp::EPlay:
                {
               /* 
                
                TODO : check how to do later!! 
                
                TInt removeStartIndex( 0 );
                
                const CUpnpItem* item = static_cast<const CUpnpItem*>(
                    iQueue[lastIndex].Data() );
                
                if( iQueue[0] == Upnp::EPlay && 
                    item                     &&
                    ResourceFromL( *item ).Value() == iCurrentUri )
                    {
                    // remove all except current ongoing play command
                    removeStartIndex = lastIndex;
                    }
                else
                    {
                    // remove all except current ongoing command and 
                    // (last stop and) play command
                    if( queueCount > 2 &&
                        iQueue[lastIndex - 1] == Upnp::EStop )
                        {
                        // stop found before last play command
                        removeStartIndex = lastIndex - 2;
                        }
                    else
                        {
                        removeStartIndex = lastIndex - 1;
                        }
                    }
                    
                for( TInt i = removeStartIndex; i > 0; i-- )
                    {
                    __LOG2( "OperationQueue: Removing %s from queue index %d",
                        CommandName( iQueue[i].Command() ), i );
                    iQueue.Remove( i );
                    } 
                    */
                break;
                }
            case Upnp::ECalibrate:
                {
                RemoveAllDublicateCommands( iQueue[lastIndex].Command() );
                break;
                }
            case Upnp::ERestart:     
            case Upnp::EPause:
            case Upnp::ESeek:
            case Upnp::EResume: 
            case Upnp::ENone:                
            case Upnp::ESync:
            case Upnp::EClose:
            case Upnp::ENext:
            case Upnp::EPrev:
            case Upnp::EBack:
            case Upnp::EJump:
            default:
                {     
                //not implemented -> do nothing      
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::RemoveAllDublicateCommands
// --------------------------------------------------------------------------
// 
void CUpnpRenderingStateMachine::RemoveAllDublicateCommands( 
Upnp::TCommand aCommand )
    {
    __LOG1( "CUpnpRenderingStateMachine::RemoveAllDublicateCommands \
            command = %d" , aCommand );
    TInt lastIndex( iQueue.Count() - 1 );    

    for( TInt i(lastIndex-1); i > 0; i-- )
       {
       if( iQueue[i].Command() == aCommand )
           {
           __LOG1( "CUpnpRenderingStateMachine::RemoveAllDublicateCommands \
                  found dublicate command index = %d",i);
           // remove dublicate command
           iQueue.Remove( i );
           }
       } 
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::Current
// --------------------------------------------------------------------------
// 
TUpnpRenderingOperation CUpnpRenderingStateMachine::Current() const
    {
    __LOG( "CUpnpRenderingStateMachine::Current" );
    return ( iQueue.Count() > 0 ) ? iQueue[0] : Upnp::ENone;
    }

// --------------------------------------------------------------------------
// ResetArray
// --------------------------------------------------------------------------
// 
void ResetArray( TAny* iParam )
    {
    __LOG( "CUpnpRenderingStateMachine::ResetArray" );
    RArray<TUpnpRenderingOperation>* array =
        static_cast< RArray<TUpnpRenderingOperation>* >( iParam );
    array->Reset();
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteRecursivelyL
// Runs an operation first in the queue. If during the operation there
// were more operations added to queue, continues running recursively.
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ExecuteRecursivelyL()
    {
    __LOG( "CUpnpRenderingStateMachine::ExecuteRecursivelyL" );
    TUpnpRenderingOperation op = Current();
    if ( op.IsValid() )
        {
        __LOG1( "OperationQueue: Executing %s",
            CommandName(op.Command()) );
        // if the operation fails, OperationArrayClose method will 
        // clean up the entire operations queue !
        CleanupStack::PushL( TCleanupItem(
            ResetArray, &iQueue ) );
        MUpnpRenderingOperationExecutor::TResult result = 
            ExecuteOperationL( op,
                    Upnp::ENone );
        CleanupStack::Pop();
        switch( result )
            {
            case ECompleted:
                {
                __LOG1( "OperationQueue: %s completed while executing",
                    CommandName(op.Command()) );
                if( iQueue.Count() > 0 )
                    {
                    iQueue.Remove( 0 );
                    ExecuteRecursivelyL(); // recur
                    }
                break;
                }
            case EContinue:
                {
                // operation continues
                break;
                }
            case EQueue:
            default:
                {
                // illegal answer
                __PANIC( __FILE__, __LINE__ );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingStateMachine::ExecuteParallelL
// Runs an operation that was requested to put to queue. Checks if it
// completes right away or if it should wait for its turn to run.
// --------------------------------------------------------------------------
//
void CUpnpRenderingStateMachine::ExecuteParallelL(
    const TUpnpRenderingOperation& aOperation )
    {
    __LOG( "CUpnpRenderingStateMachine::ExecuteParallelL" );
    TUpnpRenderingOperation current = Current();
    __LOG1( "OperationQueue: Executing %s",
        CommandName(aOperation.Command()) );
    MUpnpRenderingOperationExecutor::TResult result = 
                    ExecuteOperationL( aOperation, current );
    switch( result )
        {
        case ECompleted:
            {
            __LOG1( "OperationQueue: %s completed while executing",
                CommandName(aOperation.Command()) );
            break;
            }
        case EQueue:
            {
            PushL( aOperation );
            __LOG1( "OperationQueue: %s added to queue",
                CommandName(aOperation.Command()) );
            break;
            }
        case EContinue:
        default:
            {
            // illegal answer
            __PANIC( __FILE__, __LINE__ );
            }
        }
    }
    
// end of file
