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
* Description:  CMSMainView class implementation
*
*/



#include <mediaservant.rsg>
#include <StringLoader.h>
#include <hlplch.h>
#include <driveinfo.h>

#include "msgspluginuids.h"
#include "msstorelistcontroller.h"
#include "cmsettingsfactory.h"
#include "cmsettings.h"
#include "cmdriveinfo.h"
#include "msmaincontainer.h"
#include "mediaservant.hrh"
#include "msgsinterface.h"
#include "msengine.h"
#include "msconstants.h"
#include "msmainview.h"
#include "msappui.h"
#include "msdebug.h"
#include "cmsappwizard.h"


// --------------------------------------------------------------------------
// CMSMainView::CMSMainView()
// --------------------------------------------------------------------------
//
CMSMainView::CMSMainView(CMSAppUi& aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSMainView::CMSMainView"));

    iAppUi = &aAppUi;
    iCheckMMC = ETrue;

    }

// --------------------------------------------------------------------------
// CMSMainView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSMainView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSMainView::ConstructL"));

    BaseConstructL( R_MS_MAIN_VIEW );

    TInt error( KErrNone );
    TBool runWizard( EFalse );

    // Check if wizard has been executed:
    runWizard = iAppUi->MSEngine()->GetAppWizardState( error );

    TRACE( Print(_L("[MediaServant]\t CMSMainView::ConstructL \
                    wizard state error = %d"), error ));

    if ( error == KErrNone && !runWizard )
        {
        // prevent user events handling
        iWizardInitialization = ETrue;
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        TCallBack cb( InitializeWizardL, this );
        iIdle->Start( cb );
        }
    else
        {
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        TCallBack cb( InitializeMemoryCardL, this );
        iIdle->Start( cb );
        }
    }

// --------------------------------------------------------------------------
// CMSMainView::~CMSMainView()
// --------------------------------------------------------------------------
//
CMSMainView::~CMSMainView()
    {
    LOG(_L("[MediaServant]\t CMSMainView::~CMSMainView"));

    delete iIdle;

    DoDeactivate();
    }
// --------------------------------------------------------------------------
// TUid CMSMainView::Id()
// --------------------------------------------------------------------------
//
TUid CMSMainView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSMainView::Id"));

    return KMSMainViewId;
    }

// --------------------------------------------------------------------------
// CMSFillView::FocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSMainView::FocusIndex() const
    {
    return iContainer->SelectedItemIndex();
    }

// --------------------------------------------------------------------------
// CMSMainView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSMainView::HandleCommandL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSMainView::HandleCommandL"));

    if ( !iWizardInitialization )
        {
        switch ( aCommand )
            {
            case EMSCmdSelect:
            // fall through
            case EAknCmdOpen:
            // fall through:
            case EAknSoftkeyOpen:
            // fall through:
            case EAknSoftkeySelect:
                {
                iContainer->OpenSelectedListboxItemL();
                break;
                }

            case EMSCmdStartHarvest:
                {
                iAppUi->MSEngine()->ScanMediaServersL();
                iContainer->ShowRefreshIndicationL();
                break;
                }

            case EMSCmdStopHarvest:
                {
                // clear navi pane
                ClearCurrentNaviPaneText();

                iAppUi->MSEngine()->StopOperation();
                iContainer->StopRefreshL();
                break;
                }

            case EMSCmdSettings:
                {
                if ( iAppUi->SettingsPlugin() )
                    {
                    // activate plugin
                    iAppUi->ActivateLocalViewL(
                                iAppUi->SettingsPlugin()->Id() );
                    }
                break;
                }
            case EMSCmdRunWizard:
                {
                LOG(_L("[MediaServant]\t CMSMainView::\
                        HandleCommandL WIZARD"));
                TBool wizRetVal(KErrNone);
                CMSAppWizard* wiz =
                    CMSAppWizard::NewL( *iAppUi->MSEngine() );
                CleanupStack::PushL( wiz );
                wizRetVal = wiz->StartL();

                iAppUi->RefreshListsL();
                CleanupStack::PopAndDestroy( wiz );

                if( wizRetVal != KErrNone )
                    {
                    // Start harvest:
                    LOG(_L("[MediaServant]\t CMSMainView::\
                            HandleCommandL WIZARD, Start Harvest"));
                    iContainer->SetManualWizardInfo( ETrue );
                    iContainer->StartMetadataDeleteL();
                    iContainer->ShowRefreshIndicationL();
                    }

                break;
                }

            case EAknCmdHelp :
                {

                TRAP_IGNORE( HlpLauncher::LaunchHelpApplicationL(
                            iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() ));
                break;
                }

            default:
                {
                AppUi()->HandleCommandL( aCommand );
                break;
                }
            }
        }
    }



