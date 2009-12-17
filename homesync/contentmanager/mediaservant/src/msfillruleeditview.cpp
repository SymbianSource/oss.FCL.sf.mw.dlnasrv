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
* Description:  CMSFillRuleEditView class implementation
*
*/


#include <mediaservant.rsg>
#include <hlplch.h>
#include <StringLoader.h>
#include <aknlistquerydialog.h>

#include "msfillview.h"
#include "msengine.h"
#include "msfillruleeditlist.h"
#include "msfillruleeditview.h"
#include "msappui.h"
#include "msconstants.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSFillRuleEditView::CMSFillRuleEditView()
// --------------------------------------------------------------------------
//
CMSFillRuleEditView::CMSFillRuleEditView( CMSAppUi& aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::CMSFillRuleEditView"));

    iAppUi = &aAppUi;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSFillRuleEditView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::ConstructL"));

    BaseConstructL( R_MS_FILL_EDIT_VIEW );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditView::~CMSFillRuleEditView()
// --------------------------------------------------------------------------
//
CMSFillRuleEditView::~CMSFillRuleEditView()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::~CMSFillRuleEditView"));

    DoDeactivate();
    }

// --------------------------------------------------------------------------
// TUid CMSFillRuleEditView::Id()
// --------------------------------------------------------------------------
//
TUid CMSFillRuleEditView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::Id"));

    return KMSFillEditViewId;
    }


// --------------------------------------------------------------------------
// CMSFillRuleEditView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSFillRuleEditView::HandleCommandL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::HandleCommandL"));

    switch (aCommand)
        {
        case EAknCmdOpen:
            {
            iContainer->OpenSelectedListboxItemL();
            break;
            }
        case EAknSoftkeyDone:
            {
            iAppUi->MSEngine()->StopOperation();
            iAppUi->FillRuleController()->UpdateArrayL();
            iAppUi->ChangeViewL( KMSFillEditViewId, KMSFillViewId );
            break;
            }
        case EMSCmdRemoveRule:
            {
            iContainer->RemoveCurrentRuleL();
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
            }
        break;

        }
    }


// --------------------------------------------------------------------------
// CMSFillRuleEditView::DoActivateL()
// --------------------------------------------------------------------------
//
void CMSFillRuleEditView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::DoActivateL"));

    if ( !iContainer )
        {
        // create settings screen
        iContainer = CMSFillRuleEditList::NewL(*iAppUi,
                                               *this );
        iContainer->SetMopParent( this );

        // now control receives keyboard events
        iAppUi->AddToStackL( *this, iContainer );
        iContainer->ActivateL();
        }

    // Set title text
    HBufC* titleText = NULL;

    switch ( iContainer->ListMediaType() )
        {
        case ECmImage:
            {
            titleText = StringLoader::LoadLC(
                                        R_MS_EDIT_IMAGE_RULE_VIEW_TITLE );
            break;
            }
        case ECmVideo:
            {
            titleText = StringLoader::LoadLC(
                                        R_MS_EDIT_VIDEO_RULE_VIEW_TITLE );
            break;
            }
        case ECmAudio:
            {
            titleText = StringLoader::LoadLC(
                                        R_MS_EDIT_MUSIC_RULE_VIEW_TITLE );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditView::DoActivateL \
            list has no media type"));
            break;
            }
        }

    if ( titleText )
        {
        SetTitlePaneTextL( *titleText );
        CleanupStack::PopAndDestroy ( titleText );
        }
   }

// --------------------------------------------------------------------------
// CMSFillRuleEditView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSFillRuleEditView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::DoDeactivate"));

    ClearCurrentNaviPaneText();

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;
    }


// --------------------------------------------------------------------------
// CMSFillRuleEditView:DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CMSFillRuleEditView::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditView::DynInitMenuPaneL"));
    
    if ( aResourceId == R_MS_FILL_EDIT_MENU && iContainer )
        {
        // check if the list is empty
        if ( iContainer->IsCurrentRuleAdditional() == EFalse )
            {
            // Hide command
            aMenuPane->SetItemDimmed(EMSCmdRemoveRule, ETrue);
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditView:ShowTemplateQueryL
// --------------------------------------------------------------------------
//
TBool CMSFillRuleEditView::ShowTemplateQueryL( TInt aResourceId,
                                               TInt aQueryHeading,
                                               CDesCArray* aRuleNames,
                                               TInt &aSelected )
	{
	LOG(_L("[MediaServant]\t CMSFillRuleEditView::ShowTemplateQueryL"));
	
	TBool ret( EFalse );

    // construct and show template query
    CAknListQueryDialog* query =
                     new ( ELeave ) CAknListQueryDialog( &aSelected );
    query->PrepareLC( aResourceId );

    // read header text
    HBufC* header = StringLoader::LoadLC( aQueryHeading );
    query->SetHeaderTextL( header->Des() );
    
    // set item array
    query->SetItemTextArray( aRuleNames );
    query->SetOwnershipType( ELbmDoesNotOwnItemArray );
    CleanupStack::PopAndDestroy( header );

	if ( query->RunLD() )
		{
		ret = ETrue;
		}

    return ret;
	}

// --------------------------------------------------------------------------
// CMSFillRuleEditView:ShowQueryL
// --------------------------------------------------------------------------
//
TBool CMSFillRuleEditView::ShowQueryL( TInt aResourceId )
	{
	LOG(_L("[MediaServant]\t CMSFillRuleEditView::ShowQueryL"));
	
	TBool ret( EFalse );
	CAknQueryDialog* query= CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
	CleanupStack::PushL( query );
    if ( query->ExecuteLD( aResourceId ) )
        {
        ret = ETrue;
        }
    CleanupStack::Pop( query );
    return ret;
	}

// End of File
