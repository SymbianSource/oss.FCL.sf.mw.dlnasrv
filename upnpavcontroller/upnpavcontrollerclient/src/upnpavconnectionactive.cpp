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
* Description:      AO that monitors WLAN connection in client side
*
*/






#include "upnpavconnectionactive.h"

#include "upnpavcontrollerclient.h"

#include "upnpconnectionmonitorobserver.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::NewL
// Two-phase constructor
// --------------------------------------------------------------------------
CUPnPAVConnectionActive* CUPnPAVConnectionActive::NewL(
    RUPnPAVControllerClient& aServer,
    MUPnPConnectionMonitorObserver& aObserver )
    {
    CUPnPAVConnectionActive* self = new (ELeave) CUPnPAVConnectionActive(
        aServer, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::CUPnPAVConnectionActive
// Constructor
// --------------------------------------------------------------------------
CUPnPAVConnectionActive::CUPnPAVConnectionActive(
    RUPnPAVControllerClient& aServer,
    MUPnPConnectionMonitorObserver& aObserver ):
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::~CUPnPAVConnectionActive
// Destructor
// --------------------------------------------------------------------------
CUPnPAVConnectionActive::~CUPnPAVConnectionActive()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::ConstructL
// Two-phase constructor
// --------------------------------------------------------------------------
void CUPnPAVConnectionActive::ConstructL()
    {
        
    }

// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::RunL
// From CActive
// --------------------------------------------------------------------------
void CUPnPAVConnectionActive::RunL()
    {
    // Connection is lost
    if( iStatus.Int() == EAVControllerConnectionLost )
        {
        iObserver.ConnectionLost();
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::DoCancel
// From CActive
// --------------------------------------------------------------------------
void CUPnPAVConnectionActive::DoCancel()
    {
    iServer.CancelMonitorConnection(); // Ignore error
    }
    
// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::RunError
// From CActive
// --------------------------------------------------------------------------
TInt CUPnPAVConnectionActive::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVConnectionActive::StartMonitoring
// Starts monitoring
// --------------------------------------------------------------------------
void CUPnPAVConnectionActive::StartMonitoring()
    {
    __ASSERTD( !IsActive(), __FILE__, __LINE__ );
    iServer.MonitorConnection( iStatus );
    SetActive();
    }

// end of file
