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






#ifndef UPNP_MSPATHUTILITY_H
#define UPNP_MSPATHUTILITY_H

// INCLUDE FILES
#include <e32base.h>    // CBase
#include <f32file.h>    // TDriveNumber
#include <xml/dom/xmlengdom.h>

// FORWARD DECLARATIONS
class CRepository;
class CUpnpDlnaProtocolInfo;

// CLASS DECLARATION
/**
 *  Path utility class to get path related info for the media files
 *  @lib upnpcontentdirectoryservice.lib
 *  Note:- This class needs to be revisited once mediaserver refactoring is 
 *         over as there is definitely the need to remove duplicate code
 *        
 */
NONSHARABLE_CLASS ( CUPnPMSPathUtility ) : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUPnPMSPathUtility* NewL();

    /**
     * Two-phased constructor.
     */
    static CUPnPMSPathUtility* NewLC();

    /**
     * Destructor.
     */
    virtual ~CUPnPMSPathUtility();
        
        
public: // new functions

    /**
     * Gets the drive for the copy operation
     * @param aDrive, reference to copy path drive
     */
    void GetCopyPathDriveL( TDriveNumber& aDrive ) const;

    /**
     * Gets the path of the upnp item's file to be copied
     * @param aItem, an xml item
     * @param aProInfo, proto info of the resource of the xml item
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    HBufC* GetCopyPathL( const TXmlEngElement& aItem,
                         CUpnpDlnaProtocolInfo& aProInfo ) const;

    /**
     * Gets and creates (if necessary) the path of the upnp item's 
     * file to be copied.
     * @param aItem, an xml item
     * @param aProInfo, proto info of the resource of the xml item
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    HBufC* CreateCopyPathL( const TXmlEngElement& aItem,
                            CUpnpDlnaProtocolInfo& aProInfo ) const;
    
private:
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
    
    // DATA TYPES
    enum TUPnPItemType
        {
        ETypeAudio = 0,
        ETypeVideo,
        ETypeImage,
        ETypePlaylist,
        ETypeOther
        };
    
    /**
     * C++ default constructor.
     */
    CUPnPMSPathUtility();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(TUid aRepositoryUid);

    /**
     * Gets and creates (if necessary) the path of the upnp item's 
     * file to be copied.
     * @param aItem, an xml item
     * @param aProInfo, proto info of the resource of the xml item
     * @param aCreatePath, If ETrue path gets created otherwise not.
     * @param aDriveNumber, drive number to be used for the path calculation
     * @return HBufC*, copy path. Ownership is transferred to the caller
     *          Leaves in case of an error.    
     */
    HBufC* GetCreateCopyPathL(const TXmlEngElement& aItem,
                              CUpnpDlnaProtocolInfo& aProInfo,
                              TBool aCreatePath,
                              TDriveNumber aDriveNumber ) const;
    /**
    * Returns the file type according a mime type
    *
    * @param const TDesC8& mime type
    * @return TUPnPItemType file type
    */
   TUPnPItemType FileTypeByMimeTypeL( const TDesC8& aMimeType ) const;
   
    /**
     * Appends year, month and day to the path.
     * @param aPath, path of the upnp item
     * @param aItem, an xml item
     */        
    void AppendYearMonthDayL( 
            TDes& aPath, const TXmlEngElement& aItem ) const;
    
    /**
     * Appends artist and album to the path.
     * @param aPath, path of the upnp item
     * @param aItem, an xml item
     */        
    void AppendArtistAlbumL( 
            TDes& aPath, const TXmlEngElement& aItem ) const;
    
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
     * Converts given upnp date string into TTime.
     *
     * @param aUpnpDate value from item's dc:date element
     * @param aTime out variable to receive corresponding TTime
     * @return KErrNone if conversion was succesful, otherwise < 0
     *         for error values see TTime::Parse()
     */
    void UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
        TTime& aTime ) const;

private:    // data

    /**
    * Central repository client
    * Note:- CUpnpMediaServerSettings is not used to avoid dependency
    */  
    CRepository* iRepository;
    
    };

#endif  // UPNP_MSPATHUTILITY_H

// End of file
