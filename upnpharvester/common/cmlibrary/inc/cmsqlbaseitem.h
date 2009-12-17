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






#ifndef __CMSQLBASEITEM_H
#define __CMSQLBASEITEM_H

// INCLUDES
#include <e32base.h>
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class CSHA1;

/**
 *  CCmSqlBaseItem class
 *  Capsulating sql items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlBaseItem : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlBaseItem class.
     * @param None
     * @return  pointer to CCmSqlBaseItem class
     */
    IMPORT_C static CCmSqlBaseItem* NewL();
    
    /**
     * Creates new CCmSqlBaseItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlBaseItem class
     */
    IMPORT_C static CCmSqlBaseItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlBaseItem();

public:

    /**
    * Sets database id 
    * @since Series 60 3.1
    * @param aId, database id
    * @return None
    */
    IMPORT_C void SetId( const TInt64 aId );
 
    /**
    * Sets cds id ( received from media server )
    * @since Series 60 3.1
    * @param aCdsId, cds id
    * @return None
    */   
    IMPORT_C void SetCdsIdL( const TDesC8& aCdsId );

    /**
    * Sets hash value
    * @since Series 60 3.1
    * @param aHash, hash value
    * @return None
    */    
    IMPORT_C void SetHashL( const TDesC& aHash );

    /**
    * Sets search id of item
    * @since S60 3.1
    * @param aSearchId, search id
    * @return None
    */     
    IMPORT_C void SetSearchId( const TInt64 aSearchId );
    
    /**
    * Gets database id
    * @since Series 60 3.1
    * @param None
    * @return Database id
    */ 
    IMPORT_C TInt64 Id() const;

    /**
    * Gets cds id ( received from media server )
    * @since Series 60 3.1
    * @param None
    * @return CdsId
    */    
    IMPORT_C TDesC8& CdsId() const;
    
    /**
    * Gets hash value
    * @since Series 60 3.1
    * @param None
    * @return Hash value
    */    
    IMPORT_C TDesC& Hash() const;

    /**
    * Gets search id of item
    * @since Series 60 3.1
    * @param None
    * @return Search id
    */      
    IMPORT_C TInt64 SearchId() const;
    
    /**
    * Compares items
    * @since Series 60 3.1
    * @param aFirst, first item
    * @param aSecond, second item
    * @return Comparison result
    */     
    IMPORT_C static TInt CompareByHash( const CCmSqlBaseItem& aFirst,
                                        const CCmSqlBaseItem& aSecond );
                               
    /**
    * Compares items
    * @since Series 60 3.1
    * @param aFirst, first item
    * @param aSecond, second item
    * @return Comparison result
    */     
    IMPORT_C static TInt CompareByCdsId( const CCmSqlBaseItem& aFirst,
                                         const CCmSqlBaseItem& aSecond );
     
protected:

    /**
     * Constructor.
     */
    CCmSqlBaseItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

protected: // data
  
    // Database id
    TInt64 iId;
    
    // Item id received from media server
    HBufC8* iCdsId; // owned
    
    // Calculated hash code
    HBufC* iHash; // owned
    
    // Search id
    TInt64 iSearchId;
    
    };

#endif //  __CMSQLBASEITEM_H