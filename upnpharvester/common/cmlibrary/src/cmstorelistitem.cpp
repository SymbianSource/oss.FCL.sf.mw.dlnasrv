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
* Description:      Capsulating store file list item
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmstorelistitem.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreListItem* CCmStoreListItem::NewL()
    {    
    CCmStoreListItem* self = CCmStoreListItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmStoreListItem* CCmStoreListItem::NewLC()
    {   
    CCmStoreListItem* self = new ( ELeave ) CCmStoreListItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreListItem::~CCmStoreListItem()
    {
    iDeviceIds.Reset();
    iDeviceIds.Close();
    iStatusValues.Reset();
    iStatusValues.Close();    
    }
    
// ---------------------------------------------------------------------------
// CCmStoreListItem::SetListId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreListItem::SetListId( const TUint aListId )
    {
    iListId = aListId;
    }

// ---------------------------------------------------------------------------
// CCmStoreListItem::SetDevId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreListItem::SetDevId( const TUint8 aId, 
    TCmListItemStatus aStatus )
    {
    iDeviceIds.Append( aId );
    iStatusValues.AppendL( aStatus );
    }    
    
// ---------------------------------------------------------------------------
// CCmStoreListItem::ListId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCmStoreListItem::ListId() const
    {
    return iListId;
    }

// ---------------------------------------------------------------------------
// CCmStoreListItem::DevIds
// ---------------------------------------------------------------------------
//
EXPORT_C RArray<TInt> CCmStoreListItem::DevIds() const
    {
    return iDeviceIds;
    }    

// ---------------------------------------------------------------------------
// CCmStoreListItem::StatusValues
// ---------------------------------------------------------------------------
//
EXPORT_C RArray<TCmListItemStatus> CCmStoreListItem::StatusValues() const
    {
    return iStatusValues;
    }

// ---------------------------------------------------------------------------
// CCmStoreListItem::UpdateFileStatusL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreListItem::UpdateFileStatusL( const TUint8 aId, 
    TCmListItemStatus aStatus )
    {
    // Find index of the device
    TInt index( iDeviceIds.FindL( aId ) );
    if( KErrNotFound != index )
        {
        // Use index when updating status value
        iStatusValues[index] = aStatus;
        }

    }
        
// ---------------------------------------------------------------------------
// CCmStoreListItem::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreListItem::ExternalizeL( RWriteStream& aStream ) const
    {                
    aStream.WriteUint16L( iListId );
    aStream.WriteUint16L( iDeviceIds.Count() );
    for( TInt i = 0; i < iDeviceIds.Count(); i++ )
        {
        aStream.WriteInt16L( iDeviceIds[i] );    
        }
    aStream.WriteInt16L( iStatusValues.Count() );
    for( TInt i = 0; i < iStatusValues.Count(); i++ )
        {
        aStream.WriteInt16L( iStatusValues[i] );    
        }            
    CCmBaseListItem::ExternalizeL(aStream);              
    }
        
// ---------------------------------------------------------------------------
// CCmStoreListItem::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreListItem::InternalizeL( RReadStream& aStream )
    {
    iListId = aStream.ReadUint16L();
    TUint count( aStream.ReadUint16L() );
    iDeviceIds.Reset();
    for( TInt i = 0; i < count; i++ )
        {
        iDeviceIds.Append( aStream.ReadInt16L() );
        }
    count = aStream.ReadUint16L();
    iStatusValues.Reset();
    for( TInt i = 0; i < count; i++ )
        {
        iStatusValues.AppendL( (TCmListItemStatus) aStream.ReadInt16L() );
        }        
    CCmBaseListItem::InternalizeL(aStream);               
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmStoreListItem::CCmStoreListItem()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmStoreListItem::ConstructL()
    {
    // construct base class
    CCmBaseListItem::ConstructL();
    }    

// End of file

