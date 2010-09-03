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


#include <e32property.h>
#include <e32std.h>
#include <f32file.h>
#include <badesca.h>
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpavdevicelist.h"
#include "upnpitem.h"
#include "upnpcontainer.h"
#include <pathinfo.h>
#include <utf.h>
#include "cmserviceobserver.h"
#include "cmfmdownloadproxy.h"
#include "cmfmupnpmngr.h"
#include "cmfmfilldrivemngr.h"
#include "cmfmduplicatedetector.h"
#include "cmdriveinfo.h"
#include "cmfmfillrulefilleddatamngr.h"
#include "cmfmmpxnotifier.h"
#include "cmfmglxnotifier.h"
#include "cmfmcommon.h"
#include "cmdmmain.h"
#include "cmmmmain.h"
#include "cmfilllistitem.h"
#include "cmsqlpropertyitem.h"
#include "cmsqlpropertycontainer.h"
#include "cmfmmain.h"
#include "msdebug.h"

const TUint8 KExtensionLength = 4;
const TInt KCmFmOneFile = 1;
const TInt KCmFmLengthOfRoot = 3;
const TInt KCmFmOneSecond = 1000000;
const TInt KArrayGranularity = 10;
const TInt KExtraSpace = 5; // space reserved for backslashes etc. 
const TInt KDefaultCalculateSize = 100;

_LIT16( KCmKorp,               "<" );
_LIT16( KCmProk,               ">" );
_LIT16( KCmQuotation,          "\"" );
_LIT16( KCmDoubleBackSlash,    "\\" );
_LIT16( KCmSlash,              "/" );
_LIT16( KCmStar,               "*" );
_LIT16( KCmPipe,               "|" );
_LIT16( KCmColon,              ":");
_LIT16( KCmQuestionMark,       "?" );
_LIT16( KCmUnderScore,         "_" );

// ---------------------------------------------------------------------------
// CCmFmMain::NewL
// ---------------------------------------------------------------------------
//
CCmFmMain* CCmFmMain::NewL( MCmServiceObserver* aObserver,
    CMdESession& aSession, CCmDmMain* aDBManager, 
    CCmMmMain* aMemManager )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::NewL() start"));
    CCmFmMain* self = CCmFmMain::NewLC( aObserver, aSession,
        aDBManager, aMemManager );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmMain::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::NewLC
// ---------------------------------------------------------------------------
//
CCmFmMain* CCmFmMain::NewLC( MCmServiceObserver* aObserver,
    CMdESession& aSession, CCmDmMain* aDBManager, 
    CCmMmMain* aMemManager )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::NewLC() start"));
    CCmFmMain* self = new ( ELeave ) CCmFmMain( aObserver, 
        aMemManager, aDBManager );
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aDBManager );
    LOG(_L("[FILL MNGR]\t CCmFmMain::NewLC() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::~CCmFmMain
// ---------------------------------------------------------------------------
//
CCmFmMain::~CCmFmMain()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::~CCmFmMain() start"));
    if( IsActive() )
        {
        Cancel();
        }
    iFile.Close();        
    iFsSession.Close();
    delete iCmFmFillDriveMngr;
    delete iCmFmUpnpMngr;

    if( iDevices )
        {
        iDevices->ResetAndDestroy();
        delete iDevices;
        iDevices = NULL;
        }

    delete iCmFmFillRuleFilledDataMngr;
    delete iCmFmDownloadProxy;
    delete iCmFmDuplicateDetector;
    delete iCmFmGlxNotifier;
    delete iCmFmMpxNotifier;

    ResetDeleteQueue();
    ResetItems();
    iFullPath.Zero();
    iDrives.ResetAndDestroy();
    iDrives.Close();
    LOG(_L("[FILL MNGR]\t CCmFmMain::~CCmFmMain() end"));
    }

// ---------------------------------------------------------------------------
// CCmFmMain::CCmFmMain
// ---------------------------------------------------------------------------
//
CCmFmMain::CCmFmMain( MCmServiceObserver* aObserver,
    CCmMmMain* aMemManager, CCmDmMain* aDBManager )
    : CActive( EPriorityStandard ), iObserver( aObserver ), 
    iMemManager( aMemManager ), iDbManager( aDBManager )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::CCmFmMain()"));
        
    CActiveScheduler::Add( this );
    iProgressInfo.iService = ECmServiceNone;
    iProgressInfo.iProcessedItems = 0;
    iProgressInfo.iItemsToTransferPerService = 0;
    iProgressInfo.iTotalItems = 0;
    iService = ECmServiceFill;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ConstructL
// ---------------------------------------------------------------------------
//
void CCmFmMain::ConstructL( CMdESession& aSession, CCmDmMain* aDBManager )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ConstructL() start"));
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iFsSession.ShareProtected() );   
    iCmFmFillDriveMngr = CCmFmFillDriveMngr::NewL( iFsSession, iDrives );
    iCmFmUpnpMngr = CCmFmUpnpMngr::NewL( this );
    iCmFmFillRuleFilledDataMngr =
        CCmFmFillRuleFilledDataMngr::NewL( this, aDBManager, iItems );
    iCmFmDownloadProxy = CCmFmDownloadProxy::NewL( );
    iCmFmDuplicateDetector = CCmFmDuplicateDetector::NewL( *this, aSession );
    iCmFmGlxNotifier = CCmFmGlxNotifier::NewL( iFsSession );
    iCmFmMpxNotifier = CCmFmMpxNotifier::NewL( );
    LOG(_L("[FILL MNGR]\t CCmFmMain::ConstructL() end"));
    }

// ---------------------------------------------------------------------------
// CCmFmMain::StartFillL
// ---------------------------------------------------------------------------
//
void CCmFmMain::StartFillL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::StartFillL()"));
    
    iDbManager->GetProgressInfo( iProgressInfo.iProcessedItems,
        iProgressInfo.iTotalItems );
    iProgressInfo.iItemsToTransferPerService =
        iDbManager->FillFileCount( ECmToBeShrinked|ECmToBeFilled );
    iService = ECmServiceFill;
    iProgressInfo.iService = ECmServiceFill;
    iTempIndex = 0;
    SendProgressInfo( KErrNone );
    LoadDrivesL();
    CompleteRequest( ECmFmMMCCheckStarted );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::CancelOperation
