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






#include "cmdmsqliteconnection.h"
#include "cmdmsqlclausedef.h"
#include "cmfillrule.h"
#include "cmfillrulecontainer.h"
#include "cmmediaserverfull.h"
#include "cmstorerule.h"
#include "cmfilllistitem.h"
#include "cmstorelistitem.h"
#include "cmdriveinfo.h"
#include "cmsearchresponsehash.h"
#include "msdebug.h"

// Constants
_LIT8( KCmDbPragmaCacheSize, "PRAGMA cache_size=4096;PRAGMA page_size=4096;");
const TUint8 KAmountOfIDs = 10;

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::NewL
// ---------------------------------------------------------------------------
//
CCmDmSQLiteConnection* CCmDmSQLiteConnection::NewL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::NewL()"));
    
    CCmDmSQLiteConnection* self = CCmDmSQLiteConnection::NewLC();
    CleanupStack::Pop( self );

    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::NewLC
// ---------------------------------------------------------------------------
//    
CCmDmSQLiteConnection* CCmDmSQLiteConnection::NewLC()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::NewLC()"));
    
    CCmDmSQLiteConnection* self = new ( ELeave ) CCmDmSQLiteConnection();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::~CCmDmSQLiteConnection
// ---------------------------------------------------------------------------
//
CCmDmSQLiteConnection::~CCmDmSQLiteConnection()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::~CCmDmSQLiteConnection()"));
    
    TRAP_IGNORE( BackUpIdL( ERuleGroupID, iRuleGroupID ) );
    TRAP_IGNORE( BackUpIdL( ERuleID, iRuleID ) );
    TRAP_IGNORE( BackUpIdL( ERuleParamID, iRuleParamID ) );
    TRAP_IGNORE( BackUpIdL( EMediaServerID, iMediaServerID ) );
    TRAP_IGNORE( BackUpIdL( EStoredID, iStoredID ) );
    TRAP_IGNORE( BackUpIdL( EFilledID, iFilledID ) );
    TRAP_IGNORE( BackUpIdL( EStoreRuleID, iStoreRuleID ) );
    TRAP_IGNORE( BackUpIdL( EStoreRuleParamID, iStoreRuleParamID ) );
    TRAP_IGNORE( BackUpIdL( EFillFileListId, iFillFileId ) );
    TRAP_IGNORE( BackUpIdL( EStoreFileListId, iStoreFileId ) );
    CloseDb();
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::OpenDb
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::OpenDb( const TDesC& aDb )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::OpenDb()"));
    
    TInt err( iDatabase.Open( aDb, NULL ) );
    if ( err != KErrNone )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t CCmDmSQLiteConnection::OpenDb()\
         error %d"), err));
        }
    else 
        {
        err = iDatabase.Exec( KCmDbPragmaCacheSize );        
        
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::CloseDb
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::CloseDb()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::CloseDb()"));
    
    iDatabase.Close();         
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::InitIDs
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::InitIDs()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::InitIDs()"));
    
    TInt err = iStatement.Prepare( iDatabase, KAddIDs );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            CCmDmSQLiteConnection::SetIDs() FAILED ")));
        }
    else
        {
        for( TInt j=1; j <= KAmountOfIDs ; j++ )
            {    
            SetID( static_cast<TTypeOfID>(j) );
            }
        iStatement.Close();        
        }          
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetID
// ---------------------------------------------------------------------------
//    
void CCmDmSQLiteConnection::SetID( TTypeOfID aType )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetID()"));
    
    // Init all ids to one    
    iStatement.BindInt64( 0, 1 );
    iStatement.BindInt( 1, aType );
    iStatement.Exec();
    iStatement.Reset();        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::BackUpIdL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::BackUpIdL( TTypeOfID aType, TUint64 aID )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::BackUpIdL()"));
    
    iStatement.PrepareL( iDatabase, KUpdateID );
    iStatement.BindInt64( 0, aID );
    iStatement.BindInt( 1, aType );
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::LoadIDs
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::LoadIDs()
    {    
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::LoadIDs()"));
    
    TRAP_IGNORE( LoadIDL( iRuleGroupID, ERuleGroupID ) );
    TRAP_IGNORE( LoadIDL( iRuleID, ERuleID ) );
    TRAP_IGNORE( LoadIDL( iRuleParamID, ERuleParamID ) );
    TRAP_IGNORE( LoadIDL( iMediaServerID, EMediaServerID ) );
    TRAP_IGNORE( LoadIDL( iStoredID, EStoredID ) );
    TRAP_IGNORE( LoadIDL( iFilledID, EFilledID ) );
    TRAP_IGNORE( LoadIDL( iStoreRuleID, EStoreRuleID ) );
    TRAP_IGNORE( LoadIDL( iStoreRuleParamID, EStoreRuleParamID ) );
    TRAP_IGNORE( LoadIDL( iFillFileId, EFillFileListId) );
    TRAP_IGNORE( LoadIDL( iStoreFileId, EStoreFileListId) );       
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::LoadIDL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::LoadIDL( TUint64& aId, TTypeOfID aType )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::LoadIDL()"));
    
    TInt err = iStatement.Prepare( iDatabase, KSelectID );
    if( err )
            {
            TRACE(Print(_L("[DATABASE MANAGER]\t \
                CCmDmSQLiteConnection::LoadIDL() FAILED ")));
            User::Leave( err );
            }
    else
        {
        iStatement.BindInt( 0, aType );
        
        // get id
        err = iStatement.Next();
        if (  err == KSqlAtRow )  
            {
            aId = iStatement.ColumnInt64( 0 );
            }
        iStatement.Reset();
        iStatement.Close();                    
        }            
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::CreateDbFile
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::CreateDbFile( const TDesC& aDb )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::CreateDbFile()"));
    
    TInt err( iDatabase.Create( aDb ) );
    return err;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::Validate
// ---------------------------------------------------------------------------
//
TBool CCmDmSQLiteConnection::Validate()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::Validate()"));
    
    TInt ret( KErrNone );
    TInt err = iStatement.Prepare( iDatabase, 
        KValidateTableExistence );
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
    TBool retval = EFalse;
    if ( ret == KErrNone ) 
        {
        retval = ETrue;
        }
    return retval;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::ExecuteL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::ExecuteL( const TDesC8& aCommand )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::ExecuteL()"));
    
    TInt err( iDatabase.Exec( aCommand ) );
    if ( KErrNone > err  )
        {
        User::Leave( err );    
        }    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetFillFiles
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::SetFillFiles( 
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetFillFiles()"));
    
    iDatabase.Exec( KCmDbBeginTransaction );
    TInt err(KErrNone);
    for( TInt i = 0; i < aItems.Count(); i++ )
        {
        err = iStatement.Prepare( iDatabase, KAddFillFile );
        if( err )
            {
            TRACE(Print(_L("[DATABASE MANAGER]\t Prepare err = %d"), err ));
            }
        else
            {
            iStatement.BindInt64( 0, aItems[i]->DbId() );  
            iStatement.BindText( 1, aItems[i]->PrimaryText() );
            iStatement.BindText( 2, 
                aItems[i]->SecondaryText() );
            iStatement.BindText( 3, aItems[i]->Path() );
            iStatement.BindBinary( 4, aItems[i]->Uri() );
            iStatement.BindBinary( 5, aItems[i]->ItemId() );
            iStatement.BindText( 6, aItems[i]->UpnpClass() );
            iStatement.BindInt( 7, aItems[i]->MediaType() );
            iStatement.BindInt( 8, aItems[i]->Priority() );
            iStatement.BindInt( 9, aItems[i]->Size() );
            iStatement.BindInt( 10, aItems[i]->Status() );
            iStatement.BindInt64( 11, aItems[i]->Date().Int64() );
            iStatement.BindInt( 12, aItems[i]->ListId() );
            iStatement.BindInt( 13, aItems[i]->DevId() );
            iStatement.BindInt64( 14, aItems[i]->RefId() );
            iStatement.BindInt( 15, aItems[i]->Selected() );
            iStatement.BindInt64( 16, aItems[i]->DriveId() );
            iStatement.BindInt( 17, aItems[i]->DriveNumber() );
            iStatement.Exec();
            iStatement.Reset();  
            iStatement.Close();        
            }            
        }
    iDatabase.Exec( KCmDbSqlCommit );
    return err;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetStoreFilesL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::SetStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetStoreFilesL()"));
    
    iDatabase.Exec( KCmDbBeginTransaction );
    TInt err(KErrNone);
    for( TInt i = 0; i < aItems.Count(); i++ )
        {
        err = iStatement.Prepare( iDatabase, KAddStoreFile );
        if( err )
            {
            err = KErrGeneral;
            }
        else
            {   
            iStatement.BindInt64( 0, iStoreFileId );
            iStatement.BindText( 1, aItems[i]->PrimaryText() );
            iStatement.BindText( 2, 
                aItems[i]->SecondaryText() );
            iStatement.BindText( 3, aItems[i]->Path() );
            iStatement.BindInt( 4, aItems[i]->Size() );
            iStatement.BindInt64( 5, aItems[i]->Date().Int64() );
            iStatement.BindInt( 6, aItems[i]->ListId() );
            iStatement.Exec();
            iStatement.Reset();
            iStatement.Close();            
            }    
        /** INSERT Server */
        RArray<TInt> serverIds( aItems[i]->DevIds() );
        RArray<TCmListItemStatus> statusValues( aItems[i]->StatusValues() );
        for( TInt j = 0; j < serverIds.Count(); j++ )
            {
            err = iStatement.Prepare( iDatabase, KAddStoreFileServer );
            if( err )
                {
                err = KErrGeneral;
                }
            else
                {
                iStatement.BindInt64( 0, iStoreFileId );
                iStatement.BindInt( 1, serverIds[j] );
                iStatement.BindInt( 2, statusValues[j] );
                iStatement.Exec();
                iStatement.Reset();
                iStatement.Close();            
                }            
            }
        iStoreFileId++;                        
        }
    iDatabase.Exec( KCmDbSqlCommit );
    BackUpIdL( EStoreFileListId, iStoreFileId );
    return err;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddStoreFile
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddStoreFile( CCmStoreListItem& aItem )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddStoreFile()"));
    
    TInt err(KErrNone);
    err = iStatement.Prepare( iDatabase, KAddStoreFile );
    if( err )
        {
        err = KErrGeneral;
        }
    else
        {
        iStatement.BindInt64( 0, iStoreFileId );
        iStatement.BindText( 1, aItem.PrimaryText() );
        iStatement.BindText( 2, aItem.SecondaryText() );
        iStatement.BindText( 3, aItem.Path() );
        iStatement.BindInt( 4, aItem.Size() );
        iStatement.BindInt64( 5, aItem.Date().Int64() );
        iStatement.BindInt64( 6, aItem.ListId() );            
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        /** INSERT Server */
        RArray<TInt>serverIds( aItem.DevIds() );
        RArray<TCmListItemStatus>statusValues( aItem.StatusValues() );        
        for( TInt j = 0; j < serverIds.Count() ; j++ )
            {
            err = iStatement.Prepare( iDatabase, KAddStoreFileServer );
            if( err )
                {
                err = KErrGeneral;
                }
            else
                {
                iStatement.BindInt64( 0, iStoreFileId );
                iStatement.BindInt( 1, serverIds[j] );
                iStatement.BindInt( 2, statusValues[j] );
                iStatement.Exec();
                iStatement.Reset();
                iStatement.Close();            
                }            
            }                
        }
    iStoreFileId++;            
    return err;
    }
            
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddStoredL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddStoredL( 
    const TDesC& aFileName, 
    const TDesC8& aUDN, 
    TTime aTimeStamp )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddStoredL()"));
    /* FIRST QYERY MediaServerID from the MediaServer table
    using UDN */
    TInt err = iStatement.Prepare( iDatabase, KSelectMediaServerIDByUDN ) ;
    TUint64 mediaServerID(0);    
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t KSelectMediaServerIDByUDN \
            FAILED ")));
        err = KErrGeneral;
        }
    else
        {
        iStatement.BindBinary( 0, aUDN );
        // Loop only one row in results
        err = iStatement.Next();
        if ( err == KSqlAtRow )  
            {
            // Get cell contents    
            mediaServerID = iStatement.ColumnInt64( 0 );
            iStatement.Reset();            
            iStatement.Close();  // Destruct statement  
            err = KErrNone;          
            }        
        }
            
    err = iStatement.Prepare( iDatabase, KAddStored );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t KAddStored FAILED ")));
        err = KErrGeneral;
        }
    else
        {
        iStatement.BindInt64( 0, iStoredID );
        iStatement.BindText( 1, aFileName );          
        iStatement.BindInt64( 2, mediaServerID );    
        iStatement.BindInt64( 3, aTimeStamp.Int64() );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        err = KErrNone;   
        }
    iStoredID++;
    BackUpIdL( EStoredID, iStoredID );                

    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddFillRuleL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::AddFillRuleL( CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddFillRuleL()"));
    
    iDatabase.Exec( KCmDbBeginTransaction );
    TInt err = iStatement.Prepare( iDatabase, KAddFillGroup );
    
    if ( err == KErrNone )
        {
        if( aFillRule->ListId() )
            {
            iRuleGroupID = aFillRule->ListId();
            }
        iStatement.BindInt64( 0, iRuleGroupID );        
        iStatement.BindBinary( 1, aFillRule->Name() );
        iStatement.BindInt( 2, aFillRule->Method() );
        iStatement.BindInt( 3, aFillRule->LimitType() );
        iStatement.BindInt( 4, aFillRule->Amount() );
        iStatement.BindInt( 5, aFillRule->Selected() );
        iStatement.BindInt( 6, aFillRule->Priority() );
        iStatement.BindInt( 7, aFillRule->TemplateId() );
        iStatement.BindInt( 8, aFillRule->MediaType() );
        iStatement.BindInt( 9, aFillRule->Status() );
        iStatement.BindInt64( 10, aFillRule->ListRealCount() );
        iStatement.BindInt64( 11, aFillRule->ListRealSizeInBytes() );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();                
   
        err = AddFillRuleMediaServersL( aFillRule );
                
        if ( err == KErrNone )
            {
            err = AddFillRuleRulesAndParamsL( aFillRule );            
            }
        if ( err == KErrNone )
            {
            iRuleGroupID++;
            BackUpIdL( ERuleGroupID, iRuleGroupID );
            }
        }               

    iDatabase.Exec( KCmDbSqlCommit );    
        
    // in error cases force return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone );  
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddFillRuleMediaServersL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddFillRuleMediaServersL( CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    AddFillRuleMediaServersL()"));
    
    TInt mediaServerCount( aFillRule->MediaServerCount() );
    
    TInt mediaserverID(0);
    TInt err( KErrNone );
    for( TInt i = 0; i < mediaServerCount ; i++ )
        {
        TPtrC8 udn = aFillRule->MediaServerL( i );
        aFillRule->MediaServerL( i, mediaserverID );
        if( udn.Length() )
            {
            err = 
                iStatement.Prepare( iDatabase, KSelectMediaServerIDByUDN );
            if( !err )
                {
                iStatement.BindBinary( 0, udn );                 
                // Loop all rows in results
                while ( iStatement.Next() == KSqlAtRow )  
                    {
                    // Get cell contents    
                    mediaserverID = iStatement.ColumnInt64( 0 );
                    }                
                }
            iStatement.Reset();                     
            iStatement.Close();                                               
            }
        if( mediaserverID > 0 )
            {
            err = iStatement.Prepare( iDatabase, KAddRuleGroupServer );
            if( err )
                {
                err = KErrGeneral;
                }
            else
                {
                iStatement.BindInt64( 0, 
                    iRuleGroupID );    
                iStatement.BindInt64( 1, 
                    mediaserverID );
                iStatement.Exec();
                iStatement.Reset();
                iStatement.Close();
                err = KErrNone;                                          
                }             
            }                              
        }            
    
    return err;        
    }
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddFillRuleRulesAndParamsL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddFillRuleRulesAndParamsL( 
    CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    AddFillRuleRulesAndParamsL()"));
    
    TInt ruleCount(aFillRule->RuleCount());
    TInt err( KErrNone );
    TCmMetadataField metadataField;
    TPtrC8 dataField;
    TInt dataIndex;
    TCmOperatorType oper;
    TInt parameterCount(0);   
    for( TInt j = 0; j < ruleCount; j++)
        {
        aFillRule->RuleL( j, &metadataField, &oper, &parameterCount );

        err = iStatement.Prepare( iDatabase, KAddRule );
        
        if( err )
            {
            TRACE(Print(_L("[DATABASE MANAGER]\t \
                CCmDmSQLiteConnection::AddFilledL() FAILED ")));
            err = KErrGeneral;
            }
        else
            {
            iStatement.BindInt64( 0, iRuleID );
            iStatement.BindInt64( 1, iRuleGroupID );
            iStatement.BindInt( 2, metadataField );
            iStatement.BindInt( 3, oper );
            iStatement.Exec();
            iStatement.Reset();        
            iStatement.Close(); 
            err = KErrNone; 
            }
        
        // Add params of the rule to table        
        for( TInt k = 0; k < parameterCount; k++)
            {
            aFillRule->RuleParamL( j, k, &dataField );
            aFillRule->RuleParamL( j, k, dataIndex );
            err = iStatement.Prepare( iDatabase, KAddRuleParam );
            
            if( err )
                {
                TRACE(Print(_L("[DATABASE MANAGER]\t \
                    CCmDmSQLiteConnection::AddFilledL() FAILED ")));
                err = KErrGeneral;
                }
            else
                {
                iStatement.BindInt64( 0, iRuleParamID );
                iRuleParamID++; 
                iStatement.BindInt64( 1, iRuleID );
                if( dataField.Length() )
                    {
                    iStatement.BindBinary( 2, dataField );    
                    }
                else
                    {
                    iStatement.BindInt64( 3, dataIndex );
                    }
                iStatement.Exec();
                iStatement.Reset();      
                iStatement.Close();
                err = KErrNone;  
                }            
            }
        iRuleID++;            
        }
    BackUpIdL( ERuleID, iRuleID );
    BackUpIdL( ERuleParamID, iRuleParamID );                    

    return err;
    }        

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddStoreRuleL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddStoreRuleL( CCmStoreRule* aStoreRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddStoreRuleL()"));
    
    TInt err = iStatement.Prepare( iDatabase, KAddStoreRule );
    
    if ( err == KErrNone )
        {
        iStatement.BindInt64( 0, iStoreRuleID );
        iStatement.BindBinary( 1, aStoreRule->Name() );
        iStatement.BindInt( 2, aStoreRule->Selected() );
        iStatement.BindInt( 3, aStoreRule->Status() );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
    
        err = AddStoreRuleParamsL( aStoreRule );

        if ( err == KErrNone )
            {
            err = AddStoreRuleMediaServersL( aStoreRule );
            }
        if ( err == KErrNone )
            {
            iStoreRuleID++;
            BackUpIdL( EStoreRuleID, iStoreRuleID );
            }
        }
  
    // in error cases force return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone );    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddStoreRuleParamsL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddStoreRuleParamsL( CCmStoreRule* aStoreRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddStoreRuleParamsL()"));
    
    TInt err( KErrNone );
    TCmMediaType mediaType; 
    // Ensin tavara fillrule group tauluun
    for(TInt i = 0; i < aStoreRule->DefCount(); i++ )
        {
        aStoreRule->StoreRule(i, &mediaType);
        err = iStatement.Prepare( iDatabase, KAddStoreRuleParams );    
        if( err )
            {
            TRACE(Print(_L("[DATABASE MANAGER]\t \
                CCmDmSQLiteConnection::AddStoreRuleParamsL() FAILED ")));
            err = KErrGeneral;
            }
        else
            {        
            iStatement.BindInt64( 0, iStoreRuleID );
            iStatement.BindInt64( 1, iStoreRuleParamID );
            iStatement.BindInt( 2, (TInt)mediaType );
            iStatement.Exec();
            iStatement.Reset();
            iStatement.Close();            
            }
        iStoreRuleParamID++;            
        }
    BackUpIdL( EStoreRuleParamID, iStoreRuleParamID );        

    return err;        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddStoreRuleMediaServersL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddStoreRuleMediaServersL( 
    CCmStoreRule* aStoreRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    AddStoreRuleMediaServersL()"));
    
    TInt err( KErrNone );         
    TInt mediaServerCount( aStoreRule->MediaServerCount() );
    TRACE(Print(_L("[DATABASE MANAGER]\t \
        mediaServerCount = %d "), mediaServerCount));    
    if( mediaServerCount > KErrNone )
        {
        for( TInt i = 0; i < mediaServerCount ; i++ )
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectMediaServerIDByUDN );        

            TUint64 mediaserverID(0);            
            TPtrC8 udn = aStoreRule->MediaServerL( i );
            iStatement.BindBinary( 0, udn );
            if( err )
                {
                err = KErrGeneral;
                }
            else
                {        
                // Loop all rows in results
                if ( iStatement.Next() == KSqlAtRow )
                    {
                    // Get cell contents    
                    mediaserverID = iStatement.ColumnInt64( 0 );
                    iStatement.Reset();
                    iStatement.Close();                         
                    err = iStatement.Prepare( iDatabase, 
                        KAddStoreRuleServers );
                    if( err )
                        {
                        err = KErrGeneral;
                        }
                    else
                        {
                        iStatement.BindInt64( 0, iStoreRuleID );    
                        iStatement.BindInt64( 1, mediaserverID );
                        iStatement.Exec();
                        iStatement.Reset();
                        iStatement.Close();
                        err = KErrNone;
                        }
                                        
                    }   
                }               
            }        
        }               
    
    return err;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddMediaServerL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddMediaServerL( 
    CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddMediaServerL()"));    
    
    TInt err = iStatement.Prepare( iDatabase, KAddMediaServer );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            CCmDmSQLiteConnection::AddMediaServerL() FAILED ")));
        err = KErrGeneral;
        }
    else
        {
        iStatement.BindInt64( 0, iMediaServerID ); 
        iStatement.BindBinary( 1, aMediaServer->MediaServer() );
        iStatement.BindBinary( 2, aMediaServer->MediaServerName() );
        iStatement.BindInt( 3, aMediaServer->SystemUpdateID() );
        iStatement.BindInt64( 4, aMediaServer->VisibleDate().Int64() );
        iStatement.BindInt( 5, KErrNone );
        iStatement.BindInt( 6, KErrNone );
        iStatement.BindInt( 7, KErrNone );
        iStatement.BindInt( 8, KErrNone );
        iStatement.BindInt( 9, KErrNone );
        iStatement.BindInt( 10, (TInt)aMediaServer->CopyCapability() );
        iStatement.BindInt( 11, aMediaServer->FillUsage() );
        iStatement.BindInt( 12, aMediaServer->StoreUsage() );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        err = KErrNone;  
        }
    iMediaServerID++;
    BackUpIdL( EMediaServerID, iMediaServerID );        
   
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::AddHashValue
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::AddHashValue( TInt64 aMediaServerId, 
                                          CCmSearchResponseHash* aHashValue )
    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::AddHashValue()"));
        
    TInt err = iStatement.Prepare( iDatabase, KAddHashValue );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            CCmDmSQLiteConnection::AddHashValue() FAILED ")));
        err = KErrGeneral;
        }
    else
        {
        iStatement.BindInt64( 0, aMediaServerId ); 
        iStatement.BindInt( 1, aHashValue->StartIndex() );
        iStatement.BindInt( 2, aHashValue->ItemCount()  );
        iStatement.BindBinary( 3, aHashValue->Hash() );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        err = KErrNone;  
        }
      
    return err;
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetMediaServersL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::SetMediaServersL( 
    RPointerArray<CCmMediaServerFull>& aServers )  
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetMediaServersL()"));    
    TInt err( KErrNone );
    
    TInt sqlRet( iDatabase.Exec( KInitIsActiveFlags ) );

    if ( sqlRet >= KErrNone )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            %d rows have been changed/inserted/deleted"), sqlRet ));

        TInt id( KErrNone );
        // Add new servers into 
        HBufC8* uuid = NULL;
        for( TInt j = 0; j < aServers.Count(); j++ )
            {
            uuid = aServers[j]->MediaServer().AllocLC();
            PrepareQueryCmdL(EMediaServerIdQuery);
            id = QueryMediaServerId( *uuid );
            if( id == KErrNotFound )
                {
                AddMediaServerL( aServers[j] );
                }
            CleanupStack::PopAndDestroy( uuid );
            uuid = NULL;    
            }
        
        for( TInt i = 0; i < aServers.Count() ; i++ )
            {
            err = iStatement.Prepare( iDatabase, KUpdateMsIsActive );
        
            if( err )
                {
                TRACE(Print(_L("[DATABASE MANAGER]\t \
                CCmDmSQLiteConnection::SetMediaServersL() FAILED ")));
                err = KErrGeneral;
                }
            else
                {
                iStatement.BindInt( 0, aServers[i]->IsActive() );
                iStatement.BindInt( 1, aServers[i]->SystemUpdateID() );
                //update the server name
                iStatement.BindBinary( 2, aServers[i]->MediaServerName() );
                iStatement.BindInt( 3, aServers[i]->FillUsage() );
                iStatement.BindInt( 4, aServers[i]->StoreUsage() );
                iStatement.BindBinary( 5, aServers[i]->MediaServer() );
                iStatement.Exec();                
                }
            iStatement.Reset(); 
            iStatement.Close();            
            }     
        
        if ( err == KErrNone )
            {
            err = DeleteInActiveRuleServers();
            }
    	        
        if ( err == KErrNone )
            {
            err = DeleteInActiveFiles();            
            }        
        }

    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetSyncTime
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::SetSyncTime()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetSyncTime()"));    
    
    TInt err( iDatabase.Exec( KRemoveSyncTime ) );   
    
    if ( err == KErrNone )
        {
        err = iStatement.Prepare( iDatabase, KSetSyncTime );
        if ( err == KErrNone )
            {
            TTime time;
            time.HomeTime();
            iStatement.BindInt64( 0, time.Int64() );
            iStatement.Exec();               
            }
        iStatement.Reset(); 
        iStatement.Close();         
        }
    // in error cases force return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone ); 
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetSyncTime
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetSyncTime( TTime& aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetSyncTime()"));    
    
    TInt err( iStatement.Prepare( iDatabase, KGetSyncTime ) );
    aTime = KErrNone;
    if( err )
        {
        err = KErrGeneral;  
        }
    else
        {
        err = iStatement.Next(); 
        if ( err == KSqlAtRow )  
            { 
            aTime = iStatement.ColumnInt64( 0 );
            err = KErrNone;                
            }
        }
    iStatement.Reset(); 
    iStatement.Close();     
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetProgressInfo
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::SetProgressInfo( TInt aTransferred, 
    TInt aTotalCount)
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetProgressInfo()"));    
    
    TInt err( iDatabase.Exec( KRemoveProgressInfo ) );
    if ( err == KErrNone )
        {
        err = iStatement.Prepare( iDatabase, KSetProgressInfo );
        if( err )
            {
            err = KErrGeneral;
            }
        else
            {
            iStatement.BindInt( 0, aTransferred );
            iStatement.BindInt( 1, aTotalCount );
            iStatement.Exec();        
            }
        iStatement.Reset(); 
        iStatement.Close();                 
        }
        
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetProgressInfo
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetProgressInfo( TInt& aTransferred, 
    TInt& aTotalCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetProgressInfo()"));    
    
    TInt err( iStatement.Prepare( iDatabase, KGetProgressInfo ) );
    if( err )
        {
        err = KErrGeneral;  
        }
    else
        {
        err = iStatement.Next();
        if ( err == KSqlAtRow )  
            {
            aTransferred = iStatement.ColumnInt( 0 );
            aTotalCount = iStatement.ColumnInt( 1 );
            err = KErrNone;                         
            }
        }
    iStatement.Reset(); 
    iStatement.Close();     
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetDrivesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::SetDrivesL( RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetDrivesL()"));    
    
    TInt err( iDatabase.Exec( KRemoveDrivesInfo ) );
    
    if ( err == KErrNone)
        {
        iDatabase.Exec( KCmDbBeginTransaction );   
        for( TInt i = 0; i < aDrives.Count(); i++ )
            {
            err = iStatement.Prepare( iDatabase, KSetDrivesInfo );
            User::LeaveIfError( err ); 
            iStatement.BindInt( 0, aDrives[i]->DriveNumber() );
            iStatement.BindInt( 1, aDrives[i]->DriveType() );
            iStatement.BindText( 2, aDrives[i]->DriveName() );
            iStatement.BindInt64( 3, aDrives[i]->DriveSize() );
            iStatement.BindInt64( 4, aDrives[i]->DriveQuota() );
            iStatement.BindInt64( 5, aDrives[i]->UsedDriveQuota() );
            iStatement.BindInt64( 6, aDrives[i]->DriveId() );
            iStatement.BindInt( 7, aDrives[i]->Status() );
            iStatement.Exec();
            iStatement.Reset();
            iStatement.Close();
            }        
        }
    iDatabase.Exec( KCmDbSqlCommit );              
    }
                
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::PrepareQueryCmdL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::PrepareQueryCmdL( TCMQueryCmdType aType )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::PrepareQueryCmdL()"));
    
    TInt err( KErrNone );                  
    switch( aType )
        {
        case EFillRuleQuery:
            {
            iDatabase.Exec( KCmDbBeginTransaction );
            err = iStatement.Prepare( iDatabase, KSelectFillRule );
            break;
            }
        case EMediaServerInfoQyery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectDeviceInfoByUDN );
            break;
            }
        case EMediaServerIdQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectMediaServerIDByUDN );            
            break;
            }
        case EMediaServerUDNQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectMediaServerUDNByID );
            break;    
            }
        case EMediaServersAllQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectAllMediaServers );
            break;            
            }          
        case EHashValueQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectHashValuesByMediaServer );
            break;            
            }          
        case EStoredQuery:
            {
            err = iStatement.Prepare( iDatabase, KIsFileAlreadyStored );
            break;
            }         
        case EStoredRowQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectStoredByUDN );
            break;
            }
        case ESelectedFillRuleQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectSelectedFillRuleNames);
            break;
            }
        case EFillRuleNamesQuery:
            {
            iDatabase.Exec( KCmDbBeginTransaction );
            err = iStatement.Prepare( iDatabase, KSelectFillRuleNames );
            break;
            }
        case EStoreRuleQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectStoreRule );
            break;
            }
        case EStoreRuleNamesQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectStoreRuleNames );
            break;
            }
        case ESelectedStoreRuleQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectSelectedStoreRuleNames );
            break;
            } 
        case ETransferHistoryQuery:
            {    
            err = iStatement.Prepare(  iDatabase, 
                KSelectTransferHistory );
            break;
            }
        case EFillFilesQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectFillFiles );
            break;
            }
        case EDeletedFillFilesQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectDeletedFillFiles );
            break;
            }
        case EStoreFilesQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectStoreFiles );
            break;
            }        
        case EAllFillFilesQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectAllFillFiles );
            break;
            }        
        case EAllStoreFilesQuery:
            {
            err = iStatement.Prepare( iDatabase, KSelectAllStoreFiles ); 
            break;
            }        
        case EFilledLowPriorityQuery:
            {
            err = iStatement.Prepare( iDatabase, 
                KSelectLowestPriorityFilled );
            break;
            }
        case EAllFillFilesStatusQuery:
            {
            err = iStatement.Prepare( iDatabase, KGetAllFillFiles );
            break;
            }
        case EAllStoreFilesStatusQuery:    
            {
            err = iStatement.Prepare( iDatabase, KGetAllStoreFiles );
            break;
            }
        case EDrivesQuery:
            {
            err = iStatement.Prepare( iDatabase, KGetDrives );
            break;
            }
        default:
            {
            LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::PrepareQueryCmdL() \
            type not found"));
            break;
            }
                                                            
        }
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t err = %d "), err ));        
        User::Leave( err ); 
        }      
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryFillRuleL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryFillRuleL( CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleL()"));
            
    TInt err( KErrNone );
    TUint64 ruleGroupID(0);
    TPtrC8 tempptr;
    iStatement.BindBinary( 0, aFillRule->Name() );                
    // Loop only one row in results
    err = iStatement.Next();
    if ( err == KSqlAtRow )
        {
        // Get cell contents
        ruleGroupID = iStatement.ColumnInt64( 0 );
        aFillRule->SetListId( (TUint8)ruleGroupID );
        iStatement.ColumnBinary( 1, tempptr );
        aFillRule->SetNameL( tempptr );
        aFillRule->SetMethod( 
            (TCmFillMethod)iStatement.ColumnInt( 2 ) );
        aFillRule->SetLimitType( 
            (TCmLimitType)iStatement.ColumnInt( 3 ) );
        aFillRule->SetAmount( 
            iStatement.ColumnInt( 4 ) );
        aFillRule->SetSelected( 
            (TCmFillRuleStatus)iStatement.ColumnInt( 5 ) );
        aFillRule->SetPriority( 
            (TUint8)iStatement.ColumnInt( 6 ) );
        aFillRule->SetTemplateId( 
            (TUint8)iStatement.ColumnInt( 7 ) );
        aFillRule->SetMediaType( 
            (TCmMediaType)iStatement.ColumnInt( 8 ) );
        aFillRule->SetStatus( 
            (TCmListItemStatus)iStatement.ColumnInt( 9 ) );
        aFillRule->SetListRealCount( iStatement.ColumnInt( 10 ) );    
        aFillRule->SetListRealSizeInBytes( 
            iStatement.ColumnInt(11) );
        err = KErrNone;             
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    err = QueryFillRuleServersL( aFillRule, ruleGroupID );
    if ( err )
        {
        LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleL() \
        error getting servers"));
        }

    err = QueryFillRuleParamsL( aFillRule );    
    if ( err )
        {
        LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleL() \
        error getting fill list parameters"));
        }
    
    iDatabase.Exec( KCmDbSqlCommit );
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryFillRuleServersL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryFillRuleServersL( CCmFillRule* aFillRule, 
    TUint64 aRuleGroupID )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleServersL()"));
    
    TInt err( iStatement.Prepare( iDatabase, 
        KSelectMediaServerIDByRuleGroupID ) );
    RArray<TUint64> mediaServerIds;
    CleanupClosePushL( mediaServerIds );
    if( err )
        {
        LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
        QueryFillRuleServersL() select mediaserver fail"));
        }
    else
        {
        iStatement.BindInt64( 0, aRuleGroupID );        
        // Loop all rows in results
        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            mediaServerIds.AppendL( 
                iStatement.ColumnInt64(0) );             
            }    
        iStatement.Reset();            
        iStatement.Close();  // Destruct statement
        }
            
    TInt mediaServerCount( mediaServerIds.Count());

    for( TInt mediaServerIndex = 0; mediaServerIndex < mediaServerCount ; 
         mediaServerIndex++ )
        {   
        aFillRule->AddMediaServerL( 
            mediaServerIds[mediaServerIndex]);
        }
        
    mediaServerIds.Reset();
    CleanupStack::PopAndDestroy( &mediaServerIds );
    
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryFillRuleParamsL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryFillRuleParamsL( CCmFillRule* aFillRule )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleParamsL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KSelectRule ) );
    RArray<TUint64> ruleIds;    
    CleanupClosePushL( ruleIds );
    TPtrC8 temp;
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            CCmDmSQLiteConnection::QueryFillRule() FAIL5")));
        err = KErrGeneral; 
        }
    else
        {    
        iStatement.BindBinary( 0, aFillRule->Name() );
        // Loop all rows in results
        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell content
            ruleIds.AppendL( iStatement.ColumnInt64( 0 ) );
            aFillRule->AddRuleL( 
                (TCmMetadataField)iStatement.ColumnInt( 1 ), 
                (TCmOperatorType)iStatement.ColumnInt( 2 ) );
            }            
        
        iStatement.Reset();            
        iStatement.Close();  // Destruct statement
        }
        
    TInt numberOfRules( ruleIds.Count() );              
    for( TInt index = 0; index < numberOfRules ; index++ )
        {
        err = iStatement.Prepare( iDatabase, 
            KSelectRuleParamsByRuleID );
        if( err )
            {
            TRACE(Print(_L("[DATABASE MANAGER]\t \
                KSelectRuleParamsByRuleID FAIL")));
            err = KErrGeneral;            
            }
        else
            {
            iStatement.BindInt64( 0, ruleIds[index] );
            // Loop all rows in results
            for ( err = iStatement.Next(); 
                  err == KSqlAtRow; 
                  err = iStatement.Next() )
                {
                // Get cell contents
                iStatement.ColumnBinary( 0, temp );
                if( temp.Length() )
                    {
                    aFillRule->AddRuleParamL( index, temp );
                    }
                else
                    {
                    aFillRule->AddRuleParamL( index, 
                        iStatement.ColumnInt64( 1 ) );
                    }                                           
                }
            iStatement.Reset();            
            iStatement.Close();  // Destruct statement                    
            }    
        }                                          
    
    ruleIds.Reset();
    CleanupStack::PopAndDestroy( &ruleIds );
            
    TRACE(Print(_L("[DATABASE MANAGER]\t \
        CCmDmSQLiteConnection::QueryFillRule() end")));
    return err;                                
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QuerySelectedFillRuleNamesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QuerySelectedFillRuleNamesL( 
    RPointerArray<HBufC8>& aNames, TInt aSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    QuerySelectedFillRuleNamesL()"));   
    
    iStatement.BindInt( 0, aSelected );      
    // Loop only one row in results
    TPtrC8 tempPtr;
    while( KSqlAtRow == iStatement.Next() )   
        {
        // Get cell contents
        iStatement.ColumnBinary( 0, tempPtr );
        aNames.Append( tempPtr.AllocL() ); 
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement                                     
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryFillRuleNamesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryFillRuleNamesL( 
    RPointerArray<HBufC8>& aNames )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillRuleNamesL()"));   
                     
    // Loop only one row in results
    TPtrC8 temp;
    while( KSqlAtRow == iStatement.Next() )  
        {
        // Get cell contents               
        iStatement.ColumnBinary( 0, temp );        
        aNames.Append( temp.AllocL() );                   
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement
    iDatabase.Exec( KCmDbSqlCommit );                     
    TRACE(Print(_L("[DATABASE MANAGER]\t CCmDmSQLiteConnection::\
        QueryFillRuleNamesL() end")));                   
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryStoreRuleL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryStoreRuleL( CCmStoreRule* aStoreRule )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryStoreRuleL()"));
    
    TUint64 ruleGroupID(0);
    TPtrC8 temp;
    iStatement.BindBinary( 0, aStoreRule->Name() );                
    // Loop only one row in results
    TInt err( iStatement.Next() );
    if ( err == KSqlAtRow )  
        {
        // Get cell contents
        ruleGroupID = iStatement.ColumnInt64( 0 );
        aStoreRule->SetListId( ruleGroupID );
        iStatement.ColumnBinary( 1, temp );
        aStoreRule->SetNameL( temp );   
        aStoreRule->SetSelected( 
            (TCmFillRuleStatus)iStatement.ColumnInt( 2 ) );
        aStoreRule->SetStatus( 
            (TCmListItemStatus)iStatement.ColumnInt( 3 ) );
        err = KErrNone;                               
        }    
    iStatement.Reset();             
    iStatement.Close();  // Destruct statement
    err = QueryStoreRuleDefsL( aStoreRule, ruleGroupID );
    
    return err;        
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryStoreRuleDefsL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryStoreRuleDefsL( CCmStoreRule* aStoreRule,
    TUint64 aRuleGroupID )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryStoreRuleDefsL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KSelectStoreRuleDefs ) );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            CCmDmSQLiteConnection::QueryStoreRuleL() FAIL5")));
        err = KErrGeneral; 
        }
    else
        {  
        TPtrC8 temp;  
        iStatement.BindBinary( 0, aStoreRule->Name() );
        // Loop all rows in results
        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents
            aStoreRule->AddStoreRuleL( 
                (TCmMediaType)iStatement.ColumnInt( 1 ) ); 
            }            
        
        iStatement.Reset();            
        iStatement.Close();  // Destruct statement

        err = iStatement.Prepare( iDatabase, KSelectStoreRuleMediaServerID );
        RArray<TUint64> mediaServerIds;
        CleanupClosePushL( mediaServerIds );        
        
        if( err )
            {
            TRACE( Print(
                _L("[DATABASE MANAGER]\t KSelectMediaServerID FAIL") ) );
            }
        else
            {
            iStatement.BindInt64( 0, aRuleGroupID );
            // Loop all rows in results
            for ( err = iStatement.Next(); 
                  err == KSqlAtRow; 
                  err = iStatement.Next() )
                {
                // Get cell contents    
                mediaServerIds.AppendL( 
                    iStatement.ColumnInt64( 0 ));
                }
            iStatement.Reset();            
            iStatement.Close();// Destruct statement
            TInt mediaServerCount( mediaServerIds.Count() );             
            for( TInt mediaServerIndex = 0; 
                mediaServerIndex < mediaServerCount ; mediaServerIndex++ )
                {
                err = iStatement.Prepare( iDatabase, 
                    KSelectRuleUDNsByMediaServerID );
                if( err )
                    {
                    TRACE(Print(_L("[DATABASE MANAGER]\t \
                        KSelectRuleUDNsByMediaServerID FAIL")));
                    err = KErrGeneral;            
                    }
                else
                    {
                    iStatement.BindInt64( 0, 
                        mediaServerIds[mediaServerIndex] );
                    // Loop all rows in results
                        for ( err = iStatement.Next(); 
                              err == KSqlAtRow; 
                              err = iStatement.Next() )
                        {
                        // Get cell contents
                        iStatement.ColumnBinary( 0, temp );
                        aStoreRule->AddMediaServerL( temp );
                        }
                    iStatement.Reset();            
                    iStatement.Close();  // Destruct statement
                    }                                                         
                }
            mediaServerIds.Reset();
            CleanupStack::PopAndDestroy( &mediaServerIds );
            }
        }        
    return err;        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryStoreRuleNamesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryStoreRuleNamesL( 
    RPointerArray<HBufC8>& aNames )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryStoreRuleNamesL()"));  
    
    // Loop only one row in results
    TPtrC8 temp;
    while( KSqlAtRow == iStatement.Next() )  
        {
        // Get cell contents               
        iStatement.ColumnBinary( 0, temp );
        aNames.Append( temp.AllocL() );             
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement                        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QuerySelectedStoreRuleNamesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QuerySelectedStoreRuleNamesL(
    RPointerArray<HBufC8>& aNames, TInt aSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    QuerySelectedStoreRuleNamesL()"));
        
    // Do binding only for the first row
    iStatement.BindInt( 0, aSelected );
    // Loop only one row in results
    TPtrC8 temp;
    while( KSqlAtRow == iStatement.Next() )  
        {
        // Get cell contents
        iStatement.ColumnBinary( 0, temp );
        aNames.Append( temp.AllocL() );                
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement                    
    }    
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryMediaServerL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryMediaServerL( 
    CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryMediaServerL()"));
          
    iStatement.BindBinary( 0, aMediaServer->MediaServer() ); 
        // Loop only one row in results
    TInt err( KErrNotFound );
    if ( KSqlAtRow == iStatement.Next() )  
        {
        TPtrC8 temp;
        // Get cell contents
        iStatement.ColumnBinary( 2, temp );    
        aMediaServer->SetMediaServerNameL( temp );
        aMediaServer->SetSystemUpdateID( 
            iStatement.ColumnInt( 3 ) );
        aMediaServer->SetVisibleDate( 
            iStatement.ColumnInt64( 4 ) );
        aMediaServer->SetIsActive( 
            iStatement.ColumnInt( 5 ) ); 
        aMediaServer->SetFillUsage( 
            iStatement.ColumnInt( 6 ) );
        err = KErrNone;             
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement            

    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryMediaServerId
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::QueryMediaServerId( const TDesC8& aUDN )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryMediaServerId()"));
    
    iStatement.BindBinary( 0, aUDN );
    // Loop only one row in results
    TInt err( KErrNotFound );
    if ( KSqlAtRow == iStatement.Next() )  
        {
        // Get cell contents   
        err = iStatement.ColumnInt64( 0 );        
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement            

    return err;        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryMediaServerUdn
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryMediaServerUdn( HBufC8*& aUdn, 
                                                 const TInt32 aId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryMediaServerId()"));
    
    iStatement.BindInt64( 0, aId );
    // Loop only one row in results
    TInt err( iStatement.Next() );
    if ( err == KSqlAtRow )  
        {
        // Get cell contents
        TPtrC8 temp;
        iStatement.ColumnBinary( 0, temp );
        aUdn = temp.Alloc();
        err = KErrNone;
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement            

    return err;        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryAllMediaServersL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryAllMediaServersL( 
    CCmMediaServerFull* aMediaServer )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryAllMediaServersL()"));
    
    // Loop only one row in results
    TInt err( KErrNotFound );
    if ( KSqlAtRow == iStatement.Next() )  
        {
        TPtrC8 temp;
        // Get cell contents
        iStatement.ColumnBinary( 0, temp );
        aMediaServer->SetUDNL(temp);  
        iStatement.ColumnBinary( 1, temp );    
        aMediaServer->SetMediaServerNameL( temp );
        aMediaServer->SetSystemUpdateID( 
            iStatement.ColumnInt( 2 ) );
        aMediaServer->SetVisibleDate( 
            iStatement.ColumnInt64( 3 ) );
        aMediaServer->SetIsActive( 
            iStatement.ColumnInt( 4 ) );
        aMediaServer->SetDbId( 
            iStatement.ColumnInt( 5 ) );            
        aMediaServer->SetCopyCapability( (TBool)iStatement.ColumnInt( 6 ) );
        aMediaServer->SetFillUsage( iStatement.ColumnInt( 7 ) );
        aMediaServer->SetStoreUsage( iStatement.ColumnInt( 8 ) );
        err = KErrNone;                                       
        }      
    if( KErrNotFound == err )
        {
        iStatement.Reset();                
        iStatement.Close();  // Destruct statement    
        }            
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetMediaServersL
// ---------------------------------------------------------------------------
//    
void CCmDmSQLiteConnection::GetMediaServersL( 
    RPointerArray<CCmMediaServerFull>& aMediaServers )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetMediaServersL()"));
       
    iStatement.PrepareL( iDatabase, KSelectAllMediaServers );
    // Loop only one row in results
    while ( KSqlAtRow == iStatement.Next() )  
        {
        CCmMediaServerFull* server = CCmMediaServerFull::NewLC();
        TPtrC8 temp;
        // Get cell contents
        iStatement.ColumnBinary( 0, temp );
        server->SetUDNL(temp);  
        iStatement.ColumnBinary( 1, temp );    
        server->SetMediaServerNameL( temp );
        server->SetSystemUpdateID( 
            iStatement.ColumnInt( 2 ) );
        server->SetVisibleDate( 
            iStatement.ColumnInt64( 3 ) );
        server->SetIsActive( 
            iStatement.ColumnInt( 4 ) );
        server->SetDbId( 
            iStatement.ColumnInt( 5 ) );
        server->SetCopyCapability( 
            iStatement.ColumnInt( 6 ) );
        server->SetFillUsage( iStatement.ColumnInt( 7 ) );                
        server->SetStoreUsage( iStatement.ColumnInt( 8 ) );                
            
        aMediaServers.AppendL( server ); // transfer ownership
        CleanupStack::Pop( server );
        server = NULL;
        }      
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement               
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryHashValuesL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryHashValuesL( TInt aMediaServer, 
    RPointerArray<CCmSearchResponseHash>& aHashValues )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryHashValuesL()"));
    
    iStatement.BindInt( 0, aMediaServer );        
    // Loop only one row in results
    TInt err( KErrNone );
    while( ( err = iStatement.Next( ) ) == KSqlAtRow ) 
        {
        CCmSearchResponseHash* newHash = CCmSearchResponseHash::NewL();
        CleanupStack::PushL( newHash );
        // Get cell contents   
        newHash->SetStartIndex( iStatement.ColumnInt( 0 ));
        newHash->SetItemCount( iStatement.ColumnInt( 1 ));  
        TPtrC8 temp; 
        iStatement.ColumnBinary( 2, temp );
        newHash->SetHashL( temp );
        aHashValues.AppendL( newHash );             
        CleanupStack::Pop( newHash );
        
        }    
    iStatement.Reset( );                
    iStatement.Close( );  // Destruct statement            
    
    return err;        
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetLowPriorityFilledFilesL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetLowPriorityFilledFilesL( TInt aDrive,
    RPointerArray<CCmFillListItem>& aFilled )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    GetLowPriorityFilledFilesL()"));
    
    TPtrC temp;
    TUint32 size( KErrNone );
    TInt64 listId( KErrNone );
    TInt64 fileId( KErrNone );      
    iStatement.BindInt( 0, (TInt)ECmFilled );
    iStatement.BindInt( 1, (TInt)ECmFilled );
    iStatement.BindInt( 2, aDrive );
    TInt err( KErrNone );        

    for ( err = iStatement.Next(); 
          err == KSqlAtRow; 
          err = iStatement.Next() )
        {
        iStatement.ColumnText( 0, temp );
        size = iStatement.ColumnInt( 1 );
        listId = iStatement.ColumnInt( 2 );
        fileId = iStatement.ColumnInt64( 3 );   
        if( temp.Length() )
            {
            CCmFillListItem* filled = CCmFillListItem::NewLC();
            filled->SetPathL( temp );
            filled->SetSize( size );
            filled->SetListId( listId );
            filled->SetDbId( fileId );
            aFilled.AppendL( filled );
            CleanupStack::Pop( filled );                
            }                                
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return err;            
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryFillFileListL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::QueryFillFileListL( 
    RPointerArray<CCmBaseListItem>& aItems, 
    const TDesC8& aName, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryFillFileListL()"));
    
    iStatement.BindInt( 0, aStatus );
    iStatement.BindBinary( 1, aName );
    TPtrC temp;
    TInt ret( KErrNone );
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmBaseListItem* item = CCmBaseListItem::NewLC();
        /**
        Read rows into package
        */
        iStatement.ColumnText( 0, temp );
        item->SetPrimaryTextL( temp );
        iStatement.ColumnText( 1, temp );
        item->SetSecondaryTextL( temp );
        iStatement.ColumnText( 2, temp );
        item->SetPathL( temp );                           
        item->SetSize( iStatement.ColumnInt64( 3 ));
        item->SetStatus(
            (TCmListItemStatus)iStatement.ColumnInt( 4 ));
        item->SetDate( iStatement.ColumnInt64( 5 ) );
        item->SetDbId( iStatement.ColumnInt64( 6 ) );
        item->SetRefId( iStatement.ColumnInt64( 7 ) );
        aItems.Append( item );
        CleanupStack::Pop( item );
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;            
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetAllFillFilesL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetAllFillFilesL( 
                                    RPointerArray<CCmFillListItem>& aItems,
                                    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetAllFillFilesL()"));
    
    TInt ret(KErrNone);
    TPtrC temp16;
    TPtrC8 temp8;
    iStatement.BindInt( 0, aStatus );
    while( iStatement.Next() == KSqlAtRow)
        {
        CCmFillListItem* item = CCmFillListItem::NewLC();
        /**
        Read rows into package
        */
        item->SetDbId( iStatement.ColumnInt64( 0 ) );
        iStatement.ColumnText( 1, temp16 );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        if ( temp16.Length() != 0 ) 
            {
            item->SetSecondaryTextL( temp16 );
            }
        else 
            {
            item->SetSecondaryTextL( KNullDesC() );
            }
        
        iStatement.ColumnText( 3, temp16 );
        if ( temp16.Length() != 0 ) 
            {
            item->SetPathL( temp16 );
            }
        else 
            {
            item->SetPathL( KNullDesC() );
            }
        iStatement.ColumnBinary( 4, temp8 );
        item->SetUriL( temp8 );
        iStatement.ColumnBinary( 5, temp8 );
        item->SetItemIdL( temp8 );   
        iStatement.ColumnText( 6, temp16 );
        item->SetUpnpClassL( temp16 );
        item->SetMediaType( (TCmMediaType)
            iStatement.ColumnInt( 7 ) );    
        item->SetPriority( iStatement.ColumnInt( 8 ) );
        item->SetSize( iStatement.ColumnInt64( 9 ) );
        item->SetStatus(
            (TCmListItemStatus)iStatement.ColumnInt( 10 ) );
        item->SetDate( iStatement.ColumnInt64( 11 ) );
        item->SetListId( iStatement.ColumnInt( 12 ) );
        item->SetDevId( iStatement.ColumnInt( 13 ) );
        item->SetRefId( iStatement.ColumnInt64( 14 ) );
        item->SetSelected( (TCmFillRuleStatus)
            iStatement.ColumnInt( 15 ) );
        item->SetDriveId( iStatement.ColumnInt64( 16 ) );
        item->SetDriveNumber( iStatement.ColumnInt( 17 ) );    
        aItems.Append( item );
        CleanupStack::Pop( item );
        }       
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;    
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryStoreFileListL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryStoreFileListL( 
    RPointerArray<CCmBaseListItem>& aItems, 
    const TDesC8& aName, 
    TUint /*aStatus*/ )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryStoreFileListL()"));
    
    TInt ret(KErrNone);
    TPtrC temp16;
    iStatement.BindBinary( 0, aName );
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmBaseListItem* item = CCmBaseListItem::NewLC();        
        /**
        Read rows into package
        */
        iStatement.ColumnText( 0, temp16 );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 1, temp16 );
        item->SetSecondaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        item->SetPathL( temp16 );                       
        item->SetSize( iStatement.ColumnInt64( 3 ) );
        item->SetDate( iStatement.ColumnInt64( 4 ) );
        aItems.Append(item); // transfer ownership
        CleanupStack::Pop( item );
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetAllStoreFilesL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetAllStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems, TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetAllStoreFilesL()"));
    
    TInt ret(KErrNone);
    iStatement.BindInt( 0, aStatus );
    TPtrC temp16;
    TUint status(KErrNone);
    TUint8 devId(KErrNone);
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmStoreListItem* item = CCmStoreListItem::NewL();
        CleanupStack::PushL( item );
        /**
        Read rows into package
        */
        iStatement.ColumnText( 0, temp16 );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 1, temp16 );
        item->SetSecondaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        item->SetPathL( temp16 );                        
        item->SetSize( iStatement.ColumnInt64( 3 ));
        status = iStatement.ColumnInt( 4 );
        item->SetListId( iStatement.ColumnInt( 5 ) );
        devId = iStatement.ColumnInt( 6 );
        item->SetDevId( devId, (TCmListItemStatus)status );            
        aItems.AppendL( item ); // transfer ownership
        CleanupStack::Pop( item );
        }       
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;    
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryAllFillFilesL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryAllFillFilesL( 
    RPointerArray<CCmFillListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryAllFillFilesL()"));
    
    TInt ret(KErrNone);
    TPtrC temp16;
    TPtrC8 temp8;
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmFillListItem* item = CCmFillListItem::NewL();
        CleanupStack::PushL( item );
        /**
        Read rows into package
        */
        item->SetDbId( iStatement.ColumnInt64( 0 ) );
        iStatement.ColumnText( 1, temp16 );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        item->SetSecondaryTextL( temp16 );
        iStatement.ColumnText( 3, temp16 );
        item->SetPathL( temp16 );             
        iStatement.ColumnBinary( 4, temp8 );
        item->SetUriL( temp8 );
        iStatement.ColumnBinary( 5, temp8 );
        item->SetItemIdL( temp8 );   
        iStatement.ColumnText( 6, temp16 );
        item->SetUpnpClassL( temp16 );
        item->SetMediaType( (TCmMediaType)
            iStatement.ColumnInt( 7 ) );    
        item->SetPriority( iStatement.ColumnInt( 8 ) );
        item->SetSize( iStatement.ColumnInt64( 9 ));
        item->SetStatus(
            (TCmListItemStatus)iStatement.ColumnInt( 10 ));
        item->SetDate( iStatement.ColumnInt64( 11 ) );
        item->SetListId( iStatement.ColumnInt( 12 ) );
        item->SetDevId( iStatement.ColumnInt( 13 ) );
        item->SetRefId( iStatement.ColumnInt64( 14 ) );
        item->SetSelected( ( TCmFillRuleStatus )
            iStatement.ColumnInt( 15 ));
        item->SetDriveId( iStatement.ColumnInt64( 16 ) );
        item->SetDriveNumber( iStatement.ColumnInt( 17 ) );    
        aItems.AppendL( item ); // transfer ownership
        CleanupStack::Pop( item );
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryAllStoreFilesL
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::QueryAllStoreFilesL( 
    RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryAllStoreFilesL()"));
    
    TInt ret(KErrNone);
    TPtrC temp16;
    TBool found( EFalse );
    TInt foundIndex( KErrNone );    
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmStoreListItem* item = CCmStoreListItem::NewL();
        CleanupStack::PushL( item );
        /**
        Read rows into package
        */
        item->SetDbId( iStatement.ColumnInt64( 0 ) );
        iStatement.ColumnText( 1, temp16  );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        item->SetSecondaryTextL( temp16 );    
        iStatement.ColumnText( 3, temp16 );
        item->SetPathL( temp16 );                               
        item->SetSize( iStatement.ColumnInt64( 4 ) );
        TCmListItemStatus status = 
            (TCmListItemStatus)iStatement.ColumnInt( 5 );

        item->SetListId( iStatement.ColumnInt( 6 ) );
        TInt devId = iStatement.ColumnInt( 7 );
        item->SetDevId( devId, status );            
        for( TInt i = 0; i < aItems.Count() && !found ; i++ )
            {
            if( aItems[i]->DbId() == item->DbId() )
                {
                found = ETrue;
                foundIndex = i;
                }
            }
        if( !found )
            {
            aItems.AppendL( item ); // transfer ownership
            CleanupStack::Pop( item );
            }
        else
            {
            aItems[foundIndex]->SetDevId( devId, status );
            CleanupStack::PopAndDestroy( item ); // Destroy item
            }        
        found = EFalse;
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement    
    return ret;    
    }
                
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryStoredRowL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryStoredRowL( 
    RPointerArray<HBufC16>& aFiles, 
    const TDesC8& aUDN, 
    TInt aAmount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryStoredRowL()"));
           
    iStatement.BindBinary( 0, aUDN );
    TInt amount = aAmount;    
    // Loop only one row in results
    while( KSqlAtRow == iStatement.Next() || amount == 0 )  
        {
        // Get cell contents    
        TPtrC temp;
        iStatement.ColumnText( 0, temp );
        aFiles.Append( temp.AllocL() ); 
        if( amount != KErrNotFound )
            {
            amount--;
            }
        }    
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement                           
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryTransferHistory
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryTransferHistory( 
    const TDesC8& aUdn,
    TInt64& aDownloadData, 
    TInt64& aUploadData, 
    TInt64& aDownloadTime, 
    TInt64& aUploadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryTransferHistory()"));
    
    iStatement.BindBinary( 0, aUdn );
    // Loop only one row
    if( KSqlAtRow == iStatement.Next() )  
        {
        // Get cell contents    
        aDownloadData = iStatement.ColumnInt64( 0 );
        aUploadData = iStatement.ColumnInt64( 1 );
        aDownloadTime = iStatement.ColumnInt64( 2 );
        aUploadTime = iStatement.ColumnInt64( 3 );  
        }        
    iStatement.Reset();                
    iStatement.Close();               
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryDeletedItemsListsL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryDeletedItemsListsL( 
    RPointerArray<CCmBaseListItem>& aArray, TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    QueryDeletedItemsListsL()"));

    TPtrC temp16;
    iStatement.BindInt( 0, aStatus );        
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmBaseListItem* item = CCmBaseListItem::NewLC();
        /**
        Read rows into package
        */
        iStatement.ColumnText( 0, temp16 );
        item->SetPrimaryTextL( temp16 );
        iStatement.ColumnText( 1, temp16 );
        item->SetSecondaryTextL( temp16 );
        iStatement.ColumnText( 2, temp16 );
        item->SetPathL( temp16 );                          
        item->SetSize( iStatement.ColumnInt64( 3 ) );
        item->SetStatus(
            (TCmListItemStatus)iStatement.ColumnInt( 4 ) );
        item->SetDate(iStatement.ColumnInt64( 5 ) );
        item->SetDbId( iStatement.ColumnInt64( 6 ) );
        aArray.Append( item );
        CleanupStack::Pop( item );
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement        
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::QueryDrivesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::QueryDrivesL( 
    RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::QueryDrivesL()"));
    
    TPtrC temp16;       
    while( iStatement.Next() == KSqlAtRow )
        {
        CCmDriveInfo* drive = CCmDriveInfo::NewLC();
        /**
        Read rows into package
        */
        drive->SetDriveNumber( iStatement.ColumnInt( 0 ) );
        drive->SetDriveType( iStatement.ColumnInt( 1 ) );
        iStatement.ColumnText( 2, temp16 );
        if ( temp16.Length() != 0 ) 
            {
            drive->SetDriveNameL( temp16 );
            }
        else 
            {
            drive->SetDriveNameL( KNullDesC() );
            }           
        drive->SetDriveSize( iStatement.ColumnInt64( 3 ) );
        drive->SetDriveQuota( iStatement.ColumnInt64( 4 ) );
        drive->SetUsedDriveQuota( iStatement.ColumnInt64( 5 ) );
        drive->SetDriveId( iStatement.ColumnInt64( 6 ) );
        drive->SetStatus( (TBool)iStatement.ColumnInt( 7 ) );
        aDrives.AppendL( drive );
        CleanupStack::Pop( drive ); 
        }   
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement     
    }
      
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetAvgImageShrinkTime
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetAvgImageShrinkTime()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetAvgImageShrinkTime()"));
    
    TInt err( iStatement.Prepare( iDatabase, KImageShrinkingInfo ) );
    TInt64 count( KErrNone );
    TInt64 time( KErrNone );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            KImageShrinkingInfo illegal err = %d"), err ));
        }
    else
        {
        // Loop only one row
        err = iStatement.Next();
        if( err == KSqlAtRow )  
            {
            // Get cell contents    
            count = iStatement.ColumnInt64( 0 );
            time = iStatement.ColumnInt64( 1 );                   
            }        
        }
        
    iStatement.Reset();                
    iStatement.Close();
    if( count != KErrNone )
        {
        return TInt(time/count);
        }
    else
        {
        return KErrNone;
        }
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetFillListNameL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::GetFillListNameL( HBufC8*& aListName, 
    const TInt64 aListId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetFillListNameL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KSelectFillRuleName ) );
    if( err )
        {
        User::Leave( err );
        }
    else
        {
        iStatement.BindInt64( 0, aListId );
        // Loop only one row
        err = iStatement.Next();
        if( err == KSqlAtRow )  
            {
            TPtrC8 temp;
            iStatement.ColumnBinary( 0, temp );
            aListName = temp.Alloc();
            }        
        }
    iStatement.Reset();                
    iStatement.Close();   
    }
                
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteMediaServer
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::DeleteMediaServer( const TDesC8& aUDN )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteMediaServer()"));
    
    TInt err( iStatement.Prepare( iDatabase, KRemoveMediaServersByUDN ) );
    if( !err )
        {    
        iStatement.BindBinary( 0, aUDN );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        }

    // In error cases for return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone );    
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteFillRuleByName
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::DeleteFillRuleByName( const TDesC8& aName )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteFillRuleByName()"));
    
    // Remove ruleparams
    TInt err( iStatement.Prepare( iDatabase, KRemoveRuleParamsByRuleID) );
    if( !err )
        {
        iStatement.BindBinary( 0, aName );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close(); 

        // Remove rules
        err = iStatement.Prepare( iDatabase, KRemoveRuleByRuleGroupID);
        iStatement.BindBinary( 0, aName );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();     

        // Remove Mediaservers from the mapping table
        err = iStatement.Prepare( iDatabase, KRemoveRuleGroupServers);
        iStatement.BindBinary( 0, aName );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();

        // Remove FillRules
        err = iStatement.Prepare( iDatabase, KRemoveFillRuleByName );
        iStatement.BindBinary( 0, aName );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        }
    
    // In error cases for return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone );         
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteAllFillRules
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::DeleteAllFillRules()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteAllFillRules()"));
    
    // Remove ruleparams
    TInt err( iStatement.Prepare( iDatabase, KRemoveRuleParamAll ) );
    if( !err )
        {
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close(); 

        // Remove rules
        err = iStatement.Prepare( iDatabase, KRemoveRuleAll );
        if( !err )
            {
            iStatement.Exec();
            iStatement.Reset();
            iStatement.Close();     

            // Remove Mediaservers from the mapping table
            err = iStatement.Prepare( iDatabase, KRemoveRuleGroupServerAll);
            if( !err )
                {
                iStatement.Exec();
                iStatement.Reset();
                iStatement.Close();

                // Remove FillRules
                err = iStatement.Prepare( iDatabase, KRemoveFillRuleAll );
                if( !err )
                    {
                    iStatement.Exec();
                    iStatement.Reset();
                    iStatement.Close();
                    }
                }
            }
        iRuleGroupID = 1;
        iRuleID = 1;
        iRuleParamID = 1;             
        }

    // In error cases for return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone );          
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteAllStoreRules
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::DeleteAllStoreRules()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteAllStoreRules()"));
    
    // Remove ruleparams     
    TInt err = iStatement.Prepare( iDatabase, KRemoveStoreRuleServers );
    if( !err )
        {
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();

        // Remove rules
        err = iStatement.Prepare( iDatabase, KRemoveStoreRuleParams );
        if( !err )
            {
            iStatement.Exec();
            iStatement.Reset();
            iStatement.Close();
            // Remove Mediaservers from the mapping table
            err = iStatement.Prepare( iDatabase, KRemoveStoreRules );
            if( !err )
                {
                iStatement.Exec();
                iStatement.Reset();
                iStatement.Close();
                }
            else
                {
                err = KErrGeneral;
                }
            }
        else
            {
            err = KErrGeneral;
            }
        }

    iStoreRuleID = 1;
    iStoreRuleParamID = 1;                
  
    return err;         
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteFillFiles
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::DeleteFillFiles()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteFillFiles()"));
    
    return iDatabase.Exec( KRemoveFillFiles );
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteStoreFiles
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::DeleteStoreFiles()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteStoreFiles()"));
    
    iDatabase.Exec( KRemoveStoreFiles );    
    TInt err = iDatabase.Exec( KRemoveStoreFileServers );
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteDeletedItemsL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::DeleteDeletedItemsL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteDeletedItemsL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KRemoveDeletedFillFiles ) );
    if( !err )
        {    
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close(); 
        }
    else
        {
        User::Leave( err );
        }   
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteDeletedMediaServersL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::DeleteDeletedMediaServersL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    DeleteDeletedMediaServersL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KRemoveDeletedMediaServers ) );
    if( !err )
        {    
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close(); 
        }
    else
        {
        User::Leave( err );
        }   
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteHashValuesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::DeleteHashValuesL( const TInt aMediaserverId,
    const TInt aSearchIndex )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteHashValuesL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KRemoveHash ) );    
    if( !err )
        {
        iStatement.BindInt( 0, aMediaserverId );
        iStatement.BindInt( 1, aSearchIndex );
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        }
    else
        {
        User::Leave( err );
        }            
    }
                
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateMediaServerInfo
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::UpdateMediaServerInfo( 
    CCmMediaServerFull* aMediaServer )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateMediaServerInfo()"));
    
    // Remove ruleparams
    TInt err( iStatement.Prepare( iDatabase, KUpdateMediaServerInfo ) );
    if( !err )
        {    
        iStatement.BindBinary( 0, aMediaServer->MediaServerName() );
        iStatement.BindInt( 1, aMediaServer->SystemUpdateID() );
        iStatement.BindInt64( 2, aMediaServer->VisibleDate().Int64() );
        iStatement.BindBinary( 3, aMediaServer->MediaServer() );    
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();
        }
  
    // In error cases for return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone ); 
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateFillRuleSelected
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateFillRuleSelected( 
    const TDesC8& aName, 
    TInt aSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateFillRuleSelected()"));
    
    // Update Fill Rule
    TInt err( iStatement.Prepare( iDatabase, KUpdateFillRuleSelected ) );
    if( !err )
        {
        iStatement.BindInt( 0, aSelected );
        iStatement.BindBinary( 1, aName );    
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();        
        }    
        
    // In error cases for return value to KErrGeneral
    return ( err ? KErrGeneral : KErrNone ); 
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateFillRuleGroupInfo
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateFillRuleGroupInfo()
	{
	LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
	UpdateFillRuleGroupInfo()"));
	
	TInt err = iDatabase.Exec( KSetNullRealSizes );
	return err;
	}

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateDownloadHistory
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateDownloadHistory( TUint aMediaServerId,
    TInt64& aDownloadData, TInt64& aDownloadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateDownloadHistory()"));
    
    TInt err( iStatement.Prepare( iDatabase, KUpdateDownloadHistory ) );
    if( !err )
        {
        iStatement.BindInt64( 0, aDownloadData );
        iStatement.BindInt64( 1, aDownloadTime );
        iStatement.BindInt( 2, aMediaServerId );
        err = iStatement.Exec();
        iStatement.Reset(  );
        iStatement.Close();         
        }
    return err;        
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateUploadHistory
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateUploadHistory( TUint aMediaServerId,
    TInt64& aUploadData, TInt64& aUploadTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateUploadHistory()"));
    
    TInt err( iStatement.Prepare( iDatabase, KUpdateUploadHistory ) );
    if( !err )
        {
        iStatement.BindInt64( 0, aUploadData );
        iStatement.BindInt64( 1, aUploadTime );
        iStatement.BindInt( 2, aMediaServerId );
        err = iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();         
        }
    return err;    
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateStatusValues
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateStatusValues( 
    RPointerArray<CCmBaseListItem>& aStatusValues )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateStatusValues()"));
    
    TInt err( KErrNone );
    iDatabase.Exec( KCmDbBeginTransaction );    
    err = iStatement.Prepare( iDatabase, KUpdateFileStatus );    
    if( !err ) 
        {
        for( TInt i = 0; i < aStatusValues.Count(); i++ )
            {        
            iStatement.BindInt64( 0, aStatusValues[i]->Status() );
            iStatement.BindInt64( 1, aStatusValues[i]->DbId() );          
            iStatement.BindInt64( 2, aStatusValues[i]->DbId() );
            iStatement.Exec();
            iStatement.Reset();             
            }        
        }
    iStatement.Close();        
    iDatabase.Exec( KCmDbSqlCommit );
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateFillListDriveStatusesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::UpdateFillListDriveStatusesL( 
    CCmDriveInfo* aOrigDrive, CCmDriveInfo* aNewDrive, 
    TBool aDriveSelected )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    UpdateFillListDriveStatusesL()"));
    
    if( aDriveSelected )
        {
        // Update only drive number and drive id
        TInt err( iStatement.Prepare( iDatabase, KUpdateFillDriveInfo ) );
        User::LeaveIfError( err );
        iStatement.BindInt( 0, aNewDrive->DriveNumber() );
        iStatement.BindInt64( 1, aNewDrive->DriveId() );
        iStatement.BindInt64( 2, aOrigDrive->DriveId() );       
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();                 
        }
    else
        {
        // Set path, drive number, drive id, quota, size to null and
        // update fill status to orginal
        TInt err( iStatement.Prepare( iDatabase, 
            KUpdateFillDriveAndStatusInfo ) );
        User::LeaveIfError( err );
        iStatement.BindText( 0, KNullDesC() );
        iStatement.BindInt( 1, 0 );
        iStatement.BindInt64( 2, 0 );
        iStatement.BindInt( 3, (TInt)ECmToBeFilled );
        iStatement.BindInt64( 4, aOrigDrive->DriveId() );       
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();                          
        }
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateFillListItemPriority
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::UpdateFillListItemPriority( 
    CCmFillRuleContainer* aContainer )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    UpdateFillListItemPriority()"));
    
    iDatabase.Exec( KCmDbBeginTransaction );    
    TInt err( iStatement.Prepare( iDatabase, KUpdateFillItemPriorities ) );
    if ( !err )
        {
        for( TInt i = 0; i < aContainer->FillRuleCount(); i++ )    
            {        
            iStatement.BindInt( 0, aContainer->FillRule( i )->Priority() );
            iStatement.BindInt( 1, aContainer->FillRule( i )->Selected() );
            iStatement.BindBinary( 2, aContainer->FillRule( i )->Name() );
            iStatement.Exec();
            iStatement.Reset();                
            }      
        }
    iStatement.Close();        
    iDatabase.Exec( KCmDbSqlCommit );
    
    //update fill item's status 
    iDatabase.Exec( KCmDbBeginTransaction );    
    TInt err1( iStatement.Prepare( 
        iDatabase, KUpdateFillItemStatusByRuleName ) );
    if ( !err1 )
        {
        for( TInt i = 0; i < aContainer->FillRuleCount(); i++ )    
            {        
            iStatement.BindInt( 0, aContainer->FillRule( i )->Status() );
            iStatement.BindBinary( 1, aContainer->FillRule( i )->Name() );
            iStatement.Exec();
            iStatement.Reset();            
            }                        
        }
    iStatement.Close();
    iDatabase.Exec( KCmDbSqlCommit );
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetFillListStateL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::SetFillListStateL( const TUint aListId, 
    TCmFillRuleStatus aState )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetFillListStateL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KUpdateSelectionStatus ) );
    if( err )
        {
        User::Leave( err );
        }
    else
        {
        iStatement.BindInt( 0, aState );   
        iStatement.BindInt( 1, aListId );
        iStatement.Exec();
        iStatement.Reset();   
        iStatement.Close();        
        }         
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::RestoreFileStatusesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::RestoreFileStatusesL( 
    const RPointerArray<CCmFillListItem>& aItems, 
    const TDesC8& aListName )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::RestoreFileStatusesL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KGetFillListStatus ) );
    TRACE( Print( _L("[DB MNGR]\t KGetFillListStatus err = %d "), 
        err ) );    
    
    TUint status( KErrNone );
        
    if( err )
        {
        User::Leave( err ); 
        }        
    else
        {
        iStatement.BindBinary( 0, aListName );
        err = iStatement.Next();
        if ( err == KSqlAtRow )  
            {
            status = iStatement.ColumnInt( 0 );
            }
        }
    TRACE( Print( _L("[DB MNGR]\t Status to be updated = %d "), 
        status ) );        
    iStatement.Reset(); 
    iStatement.Close();
    DoRestoreFileStatusesL( aItems, status );   
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DoRestoreFileStatusesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::DoRestoreFileStatusesL( 
                        const RPointerArray<CCmFillListItem>& aItems, 
                        const TUint aStatus )
    {
    
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DoRestoreFileStatusesL"));
    
    iDatabase.Exec( KCmDbBeginTransaction );    
    TInt err( iStatement.Prepare( iDatabase, KRestoreFileStatus ) );    
    
    if( err )
        {
        User::Leave( err );
        }
        
    for( TInt i = 0; i < aItems.Count(); i++ )
        {            
        iStatement.BindInt( 0, aStatus );
        iStatement.BindInt64( 1, aItems[i]->DbId() );
        iStatement.BindInt64( 2, aItems[i]->DbId() );
        iStatement.Exec();
        iStatement.Reset();           
        }
        
    iStatement.Close();        
    iDatabase.Exec( KCmDbSqlCommit );
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetRefIdsToZero
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::SetRefIdsToZero()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::SetRefIdsToZero()"));
    
    iDatabase.Exec( KUpdateFillItemRef_Zero );        
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::IncrementShrinkTimeL
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::IncrementShrinkTimeL( TInt aImageCount, 
    TInt aMilliSeconds )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::IncrementShrinkTimeL()"));
    
    TInt err( iStatement.Prepare( iDatabase, KIncrementImageShrinkingInfo ) );
    if( err )
        {
        User::Leave( err );
        }
    else
        {
        iStatement.BindInt64( 0, aImageCount );
        iStatement.BindInt64( 1, aMilliSeconds );
        err = iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();        
        }        
    return err;  
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::FillListSize
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::FillListSize( const TDesC8& aListName, 
                                             TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::FillListSize()"));
    
    TInt err( iStatement.Prepare( iDatabase, KFillListSize ) );
    TInt64 size(KErrNone);
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            FillListSize illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aStatus );
        iStatement.BindBinary( 1, aListName );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            size = iStatement.ColumnInt64( 0 );             
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return size;          
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::StoreListSize
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::StoreListSize( 
    const TDesC8& aListName, 
    TUint /*aStatus*/ )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::StoreListSize()"));
    
    TInt err( iStatement.Prepare( iDatabase, KStoreListSize ) );
    TInt64 size(KErrNone);
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            StoreListSize illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindBinary( 0, aListName );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            size = iStatement.ColumnInt64( 0 );
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return size;      
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::StoreFileCount
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::StoreFileCount( TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::StoreFileCount()"));
    
    TInt err( iStatement.Prepare( iDatabase, KStoreFileCount ) );
    TInt count(KErrNone);
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            Illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aStatus );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            count = iStatement.ColumnInt( 0 );               
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return count;  
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::StoreFileCountNoDuplicates
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::StoreFileCountNoDuplicates()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    StoreFileCountNoDuplicates()"));
    
    TInt err( iStatement.Prepare( iDatabase, KStoreFileCountNoDuplicates ) );
    TInt count(KErrNone);
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            Illegal err = %d"), err ));
        }
    else
        {
        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() ) 
            {
            // Get cell contents    
            count = iStatement.ColumnInt( 0 );                
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return count;  
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::FillFileCount
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::FillFileCount( TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::FillFileCount()"));
    
    TInt err( iStatement.Prepare( iDatabase, KFillFileCount ) );
    TInt count(KErrNone);
    if( err )
        {
        // Do nothing
        }
    else
        {
        iStatement.BindInt( 0, aStatus );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            count = iStatement.ColumnInt( 0 );             
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return count;      
    }

// ---------------------------------------------------------------------------
// Returns size of the filled items
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::BytesFilled( TUint aID )
    {
    TInt err( iStatement.Prepare( iDatabase, KDataAmountFilled ) );
    TInt64 size(KErrNone);
    if( err )
        {
        return size;
        }
    else
        {
        iStatement.BindInt( 0, aID );

        for ( err = iStatement.Next();
              err == KSqlAtRow;
              err = iStatement.Next() )
            {
            // Get cell contents
            size = iStatement.ColumnInt64( 0 );
            }
        }
    iStatement.Reset();
    iStatement.Close();  // Destruct statement
    return size;
    }

// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::KBytesToBeFilled( TUint8 aMediaServerId, 
    TUint aStatus )    
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::KBytesToBeFilled()"));
    
    TInt err( iStatement.Prepare( iDatabase, KDataAmountToBeFilled ) );
    TInt64 size(KErrNone);
    if( err )
        {
        return size;
        }
    else
        {
        iStatement.BindInt( 0, aStatus );
        iStatement.BindInt( 1, aMediaServerId );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            size = iStatement.ColumnInt64( 0 );             
            }         
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return (size/1000);     
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::KBytesToBeStored
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::KBytesToBeStored( TUint8 aMediaServerId, 
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::KBytesToBeStored()"));
    
    TInt err( iStatement.Prepare( iDatabase, KDataAmountToBeStored ) );
    TInt64 size(KErrNone);
    if( !err )
        {
        iStatement.BindInt( 0, aMediaServerId );
        iStatement.BindInt( 1, aStatus );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            size = iStatement.ColumnInt64( 0 );             
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return (size/1000); 
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateTransferInfo
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::UpdateTransferInfo( TCmService aService, 
    TInt aCount, TInt aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::UpdateTransferInfo()"));
    
    TInt err( iStatement.Prepare( iDatabase, KInitTransferInfo ) );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            KInitTransferInfo illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aService );
        err = iStatement.Exec();                  
        }
    iStatement.Reset();
    iStatement.Close();         
    err = iStatement.Prepare( iDatabase, KUpdateTransferInfo );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            KUpdateTransferInfo illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aCount );
        iStatement.BindInt( 1, aTime );
        iStatement.BindInt( 2, aService );
        err = iStatement.Exec();         
        }
    iStatement.Reset();
    iStatement.Close();                    
    return err;          
    }   

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateStoreTransferStatus
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::UpdateStoreTransferStatus( TUint aStatus,
    TInt64 aFid, TInt64 aMsId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    UpdateStoreTransferStatus()"));
    
    TInt err( iStatement.Prepare( iDatabase, KUpdateStoreTransferStatus ));
    if( err )
        {
        
        }
    else
        {
        iStatement.BindInt( 0, aStatus );
        iStatement.BindInt64( 1, aFid );
        iStatement.BindInt64( 2, aMsId );
        err = iStatement.Exec();
        }
    iStatement.Reset();
    iStatement.Close();                
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::UpdateFillTransferStatus
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::UpdateFillTransferStatus( TUint aStatus, 
    CCmFillListItem* aItem )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    UpdateFillTransferStatus()"));
    
    TInt err( iStatement.Prepare( iDatabase, KUpdateFillTransferStatus ));
    if( err )
        {
        LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
        UpdateFillTransferStatus() error preparing statement"));       
        }
    else
        {
        iStatement.BindInt( 0, aStatus );
        iStatement.BindText( 1, aItem->Path() );
        iStatement.BindInt64( 2, aItem->DriveId() );
        iStatement.BindInt( 3, aItem->DriveNumber() );
        iStatement.BindInt64( 4, aItem->DevId() );
        iStatement.BindInt64( 5, aItem->DbId() );
        iStatement.BindInt64( 6, aItem->ListId() );
        iStatement.BindInt64( 7, aItem->DbId() );
        err = iStatement.Exec();
        }
    iStatement.Reset();
    iStatement.Close();      
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetTransferInfo
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::GetTransferInfo( TCmService aService, 
    TInt& aCount, TInt& aTime )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetTransferInfo()"));
    
    TInt err( iStatement.Prepare( iDatabase, KTranferValues ) );
    aCount = 0;
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            KTranferValues illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aService );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            aCount = iStatement.ColumnInt( 0 );
            err = KErrNone;                   
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    err = iStatement.Prepare( iDatabase, KTranferTime );
    aTime = 0;
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            KTranferTime illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aService );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            aTime = iStatement.ColumnInt( 0 );
            err = KErrNone;                  
            }        
        }
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement            
    return err;         
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetStoreFileCount
// ---------------------------------------------------------------------------
//    
TInt CCmDmSQLiteConnection::GetStoreFileCount( 
    const TDesC8& aListName, 
    TUint /*aStatus*/, 
    TInt& aCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetStoreFileCount()"));
    
    TInt err( iStatement.Prepare( iDatabase, KStoreListFileCount ) );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            GetStoreFileCount illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindBinary( 0, aListName );

        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() )
            {
            // Get cell contents    
            aCount = iStatement.ColumnInt( 0 );
            err = KErrNone;             
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return err;  
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::GetFillFileCount
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::GetFillFileCount( 
    const TDesC8& aListName, 
    TUint aStatus, 
    TInt& aCount )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::GetFillFileCount()"));
    
    TInt err( iStatement.Prepare( iDatabase, KFillListFileCount ) );
    if( err )
        {
        TRACE(Print(_L("[DATABASE MANAGER]\t \
            GetFillFileCount illegal err = %d"), err ));
        }
    else
        {
        iStatement.BindInt( 0, aStatus );        
        iStatement.BindBinary( 1, aListName );
        for ( err = iStatement.Next(); 
              err == KSqlAtRow; 
              err = iStatement.Next() ) 
            {
            // Get cell contents    
            aCount = iStatement.ColumnInt64( 0 );
            err = KErrNone;                               
            }        
        }    
    iStatement.Reset();            
    iStatement.Close();  // Destruct statement
    return err;  
    }

