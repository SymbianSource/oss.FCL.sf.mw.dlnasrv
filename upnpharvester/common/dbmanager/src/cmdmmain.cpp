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
* Description:      Main class in the Database manager component
*
*/






#include <e32std.h>
#include <bautils.h>
#include "cmdmsqliteconnection.h"
#include "cmdmsqldbmaintenance.h"
#include "cmfillrule.h"
#include "cmstorerule.h"
#include "cmmediaserverfull.h"
#include "cmbaselistitem.h"
#include "cmfilllistitem.h"
#include "cmstorelistitem.h"
#include "cmdriveinfo.h"
#include "cmdmmain.h"
#include "msdebug.h"


// ---------------------------------------------------------------------------
// CCmDmMain::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmDmMain* CCmDmMain::NewL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::NewL()"));   
    CCmDmMain* self = CCmDmMain::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmDmMain::NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmDmMain* CCmDmMain::NewLC()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::NewLC()"));
    CCmDmMain* self = new ( ELeave ) CCmDmMain();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmDmMain::~CCmDmMain
// ---------------------------------------------------------------------------
//
EXPORT_C CCmDmMain::~CCmDmMain()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::~CCmDmMain()")); 
    delete iConnection;
    iConnection = NULL;
    delete iMaintenance;
    iMaintenance = NULL;    
    }

