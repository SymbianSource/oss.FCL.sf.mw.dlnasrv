/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class for sending HTTP request using Symbian HTTP client 
*
*/


// INCLUDE FILES
#include "upnptransferbase.h"
#include "upnpcontentdirectory.h"
#include "httptransferbase.h"
#include "upnptransferobserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTransferBase::CUpnpTransferBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpTransferBase::CUpnpTransferBase( MUpnpTransferObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CUpnpTransferBase::~CUpnpTransferBase
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpTransferBase::~CUpnpTransferBase()
    {
    if ( iHttpTransferBase ) 
        {
        delete iHttpTransferBase;
        }
    }

TInt CUpnpTransferBase::MapStatus( TInt aStatus )
    {   
    switch ( aStatus ) 
        {
        case KErrNone:
            return EHttpOk;
        }
    // default
    return aStatus;        
    }
    
void CUpnpTransferBase::CreateAndQueueHttpFileL( const TDesC8& aUri,
                                                 const TDesC& aTargetPath, 
                                                 TInt aKey )
    {
    iHttpTransferBase->InsertFileIntoWaitQueueL( ( TAny* ) aKey, 
                                                 aTargetPath,
                                                 aUri );
    }
    
void CUpnpTransferBase::SetHeaderL( TInt aKey,
                                    const TDesC8& aFieldName, 
                                    const TDesC8& aFieldValue )
    {
    iHttpTransferBase->SetHeaderL( ( TAny * )aKey, aFieldName, aFieldValue );
    }


void CUpnpTransferBase::TransferCompleted( TAny* aKey, TInt aStatus )
    {   
    TInt contentDirStatus = MapStatus( aStatus );        
    TRAP_IGNORE( iObserver->TransferCompletedL( (TInt) aKey, contentDirStatus ) );
    }
void CUpnpTransferBase::TrackProgess(TInt aKey, TBool aSwitchOn)
    {
    iHttpTransferBase->TrackProgress( (TAny*)aKey, aSwitchOn);
    }
    
void CUpnpTransferBase::TransferProgress( TAny* aKey, 
                                           TInt aBytes,
                                           TInt aTotalBytes )
    {   
    iObserver->TransferProgress( (TInt)aKey, aBytes, aTotalBytes);    
    }    

void CUpnpTransferBase::ReadyForTransferL(TAny* aKey)
    {
    TRAPD(err, iHttpTransferBase->StartTransferL( aKey ) );
    if (err != KErrNone)
        {
        TransferCompleted(aKey, err);
        }
    }
void CUpnpTransferBase::CancelTransfer(TInt aKey)
    {
    iHttpTransferBase->CancelTransfer( (TAny*) aKey ) ;
    }
 
// End of File
