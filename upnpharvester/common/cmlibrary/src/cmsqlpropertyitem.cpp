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
* Description:      Capsulating propety items
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmsqlpropertyitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyItem* CCmSqlPropertyItem::NewL()
    {
    CCmSqlPropertyItem* self = CCmSqlPropertyItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyItem* CCmSqlPropertyItem::NewLC()
    {
    CCmSqlPropertyItem* self = new ( ELeave ) CCmSqlPropertyItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyItem::~CCmSqlPropertyItem()
    {
    delete iName;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::SetId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyItem::SetId( const TInt64 aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::SetNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyItem::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    if( &aName )
        {
        iName = aName.AllocL();
        }
    else
        {
        iName = KNullDesC8().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::SetStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyItem::SetStatus( const TBool aStatus )
    {
    iStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlPropertyItem::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::Name
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmSqlPropertyItem::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::Status
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmSqlPropertyItem::Status() const
    {
    return iStatus;
    }

// ---------------------------------------------------------------------------
// CompareItemsByName
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlPropertyItem::CompareItemsByName(
    const CCmSqlPropertyItem& aFirst, const CCmSqlPropertyItem& aSecond )
    {
    return aFirst.Name().Compare( aSecond.Name() );
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iName->Length() );
    if ( iName )
        {
        aStream << *iName;
        }
    else
        {
        aStream << KNullDesC8();
        }
    aStream.WriteUint32L( iId );
    aStream.WriteInt32L( iStatus );
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyItem::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyItem::InternalizeL( RReadStream& aStream )
    {
    // Content
    if ( iName )
        {
        delete iName;
        iName = NULL;
        }
    TInt bufLength = aStream.ReadInt32L();
    iName = HBufC8::NewL( aStream, bufLength );

    iId = aStream.ReadUint32L();
    iStatus = (TBool)aStream.ReadInt32L();
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCmSqlPropertyItem::CCmSqlPropertyItem()
    {
    }

// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyItem::CCmSqlPropertyItem(
                                    const CCmSqlPropertyItem& aItem ) :
    iId( aItem.iId ), iStatus( aItem.iStatus )
    {
    iName = aItem.Name().Alloc();
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCmSqlPropertyItem::ConstructL()
    {
    iName = KNullDesC8().AllocL();
    }

// End of file

