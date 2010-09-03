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
* Description:  Implementation of setup Wizard
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <cmsappwizard.rsg>
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpavcontrollerfactory.h"
#include <driveinfo.h>
#include <utf.h>
#include <aknnotewrappers.h>

#include "cmdriveinfo.h"
#include "cmmediaserverfull.h"
#include "cmstorerulecontainer.h"
#include "cmstorerule.h"
#include "cmfillrulecontainer.h"
#include "cmfillrule.h"
#include "cmsappwizard.h"
#include "msengine.h"
#include "cmsmultiselectionpopup.h"
#include "cmsselectionstorage.h"
#include "msconstants.h"
#include "msdebug.h"
#include "upnpavdevicelist.h"

// CONSTANTS
_LIT( KMSSetupWizardRscFile, "\\resource\\apps\\cmsappwizard.rsc" );
#ifdef _DEBUG
_LIT( KPanicedUnit, "MSAPPWIZARD" );
#endif // _DEBUG

const TInt KArrayGranularity = 3;
const TInt KFillListGranularity = 3;
const TInt KStoreListGranularity = 5;


// --------------------------------------------------------------------------
// CMSAppWizard::CMSAppWizard
// --------------------------------------------------------------------------
//
CMSAppWizard::CMSAppWizard( CMSEngine& aMSEngine )
    {
    iMSEngine = &aMSEngine;
    iMaxSteps = ELastStep;
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CMSAppWizard"));
    }

// --------------------------------------------------------------------------
// CMSAppWizard::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CMSAppWizard::ConstructL()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ConstructL begin"));

    iCoeEnv = CEikonEnv::Static();
    iFileSession = iCoeEnv->FsSession();

    // Load resource file
    TFileName rscFileName( KMSSetupWizardRscFile );
    User::LeaveIfError( CompleteWithAppPath( rscFileName ) );

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( iFileSession, rscFileName );
    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( iFileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }

    TInt err(KErrNone);
    TRAP( err, iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName ) );
    User::LeaveIfError( err );

    iSelections = CMSSelectionStorage::NewL( *iMSEngine,
                                             iMediaServers,
                                             iFileSession );

    iDeviceNameArray = new (ELeave) CDesCArrayFlat( KArrayGranularity );

    // original servers
    iOriginalServers = iMSEngine->GetMediaServersL();
    ClearServersSystemUpdateId();

    // Start searching servers
    TInt error = CreateAvControllerL();
    if ( !error && iAvController )
        {
        // Process devices that AVController has already discovered.
        FetchAlreadyDiscoveredDevicesL();
        }
    
#if _DEBUG
    RDebug::Printf("[MSAppWizard]\t CMSAppWizard::ConstructL: %s, %s\n",\
__DATE__,__TIME__);
#endif
    LOG( _L("[MSAppWizard]\t CMSAppWizard::ConstructL end") );
    }

// --------------------------------------------------------------------------
// CMSAppWizard::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CMSAppWizard* CMSAppWizard::NewL( CMSEngine& aMSEngine )
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::NewL begin") );

    CMSAppWizard* self = CMSAppWizard::NewLC( aMSEngine );
    
    CleanupStack::Pop( self );
    LOG( _L("[MSAppWizard]\t CMSAppWizard::NewL end") );
    return self;
    }

// --------------------------------------------------------------------------
// CMSAppWizard::NewLC()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CMSAppWizard* CMSAppWizard::NewLC( CMSEngine& aMSEngine )
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::NewLC begin") );

    CMSAppWizard* self = new ( ELeave) CMSAppWizard( aMSEngine );
    CleanupStack::PushL( self );

    self->ConstructL();

    LOG( _L("[MSAppWizard]\t CMSAppWizard::NewLC end") );
    return self;
    }    

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CMSAppWizard::~CMSAppWizard()
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::~CMSAppWizard begin") );

    // stop device search
    CancelDeviceSearch();
    
    // UnLoad resource file
    iCoeEnv->DeleteResourceFile( iResFileOffset );

    if ( iOriginalServers )
        {
        iOriginalServers->ResetAndDestroy();
        iOriginalServers->Close();
        delete iOriginalServers;
        }

    if ( iDeviceNameArray )
        {
        iDeviceNameArray->Reset();
        delete iDeviceNameArray;
        }

    iDriveIndexes.Close();

    iDriveArray.ResetAndDestroy();
    iDriveArray.Close();

    iStoreServers.Reset();
    iStoreServers.Close();

    iMediaServers.ResetAndDestroy();
    iMediaServers.Close();

    delete iSelections;

    if ( iAvController )
        {
        iAvController->RemoveDeviceObserver();
		iAvController->Release();
		iAvController = NULL;
        }

    CloseWaitDialog();

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    LOG( _L("[MSAppWizard]\t CMSAppWizard::~CMSAppWizard end") );
    }

