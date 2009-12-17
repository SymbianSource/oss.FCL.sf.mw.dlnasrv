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
* Description:      ECOM proxy table for GSDataCallPlugin
*
*/






// INCLUDES
#include "upnpgspluginview.h"
#include "upnpgsplugin.hrh"

#include <e32std.h>
#include <ecom/implementationproxy.h> 

_LIT( KComponentLogfile, "upnpgsplugin.txt");
#include "upnplog.h"

// CONSTANTS
const TImplementationProxy KUPnPGSPluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 
            KUPnPGSPluginImplementationUid, 
            CUPnPGSPluginView::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                  TInt& aTableCount )
    {
    __LOG( "CUPnPGSPluginView::ImplementationGroupProxy" );
    aTableCount = sizeof( KUPnPGSPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
    __LOG( "CUPnPGSPluginView::ImplementationGroupProxy-end" );    
    return KUPnPGSPluginImplementationTable;
    }



// End of File
