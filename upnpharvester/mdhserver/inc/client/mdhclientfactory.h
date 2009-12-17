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
* Description:      Abstract factory for MdhClient
*
*/







#ifndef C_CMMDHCLIENTFACTORY_H
#define C_CMMDHCLIENTFACTORY_H

#include <e32std.h>
#include <e32base.h>

/* Forward declarations. */
class MCmMdhClient;
class MCmServiceObserver;

/**
 *  Abstract factory for MdhClient
 *
 *  @lib cmmdhclient.lib
 *  @since S60 v3.1
 */
class CCmMdhClientFactory : public CBase
    {

public:

    /**
     * Creates a new instance of CSMdhClient
     * @return  pointer to CCmMdhClientEngine class
     */
    IMPORT_C static MCmMdhClient* NewCmMdhClientL( MCmServiceObserver& aServer );

    /**
     * Creates a new instance of CSMdhClient. Instance is left in
     * cleanup stack.
     * @return  pointer to CCmMdhClientEngine class
     */
    IMPORT_C static MCmMdhClient* NewCmMdhClientLC( MCmServiceObserver& aServer );

    };

#endif // C_CMMDHCLIENTFACTORY_H
