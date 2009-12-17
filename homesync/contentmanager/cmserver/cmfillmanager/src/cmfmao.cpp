/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fill rule and filled data processing Active object
*
*/


#include "cmfmfillrulefilleddatamngr.h"
#include "cmfmao.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmFmAo::NewL
// ---------------------------------------------------------------------------
//
CCmFmAo* CCmFmAo::NewL( CCmFmFillRuleFilledDataMngr& aMngr )
    {
    LOG(_L("[FILL MNGR]\t CCmFmAo::NewL() start"));    
    CCmFmAo* self = CCmFmAo::NewLC( aMngr );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmAo::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmAo::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmAo* CCmFmAo::NewLC( CCmFmFillRuleFilledDataMngr& aMngr )
    {
    LOG(_L("[FILL MNGR]\t CCmFmAo::NewLC() start"));     
    CCmFmAo* self = new ( ELeave ) CCmFmAo( aMngr );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmAo::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmAo::~CCmFmAo
// ---------------------------------------------------------------------------
//
CCmFmAo::~CCmFmAo()
    {
    LOG(_L("[FILL MNGR]\t CCmFmAo::~CCmFmAo() start"));
    Cancel();   
    LOG(_L("[FILL MNGR]\t CCmFmAo::~CCmFmAo() end"));
    }

// ---------------------------------------------------------------------------
// CCmFmAo::CCmFmAo
// ---------------------------------------------------------------------------
//            
CCmFmAo::CCmFmAo( CCmFmFillRuleFilledDataMngr& aMngr )
    :CActive( EPriorityStandard ), iMngr( aMngr ), iIdle( ETrue )
    {
    LOG(_L("[FILL MNGR]\t CCmFmAo::CCmFmAo()"));
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmFmAo::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmFmAo::ConstructL()
    { 
    LOG(_L("[FILL MNGR]\t CCmFmAo::ConstructL()"));                   
    }

// ---------------------------------------------------------------------------
// CCmFmAo::RunL
// ---------------------------------------------------------------------------
// 
void CCmFmAo::RunL()
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmAo::RunL status %d"),iStatus.Int() ));            
    if ( iIdle )
        {
        LOG(_L("[FILL MNGR]\t CCmFmAo::RunL iIdle == ETrue "));
        SetActive();
        iIdle = EFalse;
        }
    else 
        {
        LOG(_L("[FILL MNGR]\t CCmFmAo::RunL iIdle == EFalse "));
        iMngr.OperationCompletedL( iStatus.Int() );
        iIdle = ETrue;
        }
    }    

// ---------------------------------------------------------------------------
// CCmFmAo::DoCancel
// ---------------------------------------------------------------------------
//     
void CCmFmAo::DoCancel()
    {
    LOG(_L("[FILL MNGR]\t CCmFmAo::DoCancel"));
    }    

// ---------------------------------------------------------------------------
// CCmFmAo::RunError( TInt aError )
// ---------------------------------------------------------------------------
//  
TInt CCmFmAo::RunError( TInt aError )
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmAo::RunError %d"), aError ));
    return aError;
    }
    
// End of file


