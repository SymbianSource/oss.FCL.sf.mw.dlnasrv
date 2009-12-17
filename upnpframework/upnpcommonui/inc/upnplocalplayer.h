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
* Description:  Plays remote images,videos and music locally on device
*
*/


#ifndef C_CUPNPLOCALPLAYER_H
#define C_CUPNPLOCALPLAYER_H

#include <e32base.h>
#include <s32mem.h>
#include <f32file.h>
#include <AknServerApp.h>
#include <AknProgressDialog.h>
#include "upnpfiletransfersessionobserver.h"

// FORWARD DECLARATIONS

class MUPnPAVBrowsingSession;
class CUpnpObject;
class CDocumentHandler;
class CAknWaitDialog;
class CUPnPCommonUI;
class MUPnPAVController;
class MUPnPFileDownloadSession;
class MUPnPAVBrowsingSessionObserver;


// CLASS DECLARATION

/**
* CUPnPLocalPlayer
* The class works is used to play or show the remote item in
* local devices .
*
* @since S60 3.1
*/

class CUPnPLocalPlayer: public CBase,
                        public MAknServerAppExitObserver,
                        public MProgressDialogCallback,
                        public MUPnPFileTransferSessionObserver
    {
    public:  // Constructors and destructor

        IMPORT_C static CUPnPLocalPlayer* NewL(
                                    MUPnPAVController& aAVController,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI  );
        /**
        * Destructor.
        */
        virtual ~CUPnPLocalPlayer();

    public:  // new function

        /**
         * Play a given item
         *
         * @since Series 60 3.1
         * @param CUpnpObject given item to be played
         * @return none
         */
        IMPORT_C void PlayL( const CUpnpObject& aItem );

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
         * Notifies transfer progress, not implemented since the transfer
         * progress is not cared in the download and play/show case
         *
         * @since Series 60 3.2
         * @param aKey identifies the transfer
         * @param aBytes amount of bytes downloaded
         * @param aTotalBytes total amount of bytes
         */
        void TransferProgress( TInt /*aKey*/,
                               TInt /*aBytes*/,
                               TInt /*aTotalBytes*/ ){};

        /**
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.2
         * @return None
         */    
        void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );

    protected: // From MProgressDialogCallback

        /**
         * Callback function for progress bar.
         *
         * @since Series 60 3.1
         * @param aButtonId, id of the button pressed
         * @return None
         */
        void DialogDismissedL( TInt aButtonId );

    protected:  //from MAknServerAppExitObserver

         /**
         * Notifies that the image or video player has quit. 
         *
         * @since Series 60 3.1
         * @return aReason (TInt), the reason for quitting
         */    
        void HandleServerAppExit( TInt aReason );

    private:

        /**
        * Constructs the local player
        */
        CUPnPLocalPlayer( MUPnPAVController& aAVController,
                          MUPnPAVBrowsingSession& aBrowseSession,
                          CUPnPCommonUI& aCommonUI  );
       /**
        * Perform the second phase construction
        */
        void ConstructL(); 

    private:
        
        /**
         * Notifies that the copy operation is complete.
         *
         * @since Series 60 3.2
         * @param aStatus TInt status information
         * @param aFilepath filepath for a downloaded file (CopyToPhoneL)
         * @return None
         */
        void CopyCompleteL( TInt aError, 
                           const TDesC& aFilepath );
        /**
        * Start the current waiting note
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void StartWaitingNoteL();

        /**
        * Finish the current waiting note
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void FinishNote();

         /**
         * Removes those res-elements from the item that can't be used for 
         * local playback.
         * @since Series 60 3.1
         * @param CUpnpObject item to be checked
         * @return TBool is there any usable res element left.
         */
        TBool IsLocallySupportedL( CUpnpObject& aItem );

         
    private:
        
        MUPnPAVBrowsingSession*                     iBrowseSession; //not owned
        MUPnPAVBrowsingSessionObserver*             iBrowseSessionObserver;    
        CUpnpObject*                                iItem; //owned
        RFs                                         iFs; //owned         
        CDocumentHandler*                           iDocumentHandler;//owned
        CAknWaitDialog*                             iWaitNoteDialog; //owned
        HBufC*                                      iFilePath; //owned
        TInt                                        iExitReason;
        TBool                                       iWaitingNote;
        MUPnPFileDownloadSession*                   iDownloadSession;
        MUPnPAVController*                          iAVController; //not owned

        CUPnPCommonUI&                              iCommonUI; //Not owned
        };

    #endif // C_CUPNPLOCALPLAYER_H
