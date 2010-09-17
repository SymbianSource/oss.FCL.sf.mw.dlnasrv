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
* Description:  Media Server
*
*/


// INCLUDE FILES
#include "upnpmediaserver.h"
#include "upnpcontentdirectoryservice.h"
#include "upnpmediaserversession.h"
#include "upnpmediaserverglobals.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpmediaserverclient.h"
#include "upnpsecuritymanager.h"
#include "upnpfiletransferevent.h"
#include <upnpdevicedescriptionstore.h>

#include "upnpdeviceimplementation.h"
#include "upnpcm.h"
#include "upnpiconmanager.h"
#include "upnpdiskremovedetector.h"
#include <sysutil.h>
#define KLogFile _L("upnpmediaserver.txt")
#include "upnpcustomlog.h"
#include <upnpconnectionmanagernetworkeventprovider.h>
#include "upnpmediaserverdescriptionprovider.h"

const TVersion KServerSupportedVersion(
    KMediaServerMajorVersionNumber,
    KMediaServerMinorVersionNumber,
    KMediaServerBuildVersionNumber );

using namespace UpnpMediaServerSettings;

_LIT8( KUriMediaServer, "/mediaserver1/mediaserver1.xml" );
_LIT8( KUriContentDirectory, "/mediaserver1/contentdirectory1.xml" );
_LIT8( KUriConnectionManager, "/mediaserver1/connectionmanager1.xml" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMediaServer::CUpnpMediaServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMediaServer::CUpnpMediaServer():
    iMsStatus( RUpnpMediaServerClient::EStopped )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ConstructL()
    {
    BaseConstructL();
    iSettings = CUpnpMediaServerSettings::NewL();
    iDiskDetector = CUpnpDiskRemoveDetector::NewL( this );
    iStopAVMediaServerTimer = CUpnpNotifyTimer::NewL( this );
    iConnectionManagerNetworkEventProvider = 
        CUpnpConnectionManagerNetworkEventProvider::NewL( *this );
    iDescriptionProvider = CUpnpMediaServerDescriptionProvider::NewL();
    
    InitializeL( EFalse );
    
    iDescriptionStore = CUpnpDeviceDescriptionStore::NewL( *iMsName );
    iIconManager = CUpnpIconManager::NewL( *iDescriptionStore, *iDescriptionProvider );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSymbianServerBase* CUpnpMediaServer::NewLC()
    {
    CUpnpMediaServer* self = new( ELeave ) CUpnpMediaServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CUpnpMediaServer::~CUpnpMediaServer()
    {
    if ( iDescriptionStore )
        {
        TRAP_IGNORE( iDescriptionStore->SaveL() );
        }

    ASSERT( CanBeStopped() );//check if we are in correct state
    delete iStopAVMediaServerTimer;
    delete iMsName;
    delete iCdName;
    delete iCmName;

    delete iConnectionManagerNetworkEventProvider;
    
    iObserverList.ResetAndDestroy();

    delete iSettings;
    delete iManager;
    delete iIconManager;

    delete iDevice;
    delete iDescriptionStore;
    delete iContentDirecory;
    delete iCm;
    delete iDiskDetector;
    delete iDescriptionProvider;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::StopAVMediaServer
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::StopAVMediaServer( TBool aSilent )
    {
    LOGS( "CUpnpMediaServer::StopAVMediaServer begin" );
    iIsSilentShutdown = aSilent;
    //ms will be stopped in KStopAVMediaServerDelay unless he gets start request
    iStopAVMediaServerTimer->Start( KStopAVMediaServerDelay );
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServer::StopAVMediaServerImmediately
// Immediately stops media server ( no delay )
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::StopAVMediaServerImmediately( TBool aSilent, TBool aWlanLost )
    {
    iStopAVMediaServerTimer->Cancel();
    iDiskDetector->Cancel();

    delete iManager;
    iManager = NULL;

    // we do not want any stop devices in case of WLAN lost
    if ( !aWlanLost )
        {
        if ( iDevice )
            {
            if ( !aSilent )
                {
                TRAP_IGNORE( iDevice->StopL() );
                }
            else
                {
                TRAP_IGNORE( iDevice->StopL( CUpnpDeviceImplementation::EStopSilent ) );
                }
            }
        }

    delete iContentDirecory;
    iContentDirecory = NULL;

    delete iCm;
    iCm = NULL;

    delete iDevice;
    iDevice = NULL;

    SetStatus( RUpnpMediaServerClient::EStopped );

    SuggestShutdown();
    LOGS( "CUpnpMediaServer::StopAVMediaServer end" );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CUpnpMediaServer::NewSessionL( const RMessage2& /*aMessage*/ ) const
    {
    return CUpnpMediaServerSession::NewL( (CUpnpMediaServer*) this );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::Status
// Check status of the server.
// -----------------------------------------------------------------------------
//
RUpnpMediaServerClient::TServerStatus CUpnpMediaServer::Status() const
    {
    if ( iStopAVMediaServerTimer && iStopAVMediaServerTimer->IsActive() )
        {
        return RUpnpMediaServerClient::EStopped;
        }
    return iMsStatus;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::StartAVMediaServerL
// Create and start the AV Media Server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::StartAVMediaServerL()
    {
    LOGS( "CUpnpMediaServer::StartAVMediaServerL begin" );

    // cleanup because previous start could fail
    StopAVMediaServerImmediately( EFalse, EFalse );
    
    // save description
    iDescriptionStore->SaveL();

    // create device
    iDevice = CUpnpDeviceImplementation::NewL( KUriMediaServer, 
                                               *iDescriptionStore, 
                                               *iDescriptionProvider );

    // create security manager
    iManager = CUpnpSecurityManager::NewInstanceFromUpnpSettingsL();
    //iDevice->SetSecurityManager( iManager );

    // create content direcory service
    HBufC8* uploadtmp = iSettings->GetL( EUploadDirectory );
    CleanupStack::PushL( uploadtmp );
    if ( uploadtmp->Length() == 0 )
        {
        User::Leave( KErrNotReady );
        }
    HBufC* uploaddir = HBufC::NewL( uploadtmp->Length() );
    CleanupStack::PushL( uploaddir );
    uploaddir->Des().Copy( uploadtmp->Des() );
    HBufC* thumbnaildir = HBufC::NewL( uploaddir->Length()
            + KThumbnailDirLen );
    CleanupStack::PushL( thumbnaildir );
    thumbnaildir->Des().Copy( uploaddir->Des() );
    thumbnaildir->Des().Append( KThumbnailSubDir );

    TInt createThumbnails;
    User::LeaveIfError( iSettings->Get( EUpnpMediaServerThumbnails,
        createThumbnails ) );

    TInt maxReqCount;
    User::LeaveIfError( iSettings->Get( KUpnpMediaServerBrowseLimit,
        maxReqCount ) );

    TUpnpCdSettings cdset =
        {
        iDevice, *uploaddir, *thumbnaildir, *iCdName, NULL, createThumbnails,
                maxReqCount, this, KNullDesC(), NULL
        };

    iContentDirecory = CUpnpContentDirectoryService::NewL( cdset );

    // create connection manager service
    HBufC8* media = iSettings->GetL( ESupportedMedia );
    CleanupStack::PushL( media );
    if ( media->Length() == 0 )
        {
        User::Leave( KErrNotReady );
        }
    iCm = CUpnpCM::NewL( *iDevice, *media, *iCmName );
    CleanupStack::PopAndDestroy( media );

    CleanupStack::PopAndDestroy( thumbnaildir );
    CleanupStack::PopAndDestroy( uploaddir );
    CleanupStack::PopAndDestroy( uploadtmp );

    SetDescriptionSettingsL( iDevice );

    // start media server
    iDevice->StartL();
    iDiskDetector->StartL();

    SetStatus( RUpnpMediaServerClient::EStartedOnline );
    LOGS( "CUpnpMediaServer::StartAVMediaServerL end" );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::StartAVMediaServerOfflineL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::StartAVMediaServerOfflineL()
    {
    LOGS( "StartAVMediaServerOfflineL begin" );

    // cleanup because previous start could fail
    StopAVMediaServerImmediately( EFalse, EFalse );

    // create content directory
    iContentDirecory = CUpnpContentDirectoryService::NewL();
    SetStatus( RUpnpMediaServerClient::EStartedOffline );
    LOGS( "StartAVMediaServerOfflineL end" );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RestartAVMediaServerL
// Start and stop the AV Media Server.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::RestartAVMediaServerL()
    {
    if ( iDevice )
        {
        iDevice->StopL();
        SetDescriptionSettingsL( iDevice );
        iDevice->StartL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetFriendlyNameL
// Sets friendly name for device, changes are available after restarting
// the server
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetDescriptionSettingsL(
    CUpnpDeviceImplementation* aDevice )
    {
    SetDescriptionRequiredPropertyL( aDevice, EFriendlyName, KFriendlyName );
    SetDescriptionRequiredPropertyL( aDevice, EModelName, KModelName );
    SetDescriptionRequiredPropertyL( aDevice, EManufacturer, KManufacturer );

    SetDescriptionPropertyL( aDevice, EModelUrl, KModelUrl );
    SetDescriptionPropertyL( aDevice, EModelDescription, KModelDescription );
    SetDescriptionPropertyL( aDevice, EModelNumber, KModelNumber );
    SetDescriptionPropertyL( aDevice, ESerialNumber, KSerialNumber );
    SetDescriptionPropertyL( aDevice, EPresentationUrl, KPresentationUrl );
    SetDescriptionPropertyL( aDevice, EManufacturerUrl, KManufacturerUrl );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RemoveIconL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::RemoveIconL( const TDesC8& aUrl )
    {
    iIconManager->RemoveIconL( aUrl );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RemoveIconsL
// ------------------------------------------------------------------------------
//
void CUpnpMediaServer::RemoveIconsL()
    {
    iIconManager->RemoveIconsL();
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::AddIconL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::AddIconL( const TDesC8& aProperty )
    {
    iIconManager->AddIconL( aProperty );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UpdateIconL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UpdateIconL( const TDesC8& aProperty,
    const TDesC8& aUrl )
    {
    iIconManager->UpdateIconL( aProperty, aUrl );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetUploadDirToCDL
// Set the download and thumbnail in CD according to the value
// of the upload directory
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetUploadDirToCDL( const TDesC& aPath )
    {
    if ( iContentDirecory )
        {
        HBufC* uploaddir= NULL;

        if ( aPath != KNullDesC() )
            {
            uploaddir = aPath.AllocLC();
            }
        else
            {
            HBufC8* uploadtmp = iSettings->GetL( EUploadDirectory );
            CleanupStack::PushL( uploadtmp );

            uploaddir = HBufC::NewL( uploadtmp->Length() );
            uploaddir->Des().Copy( uploadtmp->Des() );
            CleanupStack::PopAndDestroy( uploadtmp );
            CleanupStack::PushL( uploaddir );
            }

        HBufC* thumbnaildir = HBufC::NewL( uploaddir->Length()
                + KThumbnailDirLen );
        CleanupStack::PushL( thumbnaildir );
        thumbnaildir->Des().Copy( uploaddir->Des() );
        thumbnaildir->Des().Append( KThumbnailSubDir );

        iContentDirecory->SetDownloadDirectoryL( uploaddir->Des() );
        iContentDirecory->SetThumbnailDirectoryL( thumbnaildir->Des() );

        CleanupStack::PopAndDestroy( thumbnaildir );
        CleanupStack::PopAndDestroy( uploaddir );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetUploadDirL
// Set the upload and thumbnail path in CD and central repository.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetUploadDirL( const TDesC& aPath )
    {
    User::LeaveIfError( iSettings->Set(
        UpnpMediaServerSettings::EUploadDirectory, aPath ) ) ;

    SetUploadDirToCDL( KNullDesC( )); // empty descriptor -> it means, value will be taken directly from cenrep
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetDescriptionPropertyL
// Sets description property for device
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetDescriptionPropertyL(
    CUpnpDeviceImplementation* aDevice, TInt aKey, const TDesC8& aName )
    {
    HBufC8* value = iSettings->GetL( aKey );
    CleanupStack::PushL( value );
    if ( value->Size() )
        {
        aDevice->SetDescriptionPropertyL( aName, *value );
        }
    else
        {
        aDevice->RemoveDescriptionPropertyL( aName );
        }

    CleanupStack::PopAndDestroy( value );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetDescriptionRequiredPropertyL
// Sets description required property for device
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetDescriptionRequiredPropertyL(
    CUpnpDeviceImplementation* aDevice, TInt aKey, const TDesC8& aName )
    {
    HBufC8* value = iSettings->GetL( aKey );

    if ( !value )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        CleanupStack::PushL( value );

        HBufC8* tmpBuf = (*value).AllocLC();
        TPtr8 tmpPtr(tmpBuf->Des() );

        RemoveWhiteSpaces( tmpPtr );
        if ( tmpPtr.Length() == 0 )
            {
            User::Leave( KErrNotFound );
            }
        CleanupStack::PopAndDestroy( tmpBuf );
        }

    TRAPD( error, aDevice->SetDescriptionPropertyL( aName, *value ) )
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );
        }

    CleanupStack::PopAndDestroy( value );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RemoveWhiteSpaces
// Removes white characters from input descriptor
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::RemoveWhiteSpaces( TDes8& aString )
    {
    for ( TInt i=0; i<aString.Length(); i++ )
        {
        if ( aString[i] == ' ' || aString[i] == '\r' || aString[i] == '\n'
                || aString[i] == '\t' )
            {
            aString.Replace( i, 1, KNullDesC8() );
            i--;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::InitPrivateDir
// Init Media Server directory structure in private directory
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::InitPrivateDirL( RFs &aFs )
    {
    TParse fp;
    TFileName path;
    aFs.PrivatePath( path );
    fp.Set( KDefaultMediaDrive(), &path, 0 );
    User::LeaveIfError( fp.AddDir( KMSPublicAccessedDir() ) );
    User::LeaveIfError( fp.AddDir( KMSDeviceXmlDescDir() ) );
    User::LeaveIfError( fp.AddDir( KIconDirectory() ) );
    path = fp.DriveAndPath();

    aFs.MkDirAll( path ) ;
    fp.PopDir();
    User::LeaveIfError( fp.AddDir( KMSMediaDir ) );
    path = fp.DriveAndPath();
    aFs.MkDirAll( path );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::InitDescFileL
// Checks is description file exists. If not creates it from teplate stored on Z:
// -----------------------------------------------------------------------------
//
HBufC* CUpnpMediaServer::InitDescFileL( RFs &aFs,
    const TDesC& aTempleteDescFile, TBool aOverwrite )
    {
    TUint temp;
    HBufC* descName = CreateFullNameL( aFs, KDefaultMediaDrive(),
        aTempleteDescFile );
    CleanupStack::PushL( descName );

    if ( aFs.Att( *descName, temp ) != KErrNone || aOverwrite )
        {

        HBufC* templName = CreateFullNameL( aFs, KDescTemplateDrive(),
            aTempleteDescFile );
        CleanupStack::PushL( templName );
        CFileMan* fileMan = CFileMan::NewL( aFs );
        CleanupStack::PushL( fileMan );
        User::LeaveIfError( fileMan->Copy( *templName, *descName ) );
        User::LeaveIfError( fileMan->Attribs( *descName, KEntryAttNormal,
            KEntryAttReadOnly, TTime( 0 ) ) );
        CleanupStack::PopAndDestroy( fileMan );
        CleanupStack::PopAndDestroy( templName );
        }
    CleanupStack::Pop( descName );
    return descName;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::InitializeL
// Initialize Media Server using files from drive z:
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::InitializeL( TBool aOverwrite )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, KMSDescriptionMaxSize,
        EDriveC ) )
        {
        User::Leave( KErrDiskFull );
        }

    InitPrivateDirL( fs );

    delete iMsName;
    iMsName= NULL;
    iMsName = InitDescFileL( fs, KMSDeviceXmlDescFile(), aOverwrite );
    iDescriptionProvider->AddUriToFileMapL( KUriMediaServer(), iMsName->Des() );
    delete iCdName;
    iCdName = NULL;
    iCdName = InitDescFileL( fs, KCDXmlDescFile(), aOverwrite );
    iDescriptionProvider->AddUriToFileMapL( KUriContentDirectory(), iCdName->Des() );
    delete iCmName;
    iCmName = NULL;
    iCmName = InitDescFileL( fs, KCMXmlDescFile(), aOverwrite );
    iDescriptionProvider->AddUriToFileMapL( KUriConnectionManager(), iCmName->Des() );

    CleanupStack::PopAndDestroy( &fs );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ResetDbL
// Reset database, renaming name of original DB
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ResetDbL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TFileName orgPath;

    User::LeaveIfError( fs.PrivatePath( orgPath ) );

    TParse fp;
    fp.Set( KDatabaseFileName(), &orgPath, 0 );
    orgPath = fp.FullName();

    TFileName newPath;
    newPath = orgPath;
    newPath.Append( KBackupSuffix );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    User::LeaveIfError( fileMan->Rename( orgPath, newPath ) );
    CleanupStack::PopAndDestroy( fileMan );

    CleanupStack::PopAndDestroy( &fs );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::DeleteDbL
// Delete database
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::DeleteDbL()
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TFileName orgPath;

    User::LeaveIfError( fs.PrivatePath( orgPath ) );

    TParse fp;
    fp.Set( KDatabaseFileName(), &orgPath, 0 );
    orgPath = fp.FullName();

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    User::LeaveIfError( fileMan->Delete( orgPath ) );
    CleanupStack::PopAndDestroy( fileMan );

    CleanupStack::PopAndDestroy( &fs );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SettingChangedL
// Function notify when setting will change
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SettingChangedL()
    {
    //no actions needed
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::CreateFullNameL
// Create full name using drive MS directory structure and file name.
// -----------------------------------------------------------------------------
//
HBufC* CUpnpMediaServer::CreateFullNameL( RFs &aFs, const TDesC& aDrive,
    const TDesC& aName )
    {
    TParse fp;
    TFileName path;
    User::LeaveIfError( aFs.PrivatePath( path ) );
    fp.Set( aDrive, &path, &aName );
    User::LeaveIfError( fp.AddDir( KMSPublicAccessedDir() ) );
    User::LeaveIfError( fp.AddDir( KMSDeviceXmlDescDir() ) );
    return fp.FullName().AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ShareItemL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ShareObjectL( CUpnpObject* aItem )
    {
    iContentDirecory->ShareObjectL( aItem );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ShareReferenceL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ShareReferenceL( CUpnpItem* aItem )
    {
    iContentDirecory->ShareReferenceL( aItem );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ShareReferenceListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ShareReferenceListL( CUpnpItemList* aItemList,
    TInt* aStatus )
    {
    iContentDirecory->ShareReferenceListL( aItemList, aStatus );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ShareItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::ShareItemListL( CUpnpItemList* aItemList, TInt* aStatus )
    {
    iContentDirecory->ShareItemListL( aItemList, aStatus );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UnshareRootL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UnshareRootL()
    {
    TBool online = ETrue;

    if ( Status()==RUpnpMediaServerClient::EStartedOnline )
        {
        online = ETrue;
        }
    else if ( Status()==RUpnpMediaServerClient::EStartedOffline )
        {
        online = EFalse;
        }

    StopAVMediaServerImmediately( EFalse, EFalse );

    DeleteDbL();

    if ( online )
        {
        StartAVMediaServerL();
        }
    else
        {
        StartAVMediaServerOfflineL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UnshareItemL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UnshareItemL( TInt aId )
    {
    iContentDirecory->UnshareItemL( aId );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UnshareContainerL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UnshareContainerL( TInt aId )
    {
    iContentDirecory->UnshareContainerL( aId );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetSharedItemL
// -----------------------------------------------------------------------------
//
CUpnpItem* CUpnpMediaServer::GetSharedItemL( TInt aItem )
    {
    return iContentDirecory->GetSharedItemL( aItem );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetSingleContainerL
// -----------------------------------------------------------------------------
//
CUpnpContainer* CUpnpMediaServer::GetSingleContainerL( TInt aContainerId )
    {
    return iContentDirecory->GetSingleContainerL( aContainerId );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UnshareItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UnshareItemListL( RArray<TInt>& aItemList,
    TInt* aExecutionStatus )
    {
    iContentDirecory->UnshareItemListL( aItemList, aExecutionStatus );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetSharedItemListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::GetSharedItemListL( TInt aContId,
    CUpnpBrowseCriteria* aBrowseCriteria, TInt* TotalMatches,
    CUpnpItemList* aItemList )
    {
    iContentDirecory->GetItemListL( aContId, aBrowseCriteria, TotalMatches,
        aItemList );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetSharedContainerListL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::GetSharedContainerListL( TInt aContId,
    CUpnpBrowseCriteria* aBrowseCriteria, TInt* TotalMatches,
    CUpnpContainerList* aContainerList )
    {
    iContentDirecory->GetContainerListL( aContId, aBrowseCriteria,
        TotalMatches, aContainerList );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetIpAndPortL
// -----------------------------------------------------------------------------
//
TInetAddr CUpnpMediaServer::GetAddress()
    {
    return iContentDirecory->GetAddress();
    }

// ============================= EVENT FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SubscribeEvents
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SubscribeEvents(
    MUpnpContentDirectoryEventObserver* aObserver )
    {
    iObserverList.AppendL( aObserver );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::UnsubscribeEvents
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::UnsubscribeEvents(
    MUpnpContentDirectoryEventObserver* aObserver )
    {
    TInt index = iObserverList.Find( aObserver );
    if ( index != KErrNotFound )
        {
        iObserverList.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::FileTransferEvent
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::FileTransferEvent( CUpnpFileTransferEvent* aMessage )
    {
    for ( TInt i=0; i<iObserverList.Count(); i++ )
        {
        iObserverList[i]->FileTransferEvent( aMessage );
        }
    delete aMessage;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::IsInterfaceUp
// -----------------------------------------------------------------------------
//
TBool CUpnpMediaServer::IsInterfaceUp()
    {
    return ( iMsStatus == RUpnpMediaServerClient::EStartedOnline );
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::CancelAVMediaServerStop
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::CancelAVMediaServerStop()
    {
    iStopAVMediaServerTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::CanBeStopped
// -----------------------------------------------------------------------------
//
TBool CUpnpMediaServer::CanBeStopped() const
    {
    return RUpnpMediaServerClient::EStopped == Status(); //note: Status(),not iMsStatus
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::TimerEventL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    if ( aTimer == iStopAVMediaServerTimer )
        {
        StopAVMediaServerImmediately( iIsSilentShutdown, EFalse );
        }
    else
        {
        CUpnpSymbianServerBase::TimerEventL( aTimer );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpMediaServer::SupportedVersion
// -----------------------------------------------------------------------------
//
TVersion CUpnpMediaServer::SupportedVersion() const
    {
    return KServerSupportedVersion;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::ServerName
// -----------------------------------------------------------------------------
//
const TDesC& CUpnpMediaServer::ServerName() const
    {
    return KMediaServerName;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::NetworkEvent
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::NetworkEvent( CUpnpNetworkEventBase* aEvent )
    {
    LOG_FUNC_NAME;

    if ( aEvent->SubscriberError() < KErrNone && aEvent->Type() == EUnknownEvent )
        {
        LOGS( "CUpnpMediaServer::NetworkEvent error" );
        return;
        }
        
    switch ( aEvent->Type() )
        {
        case EWlanLostEvent:
            {
            if ( IsInterfaceUp() )
                {
                StopAVMediaServerImmediately( ETrue, ETrue );
                NotifyStatusChanged();
                }
            }
            break;

        case EAddressChangeEvent:
            {
            if ( iMsStatus == RUpnpMediaServerClient::EStartedOnline )
                {
                TRAPD( error, RestartAVMediaServerL() );
                if ( error != KErrNone )
                    {
                    if ( iStopAVMediaServerTimer->IsActive() )
                        {
                        StopAVMediaServerImmediately( EFalse, EFalse );
                        }
                    TRAP( error, StartAVMediaServerL() );
                    if ( error != KErrNone )
                         {
                         StopAVMediaServerImmediately( EFalse, EFalse );
                         NotifyStatusChanged();
                         }
                     }
                else if ( iContentDirecory )
                    {
                    TRAP( error, iContentDirecory->AddressChangeL() );
                    if ( error != KErrNone )
                         {
                         StopAVMediaServerImmediately( EFalse, EFalse );
                         NotifyStatusChanged();
                         } 
                    }                     
                }
            }
            break; 
        
        case EConnectionClosedEvent:
            {
            ASSERT( iMsStatus != RUpnpMediaServerClient::EStartedOnline );
            }
            break;
        }
    }                

// -----------------------------------------------------------------------------
// CUpnpMediaServer::SetStatus
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::SetStatus( RUpnpMediaServerClient::TServerStatus aStatus )
    {
    iMsStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::NotifyStatusChanged
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::NotifyStatusChanged()
    {
    CUpnpMediaServerSession* ses = NULL;
    iSessionIter.SetToFirst();
    CSession2* cses = iSessionIter++;
    ses = reinterpret_cast<CUpnpMediaServerSession*>( cses );
    while ( ses )
        {
        ses->StatusChanged();
        ses = reinterpret_cast<CUpnpMediaServerSession*>( iSessionIter++ );    
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RunL
// -----------------------------------------------------------------------------
//
void CUpnpMediaServer::RunL()
    {
    const RUpnpMediaServerClient::TServerStatus beforeStatus = Status();
    CUpnpSymbianServerBase::RunL();
    if ( Status() != beforeStatus )
        {
        NotifyStatusChanged();
        }
    }

// -----------------------------------------------------------------------------
// E32Main entry point.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CUpnpMediaServer::StartServer(
        KMediaServerThreadName, CUpnpMediaServer::NewLC );
    }


// End of File
