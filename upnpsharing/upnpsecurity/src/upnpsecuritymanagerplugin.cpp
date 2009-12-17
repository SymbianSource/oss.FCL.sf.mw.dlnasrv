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
 *  Description : Implementation for UpnpSecurityManager ECom plugin.
 *
 */

// INCLUDES
#include <e32base.h>
#include <upnpaction.h>
#include <upnphttpmessage.h>
#include "upnpsecaccesscontroller.h"
#include "upnpsecuritymanagerplugin.h"

_LIT( KComponentLogfile, "upnpsecurity.txt");
#include "upnplog.h"

// ============================= MEMBER FUNCTIONS ==============================

CUpnpSecurityManagerPlugin* CUpnpSecurityManagerPlugin::NewL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::NewL" );

    CUpnpSecurityManagerPlugin* self= NULL;
    self = new( ELeave ) CUpnpSecurityManagerPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerPlugin::CUpnpSecurityManagerPlugin
// Constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerPlugin::CUpnpSecurityManagerPlugin()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::\
CUpnpSecurityManagerPlugin" );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerPlugin::~CUpnpSecurityManagerPlugin
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerPlugin::~CUpnpSecurityManagerPlugin()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::\
~CUpnpSecurityManagerPlugin" );

    delete iAccessController;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerPlugin::ConstructL()
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::ConstructL" );

    iAccessController = CUpnpSecAccessController::NewL( );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerPlugin::AuthorizeMessage
// Authorize HTTP message.
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityManagerPlugin::AuthorizeMessage(
    CUpnpHttpMessage* aMessage, TFileName& aRequestedFile )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::AuthorizeMessage" );

    TInt returnValue = KErrAccessDenied;
    if ( aMessage )
        {
        // Check if the access to the given file is granted already
        TRAP( returnValue,
                iAccessController->CheckAuthorizationL( aMessage->Sender(),
                        aRequestedFile,
                        KNullDesC8 ) );

        // If the access was not authorized, query authorization from user
        if ( returnValue != KErrNone )
            {
            TRAP( returnValue,
                    iAccessController->QueryAuthorizationL(
                            aMessage->Sender() ) );
            }
        }

    // Convert the returnvalue to either "KErrNone" or "KErrAccessDenied"
    if ( returnValue != KErrNone )
        {
        returnValue = KErrAccessDenied;
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerPlugin::AuthorizeAction
// Authorize SOAP action.
// -----------------------------------------------------------------------------
//
TInt CUpnpSecurityManagerPlugin::AuthorizeAction( CUpnpAction* aAction )
    {
    __LOG( "[UpnpSecurity]\t CUpnpSecurityManagerPlugin::AuthorizeAction" );

    TInt returnValue = KErrAccessDenied;
    if ( aAction )
        {
        // Check if the access to the given file is granted already
        TRAP( returnValue,
                iAccessController->CheckAuthorizationL( aAction->Sender(),
                        KNullDesC,
                        aAction->Name() ) );

        // If the access was not authorized, query authorization from user
        if ( returnValue != KErrNone )
            {
            TRAP( returnValue,
                    iAccessController->QueryAuthorizationL(
                            aAction->Sender() ) );
            }
        }

    // Convert the returnvalue to either "KErrNone" or "KErrAccessDenied"
    if ( returnValue != KErrNone )
        {
        returnValue = KErrAccessDenied;
        }
    return returnValue;
    }

//  End of File
