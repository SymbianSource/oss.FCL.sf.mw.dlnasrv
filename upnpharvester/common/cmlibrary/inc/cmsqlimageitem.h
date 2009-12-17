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






#ifndef __CMSQLIMAGEITEM_H
#define __CMSQLIMAGEITEM_H

// INCLUDES
#include <e32base.h>
#include "cmsqlgenericitem.h"

// FORWARD DECLARATIONS

/**
 *  CCmSqlImageItem class
 *  Capsulating sql item
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlImageItem : public CCmSqlGenericItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlImageItem class.
     * @param None
     * @return  pointer to CCmSqlImageItem class
     */
    IMPORT_C static CCmSqlImageItem* NewL();
    
    /**
     * Creates new CCmSqlImageItem class and 
     * leaves the instance in the cleanup stack.
     * @return pointer to CCmSqlImageItem class
     */
    IMPORT_C static CCmSqlImageItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlImageItem();
  
public:

    /**
    * Sets resolution id
    * @since Series 60 3.1
    * @param aResolutionId, resolution id
    * @return None
    */ 
    IMPORT_C void SetResolutionId( const TInt64 aResolutionId );

    /**
    * Sets description field
    * @since Series 60 3.1
    * @param aDescription, description text
    * @return None
    */     
    IMPORT_C void SetDescriptionL( const TDesC8& aDescription );

    /**
    * Gets resolution id
    * @since Series 60 3.1
    * @return Resolution id
    */     
    IMPORT_C TInt64 ResolutionId() const;

    /**
    * Gets description text
    * @since Series 60 3.1
    * @return Description text
    */    
    IMPORT_C TDesC8& Description() const;
    
protected:

    /**
     * Constructor.
     */
    CCmSqlImageItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data  
    
    // Resolution id 
    TInt64 iResolutionId;
    
    // Description data
    HBufC8* iDescription; // owned
    
    };

#endif //  __CMSQLIMAGEITEM_H