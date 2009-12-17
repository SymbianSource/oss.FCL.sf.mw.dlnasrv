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
* Description:      Capsulating fill file list item
*
*/






#include <e32std.h>
#include <s32mem.h>
#include "cmfilllistitem.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillListItem* CCmFillListItem::NewL()
    {    
    CCmFillListItem* self = CCmFillListItem::NewLC();
    CleanupStack::Pop( self ); 
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmFillListItem* CCmFillListItem::NewLC()
    {    
    CCmFillListItem* self = new ( ELeave ) CCmFillListItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// NewLC with parameters
// ---------------------------------------------------------------------------
// 
EXPORT_C CCmFillListItem* CCmFillListItem::NewLC( const TUint aPriority, 
    const TUint aListId, const TUint aStatus, 
    const TCmFillRuleStatus aSelected )
    {
    CCmFillListItem* self = new ( ELeave ) CCmFillListItem();
    CleanupStack::PushL( self );
    self->ConstructL( aPriority, aListId, aStatus, aSelected ); 
    return self;
    }
        
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillListItem::~CCmFillListItem()
    {
    delete iUpnpClass;
    delete iUri;
    delete iItemId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetUpnpClassL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetUpnpClassL( const TDesC& aUpnpClass )
    {
    if( &aUpnpClass )
        {
        delete iUpnpClass;
        iUpnpClass = NULL;
        iUpnpClass = aUpnpClass.AllocL();        
        }
    else
        {
        delete iUpnpClass;
        iUpnpClass = NULL;
        iUpnpClass = KNullDesC().AllocL();        
        }    
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetUriL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetUriL( const TDesC8& aUri )
    {
    if( &aUri )
        {
        delete iUri;
        iUri = NULL;
        iUri = aUri.AllocL();         
        }
    else
        {
        delete iUri;
        iUri = NULL;
        iUri = KNullDesC8().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetItemIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetItemIdL( const TDesC8& aItemId )
    {
    if( &aItemId )
        {
        delete iItemId;
        iItemId = NULL;
        iItemId = aItemId.AllocL();         
        }
    else
        {
        delete iItemId;
        iItemId = NULL;
        iItemId = KNullDesC8().AllocL();
        }            
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetListId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetListId( const TUint aId )
    {
    iListId = aId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetDevId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetDevId( const TUint8 aDevId )
    {
    iDevId = aDevId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetPriority
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetPriority( const TUint8 aPriority )
    {
    iPriority = aPriority;
    }       

// ---------------------------------------------------------------------------
// CCmFillListItem::SetSelected
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetSelected( 
    TCmFillRuleStatus aSelected  )
    {
    iSelected = aSelected;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetDriveId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::SetDriveId( const TUint aDriveId )
    {
    iDriveId = aDriveId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::SetDriveNumber
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillListItem::SetDriveNumber( const TInt aDriveNumber )
    {
    iDriveNumber = aDriveNumber;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::UpnpClass
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CCmFillListItem::UpnpClass() const
    {
    return *iUpnpClass;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::Uri
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmFillListItem::Uri() const
    {
    return *iUri;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::ItemId
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmFillListItem::ItemId() const
    {
    return *iItemId;    
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::ListId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCmFillListItem::ListId() const
    {
    return iListId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::DevId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CCmFillListItem::DevId() const
    {
    return iDevId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::Priority
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CCmFillListItem::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::Selected
// ---------------------------------------------------------------------------
//
EXPORT_C TCmFillRuleStatus CCmFillListItem::Selected() const
    {
    return iSelected;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::DriveId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCmFillListItem::DriveId() const
    {
    return iDriveId;
    }

// ---------------------------------------------------------------------------
// CCmFillListItem::DriveNumber
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillListItem::DriveNumber() const
    {
    return iDriveNumber;
    }
                     
// ---------------------------------------------------------------------------
// CCmFillListItem::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iUpnpClass->Length() );
    if ( iUpnpClass )
        {
        aStream << *iUpnpClass;
        }
    else
        {
        aStream << KNullDesC();
        }
    aStream.WriteInt32L( iUri->Length() );
    if ( iUri )
        {
        aStream << *iUri;
        }
    else
        {
        aStream << KNullDesC();
        }
    aStream.WriteInt32L( iItemId->Length() );
    if ( iItemId )
        {
        aStream << *iItemId;
        }
    else
        {
        aStream << KNullDesC();
        }
                
    aStream.WriteUint16L( iListId );
    aStream.WriteUint8L( iDevId );
    aStream.WriteUint8L( iPriority );
    aStream.WriteInt32L( iSelected );
    aStream.WriteUint32L( iDriveId );
    aStream.WriteInt32L( iDriveNumber );
    CCmBaseListItem::ExternalizeL(aStream);         
    }
        
// ---------------------------------------------------------------------------
// CCmFillListItem::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillListItem::InternalizeL( RReadStream& aStream )
    {
    // Content
    if ( iUpnpClass )
        {
        delete iUpnpClass;
        iUpnpClass = NULL;
        }
    TInt bufLength = aStream.ReadInt32L();    
    iUpnpClass = HBufC::NewL( aStream, bufLength ); 
    if ( iUri )
        {
        delete iUri;
        iUri = NULL;
        }
    bufLength = aStream.ReadInt32L();    
    iUri = HBufC8::NewL( aStream, bufLength );
    if ( iItemId )
        {
        delete iItemId;
        iItemId = NULL;
        }
    bufLength = aStream.ReadInt32L();    
    iItemId = HBufC8::NewL( aStream, bufLength );    
    iListId = aStream.ReadUint16L();
    iDevId = aStream.ReadUint8L();
    iPriority = aStream.ReadUint8L();
    iSelected = (TCmFillRuleStatus)aStream.ReadInt32L();
    iDriveId = aStream.ReadUint32L();
    iDriveNumber = aStream.ReadInt32L();
    CCmBaseListItem::InternalizeL(aStream);      
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmFillListItem::CCmFillListItem()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmFillListItem::ConstructL()
    {
    // construct base class
    CCmBaseListItem::ConstructL();
    
    // then construct the rest
    iUpnpClass = KNullDesC().AllocL();
    iUri = KNullDesC8().AllocL();
    iItemId = KNullDesC8().AllocL();
    }
    
// ---------------------------------------------------------------------------
// ConstructL with parameters
// ---------------------------------------------------------------------------
//    
void CCmFillListItem::ConstructL( const TUint aPriority, const TUint aListId, 
    const TUint aStatus, const TCmFillRuleStatus aSelected )
    {
    // construct base class
    CCmBaseListItem::ConstructL();
    
    SetPriority( aPriority );
    SetListId( aListId );
    SetStatus( (TCmListItemStatus)aStatus );
    SetSelected( aSelected );
    iUpnpClass = KNullDesC().AllocL();
    iUri = KNullDesC8().AllocL();
    iItemId = KNullDesC8().AllocL();      
    }          

// End of file

