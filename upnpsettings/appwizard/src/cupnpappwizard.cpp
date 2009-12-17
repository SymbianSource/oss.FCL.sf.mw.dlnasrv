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
* Description:      Implementation of setup Wizard
 *
*/






// INCLUDE FILES
#include <AknUtils.h>
#include <featmgr.h>
#include <bautils.h>
#include <StringLoader.h>
#include <eikspmod.h>
#include <eikclb.h>
#include <aknview.h>
#include <aknViewAppUi.h>
#include <eikprogi.h>
#include <aknmessagequerydialog.h>  //CAknMessageQueryDialog
#include <aknlistquerydialog.h>     //CAknListQueryDialog
#include <es_sock.h> //RSocketServ
#include <commdb.h>
#include <cdbcols.h>
#include <WlanCdbCols.h>
#include <wlanmgmtinterface.h>
#include <aknnotewrappers.h> //CAknWaitDialog
#include <AknWaitDialog.h>
#include <cupnpappwizard.rsg>
#include <commdb.h> // COMMS DB - for checking AP validity

// upnp stack
#include "upnpstring.h" //String::ToUnicodeL

// ADO internal interfaces
#include "upnpsettingsengine.h" //Friendly name, AP
#include "upnpsettingsengine.hrh" //AP
#include "cupnpmultiselectionui.h" //CUPnPMultiSelectionUi

// this component
#include "cupnpappwizard.h"

// logging facility
_LIT( KComponentLogfile, "upnpsetupwizard.txt");
#include "upnplog.h"

