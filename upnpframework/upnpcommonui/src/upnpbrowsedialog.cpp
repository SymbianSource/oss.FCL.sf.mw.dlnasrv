/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for UPnP Browse UI Implementation
*
*/


// INCLUDE FILES
// System
#include <featmgr.h>
#include <AknIconArray.h>
#include <aknnavide.h>
#include <eikclbd.h>
#include <akntabgrp.h>
#include <StringLoader.h>
#include <hlplch.h>
#include <commondialogs.mbg>
#include <AknWaitDialog.h>
#include <upnpstring.h>
#include <AknDialog.h>
#include <akntitle.h>
#include <aknlists.h>
#include <aknnotewrappers.h> 
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <httperr.h>

// upnp stack api's
#include <upnpitem.h>
#include <upnpcontainer.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"   //start browse session and rendering session
#include "upnpavdevice.h"
#include "upnpavbrowsingsession.h"
#include "upnpavrenderingsession.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // upnp definitions

// upnpframework / xml parser api
#include "upnpxmlparser.h"

// upnpframework / utility class UPnPItemUtility
#include "upnpitemutility.h"

// upnpframework / internal api's
#include "upnpfiletransferengine.h"
#include "upnpmusicadapter.h"
#include "upnpbrowseplaylistfiller.h"

// common ui internal
#include "upnpcommonui.h"
#include <upnpcommonui.rsg>
#include <upnpcommonui.mbg>
#include "upnpcommonutils.h"

#include "upnplocalplayer.h"
#include "upnpbrowsedialog.h"
#include "upnpcommonui.hrh"
#include "upnpbrowsecacheitem.h"
#include "upnpimageplayer.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"


//CONSTANTS
_LIT( KAknCommonUIMbmFileName, "\\resource\\apps\\upnpcommonui.mbm" );
_LIT( KFormatString,  "%d\t%S\t\t" );
_LIT( KFormatString2, "%d\t%S\t\t%d" );
_LIT( KUPNP_HLP_REMOTE_DEVICES, "UPNP_HLP_REMOTE_DEVICES" ); 
_LIT( KUPNP_HLP_CONTENT_VIEW, "UPNP_HLP_CONTENT_VIEW" ); 

const TInt KBrowseBack = -1;
const TInt KBrowseRoot = -2;
const TInt KBrowseForward = -3;
const TInt KBrowseRequestCount = 20;
const TInt KWindowBrowseTricker = 3;

const TInt KImageTimerDelay = 800000;
const TInt KImageTimerInterval = 800000;

const TInt KLength = 100;

// CONSTANTS
enum TUPnPIconTypes
    {
    EUPnPIconFolder = 1,
    //EUPnPIconFolderLocked,
    EUPnPIconMusic,
    EUPnPIconVideo,
    EUPnPIconImage,
    EUPnPIconImageShowing,
    EUPnPIconOther,
    EUPnPIconLast
    };
