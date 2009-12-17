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
* Description:      Settings list definition for Settings dialog
*
*/







#ifndef UPNPAPPSETTINGSLIST_H
#define UPNPAPPSETTINGSLIST_H

// INCLUDES
#include <aknsettingitemlist.h>
#include "upnpfilesharingengineobserver.h"

// CLASS DECLARATION
class CUPnPSettingsEngine;
class CUPnPFileSharingEngine;

// DATA TYPES
enum TWLanSecurityMode
    {
    EWLanUnsecured = 1
    };

/**
 *  Settings list class declaration
 *  @since S60 3.1
 */
class CUPnPAppSettingsList : public CAknSettingItemList
    {
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static CUPnPAppSettingsList* NewL( TBool& aSharingState,
                                       TInt aResourceId );

    /**
     * Destructor.
     */
    virtual ~CUPnPAppSettingsList();
                
public: // New functions

    /**
     * Reads data from storage to member variables
     * @since S60 3.1
     */
    void LoadDataFromEngineL();

public: // Functions from base classes

    /**
     * From CAknSettingItemList, this launches the setting page.
     * @since S60 3.1
     * @param aIndex, index of selected list item
     * @param aCalledFromMenu, indicates if editing is called
     *        trough menu or from keyboard
     */
    void EditItemL ( TInt aIndex, TBool aCalledFromMenu );

protected:
    
    /**
     * From CAknSettingItemList        
     */
    void HandleResourceChange( TInt aType );  

private:

    /**
     * C++ default constructor.
     */
    CUPnPAppSettingsList( TBool& aSharingState );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TInt aResourceId );

    /**
     * From CAknSettingItemList, Framework method to create a setting item.
     * @since S60 3.1
     * @param aIdentifier,  list item id to be created
     * @return CAknSettingItem, pointer to list item
     */
    CAknSettingItem* CreateSettingItemL( TInt aIdentifier );
        

private:    // Data

    // friendly name of my device
    TBuf<KMaxFileName>      iFriendlyName;
    // access point ID
    TInt                    iIapId;
    // wap id
    TInt                    iWapId;
    // my device name
    HBufC8*                 iName;
    // Settings engine for saving and reading data
    CUPnPSettingsEngine*    iSettingsEngine;

    // IAP ui selection (always ask, user defined or none)
    TInt                    iIapSetting;

    // The drive to store copied files
    TDriveNumber            iCopyLocationDrive;

    // The status of the sharing (on/off)
    TBool&                  iSharingState;
    };

#endif      // UPNPAPPSETTINGSLIST_H

// End of File
