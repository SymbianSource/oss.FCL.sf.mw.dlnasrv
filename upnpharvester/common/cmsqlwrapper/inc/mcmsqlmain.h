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






#ifndef M_MCMSQLMAIN_H
#define M_MCMSQLMAIN_H

#include <e32base.h>
#include <badesca.h>
#include "cmcommon.h"
#include "cmcommontypes.h"  
            
/* Forward declarations. */
class RFs;
class CCmSqlDbMaintenance;
class CCmSqlConnection;
class CCmSqlDbClause;
class CCmBaseListItem;
class CCmSqlBaseItem;
class CCmFillRule;
class CCmFillListItem;
class CCmSqlGenericItem;
class CCmSqlPropertyItem;
class CCmBaseListItem;
class CCmSqlPropertyCollector;

/**
 *  MCmSqlMain class ( Interface to sql wrapper )
 *  Part of SQL wrapper. SQL wrapper is part of
 *  Content manager component.
 *
 *  @lib cmsqlwrapper.lib
 *
 *  @since S60 5.1
 */
class MCmSqlMain
    {
public:

    /**
     * Canceling operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    virtual void CancelAsyncOperation() = 0;

    /**
     * Setting media server id
     *
     * @since S60 5.1     
     * @param aId, id to be used
     * @return None
     */ 
    virtual void SetMsId( TUint aId ) = 0;

    /**
     * Setting mmc quota
     *
     * @since S60 5.1
     * @param aQuota, quota to be used in unlimited fill lists
     * @return None
     */     
    virtual void SetQuota( TInt64 aQuota ) = 0;

    /**
     * Setting dlna profile filters
     *
     * @since S60 5.1     
     * @param aProfileIds, unsupported profile ids
     * @return None
     */    
    virtual void SetDlnaProfileFilters( const RArray<TInt64>& aProfileIds ) = 0;
     
    /**
     * Asyncronous batch add
     *
     * @since S60 5.1
     * @param aItems, items to be added
     * @param aStatus, request status to be completed
     * @return None
     */
    virtual void AsyncBatchAdd( RPointerArray<CCmSqlGenericItem>& aItems, 
        TRequestStatus& aStatus ) = 0;

    /**
     * Asyncronous batch property value add
     *
     * @since S60 5.1
     * @param aItems, property items to be added
     * @param aField, property identifier
     * @param aStatus, request status to be completed
     * @return None
     */ 
    virtual void AsyncBatchAddPropertyItems( 
         RPointerArray<CCmSqlPropertyItem>& aItems, TCmMetadataField aField, 
         TRequestStatus& aStatus ) = 0;

    /**
     * Syncronous property value add
     *
     * @since S60 5.1
     * @param aItem, property item to be added
     * @param aField, property identifier     
     * @return None
     */         
    virtual void SyncAddPropertyItemL( CCmSqlPropertyItem& aItem, 
        TCmMetadataField aField ) = 0;
    
    /**
     * Asyncronous batch delete
     *
     * @since S60 5.1
     * @param aItems, items to be deleted
     * @param aStatus, request status to be completed
     * @return None
     */       
    virtual void AsyncBatchDelete( RPointerArray<CCmSqlBaseItem>& aItems, 
        TRequestStatus& aStatus ) = 0;

    /**
     * Asyncronous metadata delete
     *
     * @since S60 5.1
     * @param aMsIds, defines where metadata is harvested
     * @param aStatus, request status to be completed
     * @return None
     */ 
    virtual void AsyncMetadataDelete( RArray<TInt>& aMsIds, 
        TRequestStatus& aStatus ) = 0;

    /**
     * SELECT XXX oldest and delete those
     * Delete defined amount of media items
     *
     * @since S60 5.1
     * @param aType, media type identifier
     * @param aCount, amount to be deleted
     * @return None
     */    
    virtual void DeleteOldestMediaItemsL( TCmMediaType aType, 
        TInt64 aCount ) = 0;

    /**
     * Deletes unused propertys
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    virtual void DeleteUnusedPropertys( ) = 0;
        
    /**
     * Asyncronous metadata query
     *
     * @since S60 5.1
     * @param aItems, result array reference
     * @param aStatus, request status to be completed
     * @return None
     */        
    virtual void GetItemsL( RPointerArray<CCmSqlBaseItem>& aItems, 
        TRequestStatus& aStatus ) = 0;

    /**
     * Asyncronous fill list item query
     *
     * @since S60 5.1
     * @param aItems, result array reference
     * @param aRule, fill list used as a query clause parameter
     * @param aStatus, request status to be completed
     * @return None 
     */
    virtual void GetFillItemsL( RPointerArray<CCmFillListItem>& aItems,
        CCmFillRule& aRule, TRequestStatus& aStatus ) = 0;
        
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
    virtual void GetPropertyValuesL( 
        RPointerArray<CCmSqlPropertyItem>& aItems, TRequestStatus& aStatus, 
        TCmMetadataField aField, TCmMediaType aMedia = ECmAll ) = 0;

    /**
     * Asyncronous metadata filtered property values query
     *
     * @since S60 5.1
     * @param aPropertys, result array reference
     * @param aStatus, request status to be completed     
     * @return None
     */         
    virtual void GetFilteredPropertyValuesL( 
        CCmSqlPropertyCollector& aPropertys, TRequestStatus& aStatus ) = 0;

     /**
     * Returns amount of media items
     *
     * @since S60 5.1
     * @param aCount, media amount on return
     * @param aType, media type
     * @return None
     */    
    virtual void GetMediaCountL( TInt64& aCount, TCmMediaType aType ) = 0;
    
     /**
     * Returns ETrue if item exist in db
     *
     * @since S60 5.1
     * @param aItem, item
     * @param aDevId, device id
     * @return ETrue if exists
     */ 
    virtual TBool ExistsL( const CCmBaseListItem& aItem, const TInt aDevId ) = 0;

    /**
     * Deletes the object
     *
     * @since S60 5.1
     */
	virtual void Close() = 0;
	
    };

#endif //  M_MCMSQLMAIN_H
