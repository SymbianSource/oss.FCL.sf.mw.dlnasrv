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
* Description:  Header file for the CUpnpDownloadHandler class
*
*/


#ifndef UPNP_DOWNLOAD_HANDLER_H
#define UPNP_DOWNLOAD_HANDLER_H

// INCLUDES
// upnpframework / avcotnroller api
#include "upnpavbrowsingsessionobserver.h"
#include "upnpavsessionobserverbase.h"
#include "upnpfiletransfersessionobserver.h"

// filetransferengine internal
#include "upnpprogressdialogobserver.h"


// FORWARD DECLARATIONS
class CUpnpObject;
class MUPnPAVBrowsingSession;
class MUPnPFileDownloadSession;
class MUPnPAVController;
class CUpnpNoteHandler;
class CUpnpPlaylistHandler;
// CLASS DECLARATION

/**
* CUpnpDownloadHandler
* The class encapsulates all the file download functionality of
* UpnpFileTransferEngine.
*
* @since S60 3.2
*/
NONSHARABLE_CLASS( CUpnpDownloadHandler ) :
                             public CBase,
                             public MUPnPFileTransferSessionObserver,
                             public MUPnPAVBrowsingSessionObserver,
                             public MUPnPProgressDialogobserver
    {
    
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aBrowsingSession (MUPnPAVBrowsingSession*) pointer to the
         *        browsing session
         */
         static CUpnpDownloadHandler* NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession );

        /**
         * Destructor.
         */
        virtual ~CUpnpDownloadHandler();

    public: // Business logic methods

        /**
         * Downloads the given objects to the handset, into the default 
         * target folder (setting in the Home Media application).
         *
         * Leaves using e32err.h error codes if the operation fails.
         *
         * @since S60 3.2
         * @param aObjectIds (CDesC8ArrayFlat*) list of object IDs to copy
         */
        void DownloadItemsL( CDesC8ArrayFlat* aObjectIds );
        
        /**
         * Return the number of copied files
         *
         *
         * @since S60 3.2
         * 
         */
        TInt GetNumCopiedFiles() const;
    public: // Call back methods of MUPnPFileTransferSessionObserver

        /**
         * Notifies that the transfer has been started
         *
         * @since Series 60 3.2
         * @param aKey identifies the transfer
         * @param aStatus status (error) code
         * @return none
         */
        void TransferStarted( TInt aKey,
                              TInt aStatus );
    
        /**
         * Notifies that the transfer has been completed
         *
         * @since Series 60 3.2
         * @param aKey identifies the transfer
         * @param aStatus status (error) code
         * @param aFilePath 
         */ 
        void TransferCompleted( TInt aKey,
                                TInt aStatus,
                                const TDesC& aFilePath );
        
         /**
         * Notifies transfer progress
         *
         * @since Series 60 3.2
         * @param aKey identifies the transfer
         * @param aBytes amount of bytes downloaded
         * @param aTotalBytes total amount of bytes
         */
        void TransferProgress( TInt aKey,
                               TInt aBytes,
                               TInt aTotalBytes );
                               
        /**
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since S60 3.2
         * @param aReason reason code
         */
        void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );                       

    public: // Call back methods of MUPnPAVBrowsingSessionObserver

        /**
         * Returns a browse result received from a Media Server.
         *         
         * @since Series 60 3.2
         * @param aBrowseResponse browse response xml document
         * @param aError system wide error code
         * @param aMatches number of returned items
         * @param aTotalCount total number of objects on container
         * @param aUpdateId update id number
         * @return None
         */
        void BrowseResponse(
                        const TDesC8& aBrowseResponse,
                        TInt aError,
                        TInt aMatches,
                        TInt aTotalCount,
                        const TDesC8& aUpdateId
                        );

        // Unused methods
        void SearchResponse( const TDesC8& /*aSearchResponse*/,
                             TInt /*aError*/,
                             TInt /*aMatches*/,
                             TInt /*aTotalCount*/,
                             const TDesC8& /*aUpdateId*/ ) {}

        // Unused methods                             
        void SearchCapabilitiesResponse(
                             TInt /*aError*/,
                             const TDesC8& /*aSearchCapabilities*/ ) {}
        
        // Unused methods
        void CreateContainerResponse(
                            TInt /*aError*/,
                            const TDesC8& /*aObjectId*/ ) {}
        
        // Unused methods
        void DeleteObjectResponse( TInt /*aError*/ ) {}

        

    public: // From MUPnPAVSessionObserverBase

        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ) {}
        
    public: // Call back methods of MUPnPProgressDialogobserver

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.2
         * @param aButtonId (TInt) ID of the button pressed
         */
        void DialogDismissedL();

    private: // Construction methods

        /**
         * Constructor
         *
         * @since 3.2
         */
        CUpnpDownloadHandler();

        /**
         * ConstructL.
         *
         * @since 3.2
         * @param aController (MUPnPAVBrowsingSession*) pointer to a valid
         *        UpnpAvController BrowsingsSession
         */
        void ConstructL( MUPnPAVBrowsingSession* aBrowsingSession );

    private:

        /**
         * Starts the next download. Leaves (KErrCompletion) if there are
         * no more objects to download.
         *
         * @since 3.2
         */
        void StartDownloadL();
        
        /**
         * Starts the next download. 
         *
         * @since 3.2
         */
        void StartDownload();
        
        /**
         * Returns a browse result received from a Media Server.
         *         
         * @since Series 60 3.2
         * @param aBrowseResponse browse response xml document
         * @param aTotalCount total number of objects on container
         * @return None
         */
        void BrowseResponseL( const TDesC8& aBrowseResponse,
                              TInt  aTotalCount );
        
        /**
         * Notify MPX when the download is complete
         * @since Series 60 3.2
         * @param (CUpnpItem&) aObject downloaded item
         * @param (TDesC&) aFilePath downloaded item file path
         */
        void NotifyMPXL( const CUpnpItem& aObject, 
                         const TDesC& aFilePath );
    private: // Data members

        /*
         * Browsing session handle. Not owned.
         */
        MUPnPAVBrowsingSession*         iBrowsingSession;

        /*
         * Backup of the client's browsing session handle. Not owned.
         */
        MUPnPAVBrowsingSessionObserver* iBrowsingSessionObserverBackup;

        /*
         * File download session handle.
         */
        MUPnPFileDownloadSession*       iDownloadSession;

        /*
         * UpnpAvControllerClient. Owned.
         */
        MUPnPAVController*              iAvController;

        /*
         * Pointer to the UpnpNoteHandler. Owned.
         */
        CUpnpNoteHandler*               iNoteHandler;

        /*
         * Holds the status code of the transfer operation.
         */
        TInt iStatusCode;

        /*
         * Holds the position information.
         */
        TInt iCopyPosition;

        /*
         * Holds the copy completeness per cent value.
         */
        TInt iCopyCompleteness;

        /*
         * Pointer to the list of objects to download. Not owned.
         */
        CDesC8ArrayFlat*                iObjectsToCopy;
        
        /*
         * Position of items to be browsed.
         */
        TInt                            iBrowsePosition;
        
        /*
         * Is it container or items download.
         */
        TBool                           iContainerCopy;
        
        /*
         * Is it normal container or playlist download
         */
        TBool                           iPlayList;
        
        /*
         * Is it the first browse when start downloading
         */
        TBool                           iFirstBrowse;
        
        /*
         * Pointer to the container id. owned.
         */
        HBufC8*                         iContainerId; //owned
        
        /*
         * Total number of items to be downloaded
         */
        TInt                            iTotalCount;
        
        /*
         * Number of items has be downloaded
         */
        TInt                            iDownloadedCount;
        
        /**
         * We need to store the items after the browse response
         * to perform mutiple downloads simultaneously.
         */
        RPointerArray<CUpnpObject>      iCopyItems; //owned
        
        /*
         * Playlist handler. owned
         */
        CUpnpPlaylistHandler*           iPlaylistHandler; 
        
        /*
         * Is there any item in copying container case
         */
        TBool                           IsAnyItem;
        
        /*
         * Flag to set the progress bar maximum value
         */
        TBool                           iProgressBarMaxValueSet;
        
    };

#endif // UPNP_DOWNLOAD_HANDLER_H

// End of file
