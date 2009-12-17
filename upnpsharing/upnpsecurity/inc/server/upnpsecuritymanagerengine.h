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
 * Description:  CUpnpSecurityManagerEngine
 *
 */

#ifndef C_CUPNPSECURITYMANAGERENGINE_H
#define C_CUPNPSECURITYMANAGERENGINE_H

// INCLUDES
#include <e32base.h>
#include "upnpsecuritymanagerclientsession.h"

// ENUMS
// Properties keys
enum TUPnPSecurityManagerProperties
    {
    EUPnPSecManFirstRun
    };

// FORWARD DECLARATIONS
class CUpnpSecurityDbConnection;

// CLASS DECLARATION

/**
 * 
 */
class CUpnpSecurityManagerEngine : public CBase
    {
public:

    /**
     * A two-phased constructor function.
     * @result A new instance of CUpnpSecurityManagerEngine
     */
    static CUpnpSecurityManagerEngine* NewL();

    /**
     * Destructor function.
     */
    virtual ~CUpnpSecurityManagerEngine();

public:
    /**
     * Adds new file to allowed file array, if not existing yet.
     * @param aFileName (const TDesC&) file path+name to new allowed file
     */
    void AddNewFileL( const TDesC& aFileName );

    /**
     * Removes file from allowed array.
     * @param aFileName (const TDesC&) File path+name to file to be removed
     */
    void RemoveFileL( const TDesC& aFileName );

    /**
     * Adds new address to allowed address array, if not existing yet.
     * @param aIpAddress (const TInetAddr&) containing IP Address of the
     *        remote party
     */
    void AddNewAddressL( const TInetAddr& aIpAddress );

    /**
     * Removes address from allowed array.
     * @param (const TInetAddr&) aIpAddress containing IP Address to be
     *        removed
     */
    void RemoveAddressL( const TInetAddr& aIpAddress );

    /**
     * Remove all files from allowed array
     */
    void ResetFileListL();

    /**
     * Remove all addresses from allowed array
     */
    void ResetAddressListL();

    /**
     * Check if ipaddress and filename is on allowed list
     */
    TAccessType CheckAuthorization( const TInetAddr& aIpAddress,
        const TDesC& aFileName );

    /**
     * Shows authorisation query to end user, and add address to allowed list
     * if user accepted it
     */
    TAccessType ShowAuthorisationQueryL( const TInetAddr& aIpAdress );

private:
    /**
     * Default constructor.
     */
    CUpnpSecurityManagerEngine();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * 
     */
    void ResetListsOnFirstRunL();

private:

    /**
     * Finds file from allowed array
     * @param (const TDesC&) aFileName File path+name to file to search
     * @return TInt, index to array. KErrNotFound, if file cannot be found.
     */
    TInt FindFileFromArray( const TDesC& aFileName );

    /**
     * Finds address from array
     * @param aIpAddress (const TInetAddr&) containing IP Address to be
     *        searched
     * @return TInt, index to array. KErrNotFound, if file cannot be found.
     */
    TInt FindAddressFromArray( const TInetAddr& aIpAddress );

private:
    // Member variables

    // Array for allowed files. Owned.
    RPointerArray<HBufC> iAllowedFiles;

    // Array for allowed IP addresses. Owned.
    RArray<TInetAddr> iAllowedAddresses;

    // Sqlite connection wrapper.
    CUpnpSecurityDbConnection* iDbConnection;
    };

#endif // C_CUPNPSECURITYMANAGERENGINE_H
// End of File
