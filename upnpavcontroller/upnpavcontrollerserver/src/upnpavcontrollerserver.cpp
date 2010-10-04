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
* Description:      AVController server
*
*/







// INCLUDE FILES
// System
#include <e32svr.h>
#include <f32file.h> 

// upnp stack
#include <upnpdevice.h>

// upnpframework / avcontroller api
#include "upnpavcontrollerglobals.h"

// upnpframework / internal api's
#include "upnpconnectionmonitor.h"

#include "upnpsecaccesscontroller.h"


// avcontroller server internal
#include "upnpavcontrollerserver.h"
#include "upnpavcontrollersession.h"

#include <upnpsettings.h>

#include "upnpavcontrolpoint.h"


#include "upnpavdispatcher.h"
#include "upnpdevicerepository.h"
#include "upnpavdeviceextended.h"
#include "upnpaverrorhandler.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt" );
#include "upnplog.h"

// CONSTANTS
_LIT8( KMediaServer,                    "MediaServer" );
_LIT8( KUPnPRootDevice,                 "upnp:rootdevice" );

const TInt KMaxDepth = 4;
const TInt KMaxDeviceCount = 6;

const TUint myRangeCount = 3;

const TInt myRanges[ myRangeCount ] = 
    {
    0, // numbers 0-18
    18, // numbers 18-81 
    81 // numbers 81-KMaxInt
    };

const TUint8 myElementsIndex[ myRangeCount ] = 
    {
    0, 
    1, 
    CPolicyServer::ENotSupported
    };
    
