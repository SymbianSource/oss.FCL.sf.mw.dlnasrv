/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Media Server Client
*
*/

#ifndef C_UPNPMEDIASERVERSTATUSWATCHER_H
#define C_UPNPMEDIASERVERSTATUSWATCHER_H

// INCLUDES

#include <e32base.h>
#include "upnpmediaserverclient.h"

    
/**
* Observer for media server status chnges
**/
class MUpnpMediaServerStatusObserver
    {
public:
    // media server status
    enum TServerState
        {
        EStartedOnline,
        EStartedOffline,
        EStopped
        };
    
public:
    /**
    * Inform about media server status change event
    * called when media server status has changed
    * @param aState new media server status
    */
    virtual void StatusChanged( MUpnpMediaServerStatusObserver::TServerState aState ) = 0;
    
    /**
    * Inform about subscribtion error
    * called when media server status subscription has failed
    * @param aError subscription error
    */
    virtual void SubscribeError( TInt aError ) = 0;
    };

/**
* Watches media server status
**/
NONSHARABLE_CLASS( CUpnpMediaServerStatusWatcher ): public CActive
    {           
public:
    /**
    * Two-phased constructor. Subscribes events for media server status
    * session to server is maintained by the class
    * @since Series 60 5.0 HN
    * @param aObserver event observer    
    * @return watcher object
    */
    IMPORT_C static CUpnpMediaServerStatusWatcher* NewL( 
                                    MUpnpMediaServerStatusObserver& aObserver );

    /**
    * Two-phased constructor. Subscribes events for media server status
    * session to server is maintained outside the class
    * @since Series 60 5.0 HN
    * @param aClient that must be opened and closed outside the class
    * @param aObserver event observer
    * @return watcher object
    */
    IMPORT_C static CUpnpMediaServerStatusWatcher* NewL( RUpnpMediaServerClient& aClient,
                                    MUpnpMediaServerStatusObserver& aObserver );
                                    
    /**
    * Destructor. Usubscribes
    */                                 
    ~CUpnpMediaServerStatusWatcher();
    
    /**
    * Subscribes to server
    * called during construction
    * @since Series 60 5.0 HN
    */
    IMPORT_C void Subscribe();
    
private:
    /**
    * Constructor.
    */
    CUpnpMediaServerStatusWatcher( MUpnpMediaServerStatusObserver& aObserver );

    /**
    * Constructor.
    */
    CUpnpMediaServerStatusWatcher( RUpnpMediaServerClient& aClient,
                                                MUpnpMediaServerStatusObserver& aObserver );
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();    

    /**
    * Dispatch response from server
    * @param aStatus current media server status
    * @since Series 60 5.0 HN
    */
    void DispatchResponseStatusEvent( TInt aStatus );
    
private:    // from CActive
    /**
    * RunL
    */
    void RunL();
    
    /**
    * RunError
    */
    TInt RunError( TInt aError );
    
    /**
    * DoCancel
    */
    void DoCancel();   

private:    
    
    // current media server status
    TInt iMediaServerStatus;
    
    // determines if session to server is maintaned by the class
    TBool iSessionOwner;
    
    // media server client
    RUpnpMediaServerClient iMediaServerClient;
    
    // event observer
    MUpnpMediaServerStatusObserver& iStatusEventObserver;      

	// the status read from media server
    TInt iCurrentStatus;
    };        
    
#endif // C_UPNPMEDIASERVERSTATUSWATCHER_H

// End of File
