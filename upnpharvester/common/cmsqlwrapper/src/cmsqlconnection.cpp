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
* Description:      SQLite Connection class
*
*/






#include <utf.h>
#include "cmsqlclausedef.h"
#include "cmsqlresolutionpropertyitem.h"
#include "cmsqlaudioitem.h"
#include "cmsqlvideoitem.h"
#include "cmsqlimageitem.h"
#include "cmfilllistitem.h"
#include "cmfillrule.h"
#include "cmsqlpropertycollector.h"
#include "cmsqlpropertycontainer.h"
#include "cmsqlconnection.h"
#include "msdebug.h"

_LIT8( KCmSqlPragmaCacheSize, 
    "PRAGMA cache_size=1024;PRAGMA page_size=4096;"
    );                   

#ifdef _DEBUG
_LIT( KCmSqlWrapper, "SqlWrapper");
#endif // _DEBUG
                            

// ---------------------------------------------------------------------------
// CCmSqlConnection::NewL
// ---------------------------------------------------------------------------
//
CCmSqlConnection* CCmSqlConnection::NewL()
    {
    TRACE(Print(_L("[SQL wrapper]\t CCmSqlConnection::NewL()")));
    CCmSqlConnection* self = CCmSqlConnection::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmSqlConnection::NewLC
// ---------------------------------------------------------------------------
//    
CCmSqlConnection* CCmSqlConnection::NewLC()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::NewLC"));
    CCmSqlConnection* self = new ( ELeave ) CCmSqlConnection();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    
        
// ---------------------------------------------------------------------------
// CCmSqlConnection::~CCmSqlConnection
// ---------------------------------------------------------------------------
//
CCmSqlConnection::~CCmSqlConnection()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::~CCmSqlConnection"));
    CloseDb();
    if( iAsyncDbHandler )
        {
        iAsyncDbHandler->Cancel();
        delete iAsyncDbHandler;       
        }
        
    delete iCountQuery;
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::OpenDb
// ---------------------------------------------------------------------------
//
TInt CCmSqlConnection::OpenDb( const TDesC& aDb )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::OpenDb"));
    
    TInt err( iDatabase.Open( aDb, NULL ) );
    if( err == KErrNone )
        {
        err = iDatabase.Exec( KCmSqlPragmaCacheSize );        
        if ( err != KErrNone )
            {
            TRACE(Print(_L("[SQL wrapper]\t KCmSqlPragmaCacheSize\
             error %d"), err));        
            }        
        }
    return err;    
    }    

// ---------------------------------------------------------------------------
// CCmSqlConnection::CloseDb
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::CloseDb()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::CloseDb"));
    
    iDatabase.Close();
    }


// ---------------------------------------------------------------------------
// CCmSqlConnection::CreateDbFile
// ---------------------------------------------------------------------------
//
TInt CCmSqlConnection::CreateDbFile( const TDesC& aDb )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::CreateDbFile"));
    
    return iDatabase.Create( aDb );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::Validate
// ---------------------------------------------------------------------------
//
TBool CCmSqlConnection::Validate()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::Validate"));
    
    TBool retval = EFalse;

    // In the test period it is useful that the database is re-created
    // every run. because most of the test cases are automatic and require
    // a known initial state.
    TInt ret( KErrNone );
    TInt err = iStatement.Prepare( iDatabase, 
        KCmSqlValidateTableExistence );
    // Loop only one row in results
    if( err )
        {
        ret = KErrGeneral;
        }
    else
        {
        err = iStatement.Next();    
        if ( err != KSqlAtRow )  
            {
            ret = KErrGeneral;       
            }
        iStatement.Reset();
        iStatement.Close();          
        }    
    

    if ( ret == KErrNone ) 
        {
        retval = ETrue;
        }

    return retval;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::ExecuteL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::ExecuteL( const TDesC8& aCommand )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::ExecuteL"));
    
    TInt err( iDatabase.Exec( aCommand ) );
    if ( KErrNone > err  )
        {
#ifdef _DEBUG        
        TPtrC errorMsg( iDatabase.LastErrorMessage() );
        TRACE(Print(_L("[SQL wrapper]\t errorMsg: %S"), &errorMsg ));
#endif        
        User::Leave( err );    
        }    
    }
             
