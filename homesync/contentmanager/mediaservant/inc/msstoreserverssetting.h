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
* Description:  CMSStoreServersSetting class definition
*
*/


#ifndef C_MSSTORESERVERSSETTING_H
#define C_MSSTORESERVERSSETTING_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMSAppUi;
class CCmMediaServerFull;
class CMSAppUi;
class CSelectionItemList;
class CCmStoreRule;


// CLASS DECLARATION

/**
 * CMSStoreServersSetting class. This class is used when
 * server list is modified
 *
 * @since S60 5.1
*/
NONSHARABLE_CLASS( CMSStoreServersSetting ): public CAknTextSettingItem
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, setting resource id
         * @param aAppUi, application UI
         * @param aText, dummy - just for setting creation
         * @return pointer to CMSStoreServersSetting
         */
        static CMSStoreServersSetting* NewL( TInt aIdentifier,
                                      CCmStoreRule* aRule,
                                      CMSAppUi& aAppUi,
                                      TDes& aText );

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, setting resource id
         * @param aAppUi, application UI
         * @param aText, dummy - just for setting creation
         * @return None
         */
        static CMSStoreServersSetting* NewLC( TInt aIdentifier,
                                      CCmStoreRule* aRule,
                                      CMSAppUi& aAppUi,
                                      TDes& aText );
                                      
        /**
        * Destructor.
        */
        virtual ~CMSStoreServersSetting();


    protected: 

        /**
         * Creates selection list for setting page
         * @since S60 3.1
         */
        void CreateSelectionItemListL();

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
        
// From base class CAknSettingItem

        /**
         * From CAknSettingItem
         * See base class definition
         */
        void EditItemL( TBool aCalledFromMenu );


    private:

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aIdentifier, setting resource id
         * @param aAppUi, application UI
         * @param aText, dummy - just for setting creation
         * @return None
         */
        CMSStoreServersSetting(
            TInt aIdentifier,
            CCmStoreRule* aRule,
            CMSAppUi& aAppUi,
            TDes& aText
            );
        
        /**
         * Performs the second phase construction.
         */
        void ConstructL();


        /**
         * Deletes all servers from rule
         *
         * @since S60 5.1
         */
        void DeleteAllServersFromRuleL();

    private:
    
        /**
         * server list
         */
        RPointerArray<CCmMediaServerFull>*  iServers;           // owned
        /**
         * current rule
         */
        CCmStoreRule*                       iRule;              // not owned
        /**
         * application ui
         */
        CMSAppUi&                           iAppUi;
        /**
         * setting page item array (servers)
         */
        CSelectionItemList*                 iItemArray;         // owned
        /**
         * Server name string (external set)
         */
        HBufC*                              iSettingText;       // owned
        /**
         * Selected item count
         */
        TInt                                iSelectedItemCount;
    };

#endif // C_MSSTORESERVERSSETTING_H

// End of File
