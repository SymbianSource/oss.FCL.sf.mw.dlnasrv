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
* Description:      Main application view
*
*/






// INCLUDE FILES
#include <homemedia.rsg>
#include <GSPluginInterface.h>  // CGSPluginInterface
#include <StringLoader.h>       // StringLoader
#include <aknnotewrappers.h>    // CAknErrorNote
#include <featmgr/featmgr.h>            // FeatureManager
#include <hlplch.h>             // HlpLauncher
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntitle.h> 
#include <akninputblock.h>      // CAknInputBlock

// upnpcontroller api
#include "upnpavcontrollerfactory.h"
#include "upnpavcontroller.h"

// upnpframework api
#include "upnppluginloader.h"
#include "upnpcommonui.h"

// upnpsettings api
#include "upnpsettingsengine.h"     // CUPnPSettingsEngine
#include "upnpsettingsengine.hrh"   // CUPnPSettingsEngine
#include "cupnpappwizard.h"
#include "upnpappfilesharingdialog.h"

// upnpsharing api
#include "upnpfilesharingengine.h"

// homemedia
#include "homemediadocument.h"
#include "homemediaappview.h"
#include "homemedia.hrh"
#include "homemediacontainer.h"
#include "upnpperiodic.h"

// debug log support
_LIT( KComponentLogfile, "homemedia.txt");
#include "upnplog.h"

//CONSTANTS
static const TUid KHomeMediaAppViewId = {1}; 
static const TUid KUPnPGSPluginImpUid = { 0x20009CB0 };
static const TInt KNumberOfItemsBeforePlugins( 2 );
static const TInt KUPnPAlreadyStarted( 0 );

