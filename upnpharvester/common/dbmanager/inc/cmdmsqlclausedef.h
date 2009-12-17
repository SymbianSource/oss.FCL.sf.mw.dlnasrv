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
* Description:      SQL clause definitions
*
*/






#ifndef __CMDMSQLCLAUSEDEF_H__
#define __CMDMSQLCLAUSEDEF_H__

// ---------------------------------------------------------------------------
// SELECT clauses
// ---------------------------------------------------------------------------
//
_LIT8( KSelectDeviceInfoByUDN, "SELECT " 
    "MediaServerID, UDN, Name, SystemUpdateID, VisibleDate, IsActive, FillUsage " 
    "FROM MediaServers " 
    "WHERE UDN = ?"
    );
    
_LIT8( KSelectAllMediaServers, "SELECT " 
    "UDN, Name, SystemUpdateID, VisibleDate, IsActive, MediaServerID, "
    "CopyCapability, FillUsage, StoreUsage " 
    "FROM MediaServers"
    );
        
_LIT8( KSelectMediaServerIDByUDN, 
    "SELECT "
    "MediaServerID "
    "FROM MediaServers "
    "WHERE UDN = ?"
    );

_LIT8( KSelectHashValuesByMediaServer, "SELECT " 
    "StartIndex, ItemCount, Hash " 
    "FROM HashValues "
    "WHERE MediaServerID = ?"
    );
    
_LIT8( KSelectMediaServerUDNByID, 
    "SELECT "
    "UDN "
    "FROM MediaServers "
    "WHERE MediaServerID = ?"
    );       

_LIT8( KSelectStoredByUDN, 
    "SELECT "
    "StoredFile " 
    "FROM Stored " 
    "WHERE MediaServerID = ( " 
    "SELECT MediaServerID "
    "FROM MediaServers "
    "WHERE UDN = ? )"
    );
    
_LIT8( KIsFileAlreadyStored,
    "SELECT "
    "MediaServerID "
    "FROM MediaServers "
    "WHERE MediaServerID = ( "    
    "SELECT "
    "MediaServerID "
    "FROM Stored "
    "WHERE StoredFile = ? ) "
    "AND "
    "UDN = ?"   
    );  

_LIT8( KSelectID,
    "SELECT "
    "IDValue "
    "FROM IDs "
    "WHERE IDType = ?"
    );

_LIT8( KSelectRuleGroupID,
    "SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ?"
    );
    
_LIT8( KSelectStoreRuleID,
    "SELECT "
    "StoreRuleId "
    "FROM StoreRules "
    "WHERE Name = ?"
    );

// Loading Fill Rule 1.    
_LIT8( KSelectFillRule,
    "SELECT "
    "RuleGroupID, Name, Method, LimitType, Quantity, Selected, Priority, "
    "TemplateId, MediaType, Status, RealCount, RealSize "
    "FROM FillRuleGroup "
    "WHERE Name = ? "
    "ORDER BY Priority ASC"
    );
    
// Loading Fill Rule 2.                
_LIT8( KSelectRule,
    "SELECT "
    "RuleID, MetadataField, Operator "
    "FROM Rules "
    "JOIN FillRuleGroup frg ON Rules.RuleGroupID = frg.RuleGroupID "
    "WHERE frg.Name = ?"
    );

// Loading Fill Rule 3.    
_LIT8( KSelectRuleParams, 
    "SELECT "
    "ComparisonFieldData "
    "FROM RuleParams "
    "WHERE RuleID = ( SELECT "
    "RuleID "
    "FROM Rules "
    "WHERE RuleGroupID = ( SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? ))"
    );

// Loading Fill Rule 3.1    
_LIT8( KSelectRuleParamsByRuleID, 
    "SELECT "
    "ComparisonFieldData, ComparisonFieldIndex "
    "FROM RuleParams "
    "WHERE RuleID = ?"
    );
    
// Loading Fill Rule 4.    
_LIT8( KSelectMediaServerID, 
    "SELECT "
    "MediaServerID "
    "FROM RuleGroupServers "
    "WHERE RuleGroupID = ( SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? )"
    );

// Loading Fill Rule 4.1    
_LIT8( KSelectMediaServerIDByRuleGroupID, 
    "SELECT "
    "MediaServerID "
    "FROM RuleGroupServers "
    "WHERE RuleGroupID = ?"
    );
    
