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
* Description:  Main class in the manory manager component
*
*/


#include <e32std.h>
#include <AknGlobalNote.h> // CAknGlobalNote
#include <AknGlobalProgressDialog.h>  // CAknGlobalProgressDialog
#include <mpxharvesterutility.h>
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <avkon.rsg> // R_AVKON_SOFTKEYS_OK_EMPTY
#include <barsc.h> // RResourceFile
#include <barsread.h> // TResourceReader
#include <bautils.h> // BaflUtils
#include <AknUtils.h>
#include <memorymanager.rsg>
#include <pathinfo.h>

#include "cmcommon.h"
#include "cmfilllistitem.h"
#include "cmdmmain.h"
#include "upnpstring.h"
#include "msdebug.h"
#include "cmmmmain.h"
#include "cmmmquotalistener.h"


// Contants
const TInt64 KDeleteLimit = 50; // 50MB
const TInt KSizeStringLength = 10;
const TInt KTimeoutValue = 3000000; // 3 seconds
_LIT16( KCmDoubleBackSlash,    "\\" ); 
_LIT16( KCmColon,              ":");

_LIT( KResourceFilePath, "\\resource\\apps\\memorymanager.rsc" );

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::NewL
// ---------------------------------------------------------------------------
//
CCmMmQuotaListener* CCmMmQuotaListener::NewL( CCmDmMain& aDbManager,
                                              TInt aDrive )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::NewL() start"));
    CCmMmQuotaListener* self = CCmMmQuotaListener::NewLC( aDbManager,
                                                          aDrive );
    CleanupStack::Pop( self );
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::NewLC
// ---------------------------------------------------------------------------
//
CCmMmQuotaListener* CCmMmQuotaListener::NewLC( CCmDmMain& aDbManager,
                                               TInt aDrive )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::NewLC() start"));
    CCmMmQuotaListener* self = new ( ELeave ) CCmMmQuotaListener( aDbManager,
                                                                   aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::NewLC() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CCmMmQuotaListener::~CCmMmQuotaListener()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::~CCmMmQuotaListener()\
             start"));
    if( IsActive() )
        {
        Cancel();
        }
    iFileSession.Close();
    
    if( iHarvester )
        {
        iHarvester->Close();
        }
    delete iProgressDialog;
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::~CCmMmQuotaListener() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::DoCancel
// ---------------------------------------------------------------------------
//
void CCmMmQuotaListener::DoCancel()
    {
    iFileSession.NotifyDiskSpaceCancel();
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::RunL
// ---------------------------------------------------------------------------
//
void CCmMmQuotaListener::RunL()
    {
    TRACE(Print(_L("[MEMORY MNGR]\t CCmMmQuotaListener:RunL status = %d"),
        iStatus.Int() ) );

    // Check if MMC is inserted
    if ( CCmMmMain::DriveState( iFileSession, iDrive ) && 
         !iProgressDialog )
        {
        // Check out that NotifyDiskSpace event came from quota
        TVolumeInfo volumeInfo;
        iFileSession.Volume( volumeInfo, iDrive );

        if ( volumeInfo.iFree >= iQuota )
            {
            // Request a notification when disk space crosses the threshold 
            // value.
            RequestDiskSpaceNotification();
            }
        else 
            {   
            // Get lowest priority list
            RPointerArray<CCmFillListItem> array;
            iDbManager.PrepareQueryCmdL( EFilledLowPriorityQuery );
            iDbManager.GetLowPriorityFilledFilesL( iDrive, array );
            // If Fill list is empty we wait 3 seconds and try again
            if ( !array.Count() )
                {
                TRACE( Print( _L("[MEMORY MNGR]\t CCmMmQuotaListener:RunL \
                    Fill list is empty") ) );
                }
            else 
                {
                switch ( iStatus.Int() )
                    {
                    case KErrNone:
                        {
                        // Remove files from the end of the list so that the 
                        // total amount of files in the list is no more than 
                        // KDeleteLimit megabytes.

                        // listsize is the total size of the files in array 
                        // in megabytes.
                        TUint32 listSize = RemoveExtraFiles( array, 
                                                             KDeleteLimit ); 

                        // Get fill list name
                        HBufC8* listName8 = NULL;
                        iDbManager.GetFillListNameL( listName8,
                                                     array[0]->ListId() );
                        CleanupStack::PushL( listName8 );
                        
                        HBufC* listName16 = 
                            HBufC::NewLC( listName8->Length() );
                        listName16->Des().Copy( *listName8 );
                        
                        // Get confirmation for the delete operation from the 
                        // user.
                        TRequestStatus status = DeleteConfirmationL( 
                            *listName16, 
                            listSize );
                        
                        // If file deletion Yes is selected
                        if ( status.Int() == EAknSoftkeyYes )
                            {
                            // delete the files
                            TUint32 delAmount = DeleteFilesL( array, 
                                                              *listName8 );
                                                              
                            TRACE( Print(_L("[MEMORY MNGR]\t Deleted %d \
                                MBytes from list %S"), 
                                delAmount, listName16 ) );
                            }
                        else
                            {
                            LOG( _L("EAknSoftkeyNo") );
                            // Request a notification when disk space crosses
                            // the threshold value.
                            iFileSession.NotifyDiskSpace( 
                                iQuota, iDrive, iStatus );
                            }

                        // clean up
                        CleanupStack::PopAndDestroy( listName16 );
                        CleanupStack::PopAndDestroy( listName8 );
                        array.ResetAndDestroy();
                        SetActive();
                        break;
                        }
                    case KErrNotReady:
                        {
                        TRACE( Print( 
                            _L("[MEMORY MNGR]\t CCmMmQuotaListener::RunL \
                            status = %d"), iStatus.Int() ) );

                        // Wait 3 seconds and try again.
                        RequestNotificationAfterTimeout( KTimeoutValue );
                        break;
                        }
                    default:
                        {
                        TRACE( Print( 
                            _L("[MEMORY MNGR]\t CCmMmQuotaListener::RunL \
                            status = %d"), iStatus.Int() ) );

                        // Request a notification when disk space crosses the
                        // threshold value.
                        RequestDiskSpaceNotification();
                        break;
                        }
                    }                
                }
            }
        }
    else if( iProgressDialog )
        {
        LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::RunL ProgressDialog"));
        iProgressDialog->CancelProgressDialog();
        iProgressDialog = NULL;
        RequestDiskSpaceNotification();
        }
    else
        {
        TRACE(Print(_L("[MEMORY MNGR]\t CCmMmQuotaListener:RunL: \
            Drive not found!")));
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCmMmQuotaListener::CCmMmQuotaListener( CCmDmMain& aDbManager, TInt aDrive )
    : CActive( EPriorityStandard ),
    iDrive( aDrive ),
    iQuota( KCmMmcLowMemory ),
    iDbManager( aDbManager )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::ConstructL
// ---------------------------------------------------------------------------
//
void CCmMmQuotaListener::ConstructL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::ConstructL() start"));

    User::LeaveIfError( iFileSession.Connect() );
        
    iHarvester = CMPXHarvesterFactory::NewL();

    // Request a notification when disk space crosses the threshold 
    // value.
    RequestDiskSpaceNotification();

    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::ConstructL() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::ReadResourceL
// ---------------------------------------------------------------------------
//
HBufC16* CCmMmQuotaListener::ReadResourceL( TInt aResourceId )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::ReadReasourceL() start"));
    TFileName rscFileName( KResourceFilePath );

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( iFileSession, rscFileName );
        
    TRACE( Print( _L("[MEMORY MNGR]\t Resource path = %s"), &rscFileName ) );
    RResourceFile resourceFile;
    TInt err( KErrNone );
    TRAP(err, resourceFile.OpenL(iFileSession, rscFileName ))
    if ( err != KErrNone )
        {        
        LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::Read from e"));
        // not found? let's try mmc drive
        rscFileName.Copy( PathInfo::MemoryCardRootPath() );
        rscFileName.Delete(2, 2); // remove //
        rscFileName.Append( KResourceFilePath );        
        resourceFile.OpenL(iFileSession, rscFileName );
        }
    HBufC8* res = resourceFile.AllocReadLC( aResourceId );    
    TResourceReader theReader;
    theReader.SetBuffer(res);
    HBufC16* des = theReader.ReadHBufC16L();
    CleanupStack::PopAndDestroy( res );
    resourceFile.Close();
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::ReadReasourceL() end"));
    return des;
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::ResolveAndAppendRootPath()
// ---------------------------------------------------------------------------
//
HBufC* CCmMmQuotaListener::ResolveAndAppendRootPathL( 
    const TInt aDriveNumber )
    {
    HBufC* buf = HBufC::NewLC( 4 );
    TChar driveChar;
    iFileSession.DriveToChar( aDriveNumber, driveChar );
    buf->Des().Append( driveChar );
    buf->Des().Append( KCmColon );
    buf->Des().Append( KCmDoubleBackSlash );
    CleanupStack::Pop( buf );
    return buf;    
    }
    
// ---------------------------------------------------------------------------
// CCmMmQuotaListener::RequestNotificationAfterTimeout()
// ---------------------------------------------------------------------------
//
void CCmMmQuotaListener::RequestNotificationAfterTimeout( TInt aTimeoutValue )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::\
        RequestNotificationAfterTimeout()"));
    TTimeIntervalMicroSeconds32 timeOut( aTimeoutValue );
    RTimer timer;
    timer.CreateLocal();
    TRequestStatus status( KRequestPending );
    timer.After( status, timeOut ); // Wait time defined by aTimeoutValue
    User::WaitForRequest( status );
    timer.Close();

    // Request a notification when disk space crosses the threshold 
    // value.
    RequestDiskSpaceNotification();
    }
    
// ---------------------------------------------------------------------------
// CCmMmQuotaListener::RequestDiskSpaceNotification()
// ---------------------------------------------------------------------------
//
void CCmMmQuotaListener::RequestDiskSpaceNotification()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::\
        RequestDiskSpaceNotification()"));
    iFileSession.NotifyDiskSpace( iQuota, iDrive, iStatus );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CCmMmQuotaListener::DeleteConfirmationL()
// ---------------------------------------------------------------------------
//
TRequestStatus CCmMmQuotaListener::DeleteConfirmationL( 
    const TDesC& aListName, 
    TUint32 aListSize )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::DeleteConfirmationL()"));
    // Get text for delete confirmation dialog
    HBufC* format = ReadResourceL( R_MSERV_CONF_AUTO_REMOVE );
    CleanupStack::PushL( format );

    HBufC* tempName = aListName.AllocLC();
        
    // Create confirm note
    HBufC* confirmNote = NULL;    
    if ( aListName.Length() == 0 )
        {
        //reserve 1 character for empty list name
        confirmNote = HBufC::NewLC( format->Length() +
                                     1 + 
                                     KSizeStringLength );
        confirmNote->Des().Format( *format,
                                   aListSize,
                                   &KNullDesC() );
        }
    else
        {
        confirmNote = HBufC::NewLC( format->Length() +
                                     aListName.Length() +
                                     KSizeStringLength );
        confirmNote->Des().Format( *format,
                                   aListSize,
                                   tempName );
        }

    // Show note to ask if it's ok to delete files.
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    TRequestStatus status = KRequestPending;
    note->SetSoftkeys( R_AVKON_SOFTKEYS_YES_NO );
    note->ShowNoteL( status,
                     EAknGlobalInformationNote,
                     *confirmNote );
    User::WaitForRequest( status );

    // Clean up
    CleanupStack::PopAndDestroy( note );
    CleanupStack::PopAndDestroy( confirmNote );
    CleanupStack::PopAndDestroy( tempName );    
    CleanupStack::PopAndDestroy( format );
    
    // Return the user selection.
    return status;
    }
    
// ---------------------------------------------------------------------------
// CCmMmQuotaListener::DeleteFilesL()
// ---------------------------------------------------------------------------
//
TUint32 CCmMmQuotaListener::DeleteFilesL( 
    const RPointerArray<CCmFillListItem>& aArray,
    const TDesC8& aListName
    )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::DeleteFilesL()"));
    // Get text for progress dialog
    HBufC* format = ReadResourceL( R_MSERV_PROGRESS_REMOVE );
    CleanupStack::PushL( format );

    HBufC* listName = UpnpString::ToUnicodeL( aListName );
    CleanupStack::PushL( listName );
    
    HBufC* progressNote = NULL;
    if ( listName->Length() == 0 )
        {
        //reserve 1 character for empty list name
        progressNote = HBufC::NewLC( format->Length() + 1 );
        progressNote->Des().Format( *format,
                                    &KNullDesC() );
        }
    else
        {
        progressNote = HBufC::NewLC( format->Length() +
                                     listName->Length() );
        progressNote->Des().Format( *format,
                                    listName );
        }

    
    // Show file deletion progress in a progress dialog.
    iProgressDialog = CAknGlobalProgressDialog::NewL();
    iProgressDialog->ShowProgressDialogL( iStatus,
                                 *progressNote,
                                 R_AVKON_SOFTKEYS_EMPTY,
                                 aArray.Count() );

    // Create files array for file paths
    CDesC16Array* files =
        new (ELeave) CDesC16ArrayFlat( aArray.Count() );
    CleanupStack::PushL( files );

    // Delete all items from current array
    HBufC* temp( NULL );
    TFileName path;
    TUint32 sizes = 0;
    
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        temp = ResolveAndAppendRootPathL( iDrive );
        CleanupStack::PushL( temp );
        path.Append( *temp );
        path.Append( aArray[i]->Path() );
        files->AppendL( path );
        iFileSession.Delete( path );
        CleanupStack::PopAndDestroy( temp );
        path.Zero();
        sizes += aArray[i]->Size();
        TRACE(Print(_L("[MEMORY MNGR]\t CCmMmQuotaListener::DeleteFilesL \
                        Deleted file %S, size %d"), 
                    &aArray[i]->Path(), 
                    aArray[i]->Size()));
        iProgressDialog->UpdateProgressDialog( i, aArray.Count() - 1 );
        }

    TInt tes( KErrNone );
    TRAP( tes, iDbManager.RestoreFileStatusesL( aArray,
                                                aListName ) );
    TRACE( Print( _L("[MEMORY MNGR]\t tes = %d "), tes ) );

    MCLFContentListingEngine* clfEngine =
                 ContentListingFactory::NewContentListingEngineLC();    
    TRAP_IGNORE( clfEngine->UpdateItemsL() );
    CleanupStack::PopAndDestroy();
    
    TRAP_IGNORE( iHarvester->RemoveFilesL( *files ) );

    CleanupStack::PopAndDestroy( files );
    iProgressDialog->ProcessFinished();
    CleanupStack::PopAndDestroy( progressNote );
    CleanupStack::PopAndDestroy( listName );
    CleanupStack::PopAndDestroy( format );

    return sizes;
    }

// ---------------------------------------------------------------------------
// CCmMmQuotaListener::RemoveExtraFiles()
// ---------------------------------------------------------------------------
//
TUint32 CCmMmQuotaListener::RemoveExtraFiles( 
    RPointerArray<CCmFillListItem>& aArray, 
    TInt64 aDeleteLimit ) const 
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmQuotaListener::RemoveExtraFiles()"));
    TUint32 listSize( 0 );
    TInt lastIndex( 0 );
    for ( TInt i = 0;
          i < aArray.Count() && listSize <= aDeleteLimit; 
          i++ )
        {
        listSize += ( aArray[i]->Size() / KMega );
        lastIndex = i;
        }

    TRACE(Print(_L("[MEMORY MNGR]\t Deleting extra \
        items...")));
    for( TInt j = ( aArray.Count() - 1 ); 
         j > lastIndex ; 
         j-- )
        {
        delete aArray[j];
        aArray.Remove( j );
        }
        
    aArray.Compress();
    return listSize;
    }
    
// End of file    
