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
* Description:  Monitors for WLAN connection
*
*/

#ifndef C_UPNPCONNECTIONMMONITOR_H
#define C_UPNPCONNECTIONMMONITOR_H


// INCLUDES
#include <e32base.h>
#include <rconnmon.h>

#include <f32file.h> 

#include "upnpconnectionmonitorobserver.h"

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
* CUPnPConnectionMonitor class provides a WLAN connection monitor
* for UPnP applications.
*
* @lib upnputilities.lib
* @since S60 3.0
*/
class CUPnPConnectionMonitor :  public CActive,
                                public MConnectionMonitorObserver
    {
    
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUPnPConnectionMonitor* NewL(
        MUPnPConnectionMonitorObserver& aObserver, TInt aAccessPoint );
    
    /**
     * Destructor.
     */
    virtual ~CUPnPConnectionMonitor();

    /**
     * Simulate connection lost case. Calling this method will cause all
     * instances of the CUPnPConnectionMonitor to call back "ConnectionLost"
     * via the observer API.
     *
     * This method is for module test use only.
     */
    IMPORT_C static void DebugSimulateConnectionLostL();
    
    /**
     * Checks current connection ids what is wlan id
     * and stores it to iConnectionId
     */
    void  ParseCurrentConnections();
    
protected: // From CActive

    void DoCancel();
    
    void RunL();

protected: // From MConnectionMonitorObserver

    /**
     * Catches the Connection monitor events 
     * @since Series 60 3.0
     * @param aConnMonEvent event
     * @return none
     */
    void EventL( const CConnMonEventBase& aConnMonEvent ) ;
        
private:

    /**
     * C++ default constructor.
     */
    CUPnPConnectionMonitor( MUPnPConnectionMonitorObserver& aObserver,
        TInt aAccessPoint );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Checks connection type
     */
    TBool IsWlanConnection( TInt aConnectionId);


private:    // Data

    // file server (used only in debugging services)
    RFs iFs;

    // Connection monitor server
    RConnectionMonitor              iConnectionMonitor;

    // Connection id
    TInt                            iConnectionId;

    // Callback pointer, not owned
    MUPnPConnectionMonitorObserver& iObserver;

    // Accesspoint to be observed
    TInt                            iAccessPoint;
    };

#endif // C_UPNPCONNECTIONMMONITOR_H
            
// End of File
