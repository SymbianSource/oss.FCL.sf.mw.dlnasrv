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
* Description:      AO for monitoring WLAN connection breakdown
*
*/






#ifndef C_UPNPAVCONNECTIONACTIVE_H
#define C_UPNPAVCONNECTIONACTIVE_H

#include <e32std.h>
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h"

class RUPnPAVControllerClient;
class MUPnPConnectionMonitorObserver;

/**
 *  Active object for connection monitoring.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVConnectionActive : public CActive
    {

public:

    /**
     * Two-phased constructor.
     */
    static CUPnPAVConnectionActive* NewL( RUPnPAVControllerClient& aServer,
        MUPnPConnectionMonitorObserver& aObserver );

    /**
     * Destructor
     */    
    virtual ~CUPnPAVConnectionActive();
       
private:

    /**
     * Constructor
     */
    CUPnPAVConnectionActive( RUPnPAVControllerClient& aServer,
        MUPnPConnectionMonitorObserver& aObserver );

    /**
     * Two-phased constructor.
     */    
    void ConstructL();

protected: // From CActive

    void RunL();
    
    void DoCancel();
    
    TInt RunError( TInt aError );

public: // New functions    

    /**
     * Starts monitoring WLAN connection
     */
    void StartMonitoring();

private:

    RUPnPAVControllerClient& iServer;

    MUPnPConnectionMonitorObserver& iObserver;       
    
    };

#endif // C_UPNPAVCONNECTIONACTIVE_H
