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
* Description:  Content Manager server's client ao implementation
*
*/


#include "cmactive.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmActive::NewL
// ---------------------------------------------------------------------------
CCmActive* CCmActive::NewL( RContentManager& aCm )
    {
    LOG(_L("[Cm Server]\t CCmActive::NewL"));
    CCmActive* self = CCmActive::NewLC( aCm );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCmActive::NewLC
// ---------------------------------------------------------------------------
CCmActive* CCmActive::NewLC( RContentManager& aCm )
    {
    LOG(_L("[Cm Server]\t CCmActive::NewL"));
    CCmActive* self = new ( ELeave ) CCmActive( aCm );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
        
// ---------------------------------------------------------------------------
// CCmActive::~CCmActive
// ---------------------------------------------------------------------------
CCmActive::~CCmActive()
    {
    LOG(_L("[Cm Server]\t CCmActive::~CCmActive"));
    }

// ---------------------------------------------------------------------------
// CCmActive::CCmActive
// ---------------------------------------------------------------------------
CCmActive::CCmActive( RContentManager& aCm )
    : CActive( EPriorityHigh ),
      iCm( aCm ), iIdle( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmActive::ConstructL
// ---------------------------------------------------------------------------
void CCmActive::ConstructL()
    {
    LOG(_L("[Cm Server]\t CCmActive::ConstructL"));
    }

// ---------------------------------------------------------------------------
// CCmActive::RunL
// ---------------------------------------------------------------------------
void CCmActive::RunL()
    {
    LOG(_L("[Cm Server]\t CCmActive::RunL"));
    TRACE(Print(_L("[Cm Server]\t status %d"), iStatus.Int() ));

    if ( iIdle )
        {
        LOG(_L("[Cm Server]\t CCmActive::RunL set active"));
        SetActive();
        iIdle = EFalse;
        }
    else
        {
        LOG(_L("[Cm Server]\t CCmActive::RunL operation completed"));
        iCm.OperationCompletedL( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CCmActive::DoCancel
// ---------------------------------------------------------------------------
void CCmActive::DoCancel()
    {
    LOG(_L("[Cm Server]\t CCmActive::DoCancel"));
    TRAPD( error, iCm.OperationCompletedL( KErrCancel ) );
    if ( error )
        {
        TRACE(Print(_L("[Cm Server]\tCCmActive::DoCancel \
        error = %d"), error ));
        }
    }

// ---------------------------------------------------------------------------
// CCmActive::RunError
// ---------------------------------------------------------------------------
TInt CCmActive::RunError( TInt aError )
    {
    TRACE( Print(_L("[Cm Server]\t CCmActive::RunError \
    error = %d"), aError ));

    // let it leave
    return aError;
    }

// End of file
