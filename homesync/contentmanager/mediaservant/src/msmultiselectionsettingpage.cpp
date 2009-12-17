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
* Description:  CMSMultiselectionSettingPage class implementation
*
*/


#include <mediaservant.rsg>
#include "msmultiselectionsettingpage.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::NewL
// --------------------------------------------------------------------------
//
CMSMultiselectionSettingPage* CMSMultiselectionSettingPage::NewL(
                                TInt aIdentifier,
                                CSelectionItemList& aItemArray,
                                TBool aFirstItemSwitch )
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::NewL"));

    CMSMultiselectionSettingPage* self =
            CMSMultiselectionSettingPage::NewLC( aIdentifier,
                                                 aItemArray,
                                                 aFirstItemSwitch );
    CleanupStack::Pop(self);

    return self;
    }

// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::NewLC
// --------------------------------------------------------------------------
//
CMSMultiselectionSettingPage* CMSMultiselectionSettingPage::NewLC(
                                TInt aIdentifier,
                                CSelectionItemList& aItemArray,
                                TBool aFirstItemSwitch )
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::NewLC"));

    CMSMultiselectionSettingPage* self = new (ELeave)
                           CMSMultiselectionSettingPage( aIdentifier,
                                                         aItemArray,
                                                         aFirstItemSwitch  );

    CleanupStack::PushL(self);
    self->ConstructL();
        
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::ConstructL
// --------------------------------------------------------------------------
//
void CMSMultiselectionSettingPage::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::ConstructL"));

    CAknCheckBoxSettingPage::ConstructL();
    }

// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::CMSMultiSelectionSettingPage
// --------------------------------------------------------------------------
//
CMSMultiselectionSettingPage::CMSMultiselectionSettingPage(
    TInt aIdentifier,
    CSelectionItemList& aItemArray,
    TBool aFirstItemSwitch
     ) :
    CAknCheckBoxSettingPage( aIdentifier, &aItemArray ),
    iItemArray ( &aItemArray ),
    iFirstItemSwitch ( aFirstItemSwitch )
    {
    }

// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::~CMSMultiSelectionSettingPage()
// --------------------------------------------------------------------------
//
CMSMultiselectionSettingPage::~CMSMultiselectionSettingPage()
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::\
    ~CMSMultiSelectionSettingPage"));

    }

// --------------------------------------------------------------------------
// CMSMultiSelectionSettingPage::SelectCurrentItemL()
// --------------------------------------------------------------------------
//
void CMSMultiselectionSettingPage::SelectCurrentItemL()
    {
    if ( iFirstItemSwitch )
        {
        HandleSelectionsL();
        }
    else
        {
        CAknCheckBoxSettingPage::SelectCurrentItemL();
        }
    }

// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::HandleSelectionsL
// Removes selection marks from other items if first one is selected
// --------------------------------------------------------------------------
//
void CMSMultiselectionSettingPage::HandleSelectionsL()
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::\
    HandleSelectionsL"));

    CEikListBox* listBox = ListBoxControl();

    const CArrayFix<TInt>* indexes = listBox->View()->SelectionIndexes();

    if ( indexes->Count() )
        {
        TKeyArrayFix sortKey(0,ECmpTUint);
        TInt dummy;

        // check if firs item selected
        if ( indexes->Find(0, sortKey, dummy) == 0 )
            {
            // clear all
            listBox->View()->ClearSelection();
            // select current item
            listBox->View()->SelectItemL( listBox->CurrentItemIndex());
            }
        else // first item not selected
            {
            // check if first item is highlighted
            if ( listBox->CurrentItemIndex() == 0 )
                {
                // remove all selections...
                listBox->View()->ClearSelection();
                }
            //...and toggle highlighted item
            listBox->View()->ToggleItemL( listBox->CurrentItemIndex());

            // check if we don't have selected items
            if ( indexes->Count() == 0 )
                {
                // check the first one
                listBox->View()->SelectItemL(0);
                }
            }
        }
    else // no items selected
        {
        listBox->View()->SelectItemL(0);
        }
    }
// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::UpdateAllSelections
// Updates selection status of the items in the item array
// --------------------------------------------------------------------------
//
void CMSMultiselectionSettingPage::UpdateAllSelections()
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::\
    UpdateAllSelections"));

    TInt count = iItemArray->Count();

    CListBoxView* view = ListBoxControl()->View();

    for (TInt index = 0; index < count; index++)
        {
        (*iItemArray)[ index ]->SetSelectionStatus(
                                    view->ItemIsSelected( index ) );
        }
    }

// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::HandlePointerEventL
// called by framework when the pointer device is pressed
// --------------------------------------------------------------------------
//
void CMSMultiselectionSettingPage::HandlePointerEventL(const TPointerEvent &aPointerEvent) 
	{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	if ( TPointerEvent::EButton1Up == aPointerEvent.iType )
		{
		SelectCurrentItemL();
		}
	}

// --------------------------------------------------------------------------
// CMSMultiselectionSettingPage::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// --------------------------------------------------------------------------
//
TBool CMSMultiselectionSettingPage::OkToExitL( TInt aButtonId )
    {
    LOG(_L("[MediaServant]\t CMSMultiSelectionSettingPage::OkToExitL"));

    if ( aButtonId != EEikCmdCanceled )
        {
        // Update item selections to the list
        UpdateAllSelections();
        }

    return ETrue;
    }

// End of File

