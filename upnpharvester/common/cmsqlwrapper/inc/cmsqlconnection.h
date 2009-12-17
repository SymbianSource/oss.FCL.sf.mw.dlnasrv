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
* Description:      SQLite Connection class
*
*/







#ifndef C_CMSQLCONNECTION_H
#define C_CMSQLCONNECTION_H

#include <e32base.h>
#include <sqldb.h>
#include "cmcommontypes.h"

/* Forward declarations. */
class CCmSqlBaseItem;
class CCmSqlGenericItem;
class CCmSqlPropertyItem;
class CCmBaseListItem;
class CCmFillListItem;
class CCmFillRule;
class CCmSqlPropertyCollector;
class CCmSqlPropertyContainer;

// Constants
const TUint KCmSqlBatchSize = 100;

// CLASS DECLARATION
 /**
  *  Class represents methods to communication with SQLite database API.
  *
  *  @lib cmsqlwrapper.lib
  *
  *  @since S60 5.1
  */
class CCmSqlConnection : public CBase
    {

private:

    // Operation modes
    enum TCmSqlOperationMode
        {
        ECmSqlFillItemGet = 1,
        ECmSqlBaseItemGet,
        ECmSqlPropertyValueGet,
        ECmSqlGenericItemAdd,
        ECmSqlPropertyItemAdd,
        ECmSqlIdle,
        ECmSqlDeletingMetadata,
        ECmSqlFilteredPropertyGet        
        };
public:
        
    /**
     * Two-phased constructor.
     */
    static CCmSqlConnection* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCmSqlConnection* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmSqlConnection();

    /**
     * Open database (creates if doesn't exist)
     *
     * @since S60 5.1
     * @param aDb, Database filename and path
     * @return error code
     */        
    TInt OpenDb( const TDesC& aDb );

    /**
     * Close opened database
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void CloseDb();

    /**
     * Creates db file
     * @since S60 5.1
     * @param aDb, Database filename and path
     * @return error code
     */  
    TInt CreateDbFile( const TDesC& aDb ); 
     
    /**
     * Validates database existence and format
     *
     * @since S60 5.1
     * @param None
     * @return ETrue if db valid
     */    
    TBool Validate();

    /**
     * Executes sql command
     *
     * @since S60 5.1
     * @param aCommand, command to be executed
     * @return None
     */    
    void ExecuteL( const TDesC8& aCommand );
    
    /**
     * Cance asyncronous operation
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
     * @param aId, media server id to be used
     * @return None
     */ 
    void SetMsId( TUint aId );

    /**
     * Setting mmc quota
     *
     * @since S60 5.1
     * @param aQuota, quota to be used
     * @return None
     */     
    void SetQuota( TInt64 aQuota );        

    /**
     * Asyncronous batch add
     *
     * @since S60 5.1
     * @param aItems, items to be inserted into database
     * @param aStatus, request status to be completed after insert is ready
     * @return None
     */
    void AsyncBatchAdd( RPointerArray<CCmSqlGenericItem>& aItems, 
        TRequestStatus& aStatus );

    /**
     * Asyncronous property item batch add
     *
     * @since S60 5.1
     * @param aItems, items to be inserted into database
     * @param aField, property type identifier
     * @param aStatus, request status to be completed after insert is ready
     * @return None
     */
    void AsyncBatchAddPropertyItems( 
        RPointerArray<CCmSqlPropertyItem>& aItems, TCmMetadataField aField,
        TRequestStatus& aStatus );

    /**
     * Syncronous property item add
     *
     * @since S60 5.1
     * @param aItem, item to be inserted into database
     * @param aField, property type identifier
     * @return None
     */    
    void SyncAddPropertyItemL( 
        CCmSqlPropertyItem& aItem, TCmMetadataField aField );

    /**
     * Asyncronous item batch delete
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
    void DeleteOldestMediaItemsL( TCmMediaType aType, TInt64 aCount );

    /**
     * Deletes unused propertys
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void DeleteUnusedPropertys( );
                
     /**
     * Asyncronous metadata column qyery
     *
     * @since S60 5.1
     * @param aItems, listed base items ( included only ItemId, dbId and 
     *                 Hash code )
     * @param aStatus, request status to be completed when result set ready
     * @return None
     */   
    void GetItemsL( RPointerArray<CCmSqlBaseItem>& aItems,
        TRequestStatus& aStatus );
        
     /**
     * Asyncronous metadata qyery for fill list
     *
     * @since S60 5.1
     * @param aItems, listed fill list items
     * @param aClause, sql clause to be executed
     * @param aRule, used fill list
     * @param aStatus, request status to be completed when result set ready
     * @return None
     */   
    void GetFillItemsL( RPointerArray<CCmFillListItem>& aItems,
        TDesC8& aClause, CCmFillRule& aRule, TRequestStatus& aStatus );
                
     /**
     * Asyncronous metadata column qyery
     *
     * @since S60 5.1
     * @param aItems, listed property values
     * @param aClause, sql clause to be executed
     * @param aStatus, request status to be completed when result set ready
     * @return None
     */   
    void GetPropertyValuesL( RPointerArray<CCmSqlPropertyItem>& aItems,
        TDesC8& aClause, TRequestStatus& aStatus );

    /**
     * Asyncronous metadata filtered property values query
     *
     * @since S60 5.1
     * @param aPropertys, result array reference
     * @param aClause, sql clause to be executed     
     * @param aStatus, request status to be completed     
     * @return None
     */         
    void GetFilteredPropertyValuesL( 
        CCmSqlPropertyCollector& aPropertys, const TDesC8& aClause, 
        TRequestStatus& aStatus );
        
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
                 
private:

    /**
     * Static callback method for db handling
     *
     * @since S60 5.1
     * @param aDbHandler, database handler
     * @return None
     */
    static TInt BatchAdd( TAny* aDbHandler );

    /**
     * Adds items into db in background
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    TInt DoBatchAdd();

    /**
     * Static callback method for db handling
     *
     * @since S60 5.1
     * @param aDbHandler, database handler
     * @return None     
     */    
    static TInt BatchDelete( TAny* aDbHandler );
    
    /**
     * Deletes items from db 
     *
     * @since S60 5.1
     * @param None     
     * @return None     
     */    
    TInt DoBatchDelete();
    
    /**
     * Static callback method for db handling
     *
     * @since S60 5.1
     * @param aDbHandler, database handler
     * @return None     
     */
    static TInt BatchGetL( TAny* aDbHandler );
    
    /**
     * Gets items from db
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */    
    TInt DoBatchGetL();
    
    /**
     * Collect item data
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */
    void CollectItemDataL();

    /**
     * Adds generic item into db
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */    
    void AddItemL();
    
    /**
     * Adds property item into db
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */    
    void AddPropertyItemL();

    /**
     * Adds resource item into db
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */     
    void AddResourceL();

    /**
     * Deletes items from the item table
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */    
    void DeleteItemL();

    /**
     * Deletes resource from the resources table
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */     
    void DeleteResourceL();

    /**
     * Deletes metadata from defined server
     *
     * @since S60 5.1
     * @param aMsId, mediaserver id
     * @return None     
     */      
    void DeleteMetadataFromDefServerL( const TInt aMsId );

    /**
     * Formats row count query clause, used in property
     * indexing
     *
     * @since S60 5.1
     * @param aField, metadata field
     * @return None     
     */
    void FormatRowCountQueryL( TCmMetadataField aField );
    
    /**
     * Returns row count, used in property indexing
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */    
    TInt64 RowCountL();

    /**
     * Helper function for format maximun index query 
     *
     * @since S60 5.1
     * @param aId, item id
     * @param aTable, table name
     * @return None     
     */ 
    void FormatCountQueryL(  const TDesC8& aId, const TDesC8& aTable );
    
    /**
     * Helper function for getting filtered propertys
     *
     * @since S60 5.1
     * @param aContainer, propertycontainer
     * @param aIndex1, column index
     * @param aIndex2, column index
     * @return None     
     */     
    void GetFilteredPropertysL( CCmSqlPropertyContainer& aContainer, 
        const TInt aIndex1, const TInt aIndex2 );
    
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmSqlConnection();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
        
private:

    /**
     * iDatabase, interface to Sqlite database
     */    
    RSqlDatabase iDatabase;
    
    /**
     * iStatement, interface to Sqlite database
     */
    RSqlStatement iStatement;
    
    /**
     * Pending request status
     */
    TRequestStatus* iStatus;

    /**
     * Media server id
     */
    TUint iMsId;
    
    /**
     * User mmc quota
     */
    TInt64 iQuota;
    
    /**
     * Generic item array pointer
     */
    RPointerArray<CCmSqlGenericItem>* iGenericItems;            // not owned
    
    /**
     * Base item array pointer
     */
    RPointerArray<CCmSqlBaseItem>* iBaseItems;                  // not owned
    
    /**
     * Fill list item array pointer
     */
    RPointerArray<CCmFillListItem>* iFillItems;                 // not owned
    
    /**
     * Property item array pointer
     */
    RPointerArray<CCmSqlPropertyItem>* iPropertyItems;          // not owned
    
    /**
     * Filtered property item array pointer
     */
    CCmSqlPropertyCollector* iPropertyCollector;                // not owned
    
    /**
     * Mediaserver ids
     */
    RArray<TInt>* iMsIds;                                       // not owned
       
    /**
     * Asyncronous database handler
     */
    CIdle* iAsyncDbHandler;                                     // owned
    
    /**
     * Operation mode
     */
    TCmSqlOperationMode iMode;
    
    /**
     * Index to item array
     */
    TInt iIndex;
    
    /**
     * Property table identifier
     */
    TCmMetadataField iField;
    
    /**
     * Fill list
     */
    CCmFillRule* iList;                                         // not owned
    
    /**
     * Fill list size
     */
    TInt64 iListSize;
    
    /**
     * Maximun size of the fill list in Bytes
     */
    TInt64 iListMaxSize;
    
    /**
     * Used to get max count of items
     */
    HBufC8* iCountQuery;                                        // Owned
    
    };

#endif //  C_CMSQLCONNECTION_H
