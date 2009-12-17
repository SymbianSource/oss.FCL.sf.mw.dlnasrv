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
* Description:  store file manager
*
*/



#ifndef __CMSMFILEMNGR_H
#define __CMSMFILEMNGR_H

#include <e32base.h>
#include <badesca.h>
#include "cmsmvideoandimagemngrobserver.h"
#include "cmsmmusicmngrobserver.h"
#include "cmsmcommon.h"
#include "cmcommontypes.h"
#include "cmcommon.h"

// Forward declarations
class CCmDmMain;
class CMdESession;
class CCmStoreRule;
class CUpnpAVDevice;
class MCmSmFileProcessingObserver;
class CCmStoreListItem;
class CCmFillListItem;
class CUpnpAVDeviceList;
class CCmSmVideoAndImageMngr;
class CCmSmClfMngr;
class CCmSmContentChangeObserver;
class CCmSmItemInfo;
class CCmSmMsInfo;


// CLASS DECLARATION  
 /**
  *  Processes store files
  * 
  *  @lib cmstoremanager.lib
  *
  *  @since S60 v5.1
  */
NONSHARABLE_CLASS( CCmSmFileMngr ): public CActive,
                      public MCmSmVideoAndImageMngrObserver,
                      public MCmSmMusicMngrObserver
    {         
public:

    /**
     * Two-phased constructor.
     * Creates new CCmSmFileMngr class
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, mde session
     * @param aDBMngr, database manager
     * @param aItems, store list items
     * @return pointer to CCmSmFileMngr class
     */
    static CCmSmFileMngr* NewL(
        MCmSmFileProcessingObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems );
    
    /**
     * Two-phased constructor.
     * Creates new CCmSmFileMngr class
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, mde session
     * @param aDBMngr, database manager
     * @param aItems, store list items
     * @return  pointer to CCmSmFileMngr class
     */
    static CCmSmFileMngr* NewLC(
        MCmSmFileProcessingObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Destructor.
     */
    virtual ~CCmSmFileMngr();

public: 

    /**
     * Starts store list processing
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void StartProcessing();

    /**
     * Provides album list
     * @param aMedia, media type
     * @param aArray, array of albums
     * @return  None
     */
    void ProcessAlbumList( TCmMediaType aMedia, 
    CDesCArray& aArray );
        
    /**
     * Canceling file processing
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CancelOperation();
        
    /**
     * Returns amount of items to be stored
     *
     * @since S60 5.1
     * @param None
     * @return count
     */        
    TInt StoreItemCount();

    /**
     * Returns amount of items to be filled
     *
     * @since S60 5.1
     * @param None
     * @return count
     */      
    TInt FillItemCount();
 
    /**
     * Returns amount of items to be stored, no duplicates
     *
     * @since S60 5.1
     * @param aCount
     * @return None
     */    
    void GetStoreFileCountNoDuplicatesL( TInt& aCount );
    
    /**
     * Updating transfer history
     * @param aInterval, time stamp
     * @param aDataAmount, transfered data
     * @param aServerId, device id
     * @return  None
     */
    void UpdateTransferHistoryData(TInt64 aInterval,
                            TInt64 aDataAmount, TUint8 aServerId );    

    /**
     * Loading all selected Store rules
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void LoadSelectedStoreRulesL();
    
    /**
     * Processing Store rules 
     *
     * @since Series 60 5.1
     * @param None
     * @return None
     */        
    void ProcessStoreRules();

    /**
     * Loads filled and stored items
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void LoadFilledAndStoredL();
    
    /**
     * Loading all filled items
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void LoadFilledL();
    
    /**
     * Loading all stored items ( filled from UDN )
     *
     * @since S60 5.1
     * @param aUDN, server identifier
     * @return None
     */    
    void LoadStoredL( const TDesC8& aUDN );

    /**
     * Reseting filled array
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void ResetFilledArray();

    /**
     * Selects media types from store rules for CLF
     * Refresh. 
     *
     * @since S60 5.1
     * @param None     
     * @return None
     */    
    void SelectMediaTypes();

    /**
     * Checks if file is filled
     *
     * @since S60 5.1
     * @param aPathAndFileName, file to be chekced
     * @return ETrue if file is filled
     */  
    TBool CheckIsFilledFileL( const TDesC& aPathAndFileName );
    
    /**
     * Checks if file is already stored
     *
     * @since S60 5.1
     * @param aPathAndFileName, file to be chekced
     * @param aStoredIds, reference to stored servers array
     * @param aStoreIds, reference to store servers array
     * @return None
     */     
    void IsAlreadyStoreL( const TDesC& aPathAndFileName, 
        RArray<TInt>& aStoredIds, RArray<TInt>& aStoreIds );

    /**
     * Identifies media types used with aDeviceIndex server
     *
     * @since S60 5.1
     * @param aIds, media server ids
     * @param aType, media type
     * @return None
     */        
    void MediaTypesL( RArray<TInt>& aIds, TCmMediaType aType );    
    
    /**
     * Loads all media server id from the database
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void LoadMediaServerIdsL();

    /**
     * Returns devide id
     *
     * @since S60 5.1
     * @param aUuid, media server identifier
     * @return id, database id
     */    
    TInt64 GetDevId( const TDesC8& aUuid );

    /**
     * Select files which is needed to be stored to def. server
     *
     * @since S60 5.1
     * @param aItemIds, item id array
     * @param aUuid, server identifier
     * @param aId, server database id
     * @return None
     */    
    void GetItemsL( RArray<TInt>& aItemIds, 
        const TDesC8& aUuid, TInt64& aId );

    /**
     * Updates store file list up to date
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void UpdateStoreFileListL();
    
    /**
     * Loads store file list from the db
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void LoadStoreFileListL();

    /**
     * Get store rule id
     *
     * @since S60 5.1
     * @param aType, media type
     * @return rule id
     */        
    TInt64 RetrieveListId( TCmMediaType aType );
    
    /**
     * Get status values and server ids 
     *
     * @since S60 5.1
     * @param aType, media type
     * @param aStatus
     * @return None
     */        
    void RetrieveListStatusValues( TCmMediaType aType,
        TCmListItemStatus& aStatus );
        
    /**
     * Deletes items which need to be deleled
     *
     * @since S60 5.1
     * @param aIds, item id array
     * @return None
     */        
    void DeleteItems( RArray<TInt>& aIds );
    
   /**
     * Reseting arrays
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ResetArrays();

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
     * Sets processing status
     * @param aStatus, processing status
     * @return  None
     */        
    void SetProcessingStatus( TCmSmFileProcessingStatus aStatus );   

protected:

// From base class CActive

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
    TInt RunError( TInt aError );    

// From base class MCmSmMediaTypeObserver

    /**
     * From MCmSmMediaTypeObserver
     * See base class definition
     */
    void ImagesAndVideosReady( const TInt aStatus );

// From base class MCmSmMusicMngrObserver

    /**
     * From MCmSmMusicMngrObserver
     * See base class definition
     */
    void MusicReady( const TInt aStatus );        
            
private:

    /**
     * Process media lists
     * 
     * @param aType, defined media type
     * @return None
     */
    void ProcessMedia( TCmMediaType aType );
    
    /**
     * Process music
     * 
     * @param None
     * @return None
     */    
    void ProcessMusics();
    
    /**
     * Completes request
     *
     * @since S60 5.1
     * @param aStatus, defined id for the completing 
     * @return None
     */     
    void CompleteRequest( TCmSmFileProcessingStatus aStatus );        
                    
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, process observer
     * @param aDBMngr, database manager
     * @param aItems, store items
     */
    CCmSmFileMngr(
        MCmSmFileProcessingObserver* aObserver, CCmDmMain* aDBMngr, 
        RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Performs the second phase construction.
     *
     * @since S60 5.1
     * @param aSesstion, mde sesstion
     */
    void ConstructL( CMdESession& aSession );

private:

    /**
     * Pointer to observer class
     */
    MCmSmFileProcessingObserver* iObserver;             // not owned    
    /**
     * Pointer to database manager
     */
    CCmDmMain* iDBManager;                              // not owned         
    /**
     * Rule array
     */
    RPointerArray<CCmStoreRule> iRuleArray;             // owned
    /**
     * Stored items array
     */
    RPointerArray<CCmSmItemInfo>iStoredArray;           // owned
    /**
     * Media server id array
     */
    RPointerArray<CCmSmMsInfo> iMsIds;                  // owned
    /**
     * Store file list items
     */
    RPointerArray<CCmStoreListItem>& iItems;            // not owned
    /**
     * Filled file list items
     */
    RPointerArray<CCmFillListItem>iFilledItems;         // owned
    /**
     * video and image process
     */
    CCmSmVideoAndImageMngr* iVideoAndImageMngr;         // owned      
    /**
     * music process temp
     */
    CCmSmClfMngr* iClfMusicMngr;                        // owned
    /**
     * observers content changes in local device
     */
    CCmSmContentChangeObserver* iCcObserver;            // owned  
    /**
     * Media type array
     */
    RArray<TCmMediaType>iMediaTypes;                    // owned
    /**
     * Used to select media to be refreshed
     */
    TInt iRefreshIndex;
    /**
     * Total transfer time estimate
     */
    TCmProgressInfo iTransferInfo;
    /**
     * Processing status
     */
    TCmSmFileProcessingStatus iProcessingStatus;

    };

#endif //  __CMSMFILEMNGR_H

