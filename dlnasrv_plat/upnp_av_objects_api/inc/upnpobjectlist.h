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
* Description:  UPnP Item information class
*
*/


#ifndef UPNPOBJECTLIST_H
#define UPNPOBJECTLIST_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpobject.h"

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION
/**
*  UPnPItemList class
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpObjectList : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpObjectList* NewL();
        IMPORT_C static CUpnpObjectList* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CUpnpObjectList();

    public: // New functions
        
        IMPORT_C CUpnpObject* operator[]( TInt aIndex ) const;
        
        /**
        * Serialize ids of contained objects to a heap based descriptor.
        * @since Series 60 3.0
        * @return pointer to the descriptor where ids are serialized.
        */
        IMPORT_C HBufC8* IdsToDes8L() const;
        
        /**
        * Serialize the whole list to a heap based descriptor.
        * @since Series 60 3.0
        * @return pointer to the descriptor where the list is serialized.
        */
        IMPORT_C HBufC8* ToDes8L() const;       

        /**
        * Appends new object to hierarchy
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType information of the type
        */
        IMPORT_C void AppendObjectL( CUpnpObject& aNewObject );
        
        /**
        * Removes object from the hierarchy.
        * @since Series 60 3.0
        * @param aId id of the object to be removed and destroyed
        */
        IMPORT_C void RemoveAndDestroy( const TDesC8& aId );

        /**
        * Detaches object from the hierarchy.
        * @since Series 60 3.0
        * @param aId id of the object to be removed and destroyed
        * @return pointer to the object
        */
        IMPORT_C CUpnpObject* Remove( const TDesC8& aId );

        /**
        * Searches object from the object hierarchy
        * @since Series 60 3.0
        * @param CUpnpObject pointer to the object found
        * @param TDesC8 referenco to Object ID which is searched
        * @return TBool indicating the status of the find
        */
        IMPORT_C CUpnpObject* FindObject( const TDesC8& aObjectId );
        
        /**
        * Returns childcount of the itemlist 
        * @since Series 60 3.0
        * @param TBool indicating whether recursive count is needed
        * @return TInt count information
        */
        IMPORT_C TInt ObjectCount( ) const;
        

    public: // Functions from CUpnpObject

        /**
        * Externalizes itemlist information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes itemlist information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RReadStream
        * @return none
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );
        
     

    protected:

        /**
        * C++ default constructor.
        */
        CUpnpObjectList();
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
     private:
        TInt FindObjectIndex(const TDesC8& aObjId) const;

    private:    // Data
        
        // objects
        RPointerArray<CUpnpObject>   iObjects;
    };

#endif      // UPNPOBJECTLIST_H   
            
// End of File
