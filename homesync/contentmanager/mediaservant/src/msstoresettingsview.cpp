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
* Description:  CMSStoreSettingsView class implementation
*
*/


#include <mediaservant.rsg>
#include <hlplch.h>

#include "msstoresettingslist.h"
#include "msstorelistcontroller.h"
#include "msengine.h"
#include "msstoresettingsview.h"
#include "msappui.h"
#include "msconstants.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSStoreSettingsView::CMSStoreSettingsView()
// --------------------------------------------------------------------------
//
CMSStoreSettingsView::CMSStoreSettingsView( CMSAppUi& aAppUi ) :
    iAppUi( aAppUi )
    {
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSStoreSettingsView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::ConstructL"));

    BaseConstructL( R_MS_STORE_SETTINGS_VIEW );
    }

// --------------------------------------------------------------------------
// CMSStoreSettingsView::~CMSStoreSettingsView()
// --------------------------------------------------------------------------
//
CMSStoreSettingsView::~CMSStoreSettingsView()
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::~CMSFillRuleEditView"));

    DoDeactivate();
    }

// --------------------------------------------------------------------------
// TUid CMSStoreSettingsView::Id()
// --------------------------------------------------------------------------
//
TUid CMSStoreSettingsView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::Id"));

    return KMSStoreSettingsViewId;
    }


// --------------------------------------------------------------------------
// CMSStoreSettingsView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSStoreSettingsView::HandleCommandL(TInt aCommand)
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::HandleCommandL"));

    switch (aCommand)
        {
        case EAknCmdOpen:
            {
            iContainer->OpenSelectedListboxItemL();
            break;
            }
        case EAknSoftkeyDone:
            {
            // getting data not yet finished so we must cancel the operation
            TCmServerState state;
            iAppUi.MSEngine()->ServerState( state );
            if ( state == ECmServerStateGettingStoreFields )
                {
                LOG( _L( "[MediaServant]\t CMSFillRuleEditList::\
                ~CMSFillRuleEditList cancel operation" ) );
                iAppUi.MSEngine()->StopOperation();
                ClearCurrentNaviPaneText();
                }

            // Get store list container
            CCmStoreRuleContainer* storeRuleContainer =                                            
                        iAppUi.StoreListController()->StoreListContainer();

            // Set rules to server
            iAppUi.MSEngine()->SetStoreRulesL( storeRuleContainer );

            iAppUi.ChangeViewL( KMSStoreSettingsViewId, KMSStoreListViewId );
            break;
            }
        case EAknCmdHelp :
            {
            TRAP_IGNORE( HlpLauncher::LaunchHelpApplicationL(
                        iEikonEnv->WsSession(),
            iEikonEnv->EikAppUi()->AppHelpContextL() ));
            break;
            }
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }


// --------------------------------------------------------------------------
// CMSStoreSettingsView::DoActivateL()
// --------------------------------------------------------------------------
//
void CMSStoreSettingsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::DoActivateL"));

    if ( !iContainer )
        {
        // create settings screen
        iContainer = CMSStoreSettingsList::NewL( R_MS_STORE_SETTINGS_LIST,
                                                 iAppUi,
                                                 *this );
        iContainer->SetMopParent( this );

        // now control receives keyboard events
        iAppUi.AddToStackL( *this, iContainer );
        iContainer->ActivateL();
        }
   }

// --------------------------------------------------------------------------
// CMSStoreSettingsView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSStoreSettingsView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSStoreSettingsView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;
    }

// End of File

