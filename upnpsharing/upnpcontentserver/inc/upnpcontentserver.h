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
* Description:      Header for upnpcontentserver 
*
*/






#ifndef __UPNPCONTENTSERVER_H__
#define __UPNPCONTENTSERVER_H__

// INCLUDES
#include <e32base.h>
#include "upnpconnectionmonitorobserver.h"
#include "upnpconnectionmonitor.h"
#include "upnpcontentserverhandler.h"
#include "upnpcontentserversession.h"

// FORWARD DECLARATIONS
class CUPnPPeriodic;

// Server shutdown delay in microseconds (2.5s)
const TInt KShutdownTimeout = 2500000;

// FORWARD DECLARATIONS
class CUpnpContentServerSession;

/**
*  A server class to initialize server.
*
*  @since S60 3.1
*/
class CUpnpContentServer : public CPolicyServer,
                           public MUPnPConnectionMonitorObserver
    {
public:
    /**
    * A Method to initialize the CleanupStack and the ActiveScheduler.
    * Creates an instance of CUpnpContentServer.
    * @param None.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *         error codes.
    */
    static TInt LaunchServerL();

    /**
    * 2-phased constructor.
    */
    static CUpnpContentServer* NewL();

    /**
    * C++ destructor.
    */
    virtual ~CUpnpContentServer();

    /**
     * Returns pointer CUpnpContentserverHandler instanse
     * @since S60 3.1
     */
    CUpnpContentServerHandler* Handler() const;
 
    /**
     * Decrements session counter
     * @since S60 3.1
     */
    void RemoveSession();

    /**
     * Decrements session counter
     * @since S60 3.1
     */
    void AddSession();

    /**
     * Indicates if server can be stopped
     * @since S60 3.1
     * @ return ETrue if there is no other session open
     */
    TBool CanStop() const;

    /**
     * Stop the server with delay
     * @since S60 3.1
     */
    void Stop();

    /**
     * Perform the shutdown of the server
     * @return KErrNone
     */
    static TInt Shutdown( TAny* aPtr );

    /**
     * Start the connection monitor if not yet started
     * Register for connection lost events
     * @since S60 3.1
     */
    TInt RequestConnectionLostL( const TInt aIapId );

    /**
     * Stop the connection monitor if only session
     * unregister the session from connection monitoring
     * @since S60 3.1
     */
    void CancelConnectionLostL();

public:
    /**
    * From CServer2, creates a new session.
    */
    CSession2* NewSessionL( const TVersion& aVersion, 
                            const RMessage2& aMessage ) const;

protected:
    /**
     * From CServer2, handle possible leaves
     */
    TInt RunError( TInt aError );

private:
    /**
    * C++ constructor.
    */
    CUpnpContentServer();

    /**
    * 2nd phase constructor.
    */
    void ConstructL( );

private:
    /**
     * From MUPnPConnectionMonitorObserver
     * Inform all sessions that connection has been lost
     * @since S60 3.1
     */
    void ConnectionLost();


private:

    /** 
     * Server has an object container index which 
     * creates an object container for each session
     * owned
     */
    CObjectConIx* iContainerIndex;

    /**
     * Pointer to handler
     * owned
     */
    CUpnpContentServerHandler* iHandler;

    /**
     * Session count
     */
    TInt iSessionCount;

    /**
     * Connection monitor handle
     */
    CUPnPConnectionMonitor* iConMon;

    /**
     * Id of the active IAP
     */
    TInt iActiveIapId;

    /**
     * Delayer for the server shutdown
     */
    CUPnPPeriodic* iIdle;

    };


#endif // __UPNPCONTENTSERVER_H__

