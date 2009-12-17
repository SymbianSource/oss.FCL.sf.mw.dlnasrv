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
#include    "upnpobjectlist.h"
#include    "upnpitem.h"
#include    "upnpcontainer.h"









// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpObjectList::CUpnpObjectList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpObjectList::CUpnpObjectList()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpObjectList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpObjectList::ConstructL( )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpObjectList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObjectList* CUpnpObjectList::NewL()
    {
    CUpnpObjectList* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpObjectList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObjectList* CUpnpObjectList::NewLC()
    {
    CUpnpObjectList* self = new( ELeave ) CUpnpObjectList;
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }
      
// Destructor
CUpnpObjectList::~CUpnpObjectList()
    {
    iObjects.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpObjectList::AppendObjectL
// Appends object to internal array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObjectList::AppendObjectL( CUpnpObject& aNewObject )
    {
    iObjects.AppendL( &aNewObject );
    }

// -----------------------------------------------------------------------------
// CUpnpObjectList::RemoveAndDestroy
// Removes and destroys object from internal array according to ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObjectList::RemoveAndDestroy( const TDesC8& aId )
    {
    TInt idx = FindObjectIndex(aId);
    if(idx != KErrNotFound)
        {
        delete iObjects[idx];
        iObjects.Remove(idx);
        }
    }
// -----------------------------------------------------------------------------
// CUpnpObjectList::Remove
// Detaches object from internal array according to ID and resturns its address.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObject* CUpnpObjectList::Remove( const TDesC8& aId )
    {
    CUpnpObject* ret = NULL;
    TInt idx = FindObjectIndex(aId);
    if(idx != KErrNotFound)
        {
        ret = iObjects[idx];
        iObjects.Remove(idx);
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpObjectList::FindObject
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObject* CUpnpObjectList::FindObject( const TDesC8& aId )
    {
    CUpnpObject* ret = NULL;
    TInt idx = FindObjectIndex(aId);
    if(idx != KErrNotFound)
        {
        ret = iObjects[idx];
        }
    return ret;
    }
  
// -----------------------------------------------------------------------------
// CUpnpObjectList::ObjectCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpObjectList::FindObjectIndex(const TDesC8& aObjId) const
    {
    TInt ret = KErrNotFound;
    
    // try to find the object with given id
    for(TInt i = 0; i < iObjects.Count(); i++)
        {
        if(iObjects[i]->Id() == aObjId)
            {
            ret = i;
            // stop searching
            break;
            }
        }
        
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpObjectList::ObjectCount
// Counts objects 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpObjectList::ObjectCount( ) const
    {    
    return iObjects.Count();
    }
    
// -----------------------------------------------------------------------------
// CUpnpObjectList::GetObjectL
// Returns object from object array according to index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpObject* CUpnpObjectList::operator[]( TInt aIndex ) const
    {
    return iObjects[aIndex];
    }

// -----------------------------------------------------------------------------
// CUpnpObjectList::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObjectList::ExternalizeL( RWriteStream& aStream ) const
	{
    // Let's write the count of items to stream first
    aStream.WriteInt16L( iObjects.Count() );
    for ( TInt i = 0; i < iObjects.Count(); i++ )
        {
        // Need to put the type of the object to stream
        TPckg<TUPnPObjectType> type( iObjects[i]->ObjectType() );
        aStream << type;
        // Then the object itself
        iObjects[i]->ExternalizeL( aStream );
        }
	}
// -----------------------------------------------------------------------------
// CUpnpObjectList::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CUpnpObjectList::InternalizeL( RReadStream& aStream )
    {
	// internalize the objects
    iObjects.ResetAndDestroy();

	// First the count of individual objects
	TInt itemCount = aStream.ReadInt16L();

    TUPnPObjectType type = EUPnPItem;
    TPckg<TUPnPObjectType> typePkg( type );
	
	// Then internalize them from the stream one by one
	for (TInt index = 0; index < itemCount; index++ )
	    {
	    aStream >> typePkg;
	    
	    switch ( type )
	        {
	        case EUPnPItem:
	            {
	            CUpnpItem* newItem = CUpnpItem::NewL();
	            CleanupStack::PushL( newItem );
	            newItem->InternalizeL( aStream );
	            AppendObjectL( *newItem );
	            CleanupStack::Pop(); 
	            break;
	            }
	        case EUPnPContainer:
	            {
	            CUpnpContainer* newObj = CUpnpContainer::NewL();
	            CleanupStack::PushL( newObj );
	            newObj->InternalizeL( aStream );
	            AppendObjectL( *newObj );
	            CleanupStack::Pop(); 
	            break;
	            }
	        default:
	            {
	            break;
	            }
	        }
	    }
    }
// -----------------------------------------------------------------------------
// CUpnpObjectList::ToDes8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC8* CUpnpObjectList::ToDes8L() const
{
    // serialize object
    CBufFlat* buf = CBufFlat::NewL(KDefBufferGranularity);
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    stream << *this;
    
    // create heap descriptor
    HBufC8* hbuf = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(hbuf->Des());
    buf->Read(0, ptr, buf->Size());
    
    // clean up
    CleanupStack::Pop(hbuf);
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    
    return hbuf;
}
// -----------------------------------------------------------------------------
// CUpnpObjectList::ToDes8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC8* CUpnpObjectList::IdsToDes8L() const
{
    // serialize object
    CBufFlat* buf = CBufFlat::NewL(KDefBufferGranularity);
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    // number of elements
    TInt n = ObjectCount();
    stream.WriteInt32L(n);
    
    // write each element
    for(TInt i = 0; i < n; i++)
    {
        stream << ( iObjects[i]->Id() );
    }
    
    // create heap descriptor
    HBufC8* hbuf = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(hbuf->Des());
    buf->Read(0, ptr, buf->Size());
    
    // clean up
    CleanupStack::Pop(hbuf);
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    
    return hbuf;
}

//  End of File
