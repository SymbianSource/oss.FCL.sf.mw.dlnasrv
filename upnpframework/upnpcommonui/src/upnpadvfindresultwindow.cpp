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
* Description:  Find results dialog class source code
*
*/


// INCLUDE FILES
// System
#include <featmgr.h>
#include <akntabgrp.h>
#include <StringLoader.h>
#include <aknlists.h>
#include <commondialogs.mbg>
#include <aknnotewrappers.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// upnp stack api's
#include <upnpitem.h>
#include <upnpobject.h>
#include <upnpstring.h>
#include <collate.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"   //start browse session and rendering session
#include "upnpavbrowsingsession.h"
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // upnp definitions
#include "upnpitemutility.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"
#include "upnpfiletransferengine.h"
#include "upnpmusicadapter.h"
#include "upnpplaylistfiller.h"

// common ui internal
#include <hlplch.h>
#include <upnpcommonui.mbg>
#include <upnpcommonui.rsg>
#include "upnpadvfindresultwindow.h"
#include "upnpcommonui.hrh"
#include "upnpimageplayer.h"
#include "upnplocalplayer.h"
#include "upnpcommonui.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

//CONSTANTS
_LIT( KAknCommonUIMbmFileName, "\\resource\\apps\\upnpcommonui.mbm" );
const TInt KMbmFileBufferSize = 256;
_LIT8( KUnknown,    "Unknown" );
_LIT( KUPNP_HLP_SEARCH, "UPNP_HLP_SEARCH" );

// Format string for image not showing
_LIT( KImageFormatString,  "%d\t%S\t\t" );

// Format string for image showing
_LIT( KImageShowingFormatString, "%d\t%S\t\t%d" );

const TInt KLength = 100;

// The delay from the Start() function of CPeriodic class 
// to the generation of the first event, in microseconds.
const TInt KImageTimerDelay = 800000;

// A callback specifying a function to be called 
// when the CPeriodic is scheduled after a timer event, in microseconds.
const TInt KImageTimerInterval = 800000;

enum TUPnPIconTypes
    {
    EUPnPIconMusic = 1,
    EUPnPIconVideo,
    EUPnPIconImage,
    EUPnPIconImageShowing,
    EUPnPIconOther,
    EUPnPIconLast
    };


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CAdvancedFindResultWindow* CAdvancedFindResultWindow::NewL(
                                    TInt aMenuResource,
                                    RPointerArray<CUpnpObject>& aResultArray,
                                    MUPnPAVController& aAVControl,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI )
    {    
    CAdvancedFindResultWindow* self = new(ELeave) CAdvancedFindResultWindow(
                                                            aAVControl,
                                                            aBrowseSession,
                                                            aCommonUI );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuResource, aResultArray );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//

void CAdvancedFindResultWindow::ConstructL( 
                                TInt aMenuResource,
                                RPointerArray<CUpnpObject>& aResultArray )
    {



    iTargetDevice = CUpnpAVDevice::NewL();
    
    iImageControlTimer = CUPnPPeriodic::NewL( CActive::EPriorityUserInput );
        
    //iCommonUI = CUPnPCommonUI::NewL();

    for( TInt index = 0; index < aResultArray.Count(); index++ )
        {
        iResultArray.AppendL( aResultArray[index] );
        }
            
    CAknDialog::ConstructL( aMenuResource );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::CAdvancedFindResultWindow
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------

CAdvancedFindResultWindow::CAdvancedFindResultWindow(
                                    MUPnPAVController& aAVControl,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI ):
                                    iBrowseSession(aBrowseSession),
                                    iAVControl(aAVControl),
                                    iCommonUI(aCommonUI)
    {
    iSourceDevice = &( iBrowseSession.Device() );
    iChildDialogOpen = 0;
    iMSDisappear = EFalse;
    iAction = CUPnPCommonUI::EUPnPNone;
    iIsMusicItem = EFalse;
    iLastImageItemIndex = KErrNotFound;
    iHashKeyFlag = EFalse;   
    }

// Destructor
CAdvancedFindResultWindow::~CAdvancedFindResultWindow()
    {
    if ( iNaviDecorator && iNaviPane )
        {
        iNaviPane->Pop( iNaviDecorator );
        }
    delete iNaviDecorator;
    
    iResultArray.Close();
    
    if ( iImagePlayer )
        {
        iImagePlayer->Stop();
        delete iImagePlayer;
        }
    delete iLocalPlayer;
    
    if ( iImageControlTimer )  
        {
        iImageControlTimer->Cancel();
        delete iImageControlTimer;
        }
        
    delete iTargetDevice;
    }

// ---------------------------------------------------------
// CAdvancedFindResultWindow::OkToExitL( TInt aButtonId )
// called by framework when the softkey is pressed
// ---------------------------------------------------------
//

TBool CAdvancedFindResultWindow::OkToExitL( TInt aButtonId )
    {
    // Translate the button presses into commands for the appui & current
    // view to handle
    
    switch ( aButtonId )        
        {                
        case EAknSoftkeyOk:
            {
            if ( iResultArray.Count() > 0 )
                {
                TRAPD( error, ShowPlayExtL() );
                HandleErrorL( error );
                }            
            break;
            }
        case EUPnPStopCmd:
            {
            StopImageControlL();
            break;
            }
        case EUPnPPlayCmd:
            {
            if( !iMusicPlay )
                {
                StopImageControlL();
                TRAPD( error, PlayL( EFalse ) );
                if( KErrNone != error )
                    {
                    iMusicPlay = EFalse;
                    }
                HandleErrorL( error );
                }           
            break;            
            }
        case EAknSoftkeyShow:
            {
            StopImageControlL();
            TRAPD( error, ShowVideoExtL() );
            HandleErrorL( error );
            break;
            }            
        case KErrSessionClosed: 
        case KErrDisconnected:
        case EAknCmdExit: // fall through
        case EAknSoftkeyBack:
            {
            iNaviPane->PushDefaultL();
            return ETrue;
            }
        case EAknSoftkeyOptions:
            {
            CAknDialog::DisplayMenuL();
            SetNaviPaneTextL();   
            break;
            }
        default:
            {
            break;
            }//switch (aButtonId) 
        }
    
    return EFalse;
    }

// ---------------------------------------------------------
// CAdvancedFindResultWindow::PreLayoutDynInitL()
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CAdvancedFindResultWindow::PreLayoutDynInitL()
    {
    iListBox = static_cast<CAknDoubleGraphicStyleListBox*>( 
                                            Control(EAdvFindResultBoxId) );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    CAknIconArray* icons = new (ELeave) CAknIconArray( EUPnPIconLast );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );
    
    
    TFileName mbmFileName( KAknCommonUIMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    
    //marked icon
    AppendIconToArrayL( icons,
        skin,
        KAvkonBitmapFile,
        KAknsIIDQgnIndiMarkedAdd,
        EMbmAvkonQgn_indi_marked_add,
        EMbmAvkonQgn_indi_marked_add_mask );
    // Music icon
    AppendIconToArrayL( icons,
        skin,
        mbmFileName,
        KAknsIIDDefault,
        EMbmUpnpcommonuiQgn_prop_mserv_music,
        EMbmUpnpcommonuiQgn_prop_mserv_music_mask );
     // Video icon
    AppendIconToArrayL( icons,
        skin,
        mbmFileName,
        KAknsIIDDefault,
        EMbmUpnpcommonuiQgn_prop_mserv_other_videos,
        EMbmUpnpcommonuiQgn_prop_mserv_other_videos_mask );
    // Image icon
    AppendIconToArrayL( icons,
        skin,
        mbmFileName,
        KAknsIIDDefault,
        EMbmUpnpcommonuiQgn_prop_mserv_other_images,
        EMbmUpnpcommonuiQgn_prop_mserv_other_images_mask );
    // Image showing icon
    AppendIconToArrayL( icons,
        skin, 
        mbmFileName, 
        KAknsIIDDefault, 
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer_list_icon, 
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer_list_icon_mask );
    //Other icon
    AppendIconToArrayL( icons,
        skin,
        KCommonDialogsBitmapFile,
        KAknsIIDQgnPropFmgrFileSound,
        EMbmCommondialogsQgn_prop_fmgr_file_other,
        EMbmCommondialogsQgn_prop_fmgr_file_other_mask );
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );
    CleanupStack::Pop(icons); 
    // set dialog title
    CEikStatusPane* statusPane = 
                    ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
    iTitlePane = (CAknTitlePane*)statusPane->ControlL( 
                                       TUid::Uid( EEikStatusPaneUidTitle ) );
    // Set the text string.
    HBufC* tempStr = StringLoader::LoadLC( 
                    R_UPNPCOMMONUI_ADVANCED_FIND_SEARCH_RESULT_TITLE ); 
    iTitlePane->SetTextL( *tempStr );
    CleanupStack::PopAndDestroy( tempStr );
    LoadListItemsL();
    SetNaviPaneTextL();
    iListBox->ActivateL();
    }

