/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Metadata Harvester server's metadata collector component
*
*/







// INCLUDE FILES
#include <upnpstring.h>
#include <upnpavcontrolpoint.h>
#include <upnperrors.h> // HTTP error codes
#include <hash.h>
#include "cmsearchresponsehash.h"
#include "mdhmetadatacollector.h"
#include "msdebug.h"
#include "mdhmediaservercontainer.h"

// CONSTANTS

const TInt KSearchTimeout = 120000000;
const TInt KMaxSearchRetries = 3;
const TInt KSearchCountLimit = 1;

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
CCmMdhMetadataCollector* CCmMdhMetadataCollector::NewL( 
        CCmMdhMediaserverContainer& aMediaserverContainer,
        CUpnpAVControlPoint& aControlPoint,
        TInt aSearchChunkSize, 
        TInt aAddGranularity )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::NewL"));
    
    CCmMdhMetadataCollector* self = new (ELeave) CCmMdhMetadataCollector ( 
        aMediaserverContainer, aControlPoint, 
        aSearchChunkSize, aAddGranularity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CCmMdhMetadataCollector::CCmMdhMetadataCollector( 
        CCmMdhMediaserverContainer& aMediaserverContainer,
        CUpnpAVControlPoint& aAvControlPoint,
        TInt aSearchChunkSize,
        TInt aAddGranularity ) :
    iMediaserverContainer( aMediaserverContainer ), 
    iAvControlPoint ( aAvControlPoint ),
    iSearchChunkSize( aSearchChunkSize ), 
    iAddGranularity( aAddGranularity )
     
    {
    iCdsSync = NULL;
    iXmlToParse = NULL;
    iUuid = NULL;
    iTimer = NULL;
    iSearchRetryCount = 0;
    iSessionId = 0;
    iTempStartIndex = -1;
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::ConstructL()
    {
    iHashGenerator = CSHA1::NewL();
    }
        
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CCmMdhMetadataCollector::~CCmMdhMetadataCollector()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::\
                             ~CCmMdhMetadataCollector"));
    delete iCdsSync;
    delete iXmlToParse;
    delete iTimer;
    iSourceDataArray.ResetAndDestroy();
    delete iHashGenerator;
    delete iUuid;
    iHashValues.ResetAndDestroy();  
    }
    
// ---------------------------------------------------------------------------
// Harvests a media server
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::DoHarvestMediaserverL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::DoHarvestMediaserver"));
    
    // iUuid must be set before calling this method.
    if ( !iUuid ) 
        {
        User::Leave( KErrNotReady );
        }
        
#ifdef _DEBUG
    
    HBufC* uuid = UpnpString::ToUnicodeL( *iUuid );
    TRACE( Print(_L("[CmMdh Server]\t search request, uuid: %S, \
        searchIndex %d, chunk size %d"), 
        uuid, iSearchIndex, iSearchChunkSize ));
    delete uuid;
    uuid = NULL;
    iDebugTime.HomeTime();
    
#endif

    TInt err = iAvControlPoint.CdsSearchActionL( *iUuid,
                                      KRootContainerId,
                                      KSearchCriteria,
                                      KMdhSearchFilter,
                                      iSearchIndex,
                                      iSearchChunkSize,
                                      KSortCriteria );
    TRACE( Print( _L("[CmMdh Server]\t search request returned %d"), err));
    if ( err > 0 ) 
        {
        iSessionId = err;
        }
    LOG( _L("[CmMdh Server]\t creating CPeriodic \
        timer for search timeout..." ) );
    iTimer = CPeriodic::NewL(EPriorityHigh);
    iTimer->Start(KSearchTimeout, 0, TCallBack(TimeoutTimerCompletedL, this));
    
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::\
        DoHarvestMediaserver end"));
    
    }  
    
// ---------------------------------------------------------------------------
// Harvests a media server
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::HarvestMediaserverL( TDesC8& aUuid )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::HarvestMediaserver"));
    iSearchIndex = 0;
    delete iUuid; iUuid = NULL;
    iUuid = aUuid.AllocL();
    iItemsToParse = 0;
    
#ifdef _DEBUG
    
    HBufC* uuid = UpnpString::ToUnicodeL( aUuid );
    
    TRACE( Print( _L("[CmMdh Server]\t starting \
            to harvest, uuid: %S"), uuid));
    delete uuid;
    
