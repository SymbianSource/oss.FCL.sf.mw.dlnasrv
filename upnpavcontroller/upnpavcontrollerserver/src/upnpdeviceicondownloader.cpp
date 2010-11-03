/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

// INCLUDES
#include <httpdownloader.h>
#include <e32property.h>
#include <sysutil.h>
#include "upnpdeviceicondownloader.h"
#include "upnpconstantdefs.h"
#include "upnpaverrorhandler.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

const TUint KBufferSize = 0x4000; // 16K
const TInt KParallerTransfers = 1;
_LIT( KIconFolder, "icons\\" );
_LIT( KIconTemp, "temp" );
_LIT( KIconExt, ".dat" );
const TInt KCleanupNeededKey = KMaxTInt;
const TInt KDefaultDiskSpaceNeeded = 0x8000; // 32K

// --------------------------------------------------------------------------
// FolderNameLC
// --------------------------------------------------------------------------
static HBufC* FolderNameLC( RFs& aFs )
    {
    HBufC* ret = HBufC::NewLC( KMaxPath );
    TPtr ptr( ret->Des() );
    aFs.PrivatePath( ptr );
    ptr.Insert( 0, TDriveUnit( RFs::GetSystemDrive() ).Name() );
    ptr.Append( KIconFolder );
    return ret;
    }

// --------------------------------------------------------------------------
// FileNameLC
// --------------------------------------------------------------------------
static HBufC* FileNameLC( RFs& aFs, const TDesC8& aDeviceUuid )
    {
    HBufC* ret = FolderNameLC( aFs );
    HBufC* uuid = HBufC::NewLC( aDeviceUuid.Length() );
    uuid->Des().Copy( aDeviceUuid );
    TPtr ptr( ret->Des() );
    ptr.Append( *uuid );
    ptr.Append( KIconExt );
    CleanupStack::PopAndDestroy( uuid );
    return ret;
    }

// --------------------------------------------------------------------------
// TempFileNameLC
// --------------------------------------------------------------------------
static HBufC* TempFileNameLC( RFs& aFs )
    {
    HBufC* ret = FolderNameLC( aFs );
    TPtr ptr( ret->Des() );
    ptr.Append( KIconTemp );
    ptr.Append( KIconExt );
    return ret;
    }

// --------------------------------------------------------------------------
// CompleteFileL
// --------------------------------------------------------------------------
static void CompleteFileL( RFs& aFs, const TDesC8& aDeviceUuid )
    {
    HBufC* src = TempFileNameLC( aFs );
    HBufC* tgt = FileNameLC( aFs, aDeviceUuid );
    User::LeaveIfError( aFs.Replace( *src, *tgt ) );
    CleanupStack::PopAndDestroy( tgt );
    CleanupStack::PopAndDestroy( src );
    }

// --------------------------------------------------------------------------
// PropertyCategory
// --------------------------------------------------------------------------
static TUid PropertyCategory()
    {
    RProcess process;
    TUid ret( TUid::Uid( process.SecureId().iId ) );
    process.Close();
    return ret;
    }

