/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 *  Description : SQLite Connection class
 *
 */

#ifndef UPNPSECURITYDBCONNECTION_H
#define UPNPSECURITYDBCONNECTION_H

// INCLUDES
#include <e32base.h>
#include <sqldb.h>
#include <f32file.h> //TDriveNumber

class TInetAddr;

// CLASS DECLARATION

/**
 * 
 */
class CUpnpSecurityDbConnection : public CBase
    {
    
public:

    /* Constructors and destructor. */

    /**
     * Creates new CUpnpSecurityDbConnection class and initializes 
     * connection to SQLite.
     * @return  pointer to CUpnpSecurityDbConnection class
     */
    static CUpnpSecurityDbConnection* NewL();

    /**
     * Creates new CUpnpSecurityDbConnection class and initializes 
     * connection to SQLite and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CUpnpSecurityDbConnection class
     */
    static CUpnpSecurityDbConnection* NewLC();

    /**
     * Destructor.
     */
    virtual ~CUpnpSecurityDbConnection();

public:
    /**
     * Open database.
     */
    TInt OpenDb( const TDesC& aDb );

    /**
     * Close opened database
     */
    void CloseDb();

    /**
     * Creates db file.
     * @param aDb, Database filename and path
     * @return error code
     */
    TInt CreateDbFile( const TDesC& aDb );

    /**
     * Validates database existence and format.
     *
     * @return ETrue if db valid
     */
    TBool Validate();

    /**
     * Executes sql command.
     *
     * @param aCommand, command to be executed
     */
    void ExecuteL( const TDesC8& aCommand );

    /**
     * Create tables.
     */
    void CreateTablesL();

    /**
     * Add item (IP address) to DB.
     */
    void AddIpAddressL( const TInetAddr& aIpAddress );

    /**
     * Add item (file name) to DB.
     */
    void AddFilenameL( const TDesC& aFilename );

    /**
     * Delete item (IP address) from DB.
     */
    void DeleteIpAddressL( const TInetAddr& aIpAddress );

    /**
     * Delete item (file name) from DB.
     */
    void DeleteFilenameL( const TDesC& aFilename );

    /**
     * Delete all items (IP addresses) from DB.
     */
    void DeleteAllIpAddressesL();

    /**
     * Delete all items (file names) from DB.
     */
    void DeleteAllFilenamesL();

    /**
     * Retreive data from DB.
     */
    void GetAllIpAddressesL( RArray<TInetAddr>& aAddressArray );

    /**
     * Retreive data from DB.
     */
    void GetAllFilenamesL( RPointerArray<HBufC>& aFilenameArray );

    /**
     * Prepare database for use, create new if not exist.
     */
    void OpenDatabaseL();

private:

    /**
     * Constructor.
     */
    CUpnpSecurityDbConnection();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
    /**
     * Leaves if there is not enough disk space 
     */
    void EnsureDiskSpaceL();

    /**
     * Returns drive where db files are stored
     */
    TDriveNumber DatabaseDrive();
    
    /**
     * Execute given db command
     * 
     * @param aCommand command do execute
     * @param aType type of statement
     */
    void ExecStatementL( const TDesC8& aCommand );

    /**
     * Execute given db command
     * 
     * @param aCommand command do execute
     * @param aTextToBind command text argument
     */
    void ExecStatementL( const TDesC8& aCommand, const TDesC& aTextToBind );
    
    /**
     * Execute given db command
     *
     * @param aCommand command do execute
     * @param aType command int argument 
     */
    void ExecStatementL( const TDesC8& aCommand, const TInt aIntToBind );
    
private:
    // data

    // iDatabase, interface to Sqlite database
    RSqlDatabase iDatabase;

    // database file name
    TFileName iDbFileName;

    //
    RFs iFs;
    
    };

#endif //  UPNPSECURITYDBCONNECTION
// End of File