// ---------------------------------------------------------------------------
// CCmDmMain::PrepareQueryCmdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::PrepareQueryCmdL( TCMQueryCmdType aType )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::PrepareQueryCmdL()")); 
    iConnection->PrepareQueryCmdL( aType );    
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::QueryFillRuleL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::QueryFillRuleL( CCmFillRule* aFillRule )
    {
    return iConnection->QueryFillRuleL( aFillRule );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QuerySelectedFillRuleNamesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QuerySelectedFillRuleNamesL( 
    RPointerArray<HBufC8>& aNames, TInt aSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QuerySelectedFillRuleNamesL()")); 
    iConnection->QuerySelectedFillRuleNamesL( aNames, aSelected );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryFillRuleNamesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryFillRuleNamesL( RPointerArray<HBufC8>& aNames )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryFillRuleNamesL()")); 
    iConnection->QueryFillRuleNamesL( aNames );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryStoreRuleL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::QueryStoreRuleL(CCmStoreRule* aStoreRule)
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryStoreRuleL()")); 
    return iConnection->QueryStoreRuleL( aStoreRule );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryStoreRuleNamesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryStoreRuleNamesL( RPointerArray<HBufC8>& aNames )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryStoreRuleNamesL()")); 
    iConnection->QueryStoreRuleNamesL( aNames );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QuerySelectedStoreRuleNamesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QuerySelectedStoreRuleNamesL(
    RPointerArray<HBufC8>& aNames, TInt aSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QuerySelectedStoreRuleNamesL()")); 
    iConnection->QuerySelectedStoreRuleNamesL( aNames, aSelected );
    }        
    
// ---------------------------------------------------------------------------
// CCmDmMain::QueryMediaServerL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryMediaServerL( CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryMediaServerL()")); 
    return iConnection->QueryMediaServerL( aMediaServer );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryMediaServerId
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt64 CCmDmMain::QueryMediaServerId( const TDesC8& aUDN )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryMediaServerId()")); 
    return iConnection->QueryMediaServerId( aUDN );
    }

// ---------------------------------------------------------------------------
// CmDmMain::QueryMediaServerUdn
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryMediaServerUdn( HBufC8*& aUdn, 
    const TInt32 aId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryMediaServerUdn()")); 
    return iConnection->QueryMediaServerUdn( aUdn, aId );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryAllMediaServersL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryAllMediaServersL( 
    CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryAllMediaServersL()")); 
    return iConnection->QueryAllMediaServersL( aMediaServer );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetMediaServersL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmDmMain::GetMediaServersL( 
    RPointerArray<CCmMediaServerFull>& aMediaServers )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetMediaServersL()")); 
    iConnection->GetMediaServersL( aMediaServers );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::GetLowPriorityFilledFilesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetLowPriorityFilledFilesL( TInt aDrive,
    RPointerArray<CCmFillListItem>& aFilled )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetLowPriorityFilledFilesL()")); 
    return iConnection->GetLowPriorityFilledFilesL( aDrive, aFilled );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryFillFileListL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::QueryFillFileListL( 
    RPointerArray<CCmBaseListItem>& aItems, 
    const TDesC8& aName, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryFillFileListL()")); 
    return iConnection->QueryFillFileListL( aItems, aName, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetAllFillFilesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetAllFillFilesL( 
                                    RPointerArray<CCmFillListItem>& aItems,
                                    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetAllFillFilesL()")); 
    return iConnection->GetAllFillFilesL( aItems, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::QueryStoreFileListL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryStoreFileListL( 
    RPointerArray<CCmBaseListItem>& aItems, 
    const TDesC8& aName, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryStoreFileListL()")); 
    return iConnection->QueryStoreFileListL( aItems, aName, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetAllStoreFilesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetAllStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems, TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetAllStoreFilesL()")); 
    return iConnection->GetAllStoreFilesL( aItems, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::QueryAllFillFilesL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryAllFillFilesL( 
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryAllFillFilesL()")); 
    return iConnection->QueryAllFillFilesL( aItems );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryAllStoreFilesL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::QueryAllStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryAllStoreFilesL()")); 
    return iConnection->QueryAllStoreFilesL( aItems );    
    }
            
// ---------------------------------------------------------------------------
// CCmDmMain::QueryStoredRowL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryStoredRowL( RPointerArray<HBufC16>& aFiles, 
    const TDesC8& aUDN, 
    TInt aAmount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryStoredRowL()")); 
    iConnection->QueryStoredRowL( aFiles, aUDN, aAmount );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::QueryTransferHistory
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryTransferHistory( 
    const TDesC8& aUdn, 
    TInt64& aDownloadData, 
    TInt64& aUploadData, 
    TInt64& aDownloadTime, 
    TInt64& aUploadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryTransferHistory()")); 
    iConnection->QueryTransferHistory( aUdn, aDownloadData, 
        aUploadData, aDownloadTime, aUploadTime );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryDeletedItemsListsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryDeletedItemsListsL( 
    RPointerArray<CCmBaseListItem>& aArray, TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryDeletedItemsListsL()")); 
    iConnection->QueryDeletedItemsListsL( aArray, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryDrivesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::QueryDrivesL( RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryDrivesL()")); 
    iConnection->QueryDrivesL( aDrives );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::GetAvgImageShrinkTime
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetAvgImageShrinkTime()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetAvgImageShrinkTime()")); 
    return iConnection->GetAvgImageShrinkTime();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetFillListNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::GetFillListNameL( HBufC8*& aListName, 
    const TInt64 aListId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetFillListNameL()")); 
    return iConnection->GetFillListNameL( aListName, aListId );
    }
        
// ---------------------------------------------------------------------------
// CCmDmMain::SetFillFiles
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::SetFillFiles( 
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetFillFiles()")); 
    return iConnection->SetFillFiles( aItems );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::SetStoreFilesL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::SetStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetStoreFilesL()")); 
    return iConnection->SetStoreFilesL( aItems );    
    }

// ---------------------------------------------------------------------------
// CCmDmMain::AddStoreFile
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::AddStoreFile( CCmStoreListItem& aItem )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddStoreFile()")); 
    return iConnection->AddStoreFile( aItem );    
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::AddStoredL
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::AddStoredL( 
    const TDesC& aFileName, 
    const TDesC8& aUDN, 
    TTime aTimeStamp )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddStoredL()")); 
    return iConnection->AddStoredL( aFileName, aUDN, aTimeStamp );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::AddFillRuleL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::AddFillRuleL( CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddFillRuleL()")); 
    return iConnection->AddFillRuleL( aFillRule );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::AddStoreRuleL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::AddStoreRuleL( CCmStoreRule* aStoreRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddStoreRuleL()")); 
    return iConnection->AddStoreRuleL( aStoreRule );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::AddMediaServerL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::AddMediaServerL( CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddMediaServerL()")); 
    return iConnection->AddMediaServerL( aMediaServer );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::AddHashValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::AddHashValue( TInt64 aMediaServerId, 
                                       CCmSearchResponseHash* aHashValue )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::AddHashValue()")); 
    return iConnection->AddHashValue( aMediaServerId, aHashValue );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::QueryHashValuesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::QueryHashValuesL( TInt aMediaServer, 
        RPointerArray<CCmSearchResponseHash>& aHashValues )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::QueryHashValuesL()")); 
    return iConnection->QueryHashValuesL( aMediaServer, aHashValues );
    }
// ---------------------------------------------------------------------------
// CCmDmMain::SetMediaServersL  
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::SetMediaServersL( 
    RPointerArray<CCmMediaServerFull>& aServers )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetMediaServersL()")); 
    return iConnection->SetMediaServersL( aServers );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::SetSyncTime
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::SetSyncTime()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetSyncTime()")); 
    return iConnection->SetSyncTime();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetSyncTime
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetSyncTime( TTime& aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetSyncTime()")); 
    return iConnection->GetSyncTime( aTime );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::SetProgressInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::SetProgressInfo( TInt aTransferred, 
    TInt aTotalCount)
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetProgressInfo()")); 
    return iConnection->SetProgressInfo( aTransferred, aTotalCount );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetProgressInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetProgressInfo( TInt& aTransferred, 
    TInt& aTotalCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetProgressInfo()")); 
    return iConnection->GetProgressInfo( aTransferred, aTotalCount );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::SetDrivesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::SetDrivesL( RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetDrivesL()")); 
    iConnection->SetDrivesL( aDrives );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::DeleteMediaServer
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::DeleteMediaServer( const TDesC8& aUDN )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteMediaServer()")); 
    return iConnection->DeleteMediaServer( aUDN );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::DeleteFillRuleByName
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt CCmDmMain::DeleteFillRuleByName( const TDesC8& aName )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteFillRuleByName()")); 
    return iConnection->DeleteFillRuleByName( aName );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::DeleteAllFillRules
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::DeleteAllFillRules()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteAllFillRules()")); 
    return iConnection->DeleteAllFillRules();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::DeleteAllStoreRules
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::DeleteAllStoreRules()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteAllStoreRules()")); 
    return iConnection->DeleteAllStoreRules();
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::DeleteFillFiles
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::DeleteFillFiles()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteFillFiles()")); 
    return iConnection->DeleteFillFiles();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::DeleteStoreFiles
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::DeleteStoreFiles()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteStoreFiles()")); 
    return iConnection->DeleteStoreFiles();
    }
                      
// ---------------------------------------------------------------------------
// CCmDmMain::DeleteDeletedItemsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::DeleteDeletedItemsL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteDeletedItemsL()")); 
    iConnection->DeleteDeletedItemsL();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::DeleteItemsByFillListId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::DeleteItemsByFillListId( TInt aFillListId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteItemsByFillListId()")); 
    return iConnection->DeleteItemsByFillListId( aFillListId );
    }


// ---------------------------------------------------------------------------
// CCmDmMain::DeleteDeletedMediaServersL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::DeleteDeletedMediaServersL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteDeletedMediaServersL()")); 
    iConnection->DeleteDeletedMediaServersL();
    }

// ---------------------------------------------------------------------------
// CCmDmMain::DeleteHashValuesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::DeleteHashValuesL( const TInt aMediaserverId,
    const TInt aSearchIndex )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::DeleteHashValuesL()")); 
    iConnection->DeleteHashValuesL( aMediaserverId, aSearchIndex );  
    }
                
// ---------------------------------------------------------------------------
// CCmDmMain::UpdateMediaServerInfo
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::UpdateMediaServerInfo( 
    CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateMediaServerInfo()")); 
    return iConnection->UpdateMediaServerInfo( aMediaServer );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::UpdateFillRuleSelected
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::UpdateFillRuleSelected( 
    const TDesC8& aName, 
    TInt aSelected )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateFillRuleSelected()")); 
    return iConnection->UpdateFillRuleSelected( aName, aSelected );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::UpdateDownloadHistory
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::UpdateDownloadHistory( TUint aMediaServerId,
    TInt64& aDownloadData, TInt64& aDownloadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateDownloadHistory()")); 
    return iConnection->UpdateDownloadHistory( aMediaServerId,
        aDownloadData, aDownloadTime );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::UpdateUploadHistory
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::UpdateUploadHistory( TUint aMediaServerId,
    TInt64& aUploadData, TInt64& aUploadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateUploadHistory()")); 
    return iConnection->UpdateUploadHistory( aMediaServerId,
        aUploadData, aUploadTime );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::UpdateStatusValues
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::UpdateStatusValues( 
    RPointerArray<CCmBaseListItem>& aStatusValues )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateStatusValues()")); 
    return iConnection->UpdateStatusValues( aStatusValues );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::UpdateFillListDriveStatusesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::UpdateFillListDriveStatusesL( 
    CCmDriveInfo* aOrigDrive, CCmDriveInfo* aNewDrive, 
    TBool aDriveSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateFillListDriveStatusesL()")); 
    iConnection->UpdateFillListDriveStatusesL( 
        aOrigDrive, aNewDrive, aDriveSelected );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::UpdateFillListItemPriority
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::UpdateFillListItemPriority( 
    CCmFillRuleContainer* aContainer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateFillListItemPriority()")); 
    return iConnection->UpdateFillListItemPriority( aContainer );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::SetFillListStateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::SetFillListStateL( const TUint aListId, 
    TCmFillRuleStatus aState )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::SetFillListStateL()")); 
    iConnection->SetFillListStateL( aListId, aState );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::RestoreFileStatusesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::RestoreFileStatusesL( 
    const RPointerArray<CCmFillListItem>& aItems, const TDesC8& aListName )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::RestoreFileStatusesL()")); 
    return iConnection->RestoreFileStatusesL( aItems, aListName );
    }
            
// ---------------------------------------------------------------------------
// CCmDmMain::IncrementShrinkTimeL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::IncrementShrinkTimeL( TInt aImageCount, 
    TInt aMilliSeconds )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::IncrementShrinkTimeL()")); 
    return iConnection->IncrementShrinkTimeL( aImageCount,
        aMilliSeconds );    
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::ListSize
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmDmMain::ListSize( 
    TCmRuleType aType, 
    const TDesC8& aListName,
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::ListSize()")); 
    
    TInt64 size( KErrNone );
    if( aType == ECmFmFillRule )
        {
        size = iConnection->FillListSize( aListName, aStatus );
        }
    else
        {
        size = iConnection->StoreListSize( aListName, aStatus );
        }
    return size;        
    }

// ---------------------------------------------------------------------------
// CCmDmMain::StoreFileCount
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::StoreFileCount( TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::StoreFileCount()")); 
    return iConnection->StoreFileCount( aStatus );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::StoreFileCountNoDuplicates
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::StoreFileCountNoDuplicates()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::StoreFileCountNoDuplicates()")); 
    return iConnection->StoreFileCountNoDuplicates();
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::FillFileCount
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::FillFileCount( TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::FillFileCount()")); 
    return iConnection->FillFileCount( aStatus );
    }

// ---------------------------------------------------------------------------
// Amount of data filled
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt64 CCmDmMain::BytesFilled( TUint aID )
    {
    return iConnection->BytesFilled( aID );
    }


// CCmDmMain::KBytesToBeFilled
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt64 CCmDmMain::KBytesToBeFilled( TUint8 aMediaServerId, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::KBytesToBeFilled()")); 
    return iConnection->KBytesToBeFilled( aMediaServerId, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::KBytesToBeStored
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt64 CCmDmMain::KBytesToBeStored( TUint8 aMediaServerId, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::KBytesToBeStored()")); 
    return iConnection->KBytesToBeStored( aMediaServerId, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::UpdateTransferInfo
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::UpdateTransferInfo( TCmService aService, 
    TInt aCount, TInt aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateTransferInfo()")); 
    return iConnection->UpdateTransferInfo( aService, aCount, aTime );
    }   

// ---------------------------------------------------------------------------
// CCmDmMain::UpdateStoreTransferStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::UpdateStoreTransferStatus( TUint aStatus,
    TInt64 aFid, TInt64 aMsId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateStoreTransferStatus()")); 
    iConnection->UpdateStoreTransferStatus( aStatus, aFid, aMsId );
    }


// ---------------------------------------------------------------------------
// CCmDmMain::UpdateFillTransferStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::UpdateFillTransferStatus( TUint aStatus, 
    CCmFillListItem* aItem )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateFillTransferStatus()")); 
    iConnection->UpdateFillTransferStatus( aStatus, aItem );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::GetTransferInfo
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::GetTransferInfo( TCmService aService, 
    TInt& aCount, TInt& aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetTransferInfo()")); 
    return iConnection->GetTransferInfo( aService, aCount, aTime );
    }
   
// ---------------------------------------------------------------------------
// CCmDmMain::GetStoreFileCount
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmDmMain::GetStoreFileCount( 
    const TDesC8& aListName, 
    TUint aStatus, 
    TInt& aCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetStoreFileCount()")); 
    return iConnection->GetStoreFileCount( aListName, aStatus, aCount );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetFillFileCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::GetFillFileCount( 
    const TDesC8& aListName, 
    TUint aStatus, 
    TInt& aCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetFillFileCount()")); 
    return iConnection->GetFillFileCount( aListName, aStatus, aCount );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::GetStoreFileCountNoDuplicatesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::GetStoreFileCountNoDuplicatesL( TInt& aCount,
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::GetStoreFileCountNoDuplicatesL()")); 
    iConnection->GetStoreFileCountNoDuplicatesL( aCount, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmDmMain::SetFillListRealCountAndSizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDmMain::SetFillListRealCountAndSizeL( TInt64 aListId, 
    TInt64 aRealCount, TInt64 aRealSize )
    {    
    TRACE( Print( _L("CCmDmMain::SetFillListRealCountAndSizeL \
                    aListId =%ld,aRealCount=%ld,aRealSize=%ld"),
                    aListId,aRealCount, aRealSize ) );

    TInt64 realCount = ( aRealCount > 0 ) ? aRealCount : 0;
    TInt64 realSize = ( aRealSize > 0 ) ? aRealSize : 0;
    iConnection->SetFillListRealCountAndSizeL( 
        aListId, realCount, realSize );
    }

// ---------------------------------------------------------------------------
// CCmDmMain::UpdateFillRuleGroupInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmDmMain::UpdateFillRuleGroupInfo()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::UpdateFillRuleGroupInfo()")); 
    return iConnection->UpdateFillRuleGroupInfo();
    }   

// ---------------------------------------------------------------------------
// CCmDmMain::CCmDmMain
// ---------------------------------------------------------------------------
//    
CCmDmMain::CCmDmMain()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::CCmDmMain()")); 
    }

// ---------------------------------------------------------------------------
// CCmDmMain::ConstructL
// ---------------------------------------------------------------------------
//    
void CCmDmMain::ConstructL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmMain::ConstructL()")); 
    
    RFs fs;
    CleanupClosePushL( fs );
    TInt err = fs.Connect();    
    TRAP(err, BaflUtils::EnsurePathExistsL(fs, KCmDbDir));
    CleanupStack::PopAndDestroy( &fs );
    
    iConnection = CCmDmSQLiteConnection::NewL();
    iMaintenance = CCmDmSqlDbMaintenance::NewL(); 
    if( iMaintenance->InitConnection(*iConnection) )
        {
        iMaintenance->CreateDbFile( *iConnection );
        }
    if( !iConnection->Validate() )
        {
        iMaintenance->CreateDb( *iConnection );
        iMaintenance->CreateIndexes( *iConnection );
        iConnection->InitIDs();        
        }         
                
    iConnection->LoadIDs(); 
    }    

// End of file

