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
* Description:  CMSFillBrowseContainer class implementation
*
*/


#include <mediaservant.rsg>
#include <StringLoader.h>
#include <utf.h>
#include <e32math.h>

#include "mserv.hlp.hrh"
#include "cmfillrule.h"
#include "msfillbrowsecontainer.h"
#include "msfillbrowseview.h"
#include "msappui.h"
#include "msengine.h"
#include "mediaservantuid.h"

#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::CMSFillBrowseContainer()
// --------------------------------------------------------------------------
//
CMSFillBrowseContainer::CMSFillBrowseContainer(
                                    CMSAppUi& aAppUi,
                                    CMSFillBrowseView& aView )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::\
    CMSFillBrowseContainer"));

    iAppUi = &aAppUi;
    iView = &aView;
    iIconCount = 0; //default icon   

    iAppUi->MSEngine()->SetObserver( this );
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::~CMSFillBrowseContainer()
// --------------------------------------------------------------------------
//
CMSFillBrowseContainer::~CMSFillBrowseContainer()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::\
    ~CMSFillBrowseContainer"));

    iStatusArray.Close();

    delete iListBox; // Deletes listbox object.
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::RandomizeListL()
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::RandomizeListL()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::RandomizeListL"));

    iAppUi->MSEngine()->PreProcessFillListL( iRule->Name() );
    HBufC* naviText = StringLoader::LoadLC( R_MS_WAIT_RANDOMIZE );
    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::HasListRandomRule
// --------------------------------------------------------------------------
//
TBool CMSFillBrowseContainer::HasListRandomRule()
    {
    TBool retval = EFalse;
    if ( iRule->Method() == ECMRandom )
        {
        retval = ETrue;
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::ReadyL( TCmService aService, TInt /*aError*/ )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::ReadyL"));

    if ( aService == ECmServicePreProcessingFilllist )
        {
        iView->ClearCurrentNaviPaneText();
        // read rules again
        iAppUi->FillRuleController()->UpdateArrayL();

        GetBrowseDataL();
        // Add data to listbox
        UpdateListBoxDataL();
        }
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::CheckIfStatusChanged()
// Checks if status values are changed
// --------------------------------------------------------------------------
//
TBool CMSFillBrowseContainer::CheckIfStatusChanged()
    {
    TBool status( EFalse );

    TInt itemCount = iItemArray->Count();

    for ( TInt i = 0; i < itemCount; i++ )
        {
        TCmListItemStatus status1 = (*iItemArray)[i]->Status();
        TCmListItemStatus status2 = iStatusArray[i];
        if ( (*iItemArray)[i]->Status() != iStatusArray[i] )
            {
            status = ETrue;
            i = itemCount; // break loop
            }
        }

    return status;
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::SetFileStatusesL()
// Sets file statuses
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::SetFileStatusesL()
    {
    if ( CheckIfStatusChanged() )
        {
        iAppUi->MSEngine()->SetFillFileStatusesL( iItemArray );
        }
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::GetBrowseDataL()
// Gets file information from engine
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::GetBrowseDataL()
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::GetBrowseDataL"));

    CMSParameterAgent* parameterAgent = iAppUi->ParameterAgent();

    TInt selected = iAppUi->ParameterAgent()->FillViewFocusIndex();

    RPointerArray<CMSFillList>* fillListArray =
                        iAppUi->FillRuleController()->FillListArray();

    CMSFillList* fillList = (*fillListArray)[selected];
    iRule = fillList->List();
    // get media type
    iMediaType = iRule->MediaType();
    // get items covered by rule
    if( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        iItemArray->Close();
        delete iItemArray;
        iItemArray = NULL;
        }
    iItemArray = iAppUi->MSEngine()->GetFillListItemsL( iRule->Name() );
    // keep original statuses safe
    for ( TInt i = 0; i < iItemArray->Count(); i++ )
        {
        iStatusArray.AppendL( (*iItemArray)[i]->Status() );
        }

    // Set title pane text
    HBufC* ruleName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                    iRule->Name() );
    CleanupStack::PushL( ruleName );
    iView->SetTitlePaneTextL( *ruleName );
    CleanupStack::PopAndDestroy( ruleName );

    // set navi pane text
    CreateAndSetNaviPaneTextL( fillList->ItemCount(), fillList->ListSize() );
    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::CreateAndSetNaviPaneTextL()
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::CreateAndSetNaviPaneTextL( TUint32 aCount,
                                                        TUint32 aSize )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::\
    CreateAndSetNaviPaneTextL"));

    // Set navi pane text
    CArrayFix<TInt>* infoArray =
        new ( ELeave ) CArrayFixFlat<TInt>(2); // only 2 numbers needed
    CleanupStack::PushL( infoArray );

    HBufC* naviText = NULL;

    TReal size( 0 );
    Math::Round( size, TReal( aSize ) / KMega, 0 );


    switch ( aCount )
        {
        case 0: // 0 items
            {
            naviText = StringLoader::LoadLC(
                        R_MS_FILL_LIST_BROWSE_NAVI_0_ITEMS );
            break;
            }
        case 1: // 1 item
            {
            naviText = StringLoader::LoadLC(
                        R_MS_FILL_LIST_BROWSE_NAVI_1_ITEM, size );
            break;
            }
        default: // many items
            {
            infoArray->AppendL( size );
            infoArray->AppendL( aCount );
            naviText = StringLoader::LoadLC(
                        R_MS_FILL_LIST_BROWSE_NAVI_ITEMS, *infoArray );
            // do number conversion
            TPtr ptr = naviText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
            break;
            }
        }

    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    CleanupStack::PopAndDestroy( infoArray );
    }


// --------------------------------------------------------------------------
// CMSFillBrowseContainer::SetStatusOfFiles()
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::SetStatusOfFiles( TCmListItemStatus aStatus )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::SetStatusOfFiles"));

    const CListBoxView::CSelectionIndexArray* selections =
                                    ListBox()->SelectionIndexes();
    TInt count = selections->Count();

    // mark used
    if ( count )
        {
        for ( TInt index = 0; index < count; index++)
            {
            SetFileStatus( (*selections)[ index ], aStatus );

            }
        }
    // set only highlighted item status
    else
        {
        SetFileStatus( ListBox()->CurrentItemIndex(), aStatus );
        }

    }

// --------------------------------------------------------------------------
// CMSFillBrowseContainer::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSFillBrowseContainer::HandleResourceChange( TInt aType )
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::\
    HandleResourceChange"));


    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------------------------
// CMSFillBrowseContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSFillBrowseContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSFillBrowseContainer::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_FILL_BROWSE;
    }

// End of File

