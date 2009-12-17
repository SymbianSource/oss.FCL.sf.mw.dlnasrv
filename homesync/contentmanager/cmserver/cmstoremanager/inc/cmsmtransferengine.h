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
* Description:  Manages Upnp actions needed by Store Manager
*  Version     : %version: tr1ido#1.1.6 % << Don't touch!
*
*/



#ifndef __CMSMTRANSFERENGINE_H
#define __CMSMTRANSFERENGINE_H

#include <e32base.h>
#include <badesca.h>
#include <f32file.h>
#include "upnpfiletransfersessionobserver.h"

// Forward declarations
class MUPnPAVController;
class MCmSmTransferObserver;
class CCmDmMain;
class CCmStoreListItem;
class MCmSqlMain;

// CLASS DECLARATION
 /**
 *  Class represents methods to communication with AV Controller API.
 *
 *  @lib cmstoremanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmSmTransferEngine ): public CBase,
                            public MUPnPFileTransferSessionObserver
    {

public:

    // panic codes
    enum TCmSmTransferEnginePanic
        {
        ECmSmTransferQueueMessedUp = 30
        };

    // Copy statuses        
    enum TCmSmCopyStatus
        {
        ECmSmCopyNever,
        ECmSmCopyContinued,
        ECmSmCopySuccess
        }; 
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aDBMngr, database manager
     * @param aItems, reference to items array
     * @param aItemIds, reference to item id array
     * @return  pointer to CCmSmTransferEngine class
     */
    static CCmSmTransferEngine* NewL( MCmSmTransferObserver* aObserver, 
        CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems, 
        RArray<TInt>& aItemIds );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aDBMngr, database manager
     * @param aItems, reference to items array
     * @param aItemIds, reference to item id array
     * @return  pointer to CCmSmTransferEngine class
     */
    static CCmSmTransferEngine* NewLC( MCmSmTransferObserver* aObserver, 
        CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems,
        RArray<TInt>& aItemIds );

    /**
     * Destructor.
     */
    virtual ~CCmSmTransferEngine();

public: 

    /**
     * Sets av controller
     *
     * @since S60 5.1
     * @param aAVControl, pointer to av controller
     * @return  None
     */        
    void SetAvController( MUPnPAVController* aAVController );
        
    /**
     * Lists media servers 
     *
     * @since S60 5.1
     * @param aDevices, device list
     * @return None
     */
    void GetMediaServersL( CUpnpAVDeviceList*& aDevices );
    
    /**
     * Resets file arrays
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ResetFileArrays();
       
    /**
     * Cancel UPnP copy operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CancelCopy();
    
    /**
     * Copy files
     *
     * @since S60 5.1
     * @param aDevId, device identifier
     * @param aDevice, device
     * @return None
     */    
    void CopyFilesL( TUint8 aDevId, CUpnpAVDevice* aDevice );
    
    /**
     * Continue Copying files
     *
     * @since S60 5.1
     * @return None
     */    
    void ContinueCopyFilesL();
    
    /**
     * Transferred data
     *
     * @since S60 5.1
     * @param None
     * @return data amount
     */      
    TInt DataAmount() const;
    
    /**
     * Returns file list
     *
     * @since S60 5.1
     * @param None
     * @return file array
     */ 
    CDesCArray& FilesToBeRemoved();

    /**
     * Delete file list
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void DeleteToBeRemoved();
    
    /**
     * Returns file list
     *
     * @since S60 5.1
     * @param None
     * @return file array
     */     
    CDesCArray& FilesToBeShrinked();
    
    /**
     * Delete file list
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void DeleteToBeShrinked();

    /**
     * Returns item ids which need to be deleted
     *
     * @since S60 5.1
     * @param None
     * @return Item ids
     */     
    RArray<TInt>& ToBeDeleted();    
    
    /**
     * Cancel operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void CancelOperation();    

protected:

// From base class MUPnPFileTransferSessionObserver

    /**
     * From MUPnPFileTransferSessionObserver 
     * See base class definition
     */
    void TransferStarted( TInt aKey, TInt aStatus );
 
    /**
     * From MUPnPFileTransferSessionObserver 
     * See base class definition
     */    
    void TransferCompleted( TInt aKey, TInt aStatus,
        const TDesC& aFilePath );
    
    /**
     * From MUPnPFileTransferSessionObserver 
     * See base class definition
     */
    void TransferProgress( TInt aKey, TInt aBytes,
        TInt aTotalBytes );   

    /**
     * From MUPnPFileTransferSessionObserver 
     * See base class definition
     */
    void MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason aReason );  
            
