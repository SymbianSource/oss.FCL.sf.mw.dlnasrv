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
* Description:  Fill rule and filled data managing
*  Version     : %version: tr1ido#8 % << Don't touch!
*
*/



#ifndef __CMFMFILLLIST_H
#define __CMFMFILLLIST_H

#include <e32base.h>
#include <f32file.h>
#include "cmcommontypes.h"
#include "cmcommon.h"
#include "cmfmcommon.h"

// Forward declarations
class MCmSqlMain;
class CCmDmMain;
class CCmFillRule;
class CCmFillListItem;
class CCmFillRuleContainer;
class CCmSqlPropertyItem;
class CCmSqlPropertyContainer;
class CCmFmAo;
class MCmFmFillRuleObserver;
class CCmSqlPropertyCollector;
             
 /**
  *  Class processes selected fill rules. Uses Database Manager and 
  *  Sqlite wrapper API to retrieve wanted metadata. Fill service is 
  *  started using processed metadata items. 
  *
  *  @lib cmfillmanager.lib
  *
  *  @since S60 5.1
  */
NONSHARABLE_CLASS( CCmFmFillRuleFilledDataMngr ): public CActive
    {

private: // enumerations

    // Metadata processing states
    enum TCmFmMetadaProcessingState
        {
        ECmFmIdle            = 0,
        ECmFmLoadingMetadata,
        ECmFmLoadingPropertys,
        ECmFmDeletingMetadata,
        ECmFmLoadingProfileIds
        };         
            
public:
       
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer for metadata processing
     * @param aDBMngr, database manager
     * @param aItems, items pointers array
     * @return  pointer to CCmFmFillRuleFilledDataMngr class
     */
    static CCmFmFillRuleFilledDataMngr* NewL(
        MCmFmFillRuleObserver* aObserver, CCmDmMain* aDBMngr,
        RPointerArray<CCmFillListItem>& aItems );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer for metadata processing
     * @param aDBMngr, database manager
     * @param aItems, items pointers array
     * @return  pointer to CCmFmFillRuleFilledDataMngr class
     */
    static CCmFmFillRuleFilledDataMngr* NewLC(
        MCmFmFillRuleObserver* aObserver, CCmDmMain* aDBMngr,
        RPointerArray<CCmFillListItem>& aItems );

    /**
     * Destructor.
     */
    virtual ~CCmFmFillRuleFilledDataMngr();

public: 

    /**
     * Notified when asyncronous operation is completed
     *
     * @since S60 5.1
     * @param aStatus, completion status
     * @return None
     */ 
    void OperationCompletedL( TInt aStatus );

   /**
     * Loads profile ids from database
     *
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void LoadProfileIdsL();
        
    /**
     * Starting fill rule pre-processing
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void PreprocessListsL();
    
    /**
     * Starting fill rule processing
     *
     * @since S60 5.1
     * @param aFillListName, name of the list
     * @return None
     */    
    void PreProcessListL( const TDesC8& aFillListName );
    
    /**
     * Provides metadata column item list
     *
     * @since S60 5.1
     * @param aType, metadata field identifier
     * @param aMedia, media type identifier
     * @param aPropertys, property container reference
     * @return  None
     */
    void GetColItemsL( TCmMetadataField aType, 
        TCmMediaType aMedia, CCmSqlPropertyContainer& aPropertys );   

    /**
     * Provides metadata items list
     *
     * @since S60 5.1
     * @param aPropertys, property collector reference
     * @return  None
     */    
    void GetMetadataItemsL( CCmSqlPropertyCollector& aPropertys );
    
    /**
     * Update fill list item priorities and reference values
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */
    void UpdatePriorities();

    /**
     * Do update fill list item reference ids
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */
    void DoUpdateReferenceIdsL();

    /**
     * Delete metadata from unselected servers
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */ 
    void DeleteMetadataL();
                
    /**
     * Sets memory card quota to wrapper
     *
     * @since S60 5.1
     * @param aQuota, fill quota
     * @return  None
     */    
    void SetMemoryCardQuota( TInt64 aQuota );
    
    /**
     * Canceling operations
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CancelOperation();
    
    /**
     * Updating transfer history
     *
     * @since S60 5.1
     * @param aInterval, used time
     * @param aDataAmount, amount of transferred data
     * @param aServerId, server from where data transferred
     * @return  None
     */
    void UpdateTransferHistoryData( TInt64 aInterval,
                                    TInt64 aDataAmount,
                                    TUint8 aServerId );

    /**
     * Loads wanted items from the db
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */                            
    void GetFillItemsL();

    /**
     * Loads to be deleted items from the db
     *
     * @since S60 5.1
     * @param aArray, array of to be deleted items
     * @return  None
     */        
    void GetToBeDeletedL( RPointerArray<CCmFillListItem>& aArray );

    /**
     * Updates listed items into db
     *
     * @since S60 5.1
     * @param aCancel, if this opration is called by cancel harvest
     * @return  None
     */       
    void UpdateFillItems( TBool aCancel = EFalse );

    /**
     * Updates listed items into db
     *
     * @since S60 5.1     
     * @param aArray, array of to be deleted items
     * @return  None
     */        
    void UpdateToBeDeleted( RPointerArray<CCmFillListItem>& aArray );

    /**
     * Loads right Uuid 
     *
     * @since S60 5.1
     * @param aUuid, contains uuid on return
     * @param aId, id of the wanted uuid
     * @return error code
     */        
    TInt GetUuidL( HBufC8*& aUuid, TUint8 aId );
    
   /**
     * Resets allocated data
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void ResetData();
        
   /**
     * Calculating transfer history time
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void CalculateAvgTransferTimeL();
    
   /**
     * Publishes transfer history time estimate
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void SendAvgTransferTime();
    
    /**
     * Returns media servers db id
     *
     * @since S60 5.1
     * @param aUuid, media server's uuids
     * @return database id
     */     
    TInt64 GetMediaServerIdL( const TDesC8& aUuid );            
                                    
private:

    /**
     * Do update fill list item priorities
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void DoUpdatePrioritiesL();
        
    /**
     * Loading all selected fill rules
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void LoadSelectedFillRulesL();

    /**
     * Loading all fill rules
     *
     * @since S60 5.1
     * @param None
     * @return Fill rule container
     */ 
    CCmFillRuleContainer* LoadAllFillRulesL();
    
    /**
     * Loads defined fill list
     *
     * @since S60 5.1
     * @param aFillListName, name of the list
     * @return None
     */        
    void LoadRuleL( const TDesC8& aFillListName );
    
    /**
     * Updates fill list item priorities up to date
     *
     * @since S60 5.1
     * @param aContainer, container containing all rules
     * @return None
     */     
    TInt UpdateListItemPriorities( CCmFillRuleContainer* aContainer );
            
    /**
     * Initializing metadata find
     *
     * @since S60 5.1
     * @param aFillRule, fill list
     * @return None
     */                        
    void LoadMetadataL( CCmFillRule* aFillRule );
    
    /**
     * Processing possible fill file list duplicates 
     * and adds reference values if needed
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void ProcessReferenceValuesL();

    /**
     * Processes possible fill file list duplicates 
     * and adds reference values if needed
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void DoProcessReferenceValuesL();
    
    /**
     * Processing possible fill file list duplicates,
     * removes duplicates appeared in new sql query
     * If same item is twice in item list, 
     * new item is removed
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ProcessDuplicates();
    
    /**
     * Selects fill list files which are with defined rule id
     *
     * @since S60 5.1
     * @param aList, list to be filled
     * @param aId, fill list id
     * @return None
     */        
    void SelectFillListFiles( RArray<TInt>& aList, TUint8 aId );
    
    /**
     * Updates status values of the fill list files
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ProcessFileStatusValuesL();
    
     /**
     * Updatas the fill file's ids.
     *
     * @since S60 5.1
     * @param aDelIds,array to store files's id 
     * @param aIndex, Index of fill items
     * @param aRealCount, count of fill items
     * @param aRealSize, size of fill items
     * @return None
     */ 
     void ProcessFileSizeAndCount( RArray<TInt>& aDelIds, TInt aIndex, 
                            TInt64& aRealCount, TInt64& aRealSize );

    /**
     * Loads all fill file items
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void LoadAllFillItemsL();
    
    /**
     * Delete duplicated fill items and adjust array
     *
     * @since S60 5.1
     * @param aIndex, Index array of fill items
     */     
	void DoDeleteProcess( TInt aIndex );

   /**
     * Defines unsupported dlna profile ids
     *
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void DefineAndSetDlnaProfileIdFiltersL();   

   /**
     * Completes request
     *
     * @since S60 5.1
     * @param aStatus, defined id for the completing 
     * @return None
     */     
    void CompleteRequest( TCmFmFillRuleProcessing aStatus );
 
    /**
     * Starts asyncronous operartion observer
     * i.e. creates active object
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void StartObserverL();

    /**
     * Creates metadata cache wrapper
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CreateWrapperL();
    
    /**
     * Deletes metadata cache wrapper
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void DeleteWrapper();     

    /**
     * Processes fill lists
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void CheckFillListsL();
    
    /**
     * Deletes hash codes
     * @since S60 5.2
     * @param None
     * @return None
     */ 
    void DeleteHashCodesL();
    
protected: 

// Functions from base class CActive

    /**
     * From CActive
     * See base class definition
     */
    void RunL();

    /**
     * From CActive
     * See base class definition
     */
    void DoCancel();

    /**
     * From CActive
     * See base class definition
     */
    virtual TInt RunError(TInt aError);
        
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, Fill list observer
     * @param aDBMngr, database manager
     * @param aItems, array of list items
     */
    CCmFmFillRuleFilledDataMngr( MCmFmFillRuleObserver* aObserver, 
        CCmDmMain* aDBMngr, RPointerArray<CCmFillListItem>& aItems );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:

    /**
     * Metadata cache wrapper
     */
    MCmSqlMain* iWrapper;                           // owned
    
    /**
     * Active object in use of sql wrapper
     */
    CCmFmAo* iAo;                                   // owned
    
    /**
     * Pointer to observer class
     */
    MCmFmFillRuleObserver* iObserver;               // not owned
    
    /**
     * Pointer to database manager
     */
    CCmDmMain* iDBManager;                          // not owned
    
    /**
     * All fill list items
     */
    RPointerArray<CCmFillListItem>& iItems;         // not owned

    /**
     * fill list items for updating to DB
     */
    RPointerArray<CCmFillListItem> iUpdateItems;    //owned
    
    /**
     * Property values loaded from the cache
     */
    RPointerArray<CCmSqlPropertyItem> iPropertys;   // owned
    
    /**
     * Property container referencing got from client
     */
    CCmSqlPropertyContainer* iPropertyContainer;    // not owned

    /**
     * Property container referencing got from client
     */
    CCmSqlPropertyCollector* iPropertyCollector;    // not owned           
        
    /**
     * Rule array
     */
    RPointerArray<CCmFillRule> iRuleArray;
    
    /**
     * Item ids on the list
     */    
    RArray<TInt> iFileIds;
    
    /**
     * State of the metadata processing
     */ 
    TCmFmMetadaProcessingState iProcessingState;
    
    /**
     * Count of new metadata items
     */ 
    TInt iNewItemCount;
    
    /**
     * Count of all metadata items
     */
    TInt iAllItemCount;
    
    /**
     * Start time of list processing
     */
    TTime iQueryStart;
    /**
     * End time of list processing
     */    
    TTime iQueryFinished;
    
    /**
     * Total transfer time estimate
     */
    TCmProgressInfo iTransferInfo;
    
    /**
     * Container for fill lists
     */
    CCmFillRuleContainer* iContainer;               // owned

    /**
     * Server list whose metadata will be deleted
     */    
    RArray<TInt> iMetadataServersToBeDeleted;
    
    /**
     * Profile filters
     */    
    RArray<TInt64> iFilteredProfiles;
    
    /**
     * File Server session
     */
    RFs iFs;
    
    /**
     * Old item ids on the list
     */    
    RArray<TInt> iOldIds;
   
    };

#endif //  __CMFMFILLLIST_H