// --------------------------------------------------------------------------
// CMSMainView::DoActivateL(...)
// --------------------------------------------------------------------------
//
void CMSMainView::DoActivateL(
   const TVwsViewId& aPrevViewId, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSMainView::DoActivateL"));
    if( KMSGSImplementationUid == aPrevViewId.iViewUid )
        {
        // Refresh store and fill rules array from server.
        iAppUi->RefreshListsL();

        CCmStoreRuleContainer* container =
                    ( iAppUi->StoreListController() )->StoreListContainer();
        // update the Table transferInfo
        iAppUi->MSEngine()->SetStoreRulesL( container );
        }

    // Set title pane text to default
    SetTitlePaneTextL( KMSDefaultTitleId );

    if ( !iContainer )
        {
        iContainer = new (ELeave) CMSMainContainer( *iAppUi,
                                                    *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
   }

// --------------------------------------------------------------------------
// CMSMainView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSMainView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSMainView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;

    ClearCurrentNaviPaneText();
    }

// --------------------------------------------------------------------------
// CMSMainView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CMSMainView::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane)
    {
    LOG(_L("[MediaServant]\t CMSMainView::DynInitMenuPaneL"));

    TCmServerState state;

    if (aResourceId == R_MAIN_VIEW_MENU )
        {
        iAppUi->MSEngine()->ServerState( state );

        switch ( state )
            {
            case ECmServerStateHarvesting:
                {
                aMenuPane->SetItemDimmed(EMSCmdStartHarvest, ETrue);
                aMenuPane->SetItemDimmed(EMSCmdSettings, ETrue);
                aMenuPane->SetItemDimmed(EMSCmdRunWizard, ETrue);
                break;
                }
            case ECmServerStateDeleting:
            // fall through
            case ECmServerStateFilling:
            // fall through
            case ECmServerStateStoring:
            // fall through
            case ECmServicePreProcessingStore:
            // fall through
            case ECmServicePreProcessingFill:
                {
                aMenuPane->SetItemDimmed(EMSCmdStartHarvest, ETrue);
                aMenuPane->SetItemDimmed(EMSCmdStopHarvest, ETrue);
                aMenuPane->SetItemDimmed(EMSCmdSettings, ETrue);
                aMenuPane->SetItemDimmed(EMSCmdRunWizard, ETrue);
                break;
                }
            default:
                {
                aMenuPane->SetItemDimmed(EMSCmdStopHarvest, ETrue);
                }
            }

        // sync item highlighted
        if ( iContainer->SelectedItemIndex() == 0 )
            {
            aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMSCmdSelect, ETrue );
            }

        }
    }

// --------------------------------------------------------------------------
// CMSMainView::SetCbaL()
// Changes softkeys
// --------------------------------------------------------------------------
//
void CMSMainView::SetCbaL( TBool aHighlightedIndex )
    {
    CEikButtonGroupContainer* cba = Cba();

    if ( aHighlightedIndex == EStoreAndFill )
        {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT );
        }
    else
        {
        cba->SetCommandSetL( R_MS_SOFTKEYS_OPTIONS_BACK__OPEN );
        }
    cba->DrawDeferred();
    }

// --------------------------------------------------------------------------
// CMSMainView::CheckMemoryCardL
// Checks if memory card has been changed
// --------------------------------------------------------------------------
//
void CMSMainView::CheckMemoryCardL()
    {
    LOG(_L("[MediaServant]\t CMSMainView::CheckMemoryCardL"));

    RFs& fileServer = iAppUi->FsSession();
    RPointerArray<CCmDriveInfo> storedDrives;
    CleanupClosePushL( storedDrives );
    RPointerArray<CCmDriveInfo> driveArray;
    CleanupClosePushL( driveArray );

    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();

    // read stored info of stored drives
    iAppUi->MSEngine()->GetDrivesL( storedDrives );

    // get system drive info
    settings->DriveListL( fileServer, driveArray );

    TInt count = driveArray.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TRACE(
            Print(_L("[MediaServant]\t CMSMainView::drivearray type = %d"), \
            driveArray[i]->DriveType()));

        // check if mmc card is found
        if ( driveArray[i]->DriveType() == DriveInfo::EDriveRemovable )
            {
            // check if memory card is used before
            TIdentityRelation<CCmDriveInfo> relation(CMSAppUi::CompareIds );
            TInt index = storedDrives.Find( driveArray[i], relation );
            if ( index == KErrNotFound )
                {
                ShowAndHandleMMCQueryL( *&storedDrives,
                                        *&driveArray,
                                        i );

                iAppUi->MSEngine()->ExecuteStorePreProcessingL();

                i = count; // break loop
                }
            }
        }

    settings->Close();
    CleanupStack::Pop(); // settings
    driveArray.ResetAndDestroy();
    storedDrives.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2, &storedDrives );
    }

