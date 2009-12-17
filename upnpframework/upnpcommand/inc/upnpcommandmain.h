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
* Description:  UpnpCommand helper API main module
*
*/


#ifndef UPNP_COMMAND_MAIN_H
#define UPNP_COMMAND_MAIN_H

// INCLUDES
#include <e32base.h>
#include "upnpcommand.h"

// FORWARD DECLARATIONS
class MUpnpCommandCallback;

// CONSTANTS

// CLASS DEFINITION

/**
 * Suport class for Upnp command helper API
 * implementation
 */
class UpnpCommandMain
    {
    public:
        /**
         * Loads upnp command by given ID
         */
        static CUpnpCommand* LoadL(
            UpnpCommand::TUpnpCommandId aCommandId,
            MUpnpCommandCallback* aCallback = 0 );

    };

// These methods are here to preserve binary compatibility of
// upnpcommand.dll by keeping the DLL export table ordinals
// unchanged.

IMPORT_C void UpnpCommandReserved1();
IMPORT_C void UpnpCommandReserved2();

#endif // UPNP_COMMAND_MAIN_H

// End of File
