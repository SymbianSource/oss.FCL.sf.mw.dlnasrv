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
* Description:  Header file for the CUpnpNoteHandler class.
*
*/


#ifndef UPNP_COMMAND_NOTE_HANDLER_TASK_H
#define UPNP_COMMAND_NOTE_HANDLER_TASK_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;

/**
* This class does all UI note handling of UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpNoteHandler : public CBase
    {
    public: // Construction methods

        /**
         * Creates a new CUpnpNoteHandler instance.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpNoteHandler
         */
        static CUpnpNoteHandler* NewL();

    public: // Business logic methods

        /**
         * Run the "Connecting" wait note.
         *
         * @since Series 60 3.2
         * @return TInt return value
         */
        TInt RunConnectingWaitNote();

        /**
         * Closes the currently displaying wait note.
         *
         * @since Series 60 3.2
         */
        void CloseWaitNote();

        /**
         * Show "DRM protected files not supported." info note.
         *
         * @since Series 60 3.2
         */
        void ShowDrmNoteL();

        /**
         * Show "Connection lost." info note.
         *
         * @since Series 60 3.2
         */
        void ShowConnectionLostNoteL();
        
        /**
         * Show info note when local MS fails to start
         * 
         * @since Series 60 3.2
         */
        void ShowLocalMSStartErrorNoteL();
        
        /**
         * Show "N File copied." info note.
         *
         * @since Series 60 3.2
         */
        void ShowCopyInfoNoteL( TInt aCount, const TDesC& aServerName );
        
        /**
         * Show "N Files moved." info note.
         *
         * @since Series 60 3.2
         */
        void ShowMoveInfoNoteL( TInt aCount, const TDesC& aServerName );
        
        /**
         * Show "Copy failed." info note.
         *
         * @since Series 60 3.2
         */
        void ShowCopyErrorNoteL();
        
        /**
         * Show "Move failed" info note.
         *
         * @since Series 60 3.2
         */
        void ShowMoveErrorNoteL();

        /**
         * Show "Connection lost" info note.
         *
         * @since Series 60 3.2
         */
        void ShowCopyDisconnectionErrorNoteL();
        
        /**
         * Show "Connection lost" info note.
         *
         * @since Series 60 3.2
         */
        void ShowMoveDisconnectionErrorNoteL();

        /**
         * Show "Playback failed" info note.
         *
         * @since Series 60 3.2
         */
        void ShowPlaybackFailedNoteL();

        /**
         * Reset iDrmNoteShown
         *
         * @since Series 60 3.2
         */
        void ResetDrmNoteCount();

    public: // Public destructor

        /**
         * Destructor.
         */
        virtual ~CUpnpNoteHandler();
        
    private: // Businnes l
    
        /**
         * Runs general wait note.
         *
         * @since Series 60 3.2
         * @return TInt return value
         */
        TInt RunWaitNote( TInt aResource );
        
        /**
         * Show error note
         *
         * @since Series 60 3.2
         */
        void ShowErrorNoteL( TInt aResource );

        /**
         * Show info note
         *
         * @since Series 60 3.2
         */
        void ShowInfoNoteL( const TDesC& aText );

    private: // Private construct/destruct methods

        /**
         * Perform the first phase of two phase construction.
         *
         * @since S60 3.2
         */
        CUpnpNoteHandler();

        /**
         * Perform the second phase of two phase construction. Reserves the
         * Upnp Fw resources (they are released when the task is destroyed).
         */
        void ConstructL();
        
    private: // Data members
    
        /**
         * Currently showing wait note. Owned.
         */
        CAknWaitDialog* iWaitNote;

        /**
         * The resource file offset.
         */
        TInt iResFileOffset;
        
        /**
         * A boolean value indicating the state of the DRM note showing.
         * The note is shown only once per session.
         */
        TBool iDrmNoteShown;

    };

#endif // UPNP_COMMAND_NOTE_HANDLER_TASK_H

// End of File