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
#include "upnpconnectionmonitorobserver.h"
#include "upnpavcontrolpointobserver.h"
#include "upnpavtimer.h"
#include "upnpdeviceicondownloader.h"

// FORWARD DECLARATIONS
class CUpnpAVControlPoint;
class CUPnPAVDispatcher;
class CUPnPConnectionMonitor;
class CUPnPDeviceRepository;
class CUpnpSettings;

// CLASS DECLARATION

/**
*  UPnP Media Server container.
*  Provides interface for Media Server maintanace purposes.
*
*  @lib - 
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS ( CUpnpAVControllerServer ) :  public CPolicyServer,
                                 private MUPnPAVTimerCallback,
                                 private MUPnPConnectionMonitorObserver,
                                 private MUpnpAVControlPointObserver,
                                 private MUpnpDeviceIconDownloadObserver
    {

private: // Internal server state

    enum TAVControllerServerState
        {
        EStateUndefined = 0,
        EStateStartingServer,
        EStateStartingControlPoint,
        EStateRunning,
        EStateShuttingDown,
        EStateShutDown
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
    
    /**
     * Return a IAP
     *
     * @return a IAP
     */
    TInt IAP();
         
    /**
     * First stage startup for the server thread
     * 
     * @return return KErrNone or panics thread
     */
    static TInt ThreadFunction();
        
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
        TInt aErr,
        const TDesC8& aSource, 
        const TDesC8& aSink );

    /**
     * Transfers device icon file to the client
     * 
     * @param aMessage message from client
     * @param aSlot message slot to be used
     * @param aDeviceUuid the device that's icon is transferred
     */
    void TransferDeviceIconFileToClientL( const RMessage2& aMessage, TInt aSlot,
        const TDesC8& aDeviceUuid );

private:  // From CActive

    /**
     * Process any errors
     * @param aError the leave code reported.
     * @result return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );
    
private: // From MUPnPAVTimerCallback

    /**
     * See upnpavtimercallback.h
     */
    void UPnPAVTimerCallback( CUPnPAVTimer::TAVTimerType aType ); 

private: // From CUPnPConnectionMonitorObserver

    /**
     * See upnpconnectionmonitorobserver.h
     */
    void ConnectionLost( TBool aUserOriented );

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
    
private: // MUpnpAVControlPointObserver

    /**
    * @see MUpnpAVControlPointObserver::ActionResponseL
    */
    void ActionResponseL( CUpnpAction* aAction );
    
    /**
    * @see MUpnpAVControlPointObserver::StateUpdatedL
    */
    void StateUpdatedL( CUpnpService* aService );
    
    /**
    * @see MUpnpAVControlPointObserver::HttpResponseL
    */
    void HttpResponseL( CUpnpHttpMessage* aMessage );
    
    /**
    * @see MUpnpAVControlPointObserver::DeviceDiscoveredL
    */
    void DeviceDiscoveredL( CUpnpDevice* aDevice );
    
    /**
    * @see MUpnpAVControlPointObserver::DeviceDisappearedL
    */
    void DeviceDisappearedL( CUpnpDevice* aDevice );
    
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
    
    /**
     * Change server state.
     */
    void ChangeState( TAVControllerServerState aState );

private: // From CServer

    /**
     * Create a time server session, and return a pointer to the created
     * object
     * @param aVersion the client version 
     * @result pointer to new session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage )  const;
    

private: // MUpnpDeviceIconDownloadObserver

    void DeviceIconDownloadedL( const TDesC8& aDeviceUuid, TInt aError );

private:
    
    /** @var iSessionCount the number of session owned by this server */
    TInt iSessionCount;
    
    CUpnpAVControlPoint*        iAVControlPoint; // Own
    
    CUPnPAVDispatcher*          iDispatcher; // Own

    CUPnPAVTimer*               iServerTimer; // Own
    
    CUPnPConnectionMonitor*     iMonitor; // Own
            
    CUPnPDeviceRepository*      iDeviceRepository; // Own
        
    TInt                        iShutdownTimeoutValue;
    
    TInt                        iIAP;    

    TAVControllerServerState    iState;
    
    CUpnpSettings*              iUpnpSettings; // Own

    CUpnpDeviceIconDownloader*  iIconDownloader; // Own
    };


#endif // C_CUPNPAVCONTROLLERSERVER_H
