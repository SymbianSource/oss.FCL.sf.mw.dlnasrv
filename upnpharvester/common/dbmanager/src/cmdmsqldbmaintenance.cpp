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






#include "cmdmsqliteconnection.h"
#include "cmdmsqlmaintenanceclauses.h"
#include "cmdmsqldbmaintenance.h"
#include "cmdmsqlclausedef.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::NewL
// ---------------------------------------------------------------------------
//
CCmDmSqlDbMaintenance* CCmDmSqlDbMaintenance::NewL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::NewL()"));
     
    CCmDmSqlDbMaintenance* self = CCmDmSqlDbMaintenance::NewLC();
    CleanupStack::Pop( self );

    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::NewLC
// ---------------------------------------------------------------------------
//    
CCmDmSqlDbMaintenance* CCmDmSqlDbMaintenance::NewLC()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::NewLC()"));
        
    CCmDmSqlDbMaintenance* self = new ( ELeave ) CCmDmSqlDbMaintenance();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::~CCmDmSqlDbMaintenance
// ---------------------------------------------------------------------------
//
CCmDmSqlDbMaintenance::~CCmDmSqlDbMaintenance()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::~CCmDmSqlDbMaintenance()"));
    }

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::InitConnection
// ---------------------------------------------------------------------------
//
TInt CCmDmSqlDbMaintenance::InitConnection(
    CCmDmSQLiteConnection& aConnection )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::InitConnection()"));
    return aConnection.OpenDb( KCmDatabase );        
    }    

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::CloseConnection
// ---------------------------------------------------------------------------
//
void CCmDmSqlDbMaintenance::CloseConnection(
    CCmDmSQLiteConnection& aConnection )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::CloseConnection()"));
    aConnection.CloseDb();        
    }

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::CreateDbFile
// ---------------------------------------------------------------------------
//
TInt CCmDmSqlDbMaintenance::CreateDbFile( 
    CCmDmSQLiteConnection& aConnection )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::CreateDbFile()"));
    return aConnection.CreateDbFile( KCmDatabase );
    }
    
// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::CreateDb
// ---------------------------------------------------------------------------
//
void CCmDmSqlDbMaintenance::CreateDb( 
    CCmDmSQLiteConnection& aConnection )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::CreateDb()"));
    
    /// DROP tables
    TRAP_IGNORE( aConnection.ExecuteL( KDropFillRuleGroup ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropRules ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropRuleParams ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropRuleGroupServers ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropMediaServers ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropHashValues ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStored ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropFilled ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropIDs ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreRules ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreRuleParams ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreRuleServers ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropFillFiles ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreFiles ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreFileServers ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropImageShrinkingInfo ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropTransferInfo ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropSyncTime ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropProgressInfo ));
    TRAP_IGNORE( aConnection.ExecuteL( KDropDrivesInfo ));
     
    // Create tables
    TRAP_IGNORE( aConnection.ExecuteL( KCreateFillRuleGroup ) );    
    TRAP_IGNORE( aConnection.ExecuteL( KCreateRules ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateRuleParams ) );   
    TRAP_IGNORE( aConnection.ExecuteL( KCreateRuleGroupServers ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateMediaServers ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateHashValues ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStored ) );   
    TRAP_IGNORE( aConnection.ExecuteL( KCreateIDs ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreRules ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreRuleParams ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreRuleServers ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateFillFiles ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreFiles ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreFileServers ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateImageShrinkingInfo ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateTransferInfo ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateSyncTime ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateProgressInfo ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateDrivesInfo ));
    
    // Init clauses
    TRAP_IGNORE( aConnection.ExecuteL( KInitShrinkingTime ) );       
    }

// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::CreateIndexes
// ---------------------------------------------------------------------------
//
void CCmDmSqlDbMaintenance::CreateIndexes( 
    CCmDmSQLiteConnection& aConnection )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::CreateIndexes()"));
    
    // Drop indexes
    TRAP_IGNORE( aConnection.ExecuteL( KDropFillListIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KDropFillItemRefIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KDropStoreStatusIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KDropFillStatusIndex ) );
    
    // Create indexes
    TRAP_IGNORE( aConnection.ExecuteL( KCreateFillListIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateFillItemRefIndex ) );        
    TRAP_IGNORE( aConnection.ExecuteL( KCreateStoreStatusIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCreateFillStatusIndex ) );
    }    
    
// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::CCmDmSqlDbMaintenance
// ---------------------------------------------------------------------------
//    
CCmDmSqlDbMaintenance::CCmDmSqlDbMaintenance()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::CCmDmSqlDbMaintenance()"));
    }
 
// ---------------------------------------------------------------------------
// CCmDmSqlDbMaintenance::ConstructL
// ---------------------------------------------------------------------------
//   
void CCmDmSqlDbMaintenance::ConstructL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSqlDbMaintenance::ConstructL()"));
    }    

// End of file

