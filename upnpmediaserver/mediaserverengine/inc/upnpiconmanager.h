/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares CUpnpIconManager class
*
*/



#ifndef C_CUPNPICONMANAGER_H
#define C_CUPNPICONMANAGER_H

// INCLUDES
#include <f32file.h>

// FORWARD DECLARATIONS
class CUpnpIcon;
class CUpnpDeviceImplementation;
class CUpnpDevice;
class CUpnpMediaServerDescriptionProvider;
class CUpnpDeviceDescriptionStore;

// CLASS DECLARATION

/**
 *  Class is responsible for MS icon managing
 *  Handles all request releated to icons
 *  Such as: Adding, Updating and Removing icons to/from MS description
 *  @since S60 v3.2
 */
class CUpnpIconManager: public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param aDescriptionFile file with device description xml
     * @param aDescriptionProvider description provider
     */
    static CUpnpIconManager* NewL( CUpnpDeviceDescriptionStore& iDescriptionStore,
                                   CUpnpMediaServerDescriptionProvider& aDescriptionProvider );

    /**
     * Two-phased constructor.
     * @param aDescriptionFile file with device description xml
     * @param aDescriptionProvider description provider
     */
    static CUpnpIconManager* NewLC( CUpnpDeviceDescriptionStore& iDescriptionStore,
                                    CUpnpMediaServerDescriptionProvider& aDescriptionProvider );

    /**
     * Destructor
     */
    ~CUpnpIconManager();
    
    /**
    * Adds icon
    * @param aIconDes icon description( serialized icon )
    */
    void AddIconL( const TDesC8& aIconDes );

    /**
    * Updates icon
    * @param aNewIconDes new icon description( serialized icon )
    * @param aUrl url of icon which will be updated
    */
    void UpdateIconL( const TDesC8& aNewIconDes, const TDesC8& aUrl );

    /**
    * Removes icon
    * @param url of icon which will be deleted
    */
    void RemoveIconL( const TDesC8& aUrl );

    /**
    * Removes all icons
    */
    void RemoveIconsL();

private:
    /**
    * Creates icon path
    * @return icon path
    */
    HBufC* GetIconPathLC();

    /**
    * Returns icon name
    * @param aPath
    * @return icon name
    */
    HBufC* GetIconNameLC( const TDesC8& aPath );

    /**
    * Copy icon to MS private directory
    * @param aIcon icon which will be copied
    * @param aDestinationPath icon's destination directory
    */
    void CopyIconToPrivateL( const CUpnpIcon* icon , const TDesC& destinationPath );

    /**
    * Prepare beckup copy of icon which is going to be updated
    * @param aPath path to icon which going to be beckuped
    * return pointer to beckup filename, used in rollback operation
    */
    HBufC* PrepareIconToRollbackLC( const TDesC& aPath );

    /**
    * Updates icon
    * @param aIcon new icon
    * @param aDestinationPath old icon path
    */
    void DoUpdateIconL( CUpnpIcon* aIcon, const TDesC& aDestinationPath );

    /**
    * Deletes icon from FS
    * @param aPath path to icon which is delete
    * @returns KErrNone if succesfull or other error code of failed
    */
    TInt DeleteIconFile( const TDesC& aPath );

    /**
    * Makes an attempt to restore icon from it's original path
    * @param aUrl icons URL
    */
    void RestoreFromOrginalPathL( const TDesC8& aUrl );

    /**
    * Checks if icon already exist in device iconList
    * Leaves with KErrNotFound if icon doesn't exist
    * @param aUrl an icon's URL to be checked
    */
    TInt FindIcon( const TDesC8& aUrl );

    /**
     * Adds all icons to the description property map.
     * 
     * @since S60 5.1
     */
    void AddIconsToDescriptionPropertyMapL();
    
private:

    /**
     * Two-phased constructor.
     * @param aDescriptionFile file with device description xml
     */
    void ConstructL();

    /**
     * Constructor.
     * @param aDescriptionProvider description provider
     */
    CUpnpIconManager( CUpnpDeviceDescriptionStore& iDescriptionStore,
                      CUpnpMediaServerDescriptionProvider& aDescriptionProvider );

private:
    
    /** File session handler - owned */
    RFs iFileSession;
    
    /** Icon list used for icon operation when device is not started */
    RPointerArray< CUpnpIcon > iIconList;
    
    /** Description provider - not owned */
    CUpnpMediaServerDescriptionProvider& iDescriptionProvider;
    
    /** Description store */
    CUpnpDeviceDescriptionStore& iDescriptionStore; 
    };


#endif//C_CUPNPICONMANAGER_H
