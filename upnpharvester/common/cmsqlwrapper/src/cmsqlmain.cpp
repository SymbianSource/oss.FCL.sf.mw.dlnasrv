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
* Description:      Main class in the SQL Wrapper component
*
*/






#include <bautils.h>
#include "cmsqlconnection.h"
#include "cmsqldbmaintenance.h"
#include "cmsqldbclause.h"
#include "cmsqlmain.h"
#include "msdebug.h"

// CONSTANTS
_LIT(KCmSqlDirC, "C:\\System\\Data\\MediaServant\\");
 
// ---------------------------------------------------------------------------
// CCmSqlMain::NewL
// ---------------------------------------------------------------------------
//
CCmSqlMain* CCmSqlMain::NewL( RFs& aFs )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::NewL"));    
    CCmSqlMain* self = CCmSqlMain::NewLC( aFs );
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmSqlMain::NewLC
// ---------------------------------------------------------------------------
//    
CCmSqlMain* CCmSqlMain::NewLC( RFs& aFs )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::NewLC"));
    CCmSqlMain* self = new ( ELeave ) CCmSqlMain( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSqlMain::~CCmSqlMain
// ---------------------------------------------------------------------------
//
CCmSqlMain::~CCmSqlMain()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::~CCmSqlMain"));
    delete iClause;
    delete iMaintenance;
    delete iConnection;   
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::CancelAsyncOperation
// ---------------------------------------------------------------------------
//
void CCmSqlMain::CancelAsyncOperation()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::CancelAsyncOperation"));
    
    iConnection->CancelAsyncOperation();
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::SetMsId
// ---------------------------------------------------------------------------
//
void CCmSqlMain::SetMsId( TUint aId )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::SetMsId"));
    
    iConnection->SetMsId( aId );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::SetQuota
// ---------------------------------------------------------------------------
//
void CCmSqlMain::SetQuota( TInt64 aQuota )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::SetQuota"));
    
    iConnection->SetQuota( aQuota );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::SetDlnaProfileFilters
// ---------------------------------------------------------------------------
//
void CCmSqlMain::SetDlnaProfileFilters( 
    const RArray<TInt64>& aProfileIds )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::SetDlnaProfileFilters"));
    
    iClause->SetDlnaProfileFilters( aProfileIds );
    }
            
// ---------------------------------------------------------------------------
// CCmSqlMain::AsyncBatchAdd
// ---------------------------------------------------------------------------
//
void CCmSqlMain::AsyncBatchAdd( 
    RPointerArray<CCmSqlGenericItem>& aItems, TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::AsyncBatchAdd"));
    
    iConnection->AsyncBatchAdd( aItems, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::AsyncBatchAddPropertyItems
// ---------------------------------------------------------------------------
//
void CCmSqlMain::AsyncBatchAddPropertyItems( 
    RPointerArray<CCmSqlPropertyItem>& aItems, TCmMetadataField aField,
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::AsyncBatchAddPropertyItems"));
    
    iConnection->AsyncBatchAddPropertyItems( aItems, aField, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::SyncAddPropertyItemL
// ---------------------------------------------------------------------------
//
void CCmSqlMain::SyncAddPropertyItemL( 
    CCmSqlPropertyItem& aItem, TCmMetadataField aField )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::SyncAddPropertyItemL"));
    
    iConnection->SyncAddPropertyItemL( aItem, aField );
    }
        
// ---------------------------------------------------------------------------
// CCmSqlMain::AsyncBatchDelete
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::AsyncBatchDelete( 
    RPointerArray<CCmSqlBaseItem>& aItems, 
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::AsyncBatchDelete"));
    
    iConnection->AsyncBatchDelete( aItems, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::AsyncMetadataDelete
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::AsyncMetadataDelete( 
    RArray<TInt>& aMsIds, TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::AsyncMetadataDelete"));
    
    iConnection->AsyncMetadataDelete( aMsIds, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::DeleteOldestMediaItemsL
// ---------------------------------------------------------------------------
//
void CCmSqlMain::DeleteOldestMediaItemsL( TCmMediaType aType, 
    TInt64 aCount )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::DeleteOldestMediaItemsL"));
    
    iConnection->DeleteOldestMediaItemsL( aType, aCount );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::DeleteUnusedPropertys
// ---------------------------------------------------------------------------
//
void CCmSqlMain::DeleteUnusedPropertys( )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::DeleteUnusedPropertys"));
    
    iConnection->DeleteUnusedPropertys( );
    }
            
// ---------------------------------------------------------------------------
// CCmSqlMain::GetItemsL
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::GetItemsL( RPointerArray<CCmSqlBaseItem>& aItems,
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::GetItemsL"));
    
    iConnection->GetItemsL( aItems, aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::GetFillItemsL
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::GetFillItemsL( 
    RPointerArray<CCmFillListItem>& aItems, CCmFillRule& aRule, 
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::GetFillItemsL"));
    
    iClause->FormatClauseL( aRule );
    iConnection->GetFillItemsL( aItems, iClause->ClauseL(), aRule, aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlMain::GetPropertyValuesL
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::GetPropertyValuesL( 
    RPointerArray<CCmSqlPropertyItem>& aItems, TRequestStatus& aStatus,
    TCmMetadataField aField, TCmMediaType aMedia )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::GetPropertyValuesL"));
    
    CCmFillRule* rule = NULL;
    iClause->FormatClauseL( *rule, aField, aMedia );
    iConnection->GetPropertyValuesL( aItems, iClause->ClauseL(), 
                                      aStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::GetFilteredPropertyValuesL
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::GetFilteredPropertyValuesL( 
    CCmSqlPropertyCollector& aPropertys, TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::GetFilteredPropertyValuesL"));
    
    iClause->FormatFilteringClauseL( aPropertys );
    iConnection->GetFilteredPropertyValuesL( aPropertys, iClause->ClauseL(), 
                                      aStatus );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlMain::GetMediaCountL
// ---------------------------------------------------------------------------
//
void CCmSqlMain::GetMediaCountL( TInt64& aCount, 
    TCmMediaType aType )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::GetMediaCountL"));
    
    iConnection->GetMediaCountL( aCount, aType );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::Exists
// ---------------------------------------------------------------------------
//   
TBool CCmSqlMain::ExistsL( const CCmBaseListItem& aItem, 
    const TInt aDevId )    
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::ExistsL"));
    
    return iConnection->ExistsL( aItem, aDevId );
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::Close
// ---------------------------------------------------------------------------
//
void CCmSqlMain::Close()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::Close"));
    
    delete this;
	}
    
// ---------------------------------------------------------------------------
// CCmSqlMain::CCmSqlMain
// ---------------------------------------------------------------------------
//    
CCmSqlMain::CCmSqlMain( RFs& aFs ): iFs( aFs )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::CCmSqlMain"));
    }

// ---------------------------------------------------------------------------
// CCmSqlMain::ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlMain::ConstructL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlMain::ConstructL"));
    
    TRAP_IGNORE( BaflUtils::EnsurePathExistsL( iFs, KCmSqlDirC ) );   
    iClause = CCmSqlDbClause::NewL();
    iMaintenance = CCmSqlDbMaintenance::NewL();
    iConnection = CCmSqlConnection::NewL();
    
    if( iMaintenance->InitConnection(*iConnection) )
        {
        iMaintenance->CreateDbFile( *iConnection );
        }
    if( !iConnection->Validate() )
        {
        iMaintenance->CreateDb( *iConnection );
        iMaintenance->CreateIndexes( *iConnection );        
        }        
    }    

// End of file

