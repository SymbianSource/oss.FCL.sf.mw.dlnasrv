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
 * Description:  Defines CUpnpSecurityManagerEngine
 *
 */
// INCLUDES
#include <e32property.h> 
#include "upnpsecuritymanagerengine.h"
#include "upnpsecuritydbconnection.h"
_LIT( KComponentLogfile, "upnpsecurityserver.txt");
#include "upnplog.h"

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::CUpnpSecurityManagerEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerEngine::CUpnpSecurityManagerEngine()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::\
CUpnpSecurityManagerEngine" );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::ConstructL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::ConstructL" );

    // create database connection object
    iDbConnection = CUpnpSecurityDbConnection::NewL();
    iDbConnection->OpenDatabaseL();

    // Clean the access lists after phone reboot 
    ResetListsOnFirstRunL();

    // load lists from database
    iDbConnection->GetAllFilenamesL( iAllowedFiles );
    iDbConnection->GetAllIpAddressesL( iAllowedAddresses );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::ResetListsOnFirstRunL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::ResetListsOnFirstRunL()
    {
    // create RProperty used as reboot marker
    TInt err = RProperty::Define( RProcess().SecureId(), EUPnPSecManFirstRun,
        RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        // property wasn't defined, so it's the first run and we clear lists
        User::LeaveIfError( err ); // other errors may occur

        // clear lists
        ResetFileListL();
        ResetAddressListL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerEngine* CUpnpSecurityManagerEngine::NewL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::NewL" );

    CUpnpSecurityManagerEngine* self =
            new (ELeave) CUpnpSecurityManagerEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::CUpnpSecurityManagerEngine
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerEngine::~CUpnpSecurityManagerEngine()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::\
~CUpnpAuthorisationNotifier" );

    // Reset whitelists
    iAllowedFiles.ResetAndDestroy();
    iAllowedAddresses.Reset();

    delete iDbConnection;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::ResetFileListL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::ResetFileListL()
    {
    iAllowedFiles.ResetAndDestroy();
    iDbConnection->DeleteAllFilenamesL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::ResetAddressListL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::ResetAddressListL()
    {
    iAllowedAddresses.Reset();
    iDbConnection->DeleteAllIpAddressesL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::CheckAuthorizationL
// -----------------------------------------------------------------------------
//
TAccessType CUpnpSecurityManagerEngine::CheckAuthorization(
    const TInetAddr& aIpAddress, const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::CheckAuthorization" );

    TAccessType accessType = ENoneAllowed;

    // Check the lists 
    TInt addressFound = KErrNotFound;
    if ( aIpAddress != TInetAddr( KInetAddrNone ) )
        {
        addressFound = FindAddressFromArray( aIpAddress );
        }
    TInt fileFound = KErrNotFound;
    if ( aFileName.Length() > 0 )
        {
        fileFound = FindFileFromArray( aFileName );
        }

    // Neither address or file was not
    if ( (addressFound == KErrNotFound) && (fileFound == KErrNotFound) )
        {
        accessType = ENoneAllowed;
        }
    // Address could be found but file not
    else if ( (addressFound >= 0) && (fileFound == KErrNotFound) )
        {
        accessType = EAddressAllowed;
        }
    // File could be found but not address
    else if ( (addressFound == KErrNotFound) && (fileFound >= 0) )
        {
        accessType = EFileAllowed;
        }
    // Both were in the white lists
    else
        {
        accessType = EFileAndAddressAllowed;
        }

    return accessType;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::AddNewFileL
// Adds new file to allowed file array, if not existing yet.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::AddNewFileL( const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::AddNewFileL" );

    if ( FindFileFromArray( aFileName ) == KErrNotFound )
        {
        HBufC* tmpBuffer = aFileName.AllocLC();
        iAllowedFiles.AppendL( tmpBuffer );
        CleanupStack::Pop( tmpBuffer );
        iDbConnection->AddFilenameL( aFileName );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::RemoveFileL
// Removes file from allowed array.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::RemoveFileL( const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::RemoveFileL" );

    TInt index = FindFileFromArray( aFileName );
    if ( index != KErrNotFound )
        {
        __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine, \
file removed from whitelist." );
        delete iAllowedFiles[index];
        iAllowedFiles.Remove( index );
        iDbConnection->DeleteFilenameL( aFileName );
        }
    else
        {
        __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine, \
FILE NOT FOUND!" );
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::FindFileFromArray
// Finds file from array and returns the index.
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityManagerEngine::FindFileFromArray( const TDesC& aFileName )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::\
FindFileFromArray" );

    TInt retVal = KErrNotFound;
    for ( TInt index = 0; index < iAllowedFiles.Count(); ++index )
        {
        // Check if file names are the same
        // Use folding to remove case sensitiveness
        if ( aFileName.CompareF( *iAllowedFiles[index] ) == 0 )
            {
            retVal = index;
            break;
            }
        }

    __LOG2( "CUpnpSecurityManagerEngine::FindFileFromArray(%S) retVal=%d",
            &aFileName, retVal );

    return retVal;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::AddNewAddressL
// Adds new address to allowed address array, if not existing yet.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::AddNewAddressL( const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::AddNewAddressL" );

    if ( FindAddressFromArray( aIpAddress ) == KErrNotFound )
        {
        iAllowedAddresses.AppendL( aIpAddress );
        iDbConnection->AddIpAddressL( aIpAddress );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::RemoveAddressL
// Removes existing address from IP address array.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerEngine::RemoveAddressL( const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::RemoveAddressL" );

    TInt index = FindAddressFromArray( aIpAddress );
    if ( index != KErrNotFound )
        {
        iAllowedAddresses.Remove( index );
        iDbConnection->DeleteIpAddressL( aIpAddress );
        __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine, \
address remove from array." );
        }
    else
        {
        __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine, \
address NOT found from the array!" );
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::FindAddressFromArray
// Finds file from array and returns the index.
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityManagerEngine::FindAddressFromArray(
    const TInetAddr& aIpAddress )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerEngine::\
FindAddressFromArray" );

    TInt retVal = KErrNotFound;
    for ( TInt index = 0; index < iAllowedAddresses.Count(); ++index )
        {
        // Compare only IP Address
        if ( iAllowedAddresses[index].Address() == aIpAddress.Address() )
            {
            retVal = index;
            break;
            }
        }

    TFileName ipAddress;
    aIpAddress.Output( ipAddress );
    __LOG2( "CUpnpSecurityManagerEngine::FindAddressFromArray(%S) retVal=%d",
            &ipAddress, retVal );

    return retVal;
    }

// End of File
