/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Definition for UPnP memory selection setting item
*
*/






#ifndef C_MSSETTINGITEMMMC_H
#define C_MSSETTINGITEMMMC_H

// INCLUDES
#include <aknsettingitemlist.h>
#include <CAknMemorySelectionSettingItemMultiDrive.h>

// CLASS DECLARATION

/**
 * CUPnPSettingItemMemorySelection class.
 */
NONSHARABLE_CLASS( CUPnPSettingItemMemorySelection ): 
    public CAknMemorySelectionSettingItemMultiDrive
    {
    public: // Constructor and destructor

       

        /**
         * NewL
         * 2-phase constructor.
         */
        static CUPnPSettingItemMemorySelection* NewL( TInt aIdentifier,
                                        TDriveNumber& aSelectedDrive );


        /**
        * Destructor.
        */
        virtual ~CUPnPSettingItemMemorySelection();


    protected: // From CAknSettingItem

        /**
        * This method activates the memory selection dialog
        */
        void EditItemL( TBool aCalledFromMenu );
        
    private:


        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();
        
          /**
          * Constructor.
          */
        CUPnPSettingItemMemorySelection(
            TInt aIdentifier,
            TDriveNumber& aSelectedDrive );        

    };

#endif // C_MSSETTINGITEMMMC_H

// End of File
