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

#ifndef UPNPSECACCESSCONTROLLER_H
#define UPNPSECACCESSCONTROLLER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RUpnpSecurityManagerClientSession;
class TInetAddr;

// ENUMS
enum TAccessType
    {
    ENoneAllowed,
    EAddressAllowed,
    EFileAllowed,
    EFileAndAddressAllowed
    };

/**
 *  Access controller class
 *
 *  @lib upnpsecurity.lib
 *  @since S60 3.0
 */
class CUpnpSecAccessController : public CBase
    {

public:
    // Constructors and destructor.

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpSecAccessController* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpSecAccessController();

public:
    // S60 UpnpMediaServer security manager activation methods.

    /**
     * Enables the security plugin in Media Server.
     *
     * @since S60 3.1
     */
    IMPORT_C static void EnableMediaServerSecurityL();

    /**
     * Disables the security plugin in Media Server.
     *
     * @since S60 3.1
     */
    IMPORT_C static void DisableMediaServerSecurityL();

    /**
     * Returns the status of the Media Server security plugin.
     *
     * @since S60 3.1
     * @return TBool indicating the current status of the MS security
     */
    IMPORT_C static TBool IsMediaServerSecurityEnabledL();

public:
    // Business logic methods for handling the whitelists.

    /**
     * Adds allowed file, so that no authorization is requested when
     * client gets this file.
     * @since S60 3.0
     * @param aFileName reference to allowed file
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt AddAllowedFile( const TDesC& aFileName );

    /**
     * Removes file from white list.
     * @since S60 3.0
     * @param aFileName reference to file to be removed
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt RemoveAllowedFile( const TDesC& aFileName );

    /**
     * Adds allowed IP address, so that no authorization is requested 
     * when client with that IP address makes connection request.
     * @since S60 3.0
     * @param aIpAddress reference to allowed IP address
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt AddAllowedAddress( const TInetAddr& aIpAddress );

    /**
     * Removes allowed IP address.
     * @since S60 3.0
     * @param aIpAddress reference to IP address to be removed
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt RemoveAllowedAddress( const TInetAddr& aIpAddress );

    /**
     * Removes all allowed files from white list.
     * @since S60 3.0
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt ResetFileList();

    /**
     * Removes all allowed IP Addresses from white list.
     * @since S60 3.0
     * @return TInt indicating the status of operation
     */
    IMPORT_C TInt ResetAddressList();

public:
    // Business logic methods.

    /**
     * Checks if the given IP-address/action/file is authorized. Null
     * parameters (filename and/or action name) are allowed.
     *
     * Leaves with standard e32err error code in case of an error, or
     * with KErrAccessDenied if access is not authorized.
     *
     * @since S60 3.1
     * @param aIpAddress (const TInetAddr&) IP address
     * @param aFileName (const TDesC&) filename
     * @param aActionName (const TDesC8&) action name
     */
    IMPORT_C void CheckAuthorizationL( const TInetAddr& aIpAddress,
            const TDesC& aFileName,
            const TDesC8& aActionName );

    /**
     * Query user to authorize connection from an IP-address.
     *
     * Leaves with standard e32err error code in case of an error, or
     * with KErrAccessDenied if access is not authorized. 
     *
     * @since S60 3.1
     * @param aIpAddress (const TInetAddr&) reference to IP Address
     */
    IMPORT_C void QueryAuthorizationL( const TInetAddr& aIpAddress );

private:
    // Business logic methods.

    /**
     * Set securitymanagersetting to given value
     * 
     * @param aSettingToSet value to set
     */
    static void SetSecurityManagerSettingL( const TDesC8& aSettingToSet );    
    
    /**
     * Checks if the requested file is automatically allowed.
     *
     * @since S60 3.1
     * @param aFileName (const TDesC&) the file that request concerns
     * @return TBool access request status
     */
    TBool IsAllowedFile( const TDesC& aFileName );

    /**
     * Checks if the requested action is automatically allowed.
     *
     * @since S60 3.1
     * @param aActionName (const TDesC8&) action name that request concerns
     * @return TBool access request status
     */
    TBool IsAllowedAction( const TDesC8& aActionName );

private:
    // Constructors

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CUpnpSecAccessController();

private:
    // Member variables
    RUpnpSecurityManagerClientSession* iSecurityManagerClientSession;
    };

#endif // UPNPSECACCESSCONTROLLER_H
// End of File
