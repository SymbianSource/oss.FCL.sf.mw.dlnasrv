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
* Description:  Header file for the MUpnpTaskHandler interface class.
*
*/


#ifndef UPNP_TASK_HANDLER_H
#define UPNP_TASK_HANDLER_H

// INCLUDES
#include <e32base.h>

/**
* This class defines the UpnpTaskHandler interface class, that is used in
* the communication between the UpnpCommandImplementation and UpnpTask 
* classes.
*
* @since S60 3.2
*/
class MUpnpTaskHandler
    {
    public:

        /**
         * Destroys the ongoing task.
         *
         * @since S60 3.2
         */
        virtual void DestroyTask() = 0;
    };

#endif // UPNP_TASK_HANDLER_H

// End of File