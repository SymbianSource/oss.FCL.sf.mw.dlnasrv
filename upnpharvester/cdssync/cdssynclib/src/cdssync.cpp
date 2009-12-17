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
* Description:      Interface class implementation of CdsSync
*
*/






#include "cdssync.h"
#include "cdssyncimplsql.h"


// --------------------------------------------------------------------------
// CCdsSync::CCdsSync
// --------------------------------------------------------------------------
//
CCdsSync::CCdsSync()
    {
    
    }
    
// --------------------------------------------------------------------------
// CCdsSync::ConstructL
// --------------------------------------------------------------------------
//
void CCdsSync::ConstructL()
    {
    iSyncImpl = CCdsSyncImpl::NewL();
    }


// --------------------------------------------------------------------------
// CCdsSync::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CCdsSync* CCdsSync::NewL()
    {
    CCdsSync* self = CCdsSync::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCdsSync::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CCdsSync* CCdsSync::NewLC()
    {
    CCdsSync* self = new( ELeave ) CCdsSync;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CCdsSync::~CCdsSync
// --------------------------------------------------------------------------
//
CCdsSync::~CCdsSync()
    {
    delete iSyncImpl;
    }
    
// --------------------------------------------------------------------------
// CCdsSync::InitL
// --------------------------------------------------------------------------
//
EXPORT_C void CCdsSync::InitL( RPointerArray<HBufC8>& aSourceDataArray,
                               const TInt& aDeviceId, 
                               MCdsSyncObserver& aObserver,
                               TInt aAddGranularity )
    {
    iSyncImpl->InitL( aSourceDataArray, 
                      aDeviceId, 
                      aObserver, 
                      aAddGranularity ); 
    }


// --------------------------------------------------------------------------
// CCdsSync::NotifySourceDataAddedL
// --------------------------------------------------------------------------
//
EXPORT_C void CCdsSync::NotifySourceDataAddedL( TBool aSourceDataComplete )
    {
    iSyncImpl->NotifySourceDataAddedL( aSourceDataComplete );
    }
   
// --------------------------------------------------------------------------
// CCdsSync::ResetL
// --------------------------------------------------------------------------
//
EXPORT_C void CCdsSync::ResetL()
    {
    iSyncImpl->ResetL();
    }

// --------------------------------------------------------------------------
// CCdsSync::ChunkCount
// --------------------------------------------------------------------------
//
EXPORT_C TInt CCdsSync::ChunkCount()
    {
    return iSyncImpl->ChunkCount();
    }
    
// --------------------------------------------------------------------------
// CCdsSync::ProcessedItemCount
// --------------------------------------------------------------------------
//
EXPORT_C TInt CCdsSync::ProcessedItemCount()
    {
    return iSyncImpl->ProcessedItemCount();
    }
    
// --------------------------------------------------------------------------
// CCdsSync::SetSearchIndex
// --------------------------------------------------------------------------
//
EXPORT_C void CCdsSync::SetSearchIndex( const TInt aSearchIndex )
    {
    iSyncImpl->SetSearchIndex( aSearchIndex );
    }        

// End of file
