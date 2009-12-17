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
* Description:  CMSRuleServersSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>


#include "msruleserverssetting.h"
#include "msmultiselectionsettingpage.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSRuleServersSetting::NewL
// --------------------------------------------------------------------------
//
CMSRuleServersSetting* CMSRuleServersSetting::NewL(
                                TInt aIdentifier,
                                CSelectionItemList& aServerList,
                                TDes& aText,
                                TInt aSecondaryTextResource )
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::NewL"));

    CMSRuleServersSetting* self = CMSRuleServersSetting::NewLC(
                                             aIdentifier,
                                             aServerList,
                                             aText,
                                             aSecondaryTextResource );

    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::NewLC
// --------------------------------------------------------------------------
//
CMSRuleServersSetting* CMSRuleServersSetting::NewLC(
                                TInt aIdentifier,
                                CSelectionItemList& aServerList,
                                TDes& aText,
                                TInt aSecondaryTextResource )
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::NewLC"));

    CMSRuleServersSetting* self = new (ELeave) CMSRuleServersSetting(
                                             aIdentifier,
                                             aServerList,
                                             aText,
                                             aSecondaryTextResource );

    CleanupStack::PushL(self);
    self->ConstructL();
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::ConstructL
// --------------------------------------------------------------------------
//
void CMSRuleServersSetting::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::ConstructL"));

    iSettingText = HBufC16::NewL( KMaxFileName );

    SetSettingItemTextL();
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::CMSRuleServersSetting
// --------------------------------------------------------------------------
//
CMSRuleServersSetting::CMSRuleServersSetting(
    TInt aIdentifier,
    CSelectionItemList& aServerList,
    TDes& aText,
    TInt aSecondaryTextResource
     ) :
    CMSTextSettingItem( aIdentifier, aText ),
    iText( aText ),
    iItemArray( aServerList ),
    iSecondaryTextResource( aSecondaryTextResource )
    {
    }
// --------------------------------------------------------------------------
// CMSRuleServersSetting::~CMSRuleServersSetting()
// --------------------------------------------------------------------------
//
CMSRuleServersSetting::~CMSRuleServersSetting()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::\
    ~CMSRuleServersSetting"));

    delete iSettingText;
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSRuleServersSetting::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::EditItemL"));

    // reset dialog acceptance indicator
    SetAcceptState( EFalse );

    // Create setting page
    CAknSettingPage* dlg = CMSMultiselectionSettingPage::NewL(
            R_MS_FILL_RULE_SERVERS_SETTING_PAGE,
            iItemArray, ETrue );
    // launch setting page
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        // dialog accepted
        SetAcceptState( ETrue );

        SetSettingItemTextL();
        LoadL();
        // show value on screen
        UpdateListBoxTextL();
        }
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::CountSelectedItems
// Counts selected items
// --------------------------------------------------------------------------
//
TInt CMSRuleServersSetting::CountSelectedItems()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::\
    CountSelectedItems"));

    TInt count(0);

    // don't calculate first item
    for ( TInt index = 1; index < iItemArray.Count(); index++ )
        {
        if ( iItemArray[ index ]->SelectionStatus() )
            {
            count++;
            }
        }

    return count;
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::SetSettingItemTextL
// Sets setting item secondary text according to selected items
// --------------------------------------------------------------------------
//
void CMSRuleServersSetting::SetSettingItemTextL()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::\
    SetSettingItemTextL"));

    TInt count = CountSelectedItems();

    HBufC* itemText = NULL;
    // Check if there is no selected items
    if ( count == 0 )
        {
        itemText = StringLoader::LoadLC( iSecondaryTextResource );
        iSettingText->Des().Copy( *itemText );
        }
    // Check if only one item is selected
    else if ( count == 1 )
        {
        TInt itemCount = iItemArray.Count();
        for ( TInt index = 1; index < itemCount; index++ )
            {
            CSelectableItem* item = iItemArray[ index ];
            if ( item->SelectionStatus() )
                {
                iSettingText->Des().Copy( item->ItemText() );
                index = itemCount; // break loop
                }
            }
        }
    // more than one item selected
    else
        {
        itemText = StringLoader::LoadLC( R_MS_ITEM_DEVICES, count );

        // do number conversion
        TPtr ptr = itemText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        iSettingText->Des().Copy( *itemText );
        }

    if ( itemText )
        {
        CleanupStack::PopAndDestroy( itemText );
        }

     // Set new value
    SetExternalText( *iSettingText );
    }

// End of File

