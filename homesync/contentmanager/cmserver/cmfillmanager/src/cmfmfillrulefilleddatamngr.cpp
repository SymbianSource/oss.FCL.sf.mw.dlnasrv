/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fill rule and filled data processing
*  Version     : %version: ou1s60rt#18 % << Don't touch!
*
*/



#include <e32property.h>
#include "upnpdlnautility.h"
#include "cmsqlmainfactory.h"
#include "mcmsqlmain.h"
#include "cmdmmain.h"
#include "cmfillrule.h"
#include "cmfillrulecontainer.h"
#include "cmsqlpropertyitem.h"
#include "cmmediaserverfull.h"
#include "cmfilllistitem.h"
#include "cmfmmain.h"
#include "cmfmao.h"
#include "cmfmfillrulefilleddatamngr.h"
#include "msdebug.h"

// Constants
const TInt KCmFmIniDownloadDataAmount = 300; // 300 kBs
const TInt KCmFmIniDownloadTime = 1; // One second


// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::NewL
// ---------------------------------------------------------------------------
//
CCmFmFillRuleFilledDataMngr* CCmFmFillRuleFilledDataMngr::NewL(
    MCmFmFillRuleObserver* aObserver, CCmDmMain* aDBMngr,
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::NewL() start"));    
    CCmFmFillRuleFilledDataMngr* self = CCmFmFillRuleFilledDataMngr::NewLC(
        aObserver, aDBMngr, aItems );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmFillRuleFilledDataMngr* CCmFmFillRuleFilledDataMngr::NewLC(
    MCmFmFillRuleObserver* aObserver, CCmDmMain* aDBMngr,
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::NewLC() start"));     
    CCmFmFillRuleFilledDataMngr* self = 
        new ( ELeave ) CCmFmFillRuleFilledDataMngr( aObserver, aDBMngr, 
                                                    aItems );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::~CCmFmFillRuleFilledDataMngr
// ---------------------------------------------------------------------------
//
CCmFmFillRuleFilledDataMngr::~CCmFmFillRuleFilledDataMngr()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
        ~CCmFmFillRuleFilledDataMngr() start"));
    Cancel();
    
    if ( iWrapper )
        {
        iWrapper->Close();    
        }    
    
    if ( iAo )
        {
        delete iAo;      
        }
    
    iObserver = NULL;
    iDBManager = NULL;
    
    if ( iContainer )
        {
        delete iContainer;    
        }
    
    iRuleArray.Reset();
    iRuleArray.Close();
    
    iUpdateItems.Reset();
    iUpdateItems.Close();        
    
    iPropertys.ResetAndDestroy();
    
    iFileIds.Close();
    iMetadataServersToBeDeleted.Close();
    iOldIds.Close();
    iFs.Close();
    iFilteredProfiles.Reset();
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
        ~CCmFmFillRuleFilledDataMngr() end"));
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CCmFmFillRuleFilledDataMngr
// ---------------------------------------------------------------------------
//            
CCmFmFillRuleFilledDataMngr::CCmFmFillRuleFilledDataMngr( 
    MCmFmFillRuleObserver* aObserver, CCmDmMain* aDBMngr,
    RPointerArray<CCmFillListItem>& aItems )
    :CActive( EPriorityStandard ), iObserver( aObserver ), 
    iDBManager( aDBMngr ), iItems(aItems)
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    CCmFmFillRuleFilledDataMngr()"));    
    
    CActiveScheduler::Add( this );
    iTransferInfo.iService = ECmServiceTransferInfoFill;
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::ConstructL()"));   
    User::LeaveIfError( iFs.Connect() );                           
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::OperationCompletedL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::OperationCompletedL( TInt aStatus )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    OperationCompletedL()"));
    
    iQueryFinished.HomeTime();
    TTimeIntervalMicroSeconds usecsFrom = 
        iQueryFinished.MicroSecondsFrom( iQueryStart );
    TRACE(Print(_L("[FILL MNGR]\t Query \
        took = %ld mseconds"), ( usecsFrom.Int64() / 1000 ) ));    
    
    if( KErrNone == aStatus )
        {
        switch( iProcessingState )
            {
            case ECmFmLoadingPropertys:
                {
                // Loading propertys ready
                CompleteRequest( EColumnDataReady );
                break;
                }
            case ECmFmLoadingMetadata:
                {
                // Loading file metadata ready ( for one fill list )
                iNewItemCount = iItems.Count() - iNewItemCount;              
                CompleteRequest( EQueryCompleted );
                break;
                }
            case ECmFmDeletingMetadata:
                {
                // Metadata deleted from defined media servers
                TRAPD( err, DeleteHashCodesL() );
                iMetadataServersToBeDeleted.Reset();
                iMetadataServersToBeDeleted.Close();
                iWrapper->DeleteUnusedPropertys( );
                if( !err )
                    {
                    CompleteRequest( EMetadataDeleted );
                    }
                else
                    {
                    TRACE(Print(_L("[FILL MNGR]\t \
                        OperationCompletedL err %d "), err ));                    
                    }                                    
                break;
                }
            case ECmFmLoadingProfileIds:
                {
                DefineAndSetDlnaProfileIdFiltersL();
                PreprocessListsL();
                break;
                }
            default:
                {
                LOG(_L("[FILL MNGR]\t OperationCompletedL processing \
                state not found"));
                break;
                }    
            }         
        }
    else
        {        
        TRACE(Print(_L("[FILL MNGR]\t OperationCompletedL aStatus = %d"), 
            aStatus ));        
        }         
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadProfileIdsL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::LoadProfileIdsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::LoadProfileIdsL()"));
    // Loads all dlna profile ids from database. Profile ids are used
    // to filter out unsupported media types
    iPropertys.ResetAndDestroy();
    iProcessingState = ECmFmLoadingProfileIds;
    CreateWrapperL();
    StartObserverL();
    iWrapper->GetPropertyValuesL( iPropertys, iAo->iStatus, 
        ECmProfileId, ECmAll );
    iQueryStart.HomeTime();
    iAo->RunL();    
    }
        
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::PreprocessListsL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::PreprocessListsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::PreprocessListsL()"));
    
#ifdef _DEBUG
    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[FILL MNGR]\t preprocess heap allocsize %d, \
    size %d, cells %d"), alloc, size, cells ));
#endif
    iProcessingState = ECmFmIdle;  
    ResetData();
    // Creates sqlite wrapper and starts preprocessing of all fill lists
    TRAP_IGNORE( CreateWrapperL() );
    iTransferInfo.iTotalItems = KErrNone;
    iTransferInfo.iProcessedItems = KErrNone;        
    CompleteRequest( EPreProcessingStarted );   
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::PreProcessListL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::PreProcessListL( 
    const TDesC8& aFillListName )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::PreProcessListL()"));
    
    iProcessingState = ECmFmIdle;
    // Creates sqlite wrapper and starts preprocessing of 
    // one defined fill list 
    CreateWrapperL();
    ResetData();
    iContainer = CCmFillRuleContainer::NewL();     
    LoadRuleL( aFillListName );
    LoadAllFillItemsL();    
    CompleteRequest( ERandomizingStarted );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetColItemsL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::GetColItemsL( TCmMetadataField aType, 
    TCmMediaType aMedia, CCmSqlPropertyContainer& aPropertys )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::GetColItemsL()"));
    
    iPropertyContainer = NULL;
    iPropertyContainer = &aPropertys;
    iPropertys.ResetAndDestroy(); 
    iProcessingState = ECmFmLoadingPropertys;
    CreateWrapperL();
    StartObserverL();
    // starts loading defined propertys from database
    iWrapper->GetPropertyValuesL(iPropertys, iAo->iStatus, aType, aMedia );
    iQueryStart.HomeTime();
    iAo->RunL();
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetMetadataItemsL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::GetMetadataItemsL(
                                    CCmSqlPropertyCollector& aPropertys )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    GetMetadataItemsL()"));
    
    iPropertyCollector = NULL;
    iPropertyCollector = &aPropertys;    
    iProcessingState = ECmFmLoadingPropertys;
    CreateWrapperL();
    StartObserverL();
    // starts loading property values. Selected artists, albums, genres and 
    // tracks are used as a filtering parameter ( e.g. if some artist is 
    // selected, only albums for that defined artist are loaded.
    iWrapper->GetFilteredPropertyValuesL( *iPropertyCollector, iAo->iStatus );
    iQueryStart.HomeTime();
    iAo->RunL();
    }
    
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::UpdatePriorities
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::UpdatePriorities()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::UpdatePriorities()"));
    
    CompleteRequest( EUpdatePriorities );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DoUpdatePrioritiesL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::DoUpdatePrioritiesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    DoUpdatePrioritiesL()"));
    
    // Updating first list item priorities    
