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
* Description:  CMSFillView class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>
#include <aknlistquerydialog.h>
#include <hlplch.h>
#include <utf.h>

#include "cmfillrule.h"
#include "msfillview.h"
#include "msengine.h"
#include "msfillcontainer.h"
#include "msappui.h"
#include "msconstants.h"
#include "mediaservant.hrh"
#include "msparameteragent.h"
#include "msdebug.h"


// CONSTANTS
_LIT( KWildCard, "*" );
_LIT( KSpace, " ");

const TInt KImageListTemplate = 1;
const TInt KVideoListTemplate = 2;
const TInt KMusicListTemplate = 3;


// --------------------------------------------------------------------------
// CMSFillView::CMSFillView()
// --------------------------------------------------------------------------
//
CMSFillView::CMSFillView( CMSAppUi& aAppUi )
    {
    LOG(_L("[MediaServant]\t CMSFillView::CMSFillView"));

    iAppUi = &aAppUi;
    }

// --------------------------------------------------------------------------
// CMSFillView::ConstructL()
// --------------------------------------------------------------------------
//
void CMSFillView::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSFillView::ConstructL"));

    BaseConstructL( R_MS_FILL_VIEW );
    }

// --------------------------------------------------------------------------
// CMSFillView::~CMSFillView()
// --------------------------------------------------------------------------
//
CMSFillView::~CMSFillView()
    {
    LOG(_L("[MediaServant]\t CMSFillView::~CMSFillView"));
    DoDeactivate();
    }

// --------------------------------------------------------------------------
// TUid CMSFillView::Id()
// --------------------------------------------------------------------------
//
TUid CMSFillView::Id() const
    {
    LOG(_L("[MediaServant]\t CMSFillView::Id"));

    return KMSFillViewId;
    }


