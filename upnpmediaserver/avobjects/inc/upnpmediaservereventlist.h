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
* Description:  Media Server event list
*
*/


#ifndef C_UPNPMEDIASERVEREVENTLIST_H
#define C_UPNPMEDIASERVEREVENTLIST_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpobject.h"
#include "upnpobject.h"
#include "upnpmediaserverevent.h"







// DATA TYPES
typedef RPointerArray<CUpnpMediaServerEvent> RUpnpMediaServerEventList;


// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpMediaServerEvent;


// CLASS DECLARATION

/**
*  Base class for Media Server events lists
*
*  @since Series 60 3.1
*/
class CUpnpMediaServerEventList : public CBase
    {
public:  // Constructors and destructor



    /**
    * Destructor.
    */
    virtual ~CUpnpMediaServerEventList();

public: // New functions

    /**
    * Serialize the whole list to a heap based descriptor.
    * @since Series 60 3.0
    * @return pointer to the descriptor where the list is serialized.
    */
    IMPORT_C HBufC8* ToDes8L() const;

    /**
    * Returns RPointerArray of the itemlist
    * @since Series 60 3.0
    * @param TBool indicating whether recursive count is needed
    * @return TInt count information
    */
    IMPORT_C RUpnpMediaServerEventList& GetPointerArray( );

public:

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
    * Internalizes itemlist information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.0
    * @param reference to RReadStream
    * @return none
    */
    virtual void InternalizeSubTypeL( RReadStream& aStream ) = 0;

    /**
    * C++ default constructor.
    */
    CUpnpMediaServerEventList();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( );

protected:
    // items
    RUpnpMediaServerEventList   iEvents;
    };

#endif      // C_UPNPMEDIASERVEREVENTLIST_H

// End of File
