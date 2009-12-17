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
* Description:  observer for CUpnpImageRenderingEngine
*
*/


#ifndef UPNP_IMAGERENDERINGENGINEOBSERVER_H
#define UPNP_IMAGERENDERINGENGINEOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h" // base class
#include "upnpitemresolverobserver.h" // base class

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class MUPnPAVController;
class MUPnPAVRenderingSession;
class MUPnPAVSessionBase;
class MUPnPItemResolver;
class CUPnPCommonUI;

/**
* Observer interface for image rendering engine.
*
* @since S60 3.2
*/
class MUpnpImageRenderingEngineObserver
    {
    public:

        /**
         * Requests for media to be rendered.
         * this method is called soon after calling PlayL().
         * the client is expected to provide an item resolver prepared with
         * the media to be played back (but not resolved yet)
         * ownership is transferred in this transaction.
         * if client returns NULL playback will fail to KErrCancelled
         * 
         * @return the resolver representing media to be played
         */
        virtual MUPnPItemResolver* GetMedia() = 0;

        /**
         * Acknowledge for render request
         * @param aError any errors that occurred
         * @param aItem the actual item that is playing, trying to play, or
         *        NULL if such is not available for some error condition
		 * @return response error code
		 *         KErrNone if the engine should carry on working
		 *         KErrDisconnected if engine should disconnect
         */
        virtual TInt RenderAck(
            TInt aError,
            const CUpnpItem* aItem ) = 0; 

        /**
         * Notifies that rendering engine will shut down due to
         * communication failure.
         * @param aError the error code (some kind of reason)
         */
        virtual void EngineShutdown(
            TInt aError ) = 0;
            
    };


#endif // UPNP_IMAGERENDERINGENGINEOBSERVER_H

// End of File
