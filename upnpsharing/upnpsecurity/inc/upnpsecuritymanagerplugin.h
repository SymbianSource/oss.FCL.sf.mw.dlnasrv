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
 *  Description : Access control interface
 *
 */

#ifndef UPNPSECURITYMANAGERPLUGIN_H
#define UPNPSECURITYMANAGERPLUGIN_H

// INCLUDES
#include <e32base.h>
#include <upnpsecuritymanager.h>
#include "upnpsecaccesscontroller.h"

// CLASS DECLARATION

/**
 *  Upnp Security Manager Plugin class
 *  @since S60 3.1
 */
class CUpnpSecurityManagerPlugin : public CUpnpSecurityManager
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUpnpSecurityManagerPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CUpnpSecurityManagerPlugin();

public:
    // From UpnpSecurityManager

    /** 
     * Authorize HTTP message.
     *
     * @since S60 3.1
     * @param aMessage (CUpnpHttpMessage*) HTTP message
     * @param aRequestedFile (TFileName&) requested file location
     * @return authorization result (KErrNone if ok, KErrAccessDenied if
     *         not)
     */
    TInt AuthorizeMessage( CUpnpHttpMessage* aMessage,
        TFileName& aRequestedFile );

    /** 
     * Authorize SOAP action
     *
     * @since S60 3.1
     * @param aAction (CUpnpAction*) SOAP action   
     * @return authorization result (KErrNone if ok, KErrAccessDenied if
     *         not)
     */
    TInt AuthorizeAction( CUpnpAction* aAction );

private:

    /**
     * C++ default constructor.
     */
    CUpnpSecurityManagerPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    // Member variables

    //
    CUpnpSecAccessController* iAccessController; // owned
    };

#endif // UPNPSECURITYMANAGERPLUGIN_H
// End of File
