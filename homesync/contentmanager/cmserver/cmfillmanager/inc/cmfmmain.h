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
* Description:  Main class in the Fill manager component
*
*/


#ifndef C_CMFMMAIN_H
#define C_CMFMMAIN_H

#include <f32file.h>
#include "cmfmfillruleobserver.h"
#include "cmfmupnpactionobserver.h"
#include "cmfmtransferobserver.h"
#include "cmfmitemhandlerobserver.h"
#include "cmmmobserver.h"        
#include "cmcommon.h"
#include "cmfillmanager.h"
   
/* Forward declarations. */
class CCmFmUpnpMngr;
class MCmServiceObserver;
class CCmFmFillDriveMngr;
class CCmFmFillRuleFilledDataMngr;
class CCmFmDuplicateDetector;
class CCmMmMain;
class CCmDmMain;
class CCmFmDownloadProxy;
class CCmFillListItem;
class CUpnpAVDeviceList;
class CCmDriveInfo;
class CMdESession;
class CCmFmMpxNotifier;
class CCmFmGlxNotifier;

// Constants
_LIT( KCmFmFillManager, "FillManager");

/**
 *  CCmFmMain class 
 *  Part of Fill manager.Fill service
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 v3.1
 */
class CCmFmMain : public CActive,
                  public MCmFmManager, 
                  public MCmFmFillRuleObserver,
                  public MCmFmUPnPActionObserver, 
                  public MCmFmTransferObserver,
                  public MCmFmItemHandlerObserver, 
                  public MCmMmObserver
    {     
    
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, service observer
     * @param aSession, mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to CCmFmMain class
     */
    static CCmFmMain* NewL(
        MCmServiceObserver* aObserver, CMdESession& aSession,
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, service observer
     * @param aSession, mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to CCmFmMain class
     */
    static CCmFmMain* NewLC(
        MCmServiceObserver* aObserver, CMdESession& aSession,
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    /**
     * Destructor.
     */
    virtual ~CCmFmMain();

    /**
     * Sort function that arranges pointers by id
     * @since S60 5.1
     * @param aItem1, item to be sorted
     * @param aItem2, item to be sorted
     */
    static TInt CompareById( const CCmFillListItem& aItem1,
                             const CCmFillListItem& aItem2 );

    /**
     * Sort function that arranges pointers by size
     * @since S60 3.2
     * @param aItem1, item to be sorted
     * @param aItem2, item to be sorted
     */
    static TInt CompareBySize( const CCmFillListItem& aItem1,
                               const CCmFillListItem& aItem2 );


protected:

// From base class MCmFmManager
    
    /**
     * From MCmFmManager
     * See base class definition
     */
    void StartFillL();

    /**
     * From MCmFmManager
     * See base class definition
     */
    void CancelOperation();

    /**
     * From MCmFmManager
     * See base class definition
     */
    void PreProcessListsL();

    /**
     * From MCmFmManager
     * See base class definition
     */
    void PreProcessListL( const TDesC8& aFillListName );

    /**
     * From MCmFmManager
     * See base class definition
     */
    void GetColItemsL( TCmMetadataField aType, 
        TCmMediaType aMedia, CDesCArray& aArray );

    /**
     * From MCmFmManager
     * See base class definition
     */
    void GetColItemsL( TCmMetadataField aType, 
                       TCmMediaType aMedia,
                       CCmSqlPropertyContainer& aPropertys );

    /**
     * From MCmFmManager
     * See base class definition
     */
    void GetMetadataItemsL( CCmSqlPropertyCollector& aPropertys );

    /**
     * From MCmFmManager
     * See base class definition
     */   
    void SetAvController( MUPnPAVController* aAVController ); 

    /**
     * From MCmFmManager
     * See base class definition
     */   
    void UpdatePriorities();

    /**
     * From MCmFmManager
     * See base class definition
     */
    void UpdateRefIds();

    /**
     * From MCmFmManager
     * See base class definition
     */
    void DeleteMetadataL();

    /**
     * From MCmFmManager
     * See base class definition
     */   
    void Close();

// From base class MCmFmFillRuleObserver

    /**
     * From MCmFmFillRuleObserver
     * See base class definition
     */   
    virtual void FillRuleProcessingStatus( TCmFmFillRuleProcessing aStatus );

// From base class MCmFmTransferObserver

    /**
     * From MCmFmTransferObserver
     * See base class definition
     */   
    virtual void TransferStatus( TCmFmTransferStatus aStatus,
                    TInt aSessionId,
                    TInt aInternalItemId,
                    TInt aError,
                    TInt64 aFSize );

// From base class MCmFmUPnPActionObserver     

    /**
     * From MCmFmUPnPActionObserver
     * See base class definition
     */   
    virtual void URICheckResult( TCmFmUriCheckResult aStatus,
        const CUpnpItem* aItem = NULL, const CUpnpElement* aRes = NULL );

// From base class MCmFmItemHandlerObserver   

    /**
     * From MCmFmItemHandlerObserver
     * See base class definition
     */  
    virtual void DuplicateL( const TBool aDuplicate );                

// From base class MCmMmObserver 

    /**
     * From MCmMmObserver
     * See base class definition
     */  
    virtual void DeleteCompleteL( TInt aErr );

    /**
     * From MCmMmObserver
     * See base class definition
     */  
    virtual void ShrinkCompleteL( TInt aErr );            

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

private:

    /**
     * Sets mmc quota to fill list manager
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    void SetMmcQuotaL();

    /**
     * Handles mmc id ok state
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void HandleIfMmcOkL();

    /**
     * Handles if files need to be deleted
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void HandleIfDeleteFilesL();

    /**
     * Completes request
     *
     * @since S60 5.1
     * @param aStatus, completing status
     * @return None
     */
    void CompleteRequest( TCmFmStatus aStatus );

    /**
     * Completes request and incerements iIndex
     *
     * @since S60 5.1
     * @param aStatus, completing status
     * @return None
     */    
    void ProceedWithNextItem( TCmFmStatus aStatus );

    /**
     * Do uri check if needed
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void DoUriCheckL();
    
    /**
     * Parses full path from the Uri and upnp class
     *
     * @since S60 5.1
     * @param None
     * @return Error code
     */
    TInt ParseFullPathL();

    /**
     * Parses file name from the Uri
     *
     * @since S60 5.1
     * @param None
     * @return Error code
     */    
    TInt ParseFileNameL();

    /**
     * Sets uri to current item
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void SetUriL();
    
    /**
     * Logs media server Uuids
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void LogServers();

    /**
     * Reset metadata item infos
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void ResetItemInfo();

    /**
     * Publishes progress info
     *
     * @since S60 5.1
     * @param aProgress, progress 
     * @return None
     */    
    void SendProgressInfo( TInt aProgress );

    /**
     * Sets status to ECmFilled for every reference file
     *
     * @since S60 5.1
     * @param aInternalItemId, item id
     * @return  None
     */    
    void SetStatusL( const TInt64 aInternalItemId );

    /**
     * Removes illegal chars from the file name
     *
     * @since S60 5.1
     * @param aFileName, file name
     * @return modified file name
     */      
    HBufC* RemoveIllegalFilenameCharactersL( const TDesC& aFileName );

    /**
     * Replaces defined chars
     *
     * @since S60 5.1
     * @param aString, file name string
     * @param aTrg, char to be changed
     * @param aReplacement, replacement char
     * @return None
     */    
    void ReplaceCharsL( TDes& aString, const TDesC& aTrg,
                   const TDesC& aReplacement );

    /**
     * Loads selected drives into iDrives array
     *
     * @since S60 5.1
     * @param None
     * @return None
     */                   
    void LoadDrivesL();                   

    /**
     * Updates selected drives into iDrives table
     *
     * @since S60 5.1
     * @param None
     * @return None
     */            
    void UpdateDrives();

    /**
     * Resolves root path of the fill drive
     *
     * @since S60 5.1
     * @param aDriveNumber, drive number
     * @return Root path
     */     
    HBufC* ResolveAndAppendRootPathL( const TInt aDriveNumber );
    
    /**
     * Updates quota of used drive
     *
     * @since S60 5.1
     * @param aDriveNumber, drive number
     * @param aQuota, new used quota
     * @return None
     */    
    void UpdateDriveQuota( const TInt aDriveNumber, 
        const TInt64 aQuota );

    /**
     * Initializes quotas of all selected drives
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void InitializeDriveQuotasL();

    /**
     * Initializes quotas of all selected drives
     *
     * @since S60 5.1
     * @param aDriveNumber, drive number
     * @return used fill space per drive
     */     
    TInt64 GetUsedQuota( const TInt aDriveNumber );


    /**
     * Creates RFile
     *
     * @since S60 5.1
     * @param aFilePath, file name and path
     * @return error code
     */    
    TInt CreateRFile( const TDesC& aFilePath );
    
    /**
     * Resolves private path of cm server
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void PrivatePathL( );
    
    /**
     * Replaces file with orginal file
     *
     * @since S60 5.1
     * @param aOriginal, orginal file name
     * @return None
     */    
    void ReplaceFileL( const TDesC& aOriginal );

    /**
     * Returns index to iDevices device array
     *
     * @since S60 5.1
     * @param aUuid, identifier of the server
     * @param aList, device list
     * @return index to iDevices 
     */    
    TInt CheckServerIndex( const TDesC8& aUuid,
        const CUpnpAVDeviceList& aList );
        
    /**
     * Returns ETrue if one of needed servers is available
     *
     * @since S60 5.1
     * @param None
     * @return ETrue if server available
     */     
    TBool NeededServerAvailableL( );

    /**
     * Reset item arrays
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void ResetItems();

    /**
     * Reset delete queue array
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ResetDeleteQueue();

    /**
     * Adds file to delete queue
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void AddFilesToDeleteQueueL();        
         
    /**
     * Performs operations after items have been checked
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void StopItemCheck();

private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, service observer
     * @param aMemManager, memory manager
     * @pram aDBManager, database manager
     */
    CCmFmMain( MCmServiceObserver* aObserver, CCmMmMain* aMemManager, 
        CCmDmMain* aDBManager );

    /**
     * Performs the second phase construction.
     *
     * @since S60 5.1
     * @param aSession, mde session
     * @param aDBManager, database manager
     */
    void ConstructL( CMdESession& aSession, CCmDmMain* aDBManager );
    

private:
    
    /**
     * Fill manager observer
     */ 
    MCmServiceObserver* iObserver;                              // Not owned
        
    /**
     * Memory manager
     */
    CCmMmMain* iMemManager;                                     // Not owned
    
    /**
     * Database manager
     */
    CCmDmMain* iDbManager;                                      // Not owned
    
    /**
     * Upnp operation manager
     */
    CCmFmUpnpMngr* iCmFmUpnpMngr;                               // Owned
    
    /**
     * fill drive id manager
     */
    CCmFmFillDriveMngr* iCmFmFillDriveMngr;                     // Owned
    
    /**
     * Fill rule and filled data manager
     */
    CCmFmFillRuleFilledDataMngr* iCmFmFillRuleFilledDataMngr;   // Owned

    /**
     * Item handler, checks if media item is already in phone
     */
    CCmFmDuplicateDetector* iCmFmDuplicateDetector;             // Owned
       
    /**
     * Download proxy
     */
    CCmFmDownloadProxy* iCmFmDownloadProxy;                     // Owned
    
    /**
     * Mpx notifier
     */
    CCmFmMpxNotifier* iCmFmMpxNotifier;                         // Owned
    
    /**
     * Glx notifier
     */
    CCmFmGlxNotifier* iCmFmGlxNotifier;                         // Owned
    
    /**
     * AV Controller
     */
    MUPnPAVController* iAvController;                           // Not owned
    
    /**
     * Array of found devices
     */
    CUpnpAVDeviceList* iDevices;                                // Owned
    
    /**
     * Array of fill list items
     */
    RPointerArray<CCmFillListItem> iItems;                      // Owned
    
    /**
     * Array of to be deleted fill list items
     */
    RPointerArray<CCmFillListItem> iToBeDeleted;                // Owned
    
    /**
     * Array of to be deleted fill file names and paths
     */
    CDesCArray* iArray;                                         // Owned 
    
    /**
     * File name and path to media file
     */
    TFileName iFullPath;
    
    /**
     * Fill progress info
     */
    TCmProgressInfo iProgressInfo;
    
    /**
     * Temporary uuid
     */
    HBufC8* iUuid;                                              // Owned
    
    /**
     * Calculating fill transfer times
     */
    TTime iFileFetchStart;
    
    /**
     * Index of the current item
     */
    TInt iIndex;
    
    /**
     * Started service
     */
    TCmService iService;
    
    /**
     * Flag identifying if fill is on or not
     */
    TBool iFetchingFile;
    
    /**
     * File server session
     */
    RFs iFsSession;
    
    /**
     * Drive info table
     */
    RPointerArray<CCmDriveInfo> iDrives;
    
    /**
     * Drive id of the fill drive
     */
    TUint iDriveId;
    
    /**
     * Drive number of the fill drive
     */
    TInt iDriveNumber;
    
    /**
     * Index to drive info array
     */
    TInt iDriveIndex;
    
    /**
     * File
     */
    RFile iFile;

    /**
     * Index of sent items
     */
    TInt iTempIndex;

    };

#endif //  C_CMFMMAIN_H
