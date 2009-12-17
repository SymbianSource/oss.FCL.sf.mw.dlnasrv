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
* Description:  Implements th upnpfiletransfer class
*
*/



#ifndef C_CUPNPFILETRANSFER_H
#define C_CUPNPFILETRANSFER_H

//  INCLUDES
#include <e32std.h>

#include "upnpcons.h"
#include "upnphttpmessage.h" 

// FORWARD DECLARATIONS
class CUpnpFileTransferEvent;


/**
*  Storage Struct for file transfer
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class CUpnpFileTransfer: public CBase
{
public:
//transfer type
enum TTransferType
    {
    EImport, 
    EExport,
    EUpload
    };

public:
    // Constructors and destructor
    
    /**
    * Two-phased constructor. returns a pointer to the new element.    
    */
    static CUpnpFileTransfer* NewL(TInt aTransferId, TInt aObjectId, TTransferType aType);
    
    /**
    * Two-phased constructor. returns a pointer to the new element.    
    */
    static CUpnpFileTransfer* NewLC(TInt aTransferId, TInt aObjectId, TTransferType aType);

    /**
    * Destructor. Deletes the element from memory. 
    */
    virtual ~CUpnpFileTransfer();
    
    TTransferStatus TransferState( );
    
    void SetTransferState(TTransferStatus aState);
    
    TInt ImportNumber();
    
    void SetImportNumber(TInt aNum);
    
    TInt TransferLength();
    
    void SetTransferLength(TInt aLen);
    
    TInt TransferTotal();
    
    void SetTransferTotal(TInt aTotal);
    
    TTransferType TransferType();
    
    TInt ObjectId();
    
    CUpnpFileTransferEvent* Event();
    
    void SetEvent(CUpnpFileTransferEvent* aEvent);
    
    CUpnpFileTransferEvent* RemoveEvent();
    
    TInt TransferId();
    
    
private:

    /**
    * C++ default constructor.
    */
    CUpnpFileTransfer(TInt aTransferId, TInt aObjectId, TTransferType aType);
    
    void ConstructL();    

public:
    // Data
    
    // Transfer Id
    TInt         iTransferId;

    // Transfer state
    TTransferStatus iTransferState;
    
    //
    TBool        iRemove;
    
    // used for checking if file transfer is Import (GET)
    // or Export (POST). iGet is ETrue if transfer was Import
    TBool        iGet;
    
    TTransferType iType;
    
    // last number in Import uri to match HTTP 200 OK with Import uri
    // number
    TInt         iImportNumber;
    
    TInt         iTransferLength;
    TInt         iTransferTotal;
    // Object id
    TInt         iObjectId;
    
    CUpnpFileTransferEvent* iEvent;
    
};

#endif      // C_CUPNPFILETRANSFER_H   
        
// End of File