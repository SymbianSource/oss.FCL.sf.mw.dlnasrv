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
 *  Description : SQLite Connection class
 *
 */
// INCLUDES
#include <e32base.h>
#include <bautils.h>
#include <sysutil.h>
#include <in_sock.h>
#include "upnpsecuritydbconnection.h"
#include "upnpsecuritysqlclausedef.h"
// CONSTANTS
_LIT( KUpnpSecurityDbFileName, "upnpsec.sq" );
_LIT8( KUpnpSecuritySqlPragmaCacheSize,
        "PRAGMA cache_size=1024;PRAGMA page_size=4096;" );

const TInt KDatabaseMinDiskSpace = 20000;

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::NewL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityDbConnection* CUpnpSecurityDbConnection::NewL()
    {
    CUpnpSecurityDbConnection* self = CUpnpSecurityDbConnection::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::NewLC
// Second phase constructor.
// -----------------------------------------------------------------------------
//    
CUpnpSecurityDbConnection* CUpnpSecurityDbConnection::NewLC()
    {
    CUpnpSecurityDbConnection* self =
            new (ELeave) CUpnpSecurityDbConnection();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::~CUpnpSecurityDbConnection
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityDbConnection::~CUpnpSecurityDbConnection()
    {
    CloseDb();
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::CUpnpSecurityDbConnection
// Default constructor.
// -----------------------------------------------------------------------------
//    
CUpnpSecurityDbConnection::CUpnpSecurityDbConnection()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::ConstructL
// -----------------------------------------------------------------------------
//   
void CUpnpSecurityDbConnection::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    const TInt driveLetterPrefixLength( 2 );//letter + ':'
    TBuf<driveLetterPrefixLength> driveLetterPrefix;
    
    TChar driveLetter;
    RFs::DriveToChar( DatabaseDrive(), driveLetter );
    const TChar KColon( ':' );
    driveLetterPrefix.Append( driveLetter );
    driveLetterPrefix.Append( KColon );

    iFs.CreatePrivatePath( DatabaseDrive() );
    User::LeaveIfError( iFs.PrivatePath( iDbFileName ) );
    iDbFileName.Insert( 0, driveLetterPrefix );
    iDbFileName.Append( KUpnpSecurityDbFileName );

    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::OpenDb
// Opens database connection and sets Cache size. 
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityDbConnection::OpenDb( const TDesC& aDb )
    {
    CloseDb();
    return iDatabase.Open( aDb, &KUpnpSecuritySqlPragmaCacheSize );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::ExecuteL
// ExecuteL, execute SQL command.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::ExecuteL( const TDesC8& aCommand )
    {
    TInt err( iDatabase.Exec( aCommand ) );
    if ( KErrNone > err )
        {
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::CloseDb
// Close database connection.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::CloseDb()
    {
    iDatabase.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::CreateDbFile
// Creates db file.
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityDbConnection::CreateDbFile( const TDesC& aDb )
    {
    return iDatabase.Create( aDb, &KUpnpSecuritySqlPragmaCacheSize );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::Validate
// Check validity of database.
// -----------------------------------------------------------------------------
//
TBool CUpnpSecurityDbConnection::Validate()
    {
    TBool result = EFalse;
    RSqlStatement statement;
    TInt err = statement.Prepare( iDatabase, KUpnpSecSqlValidateTableExistence );
    result = !err && (statement.Next() == KSqlAtRow);
    statement.Close();
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::CreateTablesL
// Create tables.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::CreateTablesL()
    {
    ExecuteL( KUpnpSecSqlDropAddressList );
    ExecuteL( KUpnpSecSqlDropFileList );

    ExecuteL( KUpnpSecSqlCreateAddressList );
    ExecuteL( KUpnpSecSqlCreateFileList );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::OpenDatabaseL
// Prepare database for use, create new if not exist.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::OpenDatabaseL()
    {
    EnsureDiskSpaceL();

    if ( OpenDb( iDbFileName ) != KErrNone )
        {
        User::LeaveIfError( CreateDbFile( iDbFileName ) );
        }

    if ( !Validate() )
        {
        CreateTablesL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::AddIpAddressL
// Add ip address to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::AddIpAddressL( const TInetAddr& aIpAddress )
    {
    EnsureDiskSpaceL();
    ExecStatementL( KUpnpSecSqlInsertIpAddress, aIpAddress.Address() );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::AddFilenameL
// Add filename to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::AddFilenameL( const TDesC& aFilename )
    {
    EnsureDiskSpaceL();
    ExecStatementL( KUpnpSecSqlInsertFile, aFilename );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::DeleteIpAddressL
// Delete ip address to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::DeleteIpAddressL( const TInetAddr& aIpAddress )
    {
    ExecStatementL( KUpnpSecSqlDeleteIpAddress, aIpAddress.Address() );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::DeleteFilenameL
// Delete filename to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::DeleteFilenameL( const TDesC& aFilename )
    {
    ExecStatementL( KUpnpSecSqlDeleteFile, aFilename );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::DeleteAllIpAddressesL
// Delete all ip addresses to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::DeleteAllIpAddressesL()
    {
    ExecStatementL( KUpnpSecSqlDeleteAllIpAddresses );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::DeleteAllFilenamesL
// Delete all filenames to DB.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::DeleteAllFilenamesL()
    {
    ExecStatementL( KUpnpSecSqlDeleteAllFiles );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::ExecStatementL
// Execute given db command
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::ExecStatementL( const TDesC8& aCommand )
    {
    RSqlStatement statement;
    User::LeaveIfError( statement.Prepare( iDatabase, aCommand ) );
    CleanupClosePushL( statement );
    User::LeaveIfError( statement.Exec() );
    CleanupStack::PopAndDestroy( &statement );    
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::ExecStatementL
// Execute given db command
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::ExecStatementL( const TDesC8& aCommand,
    const TDesC& aTextToBind )
    {
    RSqlStatement statement;
    User::LeaveIfError( statement.Prepare( iDatabase, aCommand ) );
    CleanupClosePushL( statement );
    User::LeaveIfError( statement.BindText( 0, aTextToBind ) );
    User::LeaveIfError( statement.Exec() );
    CleanupStack::PopAndDestroy( &statement );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::ExecStatementL
// Execute given db command
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::ExecStatementL( const TDesC8& aCommand,
    TInt aIntToBind )
    {
    RSqlStatement statement;
    User::LeaveIfError( statement.Prepare( iDatabase, aCommand ) );
    CleanupClosePushL( statement );    
    User::LeaveIfError( statement.BindInt( 0, aIntToBind ) );
    User::LeaveIfError( statement.Exec() );
    CleanupStack::PopAndDestroy( &statement );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::GetAllIpAddressesL
// Get all ips.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::GetAllIpAddressesL(
    RArray<TInetAddr>& aAddressArray )
    {
    TInt err;
    RSqlStatement statement;
    User::LeaveIfError( statement.Prepare( iDatabase,
        KUpnpSecSqlSelectAllIpAddresses ) );
    CleanupClosePushL( statement );
    TInt columnIndex = statement.ColumnIndex( KUpnpSecSqlIp );

    while ( (err = statement.Next()) == KSqlAtRow )
        {
        TUint32 data = statement.ColumnInt( columnIndex );
        aAddressArray.AppendL( TInetAddr( data, 0 ) );
        }

    if ( err != KSqlAtEnd )
        {
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( &statement );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::GetAllFilenamesL
// Get all filenames. 
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::GetAllFilenamesL(
    RPointerArray<HBufC>& aFilenameArray )
    {
    TInt err;
    RSqlStatement statement;
    User::LeaveIfError( statement.Prepare( iDatabase, KUpnpSecSqlSelectAllFiles ) );
    CleanupClosePushL( statement );

    TInt columnIndex = statement.ColumnIndex( KUpnpSecSqlFilename );

    while ( (err = statement.Next()) == KSqlAtRow )
        {
        TPtrC data = statement.ColumnTextL( columnIndex );
        HBufC* filename = data.AllocLC();
        aFilenameArray.AppendL( filename );
        CleanupStack::Pop( filename );
        }

    if ( err != KSqlAtEnd )
        {
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( &statement );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::EnsureDiskSpaceL
// Leaves if there is not enough disk space 
// -----------------------------------------------------------------------------
//
void CUpnpSecurityDbConnection::EnsureDiskSpaceL()
    {
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, KDatabaseMinDiskSpace,
         DatabaseDrive()) )
        {
        User::Leave( KErrDiskFull );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityDbConnection::DatabaseDrive()
// Return drive on which database file is kept  
// -----------------------------------------------------------------------------
//
TDriveNumber CUpnpSecurityDbConnection::DatabaseDrive()
    {
    return RFs::GetSystemDrive();
    }

//  End of File
