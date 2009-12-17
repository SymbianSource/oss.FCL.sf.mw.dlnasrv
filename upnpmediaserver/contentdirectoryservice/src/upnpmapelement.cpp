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
#include"upnpmapelement.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMapElement::CUpnpMapElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMapElement::CUpnpMapElement(TAny* aValue)
{
    iValue = aValue;
}

// -----------------------------------------------------------------------------
// CUpnpMapElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMapElement::ConstructL(const TDesC8& aKey)
{
    iKey = aKey.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMapElement* CUpnpMapElement::NewLC(const TDesC8& aKey, TAny* iItem)
{
    CUpnpMapElement* self = new( ELeave ) CUpnpMapElement(iItem);
    CleanupStack::PushL( self );
    self->ConstructL(aKey);
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMapElement* CUpnpMapElement::NewL(const TDesC8& aKey, TAny* iItem)
{
    CUpnpMapElement* self = CUpnpMapElement::NewLC(aKey, iItem);
    CleanupStack::Pop(self);
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::~CUpnpMapElement
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpMapElement::~CUpnpMapElement()
{
    delete iKey; 
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::Key
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpMapElement::Key()
{
    return *iKey;
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::Item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CUpnpMapElement::Value()
{
    return iValue;
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::Next
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpMapElement* CUpnpMapElement::Next()
{
    return iNext;
}
// -----------------------------------------------------------------------------
// CUpnpMapElement::SetNext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMapElement::SetNext(CUpnpMapElement* aNext)
{
    iNext = aNext;
}

//  End of File  
