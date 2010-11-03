/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
* Description:  Utility class to get path related info for the media files
*/






#ifndef UPNPPATHUTILITY_H
#define UPNPPATHUTILITY_H

// INCLUDE FILES
#include <e32base.h>    // CBase
#include <f32file.h>    // TDriveNumber

// FORWARD DECLARATIONS
class CUPnPSettingsEngine;
class CUpnpObject;
class CUpnpItem;
class CUpnpElement;
class CUpnpAttribute;
class CUpnpDlnaProtocolInfo;

// CLASS DECLARATION
/**
 *  Path utility class to get path related info for the media files
 *  @lib upnputilities.lib
 */
NONSHARABLE_CLASS ( CUPnPPathUtility ) : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUPnPPathUtility* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUPnPPathUtility* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUPnPPathUtility();
        
        
public: // new functions

    /**
     * Gets the drive for the copy operation
     * @param aDrive, reference to copy path drive
     */
    IMPORT_C void GetCopyPathDriveL( TDriveNumber& aDrive ) const;

    /**
     * Gets the path of the upnp item's file to be copied
     * @param aItem, a upnp item
     * @param aResource, resource of the upnp item
     * @param aAppendTitleAndExt, If ETrue title and extensions gets 
     *                            appended to the path otherwise not.
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    IMPORT_C HBufC* GetCopyPathL(const CUpnpItem& aItem,
                                 const CUpnpElement& aResource,
                                 TBool aAppendTitleAndExt ) const;

    /**
     * Gets the path of the upnp item's file to be copied
     * @param aItem, a upnp item
     * @param aResource, resource of the upnp item
     * @param aAppendTitleAndExt, If ETrue title and extensions gets 
     *                            appended to the path otherwise not.
     * @param aDriveNumber, drive number to be used for the path calculation
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    IMPORT_C HBufC* GetCopyPathL(const CUpnpItem& aItem,
                                 const CUpnpElement& aResource,
                                 TBool aAppendTitleAndExt,
                                 TDriveNumber aDriveNumber ) const;
    
    /**
     * Gets and creates (if necessary) the path of the upnp item's 
     * file to be copied.
     * @param aItem, a upnp item
     * @param aResource, resource of the upnp item
     * @param aAppendTitleAndExt, If ETrue title and extensions gets 
     *                            appended to the path otherwise not.
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    IMPORT_C HBufC* CreateCopyPathL(const CUpnpItem& aItem,
                                 const CUpnpElement& aResource,
                                 TBool aAppendTitleAndExt ) const;
    
    /**
     * Gets and creates (if necessary) the path of the upnp item's 
     * file to be copied.
     * @param aItem, a upnp item
     * @param aResource, resource of the upnp item
     * @param aAppendTitleAndExt, If ETrue title and extensions gets 
     *                            appended to the path otherwise not.
     * @param aDriveNumber, drive number to be used for the path calculation
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    IMPORT_C HBufC* CreateCopyPathL(const CUpnpItem& aItem,
                                 const CUpnpElement& aResource,
                                 TBool aAppendTitleAndExt,
                                 TDriveNumber aDriveNumber ) const;
    
    /**
     * Removes empty folders from the copy path 
     * @param aCopyPath, the copy path to be analysed
     */
    IMPORT_C static void RemoveEmptyFoldersFromCopyPathL(
                            const TDesC& aCopyPath );
    
private:

    /**
     * C++ default constructor.
     */
    CUPnPPathUtility();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Gets and creates (if necessary) the path of the upnp item's 
     * file to be copied.
     * @param aItem, a upnp item
     * @param aResource, resource of the upnp item
     * @param aAppendTitleAndExt, If ETrue title and extensions gets 
     *                            appended to the path otherwise not.
     * @param aCreatePath, If ETrue path gets created otherwise not.
     * @param aDriveNumber, drive number to be used for the path calculation
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    HBufC* GetCreateCopyPathL(const CUpnpItem& aItem,
                              const CUpnpElement& aResource,
                              TBool aAppendTitleAndExt,
                              TBool aCreatePath,
                              TDriveNumber aDriveNumber ) const;
    /**
     * Appends title and extension to the path.
     * @param aPath, path of the upnp item
     * @param aProtocolInfo, protocol info. attribute of the resource elem.
     * @param aItem, a upnp item
     */    
    void AppendTitleAndExtL( TDes& aPath, 
                        CUpnpDlnaProtocolInfo& aProtocolInfo, 
                        const CUpnpItem& aItem ) const;
    /**
     * Appends year, month and day to the path.
     * @param aPath, path of the upnp item
     * @param aItem, a upnp item
     */        
    void AppendYearMonthDayL( 
            TDes& aPath, const CUpnpItem& aItem ) const;
    
    /**
     * Appends artist and album to the path.
     * @param aPath, path of the upnp item
     * @param aItem, a upnp item
     */        
    void AppendArtistAlbumL( 
            TDes& aPath, const CUpnpItem& aItem ) const;
    
    /**
     * Appends data to the path's buffer.
     * Leaves if data exceeds the maxlength of the buffer
     * 
     * @param aPath, path of the upnp item
     * @param aData, data which needs to be appended
     */    
    void AppendDataL( TDes& aPath, const TDesC& aData ) const;
    
    /**
     * Checks whether the data can be appended to buffer or not.
     * Leaves if data exceeds the maxlength of the buffer
     * 
     * @param aPath, path of the upnp item
     * @param aData, data which needs to be appended
     */    
    void CheckBufferSpaceL( const TDes& aPath, 
            const TDesC& aData ) const;
    
    /**
     * Checks whether the data of the specified length
     * can be appended to buffer or not.
     * Leaves if data exceeds the maxlength of the buffer
     * 
     * @param aPath, path of the upnp item
     * @param aLength, length of the data which needs to be appended
     */    
    void CheckBufferSpaceL( const TDes& aPath, 
            const TInt& aLength ) const;
    
    /**
     * Finds an element within an object.
     *
     * @param aObject the object where to look for elements
     * @param aName element name
     * @return a pointer to the element, or NULL if not found
     */
    const CUpnpElement* FindElementByName(
        const CUpnpObject& aObject, const TDesC8& aName ) const;

    /**
      * Finds an attribute within an upnp element
      *
      * @param aElement the element where to look for attributes
      * @param aName attribute name
      * @return a pointer to the attribute found
      */
    const CUpnpAttribute* FindAttributeByName(
         const CUpnpElement& aElement, const TDesC8& aName ) const;
    
    /**
     * Converts given upnp date string into TTime.
     *
     * @param aUpnpDate value from item's dc:date element
     * @param aTime out variable to receive corresponding TTime
     * @return KErrNone if conversion was succesful, otherwise < 0
     *         for error values see TTime::Parse()
     */
    void UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
        TTime& aTime ) const;

    /**
     * Removes empty folders from the full path 
     * @param aBasePath, base path (e.g. till Images\ or Sounds\ )
     * @param aFullPath, complete path (without filename and extension) 
     */
    static void RemoveEmptyFoldersL( const TDesC& aBasePath, 
                            const TDesC& aFullPath );
        
private:    // data

    // Settings Engine
    CUPnPSettingsEngine*         iSettingsEngine;
    
    };

#endif  // UPNPPATHUTILITY_H

// End of file
