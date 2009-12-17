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
* Description:  Converts from upnp callback to upnp command observer
*
*/


#ifndef UPNP_COMMAND_CALLBACK_ADAPTER_H
#define UPNP_COMMAND_CALLBACK_ADAPTER_H

// INCLUDES
#include <e32base.h>                // CBase
#include "upnpcommandcallback.h"    // MUpnpCommandCallback

// FORWARD DECLARATIONS
class MUpnpCommandObserver;

// CLASS DEFINITION
NONSHARABLE_CLASS(CUpnpCommandCallbackAdapter)
    : public CBase
    , public MUpnpCommandCallback
    {

    public: // Construction/destruction methods

        /**
         * Constructor
         * 
         */
        CUpnpCommandCallbackAdapter(
            MUpnpCommandObserver* aObserver );

        /**
         * Destructor.
         *
         * @since S60 3.2
         */
        virtual ~CUpnpCommandCallbackAdapter();

    public: // From MUpnpCommandCallback

       /**
        * Callback event
        */
        void CommandEvent(
            UpnpCommand::TUpnpCommandEvent aEventType,
            TInt aStatusCode );

    private:

        // the observer to convert to
        MUpnpCommandObserver* iObserver;

    };

#endif // UPNP_COMMAND_CALLBACK_ADAPTER_H

// End of File
