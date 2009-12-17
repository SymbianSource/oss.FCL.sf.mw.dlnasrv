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
* Description:  Header file for the CUpnpFileTransferHandler class
*
*/


#ifndef UPNP_FILE_TRANSFER_HANDLER_H
#define UPNP_FILE_TRANSFER_HANDLER_H

// INCLUDES
// System
#include <AknProgressDialog.h>

// upnpavcotnroller api
#include "upnpavbrowsingsessionobserver.h"
#include "upnpfiletransfersessionobserver.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"                    // TUPnPItemType

// filetransferengine internal
#include "upnpplaylisthandler.h"

// CONSTANTS
enum TUpnpFileTransferMode
    {
    EUpnpIdleTransferMode = 0,
    EUpnpCopyRemoteItemsToDefaultLocation,
    EUpnpCopyRemoteContainerToDefaultLocation,
    EUpnpCopyRemotePlaylistToDefaultLocation
    };

enum TUpnpAlbumType
    {
    EUpnpFileCopyImageAlbum = 0,
    EUpnpFileCopyVideoAlbum
    };

// FORWARD DECLARATIONS
class CEikonEnv;
class MUPnPFileDownloadSession;

// CLASS DECLARATION

/**
* CUpnpFileTransferHandler
* The class works as an engine component, providing file transfer features
* of the UpnpFileTransferEngine component.
*
* @since S60 3.1
*/
NONSHARABLE_CLASS( CUpnpFileTransferHandler ) :
                                 public CBase,
                                 public MUPnPAVBrowsingSessionObserver,
                                 public MUPnPFileTransferSessionObserver,
                                 public MProgressDialogCallback
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aBrowsingSession (MUPnPAVBrowsingSession*) pointer to the
         *        browsing session
         */
         static CUpnpFileTransferHandler* NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession );

        /**
         * Destructor.
         */
        virtual ~CUpnpFileTransferHandler();

    public: // Business logic methods

        /**
         * Transfers items from a remote Upnp Media Server to the handset.
         * Removes files from the array which are succesfully transferred.
         *
         * @since S60 3.1
         * @param aTransferMode (TUpnpFileTransferMode) the transfer mode
         * @param aItems (RPointerArray<CUpnpItem>&) reference to the array
         *       of CUpnpItems, the items to copy
         */
        void TransferRemoteItemsToHandsetL(
                            TUpnpFileTransferMode aTransferMode,
                            RPointerArray<CUpnpItem>& aItems );

        /**
         * Transfers a container/playlist from a remote Upnp Media Server to
         * the handset.
         *
         * @since S60 3.1
         * @param aTransferMode (TUpnpFileTransferMode) the transfer mode
         * @param aContainer (CUpnpContainer*) pointer to the source Upnp
         *        Container object
         */
        void TransferRemoteContainerToHandsetL(
                            TUpnpFileTransferMode aTransferMode,
                            CUpnpContainer* aSourceContainer );

        /**
         * Return the number of items copied from a container
         *
         * @since S60 3.1
         * @param none
         * @return TInt number of items copied
         */
        TInt ItemCopiedFromContainer();

    public: // Call back methods of MAknProgressDialogCallback

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.1
         * @param aButtonId (TInt) ID of the button pressed
         */
        void DialogDismissedL( TInt aButtonId );

    public: // Call back methods of MUPnPAVBrowsingSessionObserver

        /**
         * Returns a browse result received from a Media Server.
         *         
         * @since Series 60 3.1
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

        /**
         * Returns a search result received from a Media Server.
         *
         * @param aSearchResponse search response xml document
         * @param aError system wide error code
         * @param aMatches number of returned items
         * @param aTotalCount total number of resulted items
         * @param aUpdateId update id number
         * @return None
         */
        void SearchResponse( 
                        const TDesC8& /*aSearchResponse*/,
                        TInt /*aError*/,
                        TInt /*aMatches*/,
                        TInt /*aTotalCount*/,
                        const TDesC8& /*aUpdateId*/
                        ){}

        /**
         * Returns search capabilities of the requested Media Server.
         *
         * @since S60 3.1
         * @param aError Status information
         * @param aSearchCapabilities Reference to the search capabilities
         *        string in HBufC8 format.
         */
        void SearchCapabilitiesResponse( TInt /*aError*/,
                                         const TDesC8& /*aSearchCapabilities*/ )
                                         {}
        

        
        /**
         * Notifies that the create container operation is complete.
         *
         * @since S60 3.1
         * @param aError Status information
         * @param aObjectId (const TDesC8&) object ID of the new container
         */
        void CreateContainerResponse( TInt /*aError*/,
                                      const TDesC8& /*aObjectId = KNullDesC8*/ )
                                      {}

        /**
         * Notifies that the requested Upnp Object deletion is complete.
         *
         * @since S60 3.1
         * @param aError Status information
         */
        void DeleteObjectResponse( TInt /*aError*/ ){}

        /**
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since S60 3.1
         * @param aReason reason code
         */
        void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );

         /**
         * Notifies that the Media Server startup has completed. 
         *
         * @since S60 3.1
         * @return aError (TInt), error code
         */
        void ReserveLocalMSServicesCompleted( TInt aError );

    public: // Call back methods of MUPnPFileTransferSessionObserver
        
        /**
         * Notifies that the transfer has been started
         *
         * @since S60 3.1
         * @param aKey identifies the transfer
         * @param aStatus status (error) code
         */ 
        void TransferStarted( TInt aKey,
                              TInt aStatus );
        /**
         * Notifies that the transfer has been completed
         *
         * @since S60 3.1
         * @param aKey identifies the transfer
         * @param aStatus status (error) code
         * @param aFilePath 
         */  
        void TransferCompleted( TInt aKey,
                                TInt aStatus,
                                const TDesC& aFilePath );
    
        /**
         * Notifies transfer progress
         * NOTE: Transfer progress is not guaranteed
         *
         * @since S60 3.1
         * @param aKey identifies the transfer
         * @param aBytes amount of bytes downloaded
         * @param aTotalBytes total amount of bytes
         */ 
        void TransferProgress( TInt aKey,
                               TInt aBytes,
                               TInt aTotalBytes );

    private: // private business methods

        /**
         * Handles the copy complete callback when copying files
         *
         * @since S60 3.1
         * @param aError (TInt) Status information
         * @param aFilepath (const TDesC&) filepath for a downloaded file
         */
        void CopyCompleteFilesL( TInt aError, 
                                 const TDesC& aFilepath );

        /**
         * Handles the copy complete callback when copying containers
         *
         * @since S60 3.1
         * @param aError (TInt) Status information
         * @param aFilepath (const TDesC&) filepath for a downloaded file
         */
        void CopyCompleteContainersL( TInt aError, 
                                      const TDesC& aFilepath );

        /**
         * Initialises and runs the progress note.
         *
         * @since S60 5.1
         * @param aCount A Count used for initialized increment values of dialog.
         *        Zero creates dialog as uninitialized.
         */
        void RunCopyNoteL( TInt aCount = 0 );
        
        /**
         * Finishes the progress and wait note if they are running.
         *
         * @since S60 3.1
         */
        void FinishNotes();

        /**
         * Resolves the media type of the given file.
         *
         * @since S60 3.1
         * @param aFilepath (const TDesC&) filename
         * @return TUPnpItemType the type of the item
         */
        TUPnPItemType ResolveFileType( const TDesC& aFilepath );

    private: // Construction methods (private)

        // Constructor
        CUpnpFileTransferHandler();

        /**
         * ConstructL.
         * @param aController (MUPnPAVBrowsingSession*) pointer to a valid
         *        UpnpAvController BrowsingsSession
         */
        void ConstructL( MUPnPAVBrowsingSession* aBrowsingSession );

    private: // Data members

        // CoeEnv and the resource offset (needed when loading and
        // unloading resources)
        CEikonEnv* iCoeEnv;                             // not owned
        TInt iResFileOffset;

        // Browsing session handles (not owned)
        MUPnPAVBrowsingSession* iBrowsingSession;
        MUPnPAVBrowsingSessionObserver *iBrowsingSessionObserverBackup;

        // Progess note dialog and progress info
        CAknProgressDialog* iProgressNoteDialog;        // owned
        CEikProgressInfo* iProgressInfo;                // not owned
        TInt iCurrentProgressValue;
        TInt iProgressIncrement;

        TUpnpFileTransferMode iFileTransferMode;
        TInt iCopyPosition;

        // Status code to store the operation status information
        TInt iStatusCode;

        //used to copy from remote to local
        RPointerArray<CUpnpItem>* iCopyItems;           // owned
        CUpnpItem* iCopyItem;                           // owned

        // Container copy
        TBool iContainerCopyFirstBrowse;
        TInt iContainerCopyBrowseIndex;
        TInt iContainerCopyBrowseTotalCount;
        CUpnpContainer* iCopyFromContainer;             // not owned

        // Playlist handler
        CUpnpPlaylistHandler* iPlaylistHandler;         // owned

        //number of items copied from a container
        TInt iNumCopiedItemFromContainer;

        /*
         * UpnpAvControllerClient. Owned.
         */
        MUPnPAVController*              iAvController;
        
        /*
         * File download session handle.
         */
        MUPnPFileDownloadSession*       iDownloadSession;
        
    };

#endif // UPNP_FILE_TRANSFER_HANDLER_H

// End of file
