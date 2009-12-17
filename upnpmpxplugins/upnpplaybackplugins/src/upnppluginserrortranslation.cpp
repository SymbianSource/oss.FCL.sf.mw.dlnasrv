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
* Description:      translate all plugins error code for music player
 *
*/






// INCLUDES

// debug
#include <e32std.h>
#include "upnppluginserrortranslation.h"
_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// --------------------------------------------------------------------------
// TUpnpPluginsErrorTranslation::ErrorTranslate
// translate function
// --------------------------------------------------------------------------
//
TInt TUpnpPluginsErrorTranslation::ErrorTranslate( TInt aError)
    {
    TInt newError = KErrNone;
    if ( aError != KErrNone)
        {
        __LOG1( "TUpnpPluginsErrorTranslation  err=%d", aError );
        }
    switch ( aError)
        {
        // if aError is valid, we don't need translate
        case KErrNone:
            newError = KErrNone;
            break;
        case KErrNotFound:
            newError = KErrGeneral;
            break;
        case KErrNotSupported:
            newError = KErrNotSupported;
            break;
        case KErrCorrupt:
            newError = KErrCorrupt;
            break;
        case KErrCompletion:
            newError = KErrCompletion;
            break;
        case KErrDied:
            newError = KErrDied;
            break;
        case KErrGeneral:
            newError = KErrGeneral;
            break;
        case KErrAccessDenied:
            newError = KErrAccessDenied;
            break;
        case KErrArgument:
            newError = KErrArgument;
            break;
        case KErrDisconnected:
            newError = KErrDisconnected;
            break;
        case KErrDiskFull: //consume current disk full
            newError = KErrDiskFull;
            break;
        case KErrNoMemory://consume memory entirely
            newError = KErrNoMemory;
            break;
        case KErrOverflow:
            newError = KErrGeneral;
            break;    
       	case KErrServerBusy://if Server has no memory
            newError = KErrServerBusy;
            break;
            // if aError is not valid, we need translate
        case KErrCancel: //fall through
        case KErrTotalLossOfPrecision: ////fall through
        case KErrBadHandle: //fall through
        case KErrUnderflow: //fall through
        case KErrAlreadyExists: //fall through
        case KErrPathNotFound: //fall through
        case KErrInUse: //fall through
        case KErrServerTerminated: //fall through
        case KErrNotReady: //fall through
        case KErrUnknown: //fall through
        case KErrLocked: //fall through
        case KErrWrite: //fall through
        case KErrDisMounted: //fall through
        case KErrEof: //fall through
        case KErrBadDriver: //fall through
        case KErrBadName: //fall through
        case KErrCommsLineFail: //fall through
        case KErrCommsFrame: //fall through
        case KErrCommsOverrun: //fall through
        case KErrCommsParity: //fall through
        case KErrTimedOut: //fall through
        case KErrCouldNotConnect: //fall through
        case KErrCouldNotDisconnect: //fall through
        case KErrBadLibraryEntryPoint: //fall through
        case KErrBadDescriptor: //fall through
        case KErrAbort: //fall through
        case KErrTooBig: //fall through
        case KErrDivideByZero: //fall through
        case KErrBadPower: //fall through
        case KErrDirFull: //fall through
        case KErrHardwareNotAvailable: //fall through
        case KErrSessionClosed: //fall through
        case KErrPermissionDenied: //fall through
        case KErrExtensionNotSupported: //fall through
        case KErrCommsBreak:
            newError = KErrDisconnected;
            break;
        default:
            newError = KErrDisconnected;
            break;
        }

    return newError;

    }
