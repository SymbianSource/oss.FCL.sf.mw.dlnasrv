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
* Description:  CMSRuleFileSizeSetting class definition
*
*/


#ifndef C_MSRULEFILESIZESETTING_H
#define C_MSRULEFILESIZESETTING_H

// INCLUDES
#include "mssettingitems.h"

// CLASS DECLARATION

/**
 * CMSRuleFileSizeSetting class. This class is used when
 * transfer time is modified
 *
 * @since S60 5.1
*/
NONSHARABLE_CLASS( CMSRuleFileSizeSetting ):
                    public CMSEnumeratedTextPopupSettingItem
    {
    public:

        /**
         * Constructor.
         *
         * @since S60 5.1
         * @param aIdentifier, resource identifier
         * @param aType, setting type ( min / max size )
         * @param aSelection, selection index
         * @param aValue, size value
         * @return None
         */
        CMSRuleFileSizeSetting(
            TInt aIdentifier,
            TInt aType,
            TInt &aSelection,
            TInt &aValue
            );

        /**
         * Destructor.
         */
        virtual ~CMSRuleFileSizeSetting();


    protected: 
   
// From base class CAknSettingItem

        /**
         * From CAknSettingItem
         * See base class definition
         */
        void EditItemL( TBool aCalledFromMenu );
        
        /**
         * From CAknSettingItem
         * See base class definition
         */
        void CompleteConstructionL();

    private:
        
        /**
         * Setting type Min/Max
         */
        TInt                                    iType;
        
        /**
         * File size in MBs
         */
        TInt&                                   iValue;
    };

#endif // C_MSRULEFILESIZESETTING_H

// End of File
