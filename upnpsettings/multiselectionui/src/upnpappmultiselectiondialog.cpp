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
* Description:      Multiselection dialog implementation
*
*/






// INCLUDE FILES
#include <StringLoader.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <aknlists.h>
#include <AknsUtils.h>
#include <eikcmbut.h>
#include "upnpappmultiselectiondialog.h"
#include "upnpfilesharingengine.h"

#include "cupnpmultiselectionui.hrh"
#include <cupnpmultiselectionui.rsg>

_LIT( KComponentLogfile, "multiselectionui.txt");
#include "upnplog.h"


//CONSTANTS
const TInt KOkSoftkeyCode = 63585;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionDialog* CUPnPAppMultiselectionDialog::NewL(
                        THomeConnectMediaType aMediaType,
                        CUPnPFileSharingEngine* aFileSharingEngine,
                        CArrayFix<TInt>* aSelectionIndexArray )
    {
    __LOG("CUPnPAppMultiselectionDialog::NewL begin");

    CUPnPAppMultiselectionDialog* self =
                        new(ELeave) CUPnPAppMultiselectionDialog();

    CleanupStack::PushL(self);
    self->ConstructL( aMediaType, aFileSharingEngine, aSelectionIndexArray );
    CleanupStack::Pop(self);
    __LOG("CUPnPAppMultiselectionDialog::NewL end");
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionDialog::ConstructL(
                        THomeConnectMediaType aMediaType,
                        CUPnPFileSharingEngine* aFileSharingEngine,
                        CArrayFix<TInt>* aSelectionIndexArray )
    {
    __LOG("CUPnPAppMultiselectionDialog::ConstructL begin");
    CAknSelectionListDialog::ConstructL( R_AVKON_SOFTKEYS_EMPTY );
    iMediaType = aMediaType;
    iFileSharingEngine = aFileSharingEngine;

    iMarkedItems = aSelectionIndexArray;
    iUtility = new (ELeave) CUpnpMultiselectionUtility();

    iMskSelectText = 
        StringLoader::LoadL( R_UPNP_MULTISELECTION_SELECT_TEXT );
    iMskUnselectText = 
        StringLoader::LoadL( R_UPNP_MULTISELECTION_UNSELECT_TEXT );

    __LOG("CUPnPAppMultiselectionDialog::ConstructL end");
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::CUPnPAppMultiselectionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionDialog::CUPnPAppMultiselectionDialog()
: CAknSelectionListDialog(iSelectedItem, NULL, NULL), iSelectedItem(0)
    {
    __LOG("CUPnPAppMultiselectionDialog::CUPnPAppMultiselectionDialog");
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::CUPnPAppMultiselectionDialog
// C++ default destructor.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionDialog::~CUPnPAppMultiselectionDialog()
    {
    __LOG("CUPnPAppMultiselectionDialog::~CUPnPAppMultiselectionDialog");

    iFileSharingEngine = NULL;

    delete iUtility;
    delete iMskSelectText;
    delete iMskUnselectText;

    __LOG("CUPnPAppMultiselectionDialog::~CUPnPAppMultiselectionDialog end");
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
//
TBool CUPnPAppMultiselectionDialog::OkToExitL(TInt aButtonId)
    {
    __LOG("CUPnPAppMultiselectionDialog::OkToExitL begin");
    TBool ret( EFalse );
    if ( aButtonId == EAknSoftkeyOk )
        {
        TBool showWaitNote( ETrue );

        const CArrayFix<TInt>* selected;

        //get the true indexes of marked items
        CAknListBoxFilterItems* filter 
                = static_cast < CAknFilteredTextListBoxModel* >     
                                ( iListBox->Model() )->Filter();
        if ( filter )
            {
            // Filter knows all.
            filter->UpdateSelectionIndexesL();
            selected = filter->SelectionIndexes();
            }
        else
            {
            // No filter.
            selected = ListBox()->View()->SelectionIndexes();
            }
            
        // Show wait note if needed.
        CAknWaitNoteWrapper* waitNoteWrapper( NULL );
        if ( selected->Count() == 1 && selected->At( 0 ) == 0 )
            {
            showWaitNote = EFalse;
            }
        if ( showWaitNote )
            {
            waitNoteWrapper = CAknWaitNoteWrapper::NewL();
            CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
            waitNoteWrapper->ExecuteL(
                R_UPNP_MULTISELECTION_COLLECTING_FILES_NOTE,      // TInt aResId,
                *this,            // MAknBackgroundProcess& aBackgroundProcess
                ETrue);
            }

        // give the selection indexes to filesharing engine
        iMarkedItems->Reset();
        for( TInt i(0); i<selected->Count(); i++ )
            {
            iMarkedItems->AppendL( selected->At( i ) );
            }
        iUtility->ConvertSelectionsForEngineL( *iMarkedItems );

        if ( showWaitNote )
            {
            CleanupStack::PopAndDestroy(waitNoteWrapper);
            }
        ret = ETrue;
        }

   else if ( aButtonId == EAknSoftkeySelect )
       {
       // msk event    
       __LOG("CUPnPAppMultiselectionDialog::msk event");
       TKeyEvent tmpEvent;
       tmpEvent.iCode = EKeyOK;
       tmpEvent.iModifiers = 0;
       tmpEvent.iRepeats = 0;
       tmpEvent.iScanCode = 0;
       TEventCode eventType = EEventKey;
       OfferKeyEventL( tmpEvent, eventType );
       
       }
   else if ( aButtonId == EAknSoftkeyBack )
        {
        ret = ETrue;
        }

    __LOG("CUPnPAppMultiselectionDialog::OkToExitL end");
    return ret;
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionDialog::PreLayoutDynInitL()
    {
    __LOG("CUPnPAppMultiselectionDialog::PreLayoutDynInitL begin");

    iListBox = static_cast<CAknSingleGraphicStyleListBox*>
        (Control(EMultiSelectionListBoxId));

    // display find box
    SetupFind( CAknSelectionListDialog::EAdaptiveFind );

    // get pointer to listbox data array
    iModel = STATIC_CAST(
        CAknFilteredTextListBoxModel*, iListBox->Model() );
    iSettingsTextArray = static_cast<CDesCArray*>(
        iModel->ItemTextArray() );

    iUtility->AppendShareAllSelectionL( iSettingsTextArray );

 
    iFileSharingEngine->RequestSelectionContentL( iMediaType );
    iState = ELoadContent;

    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
    waitNoteWrapper->ExecuteL(
        R_UPNP_MULTISELECTION_SEARCHING_FILES_NOTE,      // TInt aResId,
        *this,           // MAknBackgroundProcess& aBackgroundProcess
        EFalse);   // ETrue = Show immediately

    CleanupStack::PopAndDestroy(waitNoteWrapper);

    CAknIconArray* icons = new (ELeave) CAknIconArray(2);
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );

    iUtility->AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOn,
                        EMbmAvkonQgn_prop_checkbox_on,
                        EMbmAvkonQgn_prop_checkbox_on_mask );

    iUtility->AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOff,
                        EMbmAvkonQgn_prop_checkbox_off,
                        EMbmAvkonQgn_prop_checkbox_off_mask );


    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );

    iListBox->ActivateL();


    // set dialog title
    CEikStatusPane* statusPane =
                    (( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
    iTitlePane = (CAknTitlePane*)statusPane->ControlL(
                    TUid::Uid(EEikStatusPaneUidTitle));

    if ( iMediaType == EImageAndVideo )
        {
        iTitlePane->SetTextL( *StringLoader::LoadLC(
                                R_UPNP_MULTISELECTION_SHARING_VISUAL_TITLE_TEXT) );
        }
    else
        {
        iTitlePane->SetTextL( *StringLoader::LoadLC(
                                R_UPNP_MULTISELECTION_SHARING_MUSIC_TITLE_TEXT) );
        }
    CleanupStack::PopAndDestroy();

    CAknSelectionListDialog::PreLayoutDynInitL();
    __LOG("CUPnPAppMultiselectionDialog::PreLayoutDynInitL end");
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::PostLayoutDynInitL()
// Called by framework before dialog is shown.
// Needed because filter is not active until PreLayoutDynInitL().
// --------------------------------------------------------------------------
//
void  CUPnPAppMultiselectionDialog::PostLayoutDynInitL()
    {
    __LOG("CUPnPAppMultiselectionDialog::PostLayoutDynInitL begin");

    // Get state of do not share and share all selections and set them 
    // to utility.
    TShareSelectionState selectionState = 
        iUtility->ShareSelectionStateFromArray( *iMarkedItems );
    iUtility->SetShareSelectionState( selectionState );
    
    // filter out the first character of the search string
    FindBox()->SetListColumnFilterFlags(0xFFFFFFFE);
    
    //mark selected albums
    if ( iMarkedItems )
        {
        if ( iMarkedItems->Count() )
            {
            //
            iListBox->SetSelectionIndexesL( iMarkedItems );
            }
        }
    iUtility->CollectSelectedItemsL( iListBox, ETrue );

    //set msk's label depending on is item selected or not
    HBufC* mskLabel( iMskSelectText );
    if( iListBox->View()->ItemIsSelected( iListBox->CurrentItemIndex() ) )
        {
        mskLabel = iMskUnselectText;
        }
    ButtonGroupContainer().SetCommandL( EAknSoftkeySelect, *mskLabel );

    __LOG("CUPnPAppMultiselectionDialog::PostLayoutDynInitL end");
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::OfferKeyEventL();
// called by framework when key is pressed
// --------------------------------------------------------------------------
//
TKeyResponse CUPnPAppMultiselectionDialog::OfferKeyEventL(
            const TKeyEvent &aKeyEvent,
            TEventCode aType)
    {
    __LOG("CUPnPAppMultiselectionDialog::OfferKeyEventL begin"); 
    
    TKeyResponse status( EKeyWasNotConsumed );
    if ( aType == EEventKey ) // Is not key event
        {
        if (aKeyEvent.iCode == EKeyOK)
            {
            status = iListBox->OfferKeyEventL( aKeyEvent, aType );
            iUtility->CollectSelectedItemsL( iListBox );
            }
        else if (aKeyEvent.iCode == KOkSoftkeyCode)
            {
            // disable draw
            iListBox->View()->SetDisableRedraw(ETrue);
            status = CAknSelectionListDialog::OfferKeyEventL(aKeyEvent,aType);
            }
        else
            {
            // Let the Akn dialog handle the key event.
            // EKeyEscape key event deletes this dialog.
            status = CAknSelectionListDialog::OfferKeyEventL(aKeyEvent,aType);
            }
        }

    //
    // Set msk's label depending on is item selected or not.
    //
    // In case of EKeyEscape we cannot access objects in heap any more.
    // Only local variables in stack. This is because CAknSelectionListDialog
    // ::OfferKeyEventL deletes this dialog. When call returns heap memory is cleared
    // and we cannot rely on values of member variables.
    //
    if ( status == EKeyWasConsumed &&
         aKeyEvent.iCode != EKeyEscape )
        {
        HBufC* mskLabel(iMskSelectText);
        if( iListBox->View()->ItemIsSelected( 
                iListBox->CurrentItemIndex() ) )
            {
            mskLabel = iMskUnselectText;
            }
        ButtonGroupContainer().SetCommandL( EAknSoftkeySelect, *mskLabel );
        ButtonGroupContainer().DrawDeferred();
        }

    __LOG("CUPnPAppMultiselectionDialog::OfferKeyEventL end");
    return status;
    }

    
// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::StepL
// Step done during wait note
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionDialog::StepL(void)
    {
    switch ( iState )
        {
        case ELoadContent:
            {
            TRAPD( err, iFileSharingEngine->GetSelectionContentL( 
                *iSettingsTextArray ) );
            if ( err == KErrNone )
                {
                iState = ELoadIndexes;
                iFileSharingEngine->RequestSelectionIndexesL( iMediaType );
                }
            else if ( err == KErrInUse )
                {
                //do nothing, request is being proceeded
                }
            else
                {
                User::Leave( err );
                }
            break;
            }
        case ELoadIndexes:
            {
            TRAPD( err, iFileSharingEngine->GetSelectionIndexesL( 
                *iMarkedItems ) );
            if ( err == KErrNone )
                {
                iState = ELoadDone;
                iDone = ETrue;
                }
            else if ( err == KErrInUse )
                {
                //do nothing, request is being proceeded
                }
            else
                {
                User::Leave( err );
                }
            break;
            }
        case ELoadDone:
            {
            break;
            }
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            break;
            }
        }
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionDialog::IsProcessDone
// Returns ETrue if process finished
// --------------------------------------------------------------------------
//
TBool CUPnPAppMultiselectionDialog::IsProcessDone(void) const
    {
    return iDone;
    }

// End of file
