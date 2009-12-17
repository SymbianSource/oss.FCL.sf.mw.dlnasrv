/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CUpnpService class
 *
*/


// INCLUDES
#include "upnpsecuritymanager.h"
#include <ecom/ecom.h>
#include "upnpmediaserversettings.h"

// -----------------------------------------------------------------------------
// CUpnpSecurityManager::~CUpnpSecurityManager
//
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSecurityManager::~CUpnpSecurityManager()
{
    // Inform the framework that this specific 
    // instance of the interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
}
    
// -----------------------------------------------------------------------------
// CUpnpSecurityManager::NewL
// Two-phased constructor.
//
// Create plugin using for identification field default_data in resource file
// -----------------------------------------------------------------------------
// 
EXPORT_C CUpnpSecurityManager* CUpnpSecurityManager::NewL(const TDesC8& aType)
{
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aType);
    // Disable wildcard matching
    resolverParams.SetWildcardMatch(EFalse);

		const TUid interfaceuidRemote = {0x2001137B};
		
		RImplInfoPtrArray implArray;
		
		// check if remote access branch security plugin is found
		REComSession::ListImplementationsL(interfaceuidRemote, implArray);
		if (implArray.Count() > 0) 
		    return REINTERPRET_CAST(CUpnpSecurityManager*,
                          REComSession::CreateImplementationL(
                                              KCUpnpSecurityManagerUidRemote,
                                              _FOFF(CUpnpSecurityManager, iDtor_ID_Key),
                                              resolverParams));
 		
 		// if not, use the standard security plugin
		return REINTERPRET_CAST(CUpnpSecurityManager*,
                          REComSession::CreateImplementationL(
                                              KCUpnpSecurityManagerUid,
                                              _FOFF(CUpnpSecurityManager, iDtor_ID_Key),
                                              resolverParams));
                                              
                                              
}

// -----------------------------------------------------------------------------
// CUpnpSecurityManager::NewInstanceFromUpnpSettingsL
// 
// Creates new instance of CUpnpSecurityManager based on upnp settings,
// and NULL if there is no, or empty setting.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSecurityManager* CUpnpSecurityManager::NewInstanceFromUpnpSettingsL()
    {
    CUpnpSecurityManager* newInstance = NULL;
    CUpnpMediaServerSettings* msSettings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( msSettings );
    HBufC8* secman = msSettings->GetL( UpnpMediaServerSettings::ESecurityManager );
    CleanupStack::PushL( secman );
    if ( secman && *secman != KNullDesC8() )
        {
        newInstance = CUpnpSecurityManager::NewL( *secman );
        }
    CleanupStack::PopAndDestroy( secman );
    CleanupStack::PopAndDestroy( msSettings );
    return newInstance;
    }

// End of File

