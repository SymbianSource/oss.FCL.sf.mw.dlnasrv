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
* Description:  CMSFillContainer class implementation
*
*/


#include <AknIconArray.h>
#include <aknlists.h>
#include <AknsUtils.h>
#include <eikclbd.h>
#include <mediaservant.rsg>
#include <mediaservant.mbg>
#include <StringLoader.h>
#include <avkon.mbg>
#include <aknlistquerydialog.h>
#include "upnpstring.h"
#include <e32math.h>

#include "cmdriveinfo.h"
#include "mserv.hlp.hrh"
#include "cmfillrule.h"
#include "msfillview.h"
#include "msappui.h"
#include "mediaservantuid.h"
#include "msengine.h"
#include "msfillcontainer.h"
#include "msconstants.h"
#include "msdebug.h"

// CONSTANTS

// Listbox granularity
const TInt KListGranularity = 5;

// --------------------------------------------------------------------------
// CMSFillContainer::CMSFillContainer()
// --------------------------------------------------------------------------
//
CMSFillContainer::CMSFillContainer( CMSAppUi& aAppUi, CMSFillView& aView ) :
    iMSAppUi( aAppUi ),
    iView( aView )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::CMSFillContainer"));
    }

// --------------------------------------------------------------------------
// CMSFillContainer::~CMSFillContainer()
// --------------------------------------------------------------------------
//
CMSFillContainer::~CMSFillContainer()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::~CMSFillContainer"));

    iSelectedLists.Close();

    iView.ClearCurrentNaviPaneText();

    delete iListBox; // Deletes listbox object.

    }

// --------------------------------------------------------------------------
// CMSFillContainer::ConstructL()
// --------------------------------------------------------------------------
//
void CMSFillContainer::ConstructL( TRect aRect )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::ConstructL"));

    // fill quota size
    CalculateFillQuotaL();

    CreateWindowL();

    iListBox = new (ELeave) CAknSingleGraphicStyleListBox;

    // This is multiselection listbox but resource defines it as SELECTION
    // LIST because we don't want to let framework handle icon changes.
    // Framework toggles between two icons whereas we have to show multiple
    // icons.
    // Special icons are changed every time item selection or priority
    // changes. Calculations for icons are made in SetIcon()
    SetListBoxFromResourceL(iListBox, R_FILL_VIEW_LISTBOX);

    // Create icons
    CAknIconArray* icons = new (ELeave) CAknIconArray( EIconCount );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // selected and fits icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_white_check,
                        EMbmMediaservantQgn_mserv_white_check_mask );

    // unselected and fits icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_white_uncheck,
                        EMbmMediaservantQgn_mserv_white_uncheck_mask );

    // selected and partially fits icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_yellow_check,
                        EMbmMediaservantQgn_mserv_yellow_check_mask );

    // unselected and partially fits icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_yellow_uncheck,
                        EMbmMediaservantQgn_mserv_yellow_uncheck_mask );

    // selected and doesn't fit icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_red_check,
                        EMbmMediaservantQgn_mserv_red_check_mask );

    // unselected and doesn't fit icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_red_uncheck,
                        EMbmMediaservantQgn_mserv_red_uncheck_mask );
    // video type -icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_video_list_small,
                        EMbmMediaservantQgn_mserv_video_list_small_mask );

    // image type -icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_image_list_small,
                        EMbmMediaservantQgn_mserv_image_list_small_mask );

    // music type -icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_music_list_small,
                        EMbmMediaservantQgn_mserv_music_list_small_mask );

    // mark icon for reordering mode -icon
    AppendIconToArrayL( icons, skin,
                        KAvkonMifFileName,
                        KAknsIIDDefault,
                        EMbmAvkonQgn_indi_marked_add,
                        EMbmAvkonQgn_indi_marked_add_mask );

    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop(icons);


    // Create icon for context pane
    AknsUtils::CreateIconL( skin,
                            KAknsIIDDefault,
                            iIcon,
                            iIconMask,
                            KMSMifFileName,
                            EMbmMediaservantQgn_mserv_from_home,
                            EMbmMediaservantQgn_mserv_from_home_mask);
    // change context icon
    SetContextPaneIconL( iIcon, iIconMask );

    SetRect(aRect);

    // Get rule controller
    iFillRuleController = iMSAppUi.FillRuleController();
    // Update array
    iFillRuleController->UpdateArrayL();
    // get fill lists
    iFillListArray = iFillRuleController->FillListArray();
    // keep selections safe
    iSelectedIndexes = iListBox->SelectionIndexes();

    TInt64 capasity;
    TInt64 free;
    iMSAppUi.ReadMMCInfoL( capasity, free );
    iFreeMemory = Min( free, iQuota );

    if ( iFillListArray->Count() > 0 )
        {
        // Add data to listbox
        UpdateListBoxDataL();

        // Enable text scrolling
        iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

        // get item array
        CDesCArray* itemArray = static_cast<CDesCArray*>
                                (iListBox->Model()->ItemTextArray());

        iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
        }

    // keep selections safe
    const CListBoxView::CSelectionIndexArray* selected =
                                            iListBox->SelectionIndexes();

    for ( TInt index = 0; index < selected->Count(); index++ )
        {
        iSelectedLists.AppendL( (*selected)[index] );
        }

    // Set highlighted item
    TInt currentItem = iMSAppUi.ParameterAgent()->FillViewFocusIndex();
    if ( currentItem >= 0 )
        {
        //Set current item
        iListBox->SetCurrentItemIndex( currentItem );
        }
    else
        {
        // first item
        // This is set when new list is created without existing lists
        iListBox->SetCurrentItemIndex( 0 );
        }

    UpdateNaviTextL();

	iPreviousItemIndex = 0;
	iDragItemIndex = 0;

    // Change middle softkey according to selection
    TBool listSelected = IsCurrentListItemActive();
    iView.SetCbaL( listSelected );

    ActivateL(); // Activates window. ( Ready to draw )
    }

