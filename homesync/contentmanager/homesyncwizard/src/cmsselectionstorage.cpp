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
* Description:  Implementation of setup wizard selection storage
*
*/


// INCLUDE FILES
#include <commdb.h>
#include <WlanCdbCols.h>
#include <wlanmgmtclient.h>
#include <driveinfo.h>

#include "cmsettingsfactory.h"
#include "cmmediaserverfull.h"
#include "cmdriveinfo.h"
#include "cmsettings.h"
#include "cmsselectionstorage.h"
#include "msengine.h"
#include "cmstorerulecontainer.h"
#include "cmfillrulecontainer.h"
#include "cmstorerule.h"
#include "cmfillrule.h"
#include "msconstants.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSSelectionStorage::NewL
// --------------------------------------------------------------------------
//
CMSSelectionStorage* CMSSelectionStorage::NewL(
                            CMSEngine& aMSEngine,
                            RPointerArray<CCmMediaServerFull>& aServers,
                            RFs& aFs )
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::NewL"));
    
    CMSSelectionStorage* self = CMSSelectionStorage::NewLC( aMSEngine,
                                                            aServers,
                                                            aFs );
        
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::NewLC
// --------------------------------------------------------------------------
//
CMSSelectionStorage* CMSSelectionStorage::NewLC(
                            CMSEngine& aMSEngine,
                            RPointerArray<CCmMediaServerFull>& aServers,
                            RFs& aFs )
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::NewLC"));
    
    CMSSelectionStorage* self = new (ELeave) CMSSelectionStorage( aMSEngine,
                                                                  aServers,
                                                                  aFs );
    CleanupStack::PushL(self);
    self->ConstructL(  );
    
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSSelectionStorage::CMSSelectionStorage
// --------------------------------------------------------------------------
//
CMSSelectionStorage::CMSSelectionStorage(
                            CMSEngine& aMSEngine,
                            RPointerArray<CCmMediaServerFull>& aServers,
                            RFs& aFs ) :
    iMSEngine( aMSEngine ), iServers( aServers ), iFs( aFs )
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::CMSSelectionStorage"));
    
    // remove fill & store usage flag from every server
    for ( TInt i = 0; i < iServers.Count(); i++ )
        {
        CCmMediaServerFull* server = iServers[i];
        server->SetStoreUsage( EFalse );
        server->SetStoreUsage( EFalse );
        server->SetIsActive( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::ConstructL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::ConstructL()
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::ConstructL"));
    
    iStoreRules = iMSEngine.StoreRulesL();
    iFillRules = iMSEngine.FillRulesL();
    iServiceState = ETrue; // default: manual sync   
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::~CMSSelectionStorage
// --------------------------------------------------------------------------
//
CMSSelectionStorage::~CMSSelectionStorage()
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::~CMSSelectionStorage"));

    delete iSelectedFillLists;
    delete iSelectedStoreLists;
    delete iStoreRules;
    delete iFillRules;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::StoreMSSettingsL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::StoreMSSettingsL()
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::StoreMSSettingsL begins"));

    // Set store rules
    iMSEngine.SetStoreRulesL( iStoreRules );
    
    // Set fill rules
    iMSEngine.SetFillRulesL( iFillRules );
    
    // Set syncrhronization state

    // if ETrue -> MANUAL mode
    // if EFalse -> AUTOMATIC mode
    if( iServiceState )
        {
        iMSEngine.SetServiceState( ECmServiceContentManager,
                                   ECmServiceStateDisabled );
        }
    else
        {
        iMSEngine.SetServiceState( ECmServiceContentManager,
                                   ECmServiceStateAutomatic );
        // Set wlan scanning on if AUTOMATIC mode
        SetWlanScanL();
        }
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::StoreMSSettingsL ends"));
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetWlanScanL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetWlanScanL()
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::SetWlanScanL"));

    // open commsdb
    CCommsDatabase* commsDb = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDb );
#ifndef __WINS__
    // open wlan table
    CCommsDbTableView* view = commsDb->OpenViewMatchingUintLC
        (
        TPtrC( WLAN_DEVICE_SETTINGS ),
        TPtrC( WLAN_DEVICE_SETTINGS_TYPE ), KWlanUserSettings
        );

    User::LeaveIfError( view->GotoFirstRecord() );
    User::LeaveIfError( view->UpdateRecord() );

    // set scan interval
    view->WriteUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ), 
                      KWlanScanNetworkInterval60 );
    User::LeaveIfError( view->PutRecordChanges() );

    CleanupStack::PopAndDestroy( view );
