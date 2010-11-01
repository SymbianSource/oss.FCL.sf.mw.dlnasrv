/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <upnpsettings.h>

// dlnasrv / avcontroller api
#include "upnpavcontrollerglobals.h"

// dlnasrv / internal api's
#include "upnpconnectionmonitor.h"

// dlnasrv / avcontroller server internal
#include "upnpavcontrollerserver.h"
#include "upnpavcontrollersession.h"
#include "upnpavcontrolpoint.h"
#include "upnpavdispatcher.h"
#include "upnpdevicerepository.h"
#include "upnpavdeviceextended.h"
#include "upnpaverrorhandler.h"
#include "upnpavcpstrings.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt" );
#include "upnplog.h"

using namespace UpnpAVCPStrings;

// CONSTANTS
_LIT8( KUPnPRootDevice,                 "upnp:rootdevice" );

const TUint KMyRangeCount = 3;

const TInt KMyRanges[ KMyRangeCount ] = 
    {
    0, // numbers 0-18
    18, // numbers 18-EAVControllerRqstLast
    EAVControllerRqstLast // numbers EAVControllerRqstLast-KMaxInt
    };

const TUint8 KMyElementsIndex[ KMyRangeCount ] = 
    {
    0, 
    1, 
    CPolicyServer::ENotSupported
    };
    
