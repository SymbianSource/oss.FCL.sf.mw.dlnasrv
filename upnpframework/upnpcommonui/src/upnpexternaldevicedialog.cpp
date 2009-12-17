/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for UPnP External device dialog
*                UI Implementation
*
*/


// INCLUDE FILES
// System
#include <aknnavide.h> // CAknNavigationDecorator
#include <barsread.h> // TResourceReader
#include <AknIconArray.h>
#include <eikclbd.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <hlplch.h>
#include <upnpstring.h>
#include <layoutmetadata.cdl.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpavbrowsingsession.h"
#include "upnpavdevicelist.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"

// common ui internal
#include "upnpadvfinddialog.h"
#include "upnpcommonui.hrh"
#include "upnpexternaldevicedialog.h"
#include <upnpcommonui.rsg>
#include <upnpcommonui.mbg> //for icons
#include "upnpcommonui.h"
#include "upnpnavipanecontainer.h"

_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KAknCommonUIMbmFileName, "\\resource\\apps\\upnpcommonui.mbm" );
const TInt KLength = 100;
_LIT(KUPNP_HLP_MAIN_EXTERNAL,"UPNP_HLP_MAIN_EXTERNAL"); 


// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::CUPnPExternalDeviceDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPExternalDeviceDialog::CUPnPExternalDeviceDialog(
                                        MUPnPAVController& aAVControl,
                                        CUPnPCommonUI& aCommonUI):
                                        iAVControl(aAVControl),
                                        iCommonUI(aCommonUI)
    {
    iPreDeviceObserver = iAVControl.DeviceObserver();
    iAVControl.RemoveDeviceObserver();
    iAVControl.SetDeviceObserver( *this );
    }
    