// --------------------------------------------------------------------------
// GetIntPropertyL
// --------------------------------------------------------------------------
static void GetIntPropertyL( const TUid& aCategory, TInt aKey, TInt& aValue )
    {
    TInt initValue( aValue );
    TInt err( RProperty::Get( aCategory, aKey, aValue ) );
    if ( err == KErrNotFound )
        {
        err = RProperty::Define( aCategory, aKey, RProperty::EInt );
        aValue = initValue;
        }
    __LOG3( "CUpnpDeviceIconDownloader - GetIntPropertyL - err: %d key: %d value: %d",
            err, aKey, aValue );
    User::LeaveIfError( err );
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::NewL
// --------------------------------------------------------------------------
CUpnpDeviceIconDownloader* CUpnpDeviceIconDownloader::NewL(
        MUpnpDeviceIconDownloadObserver& aObserver, TUint aIap )
    {
    CUpnpDeviceIconDownloader* self = new ( ELeave ) CUpnpDeviceIconDownloader( 
            aObserver, aIap );
    CleanupStack::PushL( self );           
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::~CUpnpDeviceIconDownloader
// --------------------------------------------------------------------------
CUpnpDeviceIconDownloader::~CUpnpDeviceIconDownloader()
    {
    iQueue.ResetAndDestroy();
    delete iDownloader;
    delete iCurrent;
    if ( iCleanupOnShutdown )
        {
        TRAP_IGNORE( CleanupFilesL() );
        }
    iFs.Close();
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::TransferProgress
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::TransferProgress( TAny* /*aKey*/, TInt /*aBytes*/,
        TInt /*aTotalBytes*/ )
    {
    __LOG( "CUpnpDeviceIconDownloader::TransferProgress" );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::ReadyForTransferL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::ReadyForTransferL( TAny* /*aKey*/ )
    {
    __LOG( "CUpnpDeviceIconDownloader::ReadyForTransferL" );
    if ( iCurrent )
        {
        iDownloader->StartTransferL( iCurrent );
        }
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::TransferCompleted
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::TransferCompleted( TAny* /*aKey*/, TInt aStatus )
    {
    __LOG( "CUpnpDeviceIconDownloader::TransferCompleted" );
    if ( iCurrent )
        {
        TInt err( UPnPAVErrorHandler::ConvertToSymbianErrorCode( aStatus, EUPnPHTTPError ) );
        __LOG2( "CUpnpDeviceIconDownloader::TransferCompleted - Http: %d Err: %d",
                aStatus, err );
        TPtrC8 uuid( iCurrent->DeviceUuid() );
        if ( err == KErrNone )
            {
            TRAP_IGNORE( CompleteFileL( iFs, uuid ) );
            }
        TRAP_IGNORE( iObserver.DeviceIconDownloadedL( uuid, err ) );
        delete iCurrent;
        iCurrent = NULL;
        }
    DownloadNext();
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::StartDownloadL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::StartDownloadL(
        const TDesC8& aDeviceUuid, const TDesC8& aDeviceIconUrl )
    {
    __LOG( "CUpnpDeviceIconDownloader::StartDownloadL" );
    CleanupFilesIfNeededL();
    if( !FileExistsL( aDeviceUuid ) && !InProgress( aDeviceUuid ) )
        {
        if ( !iDownloader )
            {
            iDownloader = CHttpDownloader::NewL( *this, iIap, KBufferSize,
                KParallerTransfers );
            }
        CEntry* entry = CEntry::NewLC( aDeviceUuid, aDeviceIconUrl );
        iQueue.AppendL( entry );
        CleanupStack::Pop( entry );
        DownloadNext();
        }
    else
        {
        __LOG( "CUpnpDeviceIconDownloader::StartDownloadL - Skipped" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CancelDownload
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::CancelDownload( const TDesC8& aDeviceUuid )
    {
    __LOG( "CUpnpDeviceIconDownloader::CancelDownload" );
    if ( iCurrent && !iCurrent->DeviceUuid().Compare( aDeviceUuid ) )
        {
        iDownloader->CancelTransfer( iCurrent );
        delete iCurrent;
        iCurrent = NULL;
        }
    else
        {
        TInt pos( iQueue.Find( aDeviceUuid, CEntry::Compare ) );
        if ( pos >= 0 && pos < iQueue.Count() )
            {
            delete iQueue[ pos ];
            iQueue.Remove( pos );
            }
        }
    DownloadNext();
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::TransferFileToClientL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::TransferFileToClientL( const RMessage2& aMessage,
        TInt aSlot, const TDesC8& aDeviceUuid )
    {
    if ( InProgress( aDeviceUuid ) )
        {
        User::Leave( KErrNotReady );
        }
    HBufC* filename = FileNameLC( iFs, aDeviceUuid );
    __LOG1( "CUpnpDeviceIconDownloader::TransferFileToClientL - %S", filename );
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Open( iFs, *filename, EFileShareReadersOnly | EFileRead ) );
    User::LeaveIfError( file.TransferToClient( aMessage, aSlot ) );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( filename );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CEntry::NewLC
// --------------------------------------------------------------------------
CUpnpDeviceIconDownloader::CEntry* CUpnpDeviceIconDownloader::CEntry::NewLC(
        const TDesC8& aDeviceUuid, const TDesC8& aDeviceIconUrl )
    {
    CEntry* self = new ( ELeave ) CEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aDeviceUuid, aDeviceIconUrl );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CEntry::~CEntry
// --------------------------------------------------------------------------
CUpnpDeviceIconDownloader::CEntry::~CEntry()
    {
    delete iDeviceUuid;
    delete iDeviceIconUrl;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CEntry::Compare
// --------------------------------------------------------------------------
TInt CUpnpDeviceIconDownloader::CEntry::Compare( const TDesC8* aDeviceUuid,
        const CUpnpDeviceIconDownloader::CEntry& aEntry )
    {
    return !aEntry.iDeviceUuid->Compare( *aDeviceUuid );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CEntry::ConstructL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::CEntry::ConstructL( const TDesC8& aDeviceUuid,
        const TDesC8& aDeviceIconUrl )
    {
    iDeviceUuid = aDeviceUuid.AllocL();
    iDeviceIconUrl = aDeviceIconUrl.AllocL();
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CUpnpDeviceIconDownloader
// --------------------------------------------------------------------------
CUpnpDeviceIconDownloader::CUpnpDeviceIconDownloader( 
        MUpnpDeviceIconDownloadObserver& aObserver, TUint aIap ) :
    iObserver( aObserver ), iIap( aIap ) 
    {
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::ConstructL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::ConstructL()
    {
    __LOG1( "CUpnpDeviceIconDownloader::ConstructL - IAP %d", iIap );
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::DownloadNext
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::DownloadNext()
    {
    while( !iCurrent && iQueue.Count() > 0  )
        {
        iCurrent = iQueue[ 0 ];
        iQueue.Remove( 0 );
        TRAPD( err, DownloadNextL( *iCurrent ) );
        if ( err != KErrNone )
            {
            __LOG1( "CUpnpDeviceIconDownloader::DownloadNext - Error: %d", err );
            TRAP_IGNORE( iObserver.DeviceIconDownloadedL( iCurrent->DeviceUuid(), err ) );
            delete iCurrent;
            iCurrent = NULL;
            }
        }
    __LOG1( "CUpnpDeviceIconDownloader::DownloadNext - Waiting: %d", iQueue.Count() );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::DownloadNextL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::DownloadNextL( CEntry& aNext )
    {
    CheckDiskSpaceL();
    HBufC* fileName = TempFileNameLC( iFs );
    iFs.Delete( *fileName ); // Delete previous, ignore error
    iFs.MkDirAll( *fileName ); // Ensure folder exists, ignore error
    iDownloader->DownloadFileL( aNext.DeviceIconUrl(), *fileName, &aNext );
    CleanupStack::PopAndDestroy( fileName );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CleanupFilesL
// --------------------------------------------------------------------------

void CUpnpDeviceIconDownloader::CleanupFilesL()
    {
    HBufC* folderName = FolderNameLC( iFs );    
    __LOG1( "CUpnpDeviceIconDownloader::CleanupFilesL - %S", folderName );
    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    fileMan->RmDir( *folderName ); // Ignore error
    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy( folderName );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::FileExistsL
// --------------------------------------------------------------------------
TBool CUpnpDeviceIconDownloader::FileExistsL( const TDesC8& aDeviceUuid )
    {
    HBufC* filename = FileNameLC( iFs, aDeviceUuid );
    TEntry entry;
    TBool ret( iFs.Entry( *filename, entry ) == KErrNone );
    __LOG2( "CUpnpDeviceIconDownloader::FileExistsL - %S %d", filename, ret );
    CleanupStack::PopAndDestroy( filename );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CleanupFilesIfNeededL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::CleanupFilesIfNeededL()
    {
    TInt cleanupNeeded( ETrue );
    TUid category( PropertyCategory() );
    GetIntPropertyL( category, KCleanupNeededKey, cleanupNeeded );
    if ( cleanupNeeded )
        {
        CleanupFilesL();
        User::LeaveIfError( RProperty::Set( category, KCleanupNeededKey, EFalse ) );
        }
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::CheckDiskSpaceL
// --------------------------------------------------------------------------
void CUpnpDeviceIconDownloader::CheckDiskSpaceL()
    {
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, KDefaultDiskSpaceNeeded,
            RFs::GetSystemDrive() ) )
        {
        iCleanupOnShutdown = ETrue;
        User::Leave( KErrDiskFull );
        }
    }

// --------------------------------------------------------------------------
// CUpnpDeviceIconDownloader::InProgress
// --------------------------------------------------------------------------
TBool CUpnpDeviceIconDownloader::InProgress( const TDesC8& aDeviceUuid )
    {
    return ( ( iCurrent && !iCurrent->DeviceUuid().Compare( aDeviceUuid ) ) ||
             iQueue.Find( aDeviceUuid, CEntry::Compare ) != KErrNotFound );
    }

// end of file
