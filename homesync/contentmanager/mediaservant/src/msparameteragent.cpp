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
* Description:  CMSParameterAgent class implementation
*
*/


#include "msparameteragent.h"
#include "mediaservant.hrh"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSParameterAgent::CMSParameterAgent()
// --------------------------------------------------------------------------
//
CMSParameterAgent::CMSParameterAgent()
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::CMSParameterAgent"));

    iPreviousFocusIndex = 0;
    iTemplate.templateType = KErrNotFound;
    iTemplate.selectedTemplate = EMSNoTemplate;

    }

// --------------------------------------------------------------------------
// CMSParameterAgent::~CMSParameterAgent()
// --------------------------------------------------------------------------
//
CMSParameterAgent::~CMSParameterAgent()
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::~CMSParameterAgent"));
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetRuleTemplate()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetRuleTemplate( TMSTemplate aTemplate )
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::SetRuleTemplate"));

    iTemplate = aTemplate;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetRuleTemplate()
// --------------------------------------------------------------------------
//
TMSTemplate CMSParameterAgent::GetRuleTemplate() const
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::GetRuleTemplate"));

    return iTemplate;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetMainViewFocusIndex()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetMainViewFocusIndex( TInt aIndex )
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    SetMainViewFocusIndex %d"), aIndex) );

    iMainViewFocusIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::MainViewFocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSParameterAgent::MainViewFocusIndex() const
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    MainViewFocusIndex %d"), iMainViewFocusIndex) );

    return iMainViewFocusIndex;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetFillViewFocusIndex()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetFillViewFocusIndex( TInt aIndex )
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    SetFillViewFocusIndex %d"), aIndex ) );

    iFillViewFocusIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::FillViewFocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSParameterAgent::FillViewFocusIndex() const
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    FillViewFocusIndex %d"), iFillViewFocusIndex ));

    return iFillViewFocusIndex;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetStoreViewFocusIndex()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetStoreViewFocusIndex( TInt aIndex )
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    SetStoreViewFocusIndex %d"), aIndex ) );

    iStoreViewFocusIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::StoreViewFocusIndex()
// --------------------------------------------------------------------------
//
TInt CMSParameterAgent::StoreViewFocusIndex() const
    {
    TRACE( Print(_L("[MediaServant]\t CMSParameterAgent::\
    StoreViewFocusIndex %d"), iStoreViewFocusIndex) );

    return iStoreViewFocusIndex;
    }
// --------------------------------------------------------------------------
// CMSParameterAgent::SetPreviousViewId()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetPreviousViewId( TUid aViewId )
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::SetPreviousViewId"));

    iPreviousViewId = aViewId;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::PreviousViewId()
// --------------------------------------------------------------------------
//
TUid CMSParameterAgent::PreviousViewId() const
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::PreviousViewId"));

    return iPreviousViewId;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::SetBrowseTarget()
// --------------------------------------------------------------------------
//
void CMSParameterAgent::SetBrowseTarget(
                            TMSBrowseTarget aBrowseTarget )
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::SetBrowseTarget"));

    iBrowseTarget = aBrowseTarget;
    }

// --------------------------------------------------------------------------
// CMSParameterAgent::GetBrowseTarget()
// --------------------------------------------------------------------------
//
TMSBrowseTarget CMSParameterAgent::GetBrowseTarget() const
    {
    LOG(_L("[MediaServant]\t CMSParameterAgent::GetBrowseTarget"));

    return iBrowseTarget;
    }
    
// End of File

