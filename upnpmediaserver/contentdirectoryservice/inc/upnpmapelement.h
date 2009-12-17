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



#ifndef UPNPMAPELEMENT_H
#define UPNPMAPELEMENT_H

//  INCLUDES
#include "upnptablebean.h"
#include <e32base.h>
#include <d32dbms.h>

class CUpnpElement;
class CUpnpObject;
class CUri8;

// CLASS DECLARATION

/**
*  Stores the data of element table row.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpMapElement: public CBase 
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpMapElement* NewLC(const TDesC8& aKey, TAny* aValue);
    static CUpnpMapElement* NewL(const TDesC8& aKey, TAny* aValue);

    /**
    * Destructor.
    */
    virtual ~CUpnpMapElement();

public: // New functions
    
    /**
    * Adds the whole element to the given TXmlEngElement.
    * @param aElement a element to which the value will be attached
    * @since Series S60 3.0
    */
    const TDesC8& Key();
    TAny* Value();
    CUpnpMapElement* Next();
    void SetNext(CUpnpMapElement* aNext);

private:

    /**
    * C++ default constructor.
    */
    CUpnpMapElement(TAny* iItem);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(const TDesC8& aKey);
    

private:    // Data
    HBufC8*             iKey; // owned
    TAny*               iValue;// not owned
    CUpnpMapElement*    iNext;// not owned
};

#endif      // UPNPMAPELEMENT_H   
        
// End of File
