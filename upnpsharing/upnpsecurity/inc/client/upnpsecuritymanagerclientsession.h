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
 * Description:  upnpsecuritymanager / client side interface
 *
 */

#ifndef R_UPNPSECURITYMANAGERCLIENTSESSION_H
#define R_UPNPSECURITYMANAGERCLIENTSESSION_H

#include "upnpsessionbase.h"
#include "upnpsecaccesscontroller.h"    //TAccessType
#include <in_sock.h>

/**
 *  Client session to UPnP Security Manager
 *
 *  @lib upnpsecuritymanagerclient
 */
NONSHARABLE_CLASS( RUpnpSecurityManagerClientSession ) : public RUpnpSessionBase
    {
public:

    /**
     * Constructor.
     */
    IMPORT_C RUpnpSecurityManagerClientSession();

    /**
     * Establish connection with server
     */
    IMPORT_C TInt Connect();

    /**
     * Adds new file to allowed file array, if not existing yet
     *
     * @param aFileName file path+name to new allowed file
     */
    IMPORT_C void AddFileL( const TDesC& aFileName );

    /**
     * Removes file from allowed array
     *
     * @param aFileName File path+name to file to be removed
     */
    IMPORT_C void RemoveFileL( const TDesC& aFileName );

    /**
     * Adds new address to allowed address array, if not existing yet
     *
     * @param aIpAddress containing IP Address of the remote party
     */
    IMPORT_C void AddAddressL( const TInetAddr& aIpAddress );

    /**
     * Removes address from allowed array
     *
     * @param aIpAddress containing IP Address to be removed
     */
    IMPORT_C void RemoveAddressL( const TInetAddr& aIpAddress );

    /**
     * Resets file access list
     */
    IMPORT_C void ResetFileListL();

    /**
     * Resets address access list
     */
    IMPORT_C void ResetAddressListL();

    /**
     * Checks if ipAddress and file exist on access list
     *
     * @param aIpAddress  IP Address 
     * @param aFileName path+name to file 
     */
    IMPORT_C TAccessType CheckAuthorizationL(
            const TInetAddr& aIpAddress,
            const TDesC& aFileName );

    /**
     * Checks if ipAddress and file exist on access list
     *
     * @param  aIpAddress containing IP Address to be
     *        removed
     */
    IMPORT_C TAccessType QueryAuthorisationL( const TInetAddr& aIpAdress );

    };

#endif // R_UPNPSECURITYMANAGERCLIENTSESSION_H
// End of File
