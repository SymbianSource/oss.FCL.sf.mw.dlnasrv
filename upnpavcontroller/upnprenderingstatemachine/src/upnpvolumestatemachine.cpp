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
* Description:  Implementation of generic upnp volume state machine
*
*/


// INCLUDES
// avcontroller api
#include <upnpavrenderingsession.h>
#include <upnpavdevice.h> // for device.VolumeCapability()
// volume state machine
#include "upnprenderingstatemachineconstants.h" // option flags
#include "upnpvolumestatemachineobserver.h"
#include "upnpvolumestatemachine.h"

_LIT( KComponentLogfile, "upnprenderingengine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KCancelTimeMaximum = 3000000;
const TInt KCancelTimeResolution = 500000;

// ======== MEMBER FUNCTIONS ========


TUpnpVSMQueueItem::TUpnpVSMQueueItem( 
    const TUpnpVSMQueueItem::TPropertyType aProperty, const TInt aValue )
    : iProperty( aProperty )
    , iValue( aValue )
    {    
    }

TUpnpVSMQueueItem::TPropertyType TUpnpVSMQueueItem::Property() const
    {
    return iProperty;
    }
    
TInt TUpnpVSMQueueItem::Value() const
    {
    return iValue;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::NewL
// Static constructor.
// --------------------------------------------------------------------------
// 
EXPORT_C CUpnpVolumeStateMachine* CUpnpVolumeStateMachine::NewL( 
    MUPnPAVRenderingSession& aSession )
    {
    CUpnpVolumeStateMachine* self =
        new (ELeave) CUpnpVolumeStateMachine( aSession );
    // no 2nd phase construction needed
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::CUpnpVolumeStateMachine
// Default constructor.
// --------------------------------------------------------------------------
// 
CUpnpVolumeStateMachine::CUpnpVolumeStateMachine( 
    MUPnPAVRenderingSession& aSession )
    : iSession( aSession )
    {
    __LOG( "VolumeStateMachine: constructor" );
    iVolumeCapability = iSession.Device().VolumeCapability();
    iMuteCapability = iSession.Device().MuteCapability();
    iState = EOffSync;
    iCurrentVolume = KErrNotFound;
    iVolumeToSetAfterMute = KErrNotFound;
    iCurrentMute = KErrNotFound;
    iMuteRequestedByClient = EFalse;
    iCachedVolume = KErrNotFound;
    iCachedMute = KErrNotFound;
    }


// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::~CUpnpVolumeStateMachine
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpVolumeStateMachine::~CUpnpVolumeStateMachine()
    {
    __LOG( "VolumeStateMachine: destructor" );
    }


// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SetOptions
// sets the option flags for this state machine
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SetOptions( TInt aOptions )
    {
    iOptions = aOptions;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::Options
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpVolumeStateMachine::Options() const
    {
    return iOptions;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SetObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SetObserver(
    MUpnpVolumeStateMachineObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::RemoveObserver()
    {
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SyncL
// Synchronises this state machine with the renderer
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SyncL()
    {
    __LOG( "VolumeStateMachine: SyncL" );
    
    if ( iState != EOffSync && iState != EIdle )
        {
        __LOG( "VolumeStateMachine: not synchronizing" );
        return;
        }
    
    if ( !iVolumeCapability )
        {
        User::Leave( KErrNotSupported );
        }

    iSession.GetVolumeL();
    iState = ESyncing;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SetOffSync
// Forces the state machine off sync
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SetOffSync()
    {
    __ASSERTD( !IsBusy(), __FILE__, __LINE__ );
    iState = EOffSync;
    iCurrentVolume = KErrNotFound;
    iVolumeToSetAfterMute = KErrNotFound;
    iCurrentMute = KErrNotFound;
    iMuteRequestedByClient = EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::IsInSync
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpVolumeStateMachine::IsInSync() const
    {
    return iState != EOffSync;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SetVolumeL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SetVolumeL( TInt aVolume )
    {
    __LOG1( "VolumeStateMachine: SetVolumeL %d", aVolume );
    __ASSERTD( IsInSync(), __FILE__, __LINE__ );
    if ( !iVolumeCapability )
        {
        User::Leave( KErrNotSupported );
        }
    
    if ( IsBusy() )
        {
        PushIntoQueueL( TUpnpVSMQueueItem::EVolume, aVolume );
        return;
        }

    // check volume is between given limits
    aVolume = Max( aVolume, KVolumeMin );
    aVolume = Min( aVolume, KVolumeMax );

    if ( aVolume == KVolumeMin && iCurrentVolume > 0 &&
        iMuteCapability && iCurrentMute == KMuteOff &&
        iOptions & Upnp::EConvertVolumeZeroToMute )
        {
        // adjust volume to zero (using mute)
        iSession.SetMuteL( ETrue );
        iState = EAdjustingVolumeToZero;
        }
    else if ( aVolume == KVolumeMin && iCurrentMute == KMuteOn )
        {
        // no need to change volume. Call back directly.
        if ( iObserver )
            {
            iObserver->VolumeChanged(
                KErrNone, iCurrentVolume, ETrue );
            }
        }
    else if ( aVolume > KVolumeMin && iCurrentMute == KMuteOn &&
        !iMuteRequestedByClient )
        {
        // volume is at zero, and renderer is muted.
        // unmute and after that change volume if needed
        iVolumeToSetAfterMute = aVolume;
        iSession.SetMuteL( EFalse );
        iState = EAdjustingVolumeFromZero;
        }
    else if ( aVolume != iCurrentVolume )
        {
        // adjust volume normally
        iSession.SetVolumeL( aVolume );
        iState = EAdjustingVolume;
        }
    else
        {
        // no need to change volume. Call back directly.
        if ( iObserver )
            {
            iObserver->VolumeChanged(
                KErrNone, iCurrentVolume, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::Volume
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpVolumeStateMachine::Volume() const
    {
    __ASSERTD( IsInSync(), __FILE__, __LINE__ );
    TInt vol = iCurrentVolume;
    if ( iCurrentMute && (iOptions & Upnp::EConvertVolumeZeroToMute) )
        {
        vol = KVolumeMin;
        }
    return vol;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::SetMuteL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::SetMuteL( TBool aMuteState )
    {
    __LOG1( "VolumeStateMachine: SetMuteL", aMuteState );
    __ASSERTD( IsInSync(), __FILE__, __LINE__ );
    if ( !iMuteCapability )
        {
        // Could implement here FAKE MUTE SUPPORT by converting mute
        // to Volume(0). However there is no need to that since no clients
        // use the mute feature.
        User::Leave( KErrNotSupported );
        }
    if ( IsBusy() )
        {
        PushIntoQueueL( TUpnpVSMQueueItem::EMute, aMuteState );
        return;
        }

    // check mute state is between given limits
    aMuteState = Max( aMuteState, KMuteOff );
    aMuteState = Min( aMuteState, KMuteOn );

    if ( aMuteState && !iCurrentMute )
        {
        // mute
        iSession.SetMuteL( ETrue );
        iState = EAdjustingMute;
        }
    else if ( !aMuteState && iCurrentMute )
        {
        // unmute
        iSession.SetMuteL( EFalse );
        iState = EAdjustingMute;
        }
    else
        {
        // no change in mute state. Call back directly.
        if ( iObserver )
            {
            iObserver->MuteChanged(
                KErrNone, iCurrentMute, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::Mute
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpVolumeStateMachine::Mute() const
    {
    __ASSERTD( IsInSync(), __FILE__, __LINE__ );
    return ( (iCurrentMute == KMuteOn) ? KMuteOn : KMuteOff );
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::IsBusy
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpVolumeStateMachine::IsBusy() const
    {
    return iState != EIdle;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::HasVolumeCapability
// tests if renderer has volume capability
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpVolumeStateMachine::HasVolumeCapability() const
    {
    return iVolumeCapability;
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::Cancel
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::Cancel()
    {
    // actually we can not cancel anything, but providing the interface for
    // future purposes
    if ( iState != EIdle )
        {
        __LOG1( "VolumeStateMachine: Canceling in state %d", iState );
        iState = ECancelled;
        for ( TInt t = KCancelTimeMaximum;
            t > 0 && iState != EIdle;
            t -= KCancelTimeResolution )
            {
            User::After( TTimeIntervalMicroSeconds32(
                KCancelTimeResolution ) );
            }
        __LOG1( "VolumeStateMachine: state after cancel: %d", iState );
        }
    }

void CUpnpVolumeStateMachine::HandleVolumeResultInSyncState(
    TInt aError, TInt aVolumeLevel, TBool /*aActionResponse*/ )
    {
    if ( aError == KErrNone )
        {
        iCurrentVolume = aVolumeLevel;
        if ( iMuteCapability )
            {
            // continue to sync mute
            TRAPD( muteError, iSession.GetMuteL() );
            if ( muteError != KErrNone )
                {
                // error syncing mute - callback
                iState = EIdle;
                if ( iObserver )
                    {
                    iObserver->VolumeSyncReady( muteError );
                    }
                }
            }
        else
            {
            // mute not needed - callback
            iCurrentMute = KMuteOff; // assume always off
            iState = EIdle;
            // notify sync initially and volume if changed
            if ( iObserver && iCachedVolume == KErrNotFound )
                {
                iObserver->VolumeSyncReady( KErrNone );
                }
            else if ( iObserver && iCachedVolume != iCurrentVolume )
                {
                iObserver->VolumeChanged(
                    aError, iCurrentVolume, EFalse );
                }
            }
        }
    else
        {
        // error callback
        iState = EIdle;
        if ( iObserver )
            {
            iObserver->VolumeSyncReady( aError );
            }
        }
    
    }



// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::VolumeResult
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::VolumeResult(
    TInt aError, TInt aVolumeLevel, TBool aActionResponse )
    {
    __LOG3( "VolumeStateMachine: VolumeResult %d, %d state=%d",
        aError, aVolumeLevel, iState );

    //
    // RESPONSE TO A REQUEST
    //
    if ( aActionResponse )
        {
        //
        // SYNC
        //
        if( iState == ESyncing )
            {
            HandleVolumeResultInSyncState(aError, aVolumeLevel, aActionResponse);            
            }
        //
        // VOLUME ADJUST
        //
        else if ( iState == EAdjustingVolume ||
            iState == EAdjustingVolumeFromZero )
            {
            if ( aError == KErrNone )
                {
                iCurrentVolume = aVolumeLevel;
                }
            iState = EIdle;
            // volume adjust done, call back
            if ( iObserver )
                {
                iObserver->VolumeChanged(
                    aError, iCurrentVolume, ETrue );
                }
            }
        //
        // CANCEL
        //
        else if( iState == ECancelled )
            {
            iState = EIdle;
            }
        }

    //
    // UNSOLICTED VOLUME EVENT
    //
    else
        {
        if ( iState == EIdle )
            {
            if ( aVolumeLevel != iCurrentVolume )
                {
                if ( iCurrentMute == KMuteOn )
                    {
                    __LOG("unsolicted volume event during muted");
                    }
                iCurrentVolume = aVolumeLevel;
                if ( iObserver )
                    {
                    iObserver->VolumeChanged(
                        KErrNone, iCurrentVolume, EFalse );
                    }
                }
            }
        else
            {
            __LOG("ignoring unsolicted volume event in this state");
            }
        }

    iCachedVolume = iCurrentVolume;
    ProcessNextQueuedProperty();
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::MuteResult
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::MuteResult(
    TInt aError, TBool aMute, TBool aActionResponse )
    {
    __LOG3( "VolumeStateMachine: MuteResult %d, %d, %d",
        aError, aMute, aActionResponse );
    __LOG1( "VolumeStateMachine: MuteResult in state=%d", iState );

    //
    // RESPONSE TO A REQUEST
    //
    if( aActionResponse )
        {
        //
        // SYNC
        //
        if ( iState == ESyncing )
            {
            if ( aError == KErrNone )
                {
                iCurrentMute = aMute;
                iMuteRequestedByClient = EFalse;
                }
            iState = EIdle;
            // notify sync initially and volume and mute if changed
            if ( iObserver && iCachedMute == KErrNotFound )
                {
                iObserver->VolumeSyncReady( aError );
                }
            else if ( iObserver )
                {
                if( iCachedVolume != iCurrentVolume )
                    {
                    iObserver->VolumeChanged(
                            aError, iCurrentVolume, EFalse );
                    }
                if( iCachedMute != iCurrentMute )
                    {
                    iObserver->MuteChanged(
                            aError, iCurrentMute, EFalse );
                    }
                }
            }
        //
        // MUTE ADJUST
        //
        if ( iState == EAdjustingMute )
            {
            if ( aError == KErrNone )
                {
                iCurrentMute = aMute;
                iMuteRequestedByClient = aMute;
                }
            iState = EIdle;
            if ( iObserver )
                {
                iObserver->MuteChanged(
                    aError, iCurrentMute, ETrue );
                }
            }
        //
        // VOLUME ADJUST -> 0
        //
        if ( iState == EAdjustingVolumeToZero )
            {
            if ( aError == KErrNone )
                {
                iCurrentMute = aMute;
                iMuteRequestedByClient = EFalse;
                }
            iState = EIdle;
            if ( iObserver )
                {
                iObserver->VolumeChanged(
                    aError, KVolumeMin, ETrue );
                }
            }
        //
        // VOLUME ADJUST 0 -> up
        //
        if ( iState == EAdjustingVolumeFromZero )
            {
            if ( aError == KErrNone )
                {
                iCurrentMute = aMute;
                iMuteRequestedByClient = EFalse;
                TRAP( aError, iSession.SetVolumeL(
                    iVolumeToSetAfterMute ) );
                }
            if ( aError != KErrNone )
                {
                // error callback
                iVolumeToSetAfterMute = KErrNotFound;
                iState = EIdle;
                if ( iObserver )
                    {
                    iObserver->VolumeChanged(
                        aError, iCurrentVolume, ETrue );
                    }
                }
            }
        //
        // CANCEL
        //
        else if( iState == ECancelled )
            {
            iState = EIdle;
            }
        }

    //
    // UNSOLICITED MUTE EVENT
    //
    else
        {
        HandleUnsolicitedMuteEvent(aError, aMute, aActionResponse );
        }
    
    iCachedMute = iCurrentMute;
    ProcessNextQueuedProperty();
    }

void CUpnpVolumeStateMachine::HandleUnsolicitedMuteEvent(
    TInt aError, TBool aMute, TBool aActionResponse )
    {
    if ( iState == EIdle )
        {
        HandleUnsolicitedMuteEventInIdle(aError, aMute, aActionResponse);
        }
    else
        {
        __LOG("ignoring unsolicted mute event in this state");
        }
    
    }

void CUpnpVolumeStateMachine::HandleUnsolicitedMuteEventInIdle(
    TInt /*aError*/, TBool aMute, TBool /*aActionResponse*/ )
    {
    __LOG3( "VolumeStateMachine: HandleUnsolicitedMuteEventInIdle %d, %d, %d",
        aMute, iCurrentMute, iMuteRequestedByClient );

    if ( iCurrentMute && !aMute && iMuteRequestedByClient )
        {
        // client has requested mute, renderer unmuted
        iCurrentMute = aMute;
        // check if client has changed volume during mute
        if ( iVolumeToSetAfterMute > 0 &&
            iVolumeToSetAfterMute != iCurrentVolume )
            {
            TRAP_IGNORE(
                iSession.SetVolumeL( iVolumeToSetAfterMute );
                iVolumeToSetAfterMute = KErrNotFound;
                iState = EAdjustingVolume;
                );
            }
        // notify observer about mute state change
        if ( iObserver )
            {
            iObserver->MuteChanged(
                KErrNone, KMuteOff, EFalse );
            }
        }
    else if ( iCurrentMute && !aMute ||
        !iCurrentMute && aMute )
        {
        // renderer unmuted - notify volume state back
        iCurrentMute = aMute;
        // check if client has changed volume during mute
        if ( iVolumeToSetAfterMute > 0 &&
            iVolumeToSetAfterMute != iCurrentVolume &&
            !aMute &&
            ( ( (iOptions & Upnp::EConvertVolumeZeroToMute) == 0 ) ||
              iMuteRequestedByClient ) )
            {
            __LOG2( "After unmute adjusting volume %d->%d",
                iCurrentVolume, iVolumeToSetAfterMute );
            TRAP_IGNORE(
                iSession.SetVolumeL( iVolumeToSetAfterMute );
                iVolumeToSetAfterMute = KErrNotFound;
                iState = EAdjustingVolume;
                );
            }
        // notify observer
        if ( iObserver )
            {
            NotifyObserver(aMute);            
            }
        }
    else
        {
        // no need to report status change
        }    
    }

void CUpnpVolumeStateMachine::NotifyObserver(TBool aMute)
    {
    __LOG2( "VolumeStateMachine: NotifyObserver %d, %d",
        aMute, iOptions );

    if ( ( (iOptions & Upnp::EConvertVolumeZeroToMute) == 0 ) ||
         iMuteRequestedByClient )
        {
        // client prefers to use mute, or client has 
        // explicitely requested this mute session.
        iObserver->MuteChanged(
            KErrNone, aMute, EFalse );
        }
    else
        {
        // convert mute signal to volume callback
        TInt givenvol = 
                ( aMute ? KVolumeMin : iCurrentVolume );
        iObserver->VolumeChanged(
            KErrNone, givenvol, EFalse );
        }
    }



// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::CopyValues
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpVolumeStateMachine::CopyValues(
    const CUpnpVolumeStateMachine& aOther )
    {
    iCurrentVolume = aOther.Volume();
    iCurrentMute = aOther.Mute();
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::PushIntoQueueL
// --------------------------------------------------------------------------
//
void CUpnpVolumeStateMachine::PushIntoQueueL( 
    const TUpnpVSMQueueItem::TPropertyType aPropery, const TInt aValue )
    {
    __LOG( "VolumeStateMachine: PushIntoQueueL" );
    TUpnpVSMQueueItem queued( aPropery, aValue ); 
    iQueue.AppendL( queued );    
    
    CompressQueue();
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::CompressQueueL
// --------------------------------------------------------------------------
//
void CUpnpVolumeStateMachine::CompressQueue()
    {
    TInt queueCount( iQueue.Count() );  
    
    if( queueCount > 1 )
        {
        __LOG1( "VolumeStateMachine: CompressQueue (queue count: %d)", queueCount );
        
        TInt lastIndex( queueCount - 1 );
        
        if( iQueue[lastIndex].Property() == TUpnpVSMQueueItem::EVolume )
            {
            // remove all other except the last 'set volume' request
            for( TInt i( queueCount - 2 ); i >= 0; --i )
                {
                __LOG2( "VolumeStateMachine Queue: Removing %d from queue index %d",
                       iQueue[i].Property(), i );   
                
                iQueue.Remove( i );
                }
            }
        else
            {
            // remove all other except (the last 'set volume' request and)
            // the last 'set mute' request
            TBool setVolumeFound( EFalse );
            TBool setMuteFound( EFalse );
            for( TInt i( queueCount - 1 ); i >= 0; --i )
                {                          
                if( iQueue[i].Property() == TUpnpVSMQueueItem::EVolume &&
                    !setVolumeFound )                        
                    {
                    setVolumeFound = ETrue;
                    }
                else if( iQueue[i].Property() == TUpnpVSMQueueItem::EMute &&
                         !setMuteFound )               
                    {
                    setMuteFound = ETrue;
                    }
                else
                    {
                    __LOG2( "VolumeStateMachine Queue: Removing %d from queue index %d",
                        iQueue[i].Property(), i );
                    
                    iQueue.Remove( i );
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpVolumeStateMachine::ProcessNextQueuedProperty
// --------------------------------------------------------------------------
//
void CUpnpVolumeStateMachine::ProcessNextQueuedProperty()
    {
    if( iQueue.Count() > 0 )
        {        
        __LOG( "VolumeStateMachine: ProcessNextQueuedPropertyL" );
        
        TUpnpVSMQueueItem queuedItem( iQueue[0] );
        iQueue.Remove( 0 );
        
        if( queuedItem.Property() == TUpnpVSMQueueItem::EVolume )
            {
            TRAPD( err, SetVolumeL( queuedItem.Value() ) );
            if( err )
                {
                iObserver->VolumeChanged( err, iCurrentVolume, ETrue ); 
                }
            }
        else
            {
            TRAPD( err, SetMuteL( queuedItem.Value() ) );
            if( err )
                {
                iObserver->MuteChanged( err, iCurrentMute, ETrue ); 
                }            
            }
        }
    }

// end of file