// --------------------------------------------------------------------------
// CMSAppWizard::StartL
// The main loop of setup wizard
// --------------------------------------------------------------------------
//

EXPORT_C TInt CMSAppWizard::StartL()
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::StartL begin") );
    TInt ret(KErrNone);
    do
        {
        switch ( iStep )
            {
            case EStep1:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_1, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep2:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_2, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep3:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_3, ELastStep );                
                
                CDesCArrayFlat* itemArray =
                                    CreateMultiselectionFillListItemsLC();

                CArrayFix<TInt>* resultArray =
                    new (ELeave) CArrayFixFlat<TInt>( KFillListGranularity );
                CleanupStack::PushL( resultArray );
                                                    
                ret = ShowMultiselectionStepL( *itemArray,
                                               *resultArray,
                                               head->Des() );
                // keep selection safe
                iSelections->SetFillListSelections( *resultArray );                
                CleanupStack::Pop( resultArray );
                CleanupStack::PopAndDestroy( itemArray );                                                           
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep4:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_4, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep5:
                {
                CreateServerNameListL( iMediaServers );
                
                if ( iDeviceNameArray->Count() == 0 )
                    {
                    // no devices yet - show wait note                
                    ShowWaitNoteL();
                    // wait here for the first device before
                    // showing device selection
                    iWait.Start();                    
                    }
                if ( !iUserCancelledSearch )
                    {
                    HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_5, ELastStep );

                    CArrayFix<TInt>* selectedIndexes =
                       new (ELeave) CArrayFixFlat<TInt>( KArrayGranularity );
                    CleanupStack::PushL( selectedIndexes );                                                      
                    ret = ShowMultiselectionStepL( *iDeviceNameArray,
                                               *selectedIndexes,
                                               head->Des() );
                    iSelections->SetServersForFillRulesL( *selectedIndexes );
                    CleanupStack::PopAndDestroy( selectedIndexes );
                    CleanupStack::PopAndDestroy( head );                
                    }
                else
                    {
                    ret = KErrCancel;
                    }                                
                
                break;
                }
            case EStep6:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_6, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep7:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_7, ELastStep );
                //used CreateMultiselectionStoreListItemsLC before, modify then to 
                //make it the same with UI spec
                CDesCArrayFlat* itemArray =
                                    CreateMultiselectionUIStoreListStringLC();

                CArrayFix<TInt>* resultArray =
                   new (ELeave) CArrayFixFlat<TInt>( KStoreListGranularity );
                CleanupStack::PushL( resultArray );    
                                                    
                ret = ShowMultiselectionStepL( *itemArray,
                                               *resultArray,
                                               head->Des() ); 
                // keep selection safe
                iSelections->SetStoreListSelections( *resultArray );                                                                
                CleanupStack::Pop( resultArray );
                CleanupStack::PopAndDestroy( itemArray );                
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep8:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_8, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep9:
                {                
                HBufC* head = StringLoader::LoadLC(
                                    R_MS_HEADING_STEP_9, ELastStep );
                                    
				// Clear previous store servers
    			iStoreServers.Reset();
    			
        		// copy those media servers to store servers list, 
        		// where copy capability is active
				for ( TInt index=0; index < iMediaServers.Count(); index++ )
					{
					if ( iMediaServers.operator[](index)->CopyCapability() )
						{
						iStoreServers.AppendL( 
						    iMediaServers.operator[]( index ) );
						}
					}

                // append items to iDeviceNameArray
                CreateServerNameListL( iStoreServers );

                CArrayFix<TInt>* selectedIndexes =
                       new (ELeave) CArrayFixFlat<TInt>( KArrayGranularity );
                CleanupStack::PushL( selectedIndexes );                       

                if ( iDeviceNameArray->Count() )
                    {
                    ret = ShowMultiselectionStepL( *iDeviceNameArray,
                                               *selectedIndexes,
                                               head->Des() ); 
                    if ( ret > 0 && selectedIndexes->Count() == 0 )
                        {
                        ShowInfoNoteL( R_MS_NO_TARGET_DEVICES_SELECTED_INFO );
                        // clear store list selections
                        iSelections->ClearStoreListSelections();
                        }                                                
                    iSelections->SetServersForStoreRulesL( *selectedIndexes,
                                                           iStoreServers );
                    }
                else // no devices available - show info note
                    {
                    ret = ShowInfoStepL( iStep, head->Des() );
                    if ( ret > KErrNone )
                        {                        
                        // clear store list selections
                        iSelections->ClearStoreListSelections();
                        iSelections->SetServersForStoreRulesL(
                                                *selectedIndexes,
                                                iStoreServers );
                        }
                    }                    
                
                CleanupStack::PopAndDestroy( selectedIndexes );                                    
                CleanupStack::PopAndDestroy( head );                                                                                  
                break;
                }

            case EStep10:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_10, ELastStep );
                ret = ShowInfoStepL( iStep, head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep11:
                {
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_11, ELastStep );
                ret = ShowListStepL( R_MS_WIZARD_SELECTION_DIALOG_8,
                                     head->Des() );
                CleanupStack::PopAndDestroy( head );
                break;
                }
            case EStep12:
                {
                 // two parameters
                CArrayFix<TInt>* stepArray =
                                new ( ELeave ) CArrayFixFlat<TInt>(2);
                CleanupStack::PushL( stepArray );
                stepArray->AppendL( ELastStep );                                
                stepArray->AppendL( ELastStep );
                HBufC* head = StringLoader::LoadLC(
                                        R_MS_HEADING_STEP_12, *stepArray );                
                ret = ShowInfoStepL( iStep, head->Des() );                
                CleanupStack::PopAndDestroy( head );
                CleanupStack::PopAndDestroy( stepArray );
                break;
                }                
            case ELastStep:
                {                
                // stop device search
                CancelDeviceSearch();
                
                // set remove status and name to old servers
                CheckAndSetServerStateL();
                // clear ids again because there might be new unselected servers
                ClearServersSystemUpdateId();

                iMSEngine->SetMediaServersL( *iOriginalServers );

                // Set used drives
                iSelections->SetDriveInformationL();

                iSelections->StoreMSSettingsL();

                ret = ELastStep; // stop looping
                break;
                }
            case ECancelWizard:
            	{
                // stop device search
                CancelDeviceSearch();
                
            	// Wizard is cancelled, do not start information refreshing
            	ret = KErrNone;
            	break;
            	}
            default :
                {
                LOG( _L("[MSAppWizard]\t CMSAppWizard::StartL: default!") );
                __ASSERT_DEBUG( EFalse, User::Panic( KPanicedUnit, 0 ) );
                break;
                }
            }
            
        SetNextStep( ret );
        }
    while ( iStep != EWizardEnd );
    
    LOG(_L("[MSAppWizard]\t CMSAppWizard::StartL end"));
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CMSAppWizard::DialogDismissedL( TInt aButtonId )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::DialogDismissedL"));
    if( aButtonId != EAknSoftkeyDone )
        {
        LOG(_L("[MSAppWizard]\t CMSAppWizard::DialogDismissedL CANCEL"));
        if ( aButtonId == EAknSoftkeyCancel )
            {
            iUserCancelledSearch = ETrue;
            }
        }
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// CMSAppWizard::ShowInfoNoteL
// --------------------------------------------------------------------------
//
void CMSAppWizard::ShowInfoNoteL( TInt aBodyTextResource )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowInfoNoteL"));
    CAknInformationNote* dlg = new(ELeave)CAknInformationNote( ETrue );
    CleanupStack::PushL( dlg );
    HBufC* noteText = StringLoader::LoadLC( aBodyTextResource );
    dlg->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    CleanupStack::Pop( dlg );
    }