#ifdef _DEBUG
    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[FILL MNGR]\t DoUpdatePrioritiesL start heap \
    allocsize %d, size %d, cells %d"), alloc, size, cells ));    
#endif    
    
    CCmFillRuleContainer* container = LoadAllFillRulesL();
    CleanupStack::PushL( container );
    TInt err( UpdateListItemPriorities( container ) );
    TRACE(Print(_L("[FILL MNGR]\t UpdateListItemPriorities err = %d"), 
    err ));     
    CleanupStack::PopAndDestroy(container);
    CalculateAvgTransferTimeL();
    SendAvgTransferTime();

#ifdef _DEBUG    
    cells = User::Heap().AllocSize( alloc );
    size = User::Heap().Size();
    TRACE(Print(_L("[FILL MNGR]\t DoUpdatePrioritiesL end heap \
    allocsize %d, size %d, cells %d"), alloc, size, cells ));
#endif
                  
    iObserver->FillRuleProcessingStatus( EDoUpdatePriorities );   
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DoUpdateReferenceIdsL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::DoUpdateReferenceIdsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    DoUpdateReferenceIdsL()")); 
    
#ifdef _DEBUG
    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[FILL MNGR]\t DoUpdateReferenceIdsL start heap \
    allocsize %d, size %d, cells %d"), alloc, size, cells ));    
#endif
    
    LoadAllFillItemsL();
    LOG(_L("[FILL MNGR]\t Items loaded...")); 
    
    // Processing set referense ids into duplicate items
    LOG(_L("[FILL MNGR]\t Processing ref ids..."));

    DoProcessReferenceValuesL();   
       
    LOG(_L("[FILL MNGR]\t Ref ids processed..."));        
    UpdateFillItems();
    iItems.ResetAndDestroy();
    iItems.Close();
#ifdef _DEBUG    
    cells = User::Heap().AllocSize( alloc );
    size = User::Heap().Size();
    TRACE(Print(_L("[FILL MNGR]\t DoUpdateReferenceIdsL end heap \
    allocsize %d, size %d, cells %d"), alloc, size, cells ));
