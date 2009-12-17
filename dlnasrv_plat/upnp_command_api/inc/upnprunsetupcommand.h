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
* Description:  Helper class to ease the use of the Upnp Framework's
*                UpnpCommand API when running the Upnp Fw's setup wizard.
*
*/


#ifndef UPNP_RUN_SETUP_COMMAND_H
#define UPNP_RUN_SETUP_COMMAND_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpCommand;

// CLASS DEFINITION
class CUpnpRunSetupCommand: public CBase
    {

    public: // Construction/destruction methods

        /**
         * Creates a new UpnpCommand for running the Upnp Fw setup wizard.
         * Does not instantiate Upnp Framework services yet at this point.
         *
         * Leaves with KErrNotReady if Upnp dedicated IAP is not set, or with
         * using standard e32err.h error code if construction fails.
         *
         * @since S60 3.2
         * @return a new CUpnpRunSetupCommand object
         */
        IMPORT_C static CUpnpRunSetupCommand* NewL();

        /**
         * Destructor.
         *
         * @since S60 3.2
         */
        virtual ~CUpnpRunSetupCommand();

    public: // Business logic methods

        /**
         * Returns the availability information of the command.
         *
         * @since S60 3.2
         * @return TBool (ETrue if available)
         */
        IMPORT_C static TBool IsAvailableL();

        /**
         * Allocates Upnp Framework resources, and starts up the Upnp Fw's
         * setup wizard.
         *
         * Leaves if the operation fails. Standard system wide (e32err.h)
         * error codes will be used.
         *
         * @since S60 3.2
         */
        IMPORT_C void RunSetupL();

    private: // Private construction methods

        /**
         * Constructor
         *
         * @since S60 3.2
         */
        CUpnpRunSetupCommand();

        /**
         * Second phase constructor
         *
         * @since S60 3.2
         */
        void ConstructL();

    private: // Data members

        /*
         * UpnpCommand instance. Owned.
         */
        CUpnpCommand*    iCommand;

    };

#endif // UPNP_RUN_SETUP_COMMAND_H

// End of File
