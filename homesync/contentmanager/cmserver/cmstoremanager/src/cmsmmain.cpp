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
* Description:  Main class in Store manager component
*
*/


#include <e32property.h>
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpavdevicelist.h"
#include <utf.h>
#include "cmserviceobserver.h"
#include "cmsmfilemngr.h"
#include "cmsmtransferengine.h"
#include "cmstorelistitem.h"
#include "cmdmmain.h"
#include "cmmmmain.h"
#include "cmsmmain.h"
#include "msdebug.h"

// One file transfered
const TInt KCmOneFile = 1;
const TInt KCmSmClfRefreshInterval = 15000000;
const TInt KCmSmOneSecond = 1000000;

// ---------------------------------------------------------------------------
// CCmSmMain::NewL
// ---------------------------------------------------------------------------
// 
CCmSmMain* CCmSmMain::NewL( 
    MCmServiceObserver* aObserver, CMdESession& aSession,
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::NewL() start"));
    CCmSmMain* self = CCmSmMain::NewLC( aObserver, aSession,
        aDBManager, aMemManager );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmMain::NewL() end")); 
    return self;         
    }

// ---------------------------------------------------------------------------
// CCmSmMain::NewLC
// ---------------------------------------------------------------------------
//  
CCmSmMain* CCmSmMain::NewLC(
    MCmServiceObserver* aObserver, CMdESession& aSession, 
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::NewLC() start"));    
    CCmSmMain* self = new ( ELeave ) CCmSmMain( aObserver, aDBManager, 
        aMemManager );
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aDBManager );
    LOG(_L("[STORE MNGR]\t CCmSmMain::NewLC() end")); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmMain::~CCmSmMain
// ---------------------------------------------------------------------------
//
CCmSmMain::~CCmSmMain()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::~CCmSmMain() start"));
    Cancel();
    delete iCmSmTransferEngine;
    delete iCmSmFileMngr;
    DeleteDevices();
    iItems.ResetAndDestroy();
    iItems.Close();
    iItemIds.Reset();
    iItemIds.Close();
    delete iPeriodic;            
    LOG(_L("[STORE MNGR]\t CCmSmMain::~CCmSmMain() end"));     
    }

// ---------------------------------------------------------------------------
// CCmSmMain::CCmSmMain
// ---------------------------------------------------------------------------
//    
CCmSmMain::CCmSmMain( MCmServiceObserver* aObserver, CCmDmMain* aDBManager,
    CCmMmMain* aMemManager  ) 
    : CActive( EPriorityStandard ), iObserver( aObserver ), 
        iMemManager( aMemManager ), iDbManager( aDBManager )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::CCmSmMain"));
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmSmMain::ConstructL
// ---------------------------------------------------------------------------
//
void CCmSmMain::ConstructL( CMdESession& aSession, CCmDmMain* aDBManager )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::ConstructL() start"));
    iService = ECmServiceNone;
    iCmSmTransferEngine = 
        CCmSmTransferEngine::NewL( this, aDBManager, iItems, iItemIds );
    iCmSmFileMngr = 
        CCmSmFileMngr::NewL( this, aSession, aDBManager, iItems );
    LOG(_L("[STORE MNGR]\t CCmSmMain::ConstructL() end"));
    iProgressInfo.iService = ECmServiceStore;
    }     

// ---------------------------------------------------------------------------
// CCmSmMain::StartStoreL
// ---------------------------------------------------------------------------
//    
void CCmSmMain::StartStoreL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::StartStoreL()"));
    if( iService == ECmServiceAutomStoreListProcess )
        {
        iCmSmFileMngr->CancelOperation();
        }
    iCancelOnGoing = EFalse;
    iService = ECmServiceStore;
    iMemManager->SetObserver( *this );
    iDeviceIndex = KErrNone;
    iProgressInfo.iService = ECmServiceStore;
    iProgressInfo.iProcessedItems = KErrNone;
    iProgressInfo.iTotalItems = iCmSmFileMngr->StoreItemCount() + 
        iCmSmFileMngr->FillItemCount();
    SendProgressInfo( KErrNone );    
    ChangeState( ECmSmGetMediaServers );
    }

