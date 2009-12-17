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
#include "upnptransferdownloader.h"
#include "httpdownloader.h"
#include "upnpcontentdirectory.h"
#include "upnpsettings.h"
#include "upnptransferobserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::CUpnpTransferDownloader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpTransferDownloader::CUpnpTransferDownloader( 
                                   MUpnpTransferObserver* aObserver )
    : CUpnpTransferBase( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpTransferDownloader::ConstructL()
    {
    iHttpTransferBase = CHttpDownloader::NewL( *this, 
                                               CUpnpSettings::GetIapL(), 
                                               KDefaultBuffer, 
                                               KNumOfConcurrentTransactions );  
    }

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferDownloader* CUpnpTransferDownloader::NewLC( MUpnpTransferObserver* aCd )
    {
    CUpnpTransferDownloader* self = new (ELeave) CUpnpTransferDownloader( aCd );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }    

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferDownloader* CUpnpTransferDownloader::NewL( MUpnpTransferObserver* aCd )
    {
    CUpnpTransferDownloader* self = NewLC( aCd );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::~CUpnpTransferDownloader
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpTransferDownloader::~CUpnpTransferDownloader()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::DownloadFileL
// -----------------------------------------------------------------------------
//
void CUpnpTransferDownloader::DownloadFileL( const TDesC8& aUri,
                                             const TDesC& aTargetPath, 
                                             TInt aKey )
    {
    CreateAndQueueHttpFileL( aUri, aTargetPath,aKey);  
    SetHeaderL( aKey, UpnpHTTP::KConnection(), UpnpHTTP::KClose() );
    DownloadFileL( aKey );    
    }
    
// -----------------------------------------------------------------------------
// CUpnpTransferDownloader::DownloadFileL
// -----------------------------------------------------------------------------
//
void CUpnpTransferDownloader::DownloadFileL( TInt aKey )
    {
    iHttpTransferBase->MoveToTransferQueueL( ( TAny * )aKey );
    }


// End of File
