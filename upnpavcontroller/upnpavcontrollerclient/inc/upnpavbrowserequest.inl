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
* Description:      a data class containing browse-related data
*
*/






// INCLUDE FILES

#include "upnpavbrowserequest.h"

// CONSTANTS
const TInt KBrowseMaxStringLen = 255;
const TInt KBrowseBufferGranularity = 100;    


// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::CUpnpAVBrowseRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
inline CUpnpAVBrowseRequest::CUpnpAVBrowseRequest()
    {    
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::ConstructL()
    {
    iId = KNullDesC8().AllocL();
    iSearchCriteria = KNullDesC8().AllocL();
    iFilter = KNullDesC8().AllocL();
    iSortCriteria = KNullDesC8().AllocL();
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpAVBrowseRequest* CUpnpAVBrowseRequest::NewL()
    {
    CUpnpAVBrowseRequest* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpAVBrowseRequest* CUpnpAVBrowseRequest::NewLC()
    {
    CUpnpAVBrowseRequest* self = new( ELeave )
        CUpnpAVBrowseRequest;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpAVBrowseRequest::~CUpnpAVBrowseRequest()
    {
    delete iId;
    delete iSearchCriteria;
    delete iFilter;
    delete iSortCriteria;         
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::ExternalizeL( RWriteStream& aStream ) const
    {    
    aStream.WriteInt16L( iCriteriaLength );    
    aStream << *iId;
    aStream << *iSearchCriteria;
    aStream << *iFilter;
    aStream.WriteInt16L( (TInt)iBrowseFlag );
    aStream.WriteInt16L( iStartIndex );
    aStream.WriteInt16L( iRequestedCount );
    aStream << *iSortCriteria;   
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::InternalizeL
// Internalizes object
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::InternalizeL( RReadStream& aStream )
    {
    iCriteriaLength = aStream.ReadInt16L();
    
    delete iId; iId = NULL;
    iId = HBufC8::NewL( aStream, KBrowseMaxStringLen );

    delete iSearchCriteria; iSearchCriteria = NULL;
    iSearchCriteria = HBufC8::NewL( aStream, iCriteriaLength );

    delete iFilter; iFilter = NULL;
    iFilter = HBufC8::NewL( aStream, KBrowseMaxStringLen );
    
    iBrowseFlag = (MUPnPAVBrowsingSession::TBrowseFlag)aStream.ReadInt16L();
    
    iStartIndex = aStream.ReadInt16L();
    
    iRequestedCount = aStream.ReadInt16L();

    delete iSortCriteria; iSortCriteria = NULL;
    iSortCriteria = HBufC8::NewL( aStream, KBrowseMaxStringLen );
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::ToDes8L
// Externalizes object to a heap descriptor
// --------------------------------------------------------------------------
inline HBufC8* CUpnpAVBrowseRequest::ToDes8L() const
    {
    // serialize object
    CBufFlat* tempFlatBuf = CBufFlat::NewL( KBrowseBufferGranularity );
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
// CUpnpAVBrowseRequest::SetIdL
// Sets id
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetIdL( const TDesC8& aId )
    {
    HBufC8* tempBuf = aId.AllocL();
    delete iId;
    iId = tempBuf;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::Id
// Returns id
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVBrowseRequest::Id()
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
// CUpnpAVBrowseRequest::SetSearchCriteriaL
// Sets search criteria
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetSearchCriteriaL(
    const TDesC8& aSearchCriteria )
    {
    HBufC8* tempBuf = aSearchCriteria.AllocL();
	iCriteriaLength = tempBuf->Length();
	delete iSearchCriteria;
	iSearchCriteria = tempBuf;
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SearchCriteria
// Returns search criteria
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVBrowseRequest::SearchCriteria()
    {
    if( iSearchCriteria )
        {
        return *iSearchCriteria;
        }
    else
        {
        return KNullDesC8;
        }        
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SetFilterL
// Sets filter
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetFilterL( const TDesC8& aFilter )
    {
    HBufC8* tempBuf = aFilter.AllocL();
    delete iFilter;
    iFilter = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::Filter
// Returns filter
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVBrowseRequest::Filter()
    {    
    if( iFilter )
        {
        return *iFilter;
        }
    else
        {
        return KNullDesC8;
        }        
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SetBrowseFlag
// Sets browse flag
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetBrowseFlag(
    MUPnPAVBrowsingSession::TBrowseFlag aBrowseFlag )
    {
    iBrowseFlag = aBrowseFlag;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::BrowseFlag
// Returns browse flag
// --------------------------------------------------------------------------
inline MUPnPAVBrowsingSession::TBrowseFlag CUpnpAVBrowseRequest::BrowseFlag()
    {
    return iBrowseFlag;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SetStartIndex
// Sets start index
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetStartIndex( TInt aStartIndex )
    {
    iStartIndex = aStartIndex;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::StartIndex
// Returns start index
// --------------------------------------------------------------------------
inline TInt CUpnpAVBrowseRequest::StartIndex()
    {
    return iStartIndex;
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SetRequestedCount
// Sets requested count
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetRequestedCount( TInt aRequestedCount )
    {
    iRequestedCount = aRequestedCount;
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::RequestedCount
// Returns requested count
// --------------------------------------------------------------------------
inline TInt CUpnpAVBrowseRequest::RequestedCount()
    {
    return iRequestedCount;
    }

// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SetSortCriteriaL
// Sets sort criteria
// --------------------------------------------------------------------------
inline void CUpnpAVBrowseRequest::SetSortCriteriaL(
    const TDesC8& aSortCriteria )
    {
    HBufC8* tempBuf = aSortCriteria.AllocL();
    delete iSortCriteria;
    iSortCriteria = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVBrowseRequest::SortCriteria
// Returns sort criteria
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpAVBrowseRequest::SortCriteria()
    {
    if( iSortCriteria )
        {
        return *iSortCriteria;
        }
    else
        {
        return KNullDesC8;
        }
    }

// End of File

