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
* Description:      SQL database maintenance class
*
*/







#ifndef C_CMDMSQLDBMAINTENANCE_H
#define C_CMDMSQLDBMAINTENANCE_H

#include <e32base.h>

/** Literals */
_LIT( KCmDatabase, "c:\\System\\Data\\MediaServant\\cm_database.sq");
_LIT( KCmDbDir, "c:\\System\\Data\\MediaServant\\");

/* Forward declarations. */
class CCmDmSQLiteConnection;

/**
 *  CCmDmSqlDbMaintenance class
 *  Part of Database manager. Database manager is part of
 *  Content manager component.CCMDMSqlDbMaintenance creates needed
 *  database tables and initializes connenction to the database
 *
 *  @lib cmdatabasemanager.lib
 *
 *  @since S60 5.1
 */
class CCmDmSqlDbMaintenance : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCmDmSqlDbMaintenance* NewL();
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCmDmSqlDbMaintenance* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmDmSqlDbMaintenance();    
    
    /**
     * Initializes database connection
     *
     * @since S60 5.1
     * @param None
     * @return TInt, Error code
     */    
    TInt InitConnection( CCmDmSQLiteConnection& aConnection );

    /**
     * Closes database connection
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void CloseConnection( CCmDmSQLiteConnection& aConnection );

    /**
     * Creates db file
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return TInt, Error code
     */  
    TInt CreateDbFile( CCmDmSQLiteConnection& aConnection );
        
    /**
     * Creates database tables
     *
     * @since S60 5.1
     * @param aConnection, sqlite connection
     * @return None
     */        
    void CreateDb( CCmDmSQLiteConnection& aConnection );

    /**
     * Creates database indexes
     *
     * @since S60 5.1
     * @param aConnection, sqlite connection
     * @return None
     */        
    void CreateIndexes( CCmDmSQLiteConnection& aConnection );

private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmDmSqlDbMaintenance();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

    };

#endif //  C_CMDMSQLDBMAINTENANCE_H
