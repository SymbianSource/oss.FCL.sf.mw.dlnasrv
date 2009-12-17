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
* Description:      Capsulating sql itemss
*
*/






#include <e32std.h>
#include "cmsqlvideoitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlVideoItem* CCmSqlVideoItem::NewL()
    {   
    CCmSqlVideoItem* self = CCmSqlVideoItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlVideoItem* CCmSqlVideoItem::NewLC()
    {    
    CCmSqlVideoItem* self = new ( ELeave ) CCmSqlVideoItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlVideoItem::~CCmSqlVideoItem()
    {
    }

// ---------------------------------------------------------------------------
// CCmSqlVideoItem::SetGenreId
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmSqlVideoItem::SetGenreId( const TInt64 aGenreId )
    {
    iGenreId = aGenreId;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlVideoItem::GenreId
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt64 CCmSqlVideoItem::GenreId() const
    {
    return iGenreId;
    }
   
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlVideoItem::CCmSqlVideoItem()
    {                
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlVideoItem::ConstructL()
    {
    }    

// End of file

