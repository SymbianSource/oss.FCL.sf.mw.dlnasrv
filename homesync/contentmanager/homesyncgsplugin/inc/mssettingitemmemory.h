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
* Description:  Definition for MediaServant memory setting item
*
*/


#ifndef MSSETTINGITEMMEMORY_H
#define MSSETTINGITEMMEMORY_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMSEngine;
class CSelectionItemList;
class CMSSettingItemList;

// CLASS DECLARATION

/**
 * CMSSettingItemmemory class.
 */
NONSHARABLE_CLASS( CMSSettingItemMemory ): public CAknTextSettingItem
    {
    public: // Constructor and destructor

        /**
         * NewL
         * 2-phase constructor.
         *
         * @param aIdentifier, setting item identifier
         * @param aText, setting item text
         * @param aMSEngine, application engine
         * @param aParent, parent list
         * @return CMSSettingItemMemory*, the new CMSSettingItemMemory object
         */
        static CMSSettingItemMemory* NewL( TInt aIdentifier,
                                        TDes& aText,
                                        CMSEngine& aMSEngine,
                                        CMSSettingItemList& aParent );

        /**
         * NewLC
         * 2-phase constructor.
         *
         * @param aIdentifier, setting item identifier
         * @param aText, setting item text
         * @param aMSEngine, application engine
         * @param aParent, parent list
         * @return CMSSettingItemMemory*, the new CMSSettingItemMemory object
         */
        static CMSSettingItemMemory* NewLC( TInt aIdentifier,
                                        TDes& aText,
                                        CMSEngine& aMSEngine,
                                        CMSSettingItemList& aParent );

       /**
        * Destructor.
        */
        virtual ~CMSSettingItemMemory();


    protected: // From CAknSettingItem

       /**
        * This method activates the memory "setting page"
        */
        void EditItemL( TBool aCalledFromMenu );

    private: // New functions

        /**
         * Creates selection list for drive selection
         *
         * @since S60 5.1
         */
        void CreateSelectionListL();

        /**
         * Sets storage information to settings
         *
         * @since S60 5.1
         */
        void SetStorageInformationL();

        /**
         * Sets correct text setting item secondary text
         *
         * @since S60 5.1
         */
        void SetSettingItemTextL();

    private:

         /**
          * Constructor.
          */
        CMSSettingItemMemory(
            TInt aIdentifier,
            TDes& aText,
            CMSEngine& aMSEngine,
            CMSSettingItemList& aParent
            );

        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();

    private:

        /**
         * application ui
         */
        CMSEngine&                          iMSEngine;
        
        /**
         * application ui
         */
        CMSSettingItemList&                 iParent;
        
        /**
         * array of available memory cards - needed for settingpage
         */
        CSelectionItemList*                 iItemArray;     // owned
        
        /**
         * drive info array
         */
        RPointerArray<CCmDriveInfo>         iDriveArray;    // owned
    };

#endif // MSSETTINGITEMMEMORY_H

// End of File
