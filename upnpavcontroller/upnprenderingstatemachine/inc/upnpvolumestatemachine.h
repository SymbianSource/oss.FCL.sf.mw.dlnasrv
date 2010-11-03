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
* Description:  Generic upnp volume state machine
*
*/


#ifndef C_UPNPVOLUMESTATEMACHINE_H
#define C_UPNPVOLUMESTATEMACHINE_H

// INCLUDES
#include <e32base.h>

#include "upnprenderingstatemachineconstants.h"

// FORWARD DECLARATIONS
class MUPnPAVRenderingSession;
class MUpnpVolumeStateMachineObserver;

// CONSTANTS
const TInt KVolumeMin = 0;
const TInt KVolumeMax = 100;
const TInt KMuteOff = 0;
const TInt KMuteOn = 1;


class TUpnpVSMQueueItem
    {
    
public:
    
    enum TPropertyType
        {
        EVolume,
        EMute
        };
    
public:
    
    TUpnpVSMQueueItem( const TUpnpVSMQueueItem::TPropertyType aProperty, 
        const TInt aValue );
    
public:
    
    TUpnpVSMQueueItem::TPropertyType Property() const;
    
    TInt Value() const;
    
private:
    
    const TUpnpVSMQueueItem::TPropertyType iProperty;
          
    const TInt iValue;
    
    };


/**
 * Class for handling renderer volume
 *
 * @lib upnpavcontrollerhelper.lib
 * @since S60 v3.2
 */
class CUpnpVolumeStateMachine 
    : public CBase
    {
    
public: // construction / destruction

    /**
     * static constructor
     *
     * @since Series 60 3.2
     * @param aSession session where to send volume events
     */
    IMPORT_C static CUpnpVolumeStateMachine* NewL(
        MUPnPAVRenderingSession& aSession );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    IMPORT_C virtual ~CUpnpVolumeStateMachine();

private: // construction, private part

    /**
     * static constructor
     */
    CUpnpVolumeStateMachine(
        MUPnPAVRenderingSession& aSession );
    
    void HandleVolumeResultInSyncState(
            TInt aError, TInt aVolumeLevel, TBool aActionResponse );
    
    void HandleUnsolicitedMuteEvent(
        TInt aError, TBool aMute, TBool aActionResponse );
    
    void HandleUnsolicitedMuteEventInIdle(
        TInt aError, TBool aMute, TBool aActionResponse );

    void NotifyObserver(TBool aMute);
    

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
    IMPORT_C void SetObserver( MUpnpVolumeStateMachineObserver& aObserver );

    /**
     * removes the current observer
     */
    IMPORT_C void RemoveObserver();

    /**
     * Synchronises the state machine with the remote renderer
     * (updates cached volume and mute levels)
     * this is an aynchronous operation and calls back VolumeSyncReady()
     */
    IMPORT_C void SyncL();

    /**
     * Forces the state machine off sync
     * (clears cached volume and mute levels)
     */
    IMPORT_C void SetOffSync();

    /**
     * Checks if the state machine is synchronised
     * with the remote renderer
     */
    IMPORT_C TBool IsInSync() const;

    /**
     * Sets volume on the renderer.
     * This method is thread-safe, and checks limits automatically.
     * If trying to adjust volume when session is busy,
     * method will leave with KErrInUse.
     * If trying to adjust volume to an existing value,
     * method will call back immediately with KErrNone.
     * Example for increasing volume:
     *   v->SetVolumeL( v->Volume() + KDefaultVolumeResolution );
     * @param aVolume volume to set
     */
    IMPORT_C void SetVolumeL( TInt aVolume );

    /**
     * returns the current renderer volume level.
     * Note: this is not a network operation - returns a cached value
     * @return volume level
     */
    IMPORT_C TInt Volume() const;

    /**
     * Sets or unsets renderer mute state.
     * If trying to adjust volume when session is busy,
     * method will leave with KErrInUse.
     * If trying to adjust mute to a state to an existing state,
     * method will call back immediately with KErrNone.
     * example of toggling mute state:
     *    v->SetMuteL( !v->Mute() );
     * @param aMuteState either ETrue of EFalse to set mute on or off
     */
    IMPORT_C void SetMuteL( TBool aMuteState );

    /**
     * returns the current renderer muted state.
     * Note: this is not a network operation - returns a cached value
     * @return mute state
     */
    IMPORT_C TBool Mute() const;

    /**
     * Checks if the state machine is busy doing something.
     * If not, new requests can be put in.
     */
    IMPORT_C TBool IsBusy() const;

    /**
     * Tests if this renderer has volume and mute capability.
     * Calling volume handling methods for a renderer that does not have
     * this capability wil result into an error.
     * @return ETrue if device has volume control capability.
     */
    IMPORT_C TBool HasVolumeCapability() const;

    /**
     * Cancels any ongoing operation
     */
    IMPORT_C void Cancel();
    
    /**
     * Copies cached values from another instance
     */
    IMPORT_C void CopyValues( const CUpnpVolumeStateMachine& aOther );

public: // methods like in MUPnPRenderiongSessionObserver

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void VolumeResult(
        TInt aError, TInt aVolumeLevel, TBool aActionResponse );

    /**
     * @see MUPnPRenderiongSessionObserver
     * client of this state machine should forward the corresponding method
     * from the rendering session directly to this method
     */
    IMPORT_C void MuteResult(
        TInt aError, TBool aMute, TBool aActionResponse );

private:
    
    void PushIntoQueueL( const TUpnpVSMQueueItem::TPropertyType aPropery,
        const TInt aValue);
    
    void CompressQueue();
    
    void ProcessNextQueuedProperty();

private: // data

    // Rendering session
    MUPnPAVRenderingSession&            iSession;

    // the observer (no ownership!)
    MUpnpVolumeStateMachineObserver*    iObserver;

    // volume state machine option flags
    TInt                                iOptions;

    // operation types
    enum TState
        {
        EOffSync,             // not synchronised
        ESyncing,             // synchronising the state machine
        EIdle,                // doing nothing
        EAdjustingVolume,     // adjusting volume
        EAdjustingMute,       // setting/unsetting mute
        EAdjustingVolumeToZero, // setting to mute due to volume==0
        EAdjustingVolumeFromZero, // adjusting volume up from muted state
        ECancelled            // operation being cancelled
        };

    //currently ongoing operation
    TState                              iState;

    // current renderer volume level
    TInt                                iCurrentVolume;

    // volume requested by client during mute state
    TInt                                iVolumeToSetAfterMute;

    // current renderer mute state
    TInt                                iCurrentMute;

    // in case mute is on, this flag is set if mute was by client request
    TBool                               iMuteRequestedByClient;

    // volume capability
    TBool                               iVolumeCapability;

    // mute capability (only used internally)
    TBool                               iMuteCapability;
    
    RArray<TUpnpVSMQueueItem>           iQueue; 

    // last changed volume level
    TInt                                iCachedVolume;
    
    // last changed mute
    TInt                                iCachedMute;
    };


#endif // C_UPNPVOLUMESTATEMACHINE_H