// ---------------------------------------------------------------------------
// CmDmSQLiteConnection::GetStoreFileCountNoDuplicatesL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::GetStoreFileCountNoDuplicatesL( TInt& aCount,
    TUint aStatus )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    GetStoreFileCountNoDuplicatesL()"));
    
    TInt err( iStatement.Prepare( iDatabase, 
        KGetStoreFileCountNoDuplicates ));
    if( err )
        {
        User::Leave( err );
        }
    else
        {
        iStatement.BindInt( 0, aStatus );
        if( KSqlAtRow == iStatement.Next() )
            {
            aCount = iStatement.ColumnInt( 0 );
            }
        iStatement.Reset();
        iStatement.Close();    
        }        
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::SetFillListRealCountAndSizeL
// ---------------------------------------------------------------------------
//
void CCmDmSQLiteConnection::SetFillListRealCountAndSizeL( TInt64 aListId,
    TInt64 aRealCount, TInt64 aRealSize )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    SetFillListRealCountAndSizeL()"));
    
    // Update Fill Rule
    TInt err = iStatement.Prepare( iDatabase, 
                                   KUpdateFillRuleRealInformation );
    if( !err )
        {
        iStatement.BindInt64( 0, aRealCount );
        iStatement.BindInt64( 1, aRealSize );
        iStatement.BindInt64( 2, aListId );    
        iStatement.Exec();
        iStatement.Reset();
        iStatement.Close();     
        }
    else
        {
        User::Leave( err );
        }        
    }
    
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteInActiveRuleServers
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::DeleteInActiveRuleServers()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    DeleteInActiveRuleServers()"));
    
    TInt err( KErrNone );
    err = iDatabase.Exec( KRemoveInActiveFillRuleServers );
    if( err >= KErrNone )
        {
        err = iDatabase.Exec( KRemoveInActiveStoreRuleServers );
        }
    if( err >= KErrNone )
        {
        err = KErrNone;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteInActiveFiles
// ---------------------------------------------------------------------------
//
TInt CCmDmSQLiteConnection::DeleteInActiveFiles()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::DeleteInActiveFiles()"));
    
    TInt err( KErrNone );
    iDatabase.Exec( KRemoveInActiveFillFiles );
    iDatabase.Exec( KRemoveInActiveStoreFilesServers );
    err = iDatabase.Exec( KRemoveInActiveStoreFiles );
    if( err >= KErrNone )
        {
        err = KErrNone;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::DeleteItemsByFillListId
// ---------------------------------------------------------------------------
//   
TInt CCmDmSQLiteConnection::DeleteItemsByFillListId( TInt aFillListId )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::\
    DeleteItemsByFillListId()"));
    
    TInt err( KErrNone );
    err = iStatement.Prepare( iDatabase, KRemoveFillListItems );
    iStatement.BindInt( 0, aFillListId );
    iStatement.Exec();
    iStatement.Reset();
    iStatement.Close();   
    return err;
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::RuleId
// ---------------------------------------------------------------------------
//
TInt64 CCmDmSQLiteConnection::RuleId( 
    const TDesC8& aName, 
    TCmRuleType aType )
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::RuleId()"));
    
    TInt64 ret( KErrNone );
    // Fill rule id        
    if( ECmFmFillRule == aType )
        {
        ret = iStatement.Prepare( iDatabase, KSelectRuleGroupID );        
        }
    // Store rule id        
    else
        {
        ret = iStatement.Prepare( iDatabase, KSelectStoreRuleID );        
        }
    if( ret )
        {
        ret = KErrGeneral;
        }
    else
        {
        iStatement.BindBinary( 0, aName );                
        // Loop only one row in results
        ret = iStatement.Next();
        if ( ret == KSqlAtRow )  
            {
            // Get cell contents   
            ret = iStatement.ColumnInt64( 0 );        
            }    
        }
    iStatement.Reset();                
    iStatement.Close();  // Destruct statement            
    TRACE(Print(_L("[DATABASE MANAGER]\t CCmDmSQLiteConnection::RuleId()\
         end")));
    return ret;        
    }
        
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::CCmDmSQLiteConnection
// ---------------------------------------------------------------------------
//    
CCmDmSQLiteConnection::CCmDmSQLiteConnection()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::CCmDmSQLiteConnection()"));
    }
 
// ---------------------------------------------------------------------------
// CCmDmSQLiteConnection::ConstructL
// ---------------------------------------------------------------------------
//   
void CCmDmSQLiteConnection::ConstructL()
    {
    LOG( _L( "[DB MNGR]\t CCmDmSQLiteConnection::ConstructL()"));   
    }    

// End of file
