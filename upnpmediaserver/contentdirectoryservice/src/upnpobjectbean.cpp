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
* Description:  Element table data handler
*
*/



// INCLUDE FILES
#include "upnpobjectbean.h"
#include "upnpcontentdirectoryglobals.h"
#include <upnpobject.h>
#include <upnpitem.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpObjectBean::CUpnpObjectBean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpObjectBean::CUpnpObjectBean()
{
    iObjRefId = KUndef;

}

// -----------------------------------------------------------------------------
// CUpnpObjectBean::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::ConstructL()
{

}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpObjectBean* CUpnpObjectBean::NewLC()
{
    CUpnpObjectBean* self = new( ELeave ) CUpnpObjectBean;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpObjectBean* CUpnpObjectBean::NewLC(const RDbRowSet& aRowSet)
{
    CUpnpObjectBean* self = NewLC();
    self->SetL(aRowSet);
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpObjectBean::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpObjectBean* CUpnpObjectBean::NewL(const RDbRowSet& aRowSet)
{
    CUpnpObjectBean* self = NewLC(aRowSet);
    CleanupStack::Pop();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::~CUpnpObjectBean
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpObjectBean::~CUpnpObjectBean()
{
    delete iObjTitle;
    delete iObjClass;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::SetL(const RDbRowSet& aRowSet)
{
    // get cols ids
    CDbColSet* colSet = aRowSet.ColSetL();
    CleanupStack::PushL(colSet);
    const TInt idColNo          = colSet->ColNo(KObjIdColName);
    const TInt parentIDColNo    = colSet->ColNo(KObjParentIdColName);
    const TInt titleColNo       = colSet->ColNo(KObjTiltleColName);
    const TInt classColNo       = colSet->ColNo(KObjClassColName);
    const TInt restrictedColNo  = colSet->ColNo(KObjRestrictedColName);
    const TInt searchableColNo  = colSet->ColNo(KObjSearchableColName);
    const TInt refIDColNo       = colSet->ColNo(KObjRefIdColName);  
    CleanupStack::PopAndDestroy(colSet);
    
    // for each column call setter
    if( idColNo != KDbNullColNo )
    {
        SetObjId( aRowSet.ColInt(idColNo) );
    }
    if( parentIDColNo != KDbNullColNo )
    {
        SetObjParentId( aRowSet.ColInt(parentIDColNo) );
    }
    if( titleColNo != KDbNullColNo )
    {
        SetObjTitleL( aRowSet.ColDes8(titleColNo) );
    }
    if( classColNo != KDbNullColNo )
    {
        SetObjClassL( aRowSet.ColDes8(classColNo) );
    }
    if( restrictedColNo != KDbNullColNo )
    {
        SetObjRestricted( aRowSet.ColUint8(restrictedColNo) );
    }
    if( searchableColNo != KDbNullColNo && !aRowSet.IsColNull(searchableColNo) )
    {
        SetObjSearchable( aRowSet.ColUint8(searchableColNo) );
    }
    else
    {
        SetObjSearchable( KUndef);
    }
    if( refIDColNo != KDbNullColNo && !aRowSet.IsColNull(refIDColNo) )
    {
        SetObjRefId( aRowSet.ColInt(refIDColNo) );
    }
    else
    {
        SetObjRefId(KUndef);
    }
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::SetObjTitleL(const TDesC8& aObjTitle)
{
    delete iObjTitle;
    iObjTitle = NULL;
    iObjTitle = aObjTitle.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjClassL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::SetObjClassL(const TDesC8& aObjClass)
{
    delete iObjClass;
    iObjClass = NULL;
    iObjClass = aObjClass.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachObjectElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpObjectBean::AttachObjectElL(TXmlEngElement aElement)
{
    TXmlEngElement object;
    
    // check if the object is a container
    if( ObjClass().Find( KContainerClass8 ) == 0 ) 
    { // container
        object = aElement.AddNewElementSameNsL( KContainer() );
    }
    else
    { // item
        object = aElement.AddNewElementSameNsL( KItem() );
    }
    
    // add required properties
    AttachRequiredPropertiesL(object);
    
    return object;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachRequiredPropertiesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachRequiredPropertiesL(TXmlEngElement aElement)
{
    // id
    AttachIdL(aElement);
    // parentId
    AttachParentIdL(aElement);
    // restricted
    AttachRestrictedL(aElement);
    // title
    AttachTitleL(aElement);
    // class
    AttachClassL(aElement);
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachWholeObjectElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpObjectBean::AttachWholeObjectElL(TXmlEngElement aElement)
{
    // object with required properties
    TXmlEngElement object = AttachObjectElL(aElement);
    
    // optional properties:
    // refId
    AttachRefIdL(object);
    // searchable
    AttachSearchableL(object);
    
    return object;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachRefIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachRefIdL(TXmlEngElement aElement)
{
    if(ObjRefId() != CUpnpObjectBean::KUndef)
    {
        TBuf8<KMaxIntegerLen> num;
        num.Num(ObjRefId()); 
        aElement.AddNewAttributeL( KRefID(), num ); 
    }       
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachSearchableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachSearchableL(TXmlEngElement aElement)
{
    if(ObjSearchable() != CUpnpObjectBean::KUndef)
    {
        if( ObjSearchable() )
        { // true
            aElement.AddNewAttributeL( KSearchable(), KTrueValue8() );
        }
        else
        { // false
            aElement.AddNewAttributeL( KSearchable(), KFalseValue8() );
        }
    }
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachIdL(TXmlEngElement aElement)
{
    TBuf8<KMaxIntegerLen> num;
    num.Num(ObjId()); 
    aElement.AddNewAttributeL( KIdAttrName(), num );
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachParentIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachParentIdL(TXmlEngElement aElement)
{
    TBuf8<KMaxIntegerLen> num;
    num.Num(ObjParentId()); 
    aElement.AddNewAttributeL( KParentID(), num );
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachRestrictedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachRestrictedL(TXmlEngElement aElement)
{
    if( ObjRestricted() )
    { // true
        aElement.AddNewAttributeL( KRestricted(), KTrueValue8() );
    }
    else
    { // false
        aElement.AddNewAttributeL( KRestricted(), KFalseValue8() );
    }
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachTitleL(TXmlEngElement aElement)
{
    aElement.AddNewElementUsePrefixL( KTitle(), KDcNsPrefix() ).SetValueL( ObjTitle() );
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::AttachClassL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::AttachClassL(TXmlEngElement aElement)
{
    aElement.AddNewElementUsePrefixL( KClassTagName(), KUpnpNsPrefix() ).SetValueL( ObjClass() );
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::CopyToUpnpObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpObjectBean::CopyToUpnpObjectL(CUpnpObject* aObj)
{	
	HBufC8* id = HBufC8::NewLC(KMaxIntegerLen);
    id->Des().Num(iObjId);    
	aObj->SetIdL(*id);
	CleanupStack::PopAndDestroy(id); 
	
	HBufC8* parentId = HBufC8::NewLC(KMaxIntegerLen);
    parentId->Des().Num(iObjParentId);    		
	aObj->SetParentIdL(*parentId);
	CleanupStack::PopAndDestroy(parentId); 
	
	aObj->SetTitleL(*iObjTitle);
	aObj->SetRestricted(iObjRestricted);
	aObj->SetObjectClassL(*iObjClass);
	if( iObjRefId != KUndef && aObj->ObjectType() == EUPnPItem)
	{
	    CUpnpItem* item = dynamic_cast<CUpnpItem*>(aObj);
	    HBufC8* iObjRefIdString = HBufC8::NewLC(KMaxIntegerLen);
    	iObjRefIdString->Des().Num(iObjRefId);
	    item->SetRefIdL(*iObjRefIdString);
	    CleanupStack::PopAndDestroy(iObjRefIdString); 
	}
}


//  End of File  
