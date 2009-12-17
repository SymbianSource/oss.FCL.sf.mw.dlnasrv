/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Main class in the SQL wrapper component
*
*/






#ifndef C_CMSQLMAIN_H
#define C_CMSQLMAIN_H

#include <e32base.h>
#include "mcmsqlmain.h"

// Forward declarations
class RFs;

/**
 *  CCmSqlMain class ( Interface to sql wrapper )
 *  Part of SQL wrapper. SQL wrapper is part of
 *  Content manager component.
 *
 *  @lib cmsqlwrapper.lib
 *
 *  @since S60 5.1
 */
class CCmSqlMain : public CBase,
                   public MCmSqlMain
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFs, ref to file server session
     * @return  pointer to CCmSqlMain class
     */
    static CCmSqlMain* NewL( RFs& aFs );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFs, ref to file server session
     * @return  pointer to CCmSqlMain class
     */
    static CCmSqlMain* NewLC( RFs& aFs );

    /**
     * Destructor.
     */
    virtual ~CCmSqlMain();

    /**
     * Canceling operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void CancelAsyncOperation();

    /**
     * Setting media server id
     *
     * @since S60 5.1
     * @param aId, id to be used
     * @return None
     */ 
    void SetMsId( TUint aId );

    /**
     * Setting mmc quota
     *
     * @since S60 5.1
     * @param aQuota, quota to be used in unlimited fill lists
     * @return None
     */     
    void SetQuota( TInt64 aQuota );

    /**
     * Setting dlna profile filters
     *
     * @since S60 5.1
     * @param aProfileIds, unsupported profile ids
     * @return None
     */    
    void SetDlnaProfileFilters( const RArray<TInt64>& aProfileIds );
     
    /**
     * Asyncronous batch add
     *
     * @since S60 5.1
     * @param aItems, items to be added
     * @param aStatus, request status to be completed
     * @return None
     */
    void AsyncBatchAdd( RPointerArray<CCmSqlGenericItem>& aItems, 
        TRequestStatus& aStatus );

    /**
     * Asyncronous batch property value add
     *
     * @since S60 5.1
     * @param aItems, property items to be added
     * @param aField, property identifier
     * @param aStatus, request status to be completed
     * @return None
     */ 
    void AsyncBatchAddPropertyItems( 
         RPointerArray<CCmSqlPropertyItem>& aItems, TCmMetadataField aField, 
         TRequestStatus& aStatus );

    /**
     * Syncronous property value add
     *
     * @since S60 5.1
     * @param aItem, property item to be added
     * @param aField, property identifier     
     * @return None
     */         
    void SyncAddPropertyItemL( CCmSqlPropertyItem& aItem, 
        TCmMetadataField aField );
    
    /**
     * Asyncronous batch delete
     *
     * @since S60 5.1
     * @param aItems, items to be deleted
     * @param aStatus, request status to be completed
     * @return None
     */       
    void AsyncBatchDelete( RPointerArray<CCmSqlBaseItem>& aItems, 
        TRequestStatus& aStatus );

    /**
     * Asyncronous metadata delete
     *
     * @since S60 5.1
     * @param aMsIds, defines where metadata is harvested
     * @param aStatus, request status to be completed
     * @return None
     */ 
    void AsyncMetadataDelete( RArray<TInt>& aMsIds, 
        TRequestStatus& aStatus );

    /**
     * SELECT XXX oldest and delete those
     * Delete defined amount of media items
     *
     * @since S60 5.1
     * @param aType, media type identifier
     * @param aCount, amount to be deleted
     * @return None
     */    
    void DeleteOldestMediaItemsL( TCmMediaType aType, 
        TInt64 aCount );

    /**
     * Deletes unused propertys
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void DeleteUnusedPropertys( );
        
    /**
     * Asyncronous metadata query
     *
     * @since S60 5.1
     * @param aItems, result array reference
     * @param aStatus, request status to be completed
     * @return None
     */        
    void GetItemsL( RPointerArray<CCmSqlBaseItem>& aItems, 
        TRequestStatus& aStatus );

    /**
     * Asyncronous fill list item query
     *
     * @since S60 5.1
     * @param aItems, result array reference
     * @param aRule, fill list used as a query clause parameter
     * @param aStatus, request status to be completed
     * @return None 
     */
    void GetFillItemsL( RPointerArray<CCmFillListItem>& aItems,
        CCmFillRule& aRule, TRequestStatus& aStatus );
        
    /**
     * Asyncronous metadata property values query
     *
     * @since S60 5.1
     * @param aItems, result array reference
     * @param aStatus, request status to be completed     
     * @param aField, property identifier ( e.g. artist )
     * @param aMedia, media type identifier ( e.g. audio )
     * @return None
     */   
    void GetPropertyValuesL( 
        RPointerArray<CCmSqlPropertyItem>& aItems, TRequestStatus& aStatus, 
        TCmMetadataField aField, TCmMediaType aMedia = ECmAll );

    /**
     * Asyncronous metadata filtered property values query
     *
     * @since S60 5.1
     * @param aPropertys, result array reference
     * @param aStatus, request status to be completed     
     * @return None
     */         
    void GetFilteredPropertyValuesL( 
        CCmSqlPropertyCollector& aPropertys, TRequestStatus& aStatus );

     /**
     * Returns amount of media items
     *
     * @since S60 5.1
     * @param aCount, media amount on return
     * @param aType, media type
     * @return None
     */    
    void GetMediaCountL( TInt64& aCount, TCmMediaType aType );
    
     /**
     * Returns ETrue if item exist in db
     *
     * @since S60 5.1
     * @param aItem, item
     * @param aDevId, device id
     * @return ETrue if exists
     */ 
    TBool ExistsL( const CCmBaseListItem& aItem, const TInt aDevId );
    
    /**
     * Deletes the object
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */
	void Close();
	
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aFs, file server
     * @return None
     */
    CCmSqlMain( RFs& aFs );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:

    /**
     * Clause format class
     */
    CCmSqlDbClause* iClause;                        // Owned
    
    /**
     * Database maintenance class
     */
    CCmSqlDbMaintenance* iMaintenance;              // Owned
    
    /**
     * Database connection class
     */
    CCmSqlConnection* iConnection;                  // Owned
    
    /**
     * Reference to file server session
     */
    RFs& iFs;

    };

#endif //  C_CMSQLMAIN_H