// ---------------------------------------------------------------------------
// CCmSqlConnection::CancelAsyncOperation
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::CancelAsyncOperation()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::CancelAsyncOperation"));
    
    if( ECmSqlBaseItemGet == iMode || ECmSqlFillItemGet == iMode ||
        ECmSqlPropertyValueGet == iMode )
        {
        iStatement.Reset();
        iStatement.Close();        
        }
    
    iMode = ECmSqlIdle;        
    
    if ( iAsyncDbHandler->IsActive() )
        {
        User::RequestComplete( iStatus, KErrCancel );
        iAsyncDbHandler->Cancel();
        TRACE(Print(_L("[SQL wrapper]\t Canceled...")));
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::SetMsId
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::SetMsId( TUint aId )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::SetMsId"));
    
    iMsId = aId;
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::SetQuota
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::SetQuota( TInt64 aQuota )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::SetQuota"));
    
    iQuota = aQuota;
    }
        
// ---------------------------------------------------------------------------
// CCmSqlConnection::AsyncBatchAdd
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AsyncBatchAdd( 
    RPointerArray<CCmSqlGenericItem>& aItems, TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AsyncBatchAdd"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;        
    
    if( !iAsyncDbHandler->IsActive() )
        {    
        iIndex = 0;
        iGenericItems = &aItems;
        iMode = ECmSqlGenericItemAdd;  
        iAsyncDbHandler->Start( TCallBack( BatchAdd, this ) );
        }
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        }            
    }

// ---------------------------------------------------------------------------
// ACCmSqlConnection::syncBatchAddPropertyItems 
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AsyncBatchAddPropertyItems( 
    RPointerArray<CCmSqlPropertyItem>& aItems, TCmMetadataField aField,
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AsyncBatchAddPropertyItems"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;        
    
    if( !iAsyncDbHandler->IsActive() )
        {    
        iIndex = 0;
        iField = aField;
        iPropertyItems = &aItems;
        iMode = ECmSqlPropertyItemAdd;
        iAsyncDbHandler->Start( TCallBack( BatchAdd, this ) );
        }
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        } 
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::SyncAddPropertyItemL 
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::SyncAddPropertyItemL( 
    CCmSqlPropertyItem& aItem, TCmMetadataField aField )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::SyncAddPropertyItemL"));
    
    iIndex = 0;
    iField = aField;
    RPointerArray<CCmSqlPropertyItem> temp;
    CleanupClosePushL( temp );
    temp.AppendL( &aItem );
    iPropertyItems = &temp;
    AddPropertyItemL();
    FormatRowCountQueryL( aField );
    // Autoincremented row index is used as a item id
    aItem.SetId( RowCountL() );
    delete iCountQuery;
    iCountQuery = NULL;
    temp.Reset();
    CleanupStack::PopAndDestroy( &temp );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::AsyncBatchDelete
// ---------------------------------------------------------------------------
//    
void CCmSqlConnection::AsyncBatchDelete( 
    RPointerArray<CCmSqlBaseItem>& aItems, 
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AsyncBatchDelete"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;        
     
    if( !iAsyncDbHandler->IsActive() )
        {
        iBaseItems = &aItems;
        iIndex = 0;    
        iAsyncDbHandler->Start( TCallBack( BatchDelete, this ) );
        }        
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        } 
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::AsyncMetadataDelete
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AsyncMetadataDelete( RArray<TInt>& aMsIds, 
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AsyncMetadataDelete"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;         
    
    if( !iAsyncDbHandler->IsActive() )
        {
        iMsIds = &aMsIds;
        iIndex = 0;
        iMode = ECmSqlDeletingMetadata;    
        iAsyncDbHandler->Start( TCallBack( BatchDelete, this ) );
        }        
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        } 
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DeleteOldestMediaItemsL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::DeleteOldestMediaItemsL( TCmMediaType aType, 
    TInt64 aCount )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DeleteOldestMediaItemsL"));
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlSelectLimitHarvestDate );
    TInt64 timestamp( KErrNone );
    User::LeaveIfError( err );
    iStatement.BindInt( 0, aType );
    iStatement.BindInt64( 1, aCount );
    
    while( KSqlAtRow == iStatement.Next() )
        {
        timestamp = iStatement.ColumnInt64( 0 );
        }
    
    iStatement.Reset();
    iStatement.Close();    
    
    if( timestamp )
        {
        err = iStatement.Prepare( iDatabase, KCmSqlDeleteOldestMediaItems );
        User::LeaveIfError( err );
        iStatement.BindInt64( 0, timestamp );
        iStatement.BindInt( 1, aType );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();  
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DeleteUnusedPropertys()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::DeleteUnusedPropertys( )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DeleteUnusedPropertys"));
    
    iDatabase.Exec( KCmSqlDeleteUnusedAlbums );
    iDatabase.Exec( KCmSqlDeleteUnusedArtists );
    iDatabase.Exec( KCmSqlDeleteUnusedGenres );
    iDatabase.Exec( KCmSqlDeleteUnusedUpnpclasses );
    iDatabase.Exec( KCmSqlDeleteUnusedUpnpProfileIds );  
    }
                