#endif
    
    if ( !iCdsSync )
        {
        iCdsSync = CCdsSync::NewL();
        }    
    iResultIncomplete = ETrue;
    iCanceled = EFalse;
    iSearchRetryCount = 0;
    iSourceDataArray.ResetAndDestroy();
    iHashValues.ResetAndDestroy();
    
    iMediaServerId = iMediaserverContainer.IdForMediaServerL( aUuid );
    iMediaserverContainer.HashValuesForMediaServerL( iMediaServerId, iHashValues );
    
    iCdsSync->InitL( iSourceDataArray,
                     iMediaServerId, 
                     *this,
                     iAddGranularity );
                     
    DoHarvestMediaserverL();
   
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::\
        HarvestMediaserver end"));
    }

// ---------------------------------------------------------------------------
// Cancels harvest
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::CancelHarvest()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::CancelHarvestL"));
    if( iCdsSync )
        {
        // cancel parsing and mde operations
        TRAP_IGNORE( iCdsSync->ResetL() ); 
        }
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer; iTimer = NULL;
        }

    // Remove old hash values
    TRAP_IGNORE(iMediaserverContainer.DeleteOldHashValuesL(
                                              iMediaServerId,
                                              iTempStartIndex ) );

    iTempStartIndex = -1;
    iSourceDataArray.ResetAndDestroy();
    iCanceled = ETrue;
    }


// ---------------------------------------------------------------------------
// Static timer callback function
// ---------------------------------------------------------------------------
TInt CCmMdhMetadataCollector::TimeoutTimerCompletedL(TAny* aInstance)
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::\
                             TimeoutTimerCompletedL"));
    return static_cast<CCmMdhMetadataCollector*>
        ( aInstance )->DoTimeoutTimerCompletedL();
    }
   
// ---------------------------------------------------------------------------
// Timer callback function
// ---------------------------------------------------------------------------
TInt CCmMdhMetadataCollector::DoTimeoutTimerCompletedL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::\
                             DoTimeoutTimerCompletedL"));
    
    delete iTimer; iTimer = NULL;
    
    iMediaserverContainer.HarvestCompleteL( KErrTimedOut );
    
    return KErrNone;
   }


   
// ---------------------------------------------------------------------------
// Observer callback for Content Directory Search function.
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::CdsSearchResponse( TInt aSessionId, 
                                                 TInt aErr, 
                                                 const TDesC8& aResult, 
                                                 TInt aReturned, 
                                                 TInt aMatches )
    { 
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::CdsSearchResponse")); 
    TRACE( Print( _L("[CmMdh Server]\t session %d err %d \
        returned:%d matches %d"), 
        aSessionId, aErr, aReturned, aMatches) );


    TRAPD( err, HandleSearchResponseL( aSessionId, 
                                       aErr,
                                       aResult, 
                                       aReturned, 
                                       aMatches ) );
                                      
    if ( err ) 
        {
        TRACE( Print( _L("[CmMdh Server]\t HandleSearchResponse leaved with \
                          error %d"), err ) );
        }
    }

