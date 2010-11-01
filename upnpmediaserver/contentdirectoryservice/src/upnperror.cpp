/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  UPnP Error - this is internal class that converts 
*                TInt to TUpnpErrorCode it helps to avoid excessive casting                    
*
*/



// INCLUDE FILES
#include    "upnperror.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TUpnpError::TUpnpError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpError::TUpnpError(TInt aErrCode)
    {
    if(aErrCode == EUpnpOk)                     
        {
        iUpnpErrCode = EHttpOk;
        }
    else if ( ( aErrCode <= 620 && aErrCode >= 600 )
           || ( aErrCode <= 720 && aErrCode >= 701 )
           || aErrCode == EUpnpUndefined
           || aErrCode == EHttpOk
           || aErrCode == EBadRequest
           || aErrCode == EInvalidAction
           || aErrCode == EInvalidArgs
           || aErrCode == EInvalidVar
           || aErrCode == EPreconditionFailed
           || aErrCode == EInternalServerError 
           || aErrCode == EUndefined )
        {
        iUpnpErrCode = (TUpnpErrorCode)aErrCode;
        }
    // all other
    else 
        {
        iUpnpErrCode = EActionFailed;
        }
    }

// -----------------------------------------------------------------------------
// TUpnpError::TUpnpErrorCode
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpError::operator TUpnpErrorCode()
{
    return iUpnpErrCode;
}

//  End of File
