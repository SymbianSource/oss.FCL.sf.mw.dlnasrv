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
* Description:  CUPnPFileSharingEngine class implementation
*
*/



//  Include Files
#include "contentmanager.h"
#include "msengine.h"
#include "msengineao.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSEngine::CMSEngine
// --------------------------------------------------------------------------
//
CMSEngine::CMSEngine()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::CMSEngine") );
    }


// --------------------------------------------------------------------------
// CMSEngine::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CMSEngine* CMSEngine::NewLC()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::NewLC") );

    CMSEngine* self = new (ELeave) CMSEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CMSEngine::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CMSEngine* CMSEngine::NewL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::NewL") );

    CMSEngine* self = CMSEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CMSEngine::ConstructL
// --------------------------------------------------------------------------
//
void CMSEngine::ConstructL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ConstructL") );

    // Open Content manager server
    OpenServerSession();

    // Create active object
    iAO = CMSEngineAO::NewL( this );
    }

// --------------------------------------------------------------------------
// CMSEngine::~CMSEngine
// --------------------------------------------------------------------------
//
CMSEngine::~CMSEngine()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::~CMSEngine") );

    TCmServerState state;
    ServerState( state );

    if ( state != ECmServerStateIdle )
        {
        LOG( _L( "[MediaServant]\t CMSEngine::\
        ~CMSEngine cancel operation" ) );

        // notify server that observer is shutting down
        if( iAO )
            {
            iAO->ApplicationExit();
            }
        }

    // Close Content manager server
    CloseServerSession();

    delete iAO;
    }

// --------------------------------------------------------------------------
// CMSEngine::ScanMediaServersL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::ScanMediaServersL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ScanMediaServersL") );

    TInt error = iAO->ScanMediaServersL();

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::ExecuteSyncL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::ExecuteSyncL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ExecuteSyncL") );

    // Fill will be started when store is ready.
    TInt error = iAO->ExecuteStoreL();

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::ExecuteStorePreProcessingL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::ExecuteStorePreProcessingL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ExecuteStorePreProcessingL") );

    TInt error = iAO->ExecuteStorePreProcessingL();

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::ExecuteFillPreProcessingL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::ExecuteFillPreProcessingL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ExecuteFillPreProcessingL") );

    TInt error = iAO->ExecuteFillPreProcessingL();

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::GetStoreListItemsL
// --------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CCmBaseListItem>* CMSEngine::GetStoreListItemsL(
                                                          TDesC8& aRuleName)
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::GetStoreListItemsL"));

    RPointerArray<CCmBaseListItem>* storeListItems = NULL;
    if ( &iCMServer )
        {
        storeListItems = iCMServer.ListItemsL( aRuleName, ECmServiceStore );
        }

    return storeListItems;
    }

// --------------------------------------------------------------------------
// CMSEngine::GetFillListItemsL
// --------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CCmBaseListItem>* CMSEngine::GetFillListItemsL(
                                                          TDesC8& aRuleName)
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::GetFillListItemsL"));

    RPointerArray<CCmBaseListItem>* fillListItems = NULL;

    fillListItems = iCMServer.ListItemsL( aRuleName, ECmServiceFill );

    return fillListItems;
    }

// --------------------------------------------------------------------------
// CMSEngine::StopOperation
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::StopOperation()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::StopOperation") );

    iAO->StopOperation();
    }

// --------------------------------------------------------------------------
// CMSEngine::FillRulesL
// --------------------------------------------------------------------------
//
EXPORT_C CCmFillRuleContainer* CMSEngine::FillRulesL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::FillRulesL") );

    return ( iCMServer.FillRulesL() );
    }

// --------------------------------------------------------------------------
// CMSEngine::StoreRulesL
// --------------------------------------------------------------------------
//
EXPORT_C CCmStoreRuleContainer* CMSEngine::StoreRulesL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::StoreRulesL") );

    return ( iCMServer.StoreRulesL() );
    }

// --------------------------------------------------------------------------
// CMSEngine::SetStoreRulesL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::SetStoreRulesL( CCmStoreRuleContainer* aStoreRules )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetStoreRulesL") );

    iCMServer.SetStoreRulesL( *aStoreRules );
    }

// --------------------------------------------------------------------------
// CMSEngine::SetFillRulesL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::SetFillRulesL( CCmFillRuleContainer* aFillRules )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetFillRulesL") );

    iCMServer.SetFillRulesL( *aFillRules );
    }

// --------------------------------------------------------------------------
// CMSEngine::GetFilledFilesSize
// Get filled files size
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::GetFilledFilesSize( TInt64& aSize, const TInt aID )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::GetFilledFilesSize") );

    iCMServer.GetFilledFilesSize( aSize, aID );
    }

// CMSEngine::ServerState
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::ServerState( TCmServerState& aState )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ServerState") );

    aState = iCMServer.ServerState();
    }

// -------------------------------------------------------------------------
// CMSEngine::ServiceState
// --------------------------------------------------------------------------
EXPORT_C TCmServiceState CMSEngine::ServiceState( const TCmService aService )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::ServiceState") );

    TCmServiceState state;

    state = iCMServer.ServiceState( aService );

    return state;
    }

