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
* Description:      AV Controller server
*
*/






#ifndef C_CUPNPAVCONTROLLERSERVER_H
#define C_CUPNPAVCONTROLLERSERVER_H


// INCLUDES
#include "upnpavcontrollerserver.pan"

#include <upnpaction.h>
#include <upnpservice.h>
#include <upnpdevice.h>
#include <e32base.h>
#include <upnpmediaserverclient.h>
#include "upnpconnectionmonitorobserver.h"

#include "upnpavtimer.h"

// FORWARD DECLARATIONS
class CUpnpAVControlPoint;
class CUPnPAVDispatcher;
class CUPnPConnectionMonitor;
class CUPnPDeviceRepository;

// CLASS DECLARATION

/**
*  UPnP Media Server container.
*  Provides interface for Media Server maintanace purposes.
*
*  @lib - 
*  @since Series 60 3.1
*/
class CUpnpAVControllerServer :  public CPolicyServer,
                                 public MUPnPAVTimerCallback,
                                 public MUPnPConnectionMonitorObserver
    {

private: // Internal server state

    enum TAVControllerServerState
        {
        EStateUndefined = 0,
        EStateStartingUp,
        EStateRunning,
        EStateShuttingDown
        };    
    
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static CUpnpAVControllerServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CUpnpAVControllerServer();
    
public: // New functions        
     
    /**
     * Increment the count of the active sessions for this server
     */
    void IncrementSessions();

    /**
     * Decrement the count of the active sessions for this server. 
     */
    void DecrementSessions();
    
    /**
     * Handles UPnP device discoveries.
     * 
     * @param aDevice Device that is discovered.
     */
    void DeviceDiscoveredL( CUpnpDevice& aDevice);

    /**
     * Handles UPnP device disappears.
     * 
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpDevice& aDevice);
    
    /**
     * Handles UPnP device disappears.
     * 
     * @param aUuid Device that disappeared.
     */
    void DeviceDisappearedL( const TDesC8& aUuid );
    
    /**
     * Return a reference to the control point
     *
     * @return reference to the control point
     */
    CUpnpAVControlPoint& ControlPoint();

    /**
     * Return a reference to the s60 MS session
     *
     * @return reference to the s60 MS session
     */
    RUpnpMediaServerClient& MediaServer();
    
    /**
     * Return a reference to the callback dispatcher
     *
     * @return reference to the callback dispatcher
     */
    CUPnPAVDispatcher& Dispatcher();
    
    /**
     * Return a reference to the device repository
     *
     * @return reference to the device repository
     */
    CUPnPDeviceRepository& DeviceRepository();
    
    TInt IAP();
    
         
    /**
     * First stage startup for the server thread
     * 
     * @return return KErrNone or panics thread
     */
    static TInt ThreadFunction();
    
    /**
     * Starts the s60 MS if not started. Increments the reference ocunt
     * 
     * @param aMessage message
     */
    void StartMediaServerL( const RMessage2& aMessage );
    
    /**
     * Cancels the start
     * 
     * @param aMessage message
     */
    void CancelStartMediaServerL( const RMessage2& aMessage );

    /**
     * Stops the s60 MS if reference count hits 0
     * 
     * @param aMessage message
     */
    void StopMediaServerL( const RMessage2& aMessage );

    /**
     * Returns ETrue if someone is using the local Media Server
     * 
     * @param aMessage message
     */
    void MSServicesInUse( const RMessage2& aMessage );
    
    /**
     * Observer callback for Connection Manager GetProtocolInfo function.
     * 
     * @param aUuid Source device UUID. 
     * @param aSessionId 
     * @param aErr UPnP error code.
     * @param aSource
     * @param aSink
     */ 
    void CmProtocolInfoResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSource, 
        const TDesC8& aSink );
    
protected:  // From CActive

    /**
     * Process any errors
     * @param aError the leave code reported.
     * @result return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );
    
protected: // From MUPnPAVTimerCallback

    /**
     * See upnpavtimercallback.h
     */
    void UPnPAVTimerCallback( CUPnPAVTimer::TAVTimerType aType ); 

protected: // From CUPnPConnectionMonitorObserver

    /**
     * See upnpconnectionmonitorobserver.h
     */
    void ConnectionLost();

private: // 2nd phase construct

    /**
     * Constructs the server 
     *
     * @param aPriority CServer2 input parameter
     */
    CUpnpAVControllerServer(TInt aPriority);

    /**
     * Perform the second phase construction of a CUpnpMessageHandler object
     */
    void ConstructL() ;

public:
        
    /**
     * Perform the second phase startup. Starts up AV Control Point and
     * Dispatcher
     */
    void StartUpL();

    /**
     * Perform the second phase startup. Starts up AV Control Point and
     * Dispatcher
     */
    void CancelStartUp();
    
private: // New methods 

    /**
     * Panic client. 
     *
     * @param aMessage RMessage2
     * @param aPanic panic code
     */
    static void PanicClient( const RMessage2& aMessage,
        TAVControllerServerPanic aPanic );
  
    /**
     * Panic the server. 
     *
     * @param param aPanic the panic code
     * @return a updateId of container
     */
    static void PanicServer(TAVControllerServerPanic aPanic);
  
    /**
     * Second stage startup for the server thread 
     */
    static void ThreadFunctionL();
    
    /**
     * Handles embedded discovered device. Recoursive.
     *
     * @param aDevice device
     * @param aDepth device depth
     */
    void HandleEmbeddedDiscoveredDevicesL( CUpnpDevice& aDevice,
        TInt aDepth );
    
    /**
     * Handles embedded disappeared device. Recoursive.
     *
     * @param aDevice device
     * @param aDepth device depth
     */
    void HandleEmbeddedDisappearedDevicesL( CUpnpDevice& aDevice,
        TInt aDepth );
        
    /**
     * Stops the local media server
     */
    void StopMediaServer();
    
    /**
     * Error handler for failed protocolinfo-action
     *
     * @param aUuid device uuid
     * @param aDev pointer to the device in repository
     */
    void HandleFailedProtocolInfoResponse( const TDesC8& aUuid );                  

private: // From CServer

    /**
     * Create a time server session, and return a pointer to the created
     * object
     * @param aVersion the client version 
     * @result pointer to new session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage )  const;
    
private:
    
    /** @var iSessionCount the number of session owned by this server */
    TInt iSessionCount;
    
    CUpnpAVControlPoint*        iAVControlPoint; // Own
    RUpnpMediaServerClient      iMediaServer; // Own
    
    CUPnPAVDispatcher*          iDispatcher; // Own

    CUPnPAVTimer*               iServerTimer; // Own
    
    CUPnPAVTimer*               iMSTimer; // Own

    CUPnPConnectionMonitor*     iMonitor; // Own
    
    TInt                        iServerUserCount;
    
    TBool                       iMediaServerOnline;
    
    TBool                       iStartingMS;
    
    TBool                       iMSActivatedBeforeStart;
    
    RPointerArray<RMessage2>    iStartMessages; // Own
    
    CUPnPDeviceRepository*      iDeviceRepository; // Own
    
    TInt                        iDiscoveredDeviceCount;    
    
    TInt                        iDisappearedDeviceCount;
    
    TInt                        iShutdownTimeoutValue;
    
    TInt                        iIAP;    

    TAVControllerServerState    iServerState;

    
    };


#endif // C_CUPNPAVCONTROLLERSERVER_H
