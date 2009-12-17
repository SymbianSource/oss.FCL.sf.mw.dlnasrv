/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating Media servers
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>

#include "cmsearchresponsehash.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSearchResponseHash* CCmSearchResponseHash::NewL( TInt aStartIndex,
                                                             TInt aItemCount,
                                                             const TDesC8& aHash )
    {    
    CCmSearchResponseHash* self = new ( ELeave ) CCmSearchResponseHash( );
    CleanupStack::PushL( self );
    self->ConstructL( aStartIndex, aItemCount, aHash );
    CleanupStack::Pop( self );  
    return self;  
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSearchResponseHash* CCmSearchResponseHash::NewL()
    {    
    CCmSearchResponseHash* self = new ( ELeave ) CCmSearchResponseHash();
    return self;  
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCmSearchResponseHash::CCmSearchResponseHash()
    {  
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmSearchResponseHash::~CCmSearchResponseHash()
    {
    delete iHash;  
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCmSearchResponseHash::ConstructL( TInt aStartIndex,
                                        TInt aItemCount,
                                        const TDesC8& aHash )
    {    
    SetStartIndex( aStartIndex );
    SetItemCount( aItemCount );
    SetHashL( aHash );
    }
    
// ---------------------------------------------------------------------------
// SetHashL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSearchResponseHash::SetHashL( const TDesC8& aHash )
    {    
    delete iHash;
    iHash = NULL;
    iHash = aHash.AllocL();
    }
        
// ---------------------------------------------------------------------------
// Hash
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CCmSearchResponseHash::Hash() const
    {    
    if ( iHash )
        {
        return *iHash;
        }
    else
        {
        return KNullDesC8();
        }
    }

// ---------------------------------------------------------------------------
// SetStartIndex
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSearchResponseHash::SetStartIndex( TInt aStartIndex )
    {
    iStartIndex = aStartIndex;    
    }

// ---------------------------------------------------------------------------
// StartIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSearchResponseHash::StartIndex() const
    {
    return iStartIndex; 
    }

// ---------------------------------------------------------------------------
// SetItemCount
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSearchResponseHash::SetItemCount( TInt aItemCount )
    {
    iItemCount = aItemCount;    
    }

// ---------------------------------------------------------------------------
// ItemCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSearchResponseHash::ItemCount() const
    {
    return iItemCount; 
    }


// -----------------------------------------------------------------------------
// ExternalizeL
// -----------------------------------------------------------------------------
//
void CCmSearchResponseHash::ExternalizeL( RWriteStream& aStream ) const
    {      
    aStream.WriteInt32L( Hash().Length() );
    aStream << Hash();
    aStream.WriteInt32L( StartIndex() );
    aStream.WriteInt32L( ItemCount() );
    }
        
        
// -----------------------------------------------------------------------------
// InternalizeL
// -----------------------------------------------------------------------------
//
void CCmSearchResponseHash::InternalizeL( RReadStream& aStream )
    {
    delete iHash; iHash = NULL;
    TInt bufLength = aStream.ReadInt32L();    
    iHash = HBufC8::NewL( aStream, bufLength );
    
    iStartIndex = aStream.ReadInt32L();
    iItemCount = aStream.ReadInt32L();
    }
    
// End of file
