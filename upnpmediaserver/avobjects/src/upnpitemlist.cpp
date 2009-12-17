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
* Description:  UPnP container class implementation
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include	"upnpobject.h"
#include    "upnpitemlist.h"
    






// ============================ MEMBER FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpItemList::CUpnpItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpItemList::CUpnpItemList()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpItemList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpItemList::ConstructL( )
    {
    CUpnpObjectList::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CUpnpItemList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpItemList* CUpnpItemList::NewL()
    {
    CUpnpItemList* self = NewLC();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpItemList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpItemList* CUpnpItemList::NewLC()
    {
    CUpnpItemList* self = new( ELeave ) CUpnpItemList;

    CleanupStack::PushL( self );
    self->ConstructL(  );

    return self;
    }
      
// -----------------------------------------------------------------------------
// CUpnpItemList::~CUpnpItemList
// -----------------------------------------------------------------------------
//
CUpnpItemList::~CUpnpItemList()
    {
    }
// -----------------------------------------------------------------------------
// CUpnpItemList::AppendObjectL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpItemList::AppendObjectL( CUpnpObject& aNewObject )
    {
    // leave if not an item
    if(aNewObject.ObjectType() != EUPnPItem)
        {
        User::Leave(KErrArgument);
        }

    CUpnpObjectList::AppendObjectL(aNewObject);
    }
    
//  End of File
