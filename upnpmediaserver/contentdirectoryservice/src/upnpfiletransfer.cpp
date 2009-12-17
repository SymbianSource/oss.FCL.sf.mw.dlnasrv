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
* Description:  File transfer
*
*/


// INCLUDE FILES
#include "upnpfiletransfer.h"
#include "upnpfiletransferevent.h"

// -----------------------------------------------------------------------------
// CUpnpFileTransfer::CUpnpFileTransfer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpFileTransfer::CUpnpFileTransfer( TInt aTransferId, 
                                      TInt aObjectId, 
                                      TTransferType aType):
        iTransferId(aTransferId),
        iType(aType),
        iTransferLength(0),
        iTransferTotal(0),
        iObjectId(aObjectId)
    {
    }


// -----------------------------------------------------------------------------
// CUpnpFileTransfer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::ConstructL( )
    {   
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CUpnpFileTransfer::~CUpnpFileTransfer()
    {   
    delete iEvent;
    }    
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpFileTransfer* CUpnpFileTransfer::NewL(TInt aTransferId, TInt aObjectId, TTransferType aType)
    {
    CUpnpFileTransfer* self = new (ELeave) CUpnpFileTransfer( aTransferId, aObjectId, aType);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpFileTransfer* CUpnpFileTransfer::NewLC(TInt aTransferId, TInt aObjectId, TTransferType aType)
    {
    CUpnpFileTransfer* self = new (ELeave) CUpnpFileTransfer( aTransferId, aObjectId, aType);
    CleanupStack::PushL( self );
    self->ConstructL(  );    
    return self;
    }    
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferState
// 
// -----------------------------------------------------------------------------
//
TTransferStatus CUpnpFileTransfer::TransferState( )
    {   
    return iTransferState;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::SetTransferState
// 
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::SetTransferState(TTransferStatus aState )
    {   
    iTransferState = aState;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::ImportNumber
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransfer::ImportNumber( )
    {   
    return iImportNumber;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::SetImportNumber
// 
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::SetImportNumber(TInt aNum )
    {   
    iImportNumber = aNum;
    }

// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferLength
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransfer::TransferLength( )
    {   
    return iTransferLength;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferLength
// 
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::SetTransferLength(TInt aLen)
    {   
    iTransferLength = aLen;
    }           
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferLength
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransfer::TransferTotal( )
    {   
    return iTransferTotal;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferLength
// 
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::SetTransferTotal(TInt aTotal)
    {   
    iTransferTotal = aTotal;
    }  
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferType
// 
// -----------------------------------------------------------------------------
//
CUpnpFileTransfer::TTransferType CUpnpFileTransfer::TransferType()
    {   
    return iType;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::ObjectId
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransfer::ObjectId()
    {   
    return iObjectId;
    } 
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::TransferID
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransfer::TransferId()
    {   
    return iTransferId;
    }     
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::Event
// 
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEvent* CUpnpFileTransfer::Event()
    {   
    return iEvent;
    }                     
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::SetEvent
// 
// -----------------------------------------------------------------------------
//
void CUpnpFileTransfer::SetEvent(CUpnpFileTransferEvent* aEvent)
    {   
    if(iEvent)
        {
        delete iEvent;
        }
    iEvent = aEvent;
    }      
// -----------------------------------------------------------------------------
// CUpnpFileTransfer::RemoveEvent
// 
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEvent* CUpnpFileTransfer::RemoveEvent()
    {   
    CUpnpFileTransferEvent* event = iEvent;    
    iEvent = NULL;
    
    return event;
    }      
    
//end of file
