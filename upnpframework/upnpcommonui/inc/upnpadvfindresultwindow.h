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
* Description:  Implements CAdvancedFindResultWindow class
*
*/


#ifndef ADVANCEDFINDRESULTWINDOW_H
#define ADVANCEDFINDRESULTWINDOW_H

// INCLUDES
#include <aknnavide.h> 
#include <akntitle.h> //for status pane
#include <AknIconArray.h>
#include <AknDialog.h>
#include "upnpdeviceobserver.h"
#include "upnpcommonui.h"

// FORWARD DECLARATIONS
class CUPnPCommonUI;
class MUPnPAVController;
class CUpnpImagePlayer;
class CUPnPMusicAdapter;
class CUPnPLocalPlayer;
class CUPnPPeriodic;

// DATA TYPES

// CLASS DECLARATION

/**
* CAdvancedFindResultWindow dialog class
* @since Series 60 3.1
*/
NONSHARABLE_CLASS( CAdvancedFindResultWindow ) : 
                                              public CAknDialog,
                                              public MUPnPDeviceObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CAdvancedFindResultWindow* NewL(
                                    TInt aMenuResource, 
                                    RPointerArray<CUpnpObject>& aResultArray,
                                    MUPnPAVController& aAVControl,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI );
        
        /**
        * Destructor.
        */
        virtual ~CAdvancedFindResultWindow();
    
    public: //from MUPnPDeviceObserver
         
         /**
          * This is called when the WLAN or a media server to be browsing 
          * has disappeard
          * To close itself
          * @since Series 60 3.1
          * @param aError exit reason
          * @return none
          */
         void DeviceDisappeared( TInt aError );
    
    public:  //new functions
    
        /**
        * Close the browse dialog itself
        * @since Series 60 3.1
        * @param aError exit reason
        * @return None
        */
        void DismissItselfL( TInt aError );
        
        /**
         * Callback method for the iImageControlTimer.
         *
         * @since Series 60 3.2.3
         * @param aDlg current dialog which to be called
         * @return Tint
         */
        static TInt ImageControlTimerCallbackL(TAny* aDlg);
        
    protected:  // New functions
        
        /**
          * To load list items when the dialog is initialized
          * @since Series 60 3.1
          * @param none
          * @return none
          */
        void LoadListItemsL();

        /**
        * 2nd constructor
        */
        void ConstructL( TInt aMenuResource, 
                         RPointerArray<CUpnpObject>& aResultArray );
                         
        /**
        * Modifies navigation pane text
        * @since Series 60 3.1       
        */
        void SetNaviPaneTextL();

        /**
        * Loads a possibly skinned icon and adds it to icon array
        * @since Series 60 3.1
        * @param CAknIconArray, array of icons        
        * @param MAknsSkinInstance, skin instance
        * @param TDesC, reference to icon file        
        * @param TAknsItemID, skinned icon id       
        * @param TInt, bitmap id      
        * @param TInt, bitmap mask id        
        */
        
        void AppendIconToArrayL( CAknIconArray* aArray,
                                 MAknsSkinInstance* aSkin,
                                 const TDesC& aMbmFile,
                                 const TAknsItemID& aID,
                                 TInt aBitmapId,
                                 TInt aMaskId );
 
        /**
         * Mark or unmark a menu item
         * @since Series 60 3.1
         * @param aItem a menu item to be marked
         * @param aMark a menu item should be marked or not
         * @return none
         */                                     
        void MarkUnmarkL(TInt aItem, TBool aMark);
        
        /**
         * Play a seleted item
         * @since Series 60 3.1
         * @param aLocal if the selected item is a local or remote
         * @return none
         */
        void PlayL( TBool aLocal );
        
        /**
         * Show or play an item on remote renderer
         * @since Series 60 3.1
         * @param none
         * @return none
         */
        void ShowPlayExtL();
        
        /**
         * Show or play an item on local renderer
         * @since Series 60 3.1
         * @param none
         * @return none
         */
        
        void ShowPlayLocalL();
        
        /**
        * updates command button area
        * @since Series 60 3.23
        * @param aMark use for two states, item marked or no marked
        * @param aTempCounter is the number of the current item in the list.
        * @return None
        */
        void UpdateCommandButtonAreaL( TBool aMark, TInt aTempCounter );

        /**
        * updates command button area
        * @since Series 60 3.23
        * @param aMark use for two states, item marked or no marked
        * @param aTempCounter is the number of the current item in the list.
        * @return None
        */
        void UpdateCommandButtonArea( TBool aMark, TInt aTempCounter );
        
    protected:  // Functions from base classes
        
        /**
        * From CAknSelectionListDialog, handles layout initialization
        * @since Series 60 3.1
        */
        void PreLayoutDynInitL();
        
        /**
        * From CAknDialog, handles menu commands
        * @since Series 60 3.1
        * @param aCommandId, command to be handled
        * @return none
        */
        void ProcessCommandL( TInt aCommandId );
        
        /**
        * From CAknSelectionListDialog, handles key events.
        * @since Series 60 3.1
        * @param TKeyEvent, Event to handled.
        * @param TEventCode, Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, 
                                     TEventCode aType );
        
        /**
        * From CAknSelectionListDialog, handles focus changes of the dialog
        * @since Series 60 3.1
        * @param TInt, pressed button id
        * @return TBool, ETrue if ready to close dialog, EFalse otherwise
        */
        TBool OkToExitL( TInt aButtonId );
        
        /**
        * Menu observer interface. From MEikMenuObserver
        * @since Series 60 3.1
        * @param aResourceId resource ID identifying the menu pane 
        * to initialise.
        * @param aMenuPane The in-memory representation of the menu pane
        * @return none
        */
        void DynInitMenuPaneL (TInt aResourceId, CEikMenuPane *aMenuPane); 
    
    private:
        /**
        * C++ default constructor.
        */
        CAdvancedFindResultWindow( MUPnPAVController& aAVControl,
                                   MUPnPAVBrowsingSession& aBrowseSession,
                                   CUPnPCommonUI& aCommonUI );
       
        /**
        * From CoeControl
        * @param TCoeHelpContext, context of desired help
        * @return none
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
              
        /**
         * Show a video on a remote renderer
         * @since Series 60 3.1
         * @param none 
         * @return none
         */
        void ShowVideoExtL();
        
        /**
         * Show an image on a remote renderer
         * @since Series 60 3.1
         * @param none 
         * @return void
         */
        void ShowImageExtL();
        
        /**
         * Shows current image
         * @since Series 60 3.2.3
         * @param aShowImage (TBool) shows current image and draws showing 
         *  icon
         * @return None
         */
         void ShowCurrentImageL( TBool aShowImage );
         
         /**
          * Starts imageControl timer
          * @since Series 60 3.2.3
          * @param None
          * @return None
          */
          void StartImageControlTimer();
           
           /**
           * Stops ImageControl
           * @since Series 60 3.2.3
           * @param None
           * @return None
           */
           void StopImageControlL();
        
        /**
         * Handle different errors returned from other dialogs
         * @since Series 60 3.1
         * @param exiting reason
         * @return none
         */
        void HandleErrorL( TInt aError );
        
        /**
        * Handel copy
        * @since Series 60 3.1
        * @param none
        *                     
        * @return none
        */
        void HandleCopyL();
        
        /**
        * Updates softkeys 
        * @since Series 60 3.2
        * @param aResourceId (TInt) softkey resource
        * @return None
        */
        void UpdateSoftkeysL( TInt aResourceId );      
        
        /**
        * Check if any audio item is marked
        * @since Series 60 3.1
        * @param None
        * @return ETrue if at least one audio item is marked
        */
        TBool IsAudioItemMarked( void );  
        
    private: //data
        
        //used for CUpnpFileTransferEngine only
        MUPnPAVBrowsingSession&                      iBrowseSession; 
        // find results (NOT OWNED)
        RPointerArray<CUpnpObject>                   iResultArray;
        // dialog control item (NOT OWNED)
        CAknDoubleGraphicStyleListBox*               iListBox;
        // navi pane decorator
        CAknNavigationDecorator*                     iNaviDecorator;
        // navipane member (NOT OWNED)
        CAknNavigationControlContainer*              iNaviPane;
        // title pane (NOT OWNED)
        CAknTitlePane*                               iTitlePane;
        
        MUPnPAVController&                           iAVControl;
        
        // pointer to device selection
        CUPnPCommonUI&                               iCommonUI;
        
        //ImageControl periodic timer (own)
        CUPnPPeriodic*                               iImageControlTimer;
        
        CUpnpImagePlayer*                            iImagePlayer;
        
        CUPnPLocalPlayer*                            iLocalPlayer;
        
        CUpnpAVDevice*                               iTargetDevice;

        // the media server device (NOT OWNED)        
        const CUpnpAVDevice*                         iSourceDevice;
        
        TInt                                         iChildDialogOpen;
        
        //flag to either media server disappears or media renderer disappear
        TBool                                        iMSDisappear;
        
        CUPnPCommonUI::TUPnPAction                   iAction;
        
        TInt                                         iCopyIndex;
        
        TBool                                        iIsMusicItem;
        
        /**
         * for music playing, this is to prevent user to pop up renderer
         * selection and select renderer multiple times
         */
        TBool                                       iMusicPlay;
        
        // Flag to tell if the image control view is active
        TBool                                       iImageControlActive;
        
        // The Index of last show image
        TInt                                        iLastImageItemIndex;
        // Flag to tell if the the Hash (#) key is Holding down
        TBool                                       iHashKeyFlag;
    };
#endif
// End of File
