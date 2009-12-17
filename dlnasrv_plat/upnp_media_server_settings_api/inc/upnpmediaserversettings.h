/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media Server Settings
*
*/


#ifndef C_CUPNPMEDIASERVERSETTINGS_H
#define C_CUPNPMEDIASERVERSETTINGS_H

// INCLUDES


#include <e32base.h>

// CONSTANTS

const TUid KHnCRUidMediaServer = { 0x2000f87f }; //for parallel stack
const TUid KCRUidMediaServer   = { 0x101F978F };


// FORWARD DECLARATIONS
class CRepository;


namespace UpnpMediaServerSettings 
{
    
_LIT8( KFriendlyName, "friendlyName" );
_LIT8( KModelUrl, "modelURL" );
_LIT8( KModelDescription, "modelDescription" );
_LIT8( KModelNumber, "modelNumber" );
_LIT8( KSerialNumber, "serialNumber" );
_LIT8( KPresentationUrl, "presentationURL" );
_LIT8( KModelName, "modelName" );
_LIT8( KManufacturer, "manufacturer" );
_LIT8( KManufacturerUrl, "manufacturerURL" );

enum TMediaServerKeys 
    {
    EFriendlyName = 1, 
    EUploadDirectory = 2, 
    ESupportedMedia = 3, 
    ESecurityManager = 4,
    EModelUrl = 5,
    EModelDescription = 6,
    EModelNumber = 7,
    ESerialNumber = 8,
    EPresentationUrl = 9,
    EModelName = 10,
    EManufacturer = 11,
    EManufacturerUrl = 12,
    EUpnpMediaServerThumbnails = 13,
    KUpnpMediaServerBrowseLimit = 14
    };

}


/**
 *  Observer class for getting information about setting changes
 *
 *  @since S60 S60 v3.1
 */
class MUpnpSettingObserver
	{
public:
   	/**
	* This function will be invoke if setting change
	*/
	virtual void SettingChangedL() = 0;
	};
	
	
/**
 *  Class wraps Central Repository settings for Media Server.
 *  Those changes are applied only to Central Repository.
 *  In order to make them active Media Server must be restarted. 
 *
 *  @since S60 S60 v3.1
 */
class CUpnpMediaServerSettings : public CActive
    {

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpMediaServerSettings* NewL();

    /**
     * Destructor.
     */
    virtual ~CUpnpMediaServerSettings();


public: // New functions
    /**
     * Sets numeric value.
     * @param aId parameter id
     * @return error code
     */
    IMPORT_C TInt Set(TUint aId, const TInt& aValue);

    /**
     * Sets string value.
     * @param aId parameter id
     * @return error code
     */
    IMPORT_C TInt Set(TUint aId, const TDesC& aValue);

    /**
     * Sets string value.
     * @param aId parameter id
     * @return error code
     */
    IMPORT_C TInt SetL(TUint aId, const TDesC8& aValue);    
    
    /**
     * Gets numeric value.
     * @param aId parameter id
     * @return error code
     */
    IMPORT_C TInt Get(TUint aId, TInt& aValue);

    /**
     * Gets string value.
     * @param aId parameter id
     * @return error code
     */
    IMPORT_C TInt Get(TUint aId, TDes& aValue);

    /**
     * Gets string value.
     * @param aId parameter id
     * @return string value
     */
    IMPORT_C HBufC8* GetL(TUint aId);
    
    /**
     * Subcribe notification about setting change
     * Onlu one subsription can be pending for one instance of class
     * @param aId parameter id
     * @param aObserver interface to notify
     * @return error code
     */
    IMPORT_C TInt Subscribe(TUint aId, MUpnpSettingObserver* aObserver);
        

private:
    CUpnpMediaServerSettings();
    void ConstructL(TUid aRepositoryId);

    /**
    * From CActive.
    * Invoke when asynchronous request will be completed
    *
    * @since S60 S60 v3.1
    */
    void RunL();

    /**
    * From CActive.
    * Invoke when RunL leaves
    */
    TInt RunError( TInt aErr );    

    /**
    * From CActive.
    * Invoke for canceling asynchronous request
    *
    * @since S60 S60 v3.1
    */	
    void DoCancel();
	
private: 
    /**
    * Central repository client
    */	
    CRepository* iRepository;
    
    /**
    * Observer which should be informed in case of changes
    */
    MUpnpSettingObserver* iObserver;
    };

#endif // C_CUPNPMEDIASERVERSETTINGS_H

// End of File
