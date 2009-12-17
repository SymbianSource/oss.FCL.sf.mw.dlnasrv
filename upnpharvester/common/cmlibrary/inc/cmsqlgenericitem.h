/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      Capsulating sql items
*
*/






#ifndef __CMSQLGENERICITEM_H
#define __CMSQLGENERICITEM_H

// INCLUDES
#include <e32base.h>
#include "cmsqlbaseitem.h"

// FORWARD DECLARATIONS
class CCmSqlItemResource;

/**
 *  CCmSqlGenericItem class
 *    Capsulating sql items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlGenericItem : public CCmSqlBaseItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlGenericItem class.
     * @param Nonw
     * @return  pointer to CCmSqlGenericItem class
     */
    IMPORT_C static CCmSqlGenericItem* NewL();
    
    /**
     * Creates new CCmSqlGenericItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlGenericItem class
     */
    IMPORT_C static CCmSqlGenericItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlGenericItem();

public:

    /**
    * Sets size of the item
    * @since Series 60 3.1
    * @param aSize, size in bytes
    * @return None
    */
    IMPORT_C void SetSize( const TInt aSize );
    
    /**
    * Sets title of the item
    * @since Series 60 3.1
    * @param aTitle, Metadata title
    * @return None
    */    
    IMPORT_C void SetTitleL( const TDesC8& aTitle );

    /**
    * Sets date
    * @since Series 60 3.1
    * @param aDate, date
    * @return None
    */    
    IMPORT_C void SetDate( const TTime& aDate );
    
    /**
    * Sets harvesting time
    * @since Series 60 3.1
    * @param aHarvestDate, harvesting time
    * @return None
    */    
    IMPORT_C void SetHarvestDate( const TTime& aHarvestDate );    
    
    /**
    * Sets upnp class id
    * @since Series 60 3.1
    * @param aUpnpclassId, id values of the upnp class
    * @return None
    */    
    IMPORT_C void SetUpnpclassId( const TInt64 aUpnpclassId );

    /**
    * Sets upnp profile id
    * @since Series 60 3.1
    * @param aUpnpProfileId, id values of the upnp profile
    * @return None
    */    
    IMPORT_C void SetUpnpProfileId( const TInt64 aUpnpProfileId );    
    
    /**
    * Sets Item uri
    * @since Series 60 3.1
    * @param aUri, Item uri
    * @return None
    */     
    IMPORT_C void SetUriL( const TDesC8& aUri );

    /**
    * Sets media type of the item
    * @since Series 60 3.1
    * @param aMType, media type
    * @return None
    */     
    IMPORT_C void SetMediaType( const TCmMediaType aMType );
    
    /**
    * Creates new resource for the item
    * @since Series 60 3.1
    * @param aUri, uri of the resource
    * @param aSize, size of the resource
    * @param aDuration, duration of the resource
    * @param aBitrate, bitrate of the resource
    * @param aResolutionId, id to right resolution
    * @return None
    */     
    IMPORT_C void NewResourceL( const TDesC8& aUri, 
        const TInt aSize, const TInt aDuration, const TInt aBitrate,
        const TInt64 aResolutionId );    

    /**
    * Gets size of the item
    * @since Series 60 3.1
    * @return Size
    */        
    IMPORT_C TInt Size() const;

    /**
    * Gets Title of the item
    * @since Series 60 3.1
    * @return Title
    */    
    IMPORT_C TDesC8& Title() const;

    /**
    * Gets date
    * @since Series 60 3.1
    * @return Date
    */    
    IMPORT_C TTime Date() const;

    /**
    * Gets harvest date
    * @since Series 60 3.1
    * @return Harvesting date
    */    
    IMPORT_C TTime HarvestDate() const;
        
    /**
    * Gets upnp class id
    * @since Series 60 3.1
    * @return Upnp class id
    */ 
    IMPORT_C TInt64 UpnpclassId() const;

    /**
    * Gets upnp profile id
    * @since Series 60 3.1
    * @param None
    * @return Upnp profile id
    */ 
    IMPORT_C TInt64 UpnpProfileId( ) const;
    
    /**
    * Gets item uri
    * @since Series 60 3.1
    * @return Item uri
    */    
    IMPORT_C TDesC8& Uri() const;
 
    /**
    * Gets media type
    * @since Series 60 3.1
    * @return Media type
    */    
    IMPORT_C TCmMediaType MediaType() const;

    /**
    * Gets count of resources
    * @since Series 60 3.1
    * @return Resource count
    */
    IMPORT_C TInt ResourceCount() const;
        
    /**
    * Gets resource data
    * @since Series 60 3.1
    * @param aUri, uri of the resource
    * @param aSize, size of the resource
    * @param aDuration, duration of the resource
    * @param aBitrate, bitrate of the resource
    * @param aResolutionId, resolution id of the resource
    * @param aIndex, index of the resource
    * @return None
    */    
    IMPORT_C void GetResource( TDesC8& aUri, TInt& aSize, 
    TInt& aDuration, TInt& aBitrate, TInt64& aResolutionId, TInt aIndex );    
    
protected:

    /**
     * Constructor.
     */
    CCmSqlGenericItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

protected: // data  
    
    // Size of the item in bytes
    TInt iSize;
    
    // Title
    HBufC8* iTitle; // owned
    
    // Date
    TTime iDate;
    
    // Date
    TTime iHarvestDate;    
    
    // Upnp class id
    TInt64 iUpnpclassId;
    
    // Upnp profile id
    TInt64 iUpnpProfileId;
    
    // Search id
    TInt64 iSearchId;
    
    // Uri
    HBufC8* iUri; // owned
    
    // Media type
    TCmMediaType iMType;
    
    // Resource array
    RPointerArray<CCmSqlItemResource> iResources; // items owned
    
    };

#endif //  __CMSQLGENERICITEM_H