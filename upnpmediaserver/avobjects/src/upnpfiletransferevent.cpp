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
* Description:  Upnp Media Server file transfer event
*
*/

#include "upnpfiletransferevent.h"


// ---------------------------------------------------------------------------
// CUpnpFileTransferEvent::CUpnpFileTransferEvent
// ---------------------------------------------------------------------------
//
CUpnpFileTransferEvent::CUpnpFileTransferEvent(): 
                            CUpnpMediaServerEvent(0)
    {    
    }   

// ---------------------------------------------------------------------------
// CUpnpFileTransferEvent::~CUpnpFileTransferEvent
// ---------------------------------------------------------------------------
//
CUpnpFileTransferEvent::~CUpnpFileTransferEvent()
    {
    delete iFilePath;
    }


// ---------------------------------------------------------------------------
// CUpnpFileTransferEvent::ConstructL
// ---------------------------------------------------------------------------
//
void CUpnpFileTransferEvent::ConstructL()
    {
    iFilePath = HBufC::NewL(0);
    }
   
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpFileTransferEvent* CUpnpFileTransferEvent::NewLC()
    {
    CUpnpFileTransferEvent* self = new( ELeave ) CUpnpFileTransferEvent;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpFileTransferEvent* CUpnpFileTransferEvent::NewL()
    {
    CUpnpFileTransferEvent* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }    

// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::ProgressState
// -----------------------------------------------------------------------------
//   
EXPORT_C CUpnpFileTransferEvent::TProgressState CUpnpFileTransferEvent::ProgressState()
    {
    return (TProgressState) iProgress;
    }
 

// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetProgressState
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpFileTransferEvent::SetProgressState( TProgressState aState )
    {
    iProgress = aState;
    }
    
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::Direction
// -----------------------------------------------------------------------------
//      
EXPORT_C CUpnpFileTransferEvent::TDirection CUpnpFileTransferEvent::Direction()
    {
    return (TDirection) iDirection;    
    }
    
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetDirection
// -----------------------------------------------------------------------------
//       
EXPORT_C void CUpnpFileTransferEvent::SetDirection( TDirection aDirection )
    {
    iDirection = aDirection;            
    }
 
    
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::TransferId
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CUpnpFileTransferEvent::TransferId()
    {
    return iTransferId;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetTransferId
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpFileTransferEvent::SetTransferId( TInt aTransferId )
    {
    iTransferId = aTransferId;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::ContentDirectoryObjectId
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CUpnpFileTransferEvent::ContentDirectoryObjectId()
    {
    return iContentDirectoryObjectId;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetContentDirectoryObjectId
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpFileTransferEvent::SetContentDirectoryObjectId( TInt aObjectId )
    {
    iContentDirectoryObjectId = aObjectId;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::FilePath
// -----------------------------------------------------------------------------
//   
EXPORT_C TDesC& CUpnpFileTransferEvent::FilePath()
    {
    return *iFilePath;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetFilePath
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpFileTransferEvent::SetFilePathL(const TDesC& aFilePath)
    {
    delete iFilePath;
    iFilePath = NULL;
    iFilePath = aFilePath.AllocL();
    }
    

// -----------------------------------------------------------------------------
//  CUpnpFileTransferEvent::ErrorCode
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CUpnpFileTransferEvent::ErrorCode()
    {
    return iErrorCode;
    }

    
    
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::SetErrorCode
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpFileTransferEvent::SetErrorCode( TInt aError )
    {
    iErrorCode = aError;
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::CloneL
// -----------------------------------------------------------------------------
//   
EXPORT_C CUpnpFileTransferEvent* CUpnpFileTransferEvent::CloneL()
    {
    CUpnpFileTransferEvent* event = CUpnpFileTransferEvent::NewLC();
    event->SetProgressState( ( TProgressState ) iProgress);
    event->SetDirection( ( TDirection ) iDirection);
    event->SetFilePathL(*iFilePath);
    event->SetContentDirectoryObjectId(iContentDirectoryObjectId);
    event->SetTransferId(iTransferId);
    event->SetErrorCode(iErrorCode);
    CleanupStack::Pop(event);
    return event;
    }



// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEvent::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream << *iFilePath;
    aStream.WriteInt32L( iProgress );
    aStream.WriteInt32L( iDirection );    
    aStream.WriteInt32L( iContentDirectoryObjectId );
    aStream.WriteInt32L( iTransferId );
    aStream.WriteInt32L( iErrorCode );
                
    }
        
// -----------------------------------------------------------------------------
// CUpnpFileTransferEvent::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEvent::InternalizeL( RReadStream& aStream )
    {
     // Filter
    delete iFilePath;
    iFilePath = NULL;
    iFilePath = HBufC16::NewL( aStream, 256 );
    
    iProgress =  aStream.ReadInt32L();
    iDirection = aStream.ReadInt32L();
    iContentDirectoryObjectId = aStream.ReadInt32L();
    iTransferId = aStream.ReadInt32L();
    iErrorCode = aStream.ReadInt32L();
    }
