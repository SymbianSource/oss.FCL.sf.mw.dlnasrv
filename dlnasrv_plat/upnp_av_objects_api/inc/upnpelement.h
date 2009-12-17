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
* Description:  UPnP Element information
*
*/



#ifndef UPNPELEMENT_H
#define UPNPELEMENT_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpattribute.h"

_LIT8( KResEl,"res" );
_LIT8( KFilePathAttrib, "filepath" );

// CONSTANTS
// None

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None
class CUpnpAttribute;
// Array type for UPnPObjects
typedef RPointerArray<CUpnpAttribute> RUPnPAttributesArray;

// CLASS DECLARATION

/**
*  UPnPContainer interface
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpElement : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpElement* NewLC(const TDesC8& aName = KNullDesC8());
        IMPORT_C static CUpnpElement* NewL(const TDesC8& aName = KNullDesC8());
        
        /**
        * Destructor.
        */
        virtual ~CUpnpElement();

    public: // New functions
        
       /**
        * Sets name for the UPnP element. 
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for name
        * @return none
        */
        IMPORT_C void SetNameL( const TDesC8& aName );
        
        /**
        * Gets name for the UPnP element
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for parent ID
        */
        IMPORT_C const TDesC8& Name() const;
        
         /**
        * Sets value for the UPnP element. 
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for name
        * @return none
        */
        IMPORT_C void SetValueL( const TDesC8& aValue );
        
        /**
        * Gets value for the UPnP element
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for parent ID
        */
        IMPORT_C const TDesC8& Value() const;
 		
 		
 		/**
        * Gets path for the UPnP element res
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 path
        */
 		IMPORT_C const TDesC& FilePath() const;

 		/**
        * SetFilePathL path for the UPnP element res
        * @since Series 60 3.0
        * @param TDesC8 file path
        * @return none
        */
 		IMPORT_C void SetFilePathL(const TDesC& aFilePath);
 		
 		/**
        * Adds Attribute for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        
        IMPORT_C void AddAttributeL( CUpnpAttribute* aNewAttribute );
        
        /**
        * Removes specified Attribute from the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        IMPORT_C void RemoveAttributeL( const CUpnpAttribute* aNewAttribute );
        
        /**
        * Gets Attribute array of the object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        IMPORT_C const RUPnPAttributesArray& GetAttributes();      
	   
	    
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
        IMPORT_C void CopyL( const CUpnpElement& aElement );
        
        IMPORT_C void SetIsRequired(TBool aIsRequired);
        IMPORT_C TBool IsRequired() const;

    private:
       /**
        * C++ default constructor.
        */
        CUpnpElement();        

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aName);

		/**
        * Safely externalizes element information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        void SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const;
		void SafeExternalize16L(RWriteStream& aStream, const TDesC& aDesc, const TInt aNumber) const;
		TInt MapItemNameToProperLength(TDesC8& aItemName) const;

    private:
       
        //Element Name
        HBufC8*      iElementName;

        //Element Value
        HBufC8*      iElementValue;

        // File path
        HBufC*       iFilePath;
        
        // Is required
        TBool iIsRequired;
        
        //Element attributes
        RUPnPAttributesArray  iAttributes;         
        
    };

#endif      // UPNPELEMENT_H  
            
// End of File