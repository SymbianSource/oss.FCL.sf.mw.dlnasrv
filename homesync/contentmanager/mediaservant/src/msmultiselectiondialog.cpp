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
* Description:  CMSMultiselectionDialog class implementation
*
*/



// INCLUDE FILES
#include <akntitle.h>
#include <avkon.mbg>
#include <aknlists.h>
#include <akncheckboxsettingpage.h>
#include <mediaservant.rsg>

#include "msmultiselectiondialog.h"
#include "mediaservant.hrh"
#include "msconstants.h"
#include "msdebug.h"


//CONSTANTS

// Reserved list items at a time
const TInt KListGranularity = 10;
// Used for item icon definition
_LIT(KOneSlashT, "1\t");


// --------------------------------------------------------------------------
// CMSMultiselectionDialog::NewL
// --------------------------------------------------------------------------
//
CMSMultiselectionDialog* CMSMultiselectionDialog::NewL(
                                            CSelectionItemList* aItemArray,
                                            const TDesC& aTitle )
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::NewL"));

    CMSMultiselectionDialog* self =
                    CMSMultiselectionDialog::NewLC( aItemArray, aTitle );                        
    CleanupStack::Pop(self);

    return self;
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::NewLC
// --------------------------------------------------------------------------
//
CMSMultiselectionDialog* CMSMultiselectionDialog::NewLC(
                                            CSelectionItemList* aItemArray,
                                            const TDesC& aTitle )
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::NewLC"));

    CMSMultiselectionDialog* self =
                        new(ELeave) CMSMultiselectionDialog( aItemArray );

    CleanupStack::PushL(self);
    self->ConstructL( aTitle );

    return self;
    }
    
// --------------------------------------------------------------------------
// CMSMultiselectionDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::ConstructL( const TDesC& aTitle )
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    ConstructL"));

    // base class call
    // menubar id has no effect because options menu is not used
    CAknSelectionListDialog::ConstructL( R_MS_MENUBAR_MAIN_VIEW );
    
    // keep safe previous view title
    iPreviousTitle = HBufC::NewL( KMaxFileName );

    // set dialog title
    CEikStatusPane* statusPane =
                    (( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
    iTitlePane = (CAknTitlePane*)statusPane->ControlL(
                    TUid::Uid(EEikStatusPaneUidTitle));
    // keep previous title safe
    iPreviousTitle->Des().Copy( *iTitlePane->Text() );
    // set new title
    iTitlePane->SetTextL(aTitle);
    }


// --------------------------------------------------------------------------
// CMSMultiselectionDialog::CMSMultiselectionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CMSMultiselectionDialog::CMSMultiselectionDialog(
                                CSelectionItemList* aItemArray)
: CAknSelectionListDialog(iSelectedItem, NULL, NULL),
  iItemArray (aItemArray)
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog:: \
    CMSMultiselectionDialog"));
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::CMSMultiselectionDialog
// C++ default destructor.
// --------------------------------------------------------------------------
//
CMSMultiselectionDialog::~CMSMultiselectionDialog()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog:: \
    ~CMSMultiselectionDialog"));

    // set previous title back
    if( iTitlePane )
        {
        TInt error = KErrNone;
        TRAP( error, iTitlePane->SetTextL( *iPreviousTitle ) );
        if( error )
            {
            TRACE( Print(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog\
                  SetTextL error %d\n"),error ) );
            }
        }
    
    delete iPreviousTitle;

    delete iInternalItemArray;

    delete iNaviDecorator;
    }


// --------------------------------------------------------------------------
// CMSMultiselectionDialog::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
//
TBool CMSMultiselectionDialog::OkToExitL(TInt aButtonId)
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    OkToExitL"));

    TBool retval = ETrue;
    
    // When using middle softkey list update must be called from
    // this way because dialog doesn't receive middle softkey key
    // event.
    if ( aButtonId == EAknSoftkeyChange )
        {
        HandleSelectionKeyPressL();
        // don't exit dialog
        retval = EFalse;
        }

    if ( retval ) 
        {
        // Update item selections to the list
        UpdateAllSelectionsL();

        // Clear navi pane text
        iNaviPane->Pop( iNaviDecorator );

        if ( aButtonId == EAknSoftkeyBack )
            {
            iAvkonAppUi->ProcessCommandL( EEikCmdExit );
            }
        }

    return retval;
    }


// --------------------------------------------------------------------------
// CMSMultiselectionDialog::PreLayoutDynInitL()
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::PreLayoutDynInitL()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog:: \
    PreLayoutDynInitL"));

    iListBox = static_cast<CAknSingleGraphicStyleListBox*>
                            (Control(EMultiSelectionListBoxId));

    // display find box
    SetupFind(CAknSelectionListDialog::EFixedFind);

    CAknIconArray* icons = new (ELeave) CAknIconArray(2); //magic: icon count
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName() );

    AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOn,
                        EMbmAvkonQgn_indi_checkbox_on,
                        EMbmAvkonQgn_indi_checkbox_on_mask );

    AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOff,
                        EMbmAvkonQgn_indi_checkbox_off,
                        EMbmAvkonQgn_indi_checkbox_off_mask );


    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
    CleanupStack::Pop(icons);

    GenerateInternalArrayL();

    CEikStatusPane* statusPane=iEikonEnv->AppUiFactory()->StatusPane();

    // Set empty navi pane text
    iNaviPane = static_cast<CAknNavigationControlContainer*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
                // ownership of decorator is transfered to application
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( _L("") );
    iNaviPane->PushL( *iNaviDecorator );

    iListBox->ActivateL();
    CAknSelectionListDialog::PreLayoutDynInitL();
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::HandleSelectionKeyPressL()
// handles middle softkey press
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::HandleSelectionKeyPressL()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    HandleSelectionKeyPressL"));
    // Make special selection handling
    HandleSelectionsL();
    }

// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::HandlePointerEventL
// called by framework when the pointer device is pressed
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::HandlePointerEventL(
                                const TPointerEvent& aPointerEvent ) 
	{
	CCoeControl::HandlePointerEventL( aPointerEvent );
	
	if ( TPointerEvent::EButton1Up == aPointerEvent.iType )
		{
		HandleSelectionKeyPressL();
		}
	}

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::AppendIconToArray
// Load a possibly skinned icon (with mask) and append it to an
// icon array.
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId) const
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    AppendIconToArrayL"));

    __ASSERT_DEBUG( aArray, User::Leave(KErrArgument) );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    icon->SetBitmapsOwnedExternally(EFalse);

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL(icon);

    aArray->AppendL(icon);

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop(icon);
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::GenerateInternalArrayL
// Creates an internal array shown by listbox
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::GenerateInternalArrayL()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    GenerateInternalArrayL"));


    // delete existing array
    delete iInternalItemArray;
    iInternalItemArray = 0;

    // Make a new owned text array from the input one:
    iInternalItemArray = new(ELeave)CDesCArrayFlat( KListGranularity );
    TInt count = iItemArray->Count();

    for (TInt index = 0; index < count; index++)
        {
        // Fill up the control-owned space for the modified buffers:
        TInt newLen =
            ((*iItemArray)[index]->ItemText()).Length() +
            KOneSlashT().Length();

        HBufC* newText = HBufC::NewMaxLC( newLen );

        TPtr newTPtr = newText->Des();        
        newTPtr.Copy( ((*iItemArray)[index])->ItemText() );
        
        // replace '\t' with space
        TInt offset = newTPtr.Find( KSlashT );
        if ( offset >= 0 )
            {
            TInt replaceCount = KSlashT().Length();
            newTPtr.Replace( offset, replaceCount, KSPaceT );    
            }        
        
        newTPtr.Insert( 0, KOneSlashT ); // magic: insert to the beginning
        
        // A copy of the new buffer goes to the internal itemarray:                
        iInternalItemArray->AppendL(newTPtr);
        CleanupStack::PopAndDestroy( newText );
        }

    CTextListBoxModel* model=iListBox->Model();
    model->SetItemTextArray(iInternalItemArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Transfer the selected status from
    // the input array into the listbox view
    SetAllSelectionsL();
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::SetAllSelectionsL
// Sets selection status of items
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::SetAllSelectionsL()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    SetAllSelectionsL"));

    TInt count = iItemArray->Count();

    CListBoxView* view = iListBox->View();

    for (TInt index = 0; index < count; index++)
        {
        if ( (*iItemArray)[ index ]->SelectionStatus() )
            {
            view->SelectItemL( index );
            }
        else
            {
            view->DeselectItem( index );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::HandleSelectionsL
// Removes selection marks from other items if first one is selected
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::HandleSelectionsL()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionDialog::\
    HandleSelectionsL"));
    CAknListBoxFilterItems* filter =
                    static_cast < CAknFilteredTextListBoxModel* >
                    ( iListBox->Model() )->Filter();
    const CArrayFix<TInt>* indexes = iListBox->View()->SelectionIndexes();
    if ( indexes->Count() )
        {
        TKeyArrayFix sortKey(0,ECmpTUint);
        TInt dummy;
        if ( FindBox()->TextLength() == 0 ||
             filter->VisibleItemIndex( 0 ) != KErrNotFound )
            {
            // Check if first item is selected
            if ( indexes->Find(0, sortKey, dummy) == 0 )
                {
                // clear all
                iListBox->View()->ClearSelection();
                
                // select current item
                iListBox->View()->SelectItemL( iListBox->CurrentItemIndex() );
                }
            else // first item not selected
                {
                // check if first item is highlighted
                if ( iListBox->CurrentItemIndex() == 0 )
                    {
                    // remove all selections...
                    iListBox->View()->ClearSelection();
                    }
                //...and toggle highlighted item
                iListBox->View()->ToggleItemL( iListBox->CurrentItemIndex() );
                
                // check if we don't have selected items
                if ( indexes->Count() == 0 )
                    {
                    // check the first one
                    iListBox->View()->SelectItemL(0);
                    }
                }
            }
        else
            {
            //...and toggle highlighted item
            iListBox->View()->ToggleItemL( iListBox->CurrentItemIndex() );

            // check if we don't have selected items
            if ( indexes->Count() == 0 )
                {
                // check the first one
                iListBox->View()->SelectItemL(0);
                }
            }
        }
    else // no items selected
        {
        iListBox->View()->SelectItemL(0);
        }
    }

// --------------------------------------------------------------------------
// CMSMultiselectionDialog::UpdateAllSelectionsL
// Updates selection status of the items in the item array
// --------------------------------------------------------------------------
//
void CMSMultiselectionDialog::UpdateAllSelectionsL()
    {
    LOG(_L("[UPNP_HOMECONNECT]\t CMSMultiselectionDialog::\
    UpdateAllSelectionsL"));

    TInt count = iItemArray->Count();

    CAknListBoxFilterItems* filter
                = static_cast < CAknFilteredTextListBoxModel* >
                                ( iListBox->Model() )->Filter();
    // reset filtering so that real selected indexes can be get
    filter->ResetFilteringL();

    CListBoxView* view = iListBox->View();

    for (TInt index = 0; index < count; index++)
        {
        (*iItemArray)[ index ]->SetSelectionStatus(
                                    view->ItemIsSelected( index ) );
        }
    }

// End of file
