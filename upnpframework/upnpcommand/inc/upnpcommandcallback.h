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
* Description:  UpnpCommand callback API
*
*/


#ifndef UPNP_COMMAND_CALLBACK_H
#define UPNP_COMMAND_CALLBACK_H

// INCLUDES
#include <e32base.h>
#include "upnpcommandcons.h"


/**
* UpnpCommand callback interface definition.
*/
class MUpnpCommandCallback
    {

    public:

        /**
         * Indicates a command execution event occurred.
         * The event typically indicates command being completed.
         *
         * @param aEventType (see upnpcommandcons.h) type of event occurred
         *     during command execution
         * @param aStatusCode (TInt) Additional data, meaning of which depends
         *     on the event type.
         */
        virtual void CommandEvent(
            UpnpCommand::TUpnpCommandEvent aEventType,
            TInt aStatusCode ) = 0;
 
    };

#endif // UPNP_COMMAND_CALLBACK_H

// End of File