// -------------------------------------------------------------------------
// CMSEngine::SetServiceState
// --------------------------------------------------------------------------
EXPORT_C TInt CMSEngine::SetServiceState( const TCmService aService,
                                          TCmServiceState aState )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetServiceState") );

    TInt error = iCMServer.SetServiceState( aService, aState );

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::MediaServersL
// --------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CCmMediaServerFull>*
                        CMSEngine::GetMediaServersL()
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::GetMediaServersL"));

    return ( iCMServer.MediaserversL() );
    }

// --------------------------------------------------------------------------
// CMSEngine::SetMediaServersL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::SetMediaServersL(
    RPointerArray<CCmMediaServerFull>& aMediaServers )
    {

    LOG( _L("[MediaServant ENG]\t CMSEngine::SetMediaServersL") );

    TInt error = iCMServer.SetMediaserversL( aMediaServers );

    return error;
    }


// --------------------------------------------------------------------------
// CMSEngine::DeleteDeletedMediaserversL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::DeleteDeletedMediaserversL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::DeleteDeletedMediaserversL") );

    iCMServer.DeleteDeletedMediaserversL();
    }


// --------------------------------------------------------------------------
// CMSEngine::DeleteFilledFiles
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::DeleteFilledFilesL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::DeleteFilledFilesL") );

    TInt error = iAO->DeleteFilledFilesL();

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::GetItemCountsL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::GetItemCountsL( const TCmService aService,
                                         const TDesC8& aListName,
                                         TInt& aCount,
                                         TInt64& aSize ) const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::GetItemCountsL") );

    TInt error = iCMServer.GetItemCountsL( aService,
                                           aListName,
                                           aCount,
                                           aSize );

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::GetFilteredMetadataL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::GetFilteredMetadataL(
                            CCmSqlPropertyCollector*& aCollector )
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::GetFilteredMetadataL"));

    iAO->GetFilteredMetadataL( aCollector );
    }

// --------------------------------------------------------------------------
// CMSEngine::PreProcessFillList
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::PreProcessFillListL( const TDesC8& aListName ) const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::PreProcessFillList") );

    TInt error = iAO->PreProcessFillListL( aListName );

    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::UpdateFillListsL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::UpdateFillListsL() const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::UpdateFillListsL") );

    iAO->UpdateFillListsL();
    }

// --------------------------------------------------------------------------
// CMSEngine::DeleteMetaDataL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::DeleteMetadataL() const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::DeleteMetaDataL") );

    iAO->DeleteMetadataL();
    }

// --------------------------------------------------------------------------
// CMSEngine::GetAppWizardState
// --------------------------------------------------------------------------
//
EXPORT_C TBool CMSEngine::GetAppWizardState( TInt& /*aErrorCode*/ )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::GetAppWizardState") );

    TBool retVal = iCMServer.AppWizardState();

    return retVal;
    }


// --------------------------------------------------------------------------
// CMSEngine::SetAppWizardStateL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CMSEngine::SetAppWizardState()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetAppWizardState") );

    TInt retVal = iCMServer.SetAppWizardState();

    return retVal;
    }

// --------------------------------------------------------------------------
// CMSEngine::SetObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::SetObserver( MMSEngineObserver* aObserver )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetObserver") );

    iAO->SetObserver( aObserver );
    }

// --------------------------------------------------------------------------
// CMSEngine::LastSyncTime
// --------------------------------------------------------------------------
//
EXPORT_C TTime CMSEngine::LastSyncTime() const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::LastSyncTime") );

    return ( iCMServer.LastSyncTime() );
    }

// --------------------------------------------------------------------------
// CMSEngine::SetFillFileStatusesL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::SetFillFileStatusesL(
            const RPointerArray<CCmBaseListItem>* aItems )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetFillFileStatusesL") );

    iCMServer.SetFillFileStatusesL( *aItems );
    }

// --------------------------------------------------------------------------
// CMSEngine::GetTransferInfo
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::GetTransferInfo( const TCmService aService,
                                          TInt& aItemCount,
                                          TInt& aTimeInSec ) const
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::GetTransferInfo") );

    iCMServer.GetTransferInfo( aService,
                               aItemCount,
                               aTimeInSec );
    }

// --------------------------------------------------------------------------
// CMSEngine::SetDrivesL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::SetDrivesL( RPointerArray<CCmDriveInfo> aDrives )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::SetDrivesL") );

    iCMServer.SetDrivesL( aDrives );
    }


// --------------------------------------------------------------------------
// CMSEngine::GetDrivesL
// --------------------------------------------------------------------------
//
EXPORT_C void CMSEngine::GetDrivesL( RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::GetDrivesL") );

    iCMServer.GetDrivesL( aDrives );
    }

// --------------------------------------------------------------------------
// CMSEngine::DriveSelectionStatus
// --------------------------------------------------------------------------
//
EXPORT_C TBool CMSEngine::DriveSelectionStatus()
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::DriveSelectionStatus"));

    return iCMServer.DriveSelectionStatus();
    }

// --------------------------------------------------------------------------
// CMSEngine::OpenServerSession
// --------------------------------------------------------------------------
//
TInt CMSEngine::OpenServerSession()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::OpenServerSession") );
    TInt error = iCMServer.Connect();
    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::CloseServerSession
// --------------------------------------------------------------------------
//
TInt CMSEngine::CloseServerSession()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngine::CloseServerSession") );

    return ( iCMServer.Close() );
    }

// End of file
