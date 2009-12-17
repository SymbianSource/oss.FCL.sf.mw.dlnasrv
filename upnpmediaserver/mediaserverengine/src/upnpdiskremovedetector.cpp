/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Detects remove disk event.
*
*/


// INCLUDE FILES
#include "upnpdiskremovedetector.h"
#include "upnpmediaserver.h"
#include "upnpcontentdirectoryglobals.h"
#include <f32file.h>

#ifdef _DEBUG
#define KLogFile _L("UPnPMediaServer.txt")
#endif
#include "upnpcustomlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::CUpnpDiskRemoveDetector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDiskRemoveDetector::CUpnpDiskRemoveDetector( CUpnpMediaServer* aObserver ) :
    CActive( CActive::EPriorityStandard ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDiskRemoveDetector* CUpnpDiskRemoveDetector::NewLC(
    CUpnpMediaServer* aObserver )
    {
    CUpnpDiskRemoveDetector* self =
    	new (ELeave) CUpnpDiskRemoveDetector( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDiskRemoveDetector* CUpnpDiskRemoveDetector::NewL(
    CUpnpMediaServer* aObserver )
    {
    CUpnpDiskRemoveDetector* self =
            CUpnpDiskRemoveDetector::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::~CUpnpDiskRemoveDetector
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpDiskRemoveDetector::~CUpnpDiskRemoveDetector()
    {
    Cancel();
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::Start
// Enable object to watch for incoming events.
// ---------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::StartL()
    {
    if ( !IsActive() )
        {
        // we want to check the validity of upload directory on MS startup
        CheckUploadDirectoryL();
        SubscribeForDiskEvent();
        }
    }

// ---------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::CheckUploadDirectoryL
// Check if the upload direcotory is valid.
// ---------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::CheckUploadDirectoryL()
    {
    // Getting current upload directory from cenrep
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );

    HBufC8* uploadtmp = settings->GetL( UpnpMediaServerSettings::EUploadDirectory );
    CleanupStack::PushL( uploadtmp );

    HBufC* uploaddir = HBufC::NewLC( uploadtmp->Length() );
    uploaddir->Des().Copy( *uploadtmp );

    // retrieving drive info
    TParse path;
    path.Set( uploaddir->Des(), NULL, NULL );
    TDriveUnit driveUnit( path.Drive() );

    TVolumeInfo volumeInfo;
    volumeInfo.iDrive.iType = EMediaNotPresent;
    TInt err = iFs.Volume( volumeInfo, static_cast<TInt>( driveUnit ) );

    if ( err != KErrNone )
        {
        TChar systemDriveLetter;
        TInt error = RFs::DriveToChar( RFs::GetSystemDrive(), systemDriveLetter );

        RBuf tmpUploadDir;
        tmpUploadDir.CleanupClosePushL();
        tmpUploadDir.CreateL( KCharLen
                            + KCol().Length()
                            + KDiskPathElSep().Length()
                            + KDefaultUploadDir().Length() );

        tmpUploadDir.Append( systemDriveLetter );
        tmpUploadDir.Append( KCol() );
        tmpUploadDir.Append( KDiskPathElSep() );
        tmpUploadDir.Append( KDefaultUploadDir() );
        iObserver->SetUploadDirL( tmpUploadDir );
        CleanupStack::PopAndDestroy( &tmpUploadDir );
        }
    else
        {
        LOGS( "CUpnpDiskRemoveDetector - some disk was dismounted, but it's not interesting in that context" );
        }

    CleanupStack::PopAndDestroy( uploaddir );
    CleanupStack::PopAndDestroy( uploadtmp );
    CleanupStack::PopAndDestroy( settings );
    }

// ---------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::SubscribeForDiskEvent
// Subscribe this object for file system disk event
// ---------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::SubscribeForDiskEvent()
    {
    // wait for FileSystem disk event (like MMC card removal)
    if ( !IsActive() )
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::RunL
// Called after event occurs or on media server startup
// ---------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    CheckUploadDirectoryL();
    SubscribeForDiskEvent();
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpDiskRemoveDetector::RunError( TInt aError )
    {
    LOGS1( "CUpnpDiskRemoveDetector::RunError(%d)", aError );
    SubscribeForDiskEvent();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpDiskRemoveDetector::DoCancel
// Cancel watching for incoming events.
// -----------------------------------------------------------------------------
//
void CUpnpDiskRemoveDetector::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }

// End of File
