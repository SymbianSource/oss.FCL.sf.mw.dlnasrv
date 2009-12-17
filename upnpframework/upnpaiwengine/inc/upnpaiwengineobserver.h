/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This interface class defines the call back interface for the
*                UpnpAiwEngine.
*
*/


#ifndef UPNP_AIW_ENGINE_OBSERVER_H
#define UPNP_AIW_ENGINE_OBSERVER_H

/**
* Defines the call back interface for the UPnP AIW Engine.
*
* @since S60 3.0
*/
class MUPnPAiwEngineObserver
    {

    public:

        /**
         * Indicates that the play operation is complete.
         *
         * @since S60 3.0
         * @param aStatus Status information
         */
        virtual void PlayCompleteL( TInt aStatus ) = 0;

        /**
         * Indicates that the connection with the target UPnP
         * device has been lost.
         *
         * @since S60 3.0
         */
        virtual void ConnectionLostL() = 0;

    };

#endif // UPNP_AIW_ENGINE_OBSERVER_H

// End of file
