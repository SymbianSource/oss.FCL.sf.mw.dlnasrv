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
* Description:  Definition for MediaServant store list container class
*
*/


#ifndef C_MSSTORELISTCONTAINER_H
#define C_MSSTORELISTCONTAINER_H

// INCLUDES
#include <aknlists.h>
#include "msbasecontainer.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CCmStoreRuleContainer;
class CMSStoreListView;


/**
 * CMSStoreListContainer class definition
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreListContainer ): public CMSBaseContainer,
                              public MEikListBoxObserver
    {
    private:
        // Icon enumerations
        enum TMSListBoxIcons
            {
            EIconCheckboxOn = 0,
            EIconCheckboxOff,
            EIconCount
            };

    public:
        /**
         * Constructor.
         *
         * @since S60 5.1
         * @param aAppUi, application UI
         * @param aView, store list view
         * @return None
         */
        CMSStoreListContainer( CMSAppUi& aAppUi, CMSStoreListView& aView );

        /**
         * Symbian default constructor.
         */
        void ConstructL( TRect aRect );

        /**
         * Destructor.
         */
        virtual ~CMSStoreListContainer();

    public:

        /**
         * Highlighted item index
         * @since S60 5.1
         * @param None
         * @return TInt, highlighted item index
         */
        TInt SelectedItemIndex();

        /**
         * Sends store rules to server
         * @since S60 5.1
         * @param None
         * @return None
         */
        void UpdateRulesToServerL();

        /**
         * Counts new list items
         * @Since S60 5.2
         * @param None
         * @return TInt, item count
         */
        TInt ListItemCountL();

        /**
         * Return current item's selection status
         * @Since S60 5.1
         * @param None
         * @return TBool, ETrue if selected, EFalse othwerwise
         */
        TBool CurrentListItemSelectionStatus();

        /**
         * Toggles highlighted item's selection         
         * @Since S60 5.1
         * @param None
         * @return None
         */
        void ToggleCurrentItemL();
        
        /**
         * Sets selection statuses to store lists
         * @since S60 5.1
         * @param None
         * @return None
         */
        void SetServerSelectionsL();

    protected: 

// From base class CoeControl

        /**
         * From CoeControl
         * See base class definition
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * From CoeControl
         * See base class definition
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

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
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

// From base class MEikListBoxObserver

        /**
         * From MEikListBoxObserver
         * See base class definition
         */
        void HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                  TListBoxEvent aEventType );


    private:

        /**
         * Updates listbox data
         * @since S60 5.1
         * @param None
         * @return None
         */
        void UpdateListBoxDataL();

        /**
         * Checks if servers are not set and shows info note
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void CheckServersAndShowNoteL();

    private:

        /**
         * Listbox control
         */
        CAknColumnListBox*                  iListBox;           // owned
        /**
         * pointer to AppUi object
         */
        CMSAppUi&                           iMSAppUi;           // not owned
        /**
         * All store rules
         */
        CCmStoreRuleContainer*              iStoreRuleContainer;// not owned
        /**
         * list selection indexes
         */
        CArrayFix<TInt>*                    iSelectionArray;    // owned
        /*
         * Containers view
         */
        CMSStoreListView&                   iView;
        /**
         * Context pane icon
         */
        CFbsBitmap*                         iIcon;              // not owned
        /**
         * Context pane icon mask
         */
        CFbsBitmap*                         iIconMask;          // not owned
    };

#endif // C_MSSTORELISTCONTAINER_H