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
* Description:  CMSStoreListContainer class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <mediaservant.mbg>
#include <avkon.mbg>
#include <eikclbd.h>
#include <utf.h>
#include <e32math.h>
#include <aknnotewrappers.h>

#include "mserv.hlp.hrh"
#include "cmstorerulecontainer.h"
#include "cmstorerule.h"
#include "msappui.h"
#include "msstorelistcontainer.h"
#include "msstorelistcontroller.h"
#include "msstorelistview.h"
#include "msengine.h"
#include "msconstants.h"
#include "mediaservantuid.h"
#include "msdebug.h"


// CONSTANTS

// list granularity
const TInt KListGranularity = 5;

// --------------------------------------------------------------------------
// CMSStoreListContainer::CMSStoreListContainer()
// --------------------------------------------------------------------------
//
CMSStoreListContainer::CMSStoreListContainer( CMSAppUi& aAppUi,
                                              CMSStoreListView& aView ):
    iMSAppUi( aAppUi ),
    iView( aView )
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::CMSStoreListContainer"));
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::~CMSStoreListContainer()
// --------------------------------------------------------------------------
//
CMSStoreListContainer::~CMSStoreListContainer()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::\
    ~CMSStoreListContainer"));
    delete iSelectionArray;

    if ( iListBox )
        {
        delete iListBox; // Deletes listbox object.
        }
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::ConstructL()
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::ConstructL( TRect aRect )
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::ConstructL"));

    CreateWindowL();

    iListBox = new (ELeave) CAknSingleGraphicStyleListBox;

    SetListBoxFromResourceL(iListBox, R_STORE_LIST_VIEW_LISTBOX);

    // Container is listbox observer
    iListBox->SetListBoxObserver( this );

    // Get list container
    iStoreRuleContainer =
            iMSAppUi.StoreListController()->StoreListContainer();

    // creata array for selection indexes
    iSelectionArray = new (ELeave ) CArrayFixFlat<TInt>(KListGranularity);

        // Create icons
    CAknIconArray* icons = new (ELeave) CAknIconArray( EIconCount );
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

    // Create icon for context pane
    AknsUtils::CreateIconL( skin,
                            KAknsIIDDefault,
                            iIcon,
                            iIconMask,
                            KMSMifFileName,
                            EMbmMediaservantQgn_mserv_to_home,
                            EMbmMediaservantQgn_mserv_to_home_mask);
    // change context icon
    SetContextPaneIconL( iIcon, iIconMask );


    SetRect(aRect);

    // Add data to listbox
    UpdateListBoxDataL();

    //Set current item
    iListBox->SetCurrentItemIndex(
                        iMSAppUi.ParameterAgent()->StoreViewFocusIndex() );
    ActivateL(); // Activates window. ( Ready to draw )
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::CountComponentControls()
// Returns number of components.
// --------------------------------------------------------------------------
//
TInt CMSStoreListContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }


// --------------------------------------------------------------------------
// CMSStoreListContainer::ComponentControl()
// Returns pointer to particular component.
// --------------------------------------------------------------------------
//
CCoeControl* CMSStoreListContainer::ComponentControl(TInt aIndex) const

    {
    CCoeControl* retval = NULL;
    switch ( aIndex )
        {
        case 0:
            {
            retval = iListBox; // return a pointer to the listbox
            break;
            }
        default:
            {
            retval = NULL;
            break;
            }
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::OfferKeyEventL()
// Handles the key events.
// --------------------------------------------------------------------------
//
TKeyResponse CMSStoreListContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::OfferKeyEventL"));

    TKeyResponse response = EKeyWasNotConsumed;

    if ( aType != EEventKey )
        {
        response =  EKeyWasNotConsumed;
        }

    if ( iListBox )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        if ( aKeyEvent.iCode == EKeyOK &&
            iListBox->View()->ItemIsSelected( SelectedItemIndex() ))
            {
            // check that server is set
            CheckServersAndShowNoteL();
            }

		if ( !iView.PreProcessing() )
			{
			iView.CreateAndSetNaviTextL();
			}
        }

    return response;
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::HandlePointerEventL
// called by framework when the pointer device is pressed
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent ) 
	{
	CCoeControl::HandlePointerEventL( aPointerEvent );
	
	if ( TPointerEvent::EButton1Up == aPointerEvent.iType && 
	     !iView.PreProcessing() )
		{
		iView.CreateAndSetNaviTextL();
		}
	}

