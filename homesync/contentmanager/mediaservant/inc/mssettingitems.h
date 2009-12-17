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
 *
 * @Since S60 5.1
 */
class CMSTextSettingItem : public CAknTextSettingItem
    {
    public:
        /**
         * Constructor
         *
         * @Since S60 5.1
         * @param aIdentifier, resource id
         * @param aText, setting item text
         * @return None
         */
        CMSTextSettingItem( TInt aIdentifier, TDes& aText );
    
    protected:
    
// From base class MAknSettingPageObserver
    
        /**
         * From MAknSettingPageObserver
         * See base class definition
         */
        void  HandleSettingPageEventL ( CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType );
    public:
    
        /* Setting page acceptance status
         * @since S60 5.1
         * @param None
         * @return TBool, ETrue if accepted EFalse if cancelled
         */
        TBool SettingAccepted();
        
        /**
         * Sets setting page accept state
         * @since S60 5.1
         * @param aState, ETrue if accepted
         * @return None
         */
        void SetAcceptState( TBool aState );

        /**
         * Gets settingitem text
         * @since S60 5.1
         * @param None
         * @return TPtr&, setting item text
         */
        TPtr& Text(); 

        /**
         * Sets settingitem text
         * @since S60 5.1
         * @param text, new text
         * @return None
         */
        void SetText( TDesC& text );

    private:

        /*
         * Setting page acceptance status
         */
        TBool iOKPressed;
    };

/**
 * CMSEnumeratedTextPopupSettingItem
 * Enumerated text popup setting item class
 *
 * @Since S60 5.1 
 */
class CMSEnumeratedTextPopupSettingItem : 
    public CAknEnumeratedTextPopupSettingItem
    {
    public:

        /*
         * Constructor
         *
         * @Since S60 5.1
         * @param aIdentifier, resource id
         * @param aValue, selected value
         * @return None
         */
        CMSEnumeratedTextPopupSettingItem( TInt aResourceId, TInt& aValue );

    protected:

// From base class MAknSettingPageObserver

        /**
         * From MAknSettingPageObserver
         * See base class definition
         */
        void  HandleSettingPageEventL ( CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType );
    public:
        /* Setting page acceptance status
         * @since S60 5.1
         * @return TBool, ETrue if accepted EFalse if cancelled
         */
        TBool SettingAccepted();

        /**
         * Sets setting page accept state
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

/**
 * CMSTimeOrDateSettingItem
 * Time and date setting item class
 *
 * @since S60 5.1
 */
class CMSTimeOrDateSettingItem : public CAknTimeOrDateSettingItem
    {
    public:
        /*
         * Constructor
         *
         * @Since S60 5.1
         * @param aIdentifier, resource id
         * @param aMode, time or date mode
         * @param aTime, time value
         * @return None
         */
        CMSTimeOrDateSettingItem( TInt aCMSIdentifier,
            TInt aIdentifier,
            CAknTimeOrDateSettingItem::TAknTimeOrDateSettingItemMode aMode,
            TTime& aTime );
            
    protected:
    
// From base class MAknSettingPageObserver
        
        /**
         * From MAknSettingPageObserver
         * See base class definition
         */
        void  HandleSettingPageEventL ( CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType );
    public:
        /* Setting page acceptance status
         * @since S60 5.1
         * @return TBool, ETrue if accepted EFalse if cancelled
         */
        TBool SettingAccepted();

        /**
         * Sets setting page accept state
         * @since S60 5.1
         * @param aState, ETrue if accepted
         */
        void SetAcceptState( TBool aState );
   
        /**
         * This launches the setting page for Time or Date editing
         * @since S60 5.1
         * @param aState, ETrue if from menu called
         */  
        void EditItemL( TBool aCalledFromMenu );   

    private:
        /*
         * Setting page acceptance status
         */
        TBool  iOKPressed;
        /*
         * Internal time
         */
        TTime& iCMSInternalTime;
        /*
         * Resource identifier(From / Util)
         */
         TInt  iCMSIdentifier;
         
    };

#endif // C_MSSETTINGITEMS_H