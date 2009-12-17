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
* Description:      Abstract factory for CMSettings
*
*/







#ifndef C_CMSQLMAINFACTORY_H
#define C_CMSQLMAINFACTORY_H

#include <e32std.h>
#include <e32base.h>

/* Forward declarations. */
class MCmSqlMain;
class RFs;

/**
 *  Abstract factory for CCmSqlMain
 *
 *  @lib cmsettings.lib
 *
 *  @since S60 5.1
 */
class CCmSqlMainFactory : public CBase
    {

public:

    /**
     * Creates a new instance of CCmSqlMain
     *
     * @since S60 5.1
     * @param aFs, file server
     * @return  pointer to CCmSqlMain class
     */
    IMPORT_C static MCmSqlMain* NewCmSqlMainL( RFs& aFs );

    /**
     * Creates a new instance of CCmSqlMain. Instance is left in
     * cleanup stack.
     *
     * @since S60 5.1
     * @param aFs, file server
     * @return  pointer to CCmSqlMain class
     */
    IMPORT_C static MCmSqlMain* NewCmSqlMainLC( RFs& aFs );

    };

#endif // C_CMSQLMAINFACTORY_H
