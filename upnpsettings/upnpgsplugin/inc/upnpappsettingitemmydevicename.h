/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Declares home network my device name setting item class to
*                home network setting item list
*
*/






#ifndef UPNPAPPSETTINGITEMMYDEVICENAME_H
#define UPNPAPPSETTINGITEMMYDEVICENAME_H


// INCLUDES
#include <aknsettingitemlist.h>


// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;


// CLASS DECLARATION

/**
* UPnPAppSettingItemMyDeviceName class. This class is used when
* creating home network my device name setting item to setting item list
* of home network application settings dialog.
*/
class UPnPAppSettingItemMyDeviceName: public CAknTextSettingItem
    {
    public: // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static UPnPAppSettingItemMyDeviceName* NewL(TInt aIdentifier,
                                                    TDes& aText,
                                                    TBool& aSharingState );

        /**
        * Destructor.
        */
        virtual ~UPnPAppSettingItemMyDeviceName();

    public: // From CAknTextSettingItem

        /**
        * Framework calls this method in order to create
        * and execute setting page
        */
        void EditItemL(TBool aCalledFromMenu);

    private: // From CAknTextSettingItem

        /**
        * Constructor.
        */
        UPnPAppSettingItemMyDeviceName(TInt aIdentifier,
                                       TDes& aText,
                                       TBool& aSharingState );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        // Sharing state
        TBool& iSharingState;
    };

#endif

// End of File
