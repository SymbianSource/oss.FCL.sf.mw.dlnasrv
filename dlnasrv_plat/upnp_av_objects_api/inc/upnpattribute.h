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
* Description:  Upnp Attribute information
*
*/


#ifndef UPNPATTRIBUTE_H
#define UPNPATTRIBUTE_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpelement.h"

const TInt KMaxUpnpObjLongStringLen = 1024;
const TInt KMaxUpnpObjStringLen = 256;

/**
*  UPnP CUpnpAttribute header
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpAttribute : public CBase
    {
    public:
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpAttribute* NewLC(const TDesC8& aName = KNullDesC8());
        IMPORT_C static CUpnpAttribute* NewL(const TDesC8& aName = KNullDesC8());
          
        /**
        * C++ default constructor.
        */
        virtual ~CUpnpAttribute();
        
        /**
        * Externalizes attribute information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
      
        
  public: //New functions      
        /**
        * Sets name for the UPnP attribute. 
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for name
        * @return none
        */
        IMPORT_C void SetNameL( const TDesC8& aName);
        
        /**
        * Gets name for the UPnP attribute
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for parent ID
        */
        IMPORT_C const TDesC8& Name() const;
        
        /**
        * Sets scope for the UPnP attribute. 
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for name
        * @return none
        */
        IMPORT_C void SetValueL( const TDesC8& aValue );
        
        /**
        * Gets name for the UPnP attribute
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for parent ID
        */
        IMPORT_C const TDesC8& Value() const;
        
        
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes attribute information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        void InternalizeL( RReadStream& aStream );
				
    private:
       /**
        * C++ default constructor.
        */
        CUpnpAttribute();        

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aName);

		/**
        * Safely externalizes attribute information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        void SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const;		
		TInt MapItemNameToProperLength(TDesC8& aItemName) const;

    private:
        // Parameters
        HBufC8*     iName;
        HBufC8*     iValue;
    };


#endif      // UPNPATTRIBUTE_H  
            
// End of File
