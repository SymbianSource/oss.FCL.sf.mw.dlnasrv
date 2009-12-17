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
* Description:  Base class for UPnP object information
*
*/


#ifndef UPNPOBJECT_H
#define UPNPOBJECT_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpelement.h"

//CONSTANTS
const TInt KDefBufferGranularity = 100;
const TInt KDefArraySize = 8;

// FORWARD DECLARATIONS
class CUpnpObject;

// DATA TYPES
enum TUPnPObjectType 
	{
    EUPnPContainer = 1,
    EUPnPItem  
	};

// Array type for UPnPObjects
typedef RPointerArray<CUpnpElement> RUPnPElementsArray;

// CLASS DECLARATION
/**
*  UPnP Object header
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpObject : public CBase
    {
    public:  // Constructors and destructor
        
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUpnpObject();

    public: // New functions
        
        /**
        * Gets ID for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for ID
        */
        IMPORT_C void SetIdL( const TDesC8& aId );
        
        /**
        * Gets ID for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for ID
        */
        IMPORT_C const TDesC8& Id() const;
        
	    /**
        * Sets Parent ID for the UPnP Object. Leaves in case of errors.
        * @since Series 60 3.0
        * @param TInt reference for parent ID
        * @return none
        */
        IMPORT_C void SetParentIdL( const TDesC8& aParentId );
        
        /**
        * Gets Parent ID for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for parent ID
        */
        IMPORT_C const TDesC8& ParentId() const;
        
        /**
        * Sets title for the UPnP Object. Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for parent ID
        * @return none
        */
        IMPORT_C void SetTitleL( const TDesC8& aTitle );
        
        /**
        * Gets title for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for title
        */
        IMPORT_C const TDesC8& Title() const;
       
        /**
        * Sets ObjectClass for the UPnP Object. Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for parent ID
        * @return none
        */
        IMPORT_C void SetObjectClassL( const TDesC8& aObjectClass );
        
        /**
        * Gets ObjectClass for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return TDesC8 reference for title
        */
        IMPORT_C const TDesC8& ObjectClass() const;
        
        /**
        * Adds Element for the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        IMPORT_C void AddElementL( CUpnpElement* aNewElement );
        
        /**
        * Removes specified Element from the UPnP Object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        IMPORT_C void RemoveElementL( const CUpnpElement* aNewElement );
        
        /**
        * Gets Element array of the object
        * @since Series 60 3.0
        * @param none
        * @return none
        */
        IMPORT_C const RUPnPElementsArray& GetElements();

        /**
        * Sets the write status.
        * @since Series 60 3.0
        * @param aWriteStatus TBool
        * @return None
        */
        IMPORT_C void SetRestricted( TBool aRestriction );

        /**
        * Gets the write status. 
        * @since Series 60 3.0
        * @param None
        * @return TBool
        */
        IMPORT_C TBool Restricted() const;

        /**
        * Sets the write status.
        * @since Series 60 3.0
        * @param aWriteStatus TBool
        * @return None
        */
        IMPORT_C void SetWriteStatus( TBool aWriteStatus );

        /**
        * Gets the write status. 
        * @since Series 60 3.0
        * @param None
        * @return TBool
        */
        IMPORT_C TBool WriteStatus() const;
        		
        /**
        * Pure virtual function to return the concrete object type
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType object type
        */
        virtual TUPnPObjectType ObjectType() const = 0;        
        
        /**
        * Externalizes container information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        virtual void ExternalizeL( RWriteStream& aStream ) const = 0;
        
        /**
        * Internalizes container information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        virtual void InternalizeL( RReadStream& aStream ) = 0;
      
        /**
        * Copies contents from CUpnpObject given as parameter to this object.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param aObject const reference to source information
        * @return none
        */
        IMPORT_C void CopyL( const CUpnpObject& aObject );
        
        IMPORT_C HBufC8* ToDes8L() const;
		
        /**
        * Attaches proper <res> element (resource associated with an UPnP object) to protocolInfo        
        * @since Series 60 3.0
        * @param aFilename - resource file to be attached
        * @param aProtocolinfo - protocolInfo to which the resource will be attached
        * @return none
        */
        IMPORT_C void AddResourceL( const TDesC16& aFilename,const TDesC8& aProtocolinfo );
        
    protected:

        /**
        * C++ default constructor.
        */
        CUpnpObject();
        
        /**
        *  By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        CUpnpObject& operator= (const CUpnpObject& aObj);
  
        /**
        * Externalizes container information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        void BaseExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Safely externalizes object information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        void SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const;
        
        /**
        * Internalizes container information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        void BaseInternalizeL( RReadStream& aStream );
        
    private:
    
        void ValidateClassL(const TDesC8& aObjectClass);
                
    protected:    // Data
    
        // Object ID
        HBufC8*      iId;
         
        // Parent's object ID
        HBufC8*      iParentId;
        
        // Name of the object
        HBufC8*     iTitle;
        
        // object class
        HBufC8* 	iObjectClass;  
        
        // Restricted status
        TBool       iRestricted;
        
        // WriteStatus
        TBool       iWriteStatus;
        
        // Elements
        RUPnPElementsArray  iElements;

    };

#endif      // UPNPOBJECT_H  
            
// End of File