// Loading Fill Rule 5.    
_LIT8( KSelectRuleUDNs, 
    "SELECT "
    "UDN "
    "FROM MediaServers "
    "WHERE MediaServerID = ( SELECT "
    "MediaServerID "
    "FROM RuleGroupServers "
    "WHERE RuleGroupID = ( SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? ))"
    );

// Loading Fill Rule 5.1    
_LIT8( KSelectRuleUDNsByMediaServerID, 
    "SELECT "
    "UDN "
    "FROM MediaServers "
    "WHERE MediaServerID = ?"
    );
    
// Loading selected ( or not selected rules )    
_LIT8( KSelectSelectedFillRuleNames, 
    "SELECT "
    "Name "
    "FROM FillRuleGroup "
    "WHERE Selected = ?"
    );        

// Loading all fill rule names
_LIT8( KSelectFillRuleNames, 
    "SELECT "
    "Name "
    "FROM FillRuleGroup"
    );

_LIT8( KSelectFillRuleName, 
    "SELECT Name "
    "FROM FillRuleGroup "
    "WHERE RuleGroupID = ?"
    );
    
// Loading Store Rule 1.    
_LIT8( KSelectStoreRule,
    "SELECT "
    "StoreRuleID, Name, Selected, Status "
    "FROM StoreRules "
    "WHERE Name = ?"
    );

// Loading Store Rule 2.                
_LIT8( KSelectStoreRuleDefs,
    "SELECT "
    "StoreRuleParamId, MediaType "
    "FROM StoreRuleParams "
    "WHERE StoreRuleID = ( SELECT "
    "StoreRuleID "
    "FROM StoreRules "
    "WHERE Name = ? )"
    );

// Loading Store rule 3.    
_LIT8( KSelectStoreRuleMediaServerID, 
    "SELECT "
    "MediaServerID "
    "FROM StoreRuleServers "
    "WHERE StoreRuleID = ?"
    );
    
// Loading all store rule names
_LIT8( KSelectStoreRuleNames, 
    "SELECT "
    "Name "
    "FROM StoreRules"
    );        

// Loading selected ( or not selected rules )    
_LIT8( KSelectSelectedStoreRuleNames, 
    "SELECT "
    "Name "
    "FROM StoreRules "
    "WHERE Selected = ?"
    );    

_LIT8( KSelectFillFiles, 
    "SELECT "
    "PrimaryText, SecondaryText, Path, Size, Status, Date, FileId, RefId "
    "FROM FillFiles "
    "WHERE ((Status & ? ) AND ( FillListId = "
    "( SELECT RuleGroupId FROM FillRuleGroup WHERE Name = ? ) ) ) "
    "ORDER BY PrimaryText"
    );
    
_LIT8( KSelectDeletedFillFiles,
    "SELECT "
    "PrimaryText, SecondaryText, Path, Size, Status, Date, FileId "
    "FROM FillFiles "
    "WHERE ((Status & ? ) AND ( Selected = '2' ) )"
    "ORDER BY PrimaryText"
    );
        
_LIT8( KSelectAllFillFiles, 
    "SELECT "
    "FileId, PrimaryText, SecondaryText, Path, Uri, ItemId, UpnpClass, "
    "MediaType, Priority, Size, Status, Date, FillListId, MediaServerId, "
    "RefId, Selected, DriveId, DriveNumber "
    "FROM FillFiles "
    "ORDER BY Priority ASC"
    );    
    
_LIT8( KGetAllFillFiles, 
    "SELECT "
    "FileId, PrimaryText, SecondaryText, Path, Uri, ItemId, UpnpClass, "
    "MediaType, Priority, Size, Status, Date, FillListId, MediaServerId, "
    "RefId, Selected, DriveId, DriveNumber "
    "FROM FillFiles "
    "WHERE ( Status & ? ) "
    "ORDER BY Priority ASC"
    );    
        
_LIT8( KSelectStoreFiles, 
    "SELECT "
    "PrimaryText, SecondaryText, Path, Size, Date "
    "FROM StoreFiles "
    "WHERE (StoreListId = "
    "( SELECT StoreRuleID FROM StoreRules WHERE Name = ? )) "
    "ORDER BY PrimaryText"
    );

