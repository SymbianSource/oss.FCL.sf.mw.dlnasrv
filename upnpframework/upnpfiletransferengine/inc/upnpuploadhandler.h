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
* Description:  Header file for the CUpnpUploadHandler class
*
*/


#ifndef UPNP_UPLOAD_HANDLER_H
#define UPNP_UPLOAD_HANDLER_H

// INCLUDES
#include "upnpfiletransfersessionobserver.h"
#include "upnpprogressdialogobserver.h"

// FORWARD DECLARATIONS

class MUPnPAVBrowsingSession;
class MUPnPFileUploadSession;
class MUPnPAVController;
class CUpnpNoteHandler;
class CEikonEnv;

//class CUpnpPlaylistHandler;
// CLASS DECLARATION

// CONSTANTS
enum TUpnpFileUploadMode
    {
    EUpnpCopy = 0,
    EUpnpMove
    };
    
/**
* CUpnpUploadHandler
* The class encapsulates all the file download functionality of
* UpnpFileTransferEngine.
*
* @since S60 3.2
*/
NONSHARABLE_CLASS( CUpnpUploadHandler ) :
                             private CAsyncOneShot,
                             private MUPnPFileTransferSessionObserver,
                             public MUPnPProgressDialogobserver
                             
                             
                             
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aBrowsingSession (MUPnPAVBrowsingSession*) pointer to the
         *        browsing session
         */
         static CUpnpUploadHandler* NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession );

        /**
         * Destructor.
         */
        virtual ~CUpnpUploadHandler();

    public: // Business logic methods

        /**
         * Upload the given objects to the remote mediaserver, 
         *
         * Leaves using e32err.h error codes if the operation fails.
         *
         * @since S60 3.2
         * @param aObjectIds (RPointerArray<TDesC16>*) list of object 
         *        paths to copy
         * @return none
         */
        void UploadItemsL( RPointerArray<TDesC16>* aObjectPaths, 
                           TUpnpFileUploadMode aFileUploadMode );
        
        /**
         * Upload the given playlist to the remote mediaserver, 
         *
         * Leaves using e32err.h error codes if the operation fails.
         *
         * @since S60 3.2
         * @param aObjectIds (RPointerArray<TDesC16>&) list of object 
         *        paths to copy
         * @return none
         */
        void UploadPlayListL( const TDesC& aPlaylistName,
                              RPointerArray<TDesC16>* aObjectPaths,
                              TUpnpFileUploadMode aFileUploadMode );
                              
    private: // Call back methods of MUPnPFileTransferSessionObserver
    
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
         * @return none
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
         * @return none
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
         * @return none
         */
        void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );                       

    public: // Call back methods of MUPnPProgressDialogobserver

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.2
         * @param aButtonId (TInt) ID of the button pressed
         * @return none
         */
        void DialogDismissedL();
    
    private: // from CAsyncOneShot
        
        /**
         * Asynchronous execution
         */
        void RunL();

        /**
         * Execution of the error branch
         */
        TInt RunError( TInt aError );
        
    private: // Construction methods

        /**
         * Constructor
         *
         * @since 3.2
         */
        CUpnpUploadHandler();

        /**
         * ConstructL.
         *
         * @since 3.2
         * @param aController (MUPnPAVBrowsingSession*) pointer to a valid
         *        UpnpAvController BrowsingsSession
         * @return none
         */
        void ConstructL( MUPnPAVBrowsingSession* aBrowsingSession );

    private:

        /**
         * Starts the next download. Leaves (KErrCompletion) if there are
         * no more objects to download.
         *
         * @since 3.2
         * @param none
         * @return none
         */
        void StartUploadL();
        
        
    private: // Data members

        
        /*
         * Browsing session handle. Not owned.
         */
        MUPnPAVBrowsingSession*         iBrowsingSession;

        /*
         * File upload session handle.
         */
        MUPnPFileUploadSession*         iUploadSession;

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
         * Indicate whether copy was cancelled after successfully copied 
         * the file but TransferComplete not yet called.
         */
        TBool iCopiedFileStillInArray;
        
        /*
         * Holds the copy completeness per cent value.
         */
        TInt iCopyCompleteness;

        /*
         * Pointer to the list of objects to download. owned.
         */
        RPointerArray<TDesC16>*          iObjectsToCopy;
        
        /**
         * The number of items has been uploaded
         */
        TInt                            iUploadItems;
        
        /*
         * Flag to set the progress bar maximum value
         */
        TBool                           iProgressBarMaxValueSet;
        
        /*
         * Total number of items to be uploaded
         */
        TInt                            iTotalCount;
        
        TUpnpFileUploadMode             iUploadMode;
        
        CEikonEnv*                      iCoeEnv;   // not owned
        
        TBool                           iUploadFirst;
        
        //number of items should be uploaded in a group, if the total number
        // is bigger than 20, iUploadIndex = 20 
        TInt                            iUploadIndex; 
        
        /*
         * Information if drm note is already shown.
         */
        TBool                           iDrmFilesSkipped;
        
        //CUpnpPlaylistHandler* iPlaylistHandler;
        
        
    };

#endif // UPNP_DOWNLOAD_HANDLER_H

// End of file
