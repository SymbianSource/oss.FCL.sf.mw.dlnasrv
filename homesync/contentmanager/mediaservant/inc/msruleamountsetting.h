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
* Description:  CMSRuleAmountSetting class definition
*
*/


#ifndef C_MSRULEAMOUNTSETTING_H
#define C_MSRULEAMOUNTSETTING_H

// INCLUDES
#include "mssettingitems.h"


// CLASS DECLARATION

/**
 * CMSRuleAmountSetting class. This class is used when
 * transfer time is modified
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSRuleAmountSetting ):
                    public CMSEnumeratedTextPopupSettingItem
    {
    public:

        /**
         * Constructor.
         */
        CMSRuleAmountSetting( TInt aIdentifier,
                              TInt &aLimitType,
                              TInt &aValue );

        /**
         * Destructor.
         */
        virtual ~CMSRuleAmountSetting();


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
         * Limit type
         */
        TInt&                                   iLimitType;
    
        /**
         * Amount of files
         */
        TInt&                                   iValue;
    
        /**
         * Button event on setting page
         */
        TAknSettingPageEvent                    iSettingPageEvent;
    };

#endif // C_MSRULEAMOUNTSETTING_H

// End of File
