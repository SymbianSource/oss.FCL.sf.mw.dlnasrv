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
 *  Description : SQL clause definitions
 *
 */

#ifndef __UPNPSECSQLCLAUSEDEF_H__
#define __UPNPSECSQLCLAUSEDEF_H__

// CONSTANTS

///////////////////////////////////////////////////////////////////
// DROP CLAUSES
///////////////////////////////////////////////////////////////////
_LIT8( KUpnpSecSqlDropAddressList, "DROP TABLE IF EXISTS AddressList;");
_LIT8( KUpnpSecSqlDropFileList, "DROP TABLE IF EXISTS FileList;" );

///////////////////////////////////////////////////////////////////
// CREATE CLAUSES FOR TABLES
///////////////////////////////////////////////////////////////////
_LIT8( KUpnpSecSqlCreateAddressList, 
    "CREATE TABLE IF NOT EXISTS AddressList("
        "Ip                 INTEGER PRIMARY KEY);"
        );

_LIT8( KUpnpSecSqlCreateFileList, 
    "CREATE TABLE IF NOT EXISTS FileList("
        "Filename          TEXT PRIMARY KEY);"
        );

///////////////////////////////////////////////////////////////////
// INSERT STATEMENTS
///////////////////////////////////////////////////////////////////
//
// Inserting ip address into AddressList table
_LIT8( KUpnpSecSqlInsertIpAddress,
    "INSERT INTO AddressList( " 
    "Ip ) "
    "VALUES( ? );"
    );
// Inserting filename into FileList table
_LIT8( KUpnpSecSqlInsertFile,
    "INSERT INTO FileList( " 
    "Filename ) "
    "VALUES( ? );"
    );

///////////////////////////////////////////////////////////////////
// SELECT STATEMENTS
///////////////////////////////////////////////////////////////////
//
// Select all ip addresses from AddressList table
_LIT8( KUpnpSecSqlSelectAllIpAddresses,
    "SELECT Ip FROM AddressList "
    );

// Select all filenames from FileList table
_LIT8( KUpnpSecSqlSelectAllFiles, "SELECT Filename FROM FileList " 
    );

_LIT8( KUpnpSecSqlValidateTableExistence, "SELECT count(*) FROM AddressList, FileList;" );

// ---------------------------------------------------------------------------
// DELETE STATEMENTS
// ---------------------------------------------------------------------------
//
_LIT8( KUpnpSecSqlDeleteIpAddress, 
    "DELETE FROM AddressList "
    "WHERE Ip = ? ;"
    );
_LIT8( KUpnpSecSqlDeleteAllIpAddresses, 
    "DELETE FROM AddressList;"
    );
    
_LIT8( KUpnpSecSqlDeleteFile, 
    "DELETE FROM FileList "
    "WHERE Filename = ? ;"
    );
_LIT8( KUpnpSecSqlDeleteAllFiles, 
    "DELETE FROM FileList;"
    );

///////////////////////////////////////////////////////////////////
// MISC DEFINITIONS
///////////////////////////////////////////////////////////////////
//
_LIT16( KUpnpSecSqlIp, "Ip" );
_LIT16( KUpnpSecSqlFilename, "Filename" );

#endif // __UPNPSECSQLCLAUSEDEF_H__
// End of File
