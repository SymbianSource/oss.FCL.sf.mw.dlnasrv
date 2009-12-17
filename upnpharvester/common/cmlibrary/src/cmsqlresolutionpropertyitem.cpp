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
* Description:      Capsulating resolution propety items
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmsqlresolutionpropertyitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlResolutionPropertyItem* CCmSqlResolutionPropertyItem::NewL()
    {   
    CCmSqlResolutionPropertyItem* self = 
        CCmSqlResolutionPropertyItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlResolutionPropertyItem* CCmSqlResolutionPropertyItem::NewLC()
    {    
    CCmSqlResolutionPropertyItem* self = 
        new ( ELeave ) CCmSqlResolutionPropertyItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlResolutionPropertyItem::~CCmSqlResolutionPropertyItem()
    {
    }

// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::SetWidth
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlResolutionPropertyItem::SetWidth( const TInt aWidth )
    {
    iWidth = aWidth;
    }

// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::SetHeight
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlResolutionPropertyItem::SetHeight( const TInt aHeight )
    {
    iHeight = aHeight;               
    }
    
// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::SetPixelCount
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlResolutionPropertyItem::SetPixelCount( 
    const TInt aPixelCount )
    {
    iPixelCount = aPixelCount;
    }

// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::Width
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlResolutionPropertyItem::Width() const
    {
    return iWidth;
    }

// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::Height
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlResolutionPropertyItem::Height() const
    {
    return iHeight;
    }

// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::PixelCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlResolutionPropertyItem::PixelCount() const
    {
    return iPixelCount;
    }

// ---------------------------------------------------------------------------
// CompareItemsByName
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlResolutionPropertyItem::CompareItemsByName( 
    const CCmSqlResolutionPropertyItem& aFirst, 
    const CCmSqlResolutionPropertyItem& aSecond )
    {
    return aFirst.Name().Compare( aSecond.Name() );
    }
                                                               
// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlResolutionPropertyItem::ExternalizeL( 
    RWriteStream& aStream ) const
    {      
    aStream.WriteInt32L( iWidth );
    aStream.WriteInt32L( iHeight );     
    aStream.WriteInt32L( iPixelCount );
    CCmSqlPropertyItem::ExternalizeL( aStream );
    }
        
// ---------------------------------------------------------------------------
// CCmSqlResolutionPropertyItem::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlResolutionPropertyItem::InternalizeL( 
    RReadStream& aStream )
    {
    iWidth = aStream.ReadInt32L();
    iHeight = aStream.ReadInt32L();
    iPixelCount = aStream.ReadInt32L();
    CCmSqlPropertyItem::InternalizeL( aStream );
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlResolutionPropertyItem::CCmSqlResolutionPropertyItem()
    {               
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlResolutionPropertyItem::ConstructL()
    {
    }    

// End of file