// Destructor
CUPnPExternalDeviceDialog::~CUPnPExternalDeviceDialog()
    {
    __LOG( "CUPnPExternalDeviceDialog::~CUPnPExternalDeviceDialog" );
    
    delete iDeviceArray;
    iAVControl.RemoveDeviceObserver();
    
    if( iPreDeviceObserver )
        {
        iAVControl.SetDeviceObserver( *iPreDeviceObserver );
        }
    
    if ( iNaviDecorator && iNaviPaneContainer )
        {
        iNaviPaneContainer->Pop( iNaviDecorator );
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    __LOG( "CUPnPExternalDeviceDialog::~CUPnPExternalDeviceDialog End." );
    }
    
// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::ConstructL( TInt aMenuResource )
    {
    __LOG( "CUPnPExternalDeviceDialog::ConstructL" );
    
    iDeviceArray = CUpnpAVDeviceList::NewL();
    
    CAknDialog::ConstructL( aMenuResource );
    __LOG( "CUPnPExternalDeviceDialog::ConstructL End" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPExternalDeviceDialog* CUPnPExternalDeviceDialog::NewL(
    TInt aMenuResource, 
    MUPnPAVController& aAVControl,
    CUPnPCommonUI& aCommonUI)
    {
    __LOG( "CUPnPExternalDeviceDialog::NewL" );

    CUPnPExternalDeviceDialog* self = 
                        new ( ELeave ) CUPnPExternalDeviceDialog( aAVControl,
                                                                aCommonUI );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuResource );
    CleanupStack::Pop();
    __LOG( "CUPnPExternalDeviceDialog::NewL End" );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::PreLayoutDynInitL()
    {
    __LOG( "CUPnPExternalDeviceDialog::PreLayoutDynInitL" );

    // Create dialog title text
    CEikStatusPane* statusPane = 
                    ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();    
    iTitlePane = ( CAknTitlePane* )statusPane->ControlL(
                    TUid::Uid( EEikStatusPaneUidTitle ) );
    iTitlePane->SetTextL( *StringLoader::LoadLC(
                                R_UPNPCOMMONUI_EXTERNAL_MEDIA_TITLE ) );
    CleanupStack::PopAndDestroy();
    
    iListBox = static_cast<CAknSingleGraphicStyleListBox*>
              ( Control( EUPnPExternDevicesListBoxId ) );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    
    
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( 3 );
    CleanupStack::PushL(icons);

    // Mif icons
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );
    
    // Folder icon
    
    TFileName mbmFileName( KAknCommonUIMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    AppendIconToArrayL( icons, skin, 
                        mbmFileName, 
                        KAknsIIDDefault, 
                        EMbmUpnpcommonuiQgn_server_icon, 
                        EMbmUpnpcommonuiQgn_server_icon_mask );
   
                                        
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
        
    CleanupStack::Pop( icons );

    // Enable horizontal scrolling
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    DisplayMediaServersL();

    if ( !iDeviceArray->Count() )  
        {
        // show "Waiting devices" text
        iListBox->View()->SetListEmptyTextL(
            *StringLoader::LoadLC( R_UPNPCOMMONUI_EXTERNAL_WAITING_NOTE ) );
        CleanupStack::PopAndDestroy(); 
        }
    else
        {
        CEikButtonGroupContainer* cba = &ButtonGroupContainer();
        cba->SetCommandSetL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__OPEN );
        cba->DrawDeferred();
        }
    __LOG( "CUPnPExternalDeviceDialog::PreLayoutDynInitL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::PostLayoutDynInitL();
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::PostLayoutDynInitL()
    {
    iNaviPaneActive = ETrue;
    UpDateAnimationWindowL(iNaviPaneActive);//DateAnimationWindow();
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::OkToExitL(TInt aButtonId)
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
//
TBool CUPnPExternalDeviceDialog::OkToExitL( TInt aButtonId )
    {
    __LOG( "CUPnPExternalDeviceDialog::OkToExitL" );
    
    TBool returnValue = EFalse;
    iCommonUI.HandleCommonErrorL( aButtonId, NULL );
    // Translate the button presses into commands for the appui & current
    // view to handle
    switch ( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            DisplayMenuL(); 
            break;
            }
        case EAknSoftkeyOpen: //fall through
        case EAknSoftkeyOk:
            {
            CTextListBoxModel *model = iListBox->Model();

            if (model->NumberOfItems()) //browse only if list have items
                {
                iNaviPaneActive = EFalse;                
                UpDateAnimationWindowL(iNaviPaneActive);

                TInt ret = ExecuteBrowseL();
                if (ret == EAknCmdExit || 
                    ret == EEikCmdExit ||
                    ret == KErrDisconnected ||
                    ret == KErrSessionClosed
                    )
                    {
                    TryExitL( ret );
                    }
                else
                    {
                    DisplayMediaServersL();
                    iNaviPaneActive = ETrue;
                    UpDateAnimationWindowL(iNaviPaneActive);
                    }
                }
            break;
            }
        case EAknSoftkeyBack: // fall through
        case EUPnPBackCmd: // fall through
        case EAknSoftkeyCancel: // fall through
        case EAknSoftkeyExit: // fall through
        case EAknCmdExit: // fall through
        case EEikCmdExit: // fall through
        case KErrNotReady: // fall through
        case KErrDisconnected:
            {
            if( iTitlePane )
                {
                iTitlePane->SetTextToDefaultL();
                }
            returnValue = ETrue; //back to previous dialog
            break;
            }
        case KErrSessionClosed:
            {
            DisplayMediaServersL();
            iNaviPaneActive = ETrue;
            UpDateAnimationWindowL(iNaviPaneActive);

            break;
            }
        default:
            {
            // ignore
            }
        }
  
    __LOG( "CUPnPExternalDeviceDialog::OkToExitL End." );
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPExternalDevice::DynInitMenuPaneL(TInt aResourceId, 
// CEikMenuPane *aMenuPane)
// called by framework before menu panel is shown
// --------------------------------------------------------------------------
void  CUPnPExternalDeviceDialog::DynInitMenuPaneL( TInt aResourceId,
                                                   CEikMenuPane *aMenuPane )
    {
    __LOG( "CUPnPExternalDeviceDialog::DynInitMenuPaneL" );

    if( aResourceId ==  R_UPNPCOMMONUI_EXTERNAL_DEVICE_OPTION_MENU )
        {
        // Hide menu items if empty list
        CTextListBoxModel *model = iListBox->Model();
        if ( !model->NumberOfItems() ) 
            {
            aMenuPane->SetItemDimmed( EUPnPOpen, ETrue );
            aMenuPane->SetItemDimmed( EUPnPFind, ETrue );
            }

        // Hide "Find" if the currently focused device does not support it
        TInt selected_item = iListBox->CurrentItemIndex();
        if( selected_item >= 0 &&
            !( *iDeviceArray )[ selected_item ]->SearchCapability() )
            {
            aMenuPane->SetItemDimmed( EUPnPFind, ETrue );
            }

        // Hide "Help" if the help file is not available
        if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EUPnPHelp, ETrue );
            }
        }
    __LOG( "CUPnPExternalDeviceDialog::DynInitMenuPaneL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::ProcessCommandL(TInt aCommand)
// called by framework when menu item is selected
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::ProcessCommandL( TInt aCommand )
    {
    __LOG( "CUPnPExternalDeviceDialog::ProcessCommandL" );
    
    TInt error = KErrNone;
    HideMenu();
    switch ( aCommand ) 
        {
        case EAknCmdExit:
        case EEikCmdExit:
            {
            TryExitL( aCommand );
            break;
            }
        case EUPnPOpen:
            {
            iNaviPaneActive = EFalse;
            UpDateAnimationWindowL(iNaviPaneActive);

            error = ExecuteBrowseL();
            if ( error == EAknCmdExit ||
                 error == EEikCmdExit ||
                 error == KErrDisconnected ||
                 error == KErrSessionClosed )
                {
                TryExitL( error );
                }
            else 
                {
                DisplayMediaServersL(); 
                iNaviPaneActive = ETrue;
                UpDateAnimationWindowL(iNaviPaneActive); 
                }
            break;
            }
        case EUPnPFind:
            {
            iNaviPaneActive = EFalse;
            UpDateAnimationWindowL(iNaviPaneActive);  
            
            error = ExecuteFindL();
            if ( error == EAknCmdExit ||
                 error == EEikCmdExit ||
                 error == KErrDisconnected || 
                 error == KErrNotReady || 
                 error == KErrSessionClosed )
                {
                TryExitL( error );
                }
            else
                {
                iNaviPaneActive = ETrue;
                UpDateAnimationWindowL(iNaviPaneActive);
                }
            break;
            }
        case EUPnPHelp:
            {
            iNaviPaneActive = EFalse;
            UpDateAnimationWindowL(iNaviPaneActive);
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
            iEikonEnv->EikAppUi()->AppHelpContextL() );
            iNaviPaneActive = ETrue;
            UpDateAnimationWindowL(iNaviPaneActive);
            break;
            }
        default:
            break;
        }
    __LOG( "CUPnPExternalDeviceDialog::ProcessCommandL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UPnPDeviceDiscovered
// Returns discovered device from UPnP AV control point.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::UPnPDeviceDiscovered(
                                        const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDiscovered" );
    TInt error = KErrNone;
    TRAP( error, UPnPDeviceDiscoveredL( aDevice ) );
    
    __LOG1( "CUPnPExternalDeviceDialog::UPnPDeviceDiscovered %d", error);
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UPnPDeviceDiscoveredL
// Returns discovered device from UPnP AV control point.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::UPnPDeviceDiscoveredL(
                                        const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDiscovered" );
              
    // Display only Media servers
    if( aDevice.DeviceType() == CUpnpAVDevice::EMediaServer )
        {
        CUpnpAVDevice* tempDevice = CUpnpAVDevice::NewL( aDevice ) ;
                                      
        CleanupStack::PushL( tempDevice );
        iDeviceArray->AppendDeviceL( *tempDevice );
        CleanupStack::Pop( tempDevice ); 
        
        // Get device friendly name and replace illegal characters.
        HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            ( ( CUpnpAVDevice* )tempDevice )->FriendlyName() );
            
        CleanupStack::PushL( tmpfriendlyname );
        TPtrC8 friendlyname = *tmpfriendlyname;
                
        CTextListBoxModel* model = iListBox->Model();
        MDesCArray* textArray = model->ItemTextArray();
        CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
        TBuf<256> item;
        
        HBufC *tmpbuf = UpnpString::ToUnicodeL( friendlyname.Left( KLength ) );
        CleanupStack::PushL( tmpbuf ); 
        
        item.Format( _L( "%d\t%S\t\t" ),0, tmpbuf ); 
        CleanupStack::PopAndDestroy( tmpbuf ); 
        CleanupStack::PopAndDestroy( tmpfriendlyname );

        listBoxItems->AppendL( item );
        iListBox->HandleItemAdditionL(); // Update listbox

        iListBox->ActivateL();
        iListBox->DrawDeferred();
        
        //update menu pane
        CEikButtonGroupContainer* cba = &ButtonGroupContainer();
        
        cba->SetCommandSetL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__OPEN );
        cba->DrawDeferred();
        }
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDiscovered End" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UPnPDeviceDisappeared
// Returns disappeared device from UPnP AV control point.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::UPnPDeviceDisappeared(
                                        const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDisappeared" );
    
    TRAP_IGNORE( UPnPDeviceDisappearedL( aDevice ) );
    
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UPnPDeviceDisappearedL
// Returns disappeared device from UPnP AV control point.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::UPnPDeviceDisappearedL(
                                               const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDissapearedL" );
    
    CTextListBoxModel* model = iListBox->Model();
    TInt currentItem = iListBox->CurrentItemIndex();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    TInt i = 0;
    TPtrC8 ptr(aDevice.Uuid());
    while( i < iDeviceArray->Count() &&
        ptr.Compare( ( *iDeviceArray)[ i ]->Uuid() ) )
        {
        i++;
        }
    if ( i < iDeviceArray->Count() )
        {
        iDeviceArray->RemoveAndDestroy( i );
        listBoxItems->Delete( i,1 );
        AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
            iListBox, currentItem, ETrue );
        iListBox->DrawDeferred();
        }
            
    if ( !iDeviceArray->Count() )
        {
        // show "Waiting devices" text
        iListBox->View()->SetListEmptyTextL(
            *StringLoader::LoadLC( R_UPNPCOMMONUI_EXTERNAL_WAITING_NOTE ) );
        CleanupStack::PopAndDestroy(); // R_EXTERNAL_WAITING_NOTE
        
        CEikButtonGroupContainer* cba = &ButtonGroupContainer();
        cba->SetCommandSetL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_BACK );
        cba->DrawDeferred();
        }
   
    __LOG( "CUPnPExternalDeviceDialog::UPnPDeviceDissapearedL-END" );
    }
    
// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an 
// icon array.
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::AppendIconToArrayL( CAknIconArray* aArray,
                                           MAknsSkinInstance* aSkin,
                                           const TDesC& aMbmFile,
                                           const TAknsItemID& aID,
                                           TInt aBitmapId,
                                           TInt aMaskId )
    {
    __LOG( "CUPnPExternalDeviceDialog::AppendIconToArrayL" );
    
    __ASSERTD( aArray != NULL, __FILE__, __LINE__ );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( 2 ); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop();
    __LOG( "CUPnPExternalDeviceDialog::AppendIconToArrayL End" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DisplayMediaServersL
// Add visible Media Servers to Listbox of dialog
// --------------------------------------------------------------------------
void CUPnPExternalDeviceDialog::DisplayMediaServersL()
    {
    __LOG( "CUPnPExternalDeviceDialog::DisplayMediaServersL" );
    
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    
    //clear list        
    delete iDeviceArray; iDeviceArray = NULL;
    listBoxItems->Reset();
    
    TBuf<256> item;
    
    iDeviceArray=iAVControl.GetMediaServersL();
    

    if ( iDeviceArray->Count() )
        {
        for ( TInt i=0; i < iDeviceArray->Count(); i++ )
            {
            // Get device frienly name and replace illegal characters.
            HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
                ( *iDeviceArray )[ i ]->FriendlyName() );

            CleanupStack::PushL( tmpfriendlyname );
            TPtrC8 friendlyname = *tmpfriendlyname;

            HBufC* tmpbuf = UpnpString::ToUnicodeL( 
                friendlyname.Left( KLength ) );
            CleanupStack::PushL( tmpbuf ); 
            item.Format( _L( "%d\t%S\t\t" ),0, tmpbuf ); 
            CleanupStack::PopAndDestroy( tmpbuf );
            CleanupStack::PopAndDestroy( tmpfriendlyname ); 
            listBoxItems->AppendL( item );
            }
        
        iListBox->HandleItemAdditionL(); // Update listbox
        iListBox->SetCurrentItemIndexAndDraw( 0 ); // select new item        
        }
    __LOG( "CUPnPExternalDeviceDialog::DisplayMediaServersL End" );        
    }


// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::ExecuteBrowseL
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
TInt CUPnPExternalDeviceDialog::ExecuteBrowseL()
    {
    __LOG( "CUPnPExternalDeviceDialog::ExecuteBrowseL" );
    
    TInt selected_item = iListBox->CurrentItemIndex();

    const CUpnpAVDevice* selectedDevice = ( *iDeviceArray )[ selected_item ];
    
    TInt ret = iCommonUI.ExecuteBrowseDialogL(
        iAVControl, *selectedDevice );
    
    iTitlePane->SetTextL( *StringLoader::LoadLC(
        R_UPNPCOMMONUI_EXTERNAL_MEDIA_TITLE ) );
    CleanupStack::PopAndDestroy();

    
    __LOG( "CUPnPExternalDeviceDialog::ExecuteBrowseL End" );
    return ret;
    }
    
// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::ExecuteFindL
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
TInt CUPnPExternalDeviceDialog::ExecuteFindL()
    {
    __LOG( "CUPnPExternalDeviceDialog::ExecuteFindL" );
    
    TInt ret = KErrNone;
    TInt selected_item = iListBox->CurrentItemIndex();
    CUpnpAVDevice* selectedDevice = 
                            ( *iDeviceArray )[ selected_item ];
    
    if( selectedDevice )
        {
        //create a browsessesion only for advFind dialog
        
        MUPnPAVBrowsingSession* browsingSession = NULL;    
        browsingSession = &( iAVControl.StartBrowsingSessionL(
                                                *selectedDevice ) );
        
        ret = iCommonUI.ExecuteAdvFindDialogL( iAVControl,
                                               *browsingSession );
        browsingSession->RemoveObserver();
        iAVControl.StopBrowsingSession( *browsingSession );    
        }
    
    if( KErrDisconnected != ret ) //if not WLAN lost
        {
        DisplayMediaServersL();    
        }

    __LOG( "CUPnPExternalDeviceDialog::ExecuteFindL End" );
    return ret;
    }
    

            
// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::WLANConnectionLost
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
void CUPnPExternalDeviceDialog::WLANConnectionLost() 
    {
    __LOG( "CUPnPExternalDeviceDialog::WLANConnectionLost" );
    //if no dialog created in CUPnPExternalDeviceDialog, destory itself 
    //Otherwise destory other dialogs like in normal by KErrAbort
    if( !iCommonUI.PresenceOfDialog() )
        {
        TRAP_IGNORE( TryExitL( KErrDisconnected ) );
        }
    
    }


// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::GetHelpContext
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::GetHelpContext(
                                        TCoeHelpContext& aContext ) const
    {
    __LOG( "CUPnPExternalDeviceDialog::GetHelpContext" );
    
//  aContext.iMajor = TUid::Uid( KMediaGalleryUID3 );    
    aContext.iContext = KUPNP_HLP_MAIN_EXTERNAL;
    
    __LOG( "CUPnPExternalDeviceDialog::GetHelpContext End" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UpDateAnimationWindowL
// see upnpexternaldevicedialog.h
// --------------------------------------------------------------------------
//
void CUPnPExternalDeviceDialog::UpDateAnimationWindowL(TBool aAnimationState)
    {    
    __LOG( "CUPnPExternalDeviceDialog::UpDateAnimationWindowL" );
    if ( iNaviDecorator )
        {
        iNaviPaneContainer->Pop( iNaviDecorator );
        }
    
    // destroy iNaviDecorator
    delete iNaviDecorator;
    iNaviDecorator = NULL;
        
    if ( ( !Layout_Meta_Data::IsLandscapeOrientation() )
         && ( aAnimationState ) )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();

        TBool naviFaded = iStatusPane->IsFaded();

        if (!naviFaded)
            {
            iNaviPaneContainer = ( CAknNavigationControlContainer* )
                iStatusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
        
            TPoint screenPosition = iNaviPaneContainer->
                PositionRelativeToScreen();
            /**
             * adjust the screen position so that the "Searching..." could be 
             * exactly in that "blue" NaviPane
             */                        
            screenPosition.iY = screenPosition.iY+1;
            iNaviContainer = CUPnPNaviPaneContainer::NewL(
                                                 iNaviPaneContainer->Rect(),
                                                 screenPosition,
                                                 iNaviPaneContainer );
        
            iNaviDecorator = CAknNavigationDecorator::NewL(
                iNaviPaneContainer, iNaviContainer );
        iNaviDecorator->SetContainerWindowL( *iNaviPaneContainer );
        iNaviDecorator->MakeScrollButtonVisible(EFalse);
        iNaviDecorator->SetComponentsToInheritVisibility( ETrue );
            iNaviPaneContainer->PushL( *iNaviDecorator );        
            }
        }
    iListBox->ActivateL();
    
    __LOG( "CUPnPExternalDeviceDialog::UpDateAnimationWindowL End" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::HandleResourceChange
// --------------------------------------------------------------------------
//    
void CUPnPExternalDeviceDialog::HandleResourceChange(TInt aType)
    {
    __LOG("[UPnPCommonUI]\t CUPnPExternalDeviceDialog::HandleResourceChange");
    CAknDialog::HandleResourceChange(aType);
    
    // Update Titlepane only if there is no other dialogs open
    if( !iCommonUI.PresenceOfDialog() )
        {
        __LOG("[UPnPCommonUI]\t CUPnPExternalDeviceDialog::HandleResourceChange\
updating iTitlePane");

    TRAPD( err,
            iTitlePane->SetTextL( *StringLoader::LoadLC(
                R_UPNPCOMMONUI_EXTERNAL_MEDIA_TITLE ) );
            CleanupStack::PopAndDestroy();
            );
            
    if( err != KErrNone )
        {
        __LOG("[UPnPCommonUI]\t CUPnPExternalDeviceDialog::HandleResourceChange\
iTitlePane->SetTextL failed");
            }
        }
    
    if (iNaviPaneActive)
        {
        TRAP_IGNORE( UpDateAnimationWindowL(iNaviPaneActive) );
        }    
    }         
// end of file
