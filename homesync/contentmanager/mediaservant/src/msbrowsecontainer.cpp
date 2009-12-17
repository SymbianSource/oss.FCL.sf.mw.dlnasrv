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
* Description:  CMSBrowseContainer class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>
#include "upnpstring.h"

#include "mserv.hlp.hrh"
#include "cmstorerulecontainer.h"
#include "cmstorerule.h"
#include "msappui.h"
#include "msstorelistcontroller.h"
#include "msbrowseview.h"
#include "msbrowsecontainer.h"
#include "msengine.h"
#include "mediaservantuid.h"

#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSBrowseContainer::CMSBrowseContainer()
// --------------------------------------------------------------------------
//
CMSBrowseContainer::CMSBrowseContainer( CMSAppUi& aAppUi,
                                        CMSBrowseView& aView )
    {
    LOG(_L("[MediaServant]\t CMSBrowseContainer::CMSBrowseContainer"));

    iAppUi = &aAppUi;
    iView = &aView;    
    }

// --------------------------------------------------------------------------
// CMSBrowseContainer::~CMSBrowseContainer()
// --------------------------------------------------------------------------
//
CMSBrowseContainer::~CMSBrowseContainer()
    {
    LOG(_L("[MediaServant]\t CMSBrowseContainer::~CMSBrowseContainer"));

    delete iListBox; // Deletes listbox object.
    }

// --------------------------------------------------------------------------
// CMSBrowseContainer::GetBrowseDataL()
// Gets file information from engine
// --------------------------------------------------------------------------
//
void CMSBrowseContainer::GetBrowseDataL()
    {
    LOG(_L("[MediaServant]\t CMSBrowseContainer::GetBrowseDataL"));

    CMSParameterAgent* parameterAgent = iAppUi->ParameterAgent();

    TInt selected = iAppUi->ParameterAgent()->StoreViewFocusIndex();

    // get list container
    CCmStoreRuleContainer* storeRuleContainer =
                iAppUi->StoreListController()->StoreListContainer();

    // get rule
    CCmStoreRule* storeRule = storeRuleContainer->StoreRule( selected );

    // read media type
    storeRule->StoreRule(0, &iMediaType);

    iView->SetTitlePaneTextL( iMediaType );

    // get items covered by rule
    iItemArray = iAppUi->MSEngine()->GetStoreListItemsL( storeRule->Name() );

    HBufC* naviText = NULL;

    switch ( iItemArray->Count() )
        {
        case 0: // 0 items
            {
            naviText = StringLoader::LoadLC(
                                R_MS_STORE_BROWSE_NAVI_0_ITEMS );
            break;
            }
        case 1: // 1 item
            {
            naviText = StringLoader::LoadLC(
                                R_MS_STORE_BROWSE_NAVI_1_ITEM );
            break;
            }
        default: // many items
            {
            naviText = StringLoader::LoadLC(
                                R_MS_STORE_BROWSE_NAVI_ITEMS,
                                iItemArray->Count() );
            // do number conversion
            TPtr ptr = naviText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            break;
            }
        }

    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    }

// --------------------------------------------------------------------------
// CMSBrowseContainer::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSBrowseContainer::HandleResourceChange( TInt aType )
    {
    LOG(_L("[MediaServant]\t CMSBrowseContainer::HandleResourceChange"));

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------------------------
// CMSBrowseContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSBrowseContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSBrowseContainer::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_FILE_BROWSE;
    }

// End of File