const CPolicyServer::TPolicyElement myElements[] = 
    {
    {_INIT_SECURITY_POLICY_C3(ECapabilityNetworkServices,
        ECapabilityReadUserData, ECapabilityWriteUserData ),
        CPolicyServer::EFailClient },
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices),
        CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy myPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    myRangeCount,                   
    myRanges,
    myElementsIndex,
    myElements,
    };

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::CUpnpAVControllerServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUpnpAVControllerServer::CUpnpAVControllerServer( TInt aPriority ):
    CPolicyServer( aPriority, myPolicy ),
    iShutdownTimeoutValue( KTimerCycle10 ),
    iServerState( EStateUndefined )
    {    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ConstructL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ConstructL()
    {
    __LOG( "CUpnpAVControllerServer::ConstructL" );
                               
    iDispatcher = CUPnPAVDispatcher::NewL( *this );
    
    iServerState = EStateStartingUp;

    CUpnpSettings* settings = CUpnpSettings::NewL( KCRUidUPnPStack );
    settings->Get( CUpnpSettings::KUPnPStackIapId, iIAP );
    delete settings;    

    User::LeaveIfError( iMediaServer.Connect() );
    iMonitor = CUPnPConnectionMonitor::NewL( *this, iIAP );
    
    StartL( KAVControllerName );   
    
    iServerTimer = CUPnPAVTimer::NewL( *this,
        CUPnPAVTimer::ETimerServerShutdown );
    
    iServerTimer->Start( iShutdownTimeoutValue );
    
    iMSTimer = CUPnPAVTimer::NewL( *this, CUPnPAVTimer::ETimerMediaServer );
    __LOG( "CUpnpAVControllerServer::ConstructL - Finished" );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StartUpL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StartUpL()
    {
    __LOG( "CUpnpAVControllerServer::StartUpL" );

    TInt error = KErrNone;
    if( iServerState == EStateStartingUp )
        {
        __LOG( "StartUpL - Starting up" );
              
        
        if( !iAVControlPoint )
            {
            __LOG( "CUpnpAVControllerServer::StartUpL - CP" );

            TRAP( error, iAVControlPoint = CUpnpAVControlPoint::NewL( 
                                                               *iDispatcher ));
            // If operation fails for some reason , the 10 second timeout 
            // is completely useless and wrong in this case. 
            // The server should be shut down immediately   
            if( error != KErrNone )
                {
                iShutdownTimeoutValue = 0;   
                User::Leave( error );
                }
            }
        if( !iDeviceRepository )
            {
            iDeviceRepository = CUPnPDeviceRepository::NewL( *iAVControlPoint );
            }
        iServerState = EStateRunning;  
        }
    else if( iServerState == EStateShuttingDown )
        {
        __LOG( "StartUpL - Wlan disconnected or shutting down, leave" );
        User::Leave( KErrDisconnected );
        }
    else
        {
        __LOG( "StartUpL - Server running" );
        }        

    __LOG( "StartUpL - Completed" );    
    }

void CUpnpAVControllerServer::CancelStartUp()
    {
    __LOG( "CUpnpAVControllerServer::CancelStartUp" );
    
    // Cancel can occur only when the av controller instance is deleted
    // right after the asyncronous part of the construction has been
    // started. There is no proper way to cancel, but we can set the
    // shutdown timer to 0, so the server will shutdown immidiately.
    
    if( iSessionCount <= 1 )
        {
        iShutdownTimeoutValue = 0;    
        }  
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::NewLC
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUpnpAVControllerServer* CUpnpAVControllerServer::NewLC()
    {
    CUpnpAVControllerServer* self = new( ELeave )
        CUpnpAVControllerServer( EPriorityNormal );   
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVControllerServer::~CUpnpAVControllerServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUpnpAVControllerServer::~CUpnpAVControllerServer()
    {
    __LOG( "CUpnpAVControllerServer::~CUpnpAVControllerServer" );
    
    delete iAVControlPoint;
    delete iDispatcher;
    delete iDeviceRepository;
   
    delete iMonitor;
    delete iServerTimer;
    delete iMSTimer;

    iMediaServer.Close();

    for( TInt i = 0; i < iStartMessages.Count(); i++ )
        {
        iStartMessages[ i ]->Complete( KErrCancel );
        }
    iStartMessages.ResetAndDestroy();  
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::NewSessionL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CSession2* CUpnpAVControllerServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& aMessage ) const
    {
    __LOG( "CUpnpAVControllerServer::NewSessionL" );
    
    if( iServerState == EStateShuttingDown )
        {
        __LOG( "NewSessionL - server shutting down, no new sessions \
are allowed at this point" );
        User::Leave( KErrDisconnected );
        }
    else if( iServerState == EStateStartingUp && iSessionCount > 0 )
        {
        __LOG( "NewSessionL - server starting up, no new sessions \
are allowed at this point" );
        User::Leave( KErrServerBusy );
        }
        
    // Check we're the right version
    if ( !User::QueryVersionSupported( TVersion( 
            KAVControllerMajorVersionNumber,
            KAVControllerMinorVersionNumber,
            KAVControllerBuildVersionNumber ),
            aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    // Make new session
    RThread client;
    aMessage.Client(client);  
    return CUpnpAVControllerSession::NewL(
        *(CUpnpAVControllerServer*)this );
    }
    

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::UPnPAVTimerCallback
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::UPnPAVTimerCallback(
    CUPnPAVTimer::TAVTimerType aType ) 
    {
    __LOG( "CUpnpAVControllerServer::UPnPAVTimerCallback" );


    if( aType == CUPnPAVTimer::ETimerServerShutdown )
        {
        iServerState = EStateShuttingDown;
           
        if( iMSTimer->IsActive() )
            {
            // if the Media Server timer is still running for some reason
            iMSTimer->Cancel();
            StopMediaServer();
            }
        CActiveScheduler::Stop();
        }
    else if( aType == CUPnPAVTimer::ETimerMediaServer )
        {
        if( iStartingMS )
            {
            StopMediaServer();
            TInt count = iStartMessages.Count();
            for( TInt i = 0; i < count; i++ )
                {
                iStartMessages[ i ]->Complete( KErrTimedOut );
                }
            iStartMessages.ResetAndDestroy();
            
            iStartingMS = EFalse;
            }
        else // Shutting down
            {
            StopMediaServer();
            }            
        }
    else
        {
        
        }        
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ConnectionLost
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ConnectionLost()
    {
    __LOG( "CUpnpAVControllerServer::ConnectionLost" );
    
    iShutdownTimeoutValue = 0; // Set shutdown timer value to 0, we want to
    // shut down the server immidiately after the last session has been
    // closed
    
    if( iServerState == EStateRunning && iDeviceRepository )
        {
        __LOG( "ConnectionLost - Server running" );
        iDeviceRepository->ConnectionLost();    

        CSession2* s;
        iSessionIter.SetToFirst(); 
        while ( ( s = iSessionIter++ ) != NULL )
            {
            CUpnpAVControllerSession* sess =
                static_cast<CUpnpAVControllerSession*>(s);
            if( sess )
                {
                sess->ConnectionLost();    
                }
            };  
        iServerState = EStateShuttingDown;
        }
    else if (iServerState == EStateStartingUp )
        {
        __LOG( "ConnectionLost - Server starting up" );
        iServerState = EStateShuttingDown;
        }    

    // If don't have any clients connect to server and current WLAN connection
    // is lost, we want to shut down the server immidiately.
    if ( iSessionCount <= 0 )
        {
        if ( iServerTimer->IsActive() )
            {
            iServerTimer->Cancel();
            }
        iServerTimer->Start( iShutdownTimeoutValue );
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::RunError
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
TInt CUpnpAVControllerServer::RunError( TInt aError )
    {
    __LOG( "CUpnpAVControllerServer::RunError" );
       
    if ( aError == KErrBadDescriptor )
        {
        PanicClient( Message(), EAVControllerServerBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }
        
    // The leave will result in an early return from CServer::RunL(),
    // skipping the call to request another message. So do that now in order
    // to keep the server running.
    ReStart();
    // Handled the error fully
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::PanicClient
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::PanicClient(const RMessage2& aMessage,
    TAVControllerServerPanic aPanic)
    {
    __LOG( "CUpnpAVControllerServer::PanicClient" );
       
    aMessage.Panic( KAVControllerName, aPanic );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::PanicServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::PanicServer(TAVControllerServerPanic aPanic)
    {
    __LOG( "CUpnpAVControllerServer::PanicServer" );
    
    User::Panic( KAVControllerName, aPanic );
    }


// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ThreadFunctionL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ThreadFunctionL()
    {
    __LOG( "CUpnpAVControllerServer::ThreadFunctionL" );
    
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );
    // Install active scheduler
    // We don't need to check whether an active scheduler is already
    // installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );    
    // Construct our server        
    CUpnpAVControllerServer* server = CUpnpAVControllerServer::NewLC();
    
    RProcess::Rendezvous( KErrNone );                
    // Start handling requests
    CActiveScheduler::Start();      
             
    CleanupStack::PopAndDestroy( server );  
    CleanupStack::PopAndDestroy( activeScheduler );
    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::HandleEmbeddedDiscoveredDevicesL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::HandleEmbeddedDiscoveredDevicesL(
    CUpnpDevice& aDevice, TInt aDepth )
    {
    __LOG( "CUpnpAVControllerServer::HandleEmbeddedDiscoveredDevicesL" );
    
    if( aDepth <= KMaxDepth && iDiscoveredDeviceCount <= KMaxDeviceCount )
        {
        RPointerArray<CUpnpDevice>& devList = aDevice.DeviceList();
        TInt count = devList.Count();
        for( TInt i = 0; i < count; i++ )
            {
            iDeviceRepository->AddDeviceL( *devList[ i ] );
            TInt sessionId = iAVControlPoint->CmProtocolInfoActionL(
                devList[ i ]->Uuid() );
            iDiscoveredDeviceCount++;    
            
            HandleEmbeddedDiscoveredDevicesL( *devList[ i ], ++aDepth );
            }        
        }
    else
        {
        __LOG( "HandleEmbeddedDiscoveredDevicesL - max depth \
or count reached" );
        }    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::HandleEmbeddedDisappearedDevicesL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::HandleEmbeddedDisappearedDevicesL(
    CUpnpDevice& aDevice, TInt aDepth )
    {
    __LOG( "CUpnpAVControllerServer::HandleEmbeddedDisappearedDevicesL" );
    
    if( aDepth <= KMaxDepth && iDisappearedDeviceCount <= KMaxDeviceCount )
        {
        RPointerArray<CUpnpDevice>& devList = aDevice.DeviceList();
        TInt count = devList.Count();
        for( TInt i = 0; i < count; i++ )
            {
            CUpnpAVDeviceExtended& ext = iDeviceRepository->FindDeviceL(
                devList[ i ]->Uuid() );
            
            CSession2* s;
            iSessionIter.SetToFirst(); 
            while ( ( s = iSessionIter++ ) != NULL )
                {
                CUpnpAVControllerSession* sess =
                    static_cast<CUpnpAVControllerSession*>(s);
                if( sess )
                    {
                    sess->DeviceDisappearedL( ext );    
                    }
                };        

            iDeviceRepository->Remove( ext.Uuid() );
            
            iDisappearedDeviceCount++;    
            
            HandleEmbeddedDisappearedDevicesL( *devList[ i ], ++aDepth );
            }        
        }
    else
        {
        __LOG( "HandleEmbeddedDisappearedDevicesL - max depth \
or count reached" );
        }    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StopMediaServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StopMediaServer()
    {
    __LOG( "CUpnpAVControllerServer::StopMediaServer" );
    
    if( iShutdownTimeoutValue )
        {
        __LOG( "StopMediaServer - normal shutdown" );
        iMediaServer.Stop( RUpnpMediaServerClient::EStopNormal );
        }
    else
        {
        __LOG( "StopMediaServer - silent shutdown" );
        iMediaServer.Stop( RUpnpMediaServerClient::EStopSilent );
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ThreadFunction
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
TInt CUpnpAVControllerServer::ThreadFunction()
    {
    __LOG( "CUpnpAVControllerServer::ThreadFunction" );
    
    __UHEAP_MARK;
    
    User::RenameThread(KAVControllerThreadName);
    
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !(cleanupStack) )
        {
        PanicServer( EAVControllerServerCreateTrapCleanup );
        }

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        __LOG1( "ThreadFunction, creation failed: %d", err );
        //PanicServer( EAVControllerServerSrvCreateServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;
       
    __UHEAP_MARKEND;

    return err;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::IncrementSessions
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::IncrementSessions() 
    {
    __LOG( "CUpnpAVControllerServer::IncrementSessions" );
    
    iSessionCount++;
    if( iServerTimer->IsActive() )
        {
        iServerTimer->Cancel();
        __LOG( "IncrementSessions - make a search" );
        if( iAVControlPoint )
            {
            TRAP_IGNORE( iAVControlPoint->SearchL( KUPnPRootDevice ) );
            }
        }   
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DecrementSessions
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DecrementSessions()
    {
    __LOG( "CUpnpAVControllerServer::DecrementSessions" );
    
    iSessionCount--;
    if ( iSessionCount <= 0 )
        {
        if( iServerTimer->IsActive() )
            {
            iServerTimer->Cancel();
            }
        iServerTimer->Start( iShutdownTimeoutValue );
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDiscoveredL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDiscoveredL( CUpnpDevice& aDevice )
    {
    __LOG( "CUpnpAVControllerServer::DeviceDiscoveredL" );

    if( aDevice.Local() && aDevice.DeviceType().Find( KMediaServer )
        != KErrNotFound )
        {
        // It's the local S60 MS
        
        if( iStartingMS )
            {
            iMSTimer->Cancel();
            }   
        CUpnpSecAccessController* accessController = 
            CUpnpSecAccessController::NewL();
            __LOG( "CUpnpAVControllerServer::DeviceDiscoveredL \
adding the local media server IP to the list of authorized addresses." );

        accessController->AddAllowedAddress( aDevice.Address() );
        delete accessController; 
        accessController = NULL;
        }
        
    iDeviceRepository->AddDeviceL( aDevice );
    TInt sessionId = iAVControlPoint->CmProtocolInfoActionL(
        aDevice.Uuid() );
    iDiscoveredDeviceCount = 1; // First (root) device

    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDisappearedL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDisappearedL( CUpnpDevice& aDevice )
    {
    __LOG( "CUpnpAVControllerServer::DeviceDisappearedL" );
    
    if( aDevice.Local() && aDevice.DeviceType().Find( KMediaServer )
        != KErrNotFound )
        {  
        // It's the local S60 MS
        iMediaServerOnline = EFalse;
        }
    
    // Get a corresponding device from the device repository
    CUpnpAVDeviceExtended& tmp = iDeviceRepository->FindDeviceL(
        aDevice.Uuid() ); 
    
    // Let the clients know about the disappeared device
    CSession2* s;
    iSessionIter.SetToFirst(); 
    while ( ( s = iSessionIter++ ) != NULL )
        {
        CUpnpAVControllerSession* sess =
            static_cast<CUpnpAVControllerSession*>(s);
        if( sess )
            {
            sess->DeviceDisappearedL( tmp );    
            }
        };        
            // Remove from the device repository
    iDeviceRepository->Remove( aDevice.Uuid() );
    iDisappearedDeviceCount = 1;
    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDisappearedL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDisappearedL( const TDesC8& aUuid )
    {
    __LOG( "CUpnpAVControllerServer::DeviceDisappearedL uid" );
    // Get a corresponding device from the device repository
    CUpnpAVDeviceExtended& tmp = iDeviceRepository->FindDeviceL(
        aUuid ); 
    
    // Let the clients know about the disappeared device
    CSession2* s;
    iSessionIter.SetToFirst(); 
    while ( ( s = iSessionIter++ ) != NULL )
        {
        CUpnpAVControllerSession* sess =
            static_cast<CUpnpAVControllerSession*>( s );
        if ( sess )
            {
            sess->DeviceDisappearedL( tmp );    
            }
        }       
    // Remove from the device repository
    iDeviceRepository->Remove( aUuid );
    iDisappearedDeviceCount = 1;
    __LOG( "CUpnpAVControllerServer::DeviceDisappearedL uid End" );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StartMediaServerL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StartMediaServerL( const RMessage2& aMessage )
    {
    __LOG( "CUpnpAVControllerServer::StartMediaServerL" );

    if( iMediaServerOnline )
        {
        // Started already, complete the msg
        iMSTimer->Cancel();
        aMessage.Complete( EAVControllerStartMediaServerCompleted );
        iServerUserCount++;
        }
    else
        {
        // Start the media server and timer
        if( iStartMessages.Count() > 0 )
            {
            RMessage2* message = new (ELeave) RMessage2( aMessage );
            CleanupStack::PushL( message);
            iStartMessages.AppendL( message );
            CleanupStack::Pop( message );
            }
        else
            {
            // Check if the stack's security is enabled
            TBool upnpSecurityEnabled = EFalse;
            TRAPD( secCheckError, upnpSecurityEnabled = 
                CUpnpSecAccessController::IsMediaServerSecurityEnabledL() );

            // If the security is not enabled, enable it now
            if( secCheckError == KErrNone &&
                !upnpSecurityEnabled )
                {
                TRAP_IGNORE( 
                    CUpnpSecAccessController::EnableMediaServerSecurityL() );
                }

            RMessage2* message = new(ELeave) RMessage2( aMessage );
            iStartMessages.AppendL( message );
            User::LeaveIfError( iMediaServer.Start() );
            iMSTimer->Start( iShutdownTimeoutValue );
            iStartingMS = ETrue;
            }    
        }            
    }
    
// --------------------------------------------------------------------------
// CUpnpAVControllerServer::CancelStartMediaServerL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::CancelStartMediaServerL(
    const RMessage2& aMessage )
    {
    __LOG( "CUpnpAVControllerServer::CancelStartMediaServerL" );

    if( !iMSActivatedBeforeStart )
        {
        StopMediaServer();
        }

    TInt count = iStartMessages.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iStartMessages[ i ]->Complete( KErrCancel );
        }
    iStartMessages.ResetAndDestroy();
    iMSTimer->Cancel();
    
    aMessage.Complete( KErrNone );
        
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StopMediaServerL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StopMediaServerL( const RMessage2& aMessage )
    {
    __LOG( "CUpnpAVControllerServer::StopMediaServerL" );
    
    if( iMediaServerOnline )
        {
        iServerUserCount--;
        if( iServerUserCount <= 0 )
            {
            if( !iMSActivatedBeforeStart )
                {
                iMSTimer->Start( iShutdownTimeoutValue );
                }
            iServerUserCount = 0;
            }
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::MSServicesInUse
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::MSServicesInUse( const RMessage2& aMessage )
    {
    if( iServerUserCount > 0 || iStartingMS
         || iMSTimer->IsActive()
        )        
        {
        TPckg<TBool> resp0( ETrue );
        aMessage.Write( 0, resp0 );        
        }
    else
        {
        TPckg<TBool> resp0( EFalse );
        aMessage.Write( 0, resp0 );                
        }    
    aMessage.Complete( KErrNone );    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVControllerServer::CmProtocolInfoResponse
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::CmProtocolInfoResponse( const TDesC8& aUuid,
    TInt /*aSessionId*/, TInt aErr, const TDesC8& aSource,
    const TDesC8& aSink )
    {
    __LOG1( "CUpnpAVControllerServer::CmProtocolInfoResponse, \
aErr = %d", aErr );

    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPConnectionManagerError );    

    if( aErr == KErrNone )
        {
        CUpnpAVDeviceExtended* dev = NULL;
        TRAPD( err, dev = &iDeviceRepository->AddProtocolInfoL(
            aUuid, aSource, aSink ) );
        
        if( err == KErrNone )    
            {
            // Device discovered and protocolinfo was retrieved successfully
            CSession2* s;
            iSessionIter.SetToFirst(); 
            while ( ( s = iSessionIter++ ) != NULL )
                {
                CUpnpAVControllerSession* sess =
                    static_cast<CUpnpAVControllerSession*>(s);
                if( sess )
                    {
                    TRAP_IGNORE( sess->DeviceDiscoveredL( *dev ) );    
                    }
                };        
            
            if( dev->Local() )
                {
                iMediaServerOnline = ETrue;
                
                if( iStartingMS )
                    {
                    TInt count = iStartMessages.Count();
                    for( TInt i = 0; i < count; i++ )
                        {
                        iStartMessages[ i ]->Complete(
                            EAVControllerStartMediaServerCompleted );
                        iServerUserCount++;    
                        }
                    iStartMessages.ResetAndDestroy();        
                    
                    iStartingMS = EFalse;                
                    }
                else
                    {
                    __LOG( "Sharing was enabled before AVC server start" );
                    iMSActivatedBeforeStart = ETrue;
                    }    
                
                }    
            }
        else
            {
            // Could not add protocolinfo, it's invalid or corrupted
            // Device cannot be used
            HandleFailedProtocolInfoResponse( aUuid );          
            }
        }
    else
        {
        // A problem occured fetching protocolinfo
        // Device cannot be used
        HandleFailedProtocolInfoResponse( aUuid );
        }        
    }

void CUpnpAVControllerServer::HandleFailedProtocolInfoResponse(
    const TDesC8& aUuid )
    {
    __LOG( "CUpnpAVControllerServer::HandleFailedProtocolInfoResponse" );
    
    CUpnpAVDeviceExtended* dev = NULL;
    TRAPD( err, dev = &iDeviceRepository->FindDeviceL( aUuid ) )
    if( err == KErrNone )
        {
        if( iStartingMS && dev->Local() )
            {
            __LOG( "HandleFailedProtocolInfoResponse - local, stop and \
complete messages" );
            
            StopMediaServer();
            
            TInt count = iStartMessages.Count();
            for( TInt i = 0; i < count; i++ )
                {
                iStartMessages[ i ]->Complete( err );
                iServerUserCount++;    
                }
            iStartMessages.ResetAndDestroy();        
            
            iStartingMS = EFalse;
            }                           
        }
    else
        {
        // Not found, no can do
        __LOG( "HandleFailedProtocolInfoResponse - not found" );
        }    
    iDeviceRepository->Remove( aUuid );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ControlPoint
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUpnpAVControlPoint& CUpnpAVControllerServer::ControlPoint()
    {
    return *iAVControlPoint;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::MediaServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
RUpnpMediaServerClient& CUpnpAVControllerServer::MediaServer()
    {
    return iMediaServer;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVControllerServer::Dispatcher
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUPnPAVDispatcher& CUpnpAVControllerServer::Dispatcher()
    {
    return *iDispatcher;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceRepository
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUPnPDeviceRepository& CUpnpAVControllerServer::DeviceRepository()
    {
    return *iDeviceRepository;
    }

TInt CUpnpAVControllerServer::IAP()
    {
    return iIAP;
    }
    
// ============================= LOCAL FUNCTIONS ============================

// --------------------------------------------------------------------------
// E32Main entry point.
// Returns: KErrNone
// --------------------------------------------------------------------------
TInt E32Main()
    {
    return CUpnpAVControllerServer::ThreadFunction();
    }

// End of File

