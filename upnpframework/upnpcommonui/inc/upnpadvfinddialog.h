/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  advanced find dialog class header
*
*/



#ifndef UPNPADVANCEDFINDDIALOG_H
#define UPNPADVANCEDFINDDIALOG_H

//  INCLUDES
#include <aknselectionlist.h>
#include <aknlists.h> 
#include <aknsettingitemlist.h> 
#include <akntitle.h> //for status pane

// FORWARD DECLARATIONS
class CUPnPAdvancedFindList;
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class CUPnPCommonUI;

// CLASS DECLARATION

/**
*  Class declaration for UPnP Advanced find dialog
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CUPnPAdvancedFindDialog ) : public CAknDialog
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CUPnPAdvancedFindDialog* NewL(
                                TInt aMenuResource, 
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession,
                                CUPnPCommonUI& aCommonUI );
        
        /**
        * Destructor.
        */
        virtual ~CUPnPAdvancedFindDialog();

    public: // New functions
    
        /**
         * Called from CommonUI to destroy the advFind dialog itself
         * when the selected media server disappears
         * @since Series 60 3.1
         * @param aError exit error
         * @return None
         */   
        void DismissItselfL( TInt aError );

    public: // Functions from base classes

       /**
        * From CAknDialog, handles menu commands
        * @since Series 60 Series3.1
        * @param aCommandId, command to be handled
        */
        void ProcessCommandL( TInt aCommandId );
      
       /**
        * From CAknDialog, handles key events.
        * @since Series 60 Series3.1
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, 
                                     TEventCode aType );
        
    protected:  // Functions from base classes
       /**
        * From CAknDialog, handles layout initialization
        * @since Series 60 Series3.1
        * @param none
        * @return none
        */
        void PreLayoutDynInitL();

       /**
        * From CAknDialog, handles focus changes of the dialog
        * @since Series 60 Series3.1
        * @param aButtonId, pressed button id
        * @return True if ready to close dialog, False otherwise
        */
        TBool OkToExitL( TInt aButtonId );
       
       /**
        * Creates custom control
        * @since Series 60 Series3.1
        * @param aControlType, control type
        * @return SEikControlInfo control info
        */
        virtual SEikControlInfo CreateCustomControlL( TInt aControlType );
        
    private:

       /**
        * C++ default constructor.
        */
        CUPnPAdvancedFindDialog::CUPnPAdvancedFindDialog(
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession,
                                CUPnPCommonUI& aCommonUI );
       /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aMenuResource );
    
    private:
        
        /**
         * Set dialog title
         */
        void SetTitleL();
        
    private:    // Data
        
        // list member variable for dialog data
        CUPnPAdvancedFindList*        iSettingsList; // not owned
        // title pane
        CAknTitlePane*                iTitlePane; // not owned
        // status pane
        CEikStatusPane*               iStatusPane; // not owned
        
        MUPnPAVController&            iAVControl; // not owned
        MUPnPAVBrowsingSession&       iBrowseSession; // not owned
        
        // title text before find
        HBufC*                        iOriginalTitleText; // owned
        CUPnPCommonUI&                iCommonUI; //not owned
        
        TBool                         iClose;
    };

#endif      // UPNPADVANCEDFINDDIALOG_H   
            
// End of File

