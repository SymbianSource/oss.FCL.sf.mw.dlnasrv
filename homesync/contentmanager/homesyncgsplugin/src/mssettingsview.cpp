/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMSSettingsView class implementation
*
*/


#include <msgspluginsrc.rsg>
#include <hlplch.h>
#include <GSFWViewUIDs.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <msgsplugin.mbg>
#include <akntitle.h>

#include "cmcommonutils.h"
#include "msconstants.h"
#include "mssettingitemlist.h"
#include "msgspluginuids.h"
#include "mssettingsview.h"
#include "msengine.h"
#include "msdebug.h"

// CONSTANTS
_LIT( KMediaservantRscFile, "\\resource\\msgspluginsrc.rsc" );
_LIT( KMSGSMifFileName, "\\resource\\apps\\msgsplugin.mif" );

// --------------------------------------------------------------------------
// Two phased constructor.
// --------------------------------------------------------------------------
//
CMSSettingsView* CMSSettingsView::NewL()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::NewL"));

    CMSSettingsView* self = CMSSettingsView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Two phased constructor.
// --------------------------------------------------------------------------
//
CMSSettingsView* CMSSettingsView::NewLC()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::NewLC"));

    CMSSettingsView* self = new ( ELeave ) CMSSettingsView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
	
// --------------------------------------------------------------------------
// CMSSettingsView::CMSSettingsView()
// --------------------------------------------------------------------------
//
CMSSettingsView::CMSSettingsView()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::CMSSettingsView"));
    }

// --------------------------------------------------------------------------
// CMSSettingsView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSSettingsView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::ConstructL"));

    iCoeEnv = CEikonEnv::Static();    
    // Load resource file
    iResFileOffset = CmCommonUtils::LoadResourceFileL(
                                        KMediaservantRscFile(),
                                        *iCoeEnv );
    
    BaseConstructL( R_MS_SETTINGS_VIEW );
    }

// --------------------------------------------------------------------------
// CMSSettingsView::~CMSSettingsView()
// --------------------------------------------------------------------------
//
CMSSettingsView::~CMSSettingsView()
    {
    LOG(_L("[MediaServant]\t CMSMainView::~CMSSettingsView"));
    
    ClearCurrentNaviPaneText();
    
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        }
  
    DoDeactivate();
    }
    
// --------------------------------------------------------------------------
// TUid CMSSettingsView::Id()
// --------------------------------------------------------------------------
//
TUid CMSSettingsView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::Id"));

    return KMSGSImplementationUid;
    }

// --------------------------------------------------------------------------
// CMSSettingsView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSSettingsView::HandleCommandL( TInt aCommand )
    {
    TRACE(Print(_L("[MediaServant]\t CMSSettingsView::\
HandleCommandL command = %d"), aCommand ));

    switch ( aCommand )
        {
        case EAknCmdOpen:
            {
            iSettingItemList->OpenSelectedListboxItemL();
            break;
            }
        case EAknSoftkeyBack:
            {
            TCmServerState state;
            iSettingItemList->MSEngine()->ServerState( state );
            if ( state != ECmServerStateIdle )
                {
                iSettingItemList->MSEngine()->StopOperation();
                }            

            CAknView* activeView = AppUi()->View( KMSMainViewId );
            CAknView* activeView2 = AppUi()->View( KGSMainViewUid );            
            
            if ( activeView )
                {                
                AppUi()->ActivateLocalViewL( KMSMainViewId );
                }
            else if ( activeView2 )
                {
                AppUi()->ActivateLocalViewL( KGSAppsPluginUid );
                }
            else
                {
                LOG(_L("[MediaServant]\t CMSSettingsView::HandleCommandL \
                parent view not found"));
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

// --------------------------------------------------------------------------
// CMSSettingsView::DoActivateL(...)
// --------------------------------------------------------------------------
//
void CMSSettingsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::DoActivateL"));

    // Set title pane text to default
    SetTitlePaneTextL( R_MS_GS_SETTINGS_VIEW_TITLE );

    if ( !iSettingItemList )
        {
        // create settings screen
        iSettingItemList = CMSSettingItemList::NewL( R_MS_GENERAL_SETTINGS,
                                                     *this );
        iSettingItemList->SetMopParent( this );

        // now control receives keyboard events
        AppUi()->AddToStackL( *this, iSettingItemList );
        iSettingItemList->ActivateL();

        }
   }

// --------------------------------------------------------------------------
// CMSSettingsView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSSettingsView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::DoDeactivate"));

    if ( iSettingItemList )
        {
        AppUi()->RemoveFromStack( iSettingItemList );
        }

    delete iSettingItemList; // Deletes the container class object.
    iSettingItemList = NULL;
    }
    
// --------------------------------------------------------------------------
// Method for getting caption of this plugin. 
// --------------------------------------------------------------------------
//
void CMSSettingsView::GetCaptionL( TDes& aCaption ) const
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::GetCaptionL"));
    
    HBufC* pluginText = StringLoader::LoadLC( R_MS_GS_PLUGIN_TEXT );
    aCaption.Copy( *pluginText );
    CleanupStack::PopAndDestroy( pluginText );
    }

// --------------------------------------------------------------------------
// Creates a new icon of desired type.
// --------------------------------------------------------------------------
//
CGulIcon* CMSSettingsView::CreateIconL( const TUid /*aIconType*/ )
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::CreateIconL"));

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName() );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( skin,
                             KAknsIIDDefault,
                             bitmap,
                             mask,
                             KMSGSMifFileName,
                             EMbmMsgspluginQgn_mserv_app_menu_icon,
                             EMbmMsgspluginQgn_mserv_app_menu_icon_mask );

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);
    bitmap = NULL;
    mask = NULL;

    return icon;
    }

// --------------------------------------------------------------------------
// CMSSettingsView::SetNavigationPaneTextL()
// Sets navigation pane text.
// --------------------------------------------------------------------------
//
void CMSSettingsView::SetNavigationPaneTextL(const TDesC& aText )
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::SetNavigationPaneTextL"));

    // old decorator is popped and deleted
    ClearCurrentNaviPaneText();

    if ( !iNaviPane )
        {
        iNaviPane = static_cast<CAknNavigationControlContainer*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
                // ownership of decorator is transfered to application
        }

    iNaviDecorator = iNaviPane->CreateNavigationLabelL( aText );
    iNaviPane->PushL( *iNaviDecorator );
    }

// --------------------------------------------------------------------------
// CMSSettingsView::ClearCurrentNaviPaneText()
// Clears navi pane text.
// --------------------------------------------------------------------------
//
void CMSSettingsView::ClearCurrentNaviPaneText()
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::ClearCurrentNaviPaneText"));

    if ( iNaviDecorator )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }

// --------------------------------------------------------------------------
// CMSSettingsView::SetTitlePaneTextL()
// Sets text to title pane.
// --------------------------------------------------------------------------
void CMSSettingsView::SetTitlePaneTextL( TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSSettingsView::SetTitlePaneTextL"));

    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    // Makes and sets text which is used title pane.
    if ( aResourceId == KMSDefaultTitleId )
        {
        titlePane->SetTextToDefaultL();
        }
    else
        {
        TBuf<KMSTitleBufLength> titleText( 0 );
        iCoeEnv->ReadResourceL( titleText, aResourceId );
        titlePane->SetTextL( titleText );
        }
    }

// End of File
