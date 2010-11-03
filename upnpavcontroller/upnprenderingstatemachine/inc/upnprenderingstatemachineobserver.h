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
* Description:  Observer for the generic upnp rendering state machine
*
*/

#ifndef C_UPNPRENDERINGSTATEMACHINEOBSERVER_H
#define C_UPNPRENDERINGSTATEMACHINEOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "upnprenderingstatemachineconstants.h" // for types

// FORWARD DECLARATIONS

// CONSTANTS


/**
 * Class for receiving callbacks from
 * rendering state machine
 *
 * @lib upnprenderingstatemachine.lib
 */
class MUpnpRenderingStateMachineObserver
    {
public:

    /**
     * Indicates state machine has been synchronised with the renderer
     * @param aError error that occurred during sync
     */
    virtual void RendererSyncReady( TInt aError,
        Upnp::TState aState ) = 0;

    /**
     * Indicates renderer state has changed
     * either by request or spontanelously.
     * @param aError the error code
     * @param aState the new state
     * @param aUserOriented ETrue if this responds to a request
     * @param aStateParam extra info depending on the state change
     */
    virtual void RenderingStateChanged(
        TInt aError,
        Upnp::TState aState,
        TBool aUserOriented,
        TInt aStateParam ) = 0;

    /**
     * Synchronises UI with media duration and current position
     * this callback can be either automatic or explicitly requested
     * @param aError error that occurred during position sync
     * @param aMode mode in which the track is progressing
     * @param aDuration media duration in milliseconds
     * @param aPosition current position in milliseconds
     */
    virtual void PositionSync( TInt aError,
        Upnp::TPositionMode aMode,
        TInt aDuration, TInt aPosition ) = 0;

    };


#endif // C_UPNPRENDERINGSTATEMACHINEOBSERVER_H

