/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  UPnPBrowseCriteria implementation
*
*/


// INCLUDE FILES
#include "upnpbrowsecriteria.h"
#include "upnpattribute.h"
#include "upnpobject.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpBrowseCriteria::ConstructL(const TDesC8& aFilter)
{
    iFilter = aFilter.AllocL();
}

// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpBrowseCriteria* CUpnpBrowseCriteria::NewLC(const TDesC8& aFilter)
{
    CUpnpBrowseCriteria* self = new( ELeave ) CUpnpBrowseCriteria;
    CleanupStack::PushL( self );
    self->ConstructL(aFilter);
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpBrowseCriteria* CUpnpBrowseCriteria::NewL(const TDesC8& aFilter)
{
    CUpnpBrowseCriteria* self = NewLC(aFilter);
    CleanupStack::Pop(self);
    return self;
}  
  
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::CUpnpBrowseCriteria
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpBrowseCriteria::CUpnpBrowseCriteria()
    {
    }

// Destructor
CUpnpBrowseCriteria::~CUpnpBrowseCriteria()
    {
    delete iFilter;
    }

// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::SetFilterL
// Sets filter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::SetFilterL( const TDesC8& aFilter )
    {
    HBufC8* tmp = aFilter.AllocL();
    delete iFilter;
    iFilter = tmp;    
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::GetFilter
// Returns filter. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpBrowseCriteria::Filter() const
    {
    return *iFilter;
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::SetRequestCount
// Sets request count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::SetRequestedCount( TInt aRequestedCount )
    {
    iRequestedCount=aRequestedCount;
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::RequestedCount
// Returns requested count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpBrowseCriteria::RequestedCount() const
    {
    return iRequestedCount;
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::SetStartingIndex
// Sets starting index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::SetStartingIndex( TInt aStartingIndex)
    {
    iStartingIndex=aStartingIndex;
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::StartingIndex
// Returns starting index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpBrowseCriteria::StartingIndex() const
    {
    return iStartingIndex;
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::CopyL
// Copies the content from parameter CUpnpElement to this
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::CopyL( const CUpnpBrowseCriteria& aBrowseCriteria )
    {
    CBufFlat* buffer = CBufFlat::NewL( sizeof( aBrowseCriteria ) );
    CleanupStack::PushL( buffer );
    RBufWriteStream outStream;
    outStream.Open( *buffer );
    CleanupClosePushL( outStream );
    RBufReadStream inStream;
    inStream.Open( *buffer );
    CleanupClosePushL( inStream );
    aBrowseCriteria.ExternalizeL( outStream );
    InternalizeL( inStream );	
    CleanupStack::PopAndDestroy( 3 );   // inStream && outStream && buffer
    }
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::ExternalizeL( RWriteStream& aStream ) const
    {
        aStream << *iFilter;
        aStream.WriteInt32L(RequestedCount());
        aStream.WriteInt32L(StartingIndex());
    }
        
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCriteria::InternalizeL( RReadStream& aStream )
    {
     
    delete iFilter;
    iFilter = NULL;
    iFilter = HBufC8::NewL( aStream,KMaxUpnpObjStringLen);
    
    iRequestedCount=aStream.ReadInt32L();
    iStartingIndex=aStream.ReadInt32L();
    }
// -----------------------------------------------------------------------------
// CUpnpBrowseCriteria::ToDes8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC8* CUpnpBrowseCriteria::ToDes8L() const
{
    // serialize object
    CBufFlat* buf = CBufFlat::NewL(KDefBufferGranularity);
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    stream << *this;
    
    // create heap descriptor
    HBufC8* hbuf = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(hbuf->Des());
    buf->Read(0, ptr, buf->Size());
    
    // clean up
    CleanupStack::Pop(hbuf);
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    
    return hbuf;
}
//  End of File  
