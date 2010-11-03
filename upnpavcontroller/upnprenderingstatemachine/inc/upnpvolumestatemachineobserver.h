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
* Description:  Observer for generic upnp volume state machine
*
*/


#ifndef C_UPNPVOLUMESTATEMACHINEOBSERVER_H
#define C_UPNPVOLUMESTATEMACHINEOBSERVER_H

// INCLUDES
#include <e32base.h>


/**
 * Class for receiving callbacks from
 * CUPnPVolumeStateMachine
 *
 * @lib upnpavcontrollerhelper.lib
 */
class MUpnpVolumeStateMachineObserver
    {

public:

    /**
     * Indicates state machine has been synchronised with the renderer
     * @param aError error that occurred during sync
     */
    virtual void VolumeSyncReady( TInt aError ) = 0;

    /**
     * Indicates volume state has been changed
     * either by request or spontanelously.
     * @param aError the error code
     * @param aRenderingSession for playback commands
     * @param aUserOriented ETrue if this responds to a request
     */
    virtual void VolumeChanged(
        TInt aError, TInt aVolume, TBool aUserOriented ) = 0;

    /**
     * Indicates mute state has been changed
     * either by request or spontanelously.
     * @param aError the error code
     * @param aMuteState current mute state
     * @param aUserOriented ETrue if this responds to a request
     */
    virtual void MuteChanged(
        TInt aError, TBool aMuteState, TBool aUserOriented ) = 0;

    };


#endif // C_UPNPVOLUMESTATEMACHINEOBSERVER_H

