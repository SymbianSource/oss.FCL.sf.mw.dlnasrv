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


#ifndef UPNPRESOURCESBEAN_H
#define UPNPRESOURCESBEAN_H

//  INCLUDES
#include "upnptablebean.h"
#include <e32base.h>
#include <d32dbms.h>

class CUpnpElement;

// CLASS DECLARATION
/**
*  Stores the data of resources table row.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpResourcesBean: public CUpnpTableBean 
{
    public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpResourcesBean* NewLC();
    static CUpnpResourcesBean* NewL(const RDbRowSet& aRowSet);
    static CUpnpResourcesBean* NewLC(const RDbRowSet& aRowSet);
    
    /**
    * Destructor.
    */
    virtual ~CUpnpResourcesBean();

    public: // New functions
    /**
    * Sets  value.
    * @since Series S60 3.0
    * @param  value of  in the element table
    */
    void SetL(const RDbRowSet& aRowSet);

    /**
    * Sets iId value.
    * @since Series S60 3.0
    * @param aId value of rsc_id in the 'shareMap' table
    */
    void SetId(TInt64 aId);

    /**
    * Sets iObjectId value.
    * @since Series S60 3.0
    * @param aObjectId value of rsc_object_id in the 'shareMap' table
    */
    void SetObjectId(TInt aObjectId);
    
    /**
    * Sets iPath value.
    * @since Series S60 3.0
    * @param aPath value of rsc_path in the 'resources' table
    */
    void SetPathL(const TDesC& aPath);
    void SetPathL(const TDesC8& aPath);

    /**
    * Sets iIsReadonly value.
    * @since Series S60 3.0
    * @param IsReadonly value of rsc_is_readonly in the 'resources' table
    */
    void SetIsReadonly(TBool aIsReadonly);
    
    /**
    * Sets iIsThumbnail value.
    * @since Series S60 3.0
    * @param IsThumbnail value of rsc_is_thumbnail in the 'resources' table
    */
    void SetIsThumbnail(TBool aIsThumbnail);

    /**
    * Gets iId value.
    * @since Series S60 3.0
    * @return Gets the value of iId
    */  
    TInt64 Id() const;
 
    /**
    * Gets iObjectId value.
    * @since Series S60 3.0
    * @return Gets the value of iObjectId
    */  
    TInt ObjectId() const;
        
    /**
    * Gets iPath value.
    * @since Series S60 3.0
    * @return Gets the value of iPath
    */  
    TDesC& Path() const;
        
        
    /**
    * Gets iIsReadonly value.
    * @since Series S60 3.0
    * @return Gets the value of iIsReadonly
    */  
    TInt IsReadonly() const;
    
    /**
    * Gets iIsThumbnail value.
    * @since Series S60 3.0
    * @return Gets the value of iIsThumbnail
    */  
    TInt IsThumbnail() const;
        
    private:

    /**
    * C++ default constructor.
    */
    CUpnpResourcesBean();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    private:    // Data
    // Id
    TInt64    iId;
    // ObjectId
    TInt    iObjectId;
    // Path
    HBufC*  iPath;
    //
    TInt    iIsReadonly;
    //
    TInt    iIsThumbnail;
};

// inline functions
#include"upnpresourcesbean.inl"

#endif      // UPNPRESOURCESBEAN_H
// End of File