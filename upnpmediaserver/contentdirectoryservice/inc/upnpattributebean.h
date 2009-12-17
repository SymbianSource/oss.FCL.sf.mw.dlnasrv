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



#ifndef C_CUPNPATTRIBUTEBEAN_H
#define C_CUPNPATTRIBUTEBEAN_H

//  INCLUDES
#include "upnptablebean.h"
#include <e32base.h>
#include <d32dbms.h>

class CUpnpAttribute;
class CUpnpElement;

/**
*  Stores the data of attribute table row.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpAttributeBean: public CUpnpTableBean
{
    public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpAttributeBean* NewLC();
    
    /**
    * Two-phase constructor
    * aparam aRowSet a rowset of the attribute table
    */
    static CUpnpAttributeBean* NewLC(const RDbRowSet& aRowSet);
    
    
    /**
    * Destructor.
    */
    virtual ~CUpnpAttributeBean();

    public: // New functions
    
        void AttachAttrL(CUpnpElement* aElm);
    /**
    * Sets the RDbRowset and sets all values from this rowset.
    * @since Series S60 3.0
    * @param aRowSet a rowset of the attribute table
    */
    void SetL(const RDbRowSet& aRowSet);
    
    /**
    * Sets iAtrId value.
    * @since Series S60 3.0
    * @param aElId value of atr_id in the Attribute table
    */
    void SetAtrId(TInt aElId);
        
    /**
    * Sets iAtrName value.
    * @since Series S60 3.0
    * @param aAtrName value of Atr_name in the Attribute table
    */
    void SetAtrNameL(const TDesC8& aAtrName);
        
    /**
    * Sets iAtrIsRequired value.
    * @since Series S60 3.0
    * @param aAtrIsRequired value of Atr_is_required in the Attribute table
    */
    void SetAtrIsRequired(TBool aAtrIsRequired);
        
    /**
    * Sets iAtrValue value.
    * @since Series S60 3.0
    * @param aAtrValue value of Atr_value in the Attribute table
    */
    void SetAtrValueL(const RDbRowSet& aRowSet, const TInt aColNo);
		void SetAtrValueL(const TDesC8& aVal);
        
    /**
    * Sets iAtrObjId value.
    * @since Series S60 3.0
    * @param aAtrObjId value of Atr_obj_id in the Attribute table
    */
    void SetAtrElmId(TInt aAtrObjId);
        
    /**
    * Gets iAtrId value.
    * @since Series S60 3.0
    * @return Gets the value of iAtrId
    */
    TInt AtrId() const;
        
    /**
    * Gets iAtrName value.
    * @since Series S60 3.0
    * @return Value of iAtrName
    */
    TDesC8& AtrName() const;
        
    /**
    * Gets iAtrIsRequired value.
    * @since Series S60 3.0
    * @return Gets the value of iAtrIsRequired
    */
    TBool AtrIsRequired() const;
        
    /**
    * Gets iAtrValue value.
    * @since Series S60 3.0
    * @return Value of iAtrValue
    */
    TDesC8& AtrValue() const;
        
    /**
    * Gets iAtrObjId value.
    * @since Series S60 3.0
    * @return Gets the value of iAtrObjId
    */
    TInt AtrElmId() const;

    /**
    * Adds attribute to the given TXmlEngElement.
    * @since Series S60 3.0
    * @param TXmlEngElement
    * @return just attached attribute
    */
    TXmlEngAttr AttachAttrL(TXmlEngElement aElement);

    private:

    /**
    * C++ default constructor.
    */
    CUpnpAttributeBean();

    /**
    * Second phase of the constructor.
    *
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    private:    // Data
    // Attribute Id
    TInt        iAtrId;
    // Attribute name buffer
    HBufC8*     iAtrName;
    //
    TBool       iAtrHasAttribute;
    //Attribute Element Id
    TInt        iAtrElmId;
    //
    TBool       iAtrIsRequired;
    // Attribute value buffer
    HBufC8*     iAtrValue;
};

// inline functions
#include"upnpattributebean.inl"

#endif      // C_CUPNPATTRIBUTEBEAN_H   
        
// End of File
