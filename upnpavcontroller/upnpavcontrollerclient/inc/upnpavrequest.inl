/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a data class for client-server comms
*
*/






// INCLUDE FILES

#include "upnpavrequest.h"

#include <upnpitem.h>

// CONSTANTS
_LIT( KURIRequestPanicText, "CUpnpAVRequest" );
const TInt KMaxStringLen = 256;
const TInt KMaxStringLenLong = 1024;
const TInt KBufferGranularity = 100;    

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVRequest::CUpnpAVRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
inline CUpnpAVRequest::CUpnpAVRequest()
    {    
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::ConstructL()
    {
    iId = KNullDesC8().AllocL();
    iUuid = KNullDesC8().AllocL();
    iFilePath = KNullDesC().AllocL();;
    iURI = KNullDesC8().AllocL(); 
    //iItem = CUpnpItem::NewL();                              
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpAVRequest* CUpnpAVRequest::NewL()
    {
    CUpnpAVRequest* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpAVRequest* CUpnpAVRequest::NewLC()
    {
    CUpnpAVRequest* self = new( ELeave )
        CUpnpAVRequest;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// --------------------------------------------------------------------------
// CUpnpAVRequest::~CUpnpAVRequest
// Destructor
// --------------------------------------------------------------------------
inline CUpnpAVRequest::~CUpnpAVRequest()
    {
    delete iId;
    delete iUuid;
    delete iFilePath;
    delete iURI;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::ExternalizeL
// Externatizes object
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::ExternalizeL( RWriteStream& aStream ) const
    {    
    aStream << *iId;
    aStream << *iUuid;
    aStream << *iFilePath;
    aStream << *iURI;
    aStream.WriteInt16L( (TInt)iRequiresBrowse );
    aStream.WriteInt16L( (TInt)iMemoryType );
    aStream.WriteInt16L( (TInt)iFileSharing );
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::InternalizeL
// Internalizes object
// --------------------------------------------------------------------------    
inline void CUpnpAVRequest::InternalizeL( RReadStream& aStream )
    {
    delete iId; iId = NULL;
    iId = HBufC8::NewL( aStream, KMaxStringLen );

    delete iUuid; iUuid = NULL;
    iUuid = HBufC8::NewL( aStream, KMaxStringLen );
    
    delete iFilePath; iFilePath = NULL;
    iFilePath = HBufC::NewL( aStream, KMaxStringLen );

    delete iURI; iURI = NULL;
    iURI = HBufC8::NewL( aStream, KMaxStringLenLong );
    
    iRequiresBrowse = (TBool)aStream.ReadInt16L();
    
    iMemoryType = (MUPnPAVBrowsingSession::TMemoryType)aStream.ReadInt16L();
    
    iFileSharing = (MUPnPAVBrowsingSession::TFileSharing)
        aStream.ReadInt16L();
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::ToDes8L
// Externalizes object to a heap descriptor
// --------------------------------------------------------------------------
inline HBufC8* CUpnpAVRequest::ToDes8L() const
    {
    // serialize object
    CBufFlat* tempFlatBuf = CBufFlat::NewL( KBufferGranularity );
    CleanupStack::PushL( tempFlatBuf );

    RBufWriteStream stream( *tempFlatBuf );
    CleanupClosePushL( stream );
    
    stream << *this;
    
    // create heap descriptor
    HBufC8* tempBuf = HBufC8::NewLC( tempFlatBuf->Size() );
    TPtr8 ptr( tempBuf->Des() );
    tempFlatBuf->Read( 0, ptr, tempFlatBuf->Size() );
    
    // clean up
    CleanupStack::Pop( tempBuf );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( tempFlatBuf );
    
    return tempBuf;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetIdL
// Sets id
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetIdL( const TDesC8& aId )
    {
    HBufC8* tempBuf = aId.AllocL();
    delete iId;
    iId = tempBuf;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::Id
// Returns id
// --------------------------------------------------------------------------    
inline const TDesC8& CUpnpAVRequest::Id()
    {
    if( iId )
        {
        return *iId;
        }
    else
        {
        return KNullDesC8;
        }    
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetUuidL
// Sets uuid
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetUuidL( const TDesC8& aUuid )
    {
    HBufC8* tempBuf = aUuid.AllocL();
    delete iUuid;
    iUuid = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::Uuid
// Returns uuid
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVRequest::Uuid()
    {    
    if( iUuid )
        {
        return *iUuid;
        }
    else
        {
        return KNullDesC8;
        }        
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetFilePathL
// Sets filepath
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetFilePathL( const TDesC& aFilePath )
    {
    HBufC* tempBuf = aFilePath.AllocL();
    delete iFilePath;
    iFilePath = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::FilePath
// Returns filepath
// --------------------------------------------------------------------------
inline const TDesC& CUpnpAVRequest::FilePath()
    {
    if( iFilePath )
        {
        return *iFilePath;
        }
    else
        {
        return KNullDesC;
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetURIL
// Sets URI
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetURIL( const TDesC8& aURI )
    {
    HBufC8* tempBuf = aURI.AllocL();
    delete iURI;
    iURI = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::URI
// Returns URI
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVRequest::URI()
    {
    if( iURI )
        {
        return *iURI;
        }
    else
        {
        return KNullDesC8;
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetRequiresBrowse
// 
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetRequiresBrowse( TBool aBrowse )
    {
    iRequiresBrowse = aBrowse;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::RequiresBrowse
// 
// --------------------------------------------------------------------------
inline TBool CUpnpAVRequest::RequiresBrowse()
    {
    return iRequiresBrowse;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetMemoryType
// Sets used memory type
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetMemoryType(
    MUPnPAVBrowsingSession::TMemoryType aMemoryType )
    {
    iMemoryType = aMemoryType;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::MemoryType
// Returns used memory type
// --------------------------------------------------------------------------
inline MUPnPAVBrowsingSession::TMemoryType CUpnpAVRequest::MemoryType()
    {
    return iMemoryType;
    }

// --------------------------------------------------------------------------
// CUpnpAVRequest::SetFileSharing
// Sets the state of filesharing
// --------------------------------------------------------------------------
inline void CUpnpAVRequest::SetFileSharing(
    MUPnPAVBrowsingSession::TFileSharing aFileSharing )
    {
    iFileSharing = aFileSharing;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVRequest::FileSharing
// Returns the state of filesharing
// --------------------------------------------------------------------------
inline MUPnPAVBrowsingSession::TFileSharing CUpnpAVRequest::FileSharing()
    {
    return iFileSharing;
    }

// End of File

