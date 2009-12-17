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
* Description:      Main class in the Database manager component
*
*/






#ifndef C_CMDMMAIN_H
#define C_CMDMMAIN_H

#include <e32base.h>
#include <badesca.h>
#include "cmcommon.h"
#include "cmcommontypes.h"

// Operation types
enum TCMQueryCmdType
    {
    EFillRuleQuery = 1,
    EMediaServerInfoQyery,
    EMediaServerIdQuery,
    EMediaServerUDNQuery,
    EMediaServersAllQuery,
    EHashValueQuery,
    EFilledQuery,
    EStoredQuery,
    EFilledRowQuery,
    EStoredRowQuery,
    ESelectedFillRuleQuery,
    EFillRuleNamesQuery,
    EStoreRuleQuery,
    EStoreRuleNamesQuery,
    ESelectedStoreRuleQuery,
    ETransferHistoryQuery,
    EFillFilesQuery,
    EStoreFilesQuery,
    EAllFillFilesQuery,
    EAllStoreFilesQuery,
    EFilledLowPriorityQuery,
    EAllFillFilesStatusQuery,
    EAllStoreFilesStatusQuery,
    EDeletedFillFilesQuery,
    EDrivesQuery
    };    
            
/* Forward declarations. */
class CCmDmSqlDbMaintenance;
class CCmDmSQLiteConnection;
class CCmFillRule;
class CCmFillRuleContainer;
class CCmMediaServerFull;
class CCmStoreRule;
class CCmBaseListItem;
class CCmFillListItem;
class CCmStoreListItem;
class CCmSearchResponseHash;
class CCmDriveInfo;


/**
 *  CCmDmMain class
 *  Part of Database manager. Database manager is part of
 *  Content manager component.
 *
 *  @lib cmdatabasemanager.lib
 *
 *  @since S60 5.1
 */
