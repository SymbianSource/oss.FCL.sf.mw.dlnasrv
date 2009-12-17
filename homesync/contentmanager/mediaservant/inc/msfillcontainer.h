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
* Description:  Definition for MediaServant fill list container class
*
*/


#ifndef C_MSFILLCONTAINER_H
#define C_MSFILLCONTAINER_H

// INCLUDES
#include <aknlists.h>
#include "msbasecontainer.h"
#include "mediaservant.hrh"

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CAknIconArray;
class CMSAppUi;
class CCmFillRule;
class CMSFillRuleController;
class CMSFillView;
class CMSFillList;

/**
 * CMSFillContainer class
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillContainer ): public CMSBaseContainer
    {
    private:

        // icon enumerations
        enum TMSListBoxIcons
            {
            EMSSelectedFits = 0,
            EMSUnselectedFits,
            EMSSelectedPartiallyFits,
            EMSUnselectedPartiallyFits,
            EMSSelectedDoesntFit,
            EMSUnSelectedDoesntFit,
            EVideoType,
            EImageType,
            EAudioType,
            EMark,
            EIconCount
            };

    public:
    
        /**
         * Constructor.
         */
        CMSFillContainer( CMSAppUi& aAppUi, CMSFillView& aView );

        /**
         * Symbian default constructor.
         */
        void ConstructL( TRect aRect );

        /**
         * Destructor.
         */
        virtual ~CMSFillContainer();

        /**
         * Updates listbox data
         * @since S60 5.1
         */
        void UpdateListBoxDataL();

        /**
         * sets fill rule activity
         * @since S60 5.1
         */
        void SetFillRules();

        /**
         * Highlighted item index
         * @since S60 5.1
         * @return TInt, highlighted item index
         */
        TInt SelectedItemIndex();

        /**
         * Count of list items
         * @since S60 5.1
         * @return TInt, item count
         */
        TInt ListItemCount();

        /**
         * Sets reorder mode states
         * @since S60 5.1
         * @param aState, reordering state
         */
        void SetReorderStateL( TMSReorderState aState );

        /**
         * Deletes current rule
         * @since S60 5.1
         */
        void DeleteRuleL();

        /**
         * Checks if the list already have rule with this name
         * @since S60 5.1
         * @param aName, rule name
         * @TInt number of dublicates
         */
        TInt CheckIfDublicateNames( const TDesC& aName );

        /**
         * Toggles list item
         * @since S60 5.1
         */
        void ChangeListActivityL();

        /**
         * Checks if item is selected
         * @since S60 5.1
         * @return TBool, ETrue if activated
         */
        TBool IsCurrentListItemActive();

        /**
         * Checks if user has changed list selections
         * @since S60 5.1
         * @return TBool, ETrue if changed
         */
        TBool HasListSelectionsChanged();

        /**
         * Updates fill list size on navi pane
         * @since S60 5.1
         */
        void UpdateNaviTextL();

        /**
         * Checks if list(s) have been deleted
         * @since S60 5.1
         * @return TBool, ETrue if deleted, EFalse otherwise
         */
        TBool IsListDeleted();

        /**
         * Handles pointer event on touchscreen
         * @param aPointerEvent, touchscreen key event 
         */
    	void HandlePointerEventL ( const TPointerEvent& aPointerEvent );

    protected:

// From base class CCoeControl
        
        /**
         * From CoeControl
         * See base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
         * From CoeControl
         * See base class definition
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * From CoeControl
         * See base class definition
         */
        TInt CountComponentControls() const;

        /**
         * From CoeControl
         * See base class definition
         */
        void SizeChanged();

        /**
         * From CoeControl
         * See base class definition
         */
        void HandleResourceChange(TInt aType);

        /**
         * From CoeControl
         * See base class definition
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );

        /**
         * From CoeControl
         * See base class definition
         */
		void HandleDragAndDropEventsL( const TPointerEvent& aPointerEvent );

    private:


        /**
         * Handles up/down keys during reordering
         * @since S60 5.1
         * @param aKeyEvent Key event.
         * @param aType Type of key event( EEventKey, EEventKeyUp or
         * EEventKeyDown ).
         */
        TKeyResponse HandleReorderKeyEventL(const TKeyEvent& aKeyEvent,
                                            TEventCode aType );

        /**
         * Moves item on the list
         * @since S60 5.1
         * @param aDirection, moving direction
         */
        void MoveItem( TInt aDirection );

        /**
         * Sets rule activity according to checkbox states on the list
         * @since S60 5.1
         */
        void StoreSelectionStatus();

        /**
         * Gets icon for the list
         * @since S60 5.1
         * @param aListName, name of the list
         * @param aIndex, item index
         * @param aListSize, list size in bytes
         */
        TInt ListItemIconIndex( TInt aIndex,
                                TInt64 aListSize,
                                TInt64& aFreeSpace );

        /**
         * Calculates fill quota based on selected drives
         * @since S60 5.1
         */
        void CalculateFillQuotaL();

    private:

        /**
         * Listbox control
         */
        CAknColumnListBox* iListBox;
        /**
         * Pointer to application ui
         */
        CMSAppUi&                           iMSAppUi;           // not owned
        /**
         * Reorder state
         */
        TMSReorderState                     iReorderState;
        /**
         * Pointer array to rules
         */
        RPointerArray<CMSFillList>*         iFillListArray;     // not owned
        /**
         * Fill rule controller
         */
        CMSFillRuleController*              iFillRuleController;// not owned
        /**
         * Parent view
         */
        CMSFillView&                        iView;
        /**
         * Size of selected lists
         */
        TInt64                              iSizeOfSelectedLists;
        /**
         * Selected items on view activation
         */
        RArray<TInt>                        iSelectedLists;
        /**
         * Context pane icon
         */
        CFbsBitmap*                         iIcon;              // not owned
        /**
         * Context pane icon mask
         */
        CFbsBitmap*                         iIconMask;          // not owned
        /**
         * Selection indexes
         */
        const CListBoxView::CSelectionIndexArray*
                                            iSelectedIndexes;   // not owned
        /**
         * MMC free space ( for navi pane text )
         */
        TInt64                              iFreeMemory;
        /**
         * MMC quota ( set by user )
         */
        TInt64                              iQuota;
        /**
         * List delete flag
         */
        TBool                               iListDeleted;
        /**
         * Previous listbox selection
         */
        TInt                                iPreviousItemIndex;
        /**
         * Index of an item that is dragged
         */
        TInt 								iDragItemIndex;
    };

#endif // C_MSMAINCONTAINER_H