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
* Description:      AV Controller server session
*
*/






#ifndef C_CUPNPAVCONTROLLERSESSION_H
#define C_CUPNPAVCONTROLLERSESSION_H

// INCLUDES
#include "upnpavcontrollerglobals.h"

#include <e32base.h>


// FORWARD DECLARATIONS
class CUpnpAVControllerServer;
class CUPnPAVControllerImpl;
class CUpnpAVDeviceExtended;
class CUpnpDevice;

// CLASS DECLARATION

/** 
  An instance of class CUpnpAVControllerSession is created for each client
  */
class CUpnpAVControllerSession :    public CSession2
{

public: // New methods

    /**
    * Create a CUpnpAVControllerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpAVControllerSession* NewL(
        CUpnpAVControllerServer& aServer );

    /**
    * Create a CUpnpAVControllerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpAVControllerSession* NewLC(
        CUpnpAVControllerServer& aServer );

    /**
    * Destroy the object and release all memory objects
    */
    virtual ~CUpnpAVControllerSession();


public: // From CSession
    /**
    * Called after a service request from client; from class CSession
    * @param aMessage message from client
    *        (containing requested operation and any data)
    */
    void ServiceL( const RMessage2& aMessage );
	
public:

    /**
     * Handles UPnP device discoveries.
     * @since Series 60 2.6
     * @param aDevice Device that is discovered.
     */
    void DeviceDiscoveredL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Handles UPnP device disappears.
     * @since Series 60 2.6
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Handles UPnP device icon download completions.
     * @param aDevice Device that icon was downloaded.
     */
    void DeviceIconDownloadedL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Handles connection lost.
     */
    void ConnectionLost();

private: // New methods

    /**
    * Perform the first phase of two phase construction 
    */
    CUpnpAVControllerSession( CUpnpAVControllerServer& aServer );

    /**
    * Perform the second phase construction of the object
    */
    void ConstructL() ;

    /**
    * Causes the client thread to panic
    * @param panic code
    */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;
    
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceAvTransportCommandsL( const RMessage2& aMessage );
    
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceAvTransportVariablesL( const RMessage2& aMessage );
    
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceDeviceL( const RMessage2& aMessage );
    
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceSearchandBrowseL( const RMessage2& aMessage );
   
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceDownloadandUploadL( const RMessage2& aMessage );
    
    /**
     * ServiceL method extended ( cyclomatic complexity reduced)
     */
    void ServiceCommonL( const RMessage2& aMessage );

private: 

    /** @var iMediaServer pointer to Media Server */
    
    CUpnpAVControllerServer&    iAVControllerServer;
    CUPnPAVControllerImpl*      iAVController;
};

#endif // C_CUPNPAVCONTROLLERSESSION_H