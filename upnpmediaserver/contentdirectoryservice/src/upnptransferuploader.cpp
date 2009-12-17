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
#include "upnptransferuploader.h"
#include "httpuploader.h"
#include "upnpcontentdirectory.h"
#include "upnpsettings.h"
#include "upnptransferobserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTransferUploader::CUpnpTransferUploader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpTransferUploader::CUpnpTransferUploader( MUpnpTransferObserver* aObserver )
    :CUpnpTransferBase( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpTransferUploader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpTransferUploader::ConstructL()
    {
    iHttpTransferBase = CHttpUploader::NewL( *this, 
                                             CUpnpSettings::GetIapL(), 
                                             KDefaultBuffer, 
                                             KNumOfConcurrentTransactions );  
    }
// -----------------------------------------------------------------------------
// CUpnpTransferUploader::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferUploader* CUpnpTransferUploader::NewLC( 
                                    MUpnpTransferObserver* aObserver )
    {
    CUpnpTransferUploader* self = 
            new (ELeave) CUpnpTransferUploader( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }    
// -----------------------------------------------------------------------------
// CUpnpTransferUploader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferUploader* CUpnpTransferUploader::NewL( 
                                    MUpnpTransferObserver* aObserver )
    {
    CUpnpTransferUploader* self = NewLC( aObserver );
    CleanupStack::Pop();
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpTransferUploader::~CUpnpTransferUploader
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpTransferUploader::~CUpnpTransferUploader()
    {
    }

  
void CUpnpTransferUploader::UploadFileL( const TDesC8& aUri,const TDesC& aTargetPath,const TDesC8& aContentType, TInt aKey)
    {
    _LIT8(KContentFeatures, "DLNA.ORG_PN=");
    CreateAndQueueHttpFileL( aUri, aTargetPath,aKey);  
    SetHeaderL( aKey, UpnpHTTP::KConnection(), UpnpHTTP::KClose() );
    SetHeaderL( aKey, UpnpHTTP::KHdrExpect(), UpnpHTTP::K100Continue() );
    SetHeaderL( aKey, UpnpDLNA::KHdrContentFeatures(), KContentFeatures() );
    SetHeaderL( aKey, UpnpHTTP::KHdrPragma(), UpnpHTTP::KNoCache() );
    SetHeaderL( aKey, UpnpHTTP::KHdrCacheControl(), UpnpHTTP::KNoCache() );        
    SetHeaderL( aKey, UpnpHTTP::KHdrContentType(), aContentType );       
    UploadFileL( aKey );
    }
    
void CUpnpTransferUploader::UploadFileL( TInt aKey )
    {
    iHttpTransferBase->MoveToTransferQueueL( ( TAny * )aKey );
    }


// End of File
