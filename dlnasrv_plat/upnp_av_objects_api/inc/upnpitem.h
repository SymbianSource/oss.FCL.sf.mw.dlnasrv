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


#ifndef UPNPITEM_H
#define UPNPITEM_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpobject.h"

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION
/**
*  UPnPItem class
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpItem : public CUpnpObject
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpItem* NewL( );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUpnpItem();

        
    public:  // New functions
        
        /**
        * Sets File name (including path) for the UPnP item. 
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param TDesC8 reference for file name
        * @return none
        */
        IMPORT_C void SetRefIdL( const TDesC8& aRefId);
        
        /**
        * Gets File name for the UPnP item
        * @since Series 60 3.0
        * @param none
        * @return TInt reference for parent ID
        */
        IMPORT_C const TDesC8& RefId() const;


		/**
        * Returns type of the object (= item)
        * @since Series 60 3.0
        * @param none
        * @return TUPnPObjectType information of the type
        */
        TUPnPObjectType ObjectType() const;

        /**
        * Externalizes item information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.0
        * @param reference to RWriteStream
        * @return none
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes item information from stream.
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
        CUpnpItem( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private:    // Data

        // Full file name to physical file
       	HBufC8*      iRefId ;  

    };

#endif      // UPNPITEM_H   
            
// End of File
