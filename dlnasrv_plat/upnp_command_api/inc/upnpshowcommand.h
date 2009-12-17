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
*                UpnpCommand API when showing images and videos via Upnp.
*
*/


#ifndef UPNP_SHOW_COMMAND_H
#define UPNP_SHOW_COMMAND_H

// INCLUDES
#include <e32base.h>                // CBase

// FORWARD DECLARATIONS
class CUpnpCommand;
class MUpnpCommandObserver;
class CUpnpCommandCallbackAdapter;


// CLASS DEFINITION
class CUpnpShowCommand: public CBase
    {

    public: // Construction/destruction methods

        /**
         * Creates a new UpnpCommand for image and video showing purposes.
         * Does not instantiate Upnp Framework services yet at this point.
         *
         * Leaves with KErrNotReady if Upnp dedicated IAP is not set, or with
         * using standard e32err.h error code if construction fails.
         *
         * @since S60 3.2
         * @return a new CUpnpShowCommand object
         */
        IMPORT_C static CUpnpShowCommand* NewL();

        /**
         * Creates a new UpnpCommand for image and video showing purposes.
         * Does not instantiate Upnp Framework services yet at this point.
         *
         * Leaves if the given observer instance is not valid.
         *
         * @since S60 3.2
         * @param aObserver (MUpnpCommandObserver*) The observer instance
         * @return a new CUpnpShowCommand object
         */
        IMPORT_C static CUpnpShowCommand* NewL(
            MUpnpCommandObserver* aObserver );

        /**
         * Destructor.
         *
         * @since S60 3.2
         */
        virtual ~CUpnpShowCommand();

    public: // Business logic methods

        /**
         * Returns the availability information of the command.
         *
         * @since S60 3.2
         * @return TBool (ETrue if available)
         */
        IMPORT_C static TBool IsAvailableL();

        /**
         * Starts showing. Allocates Upnp Framework resources. The target
         * rendering device will be queried from the user using an UI
         * component. Local Media Server sharing will be activated (if not
         * yet activated).
         *
         * Leaves if the Upnp Framework resource allocation fails. Standard
         * system wide (e32err.h) error codes will be used.
         *
         * @since S60 3.2
         */
        IMPORT_C void StartShowingL();

        /**
         * Stops showing. Releases all Upnp Framework resources. Local Media
         * Server sharing state will be returned to it's original state.
         *
         * Leaves if the operation fails. Standard system wide (e32err.h)
         * error codes will be used.
         *
         * @since S60 3.2
         */
        IMPORT_C void StopShowingL();

        /**
         * Shows the given images on a remote Upnp Media Rendering device.
         * The method is asynchronous. Upnp Framework resources will be 
         * allocated, if not yet allocated (by calling StartShowingL). 
         *
         * MUpnpCommandObserver::CommandComplete callback method is used to
         * delived the return the return code of the operation.
         *
         * Leaves if the operation fails, or if the user cancels the
         * operations. Standard system wide (e32err.h) error codes will be
         * used.
         *
         * @since S60 3.2
         * @param aFilename (const TDesC&) filename (including path)
         */
        IMPORT_C void ShowImageL( const TDesC& aFilename );

        /**
         * Shows the given video on a remote Upnp Media Rendering device. The
         * method is asynchronous. Upnp Framework resources will be allocated,
         * if not yet allocated (by calling StartShowingL). 
         *
         * MUpnpCommandObserver::CommandComplete callback method is used to
         * delived the return the return code of the operation.
         *
         * Leaves if the operation fails, or if the user cancels the
         * operations. Standard system wide (e32err.h) error codes will be
         * used.
         *
         * @since S60 3.2
         * @param aFilename (const TDesC&) filename (including path)
         */
        IMPORT_C void ShowVideoL( const TDesC& aFilename );

    private: // Private construction methods

        /**
         * Constructor
         *
         * @since S60 3.2
         * @param aObserver (MUpnpCommandObserver*) The observer instance
         *        can be NULL
         */
        CUpnpShowCommand( MUpnpCommandObserver* aObserver );

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
        CUpnpCommand*               iCommand;

        /*
         * Observer instance. Not owned.
         */
        MUpnpCommandObserver*       iObserver;

        /*
         * Callback adapter. Owned.
         */
        CUpnpCommandCallbackAdapter* iAdapter;

    };

#endif // UPNP_SHOW_COMMAND_H

// End of File
