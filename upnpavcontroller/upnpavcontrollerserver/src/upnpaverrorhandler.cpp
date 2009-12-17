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
* Description:      errror handling in AVController
*
*/






// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <upnperrors.h>

#include "upnpaverrorhandler.h"

// CONSTANTS
const TInt KMaxSymbianErrorCode = -100;
const TInt KMinActionErrorCode = 700;

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertToSymbianErrorCode
// Returns the Symbian error code (e32err.h) corresponding the given UPnP
// error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertToSymbianErrorCode(
                              TInt aUPnPErrorCode,
                              TUPnPErrorCodeType aErrorType )
    {
    TInt convertedErrorCode = KErrGeneral;

    if( aUPnPErrorCode == EHttpOk ||  /* 200 */
        aUPnPErrorCode == KErrNone )
        {
        convertedErrorCode = KErrNone;
        }
    // Don't convert Symbian error codes
    else if( aUPnPErrorCode < 0 &&
             aUPnPErrorCode > KMaxSymbianErrorCode ) 
        {
        convertedErrorCode = aUPnPErrorCode;
        }
    else
        {
        if ( aUPnPErrorCode > 0 && 
             aUPnPErrorCode < KMinActionErrorCode )
            {
            convertedErrorCode = ConvertGeneralErrorCode( aUPnPErrorCode );
            }
        else if( aErrorType == EUPnPConnectionManagerError )
            {
            convertedErrorCode = ConvertCMErrorCode( aUPnPErrorCode );
            }
        else if( aErrorType == EUPnPContentDirectoryError )
            {
            convertedErrorCode = ConvertCDSErrorCode( aUPnPErrorCode );
            }
        else if( aErrorType == EUPnPRenderingControlError )
            {
            convertedErrorCode = ConvertRCErrorCode( aUPnPErrorCode );
            }
        else if( aErrorType == EUPnPAVTransportError )
            {
            convertedErrorCode = ConvertAVTErrorCode( aUPnPErrorCode );
            }
        else if( aErrorType == EUPnPHTTPError )
            {
            convertedErrorCode = ConvertHTTPErrorCode( aUPnPErrorCode );
            }
        else
            {
            convertedErrorCode = KErrGeneral; 
            }
        }

    return convertedErrorCode;
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertGeneralErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the
// given general UPnP error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertGeneralErrorCode(
                             TInt aUPnPErrorCode )
    {
    TInt convertedErrorCode = KErrGeneral;

    switch( aUPnPErrorCode )
        {
        case EInvalidAction: /* 400 */
            // fall through
        case EInvalidVar: /* 404 */
            // fall through
        case ENotImplemented: /* 602 */
            {
            convertedErrorCode = KErrNotSupported;
            break;
            }
        case EInvalidArgs: /* 402 */
            // fall through
        case EArgumentValue: /* 600 */
            // fall through
        case EArgumentRange: /* 601 */
            // fall through
        case EStringTooLong: /* 605 */
            {
            convertedErrorCode = KErrArgument;
            break;
            }
        case EInternalServerError: /* 500 */
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        case EActionFailed: /* 501 */
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        case EHttpInsufficientStorage:
            {
            convertedErrorCode = KErrDiskFull;
            break;
            }     
        case EOutOfMemory: /* 603 */
            {
            //Server has no memory
            convertedErrorCode = KErrServerBusy;
            break;
            }
        case EHumanIntervention: /* 604 */
            {
            convertedErrorCode = KErrDied;
            break;
            }
        case ENotAuthorized: /* 606 */
            // fall through
        case ESignatureFailure: /* 607 */
            // fall through
        case ESignatureMissing: /* 608 */
            // fall through
        case ENotEncrypted: /* 609 */
            {
            convertedErrorCode = KErrAccessDenied;
            break;
            }
        case EInvalidUrl: /* 611 */
            // fall through
        case ENoSession: /* 612 */
            {
            convertedErrorCode = KErrNotFound;
            break;
            }
        default:
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        }
    return convertedErrorCode; 
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertCMErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the 
// given Connection Manager error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertCMErrorCode(
                              TInt aUPnPErrorCode )
    {
    TInt convertedErrorCode = KErrGeneral;
    switch( aUPnPErrorCode )
        {
        case ENoSuchObject: /* 701 */
            // fall through
        case EInvalidCurrentTag: /* 702 */
            // fall through
        case EInvalidNewTag: /* 703 */
            // fall through
        case ERequiredTag: /* 704 */
            {
            convertedErrorCode = KErrCouldNotConnect;
            break;
            }
        case EReadOnlyTag: /* 705 */
            {
            convertedErrorCode = KErrAccessDenied;
            break;
            }
        case EParameterMismatch : /* 706 */
            {
            convertedErrorCode = KErrArgument;
            break;
            }
        default:
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        }
    return convertedErrorCode; 
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertCDSErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the 
// given Content Directory error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertCDSErrorCode(
                              TInt aUPnPErrorCode )
    {
    TInt convertedErrorCode = KErrGeneral;
    switch( aUPnPErrorCode )
        {
        case ENoSuchObject: /* 701 */
            // fall through
        case ENoContainer: /* 710 */
            // fall through
        case ENoSourceResource: /* 714 */
            // fall through
        case ENoDestinationResource: /* 718 */
            {
            convertedErrorCode = KErrNotFound;
            break;
            }
        case EInvalidCurrentTag: /* 702 */
            // fall through
        case EInvalidNewTag: /* 703 */
            // fall through
        case EParameterMismatch : /* 706 */
            // fall through
        case EBadMetadata: /* 712 */
            {
            convertedErrorCode = KErrArgument;
            break;
            }
        case ERequiredTag: /* 704 */
            // fall through
        case EReadOnlyTag: /* 705 */
            // fall through
        case ERestrictedObject: /* 711 */
            // fall through
        case ERestrictedParentObject: /* 713 */
            // fall through
        case ESourceAccess: /* 715 */
            // fall through
        case EDestinationAccess: /* 719 */
            {
            convertedErrorCode = KErrAccessDenied;
            break;
            }
        case EInvalidSearch: /* 708 */
            // fall through
        case EInvalidSort: /* 709 */
            {
            convertedErrorCode = KErrNotSupported;
            break;
            }
        case ETransferBusy: /* 716 */
            {
            convertedErrorCode = KErrInUse;
            break;
            }
       default:
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        }
    return convertedErrorCode; 
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertRCErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the 
// given Rendering Control error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertRCErrorCode(
                              TInt aUPnPErrorCode )
    {
    TInt convertedErrorCode = KErrGeneral;
    switch( aUPnPErrorCode )
        {
        case ENoSuchObject: /* 701 */
            // fall through
        case EInvalidCurrentTag: /* 702 */
            {
            convertedErrorCode = KErrArgument;
            break;
            }
       default:
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        }
    return convertedErrorCode; 
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertAVTErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the 
// given AV Transport error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertAVTErrorCode(
                              TInt aUPnPErrorCode )
    {
    TInt convertedErrorCode = KErrGeneral;
    switch( aUPnPErrorCode )
        {
        case ENoSuchObject: /* 701 */
            // fall through
        case ERequiredTag: /* 704 */
            // fall through
        case EInvalidSearch: /* 708 */
            // fall through
        case ENoContainer: /* 710 */
            // fall through
        case EBadMetadata: /* 712 */
            // fall through
        case ERestrictedParentObject: /* 713 */
            // fall through
        case ENoSourceResource: /* 714 */
            // fall through
        case ENoFileTransfer: /* 717 */
            {
            convertedErrorCode = KErrNotSupported;
            break;
            }
        case EInvalidCurrentTag: /* 702 */
            // fall through
        case ETransferBusy: /* 716 */
            {
            convertedErrorCode = KErrNotFound;
            break;
            }
        case EInvalidNewTag: /* 703 */
            {
            convertedErrorCode = KErrHardwareNotAvailable;
            break;
            }
        case EReadOnlyTag: /* 705 */
            {
            convertedErrorCode = KErrLocked;
            break;
            }
        case EParameterMismatch : /* 706 */
            {
            convertedErrorCode = KErrWrite;
            break;
            }
        case EInvalidSort: /* 709 */
            {
            convertedErrorCode = KErrDiskFull;
            break;
            }
        case ESourceAccess: /* 715 */
            {
            convertedErrorCode = KErrInUse;
            break;
            }
        case ENoDestinationResource: /* 718 */
            {
            convertedErrorCode = KErrArgument;
            break;
            }
       default:
            {
            convertedErrorCode = KErrGeneral;
            break;
            }
        }
    return convertedErrorCode;
    }

// --------------------------------------------------------------------------
// UPnPAVErrorHandler::ConvertHTTPErrorCode
// Returns the Symbian error code (e32err.h) corresponding to the 
// given HTTP error code.
// --------------------------------------------------------------------------
TInt UPnPAVErrorHandler::ConvertHTTPErrorCode(
                              TInt /*aUPnPErrorCode*/ )
    {
    TInt convertedErrorCode = KErrDisconnected;
        
    return convertedErrorCode;
    }

// End of File
