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
* Description:  CMSStoreListView class implementation
*
*/



#include <mediaservant.rsg>
#include <hlplch.h>
#include <StringLoader.h>

#include "msstorelistcontainer.h"
#include "mediaservant.hrh"
#include "msconstants.h"
#include "msstorelistview.h"
#include "msappui.h"
#include "msengine.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSStoreListView::CMSStoreListView()
// --------------------------------------------------------------------------
//
CMSStoreListView::CMSStoreListView( CMSAppUi& aAppUi ) :
    iAppUi( aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::CMSStoreListView"));
    }

// --------------------------------------------------------------------------
// CMSStoreListView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSStoreListView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::ConstructL"));

    BaseConstructL( R_MS_STORE_LIST_VIEW );
    }

// --------------------------------------------------------------------------
// CMSStoreListView::~CMSStoreListView()
// --------------------------------------------------------------------------
//
CMSStoreListView::~CMSStoreListView()
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::~CMSStoreListView"));

    DoDeactivate();
    }

// --------------------------------------------------------------------------
// TUid CMSStoreListView::Id()
// --------------------------------------------------------------------------
//
TUid CMSStoreListView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::Id"));

    return KMSStoreListViewId;
    }

// --------------------------------------------------------------------------
// CMSStoreListView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSStoreListView::HandleCommandL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::HandleCommandL"));

    switch (aCommand)
        {
        case EAknSoftkeyBack:
            {
            CancelAsyncOperation( &iAppUi );

            // set default icon back
            iContainer->SetDefaultContextPaneIconL();
            // update store rule for mainview redraw
            iContainer->SetServerSelectionsL();
            iContainer->UpdateRulesToServerL();
            // back to main view
            iAppUi.ChangeViewL( KMSStoreListViewId, KMSMainViewId );
            break;
            }

        case EMSStoreCmdActivate: // fall through
        case EMSStoreCmdDeActivate:
            {
            iContainer->ToggleCurrentItemL();
            break;
            }

        case EMSStoreCmdEdit:
            {
            CancelAsyncOperation( &iAppUi );
            // update store rule for mainview redraw
            iContainer->SetServerSelectionsL();
            iContainer->UpdateRulesToServerL();

            iAppUi.ChangeViewL( KMSStoreListViewId, KMSStoreSettingsViewId );
            break;
            }

        case EMSCmdBrowse:
            {
            CancelAsyncOperation( &iAppUi );

            // update store rule for mainview redraw
            iContainer->SetServerSelectionsL();
            iContainer->UpdateRulesToServerL();

            // set browse target
            iAppUi.ParameterAgent()->SetBrowseTarget( EMSStoredItems );
            iAppUi.ChangeViewL( KMSStoreListViewId, KMSBrowseViewId );
            break;
            }

        case EAknCmdHelp :
            {
            TRAPD( error, HlpLauncher::LaunchHelpApplicationL(
                        iEikonEnv->WsSession(),
            iEikonEnv->EikAppUi()->AppHelpContextL() ));
            if ( error != KErrNone )
                {
                LOG(_L("[MediaServant]\t CMSStoreListView:: \
                                HandleCommandL help leaves in \
                                LaunchHelpApplication"));
                }
            break;
            }

        default:
            {
            CancelAsyncOperation( &iAppUi );
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }



// --------------------------------------------------------------------------
// CMSStoreListView::DoActivateL(...)
// --------------------------------------------------------------------------
//
void CMSStoreListView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::DoActivateL"));

    // Set engine observer
    iAppUi.MSEngine()->SetObserver( this );
    // start preprosessing if returned from edit view
    if ( iAppUi.ParameterAgent()->PreviousViewId() ==
         KMSStoreSettingsViewId )
        {
        iAppUi.MSEngine()->ExecuteStorePreProcessingL();
        HBufC* naviText = StringLoader::LoadLC( R_MS_PREPROCESSING_TEXT );
        SetNavigationPaneTextL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
		iPreprocessingState = ETrue;
        }


    // Set title pane text to default
    SetTitlePaneTextL( R_MS_STORE_LIST_VIEW_TITLE );

    if ( !iContainer )
        {
        iContainer = new (ELeave) CMSStoreListContainer( iAppUi, *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
   }

// --------------------------------------------------------------------------
// CMSStoreListView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSStoreListView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    ClearCurrentNaviPaneText();

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;
    }

// --------------------------------------------------------------------------
// CMSStoreListView::FocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSStoreListView::FocusIndex() const
    {
    return iContainer->SelectedItemIndex();
    }

// --------------------------------------------------------------------------
// CMSStoreListView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CMSStoreListView::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane)
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::DynInitMenuPaneL"));

    if ( aResourceId == R_MS_STORE_LIST_MENU )
        {
        // selection command
        if ( iContainer->CurrentListItemSelectionStatus() )
            {
            aMenuPane->SetItemDimmed( EMSStoreCmdActivate, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMSStoreCmdDeActivate, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSStoreListView::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSStoreListView::ReadyL( TCmService aService, TInt /*aError*/ )
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::ReadyL"));

    if ( aService == ECmServicePreProcessingStore )
        {
        CreateAndSetNaviTextL();
		iPreprocessingState = EFalse;
        }
    }


// --------------------------------------------------------------------------
// CMSStoreListView::CreateAndSetNaviTextL()
// Creates and sets navi pane text
// --------------------------------------------------------------------------
//
void CMSStoreListView::CreateAndSetNaviTextL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListView::CreateAndSetNaviTextL"));

    HBufC* naviText = NULL;

    TInt count = iContainer->ListItemCountL();

    switch ( count )
        {
        case 0: // 0 items
            {
            naviText = StringLoader::LoadLC(
                                R_MS_NO_ITEMS_TO_BE_SYNCED );
            break;
            }
        case 1: // 1 item
            {
            naviText = StringLoader::LoadLC(
                                R_MS_1_ITEM_TO_BE_SYNCED );
            break;
            }
        default: // many items
            {
            naviText = StringLoader::LoadLC(
                                R_MS_N_ITEMS_TO_BE_SYNCED,
                                count );
            // do number conversion
            TPtr ptr = naviText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            break;
            } 

        }
    // update navi pane
    SetNavigationPaneTextL( *naviText );

    CleanupStack::PopAndDestroy( naviText );
    }

// --------------------------------------------------------------------------
// CMSStoreListView::PreProcessing()
// Returns preprocessing state
// --------------------------------------------------------------------------
//
TBool CMSStoreListView::PreProcessing()
	{
	return iPreprocessingState;
	}
	
// End of File

