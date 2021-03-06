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
* Description:      Defines ecom interface table
*
*/






#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "upnpmusicplayer.h"

#include "upnpmusicdownloadproxy.h"

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x200075D8, CUPnPMusicPlayer::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x200075D9, CUPnPMusicDownloadProxy::NewL )
    };


// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) /
        sizeof(TImplementationProxy);
    return ImplementationTable;
    }

