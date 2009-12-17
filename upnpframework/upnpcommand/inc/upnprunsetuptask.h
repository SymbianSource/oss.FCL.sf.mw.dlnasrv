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
* Description:  Header file for the CUpnpRunSetupTask class.
*
*/


#ifndef UPNP_RUN_SETUP_TASK_H
#define UPNP_RUN_SETUP_TASK_H

// INCLUDES
#include <e32base.h>
#include "upnptask.h"

/**
* This class defines the CUpnpRunSetupTask used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpRunSetupTask : public CUpnpTask
    {
    public: // Methods from CUpnpTask

        /**
         * Creates a new CUpnpRunSetupTask object. Allocates Upnp Fw
         * resources. Resources will be released when the task is destroyed.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpRunSetupTask, casted to CUpnpTask
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

    public: // Public destructor

        /**
         * Destructor.
         */
        virtual ~CUpnpRunSetupTask();

    private: // Private construct/destruct methods

        /**
         * Perform the first phase of two phase construction.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpRunSetupTask, casted to CUpnpTask
         */
        CUpnpRunSetupTask();

        /**
         * Perform the second phase of two phase construction. Reserves the
         * Upnp Fw resources (they are released when the task is destroyed).
         */
        void ConstructL();

    };

#endif // UPNP_RUN_SETUP_TASK_H

// End of File