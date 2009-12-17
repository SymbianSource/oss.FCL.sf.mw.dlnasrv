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
* Description:  Abstract factory for Store manager
*
*/



#ifndef C_CMSTOREMANAGERFACTORY_H
#define C_CMSTOREMANAGERFACTORY_H

#include <e32std.h>

class MCmSmManager;
class MCmServiceObserver;
class CMdESession;
class CCmDmMain;
class CCmMmMain;

/**
 * Abstract factory for Store manager
 *
 * @lib cmstoremanager.lib
 *
 * @since S60 5.1
 */
class CmStoreManagerFactory
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, Mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return instance of Store manager
     */
    IMPORT_C static MCmSmManager* NewStoreManagerL( 
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aSession, Mde session
     * @param aDBManager, database manager
     * @param aMemManager, memory manager
     * @return instance of Store manager
     */
    IMPORT_C static MCmSmManager* NewStoreManagerLC( 
        MCmServiceObserver* aObserver, CMdESession& aSession, 
        CCmDmMain* aDBManager, CCmMmMain* aMemManager );

    };

#endif // C_CMSTOREMANAGERFACTORY_H
