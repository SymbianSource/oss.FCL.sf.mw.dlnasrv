/** @file
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Control point with some basic AV services
*
*/



#ifndef C_CUPNPAVCONTROLPOINT_H
#define C_CUPNPAVCONTROLPOINT_H


//  INCLUDES
#include "upnpcontrolpoint.h"

// FORWARD DECLARATIONS
class MUpnpAVControlPointObserver;

// CLASS DECLARATION

/**
* Control point services.
* This class inherits control point base class and provides just a little
* added value on top of that. Some default construction has been taken
* care of, and some low level messaging is taken care of. Everything else
* is up to the client.
* 
* @lib - 
* @since S60 5.2
*/
NONSHARABLE_CLASS ( CUpnpAVControlPoint ): public CUpnpControlPoint
    {
public:  // Constructors and destructor
    
    /** static constructor. */
    static CUpnpAVControlPoint* NewL(
        MUpnpAVControlPointObserver& aAVControlPointObserver            
        );
    
    /** Destructor. */
    virtual ~CUpnpAVControlPoint();

private: // private construction part

    /** constructor */
    CUpnpAVControlPoint(MUpnpAVControlPointObserver& aAVControlPointObserver);

    /** 2nd phase constructor */
    void ConstructL();

public: // services

    /**
    * Returns the internally cached service for given device and 
    * service type.
    * 
    * @since S60 5.2
    * @param aDevice the device for which service is requested for.
    * @param aServiceType the upnp service required, for instance CDS.
    * @return Cached service.
    */
    CUpnpService* Service( 
            const CUpnpDevice* aDevice, 
            const TDesC8& aServiceType );
    
    /**
    * Helper method to create new action with given action name.
    * 
    * New action is created by internally cached service. The service is
    * identified with given device and service type.
    * 
    * @since S60 5.2
    * @param aDevice the device for which service is requested for
    * @param aServiceType the upnp service required, for instance CDS
    * @param aActionName Name of the action.
    * @return New action object
    * @leave KErrUnknown if internally cached service could not be found.
    *        KErrGeneral if service doesn't support action.
    *        Systemwide error code otherwise.
    */
    CUpnpAction* CreateActionLC( 
            const CUpnpDevice* aDevice, 
            const TDesC8& aServiceType,
            const TDesC8& aActionName );
    
    /**
     * Starts up the av control point by creating SSDP search.
     * 
     * @param None.
     * @return None.
     */
    void StartUpL();
    
private:  // Virtual method overrides from base classes
    
    /**
    * From CUpnpControlPoint Device discovery handler.
    */
    void DeviceDiscoveredL( CUpnpDevice* aDevice );

    /**
    * From CUpnpControlPoint Device dissappear handler.
    */
    void DeviceDisappearedL( CUpnpDevice* aDevice );

    /**
    * From CUpnpControlPoint Action response handler function.
    */
    void ActionResponseReceivedL( CUpnpAction* aAction );

    /**
    * From CUpnpControlPoint State update handler.
    */
    void StateUpdatedL( CUpnpService* aService );

    /**
    * From CUpnpControlPoint HTTP message handler function.
    */
    void HttpResponseReceivedL( CUpnpHttpMessage* aMessage );

    /**
    * This function will be invoke if some network event will occure
    * for example IP Address of UPnP changes
    */
    void NetworkEvent( CUpnpNetworkEventBase* aEvent );

private:    // Data

    //AV control point observer (engine)
    MUpnpAVControlPointObserver& iAVControlPointObserver;
    };

#endif      // C_CUPNPAVCONTROLPOINT_H
            
// End of File
