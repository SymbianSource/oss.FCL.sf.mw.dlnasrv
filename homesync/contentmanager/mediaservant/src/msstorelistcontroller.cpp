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
* Description:  CMSStoreListController class implementation
*
*/

#include <StringLoader.h>
#include <mediaservant.rsg>
#include <utf.h>

#include "cmstorerulecontainer.h"
#include "cmstorerule.h"
#include "msstorelistcontroller.h"
#include "mediaservant.hrh"
#include "msengine.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSStoreListController::NewL
// --------------------------------------------------------------------------
//
CMSStoreListController* CMSStoreListController::NewL( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::NewL"));

    CMSStoreListController* self = CMSStoreListController::NewLC(
                                                    aMSEngine );

    CleanupStack::Pop(self);
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSStoreListController::NewLC
// --------------------------------------------------------------------------
//
CMSStoreListController* CMSStoreListController::NewLC( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::NewLC"));

    CMSStoreListController* self = new (ELeave) CMSStoreListController(
                                                    aMSEngine );

    CleanupStack::PushL(self);
    self->ConstructL();
    
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSStoreListController::ConstructL
// --------------------------------------------------------------------------
//
void CMSStoreListController::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::ConstructL"));

    iStoreListContainer = iMSEngine->StoreRulesL();
    if ( iStoreListContainer &&
         iStoreListContainer->StoreRuleCount() <= 0)
        {
        CreateStoreRulesL();
        }
    else
        {
        ChangeStoreRuleNamesL();
        }

    delete iStoreListContainer;
    iStoreListContainer = NULL;
    iStoreListContainer = iMSEngine->StoreRulesL();
    }
    
// --------------------------------------------------------------------------
// CMSStoreListController::CMSStoreListController()
// --------------------------------------------------------------------------
//
CMSStoreListController::CMSStoreListController( CMSEngine& aMSEngine )
    {
    LOG(_L("[MediaServant]\t \
    CMSStoreListController::CMSStoreListController"));

    iMSEngine = &aMSEngine;
    }

// --------------------------------------------------------------------------
// CMSStoreListController::~CMSStoreListController()
// --------------------------------------------------------------------------
//
CMSStoreListController::~CMSStoreListController()
    {
    LOG(_L("[MediaServant]\t \
    CMSStoreListController::~CMSStoreListController"));

    delete iStoreListContainer;
    }

// --------------------------------------------------------------------------
// CMSStoreListController::StoreListArray()
// Returns pointer to store list array
// --------------------------------------------------------------------------
//
CCmStoreRuleContainer* CMSStoreListController::StoreListContainer()
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::StoreListContainer"));

    return iStoreListContainer;
    }

// --------------------------------------------------------------------------
// CMSStoreListController::CreateStoreRulesL()
// --------------------------------------------------------------------------
//
void CMSStoreListController::CreateStoreRulesL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::CreateStoreRulesL"));
    
    CCmStoreRuleContainer* storeRuleContainer =
                                CCmStoreRuleContainer::NewLC();

    CCmStoreRule* defaultList1 = CCmStoreRule::NewLC();
    HBufC* temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_1 );
    HBufC8* listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList1->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );
    defaultList1->AddStoreRuleL( ECmImage );
    defaultList1->SetSelected( ECmSelected );
    defaultList1->SetStatus( ECmKeepOnDevice );
    storeRuleContainer->AddStoreRuleL( defaultList1 );

    CCmStoreRule* defaultList2 = CCmStoreRule::NewLC();
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_2 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList2->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );
    defaultList2->AddStoreRuleL( ECmOtherImage );
    defaultList2->SetSelected( ECmSelected );
    defaultList2->SetStatus( ECmToBeShrinked );
    storeRuleContainer->AddStoreRuleL( defaultList2 );

    CCmStoreRule* defaultList3 = CCmStoreRule::NewLC();
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_3 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList3->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );
    defaultList3->AddStoreRuleL( ECmVideo );
    defaultList3->SetSelected( ECmSelected );
    defaultList3->SetStatus( ECmKeepOnDevice );
    storeRuleContainer->AddStoreRuleL( defaultList3 );

    CCmStoreRule* defaultList4 = CCmStoreRule::NewLC();
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_4 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList4->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );
    defaultList4->AddStoreRuleL( ECmOtherVideo );
    defaultList4->SetSelected( ECmSelected );
    defaultList4->SetStatus( ECmKeepOnDevice );
    storeRuleContainer->AddStoreRuleL( defaultList4 );

    CCmStoreRule* defaultList5 = CCmStoreRule::NewLC();
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_5 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList5->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );
    defaultList5->AddStoreRuleL( ECmAudio );
    defaultList5->SetSelected( ECmSelected );
    defaultList5->SetStatus( ECmKeepOnDevice );
    storeRuleContainer->AddStoreRuleL( defaultList5 );

    iMSEngine->SetStoreRulesL( storeRuleContainer );

    CleanupStack::Pop( defaultList5 );
    CleanupStack::Pop( defaultList4 );
    CleanupStack::Pop( defaultList3 );
    CleanupStack::Pop( defaultList2 );
    CleanupStack::Pop( defaultList1 );
    CleanupStack::PopAndDestroy( storeRuleContainer );
    }

// --------------------------------------------------------------------------
// CMSStoreListController::ChangeStoreRuleNamesL()
// Changes names of store rules
// This must be done because user may change language and rule names
// are used in other views ( read from database )
// --------------------------------------------------------------------------
//
void CMSStoreListController::ChangeStoreRuleNamesL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::\
    ChangeStoreRuleNamesL"));

    CCmStoreRule*  defaultlist1 = iStoreListContainer->StoreRule( 0 );
    HBufC* temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_1 );
    HBufC8* listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultlist1->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    // other images
    CCmStoreRule* defaultList2 = iStoreListContainer->StoreRule( 1 );
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_2 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList2->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    // phone videos
    CCmStoreRule* defaultList3 = iStoreListContainer->StoreRule( 2 );
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_3 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList3->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );


    CCmStoreRule* defaultList4 = iStoreListContainer->StoreRule( 3 );
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_4 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList4->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    // music
    CCmStoreRule* defaultList5 = iStoreListContainer->StoreRule( 4 );
    temp = StringLoader::LoadLC( R_MS_DEFAULT_STORE_LIST_5 );
    listName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *temp );
    CleanupStack::PushL( listName );
    defaultList5->SetNameL( *listName );
    CleanupStack::PopAndDestroy( 2, temp );

    iMSEngine->SetStoreRulesL( iStoreListContainer );
    }

// --------------------------------------------------------------------------
// CMSStoreListController::UpdateListsL()
// Resets old store rule array and reads new rules from server
// --------------------------------------------------------------------------
//
void  CMSStoreListController::UpdateListsL()
    {
    LOG(_L("[MediaServant]\t CMSStoreListController::UpdateListsL"));

    delete iStoreListContainer;
    iStoreListContainer = NULL;

    // load lists again
    iStoreListContainer = iMSEngine->StoreRulesL();
    }
// End of File
