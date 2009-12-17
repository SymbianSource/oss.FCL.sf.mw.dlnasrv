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
* Description:  CMSBrowseView class implementation
*
*/



#include <mediaservant.rsg>
#include <StringLoader.h>
#include <hlplch.h>

#include "msbrowsecontainer.h"
#include "msconstants.h"
#include "msbrowseview.h"
#include "msappui.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSBrowseView::CMSBrowseView()
// --------------------------------------------------------------------------
//
CMSBrowseView::CMSBrowseView( CMSAppUi& aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::CMSBrowseView"));

    iAppUi = &aAppUi;
    }

// --------------------------------------------------------------------------
// CMSBrowseView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSBrowseView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::ConstructL"));

    BaseConstructL( R_MS_BROWSE_VIEW );
    }

// --------------------------------------------------------------------------
// CMSBrowseView::~CMSBrowseView()
// --------------------------------------------------------------------------
//
CMSBrowseView::~CMSBrowseView()
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::~CMSBrowseView"));

    DoDeactivate();
    }
// --------------------------------------------------------------------------
// TUid CMSBrowseView::Id()
// --------------------------------------------------------------------------
//
TUid CMSBrowseView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::Id"));

    return KMSBrowseViewId;
    }

// --------------------------------------------------------------------------
// CMSBrowseView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSBrowseView::HandleCommandL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::HandleCommandL"));

    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            // back to store view
            iAppUi->ChangeViewL( KMSBrowseViewId, KMSStoreListViewId );
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
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }



// --------------------------------------------------------------------------
// CMSBrowseView::DoActivateL()
// --------------------------------------------------------------------------
//
void CMSBrowseView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::DoActivateL"));

    // Set title pane text to default
    CMSBaseView::SetTitlePaneTextL( KMSDefaultTitleId );

    if ( !iContainer )
        {
        iContainer = new (ELeave) CMSBrowseContainer( *iAppUi, *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        iContainer->ActivateL();
        }
   }

// --------------------------------------------------------------------------
// CMSBrowseView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSBrowseView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;

    ClearCurrentNaviPaneText();
    }

// --------------------------------------------------------------------------
// CMSBrowseView::SetTitlePaneTextL()
// --------------------------------------------------------------------------
//
void CMSBrowseView::SetTitlePaneTextL( TCmMediaType aMediaType )
    {
    LOG(_L("[MediaServant]\t CMSBrowseView::SetTitlePaneTextL"));

    TInt resourceId(0);

    switch ( aMediaType )
        {
        case ECmImage:
            {
            resourceId = R_MS_STORE_TITLE_PHONE_IMAGES;
            break;
            }
        case ECmOtherImage:
            {
            resourceId = R_MS_STORE_TITLE_IMAGES;
            break;
            }
        case ECmVideo:
            {
            resourceId = R_MS_STORE_TITLE_PHONE_VIDEOS;
            break;
            }
        case ECmOtherVideo:
            {
            resourceId = R_MS_STORE_TITLE_VIDEOS;
            break;
            }
        case ECmAudio:
            {
            resourceId = R_MS_STORE_TITLE_MUSIC;
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSBrowseView::SetTitlePaneTextL \
            mediatype not found"));
            break;
            }
        }

    if ( resourceId )
        {
        HBufC* titleText = StringLoader::LoadLC( resourceId );
        CMSBaseView::SetTitlePaneTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }
    }
    
// End of File

