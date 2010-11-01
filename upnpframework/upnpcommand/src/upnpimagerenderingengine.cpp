/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine for rendering images remotely
*
*/


// INCLUDE FILES
// upnp stack api
#include <upnpitem.h>                   // CUpnpItem
#include <upnpobject.h>                 // CUpnpObject (cast)

// upnpframework / avcontroller api
#include "upnpavrenderingsession.h"     // MUPnPAVRenderingSession
#include "upnpavsessionobserverbase.h" 

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h"           // KFilterCommon
#include "upnpitemresolver.h"           // MUPnPItemResolver
#include "upnpitemresolverobserver.h"   // MUPnPItemResolverObserver
#include "upnpitemresolverfactory.h"    // UPnPItemResolverFactory
#include "upnpitemutility.h"            // UPnPItemUtility::BelongsToClass

// upnpframework / commonui
#include "upnpcommonui.h"               // common UI for upnp video player dlg
#include "upnprenderingstatemachine.h"  // rendering state machine

// command internal
#include "upnpimagerenderingengineobserver.h"   // the observer interface
#include "upnpimagerenderingengine.h"   // self
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// CONSTANT DEFINITIONS
const TInt KReactionTimerMicrosec = 100000; // 100 millisec.

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::NewL
// --------------------------------------------------------------------------
//
CUpnpImageRenderingEngine* CUpnpImageRenderingEngine::NewL(
            MUPnPAVController& aAVController,
            MUPnPAVRenderingSession& aSession,
            MUpnpImageRenderingEngineObserver& aObserver )
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::NewL" );

    // Create instance
    CUpnpImageRenderingEngine* self = NULL;
    self = new (ELeave) CUpnpImageRenderingEngine(
            aAVController, aSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::CUpnpImageRenderingEngine
// --------------------------------------------------------------------------
//
CUpnpImageRenderingEngine::CUpnpImageRenderingEngine(
            MUPnPAVController& aAVController,
            MUPnPAVRenderingSession& aSession,
            MUpnpImageRenderingEngineObserver& aObserver )
    : iAVController( aAVController )
    , iRenderingSession( aSession )
    , iObserver( aObserver )
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine: Constructor" );

    // Initialise member variables
    iState = EIdle;
    iCurrentResolver = 0;
    iBufferedResolver = 0;
    iRenderingStateMachine = NULL;
    iWlanActive = ETrue;
    // set observer
    iRenderingSession.SetObserver( *this );
    }

// --------------------------------------------------------------------------
// Second phase constructor.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::ConstructL" );
    iTimer = CUPnPPeriodic::NewL( CActive::EPriorityStandard );
    iRenderingStateMachine = 
                  CUpnpRenderingStateMachine::NewL( iRenderingSession );
    iRenderingStateMachine->SetObserver( *this );
    iRenderingStateMachine->SyncL();
    }
    
// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpImageRenderingEngine::~CUpnpImageRenderingEngine()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine:\
        Destructor" );
    Cleanup();
    
    // Stop observing the rendering session
    iRenderingSession.RemoveObserver();

    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