// ---------------------------------------------------------
// CAdvancedFindResultWindow::PostLayoutDynInitL()
// Called by framework before dialog is shown.
// Needed because filter is not active until PreLayoutDynInitL().
// ---------------------------------------------------------
//
/*void  CAdvancedFindResultWindow::PostLayoutDynInitL()
    {
    }*/
    
// ---------------------------------------------------------
// CAdvancedFindResultWindow::OfferKeyEventL()
// called by framework when key is pressed
// ---------------------------------------------------------
//
TKeyResponse CAdvancedFindResultWindow::OfferKeyEventL ( 
                                                const TKeyEvent &aKeyEvent,
                                                TEventCode aType )
    {
    TInt tempCounter = iListBox->CurrentItemIndex();
    if (aType != EEventKey)
        {
        if ( aKeyEvent.iScanCode == EStdKeyHash )
            {
            if ( aType == EEventKeyDown )
                {
                iHashKeyFlag = ETrue;
                }
            else
                {
                iHashKeyFlag = EFalse;
                }
            }
        iListBox->OfferKeyEventL( aKeyEvent, aType ); 

        if ( aType == EEventKeyUp && !iHashKeyFlag )
            {      
            if ( !iImageControlActive )
                {
                const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
                if ( indexes->Count() )
                    {
                    UpdateCommandButtonAreaL( ETrue, tempCounter );
                    }
                else
                    {
                    UpdateCommandButtonAreaL( EFalse, tempCounter );
                    }               
                }                        
            }
        return EKeyWasConsumed;
        }
    else
        {
        if ( aKeyEvent.iCode == EKeyDownArrow ||
         aKeyEvent.iCode == EKeyUpArrow )
            {               
            if ( iImageControlActive )
                {
                iImageControlTimer->Cancel();
                StartImageControlTimer();
                }
            
            if ( aKeyEvent.iCode == EKeyDownArrow )
                {
                tempCounter++;
                }
            else
                {
                tempCounter--;
                }
            if ( tempCounter >= iResultArray.Count() )
                {
                tempCounter = 0;
                }
            if ( tempCounter < 0 )
                {
                tempCounter = iResultArray.Count()-1;
                }
            if( iImageControlActive )
                {
                if( UPnPItemUtility::BelongsToClass( 
                        *iResultArray[tempCounter], KClassImage ) ) // image  
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
                    }
                else if ( UPnPItemUtility::BelongsToClass( 
                        *iResultArray[tempCounter], KClassVideo ) ) // video
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_SHOW_STOP );
                    }
                else if ( UPnPItemUtility::BelongsToClass( 
                        *iResultArray[tempCounter], KClassAudio ) ) // music
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_PLAY_STOP );
                    }    
                else
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
                    }    
                }
             }
            
        CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        if ( ( tempCounter < iResultArray.Count() ) && tempCounter > -1 )
            {
            if ( !iImageControlActive )
                {
                const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
                if ( indexes->Count() )
                    {
                    UpdateCommandButtonAreaL( ETrue, tempCounter );
                    }
                else
                    {
                    UpdateCommandButtonAreaL( EFalse, tempCounter );
                    }               
                }
            } 
        }
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CAdvancedFindResultWindow::SetNaviPaneText()
// Sets navi pane text referring to the count of selected files
// ---------------------------------------------------------
//
void CAdvancedFindResultWindow::SetNaviPaneTextL()
    {
    TInt itemCount = iResultArray.Count();    
    HBufC* naviText;
    if (itemCount == 1)
        {        
        naviText = StringLoader::LoadLC( 
                    R_UPNPCOMMONUI_ADVANCED_FIND_SEARCH_RESULT_FILE, 
                    itemCount );
        
        }
    else
        {
        naviText = StringLoader::LoadLC( 
                    R_UPNPCOMMONUI_ADVANCED_FIND_SEARCH_RESULT_FILES,
                    itemCount );
        }
    // push text to navi pane
    CEikStatusPane* statusPane = 
                    ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
    iNaviPane = ( CAknNavigationControlContainer * )statusPane->ControlL(
                                         TUid::Uid(EEikStatusPaneUidNavi ) );
    // delete previous navidecorator
    delete iNaviDecorator; iNaviDecorator = NULL;

    iNaviDecorator = iNaviPane->CreateNavigationLabelL( *naviText );
    iNaviPane->PushL( *iNaviDecorator );
    CleanupStack::PopAndDestroy( naviText );    
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an
// icon array.
// --------------------------------------------------------------------------
//

void CAdvancedFindResultWindow::AppendIconToArrayL( CAknIconArray* aArray,
                                                    MAknsSkinInstance* aSkin,
                                                    const TDesC& aMbmFile,
                                                    const TAknsItemID& aID,
                                                    TInt aBitmapId,
                                                    TInt aMaskId )
    {    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TBuf<KMbmFileBufferSize> buffer( aMbmFile );
    AknsUtils::CreateIconLC( aSkin, aID, bitmap, mask,
                             aMbmFile, aBitmapId, aMaskId );
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );     
    bitmap = NULL;
    mask = NULL;
    CleanupStack::PushL( icon );
    aArray->AppendL( icon );
    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop( icon );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ProcessCommandL
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::ProcessCommandL( TInt aCommand )
    {
    TInt error = KErrNone;
    
    HideMenu();
    TInt selected_item = iListBox->CurrentItemIndex();
    switch (aCommand)
        {
        case EUPnPPlayExt:
        case EUPnPShowExt: //fall through
            {
            TRAP( error, ShowPlayExtL() );
            break;
            }
        case EUPnPPlayLocal: //    
        case EUPnPShowLocal: //fall through
            {
            TRAP( error, ShowPlayLocalL() );
            if( KErrSessionClosed == error )
                {
                iMSDisappear = ETrue;
                }
            break;
            }
        case EUPnPCopy:
            {
            TRAP( error, HandleCopyL() );
            if( KErrNotFound == error )
                {
                iCopyIndex = 0; 
                }
            if( error == KErrSessionClosed ) //if server lost
                {
                iMSDisappear = ETrue;
                }
            break;
            }           
        case EAknCmdHelp:
            {
            TRAPD( error, HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() ) );
            if ( error != KErrNone )
                {
                __LOG1( "CAdvancedFindResultWindow: \
    LaunchHelpApplicationL returned err=%d", error );
                }     
            break;
            }
        case EAknCmdMark:
            {
            MarkUnmarkL( iListBox->CurrentItemIndex(), ETrue );
            break;
            }
        case EAknMarkAll:
            {
            MarkUnmarkL( -1, ETrue );
            break;
            }
        case EAknCmdUnmark:
            {
            MarkUnmarkL( iListBox->CurrentItemIndex(), EFalse );
            break;
            }
        case EAknUnmarkAll:
            {
            MarkUnmarkL( -1, EFalse );
            break;
            }
        case EAknCmdExit:
            {
            //User::Exit(0);
            TryExitL( aCommand ); //no need to go to next
            return;
            }    
        default:
            CAknDialog::ProcessCommandL( aCommand );
            break;
        }   
    HandleErrorL( error );  
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::MarkUnmarkL
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::MarkUnmarkL( TInt aItem, TBool aMark )
    {    
    CTextListBoxModel* model = iListBox->Model();
    TInt itemCount = model->NumberOfItems();
    CListBoxView* listBoxView = iListBox->View();
    if ( aMark )
        {
        for ( TUint i = 0; i < itemCount; i++ )
            {
            if ( aItem == -1 || aItem == i )
                {
                listBoxView->SelectItemL( i );                
                }
            }
        UpdateCommandButtonAreaL( ETrue, iListBox->CurrentItemIndex() );
        }
    else
        {
        for ( TUint i = 0; i < itemCount; i++ )
            {
            if ( aItem == -1 || aItem == i )
                {
                listBoxView->DeselectItem( i );
                }
            }
        const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
        if ( indexes->Count() )
            {
            UpdateCommandButtonArea( ETrue, iListBox->CurrentItemIndex() );
            }
        else
            {
            UpdateCommandButtonArea( EFalse, iListBox->CurrentItemIndex() );
            }        
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::PlayL
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::PlayL( TBool aLocal )
    {
    TInt error = KErrNone;
    
    // create a playlist representation
    CUPnPPlayListFiller* filler = 
                    CUPnPPlayListFiller::NewL();
    CleanupStack::PushL( filler );
    TInt firstAudioItem = KErrNotFound;
    TBool focusedItemFound = EFalse;
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( indexes->Count() > 0 )
        {
        // play the marked files
        for (TInt i = 0; i < indexes->Count(); i++)
            {
            filler->InsertObjectL(
                         *iSourceDevice, *iResultArray[ indexes->At( i ) ] );
            
            if( UPnPItemUtility::BelongsToClass( 
                *iResultArray[ indexes->At(i) ], KClassAudio )
                && firstAudioItem == KErrNotFound )
                {
                firstAudioItem = i;
                }
            if( iResultArray[ indexes->At( i ) ]->Id()
                == iResultArray[ iListBox->CurrentItemIndex() ]->Id() )
                {
                filler->SetSelectedIndex( i );
                focusedItemFound = ETrue;
                }
            }

        if( !focusedItemFound )
            {
            filler->SetSelectedIndex( firstAudioItem );
            }
        }
    else
        {
        // play all found files
        for( TInt i = 0; i < iResultArray.Count(); i++ )
            {
            filler->InsertObjectL(
                *iSourceDevice, *iResultArray[ i ] );
            }
        filler->SetSelectedIndex( iListBox->CurrentItemIndex() );
        }

    if( !aLocal )
        {
        
        iChildDialogOpen++;
        error = iCommonUI.SelectDeviceL(
                              iAVControl,
                              *iTargetDevice,
                              EUPnPSearchRenderingDevicesWithAudioCapability,
                              EUPnPSelectDeviceTitle );       
        if ( KErrNone == error )
            {
            CleanupStack::Pop( filler ); // musicadapter handles delete
            
            iMusicPlay = ETrue;
            //filler ownership transferred
            error = iCommonUI.ExecuteMusicPlayerL( iAVControl, 
                                                   filler, 
                                                   iTargetDevice );
            iMusicPlay = EFalse;                                       
            iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL(
                                    R_AVKON_STATUS_PANE_LAYOUT_USUAL);
            iAvkonEnv->LoadAknLayoutL();
            iAvkonAppUi->ReportResourceChangedToAppL(
                                KEikDynamicLayoutVariantSwitch );
          
            }
        else
            {
            CleanupStack::PopAndDestroy( filler );
            }
        iChildDialogOpen--;
        }
    else
        {
        CleanupStack::Pop( filler );
        iChildDialogOpen++;
        iMusicPlay = ETrue;
        //filler ownership transferred
        error = iCommonUI.ExecuteMusicPlayerL( iAVControl, filler, NULL );
        iMusicPlay = EFalse;
        iChildDialogOpen--;
        iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL(
                                    R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        iAvkonEnv->LoadAknLayoutL();
        iAvkonAppUi->ReportResourceChangedToAppL(
                            KEikDynamicLayoutVariantSwitch );
        }
    if( KErrNone != error )
        {
        User::Leave( error );    
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ShowPlayExtL
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::ShowPlayExtL()
    {
    //if audio file is either in marked items or highlighted, play it
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( indexes->Count() )
        {
        if( IsAudioItemMarked() && !iMusicPlay )
            {
            TRAPD( err, PlayL( EFalse ) );
            if( KErrNone != err )
                {
                iMusicPlay = EFalse;
                }
            }    
        }
    else // if no marked items
        {
        if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                          ObjectClass().Find( KClassVideo ) == 0 )
            {
            ShowVideoExtL();
            }
        else if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                              ObjectClass().Find( KClassImage ) == 0 )
            {
            ShowImageExtL();
            }    
        else if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                          ObjectClass().Find( KClassAudio ) == 0 && 
                          !iMusicPlay )    
            {
            PlayL( EFalse );
            }
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ShowPlayLocalL
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::ShowPlayLocalL()
    {
    //if audio file is either in marked items or highlighted, play it
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( !iMusicPlay && UPnPItemUtility::BelongsToClass(
        *iResultArray[iListBox->CurrentItemIndex()], KClassAudio) )
        {
        TRAPD( err, PlayL( ETrue ) );
        if( KErrNone != err )
            {
            iMusicPlay = EFalse;
            }
        }
    else // if no marked items
        {
        if ( !indexes->Count() )
            {
            if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                ObjectClass().Find( KClassVideo ) == 0 ||
                 ( iResultArray[iListBox->CurrentItemIndex()] )->
                        ObjectClass().Find( KClassImage ) == 0 )
                {
                 if( !iLocalPlayer )
                    {
                    iLocalPlayer = CUPnPLocalPlayer::NewL( iAVControl,
                                                           iBrowseSession,
                                                           iCommonUI );
                    }
                const CUpnpObject* item = 
                                iResultArray[iListBox->CurrentItemIndex()];
                
                iLocalPlayer->PlayL( *item );
                }
            }
        }
    }
    
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::LoadListItemsL
// loads items in result array to dialog. 
// --------------------------------------------------------------------------
//    
void CAdvancedFindResultWindow::LoadListItemsL()
    {
    MarkUnmarkL( -1, EFalse ); //deselect all items
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    HBufC16* item = HBufC16::NewL(256);
    CleanupStack::PushL( item );
    listBoxItems->Reset();
    if( !iResultArray.Count() )
        {
        // show "no results" text
        HBufC* noResultsText =  StringLoader::LoadLC( 
                    R_UPNPCOMMONUI_ADVANCED_FIND_SEARCH_RESULT_NO_RESULTS );
        iListBox->View()->SetListEmptyTextL( *noResultsText );
        CleanupStack::PopAndDestroy( noResultsText );
        }
    else
        {
        for (TInt i=0; i < iResultArray.Count(); i++)
            {           
            if( ( iResultArray[i])->
                    ObjectClass().Find( KClassAudio ) == 0 ) 
                {
                if( ( iResultArray[i]->Title().Length() ) != 0 )
                    {
                    // Get title and replace illegal characters.
                    HBufC8* tmptitle = 
                    UPnPCommonUtils::FixListboxItemTextL( 
                        iResultArray[ i ]->Title().Left( KLength ) );
                    
                    CleanupStack::PushL( tmptitle );
                    TPtrC8 tmpnameptr = *tmptitle;
                    iResultArray[i]->SetTitleL( tmpnameptr );

                    HBufC* tmpStr = 
                        UpnpString::ToUnicodeL( tmpnameptr.Left( KLength ) );
                    CleanupStack::PushL( tmpStr );
                    HBufC8* artist = NULL;
                    
                    const RUPnPElementsArray& elms = 
                                        iResultArray[i]->GetElements();
                    TInt count = elms.Count();
                    for( TInt i = 0; i < count; i++)
                        {
                        if( elms[ i ]->Name() == KElementArtist() )
                            {
                            artist = elms[ i ]->Value().AllocL();
                            i = count;
                            }
                        }
                    if( !artist )
                        {
                        TBufC8<7> buf( KUnknown() );
                        artist = buf.AllocL();
                        }
                    CleanupStack::PushL( artist );
                    HBufC* tmpStr2 = 
                           UpnpString::ToUnicodeL( artist->Left( KLength ) );
                    CleanupStack::PushL( tmpStr2 );
                    item->Des().Format( _L("%d\t%S\t%S\t"), 
                                        EUPnPIconMusic, 
                                        tmpStr, 
                                        tmpStr2 );
                    CleanupStack::PopAndDestroy( tmpStr2 );
                    CleanupStack::PopAndDestroy( artist );
                    CleanupStack::PopAndDestroy( tmpStr );
                    CleanupStack::PopAndDestroy( tmptitle );
                    }
                }
            else if( (iResultArray[i])->ObjectClass().Find( KClassVideo )
                                                == 0 ) //if video
                {
                if( ( iResultArray[i]->Title().Length() ) != 0 )
                    {
                    // Get title and replace illegal characters.
                    HBufC8* tmptitle = 
                    UPnPCommonUtils::FixListboxItemTextL( 
                        iResultArray[ i ]->Title().Left( KLength ) );
                    
                    CleanupStack::PushL( tmptitle );
                    TPtrC8 tmpnameptr = *tmptitle;
                    iResultArray[i]->SetTitleL( tmpnameptr );
                    
                    HBufC* tmpStr = 
                        UpnpString::ToUnicodeL( tmpnameptr.Left( KLength ) );
                    CleanupStack::PushL( tmpStr );
                    item->Des().Format( _L("%d\t%S\t\t"), 
                                        EUPnPIconVideo, 
                                        tmpStr );
                    CleanupStack::PopAndDestroy( tmpStr );
                    CleanupStack::PopAndDestroy( tmptitle );
                    }
                  
                }
            else if( ( iResultArray[i] )->ObjectClass().Find( KClassImage )
                                                == 0 ) //if image
                {
                if( ( iResultArray[i]->Title().Length() ) != 0 )
                    {
                    // Get title and replace illegal characters.
                    HBufC8* tmptitle = 
                    UPnPCommonUtils::FixListboxItemTextL( 
                        iResultArray[ i ]->Title().Left( KLength ) );
                    
                    CleanupStack::PushL( tmptitle );
                    TPtrC8 tmpnameptr = *tmptitle;
                    iResultArray[i]->SetTitleL( tmpnameptr );
                    
                    HBufC* tmpStr = UpnpString::ToUnicodeL( 
                        tmpnameptr.Left( KLength ) );
                    CleanupStack::PushL( tmpStr );
                    item->Des().Format( _L("%d\t%S\t\t"), 
                                        EUPnPIconImage, 
                                        tmpStr );
                    CleanupStack::PopAndDestroy( tmpStr );
                    CleanupStack::PopAndDestroy( tmptitle );
                    }
                 }
            else //if not recoganized media file type
                {
                if( ( iResultArray[i]->Title().Length() ) != 0 )
                    {
                    // Get title and replace illegal characters.
                    HBufC8* tmptitle = 
                    UPnPCommonUtils::FixListboxItemTextL( 
                        iResultArray[ i ]->Title().Left( KLength ) );
                    
                    CleanupStack::PushL( tmptitle );
                    TPtrC8 tmpnameptr = *tmptitle;
                    iResultArray[i]->SetTitleL( tmpnameptr );

                    HBufC* tmpStr = UpnpString::ToUnicodeL( 
                        tmpnameptr.Left( KLength ) );
                    CleanupStack::PushL( tmpStr );
                    item->Des().Format( _L("%d\t%S\t\t"), 
                                        EUPnPIconOther, 
                                        tmpStr );
                    CleanupStack::PopAndDestroy( tmpStr );
                    CleanupStack::PopAndDestroy( tmptitle );
                    }
                }
            listBoxItems->AppendL( item->Des() );                
            } //for
        iListBox->HandleItemAdditionL(); // Update listbox
        iListBox->SetCurrentItemIndexAndDraw( 0 ); // select new item
        if( (iResultArray[0])->
                          ObjectClass().Find( KClassImage ) == 0 ||
        (iResultArray[0])->
                          ObjectClass().Find( KClassVideo ) == 0)
            {
            UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__SHOW );
            }
        else if( ( iResultArray[0] )->ObjectClass()
                                  .Find( KClassAudio ) == 0 )
            {
            UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY );
            }        
        else
            {
            UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            } 
        }


    CleanupStack::PopAndDestroy( item );
    }
    
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::DynInitMenuPaneL(TInt aResourceId, 
//                                             CEikMenuPane *aMenuPane)
// creates dynamical menu according to result array users 
// selections/highlighted item
// --------------------------------------------------------------------------
//    
void  CAdvancedFindResultWindow::DynInitMenuPaneL( TInt aResourceId, 
                                                   CEikMenuPane *aMenuPane )
    {
    TInt selected_item = iListBox->CurrentItemIndex();
    if ( aResourceId ==  R_UPNPCOMMONUI_RESULT_WINDOW_MENU )
        {
        aMenuPane->SetItemDimmed( EUPnPShow, ETrue );
        aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
        if (iResultArray.Count() == 0)
            {
            aMenuPane->SetItemDimmed( EUPnPCopy, ETrue );
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
            }
        else
            {
            const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
            if ( indexes->Count() ) //if items are marked
                {
                if( UPnPItemUtility::BelongsToClass( 
                                            *iResultArray[selected_item], 
                                            KClassAudio ) 
                    && IsAudioItemMarked() )
                    {
                    //if an audio is focused
                    aMenuPane->SetItemDimmed(EUPnPPlay, EFalse);
                    }
                else
                    { 
                    aMenuPane->SetItemDimmed(EUPnPPlay, ETrue);
                    } 
                }
            //check highlited items        
            else if ( ( ( iResultArray[iListBox->CurrentItemIndex()])->
                   ObjectClass().Find( KClassVideo ) == 0 ||
                 ( iResultArray[iListBox->CurrentItemIndex()])->
                   ObjectClass().Find( KClassImage ) == 0 ) )
                {
                aMenuPane->SetItemDimmed( EUPnPShow, EFalse );
                }
            else if ( ( iResultArray[iListBox->CurrentItemIndex()])->
                        ObjectClass().Find( KClassAudio ) 
                        == 0 )  //if music
                {
                aMenuPane->SetItemDimmed( EUPnPPlay, EFalse );
                }
            }                    

        // hide help option if not supported
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    else if( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {        
        TBool markHidden = iListBox->View()->ItemIsSelected( selected_item );
        TBool unmarkHidden = !iListBox->View()->
                                            ItemIsSelected( selected_item );
        TBool markAllHidden = iListBox->Model()->NumberOfItems() == 0 ||
        iListBox->SelectionIndexes()->Count()==iListBox->Model()->
                                                            NumberOfItems();
        TBool unmarkAllHidden = iListBox->Model()->NumberOfItems() == 0 ||
                                iListBox->SelectionIndexes()->Count() == 0;
        aMenuPane->SetItemDimmed( EAknCmdMark, markHidden );
        aMenuPane->SetItemDimmed( EAknCmdUnmark, unmarkHidden );
        aMenuPane->SetItemDimmed( EAknMarkAll, markAllHidden );
        aMenuPane->SetItemDimmed( EAknUnmarkAll, unmarkAllHidden );
        }
    
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    }
  
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::GetHelpContext
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::GetHelpContext( 
                                            TCoeHelpContext& aContext ) const
    {
    __LOG( "CAdvancedFindResultWindow::GetHelpContext");
    
//  aContext.iMajor = TUid::Uid( KMediaGalleryUID3 );    
    aContext.iContext = KUPNP_HLP_SEARCH;
    }
    
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ShowVideoExtL
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::ShowVideoExtL()
    {
    
    TInt error = KErrGeneral;
    iAction = CUPnPCommonUI::EUPnPShow;
    iChildDialogOpen++;
    error = iCommonUI.SelectDeviceL(
                              iAVControl,
                              *iTargetDevice,
                              EUPnPSearchRenderingDevicesWithVideoCapability,
                              EUPnPSelectDeviceTitle );
    iChildDialogOpen--;
    if ( KErrNone == error )
        {
        const CUpnpObject* selectedObject = iResultArray[iListBox->
                                   CurrentItemIndex()];
                    
        if( iSourceDevice )
            {
            
            MUPnPAVRenderingSession* renderingSession = 
                     &(iAVControl.StartRenderingSessionL( *iTargetDevice ) );
            iChildDialogOpen++;
            
            TInt ret = KErrNone;
            //trap here to release rendering session properly
            TRAP( error, ret = iCommonUI.ExecuteVideoPlayerL(
                                                   *renderingSession, 
                                                   *selectedObject ) );
            
            if( ret < KErrNone )
                {
                error = ret;
                }
            
            //set back panel and navi text
            
            // Get device friendly name and replace illegal characters.
            HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
                iSourceDevice->FriendlyName() );
            
            CleanupStack::PushL( tmpfriendlyname );
            TPtrC8 friendlyname = *tmpfriendlyname;
            
            HBufC* titleString = 
                UpnpString::ToUnicodeL( friendlyname.Left( KLength ) );
            CleanupStack::PushL( titleString );
            
            iTitlePane->SetTextL( *titleString );
            CleanupStack::PopAndDestroy( titleString );
            CleanupStack::PopAndDestroy( tmpfriendlyname );
            
            SetNaviPaneTextL();
            
            renderingSession->RemoveObserver();
            iAVControl.StopRenderingSession( *renderingSession );
            
            iChildDialogOpen--;
            }
        }
    if( KErrNone != error )
        {
        User::Leave( error );    
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ShowImageExtL
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::ShowImageExtL()
    {
    TInt error = KErrNone;
    iAction = CUPnPCommonUI::EUPnPShow;
    iChildDialogOpen++;
    error= iCommonUI.SelectDeviceL(
                              iAVControl,
                              *iTargetDevice,
                              EUPnPSearchRenderingDevicesWithImageCapability,
                              EUPnPSelectDeviceTitle );
    iChildDialogOpen--;
    
    if ( KErrNone == error )
        {
        if( !iImagePlayer )
            {
            iImagePlayer = CUpnpImagePlayer::NewL( iAVControl, 
                                                   *this,
                                                   iCommonUI );
            }
        iImagePlayer->SetTargetDeviceL( *iTargetDevice );
        
        StartImageControlTimer();
        iImageControlActive = ETrue;
        UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
        }
    else
        {
        User::Leave( error );
        }    
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ShowCurrentImageL
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::ShowCurrentImageL( TBool aShowImage )
    {    
    TInt currentItemIndex = iListBox->CurrentItemIndex();
    if ( currentItemIndex < 0 )
        {
        currentItemIndex = 0;
        }
    
    CDesCArray* listBoxItems = 
        static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );
    
    if( iLastImageItemIndex >= 0 ) // if last item was image
        {
        TBuf<KMaxFileName> lastItem;
        HBufC *lastTmpItem = UpnpString::ToUnicodeL( 
            iResultArray[iLastImageItemIndex]->Title().Left( KLength ) );
        CleanupStack::PushL( lastTmpItem );
        lastItem.Format( KImageFormatString(), 
                         EUPnPIconImage, 
                         lastTmpItem);
        CleanupStack::PopAndDestroy ( lastTmpItem );
                 
        listBoxItems->Delete( iLastImageItemIndex );
        listBoxItems->InsertL( iLastImageItemIndex, lastItem );
        iListBox->HandleItemAdditionL();
        if( !( UPnPItemUtility::BelongsToClass( 
                *iResultArray[iListBox->CurrentItemIndex()], KClassImage ) ) )
            {
            iLastImageItemIndex = KErrNotFound;    
            }
        }    

    if( UPnPItemUtility::BelongsToClass( 
            *iResultArray[iListBox->CurrentItemIndex()], KClassImage ) )
        {
        if( ( CUpnpItem* )iResultArray[currentItemIndex]
                                                ->Title().Length() != 0 )
            {
            HBufC *tmpItem = UpnpString::ToUnicodeL( 
                iResultArray[ currentItemIndex ]->Title().Left( KLength ) );
            CleanupStack::PushL( tmpItem );
            
            TBuf<KMaxFileName> item;
            // if not showing an image
            if ( !aShowImage )
                {   
                item.Format( KImageFormatString(), EUPnPIconImage, tmpItem);
                iLastImageItemIndex = KErrNotFound;
                listBoxItems->Delete( currentItemIndex );
                listBoxItems->InsertL( currentItemIndex, item );
                }
            else // if showing an image
                {
                
                item.Format( KImageShowingFormatString() ,EUPnPIconImage,
                                               tmpItem,
                                               EUPnPIconImageShowing );
                if(iImagePlayer)
                    {
                    iAction = CUPnPCommonUI::EUPnPShow;
                    TRAPD( error, iImagePlayer->PlayL( 
                                  *iResultArray[currentItemIndex] ) );
                    
                    HandleErrorL( error );
                    
                   }     

                // listBoxItem.Set( item );
                listBoxItems->Delete( currentItemIndex );
                listBoxItems->InsertL( currentItemIndex, item );
                iLastImageItemIndex = currentItemIndex;
                }
            CleanupStack::PopAndDestroy ( tmpItem );
            iListBox->HandleItemAdditionL();
            iListBox->SetCurrentItemIndexAndDraw( currentItemIndex );
            }
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::StartImageControlTimer
// Starts periodic timer
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::StartImageControlTimer()
    {
    __LOG( "CAdvancedFindResultWindow::StartImageControlTimer" );
    iImageControlTimer->Start( 
                        KImageTimerDelay,
                        KImageTimerInterval,
                        TCallBack( ImageControlTimerCallbackL, this ) );
    __LOG( "CAdvancedFindResultWindow::StartImageControlTimer-END" );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::ImageControlTimerCallbackL
// Callback method for the Timer.
// --------------------------------------------------------------------------
TInt CAdvancedFindResultWindow::ImageControlTimerCallbackL( TAny* aDlg )
    {
    __LOG( "CAdvancedFindResultWindow::ImageControlTimerCallbackL" );
    
    static_cast< CAdvancedFindResultWindow* >( aDlg )->
                                        iImageControlTimer->Cancel();
    static_cast< CAdvancedFindResultWindow* >( aDlg )->
                                        ShowCurrentImageL( ETrue );
    
    __LOG( "CAdvancedFindResultWindow::ImageControlTimerCallbackL-END" );
    return KErrNone;
}

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::StopImageControlL
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::StopImageControlL()
    {
    __LOG( "CAdvancedFindResultWindow::StopImageControlL" );
        
    if ( iImageControlActive )
        {
        iImageControlActive = EFalse;
        iImageControlTimer->Cancel();
        ShowCurrentImageL( EFalse );
        
        const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
        if ( indexes->Count() )
            {
            if( UPnPItemUtility::BelongsToClass( 
                            *iResultArray[iListBox->CurrentItemIndex()], 
                            KClassAudio )
                && IsAudioItemMarked() ) 
                {
                UpdateSoftkeysL( 
                            R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY );
                }
            else
                {
                UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                }
            }
        else // check the hightlighted item
            {
            TInt tempCounter = iListBox->CurrentItemIndex();
            if( UPnPItemUtility::BelongsToClass( 
                    *iResultArray[tempCounter], KClassImage ) ||
                UPnPItemUtility::BelongsToClass( 
                    *iResultArray[tempCounter], KClassVideo ) )
                {
                UpdateSoftkeysL(
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__SHOW );
                }
            else if( UPnPItemUtility::BelongsToClass( 
                    *iResultArray[tempCounter], KClassAudio ) )
                {
                UpdateSoftkeysL( 
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY );
                }        
            else
                {
                UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                }
            }

        iAction = CUPnPCommonUI::EUPnPNone;
        iImagePlayer->Stop();
        }
    
    __LOG( "CAdvancedFindResultWindow::StopImageControlL-END" );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::DismissItselfL
// --------------------------------------------------------------------------
//
void CAdvancedFindResultWindow::DismissItselfL( TInt aError )
    {
    __LOG( "CAdvancedFindResultWindow::DismissItselfL" );
    iMSDisappear = ETrue;
    if( iChildDialogOpen > 0 ) //if video or music dialog open
        {
        __LOG( "CAdvancedFindResultWindow::iCommonUI.DismissDialogL" );
        iCommonUI.DismissDialogL( aError );
        }
    else
        {
        __LOG( "CAdvancedFindResultWindow::TryExitL" );
        TryExitL( aError ); //media server disappeared or WLAN lost
        }
    __LOG( "CAdvancedFindResultWindow::DismissItselfL End" );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::DeviceDisappeared
// called by image play only, no implementation
// --------------------------------------------------------------------------
//   
void CAdvancedFindResultWindow::DeviceDisappeared( TInt aError )
    {
    TRAP_IGNORE( HandleErrorL( aError ) );
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::HandleCopyL
// --------------------------------------------------------------------------
//   
void CAdvancedFindResultWindow::HandleCopyL()
    {
    iAction = CUPnPCommonUI::EUPnPCopy;
    iCopyIndex = NULL;
    RPointerArray<CUpnpItem> tempArrayForCopy;
    CleanupResetAndDestroyPushL( tempArrayForCopy );            
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( indexes->Count() )
        {            
        for (TInt count=0; count < indexes->Count(); count++)
            {
            CUpnpItem* tempItem = CUpnpItem::NewL();
            CleanupStack::PushL( tempItem );
            tempItem->CopyL( *iResultArray[indexes->At(count)] );
            tempArrayForCopy.AppendL( tempItem );
            CleanupStack::Pop( tempItem );
            }
        iCopyIndex = indexes->Count();    
        }                
    else
        {             
        CUpnpItem* tempItem = CUpnpItem::NewL();
        CleanupStack::PushL( tempItem );
        tempItem->
                CopyL( *iResultArray[iListBox->CurrentItemIndex()] );
        tempArrayForCopy.AppendL( tempItem );
        CleanupStack::Pop( tempItem );
        iCopyIndex = 1;
        }            
    
    CUpnpFileTransferEngine* ftEngine = NULL;
    // Instantiate the UPnP File Transfer Engine
    ftEngine = CUpnpFileTransferEngine::NewL( &iBrowseSession );
    CleanupStack::PushL( ftEngine );
        
    ftEngine->CopyRemoteItemsToHandsetL( tempArrayForCopy );
        // Clean up ftEngine
    CleanupStack::PopAndDestroy( ftEngine );
    ftEngine = NULL;
        
    // Clean up tempArrayForCopy
    CleanupStack::PopAndDestroy( &tempArrayForCopy );
    }
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::HandleErrorL
// --------------------------------------------------------------------------
//     
void CAdvancedFindResultWindow::HandleErrorL( TInt aError )
    {
    iCommonUI.GetUpnpAction( iAction );
    //if media server or WLAN lost, close the browse dialog    
    if( ( KErrSessionClosed == aError && iMSDisappear )||
          KErrDisconnected == aError ||
          EAknCmdExit == aError ||
          EEikCmdExit == aError )
        {
        if( iChildDialogOpen > 0 ) 
            {
            //if some dialos are open on the top of browse dialog, 
            //close those dialogs and do the corresponding action via
            //errors returned from them 
            iCommonUI.DismissDialogL( aError );
            }
        else //if no, do the corresponding action via the error
            {
            TryExitL( aError );    
            }
        }
     else
        {
        // if media renderer disappears
        if( KErrSessionClosed == aError && !iMSDisappear )
            {
            StopImageControlL();   
            }
        
        iCommonUI.HandleCommonErrorL( aError, iCopyIndex );
        iAction = CUPnPCommonUI::EUPnPNone;
        iCopyIndex = NULL;
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::UpdateSoftkeysL
// Update CBA-keys
// --------------------------------------------------------------------------

void CAdvancedFindResultWindow::UpdateSoftkeysL( TInt aResourceId )
    {   
    __LOG( "CAdvancedFindResultWindow::UpdateSoftkeysL" );
    CEikButtonGroupContainer* cba = &ButtonGroupContainer();
    cba->SetCommandSetL( aResourceId );
    cba->DrawDeferred();
    __LOG( "CAdvancedFindResultWindow::UpdateSoftkeysL-END" );
    }
    
// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::IsAudioItemMarked
// --------------------------------------------------------------------------
TBool CAdvancedFindResultWindow::IsAudioItemMarked( void )
    {
    TBool mark = EFalse;
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( indexes->Count() ) //if items are marked
        {
        for (TInt count=0; count < indexes->Count(); count++)
            {
            if ( ( iResultArray[indexes->At(count)])->
                                ObjectClass().Find( KClassAudio )
                   == 0 ) //audio
                {
                mark = ETrue;
                count = indexes->Count();
                }                
            }
        }
    __LOG( "CAdvancedFindResultWindow::IsAudioItemMarked-END" );      
    return mark;

    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::UpdateCommandButtonAreaL( 
//      TBool aMark, TInt tempCounter )
// Updates command button area
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::UpdateCommandButtonAreaL( TBool aMark,
        TInt aTempCounter )
    {
    if( aTempCounter>=0 && iResultArray.Count() )
        {
        if( !aMark ) // no marked items in the list box
            {
            if( ( UPnPItemUtility::BelongsToClass(
                    *iResultArray[aTempCounter], KClassImage ) ) ||
            ( UPnPItemUtility::BelongsToClass(
                    *iResultArray[aTempCounter], KClassVideo ) ) )
                {
                UpdateSoftkeysL( 
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__SHOW );
                }
            else if( ( UPnPItemUtility::BelongsToClass(
                    *iResultArray[aTempCounter], KClassAudio ) ) )
                {
                UpdateSoftkeysL( 
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY );
                }        
            else if( ( UPnPItemUtility::BelongsToClass(
                    *iResultArray[aTempCounter], KClassContainer ) ) )
                {
                UpdateSoftkeysL( 
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__OPEN );
                }
            else
                {
                UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                }        
            }
        else // at least one marked item in the list box
            {
            if( ( UPnPItemUtility::BelongsToClass(
                    *iResultArray[aTempCounter], KClassContainer ) ) )
                {
                UpdateSoftkeysL( 
                        R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__OPEN );
                }        
            else if( !IsAudioItemMarked() )
                {
                //if no audio item have been marked.
                UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                }
            else
                {
                if( !( UPnPItemUtility::BelongsToClass(
                        *iResultArray[aTempCounter], KClassAudio ) ) )
                    {
                    UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                    }
                else
                    {
                    UpdateSoftkeysL( 
                            R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY );
                    }
                }       
            }
        }
    }

// --------------------------------------------------------------------------
// CAdvancedFindResultWindow::UpdateCommandButtonArea( 
//      TBool aMark, TInt tempCounter )
// Updates command button area
// --------------------------------------------------------------------------
void CAdvancedFindResultWindow::UpdateCommandButtonArea( TBool aMark,
        TInt aTempCounter )
    {
    TInt error = KErrNone;
    TRAP( error, UpdateCommandButtonAreaL( aMark, aTempCounter ) );
    if( error )
        {
        __LOG1( "UpdateCommandButtonAreaL error,error=%d", error );
        }
    }
// End of file
