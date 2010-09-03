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
* Description:  CMediaServantAppUi class implementation
*
*/


#include <StringLoader.h>
#include <mediaservant.rsg>
#include <f32file.h>
#include <driveinfo.h>

#include "cmsettingsfactory.h"
#include "cmsettings.h"
#include "cmdriveinfo.h"
#include "msappui.h"
#include "msstorelistcontroller.h"
#include "msmainview.h"
#include "msfillview.h"
#include "msfillruleeditview.h"
#include "msstorelistview.h"
#include "msstoresettingsview.h"
#include "msbrowseview.h"
#include "msfillbrowseview.h"
#include "msconstants.h"
#include "msengine.h"
#include "msparameteragent.h"
#include "cmsappwizard.h"
#include "msgsinterface.h"
#include "msgspluginuids.h"

#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSAppUi::ConstructL()
// --------------------------------------------------------------------------
//
void CMSAppUi::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::ConstructL"));

#ifndef __SERIES60_31__
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
#else
    BaseConstructL( EAknEnableSkin );
#endif
    
    // Create engine
    iMSEngine = CMSEngine::NewL();

    // Create parameter agent for message passing between views
    iParameterAgent = new ( ELeave ) CMSParameterAgent();

    // Create fill list controller
    iFillRuleController = CMSFillRuleController::NewL( *iMSEngine );

    // Create store list controller
    iStoreListController = CMSStoreListController::NewL( *iMSEngine );

    LoadSettingsPluginL(); // loads gs plugin

    // main view is created
    CMSMainView* mainView = new ( ELeave )CMSMainView( *this );
    CleanupStack::PushL( mainView );
    mainView->ConstructL();
    AddViewL( mainView );           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( mainView);

    CMSFillView* fillView = new ( ELeave ) CMSFillView( *this );
    CleanupStack::PushL( fillView );
    fillView->ConstructL();
    AddViewL( fillView );           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( fillView );

    CMSFillRuleEditView* ruleEditView = new ( ELeave )
                                            CMSFillRuleEditView( *this );
    CleanupStack::PushL( ruleEditView );
    ruleEditView->ConstructL();
    AddViewL( ruleEditView );       // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( ruleEditView );

    CMSStoreListView* storelistView = new ( ELeave )
                                CMSStoreListView( *this );
    CleanupStack::PushL( storelistView );
    storelistView->ConstructL();
    AddViewL( storelistView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( storelistView);

    CMSStoreSettingsView* storesettingView = new ( ELeave )
                                CMSStoreSettingsView( *this );
    CleanupStack::PushL( storesettingView );
    storesettingView->ConstructL();
    AddViewL( storesettingView );   // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( storesettingView );

    CMSBrowseView* browseView = new ( ELeave )
                                CMSBrowseView( *this );
    CleanupStack::PushL( browseView );
    browseView->ConstructL();
    AddViewL( browseView );   // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( browseView );

    CMSFillBrowseView* fillBrowseView =
                    new ( ELeave ) CMSFillBrowseView( *this );
    CleanupStack::PushL( fillBrowseView );
    fillBrowseView->ConstructL();
    AddViewL( fillBrowseView );   // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( fillBrowseView );


    // default view is now main view
    SetDefaultViewL( *mainView );
    }

// --------------------------------------------------------------------------
// CMSAppUi::~CMSAppUi()
// --------------------------------------------------------------------------
//
CMSAppUi::~CMSAppUi()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::~CMSAppUi"));
    
    if( iPlugin )
        {
        // Destroy Ecom plugin
        iPlugin->DestroyPlugin();
        }

    delete iMSEngine;
    delete iParameterAgent;
    delete iFillRuleController;
    delete iStoreListController;
    }

