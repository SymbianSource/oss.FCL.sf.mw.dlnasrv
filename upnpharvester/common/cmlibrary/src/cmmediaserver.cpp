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

#include "cmmediaserver.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMediaServer* CCmMediaServer::NewL()
    {
    CCmMediaServer* self = CCmMediaServer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCmMediaServer* CCmMediaServer::NewLC()
    {
    CCmMediaServer* self = new ( ELeave ) CCmMediaServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmMediaServer::~CCmMediaServer()
    {
    delete iUDN;
    }

// ---------------------------------------------------------------------------
// Retrieve mediaserver info
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CCmMediaServer::MediaServer() const
    {
    return *iUDN;
    }

// ---------------------------------------------------------------------------
// Sets UDN of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServer::SetUDNL( const TDesC8& aUDN )
    {
    delete iUDN;
    iUDN = NULL;

    if( &aUDN )
        {
        iUDN = aUDN.AllocL();
        }
    else
        {
        iUDN = KNullDesC8().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// Sets database id of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmMediaServer::DbId() const
    {
    return iDbId;
    }

// ---------------------------------------------------------------------------
// Returns SystemUpdateID
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmMediaServer::SystemUpdateID() const
    {
    return iSystemUpdateID;
    }

// ---------------------------------------------------------------------------
// Sets SystemUpdateID
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServer::SetSystemUpdateID(
    const TInt aSystemUpdateID )
    {
    iSystemUpdateID = aSystemUpdateID;
    }
    
// ---------------------------------------------------------------------------
// Gets database id of the media server
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmMediaServer::SetDbId( const TInt64 aDbId )
    {
    iDbId = aDbId;
    }

// ---------------------------------------------------------------------------
// CCmMediaServer::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmMediaServer::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iUDN->Length() );

    if ( iUDN )
        {
        aStream << *iUDN;
        }
    else
        {
        aStream << KNullDesC8();
        }
    aStream.WriteUint32L( iDbId );
    aStream.WriteInt32L( iSystemUpdateID );
    }

// ---------------------------------------------------------------------------
// CCmMediaServer::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmMediaServer::InternalizeL( RReadStream& aStream )
    {
    if ( iUDN )
        {
        delete iUDN;
        iUDN = NULL;
        }
    TInt bufLength = aStream.ReadInt32L();
    iUDN = HBufC8::NewL( aStream, bufLength );
    iDbId = aStream.ReadUint32L();
    iSystemUpdateID = aStream.ReadInt32L();
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCmMediaServer::CCmMediaServer()
    {
    iSystemUpdateID = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCmMediaServer::ConstructL()
    {
    iUDN = KNullDesC8().AllocL();
    }

// End of file

