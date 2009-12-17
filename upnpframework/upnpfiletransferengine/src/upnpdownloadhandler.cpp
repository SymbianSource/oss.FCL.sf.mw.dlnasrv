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
* Description:  Implementation of the CUpnpDownloadHandler class
*
*/


// INCLUDE FILES
// System
#include <bautils.h>                            // BaflUtils
#include <mmf/common/mmfcontrollerpluginresolver.h>

// upnp stack api
#include <upnpitem.h>                           // CUpnpItem
#include <upnpcontainer.h>                      // CUpnpContainer
#include <upnpstring.h>                         // UpnpString

// upnpframework / avcontroller api
#include "upnpavcontroller.h"                   // MUPnPAVController
#include "upnpavcontrollerfactory.h"            // UPnPAVControllerFactory
#include "upnpavbrowsingsession.h"              // MUPnPAVBrowsingSession
#include "upnpfiledownloadsession.h"            // MUPnPFileDownloadSession

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h"                   // upnp definitions

// upnpframework / xml parser api
#include "upnpxmlparser.h"                      // CUPnPXMLParser

// upnpframework / internal api's
#include "upnpcommonutils.h"                    // TUPnPItemType

// filetransferengine internal
#include "upnpdownloadhandler.h"
#include "upnpnotehandler.h"
#include "upnpplaylisthandler.h"                // CUpnpPlaylistHandler

_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KBrowseRequestCount =        1;
const TInt KZero =                      0;
const TInt KProgressBaseValue =         100;


