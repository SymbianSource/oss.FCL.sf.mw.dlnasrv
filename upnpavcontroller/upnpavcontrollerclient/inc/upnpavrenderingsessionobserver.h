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
* Description:      callback interface for a rendering session
*
*/






#ifndef M_UPNPAVRENDERINGSESSIONOBSERVER_H
#define M_UPNPAVRENDERINGSESSIONOBSERVER_H

// INCLUDE FILES
#include "upnpavsessionobserverbase.h"

// TYPEDEFS
enum TUPnPAVInteractOperation
    {
    EUPnPAVPlay = 0,
    EUPnPAVPause,
    EUPnPAVStop,
    EUPnPAVPlayUser,
    EUPnPAVPauseUser,
    EUPnPAVStopUser
    };

/**
* Defines the response interface for the media control operations of the
* UPnP AV Controller.
*
* @since Series 60 3.1
*/
class MUPnPAVRenderingSessionObserver : public MUPnPAVSessionObserverBase
    {
    
public:

    /**
     * UPnP AV Controller calls this method to return the result for the
     * 'get volume' request. Parameter contains the volume level of the
     * media renderer device to which the 'get volume' request was sent.
     *
     * @since Series 60 3.1
     * @param aError system wide error code
     * @param aVolumeLevel TInt volume level (integer value between 0 - 100)
     * @param aActionResponse ETrue, if this is a response to GetVolume()
     * @return None
     */
    virtual void VolumeResult( TInt aError, TInt aVolumeLevel,
        TBool aActionResponse ) = 0;

    /**
     * UPnP AV Controller calls this method to return the result for the
     * 'get mute' request. Parameter contains the state of the mute of the
     * media renderer device to which the 'get mute' request was sent.
     *
     * @since Series 60 3.1
     * @param aError system wide error code
     * @param aMute TBool the state of the mute (ETrue or EFalse)
     * @param aActionResponse ETrue, if this is a response to GetMute()
     * @return None
     */
    virtual void MuteResult( TInt aError, TBool aMute,
        TBool aActionResponse ) = 0;

    /**
     * UPnP AV Controller calls this method to indicate that the requested
     * interaction operation (play, stop, etc.) is complete. In other words,
     * the target rendering device has changed it's state accordingly.
     *
     * @since Series 60 3.1
     * @param aError system wide error code
     * @param aOperation TInt operation (TAVInteractOperation)
     * @return None
     */
    virtual void InteractOperationComplete( 
                      TInt aError,
                      TUPnPAVInteractOperation aOperation 
                      ) = 0;

    /**
     * UPnP AV Controller calls this method as a response to Position 
     * Info action. The current position and the total length of the 
     * track that is currently playing is returned. The results are in
     * (hh:mm:ss) format.
     *
     * UPnPAVController releases the memory allocated for aTrackPosition
     * and aTrackLength.
     *
     * @since Series 60 3.0
     * @param aStatus TInt error code
     * @param aTrackPosition TDesC8& track position
     * @param aTrackLength TDesC8& track length
     * @return None
     */
    virtual void PositionInfoResult( 
                      TInt aError, 
                      const TDesC8& aTrackPosition, 
                      const TDesC8& aTrackLength 
                      ) = 0;

    /**
     * UPnP AV Controller calls this method to return the result for the
     * 'set uri' request.
     *
     * @since Series 60 3.1
     * @param aError error code
     * @return None
     */                      
    virtual void SetURIResult( TInt aError ) = 0;

    /**
     * UPnP AV Controller calls this method to return the result for the
     * 'set next uri' request.
     *
     * @since Series 60 3.1
     * @param aError error code
     * @return None
     */                         
    virtual void SetNextURIResult( TInt aError ) = 0;                   

    /**
     * Notifies that the Media Renderer we have a session with has
     * disappeared. Session is now unusable and must be closed. 
     *
     * @since Series 60 3.1
     * @return None
     */        
    virtual void MediaRendererDisappeared( 
        TUPnPDeviceDisconnectedReason aReason ) = 0;
        
    virtual void ReserveLocalMSServicesCompleted( TInt aError ) = 0;        

    };

#endif // M_UPNPAVRENDERINGSESSIONOBSERVER_H

// End of File
