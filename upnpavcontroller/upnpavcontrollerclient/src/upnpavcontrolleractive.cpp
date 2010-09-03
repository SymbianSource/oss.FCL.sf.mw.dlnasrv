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
* Description:      AO that monitors for device events
*
*/






// INCLUDES
// upnp stack api
#include <upnpsettings.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpavdeviceobserver.h"
#include "upnpavdevicelist.h"

// avcontroller internal
#include "upnpavcontrolleractive.h"
#include "upnpavrenderingsessionimpl.h"
#include "upnpavbrowsingsessionimpl.h"
#include "upnpavconnectionactive.h"
#include "upnpfileuploadsessionimpl.h"
#include "upnpfiledownloadsessionimpl.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::NewL
// Two-phase construct
// --------------------------------------------------------------------------
CUPnPAVControllerActive* CUPnPAVControllerActive::NewL()
    {
    CUPnPAVControllerActive* self = CUPnPAVControllerActive::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::NewLC
// Two-phase construct
// --------------------------------------------------------------------------
CUPnPAVControllerActive* CUPnPAVControllerActive::NewLC()
    {
    CUPnPAVControllerActive* self = new (ELeave) CUPnPAVControllerActive;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::CUPnPAVControllerActive
// Two-phase construct
// --------------------------------------------------------------------------
CUPnPAVControllerActive::CUPnPAVControllerActive() :
    CActive( EPriorityStandard ),
    iRespBufSizePkg( iRespBufSize ),
    iDiscoveredPkg( iDiscovered ),
    iAlive( ETrue ),
    iReleaseState( EStateReleasable )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::Release
// Destructor
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::Release()
    {
    __LOG( "CUPnPAVControllerActive::Release" );
    
    if( iReleaseState == EStateReleasable )
        {
        __LOG( "Release - ok to delete" );
        delete this;
        }
    else if( iReleaseState == EStateWLANLost )
        {
        __LOG( "Release - waiting for release" );
        // Still sending WLAN lost messages, cannot delete
        // To be released
        iReleaseState = EStateWaitingForRelease;
        }
    else
        {
        __LOG( "Release - do nothing, double release" );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::~CUPnPAVControllerActive
// Destructor
// --------------------------------------------------------------------------
CUPnPAVControllerActive::~CUPnPAVControllerActive()
    {
    __LOG( "CUPnPAVControllerActive::~CUPnPAVControllerActive" );
    Cancel();
    delete iConnectionActive;
    iBrowsingSessions.ResetAndDestroy();
    iRenderingSessions.ResetAndDestroy();
    iDownloadSessions.ResetAndDestroy();
    iUploadSessions.ResetAndDestroy();

    iServer.Close();
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::ConstructL
// Two-phase construct
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::ConstructL()
    {
    __LOG( "CUPnPAVControllerActive::ConstructL" );
    
    // Check the aip
    TInt iap = 0;
    CUpnpSettings* settings = CUpnpSettings::NewL( KCRUidUPnPStack );
    settings->Get( CUpnpSettings::KUPnPStackIapId, iap );
    delete settings;
    User::LeaveIfError( iap );
    User::LeaveIfError( iServer.Connect() );
    
    iConnectionActive = CUPnPAVConnectionActive::NewL( iServer, *this );
    iConnectionActive->StartMonitoring();
    
    iServer.StartUp( iStatus ); // Start up the AV Control Point and wait
    // until it has been started. 
    SetActive();
    iWait.Start();
    __LOG1( "ConstructL - iStatus = %d", iStatus.Int() );
    User::LeaveIfError( iStatus.Int() );
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::RunL
// From CActive
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::RunL()
    {
    __LOG( "CUPnPAVControllerActive::RunL" );
    
    if( iWait.IsStarted() )
        {
        iWait.AsyncStop(); // AV Control Point has been started, continue
        // Construction
        return;
        }    
    
    switch( iStatus.Int() )
        {
        case EAVControllerDeviceCompleted:
            {
            HBufC8* tempBuf = HBufC8::NewLC( iRespBufSize );
            TPtr8 ptr = tempBuf->Des();
            if( iServer.GetDevice( ptr ) == KErrNone )
                {
                // Create a device and make a callback
                CUpnpAVDevice* tempDev = CUpnpAVDevice::NewLC(); 
                RDesReadStream stream( ptr );
                CleanupClosePushL( stream );
                stream >> *tempDev;
                CleanupStack::PopAndDestroy( &stream );
                
                if( iDiscovered == EAVDeviceDiscovered )
                    {
                    // Discovered a device
                    iDeviceObserver->UPnPDeviceDiscovered( *tempDev );
                    }
                else
                    {
                    // Device disappeared
                    iDeviceObserver->UPnPDeviceDisappeared( *tempDev );
                    }       
                CleanupStack::PopAndDestroy( tempDev );
                
                // Activate again if needed (it's possible to remove and set
                // the device obs from the UPnPDeviceDiscovered or
                // UPnPDeviceDisappeared callbacks  
                if( !IsActive() )
                    {
                    iServer.DeviceRequest( iDiscoveredPkg, iRespBufSizePkg,
                        iStatus ); 
                    SetActive();                      
                    }
                }
            else
                {
                // Can't handle the error anyhow, just log it
                __LOG1( "RunL - error: %d", iStatus.Int() );
                }
            CleanupStack::PopAndDestroy( tempBuf );
            }
            break;
            
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            }    
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::DoCancel
// From CActive
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::DoCancel()
    {
    __LOG( "CUPnPAVControllerActive::DoCancel" );
    if( !iDeviceObserver )
        {
        __LOG( "iServer.CancelStartUp()" );
        // Not a real cancel, but if this is the last session, we'll shut
        // down the server immidiately
        iServer.CancelStartUp();
        }
    else
        {
        iServer.CancelDeviceRequest();
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::RunError
// From CActive
// --------------------------------------------------------------------------
TInt CUPnPAVControllerActive::RunError(
    TInt aError
    )
    {
    // Can't handle the error anyhow, just log it
    __LOG1( "CUPnPAVControllerActive::RunError: %d", aError );
    
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerActive::SetDeviceObserver
// Sets device observer
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::SetDeviceObserver( 
    MUPnPAVDeviceObserver& aObserver )
    {
    __LOG( "CUPnPAVControllerActive::SetDeviceObserver" );
    
    // Set the observer and make an async request to the server to receive
    // device callbacks
    __ASSERTD( !iDeviceObserver, __FILE__, __LINE__ );
    
    if( iAlive )
        {
        iDeviceObserver = &aObserver;
        
        iServer.DeviceRequest( iDiscoveredPkg, iRespBufSizePkg, iStatus ); 
        SetActive();  
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::DeviceObserver
// Returns device observer
// --------------------------------------------------------------------------
MUPnPAVDeviceObserver* CUPnPAVControllerActive::DeviceObserver()
    {
    __LOG( "CUPnPAVControllerActive::DeviceObserver" );
    
    return iDeviceObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::RemoveDeviceObserver
// Removes device observer
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::RemoveDeviceObserver()
    {
    __LOG( "CUPnPAVControllerActive::RemoveDeviceObserver" );
    
    Cancel();
    
    iDeviceObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::GetMediaServersL
// Returns a list of media servers
// --------------------------------------------------------------------------
CUpnpAVDeviceList* CUPnPAVControllerActive::GetMediaServersL()
    {
    __LOG( "CUPnPAVControllerActive::GetMediaServersL" );
    
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    
    CUpnpAVDeviceList* tempList = NULL;
    
    TPckg<TAVControllerDeviceListType> type( EAVMediaServer );
    
    TInt respBufSize = 0;           
    TPckg<TInt> respBufSizePkg( respBufSize );        
    TInt err = iServer.GetDeviceListSize( type, respBufSizePkg );
    
    if( err == KErrNone )
        {
        
        HBufC8* tempBuf = HBufC8::NewLC( respBufSize );
        TPtr8 ptr = tempBuf->Des();

        err = iServer.GetDeviceList( ptr );
        if( err == KErrNone )
            {
            // Create a device and make a callback
            tempList = CUpnpAVDeviceList::NewL(); 
            RDesReadStream stream( ptr );
            stream >> *tempList;
            stream.Close();
            }
        else
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( tempBuf );        
        }
    else
        {
        User::Leave( err );
        }
    return tempList;    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::GetMediaRenderersL
// Returns a list of media renderes
// --------------------------------------------------------------------------
CUpnpAVDeviceList* CUPnPAVControllerActive::GetMediaRenderersL()
    {
    __LOG( "CUPnPAVControllerActive::GetMediaRenderersL" );
    
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }    
    
    CUpnpAVDeviceList* tempList = NULL;
    
    TPckg<TAVControllerDeviceListType> type( EAVMediaRenderer );
    
    TInt respBufSize = 0;           
    TPckg<TInt> respBufSizePkg( respBufSize );
    TInt err = iServer.GetDeviceListSize( type, respBufSizePkg );
    
    if( err == KErrNone )
        {
        
        HBufC8* tempBuf = HBufC8::NewLC( respBufSize );
        TPtr8 ptr = tempBuf->Des();

        err = iServer.GetDeviceList( ptr );
        if( err == KErrNone )
            {
            // Create a device and make a callback
            tempList = CUpnpAVDeviceList::NewL(); 
            RDesReadStream stream( ptr );
            stream >> *tempList;
            stream.Close();
            }
        else
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( tempBuf );        
        }
    else
        {
        User::Leave( err );
        }
    return tempList;    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StartBrowsingSessionL
// Starts a browsing session
// --------------------------------------------------------------------------
MUPnPAVBrowsingSession& CUPnPAVControllerActive::StartBrowsingSessionL(
    const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPAVControllerActive::StartBrowsingSessionL" );
   
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    
    if( aDevice.DeviceType() != CUpnpAVDevice::EMediaServer )
        {
        User::Leave( KErrNotSupported );
        }
    CUPnPAVBrowsingSessionImpl* sessionImpl =
        CUPnPAVBrowsingSessionImpl::NewL( iServer, aDevice );
    iBrowsingSessions.AppendL( sessionImpl );
    return *sessionImpl;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StopBrowsingSession
// Stops a browsing session
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::StopBrowsingSession(
    MUPnPAVBrowsingSession& aSession )
    {
    __LOG( "CUPnPAVControllerActive::StopBrowsingSession" );
    
    CUPnPAVBrowsingSessionImpl* sessionImpl = 
        static_cast<CUPnPAVBrowsingSessionImpl*>(&aSession);
    TInt count = iBrowsingSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ] == sessionImpl )
            {
            delete iBrowsingSessions[ i ];
            iBrowsingSessions[ i ] = NULL;
            i = count;
            }
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StartRenderingSessionL
// Starts a rendering session
// --------------------------------------------------------------------------
MUPnPAVRenderingSession& CUPnPAVControllerActive::StartRenderingSessionL(
    const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPAVControllerActive::StartRenderingSessionL" );
    
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    
    if( aDevice.DeviceType() != CUpnpAVDevice::EMediaRenderer )
        {
        User::Leave( KErrNotSupported );
        }
    CUPnPAVRenderingSessionImpl* sessionImpl =
        CUPnPAVRenderingSessionImpl::NewL( iServer, aDevice );
    iRenderingSessions.AppendL( sessionImpl );
    return *sessionImpl;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StopRenderingSession
// Stops a rendering session
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::StopRenderingSession(
    MUPnPAVRenderingSession& aSession )
    {
    __LOG( "CUPnPAVControllerActive::StopRenderingSession" );
    
    CUPnPAVRenderingSessionImpl* sessionImpl =
        static_cast<CUPnPAVRenderingSessionImpl*>(&aSession);
    TInt count = iRenderingSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iRenderingSessions[ i ] == sessionImpl )
            {
            delete iRenderingSessions[ i ];
            iRenderingSessions[ i ] = NULL; 
            i = count;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StartUploadSessionL
// Starts an upload session
// --------------------------------------------------------------------------
MUPnPFileUploadSession& CUPnPAVControllerActive::StartUploadSessionL(
    const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPAVControllerActive::StartUploadSessionL" );
   
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    
    if( aDevice.DeviceType() != CUpnpAVDevice::EMediaServer )
        {
        User::Leave( KErrNotSupported );
        }
    CUPnPFileUploadSessionImpl* sessionImpl =
        CUPnPFileUploadSessionImpl::NewL( iServer, aDevice );
    iUploadSessions.AppendL( sessionImpl );
    return *sessionImpl;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StopUploadSession
// Stops an upload session
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::StopUploadSession(
    MUPnPFileUploadSession& aSession )
    {
    __LOG( "CUPnPAVControllerActive::StopUploadSession" );
  
    CUPnPFileUploadSessionImpl* sessionImpl =
        static_cast<CUPnPFileUploadSessionImpl*>(&aSession);
    TInt count = iUploadSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ] == sessionImpl )
            {
            delete iUploadSessions[ i ];
            iUploadSessions[ i ] = NULL; 
            i = count;
            }
        }

    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StartDownloadSessionL
// Starts a download session
// --------------------------------------------------------------------------
MUPnPFileDownloadSession& CUPnPAVControllerActive::StartDownloadSessionL(
    const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPAVControllerActive::StartDownloadSessionL" );

    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    
    if( aDevice.DeviceType() != CUpnpAVDevice::EMediaServer )
        {
        User::Leave( KErrNotSupported );
        }
    CUPnPFileDownloadSessionImpl* sessionImpl =
        CUPnPFileDownloadSessionImpl::NewL( iServer, aDevice );
    iDownloadSessions.AppendL( sessionImpl );
    return *sessionImpl;
    }   

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::StopDownloadSession
// Stops a download session
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::StopDownloadSession(
    MUPnPFileDownloadSession& aSession )
    {
    __LOG( "CUPnPAVControllerActive::StopDownloadSession" );

    CUPnPFileDownloadSessionImpl* sessionImpl =
        static_cast<CUPnPFileDownloadSessionImpl*>(&aSession);
    TInt count = iDownloadSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ] == sessionImpl )
            {
            delete iDownloadSessions[ i ];
            //iRenderingSessions.Remove( i );
            iDownloadSessions[ i ] = NULL; 
            i = count;
            }
        }

    }

// --------------------------------------------------------------------------
// CUPnPAVControllerActive::ConnectionLost
// Wlan disconnected
// --------------------------------------------------------------------------
void CUPnPAVControllerActive::ConnectionLost()
    {
    __LOG( "CUPnPAVControllerActive::ConnectionLost" );
    
    iAlive = EFalse;
    
    iReleaseState = EStateWLANLost;
    
    TInt count = iRenderingSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iRenderingSessions.Count() && iRenderingSessions[ i ] )
            {
            iRenderingSessions[ i ]->WLANConnectionLost();    
            }   
        }       

    count = iBrowsingSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions.Count() && iBrowsingSessions[ i ] )
            {
            iBrowsingSessions[ i ]->WLANConnectionLost();    
            }
        }

    count = iUploadSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions.Count() && iUploadSessions[ i ] )
            {
            iUploadSessions[ i ]->WLANConnectionLost();    
            }   
        }       

    count = iDownloadSessions.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions.Count() && iDownloadSessions[ i ] )
            {
            iDownloadSessions[ i ]->WLANConnectionLost();    
            }   
        }       

    if( iDeviceObserver )
        {
        iDeviceObserver->WLANConnectionLost();
        }
        
    if( iReleaseState == EStateWaitingForRelease )
        {
        __LOG( "ConnectionLost - release" );
        delete this;
        }
        
    iReleaseState = EStateReleasable;       
    }
// end of file
