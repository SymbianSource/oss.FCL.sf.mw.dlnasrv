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






#include "cmsqldbmaintenance.h"
#include "cmsqlconnection.h"
#include "cmsqlmaintenanceclauses.h"
#include "msdebug.h" 

// Constants
_LIT(KCmSqlDatabase, "C:\\System\\Data\\MediaServant\\cmmetadatacache.sq");

// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::NewL
// ---------------------------------------------------------------------------
//
CCmSqlDbMaintenance* CCmSqlDbMaintenance::NewL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::NewL"));
        
    CCmSqlDbMaintenance* self = CCmSqlDbMaintenance::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::NewLC
// ---------------------------------------------------------------------------
//    
CCmSqlDbMaintenance* CCmSqlDbMaintenance::NewLC()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::NewLC"));   
    
    CCmSqlDbMaintenance* self = new ( ELeave ) CCmSqlDbMaintenance();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::~CCmSqlDbMaintenance
// ---------------------------------------------------------------------------
//
CCmSqlDbMaintenance::~CCmSqlDbMaintenance()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::~CCmSqlDbMaintenance"));   
    }
    
// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::InitConnection
// ---------------------------------------------------------------------------
//
TInt CCmSqlDbMaintenance::InitConnection(
    CCmSqlConnection& aConnection )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::InitConnection"));   
    
    return aConnection.OpenDb( KCmSqlDatabase );   
    }    

// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::CloseConnection
// ---------------------------------------------------------------------------
//
void CCmSqlDbMaintenance::CloseConnection(
    CCmSqlConnection& aConnection )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::CloseConnection"));   
    
    aConnection.CloseDb();        
    }

// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::CreateDbFile
// ---------------------------------------------------------------------------
//
TInt CCmSqlDbMaintenance::CreateDbFile( CCmSqlConnection& aConnection )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::CreateDbFile"));   
    
    return aConnection.CreateDbFile( KCmSqlDatabase );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::CreateDb
// ---------------------------------------------------------------------------
//
void CCmSqlDbMaintenance::CreateDb( 
    CCmSqlConnection& aConnection )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::CreateDb"));
    LOG(_L("[SQL wrapper]\t Dropping tables..."));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropArtists ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropGenres ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropAlbums ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropResolutions ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropUpnpProfiles ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropUpnpClasses ));  
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropResources ));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItems ));
           
    LOG(_L("[SQL wrapper]\t Creating tables..."));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateArtists ) );    
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateGenres ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateAlbums ) );      
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateResolutions ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateUpnpProfiles ) ); 
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateUpnpclasses ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateResources ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItems ) );     
    }

// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::CreateIndexes
// ---------------------------------------------------------------------------
//
void CCmSqlDbMaintenance::CreateIndexes( 
    CCmSqlConnection& aConnection )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::CreateIndexes"));
    
    LOG(_L("[SQL wrapper]\t Dropping indexes..."));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemsIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemTitleIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemArtistIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemAlbumIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemGenreIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemUpnpclassIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemResolutionIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemDateIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemHarvestDateIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemUpnpProfileIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropItemMediatypeIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropAlbumIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropArtistIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlDropGenreIdIndex ) );
    
    LOG(_L("[SQL wrapper]\t Creating indexes..."));
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemsIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemTitleIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemArtistIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemAlbumIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemGenreIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemUpnpclassIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemResolutionIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemDateIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemHarvestDateIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemUpnpProfileIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateItemMediatypeIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateAlbumIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateArtistIdIndex ) );
    TRAP_IGNORE( aConnection.ExecuteL( KCmSqlCreateGenreIdIndex ) );
    }
                
// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::CCmSqlDbMaintenance
// ---------------------------------------------------------------------------
//    
CCmSqlDbMaintenance::CCmSqlDbMaintenance()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::CCmSqlDbMaintenance"));
    }
 
// ---------------------------------------------------------------------------
// CCmSqlDbMaintenance::ConstructL
// ---------------------------------------------------------------------------
//   
void CCmSqlDbMaintenance::ConstructL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbMaintenance::ConstructL"));
    }    

// End of file

