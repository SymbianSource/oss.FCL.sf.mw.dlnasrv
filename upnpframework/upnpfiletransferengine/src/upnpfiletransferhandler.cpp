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
* Description:  Implementation of the CUpnpFileTransferHandler class
*
*/


// INCLUDE FILES
// System
#include <eikenv.h>                             // CEikonEnv
#include <eikprogi.h>                           // CEikProgressInfo
#include <bautils.h>                            // BaflUtils
#include <data_caging_path_literals.hrh>        // KDC_RESOURCE_FILES_DIR

#include <upnpitem.h>                           // CUpnpItem
#include <upnpcontainer.h>                      // CUpnpContainer
#include <upnpstring.h>                         // UpnpString

// upnpavcontroller api
#include "upnpavbrowsingsession.h"              // MUPnPAVSessionBase
#include "upnpavcontrollerfactory.h"            // UPnPAVControllerFactory
#include "upnpavcontroller.h"                   // MUPnPAVController
#include "upnpfiledownloadsession.h"            // MUPnPFileDownloadSession

// upnpavcontroller / xml parser api
#include "upnpxmlparser.h"

// upnpframework / internal api's
#include "upnpgallerynotifier.h"                // UpnpGalleryNotifer

// filetransferengine internal
#include <upnpfiletransferengineresources.rsg>  // Dialog resources
#include "upnpfiletransferhandler.h"            // CUpnpFileTransferHandler

// CONSTANTS
_LIT( KResFileName,                 "upnpfiletransferengineresources.rsc");
_LIT8( KBrowseSortCriteria,         "" );
_LIT8( KBrowseFilter,               "*" );
const TInt KBrowseRequestCount =        1;
const TInt KProgressInfoInitialValue =  1;
const TInt KProgressInfoFinalValue =    50000;
const TInt KProgressNoteStepper =       2;
const TInt KCopyPositionInitialValue =  0;

_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"


// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::NewL
// NewL.
// --------------------------------------------------------------------------
//
CUpnpFileTransferHandler* CUpnpFileTransferHandler::NewL(
        MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "CUpnpFileTransferHandler::NewL"  );

    // Check that the browsing session is valid and has target device set.
    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    CUpnpFileTransferHandler* self = NULL;
    self = new (ELeave) CUpnpFileTransferHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aBrowsingSession );
    CleanupStack::Pop( self );
    __LOG( "CUpnpFileTransferHandler::NewL -end"  );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpFileTransferHandler::CUpnpFileTransferHandler()
    {
    __LOG( "CUpnpFileTransferHandler::CUpnpFileTransferHandler" );
    iNumCopiedItemFromContainer = 0;
    __LOG( "CUpnpFileTransferHandler::CUpnpFileTransferHandler -end" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpFileTransferHandler::~CUpnpFileTransferHandler()
    {
    __LOG( "CUpnpFileTransferHandler::~CUpnpFileTransferHandler" );

    // Restore the browse session observer
    if( iBrowsingSession &&
        iBrowsingSessionObserverBackup )
        {
        iBrowsingSession->RemoveObserver();
        iBrowsingSession->SetObserver( *iBrowsingSessionObserverBackup );
        }

    // Delete the playlist handler.
    delete iPlaylistHandler;

    // Delete copy item (used in container and playlist copy) is it exists
    delete iCopyItem;
    
    // Deletes download session and avcontroller instances.
    if( iDownloadSession && iAvController )
        {
        iAvController->StopDownloadSession( *iDownloadSession );
        }
    delete iAvController;

    // Un-load resource file
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }
    __LOG( "CUpnpFileTransferHandler::~CUpnpFileTransferHandler -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::ConstructL(
        MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "CUpnpFileTransferHandler::ConstructL" );

    if( aBrowsingSession )
        {
        iBrowsingSession = aBrowsingSession;
        }
    else
        {
        User::Leave( KErrArgument );
        }

    // Leave if UI context is not available
    iCoeEnv = CEikonEnv::Static();
    if( !iCoeEnv )
        {
        User::Leave( KErrNotReady );
        }

    // Get the file server session handle
    RFs& fs = iCoeEnv->FsSession();

    // Parse the resource file path
    TParse parse;
    parse.Set( KResFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName rscFileName;
    rscFileName.Append( parse.FullName() );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    
    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fs, rscFileName );

    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fs, rscFileName ) )
        {
        __LOG( "Resource file does not exist!" );
        User::Leave( KErrNotFound );
        }

    // Read the resource file offset
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );

    // Backup the browse session observer and set this object as an observer.
    iBrowsingSessionObserverBackup = iBrowsingSession->Observer();
    

    iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL();

    // Create download session
    iDownloadSession = &iAvController->StartDownloadSessionL(
                                            iBrowsingSession->Device() );

    // Set this object to be the download session observer
    iDownloadSession->SetObserver( *this );
    
    iFileTransferMode = EUpnpIdleTransferMode;
    
    // Instantiate the playlist handler
    iPlaylistHandler = CUpnpPlaylistHandler::NewL();
    
    __LOG( "CUpnpFileTransferHandler::ConstructL -end" );
    }
   
// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::TransferRemoteItemsToHandsetL
// Copies items from a remote Upnp Media Server to the handset.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::TransferRemoteItemsToHandsetL(
        TUpnpFileTransferMode aTransferMode,
        RPointerArray<CUpnpItem>& aItems )
    {
    __LOG( "CUpnpFileTransferHandler::TransferRemoteItemsToHandsetL" );
    if( aItems.Count() <= 0 ||
        aTransferMode != EUpnpCopyRemoteItemsToDefaultLocation )
        {
        User::Leave( KErrArgument ); 
        }

    // Initialise member variables
    iFileTransferMode = aTransferMode;
    iCopyItems = &aItems;
    iCopyPosition = KCopyPositionInitialValue;
    iStatusCode = KErrNone;

    // Copy the first file
    TRAP( iStatusCode, iDownloadSession->StartDownloadL( 
            (*(*iCopyItems)[iCopyPosition]),
            iCopyPosition ) );
    
    // If first copy action was sent succesfully, draw the progress note,
    // otherwise leave with the error code
    if( iStatusCode == KErrNone )
        {
        RunCopyNoteL( iCopyItems->Count() );
        }

    // Notify Media Gallery of the filesystem changes (files added)
    TRAP_IGNORE( UpnpGalleryNotifier::NotifyMediaGalleryL() );

    // Clean up
    iFileTransferMode = EUpnpIdleTransferMode;

    // Leave if there was an error
    if( iStatusCode != KErrNone )
        {
        User::Leave( iStatusCode );
        }
    
    __LOG( "CUpnpFileTransferHandler::TransferRemoteItemsToHandsetL -end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::TransferRemoteContainerToHandsetL
// Copies a container from a remote Upnp Media Server to the handset.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::TransferRemoteContainerToHandsetL(
        TUpnpFileTransferMode aTransferMode,
        CUpnpContainer* aSourceContainer )
    {
    __LOG( "CUpnpFileTransferHandler::TransferRemoteContainerToHandsetL" );

    iStatusCode = KErrNone;

    if( !aSourceContainer ||
        aSourceContainer->Title() == KNullDesC8 ||
        ( aTransferMode != EUpnpCopyRemoteContainerToDefaultLocation &&
          aTransferMode != EUpnpCopyRemotePlaylistToDefaultLocation ) )
        {
        User::Leave( KErrArgument );
        }

    // Reset the playlist handler
    iPlaylistHandler->Reset();

    // Convert the container title to playlist name
    HBufC* titleUnicode = NULL;
    titleUnicode = UpnpString::ToUnicodeL( aSourceContainer->Title() );
    CleanupStack::PushL( titleUnicode );
    iPlaylistHandler->SetPlaylistNameL( *titleUnicode );
    CleanupStack::PopAndDestroy( titleUnicode );
    titleUnicode = NULL;

    // Initialise copy related member variables
    iCopyFromContainer = aSourceContainer;
    iFileTransferMode = aTransferMode;
    iContainerCopyFirstBrowse = ETrue;
    iContainerCopyBrowseIndex = 0;

    iBrowsingSession->SetObserver( *this );
    // Start the copy process
    TRAP( iStatusCode,
          iBrowsingSession->BrowseL(
                              iCopyFromContainer->Id(),
                              KBrowseFilter(),
                              MUPnPAVBrowsingSession::EDirectChildren,
                              iContainerCopyBrowseIndex,
                              KBrowseRequestCount,
                              KBrowseSortCriteria() ) );

    if( iStatusCode == KErrNone )
        {
        // Start the progress Dialog, but do not set current and 
        // final values for the progress bar. This info will be set
        // when the first browse result will be reseived.
        RunCopyNoteL();

        // If no files were copied, change the copy status code
        if( iPlaylistHandler->AudioItemCount() <= 0 &&
            iPlaylistHandler->ImageItemCount() <= 0 &&
            iPlaylistHandler->VideoItemCount() <= 0 &&
            iStatusCode == KErrNone )
            {
            iStatusCode = KErrNotFound;
            }

        // If files were copied, create playlists, albums and notify
        // Media Gallery
        else
            {
            // If audio files were copied, and the operation was to
            // copy a playlist, then create a MPX playlist
            if( iPlaylistHandler->AudioItemCount() > 0 &&
                ( aTransferMode ==
                        EUpnpCopyRemotePlaylistToDefaultLocation ) )
                {
                TRAP_IGNORE( iPlaylistHandler->CreateMusicPlaylistL() );
                }

            // If image files were copied, create an image album
            if( iPlaylistHandler->ImageItemCount() > 0 &&
                ( aTransferMode ==
                        EUpnpCopyRemoteContainerToDefaultLocation ) )
                {
                TRAP_IGNORE( iPlaylistHandler->CreateImageAlbumL() );
                }

            // If video files were copied, create a video album
            if( iPlaylistHandler->VideoItemCount() > 0 &&
                ( aTransferMode ==
                        EUpnpCopyRemoteContainerToDefaultLocation ) )
                {
                TRAP_IGNORE( iPlaylistHandler->CreateVideoAlbumL() );
                }
            }

        // Notify Media Gallery of filesystem changes, if files were
        // added.
        if( iPlaylistHandler->ImageItemCount() > 0 ||
            iPlaylistHandler->VideoItemCount() > 0 || 
            iPlaylistHandler->AudioItemCount() > 0 )
            {
            __LOG( "Notifying the Media Gallery of new \
image/video/music files." );
            TRAP_IGNORE( UpnpGalleryNotifier::NotifyMediaGalleryL() );
            }
        
        __LOG( "TransferRemoteContainerToHandsetL Reset");
        // Reset the playlist handler
        iPlaylistHandler->Reset();
        }

    // Set file transfer mode to idle
    iFileTransferMode = EUpnpIdleTransferMode;

    // If the copy failed, leave with the error code
    if( iStatusCode != KErrNone )
        {
        User::Leave( iStatusCode );
        }
    
    __LOG( "CUpnpFileTransferHandler::TransferRemoteContainerToHandsetL -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::CopyCompleteContainersL
// Handles the copy complete callback when copying containers.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::CopyCompleteContainersL(
        TInt aError,
        const TDesC& aFilepath )
    {
    __LOG( "CUpnpFileTransferHandler::CopyCompleteContainersL" );

    // Update the progress note
    iCurrentProgressValue++;
    if( iProgressInfo )
        {
        iProgressInfo->SetAndDraw( iCurrentProgressValue );
        }

    if( aError == KErrNone )
        {
        iNumCopiedItemFromContainer++;
        // Resolve the type of the media file
        TUPnPItemType itemType = ResolveFileType( aFilepath );

        // Add the file to playlist handler's lists
        if( itemType == ETypeAudio &&
            iCopyItem )
            {
            TRAP_IGNORE(
                iPlaylistHandler->NotifyNewAudioFileL(
                    aFilepath, *iCopyItem );
                iPlaylistHandler->AddAudioItemL( aFilepath ) );
            }
        else if( itemType == ETypeVideo )
            {
            TRAP_IGNORE( iPlaylistHandler->AddVideoItemL( aFilepath) );
            }
        else if( itemType == ETypeImage )
            {
            TRAP_IGNORE( iPlaylistHandler->AddImageItemL( aFilepath) );
            }
        else
            {
            // Nothing
            }
        }

    // Clean up the copy item
    delete iCopyItem;
    iCopyItem = NULL;

    // If the copy of the previous file failed with any error code, quit the
    // whole copy operation
    if( aError != KErrNone )
        {
        iStatusCode = aError;
        
        FinishNotes();
        return;
        }

    // If there are more objects (possibly items), browse for the next one,
    // otherwise finish the progress note.
    if( iContainerCopyBrowseIndex < iContainerCopyBrowseTotalCount )
        {
        iBrowsingSession->SetObserver( *this );
        TRAPD( browseError,
               iBrowsingSession->BrowseL(
                                  iCopyFromContainer->Id(),
                                  KBrowseFilter(),
                                  MUPnPAVBrowsingSession::EDirectChildren,
                                  iContainerCopyBrowseIndex,
                                  KBrowseRequestCount,
                                  KBrowseSortCriteria() ) );

        // If the browse could not be sent, finish the operation
        if( browseError != KErrNone && KErrServerBusy != browseError )
            {
            // Update the status code
            if( iStatusCode == KErrNone )
                {
                iStatusCode = browseError;
                }
            FinishNotes();
            }
            
        }
    else
        {
        FinishNotes();
        }
    
    __LOG( "CUpnpFileTransferHandler::CopyCompleteContainersL -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::CopyCompleteFilesL
// Handles the copy complete callback when copying files
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::CopyCompleteFilesL( 
        TInt aError,
        const TDesC& aFilepath )
    {
    __LOG1( "CUpnpFileTransferHandler::CopyCompleteFilesL %d", aError );

    if( aError == KErrNone )
        {
        iCurrentProgressValue = iCurrentProgressValue + iProgressIncrement;
        if( iProgressInfo )
            {
            iProgressInfo->SetAndDraw( iCurrentProgressValue );
            }
        
        // If a new file was copied to the handset, inform MPX
        TInt returnValue = KErrNone;
        if( iFileTransferMode == EUpnpCopyRemoteItemsToDefaultLocation )
            {
            if( (*iCopyItems)[iCopyPosition] )
                {
                // Resolve the type of the media file
                TUPnPItemType itemType = ResolveFileType( aFilepath );

                // Nofity MPX of a new file
                if( itemType == ETypeAudio )
                    {
                    TRAP_IGNORE( iPlaylistHandler->NotifyNewAudioFileL(
                        aFilepath, *(*iCopyItems)[iCopyPosition] ) );
                    }
                }

            // Delete the filename and remove from the array
            delete (*iCopyItems)[iCopyPosition];
            (*iCopyItems)[iCopyPosition] = NULL;
            
            iCopyItems->Remove( iCopyPosition );
            iCopyItems->Compress();
            
            if( iCopyPosition > iCopyItems->Count() ||
                iCopyPosition == iCopyItems->Count() )
                {
                __LOG( "All files copied. Exiting..." );

                // Finish the progress Dialog
                iStatusCode = aError;
                
                FinishNotes();
                }
            // If there are still more files to copy, increment the position
            // counter, and do the next copy    
            else
                {
                TRAP( returnValue, 
                      iDownloadSession->StartDownloadL( 
                                          (*(*iCopyItems)[iCopyPosition]),
                                          iCopyPosition ) );
                }
            }
            
        // In case of an error, quit copying
        if( returnValue != KErrNone )
            {
            // Update the status
            if( iStatusCode == KErrNone )
                {
                iStatusCode = returnValue;
                }
            // Finish the operation
            FinishNotes();
            }     
        } //if( aError == KErrNone )
    else
        {
        __LOG1("Unable to cont. copy: error %d!", aError );
        
        iStatusCode = aError;

        // Finish the progress Dialog
        FinishNotes();
        }
    
    __LOG( "CUpnpFileTransferHandler::CopyCompleteFilesL -end" );    
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::DialogDismissedL
// ProgressDialog call back method. Get's called when a Dialog is
// dismissed.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::DialogDismissedL( TInt aButtonId )
    {
    
    // if dialog is being dismissed, cancel every ongoing requrest
    __LOG( "CUpnpFileTransferHandler::DialogDismissedL" );
    __LOG( " User cancelled the copy operation." );

    iBrowsingSession->CancelBrowse();
    iDownloadSession->CancelAllTransfers();
    iDownloadSession->RemoveObserver();
        
    if( aButtonId == EAknSoftkeyCancel )
        {
        // Update the status code
        if( iStatusCode == KErrNone )
            {
            iStatusCode = KErrCancel;
            }
        }
    
    __LOG( "CUpnpFileTransferHandler::DialogDismissedL -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::MediaServerDisappeared
// Notifies that the Media Server we have a session with has disappeared. 
// Session is now unusable and must be closed.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "CUpnpFileTransferHandler::MediaServerDisappeared" );

    // Update the status code
    if( iStatusCode == KErrNone )
        {
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
        }

    // Finish wait/progress note
    FinishNotes();
    
    __LOG( "CUpnpFileTransferHandler::MediaServerDisappeared -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::BrowseResponse
// Returns processed browse results received from a Media Server.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::BrowseResponse( 
        const TDesC8& aBrowseResponse,
        TInt aError, 
        TInt /*aMatches*/, 
        TInt aTotalCount, 
        const TDesC8& /*aUpdateId*/ )
    {
    __LOG( "CUpnpFileTransferHandler::BrowseResponse" );

    if( iBrowsingSession &&
        iBrowsingSessionObserverBackup )
        {
        iBrowsingSession->SetObserver( *iBrowsingSessionObserverBackup );
        }
    CUPnPXMLParser* parser = NULL;
    TRAP( aError, parser = CUPnPXMLParser::NewL() );
    if( aError == KErrNone )
        {
        RPointerArray<CUpnpObject> array;
        

        TRAP( aError, parser->ParseResultDataL( array,
        aBrowseResponse ) );
        
        if( aError == KErrNone &&
            iStatusCode == KErrNone )
            {
            if( aTotalCount > 0 )
                {
                iContainerCopyBrowseTotalCount = aTotalCount;
                iContainerCopyBrowseIndex++;
                iCurrentProgressValue++;

                if( iProgressInfo )
                    {
                    // If this was the first browse in container copy
                    // operation, initialize the progress note, otherwise
                    // just update it
                    if( iContainerCopyFirstBrowse )
                        {
                        iContainerCopyFirstBrowse = EFalse;
                        iProgressInfo->SetFinalValue( ( aTotalCount * 
                            KProgressNoteStepper ) );
                        iProgressInfo->SetAndDraw( iCurrentProgressValue );
                        }
                    else
                        {
                        iProgressInfo->SetAndDraw( iCurrentProgressValue );
                        }
                    }

                // If the object was an item, create a copy of the item
                // object
                iStatusCode = KErrNone;
                if( array.Count() > 0 &&
                    array[0] &&
                    array[0]->ObjectType() == EUPnPItem )
                    {
                    // Store the item (the item's metadata is needed when the
                    // copy is complete)
                    delete iCopyItem;
                    iCopyItem = NULL;

                    TRAP( iStatusCode,
                          iCopyItem = CUpnpItem::NewL() );
                    if( iStatusCode == KErrNone )
                        {
                        TRAP( iStatusCode, iCopyItem->CopyL(
                            *(CUpnpItem*)array[0] ) );
                        }
                    }
                
                // Copy the item
                if( iStatusCode == KErrNone &&
                    iCopyItem &&
                    array.Count() > 0 &&
                    array[0] &&
                    array[0]->ObjectType() == EUPnPItem )
                    {
                    // Copy the next file
                    if( iFileTransferMode ==
                                EUpnpCopyRemoteContainerToDefaultLocation ||
                        iFileTransferMode ==
                                EUpnpCopyRemotePlaylistToDefaultLocation )
                        {
                        TRAP( iStatusCode, iDownloadSession->
                            StartDownloadL( (*(CUpnpItem*)array[0]), 0 ) );
                        }
                    else
                        {
                        // If the transfer mode is unknown, finish progress
                        // note
                        iStatusCode = KErrUnknown;
                        FinishNotes();
                        }

                    // If the copy could not be started, finish progress note
                    if( iStatusCode != KErrNone )
                        {
                        FinishNotes();
                        }
                    }
                // If the object was not an item, but there are still objects
                // left in the container, do a browse for the next object,
                // otherwise finish the progress note.
                else if( iContainerCopyBrowseIndex < 
                         iContainerCopyBrowseTotalCount )
                    {
                    // Update the progress bar ("CopyFile action part" of the 
                    // progress has to drawn here since the copy is not
                    // performed for container objects.
                    iCurrentProgressValue++;
                    if( iProgressInfo )
                        {
                        iProgressInfo->SetAndDraw( iCurrentProgressValue );
                        }

                    iBrowsingSession->SetObserver( *this );
                    
                    TRAP( iStatusCode,
                          iBrowsingSession->BrowseL(
                                    iCopyFromContainer->Id(),
                                    KBrowseFilter(),
                                    MUPnPAVBrowsingSession::EDirectChildren,
                                    iContainerCopyBrowseIndex,
                                    KBrowseRequestCount,
                                    KBrowseSortCriteria() ) );

                    // If the browse could not be sent, finish progress note
                    if( iStatusCode != KErrNone )
                        {
                        FinishNotes();
                        }

                    }
                // If the last object of the container was a container,
                // finish the progress note
                else
                    {
                    FinishNotes();
                    }
                }
            else
                {
                __LOG( "BrowseResponse, \
empty container, exit (-1)." );
                iStatusCode = KErrNotFound;
                FinishNotes();
                }
            }
        else //if( aError == KErrNone && iStatusCode == KErrNone )
            {
            if( iStatusCode == KErrNone )
                {
                iStatusCode = aError;
                }
            FinishNotes();
            __LOG1( "BrowseResponse, parsing failed, exit (%d).", aError );
            
            }
        
        array.ResetAndDestroy();
                
        delete parser;
        }
    else
        {
        __LOG1( "BrowseResponse,browse failed, exit (%d)." , aError );

        // Update the status code
        if( iStatusCode == KErrNone )
            {
            iStatusCode = aError;
            }

        // Finish the operation
        FinishNotes();
        }
    
    __LOG( "CUpnpFileTransferHandler::BrowseResponse -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::RunCopyNoteL
// Initialises and runs the progress note.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::RunCopyNoteL( TInt aCount )
    {
    __LOG( "CUpnpFileTransferHandler::RunCopyNoteL" );
    
    if( iProgressNoteDialog )
        {
        User::Leave( KErrInUse );
        }
    
    // If aCount is zero, creates "unitialized" dialog.
    if( aCount < 0 )
        {
        User::Leave( KErrArgument );
        }    
        
    // Creates dialog
    iProgressNoteDialog = new (ELeave) CAknProgressDialog(
            ( REINTERPRET_CAST( CEikDialog**, &iProgressNoteDialog ) ) );
    iProgressNoteDialog->PrepareLC( R_FT_COPY_PROGRESS_NOTE_DIALOG );
    iProgressNoteDialog->SetCallback( this );
    iProgressInfo = iProgressNoteDialog->GetProgressInfoL();
    
    if ( aCount )
        {
        // Incrementing dialog values.
        iProgressInfo->SetFinalValue( KProgressInfoFinalValue );
        iProgressIncrement = KProgressInfoFinalValue /
                         ( aCount * KProgressNoteStepper + 
                                 KProgressNoteStepper );
        iCurrentProgressValue = KProgressNoteStepper * iProgressIncrement;
        }
    else
        {
        // Uninitialized dialog values.
        iProgressInfo->SetFinalValue( KProgressInfoInitialValue );
        iProgressIncrement = 0;
        iCurrentProgressValue = 0;
        }
    
    iProgressInfo->SetAndDraw( iCurrentProgressValue );
    iProgressNoteDialog->RunLD();
    
    __LOG( "CUpnpFileTransferHandler::RunCopyNoteL -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::FinishNotes
// Finishes the progress and wait note if they are running.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::FinishNotes()
    {
    __LOG( "CUpnpFileTransferHandler::FinishNotes" );

    // If the progress note is running, finish it
    if( iProgressNoteDialog )
        {
        TRAP_IGNORE( iProgressNoteDialog->ProcessFinishedL() );
        // since ProcessFinishedL is called iProgressInfo is not 
        // valid anymore, set it to NULL
        iProgressInfo = NULL;
        }

    __LOG( "CUpnpFileTransferHandler::FinishNotes -end" );    
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::StartLocalMediaServerCompleted
// Notifies that the Media Server startup has completed.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferHandler::ReserveLocalMSServicesCompleted( TInt aError )
    {
    __LOG( "CUpnpFileTransferHandler::ReserveLocalMSServicesCompleted" );

    // If the connection is still good (we did not receive media server
    // disappear/WLAN disconnect while we were waiting), update the status
    // code.
    if( iStatusCode == KErrNone )
        {
        iStatusCode = aError;
        }

    __LOG( "CUpnpFileTransferHandler::ReserveLocalMSServicesCompleted -end" );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::ResolveFileType
// Resolves the media type of the given file.
// --------------------------------------------------------------------------
//
TUPnPItemType CUpnpFileTransferHandler::ResolveFileType(
        const TDesC& aFilepath )
    {
    __LOG( "CUpnpFileTransferHandler::ResolveFileType" );

    TUPnPItemType itemType = ETypeOther;
    TRAP_IGNORE( itemType = UPnPCommonUtils::ResolveFileTypeL( aFilepath ) );
    
    __LOG( "CUpnpFileTransferHandler::ResolveFileType -end" );
    
    return itemType;
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::ItemCopiedFromContainer
// Resolves the media type of the given file.
// --------------------------------------------------------------------------
//
TInt CUpnpFileTransferHandler::ItemCopiedFromContainer()
    {
    return iNumCopiedItemFromContainer;
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::TransferStarted
// --------------------------------------------------------------------------
// 
void CUpnpFileTransferHandler::TransferStarted( 
        TInt /*aKey*/,
        TInt aError )
    {
    __LOG1( "CUpnpFileTransferHandler::TransferStarted, %d", aError );

    // Just update the progress Dialog. No need for further error checks,
    // because in error situations UpnpAvController calls also CopyComplete,
    // and detailed error check is done there.
    if( aError == KErrNone &&
        iProgressInfo )
        {
        iCurrentProgressValue = iCurrentProgressValue + iProgressIncrement;
        iProgressInfo->SetAndDraw( iCurrentProgressValue );
        }
    __LOG( "CUpnpFileTransferHandler::TransferStarted -end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::TransferCompleted
// --------------------------------------------------------------------------
//    
void CUpnpFileTransferHandler::TransferCompleted( 
        TInt /*aKey*/,
        TInt aError,
        const TDesC& aFilepath )
    {
    __LOG1( "CUpnpFileTransferHandler::TransferCompleted, %d", aError );

    TInt copyError = KErrNone;
    
    if( iFileTransferMode == EUpnpCopyRemoteContainerToDefaultLocation ||
        iFileTransferMode == EUpnpCopyRemotePlaylistToDefaultLocation )
        {
        TRAP( copyError, CopyCompleteContainersL( aError, aFilepath ) );
        }
    else 
        {
        TRAP( copyError, CopyCompleteFilesL( aError, aFilepath ) );
        }
    if( copyError != KErrNone )
        {
        FinishNotes();
        }
    __LOG( "CUpnpFileTransferHandler::TransferCompleted -end");    
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferHandler::TransferProgress
// --------------------------------------------------------------------------
//    

void CUpnpFileTransferHandler::TransferProgress( 
        TInt /*aKey*/,
        TInt /*aBytes*/,
        TInt /*aTotalBytes*/ )
    {
    
    }

// End of file
