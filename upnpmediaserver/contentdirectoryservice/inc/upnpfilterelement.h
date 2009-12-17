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
* Description:  Contains data for filtering
*
*/



#ifndef C_CUPNPFILTERELEMENT_H
#define C_CUPNPFILTERELEMENT_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

/**
*  Contains data for filtering
*/
class CUpnpFilterElement: public CBase
{
 public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpFilterElement* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CUpnpFilterElement();

 public: // New functions
    
    /**
    * Set the iElementName value.
    * @since Series S60 3.0
    * @param aElName the name of the element
    */
    void SetElementNameL( const TDesC8& aElName );

    /**
    * Adds the attribute to the attributes list
    * @since Series S60 3.0
    * @param aAttrName the name of the attribute
    */
    void AppendAttributeL( const TDesC8& aAttrName );

    /**
    * Gets the number of attributes in the filter element
    * @since Series S60 3.0
    * @return Number of attributes
    */
    TInt Count() const;

    /**
    * Gets element name
    * @since Series S60 3.0
    * @return Non modifable pointer descriptor to the name
    */
    TPtrC8 ElementName() const;

    /**
    * Gets the name of the attribute located at the specified position
    * @since Series S60 3.0
    * @return Non modifable pointer descriptor to the attribute name
    */
    TPtrC8 operator[](TInt aPos) const;

private:

    /**
    * C++ default constructor.
    */
    CUpnpFilterElement();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

 private:    // Data
    //Element name
    HBufC8*         iElementName;
    
    //Array of element's attributes
    CDesC8ArrayFlat*    iAttributes;

};

#endif      // C_CUPNPFILTERELEMENT_H   
        
// End of File
