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
* Description:  UPnPAttribute implementation
*
*/


// INCLUDE FILES
#include "upnpattribute.h"
#include "upnpelement.h"



// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CUpnpAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAttribute::ConstructL(const TDesC8& aName)
{
    iName = aName.AllocL();
    iValue = KNullDesC8().AllocL();
}

// -----------------------------------------------------------------------------
// CUpnpAttribute::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAttribute* CUpnpAttribute::NewLC(const TDesC8& aName)
{
    CUpnpAttribute* self = new( ELeave ) CUpnpAttribute;
    CleanupStack::PushL( self );
    self->ConstructL(aName);
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpAttribute::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAttribute* CUpnpAttribute::NewL(const TDesC8& aName)
{
    CUpnpAttribute* self = NewLC(aName);
    CleanupStack::Pop(self);
    return self;
}    

// -----------------------------------------------------------------------------
// CUpnpAttribute::CUpnpAttribute
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAttribute::CUpnpAttribute()
    {
    }

// Destructor
CUpnpAttribute::~CUpnpAttribute()
    {
    delete iName;
    delete iValue;	
    }

// -----------------------------------------------------------------------------
// CUpnpAttribute::SetNameL
// Sets name for the attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAttribute::SetNameL( const TDesC8& aName )
    {
    HBufC8* tmp = aName.AllocL();
    delete iName;
    iName = tmp;    
    }
      
// -----------------------------------------------------------------------------
// CUpnpAttribute::Name
// Returns name of the attribute. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpAttribute::Name() const
    {
    return *iName;   
    }
    
// -----------------------------------------------------------------------------
// CUpnpAttribute::SetValueL
// Sets scope for the attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAttribute::SetValueL( const TDesC8& aValue )
    {
    HBufC8* tmp = aValue.AllocL();
    delete iValue;
    iValue = tmp;
    }
        
// -----------------------------------------------------------------------------
// CUpnpAttribute::Value
// Returns scope of the attribute. If null, returns KNullDesC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpAttribute::Value() const
    {
    return *iValue;
    }

// -----------------------------------------------------------------------------
// CUpnpAttribute::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
void CUpnpAttribute::ExternalizeL( RWriteStream& aStream ) const
    {
	TInt ExternalizationLength = MapItemNameToProperLength(*iName);

	SafeExternalizeL(aStream, *iName, KMaxUpnpObjStringLen);
	SafeExternalizeL(aStream, *iValue, ExternalizationLength);
    }
        
// -----------------------------------------------------------------------------
// CUpnpAttribute::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
void CUpnpAttribute::InternalizeL( RReadStream& aStream )
    {
    delete iName;
    iName = NULL;
    iName = HBufC8::NewL( aStream, KMaxUpnpObjStringLen );
	
	TInt InternalizationLength = MapItemNameToProperLength(*iName);

    
    delete iValue;
    iValue = NULL;
    iValue = HBufC8::NewL( aStream, InternalizationLength );
    }

// -----------------------------------------------------------------------------
// CUpnpAttribute::SafeExternalizeL
// Writes safely aNumber of characters from the content to the stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttribute::SafeExternalizeL(RWriteStream& aStream, const TDesC8& aDesc, const TInt aNumber) const
{	
    if( aDesc.Length() > aNumber )    
		aStream << aDesc.Mid(0, aNumber);
    else
		aStream << aDesc;    
}	

// -----------------------------------------------------------------------------
// CUpnpAttribute::MapItemNameToProperLength
// Counts how many bytes will be used for internalization
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpAttribute::MapItemNameToProperLength(TDesC8& aItemName) const
{
	// Requirement [7.3.17.4]: The following metadata properties must not exceed 256 bytes each in the XML-escaped form encoded in UTF-8:
	// - All length-unlimited DIDL-Lite schema defined attributes for <res>, except res@importUri.
	if( aItemName == _L8("importUri") )
    {
		return KMaxUpnpObjLongStringLen; //1024		
    }	
	else if( aItemName == _L8("duration") ||
			 aItemName == _L8("size") ||
		     aItemName == _L8("resolution") ||
		 	 aItemName == _L8("childCount")
	)
	{						
		return KMaxUpnpObjStringLen; // 256
	}
	else
	{
		return KMaxUpnpObjStringLen; // 256
	}
}
//  End of File  
