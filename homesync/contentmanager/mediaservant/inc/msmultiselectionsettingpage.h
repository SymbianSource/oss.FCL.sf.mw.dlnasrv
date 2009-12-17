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
* Description:  Definition for MediaServant multiselection setting page
*
*/


#ifndef C_MSMULTISELECTIONSETTINGPAGE_H
#define C_MSMULTISELECTIONSETTINGPAGE_H

// INCLUDES
#include <akncheckboxsettingpage.h>

// FORWARD DECLARATIONS
class CSelectionItemList;


// CLASS DECLARATION

/**
* CMSMultiselectionSettingPage class.
*/
NONSHARABLE_CLASS( CMSMultiselectionSettingPage ):
                            public CAknCheckBoxSettingPage
    {
    public: // Constructor and destructor

        /**
         * two-phase constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, setting page resource id
         * @param aItemArray, item list
         * @param aFirstItemSwitch, set ETrue if 'any' item is shown, EFalse
         *                          otherwise
         * @return pointer to CMSMultiselectionSettingPage
         */
        static CMSMultiselectionSettingPage* NewL( TInt aIdentifier,
                                      CSelectionItemList& aItemArray,
                                      TBool aFirstItemSwitch );

        /**
         * two-phase constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, setting page resource id
         * @param aItemArray, item list
         * @param aFirstItemSwitch, set ETrue if 'any' item is shown, EFalse
         *                          otherwise
         * @return pointer to CMSMultiselectionSettingPage
         */
        static CMSMultiselectionSettingPage* NewLC( TInt aIdentifier,
                                      CSelectionItemList& aItemArray,
                                      TBool aFirstItemSwitch );
        /**
        * Destructor.
        */
        virtual ~CMSMultiselectionSettingPage();


    protected: 

// From base class CAknCheckboxSettingPage

        /**
         * From CAknCheckboxSettingPage
         * See base class definition
         */
        void SelectCurrentItemL();

        /**
         * From CAknCheckboxSettingPage
         * See base class definition
         */
        TBool OkToExitL(TInt aButtonId);

    private:

        /**
         * Deselects other items if first one is selected
         * @since S60 5.1
         * @param None
         * @return None
         */
        void HandleSelectionsL();

    private:

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aIdentifier, setting page resource id
         * @param aItemArray, item list
         * @param aFirstItemSwitch, set ETrue if 'any' item is shown, EFalse
         *                          otherwise
         * @return None
         */
        CMSMultiselectionSettingPage(
            TInt aIdentifier,
            CSelectionItemList& aItemArray,
            TBool aFirstItemSwitch
            );

        /**
         * Performs the second phase construction.
         */
        void ConstructL();

        /**
         * Updates selections to item list
         * @since S60 5.1
         * @param None
         * @return None
         */
        void UpdateAllSelections();
        
        /**
         * handle for the pointer device input
         * @since S60 5.1
         * @param None
         * @return None
         */
        void  HandlePointerEventL (const TPointerEvent &aPointerEvent);

    private:
        /**
         * setting page item array (servers)
         */
        CSelectionItemList*                 iItemArray;         // not owned
        /**
         * Flag for indicating that special selection is needed
         */
        TBool                               iFirstItemSwitch;
    };

#endif // C_MSMULTISELECTIONSETTINGPAGE_H

// End of File
