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
* Description:      The factory for CCmSqlMain
*
*/






#include "cmsqlmainfactory.h"
#include "cmsqlmain.h"
#include "mcmsqlmain.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CCmSqlMainFactory::NewCmSqlMainL
// --------------------------------------------------------------------------
//
EXPORT_C MCmSqlMain* CCmSqlMainFactory::NewCmSqlMainL( RFs& aFs )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMainFactory::NewCmSqlMainL"));
    
    return CCmSqlMain::NewL( aFs );
    }

// --------------------------------------------------------------------------
// CCmSqlMainFactory::NewCmSqlMainL
// --------------------------------------------------------------------------
//    
EXPORT_C MCmSqlMain* CCmSqlMainFactory::NewCmSqlMainLC( RFs& aFs )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMainFactory::NewCmSqlMainLC"));
    
    return CCmSqlMain::NewLC( aFs );
    }

// end of file
