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
* Description:      Capsulating fill and store file lists
*
*/






#include <e32std.h>
#include <s32mem.h>
#include "cmbaselistitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmBaseListItem* CCmBaseListItem::NewL()
    {   
    CCmBaseListItem* self = CCmBaseListItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmBaseListItem* CCmBaseListItem::NewLC()
    {    
    CCmBaseListItem* self = new ( ELeave ) CCmBaseListItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmBaseListItem::~CCmBaseListItem()
    {
    delete iPrimaryText;
    delete iSecondaryText;
    delete iPath;
    }

    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetPrimaryTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetPrimaryTextL( const TDesC& aText )
    {
    if( &aText )
        {
        delete iPrimaryText;
        iPrimaryText = NULL;
        iPrimaryText = aText.AllocL();        
        }
    else
        {
        delete iPrimaryText;
        iPrimaryText = NULL;
        iPrimaryText = KNullDesC().AllocL();     
        }
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetSecondaryTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetSecondaryTextL( const TDesC& aText )
    {
    if( &aText )
        {
        delete iSecondaryText;
        iSecondaryText = NULL;
        iSecondaryText = aText.AllocL();        
        }
    else
        {
        delete iSecondaryText;
        iSecondaryText = NULL;        
        iSecondaryText = KNullDesC().AllocL();     
        }            
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetPathL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetPathL( const TDesC& aText )
    {
    if( &aText )
        {
        delete iPath;
        iPath = NULL;
        iPath = aText.AllocL();        
        }
    else
        {
        delete iPath;
        iPath = NULL;        
        iPath = KNullDesC().AllocL();     
        }        
    }

// ---------------------------------------------------------------------------
// CCmBaseListItem::SetSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetSize( const TUint32 aSize )
    {
    iSize = aSize;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetDate
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetDate( const TTime aDate )
    {
    iDate = aDate;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetStatus( const TCmListItemStatus aStatus )
    {
    iStatus = aStatus;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetDbId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetDbId( const TUint64 aId )
    {
    iId = aId;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetRefId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetRefId( const TUint64 aId )
    {
    iRefId = aId;
    } 
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SetMediaType
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::SetMediaType( const TCmMediaType aMediaType )
    {
    iMediaType = aMediaType;
    }
                
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::PrimaryText
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CCmBaseListItem::PrimaryText() const
    {
    return *iPrimaryText;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::SecondaryText
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CCmBaseListItem::SecondaryText() const
    {
    return *iSecondaryText;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::Path
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CCmBaseListItem::Path() const
    {
    return *iPath;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::Size
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmBaseListItem::Size() const
    {
    return iSize;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::Date
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CCmBaseListItem::Date() const
    {
    return iDate;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::Status
// ---------------------------------------------------------------------------
//
EXPORT_C TCmListItemStatus CCmBaseListItem::Status() const
    {
    return iStatus;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::DbId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint64 CCmBaseListItem::DbId() const
    {
    return iId;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::RefId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint64 CCmBaseListItem::RefId() const
    {
    return iRefId;
    }
    
// ---------------------------------------------------------------------------
// CCmBaseListItem::MediaType
// ---------------------------------------------------------------------------
//
EXPORT_C TCmMediaType CCmBaseListItem::MediaType() const
    {
    return iMediaType;
    }
            
// ---------------------------------------------------------------------------
// CCmBaseListItem::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iPrimaryText->Length() );
    if ( iPrimaryText )
        {
        aStream << *iPrimaryText;
        }
    else
        {
        aStream << KNullDesC();
        }
    aStream.WriteInt32L( iSecondaryText->Length() );
    if ( iSecondaryText )
        {
        aStream << *iSecondaryText;
        }
    else
        {
        aStream << KNullDesC();
        }
        
    aStream.WriteInt32L( iPath->Length() );
    if ( iPath )
        {
        aStream << *iPath;
        }
    else
        {
        aStream << KNullDesC();
        }
                
    aStream.WriteInt32L( iSize );
    aStream.WriteUint32L( iDate.Int64() );
    aStream.WriteInt32L( iStatus );
    aStream.WriteUint32L( iId );
    aStream.WriteUint32L( iRefId );    
    aStream.WriteInt32L( iMediaType );    
    }
        
// ---------------------------------------------------------------------------
// CCmBaseListItem::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmBaseListItem::InternalizeL( RReadStream& aStream )
    {
    // Content
    delete iPrimaryText;
    iPrimaryText = NULL;

    TInt bufLength = aStream.ReadInt32L();    
    iPrimaryText = HBufC::NewL( aStream, bufLength ); 

    delete iSecondaryText;
    iSecondaryText = NULL;

    bufLength = aStream.ReadInt32L();    
    iSecondaryText = HBufC::NewL( aStream, bufLength );

    delete iPath;
    iPath = NULL;

    bufLength = aStream.ReadInt32L();    
    iPath = HBufC::NewL( aStream, bufLength );    
    iSize = aStream.ReadInt32L();
    iDate = aStream.ReadUint32L();
    iStatus = (TCmListItemStatus)aStream.ReadInt32L();
    iId = aStream.ReadUint32L();
    iRefId = aStream.ReadUint32L();     
    iMediaType = static_cast<TCmMediaType>( aStream.ReadInt32L() );
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmBaseListItem::CCmBaseListItem()
    {            
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmBaseListItem::ConstructL()
    {
    iPrimaryText = KNullDesC().AllocL();    
    iSecondaryText = KNullDesC().AllocL();    
    iPath = KNullDesC().AllocL();
    }    

// End of file