// --------------------------------------------------------------------------
// CMSAppUi::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSAppUi::HandleCommandL(TInt aCommand)
    {
    LOG(_L("[MediaServant]\t CMSAppUi::HandleCommandL"));

    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        // fall trough
        case EEikCmdExit:
            {
            // program is exited
            Exit();
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSAppUi::HandleCommandL \
            invalid command"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSAppUi::ChangeViewL()
//
// Changes active view according to input parameter.
// --------------------------------------------------------------------------
//
void CMSAppUi::ChangeViewL(const TUid& aOldView, const TUid& aNewView)
    {
    LOG(_L("[MediaServant]\t CMSAppUi::ChangeViewL"));

    iParameterAgent->SetPreviousViewId( aOldView );

    if ( aOldView.iUid == KMSMainViewId.iUid )
        {
        // store focus index of main view
        iParameterAgent->SetMainViewFocusIndex(
                    static_cast<CMSMainView*> (iView)->FocusIndex());
        
        }
    else if ( aOldView.iUid == KMSFillViewId.iUid )
        {
        // store focus index of main view
        iParameterAgent->SetFillViewFocusIndex(
                    static_cast<CMSFillView*> (iView)->FocusIndex());
        
        }
    else if ( aOldView.iUid == KMSStoreListViewId.iUid )
        {
        // store focus index of fill view
        iParameterAgent->SetStoreViewFocusIndex(
                    static_cast<CMSStoreListView*> (iView)->FocusIndex());
        }        
    else
        {
        // no need to change view
        }

    // new view is activated
    ActivateViewL(TVwsViewId(KUidMediaServant, aNewView));
    }

// --------------------------------------------------------------------------
// CMSAppUi::StoreListController()
// Returns fill rule container
// --------------------------------------------------------------------------
//
CMSStoreListController* CMSAppUi::StoreListController()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::StoreListController"));

    return iStoreListController;
    }

// --------------------------------------------------------------------------
// CMSAppUi::MSEngine()
// Returns msengine pointer
// --------------------------------------------------------------------------
//
CMSEngine* CMSAppUi::MSEngine()
    {
    return iMSEngine;
    }

// --------------------------------------------------------------------------
// CMSAppUi::RefreshListsL()
// Refresh store and fill lists
// --------------------------------------------------------------------------
//
void CMSAppUi::RefreshListsL()    
    {
    LOG(_L("[MediaServant]\t CMSAppUi::RefreshListsL"));
    
    iStoreListController->UpdateListsL();
    iFillRuleController->UpdateArrayL();
    }

// --------------------------------------------------------------------------
// CMSFillView::ReadMMCInfoL
// Reads mmc capasity
// --------------------------------------------------------------------------
//
TInt CMSAppUi::ReadMMCInfoL( TInt64& aCapasity, TInt64& aFreeSpace )
    {
    LOG(_L("[MediaServant]\t CMSAppUi::ReadMMCInfoL"));
    RFs& sysFs = FsSession();
    RPointerArray<CCmDriveInfo> storedDrives;
    RPointerArray<CCmDriveInfo> driveArray;
    TInt error = KErrNone;
    aCapasity = 0;
    aFreeSpace = 0;
    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();

    // read stored info of stored drives
    MSEngine()->GetDrivesL( storedDrives );
    // get system drive info
    settings->DriveListL( sysFs, driveArray );

    TInt count = storedDrives.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TIdentityRelation<CCmDriveInfo> relation( CMSAppUi::CompareIds );
        TInt index = driveArray.Find( storedDrives[i], relation );
        if ( index != KErrNotFound )
            {
            TVolumeInfo volInfo;
            error = sysFs.Volume( volInfo, storedDrives[i]->DriveNumber() );
            if ( error == KErrNone )
                {
                TRACE(Print(_L("[MediaServant]\t CMSAppUi::ReadMMCInfoL %ld\n"),
                            volInfo.iSize ));
                aCapasity += volInfo.iSize;
                aFreeSpace += volInfo.iFree;
                }
            }
        }

    storedDrives.ResetAndDestroy();
    storedDrives.Close();

    driveArray.ResetAndDestroy();
    driveArray.Close();

	settings->Close();
    CleanupStack::Pop(); // settings
    return error;
    }

// --------------------------------------------------------------------------
// CMSAppUi::GetFilledFilesSize
// Get filled files size
// --------------------------------------------------------------------------
//
void CMSAppUi::GetFilledFilesSize( TInt64& aSize, const TInt aID )
    {
    iMSEngine->GetFilledFilesSize( aSize, aID );
    }


// CMSAppUi::ParameterAgent
// returns pointer to parameter agent
// --------------------------------------------------------------------------
//
CMSParameterAgent* CMSAppUi::ParameterAgent()
    {
    return iParameterAgent;
    }

// --------------------------------------------------------------------------
// CMSAppUi::FillRuleController
// returns pointer to rule controller
// --------------------------------------------------------------------------
//
CMSFillRuleController* CMSAppUi::FillRuleController()
    {
    return iFillRuleController;
    }

// --------------------------------------------------------------------------
// CMSAppUi::FsSession
// returns file server session
// --------------------------------------------------------------------------
//
RFs& CMSAppUi::FsSession()
    {
    return iEikonEnv->FsSession();
    }

// --------------------------------------------------------------------------
// CMSAppUi::DriveSelectedL
// returns drive selection status
// --------------------------------------------------------------------------
//
TBool CMSAppUi::DriveSelectedL()
    {
    TBool status( EFalse );

    if ( iMSEngine->DriveSelectionStatus() && MemoryAttachedL() )
        {
        status = ETrue;
        }
    return status;
    }


// --------------------------------------------------------------------------
// CMSAppUi::MemoryAttachedL
// Returns ETrue if selected memory is on the device
// --------------------------------------------------------------------------
//
TBool CMSAppUi::MemoryAttachedL()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::MemoryAttachedL"));
    
    TBool status( EFalse );
    RFs& fileServer = FsSession();
    RPointerArray<CCmDriveInfo> storedDrives;
    RPointerArray<CCmDriveInfo> driveArray;
    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();

    // read stored info of stored drives
    MSEngine()->GetDrivesL( storedDrives );
    // get system drive info
    settings->DriveListL( fileServer, driveArray );

    TInt count = storedDrives.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TIdentityRelation<CCmDriveInfo> relation(CMSAppUi::CompareIds );
        TInt index = driveArray.Find( storedDrives[i], relation );
        if ( index != KErrNotFound )
            {
            status = ETrue;
            i = count; // break loop
            }
        }

    storedDrives.ResetAndDestroy();
    storedDrives.Close();

    driveArray.ResetAndDestroy();
    driveArray.Close();

	settings->Close();
    CleanupStack::Pop(); // settings

    return status;
    }

