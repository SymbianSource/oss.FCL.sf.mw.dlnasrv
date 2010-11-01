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
* Description:      dispatches control point indications for clients
*
*/






#ifndef C_CUPNPAVDISPATCHER_H
#define C_CUPNPAVDISPATCHER_H


#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpAction;
class CUpnpService;
class CUpnpHttpMessage;
class CUpnpDevice;
class CUPnPAVActionInfo;
class CUPnPAVControllerImpl;
class CUpnpAVControllerServer;


// CLASS DECLARATION

/**
*  UPnP AV Controller callback dispatcher
*  
*
*  @lib - 
*  @since Series 60 3.1
*/

class CUPnPAVDispatcher : public CBase,
                          public MUpnpAVControlPointObserver
    {
public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    static CUPnPAVDispatcher* NewLC( CUpnpAVControllerServer& aServer );
    
    /**
    * Two-phased constructor.
    */
    static CUPnPAVDispatcher* NewL( CUpnpAVControllerServer& aServer );    

    /**
    * Destructor.
    */
    virtual ~CUPnPAVDispatcher();

private: // New methods

    /**
    * Constructs the server 
    * @param aPriority CServer2 input parameter
    */
    CUPnPAVDispatcher( CUpnpAVControllerServer& aServer );

    /**
     * Perform the second phase construction of a CUpnpMessageHandler object
     */
    void ConstructL();

private: // from MUpnpAVControlPointObserver

    // dispatches this event to its true handler based on who's registered
    void ActionResponseL(CUpnpAction* aAction );

    // dispatches this event to its true handler based on who's registered
    void StateUpdatedL(CUpnpService* aService);

    // dispatches this event to its true handler based on who's registered
    void HttpResponseL(CUpnpHttpMessage* aMessage);

    // dispatches this event to its true handler based on who's registered
    void DeviceDiscoveredL(CUpnpDevice* aDevice);

    // dispatches this event to its true handler based on who's registered
    void DeviceDisappearedL(CUpnpDevice* aDevice);

public: // New methods
    
    /**
     * Register itself to get any observer call from AV control point
     * @param TInt the session id
     * @param MUpnpAVControlPointObserver& observer reference
     *
     * @return none
     */
    void RegisterL( TInt aSessionId,
        MUpnpAVControlPointObserver& aObserver );

    /**
     * UnRegister itself Not to get any observer call from AV control point
     * @param TInt the session id
     *
     * @return none
     */
    void UnRegister( TInt aSessionId /*, const TDesC8& aUuid*/ );

    /**
     * Register itself to get any events call from AV control point
     * @param MUpnpAVControlPointObserver& observer reference
     * @param TDesC8 the uid
     *
     * @return none
     */
    void RegisterForEventsL( MUpnpAVControlPointObserver& aObserver,
        const TDesC8& aUuid );
    
    /**
     * UnRegister itself Not to get any events call from AV control point
     * @param MUpnpAVControlPointObserver& observer reference
     *
     * @return none
     */
    void UnRegisterEvents( MUpnpAVControlPointObserver& aObserver );

private: // New methods
    
    /**
     * Find the observer via the session id
     * @param TInt session id
     *
     * @return MUpnpAVControlPointObserver& the reference to the observer
     */
    MUpnpAVControlPointObserver& FindObserver( TInt aSessionId );   

private:
    
    CUpnpAVControllerServer&            iServer;            

    RPointerArray<CUPnPAVActionInfo>    iActionInfos; //owned

    RPointerArray<CUPnPAVActionInfo>    iActionInfosEvent; //owned
    
    };

#endif // C_CUPNPAVDISPATCHER_H