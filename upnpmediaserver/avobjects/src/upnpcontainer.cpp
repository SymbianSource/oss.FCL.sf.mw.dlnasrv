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
#include    <upnpstring.h>
#include    <e32base.h>
#include    "upnpcontainer.h"
#include    "upnpitem.h"
#include    "upnpobjectlist.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpContainer::CUpnpContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpContainer::CUpnpContainer()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpContainer::ConstructL(  )
    {
    CUpnpObject::ConstructL(  );
    iObjects = CUpnpObjectList::NewL();
    }

// -----------------------------------------------------------------------------
// CUpnpContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

EXPORT_C CUpnpContainer* CUpnpContainer::NewL()
    {
    CUpnpContainer* self = new( ELeave ) CUpnpContainer;

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CUpnpContainer::~CUpnpContainer()
    {
    delete iObjects;
    }

// -----------------------------------------------------------------------------
// CUpnpContainer::AppendObjectL
// Appends object to internal array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContainer::AppendObjectL( CUpnpObject& aNewObject )
    {
    // Set the parent Id to this container 
    aNewObject.SetParentIdL( Id() );
    iObjects->AppendObjectL( aNewObject );
    }

// -----------------------------------------------------------------------------
// CUpnpContainer::RemoveObjectL
// Removes object from internal array according to ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContainer::RemoveObjectL( const TDesC8& aObjectId )
    {
    iObjects->RemoveAndDestroy(aObjectId);
    }
             
// -----------------------------------------------------------------------------
// CUpnpContainer::FindObject
// Finds object from internal array according to id. Is recursive, so goes down
// the object tree.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObject* CUpnpContainer::FindObject( const TDesC8& aObjectId )
    {
    CUpnpObject* ret = NULL;
    if ( Id() == aObjectId )
        {
        ret = this;
        }
    else
        {
        for (TInt i = 0; i < iObjects->ObjectCount(); i++ )
            {
            if ( (*iObjects)[i]->ObjectType() == EUPnPContainer )
                {
                // Containers can contain more stuff --> recursive action needed.
                CUpnpObject* tmp = (static_cast<CUpnpContainer*>((*iObjects)[i]))
                                    ->FindObject( aObjectId );
                if ( tmp )
                    {
                    ret = tmp;
                    break;
                    }
                }
            else
                {
                // Need to check items
                if ( (*iObjects)[i]->Id() == aObjectId )
                    {
                    ret = (*iObjects)[i];
                    break;
                    }
                }
            }
        }
    return ret;
    }
        
// -----------------------------------------------------------------------------
// CUpnpContainer::ObjectCount
// Counts objects (including containers) from the object tree (recursive / not 
// recursive)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpContainer::ObjectCount()
    {
    return iObjects->ObjectCount();
    }
    
// -----------------------------------------------------------------------------
// CUpnpContainer::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContainer::ExternalizeL( RWriteStream& aStream ) const
    {
    BaseExternalizeL( aStream );
    }

// -----------------------------------------------------------------------------
// CUpnpContainer::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CUpnpContainer::InternalizeL( RReadStream& aStream )
    {
    BaseInternalizeL( aStream );
    }
    
// -----------------------------------------------------------------------------
// CUpnpContainer::ObjectType
// Returns item-type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUPnPObjectType CUpnpContainer::ObjectType() const
    {
    return EUPnPContainer;
    }
// -----------------------------------------------------------------------------
// CUpnpContainer::ObjectType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObject* CUpnpContainer::operator[](TInt aIndex)
    {
    return (*iObjects)[aIndex];
    }
  
   
//  End of File
