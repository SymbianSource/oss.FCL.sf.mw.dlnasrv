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
* Description:      implements upload session
*
*/






// INTERNAL INCLUDES
// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpfiletransfersessionobserver.h"

// avcontroller internal
#include "upnpfileuploadsessionimpl.h"
#include "upnpavcontrollerclient.h"
#include "upnpfiletransferitem.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::NewL
// Two-phase construction
// --------------------------------------------------------------------------
CUPnPFileUploadSessionImpl* CUPnPFileUploadSessionImpl::NewL(
    RUPnPAVControllerClient& aServer, const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPFileUploadSessionImpl::NewL" );
    
    CUPnPFileUploadSessionImpl* self = new (ELeave)
        CUPnPFileUploadSessionImpl( aServer );
    CleanupStack::PushL( self );
    self->iDevice = CUpnpAVDevice::NewL( aDevice );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::CUPnPFileUploadSessionImpl
// Constructor
// --------------------------------------------------------------------------
CUPnPFileUploadSessionImpl::CUPnPFileUploadSessionImpl(
    RUPnPAVControllerClient& aServer ) :
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iBufferPtr( 0, 0 ),
    iAlive( ETrue ),
    iEventPkg( iEvent )
    {
    __LOG( "CUPnPFileUploadSessionImpl::NewL" );
    
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::~CUPnPFileUploadSessionImpl
// Destructor
// --------------------------------------------------------------------------
CUPnPFileUploadSessionImpl::~CUPnPFileUploadSessionImpl()
    {
    __LOG( "CUPnPFileUploadSessionImpl::~CUPnPFileUploadSessionImpl" );
    
    CancelAllTransfers();
    
    Cancel();
    iServer.DestroyUploadSession( (TInt)this );
    
    delete iDevice;
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::ConstructL
// Two-phase construction
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::ConstructL()
    {
    __LOG( "CUPnPFileUploadSessionImpl::ConstructL" );
    
    iBuffer = iDevice->Uuid().AllocL();
    iBufferPtr.Set( iBuffer->Des() );
    User::LeaveIfError( iServer.CreateUploadSession(
        (TInt)this , iBufferPtr ) );

    iServer.GetUploadEvent( (TInt)this, iEventPkg, iStatus );
    SetActive();
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::ConstructL
// Two-phase construction
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::RunL()
    {
    __LOG( "CUPnPFileUploadSessionImpl::RunL" );

    TUpnpFileTransferEvent event = iEvent;    

    if( iStatus.Int() == KErrNone )
        {
        iServer.GetUploadEvent( (TInt)this, iEventPkg, iStatus );
        SetActive();        
        }
    else
        {
        __LOG1( "RunL - %d", iStatus.Int() );      
        }      

    // Handle event and activate again
    switch( event.iEvent )
        {
        case TUpnpFileTransferEvent::ETransferStarted:
            {
            if( iObserver )
                {
                __LOG2( "RunL - ETransferStarted, key = %d, status = %d",
                    event.iKey, event.iStatus );
                
                // iKey - 1 since it's incremented when we request the upload
                // the reason behind of this is the fact that httptransfer
                // does not accept value 0 which is legal from the API point
                // of view
                iObserver->TransferStarted( event.iKey - 1,
                    event.iStatus );
                }
            break;
            }
            
        case TUpnpFileTransferEvent::ETransferCompleted:
            {
            if( iObserver )
                {
                __LOG2( "RunL - ETransferCompleted,key = %d, status = %d",
                    event.iKey, event.iStatus );
                    
                iObserver->TransferCompleted( event.iKey - 1,
                    event.iStatus, KNullDesC );
                }                  
            break;
            }
            
        case TUpnpFileTransferEvent::ETransferProgress:
            {
            if( iObserver )
                {
                __LOG1( "RunL - ETransferProgress, status = %d",
                    event.iStatus );
                    
                iObserver->TransferProgress( event.iKey - 1,
                    event.iParam1, event.iParam2 );
                }                  
            break;
            }

        case TUpnpFileTransferEvent::EDeviceDisconnected:
            {
            __LOG( "RunL - EDeviceDisconnected" );

            CUpnpAVDevice* tmpDev = NULL;
            UPnPDeviceDisappeared( *tmpDev );
            }
            break;
            
        default:
            {
            __LOG( "RunL - default!" ); 
            break;     
            }
        }        
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::ConstructL
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::StartUploadL( const TDesC& aFilePath,
    TInt aKey )
    {
    __LOG( "CUPnPFileUploadSessionImpl::StartUploadL" );
    
    ResetL();

    CUpnpFileTransferItem* item = CUpnpFileTransferItem::NewLC();
    // aKey + 1 
    // the reason behind of this is the fact that httptransfer
    // does not accept value 0 which is legal from the API point
    // of view.
    item->SetKey( aKey + 1 );
    item->SetPathL( aFilePath );   

    iBuffer = item->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    CleanupStack::PopAndDestroy( item );
    
    User::LeaveIfError( iServer.StartUpload( (TInt)this, iBufferPtr ) );    
    }
    
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::DoCancel
// From CActive
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::DoCancel()
    {
    __LOG( "CUPnPFileUploadSessionImpl::DoCancel" );
    
    iServer.CancelUploadEvent( (TInt)this );
    
    __LOG( "CUPnPFileUploadSessionImpl::DoCancel - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::RunError
// From CActive
// --------------------------------------------------------------------------
TInt CUPnPFileUploadSessionImpl::RunError( TInt /*aError*/ )
    {
    __LOG( "CUPnPFileUploadSessionImpl::RunError" );
    
    return KErrNone;
    }

   
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::SetObserver
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::SetObserver(
    MUPnPFileTransferSessionObserver& aObserver )
    {
    __LOG( "CUPnPFileUploadSessionImpl::SetObserver" );
    
    iObserver = &aObserver;
    }
     
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::RemoveObserver
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::RemoveObserver()
    {
    __LOG( "CUPnPFileUploadSessionImpl::RemoveObserver" );
    
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::Observer
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
MUPnPFileTransferSessionObserver*
    CUPnPFileUploadSessionImpl::Observer() const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::StartTrackingProgressL
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::StartTrackingProgressL( TInt aKey )
    {
    __LOG( "CUPnPFileUploadSessionImpl::StartTrackingProgressL" );
    
    ResetL();
    User::LeaveIfError( iServer.StartTrackingUploadProgress(
        (TInt)this, aKey + 1 ) );    
    }
    
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::CancelTransfer
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::CancelTransfer( TInt aKey )
    {
    __LOG( "CUPnPFileUploadSessionImpl::CancelTransfer" );
    
    iServer.CancelUpload( (TInt)this, aKey + 1 ); // Ignore error
    }
    
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::CancelAllTransfers
// See upnpfileuploadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::CancelAllTransfers()
    {
    __LOG( "CUPnPFileUploadSessionImpl::CancelAllTransfers" );
    
    iServer.CancelAllUploads( (TInt)this ); // Ignore error
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::UPnPDeviceDiscovered
// Device discovered
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::UPnPDeviceDiscovered(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    // No implementation needed
    }

  
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::UPnPDeviceDisappeared
// Device disappeared
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::UPnPDeviceDisappeared(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::UPnPDeviceDisappeared" );
    
    iAlive = EFalse;
    if( iObserver )
        {
        iObserver->MediaServerDisappeared( 
            MUPnPAVSessionObserverBase::EDisconnected );
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::WLANConnectionLost
// Connection lost
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::WLANConnectionLost()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::WLANConnectionLost" );
    
    iAlive = EFalse;
    if( iObserver )    
        {
        iObserver->MediaServerDisappeared( 
            MUPnPAVSessionObserverBase::EWLANLost );
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::Device
// Returns used device
// --------------------------------------------------------------------------
const CUpnpAVDevice& CUPnPFileUploadSessionImpl::Device() const
    {
    return *iDevice;
    }
    
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::ResetL
// Reset
// --------------------------------------------------------------------------
void CUPnPFileUploadSessionImpl::ResetL()
    {
    __LOG( "CUPnPFileUploadSessionImpl::ResetL" );
    
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }

    delete iBuffer; iBuffer = NULL;
    }    

// end of file

