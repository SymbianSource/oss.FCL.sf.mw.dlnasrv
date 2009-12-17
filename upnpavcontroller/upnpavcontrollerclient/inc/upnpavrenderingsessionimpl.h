/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      implements a session against a renderer
*
*/






#ifndef C_UPNPAVRENDERINGSESSIONIMPL_H
#define C_UPNPAVRENDERINGSESSIONIMPL_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>

#include "upnpavrenderingsession.h"
#include "upnpavdeviceobserver.h"

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class RUPnPAVControllerClient;
class CUPnPAVEventActive;
class CUPnPAVDeviceActive;
class CUPnPAVRenderingActive;
class CUpnpItem;
class TUnsolicitedEventC;

/**
 *  AV Controller rendering session implementation.
 *
 *  This class implements the rendering session. Includes operations
 *  to set and to set next uri, play, stop and pause the playback,
 *  to set the volume and mute and to get the position information
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVRenderingSessionImpl : public CBase,
                                    public MUPnPAVRenderingSession,
                                    public MUPnPAVDeviceObserver
                                    
    {

public:

    /**
     * Static constructor
     *
     * @param aServer handle to AV Controller server
     * @param aDevice target device
     * @return a new browsing session implementation instance
     */    
    static CUPnPAVRenderingSessionImpl* NewL(
        RUPnPAVControllerClient& aServer,
        const CUpnpAVDevice& aDevice );
 
    /**
     * Destructor
     */        
    virtual ~CUPnPAVRenderingSessionImpl();
    
private:

    /**
     * 2nd phase construct
     */    
    CUPnPAVRenderingSessionImpl( RUPnPAVControllerClient& aServer );
    
    /**
     * 2nd phase construct
     */        
    void ConstructL();

protected: // From MUPnPAVRenderingSession

    /**
     * See upnpavrenderingsession.h
     */
    void SetObserver( MUPnPAVRenderingSessionObserver& aObserver );

    /**
     * See upnpavrenderingsession.h
     */
    void RemoveObserver();

    /**
     * See upnpavrenderingsession.h
     */
    MUPnPAVRenderingSessionObserver* Observer() const;
    
    /**
     * See upnpavrenderingsession.h
     */
    const CUpnpAVDevice& Device() const;
    
    /**
     * See upnpavrenderingsession.h
     */
    void ReserveLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void CancelReserveLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void ReleaseLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void SetURIL( const TDesC8& aURI, const CUpnpItem& aItem );

    /**
     * See upnpavrenderingsession.h
     */
    void SetNextURIL( const TDesC8& aURI, const CUpnpItem& aItem );

    /**
     * See upnpavrenderingsession.h
     */
    void PlayL();

    /**
     * See upnpavrenderingsession.h
     */
    void StopL();

    /**
     * See upnpavrenderingsession.h
     */
    void PauseL();

    /**
     * See upnpavrenderingsession.h
     */
    void SetVolumeL( TInt aVolumeLevel );

    /**
     * See upnpavrenderingsession.h
     */
    void GetVolumeL();

    /**
     * See upnpavrenderingsession.h
     */
    void SetMuteL( TBool aMute );

    /**
     * See upnpavrenderingsession.h
     */
    void GetMuteL();

    /**
     * See upnpavrenderingsession.h
     */
    void GetPositionInfoL();

public: // From MUPnPAVDeviceObserver

    /**
     * See upnpavdeviceobserver.h
     */
    void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

    /**
     * See upnpavdeviceobserver.h
     */
    void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );

    /**
     * See upnpavdeviceobserver.h
     */
    void WLANConnectionLost();

public: // Public methods

    /**
     * A method to receive an unsolicited event
     * @param aError error code
     * @param aEvent event
     */     
    void EventReceived( TInt aError,
        TUnsolicitedEventC aEvent );
       
private: // Private methods

    void ResetL();

private:

    RUPnPAVControllerClient&            iServer; // not owned
    
    MUPnPAVRenderingSessionObserver*    iObserver; // not owned
    
    CUPnPAVDeviceActive*                iDeviceActive; // owned
    
    CUPnPAVEventActive*                 iEventActive; // owned
    
    CUPnPAVRenderingActive*             iCommandActive; // owned
    
    CUPnPAVRenderingActive*             iSettingActive; // owned    
    
    CUpnpAVDevice*                      iDevice; // owned

    // Buffer to pass data to server
    HBufC8*                             iBuffer; // owned            
    TPtr8                               iBufferPtr;         
 
    TBool                               iAlive;
        
    TBool                               iMediaServerResourceReserved;
    
    };


#endif // C_UPNPAVRENDERINGSESSIONIMPL_H