~CUpnpImageRenderingEngine delete iCurrentResolver" );
    MUPnPItemResolver* tempCurrentResolver = iCurrentResolver;
    iCurrentResolver = NULL;
    delete tempCurrentResolver;
    
    if ( iRenderingStateMachine )
        {
        iRenderingStateMachine->RemoveObserver();
        delete iRenderingStateMachine;
        iRenderingStateMachine = NULL;
        }

    if( iTimer )
        {    
        iTimer->Cancel();
        delete iTimer;
        iTimer = 0;
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::Cleanup
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::Cleanup()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::Cleanup" );

    // reset state
    if ( iState != EShuttingDown )
        {
        iState = EIdle;
        }

    if( iTimer )
        {    
        iTimer->Cancel();
        }

    iBufferingNewImage = EFalse;
    // Delete resolvers
    // Delete for resolvers is done using temporary variables so that we can
    // first nullify the members and then delete the actual objects.
    // This is because local resolver deletion uses active scheduler loops
    // and therefore other asynchronous events may orrur. So we may end
    // up here in Cleanup again, during the resolver is being deleted.
    // if deletion is done the conventional way, the objects get deleted
    // twice, which is not what we want. :-)
    
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
Cleanup delete iBufferedResolver" );
    MUPnPItemResolver* tempBufferedResolver = iBufferedResolver;
    iBufferedResolver = NULL;
    delete tempBufferedResolver;

    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::Cleanup end" );
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::PlayL
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::PlayL()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::PlayL" );

    if ( iState != EShuttingDown )
        {
        if( iTimer->IsActive() )
            {
            __LOG( "[UpnpCommand]\t timer already active" );
            }
        else
            {
            TTimeIntervalMicroSeconds32 delay( KReactionTimerMicrosec );
            iTimer->Start( delay, delay, TCallBack( Timer, this ) );
            }
        }
    else
        {
        __LOG( "[UpnpCommand]\t not doing play in shutting down state" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::StopL
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::StopL()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::StopL" );

    // cancel any timers that are going on
    iTimer->Cancel();

    // remove buffered images
    iBufferingNewImage = EFalse;
    delete iBufferedResolver;
    iBufferedResolver = 0;

    switch( iState )
        {
        case EIdle:
        case EResolvingItem:
        case EResolveComplete:
            {
            // just cancel the sequence and do nothing
            iState = EIdle;
            break;
            }
        case ERendering: 
            {
            // Send stop action.
            iRenderingStateMachine->CommandL( Upnp::EStop, 0, NULL );
            break;
            }
        case EShuttingDown:
            {
            // command not allowed in this state
            break;
            }
        default:
            {
            __PANIC( __FILE__, __LINE__ );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::Timer
// timer callback
// --------------------------------------------------------------------------
//
TInt CUpnpImageRenderingEngine::Timer( TAny* aArg )
    {    
    CUpnpImageRenderingEngine* self =
        static_cast<CUpnpImageRenderingEngine*>( aArg );
    TRAPD( error, self->RunTimerL() )
    if ( error != KErrNone )
        {
        self->RunError( error );
        }
    return 0; // do not call again
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::RunTimerL
// Timer has triggered, start rendering media.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::RunTimerL()
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::RunTimerL, state %d",
        iState );
        
    iTimer->Cancel();

    delete iBufferedResolver;
    iBufferedResolver = iObserver.GetMedia();
    if ( iBufferedResolver == 0 )
        {
        __LOG( "[UpnpCommand]\t resolver returned zero" );
        User::Leave( KErrCancel );
        }

    switch( iState )
        {
        case EIdle: 
            {
            StartResolvingL();
            break;
            }
        case EResolvingItem: // fall through
        case EResolveComplete: // fall through
            {
            // indicate that new image is being buffered. It will be popped
            // from buffer in next callback.
            iBufferingNewImage = ETrue;
            break;
            }
        case ERendering:
            {
            // indicate that new image is being buffered. Send stop signal.
            // new item will be handled after stop completed.
            if( !iBufferingNewImage )
                {
                iBufferingNewImage = ETrue;
                iRenderingStateMachine->CommandL( Upnp::EStop, 0, NULL );
                }
            else
                {
                // stop already sent, wait for response and do nothing
                __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::RunTimerL,\
wait for stop" );
                }
            break;
            }
        case EShuttingDown:
            {
            // command not allowed in this state
            break;
            }
        default:
            {
            __PANIC( __FILE__, __LINE__ );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::RunError
// Exception occurred in the timer body
// --------------------------------------------------------------------------
//
TInt CUpnpImageRenderingEngine::RunError( TInt aError )
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RunError aError %d", aError );
    Cleanup();
    SendRenderAck( aError );
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::StartResolvingL
// Handles the start up of the item resolving.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::StartResolvingL()
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::StartResolvingL\
 in state %d",
        iState );

    __ASSERTD( iBufferedResolver, __FILE__, __LINE__ );
    if ( !iBufferedResolver )
        {
        // something is very wrong
        User::Leave( KErrDisconnected );
        }
    
    // delete old resolver
    // destruction takes time due to unsharing, so set to null first
    // so that this wont be used else where
    MUPnPItemResolver* tempCurrentResolver = iCurrentResolver;
    iCurrentResolver = NULL;
    delete tempCurrentResolver;
    
    // take queued resolver in use
    iCurrentResolver = iBufferedResolver;
    iBufferedResolver = NULL;
    iBufferingNewImage = EFalse;

    // Update the state
    iState = EResolvingItem;

    // Start resolving the item
    iCurrentResolver->ResolveL( *this );
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::ResolveComplete
// Indicates that resolving of an item is complete.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::ResolveComplete(
                                    const MUPnPItemResolver& aResolver,
                                    TInt aError )
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::ResolveComplete\
 aError %d", aError );
    
    // if engine is shutting down, no need to check these
    if ( iState == EResolvingItem )
        {
        __ASSERT( &aResolver == iCurrentResolver, __FILE__, __LINE__ );
        
        if( iBufferingNewImage )
            {
            TRAP( aError, StartResolvingL() );
            }
        else if( aError == KErrNone )
            {
            iState = ERendering;
            const CUpnpItem& item = iCurrentResolver->Item();

            if ( UPnPItemUtility::BelongsToClass( item, KClassImage ) )
                {
                __LOG1( "[UpnpCommand]\t play image item of resolver 0x%d", 
                                                    TInt(iCurrentResolver) );
                Cycle();
                }
            else if ( UPnPItemUtility::BelongsToClass( item, KClassVideo ) )
                {
                __LOG( "[UpnpCommand]\t video, inform observer to play it" );
                aError = iObserver.RenderAck( KErrNotSupported, &item );
                }
            }
            
        // error handling
        if( aError != KErrNone && iState != EShuttingDown )
            {
            SendRenderAck( aError );
            }
        }
    else if( iState == EShuttingDown )
        {
        // do nothing.
        iState = EIdle;
        }
    else
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine: state error." );
        __PANICD( __FILE__, __LINE__ );
        iState = EIdle;
        }

    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::SetURIResult
// UPnP AV Controller calls this method as a result for the 'set uri' request.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::SetURIResult( TInt aError )
    {    
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::SetURIResult\
 in aError %d", aError );
 
    if ( iState == ERendering )
        {
        if( iBufferingNewImage && aError == KErrNone)
            {
            __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
SetURIResult - pass cancel to rendering state machine" );
            iRenderingStateMachine->SetURIResult( KErrCancel );
            }
        else if( aError == KErrNone )
            {
            __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
SetURIResult - pass uri result to rendering state machine" );
            iRenderingStateMachine->SetURIResult( aError );
            }
        }
    else if ( iState == EShuttingDown )
        {
        // do nothing
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::InteractOperationComplete
// Called by UpnpAvController to indicate that play is complete.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::InteractOperationComplete(
    TInt aError,
    TUPnPAVInteractOperation aOperation )
    {
    __LOG2( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
InteractOperationComplete aOperation %d aError %d", aOperation, aError );
    if ( iState != EShuttingDown )
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
InteractOperationComplete - pass operation to rendering state machine" );
        iRenderingStateMachine->InteractOperationComplete( 
            aError, aOperation );
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::MediaRendererDisappeared
// Notifies that the Media Renderer we have a session with has disappeared.
// Session is now unusable and must be closed. 
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::MediaRendererDisappeared(
    TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
MediaRendererDisappeared in state %d", iState );

    if( iState == EShuttingDown )
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
        MediaRendererDisappeared engine already shutting down, do nothing" );
        }
    else
        {
        if( aReason == MUPnPAVSessionObserverBase::EWLANLost )
            {
            iWlanActive = EFalse;
            }
        iState = EShuttingDown; // avoid all callbacks
        iObserver.EngineShutdown( KErrDisconnected );
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::SendRenderAck
// Exception occurred in the timer body
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::SendRenderAck( TInt aError )
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
SendRenderAck(%d)", aError );

    // take a stack copy of some members
    MUpnpImageRenderingEngineObserver& observer = iObserver;
    const CUpnpItem* item = NULL;
    if ( iCurrentResolver  && 
        !( iState == EIdle || iState == EResolvingItem ) )
        {
        item = &iCurrentResolver->Item();
        }

    // cleanup if this was an error
    if ( aError != KErrNone )
        {
        __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
SendRenderAck aError=%d -> Cleanup", aError );
        Cleanup();
        }

    // call the observer
    TInt resp = observer.RenderAck( aError, item );

    // in case of disconnected error, do engine shutdown
    if ( resp == KErrDisconnected )
        {
        __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
SendRenderAck resp=%d -> EngineShutdown", resp );
        iState = EShuttingDown;
        observer.EngineShutdown( resp );
        }

    }
    
    
// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::IsWlanActive
// If connection to renderer is lost, checks if wlan is still active
// --------------------------------------------------------------------------
//
TBool CUpnpImageRenderingEngine::IsWlanActive()
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
IsWlanActive iWlanActive=%d ", iWlanActive );
    return iWlanActive;
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::Cycle
// Cycles next item by checking resolver status. 
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::Cycle()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::Cycle" );
    
    TRAPD( err, iRenderingStateMachine->CommandL( 
            Upnp::EPlay, 0, &iCurrentResolver->Item() ) );
            
    if ( KErrNone != err )
        {
        SendRenderAck( err );
        }
    }
    

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::RendererSyncReady
// Callback from rendering state machine when sync is ready.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::RendererSyncReady( TInt aError,
    Upnp::TState aState )
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RendererSyncReady aError=%d ", aError );

    if( aState != Upnp::EStopped )
        {
        // Renderer is used by another controlpoint. Cannot continue.
        iState = EShuttingDown; // avoid all callbacks
        iObserver.EngineShutdown( KErrInUse );
        }
    else if ( KErrNone != aError )
        {
        // notify observer if error. currently there will not
        // any error from rendering state machine but may be
        // in future. 
        SendRenderAck( aError );
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::RenderingStateChanged
// Callback from rendering state machine when rendering state changes.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::RenderingStateChanged( TInt aError, 
    Upnp::TState aState , TBool aActionResponse, TInt /*aStateParam*/ )
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged" );

    __LOG3( "[UpnpCommand]\t aError=%d aState=0x%x aActionResponse=%d",
                                        aError, aState, aActionResponse );

    if ( Upnp::EStopped == aState )
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged - image play stopped" );
        
        iState = EIdle;
        
        // new image waiting -> start resolving
        if( iBufferingNewImage )
            {
            __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged - start resolving new image");
            TRAP( aError, StartResolvingL() );
            }
        }
    else if ( Upnp::EPlaying == aState )
        {
        // new image waiting -> wait for stop
        // stop has already been sent
        if( iBufferingNewImage )
            {
            __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged to play - new image -> wait for stop");
            }
            
        // image playing -> inform observer  
        else if( aActionResponse &&  KErrNone == aError )
            {
            __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged - image play started ");
            SendRenderAck( aError );
            }
        }    
            
    // Error handling       
    if ( KErrNone != aError )
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RenderingStateChanged - Error situation" );
        SendRenderAck( aError );
        }
    }

// --------------------------------------------------------------------------
// CUpnpImageRenderingEngine::RendererSyncReady
// Callback from rendering state machine when rendering position is sync.
// --------------------------------------------------------------------------
//
void CUpnpImageRenderingEngine::PositionSync( TInt /*aError*/, 
        Upnp::TPositionMode /*aMode*/,
        TInt /*aDuration*/,
        TInt /*aPosition*/ )
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
PositionSync" );
    }
    
// End of File
