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
* Description:  UPnP container class implementation
*
*/


// INCLUDE FILES
#include    "upnpmediaservereventlist.h"
#include    "upnpmediaserverevent.h"

#include    <e32base.h>









// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::CUpnpMediaServerEventList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMediaServerEventList::CUpnpMediaServerEventList()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerEventList::ConstructL( )
    {
    }
    
      
// Destructor
CUpnpMediaServerEventList::~CUpnpMediaServerEventList()
    {
    iEvents.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpMediaServerEventList::ExternalizeL( RWriteStream& aStream ) const
	{
    // Let's write the count of items to stream first
    aStream.WriteInt16L( iEvents.Count() );
    for ( TInt i = 0; i < iEvents.Count(); i++ )
        {
        // Then the object itself
        iEvents[i]->ExternalizeL( aStream );
        }
	}
// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CUpnpMediaServerEventList::InternalizeL( RReadStream& aStream )
    {
	// internalize the objects
    iEvents.ResetAndDestroy();
    
    // First the count of individual objects
	TInt itemCount = aStream.ReadInt16L();

	// Then internalize them from the stream one by one
	for (TInt index = 0; index < itemCount; index++ )
	    {	    
	    InternalizeSubTypeL(aStream);	    
	    }
    }
    
    
EXPORT_C  RPointerArray<CUpnpMediaServerEvent>&  CUpnpMediaServerEventList::GetPointerArray( )
    {
    return iEvents;
    }
    
// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::ToDes8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC8* CUpnpMediaServerEventList::ToDes8L() const
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