#endif     
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DeleteMetadataL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::DeleteMetadataL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::DeleteMetadataL()")); 
    
    iMetadataServersToBeDeleted.Reset();
    
    RPointerArray<CCmMediaServerFull> mediaServers;
    CleanupClosePushL( mediaServers );
   
    iDBManager->GetMediaServersL( mediaServers ); 
    
    TBool setSystemUpdateIDChange( EFalse );
    for( TInt i = 0; i < mediaServers.Count(); i++ )
        {
        // checks that server is either inactive or deleted
        if( mediaServers[i]->DbId() != 0 && 
            !( mediaServers[i]->FillUsage() ) )
            {
            iMetadataServersToBeDeleted.AppendL( mediaServers[i]->DbId() );
            TRACE(Print(_L("[FILL MNGR]\t DbId = %ld"), 
                mediaServers[i]->DbId() ));            
                
            mediaServers[i]->SetSystemUpdateID( KErrNotFound );
                
			setSystemUpdateIDChange = ETrue;
            }
        }

	if ( setSystemUpdateIDChange )
		{
		iDBManager->SetMediaServersL( mediaServers ); 	
		}

    mediaServers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &mediaServers );
    
    iProcessingState = ECmFmDeletingMetadata;
    CreateWrapperL();
    StartObserverL();
    iWrapper->AsyncMetadataDelete( iMetadataServersToBeDeleted, 
        iAo->iStatus );
    iQueryStart.HomeTime();
    iAo->RunL();        
    }
        
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::SetMemoryCardQuota
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::SetMemoryCardQuota( TInt64 aQuota )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    SetMemoryCardQuota()")); 
    
    if( iWrapper )
        {
        iWrapper->SetQuota( aQuota );
        }
    }
    
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CancelOperation
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::CancelOperation()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::CancelOperation()"));
    
    Cancel();    
    if( iWrapper )
        {
        iWrapper->CancelAsyncOperation();
        }
    DeleteWrapper(); 
    if( iAo )
        {
        if( iAo->IsActive() )
            {
            iAo->Cancel();
            } 
        delete iAo;
        iAo = NULL;      
        }
    LOG(_L("[FILL MNGR]\t iAo canceled..."));          
    iObserver->FillRuleProcessingStatus(EProcessingCanceled);           
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::UpdateTransferHistoryData
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::UpdateTransferHistoryData( 
    TInt64 aInterval, TInt64 aDataAmount, 
    TUint8 aServerId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    UpdateTransferHistoryData()")); 
    
    iDBManager->UpdateDownloadHistory( (TUint)aServerId, aDataAmount,
        aInterval );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetFillItemsL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::GetFillItemsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::GetFillItemsL()")); 
    
    iItems.ResetAndDestroy();
    iItems.Close();
    iDBManager->PrepareQueryCmdL(EAllFillFilesStatusQuery);
    iDBManager->GetAllFillFilesL( iItems, 
        ECmToBeShrinked|ECmToBeFilled|ECmFilled|ECmLocalCopy );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetToBeDeletedL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::GetToBeDeletedL(
    RPointerArray<CCmFillListItem>& aArray )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::GetToBeDeletedL()")); 
    
    iDBManager->PrepareQueryCmdL(EAllFillFilesStatusQuery);
    iDBManager->GetAllFillFilesL( aArray, ECmToBeRemoved );   
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::UpdateFillItems
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::UpdateFillItems( TBool aCancel )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::UpdateFillItems()")); 
    
    iDBManager->DeleteFillFiles();
    if( aCancel && iUpdateItems.Count() )
        {
        iDBManager->SetFillFiles( iUpdateItems );
        }
    else
        {
        iDBManager->SetFillFiles( iItems );
        }
    iUpdateItems.Reset();
    iUpdateItems.Close();

    iItems.ResetAndDestroy();
    iItems.Close();

    TRAP_IGNORE( CalculateAvgTransferTimeL() );
    SendAvgTransferTime();    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::UpdateToBeDeleted
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::UpdateToBeDeleted( 
    RPointerArray<CCmFillListItem>& aArray )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    UpdateToBeDeleted()")); 
    
    // This function is needed when fill is processed only partially
    // Some of the to be deleted files are still on device
    iDBManager->SetFillFiles( aArray );
    aArray.ResetAndDestroy();
    aArray.Close();
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetUuidL
// ---------------------------------------------------------------------------
//
TInt CCmFmFillRuleFilledDataMngr::GetUuidL( HBufC8*& aUuid, TUint8 aId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::GetUuidL()")); 
    
    iDBManager->PrepareQueryCmdL( EMediaServerUDNQuery );
    return iDBManager->QueryMediaServerUdn(aUuid, aId);      
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ResetData
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::ResetData()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::ResetData()")); 
    
    iNewItemCount = KErrNone;
    
    iRuleArray.Reset();
    iRuleArray.Close();
    
    iUpdateItems.Reset();
    iUpdateItems.Close();    

    iItems.ResetAndDestroy();
    iItems.Close();
    
    iFileIds.Close();
    iOldIds.Close(); 
    
    if( iContainer )
        {
        delete iContainer;
        iContainer = NULL;
        }              
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CalculateAvgTransferTimeL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::CalculateAvgTransferTimeL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    CalculateAvgTransferTimeL()")); 
    
    TInt64 ud(KErrNone);
    TInt64 dd(KErrNone);
    TInt64 ut(KErrNone);
    TInt64 dt(KErrNone);
    TInt64 size( KErrNone );
    TInt64 shrinkTime(KErrNone);
    TInt64 transferTime(KErrNone);
    TInt ret( KErrNone );   
    RPointerArray<CCmMediaServerFull> servers;
    CleanupClosePushL( servers );
    
    iDBManager->PrepareQueryCmdL( EMediaServersAllQuery );
    while( !ret )
        {
        CCmMediaServerFull* server = CCmMediaServerFull::NewLC();
        ret = iDBManager->QueryAllMediaServersL( server );
        if( !ret )
            {
            servers.AppendL( server );
            CleanupStack::Pop( server );
            }
        else
            {
            CleanupStack::PopAndDestroy( server );
            }   
        }
    TRACE( Print( _L("[FILL MNGR]\t servers.Count() = %d"),
        servers.Count() ));             
    for( TInt i = 0; i < servers.Count(); i++ )
        {
        iDBManager->PrepareQueryCmdL(ETransferHistoryQuery);
        TPtrC8 ser( servers[i]->MediaServer() );
        if( &ser )
            {
            iDBManager->QueryTransferHistory( 
                ser, dd, ud, dt, ut );
            size = iDBManager->KBytesToBeFilled( 
                GetMediaServerIdL(ser), 
                ECmToBeShrinked|ECmToBeFilled );
            TRACE( Print( _L("[FILL MNGR]\t data amount \
                to be filled = %ld"), size ));
                
            TRACE( Print( _L("[FILL MNGR]\t transferred data = %ld"),
                dd )); 
                
            TRACE( Print( _L("[FILL MNGR]\t transfer time = %ld"),
                dt ));                                                 
            if( dd == KErrNone || dt == KErrNone )
                {
                dd = KCmFmIniDownloadDataAmount;
                dt = KCmFmIniDownloadTime;
                }
            // If server hasn't been handled    
            if( size != KErrNone )
                {
                transferTime = transferTime + (( dt * size ) / dd );
                }
            TRACE( Print( _L("[FILL MNGR]\t avg transfer time = %ld"),
                transferTime ));                            
            }
             
        }
    servers.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &servers );
    
    TInt shrinkCount( iDBManager->FillFileCount(ECmToBeShrinked) );
        
    shrinkTime = (iDBManager->GetAvgImageShrinkTime() * shrinkCount);     
    
    TInt totalCount( iDBManager->FillFileCount( 
        ECmToBeShrinked|ECmToBeFilled ));
    transferTime = transferTime + ( shrinkTime / 1000 );
    if( KErrNone >= totalCount )
        {
        transferTime = KErrNone;
        }
    iDBManager->UpdateTransferInfo( ECmServiceFill, totalCount, 
        transferTime );             
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::SendAvgTransferTime
// ---------------------------------------------------------------------------
//   
void CCmFmFillRuleFilledDataMngr::SendAvgTransferTime()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    SendAvgTransferTime()")); 
    
    // Load transfer information from database
    TInt err = iDBManager->GetTransferInfo( ECmServiceFill, 
        iTransferInfo.iProcessedItems, iTransferInfo.iTotalItems );
        
    TRACE( Print( _L("[FILL MNGR]\t GetTransferInfo returned %d"), err));
    
    TRACE( Print( _L(
        "[FILL MNGR]\t item count = %d, avg time = %d"),
            iTransferInfo.iProcessedItems, iTransferInfo.iTotalItems ));    
    iTransferInfo.iService = ECmServiceTransferInfoFill;
        
    TCmProgressInfoPckg transferInfoPckg( iTransferInfo );
    
    err = RProperty::Set( KCmPropertyCat, KCmProperty, 
        transferInfoPckg );
    
    TRACE( Print( _L("[FILL MNGR]\t RProperty::Set returned %d"), err));    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::GetMediaServerIdL
// ---------------------------------------------------------------------------
//     
TInt64 CCmFmFillRuleFilledDataMngr::GetMediaServerIdL( const TDesC8& aUuid )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    GetMediaServerIdL()")); 
    
    iDBManager->PrepareQueryCmdL( EMediaServerIdQuery );
    return iDBManager->QueryMediaServerId( aUuid ); 
    }
                
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadSelectedFillRulesL
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::LoadSelectedFillRulesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
        LoadSelectedFillRulesL()"));
            
    RPointerArray<HBufC8> array;
    CleanupClosePushL( array );
    
    iDBManager->PrepareQueryCmdL( ESelectedFillRuleQuery );    
    iDBManager->QuerySelectedFillRuleNamesL( array, (TInt)ECmSelected );
    // Load selected fill rules to array ( using names as a query 
    // parameter ) 
    for( TInt i = 0; i < array.Count(); i++ ) 
        {
        LoadRuleL( *array[i] );
        }
    
    // Remove random rules. When fill manager is booted up first time 
    // Do not remove random rules => need to be processed
    // If list don't have any files on the list => process list
    for( TInt j = 0; j < iRuleArray.Count(); j++ )
        {
        if( ECmRandom == iRuleArray[j]->Method() || 
            ECmRandomAlbum == iRuleArray[j]->Method() ||
            ECmRandomTrack == iRuleArray[j]->Method() )
            {
            TInt count( KErrNone );
            iDBManager->GetFillFileCount(iRuleArray[j]->Name(), 
                ECmToBeFilled|ECmToBeShrinked|ECmFilled|ECmLocalCopy, count );
            if( KErrNone < count )
                {
                LOG(_L("[FILL MNGR]\t Random rule is already processed"));
                delete iRuleArray[j];
                iRuleArray.Remove(j);
                j--;                
                }
            else
                {
                LOG(_L("[FILL MNGR]\t Random rule is not processed"));
                // Don't remove rule
                }
            }
        }
    for( TInt k = 0; k < iRuleArray.Count(); k++ )
        {
        if( KErrNone == iRuleArray[k]->MediaServerCount() )
            {
            LOG(_L("[FILL MNGR]\t No Servers included in fill rule"));
            TRACE(Print(_L("[FILL MNGR]\t => Any Server rule...")));
            }
        }
        
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    
    CompleteRequest( EFillRulesLoaded );       
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadAllFillRulesL
// ---------------------------------------------------------------------------
//
CCmFillRuleContainer* CCmFmFillRuleFilledDataMngr::LoadAllFillRulesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::FillRuleContainerL"));
    
    RPointerArray<HBufC8> array;
    CleanupClosePushL( array );
    
    // Query fill rule names
    iDBManager->PrepareQueryCmdL( EFillRuleNamesQuery );
    iDBManager->QueryFillRuleNamesL( array );
    TRACE(Print(_L("[FILL MNGR]\t fill rule array size is %d"), 
        array.Count()));
    
    CCmFillRuleContainer* ruleContainer = CCmFillRuleContainer::NewLC();
    for ( TInt i = 0; i < array.Count(); i++ )
        {
        // Query all fill rules
        CCmFillRule* rule = CCmFillRule::NewLC();
        rule->SetNameL( *(array[i]) );
        iDBManager->PrepareQueryCmdL( EFillRuleQuery );
        iDBManager->QueryFillRuleL( rule );
        
        ruleContainer->AddFillRuleL( rule );
        CleanupStack::Pop( rule );
        }
    CleanupStack::Pop( ruleContainer );
    
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    
    return ruleContainer;
    }
    
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadRuleL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::LoadRuleL( const TDesC8& aFillListName )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::LoadRuleL()")); 
    
    CCmFillRule* fillRule = CCmFillRule::NewLC();
    iDBManager->PrepareQueryCmdL(EFillRuleQuery);
    fillRule->SetNameL( aFillListName );
    iDBManager->QueryFillRuleL( fillRule );
    iRuleArray.AppendL( fillRule );   
    iContainer->AddFillRuleL( fillRule );
    CleanupStack::Pop( fillRule );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::UpdateListItemPriorities
