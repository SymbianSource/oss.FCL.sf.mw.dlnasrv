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
* Description:  Store music file processing
*  Version     : %version: tr1ido#9.1.2 % << Don't touch!
*
*/



#include <ContentListingFactory.h>
#include <MCLFContentListingEngine.h>
#include <MCLFItem.h>
#include <MCLFSortingStyle.h>
#include "upnpfileutility.h"

#include "cmstorelistitem.h"
#include "cmsmfilemngr.h"
#include "cmsmclfmngr.h"
#include "msdebug.h"

// CONSTANTS
 _LIT( KCmSmIconExtension, ".fil" );
 _LIT( ROM1, "Z" );
 _LIT( ROM2, "z" );
// Once filter maximum count;
const TInt KFilterMaxCount = 10;
        
// ---------------------------------------------------------------------------
// CCmSmClfMngr::NewL
// ---------------------------------------------------------------------------
// 
CCmSmClfMngr* CCmSmClfMngr::NewL( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::NewL() start"));    
    CCmSmClfMngr* self = CCmSmClfMngr::NewLC(
        aObserver, aFileMngr, aItems );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmClfMngr* CCmSmClfMngr::NewLC( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::NewLC() start"));     
    CCmSmClfMngr* self = new ( ELeave ) CCmSmClfMngr(
        aObserver, aFileMngr, aItems );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmClfMngr::~CCmSmClfMngr
// ---------------------------------------------------------------------------
// 
CCmSmClfMngr::~CCmSmClfMngr()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::\
        ~CCmSmClfMngr() start"));
    Cancel();
    CancelClfRefresh();
    delete iNewAudioItems;
    delete iNewImageItems;
    delete iNewVideoItems;    
    delete iEngine;
    delete iSortingStyle;    
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::\
        ~CCmSmClfMngr() end"));            
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::CCmSmClfMngr
// ---------------------------------------------------------------------------
//            
CCmSmClfMngr::CCmSmClfMngr( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems )
    :CActive( EPriorityStandard ),
    iObserver( aObserver ), iFileMngr( aFileMngr ),
    iItems( aItems )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmClfMngr::ConstructL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::ConstructL()"));
    iEngine = ContentListingFactory::NewContentListingEngineLC();
    CleanupStack::Pop();
    iNewAudioItems = iEngine->CreateListModelLC( *this );
    CleanupStack::Pop();
    
    // Creating a temporary pointer since ContentListingFactory interface 
    // forces us to use LC-method and we don't want to assign an member 
    // variable with LC-method.
    MCLFSortingStyle* sortingStyle = 
        ContentListingFactory::NewSortingStyleLC();
    CleanupStack::Pop(); // can't pop by name here
    iSortingStyle = sortingStyle;
    sortingStyle = NULL;
    
    iSortingStyle->SetOrdering( ECLFOrderingAscending );
    iSortingStyle->SetSortingDataType( ECLFItemDataTypeDesC );  
    }   
    
// ---------------------------------------------------------------------------
// CCmSmClfMngr::CancelClfRefresh
// ---------------------------------------------------------------------------
//
void CCmSmClfMngr::CancelClfRefresh()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::CancelClfRefresh()"));
    if( iNewAudioItems )
        {
        iNewAudioItems->CancelRefresh();
        }                 
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::CancelOperation
// ---------------------------------------------------------------------------
//    
void CCmSmClfMngr::CancelOperation()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::CancelOperation"));    
    
    if ( IsActive() )
        {
        Cancel();    
        }
    CancelClfRefresh();
    iObserver.MusicReady( ECmSmProcessingCanceled );
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::SetupCLFListModelsL
// ---------------------------------------------------------------------------
//    
void CCmSmClfMngr::SetupCLFListModelsL( TCmMediaType aMediaType )
    {
    LOG(_L("[STORE MNGR]\t SetupCLFListModelsL"));
    
    RArray< TInt > mediaTypes;
    CleanupClosePushL( mediaTypes );
        
    switch(aMediaType)
        {
        case ECmAudio:
            {
            mediaTypes.AppendL( ECLFMediaTypeMusic );
            mediaTypes.AppendL( ECLFMediaTypeSound );
            iNewAudioItems->SetWantedMediaTypesL( mediaTypes.Array() );
            iNewAudioItems->SetSortingStyle( iSortingStyle );
            break;
            }                      
        default:
            {
            break;
            }    
        }
    CleanupStack::PopAndDestroy( &mediaTypes );
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::DoRefreshL
// ---------------------------------------------------------------------------
//
void CCmSmClfMngr::DoRefreshL( TCmMediaType aMediaType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::DoRefreshL"));
    
    switch( aMediaType )
        {
        case ECmAudio:
            {
            SetupCLFListModelsL( aMediaType );
            iNewAudioItems->RefreshL();
            break;
            }      
        default:
            {
            LOG(_L("[STORE MNGR]\t DoRefreshL invariant media type"));
            User::Invariant();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::FilterItemsL
// ---------------------------------------------------------------------------
//    
void CCmSmClfMngr::FilterItemsL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::DoRefreshL"));
    TRACE(Print( _L("[STORE MNGR]\t Filter file start index = %d"), 
                                                             iFilterIndex ) );
    TCmSmFileProcessingStatus status = ECmSmNone;
    if ( iFilterIndex < iNewAudioItems->ItemCount() )
        {
        RArray<TInt>ids;
        CleanupClosePushL( ids );
        iFileMngr.MediaTypesL( ids, ECmAudio );
        FilterNewItemsL( iNewAudioItems, ids, ECmAudio );
        CleanupStack::PopAndDestroy( &ids );
        status = ECmSmContinueFiltering;
        }
    else
        { 
        status = ECmSmFilteringReady;
        }
    
    TRACE(Print( _L("[STORE MNGR]\t Filter file end index = %d"), 
                                                             iFilterIndex ) );
    CompleteRequest( status );    
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::FilterNewItemsL
// ---------------------------------------------------------------------------
//
void CCmSmClfMngr::FilterNewItemsL( MCLFItemListModel* aNewItems, 
    RArray<TInt>aDeviceIds, TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t Filtering defined media type items"));
    
    TInt allCount = aNewItems->ItemCount();
    
    RArray<TInt>storeIds;
    CleanupClosePushL( storeIds );
    RArray<TInt>storedIds;
    CleanupClosePushL( storedIds );
    
    // Copy all file names
    TInt count = ( iFilterIndex + KFilterMaxCount ) <= allCount ? 
                                KFilterMaxCount : ( allCount - iFilterIndex );
    TRACE(Print( _L("[STORE MNGR]\t FilterNewItemsL file count = %d"), 
                                                                    count ) );

    TBool drm( EFalse );        
    for ( TInt i = 0; i < count; i++ )
        {
        storeIds.Reset();         
        
        // Copy ids into temporary array
        for( TInt ll = 0; ll < aDeviceIds.Count(); ll++ )
            {
            storeIds.Append(aDeviceIds[ll]);
            }
        
        const MCLFItem& item = aNewItems->Item( iFilterIndex );
        // Fetch protection type
        TPtrC fileName;
        TPtrC path;
        TInt32 size( 0 );
        TTime date;
        TPtrC album;
        TPtrC mediaType;
        TPtrC pathAndFileName;
        TPtrC fileExtension;
        User::LeaveIfError( item.GetField( ECLFFieldIdPath, path ));
        User::LeaveIfError( item.GetField( ECLFFieldIdFileName, fileName ));
        User::LeaveIfError( item.GetField( ECLFFieldIdFileSize, size ));
        User::LeaveIfError( item.GetField( ECLFFieldIdFileDate, date ));
        User::LeaveIfError( item.GetField( ECLFFieldIdFileNameAndPath, 
            pathAndFileName ));
        User::LeaveIfError( item.GetField( ECLFFieldIdFileExtension,
            fileExtension ) );    
        
        // Album list is needed for UI use
        TInt error( item.GetField( ECLFFieldIdAlbum, album ) );    
            
        TRACE(Print( _L("[STORE MNGR]\t %03d: %S"), i, &fileName ) );
        
        // If the items is not an DRM item and album isn't in excluded list, 
        // it is a potential candidate to be added into list of 
        // files to transfer.
        TRAP( error, drm = 
            UPnPFileUtility::IsFileProtectedL( pathAndFileName ) );
        TRACE( Print( _L("[STORE MNGR]\t IsFileProtectedL err = %d"), error));
        if ( pathAndFileName[0] != ROM1()[0] 
            && pathAndFileName[0] != ROM2()[0] 
            && !drm 
            && KErrNotFound == fileExtension.Match( KCmSmIconExtension ) )
            {
            if( iFileMngr.CheckIsFilledFileL( pathAndFileName ) )
                {
                storeIds.Reset();                
                }
            iFileMngr.IsAlreadyStoreL( pathAndFileName, storedIds, storeIds );
            // If file is already stored to every wanted server
            // => no need to keep file on the list any longer    
            if( KErrNone < storeIds.Count() )
                {
                TCmListItemStatus status;
                CCmStoreListItem* item = CCmStoreListItem::NewLC();                
                item->SetPrimaryTextL( fileName );
                item->SetPathL( pathAndFileName );
                item->SetSize( size );
                item->SetDate( date );
                item->SetListId( iFileMngr.RetrieveListId( aType ) );
                // Ms id values are binded with file status values 
                iFileMngr.RetrieveListStatusValues( aType, status );
                // Where item will be stored 
                for( TInt s = 0 ; s < storeIds.Count(); s++ )
                    {
                    item->SetDevId( storeIds[s], status );    
                    }
                // Where item has already been stored
                for( TInt d = 0; d < storedIds.Count(); d++ )
                    {
                    item->SetDevId( storedIds[d], ECmStored );
                    }
                iItems.AppendL( item );
                CleanupStack::Pop( item );                
                }
            storedIds.Reset();         
            }              
        iFilterIndex++;
        }
    
    CleanupStack::PopAndDestroy( &storedIds );
    CleanupStack::PopAndDestroy( &storeIds );         
    }

// ---------------------------------------------------------------------------
// CCmSmClfMngr::HandleOperationEventL
// ---------------------------------------------------------------------------
//     
void CCmSmClfMngr::HandleOperationEventL(
    TCLFOperationEvent aOperationEvent, TInt aError )
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::HandleOperationEventL"));

    if ( aOperationEvent == ECLFRefreshComplete )
        {
        switch ( aError )
            {
            case KErrNone:
                {
                CompleteRequest( ECmSmAllRefreshed );
                break;
                }
            case KErrCancel:
                {
                iObserver.MusicReady( ECmSmProcessingCanceled );
                break;
                }
            default:
                {
                // Notify observer.
                iObserver.MusicReady( ECmSmRefreshError );
                break;
                }
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmSmClfMngr::CompleteRequest
// ---------------------------------------------------------------------------
// 
void CCmSmClfMngr::CompleteRequest( 
    TCmSmFileProcessingStatus aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::CompleteRequest"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmSmManager, KErrInUse ));          
    SetActive();        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );     
    }
    
// ---------------------------------------------------------------------------
// CCmSmClfMngr::RunL
// ---------------------------------------------------------------------------
//                 
void CCmSmClfMngr::RunL()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmClfMngr::RunL\
         status = %d"), iStatus.Int() ));
         
    switch( iStatus.Int() )
        {       
        case ECmSmAllRefreshed:
            {
            CompleteRequest( ECmSmStartItemFiltering );  
            break;
            }
        case ECmSmStartItemFiltering:
            {
            iFilterIndex = 0;
            TRACE(Print( _L("[STORE MNGR]\t Need filter file count = %d"), 
                                              iNewAudioItems->ItemCount() ) );
            CompleteRequest( ECmSmContinueFiltering );
            break;
            }
        case ECmSmContinueFiltering:
            {
            FilterItemsL();
            break;
            }
        case ECmSmFilteringReady:
            {
            iObserver.MusicReady( KErrNone );
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
// CCmSmClfMngr::DoCancel
// ---------------------------------------------------------------------------
//    
void CCmSmClfMngr::DoCancel()
    {
    LOG(_L("[STORE MNGR]\t CCmSmClfMngr::DoCancel"));
    }
                        
// End of file



