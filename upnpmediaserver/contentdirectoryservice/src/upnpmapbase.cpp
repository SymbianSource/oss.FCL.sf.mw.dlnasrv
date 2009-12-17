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
* Description:  Content Directory
*
*/



// INCLUDE FILES
#include "upnpmapbase.h"

// -----------------------------------------------------------------------------
// CUpnpMapBase::CUpnpMapBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMapBase::CUpnpMapBase()
{      
}
// -----------------------------------------------------------------------------
// CUpnpMapBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMapBase::ConstructL()
{
}
// -----------------------------------------------------------------------------
// CUpnpMapBase::Get
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CUpnpMapBase::Get(const TDesC8& aKey)
{
    CUpnpMapElement* mapElement = iFirst;
    TAny* buf = NULL;
    while(mapElement != 0)
    {
        if(mapElement->Key() == aKey)
        {
            buf = mapElement->Value();
            break;
        }
        mapElement = mapElement->Next();
    }
    return buf;
}
// -----------------------------------------------------------------------------
// CUpnpMapBase::PutL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMapBase::PutL(const TDesC8& aKey, TAny* aValue)
{
    CUpnpMapElement* newEl = CUpnpMapElement::NewL(aKey, aValue);
    newEl->SetNext(iFirst);
    iFirst = newEl;
}
// -----------------------------------------------------------------------------
// CUpnpMapBase::Remove
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CUpnpMapBase::Remove(const TDesC8& aKey)
{
    if( !iFirst ) // empty map
    {
        return NULL;
    }

    TAny* removed = NULL;

    // if first element to be removed
    if( iFirst->Key() == aKey )
    {
        // remove first element
        removed = iFirst->Value();
        CUpnpMapElement* next = iFirst->Next();
        delete iFirst;
        iFirst = next;
    }
    else
    {   
        CUpnpMapElement* prevElement = iFirst;
        CUpnpMapElement* element = iFirst->Next();
        while(element != 0)
        {
            if( element->Key() == aKey )
            {
                prevElement->SetNext( element->Next() );
                removed = element->Value();
                delete element;
                break;
            }
            else
            {
                prevElement = element;
                element = element->Next();
            }
        }
    }
    return removed;
}

// End of File
