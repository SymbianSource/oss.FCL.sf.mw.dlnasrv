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
* Description:  CMSRuleMultiselectionSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <akncheckboxsettingpage.h>
#include <StringLoader.h>
#include <utf.h>

#include "cmsqlpropertycontainer.h"
#include "cmsqlpropertyitem.h"
#include "msrulemultiselectionsetting.h"
#include "msconstants.h"
#include "msmultiselectiondialog.h"
#include "msdebug.h"

// CONSTANTS
const TInt KItemArrayGranularity = 3;

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::NewL
// --------------------------------------------------------------------------
//
CMSRuleMultiselectionSetting* CMSRuleMultiselectionSetting::NewL(
                                TInt aIdentifier,
                                TInt aTitleResource,
                                TInt aFirstPopupItemResource,
                                TInt aAnyItemTextResource,
                                CCmSqlPropertyContainer* aMetadataArray,
                                RArray<TInt>* aArray,
                                TInt aItemTextResource,
                                TDes& aText)
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::NewL"));

    CMSRuleMultiselectionSetting* self =
                        CMSRuleMultiselectionSetting::NewLC(
                                                    aIdentifier,
                                                    aTitleResource,
                                                    aFirstPopupItemResource,
                                                    aAnyItemTextResource,
                                                    aMetadataArray,
                                                    aArray,
                                                    aItemTextResource,
                                                    aText );

    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::NewLC
