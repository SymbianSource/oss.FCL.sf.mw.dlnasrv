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
* Description:  CMSStoreServersSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>

#include "cmmediaserverfull.h"
#include "cmstorerule.h"
#include "cmfillrule.h"
#include "msstoreserverssetting.h"
#include "msmultiselectionsettingpage.h"
#include "msconstants.h"
#include "msappui.h"
#include "msengine.h"
#include "msdebug.h"

#include "upnpstring.h"

// CONSTANTS
const TInt KItemArrayGranularity = 3;

// --------------------------------------------------------------------------
// CMSStoreServersSetting::NewL
// --------------------------------------------------------------------------
//
CMSStoreServersSetting* CMSStoreServersSetting::NewL(
                                TInt aIdentifier,
                                CCmStoreRule* aRule,
                                CMSAppUi& aAppUi,
                                TDes& aText )
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::NewL"));

    CMSStoreServersSetting* self = CMSStoreServersSetting::NewLC(
                                             aIdentifier,
                                             aRule,
                                             aAppUi,
                                             aText );

    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::NewLC
// --------------------------------------------------------------------------
//
CMSStoreServersSetting* CMSStoreServersSetting::NewLC(
                                TInt aIdentifier,
                                CCmStoreRule* aRule,
                                CMSAppUi& aAppUi,
                                TDes& aText )
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::NewLC"));

    CMSStoreServersSetting* self = new (ELeave) CMSStoreServersSetting(
                                             aIdentifier,
                                             aRule,
                                             aAppUi,
                                             aText );

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }
    
// --------------------------------------------------------------------------
// CMSStoreServersSetting::ConstructL
// --------------------------------------------------------------------------
//
void CMSStoreServersSetting::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::ConstructL"));

    iSettingText = HBufC::NewL( KMaxFileName );

    // Get media server list
    iServers = iAppUi.MSEngine()->GetMediaServersL();

    // create item array
    CreateSelectionItemListL();

    SetSettingItemTextL();
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::CMSRuleServersSetting
// --------------------------------------------------------------------------
//
CMSStoreServersSetting::CMSStoreServersSetting(
    TInt aIdentifier,
    CCmStoreRule* aRule,
    CMSAppUi& aAppUi,
    TDes& aText
     ) :
    CAknTextSettingItem( aIdentifier, aText ),
    iRule( aRule ),
    iAppUi( aAppUi )
    {
    }
// --------------------------------------------------------------------------
// CMSStoreServersSetting::~CMSStoreServersSetting()
// --------------------------------------------------------------------------
//
CMSStoreServersSetting::~CMSStoreServersSetting()
    {
    delete iSettingText;

    if ( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        }

    if ( iServers )
        {
        iServers->ResetAndDestroy();
        iServers->Close();
        delete iServers;
        }
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSStoreServersSetting::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::EditItemL"));

    // Create setting page
    CAknSettingPage* dlg = CMSMultiselectionSettingPage::NewL(
            R_MS_STORE_SETTINGS_LOCATION_SETTING_PAGE,
            *iItemArray, EFalse );
    // launch setting page
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        DeleteAllServersFromRuleL();

        for ( TInt index = 0; index < iItemArray->Count(); index++ )
                {
                CSelectableItem* item = iItemArray->At( index );
                TBool selected = item->SelectionStatus();

                CCmMediaServerFull* server = (*iServers)[index];
                // get server UDN
                TPtrC8 serverUDN = server->MediaServer();

                // add server
                if ( item->SelectionStatus() )
                    {
                    // set server for rule
                    iRule->AddMediaServerL( serverUDN );
                    }
                }

        SetSettingItemTextL();
        LoadL();
        // show value on screen
        UpdateListBoxTextL();
        }
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::CreateSelectionItemListL
// Creates selection item list
// --------------------------------------------------------------------------
//
void CMSStoreServersSetting::CreateSelectionItemListL()
    {
    if ( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        iItemArray = NULL;
        }

    iItemArray =
            new ( ELeave ) CSelectionItemList( KItemArrayGranularity );

    TBuf8<KMaxFileName> itemText;

    TInt serverCount(0);
    if ( iServers )
        {
        serverCount = iServers->Count();
        }

    // Cycle trough all servers
    for ( TInt idx = 0; idx < serverCount; idx++ )
        {
        // get server
        CCmMediaServerFull* server = (*iServers)[idx];
        // show only copy capable devices
        if ( server->StoreUsage() )
            {
            HBufC* devName =
                    UpnpString::ToUnicodeL( server->MediaServerName() );
            CleanupStack::PushL( devName );
            CSelectableItem* item = new ( ELeave ) CSelectableItem(
                                                    *devName, EFalse);

            CleanupStack::PushL( item );
            // item must be constructed
            item->ConstructL();

            // count of servers set to rule
            TInt ruleServerCount = iRule->MediaServerCount();

            // Cycle trough all servers
            for (TInt index = 0; index < ruleServerCount; index++ )
                {
                const TDesC8& ruleServerUDN = iRule->MediaServerL( index );

                if (ruleServerUDN == server->MediaServer()) // compare UDNs
                    {
                    // found - break this loop
                    index = ruleServerCount;
                    // found - mark selected
                    item->SetSelectionStatus( ETrue );
                    iSelectedItemCount++;
                    }
                }
            // append item to list
            iItemArray->AppendL(item);
            CleanupStack::Pop( item );
            CleanupStack::PopAndDestroy( devName );
            }
        else    // we don't need this kind of servers anymore so remove it
            {
            delete server;
            iServers->Remove( idx );
            idx--; // remove transfers next item to current index
            serverCount--; // servers on the list were reduced
            }
        }
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::DeleteAllServersFromRuleL
// Deletes all servers from rule
// --------------------------------------------------------------------------
//
void CMSStoreServersSetting::DeleteAllServersFromRuleL()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::\
    CMSStoreServersSetting"));

    // count of servers set to rule
    TInt ruleServerCount = iRule->MediaServerCount();

    for ( TInt index = ruleServerCount-1; index >= 0; index-- )
        {
        const TDesC8& ruleServerUDN = iRule->MediaServerL( index );
        iRule->DeleteMediaServer( ruleServerUDN );
        }
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::CountSelectedItems
// Counts selected items
// --------------------------------------------------------------------------
//
TInt CMSStoreServersSetting::CountSelectedItems()
    {
    LOG(_L("[MediaServant]\t CMSRuleServersSetting::\
    CMSStoreServersSetting"));

    TInt count(0);

    for ( TInt index = 0; index < iItemArray->Count(); index++ )
        {
        if ( (*iItemArray)[index]->SelectionStatus() )
            {
            count++;
            }
        }

    return count;
    }

// --------------------------------------------------------------------------
// CMSStoreServersSetting::SetSettingItemTextL
// Sets setting item secondary text according to selected items
// --------------------------------------------------------------------------
//
void CMSStoreServersSetting::SetSettingItemTextL()
    {
        TInt count = CountSelectedItems();

        HBufC* itemText = NULL;
        // no items selected
        if ( count == 0 )
            {
            itemText = StringLoader::LoadLC( R_MS_NO_SELECTED_ITEMS );
            iSettingText->Des().Copy( *itemText );
            }
        // just one item selected
        else if ( count == 1 )
            {
            TInt itemCount = iItemArray->Count();
            for ( TInt index = 0; index < itemCount; index++ )
                {
                CSelectableItem* item = (*iItemArray)[ index ];
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

