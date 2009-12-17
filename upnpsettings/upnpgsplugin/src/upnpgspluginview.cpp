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
* Description:      CUPnPGSPluginView class implementation
*
*/







#include <upnpgspluginrsc.rsg>
#include <hlplch.h>
#include <avkon.mbg>
#include <bautils.h>
#include <gsfwviewuids.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <akntitle.h>
#include <pathinfo.h>
#include <upnpgspluginrsc.mbg>

#include "upnpappsettingslist.h"
#include "upnpfilesharingengine.h"
#include "upnpgsplugin.hrh"
#include "upnpgspluginview.h"
#include "upnpsettingsengine.h"

_LIT( KComponentLogfile, "upnpgsplugin.txt");
#include "upnplog.h"

// CONSTANTS
const TUid KUPnPGSPluginImpUid = { KUPnPGSPluginImplementationUid };

_LIT( KUPnPGSPluginRscFile, "\\resource\\upnpgspluginrsc.rsc" );
_LIT( KAknHomeConnectMbmFileName, "\\resource\\apps\\upnpgspluginrsc.mif" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Two phased constructor.
// --------------------------------------------------------------------------
//
CUPnPGSPluginView* CUPnPGSPluginView::NewL()
    {
    CUPnPGSPluginView* self = new ( ELeave ) CUPnPGSPluginView();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::CUPnPGSPluginView()
// --------------------------------------------------------------------------
//
CUPnPGSPluginView::CUPnPGSPluginView()
    {
    __LOG("CUPnPGSPluginView::CUPnPGSPluginView");
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::ConstructL()
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::ConstructL()
    {
        __LOG( "CUPnPGSPlugin::ConstructL" );
    // Get the pointer to coe env (owned by Eikon env)
    iCoeEnv = CEikonEnv::Static();
    
    if( !iCoeEnv )
        {
        User::Leave( KErrNotSupported );
        }

    // Get the filesession reference from coe env
    RFs& fileSession = iCoeEnv->FsSession();

    // Load resource file
    TFileName rscFileName( KUPnPGSPluginRscFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );

    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        __LOG( "[UpnpCommand]\t Resource file does not exist!" );
        User::Leave( KErrNotFound );
        }

    // Read the resource file offset
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );

    BaseConstructL( R_GS_UPNP_VIEW );
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::~CUPnPGSPluginView()
// --------------------------------------------------------------------------
//
CUPnPGSPluginView::~CUPnPGSPluginView()
    {
    __LOG("CMSMainView::~CUPnPGSPluginView");

    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        }

    DoDeactivate();
    }

// --------------------------------------------------------------------------
// TUid CUPnPGSPluginView::Id()
// --------------------------------------------------------------------------
//
TUid CUPnPGSPluginView::Id() const
    {
    __LOG("CUPnPGSPluginView::Id");

    return KUPnPGSPluginImpUid;
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        
        case EAknSoftkeyChange:
            {
            TInt selectedItem = 
                iSettingItemList->ListBox()->CurrentItemIndex();
            iSettingItemList->EditItemL(selectedItem, ETrue);
            
            // fix ESLX-7LYLR7
            iSettingItemList->DrawNow();
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->ActivateLocalViewL( iViewToClose.iViewUid );
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
// CUPnPGSPluginView::DoActivateL(...)
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::DoActivateL(
   const TVwsViewId& aPrevViewId, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
    {
    __LOG("CUPnPGSPluginView::DoActivateL");

    iViewToClose = aPrevViewId;

    // Set title pane text to default
    SetTitlePaneTextL( R_UPNP_GS_SETTINGS_VIEW_TITLE );

    if( !iFileSharing )
        {
        iFileSharing = CUPnPFileSharingEngine::NewL();
        iFileSharing->SetObserver( this );
        }

    if ( !iSettingItemList )
        {
        iSharingState = static_cast<TInt>( iFileSharing->SharingStateL() );
        // create settings screen
        iSettingItemList = 
                       CUPnPAppSettingsList::NewL( iSharingState,
                                                   R_UPNP_GS_SETTINGS_LIST );
        iSettingItemList->SetMopParent( this );
        iSettingItemList->LoadDataFromEngineL();
        // now control receives keyboard events
        AppUi()->AddToStackL( *this, iSettingItemList );
        iSettingItemList->ActivateL();
        }

   }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::DoDeactivate()
    {
    __LOG("CUPnPGSPluginView::DoDeactivate");

    if( iFileSharing )
        {
        iFileSharing->SetObserver( NULL );
        }
    delete iFileSharing;
    iFileSharing = NULL;

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
void CUPnPGSPluginView::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* pluginText = StringLoader::LoadLC( 
                                        R_UPNP_GS_SETTINGS_VIEW_CAPTION );
    aCaption.Copy( *pluginText );
    CleanupStack::PopAndDestroy( pluginText );
    }

// --------------------------------------------------------------------------
// Creates a new icon of desired type.
// --------------------------------------------------------------------------
//
CGulIcon* CUPnPGSPluginView::CreateIconL( const TUid /*aIconType*/ )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( 
                         skin,
                         KAknsIIDDefault,
                         bitmap,
                         mask,
                         KAknHomeConnectMbmFileName,
                         EMbmUpnpgspluginrscQgn_menu_upnp_homemedia,
                         EMbmUpnpgspluginrscQgn_menu_upnp_homemedia_mask );
                             
    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);
    bitmap = NULL;
    mask = NULL;

    return icon;
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::SetTitlePaneTextL()
// Sets text to title pane.
// --------------------------------------------------------------------------
void CUPnPGSPluginView::SetTitlePaneTextL( TInt aResourceId )
    {
    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    TBuf<KMaxFileName> titleText( 0 );
    iCoeEnv->ReadResourceL( titleText, aResourceId );
    titlePane->SetTextL( titleText );
    }

// --------------------------------------------------------------------------
// CUPnPGSPlugin::DynInitMenuPaneL()
//
// dynamically handle help item if not supported
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::DynInitMenuPaneL( TInt aResourceId, 
                                          CEikMenuPane* aMenuPane )
    {   
    if( aResourceId == R_HOMECONNECT_SETTINGS_MENU )
        {
        User::LeaveIfNull( aMenuPane );
        
        if ( FeatureManager::FeatureSupported( KFeatureIdHelp ))
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPGSPlugin::OnSharingStatusChanged
// From MUPnPFileSharingEngineObserver
// Function informs when file sharing was enabled or disabled
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::OnSharingStatusChanged( 
    TInt aError, TBool aEnabled )
    {
    __LOG("CUPnPGSPlugin::OnSharingStatusChanged");
    if ( !aError )
        {
        iSharingState = aEnabled;
        }
    }

// --------------------------------------------------------------------------
// CUPnPGSPluginView::OnSharingConnectionLost
// From MUPnPFileSharingEngineObserver
// Callback to inform if connection has been lost
// --------------------------------------------------------------------------
//
void CUPnPGSPluginView::OnSharingConnectionLost()
    {
    iSharingState = EFalse;
    }
    
// End of File

