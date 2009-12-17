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
* Description:      Declares home network IAP setting item class to home network
*                setting item list
*
*/






#ifndef UPNPAPPSETTINGITEMHOMEIAP_H
#define UPNPAPPSETTINGITEMHOMEIAP_H


// INCLUDES
#include <aknsettingitemlist.h>


// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;


// CLASS DECLARATION

/**
* UPnPAppSettingItemHomeIAP class. This class is used when
* creating home network IAP setting item to setting item list
* of home network application settings dialog.
*
* @lib CoverDisplaySettings.app
*/
class UPnPAppSettingItemHomeIAP: public CAknEnumeratedTextPopupSettingItem
    {
    public: // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static UPnPAppSettingItemHomeIAP* NewL(TInt aIdentifier,
                                               TInt& aIAPSetting,
            TInt& aIAPId, TInt& aWAPId, TBool& aSharingState );

        /**
        * Destructor.
        */
        virtual ~UPnPAppSettingItemHomeIAP();

    public: // From CAknEnumeratedTextPopupSettingItem

        /**
        * Returns setting text according to home network IAP setting.
        * This text is visible in this setting item.
        *
        * @return home network IAP setting/name text
        */
        const TDesC& SettingTextL();

        /**
        * Framework calls this method in order to create
        * and execute setting page
        */
        void CreateAndExecuteSettingPageL();

    private: // From CAknEnumeratedTextPopupSettingItem

        /**
        * Constructor.
        */
        UPnPAppSettingItemHomeIAP(TInt aIdentifier, TInt& aIAPSetting,
            TInt& aIAPId, TInt& aWAPId, TBool& aSharingState );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New methods

        /**
        * Changes iap name according to iap id
        */
        void ChangeIAPNameL();

        /**
        * Checks if access point is unsecured and warns user
        * @since S60 3.1
        * @param aAccessPoint, selected access point
        * @return TInt, security mode
        */
        TInt CheckAPSecurityL(TInt aAccessPoint);

        /**
        * Displays message query
        * @since S60 3.1
        * @param aHeaderResourceId, resource id of the title
        * @param aMessageResourceId, resource id of the message
        * @param TIaDialogResourceIdnt, resource id of the dialog
        */
        void DisplayInfoL(TInt aHeaderResourceId,
                          TInt aMessageResourceId,
                          TInt aDialogResourceId);

        
        /**
        * Adds static items to the list
        * @since S60 3.1
        * @param aArray, list array
        */
        void AddStaticItemsL(CDesCArray* aArray);
        
        /**
        * Use previous iap
        * @since S60 5.1
        * @param aOldIap, previous iap id
        */
        void UsePreviousIapL( TInt aOldIap );
        
        /**
        * Get iap index
        * @since S60 5.1
        * @param aStaticCount the number of static items
        * @return Index of iap
        */
        TInt GetIAPIndex( TInt aStaticCount );

    private:

        // pointer to home network IAP text
        HBufC* iItemText;

        // pointer to home network IAP name
        HBufC* iIAPName;

        // home network IAP ui selection (always ask, user defined or none)
        TInt& iIAPSetting;

        // home network IAP id
        TInt& iIAPId;

        // home network IAP wap id
        TInt& iWAPId;

        // iap ids
        RArray<TInt64> iIAPIdArray;

        // Control environment
        CCoeEnv*    iCoeEnv;

        // sharing status
        TBool& iSharingState;
    };

#endif

// End of File
