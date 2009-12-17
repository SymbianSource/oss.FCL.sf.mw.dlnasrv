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
* Description:  Header file for the CUpnpBrowseTask class.
*
*/


#ifndef UPNP_BROWSE_TASK_H
#define UPNP_BROWSE_TASK_H

// INCLUDES
#include <e32base.h>
#include "upnpavsessionobserverbase.h"
#include "upnpavbrowsingsessionobserver.h"
#include "upnpavdeviceobserver.h"
#include "upnptask.h"

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class MUPnPAVSessionBase;

/**
* This class defines the CUpnpBrowseTask used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpBrowseTask : public CUpnpTask,
                        public MUPnPAVDeviceObserver
    {
    public: // Methods from CUpnpTask

        /**
         * Creates a new CUpnpBrowseTask object. Allocates Upnp Fw resources.
         * Resources will be released when the task is destroyed.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpBrowseTask, casted to type CUpnpTask
         */
        static CUpnpTask* NewL();

        /**
         * Allocates the Upnp Fw resources.
         *
         * @since S60 3.2
         */
        void AllocateResourcesL();

        /**
         * Executes the task.
         *
         * @since S60 3.2
         */
        void ExecuteL();

    public: // Call back methods of MUPnPAVDeviceObserver

        /* Not used */
        void UPnPDeviceDiscovered( const CUpnpAVDevice& /*aDevice*/ ) {}
        void UPnPDeviceDisappeared( const CUpnpAVDevice& /*aDevice*/ ) {}

        /**
         * Notifies that the WLAN connection has been lost. All sessions
         * are now usable and must be closed.
         *
         * @since Series 60 3.1
         * @return None
         */
        void WLANConnectionLost();

    public: // Public destructor

        /**
         * Destructor.
         */
        virtual ~CUpnpBrowseTask();

    private: // Private construct/destruct methods

        /**
         * Perform the first phase of two phase construction.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpShowTask, casted to type CUpnpTask
         */
        CUpnpBrowseTask();

        /**
         * Perform the second phase of two phase construction. Reserves the
         * Upnp Fw resources (they are released when the task is destroyed).
         */
        void ConstructL();

    private: // Data members

        /**
         * Pointer to the UpnpAvController instance. Owned.
         */
        MUPnPAVController* iAVController;

    };

#endif // UPNP_BROWSE_TASK_H

// End of File
