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
* Description:  Definition for MediaServant fill rule editor class
*
*/


#ifndef C_MSSETTINGITEMS_H
#define C_MSSETTINGITEMS_H

#include <aknsettingitemlist.h>

// CLASS DECLARATIONS

/**
 * CMSTextSettingItem
 * Text setting item class
 */
class CMSTextSettingItem : public CAknTextSettingItem
    {
    public:
        CMSTextSettingItem( TInt aIdentifier, TDes& aText );

    public:
        /**
         * Setting page acceptance status
         *
         * @since S60 5.1
         * @return TBool, ETrue if accepted EFalse if cancelled
         */
        TBool SettingAccepted();
        /**
         * Sets setting page accept state
         *
         * @since S60 5.1
         * @param aState, ETrue if accepted
         */
        void SetAcceptState( TBool aState );

    private:
        /*
         * Setting page acceptance status
         */
        TBool iOKPressed;
    };

#endif // C_MSSETTINGITEMS_H