// --------------------------------------------------------------------------
// CMSAppUi::CompareIds
// Compares drive ids
// --------------------------------------------------------------------------
//
TBool CMSAppUi::CompareIds( const CCmDriveInfo& aStoredDrive,
                            const CCmDriveInfo& aDrive )
    {
    TBool retval = EFalse;
    if ( aStoredDrive.DriveId() == aDrive.DriveId() )
        {
        retval = ETrue;
        }

    return retval;
    }

// ---------------------------------------------------------------------------
// CMSAppUi::LoadSettingsPluginL
// ---------------------------------------------------------------------------
//    
void CMSAppUi::LoadSettingsPluginL()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::LoadSettingsPluginL start"));
   
    TRAPD( error, iPlugin = CMSGSInterface::NewL( KMSGSImplementationUid ) );
    if ( !error )
        {
        AddViewL( iPlugin ); // transfer ownership to CAknViewAppUi    
        }    
    
    LOG(_L("[MediaServant]\t CMSAppUi::LoadSettingsPluginL end"));
    }

// ---------------------------------------------------------------------------
// CMSAppUi::SettingsPlugin
// ---------------------------------------------------------------------------
// 
CMSGSInterface* CMSAppUi::SettingsPlugin()
    {
    return iPlugin;
    }
    
// ---------------------------------------------------------------------------
// CMSAppUi::CheckDrivesSelectedL
// ---------------------------------------------------------------------------
// 
void CMSAppUi::CheckDrivesSelectedL()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::CheckDrivesSelectedL"));
    
    TBool status( EFalse );
    RFs& fileServer = FsSession();
    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();
    RPointerArray<CCmDriveInfo> storedDrives;
    RPointerArray<CCmDriveInfo> driveArray;
    CleanupClosePushL( storedDrives );
    CleanupClosePushL( driveArray );

    // read stored info of stored drives
    MSEngine()->GetDrivesL( storedDrives );
    // get system drive info
    settings->DriveListL( fileServer, driveArray );

    TInt count = storedDrives.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TIdentityRelation<CCmDriveInfo> relation(CMSAppUi::CompareIds );
        TInt index = driveArray.Find( storedDrives[i], relation );
        if ( index != KErrNotFound )
            {
            status = ETrue;
            i = count; // break loop
            }
        }
    
    // none of drives have been selected
    if( !status )
        {
        for ( TInt j = 0; j < driveArray.Count(); j++ )
            {
            // mass memory
            if ( driveArray[j]->DriveType() != DriveInfo::EDriveRemovable )
                {
                RPointerArray<CCmDriveInfo> massDrives;
                CleanupClosePushL( massDrives );
                TInt64 quota =
                ( driveArray[j]->DriveSize() * KHDDefaultQuota ) / 100;
                driveArray[j]->SetDriveQuota( quota );
                driveArray[j]->SetStatus( ETrue );
                
                massDrives.AppendL( driveArray[j] );
                MSEngine()->SetDrivesL( massDrives );
                CleanupStack::PopAndDestroy(  );// &massDrives
                //break, for only one mass memory exist
                j = driveArray.Count();
                }
            }
        }
    storedDrives.ResetAndDestroy();
    driveArray.ResetAndDestroy();
    
    CleanupStack::PopAndDestroy( 2 ); // &storedDrives, &driveArray
	settings->Close();
    CleanupStack::Pop(); // settings
    }

// ---------------------------------------------------------------------------
// CMSAppUi::RunWizard
// ---------------------------------------------------------------------------
// 
TInt CMSAppUi::RunWizardL()
	{
	LOG(_L("[MediaServant]\t CMSAppUi::RunWizardL Launch Wizard"));
	
	TInt wizRetVal(KErrNone);

    // Check drives selection status
    CheckDrivesSelectedL();
        
    CMSAppWizard* appWizard = CMSAppWizard::NewL( *iMSEngine );
    CleanupStack::PushL( appWizard );
    wizRetVal = appWizard->StartL();
    CleanupStack::PopAndDestroy( appWizard );

    if( wizRetVal != KErrNone)
        {
        LOG(_L("[MediaServant]\t CMSAppUi::RunWizardL Wizard Run OK"));        
        }
    else
        {
        TRACE(Print(_L("[MediaServant]\t RunWizardL::\
        wizard return value: %d"), wizRetVal));
        RefreshListsL();
        }
    iMSEngine->SetAppWizardState();

    return wizRetVal;
	}
// End of File
