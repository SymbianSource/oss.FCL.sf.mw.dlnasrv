/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class in the Memory manager component
*
*/


#include <e32std.h>
#include <e32base.h>
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionhelper.h>
#include <pathinfo.h>

#include "cmdmmain.h"
#include "msdebug.h"
#include "cmmmmain.h"
#include "cmdriveinfo.h"
#include "cmmmquotalistener.h"
#include "cmmmshrinker.h"
#include "cmmmobserver.h"

// ---------------------------------------------------------------------------
// CCmMmMain::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMmMain* CCmMmMain::NewL( CCmDmMain& aDbManager )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::NewL() start"));
    CCmMmMain* self = CCmMmMain::NewLC( aDbManager );
    CleanupStack::Pop( self );
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmMmMain::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMmMain* CCmMmMain::NewLC( CCmDmMain& aDbManager )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::NewLC() start"));
    CCmMmMain* self = new ( ELeave ) CCmMmMain( aDbManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::NewLC() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMmMain::~CCmMmMain()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::~CCmMmMain() start"));

    iFileSession.Close();
    iQuotaListeners.ResetAndDestroy();
    delete iDeleter;
    delete iFilesToDelete;
    delete iFilesToShrink;
    
    if( iHarvester )
        {
        iHarvester->Close();
        }

    CloseMpxCollectionHelper();

    delete iObserver;
    delete iShrinker;
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::~CCmMmMain() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::Cancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMmMain::Cancel()
    {
    if ( iDeleter )
        {
        iDeleter->Cancel();
        if ( iObserver )
            {
            // Trap here, do nothing if it leaves.
            TRAP_IGNORE( iObserver->DeleteCompleteL( KErrCancel ) );
            }
        }
    if ( iShrinker )
        {
        iShrinker->Cancel();
        if ( iObserver )
            {
            // Trap here, do nothing if it leaves
            TRAP_IGNORE( iObserver->ShrinkCompleteL( KErrCancel ) );
            }
        }
    for( TInt i = 0; i < iQuotaListeners.Count(); i++ )
        {        
        iQuotaListeners[i]->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CCmMmMain::DeleteFilesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMmMain::DeleteFilesL( CDesCArray& aFiles )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DeleteFilesL() start"));

    if ( &aFiles && aFiles.Count() > 0 )
        {
        delete iFilesToDelete;
        iFilesToDelete = NULL;
        iFilesToDelete = new ( ELeave ) CDesC16ArrayFlat( aFiles.Count() );
        for ( TInt i = 0; i < aFiles.Count(); i++ )
            {
            iFilesToDelete->AppendL( aFiles[i] );
            }
        }
    iDeleteIndex = 0;

    if ( iDeleter )
        {
        CloseMpxCollectionHelper();
        iMpxCollectionHelper = CMPXCollectionHelperFactory::NewCollectionHelperL();
        iDeleter->Start( TCallBack( BackgroundDeleteL, this ) );
        }
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DeleteFilesL() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::SetObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMmMain::SetObserver( MCmMmObserver& aObserver )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::SetObserver() start"));
    iObserver = &aObserver;
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::SetObserver() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::SetQuotaListenerStateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMmMain::SetQuotaListenerStateL( TBool aState )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::SetQuotaListenerStateL() start"));
    
    if( !aState )
        {
        iQuotaListeners.ResetAndDestroy();
        iQuotaListeners.Close();
        LOG(_L("[MEMORY MNGR]\t Quotalisteners deleted"));
        }
    else
        {
        iDbManager.PrepareQueryCmdL( EDrivesQuery );
        RPointerArray<CCmDriveInfo> drives;
        TRAP_IGNORE( iDbManager.QueryDrivesL( drives ) );
        iQuotaListeners.ResetAndDestroy();
        for( TInt i = 0 ; i < drives.Count(); i++ )
            {
            TRACE(Print(_L("[MEMORY MNGR]\t Starting quota listener to \
            drive = %d"), drives[i]->DriveNumber() ));            
            CCmMmQuotaListener* listener = 
                CCmMmQuotaListener::NewL( 
                    iDbManager, 
                    drives[i]->DriveNumber() );
            iQuotaListeners.AppendL( listener );        
            }
        drives.ResetAndDestroy();            
        }        
    }

// ---------------------------------------------------------------------------
// CCmMmMain::ShrinkImagesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMmMain::ShrinkImagesL( CDesCArray& aFiles )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::ShrinkImagesL() start"));

    if ( !iShrinker )
        {
        iShrinker = CCmMmShrinker::NewL( iDbManager );       
        }
    else
        {
        // Cancel the possible ongoing operation
        iShrinker->Cancel();
        }
    iShrinker->SetObserver( iObserver );
    iShrinker->ShrinkImagesL( aFiles );

    LOG(_L("[MEMORY MNGR]\t CCmMmMain::ShrinkImagesL() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::MmcState
// ---------------------------------------------------------------------------
//
TBool CCmMmMain::DriveState( const RFs& aFileServer, TInt aDriveNumber )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DriveState() start"));

    TDriveInfo driveInfo;
    TInt error = aFileServer.Drive( driveInfo, aDriveNumber );

    //check if MMC is present
    TBool status = EFalse;
    if ( error == KErrNone && driveInfo.iType == EMediaHardDisk )
        {
        status = ETrue;
        LOG(_L("[MEMORY MNGR]\t CCmMmMain::DriveState() Drive found"));
        }

    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DriveState() end"));
    return status;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCmMmMain::CCmMmMain( CCmDmMain& aDbManager ) :
    iDeleteIndex( 0 ),
    iQuotaListenerState( ETrue ),
    iDbManager( aDbManager )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::CCmMmMain() start"));
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::CCmMmMain() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::ConstructL
// ---------------------------------------------------------------------------
//
void CCmMmMain::ConstructL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::ConstructL() start"));

    User::LeaveIfError( iFileSession.Connect() );
    iDeleter = CIdle::NewL( CActive::EPriorityIdle );    
    iHarvester = CMPXHarvesterFactory::NewL();
    SetQuotaListenerStateL( ETrue );

    LOG(_L("[MEMORY MNGR]\t CCmMmMain::ConstructL() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmMain::BackgroundDeleteL
// ---------------------------------------------------------------------------
//
TInt CCmMmMain::BackgroundDeleteL( TAny* aMm )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::BackgroundDeleteL() start"));
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::BackgroundDeleteL() end"));
    return ((CCmMmMain*)aMm)->DoBackgroundDeleteL();
    }

// ---------------------------------------------------------------------------
// CCmMmMain::DoBackgroundDeleteL
// ---------------------------------------------------------------------------
//
TInt CCmMmMain::DoBackgroundDeleteL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DoBackgroundDeleteL() start"));
    TInt moreFiles = EFalse;
    TInt error = KErrNone;
    TInt trapinfo( KErrNone );

    if ( iFilesToDelete )
        {
        if ( iDeleteIndex < iFilesToDelete->Count() )
            {
            TPtrC filenameTemp = (*iFilesToDelete)[iDeleteIndex];
            // Delete file from filesystem
            error = iFileSession.Delete( filenameTemp );

            TRACE(Print(_L("[MEMORY MNGR]\t CCmMmMain:DoBackgroundDeleteL \
error: %d"), error));

#ifdef _DEBUG

            TRACE(Print(_L("[MEMORY MNGR]\t CCmMmMain: File %S deleted"),
                                          &filenameTemp ));
#endif // _DEBUG
            TRAP( trapinfo, iMpxCollectionHelper->RemoveL( filenameTemp ) );
            TRACE(Print(_L("[MEMORY MNGR]\t CCmMmMain:DoBackgroundDeleteL \
RemoveL: %d"), trapinfo));

            iDeleteIndex++;
            }

        // return ETrue if there are more files to delete
        if ( iDeleteIndex < iFilesToDelete->Count() )
            {
            moreFiles = ETrue;
            }
        // No more files to be deleted. Call Media Gallery update.
        else if ( iFilesToDelete->Count() )
            {
            MCLFContentListingEngine* clfEngine =
                    ContentListingFactory::NewContentListingEngineLC();
            if( clfEngine )
                {
                TRAP_IGNORE( clfEngine->UpdateItemsL() );

                TRAP_IGNORE( iHarvester->RemoveFilesL( *iFilesToDelete ) );
                }
            CleanupStack::PopAndDestroy();

            }
            
        }
                
    // If all files are deleted, inform observer that deletion is ready.
    if ( !moreFiles )
        {
        CloseMpxCollectionHelper();
        }
    
    if ( !moreFiles && iObserver )
        {
        delete iFilesToDelete;
        iFilesToDelete = NULL;
        iObserver->DeleteCompleteL( error );
        }
    LOG(_L("[MEMORY MNGR]\t CCmMmMain::DoBackgroundDeleteL() end"));

    // Return EFalse if background deletion is done, ETrue if there are more 
    // files to be deleted.
    return moreFiles;
    }

// ---------------------------------------------------------------------------
// CCmMmMain::CloseMpxCollectionHelper
// ---------------------------------------------------------------------------
//    
void CCmMmMain::CloseMpxCollectionHelper()
    {
    if( iMpxCollectionHelper )
        {
    	iMpxCollectionHelper->Close();
        }
    iMpxCollectionHelper = NULL;
    }

// End of file
