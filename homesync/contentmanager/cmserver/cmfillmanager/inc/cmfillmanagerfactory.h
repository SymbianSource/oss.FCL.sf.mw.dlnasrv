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
* Description:  Abstract factory for Fill Manager
*
*/



#ifndef C_CMFILLMANAGERFACTORY_H
#define C_CMFILLMANAGERFACTORY_H

#include <e32std.h>

class MCmFmManager;
class MCmServiceObserver;
class CMdESession;
class CCmDmMain;
class CCmMmMain;

/**
 *  Abstract factory for Fill manager
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
class CmFillManagerFactory
    {

public:

    /**
     * Creates a new instance of Fill Manager
     *
     * @since S60 5.1
     * @param aObserver, service observer
     * @param aSession, mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to CCmFmMain class
     */
    IMPORT_C static MCmFmManager* NewFillManagerL( 
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    /**
     * Creates a new instance of Fill Manager. Instance is left in
     * cleanup stack.
     *
     * @since S60 5.1     
     * @param aObserver, service observer
     * @param aSession, mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return  pointer to MCmFmManager class
     */
    IMPORT_C static MCmFmManager* NewFillManagerLC( 
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    };

#endif // C_CMFILLMANAGERFACTORY_H

//  End of File
