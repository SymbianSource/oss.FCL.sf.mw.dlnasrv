/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  CUpnpSecurityManagerServer
 *
 */

#ifndef C_CUPNPSECURITYMANAGERSERVER_H
#define C_CUPNPSECURITYMANAGERSERVER_H

// INCLUDES
#include <e32base.h>
#include "upnpsecuritymanagerclientservercommon.h"
#include "upnpsymbianserverbase.h"

// FORWARD DECLARATIONS
class CUpnpSecurityManagerEngine;

// CLASS DECLARATION

/**
 * 
 */
class CUpnpSecurityManagerServer : public CUpnpSymbianServerBase
    {
public:
    /**
     * Create a CUpnpSecurityManagerServer object using two phase construction,
     * and return a pointer to the created object, leaving a pointer to the
     * object on the cleanup stack.
     * @result pointer to created CUpnpSecurityManagerServer object
     */
    static CUpnpSymbianServerBase* NewLC();

    /**
     * Destroy the object and release all memory objects.
     */
    ~CUpnpSecurityManagerServer();

private:
    // from CUpnpSymbianServerBase

    /**
     * 
     */
    const TDesC& ServerName() const;

private:
    /**
     * Perform the first phase of two phase construction.
     */
    CUpnpSecurityManagerServer();

    /**
     * Perform the second phase construction of a CUpnpSecurityManagerServer object.
     */
    void ConstructL();

private:
    // From CUpnpSymbianServer

    /**
     * Create a time server session, and return a pointer to the created object.
     * @result pointer to new session
     */
    CSession2* NewSessionL( const RMessage2& aMessage ) const;

    /**
     * Returns version that is supported by this server.
     */
    virtual TVersion SupportedVersion() const;

private:
    //
    CUpnpSecurityManagerEngine* iEngine;
    };

#endif // C_CUPNPSECURITYMANAGERSERVER_H
// End of File
