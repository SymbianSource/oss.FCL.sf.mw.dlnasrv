/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      The factory for AVController client
*
*/






// INCLUDES
// dlnasrv / avcontroller api
#include "upnpavcontrollerfactory.h"
#include "upnpavcontrollerglobals.h"

// dlnasrv / avcontroller internal
#include "upnpavcontrolleractive.h"
#include "upnpavcontrollerclient.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// UPnPAVControllerFactory::NewUPnPAVControllerL
// Two-phase construct
// --------------------------------------------------------------------------
EXPORT_C MUPnPAVController* UPnPAVControllerFactory::NewUPnPAVControllerL()
    {
    return CUPnPAVControllerActive::NewL();
    }
    
// --------------------------------------------------------------------------
// UPnPAVControllerFactory::NewUPnPAVControllerLC
// Two-phase construct
// --------------------------------------------------------------------------
EXPORT_C MUPnPAVController* UPnPAVControllerFactory::NewUPnPAVControllerLC()
    {
    return CUPnPAVControllerActive::NewLC();
    }

// --------------------------------------------------------------------------
// UPnPAVControllerFactory::Status
// See upnpavcontrollerfactory.h
// --------------------------------------------------------------------------
EXPORT_C TInt UPnPAVControllerFactory::Status(
    TAVControllerServerStatus& aStatus )
    {
    TFindServer findAVControllerServer( KAVControllerName );
    TFullName name;

    if ( findAVControllerServer.Next( name ) == KErrNone )
        {
        // Server running
        aStatus = EStatusActive;
        }
    else
        {
        // Server not running
        aStatus = EStatusOffline;
        }
    return KErrNone;    
    }

// end of file