// Media Server shutdown delay in seconds (15s)
const TInt KMediaServerShutdownTimeout = 15000000;
// --------------------------------------------------------------------------
// ChomemediaAppView::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CHomeMediaAppView* CHomeMediaAppView::NewL()
    {
    CHomeMediaAppView* self = CHomeMediaAppView::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// ChomemediaAppView::NewLC()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CHomeMediaAppView* CHomeMediaAppView::NewLC()
    {
    CHomeMediaAppView* self = new ( ELeave ) CHomeMediaAppView;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// ChomemediaAppView::~ChomemediaAppView()
// Destructor.
// --------------------------------------------------------------------------
//
CHomeMediaAppView::~CHomeMediaAppView()
    {
    delete iFileSharing;
    delete iPluginLoader;
    delete iSettingsEngine;
    delete iNaviDecorator;
    delete iIdle;
    delete iInputBlocker;
    delete iRefreshTimer;
    }
    
// --------------------------------------------------------------------------
// ChomemediaAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::ConstructL()
    {
    BaseConstructL( R_HOMEMEDIA_MAIN_VIEW );
    ifileSharingDialog= NULL;       
    iFileSharing = CUPnPFileSharingEngine::NewL();
    iFileSharing->SetObserver( this );
    
    iPluginLoader = CUPnPPluginLoader::NewL(*this);
    iPluginArray = iPluginLoader->CreatePluginsL();
    iSettingsEngine = CUPnPSettingsEngine::NewL();
    
    TInt firstStart( 0 );
    iSettingsEngine->GetFirstStart( firstStart );    
    if( firstStart )
        {
        iInputBlocker = CAknInputBlock::NewLC();
        CleanupStack::Pop( iInputBlocker );
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        TCallBack cb( InitializeWizardL, this );
        iIdle->Start( cb );
        }
    
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::CHomeMediaAppView()
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CHomeMediaAppView::CHomeMediaAppView()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::Id()
// --------------------------------------------------------------------------
//
TUid CHomeMediaAppView::Id ()const
    {
    return KHomeMediaAppViewId;
    }
    
// --------------------------------------------------------------------------
// CHomeMediaAppView::HandleClientRectChange()
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }
        
// --------------------------------------------------------------------------
// CHomeMediaAppView::DoActivateL()
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::DoActivateL(const TVwsViewId &/*aPrevViewId*/
                                  , TUid /*aCustomMessageId*/
                                  , const TDesC8 &/*aCustomMessage*/)
    {
    if( !iContainer )
        {
        iContainer = CHomeMediaContainer::NewL( 
            ClientRect(), *iFileSharing, iPluginArray );
        iContainer->SetMopParent(this);
        AppUi()->AddToStackL( *this, iContainer );
        
        UpdateTitlePaneL();
        UpdateNaviPaneL( ETrue );  
        }
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::DoDeactivate()
// --------------------------------------------------------------------------
// 
void CHomeMediaAppView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    iContainer = NULL;
    }
    
// --------------------------------------------------------------------------
// CHomeMediaAppView::HandleCommandL()
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::HandleCommandL(TInt aCommand)
    {
    __LOG("CHomeMediaAppView::HandleCommandL");

    switch (aCommand)
        {
        case EAknSoftkeyOpen: // fall through
        case EAknCmdOpen :    // fall through
            {
            TInt index = iContainer->ListBox().CurrentItemIndex();
            if ( index == EHomeMediaFileSharingDialog )
                {
                UpdateNaviPaneL( EFalse );
                CUPnPAppFileSharingDialog* dlg =
                    CUPnPAppFileSharingDialog::NewL( 0, *iFileSharing );
                ifileSharingDialog= dlg;         
                dlg->ExecuteSharingDialogLD();
                dlg = NULL;
                ifileSharingDialog= NULL;        
                iContainer->UpdateL();
                }
            else if ( index == EHomeMediaBrowseHome )
                {
                ExecuteBrowseL();
                }
            else
                // plugin was chosen
                {
                LoadPluginL();
                }
            break;
            }
        case EHomeMediaCmdRunWizard :
            {
            StartWizardL();
            iContainer->UpdateL();
            break;
            }
        case EHomeMediaCmdSettings:
            { 
            StartSettingsViewL();
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                  iEikonEnv->WsSession(),
                  iEikonEnv->EikAppUi()->AppHelpContextL() );
            break;
            }
        case EAknSoftkeyExit :
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }
        default:
            {
            break;
            }
        }
    //Set observer back, because other ui componets might change observer
    iFileSharing->SetObserver( this );
    if( aCommand != EHomeMediaCmdSettings )
        {
        UpdateTitlePaneL();
        UpdateNaviPaneL( ETrue );        
        }
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::DynInitMenuPaneL()
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::DynInitMenuPaneL( TInt aResourceId, 
                                          CEikMenuPane* aMenuPane )
    {
    __LOG("CHomeMediaAppView::DynInitMenuPaneL");

    // hide help option if not supported
    if (aResourceId == R_HOMEMEDIA_MENU &&
        !FeatureManager::FeatureSupported(KFeatureIdHelp))
        {
        aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPAppMainDialog::StepL
// Step done during wait note
// From MAknBackgroundProcess
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::StepL(void)
    {
    if ( iBrowseState == EPhaseNotActive )
        {
        // try to start avcontroller
        // handle leaves in CycleError
        iAvControl = UPnPAVControllerFactory::NewUPnPAVControllerL();
        iBrowseState = EPhaseCompleted;
        }
    }


// --------------------------------------------------------------------------
// CUPnPAppMainDialog::IsProcessDone
// Returns ETrue if process finished
// From MAknBackgroundProcess
// --------------------------------------------------------------------------
//
TBool CHomeMediaAppView::IsProcessDone(void) const
    {
    TBool ret(EFalse);
    if ( iBrowseState == EPhaseNotActive )
        {
        // try to start avcontroller
        }
    else if ( iBrowseState == EPhaseCompleted )
        {
        ret = ETrue;
        }
    else
        {
        __LOG( "CHomeMediaAppView::IsProcessDone: else branch" );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPAppMainDialog::CycleError
// Handle error
// From MAknBackgroundProcess
// --------------------------------------------------------------------------
//
TInt CHomeMediaAppView::CycleError( TInt aError )
    {
    __LOG( "CHomeMediaAppView::CycleError" );
    delete iAvControl;
    iAvControl = NULL;
    return aError;
    }

    
// --------------------------------------------------------------------------
// CHomeMediaAppView::HandleSharingStatus
// Function informs when file sharing was enabled or disabled
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::HandleSharingStatus( 
    CUPnPFileSharingEngine& /*aEngine*/, 
    TInt /*aError*/, 
    TBool /*aPhase*/ )
    {
    __LOG("CHomeMediaAppView::HandleSharingStatus begin");
    if( iContainer )
        {
        TRAPD( err, iContainer->UpdateL() );
        if( err )
            {
            __LOG1( "error=%d", err );
            }
        }
    __LOG("CHomeMediaAppView::HandleSharingStatus end");
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::HandleSharingConnectionLost
// Callback to inform if connection has been lost
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::HandleSharingConnectionLost( 
    CUPnPFileSharingEngine& /*aEngine*/ )
    {
    __LOG("CHomeMediaAppView::HandleSharingConnectionLost begin");
    if( iContainer )
        {
        TRAPD( err, iContainer->UpdateL() );
        if( err )
            {
            __LOG1( "iContainer->UpdateL error=%d", err );
            }
        TRAP( err, UpdateNaviPaneL( ETrue ) );
        
        if( err )
            {
            __LOG1( "UpdateNaviPaneL error=%d", err );
            }
        
        }
    __LOG("CHomeMediaAppView::HandleSharingConnectionLost end");
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::PluginsUpdated
// Called by UPnPPluginLoader when plugin(s) has been updated
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::PluginsUpdated()
    {
    __LOG("CHomeMediaAppView::PluginsUpdated begin");
    if( iContainer )
        {
        TRAPD( err, iContainer->UpdateL() );
        if( err )
            {
            __LOG1( "error=%d", err );
            }
        }
    __LOG("CHomeMediaAppView::PluginsUpdated end");
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::ExecuteBrowseL
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::ExecuteBrowseL()
    {
    __LOG("CHomeMediaAppView::ExecuteBrowseL begin");
    TInt leaveCode( KErrNone );
    
    if( CheckAccessPointValid() )
        {
        UpdateNaviPaneL( EFalse );
        TRAP( leaveCode, DoExecuteBrowseL() );
        }
    else
        {
        TInt ret = StartWizardL();
        // ret: 0 when wizard is canceled
        // ret: -2 when wizard steps are completed
        if( ret ) 
            {
            UpdateNaviPaneL( EFalse );
            TRAP( leaveCode, DoExecuteBrowseL() );
            }
        }    
    
    // Inform user if browse failed
    // KErrCancel is returned when user selectes NO to WLAN query
    if( leaveCode && 
        leaveCode != KErrCancel )
        {
        ShowErrorNoteL( R_HOMEMEDIA_ERROR_CON_FAILED_TEXT );
        }
        
    __LOG("CHomeMediaAppView::ExecuteBrowseL end");    
    }
    
// --------------------------------------------------------------------------
// CHomeMediaAppView::DoExecuteBrowseL
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::DoExecuteBrowseL()
    {
    __LOG("CHomeMediaAppView::DoExecuteBrowseL begin");
    iBrowseState = EPhaseNotActive;

    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
    waitNoteWrapper->ExecuteL(
      R_HOMEMEDIA_CONNECTING_DIALOG,
      *this,
      ETrue); //ETrue = show immediately
    CleanupStack::PopAndDestroy(waitNoteWrapper);
    
    if ( iAvControl )
        {
        CUPnPCommonUI *commonui = CUPnPCommonUI::NewL();
        CleanupStack::PushL( commonui );

        TInt returnValue( KErrNone );
        returnValue = commonui->ExecuteDeviceDialogL( *iAvControl );

        CleanupStack::PopAndDestroy( commonui );
        delete iAvControl;
        iAvControl = NULL;
        if ( returnValue == EAknCmdExit ||
             returnValue == EEikCmdExit )
            {
            User::Exit( KErrNone );
            }
        }
    __LOG("CHomeMediaAppView::DoExecuteBrowseL end");    
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::StartWizardL
// --------------------------------------------------------------------------
//    
TInt CHomeMediaAppView::StartWizardL()
    {
    //Start wizzard
    
    CUPnPAppWizard* wiz = 
        CUPnPAppWizard::NewL( KNullDesC, iFileSharing );
    CleanupStack::PushL( wiz );
    TInt ret = wiz->StartL();
    CleanupStack::PopAndDestroy( wiz );
    
    return ret;
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::IAP()
// --------------------------------------------------------------------------
//    
TInt CHomeMediaAppView::IAP() const
    {
    TInt iapId( EUPnPSettingsEngineIAPIdNone );
    iSettingsEngine->GetAccessPoint( iapId );
    return iapId;
    }
 
// --------------------------------------------------------------------------
// CHomeMediaAppView::CheckAccessPointValid
// Check the current selected iap's validity
// --------------------------------------------------------------------------
//  
TBool CHomeMediaAppView::CheckAccessPointValid()
    {
    //check if the selected iap is still valid
    __LOG( "CHomeMediaAppView::CheckAccessPointValid()" );
    
    TBool validiap = EFalse;
    TRAPD(err, validiap = CUPnPSettingsEngine::IsAccessPointValidL( IAP() ) );
                        
    if( !err && validiap )
        {
        __LOG( "CheckAccessPointValid ETrue" );
        return ETrue;
        }
    else
        {
        __LOG( "etAccessPoint" );
        iSettingsEngine->SetAccessPoint( EUPnPSettingsEngineIAPIdNone );

        /**
         * These two APIs will be removed after the CUPnPSettingsEngine
         * is refactoryed
         */
        
        iSettingsEngine->SetWapId( 
                EUPnPSettingsEngineIAPItemNone );
        iSettingsEngine->SetAccessPointSetting( 
                EUPnPSettingsEngineIAPItemNone );
        
        __LOG( "CheckAccessPointValid EFalse" );  
        return EFalse;                
        }
    }    
   
// --------------------------------------------------------------------------
// CHomeMediaAppView::ShowErrorNoteL
// Shows note with error message
// --------------------------------------------------------------------------
//
void CHomeMediaAppView::ShowErrorNoteL( TInt aMain ) const
    {
    __LOG("CHomeMediaAppView::ShowErrorNoteL begin");

    // show error note
    HBufC* errorNote = StringLoader::LoadLC( aMain );
    CAknErrorNote* note = new (ELeave) CAknErrorNote(ETrue);
    note->ExecuteLD(*errorNote);
    CleanupStack::PopAndDestroy(errorNote);
    __LOG("CHomeMediaAppView::ShowErrorNoteL end");
    }

//---------------------------------------------------------------------------
// CHomeMediaAppView::UpdateTitlePaneL()
//---------------------------------------------------------------------------
//
void CHomeMediaAppView::UpdateTitlePaneL()
    {
    __LOG("CHomeMediaAppView::UpdateTitlePaneL begin");
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    HBufC* titlePaneTxt = StringLoader::LoadLC( 
        R_HOMEMEDIA_TITLE_PANE_TEXT );
    titlePane->SetTextL( *titlePaneTxt );
    CleanupStack::PopAndDestroy( titlePaneTxt );
    __LOG("CHomeMediaAppView::UpdateTitlePaneL begin");
    }
    
//---------------------------------------------------------------------------
// CHomeMediaAppView::UpdateNaviPaneL()
//---------------------------------------------------------------------------
//
void CHomeMediaAppView::UpdateNaviPaneL( TBool aShowStatus )
    {
    __LOG("CHomeMediaAppView::UpdateNaviPaneL begin");
    TUid naviPaneUid;
    naviPaneUid.iUid = EEikStatusPaneUidNavi;
    CEikStatusPane* statusPane  = StatusPane();
    CEikStatusPaneBase::TPaneCapabilities subPane =
            statusPane->PaneCapabilities(naviPaneUid);
    
    if(subPane.IsPresent()&&subPane.IsAppOwned())
        {
        CAknNavigationControlContainer* naviPane = 
         (CAknNavigationControlContainer*) statusPane->ControlL(naviPaneUid);
         
        if( aShowStatus )
            {
            delete iNaviDecorator;
            iNaviDecorator = NULL;
            HBufC* naviPaneTxt = NULL;
            
            if( iFileSharing->SharingStateL() )
                {
                TInt iapId( EUPnPSettingsEngineIAPIdNone );
                iSettingsEngine->GetAccessPoint( iapId );
                if( iapId == EUPnPSettingsEngineIAPIdAlwaysAsk )
                    {
                    __LOG("alw ask");
                    naviPaneTxt = StringLoader::LoadLC(
                        R_HOMEMEDIA_NAVI_CONNECTED_TEXT );
                    }
                else
                    {
                    naviPaneTxt = 
                            CUPnPSettingsEngine::GetCurrentIapNameL( iapId );
                    CleanupStack::PushL( naviPaneTxt );
                    __LOG16( *naviPaneTxt );
                    }
                }
            else
                {
                naviPaneTxt = StringLoader::LoadLC( 
                    R_HOMEMEDIA_NAVI_NOT_CONNECTED_TEXT );
                }    

            iNaviDecorator = naviPane->CreateNavigationLabelL(*naviPaneTxt);
            naviPane->PushL(*iNaviDecorator);
            CleanupStack::PopAndDestroy( naviPaneTxt );
            }
        else    
            {
            if ( iNaviDecorator )
                {
                naviPane->Pop( iNaviDecorator );
                }
            // destroy iNaviDecorator
            delete iNaviDecorator;
            iNaviDecorator = NULL;
            }
        }    
    __LOG("CHomeMediaAppView::UpdateNaviPaneL begin");
    }

//---------------------------------------------------------------------------
// CHomeMediaAppView::LoadPluginL()
//---------------------------------------------------------------------------
//
void CHomeMediaAppView::LoadPluginL()
    {
    __LOG("CHomeMediaAppView::LoadPluginL begin");
    TInt iapId( IAP() );
    TInt index = iContainer->ListBox().CurrentItemIndex() - 
                 KNumberOfItemsBeforePlugins;
    if ( iPluginArray[index]->RequiresUpnpConfiguration() &&
         iapId <= EUPnPSettingsEngineIAPIdNone )
        {
        //Start wizzard
        StartWizardL();

        // get the apid again
        iapId = IAP();

        //if apid is set,get into the plugin view
        if ( iapId > EUPnPSettingsEngineIAPIdNone )
            {
            // after wizard has been run, launch plugin
            iPluginArray[index]->ExecuteL();
            }
        }
    else
        {
        iPluginArray[index]->ExecuteL();
        }
    __LOG("CHomeMediaAppView::LoadPluginL begin");
    }
    
//---------------------------------------------------------------------------
// CHomeMediaAppView::StartSettingsViewL()
//---------------------------------------------------------------------------
//    
void CHomeMediaAppView::StartSettingsViewL()
    {
    CAknViewAppUi* appUi = AppUi();
    if( !appUi->View( KUPnPGSPluginImpUid ) )
        {
        // Creates GS plugin implementation. CGSPluginInterface is
        // inherited from the CAknView.
        CGSPluginInterface* settingsView = CGSPluginInterface::NewL( 
                        KUPnPGSPluginImpUid, NULL );
        CleanupStack::PushL( settingsView );
        
        // Registers destructor key to document object.
        // Ecom plugin implementation will be destroyed after view
        // destruction.
        CHomeMediaDocument* doc = static_cast<CHomeMediaDocument*>( 
                appUi->Document() );
        doc->RegisterEcomDestructorKeyL( 
                settingsView->GetEcomDestructorKey() );
        
        // Adds the view to system view framework.
        appUi->AddViewL( settingsView ); //ownership transferred
        CleanupStack::Pop( settingsView );
        }
    
    UpdateNaviPaneL( EFalse );
    
    // We check the existence of gsplugin view with hardcoded uid,
    // we active the view also with the same uid to detect
    // possible changes better (leaves if view isn't found).
    appUi->ActivateLocalViewL( KUPnPGSPluginImpUid );            
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::InitializeWizardL
// callback to run wizard
// --------------------------------------------------------------------------
//
TBool CHomeMediaAppView::InitializeWizardL( TAny* aPtr )
    {
    __LOG("[UPNP_HOMECONNECT]\t CUPnPAppMainDialog::InitializeWizardL \
begin");
    CHomeMediaAppView* main = static_cast< CHomeMediaAppView* >( aPtr );
    if( main )
        {
        if ( main->iInputBlocker )
            {
            delete main->iInputBlocker;
            main->iInputBlocker = NULL;
            }
        main->StartWizardL();
        main->iSettingsEngine->SetFirstStart( KUPnPAlreadyStarted ); 
        main->UpdateTitlePaneL();
        main->UpdateNaviPaneL( ETrue );
        if( main->iContainer )
            {
             main->iContainer->UpdateL();
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::RefreshSharelistItemL
// if get the correct pointer ,refresh the correspond item 
// in view or share dialog
// --------------------------------------------------------------------------
//  
void CHomeMediaAppView::RefreshSharelistItemL() 
    { 
    if( iContainer )    
        {
        iContainer->UpdateL();
        }
   
    if( ifileSharingDialog )
        {
        ifileSharingDialog->UpdateFileSharingListL();
        }  
    
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::RefreshVisibilitySettingItemL
// First refresh the share item either in the CHomeMediaAppView or in the 
// CUPnPAppFileSharingList,it is ten seconds that shutdown 
// mediaserver,so need a timer refresh the items in time .
// --------------------------------------------------------------------------
//  
void CHomeMediaAppView::RefreshVisibilitySettingItemL() 
    {
    RefreshSharelistItemL();
    if( iRefreshTimer )    
        {
        iRefreshTimer->Cancel();  
        delete iRefreshTimer; 
        iRefreshTimer = NULL;
        }
     
    iRefreshTimer = CUPnPPeriodic::NewL( CActive::EPriorityIdle );
    TCallBack updatelist( RefreshSharingL, this );
    iRefreshTimer->Start( KMediaServerShutdownTimeout, 
        KMediaServerShutdownTimeout, updatelist );
  
    }

// --------------------------------------------------------------------------
// CHomeMediaAppView::RefreshSharingL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CHomeMediaAppView::RefreshSharingL( TAny* aPtr )
    { 
    CHomeMediaAppView* view( NULL );
    view = static_cast<CHomeMediaAppView*>( aPtr );
    view->RefreshSharelistItemL();
  
    view->iRefreshTimer->Cancel();
    delete view->iRefreshTimer;
    view->iRefreshTimer = NULL;
    
    return KErrNone;
    }

//  End of File