class CCmDmMain : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCmDmMain* NewL();
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCmDmMain* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmDmMain();

    /**
     * Prepares query command
     *
     * @since S60 5.1
     * @param aType type of query
     * @return None
     */        
    IMPORT_C void PrepareQueryCmdL( TCMQueryCmdType aType );
        
    /**
     * Query fill rule
     *
     * @since S60 5.1
     * @param aFillRule
     * @return TInt, Error code 
     */    
    IMPORT_C TInt QueryFillRuleL( CCmFillRule* aFillRule );

    /**
     * Query names of selected fill rules
     *
     * @since S60 5.1
     * @param aNames
     * @param aSelected
     * @return None 
     */        
    IMPORT_C void QuerySelectedFillRuleNamesL( 
        RPointerArray<HBufC8>& aNames, 
        TInt aSelected );

    /**
     * Query names of fill rules
     *
     * @since S60 5.1
     * @param aNames, on return fill rule names
     * @return None 
     */    
    IMPORT_C void QueryFillRuleNamesL( RPointerArray<HBufC8>& aNames );
    
    /**
     * Query store rule
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryStoreRuleL( CCmStoreRule* aStoreRule );
    
    /**
     * Query store rule names
     *
     * @since S60 5.1
     * @param aNames, list names
     * @return None
     */    
    IMPORT_C void QueryStoreRuleNamesL( RPointerArray<HBufC8>& aNames );

    /**
     * Query selected store rule names
     *
     * @since S60 5.1
     * @param aNames, list names
     * @param aSelected, selection status
     * @return None
     */        
    IMPORT_C void QuerySelectedStoreRuleNamesL( 
        RPointerArray<HBufC8>& aNames,
        TInt aSelected );

    /**
     * Query media server info
     *
     * @since S60 5.1
     * @param aMediaServer, mediaserver
     * @return TInt, Error code 
     */            
    IMPORT_C TInt QueryMediaServerL( CCmMediaServerFull* aMediaServer );
    
    /**
     * Query media server ids
     *
     * @since S60 5.1
     * @param aUDN, server UDN
     * @return TInt64, Media server id 
     */        
    IMPORT_C TInt64 QueryMediaServerId( const TDesC8& aUDN );

    /**
     * Query media server udn
     *
     * @since S60 5.1
     * @param aUDN, server UDN
     * @param aId, server id
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryMediaServerUdn( HBufC8*& aUdn, const TInt32 aId );

    /**
     * Query All Media servers
     *
     * @since S60 5.1
     * @param aMediaServer, mediaserver
     * @return TInt, Error code 
     */    
    IMPORT_C TInt QueryAllMediaServersL( CCmMediaServerFull* aMediaServer );

    /**
     * Get media servers
     *
     * @since S60 5.1
     * @param aMediaServers, mediaserver list
     * @return None 
     */    
    IMPORT_C void GetMediaServersL( 
        RPointerArray<CCmMediaServerFull>& aMediaServers );
        
    /**
     * Query filled
     *
     * @since S60 5.1
     * @param aDrive, drive id
     * @param aFilled, fill list items
     * @return TInt, Error code 
     */        
    IMPORT_C TInt GetLowPriorityFilledFilesL( 
        TInt aDrive, 
        RPointerArray<CCmFillListItem>& aFilled );

    /**
     * Query fill file list
     *
     * @since S60 5.1
     * @param aItems, fill list items
     * @param aName, fill list name
     * @param aStatus, list status
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryFillFileListL( 
        RPointerArray<CCmBaseListItem>& aItems, 
        const TDesC8& aName, 
        TUint aStatus );

    /**
     * Query fill file list
     *
     * @since S60 5.1
     * @param aItems, fill list items
     * @param aStatus, list status
     * @return TInt, Error code 
     */    
    IMPORT_C TInt GetAllFillFilesL( 
        RPointerArray<CCmFillListItem>& aItems, 
        TUint aStatus );
        
    /**
     * Query store file list
     *
     * @since S60 5.1
     * @param aItems, list items
     * @param aName, list name
     * @param aStatus, list status
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryStoreFileListL( 
        RPointerArray<CCmBaseListItem>& aItems, 
        const TDesC8& aName, 
        TUint aStatus );

    /**
     * Query store file list
     *
     * @since S60 5.1
     * @param aItems, list items
     * @param aStatus, list status
     * @return TInt, Error code 
     */    
    IMPORT_C TInt GetAllStoreFilesL( 
        RPointerArray<CCmStoreListItem>& aItems, 
        TUint aStatus );
        
    /**
     * Query all fill files
     * @since S60 5.1
     * @param aItems
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryAllFillFilesL( 
        RPointerArray<CCmFillListItem>& aItems );

    /**
     * Query all store files
     *
     * @since S60 5.1
     * @param aItems, list items
     * @return TInt, Error code 
     */        
    IMPORT_C TInt QueryAllStoreFilesL( 
        RPointerArray<CCmStoreListItem>& aItems );
    
    /**
     * Query all stored ( use aFile as a query parameter )
     *
     * @since S60 5.1
     * @param aFiles, stored files
     * @param aUDN, server udn
     * @param aAmount, amount of files
     * @return None
     */        
    IMPORT_C void QueryStoredRowL( 
        RPointerArray<HBufC16>& aFiles, 
        const TDesC8& aUDN, 
        TInt aAmount );    

    /**
     * Query transfer history
     *
     * @since S60 5.1
     * @param aUdn, server udn
     * @param aDownloadData, download data size
     * @param aUploadData, upload data size
     * @param aDownloadTime, download time
     * @param aUploadTime, upload time 
     * @return None 
     */        
    IMPORT_C void QueryTransferHistory( 
        const TDesC8& aUdn, 
        TInt64& aDownloadData,
        TInt64& aUploadData, 
        TInt64& aDownloadTime, 
        TInt64& aUploadTime );

    /**
     * Query deleted items list
     *
     * @since S60 5.1
     * @param aUdn, server udn
     * @param aArray, array of deleted items
     * @param aStatus, status info
     * @return None 
     */  
    IMPORT_C void QueryDeletedItemsListsL( 
        RPointerArray<CCmBaseListItem>& aArray, 
        TUint aStatus );
        
    /**
     * Query drives
     *
     * @since S60 5.1
     * @param aDrives, drive array
     * @return None 
     */     
    IMPORT_C void QueryDrivesL( RPointerArray<CCmDriveInfo>& aDrives );   
    
    /**
     * Avarage image shrinking time 
     *
     * @since S60 5.1
     * @return TInt, Avarage image shrinking time in millisecond 
     */        
    IMPORT_C TInt GetAvgImageShrinkTime();

    /**
     * Get list name using list id 
     *
     * @since S60 5.1
     * @param aListName, name of the fill list
     * @param aListId, id of the named list
     * @return None
     */     
    IMPORT_C void GetFillListNameL( 
        HBufC8*& aListName, 
        const TInt64 aListId );
    
    /**
     * Set fill file list
     *
     * @since S60 5.1
     * @param aItems, fill list items
     * @return TInt Error code 
     */    
    IMPORT_C TInt SetFillFiles( RPointerArray<CCmFillListItem>& aItems );

    /**
     * Set store file list
     *
     * @since S60 5.1
     * @param aItems, store list items
     * @return TInt, Error code 
     */        
    IMPORT_C TInt SetStoreFilesL( RPointerArray<CCmStoreListItem>& aItems );
    
    /**
     * Adds one item into store file list
     *
     * @since S60 5.1
     * @param aItem, store list item
     * @return TInt, Error code 
     */        
    IMPORT_C TInt AddStoreFile( CCmStoreListItem& aItem );        
    
    /**
     * Add stored item
     *
     * @since S60 5.1
     * @param aFileName, file name
     * @param aUDN, server udn
     * @param aTimeStamp, timestamp
     * @return TInt, Error code 
     */     
    IMPORT_C TInt AddStoredL( 
        const TDesC& aFileName, 
        const TDesC8& aUDN, 
        TTime aTimeStamp );   
    
    /**
     * Add fill rule
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code 
     */        
    IMPORT_C TInt AddFillRuleL( CCmFillRule* aFillRule );
    
    /**
     * Add store rule
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code 
     */        
    IMPORT_C TInt AddStoreRuleL( CCmStoreRule* aStoreRule );    
    
    /**
     * Add Media server
     *
     * @since S60 5.1
     * @param aMediaServer, media server
     * @return TInt, Error code 
     */
    IMPORT_C TInt AddMediaServerL( CCmMediaServerFull* aMediaServer );
    
    /**
     * Add hash value
     *
     * @since S60 5.1
     * @param aMediaServerId, media server id
     * @param aHashValue, hash value
     * @return TInt, Error code 
     */
    IMPORT_C TInt AddHashValue( TInt64 aMediaServerId, 
                                CCmSearchResponseHash* aHashValue );

    /**
     * Query hash values
     *
     * @since S60 5.1
     * @param aMediaServerId, media server id
     * @param aHashValues, hash value
     * @return TInt, Error code 
     */                                       
    IMPORT_C TInt QueryHashValuesL( TInt aMediaServer, 
        RPointerArray<CCmSearchResponseHash>& aHashValues );                                       

    /**
     * Sets is active status of the media server
     *
     * @since S60 5.1
     * @param aServers, mediaservers
     * @return TInt, Error code 
     */        
    IMPORT_C TInt SetMediaServersL( 
        RPointerArray<CCmMediaServerFull>& aServers );    

    /**
     * Sets last syncs time
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code 
     */    
    IMPORT_C TInt SetSyncTime();

    /**
     * Gets last syncs time
     *
     * @since S60 5.1
     * @param aTime last sync time
     * @return TInt, Error code 
     */        
    IMPORT_C TInt GetSyncTime( TTime& aTime );
    
    /**
     * Sets Progress info
     * @since S60 5.1
     *
     * @param aTransferred how many files transferred
     * @param aTotalCount how many will be trasferred
     * @return TInt, Error code 
     */    
    IMPORT_C TInt SetProgressInfo( TInt aTransferred, TInt aTotalCount );

    /**
     * Gets Progress info
     *
     * @since S60 5.1
     * @param aTransferred how many files transferred
     * @param aTotalCount how many will be trasferred
     * @return TInt, Error code 
     */    
    IMPORT_C TInt GetProgressInfo( 
        TInt& aTransferred, 
        TInt& aTotalCount );    

    /**
     * Sets Drives info
     *
     * @since S60 5.1
     * @param aDrives, drives to be set into database
     * @return None
     */ 
    IMPORT_C void SetDrivesL( RPointerArray<CCmDriveInfo>& aDrives );
        
    /**
     * Remove media server info from database
     *
     * @since S60 5.1
     * @param aUDN, server udn
     * @return TInt, Error code 
     */         
    IMPORT_C TInt DeleteMediaServer( const TDesC8& aUDN );

    /**
     * Remove fill rule from database
     *
     * @since S60 5.1
     * @param aName, fill list name
     * @return TInt, Error code 
     */         
    IMPORT_C TInt DeleteFillRuleByName( const TDesC8& aName );
    
    /**
     * Delete all fill rules
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code 
     */         
    IMPORT_C TInt DeleteAllFillRules();

    /**
     * Delete all store rules
     *
     * @since S60 5.1
     * @param None
     * @return Error code 
     */        
    IMPORT_C TInt DeleteAllStoreRules();

    /**
     * Deletes all fill files
     *
     * @since S60 5.1
     * @param None
     * @return Error code 
     */        
    IMPORT_C TInt DeleteFillFiles();

    /**
     * Deletes all store files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code 
     */        
    IMPORT_C TInt DeleteStoreFiles();

    /**
     * Delete deleted items 
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    IMPORT_C void DeleteDeletedItemsL();

    /**
     * Delete items by FillListId
     *
     * @since S60 5.1
     * @param FillListId, fill list id
     * @return TInt, Error code
     */
    IMPORT_C TInt DeleteItemsByFillListId( TInt aFillListId );

    /**
     * Delete deleted media servers
     *
     * @since S60 5.1
     * @param None
     * @return None
     */      
    IMPORT_C void DeleteDeletedMediaServersL();
    
    /**
    * Deletes hash values
    *
    * @since S60 5.1
    * @param aMediaServerId, media server id
    * @param aSearchIndex, search index
    * @return None
    */   
    IMPORT_C void DeleteHashValuesL( const TInt aMediaserverId,
            const TInt aSearchIndex );

    /**
     * Update media server info
     *
     * @since S60 5.1
     * @param aMediaServer, media server
     * @return TInt, Error code 
     */         
    IMPORT_C TInt UpdateMediaServerInfo( CCmMediaServerFull* aMediaServer );
    
    /**
     * Update status of the fill rule ( selected or not )
     *
     * @since S60 5.1
     * @param aName, list name
     * @param aSelected, selection status
     * @return TInt, Error code 
     */         
    IMPORT_C TInt UpdateFillRuleSelected( 
        const TDesC8& aName, 
        TInt aSelected );

    /**
     * Update Download history
     *
     * @since S60 5.1
     * @param aMediaServerId, server id
     * @param aDownloadData, download data size 
     * @param aDownloadTime, download time
     * @return TInt, Error code 
     */    
    IMPORT_C TInt UpdateDownloadHistory( 
        TUint aMediaServerId,
        TInt64& aDownloadData, 
        TInt64& aDownloadTime );
    
    /**
     * Update Upload history
     *
     * @since S60 5.1
     * @param aMediaServerId, server id
     * @param aUploadData, upload data size
     * @param aUploadTime, upload time
     * @return TInt, Error code
     */    
    IMPORT_C TInt UpdateUploadHistory( 
        TUint aMediaServerId,
        TInt64& aUploadData, 
        TInt64& aUploadTime );

    /**
     * Update status values
     *
     * @since S60 5.1
     * @param aStatusValues, status values of items
     * @return TInt, Error code 
     */
    IMPORT_C TInt UpdateStatusValues( 
        RPointerArray<CCmBaseListItem>& aStatusValues );
        
    /**
     * Update fill list drive statuses
     *
     * @since S60 5.1
     * @param aDrive, drive info
     * @para aDriveSelected, if ETrue drive selected but slot changed
     * @return None 
     */    
    IMPORT_C void UpdateFillListDriveStatusesL( 
        CCmDriveInfo* aOrigDrive, 
        CCmDriveInfo* aNewDrive, 
        TBool aDriveSelected );     

    /**
     * Update fill list item priorities
     *
     * @since S60 5.1
     * @param aContainer, fill list container 
     * @return TInt, Error code 
     */
    IMPORT_C TInt UpdateFillListItemPriority( 
        CCmFillRuleContainer* aContainer );                

    /**
     * Update fill selection state into fill file table
     *
     * @since S60 5.1
     * @param aListName, fill list name
     * @param aState, fill list status
     * @return None
     */
    IMPORT_C void SetFillListStateL( 
        const TUint aListName, 
        TCmFillRuleStatus aState );

    /**
     *
     * Restoring file statuses
     * @since S60 5.1
     * @param aItems, array of filled items
     * @param aListName, list name
     * @return None
     */
    IMPORT_C void RestoreFileStatusesL( 
        const RPointerArray<CCmFillListItem>& aItems, 
        const TDesC8& aListName );
                
    /**
     *
     * Increment image shrinking history info
     *
     * @since S60 5.1
     * @param aImageCount, image count
     * @param aMilliSeconds, shrinking time
     * @return TInt, Error code 
     */            
    IMPORT_C TInt IncrementShrinkTimeL( 
        TInt aImageCount, 
        TInt aMilliSeconds );

    /**
     * Returns size of the named list
     *
     * @since S60 5.1
     * @param aType, List type
     * @param aListName, list name 
     * @param aStatus, list status
     * @return TInt64, Size in bytes
     */        
    IMPORT_C TInt64 ListSize( 
        TCmRuleType aType, 
        const TDesC8& aListName,
        TUint aStatus );

    /**
     * Returns file count which are waiting to be stored
     *
     * @since S60 5.1
     * @param aStatus, file status
     * @return TInt, File count
     */                                
    IMPORT_C TInt StoreFileCount( TUint aStatus );

    /**
     * Returns file count which are waiting to be stored
     *
     * @since S60 5.1
     * @param None
     * @return size of the list
     */      
    IMPORT_C TInt StoreFileCountNoDuplicates();
    
    /**
     * Returns file count which are waiting to be filled
     *
     * @since S60 5.1
     * @param aStatus, file status
     * @return TInt, File count
     */        
    IMPORT_C TInt FillFileCount( TUint aStatus );

    /**
     * Returns data amount filled
     * @since S60 5.1
     * @param aID id of fill rule
     * @return sum of file sizes
     */        
    IMPORT_C TInt64 BytesFilled( 
        TUint aID );

    /**
     * Returns data amount to be filled
     * @since S60 5.1
     * @param aMediaServerId, media server id
     * @param aStatus, file status
     * @return TInt64, sum of file sizes
     */        
    IMPORT_C TInt64 KBytesToBeFilled( 
        TUint8 aMediaServerId, 
        TUint aStatus );

    /**
     * Returns data amount to be stored
     *
     * @since S60 5.1
     * @param aMediaServerId, media server id
     * @param aStatus, file status
     * @return TInt64, sum of file sizes
     */        
    IMPORT_C TInt64 KBytesToBeStored( 
        TUint8 aMediaServerId, 
        TUint aStatus );

    /**
     * Updates list size and approx. transfer time
     *
     * @since S60 5.1
     * @param aService, contentmanager service
     * @param aCount, item count
     * @param aTime, transfer time in milliseconds
     * @return TInt, error code
     */           
    IMPORT_C TInt UpdateTransferInfo( 
        TCmService aService, 
        TInt aCount, 
        TInt aTime );      

    /**
     * Updates file status
     *
     * @since S60 v3.1
     * @param aStatus, file status
     * @param aFid, file id
     * @param aMsId, server id
     * @return None
     */         
    IMPORT_C void UpdateStoreTransferStatus( 
        TUint aStatus,
        TInt64 aFid, 
        TInt64 aMsId );        
        
    /**
     * Updates file status
     *
     * @since S60 5.1
     * @param aStatus, file status
     * @param aItem, fill list item
     * @return None
     */
    IMPORT_C void UpdateFillTransferStatus( 
        TUint aStatus, 
        CCmFillListItem* aItem );
        
    /**
     * Get list size and approx. transfer time
     *
     * @since S60 5.1
     * @param aService, content manager service
     * @param aCount pieces, item count
     * @param aTime, transfer time in milliseconds
     * @return TInt, error code
     */            
    IMPORT_C TInt GetTransferInfo( 
        TCmService aService, 
        TInt& aCount, 
        TInt& aTime );

    /**
     * Files to be stored and included into specified list
     *
     * @since S60 5.1
     * @param aListName, store list name
     * @param aStatus, list status
     * @param aCount, item count
     * @return TInt, error code
     */            
    IMPORT_C TInt GetStoreFileCount( 
        const TDesC8& aListName, 
        TUint aStatus, 
        TInt& aCount );
             
     /**
     * Files to be filled and included into specified list
     *
     * @since S60 5.1
     * @param aListName, fill list name
     * @param aStatus, list status
     * @param aCount, item count
     * @return TInt, error code
     */            
    IMPORT_C TInt GetFillFileCount( 
        const TDesC8& aListName, 
        TUint aStatus, 
        TInt& aCount );

    /**
     * Files to be store
     *
     * @since S60 5.1
     * @param aCount, file count 
     * @param aStatus, file status
     * @return None
     */
    IMPORT_C void GetStoreFileCountNoDuplicatesL( 
        TInt& aCount, 
        TUint aStatus );        
        
     /**
     * Sets fill list real information
     *
     * @since S60 5.1
     * @param aListId, list id
     * @param aRealCount, item count
     * @param aRealSize, list size
     * @return None
     */          
    IMPORT_C void SetFillListRealCountAndSizeL( 
        TInt64 aListId, 
        TInt64 aRealCount, 
        TInt64 aRealSize );  

    /**
    * update the fill rule realsize and realcount field
    *
    * @since S60 5.1
    * @return TInt, error code
    */        
    IMPORT_C TInt UpdateFillRuleGroupInfo();
    
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmDmMain();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:

    /**
     * Pointer to SQLite connection class
     */
    CCmDmSQLiteConnection* iConnection;             // owned
    
    /**
     * Pointer to database maintenance class
     */
    CCmDmSqlDbMaintenance* iMaintenance;            // owned
    };

#endif //  C_CMDMMAIN_H