#endif
    CleanupStack::PopAndDestroy( commsDb );

#ifndef __WINS__
    CWlanMgmtClient* wlanMgmtClient = CWlanMgmtClient::NewL();
    CleanupStack::PushL( wlanMgmtClient );
    // notify about changes
    wlanMgmtClient->NotifyChangedSettings();
    CleanupStack::PopAndDestroy( wlanMgmtClient );
 #endif
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetServersForFillRulesL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetServersForFillRulesL( 
                                CArrayFix<TInt>& aSelectedIndexes)
    {
    LOG(_L("[MSAppWizard]\t CMSSelectionStorage::SetServersForFillRulesL "));

    // set server activity anyway
    for ( TInt idx = 0; idx < aSelectedIndexes.Count(); idx++ )
        {
        CCmMediaServerFull* server = iServers[aSelectedIndexes.At(idx)];
        server->SetFillUsage( ETrue );
        server->SetIsActive( ETrue );
        }

    // Get fill list count
    TInt fillrulecount = iFillRules->FillRuleCount();
    
    // remove all servers from store lists and set unselected
    for ( TInt index = 0; index < fillrulecount; index++ )
        {
        CCmFillRule* rule = iFillRules->FillRule( index );
        // remove list selection
        rule->SetSelected( ECmUnSelected );
        // remove servers
        RemoveServersFromFillRuleL( rule );
        }

    // set selected servers for selected lists
    for( TInt i=0; i<iSelectedFillLists->Count(); i++ )
        {
        TInt index = (*iSelectedFillLists)[i];
        CCmFillRule* rule = iFillRules->FillRule(index);
        rule->SetSelected( ECmSelected );
        
        SetServersForFillRuleL( rule, aSelectedIndexes );
        }
    }
    
