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
* Description:  The factory for Fill manager
*
*/


#include "cmfillmanagerfactory.h"
#include "cmfillmanager.h"
#include "cmfmmain.h"


// --------------------------------------------------------------------------
// CmFillManagerFactory::NewFillManagerL
// --------------------------------------------------------------------------
//
EXPORT_C MCmFmManager* CmFillManagerFactory::NewFillManagerL( 
    MCmServiceObserver* aObserver, CMdESession& aSession, 
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    return CCmFmMain::NewL( aObserver, aSession, aDBManager, aMemManager );
    }

// --------------------------------------------------------------------------
// CmFillManagerFactory::NewFillManagerLC
// --------------------------------------------------------------------------
//    
EXPORT_C MCmFmManager* CmFillManagerFactory::NewFillManagerLC( 
    MCmServiceObserver* aObserver, CMdESession& aSession, 
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    return CCmFmMain::NewLC( aObserver, aSession, aDBManager, aMemManager );
    }

// end of file
