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
#include    "upnpitem.h"







// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpItem::CUpnpItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpItem::CUpnpItem()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpItem::ConstructL(  )
    {
    iRefId = KNullDesC8().AllocL();
    CUpnpObject::ConstructL(  );
    }

// -----------------------------------------------------------------------------
// CUpnpItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpItem* CUpnpItem::NewL( )
    {
    CUpnpItem* self = new( ELeave ) CUpnpItem;    
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
EXPORT_C CUpnpItem::~CUpnpItem()
    {
    delete iRefId;
    }
         
// -----------------------------------------------------------------------------
// CUpnpItem::SetRefIdL
// Sets file name for the object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpItem::SetRefIdL( const TDesC8& aRefIdL )
    {
    HBufC8* tmp = aRefIdL.AllocL();
    delete iRefId;
    iRefId = tmp;    
    }
      
// -----------------------------------------------------------------------------
// CUpnpItem::FileName
// Returns filename of the object. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpItem::RefId() const
    {
    return *iRefId;
    }    
      
// -----------------------------------------------------------------------------
// CUpnpItem::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpItem::ExternalizeL( RWriteStream& aStream ) const
    {
    BaseExternalizeL( aStream );
    aStream << RefId();
    }

// -----------------------------------------------------------------------------
// CUpnpItem::InternalizeL
// Fills item information from stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CUpnpItem::InternalizeL( RReadStream& aStream )
    {
    BaseInternalizeL( aStream );
    delete iRefId;
    iRefId = NULL;
    iRefId = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );    
    }
// -----------------------------------------------------------------------------
// CUpnpItem::ObjectType
// Returns item-type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUPnPObjectType CUpnpItem::ObjectType() const
    {
    return EUPnPItem;
    }
  
//  End of File
