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
* Description:      Cds Sync Active Object
*
*/






#include "mcmsqlmain.h"
#include "cmsqlbaseitem.h"
#include "cmsqlgenericitem.h"
#include "cdssyncsqlao.h"
#include "cdssyncimplsql.h"
#include "msdebug.h"

// CONSTANTS
const TInt KCdsSyncMaxImageItemCount = 15000;
const TInt KCdsSyncMaxVideoItemCount = 5000;
const TInt KCdsSyncMaxItemCount      = 40000;

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::NewL
// --------------------------------------------------------------------------
//
CCdsSyncSqlAo* CCdsSyncSqlAo::NewL( 
    MCmSqlMain& aSqlDb,
    CCdsSyncImpl& aSync,
    RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
    RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
    TInt aAddGranularity )
    {    
    CCdsSyncSqlAo* self = CCdsSyncSqlAo::NewLC(
        aSqlDb,
        aSync,
        aItemsToAdd,
        aItemsToDelete,
        aAddGranularity );
    CleanupStack::Pop( self );
    return self;
    }
 
// --------------------------------------------------------------------------
// CCdsSyncSqlAo::NewLC
// --------------------------------------------------------------------------
//    
CCdsSyncSqlAo* CCdsSyncSqlAo::NewLC( 
    MCmSqlMain& aSqlDb,
    CCdsSyncImpl& aSync,
    RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
    RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
    TInt aAddGranularity )
    {    
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::NewLC"));    
    CCdsSyncSqlAo* self = new (ELeave) CCdsSyncSqlAo( 
        aSqlDb, aSync, aItemsToAdd, aItemsToDelete, aAddGranularity  
        ); 
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;
    }    

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::~CCdsSyncSqlAo
// --------------------------------------------------------------------------
CCdsSyncSqlAo::~CCdsSyncSqlAo()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::~CCdsSyncSqlAo"));    
    
    if ( IsActive() )
        {
        Cancel();
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::CCdsSyncSqlAo
// --------------------------------------------------------------------------
CCdsSyncSqlAo::CCdsSyncSqlAo( MCmSqlMain& aSqlDb,
                              CCdsSyncImpl& aSync,
                              RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
                              RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
                              TInt aAddGranularity ) :
    CActive( CActive::EPriorityIdle ),
    iState( ECdsSyncSqlAoIdle ),
    iSqlDb( aSqlDb ),
    iSync( aSync ),
    iItemsToAdd( aItemsToAdd ),
    iItemsToDelete( aItemsToDelete ),
    iAddGranularity( aAddGranularity ),    
    iSourceDataComplete( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::ConstructL
// --------------------------------------------------------------------------
void CCdsSyncSqlAo::ConstructL() 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::ConstructL"));    
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::NotifyItemsAddedL
// --------------------------------------------------------------------------
void CCdsSyncSqlAo::NotifyItemsAddedL( TBool aSourceDataComplete )
    {
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::NotifyItemsAddedL"));

    // Source data is complete
    if ( aSourceDataComplete )
        {
        iSourceDataComplete = ETrue;
        }

    TRACE( Print( _L("[Cds Sync]\t addarray has %d items, srccomplete %d"), 
        iItemsToAdd.Count(), iSourceDataComplete ));
    
    if ( iState == ECdsSyncSqlAoIdle )
        {
        if ( iItemsToAdd.Count() >= iAddGranularity || 
           ( iSourceDataComplete && iItemsToAdd.Count() ) )
        
            {
            // add more items
            iStatus = KRequestPending;
            iItemsInAddition = iItemsToAdd.Count();

        #ifdef _DEBUG
            iOperationTime.HomeTime();
        #endif

            TRACE( Print( _L("[Cds Sync]\t starting to add %d items"),
                 iItemsInAddition ));
            iSqlDb.AsyncBatchAdd( iItemsToAdd, iStatus );
            SetActive();    
            iState = ECdsSyncSqlAoAdding;
            }
        else if ( iSourceDataComplete )
            {
            // all done, start deleting
       #ifdef _DEBUG
            iOperationTime.HomeTime();
       #endif
            iSqlDb.AsyncBatchDelete( iItemsToDelete, iStatus );
            SetActive();
            iState = ECdsSyncSqlAoDeleting;
           }
        }
    }

// ---------------------------------------------------------------------------
// CCdsSyncSqlAo::Activate()
// ---------------------------------------------------------------------------
void CCdsSyncSqlAo::Activate()
    {
    SetActive();
    }

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::IfOverLimitDeleteMediaItems() 
// --------------------------------------------------------------------------
void CCdsSyncSqlAo::IfOverLimitDeleteMediaItems() 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::IfOverLimitDeleteMediaItems START"));
    TInt64 musicCount( KErrNone );
    TInt64 videoCount( KErrNone );
    TInt64 imageCount( KErrNone );
    TRAP_IGNORE( iSqlDb.GetMediaCountL( musicCount, ECmAudio ) );
    TRAP_IGNORE( iSqlDb.GetMediaCountL( videoCount, ECmVideo ) );
    TRAP_IGNORE( iSqlDb.GetMediaCountL( imageCount, ECmImage ) );
    TRACE( Print( _L("[Cds Sync]\t Amout of music items %ld"), musicCount ));
    TRACE( Print( _L("[Cds Sync]\t Amout of image items %ld"), imageCount ));
    TRACE( Print( _L("[Cds Sync]\t Amout of video items %ld"), videoCount ));
    if( KCdsSyncMaxImageItemCount < imageCount )
        {
        TRAP_IGNORE( iSqlDb.DeleteOldestMediaItemsL( ECmImage, 
            imageCount - KCdsSyncMaxImageItemCount ) );
        TRACE( Print( _L("[Cds Sync]\t deleted %ld image items"), 
            ( imageCount - KCdsSyncMaxImageItemCount )));           
        TRAP_IGNORE( iSqlDb.GetMediaCountL( imageCount, ECmImage ) );
        }
    if( KCdsSyncMaxVideoItemCount < videoCount )
        {
        TRAP_IGNORE( iSqlDb.DeleteOldestMediaItemsL( ECmVideo, 
            videoCount - KCdsSyncMaxVideoItemCount ) );
        TRACE( Print( _L("[Cds Sync]\t deleted %ld video items"), 
            ( videoCount - KCdsSyncMaxVideoItemCount )));              
        TRAP_IGNORE( iSqlDb.GetMediaCountL( videoCount, ECmVideo ) );
        }
    if( KCdsSyncMaxItemCount < ( imageCount + videoCount + musicCount ) )
        {
        TRAP_IGNORE( iSqlDb.DeleteOldestMediaItemsL( ECmAudio, 
            ( imageCount + videoCount + musicCount ) - 
            KCdsSyncMaxItemCount ) );
        TRACE( Print( _L("[Cds Sync]\t deleted %ld music items"), 
            ( ( imageCount + videoCount + musicCount ) - 
            KCdsSyncMaxItemCount )));                     
        }
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::IfOverLimitDeleteMediaItems END"));
    }
    
// --------------------------------------------------------------------------
// CCdsSyncSqlAo::RunL() 
// --------------------------------------------------------------------------
void CCdsSyncSqlAo::RunL() 
    {
    TRACE( Print( _L("[Cds Sync]\t CCdsSyncSqlAo::RunL, iStatus %d"), 
        iStatus.Int() ));
    if ( iState == ECdsSyncSqlAoAdding )
        {
        iState = ECdsSyncSqlAoIdle;
        
    #ifdef _DEBUG
            
        TTime timeAfter;
        timeAfter.HomeTime();
        TRACE( Print( 
          _L("[Cds Sync]\t added %d, took %ld microsec, err %d"),
          iItemsToAdd.Count(), 
          timeAfter.MicroSecondsFrom( iOperationTime ).Int64(),
          iStatus.Int() ));
        TRACE( Print( _L("[Cds Sync]\t array has %d items"), 
           iItemsToAdd.Count() ));
        
    #endif
        
        TInt itemCount( iItemsToAdd.Count() );
        
        for ( TInt i = 0; i < itemCount; i++ )
            {
            delete iItemsToAdd[ 0 ];
            iItemsToAdd.Remove( 0 );
            }
        
        // check if there's more to add
        NotifyItemsAddedL();
        
        }
    else if ( iState == ECdsSyncSqlAoInitializing )
        {
        iSync.OperationsCompleteL( KErrNone );
        }
    else // ECdsSyncSqlAoDeleting
        {
    #ifdef _DEBUG
    
        TTime timeAfter;
        timeAfter.HomeTime();
        TRACE( Print( 
          _L("[Cds Sync]\t removed %d, took %ld microsec, err %d"),
          iItemsToDelete.Count(), 
          timeAfter.MicroSecondsFrom( iOperationTime ).Int64(),
          iStatus.Int() ));
    #endif
            
        iItemsToDelete.ResetAndDestroy();
        LOG(_L("[Cds Sync]\t Checking if too much media..."));
        IfOverLimitDeleteMediaItems();
        LOG(_L("[Cds Sync]\t Checked!"));
        LOG(_L("[Cds Sync]\t Deleting unused propertys"));
        iSqlDb.DeleteUnusedPropertys( );
        iState = ECdsSyncSqlAoIdle;
        iSync.OperationsCompleteL( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncSqlAo::DoCancel()
// --------------------------------------------------------------------------
void CCdsSyncSqlAo::DoCancel()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncSqlAo::DoCancel"));    
    }