// --------------------------------------------------------------------------
// CMSFillContainer::CountComponentControls()
// Returns number of components.
// --------------------------------------------------------------------------
//
TInt CMSFillContainer::CountComponentControls() const
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::CountComponentControls"));

    return 1; // return number of controls inside this container
    }


// --------------------------------------------------------------------------
// CMSFillContainer::ComponentControl()
// Returns pointer to particular component.
// --------------------------------------------------------------------------
//
CCoeControl* CMSFillContainer::ComponentControl(TInt /*aIndex*/) const

    {
    return iListBox; // return a pointer to the listbox
    }

// --------------------------------------------------------------------------
// CMSFillContainer::OfferKeyEventL()
// Handles the key events.
// --------------------------------------------------------------------------
//
TKeyResponse CMSFillContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::OfferKeyEventL"));

    TKeyResponse response = EKeyWasNotConsumed;

    if ( aType != EEventKey )
        {
        response =  EKeyWasNotConsumed;
        }

    else if ( iListBox && iListBox->CurrentItemIndex() >= 0 )
        {
        // check if reordering mode is activated
        if ( iReorderState == EMSFillReorderActive ||
             aKeyEvent.iCode == EKeyLeftArrow ||
             aKeyEvent.iCode == EKeyRightArrow )
            {
            response = HandleReorderKeyEventL( aKeyEvent, aType );
            }

        // delete list
        else if ( aKeyEvent.iCode == EKeyBackspace )
            {
            DeleteRuleL();
            response = EKeyWasConsumed;
            }

        // activate/deactivate list
        else if ( aKeyEvent.iCode == EKeyOK )
            {
            CCmFillRule* rule =
                (*iFillListArray)[iListBox->CurrentItemIndex()]->List();
            if ( rule->Selected() == ECmSelected )
                {
                rule->SetSelected( ECmUnSelected );
                }
            else
                {
                rule->SetSelected( ECmSelected );
                }
            iView.SetListUnselected();
            UpdateListBoxDataL();
            response = EKeyWasConsumed;
            }

        // default
        else
            {
            response = iListBox->OfferKeyEventL( aKeyEvent, aType );
            }


        TCmServerState state;
        iMSAppUi.MSEngine()->ServerState( state );
        if ( ( aKeyEvent.iCode == EKeyUpArrow ||
             ( aKeyEvent.iCode == EKeyDownArrow )))
            {
            // change navi pane text to match highlighted list size
            UpdateNaviTextL();
            }
        }