// --------------------------------------------------------------------------
// CMSAppWizard::AddDeviceNameL
// --------------------------------------------------------------------------
//
void CMSAppWizard::AddDeviceNameL( const CUpnpAVDevice& aDevice )
    {
    CUpnpAVDevice* device = const_cast<CUpnpAVDevice*> ( &aDevice );

    HBufC* devName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
	                                           device->FriendlyName() );
	CleanupStack::PushL( devName );
	TRACE(Print(_L("[MSAppWizard]\t device Name= %S"), devName ));
	iDeviceNameArray->AppendL( *devName );        
	CleanupStack::PopAndDestroy( devName );	
	}

// --------------------------------------------------------------------------
// CMSAppWizard::ShowInfoStepL
//
// --------------------------------------------------------------------------
//
TInt CMSAppWizard::ShowInfoStepL( TInt aStep, TPtr aHeading )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowInfoStepL"));
    TInt returnValue( KErrNone );

    CAknMessageQueryDialog* dlg = new (ELeave)
    CAknMessageQueryDialog( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dlg );

    dlg->PrepareLC( R_MS_WIZARD_INFO_DIALOG );

    // do number conversion
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aHeading );
    
    dlg->SetHeaderTextL( aHeading );
    HBufC* body = NULL;
    switch ( aStep )
        {
        case EStep1:
            {
            body = StringLoader::LoadLC( R_MS_STEP_1_BODY_TEXT );
            break;
            }
        case EStep2:
            {
            body = StringLoader::LoadLC( R_MS_STEP_2_BODY_TEXT );
            break;
            }                
        case EStep4:
            {
            body = StringLoader::LoadLC( R_MS_STEP_4_BODY_TEXT );
            break;
            }
        case EStep6:
            {
            body = StringLoader::LoadLC( R_MS_STEP_6_BODY_TEXT );
            break;
            }
        case EStep8:
            {
            body = StringLoader::LoadLC( R_MS_STEP_8_BODY_TEXT );
            break;
            }
        case EStep9:
            {
            body = StringLoader::LoadLC( R_MS_STEP_9_2_BODY_TEXT );
            break;
            }                            
        case EStep10:
            {
            body = StringLoader::LoadLC( R_MS_STEP_10_BODY_TEXT );
            break;
            }
        case EStep12:
            {
            body = StringLoader::LoadLC( R_MS_LAST_STEP_BODY_TEXT );
            break;
            }                                
        default:
            {
            LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowInfoStepL \
                    Step not found"));
            break;
            }                
        }
        
    if ( body )  // long body text
        {            
        dlg->SetMessageTextL( *body );
        CleanupStack::PopAndDestroy( body );
        }
    
    returnValue = dlg->RunLD();
    CleanupStack::Pop( dlg );
    
    return returnValue;
    }

