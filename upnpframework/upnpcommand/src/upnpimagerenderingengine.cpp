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

// command internal
#include "upnpimagerenderingengineobserver.h"   // the observer interface
#include "upnpimagerenderingengine.h"   // myself
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
    
    iWlanActive = ETrue;
    }
    
// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpImageRenderingEngine::~CUpnpImageRenderingEngine()
    {
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine: Destructor" );

    Cleanup();

    // Stop observing the rendering session
    iRenderingSession.RemoveObserver();

    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::~CUpnpImageRenderingEngine delete iCurrentResolver" );
    MUPnPItemResolver* tempCurrentResolver = iCurrentResolver;
    iCurrentResolver = NULL;
    delete tempCurrentResolver;
    
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
    
    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::Cleanup delete iBufferedResolver" );
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
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::PlayL in state %d",
        iState);

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
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::StopL in state %d",
        iState);

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
        case ESettingUri: // fall through
            {
            // just cancel the sequence and do nothing
            iState = EIdle;
            break;
            }
        case EStartingPlay:
            {
            // wait for PLAY complete, then do STOP
            // then wait for STOP complete
            iState = EStopping;
            break;
            }
        case EPlaying:
            {
            // Send stop action.
            iRenderingSession.StopL();
            iState = EStopping;
            break;
            }
        case EStopping:
            {
            // already stopping - do nothing
            break;
            }
        case EShuttingDown:
            {
            // command not allowed in this state
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
        self->RunError( error );
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
        case EIdle: // fall through
            {
            StartResolvingL();
            break;
            }
        case EResolvingItem: // fall through
        case EResolveComplete:
        case ESettingUri: // fall through
        case EStartingPlay:
            {
            // indicate that new image is being buffered. It will be popped
            // from buffer in next callback.
            iBufferingNewImage = ETrue;
            break;
            }
        case EPlaying:
            {
            // indicate that new image is being buffered. Send stop signal.
            // new item will be handled after stop completed.
            iBufferingNewImage = ETrue;
            iRenderingSession.StopL();
            iState = EStopping;
            break;
            }
        case EStopping:
            {
            // indicate that new image is being buffered. It will be popped
            // from buffer in next callback.
            iBufferingNewImage = ETrue;
            break;
            }
        case EShuttingDown:
            {
            // command not allowed in this state
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
 in state %d", iState );

    // if engine is shutting down, no need to check these
    if ( iState == EResolvingItem )
        {
        __ASSERTD( &aResolver == iCurrentResolver, __FILE__, __LINE__ );
        if( iBufferingNewImage )
            {
            TRAP( aError, StartResolvingL() );
            }
        else if( aError == KErrNone )
            {
            iState = EResolveComplete;
            
            // Now that we have the full metadata of the item available, we
            // can start the rendering
            TRAP( aError, InitiateShowingL() );
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
// CUpnpImageRenderingEngine::InitiateShowingL
// Handles the initiation of rendering (SetUri or video player launching).
// --------------------------------------------------------------------------
void CUpnpImageRenderingEngine::InitiateShowingL()
    {
    __LOG1( "[UpnpCommand]\t CUpnpImageRenderingEngine::InitiateShowingL\
 in state %d",
        iState );
    __ASSERTD( iCurrentResolver, __FILE__, __LINE__ );

    if ( UPnPItemUtility::BelongsToClass(
        iCurrentResolver->Item(), KClassImage ) )
        {
         // Send the setUri action
        iRenderingSession.SetURIL(
            iCurrentResolver->Resource().Value(),
            iCurrentResolver->Item() );
        // update the state
        iState = ESettingUri;
        }
    else
        {
        User::Leave( KErrNotSupported );
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
 in state %d",
        iState );

    if ( iState == ESettingUri )
        {
        //need check the aError in case of SetURIL cause a error.
        if( aError != KErrNone )
            {
            Cleanup();
            return;         
            }
        __ASSERTD( iCurrentResolver, __FILE__, __LINE__ );
        if( iBufferingNewImage )
            {
            TRAP( aError, StartResolvingL() );
            }
        else if( aError == KErrNone )
            {
            TRAP( aError, iRenderingSession.PlayL() );
            if( aError == KErrNone )
                {
                // Update the state
                iState = EStartingPlay;
                }
            }
        // error handling
        if( aError != KErrNone )
            {
            SendRenderAck( aError );
            }
        }
    else if ( iState == EShuttingDown )
        {
        // do nothing
        }
    else
        {
        __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine: state error." );
        __PANICD( __FILE__, __LINE__ );
        iState = EIdle;
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
InteractOperationComplete (%d) in state %d", aOperation, iState );

    if ( iState == EStartingPlay )
        {
        __ASSERTD( iCurrentResolver, __FILE__, __LINE__ );
        if( aOperation == EUPnPAVPlay && iBufferingNewImage )
            {
            // New image in buffer! call stop, then play new item.
            TRAP( aError, iRenderingSession.StopL() );
            if ( aError == KErrNone )
                {
                iState = EStopping;
                }
            }
        else if ( aOperation == EUPnPAVPlay && aError == KErrNone )
            {
            // update status
            iState = EPlaying;
            // response for play request
            SendRenderAck( KErrNone );
            }
        // error handling
        if ( aError != KErrNone )
            {
            SendRenderAck( aError );
            }
        }
    else if ( iState == EPlaying )
        {
        if( aOperation == EUPnPAVPlayUser )
            {
            // state change event notification
            // no need to do anything here
            }
        else if( aOperation == EUPnPAVStopUser )
            {
            // user stop notification
            // state to idle, so that no stop event will be sent
            // if starting to process new item
            iState = EIdle;
            }
        }
    else if ( iState == EStopping )
        {
        __ASSERTD( iCurrentResolver, __FILE__, __LINE__ );
        if( aOperation == EUPnPAVStop && iBufferingNewImage )
            {
            TRAP( aError, StartResolvingL() );
            }
        else if ( aOperation == EUPnPAVStop && aError == KErrNone )
            {
            // succesful stop - go IDLE
            iState = EIdle;
            }
        // error handling
        if ( aError != KErrNone )
            {
            SendRenderAck( aError );
            }
        }
    else if ( iState == EShuttingDown )
        {
        if ( aOperation == EUPnPAVStop || aOperation == EUPnPAVPlay )
            {
            iState = EIdle;
            }
        }

    __LOG( "[UpnpCommand]\t CUpnpImageRenderingEngine::InteractOperationComplete end " );
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
// CUpnpImageRenderingEngine::RunError
// Exception occurred in the timer body
// --------------------------------------------------------------------------
//
TInt CUpnpImageRenderingEngine::RunError( TInt aError )
    {
    __LOG2( "[UpnpCommand]\t CUpnpImageRenderingEngine::\
RunError in state %d aError %d", iState, aError );
    Cleanup();
    SendRenderAck( aError );
    return KErrNone;
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
    return iWlanActive;
    }
    
// End of File
