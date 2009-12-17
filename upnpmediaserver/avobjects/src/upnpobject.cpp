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
* Description:  UPnPObject implementation
*
*/


// INCLUDE FILES
#include "upnpobject.h" 


_LIT8(KContainer,"container"); 
_LIT8(KItem,"item");
_LIT8(KRes,"res");
_LIT8(KProtocolinfo,"protocolInfo");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpObject::CUpnpObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpObject::CUpnpObject()
    {
    iRestricted = ETrue;
    }
    
// Destructor
EXPORT_C CUpnpObject::~CUpnpObject()
    {
    delete iId;
    delete iParentId;
    delete iTitle;
    delete iObjectClass;
    iElements.ResetAndDestroy();
    iElements.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpObject::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpObject::ConstructL(  )
    {
    iId = KNullDesC8().AllocL();
    iParentId = KNullDesC8().AllocL();
    iTitle = KNullDesC8().AllocL();
    iObjectClass = KNullDesC8().AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpObject::SetIdL
// Sets Object Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetIdL( const TDesC8& aId )
    {
    HBufC8* tmp = aId.AllocL();
    delete iId;
    iId = tmp;    
    }

// -----------------------------------------------------------------------------
// CUpnpObject::Id
// Returns Id of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpObject::Id() const
    {
    return *iId; 
    }

// -----------------------------------------------------------------------------
// CUpnpObject::SetParentIdL
// Sets Parent Id for the object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetParentIdL( const TDesC8& aParentId )
    {
    HBufC8* tmp = aParentId.AllocL();
    delete iParentId;
    iParentId = tmp;
    }
       
// -----------------------------------------------------------------------------
// CUpnpObject::ParentId
// Returns Parent Id of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C  const TDesC8& CUpnpObject::ParentId() const
    {
    return *iParentId;
    }
  
// -----------------------------------------------------------------------------
// CUpnpObject::AddElementL
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::AddElementL( CUpnpElement* aNewElement )
    {
    if ( aNewElement )
        {
        iElements.AppendL( aNewElement );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpObject::Name
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::RemoveElementL( const CUpnpElement* aNewElement )
    {
    for ( TInt index(0); index < iElements.Count(); index++ )
        {
        if ( iElements[index] == aNewElement )
            {
            iElements.Remove( index );
           break;
            }
        }
    }  
// -----------------------------------------------------------------------------
// CUpnpObject::GetElements
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const RUPnPElementsArray& CUpnpObject::GetElements()
    {
    return iElements;
    }
    
// -----------------------------------------------------------------------------
// CUpnpObject::SetNameL
// Sets name for the object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetTitleL( const TDesC8& aTitle )
    {
    HBufC8* tmp = aTitle.AllocL();
    delete iTitle;
    iTitle = tmp;  
    }
      
// -----------------------------------------------------------------------------
// CUpnpObject::Name
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpObject::Title() const
    {
    return *iTitle;
    }
// -----------------------------------------------------------------------------
// CUpnpObject::SetObjectClassL
// Sets class object for the object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetObjectClassL( const TDesC8& aObjectClass )
    {
    ValidateClassL(aObjectClass); // leaves on error
    HBufC8* tmp = aObjectClass.AllocL();
    delete iObjectClass;
    iObjectClass = tmp;  
    }
// -----------------------------------------------------------------------------
// CUpnpObject::ValidateClassL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObject::ValidateClassL(const TDesC8& aObjectClass)   
{
    // verify the class
    if( ObjectType() == EUPnPContainer )
    { // it must contains "container" string
        if( aObjectClass.Find(KContainer) == KErrNotFound )
        {
            User::Leave(KErrArgument);
        }
    }
    else if(ObjectType() == EUPnPItem)
    {// it must contains "item" string
        if( aObjectClass.Find(KItem) == KErrNotFound )
        {
            User::Leave(KErrArgument);
        }
    }
    else
    { // should never happen
        User::Invariant();
    }
}
// -----------------------------------------------------------------------------
// CUpnpObject::ObjectClass
// Returns objectclass of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpObject::ObjectClass() const
    {
    return *iObjectClass;
    }

// -----------------------------------------------------------------------------
// CUpnpObject::SetRestriction()
// Sets the restriction info. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetRestricted( TBool aRestriction )
    {
    iRestricted = aRestriction;
    }
   
// -----------------------------------------------------------------------------
// CUpnpObject::Restriction()
// Gets the restriction info. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpObject::Restricted() const
    {
    return iRestricted;
    }

// -----------------------------------------------------------------------------
// CUpnpObject::SetWriteStatus()
// Sets the restriction info. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::SetWriteStatus( TBool aWriteStatus )
    {
    iWriteStatus = aWriteStatus;
    }
   
// -----------------------------------------------------------------------------
// CUpnpObject::Restriction()
// Gets the restriction info. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpObject::WriteStatus() const
    {
    return iWriteStatus;
    }
    
// -----------------------------------------------------------------------------
// CUpnpObject::CopyL
// Copies the content from parameter CUpnpObject to this
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::CopyL( const CUpnpObject& aObject )
    {
    // Check that parameter type is also item
    if ( aObject.ObjectType() != ObjectType() )
        {
        User::Leave( KErrArgument );
        }
        
    CBufFlat* buffer = CBufFlat::NewL( sizeof( aObject ) );
    CleanupStack::PushL( buffer );

    RBufWriteStream outStream;
    outStream.Open( *buffer );
    CleanupClosePushL( outStream );
	
    RBufReadStream inStream;
    inStream.Open( *buffer );
    CleanupClosePushL( inStream );
	
    aObject.ExternalizeL( outStream );
    InternalizeL( inStream );
	
    CleanupStack::PopAndDestroy( 3 );   // inStream && outStream && buffer
    }

// -----------------------------------------------------------------------------
// CUpnpObject::BaseExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObject::BaseExternalizeL( RWriteStream& aStream ) const 
    { 
		
    SafeExternalizeL(aStream, Id(), KMaxUpnpObjStringLen);
    SafeExternalizeL(aStream, ParentId(), KMaxUpnpObjStringLen);
    
    aStream.WriteInt8L(Restricted());
            
    SafeExternalizeL(aStream, Title(), KMaxUpnpObjStringLen);                            
    SafeExternalizeL(aStream, ObjectClass(), KMaxUpnpObjStringLen);
    
    // Elements
    aStream.WriteInt16L( iElements.Count() );
    for ( TInt index(0); index < iElements.Count(); index++ )
        {
        iElements[index]->ExternalizeL( aStream );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpObject::SafeExternalizeL
// Writes safely aNumber of characters from the content to the stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObject::SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const
{	
    if( aDesc.Length() > aNumber )    
		aStream << aDesc.Mid(0, aNumber);
    else
		aStream << aDesc;    
}

// -----------------------------------------------------------------------------
// CUpnpObject::BaseInternalizeL
// Fills item information from stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CUpnpObject::BaseInternalizeL( RReadStream& aStream )
    {
    delete iId;
    iId = NULL;
    iId = HBufC8::NewL( aStream, KMaxUpnpObjStringLen  );
    
    delete iParentId;
    iParentId = NULL;
    iParentId = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );
    iRestricted = aStream.ReadInt8L();
    
    // Name (title)
    delete iTitle;
    iTitle = NULL;
    iTitle = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );
    
    // ObjectClass
    delete iObjectClass;
    iObjectClass = NULL;
    iObjectClass = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );	
            
    // Elements internalization
    iElements.ResetAndDestroy();
    
    // Elements count
    TInt elementsCount = aStream.ReadInt16L();	
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < elementsCount; index++ )
        {
        CUpnpElement* newElement = CUpnpElement::NewLC();
        newElement->InternalizeL( aStream );
        iElements.AppendL( newElement );
        CleanupStack::Pop( newElement ); 
        }
	    
    }
// -----------------------------------------------------------------------------
// CUpnpObject::ToDes8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC8* CUpnpObject::ToDes8L() const
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
// CUpnpObject::AddResourceL
// Attaches proper <res> element (resource associated with an UPnP object) to protocolInfo
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpObject::AddResourceL( const TDesC16& aFilename,const TDesC8& aProtocolinfo )
    {
	CUpnpElement* elRes = CUpnpElement::NewL(KRes);
    CleanupStack::PushL( elRes ); 
    elRes->SetFilePathL(aFilename); 
   
    CUpnpAttribute* atImportUri =
    CUpnpAttribute::NewL(KProtocolinfo);
    CleanupStack::PushL(atImportUri);
    atImportUri->SetValueL(aProtocolinfo);
    elRes->AddAttributeL(atImportUri);
    CleanupStack::Pop(atImportUri);        
      
    AddElementL( elRes );
    CleanupStack::Pop( elRes );  
    }
//  End of File  
