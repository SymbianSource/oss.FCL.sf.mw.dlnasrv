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
* Description:      Implements a list of UPnP Devices
*
*/






// INCLUDE FILES
#include    <e32base.h>
#include    "upnpavdevicelist.h"
#include    "upnpavdevice.h"


// CONSTANTS
const TInt KBufferGranularity = 300;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::CUpnpAVDeviceList
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpAVDeviceList::CUpnpAVDeviceList()
    {
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpAVDeviceList::ConstructL( )
    {
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpAVDeviceList* CUpnpAVDeviceList::NewL()
    {
    CUpnpAVDeviceList* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpAVDeviceList* CUpnpAVDeviceList::NewLC()
    {
    CUpnpAVDeviceList* self = new( ELeave ) CUpnpAVDeviceList;
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }
      
// Destructor
CUpnpAVDeviceList::~CUpnpAVDeviceList()
    {
    iDevices.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::AppendDeviceL
// Appends a device to internal array.
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::AppendDeviceL( CUpnpAVDevice& aDevice )
    {
    iDevices.AppendL( &aDevice );
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::RemoveAndDestroy
// Removes and destroys device from internal array according to index
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::RemoveAndDestroy( TInt aIndex )
    {
    delete iDevices[ aIndex ];
    iDevices.Remove( aIndex );
    }
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::Remove
// Detaches object from internal array according to index
// (other items were commented in a header)
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::Remove( TInt aIndex)
    {
    iDevices.Remove( aIndex );
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::ResetAndDestroy
// Removes and destroys device from internal array according to index
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::ResetAndDestroy()
    {
    iDevices.ResetAndDestroy();
    }
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::Reset
// Detaches object from internal array according to index
// (other items were commented in a header)
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::Reset()
    {
    iDevices.Reset();
    }


// --------------------------------------------------------------------------
// CUpnpAVDeviceList::Count
// Counts objects 
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVDeviceList::Count( ) const
    {    
    return iDevices.Count();
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::GetObjectL
// Returns object from object array according to index
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpAVDevice* CUpnpAVDeviceList::operator[]( TInt aIndex ) const
    {
    return iDevices[ aIndex ];
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceList::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVDeviceList::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt count = iDevices.Count();

    aStream.WriteInt16L( count );
    
    for ( TInt i = 0; i < count; i++ )
        {
        iDevices[ i ]->ExternalizeL( aStream );
        }
    }
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// --------------------------------------------------------------------------
//  
EXPORT_C void CUpnpAVDeviceList::InternalizeL( RReadStream& aStream )
    {
    iDevices.ResetAndDestroy();

    TInt count = aStream.ReadInt16L();
    
    for (TInt i = 0; i< count; i++ )
        { 
        CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL();
        CleanupStack::PushL( tempDev );
        tempDev->InternalizeL( aStream );
        AppendDeviceL( *tempDev );
        CleanupStack::Pop(); 
        }
    }
// --------------------------------------------------------------------------
// CUpnpAVDeviceList::ToDes8L
// (other items were commented in a header).
// --------------------------------------------------------------------------
//  
EXPORT_C HBufC8* CUpnpAVDeviceList::ToDes8L() const
    {
    CBufFlat* buf = CBufFlat::NewL( KBufferGranularity );
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    stream << *this;
    
    HBufC8* hbuf = HBufC8::NewLC( buf->Size() );
    TPtr8 ptr( hbuf->Des() );
    buf->Read( 0, ptr, buf->Size() );
    
    CleanupStack::Pop( hbuf );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    
    return hbuf;
    }

//  End of File
