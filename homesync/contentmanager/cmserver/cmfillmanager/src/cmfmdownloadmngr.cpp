/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Download manager class, handles file download events
*
*/


#include "upnpavcontroller.h"
#include "upnpfiledownloadsession.h"
#include "cmfmtransferobserver.h"
#include "cmfmdownloadmngr.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::NewL
// ---------------------------------------------------------------------------
//
CCmFmDownloadMngr* CCmFmDownloadMngr::NewL( MUPnPAVController* aAVController, 
    MCmFmTransferObserver* aObserver, const CUpnpAVDevice& aDevice,
    const TInt aDownloadId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::NewL() start"));    
    CCmFmDownloadMngr* self = CCmFmDownloadMngr::NewLC( aAVController, 
        aObserver, aDevice, aDownloadId );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::NewLC
// ---------------------------------------------------------------------------
//   
CCmFmDownloadMngr* CCmFmDownloadMngr::NewLC( MUPnPAVController* aAVController, 
    MCmFmTransferObserver* aObserver, const CUpnpAVDevice& aDevice,
    const TInt aDownloadId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::NewLC() start"));    
    CCmFmDownloadMngr* self = 
        new ( ELeave ) CCmFmDownloadMngr( aAVController, 
            aObserver, aDownloadId );
    CleanupStack::PushL( self );
    self->ConstructL( aDevice );
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::~CCmFmDownloadMngr
// ---------------------------------------------------------------------------
//
CCmFmDownloadMngr::~CCmFmDownloadMngr()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::~CCmFmDownloadMngr()"));
    CancelOperation();
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::CCmFmDownloadMngr
// ---------------------------------------------------------------------------
//        
CCmFmDownloadMngr::CCmFmDownloadMngr( MUPnPAVController* aAVController, 
    MCmFmTransferObserver* aObserver, const TInt aDownloadId )
    : iAVController( aAVController ), iObserver( aObserver ), 
      iDownloadId( aDownloadId ) 
      
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::CCmFmDownloadMngr()"));
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::ConstructL
// ---------------------------------------------------------------------------
// 
void CCmFmDownloadMngr::ConstructL( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::ConstructL()"));
    
    iDownloadSession = &( iAVController->
            StartDownloadSessionL( aDevice ) );
    iDownloadSession->SetObserver( *this );
    }    

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::FetchFileL
// ---------------------------------------------------------------------------
//  
void CCmFmDownloadMngr::FetchFileL( const CUpnpElement& aResElement, 
    const CUpnpItem& aItem, RFile& aFile, const TInt64 aInternalItemId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::FetchFileL()"));
    
    iDownloadSession->StartDownloadL( aResElement, aItem, 
        aFile, aInternalItemId );
    iDownloadSession->StartTrackingProgressL( aInternalItemId );                    
    iFileSize = 0;            
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::CancelOperation
// ---------------------------------------------------------------------------
//     
void CCmFmDownloadMngr::CancelOperation()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::CancelOperation() start"));
    if( iAVController )
        {
        if( iDownloadSession )
            {
            iAVController->StopDownloadSession( *iDownloadSession );
            }
        }
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::CancelOperation() end"));                
    }                

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::TransferStarted
// ---------------------------------------------------------------------------
//
void CCmFmDownloadMngr::TransferStarted( TInt aKey, TInt aStatus )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::TransferStarted()"));
    iObserver->TransferStatus( ECmFmFileFetchStarted, iDownloadId, aKey, 
        aStatus, iFileSize );       
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::TransferCompleted
// ---------------------------------------------------------------------------
//
void CCmFmDownloadMngr::TransferCompleted( TInt aKey, TInt aStatus,
        const TDesC& /*aFilePath*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadMngr::TransferCompleted()"));
    iObserver->TransferStatus( ECmFmFileFetched, iDownloadId, aKey, aStatus, 
        iFileSize );           
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::TransferProgress
// ---------------------------------------------------------------------------
//
void CCmFmDownloadMngr::TransferProgress( TInt /*aKey*/, TInt aBytes,
        TInt /*aTotalBytes*/ )              
    {
    TRACE(Print(_L("[FILL MNGR]\t aBytes= %d"), aBytes ));

	// Get the transferred bytes
    iFileSize = ( aBytes / KKilo );
    } 

// ---------------------------------------------------------------------------
// CCmFmDownloadMngr::MediaServerDisappeared
// ---------------------------------------------------------------------------
//
void CCmFmDownloadMngr::MediaServerDisappeared( 
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    TRACE(Print(_L("[FILL MNGR]\t MediaServerDisappeared = %d"), iDownloadId ));
    iObserver->TransferStatus( ECmFmFileFetched, iDownloadId, KErrNone, 
        KErrDisconnected, KErrNone );
    }
    
// End of file


