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
#include    "upnpcontainerlist.h"







// ============================ MEMBER FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpContainerList::CUpnpContainerList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpContainerList::CUpnpContainerList()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContainerList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpContainerList::ConstructL( )
    {
    CUpnpObjectList::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CUpnpContainerList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContainerList* CUpnpContainerList::NewL()
    {
    CUpnpContainerList* self = NewLC();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpContainerList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContainerList* CUpnpContainerList::NewLC()
    {
    CUpnpContainerList* self = new( ELeave ) CUpnpContainerList;

    CleanupStack::PushL( self );
    self->ConstructL(  );

    return self;
    }
      
// -----------------------------------------------------------------------------
// CUpnpItemList::~CUpnpContainerList
// -----------------------------------------------------------------------------
//
CUpnpContainerList::~CUpnpContainerList()
    {
    }
// -----------------------------------------------------------------------------
// CUpnpContainerList::AppendObjectL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContainerList::AppendObjectL( CUpnpObject& aNewObject )
    {
    // leave if not an item
    if(aNewObject.ObjectType() != EUPnPContainer)
        {
        User::Leave(KErrArgument);
        }

    CUpnpObjectList::AppendObjectL(aNewObject);
    }

//  End of File
