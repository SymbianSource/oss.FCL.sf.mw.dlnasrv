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
* Description:      Singleton class for main upnp related services
*
*/






#ifndef C_UPNPSINGLETON_H
#define C_UPNPSINGLETON_H

// INCLUDES
#include <e32base.h>
#include "upnpavdeviceobserver.h"
#include "upnpmusicperiodizer.h"

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPRendererSelectorObserver;
class CUpnpAVDevice;
class CUpnpAVDeviceList;
class MUPnPAVRenderingSession;

/**
 * A singleton class that provides main upnp services
 *
 * The class owns the AVController client session. Plugins that
 * use this singleton class may then share the same session, or
 * start rendering sessions in it.
 *
 * The class also provides device discovery routines.
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPSingleton : public CBase,
                       public MUPnPAVDeviceObserver,
                       public MUPnPMusicPeriodizerObserver
    {

private:

    /**
     * State of renderer selection subprocess
     *
     * @since Series 60 3.1
     */   
    enum TSelectorState
        {
        EStateWaiting, // waiting in device discovery phase
        EStateComplete, // device discovery complete, still updating list
        EStateReady, // steady state - list is untouched
        EStateError, // WLAN connection lost
        };
    
public:

    /**
     * Singleton constructor
     *
     * @since Series 60 3.1
     * @return a new instance
     */
    static CUPnPSingleton* GetInstanceL();

    /**
     * Singleton destructor
     *
     * @since Series 60 3.1
     * @param aInstance the instance pointer to lose
     */
    static void LoseInstance( CUPnPSingleton* aInstance );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    ~CUPnPSingleton();

    /**
     * Get list of renderers from AVController. Returns names (async)
     *
     * @param aObserver the observer to receive the response
     * @since Series 60 3.1
     */
    void GetRendererNamesL( MUPnPRendererSelectorObserver& aObserver );

    /**
     * Cancels an asynchronous query if it is pending
     */
    void CancelGetRendererNames();

    /**
     * Selects renderer by index
     *
     * @since Series 60 3.1
     * @param aIndex Index of renderer to be selected
     */
    const CUpnpAVDevice* SelectRendererByIndexL( TInt aIndex );

    /**
     * Return used renderer device
     *
     * @since Series 60 3.1
     * @return CUpnpAVDevice used renderer device
     */
    const CUpnpAVDevice* DefaultDevice();

    /**
     * Provides reference to the AVController resource
     */
    MUPnPAVController& AVC();

    
protected: // From MUPnPAVDeviceObserver

    /**
     * See MUPnPAVDeviceObserver
     */
     void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

    /**
     * See MUPnPAVDeviceObserver
     */
     void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );
     
    /**
     * See MUPnPAVDeviceObserver
     */
     void WLANConnectionLost();

protected: // From MUPnPMusicPeriodizerObserver 

    /**
     * See MUPnPMusicPeriodizerObserver
     */
    void HandlePeriod();
    
private:

    /**
     * Clears the current device cache and copies the renderer list again
     * from AVController
     */
    void CacheRendererListL();

    /**
     * Sends an asynchronous response for subplayer names to observer.
     * Uses the class members as parameters.
     * @param aComplete the complete parameter to use in callback
     * @param aError the error code to be transmitted
     */
    void DeliverNamesToObserverL( TBool aComplete, TInt aError = KErrNone );

    /**
     * Checks if the device is available. If a device is not available, it
     * has been disappeared. The method uses a proprietary flag.
     * @param aDevice the device to check
     * @return EFalse if the device has disappeared.
     */
    TBool IsAvailable( const CUpnpAVDevice& aDevice ) const;

    /**
     * Sets a device availability flag. If a device is not available, it
     * has been disappeared. The method uses a proprietary flag.
     * @param aDevice the device to set
     * @param aAvailable if false, the device has disappeared.
     */
    void SetAvailable( CUpnpAVDevice& aDevice, TBool aAvailable ) const;

    /**
     * Private default constructor
     *
     * @since Series 60 3.1
     */
    CUPnPSingleton();

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */
    void ConstructL();

private: // data

    /**
     * Reference count
     */
    static TInt                         iInstanceCount;

    /**
     * The singleton
     */
    static CUPnPSingleton*              iInstance;

    /**
     * Pointer to AVController
     */
    MUPnPAVController*                  iAVController;

    /**
     * Temporary pointer to renderer selector observer 
     */
    MUPnPRendererSelectorObserver*      iRendererselectorObserver;

    /**
     * default device index
     */
    static TInt                         iDefaultRendererIndex;
    
    /**
     * Timer support
     */
    CUPnPMusicPeriodizer*               iPeriodizer;
                       
    /**
     * Array of renderer
     */        
    RPointerArray<CUpnpAVDevice>        iMediaRenderers;

    /**
     * Current selector state
     */
    TSelectorState                      iSelectorState;

    /**
     * Indicator for activity during next call to GetSubplayerNames
     */
    TSelectorState                      iStateChangeDuringNextCall;

    };


#endif // C_UPNPSINGLETON_H
