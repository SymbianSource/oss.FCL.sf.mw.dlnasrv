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
* Description:      Capsulating sql items
*
*/






#include <e32std.h>
#include "cmsqlbaseitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlBaseItem* CCmSqlBaseItem::NewL()
    {   
    CCmSqlBaseItem* self = CCmSqlBaseItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlBaseItem* CCmSqlBaseItem::NewLC()
    {    
    CCmSqlBaseItem* self = new ( ELeave ) CCmSqlBaseItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlBaseItem::~CCmSqlBaseItem()
    {
    delete iCdsId;
    delete iHash;
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::SetId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlBaseItem::SetId( const TInt64 aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::SetCdsIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlBaseItem::SetCdsIdL( const TDesC8& aCdsId )
    {
    delete iCdsId;
    iCdsId = NULL;

    if( &aCdsId )
        {
        iCdsId = aCdsId.AllocL();
        }
    else
        {
        iCdsId = KNullDesC8().AllocL();
        }               
    }
    
// ---------------------------------------------------------------------------
// CCmSqlBaseItem::SetHashL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlBaseItem::SetHashL( const TDesC& aHash )
    {
    delete iHash;
    iHash = NULL;

    if( &aHash )
        {
        iHash = aHash.AllocL();
        }
    else
        {
        iHash = KNullDesC().AllocL();
        }     
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::SetSearchId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlBaseItem::SetSearchId( 
    const TInt64 aSearchId )
    {
    iSearchId = aSearchId;     
    } 
    
// ---------------------------------------------------------------------------
// CCmSqlBaseItem::Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlBaseItem::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::CdsId
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmSqlBaseItem::CdsId() const
    {
    return *iCdsId;
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::Hash
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CCmSqlBaseItem::Hash() const
    {
    return *iHash;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlBaseItem::SearchId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlBaseItem::SearchId() const
    {
    return iSearchId;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlBaseItem::CompareByHash
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlBaseItem::CompareByHash( 
    const CCmSqlBaseItem& aFirst, const CCmSqlBaseItem& aSecond )
    {
    return aFirst.Hash().Compare( aSecond.Hash() );
    }

// ---------------------------------------------------------------------------
// CCmSqlBaseItem::CompareByCdsId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlBaseItem::CompareByCdsId( 
    const CCmSqlBaseItem& aFirst, const CCmSqlBaseItem& aSecond )
    {
    return aFirst.CdsId().Compare( aSecond.CdsId() );
    }
            
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlBaseItem::CCmSqlBaseItem()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlBaseItem::ConstructL()
    {
    iCdsId = KNullDesC8().AllocL();
    iHash = KNullDesC().AllocL();
    }    

// End of file

