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
* Description:  Header file for the CUpnpNoteHandler class
*
*/


#ifndef UPNP_FILE_TRANSFER_NOTE_HANDLER_H
#define UPNP_FILE_TRANSFER_NOTE_HANDLER_H

// INCLUDES
#include <AknProgressDialog.h>

// FORWARD DECLARATIONS
class CAknProgressDialog;
class CEikProgressInfo;
class MUPnPProgressDialogobserver;
class CEikonEnv;

// ENUMS
enum TUpnpProgressNoteType
    {
    EUpnpCopyProgressNote = 0,
    EUpnpMoveProgressNote
    };

// CLASS DECLARATION

/**
* CUpnpNoteHandler
* The class provides means for the UpnpFileTransferEngine component to show
* the state of the file transfer process.
*
* @since S60 3.2
*/
NONSHARABLE_CLASS( CUpnpNoteHandler ) : public CBase,
                                        public MProgressDialogCallback
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
         static CUpnpNoteHandler* NewL( 
                                MUPnPProgressDialogobserver* aObserver );

        /**
         * Destructor.
         */
        virtual ~CUpnpNoteHandler();

    public: // Call back methods of MAknProgressDialogCallback

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.1
         * @param aButtonId (TInt) ID of the button pressed
         */
        void DialogDismissedL( TInt aButtonId );

    public: // private business methods

        /**
         * Initialises and runs the progress note.
         *
         * @since S60 3.2
         * @param aNoteType (TUpnpProgressNoteType) type of the note
         */
        void RunProgressNoteL( TUpnpProgressNoteType aNoteType );

        /**
         * Sets the value of the progress note.
         *
         * @since S60 3.2
         * @param aProgressValue (TInt) percentage value for the note
         */
        void SetValue( TInt aProgressValue );

        /**
         * Finishes the progress note.
         *
         * @since S60 3.2
         */
        void FinishProgressNote();
        
        void SetMaxValue( TInt aMaxValue );
        
        /**
         * Skipping DRM protected files - note
         *
         * @since S60 3.2.3
         */        
        void ShowSkippingDRMFilesNoteL();
        

    private: // Construction methods (private)

        // Constructor
        CUpnpNoteHandler( MUPnPProgressDialogobserver* aObserver );

        /**
         * ConstructL.
         */
        void ConstructL();

    private: // Data members

        // CoeEnv and the resource offset (needed when loading and
        // unloading resources)
        TInt iResFileOffset;

        // Progess note dialog and progress info
        CAknProgressDialog* iProgressNoteDialog;        // owned
        CEikProgressInfo* iProgressInfo;                // not owned
        TInt iCurrentProgressValue;
        MUPnPProgressDialogobserver*                iProgressDialogObserver;
        
        TInt                                        iMaxProgressValue;
        CEikonEnv* iCoeEnv;                             // not owned
        
        /**
         * Flag progress if is finished. 
         */
        TBool                           iProgressNoteFinish;

    };

#endif // UPNP_FILE_TRANSFER_NOTE_HANDLER_H

// End of file
