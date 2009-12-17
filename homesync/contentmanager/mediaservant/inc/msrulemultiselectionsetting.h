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
* Description:  Definition for multiselection setting class
*
*/


#ifndef C_MSRULEMULTISELECTIONSETTING
#define C_MSRULEMULTISELECTIONSETTING

// INCLUDES
#include "mssettingitems.h"

// FORWARD DECLARATIONS
class CSelectionItemList;
class CCmSqlPropertyContainer;

// CLASS DECLARATION

/**
 * CMSRuleMultiSelectionSetting class. This class is used when
 * server list is modified
 *
 * @Since S60 5.1
*/
NONSHARABLE_CLASS( CMSRuleMultiselectionSetting ): public CMSTextSettingItem
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, resource identifier
         * @param aFirstPopupItemResource, first list item text resource id
         * @param aAnyItemTextRestource, 'any' item text resource
         * @param aMetadataArray, metadata list
         * @param aArray, selection list
         * @param aItemTextResource, item text resource when multiple
         *                           items selected
         * @param aText, dummy parameter - needed in item creation
         * @return None
         */
        static CMSRuleMultiselectionSetting* NewL( TInt aIdentifier,
                                    TInt aTitleResource,
                                    TInt aFirstPopupItemResource,
                                    TInt aAnyItemTextResource,
                                    CCmSqlPropertyContainer* aMetadataArray,
                                    RArray<TInt>* aArray,
                                    TInt aItemTextResource,
                                    TDes& aText);

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, resource identifier
         * @param aFirstPopupItemResource, first list item text resource id
         * @param aAnyItemTextRestource, 'any' item text resource
         * @param aMetadataArray, metadata list
         * @param aArray, selection list
         * @param aItemTextResource, item text resource when multiple
         *                           items selected
         * @param aText, dummy parameter - needed in item creation
         * @return None
         */
        static CMSRuleMultiselectionSetting* NewLC( TInt aIdentifier,
                                    TInt aTitleResource,
                                    TInt aFirstPopupItemResource,
                                    TInt aAnyItemTextResource,
                                    CCmSqlPropertyContainer* aMetadataArray,
                                    RArray<TInt>* aArray,
                                    TInt aItemTextResource,
                                    TDes& aText);
                                    
        /**
        * Destructor.
        */
        virtual ~CMSRuleMultiselectionSetting();

        /**
         * Creates item list and updates listbox text
         */
        void CreateItemListL();

    protected:

// From base class CAknSettingItem

        /**
         * From CAknSettingItem
         * See base class definition
        */
        void EditItemL( TBool aCalledFromMenu );

    private:

        /**
         * Creates selection list for setting page
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void CreateSelectionItemListL();

        /**
         * Appends selected items to the list
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void AppendSelectedItemsToListL();

        /**
         * Sets setting item text
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        virtual void SetSettingItemTextL();

    private:

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aIdentifier, resource identifier
         * @param aFirstPopupItemResource, first list item text resource id
         * @param aAnyItemTextRestource, 'any' item text resource
         * @param aMetadataArray, metadata list
         * @param aArray, selection list
         * @param aItemTextResource, item text resource when multiple
         *                           items selected
         * @param aText, dummy parameter - needed in item creation
         * @return None
         */         
        CMSRuleMultiselectionSetting(
            TInt aIdentifier,
            TInt aTitleResource,
            TInt aFirstPopupItemResource,
            TInt aAnyItemTextResource,
            CCmSqlPropertyContainer* aMetadataArray,
            RArray<TInt>* aArray,
            TInt aItemTextResource,
            TDes& aText
            );

        /**
         * Performs the second phase construction.
         */
        void ConstructL();

    private:

        /**
         * selected item count
         */
        TInt                                iSelectedItemCount;
        /**
         * item text resource
         */
        TInt                                iItemTextResource;

        /**
         * setting item text
         */
        TDesC&                              iText;
        /**
         * item array for selection dialog
         */
        CSelectionItemList*                 iItemArray;     // owned
        /**
         * Server name string (external set)
         */
        HBufC16*                            iSettingText;   // owned
        /**
         * list of selected items
         */
         RArray<TInt>*                      iList;          // not owned
        /**
         * array of metadata items
         */
         CCmSqlPropertyContainer*           iMetadataArray; // not owned
        /**
         * title resource for multiselection dialog
         */
        TInt                                iTitleResource;
        /**
         * first item resource for multiselection dialog
         */
        TInt                                iFirstPopupItemResource;
        /**
         * secondary text resource
         */
        TInt                                iAnyItemTextResource;
        /**
         * Last selected item index
         */
        TInt                                iSelectedItemIndex;
    };

#endif // C_MSRULEMULTISELECTIONSETTING

// End of File
