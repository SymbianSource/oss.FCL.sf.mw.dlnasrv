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
* Description:      a base class for browsing and rendering sessions
*
*/






#ifndef C_UPNPAVSESSIONBASE_H
#define C_UPNPAVSESSIONBASE_H

#include <e32std.h>

class CUpnpAVDevice;

/**
 *  UPnP Session base
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class MUPnPAVSessionBase
    {

public:

    /**
     * Returns the device we have a session with
     *
     * @since Series 60 3.1     
     * @return device
     */
    virtual const CUpnpAVDevice& Device() const = 0;
        
    /**
     * Reserves local Media Server services.
     *
     * @since Series 60 3.1     
     */
    virtual void ReserveLocalMSServicesL() = 0; 

    /**
     * Cancels reserving of local Media Server services.
     *
     * @since Series 60 3.1     
     */
    virtual void CancelReserveLocalMSServicesL() = 0; 

    /**
     * Releases local Media Server services.
     *
     * @since Series 60 3.1     
     */
    virtual void ReleaseLocalMSServicesL() = 0; 

protected:
    
    virtual ~MUPnPAVSessionBase() {};
    
    };


#endif // C_UPNPAVSESSIONBASE_H
