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
*                UpnpCommand API when browsing the Upnp home network.
*
*/


#ifndef UPNP_BROWSE_COMMAND_H
#define UPNP_BROWSE_COMMAND_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpCommand;

// CLASS DEFINITION
class CUpnpBrowseCommand: public CBase
    {

    public: // Construction/destruction methods

        /**
         * Creates a new UpnpCommand for Upnp home network browsing purposes.
         * Does not instantiate Upnp Framework services yet at this point.
         *
         * Leaves with KErrNotReady if Upnp dedicated IAP is not set, or with
         * using standard e32err.h error code if construction fails.
         *
         * @since S60 3.2
         * @return a new CUpnpBrowseCommand object
         */
        IMPORT_C static CUpnpBrowseCommand* NewL();

        /**
         * Destructor.
         *
         * @since S60 3.2
         */
        virtual ~CUpnpBrowseCommand();

    public: // Business logic methods

        /**
         * Returns the availability information of the command.
         *
         * @since S60 3.2
         * @return TBool (ETrue if available)
         */
        IMPORT_C static TBool IsAvailableL();

        /**
         * Allocates Upnp Framework resources, and initiates Upnp home network
         * browsing.
         *
         * Leaves if the operation fails. Standard system wide (e32err.h)
         * error codes will be used.
         *
         * @since S60 3.2
         */
        IMPORT_C void BrowseHomeNetworkL();

    private: // Private construction methods

        /**
         * Constructor
         *
         * @since S60 3.2
         */
        CUpnpBrowseCommand();

        /**
         * Second phase constructor
         *
         * @since S60 3.2
         */
        void ConstructL();

    private: // Data members

        /*
         * UpnpCommand instance, owned
         */
        CUpnpCommand*    iCommand;

    };

#endif // UPNP_BROWSE_COMMAND_H

// End of File