#ifndef __SERIES60_31__
    if ( iReorderState == EMSFillNormal )
        {
        // Change middle softkey according to selection
        TBool listSelected = IsCurrentListItemActive();
        iView.SetCbaL( listSelected );
        }
#endif

    return response;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::HandleDragAndDropEventsL()
// Handles drag and drop events on touch screen
// --------------------------------------------------------------------------
//
void CMSFillContainer::HandleDragAndDropEventsL( const TPointerEvent& aPointerEvent )
	{
	if ( aPointerEvent.iType == TPointerEvent::EButton1Down &&
	     iReorderState != EMSFillNormal )
		{
		iReorderState = EMSFillReorderActive;
	   	iDragItemIndex = iListBox->CurrentItemIndex();
		}
	else if ( aPointerEvent.iType == TPointerEvent::EButton1Up &&
	     iReorderState == EMSFillReorderActive )
		{
		iView.HandleCommandL( EMSCmdDrop );
		TInt listBoxItem = iListBox->CurrentItemIndex();
		iListBox->SetCurrentItemIndex( iDragItemIndex );

		TKeyEvent keyEvent;
		// Move dragged item downwards
		if ( listBoxItem >= iDragItemIndex )
			{
			keyEvent.iCode = EKeyDownArrow;
        	for( TInt c = 0; listBoxItem > iDragItemIndex + c; c++ )
	        	{
	        	HandleReorderKeyEventL( keyEvent, EEventKey );
	        	}
			}
		// Move dragged item upwards
		else
			{
			keyEvent.iCode = EKeyUpArrow;
        	for( TInt c = 0; listBoxItem < iDragItemIndex - c; c++ )
	        	{
	        	HandleReorderKeyEventL( keyEvent, EEventKey );
	        	}
			}
        	
		iListBox->SetCurrentItemIndex( listBoxItem );
		}
	}

// --------------------------------------------------------------------------
// CMSFillContainer::UpdateListBoxDataL()
// Creates listbox items
// --------------------------------------------------------------------------
//
void CMSFillContainer::UpdateListBoxDataL()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::UpdateListBoxDataL"));
    TInt64 freeSpace = iFreeMemory;

    TCmServerState state;
    iMSAppUi.MSEngine()->ServerState( state );

    // Keep item index safe for reordering mode because we reset the list
    TInt currentItemIndex = iListBox->CurrentItemIndex();

    // reset list size
    iSizeOfSelectedLists = 0;

    TBuf<KMaxFileName> item;
    HBufC* primary_text = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( primary_text );

    // Listbox items
    CDesCArray* itemArray = static_cast<CDesCArray*>
                                   (iListBox->Model()->ItemTextArray());
    itemArray->Reset();

    // Marked listbox item indexes
    CArrayFix<TInt>* selectionArray = new (ELeave )
                                   CArrayFixFlat<TInt>(KListGranularity);

    CleanupStack::PushL( selectionArray );

    TInt ruleCount = iFillListArray->Count();

    for ( TInt index = 0; index < ruleCount; index++ )
        {
        // Get fill rule name and put it to the listbox
        CCmFillRule* rule = (*iFillListArray)[index]->List();
        HBufC* listName = UpnpString::ToUnicodeL( rule->Name() );
        CleanupStack::PushL( listName );
        primary_text->Des().Copy( *listName );
        CleanupStack::PopAndDestroy( listName );


        // Check if rule is active and mark it in the list
        TCmFillRuleStatus status = rule->Selected();

        TInt64 listSize = (*iFillListArray)[index]->ListSize();

        if ( status == ECmSelected )
            {
            selectionArray->AppendL( index );

            TInt id = rule->ListId();
            TInt64 size = 0;
            iMSAppUi.GetFilledFilesSize( size, id );
            
            // list selected - add used memory size
            iSizeOfSelectedLists += ( listSize - size );
            }
        // calculate icon index
        TInt iconIndex = ListItemIconIndex( index,
                                            listSize,
                                            freeSpace );


        switch ( rule->MediaType() )
            {
            case ECmVideo:
                {
                item.Format( KSingleGraphicStyleFormatString,
                             iconIndex, primary_text, EVideoType );
                break;
                }
            case ECmImage:
                {
                item.Format( KSingleGraphicStyleFormatString,
                             iconIndex, primary_text, EImageType );
                break;
                }
            case ECmAudio:
                {
                item.Format( KSingleGraphicStyleFormatString,
                             iconIndex, primary_text, EAudioType );
                break;
                }
            default:
                {
                item.Format( KSingleGraphicStyleFormatStringNoTrailIcons,
                             iconIndex, primary_text);
                break;
                }
            }

        // Check if reorder mode is active and mark current item
        if ( iReorderState == EMSFillReorderActive &&
             index == currentItemIndex )
            {
            TBuf<2> markIconStr; // Magic: 2 characters reserverd for icon
            markIconStr.AppendNum( EMark );
            item.Append(markIconStr);
            }
        itemArray->AppendL( item );
        }

    iListBox->SetSelectionIndexesL( selectionArray );

    CleanupStack::PopAndDestroy( selectionArray );
    CleanupStack::PopAndDestroy( primary_text );

    UpdateNaviTextL();
    if( currentItemIndex > KErrNotFound && currentItemIndex < ruleCount )
        {
        iListBox->SetCurrentItemIndex( currentItemIndex );
        }
    iListBox->HandleItemAdditionL();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::SetFillRules()
