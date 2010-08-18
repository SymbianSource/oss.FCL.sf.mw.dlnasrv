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
#include <xml/dom/xmlengdom.h>
#include "upnpelementbean.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpcdutils.h"
#include <upnpelement.h>
#include <upnpobject.h>
#include <uriutils.h>
#include "upnpstring.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpElementBean::CUpnpElementBean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpElementBean::CUpnpElementBean()
{
}

// -----------------------------------------------------------------------------
// CUpnpElementBean::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpElementBean::ConstructL()
{
	iElmValue = KNullString8().AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpElementBean* CUpnpElementBean::NewLC()
{
    CUpnpElementBean* self = new( ELeave ) CUpnpElementBean;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpElementBean* CUpnpElementBean::NewLC(const RDbRowSet& aRowSet)
{
    CUpnpElementBean* self = NewLC();
    self->SetL(aRowSet);
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpElementBean::~CUpnpElementBean
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpElementBean::~CUpnpElementBean()
{
    delete iElmName;
    delete iElmValue;
}

// -----------------------------------------------------------------------------
// CUpnpElementBean::SetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElementBean::SetL(const RDbRowSet& aRowSet)
{
    // get cols' ids
    CDbColSet* colSet = aRowSet.ColSetL();
    CleanupStack::PushL(colSet);
    const TInt  idColNo         = colSet->ColNo(KElmIdColName);
    const TInt  nameColNo       = colSet->ColNo(KElmNameColName);
    const TInt  hasAttrColNo    = colSet->ColNo(KElmHasAttrColName);
    const TInt  objIdColNo      = colSet->ColNo(KElmObjIdColName);
    const TInt  isRequiredColNo = colSet->ColNo(KElmIsRequiredColName);
    const TInt  valueColNo      = colSet->ColNo(KElmValueColName);
    CleanupStack::PopAndDestroy(colSet);
    
    // for each column call setter
    if( idColNo != KDbNullColNo )
    {
        SetElmId( aRowSet.ColInt(idColNo) );
    }
    if( nameColNo != KDbNullColNo )
    {
        SetElmNameL( aRowSet.ColDes8(nameColNo) );
    }
    if( hasAttrColNo != KDbNullColNo )
    {
        SetElmHasAttribute( aRowSet.ColUint8(hasAttrColNo) );
    }
    if( objIdColNo != KDbNullColNo )
    {
        SetElmObjId( aRowSet.ColInt(objIdColNo) );
    }
    if( isRequiredColNo != KDbNullColNo )
    {
        SetElmIsRequired( aRowSet.ColUint8(isRequiredColNo) );
    }
    if( valueColNo != KDbNullColNo )
    {
        SetElmValueL(aRowSet, valueColNo);
    }
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElementBean::SetElmNameL(const TDesC8& aElmName)
{
    delete iElmName;
    iElmName = NULL;
    iElmName = aElmName.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElementBean::SetElmValueL(const RDbRowSet& aRowSet, const TInt aColNo)
{
    delete iElmValue;
    iElmValue = NULL;
    iElmValue = ReadLongTextColL(aRowSet, aColNo);
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpElementBean::SetElmValueL(const TDesC8& aVal)
{
	delete iElmValue;
	iElmValue = NULL;
	iElmValue = aVal.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpElementBean::ElmName() const
{
    return *iElmName;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::AttachToXmlElmL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpElementBean::AttachElmL(TXmlEngElement aElement) const
{
    // new element
    TXmlEngElement element;
    
    // Check if it's local name or if it has a namespace prefix also
    TInt pos = iElmName->Find( KColon8 );
    if( pos == KErrNotFound )
    { //local name
        element = aElement.AddNewElementL( *iElmName );
    }
    else    
    { //local name with prefix
        element = aElement.AddNewElementUsePrefixL( 
                    iElmName->Right( iElmName->Length() - pos - 1 ), // name
                    iElmName->Left( pos ) ); // prefix
    }

    // value     
    element.SetValueL( *iElmValue );

    return element;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ValAsUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpElementBean::ValAsUriL() const 
{    
    // uris have to be properly escaped
    HBufC* tmp = UpnpCdUtils::Des8ToDesLC(*iElmValue);        
    TUriParser uripar;
    uripar.Parse(*tmp);
    CUri8* uri = uri = UriUtils::ConvertToInternetFormL(uripar);
    CleanupStack::PopAndDestroy(tmp);        
    if (uri->Uri().UriDes().Length())
    {                
        HBufC8* tmp2 = UpnpString::StringReplaceL(uri->Uri().UriDes(),KHash8,KHashASCII8);        
        delete uri;
        return tmp2;                         
    }
    HBufC8* uriDes = uri->Uri().UriDes().Alloc();
    delete uri;
    return uriDes;
        
}

// -----------------------------------------------------------------------------
// CUpnpElementBean::AttachElmL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpElement* CUpnpElementBean::AttachElmL(CUpnpObject* aObj)
{
    // create element
    CUpnpElement* elm = CUpnpElement::NewLC(*iElmName);
	
	// copy values
    elm->SetValueL(*iElmValue);

	// is required
	elm->SetIsRequired( ElmIsRequired() );
	// attach
	aObj->AddElementL(elm);
	
	// clean up
	CleanupStack::Pop(elm);
	
	return elm;
}


//  End of File  
