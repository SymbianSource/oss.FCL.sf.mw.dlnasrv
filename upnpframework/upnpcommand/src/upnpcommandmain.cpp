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


#include <e32base.h>
#include "loadupnpcommand.h" // LoadUpnpCommandL inline method
#include "upnpcommandmain.h" // this class

// These methods are here to preserve binary compatibility of
// upnpcommand.dll by keeping the DLL export table ordinals
// unchanged.


// METHOD IMPLEMENTATION

CUpnpCommand* UpnpCommandMain::LoadL(
        UpnpCommand::TUpnpCommandId aCommandId,
        MUpnpCommandCallback* aCallback )
    {
    return LoadUpnpCommandL( aCommandId, aCallback );
    }

EXPORT_C void UpnpCommandReserved1()
    {
    }

EXPORT_C void UpnpCommandReserved2()
    {
    }

// End of File
