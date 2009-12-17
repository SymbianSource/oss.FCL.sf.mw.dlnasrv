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
* Description:      Includes SQL clauses for creating and dropping tables 
*
*/






#ifndef __CMDMSQLMAINTENANCECLAUSES_H__
#define __CMDMSQLMAINTENANCECLAUSES_H__

///////////////////////////////////////////////////////////////////
// DROP CLAUSES
///////////////////////////////////////////////////////////////////

// Drop tables
_LIT8( KDropFillRuleGroup,      "DROP TABLE FillRuleGroup;" );
_LIT8( KDropRules,              "DROP TABLE Rules;" );
_LIT8( KDropRuleParams,         "DROP TABLE RuleParams;" );
_LIT8( KDropRuleGroupServers,   "DROP TABLE RuleGroupServers;" );
_LIT8( KDropMediaServers,       "DROP TABLE MediaServers;" );
_LIT8( KDropHashValues,         "DROP TABLE HashValues;" );
_LIT8( KDropStored,             "DROP TABLE Stored;" );
_LIT8( KDropFilled,             "DROP TABLE Filled;" );
_LIT8( KDropIDs,                "DROP TABLE IDs;" );
_LIT8( KDropStoreRules,         "DROP TABLE StoreRules;" );
_LIT8( KDropStoreRuleParams,    "DROP TABLE StoreRuleParams;");
_LIT8( KDropStoreRuleServers,   "DROP TABLE StoreRuleServers;");

_LIT8( KDropFillFiles,          "DROP TABLE FillFiles;");
_LIT8( KDropStoreFiles,         "DROP TABLE StoreFiles;");
_LIT8( KDropStoreFileServers,   "DROP TABLE StoreFileServers;");
_LIT8( KDropImageShrinkingInfo, "DROP TABLE ImageShrinkingInfo;");
_LIT8( KDropTransferInfo,       "DROP TABLE TransferInfo;");
_LIT8( KDropSyncTime,           "DROP TABLE SyncTime;");
_LIT8( KDropProgressInfo,       "DROP TABLE ProgressInfo;"); 

_LIT8( KDropDrivesInfo,          "DROP TABLE DrivesInfo;");

///////////////////////////////////////////////////////////////////
// CREATE CLAUSES FOR TABLES
///////////////////////////////////////////////////////////////////

// ONTOLOGY TABLES
_LIT8( KCreateFillRuleGroup,
        "CREATE TABLE FillRuleGroup("
            "RuleGroupID        INTEGER NOT NULL,"
            "Name               TEXT NOT NULL,"
            "Method             INTEGER,"
            "LimitType          INTEGER,"
            "Quantity           INTEGER,"
            "Selected           INTEGER,"
            "Priority           INTEGER,"
            "TemplateId         INTEGER,"
            "MediaType          INTEGER,"
            "Status             INTEGER,"
            "RealCount          INTEGER,"
            "RealSize           INTEGER,"
            "PRIMARY KEY(RuleGroupID));"
            );

_LIT8( KCreateRules, 
        "CREATE TABLE Rules("
            "RuleID             INTEGER NOT NULL,"
            "RuleGroupID        INTEGER NOT NULL,"
            "MetadataField      INTEGER,"
            "Operator           INTEGER,"
            "PRIMARY KEY(RuleID));"
            );

_LIT8( KCreateRuleParams, 
        "CREATE TABLE RuleParams("
            "RuleParamID        INTEGER NOT NULL,"
            "RuleID             INTEGER NOT NULL,"
            "ComparisonFieldData TEXT,"
            "ComparisonFieldIndex INTEGER,"
            "PRIMARY KEY(RuleParamID));"
            );
            
_LIT8( KCreateRuleGroupServers, 
        "CREATE TABLE RuleGroupServers("
            "RuleGroupID        INTEGER NOT NULL,"
            "MediaServerID      INTEGER NOT NULL);"
            );
            
_LIT8( KCreateMediaServers, 
        "CREATE TABLE MediaServers("
            "MediaServerID          INTEGER NOT NULL,"
            "UDN                    TEXT NOT NULL,"
            "Name                   TEXT NOT NULL,"
            "SystemUpdateID         INTEGER,"
            "VisibleDate            DATETIME,"
            "DownloadData           INTEGER,"
            "UploadData             INTEGER,"
            "DownloadTime           INTEGER,"
            "UploadTime             INTEGER,"
            "IsActive               INTEGER,"
            "CopyCapability         INTEGER,"
            "FillUsage              INTEGER,"
            "StoreUsage             INTEGER,"
            "PRIMARY KEY(MediaServerID));"
            );

_LIT8( KCreateHashValues, 
        "CREATE TABLE HashValues("
            "HashValueID            INTEGER NOT NULL,"
            "MediaServerID          INTEGER NOT NULL,"
            "StartIndex             INTEGER NOT NULL,"
            "ItemCount              INTEGER NOT NULL,"
            "Hash                   TEXT NOT NULL,"
            "PRIMARY KEY(HashValueID));"
            );
            
            
_LIT8( KCreateStored, 
        "CREATE TABLE Stored("
            "StoredFile             TEXT NOT NULL,"
            "StoredID               INTEGER NOT NULL,"
            "MediaServerID          INTEGER NOT NULL," 
            "Date                   DATETIME,"
            "PRIMARY KEY(StoredID));"
            );           