// --------------------------------------------------------------------------
// CMSMainView::ShowAndHandleMMCQueryL
// Shows mmc query and handles selection
// --------------------------------------------------------------------------
//
void CMSMainView::ShowAndHandleMMCQueryL(
                                RPointerArray<CCmDriveInfo>& aStoredArray,
                                RPointerArray<CCmDriveInfo>& aDriveArray,
                                TInt index )
    {
    LOG(_L("[MediaServant]\t CMSMainView::ShowAndHandleMMCQueryL"));

    // Show delete files query
    CAknQueryDialog* query= CAknQueryDialog::NewL(
                                     CAknQueryDialog::ENoTone );

    if ( query->ExecuteLD( R_MSERV_MMC_QUERY ) )
        {
        TInt driveCount = aStoredArray.Count();
        // search and delete existing mmc card info
        for ( TInt i = driveCount - 1 ; i >= 0 ; i-- )
            {
          TRACE( Print( _L("[MediaServant]\t CMSAppUi::storedarray \
                  type = %d"), aStoredArray[i]->DriveType() ) );
            if ( aStoredArray[i]->DriveType() == DriveInfo::EDriveRemovable )
                {
                delete aStoredArray[i];
                aStoredArray[i] = NULL;
                aStoredArray.Remove(i);
                LOG(_L("[MediaServant]\t CMSMainView::CheckMemoryCardL \
                old mmc info removed"));
                }
            }
        TInt64 quota =
               ( aDriveArray[index]->DriveSize() * KMMCDefaultQuota ) / 100;
        aDriveArray[index]->SetDriveQuota( quota );
        aDriveArray[index]->SetStatus( ETrue );
        // then append new one
        aStoredArray.Append( aDriveArray[index] );
        // ownership transferred->remove pointer from drivearray
        aDriveArray.Remove(index);

        // store drives
        iAppUi->MSEngine()->SetDrivesL( aStoredArray );
        }
    }

// --------------------------------------------------------------------------
// CMSMainView::InitializeWizardL
// Starts the wizard when run first time
// --------------------------------------------------------------------------
//
TBool CMSMainView::InitializeWizardL( TAny* aPtr )
    {
    CMSMainView* main = static_cast< CMSMainView* >( aPtr );

    return main->DoInitializeWizardL();
    }

// --------------------------------------------------------------------------
// CMSMainView::InitializeMemoryCardL
// Starts the new memroy card when new memory card inserted
// --------------------------------------------------------------------------
//
TInt CMSMainView::InitializeMemoryCardL( TAny* aPtr )
    {
    CMSMainView* main = static_cast< CMSMainView* >( aPtr );

    main->DoInitializeMemoryCardL();

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CMSMainView::DoInitializeWizardL
// Do starts the wizard when run first time
// --------------------------------------------------------------------------
//
TBool CMSMainView::DoInitializeWizardL()
    {
    LOG(_L("[MediaServant]\t CMSAppUi::DoInitializeWizardL"));
        TInt wizRetVal(KErrNone);

    TRAPD( error, wizRetVal = iAppUi->RunWizardL() );

    if ( error )
        {
        TRACE(Print(_L("[MediaServant]\t DoInitializeWizardL::\
        wizard error value: %d"), error));
        }

    if( wizRetVal != KErrNone)
        {
        iContainer->SetManualWizardInfo( ETrue );
        iAppUi->RefreshListsL();
        iContainer->StartMetadataDeleteL();
        iContainer->ShowRefreshIndicationL();
        }
    else
        {
        TRACE(Print(_L("[MediaServant]\t DoInitializeWizardL::\
        wizard return value: %d"), wizRetVal));
        // wizard cancelled. Check memory card
        iContainer->MemoryCardChangedL(EMemoryCardInsert);
        }

    // let view to respond user events again
    iWizardInitialization = EFalse;

    // must return EFalse because this is not called anymore
    return EFalse;
    }

// --------------------------------------------------------------------------
// CMSMainView::DoInitializeMemoryCardL
// Do Starts the new memroy card when new memory card dialog inserted
// --------------------------------------------------------------------------
//
void CMSMainView::DoInitializeMemoryCardL()
    {
    CheckMemoryCardL();
    }

// End of File
