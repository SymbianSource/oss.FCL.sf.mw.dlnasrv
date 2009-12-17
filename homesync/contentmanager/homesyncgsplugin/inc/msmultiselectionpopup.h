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
* Description:  CMSMultiselectionPopup class definition
*
*/


#ifndef CMSMULTISELECTIONPOPUP_H
#define CMSMULTISELECTIONPOPUP_H

// INCLUDES
#include <AknIconArray.h> // CAknIconArray
#include <aknlistquerydialog.h> //CAknListQueryDialog

// CLASS DECLARATION

/**
 * CMSMultiselectionPopup dialog class
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMultiselectionPopup ) : public CAknListQueryDialog
    {
            
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since S60 v5.1
         * @param aMediaServerArray, array of servers to show
         * @param aSelectionIndexArray, currently selected servers
         * @return CMSMultiselectionPopup*, the new CMSMultiselectionPopup object
            */
        static CMSMultiselectionPopup* NewL(
                TBool aUploadCapabilitySupport,
                RPointerArray<CCmMediaServerFull>* aMediaServerArray,
                CListBoxView::CSelectionIndexArray* aSelectionIndexArray );

        /**
         * Two-phased constructor.
         *
         * @since S60 v5.1
         * @param aMediaServerArray, array of servers to show
         * @param aSelectionIndexArray, currently selected servers
         * @return CMSMultiselectionPopup*, the new CMSMultiselectionPopup object
            */
        static CMSMultiselectionPopup* NewLC(
                TBool aUploadCapabilitySupport,
                RPointerArray<CCmMediaServerFull>* aMediaServerArray,
                CListBoxView::CSelectionIndexArray* aSelectionIndexArray );
				
        /**
         * Destructor.
         */
        virtual ~CMSMultiselectionPopup();

        /**
         * Updates the list of servers.
         *
         * @since S60 v5.1
         * @param aMediaServer, found server
         */
        void UpdateAndDrawPopupL( CCmMediaServerFull* aMediaServer );

		/**
	     * Handles pointer event on touchscreen
	     *
	     * @param aPointerEvent, touchscreen key event 
	     */
		void HandlePointerEventL ( const TPointerEvent& aPointerEvent );

    protected:

        /**
         * 2nd phase constructor.
         */
        void ConstructL();


    // From base class CAknSelectionListDialog
        /**
         * From CAknSelectionListDialog
         * See base class definition
         */
        void PreLayoutDynInitL();

        /**
         * From CAknSelectionListDialog 
         * See base class definition        
         */
        void  PostLayoutDynInitL();

        /**
         * From CAknSelectionListDialog 
         * See base class definition        
         */
        TKeyResponse  OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                      TEventCode aType );
        /**
         * From CAknSelectionListDialog 
         * See base class definition        
         */
        TBool OkToExitL( TInt aButtonId );

    private:

        /**
         * C++ default constructor.
         */
        CMSMultiselectionPopup(
            TBool aUploadCapabilitySupport,            
            RPointerArray<CCmMediaServerFull>* aMediaServerArray,
            CListBoxView::CSelectionIndexArray* aSelectionIndexArray );


        /**
         * Loads a possibly skinned icon and adds it to icon array
         *
         * @since S60 v5.1
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
                                 TInt aMaskId ) const;

        /**
         * Sets the list of servers for the listbox.
         * @since S60 v5.1
         */
        void AppendMediaServersL();

        /**
         * Creates listbox item string
         * @since S60 v5.1
         * @param aServer, server info
         * @return HBufC*, listbox item
         */
        HBufC* CreateListBoxItemLC( const CCmMediaServerFull* aServer );

    // Enumerations
    
        // icons indexes for the listbox
        enum TIconList
            {
            ECheckedBox = 0,
            EUnCheckedBox,
            EUploadCapability,
            EIconLast
            };

    private: // data

        /**
         * Dialog control item
         */
        CEikListBox*                        iListBox;           // not owned

        /**
         * Indicates if upload capability is needed
         */
        TBool                               iUploadCapabilitySupport;
        
        /**
         * Media servers
         */
        RPointerArray<CCmMediaServerFull>*  iServers;           // not owned

        /**
         * Previously selected indexes
         */
        CArrayFix<TInt>*                    iPreviousSelection; // not owned
    };

#endif // CMSMULTISELECTIONPOPUP_H

// End of File
