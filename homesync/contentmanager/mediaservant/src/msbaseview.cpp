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
* Description:  CMSBaseView class implementation
*
*/



// INCLUDE FILES
#include <akntitle.h>
#include <eikspane.h>
#include <avkon.hrh>
#include <aknnavide.h>
#include <barsread.h>

#include "msbaseview.h"
#include "msconstants.h"
#include "msappui.h"
#include "msengine.h"

#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSBaseView::CMSBaseView()
// Default constructor.
// --------------------------------------------------------------------------
//
CMSBaseView::CMSBaseView()
    {
    LOG(_L("[MediaServant]\t CMSBaseView::CMSBaseView"));
    }

// --------------------------------------------------------------------------
// CMSBaseView::~CMSBaseView()
// Destructor.
// --------------------------------------------------------------------------
//
CMSBaseView::~CMSBaseView()
    {
    LOG(_L("[MediaServant]\t CMSBaseView::~CMSBaseView"));
    ClearCurrentNaviPaneText();
    }

// --------------------------------------------------------------------------
// CMSBaseView::SetTitlePaneTextL()
// Sets text to title pane.
// --------------------------------------------------------------------------
void CMSBaseView::SetTitlePaneTextL( TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSBaseView::SetTitlePaneTextL"));

    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    // Makes and sets text which is used title pane.
    if ( aResourceId == KMSDefaultTitleId )
        {
        titlePane->SetTextToDefaultL();
        }
    else
        {
        TBuf<KMSTitleBufLength> titleText( 0 );
        iCoeEnv->ReadResourceL( titleText, aResourceId );
        titlePane->SetTextL( titleText );
        }
    }

// --------------------------------------------------------------------------
// CMSBaseView::SetTitlePaneTextL()
// Sets text to title pane.
// --------------------------------------------------------------------------
void CMSBaseView::SetTitlePaneTextL( const TDesC& aText )
    {
    LOG(_L("[MediaServant]\t CMSBaseView::SetTitlePaneTextL"));

    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    titlePane->SetTextL( aText );
    }


// --------------------------------------------------------------------------
// CMSBaseView::SetNavigationPaneTextL()
// Sets navigation pane text.
// --------------------------------------------------------------------------
//
void CMSBaseView::SetNavigationPaneTextL(const TDesC& aText )
    {
    // old decorator is popped and deleted
    ClearCurrentNaviPaneText();

    if ( !iNaviPane )
        {
        iNaviPane = static_cast<CAknNavigationControlContainer*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
                // ownership of decorator is transfered to application
        }

    iNaviDecorator = iNaviPane->CreateNavigationLabelL( aText );
    iNaviPane->PushL( *iNaviDecorator );
    }

// --------------------------------------------------------------------------
// CMSBaseView::ClearCurrentNaviPaneText()
// Clears navi pane text.
// --------------------------------------------------------------------------
//
void CMSBaseView::ClearCurrentNaviPaneText()
    {
    if ( iNaviDecorator )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }

// --------------------------------------------------------------------------
// CMSFillView::CancelAsyncOperation()
// Cancels current operation
// --------------------------------------------------------------------------
//
void CMSBaseView::CancelAsyncOperation( CMSAppUi* aAppUi )
    {
    // getting data not yet finished so we must cancel the operation
    TCmServerState state;
    aAppUi->MSEngine()->ServerState( state );
    if ( state != ECmServerStateIdle )
        {
        LOG( _L( "[MediaServant]\t CMSBaseView::\
        CancelAsyncOperation cancel operation" ) );
        aAppUi->MSEngine()->StopOperation();
        ClearCurrentNaviPaneText();
        }
    }
// End of File
