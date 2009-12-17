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
* Description:  UPnP BrowseCriteria information
*
*/



#ifndef UPNPBROWSECRITERIA_H
#define UPNPBROWSECRITERIA_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CONSTANTS

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/**
*  UpnpBrowseCriteria interface
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpBrowseCriteria : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpBrowseCriteria* NewLC(const TDesC8& aFilter = KNullDesC8());
        IMPORT_C static CUpnpBrowseCriteria* NewL(const TDesC8& aFilter = KNullDesC8());
        
        /**
        * Destructor.
        */
        virtual ~CUpnpBrowseCriteria();

    public: // New functions
        
       /**
        * Sets filter. 
        * @since Series 60 3.0
        * @param TDesC8 reference for filter
        * @return none
        */
        IMPORT_C void SetFilterL( const TDesC8& aFilter );
        
        /**
        * Gets filter
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for filter
        */
        IMPORT_C const TDesC8& Filter() const;
        
         /**
        * Sets request count. 
        * @since Series 60 3.0
        * @param TInt reference for requested count
        * @return none
        */
        IMPORT_C void SetRequestedCount( TInt aRequestedCount );
        
        /**
        * Gets request count
        * @since Series 60 3.0
        * @param none
        * @return TInt reference for requested count
        */
        IMPORT_C TInt RequestedCount() const;
          
        /**
        * Sets starting index. 
        * @since Series 60 3.0
        * @param TInt reference for starting index
        * @return none
        */
        IMPORT_C void SetStartingIndex( TInt aStartingIndex );
        
        /**
        * Gets starting index
        * @since Series 60 3.0
        * @param none
        * @return TInt reference for starting index
        */
        IMPORT_C TInt StartingIndex() const;		
	
        /**
        * Externalizes element information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes element information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );
        
        /**
        * Copies contents from CUpnpElement given as parameter to this object.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param aObject const reference to source information
        * @return none
        */
        IMPORT_C void CopyL( const CUpnpBrowseCriteria& aBrowseCriteria );
        IMPORT_C HBufC8* ToDes8L() const;

    private:
       /**
        * C++ default constructor.
        */
        CUpnpBrowseCriteria();        

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aName);


    private:
       
        //Requested Count
        TInt    iRequestedCount;

        //filter
        HBufC8*     iFilter;

        // Starting index
        TInt        iStartingIndex;       
        
    };

#endif      // UPNPBROWSECRITERIA_H  
            
// End of File