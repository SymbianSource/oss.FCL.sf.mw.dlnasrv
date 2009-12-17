/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Metadata Harvester's metadata collector header
*
*/







#ifndef CMMDHMETADATACOLLECTOR_H
#define CMMDHMETADATACOLLECTOR_H

//  INCLUDES
#include <e32std.h>
#include "cdssync.h"

// FORWARD DECLARATIONS
class CCmMdhMediaserverContainer;
class CUpnpAVControlPoint;
class CSHA1;
class CCmSearchResponseHash;

// DATA TYPES
   
// CLASS DECLARATION

/**
*  CCmMdhMetadataCollector
*  This is a class for metadata harvester server's
*  metadata collector component
*
*  @since Series 60 3.0
*/
class CCmMdhMetadataCollector: public CBase, public MCdsSyncObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCmMdhMetadataCollector* NewL( 
            CCmMdhMediaserverContainer& aMediaserverContainer,
            CUpnpAVControlPoint& aControlPoint,
            TInt aSearchChunkSize, 
            TInt aAddGranularity);
        
        /**
        * Destructor.
        */
        virtual ~CCmMdhMetadataCollector();
        
    public: // New functions
    
        /**
        * Observer callback for Content Directory Search function.
        * @since Series 60 2.0
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aResult
        * @param aReturned
        * @param aMatches
        */
        void CCmMdhMetadataCollector::CdsSearchResponse(
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aResult,
            TInt aReturned,
            TInt aMatches);
        
        /**
        * Harvests a media server
        * @since Series 60 2.0
        * @param aUuid uuid
        */
        void HarvestMediaserverL( TDesC8& aUuid );
        
        /**
        * Cancels harvest
        * @since Series 60 2.0
        * @param aUuid uuid
        */
        void CancelHarvest();
        
        /**
        * Static timer callback function
        * @since Series 60 2.0
        * @param aNy instance of this class
        */
        static TInt TimeoutTimerCompletedL( TAny* aNy );
        
        
        /**
        * Timer callback function
        * @since Series 60 2.0
        */
        TInt DoTimeoutTimerCompletedL();
        

    public: // MCdsSyncObserver functions 
           
        /**
        * Send progress data
        * @since S60 3.1
        * @param aItemCount progressed items count
        */
        void ProgressL( TInt aItemCount );
        
        /**
        * Notification of processed chunk
        * @since S60 3.1
        */
        void ChunkCompleteL();        
        
        /**
        * Cds Sync completion callback function
        * @since S60 3.1
        */
        void SyncCompleteL();
        
        /**
        * Cds Sync error callback function
        * @since S60 3.1
        * @param aError error code
        */
        void SyncErrorL( TInt aError );
    
    private:

       /**
        * Constructor.
        */
        CCmMdhMetadataCollector(
            CCmMdhMediaserverContainer& aMediaserverContainer,
            CUpnpAVControlPoint& aAvControlPoint,
            TInt aSearchChunkSize,
            TInt aAddGranularity);
         
       /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Harvests a media server
        * @since Series 60 2.0
        */
        void DoHarvestMediaserverL();
        
        /**
        * Handles Content Directory search response processing.
        * @since Series 60 3.1
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aResult
        * @param aReturned
        * @param aMatches
        */
        void HandleSearchResponseL( TInt aSessionId, 
                                    TInt aErr, 
                                    const TDesC8& aResult, 
                                    TInt aReturned, 
                                    TInt aMatches );
    private:    // Data
    
        // Cds synchronizer (owned)
        CCdsSync* iCdsSync;
    
        RPointerArray<HBufC8> iSourceDataArray; // elements owned
        
        // reference to mediaserver container 
        CCmMdhMediaserverContainer& iMediaserverContainer;
        
        // reference to av control point
        CUpnpAVControlPoint& iAvControlPoint;
        
        // timeout timer 
        CPeriodic* iTimer; // owned
        
        TInt    iSearchIndex;
        TInt    iSearchRetryCount;
        TInt    iSearchChunkSize;
        TInt    iAddGranularity;
        TInt    iSessionId;
        TInt    iItemsToParse;
        TBool   iResultIncomplete;
        TBool   iCanceled;
        TBool   iInit;
        
        HBufC8* iUuid;       // owned
        HBufC8* iXmlToParse; // owned
        
        TInt    iMediaServerId;
        
#ifdef _DEBUG
        TTime   iDebugTime;
#endif // _DEBUG


        CSHA1*              iHashGenerator;
        
        RPointerArray<CCmSearchResponseHash> iHashValues;
        
        TInt                iTempStartIndex;
                
    };

#endif      // CMMDHMETADATACOLLECTOR_H   
            
// End of File
