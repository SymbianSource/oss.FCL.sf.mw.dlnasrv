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
* Description:      interface for session against a renderer
*
*/






#ifndef C_UPNPAVRENDERINGSESSION_H
#define C_UPNPAVRENDERINGSESSION_H

#include <e32std.h>
#include "upnpavsessionbase.h"

class MUPnPAVRenderingSessionObserver;
class CUpnpItem;

/**
 *  AV Controller rendering session interface.
 *
 *  This class defines the rendering interface of AV Controller.
 *  Includes operations to set and to set next uri, play, stop and
 *  pause the playback, to set the volume and mute and to get the
 *  position information
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
 class MUPnPAVRenderingSession : public MUPnPAVSessionBase
    {

public:

    /**
     * Sets the rendering session observer
     *
     * @since Series 60 3.1      
     * @param aObserver rendering session observer
     * @return none
     */
    virtual void SetObserver(
        MUPnPAVRenderingSessionObserver& aObserver ) = 0;

    /**
     * Removes the rendering session observer
     *
     * @since Series 60 3.1     
     * @param None
     * @return None
     */
    virtual void RemoveObserver() = 0;
    
    /**
     * Returns the rendering session observer
     *
     * @since Series 60 3.1     
     * @param None
     * @return observer
     */
    virtual MUPnPAVRenderingSessionObserver* Observer() const = 0;   
    
public:

    /**
     * Set the URI by providing an URI and the item  
     * 
     * @since Series 60 3.1
     * @param aURI URI of the item
     * @param aObject item
     */
    virtual void SetURIL( const TDesC8& aURI, const CUpnpItem& aItem ) = 0;
    
    /**
     * Set the next URI by providing an URI and the item  
     * 
     * @since Series 60 3.1
     * @param aURI URI of the item
     * @param aObject item
     */
    virtual void SetNextURIL( const TDesC8& aURI,
        const CUpnpItem& aItem ) = 0;

    /**
     * Starts the playback. 
     *
     * @since Series 60 3.1
     * @param None
     * @return none
     */
    virtual void PlayL() = 0;

    /**
     * Stops the playback.
     *
     * @since Series 60 3.1
     * @param None
     * @return none
     */
    virtual void StopL() = 0;

    /**
     * Pauses the playback.
     *
     * @since Series 60 3.1
     * @param None
     * @return none
     */
    virtual void PauseL() = 0;

    /**
     * Sets the volume level.
     *
     * @since Series 60 3.1
     * @param aVolumeLevel TInt Volume level in percents (0 - 100)
     * @return none
     */
    virtual void SetVolumeL( TInt aVolumeLevel ) = 0;

    /**
     * Gets the volume level.
     *
     * @since Series 60 3.1
     * @param None
     * @return none
     */
    virtual void GetVolumeL() = 0;

    /**
     * Sets the mute state.
     *
     * @since Series 60 3.1
     * @param aMute TBool desired state of the mute (ETrue of EFalse)
     * @return none
     */
    virtual void SetMuteL( TBool aMute ) = 0;

    /**
     * Gets the mute state.
     *
     * @since Series 60 3.1
     * @param None
     * @return none
     */
    virtual void GetMuteL() = 0;

    /**
     * Gets position info of the media that is currently
     * rendered on the UPnP Media Renderer.
     *
     * @since Series 60 3.0
     * @param none
     * @return none
     */
    virtual void GetPositionInfoL() = 0;
    
    };


#endif // C_UPNPAVRENDERINGSESSION_H
