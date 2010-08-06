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



#ifndef C_CUPNPOBJECTBEAN_H
#define C_CUPNPOBJECTBEAN_H

//  INCLUDES
#include "upnptablebean.h"
#include <e32base.h>
#include <d32dbms.h>
#include <xml/dom/xmlengdom.h>

class CUpnpObject;

// CLASS DECLARATION

/**
*  Stores the data of object table row.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpObjectBean: public CUpnpTableBean 
{
    public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    //static CUpnpObjectBean* NewL();
    static CUpnpObjectBean* NewLC();
    static CUpnpObjectBean* NewLC(const RDbRowSet& aRowSet);
    static CUpnpObjectBean* NewL(const RDbRowSet& aRowSet);
    
    /**
    * Destructor.
    */
    virtual ~CUpnpObjectBean();

    public: // New functions
    	void CopyToUpnpObjectL(CUpnpObject* aElement);
    /**
    * Sets 
    * @since Series S60 3.0
    * @param aRowSet
    */
    void SetL(const RDbRowSet& aRowSet);

    /**
    * Sets iObjId value.
    * @since Series S60 3.0
    * @param aObjId value of id in the 'object' table
    */
    void SetObjId(TInt aObjId);

    /**
    * Sets iObjParentId value.
    * @since Series S60 3.0
    * @param aObjParentId value of 'parentID' in the object table
    */
    void SetObjParentId(TInt aObjParentId);

    /**
    * Sets iObjTitle value.
    * @since Series S60 3.0
    * @param aObjTitle value of 'title' in the object table
    */
    void SetObjTitleL(const TDesC8& aObjTitle);

    /**
    * Sets iObjClass value.
    * @since Series S60 3.0
    * @param aObjClass value of 'class' in the object table
    */
    void SetObjClassL(const TDesC8& aObjClass);

    /**
    * Sets iObjRestricted value.
    * @since Series S60 3.0
    * @param aObjRestricted value of 'restricted' in the object table
    */
    void SetObjRestricted(TBool aObjRestricted);

    /**
    * Sets iObjSearchable value.
    * @since Series S60 3.0
    * @param aObjSearchable value of 'searchable' in the object table, -1 when undefined
    */
    void SetObjSearchable(TInt aObjSearchable);

    /**
    * Sets iObjRefId value.
    * @since Series S60 3.0
    * @param aObjRefId value of 'refID' in the object table
    */
    void SetObjRefId(TInt aObjRefId);
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TInt ObjId() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TInt ObjParentId() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TDesC8& ObjTitle() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TDesC8& ObjClass() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TBool ObjRestricted() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TInt ObjSearchable() const;
        
    /**
    * Gets iElmId value.
    * @since Series S60 3.0
    * @return Gets the value of iElmId
    */  
    TInt ObjRefId() const;
        
    /**
    * Adds object element and all required properties to the given TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    * @return TXmlEngElement just added object alement
    */  
    TXmlEngElement AttachObjectElL(TXmlEngElement aElement);
        
    /**
    * Adds object element and all properties to the given TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    TXmlEngElement AttachWholeObjectElL(TXmlEngElement aElement);

    /**
    * Adds refId attribute to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    * @return TXmlEngAttr just added object alement
    */  
    void AttachRefIdL(TXmlEngElement aElement);
    
    /**
    * Adds searchable attr to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    * @return TXmlEngAttr just added object alement
    */  
    void AttachSearchableL(TXmlEngElement aElement);
 
 private: 
    /**
    * Adds all required properties to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    void AttachRequiredPropertiesL(TXmlEngElement);
        
    /**
    * Adds id attr to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    void AttachIdL(TXmlEngElement aElement);
    
    /**
    * Adds ParentId attr to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    void AttachParentIdL(TXmlEngElement aElement);
    
    /**
    * Adds restricted attr to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    * @return TXmlEngAttr just added object alement
    */  
    void AttachRestrictedL(TXmlEngElement aElement);

    /**
    * Adds title element to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    void AttachTitleL(TXmlEngElement aElement);
    
    /**
    * Adds class element to the given object's TXmlEngElement.
    * @since Series S60 3.0
    * @param aElement
    */  
    void AttachClassL(TXmlEngElement aElement);
        
        
    private:

    /**
    * C++ default constructor.
    */
    CUpnpObjectBean();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    private:    // Data
        
    // Object Id
    TInt        iObjId;
    // Object parent Id
    TInt        iObjParentId;
    // Object Title
    HBufC8*     iObjTitle;
    // Object Class
    HBufC8*     iObjClass;
    //
    TBool       iObjRestricted;
    //
    TInt        iObjSearchable;
    // Object Reference Id
    TInt        iObjRefId;
};

// inline functions
#include"upnpobjectbean.inl"

#endif      // C_CUPNPOBJECTBEAN_H   
// End of File
