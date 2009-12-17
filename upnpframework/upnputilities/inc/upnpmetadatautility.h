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
* Description:  Utility class for meta data gathering for audio files
*
*/

#ifndef UPNPMETADATAUTILITY_H
#define UPNPMETADATAUTILITY_H


// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CMetaDataUtility;

// CLASS DECLARATION

/**
* Meta data utility to support UPnP tag information creation
*
* @lib upnputilities.lib
* @since S60 3.0
*/
class CUPnPMetaDataUtility : public CBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUPnPMetaDataUtility* NewL();
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CUPnPMetaDataUtility();

public: // New functions
    
    /**
    * Loads Meta data from given file into internal data structures.
    * Leaves in case of error
    *   - KErrAccessDenied = File is DRM protected
    *   - KErrNotSupported = Unknown file, meta data cannot be retrieved
    * @since Series 60 3.0
    * @param aFileName reference to music file
    * @return none
    */
    IMPORT_C void LoadMetaDataL( const TDesC& aFileName );
    
    /**
    * Returns title information of the file.
    * @since Series 60 3.0
    * @param none
    * @return TDesC& reference to title
    */
    IMPORT_C const TDesC& Title() const;
    
    /**
    * Returns artist information of the file.
    * @since Series 60 3.0
    * @param none
    * @return TDesC& reference to artist name
    */
    IMPORT_C const TDesC& Artist() const;
    
    /**
    * Returns album information of the file.
    * @since Series 60 3.0
    * @param none
    * @return TDesC& reference to album name
    */
    IMPORT_C const TDesC& Album() const;
    
    /**
    * Returns genre information of the file.
    * @since Series 60 3.0
    * @param none
    * @return TDesC& reference to genre name
    */
    IMPORT_C const TDesC& Genre() const;
    
    /**
    * Returns date information of the file.
    * @since Series 60 3.0
    * @param none
    * @return TDesC& reference to date
    */
    IMPORT_C const TDesC& Date() const;
    
    /**
    * Loads Title from given file into internal data structures.
    * Leaves in case of error
    *   - KErrAccessDenied = File is DRM protected
    *   - KErrNotSupported = Unknown file, meta data cannot be retrieved
    * @since Series 60 3.0
    * @param aFileName reference to music file
    * @return none
    */
    IMPORT_C void LoadTitleL( const TDesC& aFileName );

private:

    /**
    * C++ default constructor.
    */
    CUPnPMetaDataUtility();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    /**
    * Reset member variables
    */
    void Reset();

private:    // Data

    // Meta data information
    HBufC* iTitle;  // owned
    HBufC* iArtist; // owned
    HBufC* iAlbum;  // owned
    HBufC* iGenre;  // owned
    HBufC* iDate;   // owned
    
    // Utility class
    CMetaDataUtility* iMetaDataUtility; // owned
    };

#endif      // UPNPMETADATAUTILITY_H  
            
// End of File
