/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Resolver for downloading remote items
*
*/






// INCLUDE FILES
// System
#include <pathinfo.h>
#include <bautils.h>

// upnp stack api's
#include <upnpobject.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpattribute.h> // for getting resource protocol info
#include <upnpdlnaprotocolinfo.h> // for resolving object mimetype

// upnpframework / avcontroller api
#include "upnpavcontroller.h" // avcontrol services
#include "upnpavdevice.h" // avcontroller device class
#include "upnpavbrowsingsession.h" // avcontrol browsing session
#include "upnpfiledownloadsession.h" // avcontrol download session

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for element names
#include "upnpitemutility.h" // for FindAttributeByName
#include "upnpresourceselector.h" // MUPnPResourceSelector
#include "upnpitemresolverobserver.h" // observer for this class
#include "upnpdlnautility.h"  // IsSupportedDlnaProfile

// upnpframework / internal api's
#include "upnpcommonutils.h" // for FileExtensionByMimeTypeL
#include "upnpsettingsengine.h" // get selected download location

// avcontroller helper internal
#include "upnpremoteitemresolver.h" // remote item resolver impl.
#include "upnpdownloaditemresolver.h" // download item resolver impl.

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KTempPrefix, "upnpfwtemp");

// METHODS

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::NewL
//---------------------------------------------------------------------------
CUPnPDownloadItemResolver* CUPnPDownloadItemResolver::NewL(
    const TDesC8& aItemId,
    MUPnPAVController& aAvController,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    {
    CUPnPDownloadItemResolver* self = new (ELeave) CUPnPDownloadItemResolver(
        aItemId, aAvController, aHostSession, aSelector, aBrowseFilter );
    CleanupStack::PushL( self );
    self->ConstructL(
        aItemId, aAvController, aHostSession, aSelector, aBrowseFilter );
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::CUPnPDownloadItemResolver
//---------------------------------------------------------------------------
CUPnPDownloadItemResolver::CUPnPDownloadItemResolver(
    const TDesC8& /*aItemId*/,
    MUPnPAVController& aAvController,
    MUPnPAVBrowsingSession& /*aHostSession*/,
    MUPnPResourceSelector& aSelector,
    const TDesC8& /*aBrowseFilter*/ )
    : iAvController( aAvController ),
      iState( EStateIdle ),iSelector(aSelector)
    {

    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::ConstructL
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::ConstructL(
    const TDesC8& aItemId,
    MUPnPAVController& /*aAvController*/,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    {
    iRemoteResolver = CUPnPRemoteItemResolver::NewL(
        aItemId, aHostSession, aSelector, aBrowseFilter );
    iRemoteDevice = CUpnpAVDevice::NewL( aHostSession.Device() );
    iSettingsEngine = CUPnPSettingsEngine::NewL();
    
    // Open File Server session
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iFsSession.ShareProtected() );   
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::~CUPnPDownloadItemResolver
//---------------------------------------------------------------------------
CUPnPDownloadItemResolver::~CUPnPDownloadItemResolver()
    {        
    Cleanup();
    delete iRemoteResolver;
    iRemoteResolver = 0;
    delete iRemoteDevice;
    iRemoteDevice= 0;
    delete iSettingsEngine;
    iSettingsEngine = 0;
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::ResolveL
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::ResolveL(
    MUPnPItemResolverObserver& aObserver )
    {
    __LOG( "DownloadItemResolver:Resolve()" );
    __ASSERTD( iState == EStateIdle, __FILE__, __LINE__ );

    // change state
    iObserver = &aObserver;

    // first resolve the item
    iRemoteResolver->ResolveL( *this );
    iState = EStateResolving;
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::ResolveComplete
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::ResolveComplete(
    const MUPnPItemResolver& /*aResolver*/, TInt aError )
    {
    __LOG1( "DownloadItemResolver:ResolveComplete(%d)", aError );
    __ASSERTD( iState == EStateResolving, __FILE__, __LINE__ );

    if ( aError == KErrNone )
        {
        TRAP( aError, 
                const CUpnpElement& res =
                        iSelector.SelectResourceL( iRemoteResolver->Item() );
                IsLocallySupportedL( res );
                );
        if( aError )
            {
            Complete( aError );
            }
        else
            {
            TRAPD( err, InitiateDownloadL() );
            if ( err != KErrNone )
                {
                Complete( err );
                }
            }
        }
    else
        {
        Complete( aError );
        }

    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::InitiateDownloadL
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::InitiateDownloadL()
    {
    __LOG( "CUPnPDownloadItemResolver::InitiateDownloadL");
    // start a download session if not already started
    if( !iDownloadSession )
        {
        iDownloadSession =
            &iAvController.StartDownloadSessionL( *iRemoteDevice );
        }
    iDownloadSession->SetObserver( *this );

    // Get selected download location.
    HBufC* copyLocation = GetSelectedDownloadLocationL();
    CleanupStack::PushL( copyLocation );
    _LIT(KTempFolder, "temp\\");
    // Create temp file name
    HBufC* tempFileName = CreateTmpFileNameL();
    CleanupStack::PushL( tempFileName );

    // Create and save full file path.
    if( iLocalFile )
        {
        // delete if already allocated.
        delete iLocalFile;
        iLocalFile = 0;
        }
    
    iLocalFile = HBufC::NewL( copyLocation->Length() +
                              KTempFolder().Length() );
    iLocalFile->Des().Append( *copyLocation );
    iLocalFile->Des().Append( KTempFolder );
    //check the existence of the target folder
    if( !BaflUtils::FolderExists( iFsSession, *iLocalFile ) )
        {
        User::LeaveIfError( iFsSession.MkDirAll( *iLocalFile ) );
        }
        
    User::LeaveIfError( iFsSession.SetAtt( *iLocalFile, 
                                    KEntryAttHidden, 
                                    KEntryAttNormal ) );
                                        
    iLocalFile = iLocalFile->ReAllocL( 
        iLocalFile->Length() + tempFileName->Length() );
    
    iLocalFile->Des().Append( *tempFileName );

    // write the file path into the item
    CUpnpElement& writableRes =
        const_cast<CUpnpElement&>( iRemoteResolver->Resource() );
    writableRes.SetFilePathL( *iLocalFile );
    CleanupStack::PopAndDestroy( tempFileName );
    CleanupStack::PopAndDestroy( copyLocation );
    
    // Create local target file for dowload item
    TInt err = CreateRFile( *iLocalFile );

    if( err == KErrNone )
        {
        __LOG( "StartDownloading...");
        // initiate download
        
        iDownloadSession->StartDownloadL( writableRes, 
                                      iRemoteResolver->Item(), 
                                      iFile,
                                      (TInt)this );
                                      
        iState = EStateDownloading;
        }
    else // Create download target failed!
        {
        __LOG( "Create download target failed!");
        User::Leave( err );
        }
    __LOG( "CUPnPDownloadItemResolver::InitiateDownloadL -End");
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::TransferStarted
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::TransferStarted( TInt /*aKey*/,
    TInt /*aStatus*/ )
    {
    __LOG( "DownloadItemResolver:TransferStarted");
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::TransferCompleted
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::TransferCompleted( TInt /*aKey*/,
    TInt aStatus, const TDesC& /*aFilePath*/ )
    {
    __LOG1( "DownloadItemResolver:TransferCompleted(%d)", aStatus );
    iFile.Close();
    Complete( aStatus );
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::TransferProgress
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::TransferProgress( TInt /*aKey*/,
    TInt /*aBytes*/, TInt /*aTotalBytes*/ )
    {
    __LOG( "DownloadItemResolver:TransferProgress");
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::MediaServerDisappeared
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::MediaServerDisappeared(
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    __LOG( "DownloadItemResolver:MediaServerDisappeared" );
    Complete( KErrDisconnected );
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::Item
//---------------------------------------------------------------------------
const CUpnpItem& CUPnPDownloadItemResolver::Item() const
    {
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );

    return iRemoteResolver->Item();
    }


// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::Resource
//---------------------------------------------------------------------------
const CUpnpElement& CUPnPDownloadItemResolver::Resource() const
    {
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );

    return iRemoteResolver->Resource();
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::DeleteTempFilesL
// --------------------------------------------------------------------------
EXPORT_C void CUPnPDownloadItemResolver::DeleteTempDownloadFilesL()
    {
    __LOG("CUPnPDownloadItemResolver::DeleteTempDownloadFilesL begin");

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    _LIT( KAnyChar, "*");
    _LIT( KAnyExtension, "*.*");
    _LIT( KUpnpUploadTempDirectory, "temp\\" );

    // clean selected download directory
    HBufC* path = HBufC::NewLC( KMaxFileName );
    CUPnPSettingsEngine* settingsEngine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( settingsEngine );
    HBufC* copyLocation = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( copyLocation );
    TBool copyLocationIsPhoneMemory = 0; // not used in this case
    TPtr copyLocationPtr( copyLocation->Des() );
    settingsEngine->GetCopyLocationL( copyLocationPtr,
        copyLocationIsPhoneMemory );
    path->Des().Append( *copyLocation );
    path->Des().Append( KAnyChar );
    path->Des().Append( KTempPrefix );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );
    
    path->Des().Zero();
    path->Des().Append( *copyLocation );
    path->Des().Append( KUpnpUploadTempDirectory );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );

    CleanupStack::PopAndDestroy( copyLocation );
    CleanupStack::PopAndDestroy( settingsEngine );
    CleanupStack::PopAndDestroy( path );

    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy( &fs );

    __LOG("CUPnPDownloadItemResolver::DeleteTempDownloadFilesL End");
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::Complete
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::Complete( TInt aError )
    {
    __ASSERTD( iState == EStateResolving || iState == EStateDownloading,
        __FILE__, __LINE__ );

    if ( iDownloadSession != 0 )
        {
        iAvController.StopDownloadSession( *iDownloadSession );
        iDownloadSession = 0;
        }

    MUPnPItemResolverObserver& observer = *iObserver;
    iObserver = 0;
    if ( aError == KErrNone )
        {
        iState = EStateReady;
        }
    else
        {
        iState = EStateIdle;
        Cleanup();
        }

    observer.ResolveComplete( *this, aError );
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::Cleanup
//---------------------------------------------------------------------------
void CUPnPDownloadItemResolver::Cleanup()
    {
    iObserver = 0;
    iFile.Close();
    
    if ( iDownloadSession != 0 )
        {
        iAvController.StopDownloadSession( *iDownloadSession );
        iDownloadSession = 0;
        }
        
    if ( iLocalFile )
        {
        // delete the local file
        iFsSession.Delete( iLocalFile->Des() );       
        __LOG1( "DownloadItemResolver: deleted local file(%d)", iLocalFile );
        delete iLocalFile;
        iLocalFile = 0;
        }
        
    iFsSession.Close();

    iState = EStateIdle;
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::CreateTmpFileNameL
//---------------------------------------------------------------------------
HBufC* CUPnPDownloadItemResolver::CreateTmpFileNameL()
    {
    __LOG( "DownloadItemResolver::CreateTmpFileName" );
    __ASSERTD( iState == EStateResolving, __FILE__, __LINE__ );

    HBufC* tempfilename = NULL;
    HBufC* fileExt = NULL;
    
    // Get file extension
    const CUpnpAttribute* attr = UPnPItemUtility
        ::FindAttributeByName( iRemoteResolver->Resource(), 
        KAttributeProtocolInfo );
    if ( attr != 0 )
        {
        CUpnpDlnaProtocolInfo* pInfo =
            CUpnpDlnaProtocolInfo::NewL( attr->Value() );
        CleanupStack::PushL( pInfo );
        fileExt = UPnPCommonUtils::FileExtensionByMimeTypeL(
            pInfo->ThirdField() );
        CleanupStack::PopAndDestroy( pInfo );
        pInfo = NULL;
        }

    // If file extension exist create whole file name.
    if( fileExt )
        {
        CleanupStack::PushL( fileExt );
        tempfilename = HBufC::NewL( 
            KTempPrefix().Length() + 
            iRemoteResolver->Item().Id().Length() + 
            fileExt->Length() );
        CleanupStack::PushL( tempfilename );
        // Add prefix
        tempfilename->Des().Append( KTempPrefix ); 
        // Add item name. Convert 8 to 16 and replace illeagal characters.
        HBufC8* tmpItemName8 = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            iRemoteResolver->Item().Id() );
        CleanupStack::PushL( tmpItemName8 );
        HBufC* itemname = HBufC::NewL( tmpItemName8->Length() );
        CleanupStack::PushL( itemname );
        itemname->Des().Copy( *tmpItemName8 );
        tempfilename->Des().Append( *itemname );
        CleanupStack::PopAndDestroy( itemname );
        CleanupStack::PopAndDestroy( tmpItemName8 );
        // Add file extension
        tempfilename->Des().Append( *fileExt );

        CleanupStack::Pop( tempfilename );
        CleanupStack::PopAndDestroy( fileExt );
        }
    else // Create without file extension
        {
        tempfilename = HBufC::NewL( 
            KTempPrefix().Length() + iRemoteResolver->Item().Id().Length() );
        CleanupStack::PushL( tempfilename );
        // Add prefix
        tempfilename->Des().Append( KTempPrefix ); 
        // Add item name. Convert 8 to 16 and replace illeagal characters.
        HBufC8* tmpItemName8 = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            iRemoteResolver->Item().Id() );
        CleanupStack::PushL( tmpItemName8 );
        HBufC* itemname = HBufC::NewL( tmpItemName8->Length() );
        CleanupStack::PushL( itemname );
        itemname->Des().Copy( *tmpItemName8 );
        tempfilename->Des().Append( *itemname );
        CleanupStack::PopAndDestroy( itemname );
        CleanupStack::PopAndDestroy( tmpItemName8 );

        CleanupStack::Pop( tempfilename );
        }

    return tempfilename; //transfer ownership
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::GetSelectedDownloadLocationL
//---------------------------------------------------------------------------
HBufC* CUPnPDownloadItemResolver::GetSelectedDownloadLocationL()
    {
    __LOG( "DownloadItemResolver::GetSelectedDownloadLocationL" );

    HBufC* copyLocation = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( copyLocation );
    TBool copyLocationIsPhoneMemory = 0;
    TPtr copyLocationPtr( copyLocation->Des() );
    TRAPD( error, iSettingsEngine->GetCopyLocationL( 
        copyLocationPtr, copyLocationIsPhoneMemory ) )
    
    // Something wrong in getting copy location. Default to 
    // phone memory.
    if( error != KErrNone )
        {
        TPtrC phoneDrive( PathInfo::PhoneMemoryRootPath() );
        copyLocation->Des().Append( phoneDrive );
        }
    
    CleanupStack::Pop( copyLocation );
    return copyLocation;
    }

// -----------------------------------------------------------------------------
// CUPnPDownloadItemResolver::CreateRFile
// -----------------------------------------------------------------------------
// 
TInt CUPnPDownloadItemResolver::CreateRFile( const TDesC& aFilePath )
    {
    __LOG( "DownloadItemResolver::CreateRFile" );
    iFile.Close();

    // In some special case, the function Replace() will fail
    // with error -14(KErrInUse) by using 'EFileWrite' mode.
    // Maybe some other handler does not close the file. 
    TInt err = iFile.Replace(
                iFsSession,
                aFilePath,
                EFileShareReadersOrWriters );
                
    if( KErrPathNotFound == err )
        {
        __LOG( "Directory not available -> create new" );
        iFsSession.MkDirAll( aFilePath );
        err = iFile.Create( iFsSession, aFilePath, EFileWrite );
        }
    return err;
    }

// --------------------------------------------------------------------------
// CUPnPDownloadItemResolver::IsLocallySupportedL()
// Check if the audio is DLNA supported in remote to local playback 
// --------------------------------------------------------------------------
//
void CUPnPDownloadItemResolver::IsLocallySupportedL( const CUpnpElement& aRes )
    {
    const CUpnpAttribute* attr = 
                       &UPnPItemUtility::FindAttributeByNameL( 
                                              aRes, KAttributeProtocolInfo );
    // parse protocol info
    CUpnpDlnaProtocolInfo* pInfo = NULL;
    pInfo = CUpnpDlnaProtocolInfo::NewL( attr->Value() );
    CleanupStack::PushL( pInfo );
    
    //if DLNA compliant item
    if ( pInfo->PnParameter() != KNullDesC8() )
        {
        if( !UPnPDlnaUtility::IsSupportedDlnaProfile( 
                                              pInfo->PnParameter() ) )
            {
            User::Leave(KErrNotSupported);
            }
        }
    else //if not, check MIME type
        {
        if( !UPnPDlnaUtility::IsSupportedMimeType( pInfo->ThirdField() ) )
            {
            User::Leave(KErrNotSupported);
            }
        
        }
    CleanupStack::PopAndDestroy( pInfo );
    }