_LIT8( KSelectAllStoreFiles, 
    "SELECT "
    "FileId, PrimaryText, SecondaryText, Path, Size, sfd.Status, "
    "StoreListId, sfd.MediaServerId "
    "FROM StoreFiles JOIN StoreFileServers sfd ON FileId = sfd.Fid"    
    );

_LIT8( KGetAllStoreFiles, 
    "SELECT "
    "PrimaryText, SecondaryText, Path, Size, sfd.Status, "
    "StoreListId, sfd.MediaServerId "
    "FROM StoreFiles JOIN StoreFileServers sfd ON FileId = sfd.Fid "
    "WHERE ( sfd.Status & ? )"
    );         

_LIT8( KSelectLowestPriorityFilled,
    "SELECT "
    "Path, Size, FillListId, FileId "
    "FROM FillFiles "
    "WHERE ( Status & ? ) "
    "AND ( Priority = "
    "( SELECT MAX(Priority) FROM FillFiles "
    "WHERE ( Status & ? AND RefId = '0' AND DriveNumber = ? ) ) )"
    "ORDER BY Priority DESC"
    );

_LIT8( KSelectTransferHistory, 
    "SELECT "
    "DownloadData, UploadData, DownloadTime, UploadTime "
    "FROM MediaServers "
    "WHERE UDN = ?"
    );

// Count all files from selected store lists ( multiple servers included )
_LIT8( KStoreFileCount,
    "SELECT count(*) "
    "FROM StoreRules "
    "JOIN StoreFiles files,  StoreFileServers servers ON files.StoreListId = StoreRuleId "
    "WHERE selected = '1' AND servers.FId = files.FileId  AND servers.status & ?" 
    );

_LIT8( KStoreFileCountNoDuplicates,
    "SELECT count(*) "
    "FROM StoreRules "
    "JOIN StoreFiles files ON files.StoreListId = StoreRuleId "
    "WHERE selected = '1'" 
    );
        
_LIT8( KFillFileCount,
    "SELECT count(*) "
    "FROM FillFiles "
    "WHERE status & ? AND RefId = '0' AND Selected = '1'"
    );
    
_LIT8( KImageShrinkingInfo,
    "SELECT Count, Time "
    "FROM ImageShrinkingInfo"
    );

_LIT8( KTranferValues,
    "SELECT Count "
    "FROM TransferInfo "
    "WHERE Service = ?"
    );

_LIT8( KStoreListFileCount,
    "SELECT count(*) "
    "FROM StoreFiles JOIN StoreRules sr ON StoreListId = sr.StoreRuleId "
    "WHERE sr.Name = ?"
    ); 

_LIT8( KFillListFileCount,
    "SELECT count(*) "
    "FROM FillFiles "
    "WHERE (( Status & ?) AND (FillListId = "
    "(SELECT RuleGroupID FROM FillRuleGroup WHERE Name = ?)) AND "
    "( Selected = '1') )"
    );
    
_LIT8( KGetSyncTime, 
    "SELECT * "
    "FROM SyncTime" 
    );               

_LIT8( KGetProgressInfo,
    "SELECT Trasferred, TotalCount "
    "FROM ProgressInfo" 
    ); 

_LIT8( KGetFillListStatus,
    "SELECT Status "
    "FROM FillRuleGroup "
    "WHERE Name = ?"
    );
    
_LIT8( KGetDrives, 
    "SELECT DriveNumber, DriveType, DriveName, Size, Quota, UsedQuota, "
    "UniqueId, Active "
    "FROM DrivesInfo "
    "ORDER BY DriveType ASC"
    );
                                  