// ---------------------------------------------------------------------------
// CCmSmMain::CancelOperation
// ---------------------------------------------------------------------------
//
void CCmSmMain::CancelOperation()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::CancelOperation()"));    
    // If some files need to be removed or shrinked do it now
    iCancelOnGoing = ETrue;
    iItems.ResetAndDestroy();
    iItems.Close();
    iItemIds.Reset();
    iItemIds.Close();
    iCmSmFileMngr->CancelOperation();    
    iCmSmTransferEngine->CancelOperation();              
    iObserver->ServiceExecuted( iService, KErrCancel );
    // Service finished. Reset progress info.
    iProgressInfo.iService = ECmServiceNone;
    iService = ECmServiceNone;
    SendProgressInfo( KErrNone );    
    }

// ---------------------------------------------------------------------------
// CCmSmMain::PreProcessLists
// ---------------------------------------------------------------------------
//
void CCmSmMain::PreProcessLists()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::PreProcessLists"));
    
    iCancelOnGoing = EFalse;
    if( iService == ECmServiceAutomStoreListProcess )
        {
        iCmSmFileMngr->CancelOperation();
        iCmSmFileMngr->SetProcessingStatus( ECmSmProcessingFilesStarted );
        }    
    else
        {
        ChangeState( ECmSmStartFileProcessing );
        }
    
    iService = ECmServicePreProcessingStore;
    }

// ---------------------------------------------------------------------------
// CCmSmMain::GetColItemsL
// ---------------------------------------------------------------------------
//
void CCmSmMain::GetColItemsL( TCmMediaType aMedia, 
    CDesCArray& aArray )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::GetColItemsL"));
    
    iCancelOnGoing = EFalse;
    if( iService == ECmServiceAutomStoreListProcess )
        {
        iCmSmFileMngr->CancelOperation();
        }    
    iService = ECmServiceGetStoreFields;
    iCmSmFileMngr->ProcessAlbumList( aMedia, aArray );
    }

