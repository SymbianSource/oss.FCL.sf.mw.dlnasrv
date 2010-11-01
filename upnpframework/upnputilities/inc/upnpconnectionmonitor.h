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

#include "upnpconnectionmonitorobserver.h"

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
    IMPORT_C static CUPnPConnectionMonitor* NewL( TInt aAccessPoint );
    
    /**
     * Destructor.
     */
    virtual ~CUPnPConnectionMonitor();

    /**
     * Sets connection observer. Can be used if observer changes 
     * after creation of connection monitor.
     * @param aObserver Observer.
     */
    IMPORT_C void SetObserver( MUPnPConnectionMonitorObserver& aObserver );
    
    /**
     * Requests a notification when requested access point is available.
     * To avoid high power consumption, access point is observed some
     * minutes. If access point is available earlier, IapAvailable is 
     * called and monitoring stopped. Observation of access point can 
     * be canceled with NotifyIapCancel at any point.
     * @param aAccessPoint Access point from which notification is requested.
     */
    IMPORT_C void NotifyIap( TInt aAccessPoint );
    
    /**
     * Stops scanning of access point started with NotifyIap.
     */
    IMPORT_C void NotifyIapCancel();
    
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
    CUPnPConnectionMonitor( TInt aAccessPoint );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Checks connection type
     */
    TBool IsWlanConnection( TInt aConnectionId);

    /**
     * Checks current connection ids what is wlan id
     * and stores it to iConnectionId
     */
    void  ParseCurrentConnections();
    
    /**
     * Callback for iap observation stop timer.
     */
    static TInt TimeoutCallback( TAny* aSelf );

    /**
     * Implementation of timeout handling.
     */
    void StopIapObservation();
    
    /**
     * Cancels and deletes timeout timer.
     */
    void DeleteTimeoutTimer();

private:
    
    enum EMonitorState
        {
        EMonitorStateWait,
        EMonitorStateIap
        };

private:    // Data

    // Connection monitor server
    RConnectionMonitor              iConnectionMonitor;

    // Connection id on connection creation
    TInt                            iConnectionIdOnCreate;

    // Connection id on connection deletion
    TInt                            iConnectionId;

    // Callback pointer, not owned
    MUPnPConnectionMonitorObserver* iObserver;

    // Accesspoint to be observed
    TInt                            iAccessPoint;
    
    // Indication of scan request
    TBool                           iMonitorState;
    
    // Buffer for iap information
    TConnMonIapInfoBuf              iIapBuf;
    
    // Timer used when scanning iap at intervals
    RTimer                          iTimer;
    
    // Timer used to avoid scanning of access point
    // forever if client does not stop scanning.
    CPeriodic*                      iTimeout;
    };

#endif // C_UPNPCONNECTIONMMONITOR_H
            
// End of File
