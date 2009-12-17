/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      File sharing dialog class definition
*
*/






#ifndef UPNPAPPFILESHARINGDIALOG_H
#define UPNPAPPFILESHARINGDIALOG_H

//  INCLUDES
#include <AknDialog.h> // base class
#include "upnpfilesharingengineobserver.h" // base class

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;
class CEikMenuPane;
class CUPnPAppFileSharingList;
class CAknTitlePane;
class CEikStatusPane;

// CLASS DECLARATION

/**
*  File Sharing dialog class.
*  Declares file sharing dialog
*  @since S60 3.1
*/
class CUPnPAppFileSharingDialog
    : public CAknDialog
    , private MUPnPFileSharingEngineObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUPnPAppFileSharingDialog* NewL(
            TInt aMenuResource,
            CUPnPFileSharingEngine& aEng );

        /**
        * Destructor.
        */
        virtual ~CUPnPAppFileSharingDialog();

    public: // Services

        /**
         * Executes the dialog.
         */
        IMPORT_C TInt ExecuteSharingDialogLD();

        /**
         * Get the pointer of iFileSharingList and update list
         */    
        IMPORT_C void UpdateFileSharingListL() ;
     
    protected: // Functions from base classes

        /**
        * From CAknDialog, handles menu commands
        * @since S60 3.1
        * @param aCommandId, command to be handled
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CAknDialog, handles key events.
        * @since S60 3.1
        * @param aKeyEvent, Event to handled.
        * @param aType, Type of the key event.
        * @return TKeyResponse, response code
        *           (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                    TEventCode aType );

        /**
        * From CoeControl
        * @since S60 3.1
        * @param aContext, context of desired help
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    protected: // for own use

        /**
        * Opens album selection dialog and starts sharing
        * @since S60 3.1
        */
        void RunMultiselectionDialogL();

    private: //From MUPnPFileSharingEngineObserver
        /**
         * Function informs when file sharing was enabled or disabled
         * @since S60 3.1
         * @param aEngine File sharing engine
         * @param aError, error code
         * @param aPhase ETrue if current phase is sharing activation,
         * otherwise EFalse
         */
        void HandleSharingStatus( 
            CUPnPFileSharingEngine& aEngine, 
            TInt aError, 
            TBool aPhase );

        /**
         * Informs about status of the sharing progress
         * @since S60 3.1
         * @param aEngine File sharing engine
         * @param aProgress Data information about sharing progress
         */                                          
        void HandleSharingProgress(
            CUPnPFileSharingEngine& aEngine, 
            const RArray<TUpnpProgressInfo>& aProgress );

        /**
         * Function informs when file sharing is done
         * @param aEngine File sharing engine
         * @param aError, error code
         */
        void HandleSharingDone( 
            CUPnPFileSharingEngine& aEngine, TInt aError );

        /**
         * Callback to inform if connection has been lost
         * @since S60 3.1
         * @param aEngine File sharing engine
         */ 
        void HandleSharingConnectionLost( 
            CUPnPFileSharingEngine& aEngine );

    protected:  // Functions from base classes

        /**
         * From CAknDialog, This is called in CAknDialog::ExecuteLD()
         *                  before a form is drawn.
         * @since S60 3.1
         */
        void PreLayoutDynInitL();

        /**
        * From CAknDialog : This is called by the dialog framework
        *                   when softkey is pressed, returns true if the
        * @since S60 3.1
        * @param aButtonId, ID of the pressed button
        * @return TBool, returns ETrue if ready to close dialog,
        *                   EFalse otherwise.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From MEikMenuObserver
        * @since S60 3.1
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since S60 3.1
        * @param aMenuResource, menu resource of the dialog
        */
        void ConstructL(TInt aMenuResource);

        /**
        * Creates custom control
        */
        SEikControlInfo CreateCustomControlL(TInt aControlType);

        /**
         * Display information note after starting sharing
         * @since S60 3.1
         * @param aMessageResourceId Resource id for message text
         * @param aDialogResourceId Resource id for the dialog
         */
        void DisplayInfoL( TInt aHeaderResourceId,
                           TInt aMessageResourceId,
                           TInt aDialogResourceId );
        /**
         * Default constructor
         */
        CUPnPAppFileSharingDialog( CUPnPFileSharingEngine& aEng );

    private:    // Data

        // File sharing engine pointer
        CUPnPFileSharingEngine&         iFileSharingEngine; //not owned

        // Pointer to file sharing list for dialog data
        CUPnPAppFileSharingList*        iFileSharingList;   //owned
        // pointer to title pane
        CAknTitlePane*                  iTitlePane;         // not owned
        // pointer to status pane
        CEikStatusPane*                 iStatusPane;        // not owned

        // UI environment
        CEikonEnv*                      iCoeEnv;            //not owned
        // offset in resource file
        TInt                            iResFileOffset;
        /**
         *Flag to keep in mind has FeatureManager been initialized or not
         */
        TBool iFeatureManagerInitialized;

    };

#endif      // UPNPAPPFILESHARINGDIALOG_H

// End of File