enum TUPnPBrowseDirection
    {
    EBackward= 1,
    EForward
    };

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUPnPBrowseDialog* CUPnPBrowseDialog::NewL( TInt aMenuResource,
                                            MUPnPAVController& aAVControl,
                                            const CUpnpAVDevice& aDevice, 
                                            CUPnPCommonUI& aCommonUI)
    {
    __LOG( "CUPnPBrowseDialog::NewL" );

    CUPnPBrowseDialog* self = new (ELeave) CUPnPBrowseDialog(
        aAVControl, aCommonUI, aDevice );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuResource );
    
    CleanupStack::Pop( self );
    
    __LOG( "CUPnPBrowseDialog::NewL-END" );
    return self;
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ConstructL( TInt aMenuResource )
                                    
    {
    // Register as an observer to file operations (browse results)
    
    __LOG( "CUPnPBrowseDialog::ConstructL" );
    
    iBrowseSession = &iAVControl.StartBrowsingSessionL( iSourceDevice );
    iBrowseSession->SetObserver( *this );

    iFirstResultArray = ETrue;
    CAknDialog::ConstructL( aMenuResource );

    iTargetDevice = CUpnpAVDevice::NewL();
    iImageControlTimer = CUPnPPeriodic::NewL( CActive::EPriorityUserInput );
    
    iError = KErrNone;
    iDlgPreLayoutDone = EFalse;
    iErrorForCancel = KErrNone;
    __LOG( "CUPnPBrowseDialog::ConstructL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::CUPnPBrowseDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
CUPnPBrowseDialog::CUPnPBrowseDialog( MUPnPAVController& aAVControl,
                                      CUPnPCommonUI& aCommonUI,
                                      const CUpnpAVDevice& aDevice ):
                                      iCommonUI( aCommonUI ), 
                                      iAVControl( aAVControl ),
                                      iSourceDevice(aDevice)
    {
    __LOG( "CUPnPBrowseDialog::CUPnPBrowseDialog" );
    
    iBrowseRequestSent = EFalse;
    iChildDialogOpen = 0;
    iLastImageItemIndex = KErrNotFound;
    iMSDisappear = EFalse;
    iAction = CUPnPCommonUI::EUPnPNone;
    iCopyIndex = NULL;
    iRoot = ETrue;
    
    iIsMusicItem = EFalse;
    iHashKeyFlag = EFalse;
    __LOG( "CUPnPBrowseDialog::CUPnPBrowseDialog-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::~CUPnPBrowseDialog
// C++ default destructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
CUPnPBrowseDialog::~CUPnPBrowseDialog()
    {
    __LOG( "CUPnPBrowseDialog::~CUPnPBrowseDialog" );
    // Unregister as an observer to file operations (browse results)       
    TRAPD( err, DismissWaitNoteL() );
    if ( err != KErrNone )
        {
        __LOG( "CUPnPBrowseDialog::~CUPnPBrowseDialog \
                                                    delete WaitNote error" );
        } 
        
    if ( iCurrentFolderId && !iBrowseRequestSent )
        {
        TRAP_IGNORE( CacheItemUpdateL( *iCurrentFolderId, EFalse ) );
        }     
    
    delete iCurrentFolderId;
    
    delete iTargetDevice;
    delete iImagePlayer;
    iParentId.ResetAndDestroy();
    iParentName.ResetAndDestroy();
    iResultArray.Close();
    iBrowseCacheItems.ResetAndDestroy();
    iTempArray.Close();
    if ( iNaviPane && iNaviDecorator )
        {
        iNaviPane->Pop( iNaviDecorator );
        }
    
    delete iNaviDecorator;
    
    iSelectedItemsArray.ResetAndDestroy();
        
    if ( iImageControlTimer )  
        {
        iImageControlTimer->Cancel();
        delete iImageControlTimer;
        }    
     
    delete iLocalPlayer; 
    
    if( iBrowseSession )
        {
        iBrowseSession->CancelBrowse(); 
        iBrowseSession->RemoveObserver();
        iAVControl.StopBrowsingSession( *iBrowseSession );
        }
    
    __LOG( "CUPnPBrowseDialog::~CUPnPBrowseDialog-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DialogDismissedL();
// MProgressDialogCallback, progressbar callback function
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::DialogDismissedL( TInt aButtonId )
    {
    __LOG( "CUPnPBrowseDialog::DialogDismissedL" );
    // If button is cancel, inform observer parent class
    // that cancel has been made
    if( aButtonId == EEikBidCancel )
        {
        __LOG( "CUPnPBrowseDialog::DialogDismissedL: \
                Cancel was pressed.");
        iAction = CUPnPCommonUI::EUPnPNone;        
        iBrowseSession->CancelBrowse();
        iBrowseRequestSent = EFalse;
         
        TInt cachedItemIndex = CacheCheck( *iCurrentFolderId );
       
        if ( cachedItemIndex > -1 )
            {               
            delete iBrowseCacheItems[ cachedItemIndex ];
            iBrowseCacheItems[ cachedItemIndex ] = NULL;
            iBrowseCacheItems.Remove( cachedItemIndex );
            iBrowseCacheItems.Compress();
            }            
        // If button is cancel,
        // the browse dialog should not been displayed,leave it in the
        // function of PreLayoutDynInitL.
        iErrorForCancel = EEikBidCancel;     
        }
    __LOG( "CUPnPBrowseDialog::DialogDismissedL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::PreLayoutDynInitL()
    {
    __LOG( "CUPnPBrowseDialog::PreLayoutDynInitL" );

    // Browse dialog title text
    TUid titlePaneUid;
    titlePaneUid.iUid = EEikStatusPaneUidTitle;
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CEikStatusPaneBase::TPaneCapabilities titlesubPane =
        statusPane->PaneCapabilities( titlePaneUid );

    if ( titlesubPane.IsPresent() && titlesubPane.IsAppOwned() )
        {
        iTitlePane = (CAknTitlePane*) statusPane->ControlL( titlePaneUid );

        // Get device friendly name and replace illegal characters.
        HBufC8* tmpfriendlyname = 
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            iSourceDevice.FriendlyName() );
            
        CleanupStack::PushL( tmpfriendlyname );
        TPtrC8 friendlyname = *tmpfriendlyname;
        
        HBufC *tmpbuf = UpnpString::ToUnicodeL( 
            friendlyname.Left( KLength ) );
        CleanupStack::PushL( tmpbuf ); 
        iTitlePane->SetTextL( *tmpbuf );

        CleanupStack::PopAndDestroy( tmpbuf );
        CleanupStack::PopAndDestroy( tmpfriendlyname );
        }

    iListBox = static_cast<CAknSingleGraphicStyleListBox*>(
                                        Control( EUPnPBrowseListBoxId ) );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    CAknIconArray* icons = new ( ELeave ) CAknIconArray( EUPnPIconLast );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName mbmFileName( KAknCommonUIMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    AppendIconToArrayL( icons, 
        skin, 
        KAvkonBitmapFile, 
        KAknsIIDQgnIndiMarkedAdd, 
        EMbmAvkonQgn_indi_marked_add, 
        EMbmAvkonQgn_indi_marked_add_mask );

    // Folder icon
    AppendIconToArrayL( icons,
        skin, 
        mbmFileName, 
        KAknsIIDQgnPropFolderSmall, 
        EMbmUpnpcommonuiQgn_prop_mserv_folder_small, 
        EMbmUpnpcommonuiQgn_prop_mserv_folder_small_mask );
                   
                        
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
        
    //Image showing icon
    AppendIconToArrayL( icons,
        skin, 
        mbmFileName, 
        KAknsIIDDefault, 
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer_list_icon, 
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer_list_icon_mask );


    // Other icon
    AppendIconToArrayL( icons,
        skin, 
        KCommonDialogsBitmapFile, 
        KAknsIIDQgnPropFmgrFileSound, 
        EMbmCommondialogsQgn_prop_fmgr_file_other, 
        EMbmCommondialogsQgn_prop_fmgr_file_other_mask );
                                        
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    CleanupStack::Pop(icons);

    // Hide "No data" text
    iListBox->View()->SetListEmptyTextL( KNullDesC() );

    // Send browse request
    SendBrowseRequestL( KBrowseRoot );
    
    iDlgPreLayoutDone = ETrue;
    
    if( iError < KErrNone )
        {
        __LOG1( "CUPnPBrowseDialog::PreLayoutDynInitL leave %d", iError );
        User::Leave( iError );
        }  
        
    if( iErrorForCancel == EEikBidCancel )
        {
        User::Leave( EEikBidCancel );
        } 
    __LOG( "CUPnPBrowseDialog::PreLayoutDynInitL-END" );

    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::OkToExitL( TInt aButtonId )
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
TBool CUPnPBrowseDialog::OkToExitL( TInt aButtonId )
    {
    __LOG( "CUPnPBrowseDialog::OkToExitL" );
    
    TBool returnValue = EFalse;
    TInt error = KErrNone;
    switch (aButtonId )
        {
        // Connection failed. Dialog must be closed
        case KErrDisconnected:
        case KErrSessionClosed: //fall through
            {
            // exit only if PreLayoutDynInitL is done
            if( iDlgPreLayoutDone )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        case EAknSoftkeyOptions:
            {
            SelectedArrayCheckL();
            DisplayMenuL(); 
            break;
            }
        case EUPnPStopCmd: 
            {
            StopImageControlL();
            break;
            }
        case EAknSoftkeyBack:
            {
            // Check if we are in the root 
            if ( !iParentId.Count() )
                {
                ClearTitleL();
                return ETrue;
                }
            else
                {
                if ( iBrowseRequestSent )
                    {                    
                    iBrowseSession->CancelBrowse();
                    iBrowseRequestSent = EFalse;
                    }   
                 
                iListBox->ClearSelection();
                iSelectedItemsArray.ResetAndDestroy();
                SendBrowseRequestL( KBrowseBack );                    
                }           
        
            break;                               
            }
        case EUPnPPlayCmd: //for lsk only, play on external renderer
            {
            if( !iMusicPlay )
                {
                StopImageControlL();
                TRAP( error, PlayL( EFalse ) );
                if( KErrNone != error )
                    {
                    iMusicPlay = EFalse;
                    }    
                }
            
            break;
            }
        case EAknSoftkeyOpen: //fall thougth
        case EAknSoftkeyOk:
            {
            // Shift and Ok pressed so the external device dialog must not be
            // opened
            if ( iShiftAndOkPressed )
                {
                iShiftAndOkPressed = EFalse;
                return EFalse;
                }
            if ( !iResultArray.Count() || iImageControlActive )
                {
                return EFalse; // Do nothing if empty
                }
            else if ( iResultArray[iListBox->CurrentItemIndex()]->
                        ObjectType() == EUPnPContainer )
                {
                if ( !iBrowseRequestSent )
                    {
                    iListBox->ClearSelection();
                    iSelectedItemsArray.ResetAndDestroy();
                    SendBrowseRequestL( iListBox->CurrentItemIndex() );
                    }
                }
            else
                {
                // Play or Show on external device
                // Select device according to the media type
                TRAP( error, ShowPlayExtL() );
                
                /* if something happens, set iMusicPlay = EFalse,
                 * no matter what kinda of media file playing 
                 * before
                 */
                
                if( KErrNone != error )
                    {
                    iMusicPlay = EFalse;
                    }
                }
            break;
            }
        case EAknSoftkeyShow:
            {
            StopImageControlL();
            TRAP( error, ShowVideoDialogExtL() );
            break;
            }
        case EAknSoftkeyCancel:
        case EAknSoftkeyExit:
        case EAknCmdExit:
        case EEikCmdExit:   //fall through
            {
            ClearTitleL();
            return ETrue; //back to previous dialog
            }
        default:
            {
            break;
            }
        }//switch
    
    HandleErrorL( error );
    __LOG( "CUPnPBrowseDialog::OkToExitL End" );
    
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::OfferKeyEventL();
// called by framework when key is pressed 
// --------------------------------------------------------------------------
TKeyResponse CUPnPBrowseDialog::OfferKeyEventL(
            const TKeyEvent &aKeyEvent,
            TEventCode aType )
    {
    __LOG( "CUPnPBrowseDialog::OfferKeyEventL" );
    TInt currentItemIndex = iListBox->CurrentItemIndex();   
    TInt tempCounter = currentItemIndex;
    // Shift and Ok pressed at same time so the event must consumed
    if ( ( aKeyEvent.iModifiers & EModifierShift ) != 0 && 
        aType == EEventKeyDown && ( aKeyEvent.iScanCode == EStdKeyDevice3 ) )
        {
        iShiftAndOkPressed = ETrue;
        return EKeyWasConsumed;
        }
    if ( aType != EEventKey ) 
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
        CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        if ( aType == EEventKeyUp && !iHashKeyFlag )
            {
            if ( ( tempCounter < iResultArray.Count() ) && tempCounter > -1 )
                {
        
                if ( !iImageControlActive )
                    {
                    const CArrayFix<TInt>* indexes = 
                                                iListBox->SelectionIndexes();
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

    // Shift (pen) key cannot be used marking folders
    if ( currentItemIndex < 0 )
        {
        currentItemIndex = 0;
        }

    
    if ( ( aKeyEvent.iCode == EKeyUpArrow ) &&
         ( currentItemIndex == 0 ) )
        {
        return EKeyWasConsumed;
        }
    
    CTextListBoxModel* model = iListBox->Model();
    TInt numberOfItems = model->NumberOfItems();
    
    if ( iAllObjectsReceived && ( currentItemIndex == numberOfItems-1 )
        && aKeyEvent.iCode == EKeyDownArrow )
        {
        return EKeyWasConsumed;
        }
        
    // If selection is approaching the end of the list
    if ( ( aKeyEvent.iCode == EKeyDownArrow ) && ( !iRoot ) &&
         ( ( currentItemIndex + KWindowBrowseTricker ) >= numberOfItems ) &&
         ( iUppermostItem + numberOfItems < iTotalCount ) &&
         ( !iAllObjectsReceived ) &&
         ( !iBrowseRequestSent ) )
        {
        TInt startIndex = iUppermostItem + numberOfItems;   
        iAction = CUPnPCommonUI::EUPnPBrowse;         
        iOriginalBrowseRequest = ETrue;
        iBrowseSession->BrowseL( *iCurrentFolderId, 
                                 KFilterCommon,       /* filter */
                                 MUPnPAVBrowsingSession::EDirectChildren,
                                 startIndex,          /* start index */
                                 KBrowseRequestCount, /* request count */
                                 KSortNone );         /* sort criteria */
        iBrowseRequestSent = ETrue;
        iCurrentItem = currentItemIndex;
        iBrowseDirection = EForward;
        }
    if ( ( aKeyEvent.iCode == EKeyUpArrow ) && ( !iRoot ) &&
       ( ( (currentItemIndex - KWindowBrowseTricker ) <= 0 ) ) && 
       ( !iBrowseRequestSent ) && ( iUppermostItem > 0 ) )
        {
        TInt requestCount = KBrowseRequestCount;
        TInt startIndex = KErrNotFound;
        
        // To keep order of items of listbox don't change.
        if ( iNeedRepeatRequest )
            {
            startIndex = iUppermostItem - iServerReturnObjectCount;
            requestCount = iServerReturnObjectCount;
            if ( startIndex < 0 )
                {
                startIndex = 0;            
                }            
            }
        else
            {
            startIndex = iUppermostItem - KBrowseRequestCount;
            if ( startIndex < 0 )
                {
                requestCount = iUppermostItem;
                startIndex = 0;            
                }
            }
        iAction = CUPnPCommonUI::EUPnPBrowse;
        iOriginalBrowseRequest = ETrue;
        iBrowseSession->BrowseL( *iCurrentFolderId,
                                 KFilterCommon,       /* filter */
                                 MUPnPAVBrowsingSession::EDirectChildren,
                                 startIndex,     /* start index */
                                 requestCount,   /* request count */
                                 KSortNone );    /* sort criteria */
                                 
        iBrowseRequestSent = ETrue;
        iCurrentItem = currentItemIndex;                           
        iBrowseDirection = EBackward;
            
        if ( iAllObjectsReceived )
            {
            iAllObjectsReceived = EFalse;
            }
        
        }
    
    // it is checked that currentItemIndex is valid
    iCurrentItem = currentItemIndex;
    
    if ( ( aKeyEvent.iCode == EKeyDownArrow) &&
         ( currentItemIndex + 1) == numberOfItems )
        {
        return EKeyWasConsumed;
        }
        
    else if ( ( aKeyEvent.iCode == EKeyUpArrow) && 
              ( currentItemIndex == 0 ) && 
              ( iBrowseRequestSent ) )
        {
        return EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyDownArrow )
        {
        if ( ( numberOfItems - 1 ) > iCurrentItem )
            {
            iCurrentItem++;
            }
            else 
            {
            iCurrentItem = 0;   
            }       
        }
    else if ( aKeyEvent.iCode == EKeyUpArrow )
        {
         //if already if first item, should then go to the last one.
        if ( iCurrentItem > 0 )
            {
            iCurrentItem--;
            }
        }
    
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
        if ( ( tempCounter < iResultArray.Count() ) && tempCounter > -1 )
            {
            if( iImageControlActive )
                {
                // image or container
                if ( ( iResultArray[tempCounter] )->ObjectClass()
                          .Find( KClassImage ) == 0 ||
                     ( iResultArray[tempCounter] )->ObjectClass()
                                          .Find( KClassContainer ) == 0 ) 
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
                    }
                else if ( ( iResultArray[tempCounter] )->ObjectClass()
                              .Find( KClassVideo ) == 0 ) //video
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_SHOW_STOP );
                    }
                else if ( ( iResultArray[tempCounter] )->ObjectClass()
                              .Find( KClassAudio ) == 0 ) //music
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_PLAY_STOP );
                    }    
                else
                    {
                    UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
                    }    
                }
            }            
        }
    
    if ( ( currentItemIndex >= 0 ) && 
         ( currentItemIndex < iListBox->Model()->NumberOfItems() ) )
        {
        if ( ( ( iResultArray[currentItemIndex])->ObjectType()
                           != EUPnPContainer) ||
            !( aKeyEvent.iModifiers & EModifierShift ) )
            {
            if( aKeyEvent.iCode == EKeyEscape )
                {
                __LOG( "OfferKeyEventL EKeyEscape" );
                if( !iCopying )
                    {
                    CAknDialog::OfferKeyEventL( aKeyEvent, aType );
                    }
                else
                    {
                    __LOG( "Copying ongoing, app should be closed" );
                    iApplicationClose = ETrue;
                    __LOG( "Copying ongoing, app should be closed-end" );
                    }
                __LOG( "OfferKeyEventL EKeyEscape -end" );    
                }
            else
                {
                CAknDialog::OfferKeyEventL( aKeyEvent, aType );
                }    
            }
        }
    
    // no items in list, all events can be handled by system
    else if ( iListBox->Model()->NumberOfItems() == 0)  
        {
        CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }    
          
    
    __LOG( "CUPnPBrowseDialog::OfferKeyEventL End" );
    return EKeyWasConsumed;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ProcessCommandL( TInt aCommand )
// called by framework when menu item is selected
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ProcessCommandL( TInt aCommand )
    {
    __LOG( "CUPnPBrowseDialog::ProcessCommandL" );

    TInt error = KErrNone;
    TInt selected_item = iListBox->CurrentItemIndex();
    if ( selected_item < 0 )
        {
        selected_item = 0;
        }

    // Menu commands control
    HideMenu();
    switch ( aCommand ) 
        {
        case EAknCmdExit: 
        case EEikCmdExit:// fall through
            {
            TryExitL( aCommand );
            return;
            }
        case EUPnPOpen:
            {
            if ( iResultArray[selected_item]->ObjectType() == EUPnPContainer)
                {
                iListBox->ClearSelection();
                iSelectedItemsArray.ResetAndDestroy();
                SendBrowseRequestL( selected_item );
                }
            break;
            } 
        case EUPnPFind:
            {
            if ( iNaviDecorator )
                {
                iNaviPane->Pop( iNaviDecorator );
                delete iNaviDecorator;
                iNaviDecorator = NULL;
                }
            // cancel idle timer because player uses own timer
            error = iCommonUI.ExecuteAdvFindDialogL( iAVControl, 
                                                     *iBrowseSession );
                        
            //only MS or WLAN lost can make advfind dlg exit
            if( KErrSessionClosed == error )
                {
                iMSDisappear = ETrue;
                }
                
            //if not media server or wlan lost, update its navipane
            if( !iMSDisappear && error != KErrDisconnected )
                {
                UpdateNaviPaneTextL();    
                }
              
            break;
            }
        case EUPnPHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() );
            break;
            }
        case EUPnPPlayExt:
        case EUPnPShowExt: //fall through
            {
            TRAP( error, ShowPlayExtL() );
            if( KErrNone != error )
                {
                iMusicPlay = EFalse;
                }
            break;
            }
        case EUPnPShowLocal: //show image & video on local
            {
            if( !iLocalPlayer )
                {
                iLocalPlayer = CUPnPLocalPlayer::NewL( iAVControl,
                                                       *iBrowseSession,
                                                       iCommonUI );
                }
            const CUpnpObject* item = 
                            iResultArray[ iListBox->CurrentItemIndex() ];
            
            iAction = CUPnPCommonUI::EUPnPShow;                
            TRAP( error, iLocalPlayer->PlayL( *item ) );
            if( KErrSessionClosed == error )
                {
                iMSDisappear = ETrue;
                }
            break;
            }
        case EUPnPPlayLocal: //play music on local
            {
            if( ( iResultArray[ iListBox->CurrentItemIndex() ]
                        ->ObjectClass().Find( KClassAudio ) == 0 ||
                iIsMusicItem ) && !iMusicPlay )
                {
                TRAP( error, PlayL( ETrue ) );
                if( KErrNone != error )
                    {
                    iMusicPlay = EFalse;
                    }
                }
            break;
            }
        case EUPnPCopy:
            {
            TRAP( error, HandleCopyL() );
            iCopying = EFalse;
            __LOG1( "HandleCopyL is finished: %d", error );
            if( iApplicationClose )
                {
                __LOG( "copying is onging, exit" );
                TryExitL( EAknCmdExit );
                return;
                }
            
            //During copying, sometimes we get the httperr,
            //but doesn't handle it, at here I transfer the httperr
            //to symbian error.
            if ( KErrHttpPartialResponseReceived == error || 
                 KErrHttpRequestNotSent == error ||
                 KErrHttpResponseNotReceived == error )
                {
                error =  KErrSessionClosed;
                }
                
            __LOG1( "CUPnPBrowseDialog::HandleCopyL: %d", error );
            
            if( KErrSessionClosed == error ) //if server lost
                {
                iMSDisappear = ETrue;
                }
            else if( KErrNotFound == error )
                {
                if( iResultArray[iListBox->CurrentItemIndex()]->
                                            ObjectType() == EUPnPContainer )
                    {
                    if( iCopyIndex > 0 ) //if the container is not empty
                        {
                        iCommonUI.DisplayErrorTextL( 
                                R_UPNPCOMMONUI_GENERAL_FAILURE_ERROR_TEXT );
                        iAction = CUPnPCommonUI::EUPnPNone;
                        }
                    else //if the container is empty
                        {
                        iCopyIndex = ETrue;
                        }
                    
                    }
                else
                    {
                    iCopyIndex = EFalse;
                    }
                }
            break;
            }
        case EAknCmdMark:
            {
            MarkItemL( iListBox->CurrentItemIndex() );
            break;
            }
        case EAknMarkAll:
            {
            MarkAllItemsL();
            break;
            }
        case EAknCmdUnmark: 
            {
            UnmarkItem( iListBox->CurrentItemIndex() );
            break;
            }
        case EAknUnmarkAll:
            {
            UnmarkAllItems();
            break;
            }
        default:
            {
            CAknDialog::ProcessCommandL( aCommand );
            break;
            }
        }
        
    HandleErrorL( error );    
    __LOG( "CUPnPBrowseDialog::ProcessCommandL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane 
// *aMenuPane)
// called by framework before menu panel is shown
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::DynInitMenuPaneL( TInt aResourceId, 
                                          CEikMenuPane *aMenuPane )
    {
    __LOG( "CUPnPBrowseDialog::DynInitMenuPaneL" );

    TVolumeInfo info;
    
    TInt selected_item = iListBox->CurrentItemIndex();

    if ( aResourceId == R_UPNPCOMMONUI_BROWSE_OPTION_MENU )
       {
        iIsMusicItem = EFalse;
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EUPnPHelp, ETrue );
            }

        if ( !iSourceDevice.SearchCapability() )
            {
            aMenuPane->SetItemDimmed( EUPnPFind, ETrue );
            }
        if ( iResultArray.Count() == 0 )
            {
            aMenuPane->SetItemDimmed( EUPnPOpen, ETrue );
            aMenuPane->SetItemDimmed( EUPnPFind, ETrue );
            aMenuPane->SetItemDimmed( EUPnPCopy, ETrue );
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
            aMenuPane->SetItemDimmed( EUPnPShow, ETrue );
            aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
            }
        // Something marked
        else if( iSelectedItemsArray.Count() )
            {
            // Only one video or image be able to show on external device
            for ( TInt count = 0; 
                  count < iSelectedItemsArray.Count();
                  count++)
                {
                if ( ( iSelectedItemsArray[ count ] )->
                        ObjectClass().Find( KClassAudio ) == 0 )
                    {
                    //if marked items have at least one audio item
                    iIsMusicItem = ETrue;
                    count = iSelectedItemsArray.Count();
                    }
                else
                    {
                    iIsMusicItem = EFalse;
                    }                    
                }
              
            aMenuPane->SetItemDimmed( EUPnPShow, ETrue );  
            if ( UPnPItemUtility::BelongsToClass( 
                                *iResultArray[selected_item], KClassAudio )
                 && IsAudioItemMarked() )
                {
                aMenuPane->SetItemDimmed( EUPnPPlay, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
                } 
            
            // we must enable unmark all
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, EFalse );

            //only items are marked, disable EUPnPOpen
            aMenuPane->SetItemDimmed( EUPnPOpen, ETrue );
            
            }
        else if( selected_item >= 0 ) //if no marked items
            {
             // Focus on a container
            if ( iResultArray[ selected_item ]->
                            ObjectType() == EUPnPContainer )
                {
                aMenuPane->SetItemDimmed( EUPnPShow, ETrue );
                aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
                if ( iResultArray[ selected_item ]->ObjectClass()
                    .Find( KClassPlaylist ) == 0 )
                    {
                    //aMenuPane->SetItemDimmed(EUPnPPlay, ETrue);
                    }
                
                aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
                }
            else //if hightlighted item
                {
                // show edit list menu
                aMenuPane->SetItemDimmed( EAknCmdEditListMenu, EFalse );
                aMenuPane->SetItemDimmed( EUPnPOpen, ETrue );
                if( ( iResultArray[ selected_item ] )->
                                     ObjectClass().Find( KClassAudio ) == 0 )
                    {
                    aMenuPane->SetItemDimmed( EUPnPShow, ETrue );
                    aMenuPane->SetItemDimmed( EUPnPPlay, EFalse );
                    }
                else if( ( ( iResultArray[ selected_item ] )->ObjectClass()
                                        .Find( KClassImage ) == 0
                         || ( iResultArray[ selected_item ])->ObjectClass()
                                        .Find( KClassVideo ) == 0 ) 
                         && !iSelectedItemsArray.Count() )
                    {
                    if( iIsMusicItem )
                        {
                        aMenuPane->SetItemDimmed( EUPnPPlay, EFalse );
                        }
                    else
                        {
                        aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
                        }
                    aMenuPane->SetItemDimmed( EUPnPShow, EFalse );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EUPnPShow, ETrue );
                    if( iIsMusicItem )
                        {
                        aMenuPane->SetItemDimmed( EUPnPPlay, EFalse );
                        }
                    else
                        {
                        aMenuPane->SetItemDimmed( EUPnPPlay, ETrue );
                        }
                    }
                }
            }
       
        }
    else if ( aResourceId == 
                R_UPNPCOMMONUI_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {
        TBool markHidden = iListBox->View()->ItemIsSelected( selected_item );
        TBool unmarkHidden = !iListBox->View()->
                                        ItemIsSelected( selected_item );
        TBool markAllHidden = iListBox->Model()->NumberOfItems() == 0 || 
            iListBox->SelectionIndexes()->Count() == 
            iListBox->Model()->NumberOfItems();
        TBool unmarkAllHidden = iListBox->Model()->NumberOfItems() == 0 ||
            iSelectedItemsArray.Count() == 0;

        if ( iResultArray[ selected_item ]->ObjectType() == EUPnPContainer )
            {
            aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );
            aMenuPane->SetItemDimmed( EAknMarkAll, ETrue );
            }
        else 
            {
            aMenuPane->SetItemDimmed( EAknCmdMark, markHidden );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, unmarkHidden );
            aMenuPane->SetItemDimmed( EAknMarkAll, markAllHidden );
            aMenuPane->SetItemDimmed( EAknUnmarkAll, unmarkAllHidden );
            }
        }
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    __LOG( "CUPnPBrowseDialog::DynInitMenuPaneL End" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DeleteListItemsL
// Delete old items from browse list.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::DeleteListItemsL()
    {
    __LOG ( "CUPnPBrowseDialog::DeleteListItemsL" );

    CTextListBoxModel* model = iListBox->Model();
    TInt currentItem = iListBox->CurrentItemIndex();
    if ( currentItem < 0 )
        {
        currentItem = 0;
        }
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );    
    listBoxItems->Delete( 0,listBoxItems->Count() );
    AknListBoxUtils::HandleItemRemovalAndPositionHighlightL( iListBox,
                                                            currentItem,
                                                            ETrue );
    __LOG ( "CUPnPBrowseDialog::DeleteListItemsL End" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::LoadListItemsL
// Loads browse result set to the screen
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::LoadListItemsL()
    {
    __LOG( "CUPnPBrowseDialog::LoadListItemsL" );

    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );

    TBuf<KMaxFileName> item;

    if( !iResultArray.Count() )
        {
        iListBox->View()->SetListEmptyTextL(
            *StringLoader::LoadLC( R_UPNPCOMMONUI_EXTERNAL_EMPTY_FOLDER ) );
        CleanupStack::PopAndDestroy(); 
        UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
        }
    else
        {
        for (TInt i=0; i < iResultArray.Count(); i++)
            {
            // Get title and replace illegal characters.
            HBufC8* tmptitle = 
            UPnPCommonUtils::FixListboxItemTextL( 
                iResultArray[i]->Title().Left( KLength ) );
            CleanupStack::PushL( tmptitle );

            TPtrC8 tmpnameptr = *tmptitle;            
            iResultArray[i]->SetTitleL( tmpnameptr );

            HBufC *tmpItem = UpnpString::ToUnicodeL( 
                tmpnameptr.Left( KLength ) );
            CleanupStack::PushL( tmpItem );
                        
            if ( iResultArray[ i ]->ObjectType() == EUPnPContainer )
                {
                item.Format( KFormatString(), EUPnPIconFolder, tmpItem );    
                }
            else
                {
                if ( (iResultArray[ i ] )->ObjectClass()
                                        .Find( KClassAudio ) == 0 )
                    {
                    item.Format( KFormatString(), EUPnPIconMusic,
                                                        tmpItem );
                    }
                else if ( ( iResultArray[ i ] )->ObjectClass()
                                        .Find( KClassVideo ) == 0 )
                    {
                    item.Format( KFormatString(), EUPnPIconVideo,
                                                        tmpItem ); 
                    }
                else if ( ( iResultArray[ i ] )->ObjectClass()
                                        .Find( KClassImage ) == 0 )
                    {
                    item.Format( KFormatString(), EUPnPIconImage,
                                                        tmpItem );
                    }
                else
                    {
                    item.Format( KFormatString(), EUPnPIconOther,
                                                        tmpItem );
                    }
                }
            CleanupStack::PopAndDestroy ( tmpItem ); 
            CleanupStack::PopAndDestroy ( tmptitle ); 
            listBoxItems->AppendL( item );
            
            if ( iSelectedItemsArray.Count() )
                {
                for ( TInt index = 0; 
                      index < iSelectedItemsArray.Count(); 
                      index++ )
                    {
                    for ( TInt i = 0; i < iResultArray.Count() ; i++ )
                        {
                        if ( !( iResultArray[ i ])->Id().CompareC(
                                       iSelectedItemsArray[ index ]->Id() ) )
                            {
                            MarkItemL( i );
                            i = iResultArray.Count();
                            }
                        }
                    }
                }
            } //for
        
        iListBox->HandleItemAdditionL(); // Update listbox    
        // Ensure iCurrentItem isn't out of bounds
        if ( iCurrentItem < 0 )
            {
            iCurrentItem = 0;
            }
        if ( iCurrentItem >=  model->NumberOfItems() )
            {
            iCurrentItem  = model->NumberOfItems() - 1;
            }
            
        // select new item
        iListBox->SetCurrentItemIndexAndDraw( iCurrentItem );    
        if( !iImageControlActive )
            {
            if( (iResultArray[iCurrentItem])->
                              ObjectClass().Find( KClassImage ) == 0 ||
            (iResultArray[iCurrentItem])->
                              ObjectClass().Find( KClassVideo ) == 0)
                {
                UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__SHOW);
                }
            else if( ( iResultArray[iCurrentItem] )->ObjectClass()
                                      .Find( KClassAudio ) == 0 )
                {
                UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__PLAY);
                }        
            else if( ( iResultArray[iCurrentItem] )->ObjectClass()
                                      .Find( KClassContainer ) == 0 )
                {
                UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_OPTIONS_BACK__OPEN);
                }    
            else
                {
                UpdateSoftkeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                }
            }
        }

        
        
    __LOG( "CUPnPBrowseDialog::LoadListItemsL End" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::SendBrowseRequestL
// Sends the browse request to UPnP AV Controller. When result set arrives,
// UPnP AV Controller will call the "BrowseResponse" call back method,
// which is implemented below.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::SendBrowseRequestL( TInt aIndex )
    {
    __LOG1( "CUPnPBrowseDialog::SendBrowseRequestL, index: %d", aIndex );
    iFirstResultArray = ETrue;
    
    iNeedRepeatRequest = EFalse;
    iOriginalBrowseRequest = ETrue;
    iServerReturnObjectCount = 0;
    
    iBrowseDirection = EForward;
    
    TInt cache = 0;
    iDummyBrowseResponse = EFalse;
    if( iBrowseSession )
        {
        // If the given index is negative, get the root ("0") container
        if( KBrowseRoot == aIndex ) 
            {
            iUppermostItem = 0;
            //Set browse flag for root browse
            iBrowseFlag = KBrowseRoot;

            // Clear parent id table
            iParentId.ResetAndDestroy();

            // Clear temporary array for browse view
            iTempArray.Reset();
            
            // Clear previous browse view
            
            iCurrentFolderId = KContainerIdRoot().AllocL();
           
            // Make the browse request
            cache = CacheCheck( *iCurrentFolderId );
            if ( cache > 0 )
                {
                //update current cache item
                CacheItemUpdateL( *iCurrentFolderId, EFalse );
                }
            else
                {
                //not founded in cache create new cache item
                CacheItemUpdateL( *iCurrentFolderId, ETrue );
                }            
                        
            iAction = CUPnPCommonUI::EUPnPBrowse;
            iBrowseSession->BrowseL( *iCurrentFolderId, 
                                     KFilterCommon,   /* filter */
                                     MUPnPAVBrowsingSession::EDirectChildren,
                                     0,               /* start index */
                                     KBrowseRequestCount,/* request count */
                                     KSortNone );     /* sort criteria */
            }
        else if( KBrowseBack == aIndex ) // Back operation
            {
            if ( iParentId.Count() > 0 )
                {
                // Set browse flag so that cleanup can be done in response
                iBrowseFlag = KBrowseBack;

                TInt startIndex =
                iPrevHighlighteditem - KBrowseRequestCount;

                if ( startIndex < 0 )
                    {
                    startIndex = 0;
                    }
                    
                TInt requestCount = KBrowseRequestCount * 2;
                CacheItemUpdateL( *iCurrentFolderId, EFalse );

                delete iCurrentFolderId; iCurrentFolderId = NULL;
                iCurrentFolderId = 
                                iParentId[ iParentId.Count() - 1 ]->AllocL();
                cache = CacheCheck( *iCurrentFolderId );
                
                if ( cache < 0 )
                    {
                    iUppermostItem = 0;
                    iBrowseRequestSent = ETrue;
                    iAction = CUPnPCommonUI::EUPnPBrowse;
                    iBrowseSession->BrowseL( 
                                     *iCurrentFolderId,
                                     KFilterCommon,  /* filter */
                                     MUPnPAVBrowsingSession::EDirectChildren,
                                     startIndex,     /* start index */
                                     requestCount,   /* request count */
                                     KSortNone );    /* sort criteria */
                    }
                else
                    {
                    iDummyBrowseResponse = ETrue;
                    }
                }
            }
        else
            {
            // If there is an object in the browse array with the given
            // index, get the container id of that object
            iPrevHighlighteditem = iListBox->CurrentItemIndex();
            if( iResultArray.Count() >= aIndex )
                {
                iBrowseFlag = KBrowseForward;

                // store selected item index
                iSelectedItem = aIndex;

                //update current cache item
                CacheItemUpdateL( *iCurrentFolderId, EFalse );
                delete iCurrentFolderId; iCurrentFolderId = NULL;
                iCurrentFolderId =
                               iResultArray[ aIndex ]->Id().AllocL();
               
                // compare if the array what is about to be requested
                // Make the browse request
                cache = CacheCheck( *iCurrentFolderId );
                if ( cache < 0 )
                    {
                    //not founded in cache create new cache item
                    CacheItemUpdateL( *iCurrentFolderId, ETrue );
                    iUppermostItem = 0;
                    iBrowseRequestSent = ETrue;
                    iAction = CUPnPCommonUI::EUPnPBrowse;
                    iBrowseSession->BrowseL(
                       ( ( CUpnpContainer*)iResultArray[aIndex])->Id(),
                    /* CUpnpContainer (containing the object id of
                            the container */
                           KFilterCommon,       /* filter */
                           MUPnPAVBrowsingSession::EDirectChildren,
                           0,       /* start index */
                           KBrowseRequestCount, /* request count */
                           KSortNone );         /* sort criteria */
                    }
                else
                    {
                    iDummyBrowseResponse = ETrue;
                    }                   


                }
            }
        if ( iDummyBrowseResponse )
            {
            SendDummyBrowseResponseL( cache );
            }
        else
            {
            // Display waiting note if browse request is sent
            DisplayWaitNoteL( R_UPNPCOMMONUI_BROWSE_UPDATE_WAIT_NOTE_DIALOG );
            }    
        }
    else
        {
        __LOG(  " CUPnPBrowseDialog::SendBrowseRequestL: \
                FAILED AV control point is NULL" );        

        }
        
    __LOG( "CUPnPBrowseDialog::SendBrowseRequestL End" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::BrowseResponse
// Returns browse results from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::BrowseResponse( const TDesC8& aBrowseResponse,
    TInt aError, TInt /*aMatches*/, TInt aTotalCount, const TDesC8&
    /*aUpdateId*/ )
    {
    __LOG1( "CUPnPBrowseDialog::BrowseResponse: %d", aError );
       
    RPointerArray<CUpnpObject> array;
    
    if( aError == KErrNone )
        {
        iRoot = EFalse;
        CUPnPXMLParser* parser = NULL;
        TRAP( aError, parser = CUPnPXMLParser::NewL();
              parser->ParseResultDataL( array,
              aBrowseResponse ) );
        
        delete parser;
        }
        
    TRAP_IGNORE( BrowseResponseL( aError, aTotalCount, array ) );
    
    array.ResetAndDestroy();
    
    __LOG( "CUPnPBrowseDialog::BrowseResponse -end" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::BrowseResponseL
// Returns browse results from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::BrowseResponseL(
                            TInt aStatus,
                            TInt aTotalCount,
                            const RPointerArray<CUpnpObject>& aResultArray )
    {    
    __LOG( "CUPnPBrowseDialog::BrowseResponseL" );
        
    CUPnPCommonUI::TUPnPAction currentAction = iAction;
    TInt currentCopyIndex = iCopyIndex;
    iAction = CUPnPCommonUI::EUPnPBrowse;
    TRAPD( error, HandleErrorL( aStatus ) );
    iAction = currentAction;
    iCopyIndex = currentCopyIndex;
    User::LeaveIfError( error );

    if( KErrNone == aStatus )
        {
        iTotalCount = aTotalCount;
        SelectedArrayCheckL();
        iAllObjectsReceived = EFalse;    
        iBrowseRequestSent = EFalse;
        if( iBrowseFlag != KBrowseRoot )
            {
            if ( iFirstResultArray )
                {
                if( iBrowseFlag == KBrowseBack ) //Back browse was made
                    {
                    delete iParentId[ iParentId.Count() - 1 ];
                    iParentId.Remove( iParentId.Count() - 1 );

                    // delete non-relevant parent name
                    delete iParentName[iParentName.Count() - 1];
                    iParentName.Remove( iParentName.Count() - 1 );
                    }
                else //Forward browse has been made
                    {
                    // Save Parent Id
                    if ( iResultArray.Count() > 0 )
                        {
                        HBufC8* containerId = 
                            ( iResultArray[ 0 ]->ParentId() ).AllocL();
                        iParentId.AppendL( containerId );

                        // store parent name
                        HBufC8* name = 
                         ( iResultArray[ iSelectedItem ]->Title() ).AllocL();
                        iParentName.AppendL( name );
                        }
                    }
                }
            }
        // If the result array in response to first browse request
        if ( iFirstResultArray )
           {
           DeleteListItemsL();
           iResultArray.Reset();
          
           // If Server don't return all of the objects requested in 
           // an first browse request So need repeat browse request
           // to get all objects which the first browse request desired
           TInt tempCount = iTotalCount;
           if ( tempCount > KBrowseRequestCount )
               {
               tempCount = KBrowseRequestCount;
               }           
           iServerReturnObjectCount = aResultArray.Count();
           if ( iServerReturnObjectCount < tempCount )
               {
               iNeedRepeatRequest = ETrue;
               }
           }
        
        CTextListBoxModel* model = iListBox->Model();
        
        // If response to an original browse request, so could get count of 
        // all objects which the original browse request desired 
        // PS: the first browse request is actually an original browse request
        if ( iOriginalBrowseRequest )
            {
            TInt tempCount = 0;
            if ( iBrowseDirection == EForward )
                {
                tempCount = iTotalCount - iUppermostItem  -
                            model->NumberOfItems();
                }
            else if ( iBrowseDirection == EBackward )
                {
                tempCount = iUppermostItem;
                }
            
            if ( tempCount > KBrowseRequestCount )
                {
                tempCount = KBrowseRequestCount;
                }
            iNeedRecevedObjectCount = tempCount;
            iCurrentRecevedObjectIndex = 0;
            iOriginalBrowseRequest = EFalse;
            }
        
        if( ( aResultArray.Count() + 
              iUppermostItem + 
              model->NumberOfItems() ) >= aTotalCount && 
              aResultArray.Count() == 0 )
            {
            iAllObjectsReceived = ETrue; 
            }
        else //if not all the objects are recevied
            {
            if ( aResultArray.Count() < KBrowseRequestCount )
                {
                iAllObjectsReceviedInOneBrowse = EFalse;
                iNumObjectReceviedInOnBrowse+=aResultArray.Count();
                if( iNumObjectReceviedInOnBrowse >= KBrowseRequestCount )
                    {
                    iAllObjectsReceviedInOneBrowse = ETrue;
                    }
                }
            }
        
        // Copy the items from the array received as a parameter
        for( TInt index=0; index < aResultArray.Count(); index++ )
            {
            
            if ( iBrowseDirection == EForward )
                {
                if ( ( aResultArray[index]->ObjectType() ==
                                         EUPnPContainer ) )
                    {
                    CUpnpContainer* container = CUpnpContainer::NewL();
                
                    CleanupStack::PushL( container );
                    container->CopyL( *aResultArray[ index ] );
                    CleanupStack::Pop( container );
                    iResultArray.AppendL( container );
                    }
                else 
                    {
                    CUpnpItem* item = CUpnpItem::NewL();
                
                    CleanupStack::PushL( item );
                    item->CopyL( *aResultArray[ index ] );
                    CleanupStack::Pop( item );
                    iResultArray.AppendL( item );
                    }                    
                }
            else if ( iBrowseDirection == EBackward )
                {
                if ( aResultArray[ index ]->ObjectType() == EUPnPContainer )
                    {
                    CUpnpContainer* container = CUpnpContainer::NewL();
                    CleanupStack::PushL( container );
                    container->CopyL( *aResultArray[ index ] );
                    CleanupStack::Pop( container );
                    iResultArray.InsertL( container, index );
                    iUppermostItem--;
                    }
                else
                    {
                    CUpnpItem* item = CUpnpItem::NewL();
                
                    CleanupStack::PushL( item );
                    item->CopyL( *aResultArray[ index ] );
                    CleanupStack::Pop( item );
                    iResultArray.InsertL( item, index );
                    iUppermostItem--;
                    }                    
                }
            }
        
            if ( iResultArray.Count() > ( KBrowseRequestCount * 2 ) )
                {                
                if ( iBrowseDirection == EForward )
                    {
                    for ( ;
                        ( KBrowseRequestCount * 2 ) < iResultArray.Count(); )
                        {
                        delete iResultArray[ 0 ];
                        iResultArray[ 0 ] = NULL;
                        iResultArray.Remove( 0 );
                        if ( iCurrentItem > 0 )
                            {
                            iCurrentItem--;    
                            }                        
                        iUppermostItem++;
                        }                
                    }
                else if ( iBrowseDirection == EBackward )
                    {
                    for ( ; 
                        ( KBrowseRequestCount * 2 ) < iResultArray.Count(); )
                        {                    
                        delete iResultArray[ ( iResultArray.Count() - 1 ) ];
                        iResultArray[ ( iResultArray.Count() - 1 ) ] = NULL;
                        iResultArray.Remove( iResultArray.Count() - 1 );
                        iCurrentItem++;
                        }
                    }
                }
            
        // Reload the list items
        if ( iFirstResultArray )
           {
           if ( !iDummyBrowseResponse )
               {
               iCurrentItem = 0;
               }               
           iFirstResultArray = EFalse;
           // update navi pane text
           UpdateNaviPaneTextL();
           // Redraw the list
           iListBox->ActivateL();
           iListBox->DrawDeferred();
           }
        DeleteListItemsL();
        LoadListItemsL();
        
        // If count of CDS objects requesed in an original browse request
        // isn't complete, countinue send browse request
        iCurrentRecevedObjectIndex += aResultArray.Count();
        if ( iNeedRepeatRequest && 
             iCurrentRecevedObjectIndex < iNeedRecevedObjectCount )
            {
            TInt startIndex = KErrNotFound;
            if ( iBrowseDirection == EForward )
                {
                TInt numberOfItems = iListBox->Model()->NumberOfItems();
                startIndex = iUppermostItem + numberOfItems;
                }
            else if ( iBrowseDirection == EBackward )
                {
                startIndex = iUppermostItem - iServerReturnObjectCount;
                }
            iAction = CUPnPCommonUI::EUPnPBrowse;         
            iBrowseSession->BrowseL( *iCurrentFolderId, 
                                 KFilterCommon,       /* filter */
                                 MUPnPAVBrowsingSession::EDirectChildren,
                                 startIndex,          /* start index */
                                 iServerReturnObjectCount,/* request count */
                                 KSortNone );         /* sort criteria */
            iBrowseRequestSent = ETrue;
            }
        else
            {
            DismissWaitNoteL();               
            }
        }
    else
        {
        __LOG( "CUPnPBrowseDialog: \
            Browse operation failed!" );
        DismissWaitNoteL();
        //Delete the current browsed container id in the cache and
        // handle the error
        DialogDismissedL( KErrNotFound );
        }
    
    
    __LOG( "CUPnPBrowseDialog::BrowseResponseL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ClearTitleL
// Sets title back to default.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ClearTitleL()
    {
    __LOG( "tCUPnPBrowseDialog::ClearTitleL" );
    
    TUid titlePaneUid;
    titlePaneUid.iUid = EEikStatusPaneUidTitle;
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CEikStatusPaneBase::TPaneCapabilities titlesubPane =
        statusPane->PaneCapabilities( titlePaneUid );

    CAknTitlePane* titlePane =
        ( CAknTitlePane* ) statusPane->ControlL( titlePaneUid );
    __LOG( "CUPnPBrowseDialog::ClearTitleL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an 
// icon array.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::AppendIconToArrayL( CAknIconArray* aArray,
                                            MAknsSkinInstance* aSkin,
                                            const TDesC& aMbmFile,
                                            const TAknsItemID& aID,
                                            TInt aBitmapId,
                                            TInt aMaskId)
    {
    __LOG(  "CUPnPBrowseDialog::AppendIconToArrayL" );

    __ASSERTD( aArray != NULL, __FILE__, __LINE__ );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;


    AknsUtils::CreateIconL(
                aSkin, aID, bitmap, mask, aMbmFile, aBitmapId, aMaskId );
    
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    
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
    __LOG( "CUPnPBrowseDialog::AppendIconToArrayL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::MarkItemL
// Marks one item
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::MarkItemL( TInt aItemIndex )
    {
    __LOG( "CUPnPBrowseDialog::MarkItemL" );

    CTextListBoxModel* model = iListBox->Model();
    CListBoxView* listBoxView = iListBox->View();

    if( iResultArray[ aItemIndex ] )
        {
        if( iResultArray[ aItemIndex ]->ObjectType() == EUPnPItem )
            {
            listBoxView->SelectItemL( aItemIndex );       
            }           
        }
    UpdateCommandButtonAreaL( ETrue, iListBox->CurrentItemIndex() );
    __LOG( "CUPnPBrowseDialog::MarkItemL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::UnmarkItemL
// Unmarks one item
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::UnmarkItem( TInt aItemIndex )
    {
    __LOG( "CUPnPBrowseDialog::UnmarkItemL" );

    CTextListBoxModel* model = iListBox->Model();
    CListBoxView* listBoxView = iListBox->View();
    listBoxView->DeselectItem( aItemIndex );    
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    if ( indexes->Count() )
        {
        UpdateCommandButtonArea( ETrue, iListBox->CurrentItemIndex() );
        }
    else
        {
        UpdateCommandButtonArea( EFalse, iListBox->CurrentItemIndex() );
        }
    __LOG( "CUPnPBrowseDialog::UnmarkItemL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::MarkAllItemsL
// Marks all items. Does not mark any containers.
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::MarkAllItemsL()
    {
    __LOG( "CUPnPBrowseDialog::MarkAllItemsL" );

    CTextListBoxModel* model = iListBox->Model();
    TInt itemCount = model->NumberOfItems();
    CListBoxView* listBoxView = iListBox->View();

    // Go through the items and select all items
    for( TInt index = 0; index < itemCount; index++ )
        {
        if( iResultArray[ index ]->ObjectType() == EUPnPItem )
            {
            listBoxView->SelectItemL( index );
            }
        }
    UpdateCommandButtonAreaL( ETrue, iListBox->CurrentItemIndex() );
    __LOG( "CUPnPBrowseDialog::MarkAllItemsL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::UnmarkAllItems
// Unmarks all items
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::UnmarkAllItems()
    {
    __LOG( "CUPnPBrowseDialog::UnmarkAllItems" );

    CTextListBoxModel* model = iListBox->Model();
    TInt itemCount = model->NumberOfItems();
    CListBoxView* listBoxView = iListBox->View();
    iSelectedItemsArray.ResetAndDestroy();
    for( TInt index = 0; index < itemCount; index++ )
        {
        listBoxView->DeselectItem( index );
        }
    UpdateCommandButtonArea( EFalse, iListBox->CurrentItemIndex() );
    __LOG( "CUPnPBrowseDialog::UnmarkAllItems-END" );
    }
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ShowPlayExtL
// show image or video on external device
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ShowPlayExtL()
    {
    __LOG( "CUPnPBrowseDialog::ShowPlayExtL" );
    
    SelectedArrayCheckL();
    //if there is(are) audio item(s) marked, start playing it(them) on remote
    //renderer    
    if ( iSelectedItemsArray.Count() > 0 )
        {
        if( IsAudioItemMarked() )
            {
            //if marked items have at least one audio item
            iIsMusicItem = ETrue;
            }
        else
            {
            iIsMusicItem = EFalse;
            }                    
        //if at least one audio file is either in marked items
        //play it
        if( iIsMusicItem && !iMusicPlay )
            {
            PlayL( EFalse );
            }
        }
    else // if no marked items
        {
        if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                          ObjectClass().Find( KClassVideo ) == 0 )
            {
            ShowVideoDialogExtL();
            }
        else if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                              ObjectClass().Find( KClassImage ) == 0 )
            {
            StartImageControlL();
            }
        else if ( ( iResultArray[iListBox->CurrentItemIndex()] )->
                          ObjectClass().Find( KClassAudio ) == 0 &&
                          !iMusicPlay )    
            {
            PlayL( EFalse );
            }
        }
    
    __LOG( "CUPnPBrowseDialog::ShowPlayExtL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::CreateFillerLC
// Creates a playlist representing current selection for music playback
// --------------------------------------------------------------------------

CUPnPPlayListFiller* CUPnPBrowseDialog::CreateFillerLC()
    {
    CUPnPPlayListFiller* filler = NULL;

    if ( iSelectedItemsArray.Count() > 0 )
        {
        // Play marked items only
        filler = CUPnPPlayListFiller::NewL();
        CleanupStack::PushL( filler );
        TInt firstAudioItem = KErrNotFound;
        TBool focusedItemFound = EFalse;
        for( TInt i = 0; i < iSelectedItemsArray.Count(); i++ )
            {
            filler->InsertObjectL(
                iSourceDevice, *iSelectedItemsArray[ i ] );

            if( UPnPItemUtility::BelongsToClass( *iSelectedItemsArray[ i ]
                                                              , KClassAudio )
                && firstAudioItem == KErrNotFound )
                {
                firstAudioItem = i;
                }
            if( iSelectedItemsArray[i]->Id() == iResultArray[ iListBox->
                                                 CurrentItemIndex() ]->Id() )
                {
                filler->SetSelectedIndex( i );
                focusedItemFound = ETrue;
                }
            }
        // if focused item was not found set index to first audio item
        if( !focusedItemFound )
            {   
            filler->SetSelectedIndex( firstAudioItem );
            }
        }
    else
        {
        // play entire container starting from current pointer
        TInt currentIndex = iListBox->CurrentItemIndex();
        if( currentIndex < 0 )
            {
            currentIndex = 0;
            }

        if( iParentId.Count() == 0 )
            {
            //root level
            filler = CUPnPBrowsePlaylistFiller::NewL(
                KContainerIdRoot,
                iUppermostItem,
                iTotalCount );
            }
        else if( iParentId.Count() > 0)
            {
            //any other level
            filler = CUPnPBrowsePlaylistFiller::NewL(
                iResultArray[ 0 ]->ParentId(),
                iUppermostItem,
                iTotalCount );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        CleanupStack::PushL( filler );
        for( TInt i = 0; i < iResultArray.Count(); i++ )
            {
            filler->InsertObjectL( iSourceDevice, *iResultArray[ i ] );
            }
        filler->SetSelectedIndex( currentIndex );
        }

    return filler;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::PlayL
// Plays selected music file
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::PlayL( TBool aLocal )
    {
    __LOG( "CUPnPBrowseDialog::PlayL" );
    TInt error = KErrNone;    
    if( !aLocal )
        {
        if( !iImageControlActive )  //if not played via image control
            {                        //start device dialog
            iChildDialogOpen++;
            error = iCommonUI.SelectDeviceL(
                             iAVControl,
                             *iTargetDevice,
                             EUPnPSearchRenderingDevicesWithAudioCapability,
                             EUPnPSelectDeviceTitle );
            iChildDialogOpen--;                 
            }
        if ( KErrNone == error )
            {
            CUPnPPlayListFiller* filler = CreateFillerLC();
            CleanupStack::Pop( filler ); 
            iChildDialogOpen++;
            iMusicPlay = ETrue;
           //filler ownership transferred
            error = iCommonUI.ExecuteMusicPlayerL(
                iAVControl, filler, iTargetDevice );
            
            iMusicPlay = EFalse;
            iChildDialogOpen--;
            iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL(
                                    R_AVKON_STATUS_PANE_LAYOUT_USUAL);
            iAvkonEnv->LoadAknLayoutL();
            iAvkonAppUi->ReportResourceChangedToAppL(
                                    KEikDynamicLayoutVariantSwitch );
            }
        else
            {
            __LOG( "CUPnPBrowseDialog::PlayL SelectDeviceL failed" );        
            }    
        }
    else
        {
        CUPnPPlayListFiller* filler = CreateFillerLC();
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
    __LOG( "CUPnPBrowseDialog::PlayL-END" );
    }    

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DisplayWaitNoteL
// 
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::DisplayWaitNoteL( TInt aResource )
    {
    __LOG( "CUPnPBrowseDialog::DisplayWaitNoteL" );
    
    // Wait dialog is terminated from Ready function
    iWaitNoteDialog = new( ELeave )CAknWaitDialog(
        ( REINTERPRET_CAST ( CEikDialog**,&iWaitNoteDialog ) ), ETrue );
    iWaitNoteDialog->SetCallback( this );
    iWaitNoteDialog->ExecuteLD( aResource );
    
    __LOG( "CUPnPBrowseDialog::DisplayWaitNoteL-END" );
    }    
    
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DismissWaitNoteL
// 
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::DismissWaitNoteL()
    {
    __LOG( "CUPnPBrowseDialog::DismissWaitNoteL" );
    
    if ( iWaitNoteDialog )
        {
        iWaitNoteDialog->ProcessFinishedL();
        delete iWaitNoteDialog;
        iWaitNoteDialog = NULL;
        }

    __LOG( "CUPnPBrowseDialog::DismissWaitNoteL-END" );
    }  


// --------------------------------------------------------------------------
// CUPnPBrowseDialog::GetHelpContext
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    __LOG( "CUPnPBrowseDialog::GetHelpContext" );
    
//    aContext.iMajor = TUid::Uid( KMediaGalleryUID3 );

    aContext.iContext = KUPNP_HLP_REMOTE_DEVICES;

    TInt count = iResultArray.Count();
    for ( TInt index = 0; index < count; index++ )
        {
        if ( iResultArray[ index ]->ObjectType() != EUPnPContainer )
            {
            // show content help if folder contains items
            aContext.iContext = KUPNP_HLP_CONTENT_VIEW;
            // end loop
            index = count;
            }
        }
    __LOG( "CUPnPBrowseDialog::GetHelpContext-END" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::MediaServerDisappeared
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::MediaServerDisappeared( 
                                TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "CUPnPBrowseDlg::MediaServerDisappeared" );

    TInt error = KErrNone;
    if( aReason == EDisconnected )
        {
        error = KErrSessionClosed;
        iMSDisappear = ETrue;
        }
    else if( aReason == EWLANLost )
        {
        error = KErrDisconnected;
        }
    else
        {
        __PANICD( __FILE__, __LINE__ );
        }
    
    TRAPD( err, DismissWaitNoteL() );
    if ( err != KErrNone )
        {
        __LOG( "CUPnPBrowseDlg::MediaServerDisappeared \
                                                     delete WaitNote error" );
        }
    
    //Only HandleCopyL will set iAction to CUPnPCommonUI::EUPnPCopy, after
    //HandleCopyL be called, HandleErrorL also will be called. So at here 
    //HandleErrorL shouldn't be called. Otherwise will result ESLX-7M88UF
    if ( iAction != CUPnPCommonUI::EUPnPCopy )
        {
        TRAP_IGNORE( HandleErrorL( error ) );
        }
    
    __LOG( "CUPnPBrowseDlg::MediaServerDisappeared - END" );
    }

// --------------------------------------------------------------------------
// CUPnPExternalDeviceDialog::UpdateNaviPaneTextL
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::UpdateNaviPaneTextL()
    {
    __LOG( "CUPnPBrowseDialog::UpdateNaviPaneTextL" );

    // Set text to navi pane
    CEikStatusPane* sp = 
        ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
    iNaviPane = ( CAknNavigationControlContainer * )sp->ControlL(
        TUid::Uid( EEikStatusPaneUidNavi ) );

    // old decorator is popped and deleted
    if ( iNaviDecorator )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    
    if ( iImageControlActive )    
        {
        if( iTitlePane )
            {
            iTitlePane->SetTextL( *StringLoader::LoadLC(
                                R_UPNPCOMMONUI_TITLE_SHOWING ) );
            CleanupStack::PopAndDestroy();   
            }
        
        // Get device friendly name and replace illegal characters.
        HBufC8* tmpfriendlyname = 
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            iTargetDevice->FriendlyName() );
            
        CleanupStack::PushL( tmpfriendlyname );
        TPtrC8 friendlyname = *tmpfriendlyname;
        
        HBufC *tmpbuf = UpnpString::ToUnicodeL( 
            friendlyname.Left( KLength ) );
        CleanupStack::PushL( tmpbuf );   

        iNaviDecorator = iNaviPane->CreateNavigationLabelL( *tmpbuf );
        CleanupStack::PopAndDestroy( tmpbuf );
        CleanupStack::PopAndDestroy( tmpfriendlyname );
        iNaviPane->PushL( *iNaviDecorator );         
        }
    // parent name is given    
    else if ( iParentName.Count() > 0 )
        {
        //set back the title
        if( iTitlePane )
            {
            // Get device friendly name and replace illegal characters.
            HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
                iSourceDevice.FriendlyName() );
            CleanupStack::PushL( tmpfriendlyname );
            
            TPtrC8 friendlyname = *tmpfriendlyname;
            HBufC *tmpbuf = UpnpString::ToUnicodeL( 
                friendlyname.Left( KLength ) );
            CleanupStack::PushL( tmpbuf ); 
            
            iTitlePane->SetTextL( *tmpbuf );
            CleanupStack::PopAndDestroy( tmpbuf );
            CleanupStack::PopAndDestroy( tmpfriendlyname );
            }
        
        // Get name and replace illegal characters.
        HBufC8* tmpname = 
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            *iParentName[ iParentName.Count() - 1 ] );
            
        CleanupStack::PushL( tmpname );
        TPtrC8 tmpnameptr = *tmpname;
        
        HBufC* buf = UpnpString::ToUnicodeL( tmpnameptr.Left( KLength )  );
        CleanupStack::PushL(buf);
        // ownership of decorator is transfered to application
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( *buf );
        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( tmpname );
        iNaviPane->PushL( *iNaviDecorator );
        }
    __LOG( "CUPnPBrowseDialog::UpdateNaviPaneTextL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::CacheCheck
// Checks if the wanted container is in Cache
// --------------------------------------------------------------------------

TInt CUPnPBrowseDialog::CacheCheck( const TDesC8& aCheckContainerId )
    {    
    __LOG( "CUPnPBrowseDialog::CacheCheck" );
    
    TInt retContainerCacheArrayId = -1;
    for ( TInt index = 0; index < iBrowseCacheItems.Count(); index++ )
        {
        if ( aCheckContainerId.Compare( 
                        iBrowseCacheItems[ index ]->ContainerId() ) == 0 )
            {
            // item is in cache
            retContainerCacheArrayId = index;
            break;
            }
        }
    
    __LOG( "CUPnPBrowseDialog::CacheCheck-END" );    
    return retContainerCacheArrayId;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::SendDummyBrowseResponseL
// sends dummy response as browseresponse from cache
// --------------------------------------------------------------------------

void CUPnPBrowseDialog::SendDummyBrowseResponseL( TInt aCacheIndex )
    {
    __LOG( "CUPnPBrowseDialog::SendDummyBrowseResponseL" );
    iTempArray.Reset();
    
    TInt numberOfItems = iBrowseCacheItems[ aCacheIndex ]->GetNumberOfItems();
    for ( TInt index = 0; index < numberOfItems; index++ )
        {
        iTempArray.Appendl( iBrowseCacheItems[ aCacheIndex ]->
                                                          GetItem( index ) );
        }
    iUppermostItem = iBrowseCacheItems[ aCacheIndex ]->GetUpperMostItem();
    iTotalCount = iBrowseCacheItems[ aCacheIndex ]->GetTotalCount();
    iCurrentItem = iBrowseCacheItems[ aCacheIndex ]->GetHighlightedItem();
    BrowseResponseL( KErrNone, iTotalCount, iTempArray );
    __LOG( "CUPnPBrowseDialog::SendDummyBrowseResponseL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::CacheItemUpdateL
// updates containers cache
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::CacheItemUpdateL( const TDesC8& aContainerId, 
                                          TBool aNewItem )
    {
    __LOG( "CUPnPBrowseDialog::CacheItemUpdateL" );
    if ( aNewItem )
        {
        __LOG( "CUPnPBrowseDialog::CacheItemUpdateL, NewItem" );
        CUPnPBrowseCacheItem* test = CUPnPBrowseCacheItem::NewL( 
                                                        aContainerId,
                                                        iPrevHighlighteditem,
                                                        iUppermostItem );
        CleanupStack::PushL( test );
        iBrowseCacheItems.AppendL( test );
        CleanupStack::Pop( test );
        }
    else //update current item
        {
        TInt cacheItemIndex = CacheCheck( aContainerId );
        if ( cacheItemIndex > -1 )
            {
            iBrowseCacheItems[ cacheItemIndex ]->SetFirstItem( 
                                                iUppermostItem );
            iBrowseCacheItems[ cacheItemIndex ]->SetHighLightedItem(
                                                iCurrentItem );
            iBrowseCacheItems[ cacheItemIndex ]->SetTotalCount(
                                                iTotalCount );
            //iResultArray ownership transferred here
            iBrowseCacheItems[cacheItemIndex]->SetItemArrayL( iResultArray );
            }
        }
    __LOG( "CUPnPBrowseDialog::CacheItemUpdateL-END" );    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::SelectedArrayCheckL
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::SelectedArrayCheckL( void )
    {
    __LOG( "CUPnPBrowseDialog::SelectedArrayCheckL" );
    const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
    //add possible new selections
    if ( indexes->Count() )
        {
        for ( TInt selectionCount = 0; selectionCount < indexes->Count();
                                                       selectionCount++ )
            {
            TBool alreadyInArray = EFalse;            
            for ( TInt i = 0; i < iSelectedItemsArray.Count(); i++ )
                {
                //checks if the selected BrowseArray item is
                //in the selected items array
                if ( !( iSelectedItemsArray[ i ]->Id().CompareC( ( 
                            iResultArray[ indexes->At( selectionCount ) ]
                            )->Id() ) ) )
                                    
                    {                    
                    alreadyInArray = ETrue;
                    i = iSelectedItemsArray.Count();
                    }
                }
            //if not in array add it there
            if ( !alreadyInArray )
                {
                CUpnpItem* itemForSelectionArray = CUpnpItem::NewL();
                CleanupStack::PushL( itemForSelectionArray );
                
                itemForSelectionArray->CopyL( *iResultArray[ indexes->At(
                                                     selectionCount ) ] );

                iSelectedItemsArray.AppendL( itemForSelectionArray );
                CleanupStack::Pop( itemForSelectionArray );
                }
            }
        }
    CTextListBoxModel* model = iListBox->Model();
    TInt numberOfItems = model->NumberOfItems();
    
    //delete unselectedItems from selected items array    
    if ( numberOfItems )
        {
        const CArrayFix<TInt>* indexes = iListBox->SelectionIndexes();
        
        //check all items in the current dialog
        for ( TInt listItemIndex = 0; listItemIndex < numberOfItems;
                                                  listItemIndex++ )
            {
            TBool notSelected = ETrue;
            
            //check all selected items
            for ( TInt index = 0; index < indexes->Count() ; index++ )
                {
                if ( listItemIndex == indexes->At( index ) )
                    {
                    notSelected = EFalse;
                    index = indexes->Count();
                    }
                }
            //if not selected, check if the item is in selected items array
            if ( notSelected )
                {
                //check selected items array   
                for ( TInt selectionArray = 0;
                      selectionArray < iSelectedItemsArray.Count() ;
                      selectionArray++ )
                     
                    {                    
                    if ( !( iSelectedItemsArray[ selectionArray ]->
                                                               Id().CompareC(
                       ( ( CUpnpItem* )iResultArray[ listItemIndex ] )
                                                                ->Id() ) ) )
                        {
                        delete iSelectedItemsArray[ selectionArray ];
                        iSelectedItemsArray.Remove( selectionArray );
                        iSelectedItemsArray.Compress();
                        selectionArray = iSelectedItemsArray.Count();
                        }
                    }
                }
            }
        }
    __LOG( "CUPnPBrowseDialog::SelectedArrayCheckL-END" );
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::StartImageControlL
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::StartImageControlL()
    {
    __LOG( "CUPnPBrowseDialog::StartImageControlL" );
    TInt error = KErrNone;
    
    iChildDialogOpen++;
    error = iCommonUI.SelectDeviceL(
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
        
        TRAP( error, iImagePlayer->SetTargetDeviceL( *iTargetDevice ) );
        
        if( KErrNone == error )
            {
            StartImageControlTimer();
            iImageControlActive = ETrue;
            UpdateNaviPaneTextL();
            UpdateSoftkeysL( R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
            }
        else
            {
            __LOG( "CUPnPBrowseDialog::StartImageControlL SetTargetDeviceL \
didn't selected" );
            }
        }
    else
        {
        __LOG( "CUPnPBrowseDialog::StartImageControlL SelectDevice \
didn't selected" );
        }
    if( KErrNone != error )
        {
        User::Leave( error );    
        }
    
    __LOG( "CUPnPBrowseDialog::StartImageControlL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::StopImageControlL
// --------------------------------------------------------------------------
        
void CUPnPBrowseDialog::StopImageControlL()
    {
    __LOG( "CUPnPBrowseDialog::StopImageControlL" );
    if ( iImageControlActive )
        {
        iImageControlActive = EFalse;
        iImageControlTimer->Cancel();
        ShowCurrentImageL( EFalse );
        UpdateNaviPaneTextL();
        TInt currentItemIndex = iListBox->CurrentItemIndex();
        if ( currentItemIndex < 0 )
            {
            currentItemIndex = 0;
            }
        UpdateCommandButtonAreaL( EFalse, currentItemIndex );    
        iAction = CUPnPCommonUI::EUPnPNone;
        iImagePlayer->Stop();
        }
    
    __LOG( "CUPnPBrowseDialog::StopImageControlL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::UpdateSoftkeysL
// Update CBA-keys
// --------------------------------------------------------------------------

void CUPnPBrowseDialog::UpdateSoftkeysL( TInt aResourceId )
    {   
    __LOG( "CUPnPBrowseDialog::UpdateSoftkeysL" );
    CEikButtonGroupContainer* cba = &ButtonGroupContainer();
    cba->SetCommandSetL( aResourceId );
    cba->DrawDeferred();
    __LOG( "CUPnPBrowseDialog::UpdateSoftkeysL-END" );
    }
    
// --------------------------------------------------------------------------
// UPnPImagePlayerDialog::ImageControlTimerCallbackL
// Callback method for the Timer.
// --------------------------------------------------------------------------
TInt CUPnPBrowseDialog::ImageControlTimerCallbackL( TAny* aDlg )
    {
    __LOG( "CUPnPBrowseDialog::ImageControlTimerCallbackL" );
    static_cast< CUPnPBrowseDialog* >( aDlg )->iImageControlTimer->Cancel();
    if ( !( static_cast< CUPnPBrowseDialog* >( aDlg )->iBrowseRequestSent ) )
        {
        static_cast< CUPnPBrowseDialog* >( aDlg )->ShowCurrentImageL( ETrue );
        }
    else
        {
        static_cast<CUPnPBrowseDialog*>(aDlg)->StartImageControlTimer();
        }    
    __LOG( "CUPnPBrowseDialog::ImageControlTimerCallbackL-END" );
    return KErrNone;
}
    

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::StartImageControlTimer
// Starts periodic timer
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::StartImageControlTimer( void )
    {
    __LOG( "CUPnPBrowseDialog::StartImageControlTimer" );
    iImageControlTimer->Start( 
                        KImageTimerDelay,
                        KImageTimerInterval,
                        TCallBack( ImageControlTimerCallbackL, this ) );
    __LOG( "CUPnPBrowseDialog::StartImageControlTimer-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ShowCurrentImageL
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ShowCurrentImageL( TBool aShowImage )
    {
    __LOG( "CUPnPBrowseDialog::ShowCurrentImageL" );
    
    TInt currentItemIndex = iListBox->CurrentItemIndex();
    if ( currentItemIndex < 0 )
        {
        currentItemIndex = 0;
        }
    
    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast< CDesCArray* >( textArray );
    
    if( iLastImageItemIndex >= 0 ) //if last item was image
        {
        TBuf<KMaxFileName> lastItem;
        HBufC *lastTmpItem = UpnpString::ToUnicodeL( 
            iResultArray[ iLastImageItemIndex ]->Title().Left( KLength ) );
        CleanupStack::PushL( lastTmpItem );
        lastItem.Format( KFormatString(), 
                         EUPnPIconImage, 
                         lastTmpItem);
        CleanupStack::PopAndDestroy ( lastTmpItem );
                 
        listBoxItems->Delete( iLastImageItemIndex );
        listBoxItems->InsertL( iLastImageItemIndex, lastItem );
        iListBox->HandleItemAdditionL();
        if( ( iResultArray[ iListBox->CurrentItemIndex() ] )->
                          ObjectClass().Find( KClassImage ) != 0 )
            {
            iLastImageItemIndex = KErrNotFound;    
            }
        }    

    if ( ( iResultArray[ iListBox->CurrentItemIndex() ] )->
                          ObjectClass().Find( KClassImage ) == 0 )
        {
        if( ( CUpnpItem* )iResultArray[ currentItemIndex ]
                                                ->Title().Length() != 0 )
            {
            HBufC *tmpItem = UpnpString::ToUnicodeL( 
                    iResultArray[ currentItemIndex ]->
                                    Title().Left( KLength ) );
            CleanupStack::PushL( tmpItem );
            
            TBuf<KMaxFileName> item;
            //if not showing an image
            if ( !aShowImage )
                {   
                item.Format( KFormatString(), EUPnPIconImage, tmpItem);
                iLastImageItemIndex = KErrNotFound;
                listBoxItems->Delete( currentItemIndex );
                listBoxItems->InsertL( currentItemIndex, item );
                }
            else //if showing an image
                {
                
                item.Format( KFormatString2() ,EUPnPIconImage,
                                               tmpItem,
                                               EUPnPIconImageShowing );
                if(iImagePlayer)
                    {
                    __LOG( "CUPnPBrowseDialog::iImagePlayer->PlayL" );
                    iAction = CUPnPCommonUI::EUPnPShow;
                    TRAPD( error, iImagePlayer->PlayL( 
                                  *iResultArray[currentItemIndex] ) );
                    
                    HandleErrorL( error );
                    
                    }     

                //listBoxItem.Set( item );
                listBoxItems->Delete( currentItemIndex );
                listBoxItems->InsertL( currentItemIndex, item );
                iLastImageItemIndex = currentItemIndex;
                }
            CleanupStack::PopAndDestroy ( tmpItem );
            iListBox->HandleItemAdditionL();
            iListBox->SetCurrentItemIndexAndDraw( currentItemIndex );
            }
        }
    
    __LOG( "CUPnPBrowseDialog::ShowCurrentImageL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::ShowVideoDialogExt
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::ShowVideoDialogExtL()
    {
    __LOG( "CUPnPBrowseDialog::ShowVideoDialogExtL" );
    
    TInt error = KErrNone;
    
    const CUpnpObject* selectedObject = 
                                iResultArray[ iListBox->CurrentItemIndex() ];
    
    if( !iImageControlActive ) //if showing video is not from image timer
        {                        //then start select device dialog
        iChildDialogOpen++;
        error = iCommonUI.SelectDeviceL(
                     iAVControl,
                     *iTargetDevice,
                     EUPnPSearchRenderingDevicesWithVideoCapability,
                     EUPnPSelectDeviceTitle );
        iChildDialogOpen--;
        }
    else //if target renderer exists, check video capability
        {
        if( !iTargetDevice->VideoCapability() )
            {
            User::Leave( KErrNotSupported );
            }
        }
    if( KErrNone == error )
        {
        iAction = CUPnPCommonUI::EUPnPShow;    
        MUPnPAVRenderingSession* renderingSession = NULL;    
        renderingSession = &( iAVControl.StartRenderingSessionL( 
                                                  *iTargetDevice ) );
        
        iChildDialogOpen++;
        //trap here to release rendering session properly, better to 
        //create and destroy the rendering ession inside the video player
        TInt ret = KErrNone;
        TRAP( error, ret = iCommonUI.ExecuteVideoPlayerL( 
                                               *renderingSession, 
                                               *selectedObject ) );
        
        if( ret < KErrNone && error == KErrNone )
            {
            error = ret;
            }
        renderingSession->RemoveObserver();
        iAVControl.StopRenderingSession( *renderingSession );
            
        iChildDialogOpen--;
        
        //if not media server or wlan lost, update its navipane
        if( !iMSDisappear && error != KErrDisconnected )
            {
            UpdateNaviPaneTextL();    
            }
        }
     else
        {
        __LOG( "ShowVideoDialogExtL: SetTargetDeviceL FAILED" );
        }
     
     if( KErrNone != error )
        {
        User::Leave( error );
        }
     
    __LOG( "CUPnPBrowseDialog::ShowVideoDialogExtL-END" );   
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::HandleCopyL()
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::HandleCopyL() 
    {
    __LOG( "CUPnPBrowseDialog::HandleCopyL" );   
   
    iAction = CUPnPCommonUI::EUPnPCopy;
    iCopyIndex = NULL;
    TInt selected_item = iListBox->CurrentItemIndex();
    iCopying = ETrue;
    if( iResultArray[ selected_item ]->ObjectType() == EUPnPContainer )
        {
        // Instantiate the UPnP File Transfer Engine
        CUpnpFileTransferEngine* ftEngine = 
                    CUpnpFileTransferEngine::NewL( iBrowseSession );
        CleanupStack::PushL( ftEngine );
        
        TBool playlistContainer = EFalse;
        playlistContainer = UPnPCommonUtils::IsPlaylistContainerL( 
                            *iResultArray[ iListBox->CurrentItemIndex() ] );
        
        // Copy container to phone memory
        TInt copyErr = KErrNone;
        if( playlistContainer )
            {
            TRAP( copyErr, ftEngine->CopyRemotePlaylistToHandsetL(
                             ( CUpnpContainer* )iResultArray[
                             iListBox->CurrentItemIndex() ] ) );
            }
        else
            {
            TRAP( copyErr, ftEngine->CopyRemoteContainerToHandsetL(
                                         ( CUpnpContainer* )iResultArray[
                                         iListBox->CurrentItemIndex() ] ) );
            }
            
        iCopyIndex = ftEngine->ItemCopiedFromContainer();
        
        User::LeaveIfError( copyErr );
        
        // Clean up ftEngine
        CleanupStack::PopAndDestroy( ftEngine );
        ftEngine = NULL;    
        }
    else
        {
        RPointerArray<CUpnpItem> tempArrayForCopy;
        CleanupResetAndDestroyPushL( tempArrayForCopy );
        if ( iSelectedItemsArray.Count() )
            {
            for ( TInt count=0; 
                  count < iSelectedItemsArray.Count(); 
                  count++ )
                {
                CUpnpItem* tempItem = CUpnpItem::NewL();
                CleanupStack::PushL( tempItem );
                
                
                tempItem->CopyL( *iSelectedItemsArray[ count ] );
                
                //ownership transferred
                tempArrayForCopy.AppendL( tempItem );
                CleanupStack::Pop( tempItem );
                }
            iCopyIndex = iSelectedItemsArray.Count();    
            }
        else
            {
            CUpnpItem* tempItem = CUpnpItem::NewL();
            CleanupStack::PushL( tempItem );
            tempItem->CopyL( *iResultArray[selected_item] );
            //ownership transferred
            tempArrayForCopy.AppendL( tempItem );
            CleanupStack::Pop( tempItem );
            iCopyIndex = 1;
            }
        // Instantiate the UPnP File Transfer Engine
        CUpnpFileTransferEngine* ftEngine = NULL;
        ftEngine = CUpnpFileTransferEngine::NewL( iBrowseSession );
        CleanupStack::PushL( ftEngine );
         // Copy to phone
        ftEngine->CopyRemoteItemsToHandsetL( tempArrayForCopy );
        // Clean up ftEngine
        CleanupStack::PopAndDestroy( ftEngine );
        ftEngine = NULL;
        CleanupStack::PopAndDestroy( &tempArrayForCopy );
        }
     iCopying = EFalse;
     UnmarkAllItems(); 

     __LOG( "CUPnPBrowseDialog::HandleCopyL-END" );  
    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DeviceDisappeared( TInt aError )
// called in image play
//---------------------------------------------------------------------------
void CUPnPBrowseDialog::DeviceDisappeared( TInt aError )
    {
    TRAP_IGNORE( HandleErrorL( aError ) );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::HandleErrorL( TInt aError )
// Handle errors in all the case EGKL-6ZPH89
//---------------------------------------------------------------------------
void CUPnPBrowseDialog::HandleErrorL( TInt aError )
    {
    iCommonUI.GetUpnpAction( iAction );
    //if media server or WLAN lost, close the browse dialog    
    if( ( KErrSessionClosed == aError && iMSDisappear ) ||
          KErrDisconnected == aError ||
          EAknCmdExit == aError ||
          EEikCmdExit == aError )
        {
        if( iError == KErrNone )
            {
            iError = aError;
            }
        
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
            UpdateNaviPaneTextL();    
            }
        
        iCommonUI.HandleCommonErrorL( aError, iCopyIndex );
        iAction = CUPnPCommonUI::EUPnPNone;
        iCopyIndex = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::HandleResourceChange
// --------------------------------------------------------------------------
//    
void CUPnPBrowseDialog::HandleResourceChange(TInt aType)
    {
    __LOG("[UPnPCommonUI]\t CUPnPBrowseDialog::HandleResourceChange");
    CAknDialog::HandleResourceChange(aType);
    }    

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::IsAudioItemMarked
// --------------------------------------------------------------------------
TBool CUPnPBrowseDialog::IsAudioItemMarked( void )
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
    __LOG( "CUPnPBrowseDialog::IsAudioItemMarked-END" );      
    return mark;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::UpdateCommandButtonAreaL( 
//      TBool aMark, TInt tempCounter )
// Updates command button area
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::UpdateCommandButtonAreaL( TBool aMark,
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
// CUPnPBrowseDialog::UpdateCommandButtonArea( TBool aMark, TInt tempCounter )
// Updates command button area
// --------------------------------------------------------------------------
void CUPnPBrowseDialog::UpdateCommandButtonArea( TBool aMark,
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
