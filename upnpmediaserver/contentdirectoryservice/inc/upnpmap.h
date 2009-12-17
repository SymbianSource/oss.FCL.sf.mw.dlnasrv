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
* Description:  Declares ContentDirectory class.
*
*/



#ifndef UPNPMAP_H
#define UPNPMAP_H


#include <e32std.h>
#include "upnpmapelement.h"
#include "upnpmapbase.h"


// FORWARD DECLARATIONS

/**
* Implementation of simple map
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
template<class T>
class CUpnpMap : public CUpnpMapBase
{
public:
    /**
    * Two-phased constructor.
    */

    /**
    * Destructor.
    */
	virtual ~CUpnpMap();

    /**
    * Adds the whole element to the given TXmlEngElement.
    * @param aElement a element to which the value will be attached
    * @since Series S60 3.0
    */
	void PutL(const TDesC8& aKey, T* aValue);
    const T* Get(const TDesC8& aKey);
	T* Remove(const TDesC8& aKey);

	void Clear();

public:
    /**
    * C++ default constructor.
    */
	CUpnpMap();
};

// -----------------------------------------------------------------------------
// CUpnpMap::CUpnpMap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template<class T>
CUpnpMap<T>::CUpnpMap()
{
}
// -----------------------------------------------------------------------------
// CUpnpMap::Get
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template<class T>
const T* CUpnpMap<T>::Get(const TDesC8& aKey)
{
    return static_cast<T*>( CUpnpMapBase::Get(aKey) );
}
// -----------------------------------------------------------------------------
// CUpnpMap::Get
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template<class T>
void CUpnpMap<T>::PutL(const TDesC8& aKey, T* aValue)
{
    CUpnpMapBase::PutL(aKey, aValue);
}
// -----------------------------------------------------------------------------
// CUpnpMap::~CUpnpMap
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template<class T>
CUpnpMap<T>::~CUpnpMap()
{
    Clear();
}
// -----------------------------------------------------------------------------
// CUpnpMapBase::Clear
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
template<class T>
void CUpnpMap<T>::Clear()
{
    while(iFirst)
    {
        CUpnpMapElement* next = iFirst->Next();
        delete static_cast<T*>( iFirst->Value() );
        delete iFirst;
        iFirst = next;
    }
}


#endif // UPNPMAP_H
