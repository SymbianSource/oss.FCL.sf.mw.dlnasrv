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

const TInt KWaitIapInterval = 10000000; // 10 seconds
const TInt KWaitIapMaximum = 180000000; // 3 minutes

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPConnectionMonitor* CUPnPConnectionMonitor::NewL( TInt aAccessPoint )
    {
    CUPnPConnectionMonitor* self = new(ELeave) CUPnPConnectionMonitor( aAccessPoint );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::~CUPnPConnectionMonitor()
// Destructor
// ---------------------------------------------------------------------------
//
CUPnPConnectionMonitor::~CUPnPConnectionMonitor()
    {
    Cancel();

    // Disconnect from CM server
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    iTimer.Close();
    DeleteTimeoutTimer();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::SetObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CUPnPConnectionMonitor::SetObserver(
    MUPnPConnectionMonitorObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::NotifyIap
// ---------------------------------------------------------------------------
//
EXPORT_C void CUPnPConnectionMonitor::NotifyIap( TInt aAccessPoint )
    {
    __LOG1( "CUPnPConnectionMonitor::NotifyIap, aAccessPoint %d", aAccessPoint );
    iAccessPoint = aAccessPoint;
    if( IsActive() )
        {
        Cancel();
        }
    DeleteTimeoutTimer();
    // Create a timer for switching off Iap observation.
    iTimeout = CPeriodic::NewL( CActive::EPriorityStandard );
    iTimeout->Start( KWaitIapMaximum, KWaitIapMaximum, TCallBack( TimeoutCallback, this ) );
    // EConnMonIapAvailabilityChange event is generated only is there is
    // background scanning active. That is why iap is checked actively here.
    iMonitorState = EMonitorStateWait;
    iTimer.After( iStatus, KWaitIapInterval );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::NotifyIapCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CUPnPConnectionMonitor::NotifyIapCancel()
    {
    __LOG( "CUPnPConnectionMonitor::NotifyIapCancel" );
    DeleteTimeoutTimer();
    Cancel();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::DoCancel()
// Active object cancel implementation
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::DoCancel()
    {
    __LOG( "CUPnPConnectionMonitor::DoCancel" );
    iTimer.Cancel();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::RunL()
// Active object run loop
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::RunL()
    {
    __LOG2( "CUPnPConnectionMonitor::RunL, state %d, iStatus %d", 
        iMonitorState, iStatus.Int() );
    
    switch( iMonitorState )
        {
        case EMonitorStateWait:
            {
            iMonitorState = EMonitorStateIap;
            iConnectionMonitor.GetPckgAttribute(
                    EBearerIdWLAN,
                    0,
                    KIapAvailability,
                    iIapBuf,
                    iStatus );
            SetActive();
            break;
            }
        case EMonitorStateIap:
            {
            TBool found( EFalse );
            TConnMonIapInfo iaps = iIapBuf();
            TInt count( iaps.Count() );
            __LOG2( "CUPnPConnectionMonitor::RunL, iapCount %d, iAccessPoint %d", 
                count, iAccessPoint );
            for( TInt i = 0; i < count; i++ )
                {
                __LOG1( "CUPnPConnectionMonitor::RunL, iap %d", iaps.iIap[i].iIapId );
                if( iaps.iIap[i].iIapId == iAccessPoint )
                    {
                    found = ETrue;
                    DeleteTimeoutTimer();
                    if( iObserver )
                        {
                        iObserver->IapAvailable( iAccessPoint );
                        }
                    break;
                    }
                }
            if( !found )
                {
                iMonitorState = EMonitorStateWait;
                iTimer.After( iStatus, KWaitIapInterval );
                SetActive();
                }
            break;
            }
        default:
            break;
        }
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
                __LOG1( "CUPnPConnectionMonitor::EventL EConnMonCreateConnection \
WLAN connection found, connectionId %d", connectionId );
                if( iObserver )
                    {
                    iObserver->ConnectionCreated( connectionId );
                    }
                iConnectionIdOnCreate = connectionId;
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
            __LOG3( "CUPnPConnectionMonitor::EventL EConnMonDeleteConnection \
WLAN connection lost, connectionId %d (%d, %d)", 
                connectionId, iConnectionId, iConnectionIdOnCreate );
            
            // If there is new id for wlan we will pass if statement
            // because then the current is invalid then
            ParseCurrentConnections();
            if( connectionId == iConnectionId || 
                connectionId == iConnectionIdOnCreate )
                {
                TBool authoritativeDelete = eventDelete->AuthoritativeDelete();
                __LOG1( "CUPnPConnectionMonitor::EventL EConnMonDeleteConnection \
authoritativeDelete %d", authoritativeDelete );
                if( iObserver )
                    {
                    iObserver->ConnectionLost( authoritativeDelete );
                    }
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
// CUPnPConnectionMonitor::CUPnPConnectionMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUPnPConnectionMonitor::CUPnPConnectionMonitor( TInt aAccessPoint ) :
    CActive( EPriorityStandard ),
    iAccessPoint( aAccessPoint )
    {
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::ConstructL()
    {
    __LOG( "CUPnPConnectionMonitor::ConstructL" );

    CActiveScheduler::Add( this );

    iConnectionMonitor.ConnectL();
    iConnectionMonitor.NotifyEventL( *this );

    ParseCurrentConnections();
    User::LeaveIfError(iTimer.CreateLocal());
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
// CUPnPConnectionMonitor::TimeoutCallback
// ---------------------------------------------------------------------------
//
TInt CUPnPConnectionMonitor::TimeoutCallback( TAny* aSelf )
    {
    __LOG( "CUPnPConnectionMonitor::TimeoutCallback" );

    if( aSelf )
        {
        CUPnPConnectionMonitor* self = static_cast<CUPnPConnectionMonitor*>( aSelf );
        self->StopIapObservation();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::StopIapObservation
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::StopIapObservation()
    {
    __LOG( "CUPnPConnectionMonitor::StopIapObservation" );
    DeleteTimeoutTimer();
    Cancel();
    }

// ---------------------------------------------------------------------------
// CUPnPConnectionMonitor::DeleteTimeoutTimer
// ---------------------------------------------------------------------------
//
void CUPnPConnectionMonitor::DeleteTimeoutTimer()
    {
    __LOG( "CUPnPConnectionMonitor::DeleteTimeoutTimer" );
    if( iTimeout )
        {
        iTimeout->Cancel();
        delete iTimeout;
        iTimeout = 0;
        }
    }

// end of file
