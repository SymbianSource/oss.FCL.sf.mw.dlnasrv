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
#include "upnpfilterelement.h"
#include "upnpcontentdirectoryglobals.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpFilterElement::CUpnpFilterElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpFilterElement::CUpnpFilterElement()
{

}

// -----------------------------------------------------------------------------
// CUpnpFilterElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpFilterElement::ConstructL()
{
    iAttributes = new (ELeave) CDesC8ArrayFlat(KDefaultGranularity);
}
// -----------------------------------------------------------------------------
// CUpnpFilterElement::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpFilterElement* CUpnpFilterElement::NewLC()
{
    CUpnpFilterElement* self = new( ELeave ) CUpnpFilterElement;
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
}

// -----------------------------------------------------------------------------
// CUpnpFilterElement::~CUpnpFilterElement
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpFilterElement::~CUpnpFilterElement()
{
    delete iAttributes;
    delete iElementName;
}


// -----------------------------------------------------------------------------
// CUpnpFilterElement::Element
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpFilterElement::ElementName() const
{
    return *iElementName;
}
// -----------------------------------------------------------------------------
// CUpnpFilterElement::AppendAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpFilterElement::AppendAttributeL( const TDesC8& aAttrName )
{
    iAttributes->AppendL(aAttrName);
}
// -----------------------------------------------------------------------------
// CUpnpFilterElement::operator[]
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpFilterElement::operator[](TInt aPos) const
{
    return (*iAttributes)[aPos];
}
// -----------------------------------------------------------------------------
// CUpnpFilterElement::Count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpFilterElement::Count() const
{
    return iAttributes->Count();
}
// -----------------------------------------------------------------------------
// CUpnpFilterElement::SetElementNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpFilterElement::SetElementNameL( const TDesC8& aElName )
{
    delete iElementName;
    iElementName = NULL;
    iElementName = aElName.AllocL();
}


//  End of File  
