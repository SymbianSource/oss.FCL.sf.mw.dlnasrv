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



#ifndef UPNPMAPBASE_H
#define UPNPMAPBASE_H


#include <e32std.h>
#include"upnpmapelement.h"


// FORWARD DECLARATIONS

/**
* Implementation of simple map
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class CUpnpMapBase: public CBase
{
public:

    /**
    * Adds the whole element to the given TXmlEngElement.
    * @param aElement a element to which the value will be attached
    * @since Series S60 3.0
    */
	void PutL(const TDesC8& aKey, TAny* aValue);
    TAny* Get(const TDesC8& aKey);
	TAny* Remove(const TDesC8& aKey);

	virtual void Clear() = 0;

protected:

    /**
    * C++ default constructor.
    */
	CUpnpMapBase();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
	void ConstructL();

protected: // Data

	CUpnpMapElement* iFirst; // owned
};

#endif // UPNPMAPBASE_H
