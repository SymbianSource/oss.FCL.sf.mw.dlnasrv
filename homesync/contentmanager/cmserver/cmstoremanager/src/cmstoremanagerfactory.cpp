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
* Description:  The factory for store manager
*
*/


#include "cmstoremanagerfactory.h"
#include "cmstoremanager.h"
#include "cmsmmain.h"

// --------------------------------------------------------------------------
// CmStoreManagerFactory::NewStoreManagerL
// --------------------------------------------------------------------------
//
EXPORT_C MCmSmManager* CmStoreManagerFactory::NewStoreManagerL( 
    MCmServiceObserver* aObserver, CMdESession& aSession,
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    return CCmSmMain::NewL( aObserver, aSession, aDBManager, aMemManager );
    }

// --------------------------------------------------------------------------
// CmStoreManagerFactory::NewStoreManagerLC
// --------------------------------------------------------------------------
//    
EXPORT_C MCmSmManager* CmStoreManagerFactory::NewStoreManagerLC( 
    MCmServiceObserver* aObserver, CMdESession& aSession,
    CCmDmMain* aDBManager, CCmMmMain* aMemManager )
    {
    return CCmSmMain::NewLC( aObserver, aSession, aDBManager, aMemManager );
    }

// end of file
