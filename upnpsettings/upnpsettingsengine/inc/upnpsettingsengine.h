/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Settings Engine class definition
 *
*/






#ifndef __UPNPSETTINGSENGINE_H__
#define __UPNPSETTINGSENGINE_H__

// INCLUDE FILES

#include <e32base.h>    // CBase
#include <e32std.h>     // TBuf

// CONSTANTS
const TInt KMaxFriendlyNameLength( 64 );

// FORWARD DECLARATIONS
class CRepository;
class CUpnpSettings;
class CUpnpMediaServerSettings;

// CLASS DECLARATION
/**
 *  Settings engine class for UPnP Home Connect Application
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
class CUPnPSettingsEngine : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUPnPSettingsEngine* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUPnPSettingsEngine* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUPnPSettingsEngine();
        
        
public: // new functions

    /**
     * Sets friendly name
     * @since S60 3.1
     * @param aFriendlyName, reference containing the new friendly name
     * @return TInt,  indicates the success of the call
     */
    IMPORT_C TInt SetLocalFriendlyName( 
        const TDesC8& aFriendlyName ) const;

    /**
     * Gets friendly name
     * @since S60 3.1
     * @param aFriendlyName, reference where name is updated
     * @return TInt, indicates the success of the call
     */
    IMPORT_C TInt GetLocalFriendlyName( 
        TDes8& aFriendlyName ) const;


    /**
     * Sets selected Access point
     * @since S60 3.1
     * @param aIAP, the IAP id
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetAccessPoint( const TInt aIAP );
        
        
    /**
     * Gets Access Point information
     * @since S60 3.1
     * @param aIAP, reference where IAP id is put
     * @return TInt, indicates the success of the call
     */
    IMPORT_C TInt GetAccessPoint( TInt& aIAP );
        
        
    /**
     * Sets selected Access point setting
     * @since S60 3.1
     * @param aIAPSetting, the IAP setting
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetAccessPointSetting( const TInt aIAPSetting );
        
        
    /**
     * Gets Access Point setting
     * @since S60 3.1
     * @param aIAPSetting, reference to access point setting
     * @return TInt, indicates the success of the call
     */
    IMPORT_C TInt GetAccessPointSetting( TInt& aIAPSetting );
        
        
    /**
     * Sets selected wap access point id
     * @since S60 3.1
     * @param aWapId, the wap access point id
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetWapId( const TInt aWapId );
        
        
    /**
     * Gets selected wap access point id
     * @since S60 3.1
     * @param aWapId, reference to wap access point id
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt GetWapId( TInt& aWapId );

    /**
     * Gets information if this is the first start of home network
     * @since S60 3.1
     * @param aFirstStart, reference to first start information
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt GetFirstStart( TInt& aFirstStart );

    /**
     * Sets information if this is the first start of home network
     * @since S60 3.1
     * @param const aFirstStart, the new first start value
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetFirstStart( const TInt aFirstStart );
        
    /**
     * Sets the mandatory manufacturer settings to media server
     * @since S60 3.1
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetManufacturerParamsL() const;

    /**
     * Gets the drive for the copy operation
     * @since S60 3.2
     * @param aDrive, reference to copy location drive
     */
    IMPORT_C void GetCopyLocationDriveL( TDriveNumber& aDrive ) const;

    /**
     * Gets the location for the copy operation
     * @since S60 3.2
     * @param aLocation, path for copy location
     * @param aIsPhoneMemory, tells if used location is internal phone memory
     */
    IMPORT_C void GetCopyLocationL( TDes& aLocation, 
                                    TBool& aIsPhoneMemory ) const;

    /**
     * Sets the location of the copy operation
     * @since S60 3.2
     * @param aDrive The drive number of the new location 
     * as defined in TDriveNumber.
     * @return TInt, TInt indicates the success of the call
     */
    IMPORT_C TInt SetCopyLocationL( const TDriveNumber aDrive ) const;

    /**
     * Get the name of the current IAP if it is set and is not always ask
     * @since S60 5.1
     * @param aIapId, iap id
     * @return HBufC, iap name. Ownership is transferred to the caller
     * if IAP is set to None
     */
    IMPORT_C static HBufC* GetCurrentIapNameL( TInt aIapId );
    
    /**
     * Static helper method to validate iap by id.
     * 
     * @since S60 5.1
     * @param aIapId, Id of the iap to be validated.
     * @return ETrue if given iap id is valid. EFalse otherwise.
     */
    IMPORT_C static TBool IsAccessPointValidL( TInt aIapId );
        
    /**
     * Static helper method to retrieve info of access points into given
     * arrays.
     * 
     * @since S60 5.1
     * @param aNameArray,  array contains the iap names
     * @param aIapIdArr, Array for iap ids.
     */
    IMPORT_C static void GetWLANAccessPointsL( CDesCArray* aNameArray,
                                               RArray<TInt64>& aIapIdArr );
    
    /**
     * Static helper method to create the iap editing window
     * arrays.
     * 
     * @since S60 5.1
     * @param none
     */        
    IMPORT_C static void CreateAccessPointL();
    
private:

    /**
     * C++ default constructor.
     */
    CUPnPSettingsEngine();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Reads device model, it is in form "Nokia xxxx"
     * Note: Does not return correct value in subcon R&D builds
     * @since S60 3.1
     * @param aModel Device model
     * @return TInt, TInt indicates the success of the call
     */
    TInt ReadDeviceModelL( TDes8& aModel ) const;

    /**
     * Gets friendly name, internal version
     * @since S60 3.1
     * @param aFriendlyName, reference where name is updated
     * @return TInt, indicates the success of the call
     */
    TInt GetLocalFriendlyNameL( 
        TDes8& aFriendlyName ) const;

    /**
     * Initializes a property if property has not yet been set
     * @since S60 3.1
     * @param aSettings, MediaServer settings 
     * @param aKey, Propery's id
     * @param aValue, New propery's value
     */
    void InitializePropertyL( 
        CUpnpMediaServerSettings& aSettings, 
        TInt aKey, 
        const TDesC8& aValue ) const;
    
    /**
     * Helper method to validate wlan access points.
     * 
     * @since S60 5.1
     * @param aIapId, Id of the iap to be validated.
     * @return ETrue if given iap is found from the comms db and
     * id is valid. EFalse otherwise.
     */
    static TBool IsWLANAccessPointValidL( TUint32 aIapId );

private:    // data

    // Central Repository
    CRepository*                    iRepository;
    // Repository for upnp settings
    CUpnpSettings*                  iUpnpRepository;
    
    };

#endif  // __UPNPSETTINGSENGINE_H__

// End of file
