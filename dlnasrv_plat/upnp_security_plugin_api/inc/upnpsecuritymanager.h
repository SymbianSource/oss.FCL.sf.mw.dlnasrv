/** @file
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CUpnpSecurityManager class
*    
*
*/

#ifndef C_CUPNPSECURITYMANAGER_H
#define C_CUPNPSECURITYMANAGER_H

// INCLUDES
#include <e32base.h>

// UID of this interface
const TUid KCUpnpSecurityManagerUid = {0x101F977E}; // the standard plugin
const TUid KCUpnpSecurityManagerUidRemote = {0x2001137B}; // Remote Access branch version

// FORWARD DECLARATIONS
class CUpnpAction;
class CUpnpHttpMessage;


// CLASS DECLARATION
/**
* Class responsbile for security checking in mediaserver-side
*/
class CUpnpSecurityManager : public CBase
{
public:

    /**
    * Two-phased constructor.
    *
    * @param aType name of security manager name used to identification in ECOM mechanism
    * @return a pointer to an object instance that implements    
    */
    IMPORT_C static CUpnpSecurityManager* NewL(const TDesC8& aType);

    /**
     * Creates new instance of CUpnpSecurityManager based on upnp settings,
     * and NULL if there is no, or empty setting.
     */
    IMPORT_C static CUpnpSecurityManager* NewInstanceFromUpnpSettingsL();    
    
    /**
    * Destructor    
    */    
    IMPORT_C virtual ~CUpnpSecurityManager(); 
    
    /** 
    * Authorize HTTP message.
    * It gets notification about all HTTP GET,HEAD requests with real file that was requested.
    * It also gets notification about HTTP POST request with real file where content of POST will be saved
    *
    * @param aMessage HTTP message
    * @param aRequestedFile requested file location
    * @return error code , if different that KErrNone, message with code EHttpUnAuthorized will be sent to remote side
    */
    virtual TInt AuthorizeMessage(CUpnpHttpMessage* aMessage, TFileName& aRequestedFile) = 0;
    
    /** 
    * Authorize SOAP action
    * It gets notification about all SOAP requests
    *
    * @param aAction SOAP action   
    * @return error code , if different that KErrNone, message with code EHttpUnAuthorized will be sent to remote side
    */    
    virtual TInt AuthorizeAction(CUpnpAction* aAction) = 0;    
    
private:        
    // Unique instance identifier key
    TUid            iDtor_ID_Key; 

};
#endif // C_CUPNPSECURITYMANAGER_H