// --------------------------------------------------------------------------
// CMSSelectionStorage::SetServersForFillRuleL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetServersForFillRuleL( CCmFillRule* aRule,
                                        CArrayFix<TInt>& aSelectedIndexes )
    {
    LOG(_L("[MSAppWizard]\t SetServersForFillRuleL"));
    TInt ruleServerCount = aRule->MediaServerCount();

    for ( TInt iy = 0; iy < aSelectedIndexes.Count(); iy++ )
        {
        CCmMediaServerFull* server = iServers[aSelectedIndexes.At(iy)];

        TPtrC8 serverUDN = server->MediaServer();
        aRule->AddMediaServerL( serverUDN );        
        server->SetIsActive( KServerActive );
        }    
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetServersForStoreRulesL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetServersForStoreRulesL(
                                CArrayFix<TInt>& aSelectedIndexes,
                                RPointerArray<CCmMediaServerFull>& aServers )
    {

    // set server activity anyway
    for ( TInt idx = 0; idx < aSelectedIndexes.Count(); idx++ )
        {
        CCmMediaServerFull* server = aServers[aSelectedIndexes.At(idx)];
        server->SetStoreUsage( ETrue );
        server->SetIsActive( ETrue );
        }

    // remove all servers from store lists and set unselected
    for ( TInt index = 0; index < iStoreRules->StoreRuleCount(); index++ )
        {
        CCmStoreRule* rule = iStoreRules->StoreRule( index );
        // remove list selection
        rule->SetSelected( ECmUnSelected );
        RemoveServersFromStoreRuleL( rule );
        }       

    // set selected servers for selected lists
    for ( TInt i = 0; i < iSelectedStoreLists->Count(); i++ )
        {
        TInt selectionValue = iSelectedStoreLists->At( i );

        CCmStoreRule* rule = iStoreRules->StoreRule( selectionValue );
        rule->SetSelected( ECmSelected );
        //...and set selected servers to list
        SetServersForStoreRuleL( rule, aSelectedIndexes, aServers );
        }    
    }
    
// --------------------------------------------------------------------------
// CMSSelectionStorage::SetServersForStoreRuleL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetServersForStoreRuleL(
                                CCmStoreRule* aRule,
                                CArrayFix<TInt>& aSelectedIndexes,
                                RPointerArray<CCmMediaServerFull>& aServers )
    {
    LOG(_L("[MSAppWizard]\t SetServersForStoreRuleL"));            

    for ( TInt iy = 0; iy < aSelectedIndexes.Count(); iy++ )
        {
        CCmMediaServerFull* server = aServers[aSelectedIndexes.At(iy)];
        TPtrC8 serverUDN = server->MediaServer();
        aRule->AddMediaServerL( serverUDN );
        server->SetIsActive( KServerActive );
        }
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::RemoveServersFromFillRuleL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::RemoveServersFromFillRuleL( CCmFillRule* aRule )
    {
    //Remove old servers from the rule
    LOG(_L("[MSAppWizard]\t RemoveServersFromFillRuleL"));
    TInt ruleServerCount = aRule->MediaServerCount();
    for ( TInt index = ruleServerCount-1; index >= 0; index-- )
        {
        const TDesC8& ruleServerUDN = aRule->MediaServerL( index );
        aRule->DeleteMediaServer( ruleServerUDN );
        }
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::RemoveServersFromStoreRuleL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::RemoveServersFromStoreRuleL( CCmStoreRule* aRule )
    {
    //Remove old servers from the rule
    LOG(_L("[MSAppWizard]\t RemoveServersFromStoreRuleL"));
    TInt ruleServerCount = aRule->MediaServerCount();
    for ( TInt index = ruleServerCount-1; index >= 0; index-- )
        {
        const TDesC8& ruleServerUDN = aRule->MediaServerL( index );
        aRule->DeleteMediaServer( ruleServerUDN );
        }
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetStoreListSelections
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetStoreListSelections( CArrayFix<TInt>& aSelections )
    {
    if ( iSelectedStoreLists )
        {
        delete iSelectedStoreLists;
        iSelectedStoreLists = NULL;
        }

    iSelectedStoreLists = &aSelections;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::ClearStoreListSelections
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::ClearStoreListSelections()
    {
    iSelectedStoreLists->Reset();
    }
    
// --------------------------------------------------------------------------
// CMSSelectionStorage::SetFillListSelections
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetFillListSelections(
                                    CArrayFix<TInt>& aSelections )
    {
    if ( iSelectedFillLists )
        {
        delete iSelectedFillLists;
        iSelectedFillLists = NULL;
        }

    iSelectedFillLists = &aSelections;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetAutoManualSelection
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetAutoManualSelection( TBool aSelection )
    {
    iServiceState = aSelection;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::AutoManualSelection
// --------------------------------------------------------------------------
//
TBool CMSSelectionStorage::AutoManualSelection()
    {
    return iServiceState;
    }

// --------------------------------------------------------------------------
// CMSSelectionStorage::SetDriveInformationL
// --------------------------------------------------------------------------
//
void CMSSelectionStorage::SetDriveInformationL()
    {
    LOG(_L("[MSAppWizard]\t SetDriveInformationL"));

    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();
    RPointerArray<CCmDriveInfo> driveArray;
    CleanupClosePushL( driveArray );
    
    settings->DriveListL( iFs, driveArray );

    TInt64 quota(0);

    for ( TInt i = 0; i < driveArray.Count(); i++ )
        {        
        if ( driveArray[i]->DriveType() == DriveInfo::EDriveRemovable )
            {
            // mmc
            quota = ( driveArray[i]->DriveSize() * KMMCDefaultQuota ) / 100;
            LOG(_L("[MSAppWizard]\t CMSSelectionStorage::\
            SetDriveInformationL memory card quota set"));
            }
        else
            {
            // internal hd
            quota = ( driveArray[i]->DriveSize() * KHDDefaultQuota ) / 100;
            LOG(_L("[MSAppWizard]\t CMSSelectionStorage::\
            SetDriveInformationL mass memory quota set"));                      
            }

        // set quota
        driveArray[i]->SetDriveQuota( quota );
        }


    iMSEngine.SetDrivesL( driveArray );
    driveArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &driveArray );
	settings->Close();
    CleanupStack::Pop(); // settings
    }
    
// End of File