// --------------------------------------------------------------------------
// CMSAppWizard::ShowListStepL
// Show the UI dialog with selection list
// --------------------------------------------------------------------------
//
TInt CMSAppWizard::ShowListStepL( TInt aMain, TPtr aHeading )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowListStepL begin"));
    TInt returnValue(KErrNone);

    TBool currSelection =  iSelections->AutoManualSelection();

    CDesCArrayFlat* storeTypeSelection = new (ELeave) CDesCArrayFlat(
        KArrayGranularity );
    CleanupStack::PushL( storeTypeSelection );

    HBufC* itemText1 = StringLoader::LoadLC( R_MS_AUTOMATIC_TEXT );
    HBufC* itemText2 = StringLoader::LoadLC( R_MS_MANUAL_TEXT );

    storeTypeSelection->AppendL( *itemText1 );
    storeTypeSelection->AppendL( *itemText2 );

    CleanupStack::PopAndDestroy(2, itemText1 );

    CAknListQueryDialog* dlg = new
        ( ELeave ) CAknListQueryDialog( &currSelection );

    dlg->PrepareLC( aMain );

    // do number conversion
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aHeading );

    dlg->SetHeaderTextL( aHeading );
    dlg->SetItemTextArray( storeTypeSelection );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    CListBoxView* listBoxView = dlg->ListBox()->View();
    listBoxView->SetCurrentItemIndex( currSelection );

    returnValue = dlg->RunLD();

    TRACE(
        Print( _L("MSAppWizard]\t ShowListStepL exit dlg: %d" ),
        returnValue ) );
    iSelections->SetAutoManualSelection( currSelection );

    CleanupStack::PopAndDestroy( storeTypeSelection );

    LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowListStepL ends"));
    return returnValue;
    }