// ---------------------------------------------------------------------------
// Handles search response.
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::HandleSearchResponseL( TInt aSessionId, 
                                                     TInt aErr, 
                                                     const TDesC8& aResult, 
                                                     TInt aReturned, 
                                                     TInt aMatches )
    { 
    LOG(_L(
        "[CmMdh Server]\t CCmMdhMetadataCollector::HandleSearchResponseL") );

    if ( iSessionId != aSessionId ) 
        {
        LOG( _L("[CmMdh Server]\t session id mismatch, returning.."));
        return;
        }
    else 
        {
        iSessionId = 0;
        }

#ifdef _DEBUG
    TTime tempTime;
    tempTime.HomeTime();
    
    TRACE( Print( _L("[CmMdh Server]\t search request took %ld microsec"), 
        tempTime.MicroSecondsFrom( iDebugTime ).Int64() ));
#endif
    
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    
    if ( iXmlToParse )
        {
        LOG( _L("[CmMdh Server]\t deleting old xml buffer.."));
    
        delete iXmlToParse;
        iXmlToParse = NULL;
        }
    
    if ( iCanceled )
        {
        LOG(_L("[CmMdh Server]\t harvest canceled, returning..")); 
        return;
        }
        
    if ( aErr && aErr != EHttpOk )
        {
        if ( aErr == EActionFailed && 
             iSearchRetryCount++ < KMaxSearchRetries ) 
            // 501 = EActionFailed, retrying 
            {
            TRACE( Print( _L("[CmMdh Server]\t retrying search, retry #%d"), 
                    iSearchRetryCount ));
            DoHarvestMediaserverL();
            }
        else 
            {
            LOG(_L("[CmMdh Server]\t Unrecoverable response \
                error, aborting.. ")); 
            delete iTimer; iTimer = NULL;    
            iMediaserverContainer.HarvestCompleteL( aErr );
            }
        return;
        }
        
    // Check the hash value (or should it be checked in the cdssync side...?)
    iHashGenerator->Reset();
    iHashGenerator->Update( aResult );
    TPtrC8 hashValue = iHashGenerator->Final();

    TBool foundMatch( EFalse );
    for ( TInt index(0); index < iHashValues.Count() && !foundMatch; index++ )
        {
        CCmSearchResponseHash* hashInstance = iHashValues[index];
        if ( hashInstance->StartIndex() == iSearchIndex 
             && hashInstance->ItemCount() == aReturned 
             && hashInstance->Hash() == hashValue )
                {
                LOG( _L("[CmMdh Server]\t FOUND HASH MATCH!!"));
                foundMatch = ETrue;
                // Update last matching search chuck to cds sync component
                iCdsSync->SetSearchIndex( iSearchIndex + aReturned );
                }
        }

    if ( !foundMatch )
        {
        // Before updating hash values, remove all changed hash values
        iMediaserverContainer.DeleteOldHashValuesL( iMediaServerId, 
                                                  iSearchIndex );
        
        iMediaserverContainer.AddHashValueForResponseL( iMediaServerId,
                                                        iSearchIndex,
                                                        aReturned,
                                                        hashValue );

        // Save information about hash value in database in order to delete it
        iTempStartIndex = iSearchIndex;

        TRACE( Print( _L("[CmMdh Server]\t HASH= Index:%d\tCount:%d\tHash:%S"),
            iSearchIndex, aReturned, &hashValue ));    
        
        iSearchIndex += aReturned;
        iResultIncomplete = iSearchIndex < aMatches;
        iSearchRetryCount = 0;
        iItemsToParse = aReturned;        
        HBufC8* xmlToParse = aResult.AllocL();
        iSourceDataArray.Append( xmlToParse );
        TRACE( Print( _L("[CmMdh Server]\t source data array has %d items"), 
            iSourceDataArray.Count() ));
         
        iCdsSync->NotifySourceDataAddedL( !iResultIncomplete );
        
        if ( iResultIncomplete && iCdsSync->ChunkCount() < KSearchCountLimit )
            {
            DoHarvestMediaserverL();
            } 
        }
    else    // Found matching parsed search response
        {
        iSearchIndex += aReturned;
        iResultIncomplete = iSearchIndex < aMatches;
        iSearchRetryCount = 0;
        iItemsToParse = aReturned;
        // Send progress
        iMediaserverContainer.SendProgressInfo( aReturned );
        
        if ( iResultIncomplete && iCdsSync->ChunkCount() < KSearchCountLimit )
            {
            DoHarvestMediaserverL();
            }
        else
            {
            iMediaserverContainer.HarvestCompleteL( KErrNone );
            }             
        }
    }

    
// ---------------------------------------------------------------------------
// Sends progress data
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::ProgressL( TInt aItemCount ) 
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::ProgressL")); 
    
    iMediaserverContainer.SendProgressInfo( aItemCount );

    }

// ---------------------------------------------------------------------------
// Cds Sync chunk completion callback function
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::ChunkCompleteL() 
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::ChunkCompleteL")); 
    
    // to check if all of the items have been processed successfully
    // If it is not, the percentage need to be increased for those
    // items that are not processed successfully
    TInt processedItems = iCdsSync->ProcessedItemCount();
    if ( iItemsToParse > processedItems )
    	{
    	ProgressL( iItemsToParse - processedItems );
    	}
    if ( iResultIncomplete && iCdsSync->ChunkCount() < KSearchCountLimit )
        {
        DoHarvestMediaserverL();
        }
    }
    
// ---------------------------------------------------------------------------
// Cds Sync completion callback function
// ---------------------------------------------------------------------------
void CCmMdhMetadataCollector::SyncCompleteL() 
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::SyncCompleteL")); 
    if ( iCanceled )
        {
        LOG(_L("[CmMdh Server]\t harvest canceled, returning..")); 
        return;
        }
    
    iMediaserverContainer.HarvestCompleteL( KErrNone );
    }

// ---------------------------------------------------------------------------
// Cds Sync error callback function
// ---------------------------------------------------------------------------
#ifdef _DEBUG
void CCmMdhMetadataCollector::SyncErrorL( TInt aError ) 
#else // _DEBUG
void CCmMdhMetadataCollector::SyncErrorL( TInt /*aError*/ ) 
#endif // _DEBUG
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMetadataCollector::SyncErrorL")); 
    TRACE( Print( _L("[CmMdh Server]\t error is %d"), aError ));
    }
    

        
