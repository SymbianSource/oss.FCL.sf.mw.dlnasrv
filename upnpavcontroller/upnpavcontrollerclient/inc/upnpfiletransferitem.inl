/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "upnpfiletransferitem.h"

// CONSTANTS
const TInt KBrowseBufferGranularity = 100;    

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::CUpnpFileTransferItem
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
inline CUpnpFileTransferItem::CUpnpFileTransferItem()
    {    
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::ConstructL()
    {
    iUri = KNullDesC8().AllocL();
    iTitle = KNullDesC8().AllocL();
    iProtocolInfo = KNullDesC8().AllocL();
    iPath = KNullDesC().AllocL();
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpFileTransferItem* CUpnpFileTransferItem::NewL()
    {
    CUpnpFileTransferItem* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpFileTransferItem* CUpnpFileTransferItem::NewLC()
    {
    CUpnpFileTransferItem* self = new( ELeave )
        CUpnpFileTransferItem;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline CUpnpFileTransferItem::~CUpnpFileTransferItem()
    {
    delete iUri;
    delete iTitle;
    delete iProtocolInfo;
    delete iPath;         
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::ExternalizeL( RWriteStream& aStream ) const
    {    
    aStream.WriteInt16L( iUri->Length() ); 
    aStream << *iUri; 
    
    aStream.WriteInt16L( iTitle->Length() );
    aStream << *iTitle;
    
    aStream.WriteInt16L( iProtocolInfo->Length() );
    aStream << *iProtocolInfo;     

    aStream.WriteInt16L( iPath->Length() );
    aStream << *iPath;  
    
    aStream.WriteInt16L( iKey );
    //aStream.WriteL( (TUint8*)(&iEvent), 
    //    sizeof(TUpnpFileTransferEvent) );
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::InternalizeL
// Internalizes object
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::InternalizeL( RReadStream& aStream )
    {
    TInt length = aStream.ReadInt16L();     
    delete iUri; iUri = NULL;
    iUri = HBufC8::NewL( aStream, length );
    
    length = aStream.ReadInt16L();
    delete iTitle; iTitle = NULL;
    iTitle = HBufC8::NewL( aStream, length );

    length = aStream.ReadInt16L();
    delete iProtocolInfo; iProtocolInfo = NULL;
    iProtocolInfo = HBufC8::NewL( aStream, length );  

    length = aStream.ReadInt16L();
    delete iPath; iPath = NULL;
    iPath = HBufC::NewL( aStream, length );
    
    iKey = aStream.ReadInt16L();
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::ToDes8L
// Externalizes object to a heap descriptor
// --------------------------------------------------------------------------
inline HBufC8* CUpnpFileTransferItem::ToDes8L() const
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
// CUpnpFileTransferItem::SetUriL
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::SetUriL( const TDesC8& aUri )
    {
    HBufC8* tmp = aUri.AllocL();
    delete iUri;
    iUri = tmp;
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::Uri
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpFileTransferItem::Uri() const
    {
    if( iUri )
        {
        return *iUri;
        }
    else
        {
        return KNullDesC8;
        }    
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferItem::SetTitleL
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::SetTitleL( const TDesC8& aTitle )
    {
    HBufC8* tmp = aTitle.AllocL();
    delete iTitle;
    iTitle = tmp;
    }
        
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::Title
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpFileTransferItem::Title() const
    {
    if( iTitle )
        {
        return *iTitle;
        }
    else
        {
        return KNullDesC8;
        }    
    }
        
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::SetProtocolInfoL
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::SetProtocolInfoL(
    const TDesC8& aProtocolInfo )
    {
    HBufC8* tmp = aProtocolInfo.AllocL();
    delete iProtocolInfo;
    iProtocolInfo = tmp;
    }
        
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::ProtocolInfo
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline const TDesC8& CUpnpFileTransferItem::ProtocolInfo() const
    {
    if( iProtocolInfo )
        {
        return *iProtocolInfo;
        }
    else
        {
        return KNullDesC8;
        }    
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::SetPathL
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::SetPathL( const TDesC& aPath )
    {
    HBufC* tmp = aPath.AllocL();
    delete iPath;
    iPath = tmp;
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::Path
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline const TDesC& CUpnpFileTransferItem::Path() const
    {
    if( iPath )
        {
        return *iPath;
        }
    else
        {
        return KNullDesC;
        }    
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::SetKey
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline void CUpnpFileTransferItem::SetKey( TInt aKey )
    {
    iKey = aKey;
    }
    
// --------------------------------------------------------------------------
// CUpnpFileTransferItem::Key
// See upnpfiletransferitem.h
// --------------------------------------------------------------------------
inline TInt CUpnpFileTransferItem::Key()
    {
    return iKey;
    }
    
// End of File