// ---------------------------------------------------------------------------
//
void CCmFmMain::CancelOperation()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::CancelOperation()"));
    
    // Destroy all active objects
    iCmFmUpnpMngr->CancelOperation();
    iCmFmFillRuleFilledDataMngr->CancelOperation();
    iCmFmDownloadProxy->CancelOperation();
    LOG(_L("[FILL MNGR]\t Filling => Update items to database"));
    if( KErrNone < iItems.Count() )
        {
        iCmFmFillRuleFilledDataMngr->UpdateFillItems( ETrue );
        }
    if( KErrNone < iToBeDeleted.Count() )
        {
        iCmFmFillRuleFilledDataMngr->UpdateToBeDeleted( iToBeDeleted );
        }
    UpdateDrives();
    
    // File fetching was ongoing => have to notify gallery 
    // about deleted file    
    if( iFetchingFile && iIndex < iItems.Count() )
        {
        iFsSession.Delete( iFullPath );
        TRAP_IGNORE( iCmFmGlxNotifier->NotifyGalleryL( iFullPath ) );
        }

    // Update UI information
    TRAPD( err, iCmFmFillRuleFilledDataMngr->CalculateAvgTransferTimeL() );
    if( err )
        {
        TRACE(Print(_L("[FILL MNGR]\t CancelOperation error = %d"), err ));
        }
            
    iCmFmFillRuleFilledDataMngr->SendAvgTransferTime();
    iCmFmFillRuleFilledDataMngr->ResetData();
    Cancel();
    iObserver->ServiceExecuted( iService, KErrCancel );
    if( iProgressInfo.iService != ECmServiceNone )
        {
        // Service finished. Reset progress info.
        iProgressInfo.iService = ECmServiceNone;
        SendProgressInfo( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::PreProcessListsL
// ---------------------------------------------------------------------------
//
void CCmFmMain::PreProcessListsL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::PreProcessListsL()"));
    
    if( KErrNone == iDbManager->FillFileCount(
        ECmToBeShrinked|ECmToBeFilled|ECmToBeRemoved|ECmFilled ) )
        {
        // Set used fill space to zero
        InitializeDriveQuotasL();
        }
    iService = ECmServicePreProcessingFill;
  
    TRAPD( err, iCmFmFillRuleFilledDataMngr->LoadProfileIdsL() );    

    if( err )
        {
        iObserver->ServiceExecuted( iService, KErrNone );
        }

    SetMmcQuotaL();
    }

// ---------------------------------------------------------------------------
// CCmFmMain::PreProcessListL
// ---------------------------------------------------------------------------
//
void CCmFmMain::PreProcessListL( const TDesC8& aFillListName )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::PreProcessListL()"));
    
    if( KErrNone == iDbManager->FillFileCount(
        ECmToBeShrinked|ECmToBeFilled|ECmToBeRemoved|ECmFilled ) )
        {
        // Set used fill space to zero
        InitializeDriveQuotasL();
        }
    iService = ECmServicePreProcessingFilllist;
    TRAPD( err, 
           iCmFmFillRuleFilledDataMngr->PreProcessListL( aFillListName) );
    if( err )
        {
        TRACE(Print(_L("[FILL MNGR]\t PreProcessListL err = %d"), err )); 
        iObserver->ServiceExecuted( iService, KErrNone );
        }

    SetMmcQuotaL();

    }