// ---------------------------------------------------------------------------
//
TInt CCmFmFillRuleFilledDataMngr::UpdateListItemPriorities( 
    CCmFillRuleContainer* aContainer )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    UpdateListItemPriorities()"));
     
    TInt err( KErrNone );
    err = iDBManager->UpdateFillListItemPriority( aContainer );            
    TRACE(Print(_L("[FILL MNGR]\t priority update err = %d"), 
        err ));            
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadMetadataL
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::LoadMetadataL( CCmFillRule* aFillRule )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    LoadMetadataL()"));
    
    StartObserverL();    
    TRAPD( err, iWrapper->GetFillItemsL( iItems, *aFillRule, iAo->iStatus ) );
    if( !err )
        {
        iQueryStart.HomeTime();
        iAo->RunL();
        iNewItemCount = iItems.Count();
        //add the old item to list
        SelectFillListFiles( iOldIds, aFillRule->ListId() );
        iProcessingState = ECmFmLoadingMetadata;        
        }
    else
        {
        TRACE(Print(_L("[FILL MNGR]\t Metadata find err = %d"), err ));
        TRACE(Print(_L("[FILL MNGR]\t Skipping rule...")));
        iRuleArray.Remove(0);        
        iRuleArray.Compress();   
        /** Splitting long task into shorter ones */            
        CompleteRequest( ENextFillRule );       
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ProcessReferenceValuesL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::ProcessReferenceValuesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    ProcessReferenceValuesL()"));

    DoProcessReferenceValuesL();
    CompleteRequest( EProcessStatusValues );             
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DoProcessReferenceValuesL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::DoProcessReferenceValuesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    DoProcessReferenceValuesL()"));
    
    RArray<TInt> refIds;
    CleanupClosePushL( refIds );
    
    TBool ref( EFalse );
    // Processing set referense ids into duplicate items
    for( TInt j = 0; j < iItems.Count(); j++ )
        {                       
        for( TInt k = iItems.Count() - 1; k >= KErrNone; k-- )
            {
            if( ( KErrNone == iItems[k]->PrimaryText().Compare(
                iItems[j]->PrimaryText() )) && (iItems[k]->Size() 
               == iItems[j]->Size() ) && ( j != k ) && 
                    ( ECmSelected == iItems[j]->Selected() || 
                    ( ECmUnSelected == iItems[j]->Selected() && 
                    ECmUnSelected == iItems[k]->Selected() ) ) )
                {
                if( iItems[k]->Priority() > iItems[j]->Priority() )
                    {
                    refIds.InsertInOrderL( k );
                    }
                else if ( iItems[k]->Priority() < iItems[j]->Priority() )
                    {
                    // Some of the items are on the higher priority than
                    // iItems[ j ]
                    if( ECmUnSelected == iItems[k]->Selected() && 
                        ECmSelected == iItems[j]->Selected() )
                        {
                        refIds.InsertInOrderL( k );
                        }
                    else
                        {
                        ref = ETrue;
                        }    
                    }
                }
            // Check if there is a match on a selected list    
            if( ( KErrNone == iItems[k]->PrimaryText().Compare(
                iItems[j]->PrimaryText() ) ) && ( iItems[k]->Size() 
               == iItems[j]->Size() ) && ( j != k ) && 
                    ( ECmUnSelected == iItems[j]->Selected() && 
                    ECmSelected == iItems[k]->Selected() ) && ( 
                    iItems[k]->Priority() < iItems[j]->Priority() ) )
                {
                // Do not set ref id to zero
                ref = ETrue;
                }
            }
        // Updating referense ids    
        for( TInt i = 0; i < refIds.Count(); i++ )
            {
            iItems[refIds[i]]->SetRefId( iItems[j]->DbId() );
            }
        if( !ref )
            {
            iItems[j]->SetRefId( 0 );
            }
        ref = EFalse;    
        refIds.Reset();    
        }
    
    CleanupStack::PopAndDestroy( &refIds );                     
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ProcessDuplicates
// Processes duplicates. If same list has some item twice,
// newer one is removed from the list
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::ProcessDuplicates()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    ProcessDuplicates()"));

    TBool duplicateFound( EFalse );
    
    for( TInt i = 0; i < iFileIds.Count() - 1; i++ )
        {
        duplicateFound = EFalse;
        //Compare item with following items 
        for( TInt j = i+1; j < iFileIds.Count() && !duplicateFound; j++ )
            {
            if( ( iFileIds[i] < iItems.Count() ) && 
                ( iFileIds[j] < iItems.Count() ) )
                {
                if( i != j && ( ( KErrNone == iItems[iFileIds[i]]->
                    PrimaryText().Compare( 
                    iItems[iFileIds[j]]->PrimaryText() ) ) 
                    && ( iItems[iFileIds[i]]->Size() == 
                    iItems[iFileIds[j]]->Size() ) || 
                    iItems[iFileIds[i]]->DbId() == 
                    iItems[iFileIds[j]]->DbId() ) ) 
                    {
                    if( ( ECmImage == iItems[iFileIds[j]]->MediaType() &&
                         ECmToBeShrinked == iItems[iFileIds[i]]->Status()) ||
                        ( ECmImage == iItems[iFileIds[j]]->MediaType() &&
                          ECmToBeFilled == iItems[iFileIds[i]]->Status()) )
	                    {
	                    // if the media type is image and never be filled
	                    //before ,so do not change the new items status value
	                    //in this cycle , just delete the old items 	
	                    }
	                else
	                    {
	                    iItems[iFileIds[j]]->SetStatus( 
                            iItems[iFileIds[i]]->Status() ) ;	
	                    }
                    TRAP_IGNORE( iItems[iFileIds[j]]->SetPathL( 
                        iItems[iFileIds[i]]->Path() ) );
                    iItems[iFileIds[j]]->SetDriveId( 
                        iItems[iFileIds[i]]->DriveId() );
                    iItems[iFileIds[j]]->SetDriveNumber( 
                        iItems[iFileIds[i]]->DriveNumber() );                                           

                    duplicateFound = ETrue;
                    }                
                }                           
            }
        //iItems[iFileIds[i]] is duplicated, should delete it
        if( duplicateFound )
            {
            DoDeleteProcess( iFileIds[i] );
            i--;
            }
        }  
    iItems.Compress();
     
    /** Spliting long task into shorter ones */            
    CompleteRequest( EProcessReferenceValues );
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::SelectFillListFiles
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::SelectFillListFiles( RArray<TInt>& aList,
                                                       TUint8 aId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    SelectFillListFiles()"));
    
    TRACE(Print(_L("[FILL MNGR]\t LIST ID = %d"), aId ));
    aList.Reset();
    aList.Close();
    TInt count( iItems.Count() );
    for( TInt i = 0; i < count; i++ )
        {
        if( iItems[i]->ListId() == aId )
            {
            aList.InsertInOrderL( i );
            }
        }    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ProcessFileStatusValuesL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::ProcessFileStatusValuesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    ProcessFileStatusValuesL()"));
    
    // Let's update priorities,is it possible that priority has changed
    // And list ids
    TInt64 ruleSize( KErrNone );
    TInt64 realSize( KErrNone );
    TInt err( KErrNone );
    if( iRuleArray.Count() > 0 )
	    {
	    TRAP( err, iDBManager->SetFillListRealCountAndSizeL( 
	        iRuleArray[0]->ListId(), 0, 0 ) );	
	    }
   
    TRACE(Print(_L("[FILL MNGR]\t Size set err = %d"), 
        err ));                    
    TInt64 realCount( iFileIds.Count() );
    for( TInt k = ( iFileIds.Count() - 1 ); k >= KErrNone && 
        iFileIds[k] < iItems.Count() ; k-- )
        {
        // Update priority
        iItems[iFileIds[k]]->SetPriority( iRuleArray[0]->Priority() );
        // Update list id
        iItems[iFileIds[k]]->SetListId( iRuleArray[0]->ListId() );        
        if ( iItems[iFileIds[k]]->Status() == ECmToBeFilled || 
            iItems[iFileIds[k]]->Status() == ECmToBeShrinked ||
            iItems[iFileIds[k]]->Status() == ECmFilled ||
            iItems[iFileIds[k]]->Status() == ECmLocalCopy )
            {
            // increases list size
            realSize += iItems[iFileIds[k]]->Size();
            }

        else if ( iItems[iFileIds[k]]->Status() == ECmToBeRemoved )
            {
            // reduces list size
            realCount--;
            }
                   
        ruleSize = ruleSize + iItems[iFileIds[k]]->Size();
        } 
              
    TInt index( KErrNone );
    RArray<TInt> delIds;
    CleanupClosePushL( delIds );
    
    TRACE( Print( _L("[FILL MNGR]\t iAllItemCount = %d"), iAllItemCount ) );
    TRACE( Print( _L("[FILL MNGR]\t iFileIds.Count() = %d"), 
                  iFileIds.Count() ) );
    TRACE( Print( _L("[FILL MNGR]\t iNewItemCount = %d"), 
                  iNewItemCount ) );
    // Let's mark to be remove extra files  
    if( iRuleArray.Count() > 0 )
        {
        TRACE( Print( _L("[FILL MNGR]\t fill rule array is not empty!")));
        if( iRuleArray[0]->LimitType() == EMbits )  
		    {
		    TRACE( Print( _L("[FILL MNGR]\t fill rule\
		    Limit Type is EMbits!") ) );
		    
		    // KMega=1024*1024
		    // The size of files to be filled should be small than
		    // the amount value set by user.
		    // When realSize be 0 , realCount should be 0 also. 
		    // iAllItemCount > iNewItemCount : sure that extra file be delete
		    while( ( (( iRuleArray[0]->Amount()*KMega ) < realSize ) &&
		              ( realCount>0 )) ||
		                iAllItemCount > iNewItemCount )
		        { 
		        // handle the fill file's count and size.         
		        ProcessFileSizeAndCount( delIds, index, realCount, realSize);
		        index++;         
		        iAllItemCount--;    
		        }  	
		     }
        else
	        {
	        // when limit tyep are EUnlimited and EPieces 
	        // do not need to limit the size of the file that to be filled.
	        while( iAllItemCount > iNewItemCount )
	            {         
	            // handle the fill file's count and size.
                ProcessFileSizeAndCount( delIds, index, realCount, realSize );
	            index++;         
	            iAllItemCount--;    
	            }      	
	        }
        }
    else
        {
     	// if the fill rule array is empty ,just show the information.
     	TRACE( Print( _L("[FILL MNGR]\t fill rule array is empty!")));
        }
    // Let's remove items which had ECmToBeFilled or ECmToShrinked status    
    if( delIds.Count() > KErrNone )
        {
        index = KErrNone;
        while( delIds.Count() != KErrNone )
            {
            ruleSize = ruleSize - iItems[delIds[0] - index]->Size();
            delete iItems[delIds[0] - index];
            iItems.Remove( delIds[0] - index );
            delIds.Remove(0);
            index++;
            }        
        }
    
    CleanupStack::PopAndDestroy( &delIds);
    
    if( iRuleArray.Count() > 0 )
	    {
	    TRAP( err, iDBManager->SetFillListRealCountAndSizeL( 
             iRuleArray[0]->ListId(), realCount, ruleSize ) );
            iUpdateItems.Reset();
            for( TInt j = 0; j < iItems.Count(); j++ )
                {
                iUpdateItems.AppendL( iItems[j] );
                }
	    }
     
    
    TRACE(Print(_L("FILL RULE INFORMATION UPDATE err = %d"), err ));
    if ( iRuleArray.Count() > 0 )
    	{
    	iRuleArray.Remove(0);        
    	iRuleArray.Compress();    	
    	}
    
    TRACE(Print(_L("****** FILL RULE TOTAL SIZE ******")));
    TRACE(Print(_L("FILL RULE SIZE = %ld ( Bytes )"), ruleSize ));
    TRACE(Print(_L("****** FILL RULE TOTAL SIZE ******")));
             
    /** Spliting long task into shorter ones */            
    CompleteRequest( ENextFillRule );           
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::ProcessFileSizeAndCount
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::ProcessFileSizeAndCount(
                                        RArray<TInt>& aDelIds, TInt aIndex, 
                                        TInt64& aRealCount,
                                        TInt64& aRealSize )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    ProcessFileSizeAndCount()"));
    
	TRACE( Print( _L(" realCount = %ld, aDelIds's count =%d") , aRealCount,
	aDelIds.Count() ));
	TRACE( Print( _L(" realSize = %ld ( Bytes )") , aRealSize ));
	
	// Should this status be set if priority is higher than duplicates
    // priority
    if( iItems[iFileIds[aIndex]]->Status() == ECmFilled )
        {
        iItems[iFileIds[aIndex]]->SetStatus( ECmToBeRemoved );
        aRealCount--;
        for( TInt i = 0; i < iItems.Count(); i++ )
            {
            if( iItems[i]->RefId() == iItems[iFileIds[aIndex]]->DbId() )
                {
                // Set also reference ids to status to be removed
                iItems[i]->SetStatus( ECmToBeRemoved );
                }
            }              
        }
    else if( iItems[iFileIds[aIndex]]->Status() == ECmToBeFilled ||
         iItems[iFileIds[aIndex]]->Status() == ECmToBeShrinked )
        {
        aRealSize -= iItems[iFileIds[aIndex]]->Size();
        aDelIds.InsertInOrderL( iFileIds[aIndex] );
        aRealCount--;
        }
    else // ECmLocalCopy
        {
        TRACE(Print(_L("[FILL MNGR]\t No effect on size")));
        }    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::LoadAllFillItemsL
// ---------------------------------------------------------------------------
//    
void CCmFmFillRuleFilledDataMngr::LoadAllFillItemsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::LoadAllFillItemsL()"));
    
    iItems.ResetAndDestroy();
    iItems.Close();    
    iDBManager->PrepareQueryCmdL( EAllFillFilesQuery );
    TInt err = iDBManager->QueryAllFillFilesL(iItems);
    TRACE( Print( _L("[FILL MNGR]\t QueryAllFillFilesL err = %d"), err ));
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DefineAndSetDlnaProfileIdFiltersL
// ---------------------------------------------------------------------------
//
void CCmFmFillRuleFilledDataMngr::DefineAndSetDlnaProfileIdFiltersL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    DefineAndSetDlnaProfileIdFiltersL()"));
    
    CDesC8Array* array = 
        &UPnPDlnaUtility::GetSupportedProfilesL( UPnPDlnaUtility::EDMPMode );
    CleanupStack::PushL( array );
    TInt okProfile( EFalse );
    iFilteredProfiles.Reset();
    for( TInt i = 0 ; i < iPropertys.Count(); i++ )
        {
        for( TInt j = 0; j < array->Count(); j++ )
            {
            if( KErrNotFound != 
                iPropertys[i]->Name().Match( array->MdcaPoint(j) ) )
                {
                // Profile is supported
                okProfile = ETrue;
                // profile is supported, end loop
                j = array->Count();
                }
            }
        if( !okProfile )
            {
            iFilteredProfiles.InsertInSignedKeyOrder( iPropertys[i]->Id() );
            }
        okProfile = EFalse;            
        }
    iPropertys.ResetAndDestroy();    
    CleanupStack::PopAndDestroy( array );    
    }
        
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CompleteRequest
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::CompleteRequest( 
    TCmFmFillRuleProcessing aStatus )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::CompleteRequest()"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmFmFillManager, KErrInUse ));          
    SetActive();        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );     
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::StartObserverL
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::StartObserverL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::StartObserverL()"));
    
    if( iAo )
        {
        delete iAo;
        iAo = NULL;
        }
    iAo = CCmFmAo::NewL( *this );    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CreateWrapperL
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::CreateWrapperL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::CreateWrapperL()"));
    
    DeleteWrapper();
    iWrapper = CCmSqlMainFactory::NewCmSqlMainL( iFs );