private:

    /**
     * Copying one file
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void CopySingleFileL();

    /**
     * Connects to device
     *
     * @since S60 5.1
     * @param aDevice to be connected
     * @return None
     */    
    void ConnectedToDeviceL( const CUpnpAVDevice& aDevice );
    
    /**
     * Wait
     *
     * @since S60 5.1
     * @param aDelay, delay in seconds
     * @return None
     */        
    void WaitForProgress( TInt aDelay );
    
    /**
     * Abort operation
     *
     * @since S60 5.1
     * @param aError, error code
     * @return None
     */
    void AbortTransfer( TInt aError );
    
    /**
     * Adds stored item to database
     *
     * @since S60 5.1
     * @param aFileName, name of the stored file
     * @param aUDN, where file was stored
     * @param aTimeStamp, time stamp for store
     * @return None
     */       
    void AddStoredL( const TDesC& aFileName, const TDesC8& aUDN, 
        TTime aTimeStamp );

    /**
     * Handles item status values
     * @param aKey, database id of item
     * @return  None
     */        
    void HandleItemStatusL( const TInt aKey );

     /**
     * Updates status
     *
     * @since S60 5.1
     * @param aStatus, status to be set
     * @param aFid, file id
     * @param aMsId, media server id
     * @return None
     */    
    void UpdateStoreTransferStatus( TUint aStatus, TInt64 aFid, 
        TInt64 aMsId );
        
     /**
     * Returns index to transferred items array
     *
     * @since S60 5.1
     * @param aKey, database id
     * @return index to item array
     */  
    TInt GetArrayIndex( TInt aKey );

     /**
     * Returns ETrue if file is already on server
     *
     * @since S60 5.1
     * @param aIndex, index to iItems array
     * @return ETrue if file is duplicate
     */     
    TBool IsDuplicateL( const TInt aIndex );

    /**
     * Handle to copying files once
     *
     * @since S60 3.2
     * @param None
     * @return None
     */  
    void CopyFilesOnceL();

    /**
     * Complete the copy process
     *
     * @since S60 3.2
     * @param None
     * @return None
     */  
    void FinishCopy();   

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, transfer observer
     * @param aDBMngr, database manager
     * @param aItems, store items
     * @param aItemIds, item ids
     * @return None
     */
    CCmSmTransferEngine( MCmSmTransferObserver* aObserver, 
        CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems,
        RArray<TInt>& aItemIds );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:

    /**
     * Pointer to AV Controller
     */
    MUPnPAVController* iAVController;               // not owned

    /**
     * Browse action Observer
     */
    MCmSmTransferObserver* iObserver;               // not owned
    
    /**
     * Database manager
     */
    CCmDmMain* iDBManager;                          // not owned  
    
    /**
     * File transfer started
     */
    TTime iTransferStarted;
    
    /**
     * Device
     */  
    CUpnpAVDevice* iDevice;                         // not owned            

    /**
     * File Data amount in kbytes
     */
    TInt iTransferredData;
    
    /**
     * Total transferred data amount in kbytes
     */
    TInt iTotalTransferredData;
    
    /**
     * Items to store
     */
    RPointerArray<CCmStoreListItem>& iItems;
    
    /**
     * Ids of the selected store items
     */
    RArray<TInt>& iItemIds;
    
    /**
     * Item ids to be deleted
     */
    RArray<TInt> iToBeDeleted;                      // owned
    
    /**
     * Index of the processed file
     */
    TInt iIndex;
    
    /**
     * Index of the current mediaserver
     */
    TUint8 iDevId;
    
    /**
     * File to be removed
     */
    CDesCArray* iFilesToBeRemoved;                  // owned

    /**
     * File to be shrinked
     */
    CDesCArray* iFilesToBeShrinked;                 // owned
    
    /**
     * Database wrapper ( member variable because of performance issues )
     */
    MCmSqlMain* iDbWrapper; // owned
    
    /** Browse session */
    MUPnPFileUploadSession* iUploadSession; // not owned
    
    /**
     * Transfer queue
     */    
    RArray<TInt> iTransferQueue;    
    
    /**
     * Copy status
     */
    TCmSmCopyStatus iCopyStatus;
    
    /**
     * File server session
     */
    RFs iFSession;
    
    };

#endif //  __CMSMTRANSFERENGINE_H