// CONSTANTS
// Filename of rsc file
_LIT( KUPnPSetupWizardRscFile, "\\resource\\cupnpappwizard.rsc" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAppWizard::CUPnPAppWizard
// --------------------------------------------------------------------------
//
CUPnPAppWizard::CUPnPAppWizard()
    {
    __LOG("CUPnPAppWizard::CUPnPAppWizard");
    iDialogDismissed = ETrue;
    iFeatureManagerInitialized = EFalse;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::ConstructL( const TDesC& aFirstStepText,
                                 CUPnPFileSharingEngine* aEng )
    {
    __LOG("CUPnPAppWizard::ConstructL begin");

    iShareImgAndVideoArray = new (ELeave) CArrayFixFlat<TInt>(3);
    iShareMusicArray = new (ELeave) CArrayFixFlat<TInt>(3);
    
    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();


    // Load resource file
    TFileName rscFileName( KUPnPSetupWizardRscFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    
    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }
    TInt err( KErrNone );
    TRAP( err, iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName ) );

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    // Get AppUI pointer
    iAppUi = static_cast<CAknViewAppUi*>( iCoeEnv->EikAppUi() );

    iSetEng = CUPnPSettingsEngine::NewL();
    if ( aEng )
        {
        iFileEng = aEng;
        }
    else
        {
        User::Leave( KErrArgument);
        }
    iFileEng->SetObserver( this );
    iFirstStepText = aFirstStepText.AllocL();

    iSharingState = iFileEng->SharingStateL();
    iSelections = new (ELeave) CSelectionStorage( iSetEng, iFileEng );
    __LOG("CUPnPAppWizard::ConstructL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPAppWizard* CUPnPAppWizard::NewL( const TDesC& aFirstStepText,
                                               CUPnPFileSharingEngine* aEng )
    {
    __LOG("CUPnPAppWizard::NewL begin");

    CUPnPAppWizard* self = new ( ELeave) CUPnPAppWizard();
    CleanupStack::PushL( self );

    self->ConstructL( aFirstStepText, aEng );

    CleanupStack::Pop();
    __LOG("CUPnPAppWizard::NewL end");
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAppWizard::~CUPnPAppWizard()
    {
    __LOG("CUPnPAppWizard::~CUPnPAppWizard begin");

    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }

    // Un-Load resource file
    if ( iResFileOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }

    delete iSetEng;

    delete iSelections;
    delete iIapNameArr;
    delete iFirstStepText;

    if ( iWait.IsStarted() )
        {
        if ( iWait.CanStopNow() )
            {
            iWait.AsyncStop();
            }
        else
            {
            __LOG("CUPnPAppWizard: \
Error cant stop iWait");
            }
        }

    if ( iFileEng )
        {
        iFileEng->SetObserver( NULL );
        }

    delete iShareImgAndVideoArray;
    delete iShareMusicArray;
    
    __LOG("CUPnPAppWizard::~CUPnPAppWizard end");
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::StartL
// The main loop of setup wizard
// --------------------------------------------------------------------------
//

EXPORT_C TInt CUPnPAppWizard::StartL( )
    {
    __LOG("CUPnPAppWizard::StartL begin");
    TInt ret(KErrNone);
    do
        {
        switch ( iStep )
            {
        case EInfo1:
            {
            ret = ShowFirstStepL( R_UPNP_WIZARD_POPUP_DIALOG_1 );
            }
            break;
        case EInfo2:
            {
            ret = ShowInfoStepL( R_UPNP_WIZARD_POPUP_DIALOG_2 );
            }
            break;
        case EDeviceName:
            {
            ret = ShowTextStepL( R_UPNP_WIZARD_POPUP_DIALOG_3 );
            }
            break;
        case EInfo4:
            {
            ret = ShowInfoStepL( R_UPNP_WIZARD_POPUP_DIALOG_4 );
            }
            break;
        case EAccessPoint:
            {
            ret = ShowListStepL( R_UPNP_WIZARD_POPUP_DIALOG_5 );
            }
            break;
        case EInfo6:
            {
            ret = ShowInfoStepL( R_UPNP_WIZARD_POPUP_DIALOG_6 );
            }
            break;
        case EShareImages:
            {
            ret = ShowMultiselectionStepL( R_UPNP_WIZARD_HEAD_7_TEXT );
            }
            break;
        case EShareMusic:
            {
            ret = ShowMultiselectionStepL(R_UPNP_WIZARD_HEAD_8_TEXT );
            }
            break;
        case EInfo9:
            {
            if ( iSelections->HasSomethingToShare() )
                {
                ret = ShowInfoStepL( R_UPNP_WIZARD_POPUP_DIALOG_9A );
                }
            else
                {
				iFileEng->SetObserver( this );
                ret = ShowInfoStepL( R_UPNP_WIZARD_POPUP_DIALOG_9B );
                }
            }
            break;
        case EStoreSettings:
            {
            iSelections->StoreSettingsL();
            if ( !iSharingState && iSelections->HasSomethingToShare() )
                {
                __LOG("CUPnPAppWizard::StartL: \
Staring sharing..");
                // need to wait for DialogDismissed()
                iDialogDismissed = EFalse;
                ShowWaitNoteL( R_UPNP_WIZARD_STARTING_FILE_SHARING_NOTE );
                DisplayInfoL( R_UPNP_WIZARD_UPDATING_CONTENT_HEADER,
                              R_UPNP_WIZARD_UPDATING_CONTENT_NOTE_TEXT,
                              R_UPNP_WIZARD_INFO_QUERY );
                }
            else if ( iSharingState && !iSelections->HasSomethingToShare() )
                {
                __LOG("CUPnPAppWizard::StartL: \
Stopping sharing..");
                iFileEng->SetSharingStateL( EFalse );
                iWait.Start();
                }

            // Stores also videofiles
            iSelections->ImageStoreSharedFilesL();


            }
            break;
        default :
            {
            __LOG("CUPnPAppWizard::StartL: default!");
            _LIT(KUser, "USER");
            User::Panic(KUser, 0);
            }
            break;
            }
        SetNextStep( ret );
        }
    while ( iStep != EFinished );
    __LOG("CUPnPAppWizard::StartL end");
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowListStepL
// Show the UI dialog with selection list
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::ShowListStepL( TInt aMain )
    {
    __LOG("CUPnPAppWizard::ShowListStepL begin");
    TInt returnValue(KErrNone);
    TInt currentSel(0);
    TInt selectedIap(1);
    TInt oldIap;
    iSelections->GetIapId( oldIap );
    TBool sharingCheck;
    RArray<TInt64> iapIdArr;
    TInt oldIapCount(0);

    if ( !iIapNameArr )
        {
        iIapNameArr = new (ELeave) CDesCArrayFlat(3);
        }

    do
        {
        sharingCheck = ETrue;// ok to exit
        currentSel = 0;
        iSelections->InitializeIapArraysL( iIapNameArr, iapIdArr );
        CUPnPSettingsEngine::GetWLANAccessPointsL( iIapNameArr, iapIdArr );
        
        // Set correct initial selection for ui
        if ( currentSel != KUPnPCreateNewInd )
            {
            iSelections->GetCurrentSelection( currentSel, iapIdArr );
            }
        else
            {
            // ..in case user has selected "Create new"
            if ( !iSharingState &&
                 returnValue &&
                 oldIapCount )
                {
                if ( oldIapCount != iapIdArr.Count() )
                    {
                    currentSel = iapIdArr.Count() - 1;
                    }
                }
            }
        CAknListQueryDialog* dlg = new
            ( ELeave ) CAknListQueryDialog( &currentSel );

        if ( !dlg )
            {
            User::Leave( KErrNoMemory );
            }
        dlg->PrepareLC( aMain );
        
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_5_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
    
        dlg->SetItemTextArray( iIapNameArr );
        dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

        CListBoxView* listBoxView = dlg->ListBox()->View();
        listBoxView->SetCurrentItemIndex( currentSel );

        returnValue = dlg->RunLD();

        TInt c( iapIdArr.Count() );

        if ( currentSel == KUPnPCreateNewInd )
            {
            /* call to Wlan creator API */
            
            __LOG("CUPnPAppWizard::ShowListStepL:\
Create new IAP..");
            CUPnPSettingsEngine::CreateAccessPointL();
            oldIapCount = iapIdArr.Count();
            }
        else if ( currentSel-1 < c )
            {
            // set last iap as selected if user succesfully created new iap
            selectedIap = iapIdArr[ currentSel ];
            }
        if ( iSharingState &&
             ( selectedIap != oldIap ) &&
             returnValue )
            {
            // show info note if sharing is on and user pressed ok
            sharingCheck = EFalse;
            ShowInfoNoteL( R_UPNP_WIZARD_IAP_TURN_SHARING_OFF );
            }
        iapIdArr.Reset();
        iIapNameArr->Reset();
        }
    while ( currentSel == KUPnPCreateNewInd ||
            ! sharingCheck );

    iSelections->SetIapId( selectedIap, currentSel );

    iapIdArr.Reset();
    __LOG("CUPnPAppWizard::ShowListStepL end");
    return returnValue;
    }

// ---------------------------------------------------------
// CUPnPAppWizard::ShowTextStepL( TInt aMain )
// Shows popup with text and header
// ---------------------------------------------------------
//
TInt CUPnPAppWizard::ShowTextStepL( TInt aMain )
    {
    __LOG("CUPnPAppWizard::ShowTextStepL");
    TInt returnValue(KErrNone);

    TBuf<64> data;
    TBool sharingCheck( ETrue );
    iSelections->GetDeviceNameL( data );
    HBufC16* oldName16 = HBufC16::NewL( data.Size() );
    CleanupStack::PushL( oldName16 );
    *oldName16 = data;

    do
        {
        sharingCheck = ETrue;
        CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL( data );

        dlg->PrepareLC( aMain );
        
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_3_TEXT );
        dlg->SetPromptL( *head );
        CleanupStack::PopAndDestroy( head );    
    
        returnValue = dlg->RunLD();

        if ( iSharingState &&
             returnValue )
            {
            TInt compRes = oldName16->CompareF( data );
            if ( compRes )
                {
                // show info note if sharing is on
                sharingCheck = EFalse;
                ShowInfoNoteL( R_UPNP_WIZARD_IAP_TURN_SHARING_OFF );
                data = *oldName16;
                }
            }
        }
    while ( !sharingCheck );
    CleanupStack::PopAndDestroy( oldName16 );
    if ( returnValue )
        {
        iSelections->SetDeviceNameL( data );
        }
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowInfoNoteL
// Show the information note without buttons
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::ShowInfoNoteL( TInt aMain ) const
    {
    __LOG("CUPnPAppWizard::ShowInfoNoteL");

    CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
    HBufC* noteText = CCoeEnv::Static()->AllocReadResourceLC(
        aMain);
    note->ExecuteLD(*noteText);
    CleanupStack::PopAndDestroy(noteText);
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowInfoStepL
// Show the information note with OK BACK buttons
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::ShowInfoStepL( TInt aMain ) const
    {
    __LOG("CUPnPAppWizard::ShowInfoStepL");
    TInt returnValue(KErrNone);
    HBufC* messageText;

    CAknMessageQueryDialog* dlg = new (ELeave)
        CAknMessageQueryDialog( CAknQueryDialog::ENoTone );

    dlg->PrepareLC( aMain );

    switch( aMain )
        {
    case R_UPNP_WIZARD_POPUP_DIALOG_2:
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_2_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
        
        HBufC* body = StringLoader::LoadLC( R_UPNP_WIZARD_2_TEXT );
        dlg->SetMessageTextL( *body );
        CleanupStack::PopAndDestroy( body );
        
        break;
        }
    case R_UPNP_WIZARD_POPUP_DIALOG_5:
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_5_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
        break;
        }    
    case R_UPNP_WIZARD_POPUP_DIALOG_6:
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_6_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
    
        messageText = StringLoader::LoadLC( R_UPNP_WIZARD_6_2_TEXT );
        dlg->SetMessageTextL( messageText->Des() );
        CleanupStack::PopAndDestroy( );
        break;
        }
    case R_UPNP_WIZARD_POPUP_DIALOG_9A:
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_9_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
        
        messageText = StringLoader::LoadLC( R_UPNP_WIZARD_9A_TEXT );
        dlg->SetMessageTextL( messageText->Des() );
        CleanupStack::PopAndDestroy( );
        break;
        }
    case R_UPNP_WIZARD_POPUP_DIALOG_9B:
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_9_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
        
        messageText = StringLoader::LoadLC( R_UPNP_WIZARD_9B_TEXT );
        dlg->SetMessageTextL( messageText->Des() );
        CleanupStack::PopAndDestroy( );
        break;
        }
    default:
        break;
        }

    if ( iStep == EInfo4 )
        {
        HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_4_TEXT );
        dlg->SetHeaderTextL( *head );
        CleanupStack::PopAndDestroy( head );
        
        HBufC* itemText2 = StringLoader::LoadLC( R_UPNP_WIZARD_ALWAYS_ASK_TEXT );
        HBufC* itemText1 = StringLoader::LoadLC( R_UPNP_WIZARD_4_TEXT,
                                                 *itemText2 );

        dlg->SetMessageTextL( itemText1->Des() );
        CleanupStack::PopAndDestroy( 2 ); //itemText 1 & 2
        }
    returnValue = dlg->RunLD();

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowFirstStepL
// Show the first information note with OK BACK buttons and given text
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::ShowFirstStepL( TInt aMain ) const
    {
    __LOG("CUPnPAppWizard::ShowFirstStepL");
    TInt returnValue( KErrNone );

    CAknMessageQueryDialog* dlg = new (ELeave)
        CAknMessageQueryDialog( CAknQueryDialog::ENoTone );

    dlg->PrepareLC( aMain );
    HBufC* head = StringLoader::LoadLC( R_UPNP_WIZARD_HEAD_1_TEXT );
    dlg->SetHeaderTextL( *head );
    CleanupStack::PopAndDestroy( head );
    
    if ( iFirstStepText->Length() )
        {
        // if the text for first step is given use it insted of default
        __LOG( "Setting text" );
        dlg->SetMessageTextL( *iFirstStepText );
        }       
    else
        {
        __LOG( " StringLoader::LoadLC" );
        HBufC* body = StringLoader::LoadLC(R_UPNP_WIZARD_1_TEXT);
        __LOG( "SetMessageTextL" );
        dlg->SetMessageTextL( *body );
        __LOG( "PopAndDestroy" );
        CleanupStack::PopAndDestroy();
        }
    returnValue = dlg->RunLD();

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowInfoStepL
// Show the multiselection popup dialog for setting content to share
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::ShowMultiselectionStepL( TInt aText )
    {
    __LOG("CUPnPAppWizard::ShowMultiselectionStepL");

    TInt returnValue(KErrNone);

    THomeConnectMediaType mediaType( EImageAndVideo );

    CArrayFix<TInt>* resArr = new (ELeave) CArrayFixFlat<TInt>(5);
    CleanupStack::PushL( resArr );

    if ( iStep == EShareMusic )
        {
        mediaType = EPlaylist;
        }
    TInt visualSharingState;
    HBufC* head = StringLoader::LoadLC( aText );
    
    // no ownership transfers
    CUPnPMultiselectionUi* uiDlg = CUPnPMultiselectionUi::
        NewL(mediaType,
             iFileEng,
             visualSharingState,
             resArr,
             CUPnPMultiselectionUi::EPopup,
             head );
    CleanupStack::PushL( uiDlg );

    ModifyMarkedArrayL(ESetMarked,NULL,uiDlg);
    
    returnValue = uiDlg->ExecutePopupLD();

    CleanupStack::PopAndDestroy( uiDlg );
    
    CleanupStack::PopAndDestroy( head );    

    if ( returnValue )
        {
        ModifyMarkedArrayL(EResetArray);
        for( TInt i = 0 ; i < resArr->Count() ; ++i )
            {
            TInt tmp = resArr->At(i);
            ModifyMarkedArrayL(EAddToArray,tmp);
            }
        // Transfer ownership of all dynamic variables( resARr )
        iSelections->SetSharedContent( mediaType,
                                       resArr );
        iFileEng->SetSelectionIndexesL( mediaType, *resArr );
        }

    CleanupStack::PopAndDestroy( resArr );
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ModifyMarkedArrayL
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::ModifyMarkedArrayL( TShareArrayState aArrayState 
                                       , TInt aValue 
                                       , CUPnPMultiselectionUi* aUiDlg )
    {
    CArrayFix<TInt>* array = NULL;
    if( iStep == EShareMusic )
        {
        array = iShareMusicArray;
        }
    else if( EShareImages == iStep )
        {
        array = iShareImgAndVideoArray;
        }
    if( array )
        {
        switch(aArrayState)
            {
            case ESetMarked:
                {
                if( aUiDlg )
                    {
                    aUiDlg->SetMarkedItemsL( *array );
                    }
                break;
                }
            case EResetArray:
                {
                array->Reset();
                break;
                }
            case EAddToArray:
                {
                array->AppendL(aValue);
                break;
                }
            default:
            	break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::SetNextStep
// Calculates the next step in wizard
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::SetNextStep( const TInt& aSelection )
    {
    __LOG("CUPnPAppWizard::SetNextStep");

    switch ( iStep )
        {
    case EInfo1:
        {
        if ( aSelection )
            {
            iStep++;
            }
        else
            {
            iStep = EFinished;
            }
        }
        break;
    case EInfo2:
        // fall through
    case EDeviceName:
        // fall through
    case EInfo4:
        // fall through
    case EAccessPoint:
        // fall through
    case EInfo6:
        // fall through
    case EShareImages:
        // fall through
    case EShareMusic:
        // fall through
    case EInfo9:
        // fall through
    case EStoreSettings:
        {
        if ( aSelection )
            {
            iStep++;
            }
        else
            {
            iStep--;
            }
        }
        break;
    default :
        {
        __LOG("CUPnPAppWizard::SetNextStep: default!");
        _LIT(KUser, "USER");
        User::Panic(KUser, 0);
        }
        break;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowErrorNoteL
// Shows note with error message
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::ShowErrorNoteL( TInt aMain ) const
    {
    __LOG("CUPnPAppWizard::ShowErrorNoteL begin");

    // show error note
    HBufC* errorNote = StringLoader::LoadLC( aMain );
    CAknErrorNote* note = new (ELeave) CAknErrorNote(ETrue);
    note->ExecuteLD(*errorNote);
    CleanupStack::PopAndDestroy(); //errorNote
    __LOG("CUPnPAppWizard::ShowErrorNoteL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::ShowWaitNoteL
// Shows standard wait note
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::ShowWaitNoteL( TInt aMain )
    {
    __LOG("CUPnPAppWizard::ShowWaitNoteL begin");

    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
    waitNoteWrapper->ExecuteL(
        aMain,      // TInt aResId,
        *this,            // MAknBackgroundProcess& aBackgroundProcess
        ETrue);
    CleanupStack::PopAndDestroy();
    __LOG("CUPnPAppWizard::ShowWaitNoteL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::StepL
// Step done during wait note
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::StepL(void)
    {
    if ( iStartingSharing == ENotActive )
        {
        iStartingSharing = EStartingSharing;
        }

    }


// --------------------------------------------------------------------------
// CUPnPAppWizard::IsProcessDone
// Returns ETrue if process finished
// --------------------------------------------------------------------------
//
TBool CUPnPAppWizard::IsProcessDone(void) const
    {

    TBool ret(EFalse);
    if ( iStartingSharing == ENotActive )
        {
        TRAPD( err, iFileEng->SetSharingStateL( ETrue ) );
        if( err != KErrNone )
            {
            return ETrue;
            }
        }
    else if ( iStartingSharing == ESharingStarted )
        {
        ret = ETrue;
        }
    else
        {
        __LOG( "CUPnPAppWizard::IsProcessDone: else branch");
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::HandleSharingStatus
// Function informs when file sharing was enabled or disabled
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::HandleSharingStatus( 
        CUPnPFileSharingEngine& /*aEngine*/, 
        TInt aError, 
        TBool aPhase )
    {
    __LOG("CUPnPAppWizard::HandleSharingStatus begin");

    if ( aPhase )
        {
        __LOG("CUPnPAppWizard::HandleSharingStatus:\
 activating sharing");
        iStartingSharing = ESharingStarted;
        if ( aError )
            {
            // If there domtree wasn't able to be written due to disk full
            // situation, send leave to kernel to inform user
            if( aError == KErrDiskFull ||
                aError == KErrNoMemory ||
                aError == KErrInUse )
                {
                //show some critical errors using
                //default symbian error notes
                CActiveScheduler::Current()->Error( aError );
                }
            else
                {
                if ( !iDialogDismissed )
                    {
                    // must ensure that wait note is gone, see DialogDismissed
                    iWait.Start();
                    }
                TRAP_IGNORE( 
                    ShowErrorNoteL( 
                        R_UPNP_WIZARD_START_SHARING_ERROR_TEXT ) );
                }
            }
        if ( iWait.IsStarted() )
            {
            if ( iWait.CanStopNow() )
                {
                iWait.AsyncStop();
                }
            else
                {
                __LOG("CUPnPAppWizard: \
Error cant stop iWait");
                }
            }
        }
    else
        {
        __LOG("CUPnPAppWizard::HandleSharingStatus:\
 deactivating sharing");
        // stop showing wait note
        if ( iWait.IsStarted() )
            {
            iWait.AsyncStop();
            }
        }

    __LOG("CUPnPAppWizard::HandleSharingStatus end");
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::HandleSharingConnectionLost
// Callback to inform if connection has been lost
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::HandleSharingConnectionLost( 
    CUPnPFileSharingEngine& /*aEngine*/ )
    {
    __LOG( "[CUPnPAppWizard]\t CUPnPAppWizard::HandleSharingConnectionLost\
 begin" );

    iSharingState = EFalse;

    __LOG( "[CUPnPAppWizard]\t CUPnPAppWizard::HandleSharingConnectionLost\
 end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::DialogDismissedL
// Callback for dialog disappearing
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::DialogDismissedL( TInt /*aButtonId*/ )
    {
    __LOG( "[CUPnPAppWizard]\t DialogDismissedL begin" );

    iDialogDismissed = ETrue;
    if ( iWait.IsStarted())
        {
        iWait.AsyncStop();
        }

    __LOG( "[CUPnPAppWizard]\t DialogDismissedL end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppWizard::DisplayInfoL()
// Shows message query to user
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::DisplayInfoL(
    TInt aHeaderResourceId,
    TInt aMessageResourceId,
    TInt aDialogResourceId )
    {
    __LOG("CUPnPAppWizard::DisplayInfoL begin");
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
    __LOG("CUPnPAppWizard::DisplayInfoL end");
    }


// --------------------------------------------------------------------------
// CUPnPAppWizard::CSelectionStorage functions
//
// --------------------------------------------------------------------------
//

// --------------------------------------------------------------------------
// CSelectionStorage::CSelectionStorage
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAppWizard::CSelectionStorage::~CSelectionStorage()
    {
    __LOG("[UPnPAppWizard]\tCSelectionStorage::~CSelectionStorage() begin");
    delete iDeviceName;
    delete iMusicSelectionArray;
    delete iImageSelectionArray;
    __LOG("CSelectionStorage::~CSelectionStorage() end");
    }

// --------------------------------------------------------------------------
// CSelectionStorage::CSelectionStorage
// Constructor
// --------------------------------------------------------------------------
//
CUPnPAppWizard::CSelectionStorage::CSelectionStorage(
    CUPnPSettingsEngine* aSetEng,
    CUPnPFileSharingEngine* aFileEng )
    {
    __LOG("CSelectionStorage::CSelectionStorage() begin");
    iSetEng = aSetEng;
    iFileEng = aFileEng;
    __LOG("CSelectionStorage::CSelectionStorage() end");
    }

// --------------------------------------------------------------------------
// CSelectionStorage::SetDeviceNameL
// Function for setting the device name to local storage
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::SetDeviceNameL( TDes& aDeviceName )
    {
    __LOG("CSelectionStorage::SetDeviceNameL begin");
    delete iDeviceName;
    iDeviceName = NULL;
    iDeviceName = HBufC::NewL(aDeviceName.Length());

    *iDeviceName = aDeviceName;
    __LOG("CSelectionStorage::SetDeviceNameL end");
    }

// --------------------------------------------------------------------------
// CSelectionStorage::GetDeviceNameL
// Function for getting the device name from local storage or from cenrep
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::CSelectionStorage::GetDeviceNameL( TDes& aDeviceName )
    {
    __LOG("CSelectionStorage::GetDeviceNameL begin");
    TInt err(0);
    if ( iDeviceName )
        {
        aDeviceName = iDeviceName->Des();
        }
    else
        {
        THostName data;
        TBuf8<65> deviceName8;
        err = iSetEng->GetLocalFriendlyName( deviceName8 );

        HBufC* deviceName16 = UpnpString::ToUnicodeL( deviceName8 );
        CleanupStack::PushL( deviceName16 );
        aDeviceName = deviceName16->Des();
        CleanupStack::PopAndDestroy( deviceName16 );
        }
    __LOG("CSelectionStorage::GetDeviceNameL end");
    return err;
    }

// --------------------------------------------------------------------------
// CSelectionStorage::GetIapId
// Function for getting the iapid from local storage or from cenrep
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::CSelectionStorage::GetIapId( TInt& aIapId )
    {
    __LOG("CSelectionStorage::GetIapId begin");
    TInt err( KErrNone );
    if ( iIapId )
        {
        aIapId = iIapId;
        }
    else
        {
        err = iSetEng->GetAccessPoint( iIapId );
        aIapId = iIapId;
        }
    __LOG("CSelectionStorage::GetIapId end");
    return err;
    }

// --------------------------------------------------------------------------
// CSelectionStorage::SetIapId
// Function for setting the iapid to local storage
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::SetIapId( const TInt aIapId,
                                                  const TInt aIndexForUi )
    {
    __LOG("CSelectionStorage::SetIapId begin");

    iIapId = aIapId;
    iIndexForUi = aIndexForUi;

    __LOG("CSelectionStorage::SetIapId end");
    }

// --------------------------------------------------------------------------
// CSelectionStorage::StoreSettingsL
// Function for storing devicename and iapid to cenrep
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::CSelectionStorage::StoreSettingsL()
    {
    __LOG("CSelectionStorage::StoreSettingsL begin");
    TInt err(0);
    if ( iDeviceName )
        {
        HBufC8* name8 = UpnpString::FromUnicodeL( iDeviceName->Des() );
        CleanupStack::PushL( name8 );
        err = iSetEng->SetLocalFriendlyName( name8->Des() );
        CleanupStack::PopAndDestroy( name8 );
        }
    if ( iIapId >= EUPnPSettingsEngineIAPIdAlwaysAsk )
        {
        // Stores iap id.
        iSetEng->SetAccessPoint( iIapId );
        
        // Stores iap selection type.
        TInt iapForHomeNetworkUi( EUPnPSettingsEngineIAPItemUserDefined );
        if ( iIapId == EUPnPSettingsEngineIAPIdAlwaysAsk )
            {
            iapForHomeNetworkUi = EUPnPSettingsEngineIAPItemAlwaysAsk;
            }
        iSetEng->SetAccessPointSetting( iapForHomeNetworkUi );
        
        // index for HN ui?
        iSetEng->SetWapId( iIndexForUi+1 );
        }

    __LOG("CSelectionStorage::StoreSettingsL end");
    return err;
    }

// --------------------------------------------------------------------------
// CSelectionStorage::ImageStoreSharedFilesL
// Function for storing image and video
// --------------------------------------------------------------------------
//
TInt CUPnPAppWizard::CSelectionStorage::ImageStoreSharedFilesL( )
    {
    /* Save Images and Videos */
    __LOG( "CSelectionStorage::Sharing image and video .." );
    if ( iFileEng )
        {
        iFileEng->ShareMediaL( EImageAndVideo );
        iFileEng->ShareMediaL( EPlaylist );
        }   
    __LOG( "CSelectionStorage::ImageStoreSharedFilesL end" );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CSelectionStorage::InitializeIapArraysL
// Add default items to iap name array
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::InitializeIapArraysL(
    CDesCArray* aIapNameArr,
    RArray<TInt64>& aIapIdArr ) const
    {
    __LOG("[UPnPAppWizard]\tCSelectionStorage::InitializeIapArraysL begin");

    HBufC* itemText1 = StringLoader::LoadLC( R_UPNP_WIZARD_ALWAYS_ASK_TEXT );
    HBufC* itemText2 = StringLoader::LoadLC( R_UPNP_WIZARD_CREATE_NEW_TEXT );

    aIapNameArr->AppendL( itemText1->Des() );
    aIapIdArr.AppendL( EUPnPSettingsEngineIAPIdAlwaysAsk );
    aIapNameArr->AppendL( itemText2->Des() );
    aIapIdArr.AppendL( EUPnPSettingsEngineIAPIdCreateNew );

    //aIapNameArr does not own the items
    CleanupStack::PopAndDestroy(2);

    __LOG("[UPnPAppWizard]\tCSelectionStorage::InitializeIapArraysL end");
    }

// -------------------------------------------------------------
// CSelectionStorage::SetSharedContent()
// Sets the users sharing selections to local storage
// -------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::SetSharedContent(
    THomeConnectMediaType aMedia,
    CArrayFix<TInt>* aSelections )
    {
    __LOG("CSelectionStorage::SetSharedContent begin");

    if ( aMedia == EImageAndVideo )
        {
        // determine the users selections from UI
        // if there is only one item selected and its value is not 0
        // -> Share all is selected
        if ( ! (aSelections->Count() == 1 && aSelections->At(0) == 0 ) )
            {
            iVisualImageSelection = ETrue;
            }
        }
    else
        {

        if ( ! (aSelections->Count() == 1 && aSelections->At(0) == 0 ) )
            {
            // no difference between EShareAll and EShareMany
            iVisualMusicSelection = ETrue;
            }

        }

    __LOG("CSelectionStorage::SetSharedContent end");
    }


// -------------------------------------------------------------
// CSelectionStorage::SetSharedContent()
// Gets the users sharing selections from local storage
// -------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::GetSelectionsL(
    THomeConnectMediaType aMedia,
    CArrayFix<TInt>* aSelections )
    {
    __LOG("CSelectionStorage::GetSelectionsL begin");

    if ( aMedia == EImageAndVideo )
        {
        if ( iImageSelectionArray )
            {
            for ( TInt i = 0; i < iImageSelectionArray->Count(); i++ )
                {
                TInt tmp = iImageSelectionArray->At( i );
                aSelections->AppendL( tmp );
                }
            }
        }
    else
        {
        if ( iMusicSelectionArray )
            {
            for ( TInt i = 0; i < iMusicSelectionArray->Count(); i++ )
                {
                TInt tmp = iMusicSelectionArray->At( i );
                aSelections->AppendL( tmp );
                }
            }
        }
    __LOG("CSelectionStorage::GetSelectionsL end");
    }

// -------------------------------------------------------------
// CSelectionStorage::HasSomethingToShare()
// Determines if the user has selected content to share
// -------------------------------------------------------------
//
TBool CUPnPAppWizard::CSelectionStorage::HasSomethingToShare() const
    {
    __LOG("CSelectionStorage::HasSomethingToShare begin");
    TBool retVal( EFalse );
    if ( iVisualImageSelection || iVisualMusicSelection )
        {
        retVal = ETrue;
        }
    __LOG("CSelectionStorage::HasSomethingToShare end");
    return retVal;
    }

// --------------------------------------------------------------------------
// CSelectionStorage::GetCurrentSelection
// Gets the user selected iapid or iapid from cenrep
// --------------------------------------------------------------------------
//
void CUPnPAppWizard::CSelectionStorage::GetCurrentSelection(
    TInt& aCurrentSelection, RArray<TInt64>& aIapIdArr )
    {
    __LOG("CSelectionStorage::GetCurrentSelection begin");
    // Get the current selection
    TInt userIapId( 0 );
    GetIapId( userIapId );

    if ( userIapId < 1 ||
         userIapId == aIapIdArr[ KUPnPAlwaysAskInd ] )
        {
        aCurrentSelection = 0;
        }
    else
        {
        aCurrentSelection = aIapIdArr.Find( userIapId );

        if ( aCurrentSelection <= KUPnPCreateNewInd )
            {
            /* Current iap is not in the list or it is Always Ask */
            aCurrentSelection = 0;
            }
        }
    __LOG("CSelectionStorage::GetCurrentSelection end");
    }


// End of File
