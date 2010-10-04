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
* Description:      Multiselection popup implementation
*
*/






// INCLUDE FILES
#include <StringLoader.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <eiklbv.h> //clistboxview
#include <aknlists.h>
#include <AknsUtils.h>
#include <eikcmbut.h>
#include <badesca.h>

#include "upnpappmultiselectionpopup.h"
#include "upnpmultiselectionutility.h"
#include "upnpfilesharingengine.h"

#include "cupnpmultiselectionui.hrh"
#include <cupnpmultiselectionui.rsg>

_LIT( KComponentLogfile, "multiselectionui.txt");
#include "upnplog.h"


//CONSTANTS
const TInt KOkSoftkeyCode = 63585;

//FORWARD DECLARATIONS
class CUPnPAppMultiselectionDialog;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionPopup* CUPnPAppMultiselectionPopup::NewL(
    THomeConnectMediaType aMediaType,
    CUPnPFileSharingEngine* aFileSharingEngine,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    const HBufC* aHeading )
    {
    __LOG( "CUPnPAppMultiselectionPopup::NewL begin" );

    CUPnPAppMultiselectionPopup* self =
                   new(ELeave) CUPnPAppMultiselectionPopup( NULL, aHeading );
    CleanupStack::PushL( self);

    self->ConstructL( aMediaType, aFileSharingEngine,aSelectionIndexArray );
	
    CleanupStack::Pop( self);
    
    __LOG("CUPnPAppMultiselectionPopup::NewL end");
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionPopup::ConstructL(
    THomeConnectMediaType aMediaType,
    CUPnPFileSharingEngine* aFileSharingEngine,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray )
    {
    __LOG("CUPnPAppMultiselectionPopup::ConstructL begin");

    iMediaType = aMediaType;
    iFileSharingEngine = aFileSharingEngine;
    iPreviousSelection = aSelectionIndexArray;
    iUiMarked = new(ELeave) CArrayFixFlat<TInt>(3);
    iMarkedItems = new(ELeave) CArrayFixFlat<TInt>(3);
    iUtility = new (ELeave) CUpnpMultiselectionUtility();
    iMskSelectText = 
        StringLoader::LoadL( R_UPNP_MULTISELECTION_SELECT_TEXT );
    iMskUnselectText = 
        StringLoader::LoadL( R_UPNP_MULTISELECTION_UNSELECT_TEXT );
    
    __LOG("CUPnPAppMultiselectionPopup::ConstructL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::CUPnPAppMultiselectionPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionPopup::CUPnPAppMultiselectionPopup(
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray, 
    const HBufC* aHeading )
    : CAknListQueryDialog( aSelectionIndexArray )

    {
    iHeading = aHeading;
    __LOG("CUPnPAppMultiselectionPopup::CUPnPAppMultiselectionPopup");
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::~CUPnPAppMultiselectionPopup
// C++ default destructor.
// --------------------------------------------------------------------------
//
CUPnPAppMultiselectionPopup::~CUPnPAppMultiselectionPopup()
    {
    __LOG("CUPnPAppMultiselectionPopup::~CUPnPAppMultiselectionPopup");

    iFileSharingEngine = NULL;

    delete iUiMarked;
    delete iMarkedItems;
    delete iUtility;
    delete iMskSelectText;
    delete iMskUnselectText;

    __LOG("CUPnPAppMultiselectionPopup::~CUPnPAppMultiselectionPopup end");
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
//
TBool CUPnPAppMultiselectionPopup::OkToExitL( TInt aButtonId )
    {
    __LOG("CUPnPAppMultiselectionPopup::OkToExitL begin");

    if ( aButtonId == EAknSoftkeyOk )
        {
        __LOG("CUPnPAppMultiselectionPopup::OkToExitL: OK");

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
            
        if ( iPreviousSelection )
            {
            iPreviousSelection->Reset();
            for ( TInt i = 0; i < selected->Count(); i++ )
                {
                TInt tmp = selected->At( i );
                iPreviousSelection->AppendL( tmp );
                }
            iUtility->ConvertSelectionsForEngineL( *iPreviousSelection );
            }
        }


    __LOG("CUPnPAppMultiselectionPopup::OkToExitL end");
    return ETrue;
    }


// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionPopup::PreLayoutDynInitL()
    {
    __LOG("CUPnPAppMultiselectionPopup::PreLayoutDynInitL begin");

    //mark selected albums

    iListBox = ListBox();
    iModel = STATIC_CAST(
        CAknFilteredTextListBoxModel*, iListBox->Model());

    iSettingsTextArray = static_cast<CDesCArray*>(
        iModel->ItemTextArray());

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


    CAknListQueryDialog::SetIconArrayL(icons);

    CleanupStack::Pop(icons);
    
    SetHeaderTextL( *iHeading );
    
    ListBox()->ActivateL();

    CAknListQueryDialog::PreLayoutDynInitL();

    __LOG("CUPnPAppMultiselectionPopup::PreLayoutDynInitL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::PostLayoutDynInitL()
// Called by framework before dialog is shown.
// Needed because filter is not active until PreLayoutDynInitL().
// --------------------------------------------------------------------------
//
void  CUPnPAppMultiselectionPopup::PostLayoutDynInitL()
    {
    __LOG("CUPnPAppMultiselectionPopup::PostLayoutDynInitL begin");

    // Get state of do not share and share all selections and set them 
    // to utility.
    TShareSelectionState selectionState = 
        iUtility->ShareSelectionStateFromArray( *iMarkedItems );
    iUtility->SetShareSelectionState( selectionState );
        
    TBool useMarked = EFalse;
    //mark selected albums
    if ( iMarkedItems && iMarkedItems->Count() )
        {
        if( iMarkedItems->At(0) != 0 )
            {
            __LOG("CUPnPAppMultiselectionPopup::PostLayoutDynInitL using iMarkedItems");
            // if got selections stored in content server use that
            iListBox->SetSelectionIndexesL( iMarkedItems );
            useMarked = ETrue;
            }
        }
    if( !useMarked && iUiMarked->Count() )
        {
        __LOG("CUPnPAppMultiselectionPopup::PostLayoutDynInitL using iUiMarked");
        iListBox->SetSelectionIndexesL( iUiMarked );
        }

    iUtility->CollectSelectedItemsL( iListBox, ETrue );

    //set msk's label depending on is item selected or not
    HBufC* mskLabel( iMskSelectText );
    if( iListBox->View()->ItemIsSelected( iListBox->CurrentItemIndex() ) )
        {
        mskLabel = iMskUnselectText;
        }
    ButtonGroupContainer().SetCommandL( 
        EAknSoftkeyForwardKeyEvent, *mskLabel );

    __LOG("CUPnPAppMultiselectionPopup::PostLayoutDynInitL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::OfferKeyEventL();
// called by framework when key is pressed
// --------------------------------------------------------------------------
//
TKeyResponse CUPnPAppMultiselectionPopup::OfferKeyEventL (
            const TKeyEvent &aKeyEvent,
            TEventCode aType)
    {
    __LOG("CUPnPAppMultiselectionPopup::OfferKeyEventL begin");

    TKeyResponse status( EKeyWasNotConsumed );
    if ( aType == EEventKey ) // Is not key event
        {
        if (aKeyEvent.iCode == EKeyOK)
            {
            status = ListBox()->OfferKeyEventL( aKeyEvent, aType );
            iUtility->CollectSelectedItemsL( iListBox );
            }
        else if (aKeyEvent.iCode == KOkSoftkeyCode)
            {
            // disable draw
            status = CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
            }
        else
            {
            // Let the Akn dialog handle the key event.
            // EKeyEscape key event deletes this dialog.
            status = CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
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
        ButtonGroupContainer().SetCommandL( 
            EAknSoftkeyForwardKeyEvent, *mskLabel );
        ButtonGroupContainer().DrawDeferred();
        }

    __LOG("CUPnPAppMultiselectionPopup::OfferKeyEventL end");
    return status;
    }



// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::StepL
// Step done during wait note
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionPopup::StepL(void)
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
// CUPnPAppMultiselectionPopup::IsProcessDone
// Returns ETrue if process finished
// --------------------------------------------------------------------------
//
TBool CUPnPAppMultiselectionPopup::IsProcessDone(void) const
    {
    return iDone;
    }

// --------------------------------------------------------------------------
// CUPnPAppMultiselectionPopup::SetMarkedItemsL
// --------------------------------------------------------------------------
//
void CUPnPAppMultiselectionPopup::SetMarkedItemsL(CArrayFix<TInt>& aMarkedItems)
    {
    iUiMarked->Reset();
    for( TInt i=0 ; i < aMarkedItems.Count() ; ++i )
        {
        iUiMarked->AppendL( aMarkedItems.At(i) );
        }
    }

// End of file