// --------------------------------------------------------------------------
// CMSAppWizard::ShowMultiselectionStepL
// Show the multiselection popup dialog for setting content to share
// --------------------------------------------------------------------------
//
TInt CMSAppWizard::ShowMultiselectionStepL( CDesCArrayFlat& aItemArray,
                                            CArrayFix<TInt>& aResultArray,
                                            TPtr aHeading
                                             )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ShowMultiselectionStepL"));
    TInt returnValue(KErrNone);

    // do number conversion
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aHeading );

    iMultiSelectionDlg = CMSMultiselectionPopup::NewL(
                                &aItemArray,
                                &aResultArray,
                                aHeading );
    
    iMultiSelectionDlg->PrepareLC( R_MS_WIZARD_MULTISELECTION_DIALOG );

    // For Device selection
    if ( iStep == EStep5 || iStep == EStep9 )
        {
#ifndef __WINS__
        iMultiSelectionDlg->QueryHeading()->SetHeaderAnimationL(
                                    R_MS_ANIMATION_FOR_SELECTION_DIALOG );
#endif
        if ( iStep == EStep9 ) // no upload servers found
            {            
            HBufC* defaultText =
                        StringLoader::LoadLC( R_MS_STEP_9_2_BODY_TEXT );
            iMultiSelectionDlg->ListBox()->View()->SetListEmptyTextL(
                                                             *defaultText );
            CleanupStack::PopAndDestroy( defaultText );
            }
        }
      
    returnValue = iMultiSelectionDlg->RunLD();        
    iMultiSelectionDlg = NULL;        
    
    return returnValue;
    }

// --------------------------------------------------------------------------
// CMSAppWizard::ShowWaitNoteL
// --------------------------------------------------------------------------
//
TInt CMSAppWizard::ShowWaitNoteL()
    {
    TInt returnValue( 0 );
        
    iUserCancelledSearch = EFalse;
    iWLanLost = EFalse;
    iWaitDialog = new ( ELeave ) CAknWaitDialog(
              ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ) );
    iWaitDialog->SetCallback( this );
    iWaitDialog->ExecuteLD( R_MS_WIZARD_WAIT_DIALOG );

    return returnValue;
    }

// --------------------------------------------------------------------------
// CMSAppWizard::SetNextStep
// Calculates the next step in wizard
// --------------------------------------------------------------------------
//
void CMSAppWizard::SetNextStep( const TInt aDirection )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::SetNextStep"));

    if ( aDirection > 0 || EEikBidOk == aDirection )    
        {
        iStep++;     
        }
    else if ( iStep <= 0 )
	    {
	    iStep = ECancelWizard;
	    }
    else
    	{
        iStep--;
        }    
    }

// --------------------------------------------------------------------------
// CMSAppWizard::CancelDeviceSearch
//
// --------------------------------------------------------------------------
//
void CMSAppWizard::CancelDeviceSearch()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CancelDeviceSearch"));

    if ( iAvController )
        {
        iAvController->RemoveDeviceObserver();
		iAvController->Release();
		iAvController = NULL;
        }
    }

// --------------------------------------------------------------------------
// CMSAppWizard::CreateAvControllerL
//
// --------------------------------------------------------------------------
//
TInt CMSAppWizard::CreateAvControllerL()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CreateAvControllerL begins"));

    TInt retVal(KErrNone);


    iMediaServers.ResetAndDestroy();
    iMediaServers.Close();


    if( iDeviceNameArray )
        {
        iDeviceNameArray->Reset();
        delete iDeviceNameArray;
        iDeviceNameArray = NULL;
        iDeviceNameArray = new (ELeave) CDesCArrayFlat( KArrayGranularity );
        }
    
    if( iAvController )
        {
		iAvController->Release();
		iAvController = NULL;
        }
    
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CreateAvControllerL before"));
    TRAP( retVal, 
          iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL() );
    
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CreateAvControllerL afterr"));
    
    if( retVal != KErrNone )
        {
        TRACE( Print(
             _L("[MediaServant]\t AvController create FAILED, \
                 err = %d"), retVal)
             );
        if( retVal == KErrCancel )
            {
            //operation cancelled, return to dialog 4/8
            CloseWaitDialog();
            }
        }
    else
        {
        iAvController->SetDeviceObserver( *this );
        }

    LOG(_L("[MSAppWizard]\t CMSAppWizard::CreateAvControllerL ends"));
    return retVal;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::UpdateDevicePopupDlgL
// Updates popup data with found device
// ---------------------------------------------------------------------------
//
void CMSAppWizard::UpdateDevicePopupDlgL()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::UpdateDevicePopupDlgL begins"));

    iMultiSelectionDlg->UpdateAndDrawPopupL( iDeviceNameArray );
    LOG(_L("[MSAppWizard]\t CMSAppWizard::UpdateDevicePopupDlgL ends"));
    }