// Sends fill rules to server
// --------------------------------------------------------------------------
//
void CMSFillContainer::SetFillRules()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::SetFillRules"));

    // Get marked items indexes
    const CListBoxView::CSelectionIndexArray* selected =
            iListBox->SelectionIndexes();

    // Listbox item count
    TInt itemCount = iFillListArray->Count();

    // Selected item index
    TInt itemIdx = 0;

    // Sort criteria, offset = 0
    TKeyArrayFix sortKey( 0, ECmpTUint );

    // count selected items
    TInt count = selected->Count();

    for ( TInt index = 0; index < itemCount; index++ )
        {
        // check if listbox item is found from selected item list
        TInt found = selected->Find( index, sortKey, itemIdx );

        CCmFillRule* rule = (*iFillListArray)[index]->List();

        // FOUND
        if ( found == 0 )
            {
            // set rule selected
            rule->SetSelected( ECmSelected );
            }
        // NOT FOUND
        else
            {
            // set rule unselected
            rule->SetSelected( ECmUnSelected );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::SelectedItemIndex()
// Highlighted item index
// --------------------------------------------------------------------------
//
TInt CMSFillContainer::SelectedItemIndex()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::SelectedItemIndex"));

    return iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::ListItemCount()
// coun of list items
// --------------------------------------------------------------------------
//
TInt CMSFillContainer::ListItemCount()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::ListItemCount"));

    return iListBox->Model()->NumberOfItems();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::SetReorderStateL()
// Sets reorder state
// --------------------------------------------------------------------------
//
void CMSFillContainer::SetReorderStateL( TMSReorderState aState )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::SetReorderStateL"));

    switch ( aState )
        {
        case EMSFillNormal:
            {
            iReorderState = EMSFillNormal;
            break;
            }
        case EMSFillReorderActive:
            {
            iReorderState = EMSFillReorderActive;
            UpdateListBoxDataL();
            break;
            }
        case EMSFillGrabActive:
            {
            iReorderState = EMSFillGrabActive;
            UpdateListBoxDataL();
            break;
            }
        default:
            {
            iReorderState = EMSFillNormal;
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::MoveItem()
// moves item up/down on the list
// --------------------------------------------------------------------------
//
void CMSFillContainer::MoveItem( TInt aDirection )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::MoveItem"));

    TInt currentIdx = iListBox->CurrentItemIndex();
    TInt totalItem = iListBox->Model()->NumberOfItems();

    // store selection status of all rules
    StoreSelectionStatus();

    // up
    if (aDirection == -1 && currentIdx)
        {
        iFillRuleController->ChangePriority( totalItem,
                                             currentIdx,
                                             aDirection );
        }
    // down
    else if ( aDirection == 1 && currentIdx < totalItem - 1 )
        {
        iFillRuleController->ChangePriority( totalItem,
                                             currentIdx,
                                             aDirection );
        }
    else
        {
        // Do nothing
        }
    }


// --------------------------------------------------------------------------
// CMSFillContainer::HandleReorderKeyEventL
// Handle key events in reordering mode.
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
TKeyResponse CMSFillContainer::HandleReorderKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::HandleReorderKeyEventL"));

    TKeyResponse keyResponse = EKeyWasConsumed;

    TInt currentIdx = iListBox->CurrentItemIndex();
    TInt totalItems = iListBox->Model()->NumberOfItems();

    TBool reqToRedraw = EFalse;
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            // fall through
            case EKeyLeftArrow:
                {
                if ( currentIdx > 0 )
                    {
                    MoveItem(-1);
                    iListBox->SetCurrentItemIndex( currentIdx - 1 );
                    reqToRedraw = ETrue;
                    }
                break;
                }
            case EKeyDownArrow:
            // fall through
            case EKeyRightArrow:
                {
                if ( currentIdx < totalItems-1 ) // because index is 0 based
                    {
                    MoveItem(1);
                    iListBox->SetCurrentItemIndex( currentIdx + 1 );
                    reqToRedraw = ETrue;
                    }
                break;
                }
            default:
                {
                LOG(_L("[MediaServant]\t CMSFillContainer::\
                HandleReorderKeyEventL invalid keycode"));
                break;
                }
            }
        }

    if( reqToRedraw )
        {
        // update selected item indexes
        const CListBoxView::CSelectionIndexArray* selected =
                                                iListBox->SelectionIndexes();
        UpdateListBoxDataL();
        iSelectedLists.Reset();
        for ( TInt index = 0; index < selected->Count(); index++ )
            {
            TInt a = (*selected)[index];
            iSelectedLists.AppendL( (*selected)[index] );
            }
        }

    return keyResponse;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::StoreSelectionStatus()
// stores fill rule selection status to rule container
// --------------------------------------------------------------------------
//
void CMSFillContainer::StoreSelectionStatus()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::StoreSelectionStatusL"));

    // index of selected item
    TInt itemIdx = 0;
    // Sort criteria, offset = 0
    TKeyArrayFix sortKey( 0, ECmpTUint );

    // Listbox array
    CDesCArray* itemArray = static_cast<CDesCArray*>
                                    (iListBox->Model()->ItemTextArray());

    // Listbox item count
    TInt itemCount = itemArray->Count();

    const CListBoxView::CSelectionIndexArray* selected =
            iListBox->SelectionIndexes();

    // count selected items
    TInt count = selected->Count();

    for ( TInt index = 0; index < itemCount; index++ )
        {
        // check if listbox item is found from selected item list
        TInt found = selected->Find( index, sortKey, itemIdx );

        if ( found == 0 )
            {
            // found - get correcponding index for rule table with At()
            CCmFillRule* rule =
                (*iFillListArray)[selected->At( itemIdx )]->List();
            // set rule selected
            rule->SetSelected( ECmSelected );
            }
        else
            {
            // not found
            CCmFillRule* rule = (*iFillListArray)[index]->List();
            // set rule unselected
            rule->SetSelected( ECmUnSelected );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::DeleteRuleL()
// Deletes highlighted rule
// --------------------------------------------------------------------------
//
void CMSFillContainer::DeleteRuleL()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::DeleteRuleL"));

    // Show delete files query
    CAknQueryDialog* query= CAknQueryDialog::NewL(
                                   CAknQueryDialog::ENoTone );

    if ( query->ExecuteLD( R_MSERV_DELETE_LIST_QUERY ) )
        {
        TInt currentItemIndex = iListBox->CurrentItemIndex();
        // delete rule
        iFillRuleController->DeleteRuleL( currentItemIndex );

        // Listbox items
        CDesCArray* itemArray = static_cast<CDesCArray*>
                                       (iListBox->Model()->ItemTextArray());

        // delete list item
        itemArray->Delete( currentItemIndex );
        iListBox->HandleItemRemovalL();
        iListDeleted = ETrue;

        if ( currentItemIndex > 0 )
            {
            // highligth previous
            iListBox->SetCurrentItemIndexAndDraw( currentItemIndex - 1 );
            }
        iMSAppUi.MSEngine()->DeleteFilledFilesL();
        iView.ShowNaviTextL();
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::IsListDeleted()
// Returns list delete status
// --------------------------------------------------------------------------
//
TBool CMSFillContainer::IsListDeleted()
    {
    return iListDeleted;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::HandlePointerEventL()
// Handles touchscreen event
// --------------------------------------------------------------------------
//
void CMSFillContainer::HandlePointerEventL ( const TPointerEvent& aPointerEvent )
    {
	CCoeControl::HandlePointerEventL( aPointerEvent );
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
    	{
    	// Do not change listbox item state, when highlighting an item or,
	    // when reordering state is active
    	if ( iListBox->CurrentItemIndex() == iPreviousItemIndex &&
             iReorderState == EMSFillNormal )
	    	{	
			TKeyEvent keyEvent;
			keyEvent.iCode = EKeyOK;
			OfferKeyEventL( keyEvent, EEventKey );
    		}
    	else
    		{
    		UpdateListBoxDataL();
    		}
	   	iPreviousItemIndex = iListBox->CurrentItemIndex();
		}
		
	// Handle Drag and drop events, when reorder state is active
    if ( iReorderState != EMSFillNormal )
    	{
    	HandleDragAndDropEventsL( aPointerEvent );
    	}
    }

// --------------------------------------------------------------------------
// CMSFillContainer::CheckIfDublicateName()
// Checks how many dublicate items is on the fill list
// --------------------------------------------------------------------------
//
TInt CMSFillContainer::CheckIfDublicateNames( const TDesC& aName )
    {
    CDesCArray* itemArray = static_cast<CDesCArray*>
                                   (iListBox->Model()->ItemTextArray());
    TInt count(0);

    for ( TInt index = 0; index < itemArray->MdcaCount(); index++ )
        {
        if ( KErrNotFound != itemArray->MdcaPoint( index ).Match( aName ) )
            {
            count++;
            }
        }
    return count;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::UpdateNaviTextL()
// updates list size on navi pane
// --------------------------------------------------------------------------
//
void CMSFillContainer::UpdateNaviTextL()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::UpdateNaviTextL"));

    TInt currentItemIndex = iListBox->CurrentItemIndex();

    if ( currentItemIndex != KErrNotFound )
        {
        CArrayFix<TInt>* infoArray = new ( ELeave ) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL( infoArray );

        CMSFillList* fillList = (*iFillListArray)[currentItemIndex];
        CCmFillRule* rule = fillList->List();

        HBufC* naviText = NULL;

        TInt64 freeMemory = iFreeMemory - iSizeOfSelectedLists;

        // round values
        TReal size(0);
        Math::Round( size, TReal(fillList->ListSize()) / KMega, 0 );

        TReal freeSize(0);
        Math::Round( freeSize, TReal(freeMemory) / KMega, 0 );

        infoArray->AppendL( size );
        infoArray->AppendL( freeSize );

        naviText = StringLoader::LoadLC( R_MS_NAVI_LIST_SIZE,
                                         *infoArray );
        // do number conversion
        TPtr ptr = naviText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        iView.SetNavigationPaneTextL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
        CleanupStack::PopAndDestroy( infoArray );
        }
    else
        {
        iView.ClearCurrentNaviPaneText();
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::ChangeListActivityL()
// Toggles list item
// --------------------------------------------------------------------------
//
void CMSFillContainer::ChangeListActivityL()
    {
    CCmFillRule* rule =
        (*iFillListArray)[iListBox->CurrentItemIndex()]->List();
    if ( rule->Selected() == ECmSelected )
        {
        rule->SetSelected( ECmUnSelected );
        }
    else
        {
        rule->SetSelected( ECmSelected );
        }
    UpdateListBoxDataL();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::IsCurrentListItemActive()
// Checks if item is selected
// --------------------------------------------------------------------------
//
TBool CMSFillContainer::IsCurrentListItemActive()
    {
    return iListBox->View()->ItemIsSelected( iListBox->CurrentItemIndex() );
    }

// --------------------------------------------------------------------------
// CMSFillContainer::HasListSelectionsChanged
// Checks if item is selected
// --------------------------------------------------------------------------
//
TBool CMSFillContainer::HasListSelectionsChanged()
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::HasListSelectionsChanged"));

    TBool status = EFalse;

    TInt originalCount = iSelectedLists.Count();

    const CListBoxView::CSelectionIndexArray* selected =
                                                iListBox->SelectionIndexes();

    TKeyArrayFix sortKey( 0, ECmpTUint );
    // dummy index - not interested of this
    TInt dummy = 0;

    for ( TInt index = 0; index < originalCount; index++ )
        {
        // check if listbox item is found from selected item list
        TInt found = selected->Find(
                             iSelectedLists[index], sortKey, dummy );
        if ( found != 0 )
            {
            // something has changed on the list
            status = ETrue;
            index = originalCount; // break loop
            }
        }

    return status;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::ListItemIconIndex
// Returns icon index based on list size and free space
// --------------------------------------------------------------------------
//
TInt CMSFillContainer::ListItemIconIndex( TInt aIndex,
                                          TInt64 aListSize,
                                          TInt64& aFreeSpace )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::ListItemIconIndex"));

    TInt iconIndex( 0 );
    // check if item is selected
    CCmFillRule* rule = (*iFillListArray)[aIndex]->List();
    if(  ECmSelected == rule->Selected() )
        {
        // doesn't fit
        if ( aFreeSpace <= 0 )
            {
            iconIndex = EMSSelectedDoesntFit;
            aFreeSpace = 0;
            }
        // fits
        else if ( aFreeSpace - aListSize > 0 )
            {
            iconIndex = EMSSelectedFits;
            aFreeSpace -= aListSize;
            }
        // fits partially
        else
            {
            iconIndex = EMSSelectedPartiallyFits;
            aFreeSpace = 0;
            }
        }
    else
        {
        // doesn't fit
        if ( aFreeSpace <= 0)
            {
            iconIndex = EMSUnSelectedDoesntFit;
            aFreeSpace = 0;
            }
        // fits
        else if ( aFreeSpace - aListSize > 0 )
            {
            iconIndex = EMSUnselectedFits;
            aFreeSpace -= aListSize;
            }
        // fits partially
        else
            {
            iconIndex = EMSUnselectedPartiallyFits;
            aFreeSpace = 0;
            }
        }
    return iconIndex;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::CalculateFillQuotaL()
// Gets selected drives and calculates quota size
// --------------------------------------------------------------------------
//
void CMSFillContainer::CalculateFillQuotaL()
    {
    RPointerArray<CCmDriveInfo> drives;

    iMSAppUi.MSEngine()->GetDrivesL( drives );
    for ( TInt i = 0; i < drives.Count(); i++ )
        {
        iQuota += drives[i]->DriveQuota();
        iQuota -= drives[i]->UsedDriveQuota();
        }
    drives.ResetAndDestroy();
    drives.Close();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSFillContainer::SizeChanged()
    {
    // container control resize code.
    if ( iListBox )
        {
        iListBox->SetRect( Rect() ); // Set rectangle of listbox.
        }
    }

// --------------------------------------------------------------------------
// CMSFillContainer::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSFillContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView.ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------------------------
// CMSFillContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSFillContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_FILL_LIST;
    }

// End of File