// --------------------------------------------------------------------------
// CMSStoreListContainer::ToggleCurrentItemL()
// Toggles item selection
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::ToggleCurrentItemL()
    {
    TInt itemIndex = SelectedItemIndex();

    iListBox->View()->ToggleItemL( itemIndex );

    if ( iListBox->View()->ItemIsSelected( itemIndex ) )
        {
        // check that server is set
        CheckServersAndShowNoteL();
        }
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::CheckServersAndShowNoteL()
// Checks if server is not set and shows info note
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::CheckServersAndShowNoteL()
    {
    CCmStoreRule* rule =
                iStoreRuleContainer->StoreRule( SelectedItemIndex() );
    if ( !rule->MediaServerCount() )
        {
        CAknInformationNote* dlg = new(ELeave)CAknInformationNote();
        CleanupStack::PushL( dlg );
        HBufC* noteText =
                StringLoader::LoadLC( R_MS_TEXT_TARGET_DEVICE_NEEDED );
        dlg->ExecuteLD( *noteText );
        CleanupStack::PopAndDestroy( noteText );
        CleanupStack::Pop( dlg );
        }

    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::HandleListBoxEventL()
// Handles listbox event.
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::HandleListBoxEventL"));

    if ( ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed ) ||
         ( aEventType == MEikListBoxObserver::EEventItemClicked ) )
        {
        ToggleCurrentItemL();
        }
    }


// --------------------------------------------------------------------------
// CMSStoreListContainer::UpdateListBoxDataL()
// Updates listbox content
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::UpdateListBoxDataL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::UpdateListBoxDataL"));

    TCmServerState state;
    iMSAppUi.MSEngine()->ServerState( state );

    CDesCArray* itemArray = static_cast<CDesCArray*>
                                        (iListBox->Model()->ItemTextArray());


    TInt ruleCount = iStoreRuleContainer->StoreRuleCount();

    for ( TInt index = 0; index < ruleCount; index++ )
        {


        CCmStoreRule* rule = iStoreRuleContainer->StoreRule( index );
        TPtrC8 ruleName = rule->Name();

        HBufC* itemText =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L( rule->Name() );
        CleanupStack::PushL( itemText );

        if ( rule->Selected() )
            {
            iSelectionArray->AppendL( index );
            }

        AppendItemToListBoxArrayL( iListBox,
                                   iListBox->Model(),
                                   EIconCheckboxOff,
                                   *itemText );

        CleanupStack::PopAndDestroy( itemText );
        }

    iListBox->HandleItemAdditionL();


    iListBox->SetSelectionIndexesL( iSelectionArray );

    if ( state != ECmServerStatePreProcessingStore )
        {
        iView.CreateAndSetNaviTextL();
        }
    }



// --------------------------------------------------------------------------
// CMSStoreListContainer::SelectedItemIndex()
// Highlighted item index
// --------------------------------------------------------------------------
//
TInt CMSStoreListContainer::SelectedItemIndex()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::SelectedItemIndex"));

    return iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::UpdateRulesToServerL()
// Send rules to server
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::UpdateRulesToServerL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::UpdateRulesToServerL"));

    // Send rules to server
    if ( iStoreRuleContainer )
        {
        iMSAppUi.MSEngine()->SetStoreRulesL( iStoreRuleContainer );
        }
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::SetServerSelectionsL()
// Sets selection statuses to servers
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::SetServerSelectionsL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::SetServerSelectionsL"));

    // remove all selections
    for ( TInt i = 0; i < iStoreRuleContainer->StoreRuleCount(); i++ )
        {
        CCmStoreRule* rule =
                    iStoreRuleContainer->StoreRule( i );
        rule->SetSelected( ECmUnSelected );
        }

    // Get marked items indexes
        const CListBoxView::CSelectionIndexArray* selections =
                                                iListBox->SelectionIndexes();
    // add user selections
    for ( TInt j = 0; j < selections->Count(); j++ )
        {
        TInt selectionIndex = (*selections)[j];
        CCmStoreRule* rule =
                    iStoreRuleContainer->StoreRule( selectionIndex );

        rule->SetSelected( ECmSelected );

        }

    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::ListItemCountL()
// Gets list item count & size
// --------------------------------------------------------------------------
//
TInt CMSStoreListContainer::ListItemCountL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::ListItemCountL"));

    TInt fileCount( 0 );
    TInt64 size( 0 );

    CCmStoreRule* rule = iStoreRuleContainer->StoreRule(
                                SelectedItemIndex() );
    TPtrC8 ruleName = rule->Name();

    if ( ruleName.Size() )
        {
        iMSAppUi.MSEngine()->GetItemCountsL( ECmServiceStore,
                                             ruleName,
                                             fileCount,
                                             size );
        }

    return fileCount;
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::CurrentListItemSelectionStatus()
// Return list item selection status
// --------------------------------------------------------------------------
//
TBool CMSStoreListContainer::CurrentListItemSelectionStatus()
    {
    return iListBox->View()->ItemIsSelected( SelectedItemIndex() );
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::SizeChanged()
    {
    // container control resize code.
    if (iListBox)
        {
        iListBox->SetRect(Rect()); // Set rectangle of listbox.
        }
    }

// --------------------------------------------------------------------------
// CMSStoreListContainer::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSStoreListContainer::HandleResourceChange(TInt aType)
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect(iView.ClientRect());
        }

    CCoeControl::HandleResourceChange(aType);
    }

// ---------------------------------------------------------------------------
// CMSStoreListContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSStoreListContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSStoreListContainer::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_STORE_LIST;
    }
// End of File

