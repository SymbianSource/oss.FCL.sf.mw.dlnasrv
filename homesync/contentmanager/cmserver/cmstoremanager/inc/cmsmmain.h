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
* Description:  Main class in the Store manager component
*
*/


#ifndef C_CMSMMAIN_H
#define C_CMSMMAIN_H

#include <e32base.h>
#include "cmsmfileprocessingobserver.h"
#include "cmsmtransferobserver.h"
#include "cmmmobserver.h"
#include "cmcommon.h"
#include "cmstoremanager.h"
   
// Forward declarations
class MUPnPAVController;
class MCmServiceObserver;
class CCmSmTransferEngine;
class CCmSmFileMngr;
class CCmDmMain;
class CCmStoreListItem;
class CCmSqlPropertyContainer;
class CCmMmMain;
class CUpnpAVDeviceList;


/**
 *  CCmSmMain class definition
 *
 *  @lib cmstoremanager.lib
 *
 *  @since S60 v5.1
 */    
class CCmSmMain : public CActive,
                  public MCmSmManager, 
                  public MCmSmFileProcessingObserver,
                  public MCmSmTransferObserver,
                  public MCmMmObserver
    {
        
public:

    /**
     * Two-phased constructor.
     * Creates new CCmSmMain class and opens connection to database.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, Mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to CCmSmMain class
     */
    static CCmSmMain* NewL( 
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );
    
    /**
     * Two-phased constructor.
     * Creates new CCmSmMain class and opens connection to database and 
     * leaves the instance in the cleanup stack.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, Mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to CCmSmMain class
     */
    static CCmSmMain* NewLC(
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    /**
     * Destructor.
     */
    virtual ~CCmSmMain();

public: 

    /**
     * Starts store service
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    void StartStoreL();

    /**
     * Cancels store service
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */        
    void CancelOperation();

    /**
     * Starts store list processing
     *
     * @since S60 5.1     
     * @param None
     * @return  None
     */        
    void PreProcessLists();

    /**
     * Provides album list
     *
     * @since S60 5.1
     * @param aMedia, media type
     * @param aArray, list of values
     * @return  None
     */
    void GetColItemsL( TCmMediaType aMedia, 
        CDesCArray& aArray );
        
    /**
     * Provides metadata column item list
     *
     * @since S60 5.1
     * @param aMedia, media type
     * @param aPropertys, list of values
     * @return  None
     */
    void GetColItemsL( TCmMediaType aMedia, 
        CCmSqlPropertyContainer& aPropertys );        
    
    /**
     * Sets av controller
     *
     * @since S60 5.1     
     * @param aAVController, av controller pointer
     * @return  None
     */        
    void SetAvController( MUPnPAVController* aAVController );    
    
// From base class MCmSmManager
    
    /**
     * From MCmSmManager
     * See base class definition
     */    
    void Close();    
    
// From base class MCmSmFileProcessingObserver

    /**
     * From MCmSmFileProcessingObserver
     * See base class definition
     */
    void FileProcessingStatus( TCmSmFileProcessingStatus aStatus );
    
// From base class MCmSmTransferObserver

    /**
     * From MCmSmTransferObserver
     * See base class definition
     */
    void TransferStatus( TCmSmTransferStatus aStatus );        

// From base class MCmMmObserver

    /**
     * From MCmMmObserver
     * See base class definition
     */     
    void DeleteCompleteL( TInt aErr );

    /**
     * From MCmMmObserver
     * See base class definition
     */     
    void ShrinkCompleteL( TInt aErr );

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
                
private:

    /**
     * Managing files after storing
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    void ManageFileL();    

#ifdef _DEBUG        
    /**
     * Logs server Uuids
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    void LogServers();    
#endif    
  
    /**
     * Publish progress info
     *
     * @since S60 5.1
     * @param aProgress
     * @return  None
     */        
    void SendProgressInfo( TInt aProgress );
    
    /**
     * Changes state
     *
     * @since S60 5.1
     * @param aStatus, defined id for the completing 
     * @return None
     */     
    void ChangeState( TCmSmMainSequencer aStatus );
    
    /**
     * Starts clf refresh timer
     *
     * @since S60 5.1
     * @param None
     * @return None
     */  
    void StartMdETimerL();

    /**
     * Cancels mde refresh timer
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void CancelMdETimer();

    /**
     * Call back for clf refresh timer
     *
     * @since S60 5.1
     * @param aPtr, this
     * @return None
     */ 
    static TInt CheckItems( TAny* aPtr );
    
    /**
     * Starts clf refresh if no services ongoing
     *
     * @since S60 5.1
     * @param None
     * @return KErrNone
     */     
    TInt CheckItemsChanged( );

    /**
     * Delete devices
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void DeleteDevices();
    
    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, service observer
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     */
    CCmSmMain( MCmServiceObserver* aObserver, CCmDmMain* aDBManager, 
        CCmMmMain* aMemManager );

    /**
     * Performs the second phase construction.
     */
    void ConstructL( CMdESession& aSession, CCmDmMain* aDBManager );
    

private:
    
    /**
     * Service observer
     */ 
    MCmServiceObserver* iObserver;                  // not owned
    
    /**
     * Database manager
     */
    CCmMmMain* iMemManager;                         // not owned
        
    /**
     * Memory manager
     */
    CCmDmMain* iDbManager;                          // not owned
    
    /**
     * Upnp operation manager
     */
    CCmSmTransferEngine* iCmSmTransferEngine;       // owned
    
    /**
     * File manager
     */
    CCmSmFileMngr* iCmSmFileMngr;                   // owned
    
    /**
     * Checks if clf state has changed
     */
    CPeriodic* iPeriodic;                           // owned
    
    /**
     * Array of found devices
     */
    CUpnpAVDeviceList* iDevices;                    // owned
    
    /**
     * Fill progress info
     */
    TCmProgressInfo iProgressInfo;
    
    /**
     * Logged storing time values
     */
    TTime iStoreStarted;
    
    /**
     * Index to device array
     */
    TInt iDeviceIndex;
    
    /**
     * List of store file items
     */
    RPointerArray<CCmStoreListItem> iItems;         // owned
    
    /**
     * List of store item ids
     */
    RArray<TInt>iItemIds;                           // owned
    
    /**
     * Id of the current mediaserver 
     */
    TInt64 iId;
    
    /**
     * Service
     */
    TCmService iService;
    
    /**
     * Flag defining if cancel is on
     */
    TBool iCancelOnGoing;
    
    };

#endif //  C_CMSMMAIN_H
