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
* Description: 
*
*/


// INCLUDE FILES
#include "upnptransferhandler.h"
#include "upnpcontentdirectory.h"
#include "upnpfiletransfer.h"
#include "upnptransferdownloader.h"
#include "upnptransferuploader.h"

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::CUpnpTransferHandler
// -----------------------------------------------------------------------------
//
CUpnpTransferHandler::CUpnpTransferHandler( )
    {

    }
// -----------------------------------------------------------------------------
// CUpnpTransferHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::ConstructL()
    {
    iDownloader = CUpnpTransferDownloader::NewL( this );
    iUploader = CUpnpTransferUploader::NewL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferHandler* CUpnpTransferHandler::NewL( )
    {
    CUpnpTransferHandler* self =
            new (ELeave) CUpnpTransferHandler( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler destructor
// -----------------------------------------------------------------------------
//
CUpnpTransferHandler::~CUpnpTransferHandler()
    {
    delete iDownloader;
    delete iUploader;
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::UploadFileL
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::UploadFileL( TInt aKey, const TDesC8& aUri, const TDesC& aTargetPath,
                                        const TDesC8& aContentType )
    {
    if( iUploader )
        {
        iUploader->UploadFileL(aUri, aTargetPath, aContentType, aKey);
        TrackUploadProgress(aKey, ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::DownloadFileL
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::DownloadFileL(TInt aKey, const TDesC8& aUri,const TDesC& aTargetPath)
    {
    if( iDownloader )
        {
        iDownloader->DownloadFileL(aUri, aTargetPath, aKey);
        TrackDownloadProgress(aKey, ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::CancelUpload
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::CancelUpload( TInt aKey )
    {
    if( iUploader )
        {
        iUploader->CancelTransfer( aKey);
        }
    }
// -----------------------------------------------------------------------------
// CUpnpTransferHandler::CancelDownload
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::CancelDownload( TInt aKey )
    {
    if( iDownloader )
        {
        iDownloader->CancelTransfer( aKey);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::HandleUploadL
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::TransferCompletedL(  TInt aKey, TInt aResultCode  )
    {
    iObserver->TransferCompletedL( aKey, aResultCode );
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::SetObserver
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::SetObserver( MUpnpTransferObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::TrackUploadProgress
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::TrackUploadProgress( TInt aKey, TBool aSwitchOn )
    {
    if( iUploader)
        {
        iUploader->TrackProgess(aKey, aSwitchOn);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::TrackDownloadProgress
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::TrackDownloadProgress( TInt aKey, TBool aSwitchOn )
    {
    if( iDownloader)
        {
        iDownloader->TrackProgess(aKey, aSwitchOn);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTransferHandler::TransferProgress
// -----------------------------------------------------------------------------
//
void CUpnpTransferHandler::TransferProgress( TInt aKey, TInt aBytes, TInt aTotalBytes)
    {
    iObserver->TransferProgress(aKey, aBytes, aTotalBytes);
    }

// End of File