// ---------------------------------------------------------------------------
// CCmFmMain::GetColItemsL
// ---------------------------------------------------------------------------
//
void CCmFmMain::GetColItemsL( TCmMetadataField /*aType*/,
    TCmMediaType /*aMedia*/, CDesCArray& /*aArray*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::GetColItemsL()"));
    
    iService = ECmServiceGetFillFields;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::GetMetadataItemsL
// ---------------------------------------------------------------------------
//
void CCmFmMain::GetMetadataItemsL(
                                CCmSqlPropertyCollector& aPropertys )

    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::GetMetadataItemsL()"));
    
    iService = ECmServiceGetFillFields;
    iCmFmFillRuleFilledDataMngr->GetMetadataItemsL( aPropertys );
    }
    
// ---------------------------------------------------------------------------
// CCmFmMain::GetColItemsL
// ---------------------------------------------------------------------------
//
void CCmFmMain::GetColItemsL( TCmMetadataField aType,
    TCmMediaType aMedia, CCmSqlPropertyContainer& aPropertys )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::GetColItemsL()"));
    
    iService = ECmServiceGetFillFields;
    iCmFmFillRuleFilledDataMngr->GetColItemsL( aType, aMedia, aPropertys );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::SetAvController
// ---------------------------------------------------------------------------
//
void CCmFmMain::SetAvController( MUPnPAVController* aAVController )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::SetAvController()"));
    
    iAvController = aAVController;
    iCmFmUpnpMngr->SetAvController( aAVController );
    iCmFmDownloadProxy->CancelOperation( );
    ResetItemInfo();
    iDrives.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCmFmMain::UpdatePriorities
// ---------------------------------------------------------------------------
//
void CCmFmMain::UpdatePriorities()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::UpdatePriorities()"));
    
    iCmFmFillRuleFilledDataMngr->UpdatePriorities();
    iService = ECmServiceUpdateFillLists;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::UpdateRefIds
// ---------------------------------------------------------------------------
//
void CCmFmMain::UpdateRefIds()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::UpdateRefIds()"));
    
    TRAPD( err, iCmFmFillRuleFilledDataMngr->DoUpdateReferenceIdsL() );
    if( err )
        {
        TRACE(Print(_L("[FILL MNGR]\t UpdateRefIds err = %d"), err ));
        }    
    }

// ---------------------------------------------------------------------------
// CCmFmMain::DeleteMetadataL
// ---------------------------------------------------------------------------
//
void CCmFmMain::DeleteMetadataL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::DeleteMetadataL()"));
    
    iCmFmFillRuleFilledDataMngr->DeleteMetadataL();
    iService = ECmServiceDeleteMetadata;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::SetMmcQuotaL
// ---------------------------------------------------------------------------
//
void CCmFmMain::SetMmcQuotaL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::SetMmcQuotaL()"));
    
    LoadDrivesL();
    TInt64 quota( KErrNone );
    for( TInt i = 0 ; i < iDrives.Count(); i++ )
        {
        if( quota < iDrives[i]->DriveQuota() )
            {
            quota = iDrives[i]->DriveQuota();
            }
        }
    iDrives.ResetAndDestroy();
    TRACE(Print(_L("[FILL MNGR]\t Max fill list size in bytes = %ld"),
        quota ));
    iCmFmFillRuleFilledDataMngr->SetMemoryCardQuota( quota );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::FillRuleProcessingStatus
// ---------------------------------------------------------------------------
//
void CCmFmMain::FillRuleProcessingStatus( TCmFmFillRuleProcessing aStatus )
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::\
        FillRuleProcessingStatus status = %d"), (TInt)aStatus ));
        
    switch( aStatus )
        {
        case EProcessingReady:
            {
            iObserver->ServiceExecuted( iService,
                KErrNone );
            break;
            }
        case ENoRulesSelected:    // fall through
        case ENoObjectsSelected:  // fall through
        case EColumnDataReady:    // fall through
        case EDoUpdatePriorities: // fall through
        case EMetadataDeleted:    
            {
            // Finish fill operation
            iObserver->ServiceExecuted( iService, KErrNone );
            break;
            }
        case EProcessingCanceled:
            {
            break;
            }
        case EProcessingFailed:
            {
            iObserver->ServiceExecuted( iService,
                KErrUnknown );
            break;
            }
        default:
            {
            LOG(_L("[FILL MNGR]\t Wrong status in Fill rule processing"));
            User::Invariant();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::TransferStatus
// ---------------------------------------------------------------------------
//    
void CCmFmMain::TransferStatus( TCmFmTransferStatus aStatus, TInt aSessionId,
    TInt aInternalItemId, TInt aError, TInt64 aFSize )
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::TransferStatus status = %d \
    error = %d"), (TInt)aStatus, (TInt)aError ));

    TInt err( KErrNone );    
    switch( aStatus )
        {
        case ECmFmFileFetchStarted: 
            {
            if( !aError )
                {
                TRACE(Print(_L("[FILL MNGR]\t File download started!"))); 
                iFetchingFile = ETrue;                
                }
            else if( KErrDisconnected == aError )
                {
                TransferStatus( ECmFmFileFetched, aSessionId, 0, 
                    aError, 0 );
                }
            else
                {
                ProceedWithNextItem( ECmFmCheckItem );                   
                }                
            break;
            }
        case ECmFmFileFetched:
            {
            iFetchingFile = EFalse; 
            iFile.Close();
            if( !aError )
                {
                TTime fetchFinished;                           
                fetchFinished.HomeTime();
                TTimeIntervalMicroSeconds usecsFrom =
                    fetchFinished.MicroSecondsFrom(iFileFetchStart);
                TRACE(Print(_L("[FILL MNGR]\t File Fetch \
                    took = %ld microseconds"), usecsFrom.Int64() ));

                // Update Fill quota
                TInt64 fillSpace( KErrNone );
                fillSpace = iDrives[iDriveIndex]->UsedDriveQuota() +
                    ( aFSize * KKilo );
                UpdateDriveQuota( iDriveNumber, fillSpace );

                TInt64 timeinsecs((TInt64)(usecsFrom.Int64()/KCmFmOneSecond));
                // Update transfer history information
                iCmFmFillRuleFilledDataMngr->UpdateTransferHistoryData(
                    timeinsecs, aFSize,
                    iItems[iIndex]->DevId() );

                // Update progress info
                SendProgressInfo( KCmFmOneFile );

                if( iItems[iIndex]->Status() == ECmToBeShrinked )
                    {
                    // Let's update file status
                    TRAP( err, SetStatusL( aInternalItemId ) );
                    TRACE(Print(_L("[FILL MNGR]\t SetStatusL err = %d"), err));
                    CompleteRequest( ECmFmShrinkImage );
                    }
                else
                    {
                    TRAP( err, ReplaceFileL( iItems[iIndex]->Path() ) );
                    TRACE(Print(_L("[FILL MNGR]\t ReplaceFileL err = %d"), 
                    err));
                    TRAP_IGNORE( iCmFmGlxNotifier->NotifyGalleryL( iFullPath ) ); 
                    if( iItems[iIndex]->MediaType() == ECmAudio )
                        {
                        TRAP( err, iCmFmMpxNotifier->NotifyMpxL( 
                            iFullPath, iCmFmUpnpMngr->UpnpItem() ) );
                        TRACE(Print(_L("[FILL MNGR]\t NotifyMpxL err = %d"), 
                        err));                        
                        }
                    else
                        {
                        TRACE(Print(_L("[FILL MNGR]\t Image or video file")));
                        }                                          
                    // Let's update file status to filled
                    TRAP( err, SetStatusL( aInternalItemId ) );
                    TRACE(Print(_L("[FILL MNGR]\t SetStatusL err = %d"), err));
                    // Jump to next file => Update item index
                    ProceedWithNextItem( ECmFmCheckItem );
                    }                
                }
            else if( KErrDisconnected == aError )
                {
                // Close used session: wlan connection lost or 
                // used media server disappeared
                if( iCmFmDownloadProxy->CloseDownloadSession( aSessionId ) )
                    {
                    // Continue if some sessions still available
                    if( !IsActive() )
                        {
                    ProceedWithNextItem( ECmFmCheckItem );
                        }
                    }
                else
                    {
                    // Update item information into db
                    if( KErrNone < iItems.Count() )
                        {
                        iCmFmFillRuleFilledDataMngr->UpdateFillItems();
                        }
                    if( KErrNone < iToBeDeleted.Count() )
                        {
                        iCmFmFillRuleFilledDataMngr->UpdateToBeDeleted( 
                            iToBeDeleted );
                        }
                    UpdateDrives();
                    // Finish filling if connection to all sessions lost
                    iObserver->ServiceExecuted( iService, KErrGeneral );
                    }                                     
                }
            else
                {
                ProceedWithNextItem( ECmFmCheckItem );                
                }                                                
            break;
            }
        default:
            {
            LOG(_L("[FILL MNGR]\t Wrong status in transfer"));
            User::Invariant();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::URICheckResult
// ---------------------------------------------------------------------------
//
void CCmFmMain::URICheckResult( TCmFmUriCheckResult aStatus, 
    const CUpnpItem* aItem, const CUpnpElement* aRes )
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::URICheckResult status = %d"),
        (TInt)aStatus ));
        
    switch( aStatus )
        {
        case ECanceled:
            {
            // Finish operation
            iIndex = iItems.Count();
            CompleteRequest( ECmFmCheckItem );
            break;
            }
        case ENoUriAvailable:
            {
            // No uri available => Jump to next item
            ProceedWithNextItem( ECmFmCheckItem );
            break;
            }
        case EUriChecked:
            {
            TRAPD( err, SetUriL( ) );
            TRACE(Print(_L("[FILL MNGR]\t SetUriL err = %d"), err ));
            TInt error( KErrNone );
            TRAP(  err, error = ParseFullPathL() );
            TRACE(Print(_L("[FILL MNGR]\t ParseFullPathL err=%d"), err ));
            if( KErrNone == error && 
                iItems[iIndex]->Uri().Length() != KErrNone )
                {
                iFullPath.Zero();
                TRAP( err, PrivatePathL( ) );
                TRACE(Print(_L("[FILL MNGR]\t PrivatePathL err=%d"), err ));
                error  = CreateRFile( iFullPath );
                TRACE(Print(_L("[FILL MNGR]\t CreateRFile error = %d"), 
                    error ));                
                if( !error )
                    {
                    iFileFetchStart.HomeTime();
                    TRAP( error, iCmFmDownloadProxy->FetchFileL( this, 
                        iAvController, iDevices, *aRes, *aItem, 
                        iFile, CheckServerIndex( *iUuid, *iDevices ), 
                        iItems[iIndex]->DbId() ) );
                    TRACE(Print(_L("[FILL MNGR]\t FetchFileL error = %d"), 
                        error));                    
                    }
                if( error )
                    {
                    TRACE(Print(_L("[FILL MNGR]\t Error = %d"), error ));
                    ProceedWithNextItem( ECmFmCheckItem );                    
                    }
                }
            else
                {
                ProceedWithNextItem( ECmFmCheckItem );
                }
            break;            
            }
        default:
            {
            LOG(_L("[FILL MNGR]\t Wrong URI check value"));
            User::Invariant();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::Duplicate
// ---------------------------------------------------------------------------
//
void CCmFmMain::DuplicateL( const TBool aDuplicate )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::Duplicate()"));
    
    if( !aDuplicate )
        {
        // Not duplicate
        CompleteRequest( ECmFmStartUriCheck );
        }
    else
        {
        TInt index( iIndex );
        // Duplicate => skip item and check next one
        ProceedWithNextItem( ECmFmCheckItem );
        
        if ( index < iItems.Count() )
			{
			// Update status of duplicate item to filled
			iItems[index]->SetStatus( ECmFilled );
			RPointerArray<CCmBaseListItem> items;
			CleanupClosePushL( items );
			items.AppendL( iItems[index] );
			iDbManager->UpdateStatusValues( items );
			items.Reset();
			CleanupStack::PopAndDestroy( &items );
			
			// Update transfer info
	        TInt count( KErrNone );
	        TInt time( KErrNone );
	        iDbManager->GetTransferInfo( ECmServiceFill, count, time );
	        iProgressInfo.iItemsToTransferPerService =
	            iDbManager->FillFileCount( ECmToBeShrinked|ECmToBeFilled );
	        iDbManager->UpdateTransferInfo( ECmServiceFill,
	            iProgressInfo.iItemsToTransferPerService, time );
			}
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmMain::DeleteCompleteL
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CCmFmMain::DeleteCompleteL( TInt aErr )
#else
void CCmFmMain::DeleteCompleteL( TInt /*aErr*/ )
#endif
    {
    TRACE(Print(_L("[FILL MNGR]\t DeleteCompleteL err = %d"), aErr ));
    //this function is called back by CmMmMain
    //after all array is be deleted or cancel
    for( TInt i = iToBeDeleted.Count() - 1; i >= 0; i-- )
        {
        if ( KErrNone == iToBeDeleted[i]->RefId() )
            {
            delete iToBeDeleted[i];
            iToBeDeleted.Remove( i );
            }
        }
    iToBeDeleted.Compress();
    CompleteRequest( ECmFmCheckItem );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ShrinkCompleteL
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CCmFmMain::ShrinkCompleteL( TInt aErr )
#else
void CCmFmMain::ShrinkCompleteL( TInt /*aErr*/ )
#endif
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::\
    ShrinkCompleteL err = %d"), aErr ));
    
    ReplaceFileL( iItems[iIndex]->Path() );
    iCmFmGlxNotifier->NotifyGalleryL( iFullPath );
    ProceedWithNextItem( ECmFmCheckItem );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::RunL
// ---------------------------------------------------------------------------
//
void CCmFmMain::RunL()
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::\
    RunL status = %d"), iStatus.Int() ));
    
    switch( iStatus.Int() )
        {
        case ECmFmMMCCheckStarted:
            {
            // Update updated drive array into database
            UpdateDrives();
            // Check first Drive Id
            CompleteRequest( iCmFmFillDriveMngr->CheckDriveId() );
            break;
            }
        case ECmFmWrongMMC:
            {
            // Error Wrong MMC
            iObserver->ServiceExecuted( iService, KErrNone );
            break;
            }
        case ECmFmMMCOk:
            {
            HandleIfMmcOkL();
            break;
            }
        case ECmFmMMCCheckFailed:
            {
            // Error MMC check failed
            iObserver->ServiceExecuted( iService, KErrNone );
            break;
            }
        case ECmFmStartUriCheck:
            {
            if( iIndex < iItems.Count() &&
                ECmFilled != iItems[iIndex]->Status() &&
                ECmLocalCopy != iItems[iIndex]->Status() &&
                KErrNone == iItems[iIndex]->RefId() &&
                ECmSelected == iItems[iIndex]->Selected() )
                {
                DoUriCheckL();
                }
            else
                {
                ProceedWithNextItem( ECmFmCheckItem );
                }
            break;
            }
        case ECmFmBrowseReady:
            {
            TRAP_IGNORE( SetUriL() );
            CompleteRequest( ECmFmUriOK );
            break;
            }
        case ECmFmCheckItem:
            {
            if( iIndex < iItems.Count() )
                {
                TBool checkneed( EFalse );
                if( iIndex < iItems.Count() &&
                    ECmFilled != iItems[iIndex]->Status() &&
                    ECmLocalCopy != iItems[iIndex]->Status() &&
                    KErrNone == iItems[iIndex]->RefId() &&
                    ECmSelected == iItems[iIndex]->Selected() )
                    {
                    TRAPD( err, 
                      iCmFmDuplicateDetector->CheckItemL( *iItems[iIndex] ) );
                    if( !err )
                        {
                        checkneed = ETrue;
                        }
                    }
                if( !checkneed )
                    {
                    // Error occurred => Jump to next item
                    ProceedWithNextItem( ECmFmCheckItem );
                    }                
                }
            else
                {
                StopItemCheck();
                iObserver->ServiceExecuted( iService, KErrNone );
                }                
            break;
            }            
        case ECmFmDeleteFiles:
            {
            HandleIfDeleteFilesL();
            break;
            }
        case ECmFmShrinkImage:
            {
            ResetDeleteQueue();
            iArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
            iArray->AppendL( iFullPath );
            // Let's ensure that fill manager is set as a observer
            iMemManager->SetObserver( *this );
            iMemManager->ShrinkImagesL( *iArray );
            break;
            }
        default:
            {
            // NOT HERE -> Panic
            LOG(_L("[FILL MNGR]\t RunL default"));
            User::Invariant();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::DoCancel
// ---------------------------------------------------------------------------
//
void CCmFmMain::DoCancel()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::DoCancel()"));
    }

// ---------------------------------------------------------------------------
// CCmFmMain::RunError
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CCmFmMain::RunError( TInt aError )
#else // _DEBUG
TInt CCmFmMain::RunError( TInt /*aError*/ )
#endif // _DEBUG
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::RunError = %d"), aError ));
    
    iObserver->ServiceExecuted( iService, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::HandleIfMmcOkL
// ---------------------------------------------------------------------------
//
void CCmFmMain::HandleIfMmcOkL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::HandleIfMmcOkL()"));
    
    iIndex = KErrNone;
    iCmFmFillRuleFilledDataMngr->GetFillItemsL();
    iCmFmFillRuleFilledDataMngr->GetToBeDeletedL(iToBeDeleted);
    if ( iDevices )
        {
        iDevices->ResetAndDestroy();
        delete iDevices;
        iDevices = NULL;
        }
    TRAPD( err, iCmFmUpnpMngr->GetMediaServersL( iDevices ) );
    TRACE(Print(_L("[FILL MNGR]\t GetMediaServersL err = %d"), err ));
    if( err )
        {
        iObserver->ServiceExecuted( iService, KErrNone );
        }
    else
        {
#ifdef _DEBUG
        LogServers();
#endif
        if( !iDevices->Count() )
            {
            TRACE(Print(_L("[FILL MNGR]\t No devices selected")));
            iObserver->ServiceExecuted( iService, KErrNotFound );
            ResetItems();
            }
        else
            {
            // If some of needed servers is available and there is items to
            // be transfered => Start fill
            if( NeededServerAvailableL() && 
                iProgressInfo.iItemsToTransferPerService )
                {
                CompleteRequest( ECmFmDeleteFiles );                
                }
            else
                {
                TRACE(Print(_L("[FILL MNGR]\t No needed server available")));
                TRACE(Print(_L("[FILL MNGR]\t Or no items to be transfered.")));
                TInt errorCode( KErrNone );
                if( 0 != iProgressInfo.iItemsToTransferPerService )
                    {
                    errorCode = KErrNotFound;
                    }
                iObserver->ServiceExecuted( iService , errorCode );
                ResetItems();
                }                
            }                
        }     
    }

// ---------------------------------------------------------------------------
// CCmFmMain::HandleIfDeleteFilesL
// ---------------------------------------------------------------------------
//
void CCmFmMain::HandleIfDeleteFilesL()
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMain::ECmFmDeleteFiles, iIndex = %d"),
                iIndex ));
    ResetDeleteQueue();
    AddFilesToDeleteQueueL();
    if( KErrNone != iArray->Count() )
        {
        iMemManager->SetObserver( *this );
        iMemManager->DeleteFilesL( *iArray );
        }
    else
        {
        iCmFmDuplicateDetector->ResetArray();
        CompleteRequest( ECmFmCheckItem );
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::CompleteRequest
// ---------------------------------------------------------------------------
//
void CCmFmMain::CompleteRequest(
    TCmFmStatus aStatus )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::CompleteRequest()"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmFmFillManager, KErrInUse ));
    SetActive();
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ProceedWithNextItem
// ---------------------------------------------------------------------------
//
void CCmFmMain::ProceedWithNextItem( TCmFmStatus aStatus )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ProceedWithNextItem()"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmFmFillManager, KErrInUse ));
    iIndex++;        
    SetActive();
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );    
    }
    