const CPolicyServer::TPolicyElement KMyElements[] = 
    {
    {_INIT_SECURITY_POLICY_C3(ECapabilityNetworkServices,
        ECapabilityReadUserData, ECapabilityWriteUserData ),
        CPolicyServer::EFailClient },
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices),
        CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy KMyPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    KMyRangeCount,                   
    KMyRanges,
    KMyElementsIndex,
    KMyElements,
    };

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::CUpnpAVControllerServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CUpnpAVControllerServer::CUpnpAVControllerServer( TInt aPriority ):
    CPolicyServer( aPriority, KMyPolicy ),
    iShutdownTimeoutValue( KTimerCycle10 ),
    iState( EStateUndefined )
    {    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ConstructL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ConstructL()
    {
    __LOG( "CUpnpAVControllerServer::ConstructL" );

    ChangeState( EStateStartingServer );

    __LOG( "ConstructL - Starting server" );

    // create av dispatcher
    iDispatcher = CUPnPAVDispatcher::NewL( *this );

    // create av control point
    iAVControlPoint = CUpnpAVControlPoint::NewL( *iDispatcher );    
    
    // create device repository
    iDeviceRepository = CUPnPDeviceRepository::NewL( *iAVControlPoint );
    
    iUpnpSettings = CUpnpSettings::NewL( KCRUidUPnPStack );
    iUpnpSettings->Get( CUpnpSettings::KUPnPStackIapId, iIAP );
    
    iMonitor = CUPnPConnectionMonitor::NewL( iIAP );
    iMonitor->SetObserver( *this );
    
    StartL( KAVControllerName );   
    
    iServerTimer = CUPnPAVTimer::NewL( *this,
        CUPnPAVTimer::ETimerServerShutdown );
    
    iServerTimer->Start( iShutdownTimeoutValue );

    iIconDownloader = CUpnpDeviceIconDownloader::NewL( *this, iIAP );

    __LOG( "CUpnpAVControllerServer::ConstructL - Finished" );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StartUpL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StartUpL()
    {
    __LOG( "CUpnpAVControllerServer::StartUpL" );

    if( iState == EStateStartingServer )
        {
        ChangeState( EStateStartingControlPoint );

        __LOG( "StartUpL - Starting control point" );
        
        // start SSPD search
        TInt err = KErrNone;
        TRAP( err, iAVControlPoint->StartUpL() );
        if( err == KErrNone )
            {
            __LOG( "StartUpL - Searching root device" );
            
            iAVControlPoint->SearchL( KUPnPRootDevice );
            ChangeState( EStateRunning );
            }
        else
            {
            iShutdownTimeoutValue = 0;
            User::Leave( err );
            }
        }        
    else if( iState == EStateShuttingDown )
        {
        __LOG( "StartUpL - Wlan disconnected or shutting down, leave" );
        User::Leave( KErrDisconnected );
        }
    else if( iState == EStateStartingControlPoint )
        {
        __LOG( "StartUpL - Already starting control point" );
        }
    else
        {
        __LOG( "StartUpL - Server already running" );
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
    
    delete iUpnpSettings;
    delete iIconDownloader;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::NewSessionL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
CSession2* CUpnpAVControllerServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    __LOG( "CUpnpAVControllerServer::NewSessionL" );
    
    TInt err = KErrNone;
    
    // check if the server is shutting down
    if( iState == EStateShuttingDown )
        {
        __LOG( "NewSessionL - server shutting down, no new sessions are allowed at this point" );
        err = KErrDisconnected;
        }
        
    // Check we're the right version
    else if ( !User::QueryVersionSupported( TVersion( 
            KAVControllerMajorVersionNumber,
            KAVControllerMinorVersionNumber,
            KAVControllerBuildVersionNumber ),
            aVersion ) )
        {
        __LOG( "NewSessionL - incorrect client version" );
        err = KErrNotSupported;
        }
    
    // leave if error
    User::LeaveIfError( err );

    // Make new session
    return CUpnpAVControllerSession::NewL(
        const_cast<CUpnpAVControllerServer&>( *this ) );
    }
    
// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ActionResponseL
// From MUpnpAVControlPointObserver
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ActionResponseL( CUpnpAction* aAction )
    {
    if (aAction->Name().Compare( KGetProtocolInfo ) == 0)
        {
        const TDesC8& uuid = aAction->Service().Device().Uuid();
        CmProtocolInfoResponse(
            uuid,
            aAction->Error(),
            aAction->ArgumentValue( KSource ), 
            aAction->ArgumentValue( KSink )
            );
        iDispatcher->UnRegister(aAction->SessionId());
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::StateUpdatedL
// From MUpnpAVControlPointObserver
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::StateUpdatedL( CUpnpService* /*aService*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::HttpResponseL
// From MUpnpAVControlPointObserver
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::HttpResponseL( CUpnpHttpMessage* /*aMessage*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDiscoveredL
// From MUpnpAVControlPointObserver
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDiscoveredL( CUpnpDevice* /*aDevice*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDisappearedL
// From MUpnpAVControlPointObserver
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDisappearedL(CUpnpDevice* /*aDevice*/)
    {
    // No implementation required        
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
        ChangeState( EStateShutDown );
        CActiveScheduler::Stop();
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ConnectionLost
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ConnectionLost( TBool /*aUserOriented*/ )
    {
    __LOG( "CUpnpAVControllerServer::ConnectionLost" );
    
    iShutdownTimeoutValue = 0; // Set shutdown timer value to 0, we want to
    // shut down the server immidiately after the last session has been
    // closed
    
    if( iState == EStateRunning && iDeviceRepository )
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
        ChangeState( EStateShuttingDown );
        }
    else if( iState == EStateStartingServer )
        {
        __LOG( "ConnectionLost - Server starting" );
        
        ChangeState( EStateShuttingDown );
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
    __LOG2( "CUpnpAVControllerServer::RunError msg: %d err: %d",
        Message().Function(), aError );

    if ( aError == KErrBadDescriptor )
        {
        PanicClient( Message(), EAVControllerServerBadDescriptor );
        }
    else if ( !Message().IsNull() )
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
    __LOG1( "CUpnpAVControllerServer::PanicClient %d", aPanic );
       
    aMessage.Panic( KAVControllerName, aPanic );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::PanicServer
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::PanicServer(TAVControllerServerPanic aPanic)
    {
    __LOG1( "CUpnpAVControllerServer::PanicServer %d", aPanic );
    
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

    __LOG( "CUpnpAVControllerServer::ThreadFunctionL end" );
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

    __LOG1( "CUpnpAVControllerServer::ThreadFunction end %d", err );

    return err;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::IncrementSessions
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::IncrementSessions() 
    {
    __LOG2( "CUpnpAVControllerServer::IncrementSessions, %d, %d", 
            iSessionCount, iState );
    
    iSessionCount++;
    if( iServerTimer->IsActive() )
        {
        iServerTimer->Cancel();
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DecrementSessions
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DecrementSessions()
    {
    __LOG2( "CUpnpAVControllerServer::DecrementSessions, %d, %d", 
            iSessionCount, iState );
    
    iSessionCount--;
    if( iState != EStateShutDown )
        {
        if ( iSessionCount <= 0 )
            {
            if( iServerTimer->IsActive() )
                {
                iServerTimer->Cancel();
                }
            iServerTimer->Start( iShutdownTimeoutValue );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDiscoveredL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDiscoveredL( CUpnpDevice& aDevice )
    {
    __LOG( "CUpnpAVControllerServer::DeviceDiscoveredL" );

    iDeviceRepository->AddDeviceL( aDevice );

    CUpnpAction* action = iAVControlPoint->CreateActionLC( 
            &aDevice, KConnectionManager, KGetProtocolInfo );

    iAVControlPoint->SendL( action ); // takes ownership
    CleanupStack::Pop( action );
    iDispatcher->RegisterL( action->SessionId(), *this );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceDisappearedL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceDisappearedL( CUpnpDevice& aDevice )
    {
    __LOG( "CUpnpAVControllerServer::DeviceDisappearedL" );
    
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
    TPtrC8 uuid( aDevice.Uuid() );
    iDeviceRepository->Remove( uuid );
    // Ensure that icon download is canceled
    iIconDownloader->CancelDownload( uuid );
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
    // Ensure that icon download is canceled
    iIconDownloader->CancelDownload( aUuid );

    __LOG( "CUpnpAVControllerServer::DeviceDisappearedL uid End" );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::CmProtocolInfoResponse
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::CmProtocolInfoResponse( const TDesC8& aUuid,
    TInt aErr, const TDesC8& aSource,
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
            // Start icon download if icon url is defined
            TPtrC8 iconUrl( dev->IconUrl() );
            if ( iconUrl.Length() > 0 )
                {
                TRAP_IGNORE( iIconDownloader->StartDownloadL( dev->Uuid(), iconUrl ) );
                }
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

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::TransferDeviceIconFileToClientL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::TransferDeviceIconFileToClientL(
        const RMessage2& aMessage, TInt aSlot, const TDesC8& aDeviceUuid )
    {
    return iIconDownloader->TransferFileToClientL( aMessage, aSlot, aDeviceUuid );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::HandleFailedProtocolInfoResponse
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::HandleFailedProtocolInfoResponse(
    const TDesC8& aUuid )
    {
    __LOG( "CUpnpAVControllerServer::HandleFailedProtocolInfoResponse" );
    
    iDeviceRepository->Remove( aUuid );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::ChangeState
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::ChangeState( TAVControllerServerState aState )
    {
    __LOG( "CUpnpAVControllerServer::ChangeState" );
    
    if( iState != aState )
        {
        __LOG2( "ChangeState: Changing state [%d] -> [%d]",
            iState, aState );
        iState = aState;
        }
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

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::IAP
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
TInt CUpnpAVControllerServer::IAP()
    {
    return iIAP;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerServer::DeviceIconDownloadedL
// See upnpavcontrollerserver.h
// --------------------------------------------------------------------------
void CUpnpAVControllerServer::DeviceIconDownloadedL( const TDesC8& aDeviceUuid,
        TInt aError )
    {
    __LOG( "CUpnpAVControllerServer::DeviceIconDownloadedL" );
    if ( aError == KErrNone )
        {
        // Get a corresponding device from the device repository
        CUpnpAVDeviceExtended& tmp = iDeviceRepository->FindDeviceL(
            aDeviceUuid );
        // Let the clients know about downloaded icon
        CSession2* s;
        iSessionIter.SetToFirst(); 
        while ( ( s = iSessionIter++ ) != NULL )
            {
            CUpnpAVControllerSession* sess =
                static_cast<CUpnpAVControllerSession*>( s );
            if ( sess )
                {
                sess->DeviceIconDownloadedL( tmp );
                }
            }
        }
    __LOG( "CUpnpAVControllerServer::DeviceIconDownloadedL End" );
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

