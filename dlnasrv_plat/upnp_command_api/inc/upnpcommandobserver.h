/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Header file for the UpnpCommand ECom plugin. Defines the
*                Upnp Framework's UpnpCommand callback API.
*
*/


#ifndef UPNP_COMMAND_OBSERVER_H
#define UPNP_COMMAND_OBSERVER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KUpnpCommandStatusStartPlayVideo = 6; // video is ready to be played.

/**
* UpnpCommand Observer interface class definition.
*
* @since S60 3.2
*/
class MUpnpCommandObserver
    {

    public:

        /**
        * Indicates that the command has been completed.
        *
        * If the WLAN connection or the target device is lost,
        * KErrDisconnected will be returned.
        * 
        * When video is ready to be played (launching 
        * videoplayerdialog), KUpnpCommandStatusStartPlayVideo 
        * will be returned.
        *
        * @since S60 3.2
        * @param aStatusCode (TInt) status code
        */
        virtual void CommandComplete( TInt aStatusCode ) = 0;

    };

#endif // UPNP_COMMAND_OBSERVER_H

// End of File