_LIT8( KCreateIDs, 
        "CREATE TABLE IDs("
            "IDValue                INTEGER NOT NULL,"
            "IDType                 INTEGER NOT NULL,"
            "PRIMARY KEY(IDType));"
            );
            
_LIT8( KCreateStoreRules, 
        "CREATE TABLE StoreRules("
            "StoreRuleId            INTEGER NOT NULL,"
            "Name                   TEXT NOT NULL,"
            "Selected               INTEGER,"
            "Status                 INTEGER,"
            "PRIMARY KEY(StoreRuleId));"
            );
            
_LIT8( KCreateStoreRuleParams, 
        "CREATE TABLE StoreRuleParams("
            "StoreRuleId            INTEGER NOT NULL,"
            "StoreRuleParamId       INTEGER NOT NULL,"
            "MediaType              INTEGER,"
            "PRIMARY KEY(StoreRuleParamId));"
            );

_LIT8( KCreateStoreRuleServers, 
        "CREATE TABLE StoreRuleServers("
            "StoreRuleId            INTEGER NOT NULL,"
            "MediaServerID          INTEGER NOT NULL);"
            );
                   
_LIT8( KCreateFillFiles, 
        "CREATE TABLE FillFiles("
            "FileId                 INTEGER NOT NULL,"
            "PrimaryText            TEXT NOT NULL,"    
            "SecondaryText          TEXT,"    
            "Path                   TEXT,"
            "Uri                    TEXT NOT NULL,"
            "ItemId                 TEXT NOT NULL,"
            "UpnpClass              TEXT,"
            "MediaType              INTEGER,"
            "Priority               INTEGER,"// Highest priority is zero
            "Size                   INTEGER,"// In bytes
            "Status                 INTEGER,"                
            "Date                   DATETIME,"
            "FillListId             INTEGER NOT NULL,"
            "MediaServerId          INTEGER NOT NULL,"
            "Selected               INTEGER,"
            "RefId                  INTEGER,"
            "DriveId                INTEGER,"
            "DriveNumber            INTEGER );"
            //"PRIMARY KEY(FileId));"
            );
                    
_LIT8( KCreateStoreFiles, 
        "CREATE TABLE StoreFiles("
            "FileId                 INTEGER NOT NULL,"
            "PrimaryText            TEXT NOT NULL,"
            "SecondaryText          TEXT,"
            "Path                   TEXT,"
            "Size                   INTEGER,"
            "Date                   DATETIME,"
            "StoreListId            INTEGER NOT NULL,"
            "PRIMARY KEY(FileId));"
            );
                       
_LIT8( KCreateStoreFileServers, 
        "CREATE TABLE StoreFileServers("
            "Fid                    INTEGER NOT NULL,"
            "MediaServerId          INTEGER NOT NULL,"
            "Status                 INTEGER );"
            );                                    

_LIT8( KCreateImageShrinkingInfo, 
        "CREATE TABLE ImageShrinkingInfo("
            "Count                  INTEGER,"
            "Time                   INTEGER );"
            );
            
_LIT8( KCreateTransferInfo,
        "CREATE TABLE TransferInfo("
            "Service                INTEGER,"
            "Count                  INTEGER,"
            "Time                   INTEGER,"
            "PRIMARY KEY(Service));"
        );

_LIT8( KCreateSyncTime,
        "CREATE TABLE SyncTime("
        "Time                       DATETIME );"
        );
        
_LIT8( KCreateProgressInfo,
        "CREATE TABLE ProgressInfo("
        "Trasferred                 INTEGER,"
        "TotalCount                 INTEGER );"
        );        

_LIT8( KCreateDrivesInfo,
        "CREATE TABLE  DrivesInfo("
        "DriveNumber                INTEGER,"
        "DriveType                  INTEGER,"
        "DriveName                  TEXT,"
        "Size                       INTEGER,"
        "Quota                      INTEGER,"
        "UsedQuota                  INTEGER,"
        "UniqueId                   INTEGER,"
        "Active                     INTEGER );"
        );
               
///////////////////////////////////////////////////////////////////
// CREATE CLAUSES FOR INDEXES
///////////////////////////////////////////////////////////////////
_LIT8( KCreateStoreStatusIndex, 
        "CREATE INDEX Store_status_index ON StoreFiles(Status);"
        );

_LIT8( KCreateFillStatusIndex, 
        "CREATE INDEX Fill_status_index ON FillFiles(Status);"
        );        

_LIT8( KCreateFillListIdIndex, 
        "CREATE INDEX FillListIdIndex ON FillFiles(FillListId);"
        ); 

_LIT8( KCreateFillItemRefIndex,
        "CREATE INDEX FillList_Ref_Index ON FillFiles( FileId, "
        "RefId, Priority, Selected );"
        );
                
// Dropping indexes
_LIT8( KDropStoreStatusIndex, 
    "DROP INDEX Store_status_index;"
    );

_LIT8( KDropFillStatusIndex,
    "DROP INDEX Fill_status_index;"
    );    

_LIT8( KDropFillListIdIndex, 
        "DROP INDEX FillListIdIndex;"
        );
        
_LIT8( KDropFillItemRefIndex,
        "DROP INDEX FillList_Ref_Index;"
        );         
            
#endif // __CMDMSQLMAINTENANCECLAUSES_H__