// --------------------------------------------------------------------------
//
CMSRuleMultiselectionSetting* CMSRuleMultiselectionSetting::NewLC(
                                TInt aIdentifier,
                                TInt aTitleResource,
                                TInt aFirstPopupItemResource,
                                TInt aAnyItemTextResource,
                                CCmSqlPropertyContainer* aMetadataArray,
                                RArray<TInt>* aArray,
                                TInt aItemTextResource,
                                TDes& aText)
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::NewLC"));

    CMSRuleMultiselectionSetting* self =
                        new (ELeave) CMSRuleMultiselectionSetting(
                                                    aIdentifier,
                                                    aTitleResource,
                                                    aFirstPopupItemResource,
                                                    aAnyItemTextResource,
                                                    aMetadataArray,
                                                    aArray,
                                                    aItemTextResource,
                                                    aText );

    CleanupStack::PushL(self);
    self->ConstructL();
    
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::ConstructL
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::ConstructL"));

    iItemArray =
            new ( ELeave ) CSelectionItemList( KItemArrayGranularity );

    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::SetSettingItemTextL
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::SetSettingItemTextL()
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::\
    SetSettingItemTextL"));

    HBufC* itemText = NULL;

    if ( iSelectedItemCount == 1 )
        {
        // only one item selected so we can use item index to find text
        CSelectableItem* apu = (*iItemArray)[ iSelectedItemIndex ];
        itemText = apu->ItemText().AllocLC();
        }
    else if ( iSelectedItemCount > 1 )
        {
        itemText = StringLoader::LoadLC( iItemTextResource,
                                         iSelectedItemCount );
        // do number conversion
        TPtr ptr = itemText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        }
    else
        {
        itemText = StringLoader::LoadLC( iAnyItemTextResource );
        }

    // Set new text
    SetExternalText( *itemText );
    // Load text
    LoadL();

    CleanupStack::PopAndDestroy( itemText );
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::CMSRuleMultiselectionSetting
// --------------------------------------------------------------------------
//
CMSRuleMultiselectionSetting::CMSRuleMultiselectionSetting(
    TInt aIdentifier,
    TInt aTitleResource,
    TInt aFirstPopupItemResource,
    TInt aAnyItemTextResource,
    CCmSqlPropertyContainer* aMetadataArray,
    RArray<TInt>* aArray,
    TInt aItemTextResource,
    TDes& aText
     ) :
    CMSTextSettingItem( aIdentifier, aText ),
    iItemTextResource ( aItemTextResource ),
    iText( aText ),
    iList (aArray),
    iMetadataArray( aMetadataArray ),
    iTitleResource( aTitleResource ),
    iFirstPopupItemResource( aFirstPopupItemResource ),
    iAnyItemTextResource( aAnyItemTextResource )
    {
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::~CMSRuleMultiselectionSetting()
// --------------------------------------------------------------------------
//
CMSRuleMultiselectionSetting::~CMSRuleMultiselectionSetting()
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::\
    ~CMSRuleMultiselectionSetting"));

    if ( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        }
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::EditItemL"));

    HBufC* title = StringLoader::LoadLC( iTitleResource );
    CMSMultiselectionDialog* dlg =
                    CMSMultiselectionDialog::NewL(iItemArray, *title);

    // reset dialog acceptance indicator
    SetAcceptState( EFalse );

    // launch dialog
    if ( dlg->ExecuteLD( R_MS_MULTISELECTION_DIALOG ) )
        {
        // dialog accepted
        SetAcceptState( ETrue );

        // reset selected item count
        iSelectedItemCount = 0;

        // remove previous items
        iList->Reset();

        // Check if first item ("Any") is selected
        if ( (*iItemArray)[ 0 ]->SelectionStatus() == EFalse )
            {
            AppendSelectedItemsToListL();
            }

        // set item text
        SetSettingItemTextL();
        // load and update text
        LoadL();
        UpdateListBoxTextL();
        }
    CleanupStack::PopAndDestroy( title );
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::CreateSelectionItemListL
// Creates selection item list
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::CreateSelectionItemListL()
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::\
    CreateSelectionItemListL"));

    // delete old items
    iItemArray->ResetAndDestroy();
    iSelectedItemCount = 0;

    // Cycle trough all metadata items
    iMetadataArray->SortPropertyItem();
    for ( TInt idx = 0; idx < iMetadataArray->PropertyItemCount(); idx++ )
        {
        CCmSqlPropertyItem* propertyItem =
                            iMetadataArray->PropertyItem( idx );

        HBufC* itemStr = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                        propertyItem->Name() );
        CleanupStack::PushL( itemStr );

        // Check if item was selected last time
        TInt found = iList->Find( propertyItem->Id() );

        CSelectableItem* item;

        if ( found != KErrNotFound )
            {
            // create item - selected
            item = new ( ELeave ) CSelectableItem( *itemStr, ETrue );
            iSelectedItemCount++;
            iSelectedItemIndex = idx + 1; //first item is "any"
            }
        else
            {
            // create item - not selected
            item = new ( ELeave ) CSelectableItem( *itemStr, EFalse );
            }

        CleanupStack::PushL( item );

        // item must be constructed
        item->ConstructL();

        // append item to list
        iItemArray->AppendL(item);

        CleanupStack::Pop( item );
        CleanupStack::PopAndDestroy( itemStr );
        }

    // Create and append default item (Any/None)
    HBufC* itemName = StringLoader::LoadLC( iFirstPopupItemResource );
    CSelectableItem* item = NULL;
    if ( iSelectedItemCount )
        {
        item = new ( ELeave ) CSelectableItem( *itemName, EFalse );
        }
    else
        {
        item = new ( ELeave ) CSelectableItem( *itemName, ETrue );
        }

    CleanupStack::PushL( item );
    item->ConstructL();
    iItemArray->InsertL(0, item);
    CleanupStack::Pop( item );
    CleanupStack::PopAndDestroy( itemName );

    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::AppendSelectedItemsToListL
// Appends selected items to the list
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::AppendSelectedItemsToListL()
    {
    LOG(_L("[MediaServant]\t CMSRuleMultiselectionSetting::\
    AppendSelectedItemsToListL"));

    // first index '0' is "Any"
    for ( TInt index = 1; index < iItemArray->Count(); index++ )
        {
        if ( (*iItemArray)[index]->SelectionStatus() )
            {
            CCmSqlPropertyItem* propertyItem =
                            iMetadataArray->PropertyItem( index - 1 );
            iList->AppendL( propertyItem->Id() );
            iSelectedItemCount++;
            iSelectedItemIndex = index;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSRuleMultiselectionSetting::CreateItemListL
// Creates item list and updates listbox
// --------------------------------------------------------------------------
//
void CMSRuleMultiselectionSetting::CreateItemListL()
    {
    // Create item list
    CreateSelectionItemListL();
    // set item text
    SetSettingItemTextL();
    // update listbox
    UpdateListBoxTextL();
    }

// End of File

