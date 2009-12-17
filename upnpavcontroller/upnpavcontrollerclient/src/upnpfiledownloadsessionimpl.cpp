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
* Description:      Implements download session
*
*/






// INTERNAL INCLUDES
// upnp stack api
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpstring.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpfiletransfersessionobserver.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpitemutility.h" // FindAttributeByName
#include "upnpfileutility.h" // FitsInMemory

// avcontroller internal
#include "upnpfiledownloadsessionimpl.h"
#include "upnpavcontrollerclient.h"
#include "upnpfiletransferitem.h"

// EXTERNAL INCLUDES

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::NewL
// Two-phase construction
// --------------------------------------------------------------------------
CUPnPFileDownloadSessionImpl* CUPnPFileDownloadSessionImpl::NewL(
    RUPnPAVControllerClient& aServer, const CUpnpAVDevice& aDevice )
    {
    CUPnPFileDownloadSessionImpl* self = new (ELeave)
        CUPnPFileDownloadSessionImpl( aServer );
    CleanupStack::PushL( self );
    self->iDevice = CUpnpAVDevice::NewL( aDevice );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::CUPnPFileDownloadSessionImpl
// Constructor
// --------------------------------------------------------------------------
CUPnPFileDownloadSessionImpl::CUPnPFileDownloadSessionImpl(
    RUPnPAVControllerClient& aServer ) :
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iBufferPtr( 0, 0 ),
    iBufferPtr2( 0, 0 ),
    iAlive( ETrue ),
    iEventPkg( iEvent )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::~CUPnPFileDownloadSessionImpl
// Destructor
// --------------------------------------------------------------------------
CUPnPFileDownloadSessionImpl::~CUPnPFileDownloadSessionImpl()
    {
    __LOG( "CUPnPFileDownloadSessionImpl::~CUPnPFileDownloadSessionImpl" );
    
    CancelAllTransfers();
    
    Cancel();
    iServer.DestroyDownloadSession( (TInt)this );
    
    delete iDevice;
    delete iBuffer;
    delete iBuffer2;
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::ConstructL
// Two-phase construction
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::ConstructL()
    {
    __LOG( "CUPnPFileDownloadSessionImpl::ConstructL" );
    
    iBuffer = iDevice->Uuid().AllocL();
    iBufferPtr.Set( iBuffer->Des() );
    User::LeaveIfError( iServer.CreateDownloadSession(
        (TInt)this , iBufferPtr ) );
    
    iBuffer2 = HBufC::NewL( KMaxPath );
    iBufferPtr2.Set( iBuffer2->Des() );    
    iServer.GetDownloadEvent( (TInt)this, iEventPkg, iStatus, iBufferPtr2 );
    SetActive();
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::ConstructL
// From CActive
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::RunL()
    {
    __LOG( "CUPnPFileDownloadSessionImpl::RunL" );

    TUpnpFileTransferEvent event = iEvent;

    if( iStatus.Int() == KErrNone )
        {
        __LOG( "RunL - Activate again" );

        iServer.GetDownloadEvent( (TInt)this, iEventPkg, iStatus,
            iBufferPtr2 );
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
                __LOG( "RunL - ETransferStarted" );
                
                // iKey - 1 since it's incremented when we request the
                // download. The reason behind of this is the fact that
                // httptransfer does not accept value 0 which is legal from
                // the API point of view
                iObserver->TransferStarted( event.iKey - 1,
                    event.iStatus );
                }
            break;
            }

        case TUpnpFileTransferEvent::ETransferCompleted:
            {
            if( iObserver )
                {
                __LOG( "RunL - ETransferCompleted" );
                
                iObserver->TransferCompleted( event.iKey - 1,
                    event.iStatus, *iBuffer2 );   
                }                  
            break;
            }
            
        case TUpnpFileTransferEvent::ETransferProgress:
            {
            if( iObserver )
                {
                __LOG( "RunL - ETransferProgress" );
                
                iObserver->TransferProgress( event.iKey - 1,
                    event.iParam1, event.iParam2 );
                }                  
            break;
            }
            
        case TUpnpFileTransferEvent::EDeviceDisconnected:
            {
            CUpnpAVDevice* tmpDev = NULL;
            UPnPDeviceDisappeared( *tmpDev );
            break;
            }
            
        default:
            {
            __LOG( "RunL - default!" );      
            break;
            }
        
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::DoCancel
// From CActive
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::DoCancel()
    {
    __LOG( "CUPnPFileDownloadSessionImpl::DoCancel" );
    
    iServer.CancelDownloadEvent( (TInt)this );
    
    __LOG( "CUPnPFileDownloadSessionImpl::DoCancel - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::RunError
// From CActive
// --------------------------------------------------------------------------
TInt CUPnPFileDownloadSessionImpl::RunError( TInt /*aError*/ )
    {
    __LOG( "CUPnPFileDownloadSessionImpl::RunError" );
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::StartDownloadL
// From CActive
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::StartDownloadL(
    const CUpnpItem& aItem, TInt aKey )
    {
    __LOG( "CUPnPFileDownloadSessionImpl::StartDownloadL" );
    
    ResetL();

    CUpnpFileTransferItem* item = CUpnpFileTransferItem::NewLC();
    item->SetTitleL( aItem.Title() );
    // aKey + 1 
    // the reason behind of this is the fact that httptransfer
    // does not accept value 0 which is legal from the API point
    // of view.    
    item->SetKey( aKey + 1 );

    // Get the most suitable (original) res-element from the item
    const CUpnpElement& tmpEl = UPnPItemUtility::ResourceFromItemL( 
        aItem );
    
    if( !UPnPFileUtility::FitsInMemory( tmpEl ) )
        {
        User::Leave( KErrDiskFull );
        }

    item->SetUriL( tmpEl.Value() );    

    // Get the protocolinfo-attribute
    const CUpnpAttribute* tmpPInfo = UPnPItemUtility::FindAttributeByName(
        tmpEl, KAttributeProtocolInfo );
    if( tmpPInfo )
        {
        item->SetProtocolInfoL( tmpPInfo->Value() );
        }
    else
        {
        User::Leave( KErrArgument );
        }    

    iBuffer = item->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    CleanupStack::PopAndDestroy( item );
    
    User::LeaveIfError( iServer.StartDownload( (TInt)this, iBufferPtr ) );
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::StartDownloadL
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::StartDownloadL( 
    const CUpnpElement& aResElement, const CUpnpItem& aItem,
    RFile& aFile, TInt aKey )
    {
    __LOG( "CUPnPFileDownloadSessionImpl::StartDownloadL" );
    
    ResetL();

    CUpnpFileTransferItem* item = CUpnpFileTransferItem::NewLC();
    item->SetTitleL( aItem.Title() );
    item->SetUriL( aResElement.Value() );
    item->SetKey( aKey + 1 );

    const CUpnpAttribute* pInfo = UPnPItemUtility::FindAttributeByName(
        aResElement, KAttributeProtocolInfo );
    if( pInfo )
        {
        item->SetProtocolInfoL( pInfo->Value() );
        }
    else
        {
        User::Leave( KErrArgument );
        }   

    iBuffer = item->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    CleanupStack::PopAndDestroy( item );
    
    User::LeaveIfError( iServer.StartDownload( (TInt)this, iBufferPtr,
        aFile ) );
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::SetObserver
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::SetObserver(
    MUPnPFileTransferSessionObserver& aObserver )
    {
    __LOG( "CUPnPFileDownloadSessionImpl::SetObserver" );
    
    iObserver = &aObserver;
    }
     
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::RemoveObserver
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::RemoveObserver()
    {
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::Observer
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
MUPnPFileTransferSessionObserver* CUPnPFileDownloadSessionImpl::Observer()
    const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::StartTrackingProgressL
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::StartTrackingProgressL( TInt aKey )
    {
    ResetL();
    User::LeaveIfError( iServer.StartTrackingDownloadProgress(
        (TInt)this, aKey + 1 ) );
                   
    }
    
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::CancelTransfer
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::CancelTransfer( TInt aKey )
    {
    iServer.CancelDownload( (TInt)this, aKey + 1 ); // Ignore error
    }
    
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::CancelAllTransfers
// See upnpfiledownloadsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::CancelAllTransfers()
    {
    iServer.CancelAllDownloads( (TInt)this ); // Ignore error
    }

// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::UPnPDeviceDiscovered
// Device discovered
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::UPnPDeviceDiscovered(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    // No implementation needed
    }

  
// --------------------------------------------------------------------------
// CUPnPFileUploadSessionImpl::UPnPDeviceDisappeared
// Device disappeared
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::UPnPDeviceDisappeared(
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
void CUPnPFileDownloadSessionImpl::WLANConnectionLost()
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
// CUPnPFileDownloadSessionImpl::Device
// Returns used device
// --------------------------------------------------------------------------
const CUpnpAVDevice& CUPnPFileDownloadSessionImpl::Device() const
    {
    return *iDevice;
    }
    
// --------------------------------------------------------------------------
// CUPnPFileDownloadSessionImpl::ResetL
// Reset
// --------------------------------------------------------------------------
void CUPnPFileDownloadSessionImpl::ResetL()
    {
    __LOG( "CUPnPFileDownloadSessionImpl::ResetL" );
    
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }

    delete iBuffer; iBuffer = NULL;
    }    

// end of file
