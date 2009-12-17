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
* Description:  CMSStoreSettingsList class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>

#include "cmsqlpropertycontainer.h"
#include "mserv.hlp.hrh"
#include "cmstorerule.h"
#include "cmstorerulecontainer.h"
#include "msengine.h"
#include "msstorelistcontroller.h"
#include "msappui.h"
#include "msstoresettingslist.h"
#include "msstoresettingsview.h"
#include "msconstants.h"
#include "msstoreserverssetting.h"        // Servers
#include "msstorekeeponphonesetting.h"    // Keep on phone
#include "mediaservant.hrh"
#include "mediaservantuid.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSStoreSettingsList::NewL()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSStoreSettingsList* CMSStoreSettingsList::NewL( TInt aResourceId,
                CMSAppUi& aAppUi, CMSStoreSettingsView& aView )
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::NewL"));
    
    CMSStoreSettingsList* self =
                    CMSStoreSettingsList::NewLC( aResourceId,
                                                 aAppUi,
                                                 aView );
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::NewLC()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSStoreSettingsList* CMSStoreSettingsList::NewLC( TInt aResourceId,
                CMSAppUi& aAppUi, CMSStoreSettingsView& aView )
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::NewLC"));
    
    CMSStoreSettingsList* self =
                    new ( ELeave ) CMSStoreSettingsList( aAppUi, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId );
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::CMSStoreSettingsList()
// --------------------------------------------------------------------------
//
CMSStoreSettingsList::CMSStoreSettingsList( CMSAppUi& aAppUi,
                                            CMSStoreSettingsView& aView )
    : iAppUi( aAppUi ), iView( aView )
    {
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::~CMSStoreSettingsList()
// --------------------------------------------------------------------------
//
CMSStoreSettingsList::~CMSStoreSettingsList()
    {
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::ConstructL()
// Second phase constructor.
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::ConstructL( TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::ConstructL"));

    iAppUi.MSEngine()->SetObserver( this );

    // get selected item index from previous screen
    TInt selected = iAppUi.ParameterAgent()->StoreViewFocusIndex();
    
    // get list container
    iStoreRuleContainer =
            iAppUi.StoreListController()->StoreListContainer();
    
    // get rule
    iStoreRule = iStoreRuleContainer->StoreRule( selected );

    // read media type
    iStoreRule->StoreRule(0, &iMediaType);

    // Set title pane
    SetTitlePaneTextL();

    // Read rule status
    ReadRuleStatus();

    CAknSettingItemList::ConstructFromResourceL( aResourceId );
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::SaveKeepOnPhoneSetting
// Save value to rule
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::SaveKeepOnPhoneSetting()
    {
    // KEEP ON PHONE
    switch ( iKeepOnPhone )
        {
        case EMSDontKeep:
            {
            iStoreRule->SetStatus( ECmToBeRemoved );
            break;
            }
        case EMSKeep:
            {
            iStoreRule->SetStatus( ECmKeepOnDevice );
            break;
            }
        case EMSKeepOriginalSize:
            {
            iStoreRule->SetStatus( ECmKeepOnDevice );
            break;
            }
        case EMSKeepScreenSize:
            {
            iStoreRule->SetStatus( ECmToBeShrinked );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSStoreSettingsList::\
            SaveValues keep on phone setting not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::ReadRuleStatus
// Reads rule status
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::ReadRuleStatus()
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::ReadRuleStatus"));

    switch ( iStoreRule->Status() )
        {
        case ECmToBeRemoved:
            {
            iKeepOnPhone = EMSDontKeep;
            break;
            }
        case ECmKeepOnDevice:
            {
            if ( iMediaType == ECmImage || iMediaType == ECmOtherImage )
                {
                iKeepOnPhone = EMSKeepOriginalSize;
                }
            else
                {
                iKeepOnPhone = EMSKeep;
                }
            break;
            }
        case ECmToBeShrinked:
            {
            iKeepOnPhone = EMSKeepScreenSize;
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSStoreSettingsList::\
            SaveValues keep on phone setting not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CAknSettingItem* CMSStoreSettingsList::CreateSettingItemL()
// Takes care of creating actual setting items.
// --------------------------------------------------------------------------
//
CAknSettingItem* CMSStoreSettingsList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* settingItem = NULL;

    // setting item is created according to aIdentifier
    switch ( aIdentifier )
        {
        case EMSKeepOnPhone:
            {
            settingItem = new ( ELeave ) CMSStoreKeepOnPhoneSetting(
                                            aIdentifier,
                                            iMediaType,
                                            iKeepOnPhone );
            break;
            }

        case EMSSourceServers:
            {
            settingItem = CMSStoreServersSetting::NewL(
                                            aIdentifier,
                                            iStoreRule,
                                            iAppUi,
                                            iDummyText );
            break;
            }
        default:
            {
            LOG( _L( "[MediaServant]\t CMSStoreSettingsList::\
            CreateSettingItemL invalid setting item" ) );
            break;
            }
        }
    return settingItem;
    }


// ---------------------------------------------------------
// CMSStoreSettingsList::OpenSelectedListboxItemL
// ---------------------------------------------------------
//
void CMSStoreSettingsList::OpenSelectedListboxItemL()
    {
    HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
    }


// --------------------------------------------------------------------------
// CMSStoreSettingsList::HandleListBoxEventL()
// Handles listbox events
// --------------------------------------------------------------------------
//
void  CMSStoreSettingsList::HandleListBoxEventL ( CEikListBox *aListBox,
                                                 TListBoxEvent aEventType)
    {
    // Base class call
    CAknSettingItemList::HandleListBoxEventL ( aListBox, aEventType );
    // values need to be stored
    StoreSettingsL();


    CAknSettingItemArray* itemArray = SettingItemArray();

    TInt currentItem = ListBox()->CurrentItemIndex();
    if ( currentItem < itemArray->Count() )
        {
        CAknSettingItem* item= (*itemArray)[currentItem];

        // show add fill rule query if last item "Add more rules" selected
        if ( item->Identifier() == EMSKeepOnPhone )
            {
            SaveKeepOnPhoneSetting();
            }
        }
    }


// --------------------------------------------------------------------------
// CMSStoreSettingsList::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::ReadyL( TCmService aService, TInt /*aError*/ )
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::ReadyL"));

    switch ( aService )
        {

        case ECmServiceStore:
            {
            // stop showing 'reading date text
            iView.ClearCurrentNaviPaneText();
            break;
            }

        default:
            {
            LOG(_L("[MediaServant]\t CMSStoreSettingsList::ReadyL \
            service not found"));
            break;
            }
        }

    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::SetTitlePaneTextL
// Sets title pane text
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::SetTitlePaneTextL()
    {
    LOG( _L( "[MediaServant]\t CMSStoreSettingsList::SetTitlePaneTextL" ) );

    HBufC* title(0);

    switch ( iMediaType )
        {
        case ECmVideo:
            {
            title = StringLoader::LoadLC( R_MS_STORE_TITLE_PHONE_VIDEOS );
            break;
            }
        case ECmOtherVideo:
            {
            title = StringLoader::LoadLC( R_MS_STORE_TITLE_VIDEOS );
            break;
            }
        case ECmOtherImage:
            {
            title = StringLoader::LoadLC( R_MS_STORE_TITLE_IMAGES );
            break;
            }
        case ECmAudio:
            {
            title = StringLoader::LoadLC( R_MS_STORE_TITLE_MUSIC );
            break;
            }
        case ECmImage:
            {
            title = StringLoader::LoadLC( R_MS_STORE_TITLE_PHONE_IMAGES );
            break;
            }
        default:
            {
            LOG( _L( "[MediaServant]\t CMSStoreSettingsList::\
            SetTitlePaneTextL invalid mediatype" ) );
            break;
            }
        }

    if ( title )
        {
        // Set title pane text
        iView.SetTitlePaneTextL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::SizeChanged()
    {
    ListBox()->SetRect( Rect() ); // Set rectangle of listbox.
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsList::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSStoreSettingsList::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView.ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------------------------
// CMSStoreSettingsList::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSStoreSettingsList::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsList::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_STORE_SETTINGS;
    }

// End of File