// ---------------------------------------------------------------------------
// CCmSqlConnection::GetItemsL
// ---------------------------------------------------------------------------
// 
void CCmSqlConnection::GetItemsL( RPointerArray<CCmSqlBaseItem>& aItems,
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::GetItemsL"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;         
    
    if( !iAsyncDbHandler->IsActive() )
        {    
        iBaseItems = &aItems;
        iMode = ECmSqlBaseItemGet;
        iIndex = 0;
        TInt err = iStatement.Prepare( iDatabase, 
            KCmSqlSelectGenericItem );
        User::LeaveIfError( err );
        iStatement.BindInt( 0, iMsId );   
        iAsyncDbHandler->Start( TCallBack( BatchGetL, this ) );
        }        
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        } 
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::GetFillItemsL
// ---------------------------------------------------------------------------
// 
void CCmSqlConnection::GetFillItemsL( RPointerArray<CCmFillListItem>& aItems,
    TDesC8& aClause, CCmFillRule& aRule, TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::GetFillItemsL"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));  
    iStatus = &aStatus;    
    
    if( !iAsyncDbHandler->IsActive() )
        {    
        iList = &aRule;
        iIndex = 0;
        iListSize = 0;
        iListMaxSize = 0;
        iFillItems = &aItems;
        iMode = ECmSqlFillItemGet;
        if( EMbits == iList->LimitType() )
            {
            // Converting list max size to bytes
            iListMaxSize = ( iList->Amount() * 1000000 );
            }
        // Unlimited lists are limited to mmc quota    
        else if( EUnlimited == iList->LimitType() )
            {
            iListMaxSize = iQuota;
            }
            
        TInt err = iStatement.Prepare( iDatabase, aClause );
        User::LeaveIfError( err );  
        iAsyncDbHandler->Start( TCallBack( BatchGetL, this ) );
        }
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        } 
    }
                
// ---------------------------------------------------------------------------
// CCmSqlConnection::GetPropertyValuesL
// ---------------------------------------------------------------------------
//    
void CCmSqlConnection::GetPropertyValuesL( 
    RPointerArray<CCmSqlPropertyItem>& aItems, TDesC8& aClause,
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::GetPropertyValuesL"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;        
    
    if( !iAsyncDbHandler->IsActive() )
        {
        iPropertyItems = &aItems;
        iIndex = 0;
        iMode = ECmSqlPropertyValueGet;
        TInt err = iStatement.Prepare( iDatabase, aClause );
        User::LeaveIfError( err );
        iAsyncDbHandler->Start( TCallBack( BatchGetL, this ) );        
        }        
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        }  
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::GetFilteredPropertyValuesL
// ---------------------------------------------------------------------------
//    
void CCmSqlConnection::GetFilteredPropertyValuesL( 
    CCmSqlPropertyCollector& aPropertys, const TDesC8& aClause, 
    TRequestStatus& aStatus )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::GetFilteredPropertyValuesL"));
    
    __ASSERT_DEBUG( iAsyncDbHandler,\
        User::Panic( KCmSqlWrapper, KErrCorrupt ));
    iStatus = &aStatus;        
    
    if( !iAsyncDbHandler->IsActive() )
        {
        iPropertyCollector = &aPropertys;
        iIndex = 0;
        iMode = ECmSqlFilteredPropertyGet;
        TInt err = iStatement.Prepare( iDatabase, aClause );
        User::LeaveIfError( err );
        iAsyncDbHandler->Start( TCallBack( BatchGetL, this ) );  
        }        
    else
        {
        User::RequestComplete( iStatus, KErrNotReady );
        }     
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::GetMediaCountL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::GetMediaCountL( TInt64& aCount, 
    TCmMediaType aType )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::GetMediaCountL"));
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlSelectMediaCount );
    User::LeaveIfError( err );
    err = iStatement.BindInt( 0, aType );
    
    if( !err )
        {
        if( KSqlAtRow == iStatement.Next() )
            {
            aCount = iStatement.ColumnInt( 0 );
            }
        iStatement.Reset();
        iStatement.Close();
        }        
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::ExistsL
// ---------------------------------------------------------------------------
//   
TBool CCmSqlConnection::ExistsL( const CCmBaseListItem& aItem, 
    const TInt aDevId )    
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::ExistsL"));
    
    TBool exists( EFalse );
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlSelectItemId );
    User::LeaveIfError( err );
    
    // convert lowercase
    TBuf<KMaxFileName> temp;
    temp.Copy(aItem.PrimaryText());
    temp.LowerCase();
    
    iStatement.BindText( 0, temp );
    iStatement.BindInt( 1, aItem.Size() );
    iStatement.BindInt( 2, aDevId );     
        
    if( KSqlAtRow == iStatement.Next() )
        {
        exists = ETrue;
        }
    
    iStatement.Reset();
    iStatement.Close();
    return exists;            
    }
        
