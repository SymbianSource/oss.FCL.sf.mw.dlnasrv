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
* Description:      Capsulating propety item objects
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmsqlpropertyitem.h"
#include "cmsqlpropertycontainer.h"
#include "msdebug.h"

// CONSTANTS
const TInt KArrayGranularity = 16;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyContainer* CCmSqlPropertyContainer::NewL()
    {
    CCmSqlPropertyContainer* self = CCmSqlPropertyContainer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyContainer* CCmSqlPropertyContainer::NewLC()
    {
    CCmSqlPropertyContainer* self = new ( ELeave ) CCmSqlPropertyContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyContainer::~CCmSqlPropertyContainer()
    {
    iPropertyItems.ResetAndDestroy();
    iPropertyItems.Close();
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::AddPropertyItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::AddPropertyItemL(
    CCmSqlPropertyItem* aItem )
    {
    iPropertyItems.AppendL( aItem );
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::DeletePropertyItem
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::DeletePropertyItem( TInt aIndex )
    {
    if( iPropertyItems.Count() > aIndex )
        {
        delete iPropertyItems[aIndex];
        iPropertyItems.Remove(aIndex);
        iPropertyItems.Compress();
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::PropertyItem
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyItem* CCmSqlPropertyContainer::PropertyItem(
    TInt aIndex )
    {
    return iPropertyItems[aIndex];
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::PropertyItemCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlPropertyContainer::PropertyItemCount() const
    {
    return iPropertyItems.Count();
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::SetType
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::SetType( TCmMetadataField aType )
    {
    iType = aType;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::Type
// ---------------------------------------------------------------------------
//
EXPORT_C TCmMetadataField CCmSqlPropertyContainer::Type( )
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::IsDuplicate
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmSqlPropertyContainer::IsDuplicate(
    CCmSqlPropertyItem& aItem )
    {
    TBool ret( EFalse );
    TInt64 id( aItem.Id() );
    TInt count( iPropertyItems.Count() );
    for( TInt i = 0; i < count; i++ )
        {
        if( id == iPropertyItems[i]->Id() )
            {
            ret = ETrue;
            // Duplicate found => end loop
            i = count;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::ExternalizeL(
    RWriteStream& aStream ) const
    {
    // Let's write the count of fill rules to stream first
    aStream.WriteInt16L( iPropertyItems.Count() );
    aStream.WriteUint8L( iType );
    for ( TInt index = 0; index < iPropertyItems.Count(); index++ )
        {
        CCmSqlPropertyItem* item = iPropertyItems[index];
        // Then the object itself
        item->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::InternalizeL( RReadStream& aStream )
    {
    // Then internalize the objects
    if ( iPropertyItems.Count() > KErrNone )
        {
        iPropertyItems.ResetAndDestroy();
        }
    // First the count of fill rules
    TInt itemCount = aStream.ReadInt16L();
    iType = (TCmMetadataField)aStream.ReadInt8L();
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < itemCount; index++ )
        {
        CCmSqlPropertyItem* newItem = CCmSqlPropertyItem::NewL();
        CleanupStack::PushL( newItem );
        newItem->InternalizeL( aStream );
        AddPropertyItemL( newItem );
        CleanupStack::Pop( newItem );
        newItem = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CCmSqlPropertyContainer::SortPropertyItem
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyContainer::SortPropertyItem( )
    {
    if( ( ECmArtist == iType ) || ( ECmAlbum == iType )
               || ( ECmGenre == iType ) || ( ECmTitle == iType ) )
        {
        if( iPropertyItems.Count() > 0 )
            {
            iPropertyItems.Sort( TLinearOrder<CCmSqlPropertyItem>
                                ( CCmSqlPropertyContainer::CompareItem ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer::CCmSqlPropertyContainer() :
    iPropertyItems( KArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCmSqlPropertyContainer::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CCmSqlPropertyContainer::CompareItem
// -----------------------------------------------------------------------------
//
TInt CCmSqlPropertyContainer::CompareItem(
    const CCmSqlPropertyItem& aItemOne,const CCmSqlPropertyItem& aItemTwo )
    {
    TInt ret = 0;
    ret = ( aItemOne.Name() ).CompareC( aItemTwo.Name() );
    return ret;
    }

// End of file
