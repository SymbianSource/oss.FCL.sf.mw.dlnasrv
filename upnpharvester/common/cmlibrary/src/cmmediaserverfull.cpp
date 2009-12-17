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

#include "cmmediaserverfull.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMediaServerFull* CCmMediaServerFull::NewL()
    {
    CCmMediaServerFull* self = CCmMediaServerFull::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMediaServerFull* CCmMediaServerFull::NewLC()
    {
    CCmMediaServerFull* self = new ( ELeave ) CCmMediaServerFull();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMediaServerFull::~CCmMediaServerFull()
    {
    delete iName;
    iVisibleDate = NULL;
    }

// ---------------------------------------------------------------------------
// Returns name of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CCmMediaServerFull::MediaServerName() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// Sets media server name
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetMediaServerNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// Returns visible date of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CCmMediaServerFull::VisibleDate() const
    {
    return iVisibleDate;
    }

// ---------------------------------------------------------------------------
// Sets visible date of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetVisibleDate( TTime aTime )
    {
    iVisibleDate = aTime;
    }

// ---------------------------------------------------------------------------
// Gets is active status of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CCmMediaServerFull::IsActive() const
    {
    return iIsActive;
    }

// ---------------------------------------------------------------------------
// Sets is active status of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetIsActive( TUint8 aIsActive )
    {
    iIsActive = aIsActive;
    }

// ---------------------------------------------------------------------------
// Sets copy capability info
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmMediaServerFull::CopyCapability() const
    {
    return iCopyCapability;
    }

// ---------------------------------------------------------------------------
// Gets copy capability info
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetCopyCapability( TBool aCopyCapability )
    {
    iCopyCapability = aCopyCapability;
    }


// ---------------------------------------------------------------------------
// Sets server status ( used for store )
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetStoreUsage( TBool aUsed )
    {
    iStoreServer = aUsed;
    }

// ---------------------------------------------------------------------------
// Gets server status ( store usage )
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmMediaServerFull::StoreUsage()
    {    
    return iStoreServer;
    }  

// ---------------------------------------------------------------------------
// Sets server status ( used for fill )
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::SetFillUsage( TBool aUsed )
    {
    iFillServer = aUsed;
    }        

// ---------------------------------------------------------------------------
// Gets server status ( fill usage )
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmMediaServerFull::FillUsage()
    {    
    return iFillServer;
    }    

// ---------------------------------------------------------------------------
// CCmFillRule::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::ExternalizeL( RWriteStream& aStream ) const
    {
    CCmMediaServer::ExternalizeL(aStream);
    aStream.WriteInt32L( iName->Length() );
    if ( iName )
        {
        aStream << *iName;
        }
    else
        {
        aStream << KNullDesC8();
        }    
    aStream << iVisibleDate.Int64();
    aStream.WriteInt8L( iIsActive );
    aStream.WriteInt8L( iCopyCapability );
    aStream.WriteInt8L( iFillServer );
    aStream.WriteInt8L( iStoreServer );
    }

// ---------------------------------------------------------------------------
// CCmFillRule::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServerFull::InternalizeL( RReadStream& aStream )
    {
    CCmMediaServer::InternalizeL(aStream);
    if ( iName )
        {
        delete iName;
        iName = NULL;
        }
    TInt bufLength = aStream.ReadInt32L();
    iName = HBufC8::NewL( aStream, bufLength );    
    TInt64 tempTime;
    aStream >> tempTime;
    iVisibleDate = tempTime;
    iIsActive = aStream.ReadInt8L();
    iCopyCapability = aStream.ReadInt8L();
    iFillServer = aStream.ReadInt8L();
    iStoreServer = aStream.ReadInt8L();
    }


// End of file
