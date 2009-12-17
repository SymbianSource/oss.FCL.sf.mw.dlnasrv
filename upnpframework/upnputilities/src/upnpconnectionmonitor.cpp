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
* Description:  UPnP Connection Monitor class implementation.
*
*/


// INCLUDE FILES
#include <nifvar.h>

#include "upnpconnectionmonitor.h"

// logging
_LIT( KComponentLogfile, "upnputilities.txt");
#include "upnplog.h"

// CONSTANTS
#ifdef __UPNP_CONSOLE_MT__
_LIT( KConnectionBreakdownSimulationFile, "C:\\Data\\Wlan" );
#endif // __UPNP_CONSOLE_MT__



// ========================== MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::CUPnPConnectionMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUPnPConnectionMonitor::CUPnPConnectionMonitor(
    MUPnPConnectionMonitorObserver& aObserver, TInt aAccessPoint ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iAccessPoint( aAccessPoint )
    {
    }


// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPConnectionMonitor* CUPnPConnectionMonitor::NewL(
    MUPnPConnectionMonitorObserver& aObserver, TInt aAccessPoint )
    {
    CUPnPConnectionMonitor* self = new(ELeave) CUPnPConnectionMonitor(
        aObserver, aAccessPoint );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }


// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::ConstructL()
    {
    __LOG( "CUPnPConnectionMonitor::ConstructL" );

#ifdef __UPNP_CONSOLE_MT__

    // monitor filesystem for connection breakdown simulation
    CActiveScheduler::Add( this );
    User::LeaveIfError( iFs.Connect() );
    iFs.NotifyChange( ENotifyFile, iStatus,
        KConnectionBreakdownSimulationFile );
    SetActive();

#endif // __UPNP_CONSOLE_MT__

    iConnectionMonitor.ConnectL();
    iConnectionMonitor.NotifyEventL( *this );

    ParseCurrentConnections();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::ParseCurrentConnections()
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::ParseCurrentConnections()
    {    
    // Get the count of connections
    TRequestStatus status = KRequestPending;
    TUint connectionCount = 0;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest( status ); 
    // Go through available connections and check to see
    // WLAN connection is already running
    if( !status.Int() )
        {
        for( TUint i=1; i < connectionCount+1;  i++ )
            {
            TUint connectionId;
            TUint subConnectionCount;

            iConnectionMonitor.GetConnectionInfo( 
                                            i,
                                            connectionId, 
                                            subConnectionCount);

            if( IsWlanConnection( connectionId ) )
                {
                __LOG( "CUPnPConnectionMonitor - Found WLAN connection" );
                iConnectionId = connectionId;
                }
            }   
        }  
    __LOG2( "CUPnPConnectionMonitor::ParseCurrentConnections() \
    wlanId = %d connectionCount = %d ", iConnectionId , connectionCount );
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::~CUPnPConnectionMonitor()
// Destructor
// ---------------------------------------------------------------------------
//
CUPnPConnectionMonitor::~CUPnPConnectionMonitor()
    {
#ifdef __UPNP_CONSOLE_MT__

    // stop monitoring filesystem
    Cancel();
    iFs.Close();

#endif // __UPNP_CONSOLE_MT__

    // Disconnect from CM server
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::EventL()
// Receives event from connection monitor
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    __LOG1( "CUPnPConnectionMonitor::EventL type %d", aConnMonEvent.EventType() );
    TUint connectionId = 0;

    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonCreateConnection:
            {
            const CConnMonCreateConnection* eventCreate; 
            eventCreate = (const CConnMonCreateConnection*)&aConnMonEvent;
            connectionId = eventCreate->ConnectionId();

            // Save connectionId if type is WLAN
            if( IsWlanConnection( connectionId ))
                {
                __LOG( "CUPnPConnectionMonitor::EventL EConnMonCreateConnection \
WLAN connection found" );
                iConnectionId = connectionId;
                }

            break;
            }

        // Connection is deleted
        case EConnMonDeleteConnection:
            {
            const CConnMonDeleteConnection* eventDelete; 
            eventDelete = 
                    ( const CConnMonDeleteConnection* ) &aConnMonEvent;
            connectionId = eventDelete->ConnectionId();
            
            // If there is new id for wlan we will pass if statement
            // because then the current is invalid then
            ParseCurrentConnections();
            if( connectionId == iConnectionId )
                {
                __LOG( "CUPnPConnectionMonitor::EventL EConnMonDeleteConnection \
WLAN connection found" );
                iObserver.ConnectionLost();
                }
            break;
            }
         default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::IsWlanConnection()
// Checks if connection type is WLAN
// ---------------------------------------------------------------------------
//
TBool CUPnPConnectionMonitor::IsWlanConnection( TInt aConnectionId)
    {
    __LOG( "CUPnPConnectionMonitor::IsWlanConnection" );
    TBool ret = EFalse;
    TInt bearer = 0;
    TInt bearerinfo = 0;
    
    TRequestStatus status = KRequestPending;
    iConnectionMonitor.GetIntAttribute( 
                                    aConnectionId, 
                                    0, 
                                    KBearer, 
                                    (TInt &) bearer,
                                    status );
    User::WaitForRequest( status ); 
    
    TRequestStatus status2 = KRequestPending;
    iConnectionMonitor.GetIntAttribute( 
                                    aConnectionId, 
                                    0, 
                                    KBearerInfo, 
                                    (TInt &) bearerinfo,
                                    status2 );
    User::WaitForRequest( status2 ); 
    
    if( bearer == EBearerWLAN && bearerinfo == EBearerWLAN )
        {
        ret = ETrue;
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::RunL()
// Active object run loop
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::RunL()
    {
#ifdef __UPNP_CONSOLE_MT__
    // simulated connection break has been activated
    iObserver.ConnectionLost();
    iFs.Delete( KConnectionBreakdownSimulationFile );
#endif // __UPNP_CONSOLE_MT__
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::DoCancel()
// Active object cancel implementation
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::DoCancel()
    {
#ifdef __UPNP_CONSOLE_MT__
    // cancel notifications from FS
    iFs.NotifyChangeCancel();
#endif // __UPNP_CONSOLE_MT__    
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::DebugSimulateConnectionLostL()
// Simulate connection lost case.
// ---------------------------------------------------------------------------
//
EXPORT_C void CUPnPConnectionMonitor::DebugSimulateConnectionLostL()
    {
#ifdef __UPNP_CONSOLE_MT__
    // signal connection breakdown via filesystem
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    TInt err = fs.Delete( KConnectionBreakdownSimulationFile );
    if ( err != KErrNone &&
         err != KErrNotFound )
        {
        User::Leave( err );
        }
    
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Create( fs,
        KConnectionBreakdownSimulationFile, EFileWrite ) );
    
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    
#else // __UPNP_CONSOLE_MT__
    // connection simulation method called, but feature is not active !
    __PANICD( __FILE__, __LINE__ );
#endif // __UPNP_CONSOLE_MT__
    }

// end of file