// ---------------------------------------------------------------------------
// CCmSqlConnection::BatchAdd
// ---------------------------------------------------------------------------
//  
TInt CCmSqlConnection::BatchAdd( TAny* aDbHandler )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::BatchAdd"));
    
    return ((CCmSqlConnection*)aDbHandler)->DoBatchAdd();
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DoBatchAdd
// ---------------------------------------------------------------------------
//
TInt CCmSqlConnection::DoBatchAdd()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DoBatchAdd"));
    
    TInt ret( EFalse );
    // Add items in chucks of X items ( x must be changeable )
    // return true if not all added
    TInt err( iDatabase.Exec( KCmSqlBeginTransaction ) ); 
    TInt added( 0 );
    switch( iMode )
        {
        case ECmSqlPropertyItemAdd:
            {
            if( iPropertyItems )
                {
                LOG(_L("[SQL Wrapper]\t Adding property item batch"));
                // Add batch size pcs. items at a time
                for( TInt i = iIndex; i < iPropertyItems->Count() && 
                    ( ( added + 1 ) % KCmSqlBatchSize != 0 ) ; i++ )
                    {
                    if( ( *iPropertyItems )[i] )
                        {                            
                        TRAP_IGNORE( AddPropertyItemL() );
                        }
                    iIndex++;    
                    ret = ETrue;
                    added++;
                    }                               
                } 
            break;
            }        
        case ECmSqlGenericItemAdd:
            {
            if( iGenericItems )
                {
                LOG(_L("[SQL Wrapper]\t Adding generic item batch"));
                // Add batch size pcs. items at a time
                for( TInt i = iIndex; i < iGenericItems->Count()  && 
                    ( ( added + 1 ) % KCmSqlBatchSize != 0 ) ; i++ )
                    {
                    if( (*iGenericItems)[i] )
                        {
                        TRAP_IGNORE( AddItemL() );
                        TRAP_IGNORE( AddResourceL() );
                        }
                    iIndex++;     
                    ret = ETrue;
                    added++;
                    }                                
                }
            break;
            }
        default:
            {
            break;
            }    
        }
    err = iDatabase.Exec( KCmSqlCommit );         
    if( !ret )
        {
        LOG(_L("[SQL Wrapper]\t Completing Async batch add..."));
        User::RequestComplete( iStatus, KErrNone );   
        }            
    return ret;
    }
 
