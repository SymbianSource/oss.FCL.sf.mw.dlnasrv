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
* Description:      AO for receiving events from media renderer
*
*/






#ifndef C_UPNPAVEVENTACTIVE_H
#define C_UPNPAVEVENTACTIVE_H

#include <e32std.h>
#include <e32base.h>

#include "upnpavrenderingsessionobserver.h"
#include "upnpavcontrollerglobals.h"

class RUPnPAVControllerClient;
class CUPnPAVRenderingSessionImpl;

/**
 *  Active object for listening AV Events.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVEventActive : public CActive
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aServer session handle
     * @param aObserver device observer
     * @return an instance
     */
    static CUPnPAVEventActive* NewL( RUPnPAVControllerClient& aServer,
        CUPnPAVRenderingSessionImpl& aObserver );
    
    /**
     * Destructor
     */
    virtual ~CUPnPAVEventActive();
       
private:

    /**
     * Constructor
     */
    CUPnPAVEventActive( RUPnPAVControllerClient& aServer,
        CUPnPAVRenderingSessionImpl& aObserver );
    
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
     * Starts listening for events
     *
     * @param aId session id
     */
    void StartListeningEvents( TInt aId );

private:

    RUPnPAVControllerClient& iServer;
    
    TInt iId;

    TUnsolicitedEventC           iEvent;
    TPckg<TUnsolicitedEventC>    iEventPkg;
    
    CUPnPAVRenderingSessionImpl& iObserver;       
    
    };

#endif // C_UPNPAVEVENTACTIVE_H
