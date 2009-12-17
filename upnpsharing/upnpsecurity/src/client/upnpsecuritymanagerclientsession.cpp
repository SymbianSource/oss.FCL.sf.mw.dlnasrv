/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  upnpsecuritymanager / client side
 *
 */

// INCLUDES
#include <e32base.h>
#include "upnpsecuritymanagerclientservercommon.h"
#include "upnpsecuritymanagerclientsession.h"

// CONSTANTS
const TInt KServerMessageSlots = 1; //Number of message slots available per session.
const TInt KServerRetryCount = 2;

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::RUpnpSecurityManagerClientSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RUpnpSecurityManagerClientSession::RUpnpSecurityManagerClientSession() :
RUpnpSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::Connect
// Establish connection to server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpSecurityManagerClientSession::Connect()
    {
    return RUpnpSessionBase::Connect( KUpnpSecurityManagerName, TVersion(
                    KUpnpSecurityManagerMajorVersionNumber,
                    KUpnpSecurityManagerMinorVersionNumber,
                    KUpnpSecurityManagerBuildVersionNumber ), KServerMessageSlots,
            KServerRetryCount, KUpnpSecurityManagerFilename, KServerUid3 );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::AddFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpSecurityManagerClientSession::AddFileL(
        const TDesC& aFileName )
    {
    User::LeaveIfError( SendReceive( EAddFile, TIpcArgs( &aFileName ) ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::RemoveFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpSecurityManagerClientSession::RemoveFileL(
        const TDesC& aFileName )
    {
    User::LeaveIfError( SendReceive( ERemoveFile, TIpcArgs( &aFileName ) ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::AddAddressL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpSecurityManagerClientSession::AddAddressL(
        const TInetAddr& aIpAddress )
    {
    TPckg<TInetAddr> addressPck( aIpAddress );
    User::LeaveIfError( SendReceive( EAddAddress, TIpcArgs( &addressPck ) ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::RemoveAddressL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpSecurityManagerClientSession::RemoveAddressL(
        const TInetAddr& aIpAddress )
    {
    TPckg<TInetAddr> addressPck( aIpAddress );
    User::LeaveIfError( SendReceive( ERemoveAddress, TIpcArgs( &addressPck ) ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::ResetFileListL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpSecurityManagerClientSession::ResetFileListL()
    {
    User::LeaveIfError( SendReceive( EResetFileList ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::ResetAddressListL
// -----------------------------------------------------------------------------
//

EXPORT_C void RUpnpSecurityManagerClientSession::ResetAddressListL()
    {
    User::LeaveIfError( SendReceive( EResetAddressList ) );
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::CheckAuthorizationL
// -----------------------------------------------------------------------------
//
EXPORT_C TAccessType RUpnpSecurityManagerClientSession::CheckAuthorizationL(
        const TInetAddr& aIpAddress, const TDesC& aFileName )
    {
    TAccessType accessType = ENoneAllowed;
    TPckg<TAccessType> accessTypeBuf( accessType );
    TPckg<TInetAddr> addressPck( aIpAddress );
    User::LeaveIfError( SendReceive( ECheckAuthorization, TIpcArgs( &addressPck,
                            &aFileName, &accessTypeBuf ) ) );

    return accessType;
    }

// -----------------------------------------------------------------------------
// RUpnpSecurityManagerClientSession::QueryAuthorisationL
// -----------------------------------------------------------------------------
//
EXPORT_C TAccessType RUpnpSecurityManagerClientSession::QueryAuthorisationL(
        const TInetAddr& aIpAddress )
    {
    TAccessType accessType = ENoneAllowed;
    TPckg<TAccessType> accessTypeBuf(accessType);
    TPckg<TInetAddr> addressPck( aIpAddress );
    User::LeaveIfError( SendReceive( EQueryAuthorisation, TIpcArgs( &addressPck,
                            &accessTypeBuf ) ) );

    if ( accessType == ENoneAllowed )
        {
        User::Leave( KErrAccessDenied );
        }

    return accessType;
    }

//  End of File