// ---------------------------------------------------------------------------
// CMSAppWizard::UPnPDeviceDiscovered
// Returns discovered device from UPnP AV control point
// ---------------------------------------------------------------------------
//
void CMSAppWizard::UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemDevices::\
             UPnPDeviceDiscovered"));

    TRAP_IGNORE( UPnPDeviceDiscoveredL(aDevice) );
    }


// ---------------------------------------------------------------------------
// CMSAppWizard::UPnPDeviceDiscoveredL
//
// ---------------------------------------------------------------------------
//
void CMSAppWizard::UPnPDeviceDiscoveredL( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::DeviceDiscoveredL begins"));

    CUpnpAVDevice* device = const_cast<CUpnpAVDevice*> ( &aDevice );

    if( device->DeviceType() == CUpnpAVDevice::EMediaServer &&
        device->SearchCapability() &&
        device->DlnaCompatible() )
        {
        LOG(_L("[MSAppWizard]\t Found device is Media server!"));
        CCmMediaServerFull* tempServer = CCmMediaServerFull::NewL();
        CleanupStack::PushL( tempServer );
        tempServer->SetUDNL( device->Uuid() );
        tempServer->SetMediaServerNameL( device->FriendlyName() );
        tempServer->SetSystemUpdateID( -1 );      
        tempServer->SetCopyCapability( aDevice.CopyCapability() );
            
        TTime timeStamp;
        timeStamp.HomeTime();
        tempServer->SetVisibleDate( timeStamp );
        iMediaServers.AppendL( tempServer );
        
        if ( iStep == EStep9 )
        	{
	        // Add only store servers in step EStep9
        	if ( tempServer->CopyCapability() )
        		{
        		iStoreServers.AppendL( tempServer );
        		AddDeviceNameL( aDevice );
        		}
        	}
        else
        	{
        	AddDeviceNameL( aDevice );
        	}

        CleanupStack::Pop( tempServer );
        }
    else
        {
        LOG(_L("[MSAppWizard]\t Found device is NOT Dlna compliant \
        Media server!"));
        return;
        }

    if ( iWaitDialog )
        {
        CloseWaitDialog();
        }
    // Ensure that we are in device selection step before adding items
    // to the dialog
    else if( iMultiSelectionDlg  && ( iStep == EStep5 || iStep == EStep9 ))
        {
        //update device list for UI
        UpdateDevicePopupDlgL();
        }

    LOG(_L("[MSAppWizard]\t CMSAppWizard::DeviceDiscoveredL ends"));

    }