// ---------------------------------------------------------------------------
// CCmSmMain::GetColItemsL
// ---------------------------------------------------------------------------
//
void CCmSmMain::GetColItemsL( TCmMediaType /*aMedia*/, 
    CCmSqlPropertyContainer& /*aPropertys*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::GetColItemsL"));
    
    iService = ECmServiceGetFillFields;
    ChangeState( ECmSmFinishing );
    }
    
// ---------------------------------------------------------------------------
// CCmSmMain::SetAvController
// ---------------------------------------------------------------------------
//    
void CCmSmMain::SetAvController( MUPnPAVController* aAVController )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::SetAvController"));
    
    iCmSmTransferEngine->SetAvController(aAVController);
    }

// ---------------------------------------------------------------------------
// CCmSmMain::FileProcessingStatus
// ---------------------------------------------------------------------------
//    
void CCmSmMain::FileProcessingStatus( TCmSmFileProcessingStatus aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::FileProcessingStatus()"));
    
    CancelMdETimer();
    switch(aStatus)
        {
        case ECmSmProcessingFilesStarted:
            {
            if( iService == ECmServicePreProcessingStore )
                {
                ChangeState( ECmSmStartFileProcessing );
                }
            break;
            }
        case ECmSmProcessingFilesReady:
            {
            break;
            }
        case ECmSmProcessingCanceled:
            {           
            break;
            }
        case ECmSmNoStoreRulesSelected:
            {
            if( iService == ECmServiceAutomStoreListProcess )
                {
                // Do not complete message
                }
            else
                {
                ChangeState( ECmSmFinishing );
                }                                        
            break;
            }
        case ECmSmPreProcessingReady:
            {
            if( iService == ECmServiceAutomStoreListProcess )
                {
                // Do not complete message
                }
            else
                {
                iObserver->ServiceExecuted( iService, KErrNone );
                }
            iService = ECmServiceNone;                    
            break;
            }
        case ECmSmRefreshError:
            {
            if( iService == ECmServiceAutomStoreListProcess )
                {
                // Do not complete message
                }
            else
                {
                iObserver->ServiceExecuted( iService, 
                    KErrGeneral );
                }
            iService = ECmServiceNone;                                
            break;
            }
        case ECmSmAlbumProcessingReady:
            {
            if( iService == ECmServiceAutomStoreListProcess )
                {
                // Do not complete message
                }
            else
                {
                iObserver->ServiceExecuted( iService, KErrNone );
                }
            iService = ECmServiceNone;                                  
            break;
            }
        case ECmSmContentChanged:
            {
            // If timer start up fails => wait next content change event
            TRAPD( err, StartMdETimerL() );
            if( err )
                {
                TRACE(Print(_L("[STORE MNGR]\t StartMdETimerL err = %d"), 
                    err ));
                }           
            break;
            }
        default:
            {
            break;
            }    
        }
    
    }
    
// ---------------------------------------------------------------------------
// CCmSmMain::TransferStatus
// ---------------------------------------------------------------------------
//    
void CCmSmMain::TransferStatus( TCmSmTransferStatus aStatus )    
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::TransferStatus()"));
    switch( aStatus )
        {
        case ECmSmTransferCompleted:
            {
            TTime storeFinished;
            storeFinished.HomeTime();
            TTimeIntervalMicroSeconds usecsFrom = 
                storeFinished.MicroSecondsFrom(iStoreStarted);
            TInt64 timeinsecs((TInt64)( usecsFrom.Int64() / KCmSmOneSecond ));
            TRACE(Print(_L("[STORE MNGR]\t Store took = %ld seconds"), 
                timeinsecs ));                        
                
            iCmSmFileMngr->UpdateTransferHistoryData( timeinsecs, 
                iCmSmTransferEngine->DataAmount(), (TUint8)iId );            
            ChangeState( ECmSmManageFiles );
            break;
            }
        case ECmSmAborted:
            {
            ChangeState( ECmSmStartCopying );            
            break;
            }
        case ECmSmTransferCanceled:
            {           
            break;
            }
        case ECmSmFileTransferred:
            {
            SendProgressInfo( KCmOneFile );
            break;
            }
        case ECmSmFileTransferFailed:
            {
            // File transferring failed => skip file
            SendProgressInfo( KCmOneFile );
            break;
            }
        case ECmSmWlanLost:
            {
            if( iDevices )
                {
                iDevices->ResetAndDestroy();
                delete iDevices;
                iDevices = NULL;                
                }
            iItemIds.Reset();
            iItemIds.Close();
            iCmSmFileMngr->ResetArrays();    
            iObserver->ServiceExecuted( iService, KErrGeneral );
            iProgressInfo.iService = ECmServiceNone;
            iService = ECmServiceNone;
            SendProgressInfo( KErrNone );
            break;
            }
        case ECmSmTransferContinued:
            {
            ChangeState( ECmSmContinueCopying ); 
            break;
            }
        default:
            {
            break;
            }                
        }
    }

#ifdef _DEBUG 
// ---------------------------------------------------------------------------
// CCmSmMain::LogServers
// ---------------------------------------------------------------------------
//
void CCmSmMain::LogServers()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::LogServers"));
    
    for( TInt i = 0; i < iDevices->Count(); i++ )
        {
        TBuf<KMaxName>temp;
        if((*iDevices)[i])
            {
            if( &(*iDevices)[i]->Uuid() )
                {
                if( (*iDevices)[i]->Uuid().Length() < KMaxName )
                    {
                    temp.Copy( (*iDevices)[i]->Uuid() );
                    TRACE(Print(_L("[STORE MNGR]\t FOUND DEVICE %d = %S"), 
                        i+1, &temp ));        
                    }
                temp.Zero();                
                }            
            }        
        }            
    }
#endif

// ---------------------------------------------------------------------------
// CCmSmMain::SendProgressInfo
// ---------------------------------------------------------------------------
//
void CCmSmMain::SendProgressInfo( TInt aProgress ) 
    {
    TRACE( Print( _L(
        "[STORE MNGR]\t CCmSmMain::SendProgressInfo( %d )"), 
        aProgress));
    
    iProgressInfo.iProcessedItems += aProgress;
    TRAPD( error, iCmSmFileMngr->GetStoreFileCountNoDuplicatesL( 
        iProgressInfo.iItemsToTransferPerService ) );
    if ( error ) 
        {
        TRACE( Print( _L( "[STORE MNGR]\t GetStoreFileCountNoDuplicatesL \
            failed with error %d"), error ) );
        }
    TCmProgressInfoPckg progressPckg( iProgressInfo );
    
    TRACE( Print( _L(
        "[STORE MNGR]\t processed: %d total %d"), 
        iProgressInfo.iProcessedItems, iProgressInfo.iTotalItems));
    
    TInt err = RProperty::Set( KCmPropertyCat, KCmProperty, progressPckg );
    
    TRACE( Print( _L("[STORE MNGR]\t RProperty::Set returned %d"), err));
    }

// ---------------------------------------------------------------------------
// CCmSmMain::ManageFileL
// ---------------------------------------------------------------------------
//
void CCmSmMain::ManageFileL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::ManageFileL"));
    
    if( KErrNone < iCmSmTransferEngine->FilesToBeRemoved().Count() )
        {
        iMemManager->DeleteFilesL( iCmSmTransferEngine->FilesToBeRemoved() );
        }
    else if ( KErrNone < iCmSmTransferEngine->FilesToBeShrinked().Count() )
        {
        TRAPD( err, iMemManager->ShrinkImagesL( 
            iCmSmTransferEngine->FilesToBeShrinked() ) );
        if( err )
            {
            ChangeState( ECmSmStartCopying );
            }
        }
    else if( iCancelOnGoing )
        {
        ChangeState( ECmSmFinishCanceling );      
        }
    else
        {
        // Store to next server...        
        ChangeState( ECmSmStartCopying );  
        }    
    }

// ---------------------------------------------------------------------------
// CCmSmMain::DeleteCompleteL
// ---------------------------------------------------------------------------
//
void CCmSmMain::DeleteCompleteL( TInt /*aErr*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteCompleteL"));
    
    iCmSmTransferEngine->DeleteToBeRemoved();
    ManageFileL();
    }

// ---------------------------------------------------------------------------
// CCmSmMain::ShrinkCompleteL
// ---------------------------------------------------------------------------
//    
void CCmSmMain::ShrinkCompleteL( TInt /*aErr*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::ShrinkCompleteL"));
    
    iCmSmTransferEngine->DeleteToBeShrinked();
    ManageFileL();    
    }

// ---------------------------------------------------------------------------
// CCmSmMain::ChangeState
// ---------------------------------------------------------------------------
// 
void CCmSmMain::ChangeState( 
    TCmSmMainSequencer aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::ChangeState"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmSmManager, KErrInUse ));          
    SetActive();
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmSmMain::StartMdETimerL
// ---------------------------------------------------------------------------
// 
void CCmSmMain::StartMdETimerL()
    {
    LOG( _L("[STORE MNGR] CCmSmMain::StartMdETimerL start"));
    
    if( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    else
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );
        }
    iPeriodic->Start( KCmSmClfRefreshInterval, KCmSmClfRefreshInterval,
            TCallBack( CheckItems, this ) );
    LOG( _L("[STORE MNGR] CCmSmMain::StartMdETimerL end"));                    
    }

// ---------------------------------------------------------------------------
// CCmSmMain::CancelMdETimer
// ---------------------------------------------------------------------------
//     
void CCmSmMain::CancelMdETimer()
    {
    LOG( _L("[STORE MNGR] CCmSmMain::CancelMdETimer start"));
    
    if( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    LOG( _L("[STORE MNGR] CCmSmMain::CancelMdETimer end"));        
    } 
    
// -----------------------------------------------------------------------------
// CCmSmMain::CheckItems
// -----------------------------------------------------------------------------
//
TInt CCmSmMain::CheckItems( TAny* aPtr )
    {
    LOG( _L("[STORE MNGR] CCmSmMain::CheckItems"));
    
    return ( (CCmSmMain*)aPtr)->CheckItemsChanged();
    }
    
// -----------------------------------------------------------------------------
// CCmSmMain::CheckItemsChanged
// -----------------------------------------------------------------------------
//
TInt CCmSmMain::CheckItemsChanged( )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::CheckItemsChanged"));
    
    TInt restart( 1 );
    if( iService == ECmServiceNone )
        {
        LOG( _L("[STORE MNGR] Starting autom. store list process..."));
        iService = ECmServiceAutomStoreListProcess;
        iCmSmFileMngr->StartProcessing();
        }
    else
        {
        LOG( _L("[STORE MNGR] Store manager busy wait another x mins..."));
        restart = 0;
        }                        
    return restart;
    }
        
// ---------------------------------------------------------------------------
// CCmSmMain::DeleteDevices
// ---------------------------------------------------------------------------
// 
void CCmSmMain::DeleteDevices()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices"));
    
    if ( iDevices )
        {
        iDevices->ResetAndDestroy();
        delete iDevices;
        iDevices = NULL;
        }    
    }
    
// ---------------------------------------------------------------------------
// CCmSmMain::RunL
// ---------------------------------------------------------------------------
//    
void CCmSmMain::RunL()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmMain::RunL status = %d"),
                iStatus.Int() ));
                
    switch( iStatus.Int() )
        {
        case ECmSmGetMediaServers:
            {
            DeleteDevices();            
            TRAPD( err, iCmSmTransferEngine->GetMediaServersL( iDevices ) );
            TRACE(Print(_L("[STORE MNGR]\t GetMediaServersL err = %d"), 
                err ));            
            if( err || 0 == iDevices->Count() )
                {
                iObserver->ServiceExecuted( iService, KErrNotFound );
                iService = ECmServiceNone;
                }
            else
                {
#ifdef _DEBUG            
                LogServers();
#endif        
                iDeviceIndex = 0;
                // Load store list into memory
                iCmSmFileMngr->LoadStoreFileListL();
                iCmSmTransferEngine->ResetFileArrays();    
                ChangeState( ECmSmStartCopying );                 
                }                           
            break;
            }
        case ECmSmStartFileProcessing:
            { 
            if( iService == ECmServicePreProcessingStore )
                {
                iCmSmFileMngr->StartProcessing();
                }
            break;
            }
        case ECmSmStartCopying:
            {
            LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices \
                    start file copy"));

            iStoreStarted.HomeTime();    
            if( iDeviceIndex < iDevices->Count())
                {                    
                iCmSmFileMngr->GetItemsL( iItemIds, 
                    (*iDevices)[iDeviceIndex]->Uuid(), iId );
                if( KErrNone < iItemIds.Count() )
                    {
                    LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices \
                    files to be copied"));
                    iCmSmTransferEngine->CopyFilesL( (TUint8)iId, 
                        (*iDevices)[iDeviceIndex] );
                    }
                else
                    {
                    // Jump to next device
                    LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices \
                    undefined mediaserver - no items"));
                    ChangeState( ECmSmStartCopying );
                    }    
                iDeviceIndex++;
                }
            else
                {
                iCmSmFileMngr->DeleteItems(
                    iCmSmTransferEngine->ToBeDeleted() );
                iCmSmFileMngr->UpdateStoreFileListL();
                iDbManager->SetProgressInfo( iProgressInfo.iProcessedItems, 
                    iProgressInfo.iTotalItems );
                DeleteDevices();
                iItemIds.Reset();
                iItemIds.Close();
                iCmSmFileMngr->ResetArrays();    
                iObserver->ServiceExecuted( iService, KErrNone );
                iService = ECmServiceNone;
                }    
            break;
            }
        case ECmSmContinueCopying:
            {
            iCmSmTransferEngine->ContinueCopyFilesL();
            break;
            }
        case ECmSmFinishing:
            {
            LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices no \
            rules selected"));
            iObserver->ServiceExecuted( iService, 
                KErrNone );
            iService = ECmServiceNone;                            
            break;
            }
        case ECmSmManageFiles:
            {
            ManageFileL();
            break;
            }
        case ECmSmFinishCanceling:
            {
            iCancelOnGoing = EFalse;
            // Before canceling update content of the db    
            if( KErrNone < iItems.Count() )
                {
                iCmSmFileMngr->DeleteItems( 
                    iCmSmTransferEngine->ToBeDeleted() );
                iCmSmFileMngr->UpdateStoreFileListL();        
                }            
            if( IsActive() )
                {
                Cancel();
                }           
            break;
            }
        default:
            {
            LOG(_L("[STORE MNGR]\t CCmSmMain::DeleteDevices \
            no defined status"));
            break;
            }    
        }
    }
    
// ---------------------------------------------------------------------------
// CCmSmMain::Close
// ---------------------------------------------------------------------------
//      
void CCmSmMain::Close()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMain::Close"));
    
    delete this;
    }

// ---------------------------------------------------------------------------
// CCmSmMain::DoCancel
// ---------------------------------------------------------------------------
//    
void CCmSmMain::DoCancel()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmMain::DoCancel()")));
    }

// ---------------------------------------------------------------------------
// CCmSmMain::RunError
// ---------------------------------------------------------------------------
//   
TInt CCmSmMain::RunError( TInt aError )
    {
    TRACE(Print(_L("[STORE MNGR]\t RunError aError = %d"), aError ));
    return aError;
    }
    
// End of file



