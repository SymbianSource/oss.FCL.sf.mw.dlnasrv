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
* Description:  CMSFillRuleController class implementation
*
*/


#include <StringLoader.h>
#include <mediaservant.rsg>
#include <utf.h>

#include "cmfillrulecontainer.h"
#include "cmfillrule.h"
#include "msfillrulecontroller.h"
#include "msconstants.h"
#include "mediaservant.hrh"
#include "msengine.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSFillRuleController::NewL
// --------------------------------------------------------------------------
//
CMSFillRuleController* CMSFillRuleController::NewL( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::NewL"));

    CMSFillRuleController* self = CMSFillRuleController::NewLC( aMSEngine );    
    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::NewLC
// --------------------------------------------------------------------------
//
CMSFillRuleController* CMSFillRuleController::NewLC( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::NewLC"));

    CMSFillRuleController* self = new (ELeave) CMSFillRuleController(
                                                    aMSEngine );

    CleanupStack::PushL(self);
    self->ConstructL();
        
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSFillRuleController::ConstructL
// --------------------------------------------------------------------------
//
void CMSFillRuleController::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::ConstructL"));

    if (iMSEngine)
        {
        InitializeRuleContainersL();
        }
    }
// --------------------------------------------------------------------------
// CMSFillRuleController::CMSFillRuleController()
// --------------------------------------------------------------------------
//
CMSFillRuleController::CMSFillRuleController( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::CMSFillRuleController"));

    iMSEngine = &aMSEngine;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::~CMSFillRuleController()
// --------------------------------------------------------------------------
//
CMSFillRuleController::~CMSFillRuleController()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::~CMSFillRuleController"));

    iFillListArray.ResetAndDestroy();
    iFillListArray.Close();

    delete iRuleContainer;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::FillListArray()
// Returns pointer to fill list array
// --------------------------------------------------------------------------
//
RPointerArray<CMSFillList>* CMSFillRuleController::FillListArray()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::FillListArray"));

    return &iFillListArray;
    }

// ---------------------------------------------------------------------------
// CMSFillRuleController::InitializeRuleContainersL
// initializes rule containers
// ---------------------------------------------------------------------------
//
void CMSFillRuleController::InitializeRuleContainersL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::\
    InitializeRuleContainersL"));

    TInt error( KErrNone );
    // Check if wizard has been executed
    TBool runWizard = iMSEngine->GetAppWizardState( error );

    if ( !runWizard  )
        {
        CreateFillRulesL();
        delete iRuleContainer;
        iRuleContainer = NULL;
        iRuleContainer = iMSEngine->FillRulesL();
        }
    else
        {
        // FILL RULES
        iRuleContainer = iMSEngine->FillRulesL();
        }

    // Creates pointer array to rules so we can change rule order at UI
    // Operations done trough pointer array affects original rules
    // in RuleContainer
    if ( iRuleContainer )
        {
        // Arrange rules by priority to pointer table
        TInt rulecount = iRuleContainer->FillRuleCount();

        for (TUint8 order = 0; order < rulecount; order++)
            {
            // search rule with desired priority
            for (TInt index = 0; index < rulecount; index++)
                {

                CCmFillRule* rule = iRuleContainer->FillRule(index);
                TUint8 priority = rule->Priority();

                if ( order == priority )
                    {
                    CMSFillList* list =
                        CMSFillList::NewL( *iMSEngine, *rule );
                    // add list to array
                    iFillListArray.AppendL( list );

                    // stop for loop
                    index = rulecount;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::CreateFillRulesL()
// FOR TEST
// --------------------------------------------------------------------------
//
void CMSFillRuleController::CreateFillRulesL()
    {
    CCmFillRuleContainer* fillRuleContainer = CCmFillRuleContainer::NewLC();

    // default list 1
    CCmFillRule* defaultList1 = CCmFillRule::NewLC();

    HBufC* temp = StringLoader::LoadLC( R_MS_DEFAULT_FILL_LIST_1 );
    HBufC8* listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList1->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    defaultList1->SetMediaType( ECmImage );
    defaultList1->SetMethod( ECmLatest );
    defaultList1->SetSelected( ECmUnSelected );
    defaultList1->SetPriority( 0 );
    defaultList1->SetTemplateId( EMSDefaultImageList );
    defaultList1->SetStatus( ECmToBeFilled );
    defaultList1->SetLimitType( EMbits );
    defaultList1->SetAmount( KDefaultListSize50 );

    fillRuleContainer->AddFillRuleL( defaultList1 );

    // default list 2
    CCmFillRule* defaultList2 = CCmFillRule::NewLC();

    temp = StringLoader::LoadLC( R_MS_DEFAULT_FILL_LIST_2 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList2->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    defaultList2->SetMediaType( ECmVideo );
    defaultList2->SetMethod( ECmLatest );
    defaultList2->SetSelected( ECmUnSelected );
    defaultList2->SetPriority( 1 );
    defaultList2->SetTemplateId( EMSDefaultVideoList );
    defaultList2->SetStatus( ECmToBeFilled );
    defaultList2->SetLimitType( EMbits );
    defaultList2->SetAmount( KDefaultListSize50 );

    fillRuleContainer->AddFillRuleL( defaultList2 );

    // default list 3
    CCmFillRule* defaultList3 = CCmFillRule::NewLC();

    temp = StringLoader::LoadLC( R_MS_DEFAULT_FILL_LIST_3 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList3->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    defaultList3->SetMediaType( ECmAudio );
    defaultList3->SetMethod( ECmLatest );
    defaultList3->SetSelected( ECmUnSelected );
    defaultList3->SetPriority( 2 );
    defaultList3->SetTemplateId( EMSDefaultMusicList );
    defaultList3->SetStatus( ECmToBeFilled );
    defaultList3->SetLimitType( EMbits );
    defaultList3->SetAmount( KDefaultListSize200 );

    fillRuleContainer->AddFillRuleL( defaultList3 );

    iMSEngine->SetFillRulesL( fillRuleContainer );

    CleanupStack::Pop( defaultList3 );
    CleanupStack::Pop( defaultList2 );
    CleanupStack::Pop( defaultList1 );
    CleanupStack::PopAndDestroy( fillRuleContainer );
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::DeleteRuleL()
// Deletes current rule
// --------------------------------------------------------------------------
//
void CMSFillRuleController::DeleteRuleL( TInt aCurrentIndex )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::DeleteRuleL"));

    // get priority of selected rule
    TInt currentPriority =
            iFillListArray[ aCurrentIndex ]->List()->Priority();

    // Rulecontainer must be scanned to find the index index of
    // rule container.
    // Priority can be used to compare because there can't be two rules
    // with same priority.
    for (TInt index = 0; index < iRuleContainer->FillRuleCount(); index++)
        {
        if (currentPriority == iRuleContainer->FillRule( index )->Priority())
            {
            iFillListArray[ aCurrentIndex ]->
                    List()->SetSelected( ECmDeleted );
            }
        }

    // recalculate priorities
    SetPriorities();

    // store rules to server
    iMSEngine->SetFillRulesL( iRuleContainer );

    // update pointer array
    UpdateArrayL();

    }

// --------------------------------------------------------------------------
// CMSFillRuleController::AddRuleL()
// Adds new rule
// --------------------------------------------------------------------------
//
void CMSFillRuleController::AddRuleL( CCmFillRule* aRule )
    {
    LOG(_L("[MediaServant]\t CMSFillContainer::DeleteRuleL"));

    if ( aRule )
        {
        TInt priority = iFillListArray.Count();
        aRule->SetPriority( priority );
        iRuleContainer->AddFillRuleL( aRule );

        // store rules to server
        iMSEngine->SetFillRulesL( iRuleContainer );

        // update pointer array
        UpdateArrayL();
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::SetPriorities()
// Sets priorities of rule array
// --------------------------------------------------------------------------
//
void CMSFillRuleController::SetPriorities()
    {
    TInt priority = 0;

    for ( TInt index = 0; index < iFillListArray.Count(); index++ )
        {
        if ( iFillListArray[index]->List()->Selected() != ECmDeleted )
            {
            iFillListArray[index]->List()->SetPriority( priority );
            priority++; // next list priority
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::compare()
// For sort. Sort is based on rule priority
// --------------------------------------------------------------------------
//
TInt CMSFillRuleController::compare ( const CMSFillList& rule1,
                                      const CMSFillList& rule2)

    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::compare"));

    TInt result = 0;

    if ( rule1.Priority() < rule2.Priority() )
        {
        result = -1;
        }
    else if ( rule1.Priority() > rule2.Priority() )
        {
        result = 1;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::ChangePriority()
// Change priorities of rules
// --------------------------------------------------------------------------
//
void CMSFillRuleController::ChangePriority( TUint aTotalItem,
                                            TUint aCurrentItem,
                                            TInt aDirection )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::ChangePriority"));

    TLinearOrder<CMSFillList> key(CMSFillRuleController::compare);

    // up
    if (aDirection == -1 && aCurrentItem)
        {
        // swap priotities of rules
        iFillListArray[aCurrentItem]->List()->SetPriority(aCurrentItem-1);
        iFillListArray[aCurrentItem-1]->List()->SetPriority(aCurrentItem);

        iFillListArray.Sort(key);
        }
    // down
    else if ( aDirection == 1 && aCurrentItem < aTotalItem - 1 )
        {
        // swap priotities of rules
        iFillListArray[aCurrentItem]->List()->SetPriority(aCurrentItem+1);
        iFillListArray[aCurrentItem+1]->List()->SetPriority(aCurrentItem);

        iFillListArray.Sort(key);
        }
    else
        {
        // Do nothing
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::UpdateArrayL()
// Resets old fill rule array and reads new rules from server
// --------------------------------------------------------------------------
//
void  CMSFillRuleController::UpdateArrayL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::UpdateArrayL"));

    // FILL RULES
    delete iRuleContainer;
    iRuleContainer = NULL;
    iRuleContainer = iMSEngine->FillRulesL();

    // Creates pointer array to rules so we can change rule order at UI
    // Operations done trough pointer array affects original rules
    // in RuleContainer
    if ( iRuleContainer )
        {
        iFillListArray.ResetAndDestroy();
        // Arrange rules by priority to pointer table
        TInt rulecount = iRuleContainer->FillRuleCount();

        for (TUint8 order = 0; order < rulecount; order++)
            {
            // search rule with desired priority
            for (TInt index = 0; index < rulecount; index++)
                {

                CCmFillRule* rule = iRuleContainer->FillRule(index);
                TUint8 priority = rule->Priority();

                if ( order == priority )
                    {
                    // stop for loop
                    index = rulecount;
                    CMSFillList* list =
                            CMSFillList::NewLC( *iMSEngine, *rule );
                    // add rule to array
                    iFillListArray.AppendL( list );
                    CleanupStack::Pop( list );
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::SaveRulesL()
// Sends fill rules to server
// --------------------------------------------------------------------------
//
void  CMSFillRuleController::SaveRulesL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::SaveRulesL"));

    iMSEngine->SetFillRulesL( iRuleContainer );
    }


// --------------------------------------------------------------------------
// CMSFillRuleController::HasPriorityChangedL()
// Checks if list priorities has been changed
// --------------------------------------------------------------------------
//
TBool CMSFillRuleController::HasPriorityChangedL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::\
    HasPriorityChangedL"));

    TBool status( EFalse );

    CCmFillRuleContainer* fillLists = iMSEngine->FillRulesL();
    CleanupStack::PushL( fillLists );
    TInt listCount = fillLists->FillRuleCount();
    TInt arrayCount = iFillListArray.Count();

    if ( listCount == arrayCount )
        {
        for ( TInt index = 0; index < listCount; index++ )
            {
            CCmFillRule* origList = fillLists->FillRule(index);
            TInt origPriority = origList->Priority();

            // lists are in priority order in iFillListArray
            CCmFillRule* modList = iFillListArray[ origPriority ]->List();

            // if list names are same we have the same lists
            if ( origList->Name().Compare( modList->Name() ) )
                {
                status = ETrue;
                index = listCount; // break loop
                }
            }
        }
    else
        {
        status = ETrue;
        }

    CleanupStack::PopAndDestroy( fillLists );
    return status;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::HasSelectionsChanged()
// Checks if list activity has been changed
// --------------------------------------------------------------------------
//
TBool CMSFillRuleController::HasSelectionsChanged( TCmFillRuleStatus aStatus )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::\
    HasSelectionsChanged"));

    TBool status( EFalse );

    TInt listCount = iFillListArray.Count();

    for ( TInt index = 0; index < listCount; index++ )
        {
        if ( iFillListArray[ index ]->OriginalSelectionStatus() !=
             iFillListArray[ index ]->List()->Selected() &&
             iFillListArray[ index ]->OriginalSelectionStatus() != aStatus )
            {
            // activity changed
            status = ETrue;
            index = listCount;
            }

        }
    return status;
    }

// --------------------------------------------------------------------------
// CMSFillRuleController::RestoreOriginalSelections()
// --------------------------------------------------------------------------
//
void CMSFillRuleController::RestoreOriginalSelections()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::\
    RestoreOriginalSelections"));
    TInt listCount = iFillListArray.Count();

    for ( TInt index = 0; index < listCount; index++ )
        {
        iFillListArray[index]->List()->SetSelected(
                iFillListArray[index]->OriginalSelectionStatus() );
        }
    }





/***************** CMSFillList class implementation **********************/


// --------------------------------------------------------------------------
// CMSFillList::NewL
// --------------------------------------------------------------------------
//
CMSFillList* CMSFillList::NewL( CMSEngine& aMSEngine, CCmFillRule& aFillList )
    {
    LOG(_L("[MediaServant]\t CMSFillList::NewL"));

    CMSFillList* self = CMSFillList::NewLC( aMSEngine, aFillList );
    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSFillList::NewLC
// --------------------------------------------------------------------------
//
CMSFillList* CMSFillList::NewLC( CMSEngine& aMSEngine,
                                 CCmFillRule& aFillList )
    {
    LOG(_L("[MediaServant]\t CMSFillList::NewLC"));

    CMSFillList* self = new (ELeave) CMSFillList( aMSEngine, aFillList );

    CleanupStack::PushL(self);
    self->ConstructL();
    
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSFillList::ConstructL
// --------------------------------------------------------------------------
//
void CMSFillList::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSFillList::ConstructL"));

    iListSize = iFillList->ListRealSizeInBytes();
    iItemCount = iFillList->ListRealCount();
    }
// --------------------------------------------------------------------------
// CMSFillList::CMSFillList()
// --------------------------------------------------------------------------
//
CMSFillList::CMSFillList( CMSEngine& aMSEngine, CCmFillRule& aFillList )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleController::CMSFillRuleController"));
    iFillList = &aFillList;
    iMSEngine = &aMSEngine;

    iSelected = iFillList->Selected();
    }

// --------------------------------------------------------------------------
// CMSFillList::~CMSFillList()
// --------------------------------------------------------------------------
//
CMSFillList::~CMSFillList()
    {
    LOG(_L("[MediaServant]\t CMSFillList::~CMSFillList"));
    }

// --------------------------------------------------------------------------
// CMSFillList::List()
// --------------------------------------------------------------------------
//
CCmFillRule* CMSFillList::List()
    {
    LOG(_L("[MediaServant]\t CMSFillList::List"));

    return iFillList;
    }

// --------------------------------------------------------------------------
// CMSFillList::List()
// --------------------------------------------------------------------------
//
TUint32 CMSFillList::ListSize()
    {
    LOG(_L("[MediaServant]\t CMSFillList::ListSize"));

    return iListSize;
    }

// --------------------------------------------------------------------------
// CMSFillList::List()
// --------------------------------------------------------------------------
//
TUint32 CMSFillList::ItemCount()
    {
    LOG(_L("[MediaServant]\t CMSFillList::ItemCount"));

    return iItemCount;
    }

// --------------------------------------------------------------------------
// CMSFillContainer::Priority
// Return list priority
// --------------------------------------------------------------------------
//
TInt CMSFillList::Priority() const
    {
    return iFillList->Priority();
    }

// --------------------------------------------------------------------------
// CMSFillContainer::Priority
// Return list priority
// --------------------------------------------------------------------------
//
TCmFillRuleStatus CMSFillList::OriginalSelectionStatus()
    {
    return iSelected;
    }

// End of File