// ---------------------------------------------------------------------------
// INSERT clauses
// ---------------------------------------------------------------------------
//    
// FillRuleGroup table
_LIT8( KAddFillGroup,
    "INSERT INTO FillRuleGroup("
        "RuleGroupID, Name, Method, LimitType, Quantity, "
        "Selected, Priority, TemplateId, MediaType, Status, RealCount, "
        "RealSize )"
        "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );
        
// Rules table
_LIT8( KAddRule,
    "INSERT INTO Rules("
        "RuleID, RuleGroupID, MetadataField, Operator )"
        "VALUES(?, ?, ?, ?)"
        );
        
_LIT8( KAddRuleParam,
    "INSERT INTO RuleParams("
    "RuleParamID, RuleID, ComparisonFieldData, ComparisonFieldIndex )"
    "VALUES(?, ?, ?, ?)"
    );
    
_LIT8( KAddRuleGroupServer,
    "INSERT INTO RuleGroupServers("
    "RuleGroupID, MediaServerID )"
    "VALUES(?, ?)"
    );            

_LIT8( KAddMediaServer,
    "INSERT INTO MediaServers("
    "MediaServerID, UDN, Name, SystemUpdateID, VisibleDate, "
    "DownloadData, UploadData, DownloadTime, UploadTime, "
    "IsActive, CopyCapability, FillUsage, StoreUsage )"
    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

_LIT8( KAddHashValue,
    "INSERT INTO HashValues("
    "MediaServerID, StartIndex, ItemCount, Hash )"
    "VALUES(?, ?, ?, ?)"
    );
    
    
_LIT8( KAddStored,
    "INSERT INTO Stored("
    "StoredID, StoredFile, MediaServerID, Date )"
    "VALUES(?, ?, ?, ?)"
    );
            
_LIT8( KAddIDs,
    "INSERT INTO IDs("
    "IDValue, IDType )"
    "VALUES(?, ?)"
    );
    
_LIT8( KAddStoreRule,
    "INSERT INTO StoreRules("
    "StoreRuleId, Name, Selected, Status )"
    "VALUES(?, ?, ?, ?)"
    );
    
_LIT8( KAddStoreRuleParams,
    "INSERT INTO StoreRuleParams("
    "StoreRuleId, StoreRuleParamId, MediaType )"
    "VALUES(?, ?, ?)"
    );    

_LIT8( KAddStoreRuleServers,
    "INSERT INTO StoreRuleServers("
    "StoreRuleId, MediaServerID )"
    "VALUES(?, ?)"
    );

_LIT8( KAddFillFile, 
    "INSERT INTO FillFiles("
    "FileId, PrimaryText, SecondaryText, Path, Uri, ItemId, UpnpClass, "
    "MediaType, Priority, Size, Status, Date, FillListId, MediaServerId, "
    "RefId, Selected, DriveId, DriveNumber ) "
    "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"  
    );

_LIT8( KAddStoreFile, 
    "INSERT INTO StoreFiles("
    "FileId, PrimaryText, SecondaryText, Path, Size, Date, StoreListId) "
    "VALUES(?, ?, ?, ?, ?, ?, ?)"  
    );    
           
_LIT8( KAddStoreFileServer, 
    "INSERT INTO StoreFileServers("
    "Fid, MediaServerId, Status ) "
    "VALUES(?, ?, ?)"  
    );
        
_LIT8( KInitShrinkingTime,
    "INSERT INTO ImageShrinkingInfo("
    "Count, Time ) "
    "VALUES(0, 0)"      
    );    
        
_LIT8( KInitTransferInfo,
     "INSERT INTO TransferInfo("
    "Service, Count, Time ) "
    "VALUES(?,0,0)"    
    );        

_LIT8( KSetSyncTime,
    "INSERT INTO SyncTime("
    "Time ) "
    "VALUES( ? )"
    );
    
_LIT8( KSetProgressInfo,
    "INSERT INTO ProgressInfo( "
    "Trasferred, TotalCount ) "
    "VALUES( ?, ? )"
    ); 

_LIT8( KSetDrivesInfo,
    "INSERT INTO DrivesInfo("
    "DriveNumber, DriveType, DriveName, Size, Quota, UsedQuota, UniqueId, "
    "Active ) "
    "VALUES( ?, ?, ?, ?, ?, ?, ?, ? )"
    ); 
                            
// count queries
   
_LIT8( KValidateTableExistence,
    "SELECT count(*)"
        " FROM IDs"
        );

// Sum queries
_LIT8( KFillListSize,
    "SELECT SUM(Size) "
    "FROM FillFiles "
    "WHERE (( Status & ?) AND (FillListId = "
    "(SELECT RuleGroupID FROM FillRuleGroup WHERE Name = ?)))"
    );
    
_LIT8( KStoreListSize,
    "SELECT SUM(Size) "
    "FROM StoreFiles "
    "WHERE ( StoreListId = "
    "( SELECT StoreRuleId FROM StoreRules WHERE Name = ? ))"
    );    

_LIT8( KDataAmountFilled,
    "SELECT SUM(Size) "
    "FROM FillFiles "
    "WHERE (( Status & 2 ) AND ( FillListId = ? ))" 
    );
_LIT8( KDataAmountToBeFilled,
    "SELECT SUM(Size) "
    "FROM FillFiles "
    "WHERE (( Status & ?) AND (MediaServerId = ?) AND "
    "( Selected = '1'))" 
    );

_LIT8( KDataAmountToBeStored,
    "SELECT SUM(Size) "
    "FROM StoreFiles JOIN StoreFileServers sfd ON FileId = sfd.Fid "
    "WHERE ((sfd.MediaServerId = ?) AND (sfd.Status & ?))" 
    );

_LIT8( KTranferTime,
    "SELECT Time "
    "FROM TransferInfo "
    "WHERE Service = ?"
    );

_LIT8( KGetStoreFileCountNoDuplicates, 
    "SELECT COUNT( DISTINCT Fid ) "
    "FROM StoreFileServers servers "
    "JOIN StoreFiles files, StoreRules rules "
    "ON (files.FileId = servers.Fid AND rules.StoreRuleId = files.StoreListId) "
    "WHERE (servers.Status & ? AND rules.Selected = 1)" 
    );    
                             
// ---------------------------------------------------------------------------
// UPDATE / REPLACE clauses
// ---------------------------------------------------------------------------
//
_LIT8( KUpdateMediaServerInfo,
    "UPDATE MediaServers"
        " SET Name = ?, "
        "SystemUpdateID = ?, "
        "VisibleDate = ?"
        " WHERE UDN = ?"
        );
        
_LIT8( KUpdateFillRule,
    "UPDATE FillRuleGroup"
    " SET Quantity = ?"
    " WHERE Name = ?"
    );
    
_LIT8( KUpdateID,
    "UPDATE IDs"
    " SET IDValue = ?"
    " WHERE IDType = ?"
    );
    
// Updating Fill rules selected state
_LIT8( KUpdateFillRuleSelected, 
    "UPDATE FillRuleGroup"
    " SET Selected = ?"
    " WHERE Name = ?"
    );

// Updating Fill rules selected state
_LIT8( KUpdateFillRuleRealInformation, 
    "UPDATE FillRuleGroup"
    " SET RealCount = ?, RealSize = ?"
    " WHERE RuleGroupID = ?"
    );      

// Updating download history
_LIT8( KUpdateDownloadHistory,
    "UPDATE MediaServers "
    "SET DownloadData = DownloadData + ?, DownloadTime = DownloadTime + ? "
    "WHERE MediaServerId = ?"
    );
        
// Updating upload history        
_LIT8( KUpdateUploadHistory,
    "UPDATE MediaServers "
    "SET UploadData = UploadData + ?, UploadTime = UploadTime + ? "
    "WHERE MediaServerId = ?"
    );
    
_LIT8( KIncrementImageShrinkingInfo,
    "UPDATE ImageShrinkingInfo "
    "SET Count = Count + ?, Time = Time + ?"
    );    

_LIT8( KUpdateTransferInfo,
    "UPDATE TransferInfo "
    "SET Count = ?, Time = ? "
    "WHERE Service = ?"
    );

//updates active status, name, fillusage and storeusage
_LIT8( KUpdateMsIsActive,
    "UPDATE MediaServers "
    "SET IsActive = ?, SystemUpdateID = ?, Name = ?, FillUsage = ?, StoreUsage = ? "
    "WHERE UDN = ?"
    );
    
_LIT8( KInitIsActiveFlags,
    "UPDATE MediaServers "
    "SET IsActive = '0'"
    );    

_LIT8( KUpdateFileStatus,
    "UPDATE FillFiles "
    "SET Status = ? "
    "WHERE ( ( FileId = ? ) OR ( RefId = ? ) )"
    );

_LIT8( KUpdateFillItemPriorities, 
    "UPDATE FillFiles "
    "SET Priority = ?, Selected = ? "
    "WHERE FillListId = "
    "( SELECT RuleGroupID FROM FillRuleGroup "
    "WHERE Name = ? )"
    );

//Updates Fill item's status if the status isn't equal to
//ECmFilled    
_LIT8( KUpdateFillItemStatusByRuleName, 
    "UPDATE FillFiles "
     "SET Status = ? "
    "WHERE  FillListId = "
    "( SELECT RuleGroupID FROM FillRuleGroup "
    "WHERE Name = ? )"
    "AND ( Status > '2' OR Status < '2' )"
    "AND ( Status != '64' )"
    );
           
// Updates highest priority item as a target for
// the reference
_LIT8( KUpdateFillItemRef_Zero,
    "UPDATE FillFiles "
    "SET RefId = '0' WHERE ( Priority = "
    "( SELECT MIN( Priority ) FROM FillFiles "
    "WHERE Selected = '1' ) )"
    );

_LIT8( KRestoreListItemStatuses, 
    "UPDATE FillFiles "
    "SET Status = ? "
    "WHERE (( FillListId = ? ) AND ( RefId = '0' ) )"
    );

_LIT8( KRestoreFileStatus, 
    "UPDATE FillFiles "
    "SET Status = ? "
    "WHERE ( FileId = ? OR RefId = ? )"
    );

_LIT8( KUpdateSelectionStatus,
    "UPDATE FillFiles "
    "SET Selected = ?, "
    "FillListId = 255, "
    "Priority = 255 "
    "WHERE FillListId = ?"
    );

_LIT8( KUpdateStoreTransferStatus, 
    "UPDATE StoreFileServers SET Status = ? WHERE "
    "Fid = ? AND MediaServerId = ?;"
    );
            
_LIT8( KUpdateFillTransferStatus, 
    "UPDATE FillFiles SET Status = ?, Path = ?, "
    "DriveId = ?, DriveNumber = ? WHERE "
    "MediaServerId =? AND ((FileId =? AND FillListId =?) OR (RefId =?));"
    );

_LIT8( KUpdateFillDriveInfo,
    "UPDATE FillFiles "
        "SET DriveNumber = ?, "
        "DriveId = ? "
        "WHERE DriveId = ?"
        );

_LIT8( KUpdateFillDriveAndStatusInfo,
    "UPDATE FillFiles "
        "SET Path = ?, "
        "DriveNumber = ?, "
        "DriveId = ?, "
        "Status = ? " // Set all to EToBeFilled
        "WHERE DriveId = ?"
        );
                                            
// ---------------------------------------------------------------------------
// REMOVE clauses
// ---------------------------------------------------------------------------
//
_LIT8( KRemoveMediaServersByUDN,
    "DELETE FROM MediaServers"
    " WHERE UDN = ?"
    );
    
_LIT8( KRemoveFillRuleByName,
    "DELETE FROM FillRuleGroup"
    " WHERE Name = ?"
    );
        
_LIT8( KRemoveRuleByRuleGroupID,
    "DELETE FROM Rules"
    " WHERE RuleGroupID = (SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? )"
    );

_LIT8( KRemoveRuleParamsByRuleID,
    "DELETE FROM RuleParams"
    " WHERE RuleID = (SELECT "
    "RuleID "
    "FROM Rules "
    "WHERE RuleGroupID = (SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? ) )"
    );

_LIT8( KRemoveRuleGroupServers,
    "DELETE FROM RuleGroupServers "
    "WHERE RuleGroupID = (SELECT "
    "RuleGroupID "
    "FROM FillRuleGroup "
    "WHERE Name = ? )"
    );
    
/** Delete all fill rules */ 
   
_LIT8( KRemoveFillRuleAll,
    "DELETE FROM FillRuleGroup"
    );
        
_LIT8( KRemoveRuleAll,
    "DELETE FROM Rules"
    );

_LIT8( KRemoveRuleParamAll,
    "DELETE FROM RuleParams"
    );

_LIT8( KRemoveRuleGroupServerAll,
    "DELETE FROM RuleGroupServers"
    );
    
/** Delete all store rules */

_LIT8( KRemoveStoreRuleServers,
    "DELETE FROM StoreRuleServers"
    );
        
_LIT8( KRemoveStoreRuleParams,
    "DELETE FROM StoreRuleParams"
    );

_LIT8( KRemoveStoreRules,
    "DELETE FROM StoreRules"
    );

_LIT8( KRemoveRuleGroupServer,
    "DELETE FROM RuleGroupServers "
    "WHERE RuleGroupID = ? AND MediaServerID = ?"
    );
    
_LIT8( KRemoveStoreRuleServer,
    "DELETE FROM StoreRuleServers "
    "WHERE StoreRuleID = ? AND MediaServerID = ?"
    );

/** Delete All Filled files */
_LIT8( KRemoveFillFiles, 
    "DELETE FROM FillFiles"
    );
    
/** Delete all fill rule's files */
_LIT8( KRemoveFillList, 
    "DELETE FROM FillFiles "
    "WHERE FillListId = ( SELECT RuleGroupID FROM FillRuleGroup "
    "WHERE Name = ?)"
    );    
    
/** Delete all fillFiles by defined FillListId */
_LIT8( KRemoveFillListItems, 
    "DELETE FROM FillFiles "
    "WHERE FillListId = ?"
    );
    
/** Delete Stored file */
_LIT8( KRemoveStoreFile, 
    "DELETE FROM StoreFiles "
    "WHERE PrimaryText = ?"
    );    
        
/** Delete All Stored files */
_LIT8( KRemoveStoreFiles, 
    "DELETE FROM StoreFiles"
    );
    
_LIT8( KRemoveStoreFileServers,
    "DELETE FROM StoreFileServers"
    );    
 
/** Delete in active fill rule servers */
_LIT8( KRemoveInActiveFillRuleServers,
    "DELETE FROM RuleGroupServers "
    "WHERE MediaServerID IN ( SELECT MediaServerID "
    "FROM MediaServers WHERE MediaServers.FillUsage = '0' )"
    ); 

/** Delete in active store rule servers */    
_LIT8( KRemoveInActiveStoreRuleServers,
    "DELETE FROM StoreRuleServers "
    "WHERE MediaServerID IN ( SELECT MediaServerID "
    "FROM MediaServers WHERE StoreUsage = '0')"    
    );
    
/** Delete in active fill files */    
_LIT8( KRemoveInActiveFillFiles,
    "DELETE FROM FillFiles "
    "WHERE MediaServerId IN ( SELECT MediaServerID "
    "FROM MediaServers WHERE FillUsage = '0' )" 
    );    

/** Delete in active store file servers */
_LIT8( KRemoveInActiveStoreFilesServers,
    "DELETE FROM StoreFileServers "
    "WHERE MediaServerId IN ( SELECT MediaServerID "
    "FROM MediaServers WHERE StoreUsage = '0')" 
    ); 

/** Delete in active store files */
_LIT8( KRemoveInActiveStoreFiles,
    "DELETE FROM StoreFiles "
    "WHERE FileId NOT IN ( SELECT Fid "
    "FROM StoreFileServers )"    
    );

_LIT8( KRemoveProgressInfo,
    "DELETE FROM ProgressInfo"
    );
    
_LIT8( KRemoveDrivesInfo,
    "DELETE FROM DrivesInfo"
    );    

_LIT8( KRemoveSyncTime,
    "DELETE FROM SyncTime"
    );

_LIT8( KRemoveDeletedFillFiles, 
    "DELETE FROM FillFiles "
    "WHERE Selected = '2'"
    );

_LIT8( KRemoveDeletedMediaServers, 
    "DELETE FROM MediaServers "
    "WHERE FillUsage = '0' AND StoreUsage = '0'"
    );

_LIT8( KRemoveHash,
    "DELETE FROM HashValues "
    "WHERE ( ( MediaServerID = ? ) AND ( StartIndex >= ? ) )"
    );

_LIT8( KSetNullRealSizes,
    "UPDATE FillRuleGroup SET RealCount = '0', RealSize = '0' "
    "WHERE RuleGroupID NOT IN "
    "( SELECT FillListId FROM FillFiles ) "
    );
    
// ---------------------------------------------------------------------------
// TRANSACTION HANDLING
// ---------------------------------------------------------------------------
//
_LIT8(KCmDbBeginTransaction, "BEGIN");
_LIT8(KCmDbSqlCommit, "COMMIT");
                                    
#endif // __CMDMSQLCLAUSEDEF_H__



