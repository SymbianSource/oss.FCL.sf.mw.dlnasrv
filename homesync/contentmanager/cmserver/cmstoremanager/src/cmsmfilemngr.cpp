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
* Description:  Store file processing
*
*/


#include <e32property.h>
#include "cmdmmain.h"
#include "cmstorerule.h"
#include "cmstorelistitem.h"
#include "cmfilllistitem.h"
#include "cmsmfileprocessingobserver.h"
#include "cmsmcontentchangeobserver.h"
#include "cmsmvideoandimagemngr.h"
#include "cmsmclfmngr.h"
#include "cmsmiteminfo.h"
#include "cmsmmsinfo.h"
#include "cmsmfilemngr.h"
#include "msdebug.h"

// CONSTANTS
const TInt KCmSmIniStoreTransferSpeed = 200; // 200 kBs
const TInt KCmSmIniStoreTransferTime = 1; // One second


// ---------------------------------------------------------------------------
// CCmSmFileMngr::NewL
// ---------------------------------------------------------------------------
// 
CCmSmFileMngr* CCmSmFileMngr::NewL(
    MCmSmFileProcessingObserver* aObserver, CMdESession& aSession, 
    CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::NewL() start"));    
    CCmSmFileMngr* self = CCmSmFileMngr::NewLC(
        aObserver, aSession, aDBMngr, aItems );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmFileMngr* CCmSmFileMngr::NewLC( 
    MCmSmFileProcessingObserver* aObserver, CMdESession& aSession, 
    CCmDmMain* aDBMngr,
    RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::NewLC() start"));     
    CCmSmFileMngr* self = new ( ELeave ) CCmSmFileMngr(
        aObserver, aDBMngr, aItems );
    CleanupStack::PushL( self );
    self->ConstructL( aSession );
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmFileMngr::~CCmSmFileMngr
// ---------------------------------------------------------------------------
// 
CCmSmFileMngr::~CCmSmFileMngr()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::\
        ~CCmSmFileMngr() start"));
    Cancel();    
    
    delete iVideoAndImageMngr;
    delete iClfMusicMngr;
    iRuleArray.ResetAndDestroy();
    iRuleArray.Close();
    
    iFilledItems.ResetAndDestroy();
    iFilledItems.Close();
        
    iStoredArray.ResetAndDestroy();
    iStoredArray.Close();
    
    iMsIds.ResetAndDestroy();
    iMsIds.Close();
    
    iMediaTypes.Reset();
    iMediaTypes.Close();
    
    delete iCcObserver;
         
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::\
        ~CCmSmFileMngr() end"));            
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::CCmSmFileMngr
// ---------------------------------------------------------------------------
//            
CCmSmFileMngr::CCmSmFileMngr( 
    MCmSmFileProcessingObserver* aObserver, CCmDmMain* aDBMngr,
    RPointerArray<CCmStoreListItem>& aItems )
    :CActive( EPriorityStandard ),
    iObserver( aObserver ), iDBManager( aDBMngr ), iItems( aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::CCmSmFileMngr()"));
    
    CActiveScheduler::Add( this );
    iTransferInfo.iTotalItems = KErrNone;
    iTransferInfo.iProcessedItems = KErrNone;    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmFileMngr::ConstructL( CMdESession& aSession )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ConstructL()"));
    iVideoAndImageMngr = 
        CCmSmVideoAndImageMngr::NewL( *this, *this, aSession, iItems );
    iClfMusicMngr = CCmSmClfMngr::NewL( *this, *this, iItems );
    iCcObserver = 
            CCmSmContentChangeObserver::NewL( aSession, *iObserver );        
    iProcessingStatus = ECmSmNone;
    }    

// ---------------------------------------------------------------------------
// CCmSmFileMngr::StartProcessing
// ---------------------------------------------------------------------------
// 
void CCmSmFileMngr::StartProcessing()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::StartProcessing()"));
    
    iTransferInfo.iTotalItems = KErrNone;
    iTransferInfo.iProcessedItems = KErrNone;
    if( !iCcObserver->IsStarted() )
        {
        TRAPD( err, iCcObserver->StartObserversL() );
        if( err )
            {
            TRACE(Print( _L("[STORE MNGR] StartObserversL err = %d"),
                err ) );
            }
        }    
    CompleteRequest( ECmSmPreProcessingStarted );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ProcessAlbumList
// ---------------------------------------------------------------------------
// 
void CCmSmFileMngr::ProcessAlbumList( TCmMediaType /*aMedia*/, 
    CDesCArray& /*aArray*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ProcessAlbumList()"));
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::StoreItemCount
// ---------------------------------------------------------------------------
//        
TInt CCmSmFileMngr::StoreItemCount()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::StoreItemCount()"));
    
    return iDBManager->StoreFileCount( 
        ECmToBeRemoved|ECmToBeShrinked|ECmKeepOnDevice );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::FillItemCount
// ---------------------------------------------------------------------------
//
TInt CCmSmFileMngr::FillItemCount()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::FillItemCount()"));
    
    return iDBManager->FillFileCount( ECmToBeShrinked|ECmToBeFilled );
    }
    
// ---------------------------------------------------------------------------
// CCmSmFileMngr::GetStoreFileCountNoDuplicatesL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::GetStoreFileCountNoDuplicatesL( TInt& aCount )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::\
    GetStoreFileCountNoDuplicatesL()"));
    
    iDBManager->GetStoreFileCountNoDuplicatesL( aCount, 
        ECmToBeRemoved|ECmToBeShrinked|ECmKeepOnDevice );
    TRACE(Print( _L("[STORE MNGR] GetStoreFileCountNoDuplicatesL count = %d"),
        aCount ) );        
    TInt count = 0;
    TInt time = 0;
    iDBManager->GetTransferInfo( ECmServiceStore, count, time );
    iDBManager->UpdateTransferInfo( ECmServiceStore, aCount, 
        time );    
    }
        
// ---------------------------------------------------------------------------
// CCmSmFileMngr::UpdateTransferHistoryData
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::UpdateTransferHistoryData(
    TInt64 aInterval, TInt64 aDataAmount, 
    TUint8 aServerId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::UpdateTransferHistoryData()"));
    
    iDBManager->UpdateUploadHistory( aServerId,
        aDataAmount, aInterval );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::CancelOperation
// ---------------------------------------------------------------------------
//    
void CCmSmFileMngr::CancelOperation()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::CancelOperation()"));
    
    Cancel();
    iVideoAndImageMngr->CancelOperation();
    iClfMusicMngr->CancelOperation();
    iObserver->FileProcessingStatus( ECmSmProcessingCanceled );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ImagesAndVideosReady
// ---------------------------------------------------------------------------
// 
void CCmSmFileMngr::ImagesAndVideosReady( const TInt /*aStatus*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ImagesAndVideosReady()"));
    
    CompleteRequest( ECmSmMediaTypesSelected );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::MusicReady
// ---------------------------------------------------------------------------
// 
void CCmSmFileMngr::MusicReady( const TInt /*aStatus*/ )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::MusicReady()"));
    
    CompleteRequest( ECmSmMediaTypesSelected );
    }
            
// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadSelectedStoreRulesL
// ---------------------------------------------------------------------------
//        
void CCmSmFileMngr::LoadSelectedStoreRulesL()
    {
    LOG(_L("[FILL MNGR]\t CCmSmFileMngr::LoadSelectedStoreRulesL()"));
    
    RPointerArray<HBufC8> array;
    CleanupClosePushL( array );
    iDBManager->PrepareQueryCmdL( ESelectedStoreRuleQuery );    
    iDBManager->QuerySelectedStoreRuleNamesL( array, 1 );
    // Load selected store rules to array ( using names as a qyery 
    // parameter )
    for( TInt i = 0; i < array.Count(); i++ ) 
        {
        CCmStoreRule* storeRule = CCmStoreRule::NewLC();    
        iDBManager->PrepareQueryCmdL(EStoreRuleQuery);
        storeRule->SetNameL( *array[i] );        
        iDBManager->QueryStoreRuleL( storeRule );
        iRuleArray.AppendL( storeRule );
        CleanupStack::Pop(storeRule);
        }
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ProcessStoreRules
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::ProcessStoreRules()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ProcessStoreRules()"));    
    
    TPtrC8 mediaServer;        
    for( TInt i = iRuleArray.Count() - 1; i >= 0; i-- )
        {
        if( KErrNone == iRuleArray[i]->MediaServerCount() )    
            {
            // If no servers defined => DO NOT use rule at all
            // First delete the object
            delete iRuleArray[i];
            // Then delete the pointer
            iRuleArray.Remove(i);    
            LOG(_L("[STORE MNGR]\t Store rule doesn't have any \
                defined servers"));
            LOG(_L("[STORE MNGR]\t ******* ==> RULE SKIPPED \
                ******************"));      
            }
        else
            {
            // Do nothing
            }        
        }
    iRuleArray.Compress();                
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadFilledAndStoredL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::LoadFilledAndStoredL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::LoadFilledAndStoredL()"));    
    
    LoadMediaServerIdsL();
    LoadFilledL();    
    for( TInt i = 0; i < iMsIds.Count(); i++ )
        {
        LoadStoredL( *iMsIds[i]->iUuid );
        }
    CompleteRequest( ECmSmAllLoaded );            
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadFilledL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::LoadFilledL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::LoadFilledL()"));
    
    iFilledItems.ResetAndDestroy();
    iFilledItems.Close();
    iDBManager->PrepareQueryCmdL(EAllFillFilesStatusQuery);    
    iDBManager->GetAllFillFilesL( 
        iFilledItems,ECmFilled|ECmToBeRemoved|ECmLocalCopy );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadStoredL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::LoadStoredL( const TDesC8& aUDN )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::LoadStoredL()"));        
    
    // Load to spesific server filled items
    iDBManager->PrepareQueryCmdL( EMediaServerIdQuery );
    TInt64 id( iDBManager->QueryMediaServerId( aUDN ) );
    if( KErrNone < id )
        {
        RPointerArray<HBufC16> array;
        CleanupClosePushL( array );
        iDBManager->PrepareQueryCmdL(EStoredRowQuery);
        iDBManager->QueryStoredRowL( array, aUDN, KErrNotFound );
        for( TInt i = 0; i < array.Count(); i++ )
            {            
            CCmSmItemInfo* storedItemInfo = 
                CCmSmItemInfo::NewLC( *array[i], id );
            iStoredArray.Append( storedItemInfo );
            CleanupStack::Pop( storedItemInfo );
            }
        // Ownership didn't change, reset and destroy    
        array.ResetAndDestroy();
        CleanupStack::PopAndDestroy( &array );
        }
    else
        {
        LOG(_L("[STORE MNGR]\t No media server id found"));        
        }                        
    }
    
// ---------------------------------------------------------------------------
// CCmSmFileMngr::ResetFilledArray
// ---------------------------------------------------------------------------
//    
void CCmSmFileMngr::ResetFilledArray()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ResetFilledArray()"));
            
    iFilledItems.ResetAndDestroy();
    iFilledItems.Close();
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::SelectMediaTypes
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::SelectMediaTypes()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::SelectMediaTypes()"));
    
    TCmMediaType mediaType;
    for( TInt i = 0; i < iRuleArray.Count(); i++ )
        {
        for( TInt j = 0; j < iRuleArray[i]->DefCount(); j++ )
            {
            iRuleArray[i]->StoreRule( j, &mediaType );
            if( KErrNotFound == iMediaTypes.Find( mediaType ) )
                {
                iMediaTypes.Append( mediaType );
                }
            }
        }                                                   
    iRefreshIndex = 0;            
    CompleteRequest( ECmSmMediaTypesSelected );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::CheckIsFilledFileL
// ---------------------------------------------------------------------------
//
TBool CCmSmFileMngr::CheckIsFilledFileL( const TDesC& aPathAndFileName )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::CheckIsFilledFileL()"));
    
    TBool processed( EFalse );
    // Check if file is filled from some listed server
    for( TInt j = 0; j < iFilledItems.Count() && !processed; j++ )
        {
        if( KErrNotFound != aPathAndFileName.FindC( 
            iFilledItems[j]->Path() ) )
            {
            LOG(_L("[STORE MNGR]\t Filled file!!!"));
            LOG(_L("[STORE MNGR]\t File not stored!!!"));            
            processed = ETrue;
            // End loop
            j = iFilledItems.Count();
            }
        }
    return processed;    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::IsAlreadyStoreL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::IsAlreadyStoreL( const TDesC& aPathAndFileName, 
    RArray<TInt>& aStoredIds, RArray<TInt>& aStoreIds )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::IsAlreadyStoreL()"));
    
    // Check if file is already stored to some of the defined servers
    for( TInt k = 0; k < iStoredArray.Count() ;k++ )
        {
        if(  KErrNone == iStoredArray[k]->iFile->
            Compare( aPathAndFileName ) )
                {
                for( TInt l = aStoreIds.Count() - 1; l >= 0; l-- )
                    {
                    if( aStoreIds[l] == iStoredArray[k]->iId )
                        {
                        aStoredIds.Append( aStoreIds[l] );
                        aStoreIds.Remove(l);
                        LOG(_L("[STORE MNGR]\t Stored file!!!"));
                        }
                    }
                aStoreIds.Compress();   
                }                        
        }     
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::MediaTypesL
// ---------------------------------------------------------------------------
//    
void CCmSmFileMngr::MediaTypesL( RArray<TInt>& aIds, TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::MediaTypesL()"));
    
    // Loop for store rules
    for( TInt i = 0; i < iRuleArray.Count(); i++ )
        {
        TBool addServers = EFalse;
        TInt msCount(iRuleArray[i]->MediaServerCount());
        TInt defCount( iRuleArray[i]->DefCount() ); 
        TCmMediaType temptype;
        
        // In specific store rule, loop to check if the rule has relevant 
        // media type.
        for( TInt n = 0; n < defCount; n++ )
            {
            iRuleArray[i]->StoreRule(n, &temptype );
            if(  aType == temptype )
                {
                addServers = ETrue;            
                }
            }
        if( addServers )
            {
            // Loop for media servers
            for( TInt k = 0; k < msCount ; k++ )
                {
                const TPtrC8 ms = iRuleArray[i]->MediaServerL( k );
                
                // To find if Uuid of ms has been in iMsIds.
                for( TInt l = 0 ; l < iMsIds.Count(); l++ )
                    {
                    if( KErrNone == ms.Compare( *iMsIds[l]->iUuid ) )
                        {
                        aIds.InsertInOrderL( iMsIds[l]->iId );
                        // End loop    
                    	l = iMsIds.Count();  
                        }
                    }
                }
            }
        }    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadMediaServerIdsL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::LoadMediaServerIdsL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::LoadMediaServerIdsL()"));
    
    TInt64 id(KErrNone);
    RArray<TInt64> tempIds;
    CleanupClosePushL( tempIds );
    for( TInt i = 0; i < iRuleArray.Count(); i++ )
        {
        for( TInt j = 0 ; j < iRuleArray[i]->MediaServerCount(); j++ )
            {
            const TDesC8& mediaServer = iRuleArray[i]->MediaServerL( j );
            iDBManager->PrepareQueryCmdL( EMediaServerIdQuery );
            id = iDBManager->QueryMediaServerId( mediaServer );
            if( KErrNone < id && KErrNotFound == tempIds.Find(id) )
                {
                HBufC8* uuid = mediaServer.AllocLC();
                CCmSmMsInfo* msInfo = CCmSmMsInfo::NewLC( *uuid, id );
                tempIds.AppendL(id);
                iMsIds.AppendL(msInfo);                
                CleanupStack::Pop( msInfo );
                CleanupStack::PopAndDestroy( uuid );
                }
            }
        }
    tempIds.Reset();
    CleanupStack::PopAndDestroy( &tempIds );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::GetDevId
// ---------------------------------------------------------------------------
//
TInt64 CCmSmFileMngr::GetDevId( const TDesC8& aUuid )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::GetDevId()"));
    
    TInt64 id( KErrNone );
    TBool flag( EFalse );
    for( TInt i = 0; i < iMsIds.Count() && !flag; i++ )
        {
        if( KErrNotFound != iMsIds[i]->iUuid->Match( aUuid ) )
            {
            id = iMsIds[i]->iId;
            flag = ETrue;
            }
        }
    return id;    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::GetItemsL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::GetItemsL( RArray<TInt>& aItemIds,
    const TDesC8& aUuid, TInt64& aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::GetItemsL()"));
    
    aItemIds.Reset();        
    
    RArray<TInt>ids;    
    RArray<TCmListItemStatus> statusValues;
    
    aId = GetDevId( aUuid );
    for( TInt i = 0; i < iItems.Count() && aId != KErrNone; i++ )
        {
        ids = iItems[i]->DevIds();
        statusValues = iItems[i]->StatusValues();
        TInt index( ids.FindInOrder( aId ) );
        if( KErrNotFound != index )
            {
            TBool itemOnSelectedList( EFalse );
            // check that item belongs to selected list
            for ( TInt j = 0; j < iRuleArray.Count(); j++ )
                {
                CCmStoreRule* rule = iRuleArray[j];
                if ( iItems[i]->ListId() ==  rule->ListId() )
                    {
                    itemOnSelectedList = ETrue;
                    }
                }

            // Check that item isn't stored yet            
            if( index < statusValues.Count() )
                {
                if( itemOnSelectedList &&
                    ECmStored != statusValues[ index ] )
                    {
                    aItemIds.AppendL( i );
                    }                
                }            
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::UpdateStoreFileListL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::UpdateStoreFileListL()
    {
    TRACE( Print( _L("[STORE MNGR]\t UpdateStoreFileListL iItemsCount = %d"), 
        iItems.Count() ));
        
    iDBManager->DeleteStoreFiles();
    iDBManager->SetStoreFilesL( iItems );
    iItems.ResetAndDestroy();
    iItems.Close();            
    CalculateAvgTransferTimeL();
    SendAvgTransferTime();    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::LoadStoreFileListL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::LoadStoreFileListL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::LoadStoreFileListL()"));
    
    iItems.ResetAndDestroy();
    iItems.Close();
    ResetArrays();
    LoadSelectedStoreRulesL();
    ProcessStoreRules();
    LoadMediaServerIdsL();

    iDBManager->PrepareQueryCmdL( EAllStoreFilesQuery );
    iDBManager->QueryAllStoreFilesL( iItems );    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::RetrieveListId
// ---------------------------------------------------------------------------
//    
TInt64 CCmSmFileMngr::RetrieveListId( TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::RetrieveListId()"));
    
    TCmMediaType type;
    TUint id( KErrNone );
    TBool flag( EFalse );
    for( TInt i = 0; i < iRuleArray.Count() && !flag; i++ )
        {
        TInt defCount = iRuleArray[i]->DefCount();
        for( TInt j = 0; j < defCount && !flag; j++ )
            {
            iRuleArray[i]->StoreRule( j, &type );
            if( type == aType )
                {
                flag = ETrue;
                id = iRuleArray[i]->ListId();
                }
            }
        }
    return id;    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::RetrieveListStatusValues
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::RetrieveListStatusValues( TCmMediaType aType,
    TCmListItemStatus& aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::RetrieveListStatusValues()"));
    
    TCmMediaType type;
    TBool flag( EFalse );
    for( TInt i = 0; i < iRuleArray.Count() && !flag; i++ )
        {
        TInt defCount = iRuleArray[i]->DefCount();
        for( TInt j = 0; j < defCount && !flag; j++ )
            {
            iRuleArray[i]->StoreRule( j, &type );
            if( type == aType )
                {
                flag = ETrue;
                // Set found status value
                aStatus = iRuleArray[i]->Status();
                }
            }
        }    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::DeleteItems
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::DeleteItems( RArray<TInt>& aIds )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::DeleteItems()"));
    
    TInt i( 0 );
    // aIds has indexes to iItems array. 
    while( aIds.Count() )
        {
        if( ( aIds[0] - i ) < iItems.Count() )
            {
            delete iItems[aIds[0] - i];
            iItems.Remove( aIds[0] - i );
            aIds.Remove(0);            
            }
        i++;
        }
    iItems.Compress();    
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ResetArrays
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::ResetArrays()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ResetArrays()"));
    
    // Let's reset all needed array    
    iRuleArray.ResetAndDestroy();
    iRuleArray.Close();
 
    iFilledItems.ResetAndDestroy();
    iFilledItems.Close(); 
    iStoredArray.ResetAndDestroy();
    iStoredArray.Close();
    
    iMsIds.ResetAndDestroy();
    iMsIds.Close();  
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ProcessMedia
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::ProcessMedia( TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ProcessMedia"));
    switch( aType )
        {
        case ECmAudio:
            {
            ProcessMusics();
            break;
            }                
        case ECmVideo: // Fall through
        case ECmImage: // Fall through
        case ECmOtherImage: // Fall through
        case ECmOtherVideo: // Fall through       
            {
            iVideoAndImageMngr->ProcessMedia( aType );
            break;
            }
        default:
            {
            User::Invariant();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::ProcessMusics
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::ProcessMusics()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::ProcessMusics()"));
    
    TRAPD( err, iClfMusicMngr->DoRefreshL( ECmAudio ) );
    if( err )
        {
        TRACE( Print( _L( "ProcessMusics err = %d"), err ));
        CompleteRequest( ECmSmPreProcessingReady );        
        }    
    } 
                
// ---------------------------------------------------------------------------
// CCmSmFileMngr::CalculateAvgTransferTimeL
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::CalculateAvgTransferTimeL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::CalculateAvgTransferTimeL()"));
    
    TInt64 ud(KErrNone);
    TInt64 dd(KErrNone);
    TInt64 ut(KErrNone);
    TInt64 dt(KErrNone);
    TInt64 size( KErrNone );
    TInt64 shrinkTime(KErrNone);
    TInt64 transferTime(KErrNone);
    for( TInt i = 0; i < iMsIds.Count(); i++ )
        {
        iDBManager->PrepareQueryCmdL(ETransferHistoryQuery);
        iDBManager->QueryTransferHistory( 
            *iMsIds[i]->iUuid, dd, ud, dt, ut );
        size = iDBManager->KBytesToBeStored( iMsIds[i]->iId, 
            ECmToBeRemoved|ECmToBeShrinked|ECmKeepOnDevice );
        if( ud == KErrNone || ut == KErrNone )
            {
            ud = KCmSmIniStoreTransferSpeed;
            ut = KCmSmIniStoreTransferTime;
            }            
        transferTime = transferTime + (( ut * size ) / ud );  
        }
    
    TInt shrinkCount( iDBManager->StoreFileCount(ECmToBeShrinked) );
        
    shrinkTime = (iDBManager->GetAvgImageShrinkTime() * shrinkCount);     
    
    TInt totalCount(KErrNone);
    totalCount = iDBManager->StoreFileCountNoDuplicates();
    transferTime = transferTime + (shrinkTime / 1000 );
    if( KErrNone >= totalCount ) 
        {
        transferTime = KErrNone;
        }
    iDBManager->UpdateTransferInfo( ECmServiceStore, totalCount, 
        transferTime );
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::SendAvgTransferTime
// ---------------------------------------------------------------------------
//   
void CCmSmFileMngr::SendAvgTransferTime()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::SendAvgTransferTime()"));
          
    iDBManager->GetTransferInfo( ECmServiceStore, 
        iTransferInfo.iProcessedItems, iTransferInfo.iTotalItems );
    iTransferInfo.iService = ECmServiceTransferInfoStore;
    
    TRACE( Print( _L("[STORE MNGR]\t Store file count %d"), 
        iTransferInfo.iProcessedItems)); 
            
    TCmProgressInfoPckg transferInfoPckg( iTransferInfo );
    
    TInt err = RProperty::Set( KCmPropertyCat, KCmProperty, 
        transferInfoPckg );
    
    TRACE( Print( _L("[STORE MNGR]\t RProperty::Set returned %d"), err));    
    }
    
// ---------------------------------------------------------------------------
// CCmSmFileMngr::SetProcessingStatus
// ---------------------------------------------------------------------------
//
void CCmSmFileMngr::SetProcessingStatus( TCmSmFileProcessingStatus aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::SetProcessingStatus()"));
    
    iProcessingStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// CCmSmFileMngr::CompleteRequest
// ---------------------------------------------------------------------------
// 
void CCmSmFileMngr::CompleteRequest( 
    TCmSmFileProcessingStatus aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::CompleteRequest()"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmSmManager, KErrInUse ));          
    SetActive();        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );     
    }
    
// ---------------------------------------------------------------------------
// CCmSmFileMngr::RunL
// ---------------------------------------------------------------------------
//                 
void CCmSmFileMngr::RunL()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmFileMngr::RunL\
         status = %d"), iStatus.Int() ));        
    switch( iStatus.Int() )
        {
        case ECmSmPreProcessingStarted:
            {
            LoadSelectedStoreRulesL();
            CompleteRequest( ECmSmStoreRulesLoaded );              
            break;
            }
        case ECmSmStoreRulesLoaded:
            {
            ProcessStoreRules();
            if( KErrNone < iRuleArray.Count() )
                {
                CompleteRequest( ECmSmLoadFilledAndStored );
                }
            else
                {
                // No selected rules => Delete all store files
                TInt err( iDBManager->DeleteStoreFiles() );
                TRACE(Print(_L("[STORE MNGR]\t DeleteStoreFiles error = %d"),
                      err ));
                CalculateAvgTransferTimeL();
                SendAvgTransferTime();
                iObserver->FileProcessingStatus( ECmSmNoStoreRulesSelected );
                }               
            break;
            }
        case ECmSmLoadFilledAndStored:
            {
            LoadFilledAndStoredL();
            break;
            }
        case ECmSmAllLoaded:
            {
            SelectMediaTypes();
            break;
            }
        case ECmSmMediaTypesSelected:
            {
            if( iMediaTypes.Count() > iRefreshIndex )
                {
                ProcessMedia( iMediaTypes[iRefreshIndex] );
                iRefreshIndex++;
                if( iProcessingStatus == ECmSmProcessingFilesStarted )
                    {
                    iObserver->FileProcessingStatus( iProcessingStatus );
                    iProcessingStatus = ECmSmNone;
                    }
                }
            else
                {
                LOG(_L("[STORE MNGR]\t Store list processing ready"));
                UpdateStoreFileListL();
                CompleteRequest( ECmSmPreProcessingReady );                
                }                            
            break;
            }
        case ECmSmPreProcessingReady:
            {
            ResetArrays();
            if( iProcessingStatus == ECmSmProcessingFilesStarted )
                {
                iObserver->FileProcessingStatus( iProcessingStatus );
                iProcessingStatus = ECmSmNone;
                }
            iObserver->FileProcessingStatus( ECmSmPreProcessingReady );            
            break;
            }
        default:
            {
            LOG(_L("[STORE MNGR]\t Store file processing RunL default"));
            User::Invariant();
            break;
            }
        }
    }    

// ---------------------------------------------------------------------------
// CCmSmFileMngr::DoCancel
// ---------------------------------------------------------------------------
//    
void CCmSmFileMngr::DoCancel()
    {
    LOG(_L("[STORE MNGR]\t CCmSmFileMngr::DoCancel()"));     
    }
  
// ---------------------------------------------------------------------------
// CCmSmFileMngr::RunError
// ---------------------------------------------------------------------------
// 
TInt CCmSmFileMngr::RunError( TInt aError )
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmFileMngr::RunError\
         aError = %d"), aError ));     
    return aError;
    }
                            
// End of file



