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
* Description:  UPnP Container information
*
*/


#ifndef UPNPCONTAINER_H
#define UPNPCONTAINER_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpobject.h"

// CONSTANTS
// None

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUpnpObjectList;

// CLASS DECLARATION

/**
*  UPnPContainer interface
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpContainer : public CUpnpObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpContainer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CUpnpContainer();

    public: // New functions
        
        IMPORT_C CUpnpObject* operator[](TInt aIndex);
        
        /**
        * Appends new object to hierarchy
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType information of the type
        */
        IMPORT_C void AppendObjectL( CUpnpObject& aNewObject );
        
        /**
        * Removes object from the hierarchy if found.
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType information of the type
        */
        IMPORT_C void RemoveObjectL( const TDesC8& aObjectId );

        /**
        * Searches object from the object hierarchy
        * @since Series 60 3.0
        * @param CUpnpObject pointer to the object found
        * @param TDesC8 referenco to Object ID which is searched
        * @return TBool indicating the status of the find
        */
        IMPORT_C CUpnpObject* FindObject( const TDesC8& aObjectId );
        
        /**
        * Returns childcount of the container (including sub-containers)
        * @since Series 60 3.0
        * @return TInt count information
        */
        IMPORT_C TInt ObjectCount();
        
        
    public: // Functions from CUpnpObject

		/**
        * Returns type of the object (= container)
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType information of the type
        */
        TUPnPObjectType ObjectType() const;

        /**
        * Externalizes container information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes container information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );
        
      
        


    private:

        /**
        * C++ default constructor.
        */
        CUpnpContainer( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private:
        
        // Child object array.        
        CUpnpObjectList*     iObjects;


    };

#endif      // UPNPCONTAINER_H  
            
// End of File
