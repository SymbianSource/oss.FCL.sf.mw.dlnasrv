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
* Description:  CMSRuleServersSetting class definition
*
*/


#ifndef C_MSRULESERVERSSETTING_H
#define C_MSRULESERVERSSETTING_H

// INCLUDES
#include "mssettingitems.h"

// FORWARD DECLARATIONS
class CSelectionItemList;


// CLASS DECLARATION

/**
 * CMSRuleServersSetting class. This class is used when
 * server list is modified
 *
 *
 * @Since S60 5.1
*/
NONSHARABLE_CLASS( CMSRuleServersSetting ): public CMSTextSettingItem
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @Since S60 5.1
         * @param aIdentifier, setting resource id
         * @param aText, dummy text - needed on creation
         * @param aDefaultItemTextResource
         * @param aSecondaryTextResource, resource id for
         *                                text 'no items' case
         */
        static CMSRuleServersSetting* NewL( TInt aIdentifier,
                                      CSelectionItemList& aServerList,
                                      TDes& aText,
                                      TInt aSecondaryTextResource );

        /**
         * Two-phased constructor.
         *
         * @Since S60 5.1
         * @param aIdentifier, setting resource id
         * @param aText, dummy text - needed on creation
         * @param aDefaultItemTextResource
         * @param aSecondaryTextResource, resource id for
         *                                text 'no items' case
         */
        static CMSRuleServersSetting* NewLC( TInt aIdentifier,
                                      CSelectionItemList& aServerList,
                                      TDes& aText,
                                      TInt aSecondaryTextResource );
                                      
        /**
        * Destructor.
        */
        virtual ~CMSRuleServersSetting();

        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();

    protected: // From CAknSettingItem

        /**
        * This method activates the transfer "setting page"
        */
        void EditItemL( TBool aCalledFromMenu );

    protected: // New functions

        /**
         * Counts selected items
         * @since S60 3.1
         * @return TInt, item count
         */
        TInt CountSelectedItems();

        /**
         * Sets setting item secondary text according to selected devices
         * @since S60 3.1
         */
        void SetSettingItemTextL();

    protected:

        /**
         * Constructor.
         */
        CMSRuleServersSetting(
            TInt aIdentifier,
            CSelectionItemList& aServerList,
            TDes& aText,
            TInt aSecondaryTextResource
            );

    protected:
        /**
         * Server name string
         */
        TDesC&                              iText;
        /**
         * setting page item array (servers)
         */
        CSelectionItemList&                 iItemArray;
        /**
         * Server name string (external set)
         */
        HBufC16*                            iSettingText;           // owned
        /**
         * Selected item count
         */
        TInt                                iSelectedItemCount;
        /**
         * Default item secondary text resource
         */
        TInt                                iSecondaryTextResource;
    };

#endif // C_MSRULESERVERSSETTING_H

// End of File
