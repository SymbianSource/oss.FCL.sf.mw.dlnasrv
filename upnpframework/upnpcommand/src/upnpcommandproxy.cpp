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
* Description:  Source file for UpnpCommand ECom plugin proxy.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "upnpcommandimplementation.h"
#include "upnpcommanduids.hrh"

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KUpnpCommandPluginImplementationUid,
                                CUpnpCommandImplementation::NewL )
    };

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                         TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) /
                  sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of File
