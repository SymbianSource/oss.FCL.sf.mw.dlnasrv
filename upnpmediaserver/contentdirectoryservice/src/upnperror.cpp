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
        if(aErrCode == EUpnpOk)                     iUpnpErrCode = EHttpOk;
        else if(aErrCode == EHttpOk)                iUpnpErrCode = EHttpOk;
        else if(aErrCode == EBadRequest)            iUpnpErrCode = EBadRequest;
        else if(aErrCode == EInvalidAction)         iUpnpErrCode = EInvalidAction;
        else if(aErrCode == EInvalidArgs)           iUpnpErrCode = EInvalidArgs;
        else if(aErrCode == EInvalidVar)            iUpnpErrCode = EInvalidVar;
        else if(aErrCode == EPreconditionFailed)    iUpnpErrCode = EPreconditionFailed;
        else if(aErrCode == EInternalServerError)   iUpnpErrCode = EInternalServerError;
        else if(aErrCode == EActionFailed)          iUpnpErrCode = EActionFailed;
        else if(aErrCode == EArgumentValue)         iUpnpErrCode = EArgumentValue;
        else if(aErrCode == EArgumentRange)         iUpnpErrCode = EArgumentRange;
        else if(aErrCode == ENotImplemented)        iUpnpErrCode = ENotImplemented;
        else if(aErrCode == EOutOfMemory)           iUpnpErrCode = EOutOfMemory;
        else if(aErrCode == EHumanIntervention)     iUpnpErrCode = EHumanIntervention;
        else if(aErrCode == EStringTooLong)         iUpnpErrCode = EStringTooLong;
        else if(aErrCode == ENotAuthorized)         iUpnpErrCode = ENotAuthorized;
        else if(aErrCode == ESignatureFailure)      iUpnpErrCode = ESignatureFailure;
        else if(aErrCode == ESignatureMissing)      iUpnpErrCode = ESignatureMissing;
        else if(aErrCode == ENotEncrypted)          iUpnpErrCode = ENotEncrypted;
        else if(aErrCode == EInvalidSequence)       iUpnpErrCode = EInvalidSequence;
        else if(aErrCode == EInvalidUrl)            iUpnpErrCode = EInvalidUrl;
        else if(aErrCode == ENoSession)             iUpnpErrCode = ENoSession;
        else if(aErrCode == ENoSuchObject)          iUpnpErrCode = ENoSuchObject;
        else if(aErrCode == EInvalidCurrentTag)     iUpnpErrCode = EInvalidCurrentTag;
        else if(aErrCode == EInvalidNewTag)         iUpnpErrCode = EInvalidNewTag;
        else if(aErrCode == ERequiredTag)           iUpnpErrCode = ERequiredTag;
        else if(aErrCode == EReadOnlyTag)           iUpnpErrCode = EReadOnlyTag;
        else if(aErrCode == EParameterMismatch)     iUpnpErrCode = EParameterMismatch;
        else if(aErrCode == EInvalidSearch)         iUpnpErrCode = EInvalidSearch;
        else if(aErrCode == EInvalidSort)           iUpnpErrCode = EInvalidSort;
        else if(aErrCode == ENoContainer)           iUpnpErrCode = ENoContainer;
        else if(aErrCode == ERestrictedObject)      iUpnpErrCode = ERestrictedObject;
        else if(aErrCode == EBadMetadata)           iUpnpErrCode = EBadMetadata;
        else if(aErrCode == ERestrictedParentObject)iUpnpErrCode = ERestrictedParentObject;
        else if(aErrCode == ENoSourceResource)      iUpnpErrCode = ENoSourceResource;
        else if(aErrCode == ESourceAccess)          iUpnpErrCode = ESourceAccess;
        else if(aErrCode == ETransferBusy)          iUpnpErrCode = ETransferBusy;
        else if(aErrCode == ENoFileTransfer)        iUpnpErrCode = ENoFileTransfer;
        else if(aErrCode == ENoDestinationResource) iUpnpErrCode = ENoDestinationResource;
        else if(aErrCode == EDestinationAccess)     iUpnpErrCode = EDestinationAccess;
        else if(aErrCode == ECannotProcess)         iUpnpErrCode = ECannotProcess;
        else if(aErrCode == EUndefined)             iUpnpErrCode = EUndefined;
        // all other
        else iUpnpErrCode = EActionFailed;
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