// ---------------------------------------------------------------------------
// CCmSqlConnection::BatchDelete
// ---------------------------------------------------------------------------
//  
TInt CCmSqlConnection::BatchDelete( TAny* aDbHandler )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::BatchDelete"));
    
    return ((CCmSqlConnection*)aDbHandler)->DoBatchDelete();
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DoBatchDelete
// ---------------------------------------------------------------------------
//
TInt CCmSqlConnection::DoBatchDelete()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DoBatchDelete"));
    
    TInt ret( EFalse );
    // Delete items in chucks of KCmSqlBatchSize items
    // return true if not all deleted
    if( iMode == ECmSqlDeletingMetadata )
        {
        if( iIndex < iMsIds->Count() )
            {
            TRAP_IGNORE( 
                DeleteMetadataFromDefServerL( (*iMsIds)[iIndex] ) );
            iIndex++;
            ret = ETrue;            
            }
        }
    else
        {
        TInt err( iDatabase.Exec( KCmSqlBeginTransaction ) );
        TInt count( 0 );
        for( TInt i = iIndex; i < iBaseItems->Count() && 
            ((count + 1 ) % KCmSqlBatchSize != 0 ) ; i++ )
            {
            TRAP_IGNORE( DeleteItemL() );
            TRAP_IGNORE( DeleteResourceL() ); 
            count++;
            iIndex++;
            ret = ETrue;     
            }    
        err = iDatabase.Exec( KCmSqlCommit );        
        }
    if( !ret )
        {
        User::RequestComplete( iStatus, KErrNone );
        }                
    return ret;
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::BatchGetL
// ---------------------------------------------------------------------------
//  
TInt CCmSqlConnection::BatchGetL( TAny* aDbHandler )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::BatchGetL"));
    
    return ((CCmSqlConnection*)aDbHandler)->DoBatchGetL();
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DoBatchGetL
// ---------------------------------------------------------------------------
//
TInt CCmSqlConnection::DoBatchGetL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DoBatchGetL() start"));
    TInt ret( EFalse ); 
    
    // Get items in chucks of KCmSqlBatchSize
    TInt stat( KSqlAtRow ); 
    for( TInt i = 0; i < KCmSqlBatchSize && 
        stat == KSqlAtRow ; i++ )
        {
        stat = iStatement.Next();
        if( stat == KSqlAtRow )
            {
            CollectItemDataL();
            ret = ETrue;
            if( iMode == ECmSqlFillItemGet && 
                iListSize > iListMaxSize && 
                iListMaxSize )
                {
                stat = KSqlAtEnd;
                }
            } 
        }
                
    // If no items found or quota is full    
    if( stat == KSqlAtEnd || !ret )
        {
        ret = EFalse;
        iStatement.Reset();
        iStatement.Close();
        iMode = ECmSqlIdle;
        User::RequestComplete( iStatus, KErrNone );
        }        
    return ret;
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::CollectItemDataL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::CollectItemDataL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::CollectItemDataL"));
    
    switch( iMode )
        {
        case ECmSqlFillItemGet:
            {
            CCmFillListItem* item = CCmFillListItem::NewLC(
                iList->Priority(), iList->ListId(), iList->Status(),
                iList->Selected() );
            item->SetMediaType( iList->MediaType() );    
            item->SetPrimaryTextL( iStatement.ColumnTextL( 0 ) );
            item->SetSecondaryTextL( iStatement.ColumnTextL( 1 ) );
            TPtrC8 data;
            item->SetSize(iStatement.ColumnInt( 2 ) );
            iListSize = iListSize + item->Size(); 
            data.Set( iStatement.ColumnBinaryL( 3 ) ); 
            item->SetUriL( data );
            item->SetDevId(iStatement.ColumnInt( 4 ) );   
            data.Set( iStatement.ColumnBinaryL( 5 ) );
            item->SetItemIdL( data );
            item->SetDbId( iStatement.ColumnInt64( 6 ) );             
            if( iListSize > iListMaxSize && iListMaxSize )
                {
                CleanupStack::PopAndDestroy( item );
                }
            else
                {
                iFillItems->AppendL( item ); 
                CleanupStack::Pop( item );
                }
            break;
            }        
        case ECmSqlBaseItemGet:
            {
            CCmSqlBaseItem* item = CCmSqlBaseItem::NewL();
            CleanupStack::PushL( item ); 
            item->SetId( iStatement.ColumnInt64( 0 ) );
            TPtrC8 data = iStatement.ColumnBinaryL( 1 );
            item->SetCdsIdL( data );
            item->SetHashL( iStatement.ColumnTextL( 2 ) );
            item->SetSearchId( iStatement.ColumnInt64( 3 ) );
            iBaseItems->AppendL( item );
            CleanupStack::Pop( item );
            break;
            }        
        case ECmSqlPropertyValueGet:
            {
            CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewL();
            CleanupStack::PushL( item );
            item->SetId( iStatement.ColumnInt64( 0 ) );
            TPtrC data16 = iStatement.ColumnTextL( 1 );
            HBufC8* data8bitBuf = HBufC8::NewLC(
                data16.Length());
            TPtr8 data8bitPtr = data8bitBuf->Des();
            TInt conversionError = 
                CnvUtfConverter::ConvertFromUnicodeToUtf8( data8bitPtr, 
                    data16 );
            if( !conversionError )
                {
                item->SetNameL( data8bitPtr );
                }
            CleanupStack::PopAndDestroy( data8bitBuf );                    
            item->SetStatus( ETrue );
            iPropertyItems->AppendL( item );
            CleanupStack::Pop( item );
            break;
            }
        case ECmSqlFilteredPropertyGet:
            {
            TInt couunt = iPropertyCollector->PropertyContainerCount();
            for( TInt i = 0 ; i < couunt; i++ )
                {
                CCmSqlPropertyContainer* temp = 
                    iPropertyCollector->PropertyContainer( i );
                switch( temp->Type() )
                    {
                    case ECmTitle:
                        {
                        GetFilteredPropertysL( *temp, 0, 1 );
                        break;
                        }                    
                    case ECmArtist:
                        {
                        GetFilteredPropertysL( *temp, 2, 3 );
                        break;
                        }                    
                    case ECmAlbum:
                        {
                        GetFilteredPropertysL( *temp, 4, 5 );
                        break;
                        }                    
                    case ECmGenre:
                        {
                        GetFilteredPropertysL( *temp, 6, 7 );
                        break;
                        }                    
                    default:
                        {
                        break;
                        }
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }                       
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::AddItemL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AddItemL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AddItemL"));
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlInsertItem );
    User::LeaveIfError( err );        
    // Bind item prameters
    if( &(*iGenericItems)[iIndex]->CdsId() ) 
        {
        iStatement.BindBinary( 0, 
            (*iGenericItems)[iIndex]->CdsId() );            
        }
    if( &(*iGenericItems)[iIndex]->Hash() )
        {
        iStatement.BindText( 1, 
            (*iGenericItems)[iIndex]->Hash() );                            
        }
    if( &(*iGenericItems)[iIndex]->Uri()  )
        {
        iStatement.BindBinary( 2, 
            (*iGenericItems)[iIndex]->Uri() );                             
        }
    if( &(*iGenericItems)[iIndex]->Title() )
        {
        HBufC* data16bitBuf = HBufC::NewLC(
            iGenericItems->operator[](iIndex)->Title().Length());
        TPtr data16bitPtr = data16bitBuf->Des();
        TInt conversionError = 
            CnvUtfConverter::ConvertToUnicodeFromUtf8( data16bitPtr, 
                (*iGenericItems)[iIndex]->Title() );
        if( !conversionError )
            {
            iStatement.BindText( 3, data16bitPtr );
            }
        CleanupStack::PopAndDestroy( data16bitBuf );
        }
    iStatement.BindInt( 5, iMsId ); 
    
    iStatement.BindInt64( 8, 
        (*iGenericItems)[iIndex]->Date().Int64() ); 
    iStatement.BindInt64( 9, 
        (*iGenericItems)[iIndex]->HarvestDate().Int64() );
    iStatement.BindInt( 10, 
        (*iGenericItems)[iIndex]->Size() );
    iStatement.BindInt( 11, 
        (*iGenericItems)[iIndex]->MediaType() );
    iStatement.BindInt64( 12, 
        (*iGenericItems)[iIndex]->UpnpclassId() );         
    iStatement.BindInt64( 17, 
        (*iGenericItems)[iIndex]->UpnpProfileId() );
    iStatement.BindInt64( 18, 
        (*iGenericItems)[iIndex]->SearchId() );        
        
    switch( (*iGenericItems)[iIndex]->MediaType() )
        {
        case ECmAudio:
            {
            CCmSqlAudioItem* item = static_cast<CCmSqlAudioItem*>(
                (*iGenericItems)[iIndex] );
            iStatement.BindInt( 6, item->Duration() );
            iStatement.BindInt( 7, item->Bitrate() );                     
            iStatement.BindInt64( 13, item->ArtistId() );
            iStatement.BindInt64( 14, item->AlbumId() );
            iStatement.BindInt64( 15, item->GenreId() );
            if( &item->AlbumArtUri()  )
                {
                iStatement.BindBinary( 4, item->AlbumArtUri() );
                }            
            break;
            }            
        case ECmVideo:
            {
            CCmSqlVideoItem* item = static_cast<CCmSqlVideoItem*>(
                (*iGenericItems)[iIndex] );
            iStatement.BindInt64( 15, item->GenreId() );
            break;
            }            
        case ECmImage:
            {
            CCmSqlImageItem* item = static_cast<CCmSqlImageItem*>(
                (*iGenericItems)[iIndex] );                    
            iStatement.BindInt64( 16, item->ResolutionId() );
            }            
        default:
            {
            break;
            }
        }
    iStatement.Exec();        
    iStatement.Reset();
    iStatement.Close();        
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::AddPropertyItemL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AddPropertyItemL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AddPropertyItemL"));
    
    TInt err( KErrNone );
    switch( iField )
        {
        case ECmArtist:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertArtist );
            break;
            }            
        case ECmAlbum:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertAlbum );
            break;
            }            
        case ECmGenre:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertGenre );
            break;
            }            
        case ECmUpnpClass:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertUpnpclass );
            break;
            }
        case ECmProfileId:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertUpnpProfile );
            break;
            }
        case ECmResolution:
            {
            err = iStatement.Prepare( iDatabase, KCmSqlInsertResolution );
            break;
            }
        default:
            {
            break;
            }
        }
    User::LeaveIfError( err );
    HBufC* data16bitBuf = HBufC::NewLC(
        (*iPropertyItems)[iIndex]->Name().Length());
    TPtr data16bitPtr = data16bitBuf->Des();
    
    TInt conversionError = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8( data16bitPtr, 
            (*iPropertyItems)[iIndex]->Name() );
    
    if( !conversionError )
        {
        iStatement.BindText( 0, data16bitPtr );
        }    
    CleanupStack::PopAndDestroy( data16bitBuf );                  
    
    if( ECmResolution == iField )
        {
        CCmSqlResolutionPropertyItem* resolution =
            static_cast<CCmSqlResolutionPropertyItem*> 
            ( (*iPropertyItems)[iIndex] );
        iStatement.BindInt( 1, resolution->Width() );
        iStatement.BindInt( 2, resolution->Height() );
        iStatement.BindInt( 3, resolution->PixelCount() );
        resolution = NULL;                                                 
        }            
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::AddResourceL
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::AddResourceL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::AddResourceL"));
    
    TPtrC8 uri;
    TInt size( KErrNone );
    TInt duration( KErrNone );
    TInt bitrate( KErrNone );
    TInt64 resolutionId( KErrNone );
    TInt err( KErrNone );    
    for( TInt i = 0; i < 
        (*iGenericItems)[iIndex]->ResourceCount(); 
        i++ )
        {
        FormatRowCountQueryL( ECmTitle );
        // Resources references to it's item
        TInt64 itemId( RowCountL() );        
        err = iStatement.Prepare( iDatabase, KCmSqlInsertResource );
        User::LeaveIfError( err );
        (*iGenericItems)[iIndex]->GetResource( uri, size,
            duration, bitrate, resolutionId, i );
  
         // Bind item prameters
        iStatement.BindInt64( 0, itemId );
        iStatement.BindInt64( 1, resolutionId );
        if( &uri )
            {
            iStatement.BindBinary( 2, uri );
            }
        iStatement.BindInt( 3, size );
        iStatement.BindInt( 4, duration );
        iStatement.BindInt( 5, bitrate );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();            
        }          
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::DeleteItemL()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::DeleteItemL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DeleteItemL"));
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlDeleteItem );
    User::LeaveIfError( err );
    iStatement.BindInt64( 0,  
        (*iBaseItems)[iIndex]->Id() );    
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();     
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DeleteResourceL()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::DeleteResourceL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::DeleteResourceL"));
    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlDeleteResource );
    User::LeaveIfError( err );
    iStatement.BindInt64( 0,  
        (*iBaseItems)[iIndex]->Id() );
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();      
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::DeleteMetadataFromDefServerL()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::DeleteMetadataFromDefServerL( const TInt aMsId )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::\
    DeleteMetadataFromDefServerL"));
    
    TRACE(Print(_L("[SQL wrapper]\t CCmSqlConnection::\
        DeleteMetadataFromDefServerL aMsId = %d"), aMsId ));    
    TInt err = iStatement.Prepare( iDatabase, KCmSqlDeleteMetadata );
    User::LeaveIfError( err );
    iStatement.BindInt( 0, aMsId );    
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();     
    }
        