// --------------------------------------------------------------------------
// CMSFillView::HandleCommandL(TInt aCommand)
// --------------------------------------------------------------------------
//
void CMSFillView::HandleCommandL(TInt aCommand)
    {
    LOG(_L("[MediaServant]\t CMSFillView::HandleCommandL"));

    CEikButtonGroupContainer* cba = Cba();

    switch (aCommand)
        {
        case EAknSoftkeyBack:
            {
            CancelAsyncOperation( iAppUi );
            iNextView = KMSMainViewId;

            CheckAndHandleChangesL();

            break;
            }

        case EAknSoftkeyDone:
            {
            CancelAsyncOperation(  iAppUi );
            iContainer->SetReorderStateL( EMSFillNormal );
            cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            cba->DrawNow();

            // Set title pane text
            SetTitlePaneTextL( R_MS_FILL_VIEW_TITLE );
            break;
            }

        case EMSCmdActivate: // fall through
        case EMSCmdDeActivate:
            {
            iContainer->ChangeListActivityL();
            SetListUnselected();
            break;
            }
        // Edit fill rule
        case EMSCmdEditRule:
            {
            CancelAsyncOperation(  iAppUi );
            TMSTemplate ruleTemplate;
            ruleTemplate.templateType = aCommand;
            ruleTemplate.selectedTemplate = EMSNoTemplate;
            iAppUi->ParameterAgent()->SetRuleTemplate( ruleTemplate );

            iNextView = KMSFillEditViewId;
            CheckAndHandleChangesL();

            break;
            }

        case EMSCmdBrowse:
            {
            CancelAsyncOperation(  iAppUi );
            iNextView = KMSFillBrowseViewId;
            CheckAndHandleChangesL();

            break;
            }

        case EMSCmdNewImageList: // fall through
        case EMSCmdNewVideoList: // fall through
        case EMSCmdNewMusicList:
            {
            CancelAsyncOperation(  iAppUi );
            ShowTemplateQueryL( aCommand );
            iNextView = KMSFillEditViewId;
            CheckAndHandleChangesL();
            break;
            }

        // Reorder fill rule
        case EMSCmdReorder:
            {
            iContainer->SetReorderStateL( EMSFillReorderActive );
            cba->SetCommandSetL( R_SOFTKEYS_DROP_EMPTY );
            cba->DrawNow();
            // Set title pane text
            SetTitlePaneTextL( R_MS_FILL_VIEW_TITLE_REORDER_MODE );
            break;
            }

        // Drop rule to new location
        case EMSCmdDrop:
            {
            iContainer->SetReorderStateL( EMSFillGrabActive );
            cba->SetCommandSetL( R_SOFTKEYS_GRAB_DONE );
            cba->DrawNow();
            break;
            }
        // Reorder mode active - grab item
        case EMSCmdGrab:
            {
            iContainer->SetReorderStateL( EMSFillReorderActive );
            cba->SetCommandSetL( R_SOFTKEYS_DROP_EMPTY );
            cba->DrawNow();
            break;
            }
        // Delete fill rule
        case EMSCmdDelete:
            {
            iContainer->DeleteRuleL();
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
            CancelAsyncOperation(  iAppUi );
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }


// --------------------------------------------------------------------------
// CMSFillView::DoActivateL()
// --------------------------------------------------------------------------
//
void CMSFillView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    LOG(_L("[MediaServant]\t CMSFillView::DoActivateL"));

    // Set engine observer
    iAppUi->MSEngine()->SetObserver( this );

    // Set title pane text
    SetTitlePaneTextL( R_MS_FILL_VIEW_TITLE );

    if ( !iContainer )
        {
        iContainer = new (ELeave) CMSFillContainer( *iAppUi, *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
    SetListUnselected();
    }

// --------------------------------------------------------------------------
// CMSFillView::DoDeactivate()
// --------------------------------------------------------------------------
//
void CMSFillView::DoDeactivate()
    {
    LOG(_L("[MediaServant]\t CMSFillView::DoDeactivate"));

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer; // Deletes the container class object.
    iContainer = NULL;
    SetListUnselected();
    }

// --------------------------------------------------------------------------
// CMSFillView::FocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSFillView::FocusIndex() const
    {
    return iContainer->SelectedItemIndex();
    }

// --------------------------------------------------------------------------
// CMSFillView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// --------------------------------------------------------------------------
//
void CMSFillView::DynInitMenuPaneL( TInt aResourceId,
                                    CEikMenuPane* aMenuPane )
    {
    LOG(_L("[MediaServant]\t CMSFillView::DynInitMenuPaneL"));
    
    if ( aResourceId == R_FILL_VIEW_MENU && iContainer )
        {
        // check if the list is empty
        if ( iContainer->ListItemCount() <= 0 )
            {
            // Hide commands
            aMenuPane->SetItemDimmed(EMSCmdEditRule, ETrue);
            aMenuPane->SetItemDimmed(EMSCmdReorder, ETrue);
            aMenuPane->SetItemDimmed(EMSCmdDelete, ETrue);
            aMenuPane->SetItemDimmed(EMSCmdBrowse, ETrue);
            aMenuPane->SetItemDimmed(EMSCmdActivate, ETrue);
            aMenuPane->SetItemDimmed(EMSCmdDeActivate, ETrue);
            }

        if ( iContainer->IsCurrentListItemActive() )
            {
            aMenuPane->SetItemDimmed(EMSCmdActivate, ETrue);
            }
        else
            {
            aMenuPane->SetItemDimmed(EMSCmdDeActivate, ETrue);
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillView::ShowTemplateQueryL
// shows template query
// --------------------------------------------------------------------------
//
void CMSFillView::ShowTemplateQueryL( TInt aCommand )
    {
    LOG(_L("[MediaServant]\t CMSFillView::ShowTemplateQueryL"));
    
    HBufC* defaultListName = NULL;;

    TMSTemplate ruleTemplate;
    // set template info
    ruleTemplate.templateType = aCommand;

    switch ( aCommand )
        {
        case EMSCmdNewImageList:
            {
            defaultListName = StringLoader::LoadLC(
                                        R_MS_CUSTOM_LIST_IMAGE_NAME );
            ruleTemplate.selectedTemplate = KImageListTemplate;
            break;
            }
        case EMSCmdNewVideoList:
            {
            defaultListName = StringLoader::LoadLC(
                                        R_MS_CUSTOM_LIST_VIDEO_NAME );
            ruleTemplate.selectedTemplate = KVideoListTemplate;
            break;
            }
        case EMSCmdNewMusicList:
            {
            defaultListName = StringLoader::LoadLC(
                                        R_MS_CUSTOM_LIST_MUSIC_NAME );
            ruleTemplate.selectedTemplate = KMusicListTemplate;
            break;
            }
        default:
            {
            defaultListName = StringLoader::LoadLC(
                                        R_MS_CUSTOM_LIST_IMAGE_NAME );
            ruleTemplate.selectedTemplate = KImageListTemplate;
            // command not found
            break;
            }
        }

    // Create search string with wildcards
    HBufC* searchStr = HBufC::NewLC( KMaxFileName );
    searchStr->Des().Copy( KWildCard );
    searchStr->Des().Append( defaultListName->Des() );
    searchStr->Des().Append( KWildCard );

    // Check if fill list have dublicate names
    TInt count = iContainer->CheckIfDublicateNames( *searchStr );

    CleanupStack::PopAndDestroy( searchStr );
    // keep template name safe
    ruleTemplate.templateName.Copy( defaultListName->Des() );
                        
    // if existing items...
    searchStr = HBufC::NewLC( KMaxFileName );
    searchStr->Des().Append( defaultListName->Des() );

    TBool same = EFalse;

    RPointerArray<CMSFillList>* iFillListArray =
                    iAppUi->FillRuleController()->FillListArray();

    // if there are same name items
    for ( TInt index = 0; index < iFillListArray->Count(); ++index )
        {
        CCmFillRule* rule = (*iFillListArray)[index]->List();
        HBufC* listName =
                CnvUtfConverter::ConvertToUnicodeFromUtf8L( rule->Name() );
        CleanupStack::PushL( listName );
        if ( *searchStr == *listName )
            {
            same = ETrue;
            }
        CleanupStack::PopAndDestroy( listName );
        }

    CleanupStack::PopAndDestroy( searchStr );
    // exist same name items
    if ( count > 0 && same )
        {
        TInt postfix;
        // calculate a postfix number to add the end of the name
        for ( postfix = 1; ; ++postfix )
            {
            HBufC* sameStr = HBufC::NewL( KMaxFileName );
            CleanupStack::PushL( sameStr );
            sameStr->Des().Copy( KWildCard );
            sameStr->Des().Append( defaultListName->Des() );
            sameStr->Des().Append( KSpace );
            sameStr->Des().AppendNum( postfix );
            sameStr->Des().Append( KWildCard );
               
            // do number conversion            
            TPtr ptr = sameStr->Des();
            AknTextUtils::
            DisplayTextLanguageSpecificNumberConversion( ptr );
                                
            if ( 0 == iContainer->CheckIfDublicateNames( *sameStr ) )
                {
                // after append the current number, there are not
                //same name items, so jump out the code block
                CleanupStack::PopAndDestroy( sameStr );
                break;
                }
            CleanupStack::PopAndDestroy( sameStr );
            }

        // append order number to name
        ruleTemplate.templateName.Append( KSpace );
        ruleTemplate.templateName.AppendNum( postfix );

        // do number conversion            
        AknTextUtils::
            DisplayTextLanguageSpecificNumberConversion(
                ruleTemplate.templateName );            
        }

    // store template info to parameter agent
    iAppUi->ParameterAgent()->SetRuleTemplate( ruleTemplate );

    CleanupStack::PopAndDestroy( defaultListName );
    }

// ---------------------------------------------------------------------------
// CMSFillView::ShowNaviTextL
// ---------------------------------------------------------------------------
//
void CMSFillView::ShowNaviTextL()
    {
    LOG(_L("[MediaServant]\t CMSFillView::ShowNaviTextL"));
    
    HBufC* naviText = StringLoader::LoadLC( R_MS_PREPROCESSING_TEXT );
    SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    }

// ---------------------------------------------------------------------------
// CMSFillView::SetListUnselected
// ---------------------------------------------------------------------------
//
void CMSFillView::SetListUnselected( )
    {
    LOG(_L("[MediaServant]\t CMSFillView::SetListUnselected"));
    
    iListUnSelected =  iAppUi->FillRuleController()->
                                       HasSelectionsChanged( ECmUnSelected );
    }

// ---------------------------------------------------------------------------
// CMSFillView::CheckAndHandleChangesL
// ---------------------------------------------------------------------------
//
void CMSFillView::CheckAndHandleChangesL()
    {
    LOG(_L("[MediaServant]\t CMSFillView::CheckAndHandleChangesL"));
    
    TBool priorityChanged( EFalse );

    // Check if priorities has been changed
    priorityChanged = iAppUi->FillRuleController()->
                                   HasPriorityChangedL();

    iNewListSelected = iAppUi->FillRuleController()->
                                   HasSelectionsChanged( ECmSelected );
    // check if there is deleted lists
    TBool listDeleted = iContainer->IsListDeleted();

    // set activity for all rules
    iContainer->SetFillRules();

    if ( iListUnSelected )
        {
        // unselected list(s) - show query
        DoDeleteQueryL();
        }

    // send rules to server
    iAppUi->FillRuleController()->SaveRulesL();

    // update lists if list(s) have been deleted, created or their selection
    // statuses or priorities have been changed
    if ( listDeleted ||
         priorityChanged ||
         iListUnSelected ||
         iNewListSelected )
        {
        // disable right softkey
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        cba->SetCommandSetL( R_MS_SOFTKEYS_OPTIONS_EMPTY );
        cba->DrawDeferred();

        ShowNaviTextL();
        iAppUi->MSEngine()->UpdateFillListsL();
        }
    else // no list update needed
        {
        // retrieve new information
        iAppUi->FillRuleController()->UpdateArrayL();
        iAppUi->ChangeViewL( KMSFillViewId, iNextView );
        }
    }

// ---------------------------------------------------------------------------
// CMSFillView::DoDeleteQueryL
// ---------------------------------------------------------------------------
//
void CMSFillView::DoDeleteQueryL()
    {
    LOG(_L("[MediaServant]\t CMSFillView::DoDeleteQueryL"));

    // Show delete files query
    CAknQueryDialog* query= CAknQueryDialog::NewL(
                                   CAknQueryDialog::ENoTone );

    if ( query->ExecuteLD( R_MSERV_DELETE_FILES_QUERY ) )
        {
        // do nothing
        }
    else // canceled
        {
        iAppUi->FillRuleController()->RestoreOriginalSelections();
        }
    }

// --------------------------------------------------------------------------
// CMSFillView::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSFillView::ReadyL( TCmService aService, TInt /*aError*/ )
    {
    LOG(_L("[MediaServant]\t CMSFillView::ReadyL"));

    switch ( aService )
        {
        case ECmServiceUpdateFillLists:
            {
            // set original softkeys back
            CEikButtonGroupContainer* cba =
                            CEikButtonGroupContainer::Current();
            cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            cba->DrawDeferred();

            // retrieve new information
            iAppUi->FillRuleController()->UpdateArrayL();

            ClearCurrentNaviPaneText();
            iAppUi->ChangeViewL( KMSFillViewId, iNextView );
            break;
            }
        case ECmServiceDeleteFilledFiles:
            {
            LOG(_L("[MediaServant]\t CMSFillView::ReadyL delete complete"));
            ClearCurrentNaviPaneText();
            // Set context pane icon
            iContainer->SetDefaultContextPaneIconL();

            iContainer->UpdateListBoxDataL();
            break;
            }

        default:
            {
            LOG(_L("[MediaServant]\t CMSFillView::ReadyL \
            service not found"));
            }

        }
    }

// --------------------------------------------------------------------------
// CMSFillView::SetCbaL()
// Changes softkeys
// --------------------------------------------------------------------------
//
void CMSFillView::SetCbaL( TBool aSelected )
    {
    LOG(_L("[MediaServant]\t CMSFillView::SetCbaL"));
    
    CEikButtonGroupContainer* cba = Cba();

    if ( aSelected )
        {
        cba->SetCommandSetL( R_MS_SOFTKEYS_OPTIONS_BACK__DEACTIVATE );
        }
    else
        {
        cba->SetCommandSetL( R_MS_SOFTKEYS_OPTIONS_BACK__ACTIVATE );
        }
    cba->DrawDeferred();
    }

// End of File

