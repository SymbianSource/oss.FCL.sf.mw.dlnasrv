/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      UPnP Connection Monitor class implementation.
*
*/






// INCLUDE FILES
#include <nifvar.h>

#include "mdhconnectionmonitor.h"

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CMdHConnectionMonitor::CMdHConnectionMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMdHConnectionMonitor::CMdHConnectionMonitor(
    MMdHConnectionMonitorObserver& aObserver, TInt aAccessPoint ) :
    iObserver( aObserver ),
    iAccessPoint( aAccessPoint )
    {
    }


// ---------------------------------------------------------------------------
// CMdHConnectionMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMdHConnectionMonitor* CMdHConnectionMonitor::NewL(
    MMdHConnectionMonitorObserver& aObserver, TInt aAccessPoint )
    {
    CMdHConnectionMonitor* self = new(ELeave) CMdHConnectionMonitor(
        aObserver, aAccessPoint );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }


// ---------------------------------------------------------------------------
// CMdHConnectionMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMdHConnectionMonitor::ConstructL()
    {    
    iConnectionMonitor.ConnectL();

    iConnectionMonitor.NotifyEventL( *this );

    // Get the count of connections
    TRequestStatus status = KRequestPending;
    TUint connectionCount = 0;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest( status ); 
    
    // Go through available connections and check to see
    // if connection with iAccessPoint IAP is already running
    if( status == KErrNone )
        {
        for( TUint i=1; i < connectionCount+1;  i++ )
            {
            TUint connectionId;
            TUint subConnectionCount;
            TUint iapId = 0;

            iConnectionMonitor.GetConnectionInfo( 
                                            i,
                                            connectionId, 
                                            subConnectionCount);

            TRequestStatus status = KRequestPending;

            iConnectionMonitor.GetUintAttribute( 
                                            connectionId, 
                                            0, 
                                            KIAPId, 
                                            (TUint &) iapId,
                                            status );
            User::WaitForRequest( status ); 

            // Save connectionId if same iap
            if( status == KErrNone && iAccessPoint == iapId )
                {
                iConnectionId = connectionId;
                }
            }    
        }    

    }

    
// ---------------------------------------------------------------------------
// CMdHConnectionMonitor::~CMdHConnectionMonitor()
// Destructor
// ---------------------------------------------------------------------------
//
CMdHConnectionMonitor::~CMdHConnectionMonitor()
    {
    iConnectionMonitor.CancelNotifications();

    // Disconnect from CM server
    iConnectionMonitor.Close();
    }



// ---------------------------------------------------------------------------
// CMdHConnectionMonitor::EventL()
// Connection monitor observer
// ---------------------------------------------------------------------------
//
void CMdHConnectionMonitor::EventL( const CConnMonEventBase& aConnMonEvent )
    {

    TUint connectionId = 0;

    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonCreateConnection:
            {
            TUint iapId = 0;

            const CConnMonCreateConnection* eventCreate; 
            eventCreate = (const CConnMonCreateConnection*)&aConnMonEvent;
            connectionId = eventCreate->ConnectionId();

            TRequestStatus status = KRequestPending;

            // Get IAP id
            iConnectionMonitor.GetUintAttribute( 
                                            eventCreate->ConnectionId(), 
                                            0, 
                                            KIAPId, 
                                            iapId, 
                                            status );
            User::WaitForRequest( status ); 

            // Save connectionId if same iap
            if( iAccessPoint == iapId )
                {
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

            // If connection id same as in createConnection
            if( connectionId == iConnectionId )
                {
                // Callback function
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

// end of file