#ifndef __SERIES60_31__     
    iWrapper->SetDlnaProfileFilters( iFilteredProfiles );
#endif    
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DeleteWrapper
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::DeleteWrapper()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::DeleteWrapper()"));
    
    if( iWrapper )
        {
        iWrapper->Close();
        iWrapper = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::CheckFillListsL
// ---------------------------------------------------------------------------
//  
void CCmFmFillRuleFilledDataMngr::CheckFillListsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::CheckFillListsL()"));
    
    delete iContainer;
    iContainer = NULL;
    iContainer = LoadAllFillRulesL();
    iRuleArray.Reset();
    for( TInt j = (iContainer->FillRuleCount() - 1 ) ; j >= 0;
        j-- )
        {
        if( ECmRandom ==  iContainer->FillRule(j)->Method() ||
            ECmRandomAlbum == iContainer->FillRule(j)->Method() || 
            ECmRandomTrack == iContainer->FillRule(j)->Method() )
            {
            TInt count( KErrNone );
            iDBManager->GetFillFileCount(
                iContainer->FillRule(j)->Name(), 
                ECmToBeFilled|ECmToBeShrinked|ECmFilled|ECmLocalCopy, 
                count );
            if( KErrNone < count )
                {
                // If rule is random rule which is already processed
                // do not process rule automatically 
                LOG(_L("[FILL MNGR]\t Random rule already processed"));
                iContainer->DeleteFillRule( j );
                }
            else
                {
                LOG(_L("[FILL MNGR]\t Random rule is not processed"));
                // Don't remove rule
                iRuleArray.AppendL( iContainer->FillRule( j ) );
                }                   
            }
        else
            {
            LOG(_L("[FILL MNGR]\t Rule is not random rule"));
            iRuleArray.AppendL( iContainer->FillRule( j ) );
            }    
        }
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DeleteHashCodesL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::DeleteHashCodesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::DeleteHashCodesL()"));
    
    for( TInt i = 0; i < iMetadataServersToBeDeleted.Count(); i++ )
        {
        iDBManager->DeleteHashValuesL( iMetadataServersToBeDeleted[i], 0 );
        }
    }
    
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::RunL
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::RunL()
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::RunL\
         status = %d"), iStatus.Int() ));        
    switch( iStatus.Int() )
        {
        case EPreProcessingStarted:
            {
            CheckFillListsL();
            CompleteRequest( EFillRulesLoaded );                       
            break;
            }
        case EQueryFailed:
            {
            // Error condition what about now...
            TRACE(Print(_L("[FILL MNGR]\t EQueryFailed")));
            if( KErrNone < iRuleArray.Count() )
                {
                iRuleArray.Remove(0);        
                iRuleArray.Compress();                
                }
            TRACE(Print(_L("[FILL MNGR]\t Jump to next rule")));
            CompleteRequest( ENextFillRule );             
            break;
            }
        case EQueryCompleted:
            {
            // Select needed amount of Fill rules metadata objects
            if( KErrNone < iRuleArray.Count() )
                {
                SelectFillListFiles( iFileIds, iRuleArray[0]->ListId() );
                iAllItemCount = iFileIds.Count();
                TRACE(Print(_L("[FILL MNGR]\t Already on the list = %d"), 
                    iAllItemCount ));
                CompleteRequest( EProcessDuplicates );
                }
            else
                {
                ResetData();
                DeleteWrapper();
                iObserver->FillRuleProcessingStatus(ENoObjectsSelected);
                }    
            break;
            }
        case ENextFillRule:
            {       
            // Start processing next fill rule
            if( KErrNone < iRuleArray.Count() )
                {
                LoadMetadataL(iRuleArray[0]);
                }
            else
                {
                TRACE(Print(_L("[FILL MNGR]\t Rule array empty")));
                UpdateFillItems();
                ResetData();
                DeleteWrapper();                
#ifdef _DEBUG
                TInt alloc;
                TInt cells = User::Heap().AllocSize( alloc );
                TInt size = User::Heap().Size();
                TRACE(Print(_L("[FILL MNGR]\t preprocess end heap \
                allocsize %d, size %d, cells %d"), alloc, size, cells ));
#endif                
                iObserver->FillRuleProcessingStatus(EProcessingReady);
                }    
            break;
            }
        case EFillRulesLoaded:
            {
            LoadAllFillItemsL();
            CompleteRequest( EFillFilesLoaded );
            break;
            }
        case EFillFilesLoaded:
            {
            if( KErrNone < iRuleArray.Count() )
                {
                LoadMetadataL( iRuleArray[0] );    
                }
            else
                {
                TRACE(Print(_L("[FILL MNGR]\t Rule array empty")));
                TRAP_IGNORE( CalculateAvgTransferTimeL() );
                SendAvgTransferTime();
                ResetData();
                DeleteWrapper();                
                iObserver->FillRuleProcessingStatus(ENoRulesSelected);
                }
            break;    
            }
        case EProcessDuplicates:
            {
            ProcessDuplicates();
            break;
            }
        case EProcessReferenceValues:
            {
            ProcessReferenceValuesL();
            break;
            }
        case EProcessStatusValues:
            {
            ProcessFileStatusValuesL()  ;
            break;
            }
        case ERandomizingStarted:
            {
            if( KErrNone < iRuleArray.Count() )
                {
                LoadMetadataL( iRuleArray[0] );    
                }
            else
                {
                TRACE(Print(_L("[FILL MNGR]\t Rule array empty")));
                ResetData();
                DeleteWrapper();
                iObserver->FillRuleProcessingStatus(ENoRulesSelected);
                }
            break;
            }
        case EColumnDataReady:
            {
            DeleteWrapper();
            iObserver->FillRuleProcessingStatus(EColumnDataReady);
            break;
            }
        case EUpdatePriorities:
            {
            DoUpdatePrioritiesL();
            break;
            }
        case EMetadataDeleted:
            {
            iObserver->FillRuleProcessingStatus( EMetadataDeleted );
            break;
            }
        default:
            {
            LOG(_L("[FILL MNGR]\t Fill rule processing RunL default"));
            User::Invariant();
            break;
            }
        }
    }    

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::RunError
// ---------------------------------------------------------------------------
// 
#ifdef _DEBUG  
TInt CCmFmFillRuleFilledDataMngr::RunError( TInt aError )
#else
TInt CCmFmFillRuleFilledDataMngr::RunError( TInt /*aError*/ )
#endif
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::\
    RunError = %d"), aError ));       
    
    iObserver->FillRuleProcessingStatus( EProcessingFailed );
    return KErrNone;     
    }

// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DoCancel
// ---------------------------------------------------------------------------
//     
void CCmFmFillRuleFilledDataMngr::DoCancel()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::DoCancel()"));
    }
        
// ---------------------------------------------------------------------------
// CCmFmFillRuleFilledDataMngr::DoDeleteProcess
// ---------------------------------------------------------------------------
// 
void CCmFmFillRuleFilledDataMngr::DoDeleteProcess( TInt aIndex )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillRuleFilledDataMngr::DoDeleteProcess()"));
    TBool delNew( ETrue );
    
    //delete duplicated fill item
    delete iItems[aIndex];
    iItems.Remove( aIndex );

    //adjust iOldIds, and judge the item new item or old item
    for( TInt i = 0; i < iOldIds.Count(); i++ )
        {
        if( iOldIds[i] > aIndex )
            {
            iOldIds[i] = iOldIds[i] - 1;
            }
        else if( iOldIds[i] == aIndex )
            {
            delNew = EFalse;
            iOldIds.Remove(i);
            i--;
            }
        }
    
    //adjust iFileIds
    for( TInt i = 0; i < iFileIds.Count(); i++ )
        {
        if( iFileIds[i] > aIndex )
            {
            iFileIds[i] = iFileIds[i] - 1;
            }
        else if( iFileIds[i] == aIndex )
            {
            iFileIds.Remove(i);
            i--;
            }
        }

    //count update
    iAllItemCount--;
    if( delNew )
        {
        iNewItemCount--;
        }
    }


// End of file
