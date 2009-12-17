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
 * Description:  Defines the CUpnpSecAccessController class
 *
 */

// INCLUDES
#include <e32base.h>
#include <upnpmediaserversettings.h>
#include "upnpsecaccesscontroller.h"
#include "upnpsecuritymanagerclientsession.h"

_LIT( KComponentLogfile, "upnpsecurity.txt");
#include "upnplog.h"

// CONSTANTS

_LIT8( KSecurityPluginName, "UpnpSecurityManager" );
_LIT( KMediaServerXMLFile, "MediaServer1\\MediaServer1.xml" );
_LIT( KConnectionManagerXMLFile, "MediaServer1\\ConnectionManager1.xml" );
_LIT( KContentDirectoryXMLFile, "MediaServer1\\ContentDirectory1.xml" );
_LIT( KIconFile0, "MediaServer1\\icon\\0" );
_LIT( KIconFile1, "MediaServer1\\icon\\1" );
_LIT( KIconFile2, "MediaServer1\\icon\\2" );
_LIT( KIconFile3, "MediaServer1\\icon\\3" );
_LIT8( KGetProtocolInfo, "GetProtocolInfo" );

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSecAccessController* CUpnpSecAccessController::NewL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::NewL" );

    CUpnpSecAccessController* self = new (ELeave) CUpnpSecAccessController;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpSecAccessController::ConstructL()
    {
    iSecurityManagerClientSession = new (ELeave) RUpnpSecurityManagerClientSession();
    User::LeaveIfError( iSecurityManagerClientSession->Connect( ) );
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::CUpnpSecAccessController
// Constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecAccessController::CUpnpSecAccessController()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
CUpnpSecAccessController" );
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::~CUpnpSecAccessController
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSecAccessController::~CUpnpSecAccessController()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
~CUpnpSecAccessController" );
    if ( iSecurityManagerClientSession )
        {
        iSecurityManagerClientSession->Close();
        delete iSecurityManagerClientSession;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::EnableMediaServerSecurityL
// Enables the security plugin in Media Server.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSecAccessController::EnableMediaServerSecurityL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
EnableMediaServerSecurityL" );
    SetSecurityManagerSettingL( KSecurityPluginName );
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::DisableMediaServerSecurityL
// Disables the security plugin in Media Server.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSecAccessController::DisableMediaServerSecurityL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
DisableMediaServerSecurityL" );
    SetSecurityManagerSettingL( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::SetSecurityManagerSettingL
// -----------------------------------------------------------------------------
//
void CUpnpSecAccessController::SetSecurityManagerSettingL( const TDesC8& aSettingToSet )
    {
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );
    User::LeaveIfError( settings->SetL(
                    UpnpMediaServerSettings::ESecurityManager, aSettingToSet ) );
    // Clean up
    CleanupStack::PopAndDestroy( settings );    
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::IsMediaServerSecurityEnabledL
// Returns the status of the Media Server security plugin.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSecAccessController::IsMediaServerSecurityEnabledL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
IsMediaServerSecurityEnabledL" );

    TBool returnValue = EFalse;
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );
    HBufC8* value =
    settings->GetL( UpnpMediaServerSettings::ESecurityManager );
    CleanupStack::PushL( value );
    if ( value && value->Des() != KNullDesC8 )
        {
        returnValue = ETrue;
        }
    // Clean up
    CleanupStack::PopAndDestroy( value );
    value = NULL;
    CleanupStack::PopAndDestroy( settings );
    settings = NULL;
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::AddAllowedFile
// Adds file to notifier allowed list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::AddAllowedFile(
        const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::AddAllowedFile" );

    TInt error = KErrNone;
    if ( !IsAllowedFile( aFileName ) )
        {
        TRAP(error, iSecurityManagerClientSession->AddFileL( aFileName ));
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::RemoveAllowedFile
// Removes file from notifier allowed list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::RemoveAllowedFile(
        const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::RemoveAllowedFile" );

    TInt error = KErrNone;
    if ( !IsAllowedFile( aFileName ) )
        {
        // We're not expecting an answer...
        TRAP(error, iSecurityManagerClientSession->RemoveFileL( aFileName ));
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::AddAllowedAddress
// Adds IP Address to notifier allowed list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::AddAllowedAddress(
        const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::AddAllowedAddress" );
    TInt error = KErrNone;
    TRAP( error, iSecurityManagerClientSession->AddAddressL( aIpAddress ));
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::RemoveAllowedAddress
// Removes IP address from notifier allowed list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::RemoveAllowedAddress(
        const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::AddAllowedAddress" );
    TInt error = KErrNone;
    TRAP( error, iSecurityManagerClientSession->RemoveAddressL( aIpAddress ));
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::ResetFileList
// Resets allowed files list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::ResetFileList()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::ResetFileList" );

    TInt error = KErrNone;
    TRAP( error, iSecurityManagerClientSession->ResetFileListL() );
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::ResetAddressList
// Resets allowed addresses list.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSecAccessController::ResetAddressList()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::ResetAddressList" );

    TInt error = KErrNone;
    TRAP( error, iSecurityManagerClientSession->ResetAddressListL());
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::QueryAuthorizationL
// Query user to authorize connection from an IP-address.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSecAccessController::QueryAuthorizationL(
        const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController:: QueryAuthorizationL" );

    iSecurityManagerClientSession->QueryAuthorisationL( aIpAddress );
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::CheckAuthorizationL
// Checks if the given IP-address/action/file is authorized.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSecAccessController::CheckAuthorizationL(
        const TInetAddr& aIpAddress, const TDesC& aFileName,
        const TDesC8& aActionName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::\
CheckAuthorizationL" );

    TBool authorized = EFalse;

    // First, check if the access to the file can be automatically authorized
    if ( aFileName != KNullDesC )
        {
        authorized = IsAllowedFile( aFileName );
        }

    // If file was not authorized, check the action
    if ( !authorized && aActionName != KNullDesC8 )
        {
        authorized = IsAllowedAction( aActionName );
        }

    // If neither file or action were authorized, query the authorization
    // from the notifier
    if ( !authorized )
        {
        TAccessType accessType = iSecurityManagerClientSession->CheckAuthorizationL(
                aIpAddress, aFileName );

        // If the address, file or both are allowed, authorize the access
        if ( accessType == EAddressAllowed || accessType == EFileAllowed
                || accessType == EFileAndAddressAllowed )
            {
            authorized = ETrue;
            }
        }

    // Leave if not authorized
    if ( !authorized )
        {
        User::Leave( KErrAccessDenied );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::IsAllowedFile
// Checks if the requested file is automatically allowed.
// -----------------------------------------------------------------------------
//
TBool CUpnpSecAccessController::IsAllowedFile( const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::IsAllowedFile" );

    TBool returnValue = EFalse;
    if ( aFileName.Length( ) > 0 )
        {
        // Do folded find (non-case-sensitive)
        if ( aFileName.FindF( KMediaServerXMLFile ) != KErrNotFound
                || aFileName.FindF( KConnectionManagerXMLFile )
                    != KErrNotFound
                || aFileName.FindF( KContentDirectoryXMLFile )
                    != KErrNotFound
                || aFileName.FindF( KIconFile0 ) != KErrNotFound
                || aFileName.FindF( KIconFile1 ) != KErrNotFound
                || aFileName.FindF( KIconFile2 ) != KErrNotFound
                || aFileName.FindF( KIconFile3 ) != KErrNotFound )
            {
            returnValue = ETrue;
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CUpnpSecAccessController::IsAllowedAction
// Checks if the requested action is automatically allowed.
// -----------------------------------------------------------------------------
//
TBool CUpnpSecAccessController::IsAllowedAction( const TDesC8& aActionName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecAccessController::IsAllowedAction" );

    TBool returnValue = EFalse;
    if ( aActionName.Length( ) > 0 )
        {
        if ( aActionName.CompareF( KGetProtocolInfo ) == 0 )
            {
            returnValue = ETrue;
            }
        }
    return returnValue;
    }

//  End of File
