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
* Description:  CMSDocument class implementation
*
*/



#include "msdebug.h"
#include "msdocument.h"
#include "msappui.h"

// --------------------------------------------------------------------------
// CMSDocument::CMSDocument( CEikApplication& aApp )
// --------------------------------------------------------------------------
//
CMSDocument::CMSDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {    LOG(_L("[MediaServant]\t CMSDocument::CMSDocument"));
    }

// --------------------------------------------------------------------------
// void CMSDocument::ConstructL()
// --------------------------------------------------------------------------
//
void CMSDocument::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSDocument::ConstructL"));
    }

// --------------------------------------------------------------------------
// CMSDocument* CMSDocument::NewL( CEikApplication& aApp )
// --------------------------------------------------------------------------
//
CMSDocument* CMSDocument::NewL(CEikApplication& aApp )
    {
    LOG(_L("[MediaServant]\t CMSDocument::NewL"));

    CMSDocument* self = CMSDocument::NewLC( aApp );
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CMSDocument* CMSDocument::NewLC( CEikApplication& aApp )
// --------------------------------------------------------------------------
//
CMSDocument* CMSDocument::NewLC(CEikApplication& aApp )
    {
    LOG(_L("[MediaServant]\t CMSDocument::NewL"));

    CMSDocument* self = new ( ELeave ) CMSDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
    
// --------------------------------------------------------------------------
// CMSDocument::CreateAppUiL()
// --------------------------------------------------------------------------
//
CEikAppUi* CMSDocument::CreateAppUiL()
    {
    LOG(_L("[MediaServant]\t CMSDocument::CreateAppUiL"));

    return new ( ELeave ) CMSAppUi;
    }

// --------------------------------------------------------------------------
// CMSDocument::~CMSDocument()
// --------------------------------------------------------------------------
//
CMSDocument::~CMSDocument()
    {
    LOG(_L("[MediaServant]\t CMSDocument::~CMSDocument"));
    }

// End of File
