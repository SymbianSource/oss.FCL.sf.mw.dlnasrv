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



#ifndef C_CUPNPELEMENTBEAN_H
#define C_CUPNPELEMENTBEAN_H

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
class CUpnpElementBean: public CUpnpTableBean 
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpElementBean* NewLC();

    /**
    * Two-phased constructor.
    * @param aRowSet
    */
    static CUpnpElementBean* NewLC(const RDbRowSet& aRowSet);
    
    
    /**
    * Destructor.
    */
    virtual ~CUpnpElementBean();

public: // New functions
    
        CUpnpElement* AttachElmL(CUpnpObject* aElm);
    /**
    * Sets all values of the object value.
    * @since Series S60 3.0
    * @param aRowSet
    */
    void SetL(const RDbRowSet& aRowSet);
    
    /**
    * Sets iElmId value.
    * @since Series S60 3.0
    * @param aElId value of elm_id in the element table
    */
    void SetElmId(TInt aElId);
        
    /**
    * Sets iElmName value.
    * @since Series S60 3.0
    * @param aElmName value of elm_name in the element table
    */
    void SetElmNameL(const TDesC8& aElmName);
        
    /**
    * Sets iElmHasAttribute value.
    * @since Series S60 3.0
    * @param aElmHasAttribute value of elm_has_attribute in the element table
    */
    void SetElmHasAttribute(TBool aElmHasAttribute);
        
    /**
    * Sets iElmIsRequired value.
    * @since Series S60 3.0
    * @param aElmIsRequired value of elm_is_required in the element table
    */
    void SetElmIsRequired(TBool aElmIsRequired);
        
    /**
    * Sets iElmValue value.
    * @since Series S60 3.0
    * @param aElmValue value of elm_value in the element table
    */
    void SetElmValueL(const RDbRowSet& aRowSet, const TInt aColNo);
		void SetElmValueL(const TDesC8& aVal);
        
    /**
    * Sets iElmObjId value.
    * @since Series S60 3.0
    * @param aElmObjId value of elm_obj_id in the element table
    */
    void SetElmObjId(TInt aElmObjId);
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */
    TInt ElmId() const;
        
    /**
    * Gets iElmName value.
    * @since Series S60 3.0
    * @return Value of iElmName
    */
		TDesC8& ElmName() const;
        
    /**
    * Gets iElmHasAttribute value.
    * @since Series S60 3.0
    * @return Gets the value of iElmHasAttribute
    */
    TBool ElmHasAttribute() const;
        
    /**
    * Gets iElmIsRequired value.
    * @since Series S60 3.0
    * @return Gets the value of iElmIsRequired
    */
    TBool ElmIsRequired() const;
        
    /**
    * Gets iElmValue value.
    * @since Series S60 3.0
    * @return Value of iElmValue
    */
    TDesC8& ElmValue() const;
        
    /**
    * Gets iElmObjId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmObjId
    */
    TInt ElmObjId() const;
        
    /**
    * Adds the whole element to the given TXmlEngElement.
    * @param aElement a element to which the value will be attached
    * @since Series S60 3.0
    */
    TXmlEngElement AttachElmL(TXmlEngElement aElement) const;


private:

    /**
    * C++ default constructor.
    */
    CUpnpElementBean();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    HBufC8* ValAsUriL() const;

private:    // Data
    // Element Id
    TInt        iElmId;
    // Element Name
    HBufC8*     iElmName;
    //
    TBool       iElmHasAttribute;
    // 
    TInt        iElmObjId;
    //
    TBool       iElmIsRequired;
    // Element Value
    HBufC8*     iElmValue;
};

// inline functions
#include"upnpelementbean.inl"

#endif      // C_CUPNPELEMENTBEAN_H   
        
// End of File
