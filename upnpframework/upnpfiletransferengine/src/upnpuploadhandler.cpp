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
* Description:  Implementation of the CUpnpUploadHandler class
*
*/


// INCLUDE FILES
// System
#include <AknUtils.h>                           // CEikonEnv
#include <bautils.h>                            // BaflUtils

// upnpframework / avcontroller api
#include "upnpavcontrollerfactory.h"            // UPnPAVControllerFactory
#include "upnpavcontroller.h"                   // MUPnPAVController
#include "upnpavbrowsingsession.h"              // MUPnPAVBrowsingSession
#include "upnpfileuploadsession.h"              // MUPnPFileDownloadSession

// upnpframework / avcontroller helper api
#include "upnpfileutility.h"                    // IsFileProtectedL()

// filetransferengine internal
#include "upnpnotehandler.h"                 
#include "upnpuploadhandler.h"


_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KZero =                      0;   
const TInt KProgressBaseValue =         100;

const TInt KUploadItemValue =           1;

// --------------------------------------------------------------------------
// CUpnpUploadHandler::NewL
// NewL.
// --------------------------------------------------------------------------
//
CUpnpUploadHandler* CUpnpUploadHandler::NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[CUpnpUploadHandler] CUpnpUploadHandler: NewL" );

    // Check that the browsing session is valid and has target device set.
    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    CUpnpUploadHandler* self = NULL;
    self = new (ELeave) CUpnpUploadHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aBrowsingSession );
    CleanupStack::Pop( self );
    __LOG( "[CUpnpUploadHandler] CUpnpUploadHandler: NewL end" );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpUploadHandler::CUpnpUploadHandler()
    : CAsyncOneShot( EPriorityStandard )
    {
    __LOG( "[CUpnpUploadHandler] Constructor" );
    iUploadFirst = ETrue;
    iCopiedFileStillInArray = EFalse; 
    iDrmFilesSkipped = EFalse;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpUploadHandler::~CUpnpUploadHandler()
    {
    __LOG( "[CUpnpUploadHandler] Destructor" );

    // If upload session is running, stop it
    if( iAvController &&
         iUploadSession)
        {
        iAvController->StopUploadSession( *iUploadSession );
        }

    // Delete the note handler.
    delete iNoteHandler;

    // delete the UpnpAvControllerClient
    delete iAvController;
    
    TInt i = 0;
    while( i < iObjectsToCopy->Count() )
        {
        if( (*iObjectsToCopy)[i] == NULL )
            {
            iObjectsToCopy->Remove( i );
            }
        else
            {
            i++;
            }    
        }
    
    __LOG1( "[CUpnpUploadHandler] copied %d", iTotalCount-iObjectsToCopy->Count() );
    iObjectsToCopy->Compress();
    __LOG( "[CUpnpUploadHandler] Destructor end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::ConstructL(
                             MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[CUpnpUploadHandler] ConstructL" );

    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    // Leave if UI context is not available
    iCoeEnv = CEikonEnv::Static(); // Not owned
    if( !iCoeEnv )
        {
        User::Leave( KErrNotSupported );
        }
        
        
    // Store the browsing session
    iBrowsingSession = aBrowsingSession;

    // Create UpnpAvControllerClient
    iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL();

    // Create upload session
    iUploadSession = &iAvController->StartUploadSessionL(
                                            iBrowsingSession->Device() );

    // Set this object to be the upload session observer
    iUploadSession->SetObserver( *this );

    iNoteHandler = CUpnpNoteHandler::NewL( this );
    __LOG( "[CUpnpUploadHandler] ConstructL end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::UploadItemsL
// Uploads the given objects to the remote server
// (setting in the Home Media application).
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::UploadItemsL( RPointerArray<TDesC16>* aObjectIds,
                                       TUpnpFileUploadMode aFileUploadMode )
    {
    __LOG( "[CUpnpUploadHandler] UploadItemsL" );
    // Check parameter
    if( aObjectIds->Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }
    
    iUploadMode = aFileUploadMode;
    iObjectsToCopy = aObjectIds;
    
    iTotalCount = iObjectsToCopy->Count();

    // Initialise values
    iCopyPosition = -1;
    iCopyCompleteness = KZero;
    iCopiedFileStillInArray = EFalse;
    
    iStatusCode = KErrNone;
   
    // Start uploading the objects
    TRAP( iStatusCode, StartUploadL() );

    if( iStatusCode == KErrNone )
        {
        if( iUploadMode == EUpnpMove )
            {
            iNoteHandler->RunProgressNoteL( EUpnpMoveProgressNote );
            }
        else
            {
            iNoteHandler->RunProgressNoteL( EUpnpCopyProgressNote );
            }
        }
    
     /**
     * If the user cancels the copy there might be a situation 
     * where the file has just been uploaded successful but the 
     * TransferComplete is not called. 
     * In this case we need to delete that file from iObjectsToCopy.
     */

    if ( iStatusCode == KErrCancel )
        {
        if( iCopiedFileStillInArray )
            {                                           
            delete (*iObjectsToCopy)[iUploadItems];
            (*iObjectsToCopy)[iUploadItems] = NULL;  
            
            }
        }    
       
    
    // Leave if there was an error
    if( iStatusCode != KErrNone )
        {
        __LOG1( "[CUpnpUploadHandler] UploadItemsL leave %d", iStatusCode );
        User::Leave( iStatusCode );
        }
    
     
    __LOG( "[CUpnpUploadHandler] UploadItemsL end" );    
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::UploadPlayListL
// Uploads the given playlist to the remote server
// (setting in the Home Media application).
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::UploadPlayListL(
                                     const TDesC& /*aPlaylistName*/,
                                     RPointerArray<TDesC16>* aObjectPaths,
                                     TUpnpFileUploadMode aFileUploadMode )
    {
    __LOG( "[CUpnpUploadHandler] UploadPlayListL" );
    UploadItemsL( aObjectPaths, aFileUploadMode );
    __LOG( "[CUpnpUploadHandler] UploadPlayListL end" );    
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::DialogDismissedL
// ProgressDialog call back method. Get's called when a dialog is
// dismissed.
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::DialogDismissedL( )
    {
    __LOG( "[UpnpDownloadHandler] DialogDismissedL" );
    // Update the status code
    if( iStatusCode == KErrNone )
        {
        iStatusCode = KErrCancel;
        }
    
    iUploadSession->CancelAllTransfers();
    __LOG( "[UpnpDownloadHandler] DialogDismissedL end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::MediaServerDisappeared
// --------------------------------------------------------------------------
//    
void CUpnpUploadHandler::MediaServerDisappeared( 
                                    TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "[UpnpDownloadHandler]\t MediaServerDisappeared()" );

    // Update the status code
    if( aReason == EDisconnected )
        {
        iStatusCode = KErrSessionClosed;
        }
    else if( aReason == EWLANLost )
        {
        iStatusCode = KErrDisconnected;
        }
    else
        {
        iStatusCode = KErrUnknown;
        }

    // Finish the progress note
    iNoteHandler->FinishProgressNote();
    
    __LOG( "[UpnpDownloadHandler]\t MediaServerDisappeared() end" );
    }
// --------------------------------------------------------------------------
// CUpnpUploadHandler::TransferStarted
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::TransferStarted( TInt aKey, TInt aStatus )
    {
    __LOG1( "[CUpnpUploadHandler] TransferStarted() aKey %d", aKey );
    
    if( !iProgressBarMaxValueSet )
        {
        iNoteHandler->SetMaxValue( iTotalCount * KProgressBaseValue );
        iProgressBarMaxValueSet = ETrue;
        }
    
    
    if( aStatus != KErrNone)
        {
        iStatusCode = aStatus;    
        }
    else if( aKey < 0 || aKey > iCopyPosition )
        {
        iStatusCode = KErrGeneral;
        }
    
    if( KErrNone != iStatusCode )
        {
        iNoteHandler->FinishProgressNote();    
        }
    else
        {
        TRAP_IGNORE( iUploadSession->StartTrackingProgressL( aKey ) );    
        }     
    __LOG( "[CUpnpUploadHandler] TransferStarted() end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::TransferCompleted
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::TransferCompleted( TInt aKey,
                                            TInt aStatus,
                                            const TDesC& /*aFilePath*/ )
    {
    __LOG( "[CUpnpUploadHandler] TransferCompleted()" );
    __LOG1( "[CUpnpUploadHandler] aKey %d", aKey );
    __LOG1( "[CUpnpUploadHandler] aStatus %d", aStatus );
    __LOG1( "[CUpnpUploadHandler] iTotalCount %d", iTotalCount );
        
    iStatusCode = aStatus;    
    iUploadItems++;
    
    __LOG1( "[CUpnpUploadHandler] iUploadItems %d", iUploadItems );
    
    if( KErrNone == iStatusCode && iObjectsToCopy->Count() > 0 ) 
        {
        if( iUploadMode == EUpnpMove )
            {
            __LOG( "Deleting file..." );
            TInt deleteError = BaflUtils::DeleteFile( 
                                            iCoeEnv->FsSession(),
                                            *(*iObjectsToCopy)[aKey] );
            if( deleteError != KErrNone )
            {
            __LOG( "Failed to delete file!");
            }
            }
        delete (*iObjectsToCopy)[aKey];
        (*iObjectsToCopy)[aKey] = NULL;
        iCopiedFileStillInArray = EFalse;
        // Update the download completeness percentage value
        iCopyCompleteness = KProgressBaseValue * iUploadItems;

        // Update the progress note
        iNoteHandler->SetValue( iCopyCompleteness );
        }
    
    if( KErrNone != iStatusCode || 
      ( aKey < 0 || aKey > iCopyPosition ) || 
        iTotalCount <= iUploadItems )
        {
        iNoteHandler->FinishProgressNote();    
        }
    else if( --iUploadIndex <=0 )
        {
        TRAPD( error, StartUploadL() );
        if( error != KErrNone )
            {
            iStatusCode = error;
            iNoteHandler->FinishProgressNote(); 
            }
        }
    __LOG( "[CUpnpUploadHandler] TransferCompleted() end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::TransferProgress
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::TransferProgress( TInt aKey,
                                           TInt aBytes,
                                           TInt aTotalBytes )
    {
    __LOG1( "[CUpnpUploadHandler] TransferProgress() aKey %d", aKey );
    __LOG1( "[CUpnpUploadHandler] TransferProgress() iCopyPosition %d", iCopyPosition );
    if( aKey < 0 || aKey > iCopyPosition )
        {
        iStatusCode = KErrGeneral;
        iNoteHandler->FinishProgressNote();
        }
    else
        {
        float progress = ((float)aBytes / aTotalBytes) * 100;
     
        if( aBytes == aTotalBytes )
            { 
            iCopiedFileStillInArray = ETrue;
            }
        
        iNoteHandler->SetValue( iCopyCompleteness + (TInt)progress);    
        }    
    __LOG( "[CUpnpUploadHandler] TransferProgress() -end" );
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::StartUploadL
// Starts the next upload. Leaves (KErrCompletion) if there are no more
// objects to upload.
// --------------------------------------------------------------------------
//
void CUpnpUploadHandler::StartUploadL()
    {
    __LOG( "[CUpnpUploadHandler] StartUploadL()" );
    
    TInt error = KErrNone;
     
    if( iUploadFirst )
        {
        iUploadIndex = 1;
        iUploadFirst = EFalse;
        }
    else
        {
        iUploadIndex = iTotalCount - iUploadItems;
    
        if( iUploadIndex > KUploadItemValue )
            {
            iUploadIndex = KUploadItemValue;
            }
        else if( iUploadIndex <= 0 )
            {
            //copy is fininshed
            iStatusCode = error;
            iNoteHandler->FinishProgressNote();
            return;
            }    
        }

    for( TInt i = 0; i< iUploadIndex; i++ )
        {
        iCopyPosition++;
        
        TBool isDrmProtected = ETrue;
        TRAP( error, isDrmProtected = UPnPFileUtility::IsFileProtectedL( *(*iObjectsToCopy)[ iCopyPosition ] ) );

        if( error == KErrNone && isDrmProtected == EFalse)
            {
            TRAP( error, iUploadSession->StartUploadL( 
                                    *(*iObjectsToCopy)[ iCopyPosition ],
                                    iCopyPosition ) );
            }
        
        // If parsing or sending the download action failed, exit
        if( error != KErrNone )
            {
            iUploadSession->CancelAllTransfers();  
            iStatusCode = error;
            i = iTotalCount;
            iNoteHandler->FinishProgressNote();
            } 
        else if( isDrmProtected )
            {
            __LOG( "[CUpnpUploadHandler] StartUploadL() drm file, start timer" );
            // show note only once if there is several drm protected files
            if( !iDrmFilesSkipped )
                {
                TRAP_IGNORE( iNoteHandler->ShowSkippingDRMFilesNoteL() );
                iDrmFilesSkipped = ETrue;
                }
             
            // start timer to process next file   
            Call();             
            }
        }
    __LOG( "[CUpnpUploadHandler] StartUploadL() end" );
    }





// --------------------------------------------------------------------------
// CUpnpUploadHandler::RunL
// --------------------------------------------------------------------------
void CUpnpUploadHandler::RunL()
    {
    __LOG( "CUpnpUploadHandler::RunL" );
    
    iUploadItems++;
    
    if( --iUploadIndex <=0 )
        {
        StartUploadL();
        }
    }

// --------------------------------------------------------------------------
// CUpnpUploadHandler::RunError
// --------------------------------------------------------------------------
TInt CUpnpUploadHandler::RunError( TInt aError )
    {
    __LOG1( "CUpnpUploadHandler::RunError %d", aError );
    iUploadSession->CancelAllTransfers();  
    iStatusCode = aError;
    iNoteHandler->FinishProgressNote();
    
    return KErrNone;
    }   
    
  
        
// End of file
