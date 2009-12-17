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







#ifndef C_CMSQLDBMAINTENANCE_H
#define C_CMSQLDBMAINTENANCE_H

#include <e32base.h>

// Forward declarations
class CCmSqlConnection;

/**
 *  CCmSqlDbMaintenance class
 *  Part of SQL wrapper. SQL wrapper is part of
 *  Content manager component.CCmSqlDbMaintenance creates needed
 *  database tables and initializes connenction to the database
 *
 *  @lib cmsqlwrapper.lib
 *
 *  @since S60 v3.1
 */
class CCmSqlDbMaintenance : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCmSqlDbMaintenance* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCmSqlDbMaintenance* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmSqlDbMaintenance();    
    
    /**
     * Initializes database connection
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return Error code
     */    
    TInt InitConnection( CCmSqlConnection& aConnection );

    /**
     * Closes database connection
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return None
     */        
    void CloseConnection( CCmSqlConnection& aConnection );

    /**
     * Creates db file
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return Error code
     */  
    TInt CreateDbFile( CCmSqlConnection& aConnection );    
    
    /**
     * Creates database tables
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return None
     */        
    void CreateDb( CCmSqlConnection& aConnection );

    /**
     * Creates database indexes
     *
     * @since S60 5.1
     * @param aConnection, reference to connection class
     * @return None
     */        
    void CreateIndexes( CCmSqlConnection& aConnection );

private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmSqlDbMaintenance();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private: // data

    };

#endif //  C_CMSQLDBMAINTENANCE_H
