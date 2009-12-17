/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      File sharing view of Home Network application
*
*/






// INCLUDE FILES
// system
#include <aknlists.h>
#include <akntitle.h>
#include <AknProgressDialog.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <hlplch.h>
#include <eikprogi.h>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h> //displayinfol
#include <akninputblock.h>          // CAknInputBlock
#include <bautils.h> // BaflUtils

// internal interfaces
#include "cupnpmultiselectionui.h"
#include "upnpfilesharingengine.h"
#include "upnpfilesharingengineobserver.h"

// this component
#include <upnpsharingui.rsg>
#include "upnpsharingui.hrh"
#include "upnpappfilesharinglist.h"
#include "upnpappfilesharingdialog.h"


_LIT( KComponentLogfile, "upnpsharingui.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KHomeConnectUID3 = 0x10208A0A; // needed for help context
_LIT( KUPNP_HLP_SHARE_CONTENT, "UPNP_HLP_SHARE_CONTENT" ); 
// Filename of rsc file
_LIT( KSharingUiRscFile, "\\resource\\upnpsharingui.rsc" );


// ============================ MEMBER FUNCTIONS ============================


// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPAppFileSharingDialog* CUPnPAppFileSharingDialog::NewL( 
    TInt aMenuResource,
    CUPnPFileSharingEngine& aEng )
    {
    __LOG("CUPnPAppFileSharingDialog::NewL");

    CUPnPAppFileSharingDialog* self =
        new (ELeave) CUPnPAppFileSharingDialog( aEng );
    CleanupStack::PushL(self);
    self->ConstructL( aMenuResource );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CUPnPAppFileSharingDialog::CUPnPAppFileSharingDialog( 
    CUPnPFileSharingEngine& aEng )
    : iFileSharingEngine( aEng )
    , iFeatureManagerInitialized( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::ConstructL( TInt aMenuResource )
    {
    __LOG("CUPnPAppFileSharingDialog::ConstructL");

    iFileSharingEngine.SetObserver(this);

    if ( aMenuResource == 0 )
        {
        aMenuResource = R_SHARINGUI_MENUBAR;
        }

    // determine resource file location
    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();
    TFileName rscFileName( KSharingUiRscFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }
    // load the resource file
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );
    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    CAknDialog::ConstructL( aMenuResource );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAppFileSharingDialog::~CUPnPAppFileSharingDialog()
    {
    __LOG("CUPnPAppFileSharingDialog::~CUPnPAppFileSharingDialog");

    iFileSharingEngine.SetObserver( NULL );

    // unload resource file
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }

    if( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::ExecuteSharingDialogLD()
// Executes the dialog using the component's own resource
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPAppFileSharingDialog::ExecuteSharingDialogLD()
    {
    return ExecuteLD( R_SHARINGUI_DIALOG );
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
//
TBool CUPnPAppFileSharingDialog::OkToExitL(TInt aButtonId)
{
   __LOG("CUPnPAppFileSharingDialog::OkToExitL");

   TBool retval(EFalse);
   if (aButtonId == EAknSoftkeyOptions)
       {
       DisplayMenuL();
       }
   else if (aButtonId == EAknSoftkeyBack)
       {
       iTitlePane->SetTextToDefaultL();
       retval = ETrue; //back to previous dialog
       }
   else if ( aButtonId == EAknSoftkeyChange )
       {
       // msk event
       TKeyEvent tmpEvent;
       tmpEvent.iCode = EKeyOK;
       tmpEvent.iModifiers = 0;
       tmpEvent.iRepeats = 0;
       tmpEvent.iScanCode = 0;
       TEventCode eventType = EEventKey;
       OfferKeyEventL( tmpEvent, eventType );
       }
    return retval;
}

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::DynInitMenuPaneL
// Dynamically initalizes menu pane
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {
    __LOG("CUPnPAppMainDialog::CUPnPAppFileSharingDialog");

    if (aResourceId == R_SHARINGUI_MENU && 
        !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::PreLayoutDynInitL()
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::PreLayoutDynInitL()
    {
    __LOG("CUPnPAppFileSharingDialog::PreLayoutDynInitL");

    iFileSharingList->LoadDataFromEngineL();

    // set dialog title
    iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    iTitlePane = (CAknTitlePane*)iStatusPane->ControlL(
                    TUid::Uid(EEikStatusPaneUidTitle));
    iTitlePane->SetTextL(*StringLoader::LoadLC( R_SHARINGUI_TITLE_TEXT ) );
    CleanupStack::PopAndDestroy();
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::ProcessCommandL()
// called by framework when menu command is selected
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::ProcessCommandL(TInt aCommand)
    {
    __LOG("CUPnPAppFileSharingDialog::ProcessCommandL");

    HideMenu();

    switch ( aCommand )
        {
        case ESharingUiCmdRefresh: 
            {
            TInt selectedItem = iFileSharingList->
                ListBox()->CurrentItemIndex();
            iFileSharingEngine.RefreshSharedMediaL( EImageAndVideo );
            iFileSharingEngine.RefreshSharedMediaL( EPlaylist );
            DisplayInfoL( R_SHARINGUI_UPDATING_CONTENT_HEADER,
                          R_SHARINGUI_UPDATING_CONTENT_NOTE_TEXT,
                          R_SHARINGUI_INFO_QUERY );
            break;
            }
    case ESharingUiCmdAppChange:
        {
        TInt selectedItem = iFileSharingList->ListBox()->CurrentItemIndex();
        if ( selectedItem == ESharingUiSettingItemVisibility )
            {
            iFileSharingList->EditItemL(selectedItem, ETrue);
            }
        else
            {
            RunMultiselectionDialogL();
            }
        break;
        }

    case EAknCmdExit:
        {
        User::Exit(KErrNone);
        break;
        }

    case EAknCmdHelp:
        {
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                     iEikonEnv->EikAppUi()->AppHelpContextL() );
        break;
        }

    default:
        {
        __LOG("CUPnPAppFileSharingDialog::ProcessCommandL Unknown cmd");
        }
        }

    CAknDialog::ProcessCommandL(aCommand);
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsDialog::OfferKeyEventL()
// called by framework when key is pressed
// --------------------------------------------------------------------------
//
TKeyResponse CUPnPAppFileSharingDialog::OfferKeyEventL(
    const TKeyEvent &aKeyEvent, TEventCode aType )
    {
    __LOG("CUPnPAppFileSharingDialog::OfferKeyEventL");

    TKeyResponse retval = EKeyWasNotConsumed;
    switch ( aKeyEvent.iCode )
        {
        case EKeyDownArrow:
        case EKeyUpArrow:
            {
            if (aType == EEventKey)
                {
                retval = iFileSharingList->OfferKeyEventL( aKeyEvent, aType );
                }
            break;
            }

        case EKeyOK:
            {
            if ( aType == EEventKey )
                {
                TInt selectedItem =
                    iFileSharingList->ListBox()->CurrentItemIndex();
                if ( selectedItem == ESharingUiSettingItemVisibility )
                    {
                    retval = iFileSharingList->OfferKeyEventL(aKeyEvent, aType);
                    }
                else
                    {
                    RunMultiselectionDialogL();
                    retval = EKeyWasConsumed;
                    }
                }
            break;
            }

        default:
            {
            retval = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
            break;
            }
        }
        
    return retval;
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::HandleSharingStatus
// Function informs when file sharing was enabled or disabled
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::HandleSharingStatus( 
        CUPnPFileSharingEngine& aEngine, 
        TInt aError, 
        TBool aPhase )
    {
    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingStatus begin");

    iFileSharingList->HandleSharingStatus( aEngine, aError, aPhase );

    if( !aError )
        {
        TInt strId( R_UPNP_PHONE_HIDDEN_TEXT );
        if( aPhase )
            {        
            strId = R_UPNP_PHONE_VISIBLE_TEXT;
            }

        TRAP_IGNORE(
            HBufC* str = StringLoader::LoadLC( strId );
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
            note->ExecuteLD( *str );
            CleanupStack::PopAndDestroy( str ) );
        }

    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingStatus end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::HandleSharingProgress
// Informs about status of the sharing progress
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::HandleSharingProgress(
        CUPnPFileSharingEngine& /*aEngine*/, 
        const RArray<TUpnpProgressInfo>& aProgress )
    {
    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingProgress begin");

    TRAPD( err,
        CAknInputBlock::NewLC();
        iFileSharingList->SetFilesharingTextsL( aProgress );
        CleanupStack::PopAndDestroy() );
    if ( err != KErrNone )
        {
        //show some critical errors using
        //default symbian error notes
        CActiveScheduler::Current()->Error( err );
        }

    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingProgress end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::HandleSharingDone
// Function informs when file sharing is done
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::HandleSharingDone( 
        CUPnPFileSharingEngine& /*aEngine*/, TInt aError )
    {
    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingDone begin");

    //Show information note that sharing is completed
    if ( aError == KErrInUse ||
         aError == KErrNoMemory ||
         aError == KErrDiskFull )
        {
        //show some critical errors using
        //default symbian error notes
        CActiveScheduler::Current()->Error( aError );
        }

    __LOG( "[CUPnPAppFileSharingDialog]\t CUPnPAppFileSharingDialog::\
HandleSharingDone end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::HandleSharingConnectionLost
// Callback to inform if connection has been lost
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::HandleSharingConnectionLost( 
        CUPnPFileSharingEngine& aEngine )
    {
    __LOG("CUPnPAppFileSharingDialog::HandleSharingConnectionLost");

    TRAPD( err,
        CAknInputBlock::NewLC();
        iFileSharingList->HandleSharingConnectionLost( aEngine );
        CleanupStack::PopAndDestroy() ); //CAknInputBlock
    if ( err != KErrNone )
        {
        //show some critical errors using
        //default symbian error notes
        CActiveScheduler::Current()->Error( err );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::RunMultiselectionDialogL();
// Opens album selection dialog and sends selected albums
// to engine
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::RunMultiselectionDialogL(void)
    {
    __LOG("CUPnPAppFileSharingDialog::RunMultiselectionDialogL");

    CUPnPMultiselectionUi* multiselectionDialog;

    TInt selectedItem = iFileSharingList->ListBox()->CurrentItemIndex();

    TInt visualSharingState;


    THomeConnectMediaType mediaType;
    if ( selectedItem == ESharingUiSettingItemVisual )
        {
        mediaType =
                                EImageAndVideo;
        }

    else
        {
        mediaType =
                                EPlaylist;
        }
    CArrayFix<TInt>* markedItems = new(ELeave) CArrayFixFlat<TInt>(3);
    CleanupStack::PushL( markedItems );
    multiselectionDialog = CUPnPMultiselectionUi::NewL(
                                mediaType,
                                &iFileSharingEngine,
                                visualSharingState,
                                markedItems );  
    CleanupStack::PushL( multiselectionDialog );
    TInt ret = multiselectionDialog->ExecuteDlgLD();

    CleanupStack::PopAndDestroy( multiselectionDialog );

    multiselectionDialog = NULL;

    // Set title back
    iTitlePane->SetTextL(*StringLoader::LoadLC( R_SHARINGUI_TITLE_TEXT ));
    CleanupStack::PopAndDestroy();

    if ( ret )
        {
        __LOG("CUPnPAppFileSharingDialog::RunMultiselectionDialogL: \
Calling ShareMediaFilesL..");
        // set selections
        iFileSharingEngine.SetSelectionIndexesL( mediaType, *markedItems );

        // Share files asynchronously
        iFileSharingEngine.ShareMediaL( mediaType );

        DisplayInfoL( R_SHARINGUI_UPDATING_CONTENT_HEADER,
                      R_SHARINGUI_UPDATING_CONTENT_NOTE_TEXT,
                      R_SHARINGUI_INFO_QUERY );
        }
    CleanupStack::PopAndDestroy( markedItems );
    
     }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::GetHelpContext
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::GetHelpContext(
                                TCoeHelpContext& aContext) const
    {
    __LOG("CUPnPAppMainDialog::CUPnPAppFileSharingDialog");

    aContext.iMajor = TUid::Uid( KHomeConnectUID3 );
    aContext.iContext = KUPNP_HLP_SHARE_CONTENT;
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::CreateCustomControlL
// From CAknDialog
// --------------------------------------------------------------------------
//
SEikControlInfo CUPnPAppFileSharingDialog::CreateCustomControlL(
                                                    TInt aControlType)
    {
    __LOG( "[CUPnPAppFileSharingDialog]\t CreateCustomControlL");
    iFileSharingList = CUPnPAppFileSharingList::NewL( 
        &iFileSharingEngine );

    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    
    if ( aControlType == KAknCtLastControlId )
        {
        controlInfo.iControl = iFileSharingList;
        }
    iFileSharingEngine.RequestSharingProgressL();

    return controlInfo;
    }
    
// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::DisplayInfoL();
// Shows message query to user
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingDialog::DisplayInfoL(TInt aHeaderResourceId,
                                             TInt aMessageResourceId,
                                             TInt aDialogResourceId)
    {
    HBufC* noteHeader = StringLoader::LoadL(
        aHeaderResourceId);
    CleanupStack::PushL(noteHeader);
    HBufC* noteMsg = StringLoader::LoadL(
        aMessageResourceId);
    CleanupStack::PushL(noteMsg);

    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*noteMsg);

    dlg->PrepareLC(aDialogResourceId);
    dlg->QueryHeading()->SetTextL(*noteHeader);
    dlg->RunLD();

    CleanupStack::PopAndDestroy(noteMsg);
    CleanupStack::PopAndDestroy(noteHeader);
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingDialog::UpdateFileSharingListL();
// Get the pointer of iFileSharingList,update the visibity of item
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAppFileSharingDialog::UpdateFileSharingListL() 
    {
    if( iFileSharingList )
        {  
        iFileSharingList->UpdateVisibitySettingItemL();
        }
   
    }
//  End of File
