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
* Description:  UPnPElement implementation
*
*/


// INCLUDE FILES
#include "upnpelement.h"
#include "upnpobject.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpElement::ConstructL(const TDesC8& aName)
{
    iElementName = aName.AllocL();
    iElementValue = KNullDesC8().AllocL();
    iFilePath = KNullDesC().AllocL();
}

// -----------------------------------------------------------------------------
// CUpnpElement::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpElement* CUpnpElement::NewLC(const TDesC8& aName)
{
    CUpnpElement* self = new( ELeave ) CUpnpElement;
    CleanupStack::PushL( self );
    self->ConstructL(aName);
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpElement::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpElement* CUpnpElement::NewL(const TDesC8& aName)
{
    CUpnpElement* self = NewLC(aName);
    CleanupStack::Pop(self);
    return self;
}    

// -----------------------------------------------------------------------------
// CUpnpElement::CUpnpElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpElement::CUpnpElement()
    {
    }

// Destructor
CUpnpElement::~CUpnpElement()
    {
    delete iElementName;
    delete iElementValue;
    delete iFilePath;
    iAttributes.ResetAndDestroy();
    iAttributes.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpElement::SetElementNameL
// Sets name for the attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::SetNameL( const TDesC8& aElementName )
    {
    HBufC8* tmp = aElementName.AllocL();
    delete iElementName;
    iElementName = tmp;    
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::Name
// Returns name of the attribute. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpElement::Name() const
    {
    return *iElementName;
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::SetElementNameL
// Sets name for the attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::SetValueL( const TDesC8& aElementValue )
    {
    HBufC8* tmp = aElementValue.AllocL();
    delete iElementValue;
    iElementValue = tmp;
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::Name
// Returns name of the attribute. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpElement::Value() const
    {
    return *iElementValue;
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::SetFilePathL
// Returns path of the attribute res. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::SetFilePathL(const TDesC& aFilePath)
    {
    HBufC* tmp = aFilePath.AllocL();
    delete iFilePath;
    iFilePath = tmp;
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::FilePath
// Returns path of the attribute res. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUpnpElement::FilePath() const
    {
    return *iFilePath;
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::AddAttributeL
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::AddAttributeL( CUpnpAttribute* aNewAttribute )
    {
    if ( aNewAttribute )
        {
        iAttributes.AppendL( aNewAttribute );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpElement::RemoveAttributeL
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::RemoveAttributeL( const CUpnpAttribute* aNewAttribute )
    {
    for ( TInt index(0); index < iAttributes.Count(); index++ )
        {
        if ( iAttributes[index] == aNewAttribute )
            {
            iAttributes.Remove( index );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpElement::Name
// Returns name of the object. If null, returns KNullDesC8
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const RUPnPAttributesArray& CUpnpElement::GetAttributes()
    {
    return iAttributes;
    }

// -----------------------------------------------------------------------------
// CUpnpElement::CopyL
// Copies the content from parameter CUpnpElement to this
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::CopyL( const CUpnpElement& aElement )
    {
      
    CBufFlat* buffer = CBufFlat::NewL( sizeof( aElement ) );
    CleanupStack::PushL( buffer );

    RBufWriteStream outStream;
    outStream.Open( *buffer );
    CleanupClosePushL( outStream );
	
    RBufReadStream inStream;
    inStream.Open( *buffer );
    CleanupClosePushL( inStream );
	
    aElement.ExternalizeL( outStream );
    InternalizeL( inStream );
	
    CleanupStack::PopAndDestroy( 3 );   // inStream && outStream && buffer
    }
    
// -----------------------------------------------------------------------------
// CUpnpElement::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::ExternalizeL( RWriteStream& aStream ) const
    {
	TInt ExternalizationLength = MapItemNameToProperLength(*iElementName);

	SafeExternalizeL(aStream, *iElementName, KMaxUpnpObjStringLen);
	SafeExternalizeL(aStream, *iElementValue, ExternalizationLength);
	SafeExternalize16L(aStream, *iFilePath, KMaxUpnpObjLongStringLen);

    aStream.WriteInt8L( IsRequired() );
       
    aStream.WriteInt16L( iAttributes.Count() );
    for ( TInt index(0); index < iAttributes.Count(); index++ )
        {
        iAttributes[index]->ExternalizeL( aStream );
        }
    }
        
// -----------------------------------------------------------------------------
// CUpnpElement::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::InternalizeL( RReadStream& aStream )
    {	
    delete iElementName;
    iElementName = NULL;
    iElementName = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );
    
	TInt InternalizationLength = MapItemNameToProperLength(*iElementName);

    // Element value   
    delete iElementValue;
    iElementValue = NULL;
    iElementValue = HBufC8::NewL( aStream, InternalizationLength );
    
    // FilePath
    delete iFilePath;
    iFilePath = NULL;
    iFilePath = HBufC::NewL( aStream, KMaxUpnpObjLongStringLen );
    
    // Is required
    SetIsRequired( aStream.ReadInt8L() );
      
    // cleanup
    iAttributes.ResetAndDestroy();
    
    // Attributes count
    TInt attributesCount = aStream.ReadInt16L();
	
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < attributesCount; index++ )
        {
        CUpnpAttribute* newAttribute = CUpnpAttribute::NewLC();
        newAttribute->InternalizeL( aStream );
        iAttributes.AppendL( newAttribute );
        CleanupStack::Pop( newAttribute ); 
        }
    }
// -----------------------------------------------------------------------------
// CUpnpElement::SetIsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpElement::SetIsRequired(TBool aIsRequired)
{
    iIsRequired = aIsRequired;
}
// -----------------------------------------------------------------------------
// CUpnpElement::IsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpElement::IsRequired() const
{
    return iIsRequired;
}

// -----------------------------------------------------------------------------
// CUpnpElement::SafeExternalizeL
// Writes safely aNumber of characters from the content to the stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElement::SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const
{	
    if( aDesc.Length() > aNumber )    
		aStream << aDesc.Mid(0, aNumber);
    else
		aStream << aDesc;    
}

// -----------------------------------------------------------------------------
// CUpnpElement::SafeExternalize16L
// Writes safely aNumber of characters from the content to the stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElement::SafeExternalize16L(RWriteStream& aStream, const TDesC& aDesc, const TInt aNumber) const
{	
    if( aDesc.Length() > aNumber )    
		aStream << aDesc.Mid(0, aNumber);
    else
		aStream << aDesc;    
}

// -----------------------------------------------------------------------------
// CUpnpElement::MapItemNameToProperLength
// Counts how many bytes will be used for internalization
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpElement::MapItemNameToProperLength(TDesC8& aItemName) const
{
	// DLNA Networked Device Interoperability Guidelines
	// Requirement [7.3.17.4]: The following metadata properties must not exceed 256 bytes each in the XML-escaped form encoded in UTF-8	
	if( aItemName == _L8("dc:date") ||
		aItemName == _L8("dc:creator") ||
		aItemName == _L8("upnp:genre") ||
		aItemName == _L8("upnp:album") ||
		aItemName == _L8("upnp:albumArtURI") ||
		aItemName == _L8("upnp:channelNr") ||
		aItemName == _L8("upnp:channelName")
	)
    {
		return KMaxUpnpObjStringLen; // 256
    }	
	else 
	{
		return KMaxUpnpObjLongStringLen; //1024
	}
}

//  End of File  