// ---------------------------------------------------------------------------
// CMSAppWizard::UPnPDeviceDisappeared
// Returns disappeared device from UPnP AV control point.
// ---------------------------------------------------------------------------
//
void CMSAppWizard::UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemDevices::\
             UPnPDeviceDisappeared"));

    TRAP_IGNORE( UPnPDeviceDisappearedL( aDevice ) );
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::UPnPDeviceDisappearedL
//
// ---------------------------------------------------------------------------
//
void CMSAppWizard::UPnPDeviceDisappearedL( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::DeviceDisappearedL begins"));
    TPtrC8 ptr( aDevice.Uuid() );
    TInt serverIndex(0);

    if (!ptr.Compare(_L8("ConnectionLost")))
        {
        LOG(_L("[MSAppWizard]\t CMSAppWizard::\
             UPnPDeviceDisappearedL: Connection lost!"));
        }

    TPtrC8 serverUid = aDevice.Uuid();

    if( MatchFoundServer( serverUid, serverIndex ) == KErrNone )
        {
        // remove server and update screen
        LOG(_L("[MSAppWizard]\t CMSAppWizard::DeviceDisappearedL, MATCH"));
        delete (iMediaServers)[serverIndex];
        (iMediaServers)[serverIndex] = NULL;
        iMediaServers.Remove( serverIndex );
        iDeviceNameArray->Delete( serverIndex );
        
        if( iMultiSelectionDlg  && ( iStep == EStep5 || iStep == EStep9 ) )
        	{
        	UpdateDevicePopupDlgL();
        	}
        }

    LOG(_L("[MSAppWizard]\t CMSAppWizard::DeviceDisappearedL ends"));
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::WLANConnectionLost
// Called when wlan connection is lost
// ---------------------------------------------------------------------------
//
void CMSAppWizard::WLANConnectionLost()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::WLANConnectionLost"));
    iWLanLost = ETrue;
    TRAP_IGNORE( DialogDismissedL( EAknSoftkeyCancel ) );
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::MatchFoundServer
// returns KErrNone if match found
// ---------------------------------------------------------------------------
//
TInt CMSAppWizard::MatchFoundServer( TDesC8& aUid, TInt &aServerIndex )
    {
    TInt retVal( KErrNotFound );
    TInt devicecount = iMediaServers.Count();

    for( TInt i=0; i<devicecount; i++ )
        {

        retVal = (iMediaServers)[i]->MediaServer().Compare( aUid );

        if ( retVal == KErrNone )
            {
            aServerIndex = i;
            i = devicecount; // break from the for loop
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::CheckAndSetServerStateL
// sets remove state to server if it is old one
// also update the server name
// ---------------------------------------------------------------------------
//
void CMSAppWizard::CheckAndSetServerStateL()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CheckAndSetServerStateL"));

    TKeyArrayFix sortKey( 0, ECmpNormal );

    // set remove status to all original servers
    for ( TInt ind = 0; ind < iOriginalServers->Count(); ind++ )
        {
        CCmMediaServerFull* tempServer = (*iOriginalServers)[ind];
        tempServer->SetIsActive( KServerRemoved );
        tempServer->SetStoreUsage( EFalse );
        tempServer->SetFillUsage( EFalse );
        }

    // Change found servers statuses
    for ( TInt i = 0; i < iMediaServers.Count(); i++ )
        {
        CCmMediaServerFull* newServer = iMediaServers[i];
        TBool found( EFalse );
        TInt iterations = iOriginalServers->Count();
        for ( TInt j = 0; j < iterations; j++ )
            {
            CCmMediaServerFull* origServer = (*iOriginalServers)[j];
                {
                if ( origServer->MediaServer() == newServer->MediaServer())
                    {
                    origServer->SetIsActive( newServer->IsActive() );
                    origServer->SetStoreUsage( newServer->StoreUsage() );
                    origServer->SetFillUsage( newServer->FillUsage() );
                    found = ETrue;
                    j = iterations; // break loop
                    //update the server name
                    origServer->SetMediaServerNameL(
                        newServer->MediaServerName() );
                    }
                }
            }
        if ( !found )
            {            
            // append new server to the server list
            iOriginalServers->Append( newServer );
            
            iMediaServers.Remove( i );
            // reduce index because of the server remove to
            // check all items
            i--;
            }
        }

    }

// ---------------------------------------------------------------------------
// CMSAppWizard::ClearServersSystemUpdateId
// clears system update ids of unselected servers
// ---------------------------------------------------------------------------
//
void CMSAppWizard::ClearServersSystemUpdateId()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::ClearServersSystemUpdateId"));

    for ( TInt ind = 0; ind < iOriginalServers->Count(); ind++ )
        {
        CCmMediaServerFull* server = (*iOriginalServers)[ind];
        if ( !server->IsActive() )
            {
            server->SetSystemUpdateID( -1 );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::FetchAlreadyDiscoveredDevicesL
// Fetches devices from AVController and calls
// DeviceDiscovered-callback for each of them.
// ---------------------------------------------------------------------------
//
void CMSAppWizard::FetchAlreadyDiscoveredDevicesL()
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::FetchAlreadyDiscoveredDevicesL") );

    // If iAvController is not created, this method is unusable. 
    // Call CreateAvControllerL first.
    User::LeaveIfNull( iAvController );

    // Fetch already found devices from AVController.
    CUpnpAVDeviceList* discoveredDevices =
        iAvController->GetMediaServersL();
    CleanupStack::PushL( discoveredDevices );

    if ( discoveredDevices && discoveredDevices->Count() > 0 )
        {
        // the AVController already has some devices.
        LOG(_L("[MSAppWizard]\t CMSAppWizard::FetchAlreadyDiscoveredDevicesL:\
                AVController has already discovered devices"));

        // process the already existing devices
        for ( TInt i = 0 ; i < discoveredDevices->Count() ; i++ )
            {
            UPnPDeviceDiscovered(
                *( discoveredDevices->operator[](i) ) );
            }
        }

    // clean up
    CleanupStack::PopAndDestroy( discoveredDevices );
    discoveredDevices = NULL;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::CloseWaitDialog
// Close wait dialog
// ---------------------------------------------------------------------------
void CMSAppWizard::CloseWaitDialog()
    {
    LOG( _L("[MSAppWizard]\t CMSAppWizard::CloseWaitDialog") );
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CMSAppWizard::CreateMultiselectionFillListItemsLC
// ---------------------------------------------------------------------------    
CDesCArrayFlat* CMSAppWizard::CreateMultiselectionFillListItemsLC()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard:: \
            CreateMultiselectionFillListItemsLC"));
    
    CDesCArrayFlat* itemArray = new (ELeave) CDesCArrayFlat(
                                                KFillListGranularity );
    CleanupStack::PushL( itemArray );                                                     
    
    // Read fill lists from engine
    CCmFillRuleContainer* listContainer = iMSEngine->FillRulesL();

    for ( TInt index = 0; index < listContainer->FillRuleCount(); index++ )
        {
        CCmFillRule* rule = listContainer->FillRule( index );
        HBufC* itemText = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                               rule->Name() );
        CleanupStack::PushL( itemText );
        
        itemArray->AppendL( *itemText );

        CleanupStack::PopAndDestroy( itemText );        
        }   

    delete listContainer;
    listContainer = NULL;

    return itemArray;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::CreateMultiselectionStoreListItemsLC
// ---------------------------------------------------------------------------    
CDesCArrayFlat* CMSAppWizard::CreateMultiselectionStoreListItemsLC()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard:: \
            CreateMultiselectionStoreListItemsLC"));
    
    CDesCArrayFlat* itemArray = new (ELeave) CDesCArrayFlat(
                                                    KFillListGranularity );
    CleanupStack::PushL( itemArray );                                                    
    
    // Read store lists from engine
    CCmStoreRuleContainer* listContainer = iMSEngine->StoreRulesL();

    for ( TInt index = 0; index < listContainer->StoreRuleCount(); index++ )
        {
        CCmStoreRule* rule = listContainer->StoreRule( index );
        HBufC* itemText = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                            rule->Name() );
        CleanupStack::PushL( itemText );

        itemArray->AppendL( *itemText );

        CleanupStack::PopAndDestroy( itemText );        
        }   

    delete listContainer;
    listContainer = NULL;

    return itemArray;
    }


// ---------------------------------------------------------------------------
// CMSAppWizard::CreateMultiselectionUIStoreListStringLC
// ---------------------------------------------------------------------------    
CDesCArrayFlat* CMSAppWizard::CreateMultiselectionUIStoreListStringLC()
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard:: \
            CreateMultiselectionUIStoreListStringLC"));
    
    CDesCArrayFlat* itemArray = new (ELeave) CDesCArrayFlat(
    		                                        KStoreListGranularity );
    CleanupStack::PushL( itemArray );
   
    HBufC* capturedPic = StringLoader::LoadLC( R_MS_WIZARD_CAPTURED_IMAGES );
    HBufC* otherPic = StringLoader::LoadLC( R_MS_WIZARD_OTHER_IMAGES );
    HBufC* capturedVideo = StringLoader::LoadLC( R_MS_WIZARD_CAPTURED_VIDEOS );
    HBufC* otherVideo = StringLoader::LoadLC( R_MS_WIZARD_OTHER_VIDEOS );
    HBufC* music = StringLoader::LoadLC( R_MS_WIZARD_MUSIC );
    
    itemArray->AppendL( *capturedPic );
    itemArray->AppendL( *otherPic );
    itemArray->AppendL( *capturedVideo );
    itemArray->AppendL( *otherVideo );
    itemArray->AppendL( *music );
    
    CleanupStack::PopAndDestroy( 5 );   

    return itemArray;
    }

// ---------------------------------------------------------------------------
// CMSAppWizard::CreateServerNameListL
// ---------------------------------------------------------------------------    
void CMSAppWizard::CreateServerNameListL(
                            RPointerArray<CCmMediaServerFull>& aServerArray )
    {
    LOG(_L("[MSAppWizard]\t CMSAppWizard::CreateServerNameListL"));

    // reset previous names
    iDeviceNameArray->Reset();

    for ( TInt i = 0; i < aServerArray.Count(); i++ )            
        {
        CCmMediaServerFull* server = aServerArray[i];
        HBufC* name = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                server->MediaServerName() );
        CleanupStack::PushL( name );                                                
        iDeviceNameArray->AppendL( *name );
        CleanupStack::PopAndDestroy( name );
        }
    }   
    
// End of File
