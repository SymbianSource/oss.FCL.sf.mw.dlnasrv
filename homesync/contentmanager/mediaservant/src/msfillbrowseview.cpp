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
* Description:  CMSFillBrowseView class implementation
*
*/



#include <mediaservant.rsg>
#include <StringLoader.h>
#include <hlplch.h>

#include "msfillbrowsecontainer.h"
#include "msconstants.h"
#include "msfillbrowseview.h"
#include "msappui.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSFillBrowseView::CMSFillBrowseView()
// --------------------------------------------------------------------------
//
CMSFillBrowseView::CMSFillBrowseView(CMSAppUi& aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::CMSFillBrowseView"));

    iAppUi = &aAppUi;
    }

// --------------------------------------------------------------------------
// CMSFillBrowseView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSFillBrowseView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::ConstructL"));

    BaseConstructL( R_MS_FILL_BROWSE_VIEW );
    }

// --------------------------------------------------------------------------
// CMSFillBrowseView::~CMSFillBrowseView()
// --------------------------------------------------------------------------
//
CMSFillBrowseView::~CMSFillBrowseView()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::~CMSFillBrowseView"));

    DoDeactivate();
    }
// --------------------------------------------------------------------------
// TUid CMSFillBrowseView::Id()
// --------------------------------------------------------------------------
//
TUid CMSFillBrowseView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::Id"));

    return KMSFillBrowseViewId;
    }

// --------------------------------------------------------------------------
// CMSFillBrowseView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSFillBrowseView::HandleCommandL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::HandleCommandL"));

    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            // update status before exit
            iContainer->SetFileStatusesL();            
            // back to fill list view
            iAppUi->ChangeViewL( KMSBrowseViewId, KMSFillViewId );
            break;
            }

        case EMSCmdLock:
            {
            iContainer->SetStatusOfFiles( ECmLocalCopy );
            break;
            }

        case EMSCmdUnLock:
            {
            iContainer->SetStatusOfFiles( ECmFilled );
            break;
            }

        case EMSCmdRandomize:
            {
            iContainer->RandomizeListL();
            break;
            }

        case EAknCmdMark:   // fall through
        case EAknCmdUnmark: // fall through
        case EAknMarkAll:   // fall through
        case EAknUnmarkAll:
            {
            // Gets pointer of current listbox.
            CEikListBox* listbox = iContainer->ListBox();

            if ( listbox )
                {
                AknSelectionService::HandleMarkableListProcessCommandL(
                                                        aCommand, listbox );
                }
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
            // update status before exit
            iContainer->SetFileStatusesL();

            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }



// --------------------------------------------------------------------------
// CMSFillBrowseView::DoActivateL(...)
// --------------------------------------------------------------------------
//
void CMSFillBrowseView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::DoActivateL"));

    // Set title pane text to default
    SetTitlePaneTextL( KMSDefaultTitleId );

    if ( !iContainer )
        {
        iContainer = new (ELeave) CMSFillBrowseContainer(
                                                    *iAppUi,
                                                    *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        iContainer->ActivateL();

        // marked item indexes
        iSelections = iContainer->ListBox()->SelectionIndexes();

        }
   }

// --------------------------------------------------------------------------
// CMSFillBrowseView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSFillBrowseView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;

    ClearCurrentNaviPaneText();
    }


// --------------------------------------------------------------------------
// CMSFillBrowseView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CMSFillBrowseView::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane)
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseView::DynInitMenuPaneL"));

    TInt numberOfItems = iContainer->ListBox()->Model()->NumberOfItems();

    if ( aResourceId == R_MS_FILL_BROWSE_MENU )
        {
        // check if random lists exist or there is no items
        if ( iContainer->HasListRandomRule() == EFalse )
            {
            aMenuPane->SetItemDimmed( EMSCmdRandomize, ETrue );
            }

        if ( numberOfItems )
            {
            TCmListItemStatus fileStatus = iContainer->FileStatus(
                                iContainer->ListBox()->CurrentItemIndex() );

            if (  fileStatus == ECmToBeFilled ||
                  fileStatus == ECmToBeShrinked )
                {
                aMenuPane->SetItemDimmed( EMSCmdUnLock, ETrue );
                aMenuPane->SetItemDimmed( EMSCmdLock, ETrue );
                aMenuPane->SetItemDimmed( EMSCmdMark, ETrue );
                }
            else
                {
                if ( fileStatus == ECmFilled )
                    {
                    aMenuPane->SetItemDimmed( EMSCmdUnLock, ETrue );
                    }
                else // ECmLocalCopy
                    {
                    aMenuPane->SetItemDimmed( EMSCmdLock, ETrue );
                    }
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( EMSCmdLock, ETrue );
            aMenuPane->SetItemDimmed( EMSCmdUnLock, ETrue );
            aMenuPane->SetItemDimmed( EMSCmdMark, ETrue );
            }
        }

    // mark submenu
    if ( aResourceId == R_MS_MARK_MENUPANE && numberOfItems )
        {

        if ( iSelections->Count() == 0 )
            {
            aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );
            aMenuPane->SetItemDimmed( EAknUnmarkAll, ETrue );
            }
        else
            {
            // index of selected item
            TInt itemIdx = 0;
            // highlighted item index
            TInt currentItem = iContainer->ListBox()->CurrentItemIndex();
            // Sort criteria, offset = 0
            TKeyArrayFix sortKey( 0, ECmpTUint );
            TInt found = iSelections->Find( currentItem, sortKey, itemIdx );
            if ( found == 0 )
                {
                aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
                }
            }
        }
    }

// End of File