// ---------------------------------------------------------------------------
// CCmSqlConnection::FormatRowCountQueryL()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::FormatRowCountQueryL( TCmMetadataField aField )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::FormatRowCountQueryL"));

    switch( aField )
        {
        case ECmArtist:
            {
            FormatCountQueryL( KCmSqlArtistId(), KCmSqlArtist() );  
            break;
            }            
        case ECmAlbum:
            {
            FormatCountQueryL( KCmSqlAlbumId(), KCmSqlAlbum() );            
            break;
            }            
        case ECmGenre:
            {
            FormatCountQueryL( KCmSqlGenreId(), KCmSqlGenre() );            
            break;
            }            
        case ECmUpnpClass:
            {
            FormatCountQueryL( KCmSqlUpnpclassId(), KCmSqlUpnpclass() );
            break;
            }
        case ECmProfileId:
            {
            FormatCountQueryL( KCmSqlProfileId(), KCmSqlUpnpProfiles() );
            break;            
            }
        case ECmResolution:
            {
            FormatCountQueryL( KCmSqlResolutionId(), KCmSqlResolutions() );
            break;
            }
        case ECmTitle:
            {
            delete iCountQuery;
            iCountQuery = NULL;
            iCountQuery = HBufC8::NewL( KCmSqlSelectMaxItemIndex().Length() );
            iCountQuery->Des().Append( KCmSqlSelectMaxItemIndex );            
            break;
            }
        default:
            {
            break;
            }
        }    
    }
    
