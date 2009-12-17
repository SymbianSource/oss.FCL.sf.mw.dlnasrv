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
* Description:  Header file for the UPnPSelectionDialog class implementation
*
*/


#ifndef __UPNP_BROWSE_DIALOG_H__
#define __UPNP_BROWSE_DIALOG_H__

// INCLUDES
// System
#include <AknProgressDialog.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsessionobserver.h"
#include "upnpavrenderingsessionobserver.h"

// commonui internal
#include "upnpdeviceobserver.h"

// FORWARD DECLARATIONS
class CUPnPMusicAdapter;
class CUPnPLocalPlayer;
class CUPnPBrowseCacheItem;
class CAknIconArray;
class CUPnPCommonUI;
class CUpnpImagePlayer;
class CUPnPBrowsePlaylistFiller;
class CUPnPPeriodic;

/**
* Browse dialog class of Common UI library
*/
class CUPnPBrowseDialog : public CAknDialog,
                          public MUPnPAVBrowsingSessionObserver,
                          public MProgressDialogCallback,
                          public MUPnPDeviceObserver
                          
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUPnPBrowseDialog* NewL(
                                            TInt aMenuResource,
                                            MUPnPAVController& aAVControl,
                                            const CUpnpAVDevice& aDevice,
                                            CUPnPCommonUI& aCommonUI );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUPnPBrowseDialog();

    public: // Methods from MUPnPAVBrowsingSessionObserver

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
                         ){};

        /**
         * Notifies that the create container operation is complete.
         *
         * @since Series 60 3.1
         * @param TInt, status information
         * @param aObjectId (const TDesC8&) object ID of the new container
         */
        void CreateContainerResponse( TInt /*aError*/,
                        const TDesC8& /*aObjectId = KNullDesC8*/ ){};

        /**
         * Notifies that the requested UPnP Object deletion is complete.
         *
         * @since Series 60 3.1
         * @param TInt, status information
         * @return None
         */
        void DeleteObjectResponse( TInt /*aError*/ ){};

         /**
         * Returns search capabilities of the requested Media Server.
         *
         * @since Series 60 3.1
         * @param TInt, status information
         * @param HBufC8&, the search capabilities string
         * @return None
         */
        void SearchCapabilitiesResponse( TInt /*aError*/,
            const TDesC8& /*aSearchCapabilities*/  ){};
        
        
        /**
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */ 
        void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );
        
        /**
         * Notifies that the Local Media Server has been put on sharing state
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */
        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ){};
        
   protected: // from CoeControl
   
        /**
        * Gets the control's help context. 
        * @param TCoeHelpContext, context of desired help
        * @return none
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
   
    public: // From MProgressDialogCallback

        /**
         * Callback function for progress bar.
         *
         * @since Series 60 3.1
         * @param aButtonId, id of the button pressed
         * @return None
         */
        void DialogDismissedL( TInt aButtonId );

    public: // New functions

        /**
        * Sends the browse request to UPnP AV Controller. When result set
        * arrives, UPnP AV Controller will call the "BrowseResponse" call
        * back method.
        *
        * @param TInt index of the iBrowseArray object, if -1 is given,
        *             the root container will be browsed.
        * @return None
        */
        void SendBrowseRequestL( TInt aIndex );

        /**
         * Callback method for the iImageControlTimer.
         *
         * @since Series 60 3.1
         * @param aDlg current dialog which to be called
         * @return Tint
         */
        static TInt ImageControlTimerCallbackL(TAny* aDlg);


    protected:  // Functions from base classes

        /**
        * From CAknDialog, handles menu commands
        * @param aCommandId, command to be handled
        * @return none
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CAknDialog, handles key events.
        * @param TKeyEvent, Event to handled.
        * @param TEventCode, Type of the key event.
        * @return TKeyResponse, response code (EKeyWasConsumed, 
        *         EKeyWasNotConsumed).
        */
        TKeyResponse  OfferKeyEventL (const TKeyEvent &aKeyEvent,
                                      TEventCode aType);

        /**
        * From CAknDialog, handles layout initialization
        */
        void PreLayoutDynInitL();

       /**
        * Menu observer interface. From MEikMenuObserver
        * @since Series 60 3.1
        * @param aResourceId resource ID identifying the menu pane to 
        * initialise.
        * @param aMenuPane The in-memory representation of the menu pane
        * @return none
        */
        void DynInitMenuPaneL (TInt aResourceId, CEikMenuPane *aMenuPane);

        /**
        * From CAknDialog, handles focus changes of the dialog
        * @param aButtonId, pressed button id
        * @return True if ready to close dialog, False otherwise
        */
        TBool OkToExitL( TInt aButtonId );
        
        /**
        * From CoeControl
        */
        void HandleResourceChange(TInt aType);

        
    public: // Methods from MUPnPDeviceObserver
    
        /**
         * Notifies that the target renderer disappeared
         *
         * @since Series 60 3.1
         * @param aError exit reason
         * @return None
         */
        void DeviceDisappeared( TInt aError );
        
    private:    // New Functions

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
                                 TInt aMaskId);

        /**
        * Marks one item
        * @since Series 60 3.1
        * @param aItemIndex (TInt) index of item that should be marked
        * @return None
        */
        void MarkItemL( TInt aItemIndex );

        /**
        * Unmarks one item
        * @since Series 60 3.1
        * @param aItemIndex (TInt) index of item that should be unmarked
        * @return None
        */
        void UnmarkItem( TInt aItemIndex );

        /**
        * Marks all items. Does not mark any containers.
        * @since Series 60 3.1
        * @return None
        */
        void MarkAllItemsL();

        /**
        * Unmarks all items
        * @since Series 60 3.1
        * @return None
        */
        void UnmarkAllItems();

        /**
        * Unmarks one item
        * @since Series 60 3.1
        * @param aItemIndex (TInt) index of item that should be unmarked
        * @return None
        */
        void UnMarkItemL( TInt aItemIndex );

        /**
        * Plays music file
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void PlayL(TBool aLocal);

        /**
        * Show image, video and music on external media renderer
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void ShowPlayExtL();

        /**
        * Displays wait note
        * @since Series 60 3.1
        * @param TInt, note resource
        */
        void DisplayWaitNoteL( TInt aResource );


        /**
        * Dismisses wait note
        * @since Series 60 3.2.3
        */
        void DismissWaitNoteL();
        
        /**
        * Called when target device responds to browse query
        * @since Series 60 3.1
        * @param TInt, query status
        * @param aTotalCount TInt total number of objects
        * @param RPointerArray&, returned item array
        * @return None
        */
        void BrowseResponseL( 
                    TInt aError,
                    TInt aTotalCount,
                    const RPointerArray<CUpnpObject>& aResultArray );

        
        
        
        /**
        * updates selected items array
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void SelectedArrayCheckL(void);
        
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
         
    private:

        /**
        * C++ default constructor.
        */
        CUPnPBrowseDialog( MUPnPAVController& aAVControl,
                           CUPnPCommonUI& aCommonUI,
                           const CUpnpAVDevice& aDevice );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aMenuResource );

        /**
        * Loads listbox items
        * @since Series 60 3.1
        * @return none
        */
        void LoadListItemsL( void );

        /**
        * Delete old list items
        * @since Series 60 3.1
        * @return none
        */
        void DeleteListItemsL( void );

        /**
        * Clears dialog title
        * @since Series 60 3.1
        * @return none
        */
        void ClearTitleL();

        /**
        * Updates navi pane text
        * @since Series 60 3.1
        * @return none
        */
        void UpdateNaviPaneTextL();

        /**
         * Check if the container with the specified container Id exists
         * @since Series 60 3.1
         * @param aCheckContainerId the container Id to be checked
         * @return the found result
         */
        TInt CacheCheck( const TDesC8& aCheckContainerId );
        
        /**
         * Update the cache if any new container is found
         * @since Series 60 3.1
         * @param aContainerId the container Id to be checked
         * @param aNewItem check if the container is new or not
         * @return the found result
         */
        void CacheItemUpdateL( const TDesC8& aContainerId, TBool aNewItem );
        
        /**
         * Send dummy browse response
         * @since Series 60 3.1
         * @param aCacheIndex the index in the cache array
         *
         * @return none
         */
        void SendDummyBrowseResponseL( TInt aCacheIndex );
        
        /**
        * Starts ImageControl
        * @since Series 60 3.1
        * @param None
        * @return none
        */
        void StartImageControlL();
        
        /**
        * Stops ImageControl
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void StopImageControlL();
        
        /**
        * Updates softkeys 
        * @since Series 60 3.1
        * @param aResourceId (TInt) softkey resource
        * @return None
        */
        void UpdateSoftkeysL( TInt aResourceId );
        
        /**
        * Starts imageControl timer
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void StartImageControlTimer( void );
        
        /**
        * Shows current image
        * @since Series 60 3.1
        * @param aShowImage (TBool) shows current image and draws showing 
        *  icon
        * @return None
        */
        void ShowCurrentImageL( TBool aShowImage );
        
        /**
        * Shows current video
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void ShowVideoDialogExtL( );
        
        /**
        * Handel copy
        * @since Series 60 3.1
        * @param None
        *                     
        * @return none
        */
        void HandleCopyL();
        
        /**
        * Create a playlist filler representing current music selection
        * for starting music playback
        * @since Series 60 3.1
        * @param none
        *                     
        * @return CUPnPPlayListFiller filler instance
        */
        CUPnPPlayListFiller* CreateFillerLC();
         
        /**
         * Handle different errors returned from other dialogs
         * @since Series 60 3.1
         * @param exiting reason
         * @return none
         */
        void HandleErrorL( TInt aError ); 
        
        /**
        * Check if any audio item is marked
        * @since Series 60 3.1
        * @param None
        * @return ETrue if at least one audio item is marked
        */
        TBool IsAudioItemMarked( void );  
 
    private:    // Data

        //listbox
        CEikColumnListBox*          iListBox; //not owned
        //browse result array
        RPointerArray<CUpnpObject>  iResultArray; //item inside not owned
        //navi pane
        CAknNavigationDecorator*    iNaviDecorator; //owned
        // navi pane
        CAknNavigationControlContainer* iNaviPane; //not owned
        //AV controller
        MUPnPAVBrowsingSession*     iBrowseSession; //owned
        
        CUPnPCommonUI&              iCommonUI; //not owned
        
        MUPnPAVController&          iAVControl; //not owned
        //parent container
        RPointerArray<HBufC8>       iParentId;  //owned
        // parent name
        RPointerArray<HBufC8>       iParentName; //owned
        //common UI
           
        //wait dialog
        CAknWaitDialog*             iWaitNoteDialog; //owned    
        //temporary browse result array
        RPointerArray<CUpnpObject>  iTempArray; // items inside not owned
        //browse direction
        TInt                        iBrowseFlag;
        
        // stores selected item index
        TInt                        iSelectedItem;
        
        //exit timer
        TBool                       iFirstResultArray;
        TInt                        iCurrentItem;
        TBool                       iAllObjectsReceived;

        TInt                        iPrevHighlighteditem;
        TInt                        iUppermostItem;
        TInt                        iBrowseDirection;
        
        TBool                       iDummyBrowseResponse;
        HBufC8*                     iCurrentFolderId; //owned
        TBool                       iBrowseRequestSent;

        RPointerArray<CUPnPBrowseCacheItem>        iBrowseCacheItems; //owned

        CPeriodic*                  iPeriodic; //owned       

        //Array for selected items
        RPointerArray<CUpnpObject>  iSelectedItemsArray; //owned
        //Flag to tell if the image control view is active
        TBool                       iImageControlActive;        
        //ImageControl periodic timer
        CUPnPPeriodic*              iImageControlTimer; //owned
        
        CUpnpImagePlayer*           iImagePlayer; //owned
        
        CUPnPLocalPlayer*           iLocalPlayer; //owned
        
        CUpnpAVDevice*              iTargetDevice; //owned
        
        const CUpnpAVDevice&        iSourceDevice; //not owned
        
        // title pane
        CAknTitlePane*              iTitlePane; //not owned
        
        //Flag to tell if shift and Ok was pressed the same time
        TBool                       iShiftAndOkPressed;
        
        TInt                        iLastImageItemIndex;
        
        TInt                        iChildDialogOpen;
        
        //flag to either media server disappears or media renderer disappear
        TBool                       iMSDisappear;
        
        TInt                        iTotalCount;
        
        CUPnPCommonUI::TUPnPAction               iAction;
        
        TInt                        iCopyIndex;
        
        TBool                       iRoot;
        
        TBool                       iIsMusicItem;
        
        TBool                       iAllObjectsReceviedInOneBrowse;
        
        TInt                        iNumObjectReceviedInOnBrowse;
        
        /**
         * for music playing, this is to prevent user to pop up renderer
         * selection and select renderer multiple times
         */
        TBool                       iMusicPlay;
        
        TInt                        iError;
        
        // is PreLayoutDynInitL done. Used when exiting dialog
        TBool                       iDlgPreLayoutDone;
        
        // Flag to tell if the the Hash (#) key is Holding down
        TBool                       iHashKeyFlag;
        
        // Flag to tell if need repeat browse request to get all objects 
        // which an original browse request desired
        TBool iNeedRepeatRequest;
        
        // Flag to tell if the browse request is original browse request.
        TBool iOriginalBrowseRequest;
        
        // Current number of objects receved from original browse request
        TInt iCurrentRecevedObjectIndex;
        
        // Count of objects which an original browse request desired
        TInt iNeedRecevedObjectCount;
        
        // Server return count of objects in first browse request
        TInt iServerReturnObjectCount;
        
        // If wait note is cancel before the browse dialog is displayed,
        // set a value for leave.
        TInt iErrorForCancel;	
        
        // Flag to tell if the copying is onging, ETrue: Ongoing
        TBool                       iCopying;
        
        // Flag to tell if the application should be closed, ETrue: should be closed
        TBool                       iApplicationClose;
    };

#endif // __UPNP_BROWSE_DIALOG_H__

// End of file
