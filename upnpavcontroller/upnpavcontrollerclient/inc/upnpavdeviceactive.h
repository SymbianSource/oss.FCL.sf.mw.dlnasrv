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
* Description:      AO for monitoring device disconnections per session
*
*/






#ifndef C_UPNPAVDEVICEACTIVE_H
#define C_UPNPAVDEVICEACTIVE_H

#include <e32std.h>
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h"

class RUPnPAVControllerClient;
class MUPnPAVDeviceObserver;

/**
 *  Device active
 *
 *  Active object for listening if a device disappears.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVDeviceActive : public CActive
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aServer session handle
     * @param aObserver device observer
     * @return an instance
     */
    static CUPnPAVDeviceActive* NewL( RUPnPAVControllerClient& aServer,
        MUPnPAVDeviceObserver& aObserver );
    
    /**
     * Destructor
     */
    virtual ~CUPnPAVDeviceActive();
       
private:

    /**
     * Constructor.
     */
    CUPnPAVDeviceActive( RUPnPAVControllerClient& aServer,
        MUPnPAVDeviceObserver& aObserver );
    
    /**
     * 2nd phase construct
     */
    void ConstructL();

protected: // From CActive

    /**
     * See e32base.h
     */
    void RunL();
    
    /**
     * See e32base.h
     */
    void DoCancel();
    
    /**
     * See e32base.h
     */
    TInt RunError( TInt aError );

public: // New functions    

    /**
     * Starts listening
     *
     * @param aId session id
     */
    void StartListening( TInt aId );

private:

    RUPnPAVControllerClient& iServer;
    
    TInt iId;

    MUPnPAVDeviceObserver& iObserver;       
    
    };

#endif // C_UPNPAVDEVICEACTIVE_H