// ---------------------------------------------------------------------------
// CCmSqlConnection::RowCountL()
// ---------------------------------------------------------------------------
//
TInt64 CCmSqlConnection::RowCountL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::RowCountL"));
    
    TInt err = iStatement.Prepare( iDatabase, *iCountQuery );
    TInt64 count( KErrNone );
    User::LeaveIfError( err );
    err = iStatement.Next();
    if ( KSqlAtRow == err )  
        {
        count = iStatement.ColumnInt( 0 );
        }
    iStatement.Reset();
    iStatement.Close();
    return count;          
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::FormatCountQueryL()
// ---------------------------------------------------------------------------
//
void CCmSqlConnection::FormatCountQueryL( const TDesC8& aId, 
    const TDesC8& aTable )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::FormatCountQueryL"));
    
    delete iCountQuery;
    iCountQuery = NULL;    
    iCountQuery = HBufC8::NewL( KCmSqlSelectMaxIndex().Length() + 
       aId.Length() + aTable.Length() + KCmSqlSemicolon().Length() );
    HBufC8* temp = HBufC8::NewLC( KCmSqlSelectMaxIndex().Length() + 
       aTable.Length() + KCmSqlSemicolon().Length() );               
    temp->Des().Append( KCmSqlSelectMaxPropertyIndex );            
    iCountQuery->Des().Format( temp->Des(), &aId, &aTable );
    iCountQuery->Des().Append( KCmSqlSemicolon );
    CleanupStack::PopAndDestroy( temp );    
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::GetFilteredPropertysL()
// ---------------------------------------------------------------------------
//  
void CCmSqlConnection::GetFilteredPropertysL( 
    CCmSqlPropertyContainer& aContainer, const TInt aIndex1, 
    const TInt aIndex2 )
    {
    CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewLC();
    item->SetId( iStatement.ColumnInt64( aIndex1 ) );
    item->SetStatus( ETrue );
    
    TPtrC data16 = iStatement.ColumnTextL( aIndex2 );
    HBufC8* data8bitBuf = HBufC8::NewLC(
            data16.Length() * 3 );
    TPtr8 data8bitPtr = data8bitBuf->Des();
    TInt conversionError =
        CnvUtfConverter::ConvertFromUnicodeToUtf8( data8bitPtr,
            data16 );
    if( !conversionError )
        {
        item->SetNameL( data8bitPtr );
        }
    CleanupStack::PopAndDestroy( data8bitBuf );
    
    if( !aContainer.IsDuplicate( *item ) )
        {
        aContainer.AddPropertyItemL( item );
        CleanupStack::Pop( item );
        }
    else
        {
        CleanupStack::PopAndDestroy( item );
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlConnection::CCmSqlConnection
// ---------------------------------------------------------------------------
//    
CCmSqlConnection::CCmSqlConnection()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::CCmSqlConnection"));
    }
 
// ---------------------------------------------------------------------------
// CCmSqlConnection::ConstructL
// ---------------------------------------------------------------------------
//   
void CCmSqlConnection::ConstructL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlConnection::ConstructL"));
    
    iAsyncDbHandler = CIdle::NewL( CActive::EPriorityStandard );      
    }    

// End of file

