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







#ifndef C_CMDMSQLITECONNECTION_H
#define C_CMDMSQLITECONNECTION_H

#include <e32base.h>

#include "sqldb.h" // Symbian SQLite
#include "cmdmmain.h" // TCMQueryCmdType

/* Forward declarations. */
class CCmFillRule;
class CCmMediaServerFull;
class CCmStoreRule;
class CCmBaseListItem;
class CCmFillListItem;
class CCmStoreListItem;
class CCmDriveInfo;


// CLASS DECLARATION
 /**
 *  Class represents methods to communication with SQLite database API.
 *  @lib cmdatabasemanager.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmDmSQLiteConnection ): public CBase
    {

public:

    // ID types
    enum TTypeOfID
        {
        ERuleGroupID = 1,
        ERuleID,
        ERuleParamID,
        EMediaServerID,
        EStoredID,
        EFilledID,
        EStoreRuleID,
        EStoreRuleParamID,
        EFillFileListId,
        EStoreFileListId
        };
        
    /**
     * Two-phased constructor.
     */
    static CCmDmSQLiteConnection* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCmDmSQLiteConnection* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmDmSQLiteConnection();

    /**
     * Open database (creates if doesn't exist)
     *
     * @since S60 5.1
     * @param aDb, Database filename and path
     * @return TInt, Error code
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
     * Initializes Database ids
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void InitIDs();
    
    /**
     * Initializes Database id
     *
     * @since S60 5.1
     * @param aType, ID type
     * @return None
     */           
    void SetID( TTypeOfID aType );

    /**
     * Backups id to database
     *
     * @since S60 5.1
     * @param aType, ID type
     * @param aID, ID value
     * @return None
     */            
    void BackUpIdL( TTypeOfID aType, TUint64 aID );
        
    /**
     * Loads Database ids
     *
     * @since S60 5.1
     * @param None
     * @return None
     */            
    void LoadIDs();
    
    /**
     * Loads Database id
     *
     * @since S60 5.1
     * @param aId, ID value
     * @param aType, ID type
     * @return None
     */        
    void LoadIDL( TUint64& aId, TTypeOfID aType );

    /**
     * Creates db file
     * @since S60 5.1
     * @param aDb, Database filename and path
     * @return TInt, error code
     */  
    TInt CreateDbFile( const TDesC& aDb ); 
        
    /**
     * Check Validity of database
     *
     * @since S60 5.1
     * @param None
     * @return ETrue if database is OK
     */            
    TBool Validate();
    
    /**
     * Execute sql command (not a query)
     *
     * @since S60 5.1
     * @param aCommand, SQL clause to be executed (can include wildcards)
     * @return TInt, KErrNone if successful, otherwise one of the error codes.
     */
    void ExecuteL( const TDesC8& aCommand );
 
    /**
     * Sets fill file list into db
     *
     * @since S60 5.1
     * @param aItems, fill list item array
     * @return TInt, Error code
     */            
    TInt SetFillFiles( RPointerArray<CCmFillListItem>& aItems );
    
    /**
     * Sets store file list into db
     *
     * @since S60 5.1
     * @param aItems, store list item array
     * @return TInt, Error code
     */    
    TInt SetStoreFilesL( RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Set one store file into db
     *
     * @since S60 5.1
     * @param aItem, store list item
     * @return TInt, Error code
     */      
    TInt AddStoreFile( CCmStoreListItem& aItem );
    
    /**
     * Add stored media into database
     *
     * @since S60 5.1
     * @param aFileName, store file name
     * @param aUDN, mediaserver udn
     * @param aTimeStamp, timestamp
     * @return TInt, Error code
     */                
    TInt AddStoredL( 
        const TDesC& aFileName, 
        const TDesC8& aUDN, 
        TTime aTimeStamp );

    /**
     * Add fill rule into database
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code
     */              
    TInt AddFillRuleL( CCmFillRule* aFillRule );
    
    /**
     * Add fill rule specific media servers
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code
     */            
    TInt AddFillRuleMediaServersL( CCmFillRule* aFillRule );

    /**
     * Add fill rule informations and parameters
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code
     */            
    TInt AddFillRuleRulesAndParamsL( CCmFillRule* aFillRule );
    
    /**
     * Add store rule
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code
     */        
    TInt AddStoreRuleL( CCmStoreRule* aStoreRule );

    /**
     * Add store rule informations and parameters
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code
     */        
    TInt AddStoreRuleParamsL( CCmStoreRule* aStoreRule );

    /**
     * Add store rule mediaServers
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code
     */        
    TInt AddStoreRuleMediaServersL( CCmStoreRule* aStoreRule );

    /**
     * Add media server info into database
     *
     * @since S60 5.1
     * @param aMediaServer, media server
     * @return TInt, Error code
     */              
    TInt AddMediaServerL( CCmMediaServerFull* aMediaServer );

    /**
     * Add hash value
     *
     * @since S60 5.1
     * @param aMediaServer, media server id
     * @param aHashValue, hash value
     * @return TInt, Error code
     */
    TInt AddHashValue( TInt64 aMediaServerId, 
                                       CCmSearchResponseHash* aHashValue );

    /**
     * Query hash value
     *
     * @since S60 5.1
     * @param aMediaServer, media server id
     * @param aHashValues, hash values
     * @return TInt, Error code
     */    
    TInt QueryHashValuesL( TInt aMediaServer, 
        RPointerArray<CCmSearchResponseHash>& aHashValues );
    
    /**
     * Sets is active status of the media server
     * Sets the Media server's name
     * @since S60 5.1
     * @param aServers, server list
     * @return TInt, Error code 
     */        
    TInt SetMediaServersL( 
        RPointerArray<CCmMediaServerFull>& aServers );    

    /**
     * Sets last syncs time
     * @since S60 5.1
     * @param None
     * @return TInt, Error code 
     */    
     TInt SetSyncTime();

    /**
     * Gets last syncs time
     * @since S60 5.1
     * @param aTime last sync time
     * @return TInt, Error code 
     */        
    TInt GetSyncTime( TTime& aTime );

    /**
     * Sets Progress info
     * @since S60 5.1
     * @param aTransferred how many files transferred
     * @param aTotalCount how many will be trasferred
     * @return TInt, Error code 
     */    
    TInt SetProgressInfo( TInt aTransferred, TInt aTotalCount );

    /**
     * Gets Progress info
     * @since S60 5.1
     * @param aTransferred how many files transferred
     * @param aTotalCount how many will be trasferred
     * @return TInt, Error code 
     */    
    TInt GetProgressInfo( TInt& aTransferred, TInt& aTotalCount );

    /**
     * Sets Drives info
     * @since S60 5.1
     * @param aDrives, drives to be set into database
     * @return TInt, None
     */ 
    void SetDrivesL( RPointerArray<CCmDriveInfo>& aDrives );
            
    /**
     * Delete media server info from the database
     *
     * @since S60 5.1
     * @param aUDN, server udn
     * @return TInt, Error code
     */        
    TInt DeleteMediaServer( const TDesC8& aUDN );

    /**
     * Delete fill rule
     *
     * @since S60 5.1
     * @param aName, fill list name
     * @return TInt, Error code
     */            
    TInt DeleteFillRuleByName( const TDesC8& aName );

    /**
     * Delete all fill rules
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code
     */                
    TInt DeleteAllFillRules();

    /**
     * Delete all store rules
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code
     */    
    TInt DeleteAllStoreRules(); 
    
    /**
     * Delete all fill files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code
     */             
    TInt DeleteFillFiles();

    /**
     * Delete all store files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code
     */         
    TInt DeleteStoreFiles();

    /**
     * Delete deleted items 
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void DeleteDeletedItemsL();
    
    /**
     * Delete deleted media servers
     *
     * @since S60 5.1
     * @param None
     * @return None
     */      
    void DeleteDeletedMediaServersL();

    /**
    * Deletes hash values
    *
    * @since S60 5.1
    * @param aMediaServerId, media server id
    * @param aSearchIndex, search index
    * @return None
    */   
    void DeleteHashValuesL( const TInt aMediaserverId,
            const TInt aSearchIndex );
                    
    /**
     * Update media server info
     *
     * @since S 60 5.1
     * @param aMediaServer, mediaserver
     * @return TInt, Error code
     */            
    TInt UpdateMediaServerInfo( CCmMediaServerFull* aMediaServer );
    
    /**
     * Update Fill rules selected state
     *
     * @since S60 5.1
     * @param aName, fill list name
     * @param aSelected, selection status
     * @return TInt, Error code
     */        
    TInt UpdateFillRuleSelected( const TDesC8& aName, TInt aSelected );
    
    /**
    * update the fill rule realsize and realcount field
    * @since S60 5.1
    * @return TInt, error code
    */      
    TInt UpdateFillRuleGroupInfo();

    /**
     * Update download history 
     *
     * @since S60 5.1
     * @param aMediaServerId, mediaserver id
     * @param aDownloadData, download data size
     * @param aDownloadTime, download time
     * @return TInt, Error code
     */          
    TInt UpdateDownloadHistory( TUint aMediaServerId, TInt64& aDownloadData,
                                TInt64& aDownloadTime );

    /**
     * Update upload history 
     *
     * @since S60 5.1
     * @param aMediaServerId, mediaserver id
     * @param aUploadData, upload data size
     * @param aUploadTime, upload time
     * @return TInt, Error code
     */                                     
    TInt UpdateUploadHistory( TUint aMediaServerId, TInt64& aUploadData, 
                            TInt64& aUploadTime );

    /**
     * Update status values
     *
     * @since S60 5.1
     * @param aStatusValues, item statuses
     * @return TInt, Error code 
     */
    TInt UpdateStatusValues( 
        RPointerArray<CCmBaseListItem>& aStatusValues );

    /**
     * Update fill list drive statuses
     *
     * @since S60 5.1
     * @param aDrive, drive info
     * @para aDriveSelected, if ETrue drive selected but slot changed
     * @return None 
     */     
    void UpdateFillListDriveStatusesL( CCmDriveInfo* aOrigDrive, 
        CCmDriveInfo* aNewDrive, TBool aDriveSelected ); 
                                            
    /**
     * Update fill list item priorities and status
     *
     * @since S60 5.1
     * @param aContainer, fill list container 
     * @return TInt, Error code 
     */
    TInt UpdateFillListItemPriority( CCmFillRuleContainer* aContainer );

    /**
     * Update fill selection state into fill file table
     *
     * @since S60 5.1
     * @param aListName, fill list name
     * @param aState, list status
     * @return None
     */
    void SetFillListStateL( const TUint aListName, 
        TCmFillRuleStatus aState );

    /**
     * Restoring file statuses
     *
     * @since S60 5.1
     * @param aFileNames, array of filled items
     * @param aListName, list name
     * @return None
     */
    void RestoreFileStatusesL( 
        const RPointerArray<CCmFillListItem>& aItems, 
        const TDesC8& aListName );

    /**
     * DoRestoring file statuses
     *
     * @since S60 5.1
     * @param aItems, array of filled items
     * @param aStatus, Status to be restored
     * @return None
     */
    void DoRestoreFileStatusesL( 
        const RPointerArray<CCmFillListItem>& aItems,
        const TUint aStatus );
                    
    /**
     * Initializes all reference ids to zero
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void SetRefIdsToZero();
              
    /**
     * Increment image shrinking history info
     * @since S60 5.1
     * @param aImageCount, image count
     * @param aMilliSeconds, shrinking time
     * @return TInt, Error code 
     */            
    TInt IncrementShrinkTimeL( TInt aImageCount, 
        TInt aMilliSeconds );
    
    /**
     * Prepare query
     *
     * @since S60 5.1
     * @param Type, query type
     * @return None
     */         
    void PrepareQueryCmdL( TCMQueryCmdType aType );
    
    /**
     * Query fill rule
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code
     */         
    TInt QueryFillRuleL( CCmFillRule* aFillRule );

    /**
     * Query fill rule servers
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @param aRuleGroupID, rule group id
     * @return TInt, Error code
     */    
    TInt QueryFillRuleServersL( CCmFillRule* aFillRule, 
                                TUint64 aRuleGroupID );
    
    /**
     * Query fill rule params
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return TInt, Error code
     */    
    TInt QueryFillRuleParamsL( CCmFillRule* aFillRule );

    /**
     * Query selected ( 1 = selected and  0 = unselected ) 
     * fill rule names
     *
     * @since S60 5.1
     * @param aNames, list names
     * @param aSelected, selection status
     * @return None
     */             
    void QuerySelectedFillRuleNamesL( RPointerArray<HBufC8>& aNames, 
        TInt aSelected );

    /**
     * Query names of all fill rules
     *
     * @since S60 5.1
     * @param aNames, on return fill rule names
     * @return None
     */         
    void QueryFillRuleNamesL( RPointerArray<HBufC8>& aNames );

    /**
     * Query store rule
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @return TInt, Error code
     */        
    TInt QueryStoreRuleL( CCmStoreRule* aStoreRule );
    
    /**
     * Query store rule defs
     *
     * @since S60 5.1
     * @param aStoreRule, store list
     * @param aRuleGroupID, rule group id
     * @return TInt, Error code
     */        
    TInt QueryStoreRuleDefsL( CCmStoreRule* aStoreRule, 
                              TUint64 aRuleGroupID );

    /**
     * Query store rule names
     *
     * @since S60 5.1
     * @param aNames, store list names
     * @return None
     */        
    void QueryStoreRuleNamesL( RPointerArray<HBufC8>& aNames );

    /**
     * Query selected store rule namess
     *
     * @since S60 5.1
     * @param aNames, store list names
     * @param aSelected, selection status
     * @return TInt, Error code
     */        
    void QuerySelectedStoreRuleNamesL( RPointerArray<HBufC8>& aNames,
        TInt aSelected );  
        
    /**
     * Query media server
     *
     * @since S60 5.1
     * @param aMediaServer, mediaserver
     * @return TInt, Error code
     */             
    TInt QueryMediaServerL( CCmMediaServerFull* aMediaServer );

    /**
     * Query media server Id
     *
     * @since S60 5.1
     * @param aUDN, server udn
     * @return TInt64, Server id
     */         
    TInt64 QueryMediaServerId( const TDesC8& aUDN );

    /**
     * Query media server udn
     *
     * @since S60 5.1
     * @param aUDN, server udn
     * @param aId, server id
     * @return TInt, Error code
     */        
    TInt QueryMediaServerUdn( HBufC8*& aUdn, 
        const TInt32 aId );    

    /**
     * Query all media servers
     *
     * @since S60 5.1
     * @param aMediaServer, mediaserver
     * @return TInt, Error code
     */    
    TInt QueryAllMediaServersL( CCmMediaServerFull* aMediaServer );
    
    /**
     * Get media servers
     *
     * @since S60 5.1
     * @param aMediaServers, mediaserver list
     * @return None 
     */    
    void GetMediaServersL( 
        RPointerArray<CCmMediaServerFull>& aMediaServers );    
         
    /**
     * Query all lowest priority filled files
     *
     * @since S60 5.1
     * @param aDrive, drive id
     * @param aFilled, list of filled items
     * @return TInt, Error code 
     */             
    TInt GetLowPriorityFilledFilesL( TInt aDrive,
        RPointerArray<CCmFillListItem>& aFilled );

    /**
     * Query all files defined in fill file list using fill list name as
     * a query parameter
     *
     * @since S60 5.1
     * @param aItems, fill list items
     * @param aName, fill list name
     * @param aStatus, selection status
     * @return TInt, Error code 
     */        
    TInt QueryFillFileListL( 
        RPointerArray<CCmBaseListItem>& aItems, 
        const TDesC8& aName, 
        TUint aStatus );
        
     /**
     * Query fill file list
     * @since S60 5.1
     * @param aItems, fill list items
     * @param aStatus, list status
     * @return TInt, Error code 
     */    
    TInt GetAllFillFilesL( 
        RPointerArray<CCmFillListItem>& aItems, TUint aStatus );    
        
    /**
     * Query all files defined in store file list using store list name as
     * a query parameter
     *
     * @since S60 5.1
     * @param aItems, store list items
     * @param aName, store list name
     * @param aStatus, store list status
     * @return TInt, Error code 
     */            
    TInt QueryStoreFileListL( 
        RPointerArray<CCmBaseListItem>& aItems, 
        const TDesC8& aName, 
        TUint aStatus );

     /**
     * Query store file list
     * @since S60 5.1
     * @param aItems, store list items
     * @param aStatus, list status
     * @return TInt, Error code 
     */    
    TInt GetAllStoreFilesL( 
        RPointerArray<CCmStoreListItem>& aItems, TUint aStatus );
                
    /**
     * Query all files defined in fill file list
     *
     * @since S60 5.1
     * @param aItems, fill list items
     * @return TInt, Error code 
     */        
    TInt QueryAllFillFilesL( RPointerArray<CCmFillListItem>& aItems);

    /**
     * Query all files defined in store file list
     *
     * @since S60 5.1
     * @param aItems, store list items
     * @return TInt, Error code 
     */        
    TInt QueryAllStoreFilesL( RPointerArray<CCmStoreListItem>& aItems );
         
    /**
     * Query stored row
     *
     * @since S60 5.1
     * @param aFiles, stored items
     * @param aUDN, server udn
     * @param aAmount, item count
     * @return None
     */         
    void QueryStoredRowL( 
        RPointerArray<HBufC16>& aFiles, 
        const TDesC8& aUDN, 
        TInt aAmount );

    /**
     * Query transfer history
     *
     * @since S60 5.1
     * @param aUdn, server udn
     * @param aDownloadData, downloaded data
     * @param aUploadData, uploaded data
     * @param aDownloadTime, download time
     * @param aUploadTime, upload time
     * @return None
     */        
    void QueryTransferHistory( 
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
     * @param aArray, deleted items
     * @param aStatus, item status
     * @return None 
     */  
    void QueryDeletedItemsListsL( 
        RPointerArray<CCmBaseListItem>& aArray, TUint aStatus );
        
    /**
     * Query drives
     *
     * @since S60 5.1
     * @param aDrives, drive array
     * @return None 
     */     
    void QueryDrivesL( RPointerArray<CCmDriveInfo>& aDrives ); 
    
    /**
     * Avarage image shrinking time 
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Avarage image shrinking time in millisecond 
     */        
    TInt GetAvgImageShrinkTime();

    /**
     * Get list name using list id 
     *
     * @since S60 5.1
     * @param aListName, name of the fill list
     * @param aListId, id of the named list
     * @return None
     */     
    void GetFillListNameL( HBufC8*& aListName, 
        const TInt64 aListId );
            
    /**
     * Returns size of the named fill list
     *
     * @since S60 5.1
     * @param aListName, fill list name
     * @param aStatus, list status
     * @return TInt64, size of the list
     */    
    TInt64 FillListSize( const TDesC8& aListName, TUint aStatus );
    
    /**
     * Returns size of the named store list
     *
     * @since S60 5.1
     * @param aListName, store list name
     * @param aStatus, lis status
     * @return TInt64, size of the list
     */    
    TInt64 StoreListSize( const TDesC8& aListName, TUint aStatus );

    /**
     * Returns file count which are waiting to be stored
     *
     * @since S60 5.1
     * @param aStatus, item status
     * @return TInt, size of the list
     */        
    TInt StoreFileCount( TUint aStatus );

    /**
     * Returns file count which are waiting to be stored
     *
     * @since S60 5.1
     * @param None
     * @return TInt, size of the list
     */      
    TInt StoreFileCountNoDuplicates();

    /**
     * Returns file count which are waiting to be filled
     *
     * @since S60 5.1
     * @param aStatus, item status
     * @return TInt, size of the list
     */    
    TInt FillFileCount( TUint aStatus );
    
    /**
     * Returns data amount filled
     * @since S60 v3.1
     * @param aID id of fill rule
     * @return sum of file sizes
     */        
    TInt64 BytesFilled( TUint aID );

    /**
     * @since S60 5.1
     * @param aMediaServerId, mediaserver id
     * @param aStatus, item status
     * @return TInt64, sum of file sizes
     */        
    TInt64 KBytesToBeFilled( TUint8 aMediaServerId, 
        TUint aStatus );

    /**
     * Returns data amount to be stored
     *
     * @since S60 5.1
     * @param aMediaServerId, mediaserver id
     * @param aStatus, item status
     * @return TInt64, sum of file sizes
     */        
    TInt64 KBytesToBeStored( TUint8 aMediaServerId, 
        TUint aStatus );    
    
    /**
     * Updates list size and approx. transfer time
     *
     * @since S60 5.1
     * @param aService, service
     * @param aCount, pieces
     * @param aTime, in milliseconds
     * @return TInt, error code
     */           
    TInt UpdateTransferInfo( TCmService aService, 
        TInt aCount, TInt aTime );     

    /**
     * Updates file status
     *
     * @since S60 5.1
     * @param aStatus, transfer status
     * @param aFid, file id
     * @param aMsId, server id
     * @return None
     */         
    void UpdateStoreTransferStatus( TUint aStatus,
        TInt64 aFid, TInt64 aMsId ); 

    /**
     * Updates file status
     *
     * @since S60 5.1
     * @param aStatus, transfer status
     * @param aItem, fill list item
     * @return None
     */
    void UpdateFillTransferStatus( TUint aStatus, 
        CCmFillListItem* aItem );
        
    /**
     * Get list size and approx. transfer time
     *
     * @since S60 5.1
     * @param aService, service
     * @param aCount, pieces
     * @param aTime, in milliseconds
     * @return TInt, error code
     */            
    TInt GetTransferInfo( TCmService aService, TInt& aCount, TInt& aTime );

    /**
     * Files to be stored and included into specified list
     *
     * @since S60 5.1
     * @param aListName, store list name
     * @param aStatus, list status
     * @param aCount, pieces
     * @return TInt, error code
     */            
    TInt GetStoreFileCount( 
        const TDesC8& aListName, 
        TUint aStatus, 
        TInt& aCount );
             
    /**
     * Files to be filled and included into specified list
     *
     * @since S60 5.1
     * @param aListName, fill list name
     * @param aStatus, list status
     * @param aCount, pieces
     * @return TInt, error code
     */            
    TInt GetFillFileCount( 
        const TDesC8& aListName, 
        TUint aStatus, 
        TInt& aCount );

    /**
     * Files to be stored
     * @since S60 5.1
     * @param aCount, file count
     * @param aStatus, status
     * @return None
     */
    void GetStoreFileCountNoDuplicatesL( TInt& aCount, TUint aStatus );
    
    /**
     * Sets fill list real information
     *
     * @since S60 5.1
     * @param aListId, fill list id
     * @param aRealCount, real item count
     * @param aRealSize, real data size
     * @return None
     */          
    void SetFillListRealCountAndSizeL( TInt64 aListId, 
        TInt64 aRealCount, TInt64 aRealSize );            
    
    /**
     * Deletes in active rule servers
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    TInt DeleteInActiveRuleServers();
    
    /**
     * Deletes in active rule servers
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */    
    TInt DeleteInActiveFiles();
    
    /**
     * Deletes item by FillListId
     *
     * @since S60 5.1
     * @param FillListId, fill list id
     * @return TInt error code
     */ 
    TInt DeleteItemsByFillListId( TInt aFillListId );
                
    /**
     * Returns rule id with defined type
     *
     * @since S60 5.1
     * @param aName, rule name
     * @param aType, rule type
     * @return TInt64, Rule id
     */    
    TInt64 RuleId( const TDesC8& aName, TCmRuleType aType );
    
               
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmDmSQLiteConnection();

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
     * rule group id
     */
    TUint64 iRuleGroupID;
    /**
     * rule id
     */
    TUint64 iRuleID;
    /**
     * rule param id
     */
    TUint64 iRuleParamID;
    /**
     * mediaserver id
     */    
    TUint64 iMediaServerID;
    /**
     * stored id
     */    
    TUint64 iStoredID;
    /**
     * filled id
     */
    TUint64 iFilledID;
    /**
     * store rule id
     */
    TUint64 iStoreRuleID;
    /**
     * store rule param id
     */
    TUint64 iStoreRuleParamID;
    /**
     * fill file id
     */    
    TUint64 iFillFileId;
    /**
     * store file id
     */
    TUint64 iStoreFileId;    
    /**
     * Query handle
     */
    TInt iHandle;
    
    };

#endif //  C_CMDMSQLITECONNECTION_H
