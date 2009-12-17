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
* Description:      Interface class of cdssync.
*
*/






#ifndef __CDSSYNC_H__
#define __CDSSYNC_H__

#include <e32cons.h>
#include <e32base.h> 

class CCdsSyncImpl;

/**
 * Callback class of CdsSync
 *
 * @lib cdssync.lib
 *
 * @since S60 5.1
 */
class MCdsSyncObserver
    {
public:

    /**
     * Indicates the number of processed items.
     *
     * @since S60 5.1
     * @param aItemCount, number of processed items
     */
    virtual void ProgressL( TInt aItemCount ) = 0;
    
    /**
     * Indicates that new chunk is completely processed.
     *
     * @since S60 5.1     
     */
    virtual void ChunkCompleteL() = 0;
    
    /**
     * Whole synchronization is complete without errors
     *
     * @since S60 5.1     
     */
    virtual void SyncCompleteL() = 0;
    
    /**
     * Error happened during synchronization.
     *
     * @since S60 5.1
     * @param aError, error code
     */
    virtual void SyncErrorL( TInt aError ) = 0;
    };

/**
 * CDS Sync class definition
 *
 * @since S60 5.1
 */
class CCdsSync : public CBase
    {

public:
    
    /**
     * Two-phase constructor
     */
    IMPORT_C static CCdsSync* NewLC();
    
    /**
     * Two-phase constructor
     */
    IMPORT_C static CCdsSync* NewL();
        
    /**
     * Destructor
     */
    virtual ~CCdsSync();
    
    
public:    
    
    /**
     * Initialises the instance with source array and correct device id
     *
     * @since S60 5.1
     * @param aSourceDataArray, array where search responses are added 
     * @param aDeviceId, database id of the device which is harvested
     * @param aObserver, callback to observer
     * @param aAddGranularity, granularity how much items are
     *        added to db at once
     */
    IMPORT_C void InitL( RPointerArray<HBufC8>& aSourceDataArray,
                         const TInt& aDeviceId, 
                         MCdsSyncObserver& aObserver,
                         TInt aAddGranularity);
    
    
    /**
     * Client indicates that new search response has been added to 
     * source data array.
     *
     * @since S60 5.1
     * @param aSourceDataComplete, boolean indicating whether the device
     *                             has been fully searched.
     */
    IMPORT_C void NotifySourceDataAddedL( 
        TBool aSourceDataComplete = EFalse );
        
    /**
     * Reset the cdssync instance to default state.
     *
     * @since S60 5.1
     */
    IMPORT_C void ResetL();      
    
    /**
     * Returns the source data chunk count including the current chunk 
     * in process.
     *
     * @since S60 5.1
     * @return TInt, count of chunks to be processed
     */
    IMPORT_C TInt ChunkCount();
    
    
    /**
     * Returns successfully processed item count. 
     *
     * @since S60 5.1
     * @return TInt count of items that has been processed successfully.
     */
    IMPORT_C TInt ProcessedItemCount();

    /**
     * Sets search index to cds sync
     *
     * @since S60 5.1
     * @param aSearchIndex, search index
     */    
    IMPORT_C void SetSearchIndex( const TInt aSearchIndex );
    
private:

    // Default constructor
    CCdsSync();
    
    // Second-phase constructor
    void ConstructL(); 
     
private:
    
    /**
     * Actual implementation instance
     */
    CCdsSyncImpl*                   iSyncImpl;      // Owned

    };
    
#endif