// --------------------------------------------------------------------------
// CUpnpDownloadHandler::NewL
// NewL.
// --------------------------------------------------------------------------
//
CUpnpDownloadHandler* CUpnpDownloadHandler::NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[UpnpDownloadHandler] CUpnpDownloadHandler: NewL" );

    // Check that the browsing session is valid and has target device set.
    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    CUpnpDownloadHandler* self = NULL;
    self = new (ELeave) CUpnpDownloadHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aBrowsingSession );
    CleanupStack::Pop( self );
    __LOG( "[UpnpDownloadHandler] CUpnpDownloadHandler: NewL end" );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpDownloadHandler::CUpnpDownloadHandler()
    {
    __LOG( "[UpnpDownloadHandler] Constructor" );
    iContainerCopy = EFalse;
    iBrowsePosition = -1;
    iFirstBrowse = ETrue;

    iDownloadedCount = KZero;
    __LOG( "[UpnpDownloadHandler] Constructor -end" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpDownloadHandler::~CUpnpDownloadHandler()
    {
    __LOG( "[UpnpDownloadHandler] Destructor" );

    // If download session is running, stop it
    if( iAvController &&
        iDownloadSession )
        {
        iAvController->StopDownloadSession( *iDownloadSession );
        }

    // Delete the note handler.
    delete iNoteHandler;

    // delete the UpnpAvControllerClient
    delete iAvController;
    
    // delete the container id
    delete iContainerId;
    
    delete iPlaylistHandler;
    iCopyItems.ResetAndDestroy();
    iCopyItems.Close();
    //iCopyItem.ResetAndDestroy();
    // Restore the browse session observer
    if( iBrowsingSession &&
        iBrowsingSessionObserverBackup )
        {
        iBrowsingSession->RemoveObserver();
        iBrowsingSession->SetObserver( *iBrowsingSessionObserverBackup );
        }
    __LOG( "[UpnpDownloadHandler] Destructor -end" );    
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::ConstructL(
                                MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[UpnpDownloadHandler] ConstructL" );

    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    // Store the browsing session
    iBrowsingSession = aBrowsingSession;

    // Create UpnpAvControllerClient
    iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL();

    // Create download session
    iDownloadSession = &iAvController->StartDownloadSessionL(
                                            iBrowsingSession->Device() );

    // Set this object to be the download session observer
    iDownloadSession->SetObserver( *this );

    // Backup the browse session observer and set this object as an observer.
    iBrowsingSessionObserverBackup = iBrowsingSession->Observer();
    iBrowsingSession->RemoveObserver();
    iBrowsingSession->SetObserver( *this );

    iNoteHandler = CUpnpNoteHandler::NewL( this );
    
    iPlaylistHandler = CUpnpPlaylistHandler::NewL();
    __LOG( "[UpnpDownloadHandler] ConstructL -end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::DownloadItemsL
// Downloads the given objects to the handset, into the default target folder
// (setting in the Home Media application).
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::DownloadItemsL( CDesC8ArrayFlat* aObjectIds  )
    {
    
    __LOG( "[UpnpDownloadHandler] DownloadItemsL" );
    // Check parameter
    if( !aObjectIds ||
        aObjectIds->Count() <= KZero )
        {
        User::Leave( KErrArgument );
        }

    iTotalCount = aObjectIds->Count();
    iObjectsToCopy = aObjectIds;

    // Initialise values
    iCopyPosition = -1;
    iCopyCompleteness = KZero;

    iStatusCode = KErrNone;

    // Start fetching the object metadata
    StartDownload();

    if( iStatusCode == KErrNone )
        {
        iNoteHandler->RunProgressNoteL( EUpnpCopyProgressNote );
        }
        
    // Leave if there was an error
    if( iStatusCode != KErrNone )
        {
        iDownloadSession->CancelAllTransfers();
        User::Leave( iStatusCode );
        }
    else
        {
        if( iContainerCopy )
            {
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
                if( iPlaylistHandler->AudioItemCount() > 0 && iPlayList )
                    {
                    iPlaylistHandler->CreateMusicPlaylistL();
                    }

                // If image files were copied, create an image album
                if( iPlaylistHandler->ImageItemCount() > 0 )
                    {
                    iPlaylistHandler->CreateImageAlbumL();
                    }

                // If video files were copied, create a video album
                if( iPlaylistHandler->VideoItemCount() > 0 )
                    {
                    iPlaylistHandler->CreateVideoAlbumL();
                    }
                }

            // Reset the playlist handler
            iPlaylistHandler->Reset();
            }    
        }
    __LOG( "[UpnpDownloadHandler] DownloadItemsL -end" );    
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::DialogDismissedL
// ProgressDialog call back method. Get's called when a dialog is
// dismissed.
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::DialogDismissedL( )
    {
    __LOG( "[UpnpDownloadHandler] DialogDismissedL" );
    // Update the status code
    if( iStatusCode == KErrNone )
        {
        iStatusCode = KErrCancel;
        }
    iBrowsingSession->CancelBrowse();
    iDownloadSession->CancelAllTransfers();
    __LOG( "[UpnpDownloadHandler] DialogDismissedL -end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpDownloadHandler::MediaServerDisappeared
// Notifies that the Media Server we have a session with has disappeared. 
// Session is now unusable and must be closed.
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::MediaServerDisappeared(
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
    
    __LOG( "[UpnpDownloadHandler]\t MediaServerDisappeared() -end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::BrowseResponse
// Returns unprocessed browse results received from a Media Server.
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::BrowseResponse( const TDesC8& aBrowseResponse,
                                           TInt aError,
                                           TInt /*aMatches*/,
                                           TInt  aTotalCount,
                                           const TDesC8& /*aUpdateId*/ )
    {
    __LOG( "[UpnpDownloadHandler] BrowseResponse()" );
    
    if( aBrowseResponse != KNullDesC8 &&
        aError == KErrNone )
        {
        TRAP( aError, BrowseResponseL( aBrowseResponse, 
                                       aTotalCount ) );
        }
    else if( aBrowseResponse == KNullDesC8 && KErrNone!= aError )
        {
        aError = KErrArgument;
        }
    
    // If parsing or sending the download action failed, exit
    if( aError != KErrNone )
        {
        
        if( KErrCompletion == aError )
            {
            iStatusCode = KErrNone;
            }
        else
            {
            iStatusCode = aError;    
            }
        iNoteHandler->FinishProgressNote();
        }

    
    
    __LOG( "[UpnpDownloadHandler] BrowseResponse() -end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpDownloadHandler::BrowseResponseL
// Returns unprocessed browse results received from a Media Server.
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::BrowseResponseL( const TDesC8& aBrowseResponse,
                                            TInt aTotalCount )
                                           
    {
    __LOG( "[UpnpDownloadHandler] BrowseResponseL()" );

    //get the number of items in the container copy
    if( !iFirstBrowse && iContainerCopy )
        {
        iTotalCount = aTotalCount;
        if( !iProgressBarMaxValueSet )
            {
            iProgressBarMaxValueSet = ETrue;
            iNoteHandler->SetMaxValue( iTotalCount * KProgressBaseValue );
            }
        
        }
        // Parse the result
        
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    CUPnPXMLParser* parser = NULL;
    parser = CUPnPXMLParser::NewLC();        
    
    
    parser->ParseResultDataL( array, aBrowseResponse );
    CleanupStack::PopAndDestroy( parser );
    
    /**
     * Seldomly the aError is KErrNone, but no object in array
     * if this is for the container copying, return KErrArgument
     * otherwise, continue to browse the next item
     */
    if( array.Count() > 0 )
        {
        if( array[KZero]->ObjectType() == EUPnPItem )
            { 
            iFirstBrowse = EFalse;
            IsAnyItem = ETrue;
            
            if( !iProgressBarMaxValueSet )
                {
                iProgressBarMaxValueSet = ETrue;
                iNoteHandler->SetMaxValue( iTotalCount * KProgressBaseValue );
                }
            
            CUpnpItem* tempItem = NULL;
            tempItem = CUpnpItem::NewL();
            tempItem->CopyL( *array[KZero] );
             
            CleanupStack::PushL( tempItem );
             
            iCopyItems.AppendL( tempItem ); //transfer ownership
            CleanupStack::Pop( tempItem );
            
            // Start downloading the next object
            StartDownload();
            }
        else if( array[KZero]->ObjectType() == EUPnPContainer )
            {
            //To get the parent container Id
            if( iFirstBrowse )
                {                
                CUpnpContainer* sourseContainer = NULL;
                sourseContainer = ( CUpnpContainer* )array[KZero];
                iPlayList = UPnPCommonUtils::IsPlaylistContainerL( 
                                            *sourseContainer);
                        
                iPlaylistHandler->Reset();
                
                // Convert the container title to playlist name
                HBufC* titleUnicode = NULL;
                titleUnicode = UpnpString::ToUnicodeL( sourseContainer->Title() );
                CleanupStack::PushL( titleUnicode );
                iPlaylistHandler->SetPlaylistNameL( *titleUnicode );
                CleanupStack::PopAndDestroy( titleUnicode );
                
                iContainerCopy = ETrue;
                iBrowsePosition = -1; //reset the browes position
                iFirstBrowse = EFalse;
                delete iContainerId; iContainerId = NULL;
                iContainerId = array[KZero]->Id().AllocL();
                }
            }
        } //if( array[KZero] > 0 )
    else //if no items
        {
        if( iFirstBrowse )
            {
            User::LeaveIfError( KErrArgument );
            }
        }
    
    CleanupStack::PopAndDestroy( &array );    
    
    //try to start browsing next object
    if( iTotalCount > ++iBrowsePosition )
        {
        if( !iContainerCopy )
            {
            iBrowsingSession->BrowseL( iObjectsToCopy->MdcaPoint( 
                                       iBrowsePosition ),
                                       KFilterFull(),
                                       MUPnPAVBrowsingSession::EMetadata,
                                       iBrowsePosition,
                                       KBrowseRequestCount,
                                       KSortCriteria() );
            }
         else
            {
            // if we are copying a container, 
            // browse the next item
            if( iContainerId )
                {
                iBrowsingSession->BrowseL( 
                              *iContainerId,
                              KFilterFull(),
                              MUPnPAVBrowsingSession::EDirectChildren,
                              iBrowsePosition,
                              KBrowseRequestCount,
                              KSortCriteria() );
                }                                    
            }
        } //if( iTotalCount > ++iBrowsePosition )
    else //if there is no item in a container
        {
        if( iContainerCopy && !IsAnyItem )
            {
            iNoteHandler->FinishProgressNote();
            }
        }             
    __LOG( "[UpnpDownloadHandler] BrowseResponseL() -end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::TransferStarted
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::TransferStarted( TInt aKey,
                                            TInt aStatus )
    {
    __LOG( "[UpnpDownloadHandler] TransferStarted()" );

    // If the transfer failed to start, exit
    
    if( aStatus != KErrNone)
        {
        iStatusCode = aStatus;    
        }
    else if( aKey < 0 || aKey > iCopyPosition )
        {
        iStatusCode = KErrGeneral;
        }
    
    if( KErrServerBusy == iStatusCode )
        {
        iBrowsePosition--;
        iCopyPosition--;
        iStatusCode = KErrNone;
        }
        
    if( KErrNone != iStatusCode )
        {
        iNoteHandler->FinishProgressNote(); //this is sychro. call    
        }
    else
        {
        TRAP_IGNORE( iDownloadSession->StartTrackingProgressL( aKey ) );    
        }    
    
    __LOG( "[UpnpDownloadHandler] TransferStarted() -end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::TransferProgress
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::TransferProgress( TInt aKey,
                                             TInt aBytes,
                                             TInt aTotalBytes )
    {
    __LOG( "[UpnpDownloadHandler] TransferProgress()" );
    if( aKey < 0 || aKey > iCopyPosition )
        {
        iStatusCode = KErrGeneral;
        iNoteHandler->FinishProgressNote();
        }
    else
        {
        float progress = ((float)aBytes / aTotalBytes) * 100;
        iNoteHandler->SetValue( iCopyCompleteness + (TInt)progress);    
        }    
    
    __LOG( "[UpnpDownloadHandler] TransferProgress() -end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpDownloadHandler::TransferCompleted
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::TransferCompleted( TInt aKey,
                                              TInt aStatus,
                                              const TDesC& aFilePath )
    {
    __LOG( "[UpnpDownloadHandler] TransferCompleted()" );
    
    
    iStatusCode = aStatus;
    iDownloadedCount++;
    
    if( KErrNone != iStatusCode || ( aKey < 0 || aKey > iCopyPosition ) ||
        iTotalCount <= iDownloadedCount )
        {
        iNoteHandler->FinishProgressNote();
        }
    else    
        {
        // Update the download completeness percentage value
        iCopyCompleteness = iDownloadedCount * KProgressBaseValue;
            // Update the progress note
        iNoteHandler->SetValue( iCopyCompleteness );
        
        if( iCopyItems.Count() > aKey )
            {
            CUpnpItem* obj = NULL;
            obj = (CUpnpItem*)iCopyItems[aKey];
            if( obj )
                {
                TRAP_IGNORE( NotifyMPXL( *obj, aFilePath ) );        
            
                delete iCopyItems[aKey];
                iCopyItems[aKey] = NULL;
                }
            }
         
        }
    __LOG( "[UpnpDownloadHandler] TransferCompleted() -end" );    
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::NotifyMPXL
// Notify MPX when download is complete
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::NotifyMPXL( 
                        const CUpnpItem& aObject, const TDesC& aFilePath )
    {
    __LOG( "[UpnpDownloadHandler] NotifyMPXL()" );
    TUPnPItemType itemType = ETypeOther;
    itemType = UPnPCommonUtils::ResolveFileTypeL( aFilePath );
    
    // Nofity MPX of a new file
    if( itemType == ETypeAudio )
        {
        if( iContainerCopy )
            {
            iPlaylistHandler->AddAudioItemL( aFilePath );
            }
        iPlaylistHandler->NotifyNewAudioFileL( aFilePath, aObject );
        }
    
    if( iContainerCopy )
        {
        if( itemType == ETypeVideo )
            {
            iPlaylistHandler->AddVideoItemL( aFilePath);
            }
        else if( itemType == ETypeImage )
            {
            iPlaylistHandler->AddImageItemL( aFilePath);
            }
        }
    __LOG( "[UpnpDownloadHandler] NotifyMPXL() end" );    
    }


// --------------------------------------------------------------------------
// CUpnpDownloadHandler::StartDownload
// Start download a file
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::StartDownload()
    {
    __LOG( "[UpnpDownloadHandler] StartDownload()" );
    if( iStatusCode == KErrNone )
        {
        TRAP( iStatusCode, StartDownloadL() );    
        }
    
    
    // If the downloading of the next file failed (browse failed or there
    // are no more files), exit
    
    if( iStatusCode!= KErrNone )
        {
        if( iStatusCode == KErrCompletion )
            {
            iStatusCode = KErrNone;
            }
        iNoteHandler->FinishProgressNote();     
        }
    __LOG( "[UpnpDownloadHandler] StartDownload() end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::StartDownloadL
// Starts the next download. Leaves (KErrCompletion) if there are no more
// objects to download.
// --------------------------------------------------------------------------
//
void CUpnpDownloadHandler::StartDownloadL()
    {
    __LOG( "[UpnpDownloadHandler] StartDownloadL()" );

    if( !iFirstBrowse )
        {
        //start downloading
        // check if there are any CUpnpObject left in the array
        if( iCopyItems.Count() > ++iCopyPosition )
            {
            iDownloadSession->
                    StartDownloadL( *(CUpnpItem*)iCopyItems[iCopyPosition],
                                    iCopyPosition );
            }
        else
            {
            User::Leave( KErrCompletion );
            }
        }
    else //this is called only once at the beginning of copying
        {
        //start browsing
        iBrowsePosition++;

        iBrowsingSession->BrowseL( 
                        iObjectsToCopy->MdcaPoint( iBrowsePosition ),
                        KFilterFull(),
                        MUPnPAVBrowsingSession::EMetadata,
                        iBrowsePosition,
                        KBrowseRequestCount,
                        KSortCriteria() );
        
        }
    __LOG( "[UpnpDownloadHandler] StartDownloadL() -end" );
    }

// --------------------------------------------------------------------------
// CUpnpDownloadHandler::GetNumCopiedFiles
// --------------------------------------------------------------------------
//
TInt CUpnpDownloadHandler::GetNumCopiedFiles() const
    {
    return iDownloadedCount;
    }
// End of file