// ---------------------------------------------------------------------------
// CCmFmMain::DoUriCheckL
// ---------------------------------------------------------------------------
//
void CCmFmMain::DoUriCheckL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::DoUriCheckL()"));
    
    ResetItemInfo();
    TInt ret( KErrNone );
    if( iIndex < iItems.Count() )
        {
        ret = iCmFmFillRuleFilledDataMngr->GetUuidL( iUuid,
            iItems[iIndex]->DevId() );
        }
    else
        {
        ret = KErrNotFound;
        }
    // Check if file fits to mmc
    iDriveIndex = iCmFmFillDriveMngr->CheckFreeDiscSpace(
        iItems[iIndex]->Size(), iDriveId, iDriveNumber );
    iItems[iIndex]->SetDriveNumber( iDriveNumber );
    iItems[iIndex]->SetDriveId( iDriveId );
    if( ( KErrNone == ret ) && 
        ( KErrNone < iDevices->Count() ) && iDriveNumber != -1 )
        {
        TBool found(EFalse);
        TInt index(0);
        for( TInt i = 0; i < iDevices->Count() && !found &&
            iUuid ; i++ )
            {
            if( KErrNotFound !=  (((*iDevices)[i]->Uuid()).Match(*iUuid) ) )
                {
                found = ETrue;
                index = i;
                TRACE(Print(_L("[FILL MNGR]\t device found= %d"), index ));
                }
            }
        if( found )
            {
            TRAPD( error, iCmFmUpnpMngr->CheckURIL( (*iDevices)[index], 
                *iItems[ iIndex ]) );
            if( error )
                {
                TRACE(Print(_L("[FILL MNGR]\t Uri check failed...")));
                ProceedWithNextItem( ECmFmCheckItem );
                }
            }
        else
            {
            // Jump to next item
            TRACE(Print(_L("[FILL MNGR]\t Device not found")));
            ProceedWithNextItem( ECmFmCheckItem );
            }
        }
    else if( KErrNotFound  == ret )
        {
        // Jump to next item
        TRACE(Print(_L("[FILL MNGR]\t Media server not in db")));
        ProceedWithNextItem( ECmFmCheckItem );
        }
    else if( iDriveNumber == -1 && KErrNone < iToBeDeleted.Count() )
        {
        // Jump to next item
        TRACE(Print(_L("[FILL MNGR]\t FILE TOO BIG OR NO DRIVES SELECTED")));
        ProceedWithNextItem( ECmFmCheckItem );
        }
    else
        {
        ProceedWithNextItem( ECmFmCheckItem );
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ParseFullPathL
// ---------------------------------------------------------------------------
//
TInt CCmFmMain::ParseFullPathL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ParseFullPathL()"));
    
    iFullPath.Zero();
    TInt ret( KErrNone );
    if( iIndex < iItems.Count() )
        {
        HBufC* temp = ResolveAndAppendRootPathL( iDriveNumber );
        CleanupStack::PushL( temp );
        
        iFullPath.Append( *temp );
        
        CleanupStack::PopAndDestroy( temp );
        
        if( ECmAudio == iItems[iIndex]->MediaType() )
            {
            iFullPath.Append( PathInfo::SoundsPath() );
            }
        else if( ECmVideo == iItems[iIndex]->MediaType() )
            {
            iFullPath.Append( PathInfo::VideosPath() );
            }
        else if( ECmImage == iItems[iIndex]->MediaType() )
            {
            iFullPath.Append( PathInfo::ImagesPath() );
            }
        else
            {
            // Not supported upnp class use OthersPath
            iFullPath.Append( PathInfo::OthersPath() );
            }
        ret = ParseFileNameL();
        }
    else
        {
        ret = KErrNotFound;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ParseFileNameL
// ---------------------------------------------------------------------------
//
TInt CCmFmMain::ParseFileNameL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ParseFileNameL()"));
    
    TInt index(0);
    TInt ret( KErrNone );
    if( KErrNone != iItems[iIndex]->Uri().Length() )
        {
		const TChar slash( '/' );
        index = iItems[iIndex]->Uri().LocateReverse( slash );
        if( KErrNotFound < index && KErrNone !=
            iItems[iIndex]->PrimaryText().Length() )
            {
            HBufC* data16bitBuf = RemoveIllegalFilenameCharactersL(
                iItems[iIndex]->PrimaryText() );
            TBuf<KExtensionLength>fileExtension;
            fileExtension.Zero();
            TInt conversionError = CnvUtfConverter::
                ConvertToUnicodeFromUtf8(fileExtension,
                    iItems[iIndex]->Uri().Mid(
                    (iItems[iIndex]->Uri().Length() -
                    KExtensionLength), KExtensionLength) );

            if( conversionError )
                {
                ret = KErrGeneral;
                }
            else if ( ( iFullPath.Length() +
                    data16bitBuf->Length() + fileExtension.Length() ) <
                    KMaxFileName )
                {
                iFullPath.Append(*data16bitBuf);
                iFullPath.Append(fileExtension);
                iItems[iIndex]->SetPathL(
                    iFullPath.Mid( KCmFmLengthOfRoot ) );
                iItems[iIndex]->SetDriveNumber( iDriveNumber );
                iItems[iIndex]->SetDriveId( iDriveId );
                iFullPath.Zero();     
                }
            else
                {
                iFullPath.Append( data16bitBuf->Left(
                	KMaxFileName - iFullPath.Length() -
                	fileExtension.Length() - KExtraSpace ) );

                iFullPath.Append(fileExtension);
                iItems[iIndex]->SetPathL(
                    iFullPath.Mid( KCmFmLengthOfRoot ) );
                iItems[iIndex]->SetDriveNumber( iDriveNumber );
                iItems[iIndex]->SetDriveId( iDriveId );
                iFullPath.Zero();    
                }
            delete data16bitBuf;
            data16bitBuf = NULL;
            }
        }
    else
        {
        LOG(_L("[FILL MNGR]\t File name parsing, compose failed)"));
        ret = KErrGeneral;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::SetUriL
// ---------------------------------------------------------------------------
//
void CCmFmMain::SetUriL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::SetUriL()"));
    
    if( iIndex < iItems.Count() )
        {
        iItems[iIndex]->SetUriL( iCmFmUpnpMngr->Uri() );
        }
    LOG(_L("[FILL MNGR]\t SetUriL end"));
    }

// ---------------------------------------------------------------------------
// CCmFmMain::LogServers
// ---------------------------------------------------------------------------
//
void CCmFmMain::LogServers()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::LogServers()"));
    
    for( TInt i = 0; i < iDevices->Count(); i++ )
        {
        TBuf<KMaxName>temp;
        if((*iDevices)[i])
            {
            if( &(*iDevices)[i]->Uuid())
                {
                if( (*iDevices)[i]->Uuid().Length() < KMaxName )
                    {
                    temp.Copy( (*iDevices)[i]->Uuid() );
                    TRACE(Print(_L("[FILL MNGR]\t FOUND DEVICE %d = %S"),
                        i+1, &temp ));
                    }
                temp.Zero();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ResetItemInfo
// ---------------------------------------------------------------------------
//
void CCmFmMain::ResetItemInfo()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ResetItemInfo()"));
    
    if( iUuid )
        {
        delete iUuid;
        iUuid = NULL;
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmMain::SendProgressInfo
// ---------------------------------------------------------------------------
//
void CCmFmMain::SendProgressInfo( TInt aProgress )
    {
    if ( iProgressInfo.iTotalItems > iProgressInfo.iProcessedItems )
        {
        iProgressInfo.iProcessedItems += aProgress;
        iProgressInfo.iItemsToTransferPerService -= aProgress;
        TCmProgressInfoPckg progressPckg( iProgressInfo );
        TRACE( Print( _L("[FILL MNGR]\t processed: %d total %d"),
                iProgressInfo.iProcessedItems, iProgressInfo.iTotalItems) );
       
        if ( ( iProgressInfo.iTotalItems <= iProgressInfo.iProcessedItems ) ||
             ( iProgressInfo.iTotalItems > iProgressInfo.iProcessedItems &&
               iProgressInfo.iProcessedItems > 0 &&
               ( iProgressInfo.iProcessedItems - iTempIndex ) >=
               iProgressInfo.iTotalItems / KDefaultCalculateSize )
           )
            {
            iTempIndex = iProgressInfo.iProcessedItems;
            TInt err = RProperty::Set( KCmPropertyCat, KCmProperty, 
                                  progressPckg );
            TRACE( Print( _L("[FILL MNGR]\t RProperty::Set returned %d"), 
                  err)); 
            }
        else
            {
            LOG(_L("[FILL MNGR]\t progress small no need to pub&sub"));
            }
        }

    else
        {
        iProgressInfo.iProcessedItems += aProgress;
        LOG(_L("[FILL MNGR]\t Bigger than 100%% or not beginning"));
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::SetStatusL
// ---------------------------------------------------------------------------
//
void CCmFmMain::SetStatusL( const TInt64 aInternalItemId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::SetStatusL()"));
    
    if( iIndex < iItems.Count() )
        {
        iItems[iIndex]->SetStatus( ECmFilled );
        TUint id(iItems[iIndex]->DbId());
        for( TInt i = 0; i < iItems.Count(); i++ )
            {
            // Set all reference items also filled
            if( aInternalItemId == iItems[i]->RefId() &&
                iItems[i]->Selected() == ECmSelected )
                {
                iItems[i]->SetStatus( ECmFilled );
                iItems[i]->SetPathL( iItems[iIndex]->Path() );
                iItems[i]->SetDriveId( iItems[iIndex]->DriveId() );
                iItems[i]->SetDriveNumber( iItems[iIndex]->DriveNumber() );
                }
            }
        // add to duplicatied detector
        iCmFmDuplicateDetector->AppendToArrayL( *iItems[iIndex] );
        // Update statuses into db
        iDbManager->UpdateFillTransferStatus( ECmFilled, iItems[iIndex] );
        TInt count( KErrNone );
        TInt time( KErrNone );
        iDbManager->GetTransferInfo( ECmServiceFill, count, time );
        
        iDbManager->UpdateTransferInfo( ECmServiceFill,
            iProgressInfo.iItemsToTransferPerService, time );
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::RemoveIllegalFilenameCharactersL
// ---------------------------------------------------------------------------
//
HBufC* CCmFmMain::RemoveIllegalFilenameCharactersL( const TDesC& aFileName )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::RemoveIllegalFilenameCharactersL()"));
    
    HBufC* buf = aFileName.AllocLC();
    TPtr ptr = buf->Des();
    ReplaceCharsL( ptr, KCmProk(), KCmUnderScore() );        // >
    ReplaceCharsL( ptr, KCmKorp(), KCmUnderScore() );        // <
    ReplaceCharsL( ptr, KCmQuotation(), KCmUnderScore() );   // "
    // \ character
    ReplaceCharsL( ptr, KCmDoubleBackSlash(), KCmUnderScore() );
    ReplaceCharsL( ptr, KCmSlash(), KCmUnderScore() );       // /
    ReplaceCharsL( ptr, KCmStar(), KCmUnderScore() );        // *
    ReplaceCharsL( ptr, KCmPipe(), KCmUnderScore() );        // |
    ReplaceCharsL( ptr, KCmColon(), KCmUnderScore() );       // :
    ReplaceCharsL( ptr, KCmQuestionMark(), KCmUnderScore() );// question mark
    CleanupStack::Pop(); // Can't pop by name, name invalid now
    return buf;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ReplaceCharsL
// ---------------------------------------------------------------------------
//
void CCmFmMain::ReplaceCharsL( TDes& aString, const TDesC& aTrg,
        const TDesC& aReplacement )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ReplaceCharsL()"));
    
    __ASSERT_DEBUG( aTrg != aReplacement, User::Panic( _L( "CmFm" ), 1) );
    TPtrC ptr( aString );
    TInt position( 0 );

    TBool continueLoop = ETrue;
    while ( continueLoop )
        {
        TInt found = ptr.Find( aTrg );
        if( found < 0 )
            {
            continueLoop = EFalse;
            }
        else 
            {
            aString.Replace( position + found, aTrg.Length(), aReplacement );
            if( position + found + aReplacement.Length() < aString.Length() )
                {
                position += found + aReplacement.Length();
                ptr.Set( aString.Mid( position ) );
                }
            else
                {
                continueLoop = EFalse;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::LoadDrivesL()
// ---------------------------------------------------------------------------
//
void CCmFmMain::LoadDrivesL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::LoadDrivesL()"));
    
    iDrives.ResetAndDestroy();
    iDbManager->PrepareQueryCmdL( EDrivesQuery );
    iDbManager->QueryDrivesL( iDrives );
    }

// ---------------------------------------------------------------------------
// CCmFmMain::UpdateDrives()
// ---------------------------------------------------------------------------
//
void CCmFmMain::UpdateDrives()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::UpdateDrives()"));
    if ( 0 != iDrives.Count() )
        {
        #ifdef _DEBUG
            TRAPD( err, iDbManager->SetDrivesL( iDrives ) );
            TRACE(Print(_L("[FILL MNGR]\t drives update err = %d"),
                 err ));
        #else // _DEBUG
            TRAP_IGNORE( iDbManager->SetDrivesL( iDrives ) );
        #endif // _DEBUG
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ResolveAndAppendRootPath()
// ---------------------------------------------------------------------------
//
HBufC* CCmFmMain::ResolveAndAppendRootPathL( const TInt aDriveNumber )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ResolveAndAppendRootPathL()"));
    
    HBufC* buf = HBufC::NewLC( KExtensionLength );
    TChar driveChar;
    iFsSession.DriveToChar( aDriveNumber, driveChar );
    buf->Des().Append( driveChar );
    buf->Des().Append( KCmColon );
    buf->Des().Append( KCmDoubleBackSlash );
    CleanupStack::Pop();
    return buf;
    }

// ---------------------------------------------------------------------------
// CCmFmMain::UpdateDriveQuota()
// ---------------------------------------------------------------------------
//
void CCmFmMain::UpdateDriveQuota( const TInt aDriveNumber,
    const TInt64 aQuota )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::UpdateDriveQuota()"));
    
    for( TInt i = 0 ; i < iDrives.Count(); i++ )
        {
        if( iDrives[i]->DriveNumber() == aDriveNumber )
            {
            iDrives[i]->SetUsedDriveQuota( aQuota );
            i =  iDrives.Count();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::InitializeDriveQuotasL()
// ---------------------------------------------------------------------------
//
void CCmFmMain::InitializeDriveQuotasL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::InitializeDriveQuotasL()"));
    
    LoadDrivesL();
    for( TInt i = 0 ; i < iDrives.Count(); i++ )
        {
        iDrives[i]->SetUsedDriveQuota( 0 );
        }
    iDrives.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCmFmMain::GetUsedQuota()
// ---------------------------------------------------------------------------
//
TInt64 CCmFmMain::GetUsedQuota( const TInt aDriveNumber )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::GetUsedQuota()"));
    
    TInt64 quota( 0 );
    for( TInt i = 0; i < iDrives.Count(); i++ )
        {
        if( iDrives[i]->DriveNumber() == aDriveNumber )
            {
            quota = iDrives[i]->UsedDriveQuota();
            }
        }
    return quota;
    }

// -----------------------------------------------------------------------------
// CCmFmMain::CreateRFile()
// -----------------------------------------------------------------------------
// 
TInt CCmFmMain::CreateRFile( const TDesC& aFilePath )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::CreateRFile()"));
    
    iFile.Close();
    TRACE(Print(_L("[FILL MNGR]\t Creating file : %S"), &aFilePath ));
    TInt error(iFile.Create( iFsSession, aFilePath, EFileWrite ));
    if( KErrAlreadyExists == error )
        {
        TRACE(Print(_L("[FILL MNGR]\t Allready exists => delete old file") ));
        error = iFsSession.Delete( aFilePath );
        error = iFile.Create( iFsSession, aFilePath, EFileWrite );
        }
    TRACE(Print(_L("[FILL MNGR]\t CreateRFile err: %d"), error ));
    return error; 
    }

// ---------------------------------------------------------------------------
// CCmFmMain::PrivatePathL
// ---------------------------------------------------------------------------
//   
void CCmFmMain::PrivatePathL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::PrivatePathL()"));
    
    iFullPath.Zero();
    HBufC* temp = ResolveAndAppendRootPathL( iItems[ iIndex ]->DriveNumber() );
    CleanupStack::PushL( temp );
    iFullPath.Append( *temp );
    CleanupStack::PopAndDestroy( temp );
    
    TFileName privatePath;
    TInt err = iFsSession.PrivatePath( privatePath );
    if ( !err )
        {
        iFullPath.Append( privatePath.Mid( 1 ) );
        
        // Now the path contains everything but filename and extension
        // => check that the directory exists. If not, it will be created.
        // Possible error is ignored at the moment 
        // (normal case is KErrAlreadyExists)
        err = iFsSession.MkDirAll( iFullPath );
        
        iFullPath.Append( KCmFmTempFile() );
        }
    LOG(_L("[FILL MNGR]\t CCmFmMain::PrivatePath() end"));        
    }  

// ---------------------------------------------------------------------------
// CCmFmMain::ReplaceFileL
// ---------------------------------------------------------------------------
//  
void CCmFmMain::ReplaceFileL( const TDesC& aOriginal )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ReplaceFileL()"));
    
    HBufC* temp = ResolveAndAppendRootPathL(
                            iItems[ iIndex ]->DriveNumber() );
//    CleanupStack::PushL( temp );                            
    temp = temp->ReAllocL( temp->Length() + aOriginal.Length() );
    temp->Des().Append( aOriginal );
    
    TInt error = iFsSession.Replace( iFullPath, *temp );
    if( error == KErrPathNotFound )
        {
        error = iFsSession.MkDirAll( *temp );
        TRACE(Print(_L("[FILL MNGR]\t MkDir err: %d"), error ));
        
        error = iFsSession.Replace( iFullPath, *temp );
        TRACE(Print(_L("[FILL MNGR]\t MkDir err: %d"), error ));
        }
    iFullPath.Copy( *temp );
    delete temp;
//    CleanupStack::PopAndDestroy( temp );
    
    TRACE(Print(_L("[FILL MNGR]\t File Replace done err: %d"), error ));
    }

// ---------------------------------------------------------------------------
// CCmFmMain::CheckServerIndex
// ---------------------------------------------------------------------------
// 
TInt CCmFmMain::CheckServerIndex( const TDesC8& aUuid, 
    const CUpnpAVDeviceList& aList )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::CheckServerIndex()"));
    
    TInt index( KErrNotFound ); 
    for( TInt i = 0; i < aList.Count(); i++ )
        {
        if( KErrNotFound != aList[i]->Uuid().Match( aUuid ) )
            {
            index = i;
            i = aList.Count();
            }
        }
    return index;        
    }

// ---------------------------------------------------------------------------
// CCmFmMain::NeededServerAvailableL
// ---------------------------------------------------------------------------
// 
TBool CCmFmMain::NeededServerAvailableL( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::NeededServerAvailableL()"));
    
    TBool available( EFalse );
    TInt index( 0 );
    for( TInt i = 0 ; i < iDevices->Count(); i++ )
        {
        index = 
            iCmFmFillRuleFilledDataMngr->GetMediaServerIdL( 
                (*iDevices)[i]->Uuid() );
        if( index != KErrNotFound )
            {
            available = ETrue;
            i = iDevices->Count();
            LOG(_L("[FILL MNGR]\t Server available.")); 
            }
        }
    return available;               
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ResetItems
// ---------------------------------------------------------------------------
// 
void CCmFmMain::ResetItems()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ResetItems()"));
    
    iItems.ResetAndDestroy();
    iItems.Close();
    iToBeDeleted.ResetAndDestroy();
    iToBeDeleted.Close();     
    }

// ---------------------------------------------------------------------------
// CCmFmMain::ResetDeleteQueue
// ---------------------------------------------------------------------------
//  
void CCmFmMain::ResetDeleteQueue( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::ResetDeleteQueue()"));
    
    if( iArray )
        {
        iArray->Reset();
        delete iArray;
        iArray = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// CCmFmMain::AddFilesToDeleteQueueL
// ---------------------------------------------------------------------------
// 
void CCmFmMain::AddFilesToDeleteQueueL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::AddFilesToDeleteQueueL()"));
    
    iArray = new ( ELeave ) CDesCArrayFlat( 10 );
    HBufC* temp = NULL;
    TFileName path;
    TInt64 totalSize( KErrNone );
    TBool drivechange( EFalse );
    for( TInt i = 0; i < iToBeDeleted.Count(); i++ )
        {
        if( KErrNone == iToBeDeleted[i]->RefId() )
            {
            drivechange = ETrue;
            temp = ResolveAndAppendRootPathL(
                iToBeDeleted[i]->DriveNumber() );
            path.Append( *temp );
            path.Append( iToBeDeleted[i]->Path() );
            iArray->AppendL( path );
            delete temp;
            path.Zero();
            
            totalSize = GetUsedQuota(
                        iToBeDeleted[i]->DriveNumber() );
            totalSize -= iToBeDeleted[i]->Size();
            UpdateDriveQuota( iToBeDeleted[i]->DriveNumber(),
                              totalSize );
            }
        }
    
    if( drivechange )
        {
        UpdateDrives();
        }
    }

// ---------------------------------------------------------------------------
// CCmFmMain::StopItemCheck
// ---------------------------------------------------------------------------
//
void CCmFmMain::StopItemCheck()
    {
    if( KErrNone < iItems.Count() )
    	{
    	iCmFmFillRuleFilledDataMngr->UpdateFillItems();
    	}
    if( KErrNone < iToBeDeleted.Count() )
    	{
    	iCmFmFillRuleFilledDataMngr->UpdateToBeDeleted(
    			iToBeDeleted );
    	}
    iCmFmDownloadProxy->CancelOperation();
    iFullPath.Zero();
    if( iDevices )
    	{
    	iDevices->ResetAndDestroy();
    	}
    delete iDevices;
    iDevices = NULL;
    UpdateDrives();
    iDrives.ResetAndDestroy();
    iToBeDeleted.ResetAndDestroy();
    ResetDeleteQueue();
    }

// ---------------------------------------------------------------------------
// CCmFmMain::Close
// ---------------------------------------------------------------------------
//    
void CCmFmMain::Close()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMain::Close()"));
    
    delete this;
    }

// ---------------------------------------------------------------------------
// CompareById
// For sort. Sort is based on db id
// ---------------------------------------------------------------------------
// 
TInt CCmFmMain::CompareById( const CCmFillListItem& aItem1,
                             const CCmFillListItem& aItem2 )
    {
    TInt result = 0;

    if ( aItem1.DbId() < aItem2.DbId() )
        {
        result = -1;
        }
    else if ( aItem1.DbId() > aItem2.DbId() )
        {
        result = 1;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CompareBySize
// For sort. Sort is based on item size
// ---------------------------------------------------------------------------
//
TInt CCmFmMain::CompareBySize( const CCmFillListItem& aItem1,
                                               const CCmFillListItem& aItem2 )
    {
    TInt result = 0;

    if ( aItem1.Size() < aItem2.Size() )
        {
        result = -1;
        }
    else if ( aItem1.Size() > aItem2.Size() )
        {
        result = 1;
        }

    return result;
    }

// End of file

