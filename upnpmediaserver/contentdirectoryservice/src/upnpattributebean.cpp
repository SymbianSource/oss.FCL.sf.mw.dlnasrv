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
* Description:  Attribute table data handler
*
*/



// INCLUDE FILES
#include <xml/dom/xmlengdom.h>
#include "upnpattributebean.h"
#include "upnpcontentdirectoryglobals.h"
#include <upnpattribute.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAttributeBean::CUpnpAttributeBean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean::CUpnpAttributeBean()
{
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::ConstructL()
{
	iAtrValue = KNullString8().AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean* CUpnpAttributeBean::NewLC()
{
    CUpnpAttributeBean* self = new( ELeave ) CUpnpAttributeBean;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean* CUpnpAttributeBean::NewLC(const RDbRowSet& aRowSet)
{
    CUpnpAttributeBean* self = NewLC();
    self->SetL(aRowSet);
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::~CUpnpAttributeBean
// Destructor
// -----------------------------------------------------------------------------
// 
CUpnpAttributeBean::~CUpnpAttributeBean()
{
    delete iAtrName;
    delete iAtrValue;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::SetL(const RDbRowSet& aRowSet)
{
    // get cols' ids
    CDbColSet* colSet = aRowSet.ColSetL();
    CleanupStack::PushL(colSet);
    const TInt  idColNo         = colSet->ColNo(KAtrIdColName);
    const TInt  nameColNo       = colSet->ColNo(KAtrNameColName);
    const TInt  elmIdColNo      = colSet->ColNo(KAtrElmIdColName);
    const TInt  isRequiredColNo = colSet->ColNo(KAtrIsRequiredColName);
    const TInt  valueColNo      = colSet->ColNo(KAtrValueColName);
    CleanupStack::PopAndDestroy(colSet);
    
    // for each column call setter
    if( idColNo != KDbNullColNo )
    {
        SetAtrId( aRowSet.ColInt(idColNo) );
    }
    if( nameColNo != KDbNullColNo )
    {
        SetAtrNameL( aRowSet.ColDes8(nameColNo) );
    }
    if( elmIdColNo != KDbNullColNo )
    {
        SetAtrElmId( aRowSet.ColInt(elmIdColNo) );
    }
    if( isRequiredColNo != KDbNullColNo )
    {
        SetAtrIsRequired( aRowSet.ColUint8(isRequiredColNo) );
    }
    if( valueColNo != KDbNullColNo )
    {
        SetAtrValueL( aRowSet, valueColNo );
    }
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::SetAtrValueL(const RDbRowSet& aRowSet, const TInt aColNo)
{
    delete iAtrValue;
    iAtrValue = 0;
    iAtrValue = ReadLongTextColL(aRowSet, aColNo);
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::SetAtrValueL(const TDesC8& aVal)
{
	delete iAtrValue;
	iAtrValue = 0;
	iAtrValue = aVal.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::SetAtrNameL(const TDesC8& aAtrName)
{
    delete iAtrName;
    iAtrName = NULL;
    iAtrName = aAtrName.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AttachToXmlElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngAttr CUpnpAttributeBean::AttachAttrL(TXmlEngElement aElement)
{
    // add attribute
    TXmlEngAttr ret = aElement.AddNewAttributeL( AtrName(), AtrValue() );

     return ret;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::CopyToUpnpElementL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAttributeBean::AttachAttrL(CUpnpElement* aElm)
{
    // create element
    CUpnpAttribute* atr = CUpnpAttribute::NewLC(*iAtrName);
	
	// copy values
	atr->SetValueL(*iAtrValue);
	
	// attach
	aElm->AddAttributeL(atr);
	
	// clean up
	CleanupStack::Pop(atr);
}

//  End of File  
