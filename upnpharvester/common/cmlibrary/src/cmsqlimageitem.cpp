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
#include "cmsqlimageitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlImageItem* CCmSqlImageItem::NewL()
    {   
    CCmSqlImageItem* self = CCmSqlImageItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlImageItem* CCmSqlImageItem::NewLC()
    {    
    CCmSqlImageItem* self = new ( ELeave ) CCmSqlImageItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlImageItem::~CCmSqlImageItem()
    {
    delete iDescription;
    }

// ---------------------------------------------------------------------------
// CCmSqlImageItem::SetResolutionId
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmSqlImageItem::SetResolutionId( const TInt64 aResolutionId )
    {
    iResolutionId = aResolutionId;
    }

// ---------------------------------------------------------------------------
// CCmSqlImageItem::SetDescription
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmSqlImageItem::SetDescriptionL( const TDesC8& aDescription )
    {
    delete iDescription;
    iDescription = NULL;

    if( &aDescription )
        {
        iDescription = aDescription.AllocL();        
        }
    else
        {
        iDescription = KNullDesC8().AllocL();
        }    
    }
        
// ---------------------------------------------------------------------------
// CCmSqlImageItem::ResolutionId
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt64 CCmSqlImageItem::ResolutionId() const
    {
    return iResolutionId;
    }

// ---------------------------------------------------------------------------
// CCmSqlImageItem::Description
// ---------------------------------------------------------------------------
// 
EXPORT_C TDesC8& CCmSqlImageItem::Description() const
    {
    return *iDescription;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlImageItem::CCmSqlImageItem()
    {               
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlImageItem::ConstructL()
    {
    iDescription = KNullDesC8().AllocL();
    }    

// End of file

