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
* Description:  List for data of settings dialog
*
*/


#ifndef UPNPADVANCEDFINDLIST_H
#define UPNPADVANCEDFINDLIST_H

// INCLUDES
// System
#include <aknsettingitemlist.h> 
#include <AknProgressDialog.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsessionobserver.h"

// commonui internal
#include "upnpcommonui.h"


const TInt KMaxNameLength = 256;
const TInt KGranularityOfArrays = 8;
const TInt KSizeOfTBufC16 = 32;

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class CAdvancedFindResultWindow;
class CUPnPAdvancedFindDialog;
class CUPnPCommonUI;

/**
*  Settings list class declaration
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CUPnPAdvancedFindList ) : public CAknSettingItemList,
                              public MUPnPAVBrowsingSessionObserver,
                              public MProgressDialogCallback
    {
    public:  // Constructors and destructor
        
       /**
        * Two-phased constructor.
        */
        static CUPnPAdvancedFindList* NewL( 
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession,
                                CUPnPAdvancedFindDialog& aParent,
                                CUPnPCommonUI& aCommonUI );
        
       /**
        * Destructor.
        */
        virtual ~CUPnPAdvancedFindList();

    public: // Functions from base classes

       /**
        * From CAknSettingItemList, this launches the setting page.
        * @since Series 60 Series3.1
        * @param aIndex, index of selected list item
        * @param aCalledFromMenu, indicates if editing is called trough menu 
        *                         or from keyboard
        * @return none
        */
        void EditItemL ( TInt aIndex, TBool aCalledFromMenu );
        
        /** 
         * Launch up CAknSettingItemList
         * @since Series 60 3.1
         * @param none
         * @return error code
         */
        TInt MakeQueryL( );
        
    public: //From MProgressDialogCallback
        
        /**
         * Callback method Get's called when a dialog is dismissed
         * @since Series 60 3.1
         * @param aButtonId the reason when the dialog is dismissed
         * @return none
         */
        void DialogDismissedL( TInt aButtonId );

    public: //From MUPnPAVBrowsingSessionObserver

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
                            const TDesC8& /*aBrowseResponse*/,
                            TInt /*aError*/,
                            TInt /*aMatches*/,
                            TInt /*aTotalCount*/,
                            const TDesC8& /*aUpdateId*/
                            ){};

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
                            const TDesC8& aSearchResponse,
                            TInt aError,
                            TInt aMatches,
                            TInt aTotalCount,
                            const TDesC8& aUpdateId
                            );

         /**
         * Returns processed search results received from a Media Server.
         *
         * @since Series 60 3.1
         * @param TInt aError status information
         * @param aTotalCount TInt total number of objects
         * @param RPointerArray of CUpnpObject objects
         * @return None
         */
         void SearchResponseL( 
                         TInt aStatus,
                         const RPointerArray<CUpnpObject>& aResultArray );

        /**
         * Returns search capabilities of the requested Media Server.
         *
         * @since Series 60 3.1
         * @param TInt aError status information
         * @param HBufC8& the search capabilities string
         * @return None
         */
        void SearchCapabilitiesResponse( TInt /*aError*/,
                         const TDesC8& /*aSearchCapabilities*/ ){};

        /**
         * Notifies that the create container operation is complete.
         *
         * @since Series 60 3.1
         * @param aError status information
         * @param aObjectId (const TDesC8&) object ID of the new container
         */
        void CreateContainerResponse( TInt /*aError*/,
                                      const TDesC8& /*aObjectId = KNullDesC8*/ )
                                      {};

        /**
         * Notifies that the requested UPnP Object deletion is complete.
         *
         * @since Series 60 3.1
         * @param aError status information
         * @return None
         */
        void DeleteObjectResponse( TInt /*aError*/ ){};

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
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */
        void MediaServerDisappearedL( TUPnPDeviceDisconnectedReason aReason );
        
        /**
         * Notifies that a local media server has been on sharing
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */      
        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ){};

    public: //business logic
        
        /**
         * Check if the setting page is open 
         *
         * @since Series 60 3.1
         * @param none
         * @return TBool ETrue if the setting is open
         */ 
        TBool IsSettingPageOpen() const;
        
    protected:  // Functions from base classes
        
        /**
         * Handles a change to the control's resources
         *
         * @since Series 60 3.1
         * @param aType a message UID value.
         * @return None
         */  
        void HandleResourceChange( TInt aType ); 

    private:
        
        /**
        * C++ default constructor.
        */
        CUPnPAdvancedFindList( MUPnPAVController& aAVControl, 
                               MUPnPAVBrowsingSession& aBrowsingSession,
                               CUPnPAdvancedFindDialog& aParent,
                               CUPnPCommonUI& aCommonUI );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * From CAknSettingItemList, Framework method to create a setting item.
        * @since Series 60 Series3.1
        * @param aIdentifier,  list item id to be created
        * @return CAknSettingItem, pointer to list item
        */        
        CAknSettingItem* CreateSettingItemL( TInt aIdentifier );
        
        /**
        * Get search criteria
        * @since Series 60 Series3.1
        * @param none
        * @return search criteria
        */
        HBufC8* BuildSearchCriteriaL() const;
        
    private:    // Data

        //time variable for date from item
        TTime iDateFrom;
        //time variable for date Until item
        TTime iDateUntil;
        //time variable for initial date from item
        TTime iDateInitial;

        // Boolean flag telling if Search response received
        TBool iSearchResponseReceived;
                        
        // text for artist selection item
        TBuf<KMaxNameLength> iArtistText;
        // text for album selection item
        TBuf<KMaxNameLength> iAlbumText;
        // text for Genre selection item
        TBuf<KMaxNameLength> iGenreText;
        
        TBuf<KMaxNameLength> iFileName;
        
        //chosen media type
        TInt iMediatype;

          // wait note dialog
        CAknWaitDialog*                         iWaitNoteDialog;
        
        
        MUPnPAVController&                      iAVControl;
                
        //observer for browse results
        MUPnPAVBrowsingSessionObserver*         iBrowseObserver;       

        //Array for search result
        RPointerArray<CUpnpObject>              iResultArray;
        
        CAdvancedFindResultWindow*              iFindResultWindow; //not owned
        CUPnPCommonUI&                          iCommonUI;
        CUPnPAdvancedFindDialog&                iAdvancedFindDialog;
        
        MUPnPAVBrowsingSession&                 iBrowseSession;
        
        CUPnPCommonUI::TUPnPAction              iAction;
        
        TInt iExitCode;
        
        // iShowingErrorNote tells if we are showing the error note. 
        // During the error note is shown, we cannot destroy our parent dialog
        // because then the continuation after error note fails (crashes)
        TBool iShowingErrorNote;
        
        TBool                                   iClose;
        TBool                                   iSettingPageOpen;
        
        TUPnPDeviceDisconnectedReason           iCloseReason;
        
    };
#endif      // UPNPADVANCEDFINDLIST_H
            
// End of File
