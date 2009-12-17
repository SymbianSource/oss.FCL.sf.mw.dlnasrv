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


#ifndef UPNPITEMLIST_H
#define UPNPITEMLIST_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpobject.h"
#include "upnpitem.h"
#include "upnpobjectlist.h"

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION
/**
*  UPnPItemList class
*
*  @lib upnpavobjects.lib
*  @since Series 60 3.0
*/
class CUpnpItemList : public CUpnpObjectList
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpItemList* NewL();
        IMPORT_C static CUpnpItemList* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CUpnpItemList();

    public: // Functions from base classes 

        /**
        * Appends new object to hierarchy.
        * This function verifies if the given object is of CUpnpItem type 
        * and leaves with KErrArgument if not.
        *
        * @since Series 60 3.0
        * @param aNewObject an object to be added to the list.
        *           NOTE: The object must be of CUpnpItem type.
        */
        IMPORT_C void AppendObjectL( CUpnpObject& aNewObject );

    private:

        /**
        * C++ default constructor.
        */
        CUpnpItemList();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
    private:    // Data
        
    };

#endif      // UPNPITEMLIST_H   
            
// End of File
