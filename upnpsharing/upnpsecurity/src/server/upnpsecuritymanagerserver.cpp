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
 * Description:  class CUpnpSecurityManagerServer
 *
 */

// INCLUDES
#include "upnpsecuritymanagerserver.h"
#include "upnpsecuritymanagerengine.h"
#include "upnpsecuritymanagersession.h"

// CONSTANTS
const TVersion KServerSupportedVersion(
    KUpnpSecurityManagerMajorVersionNumber,
    KUpnpSecurityManagerMinorVersionNumber,
    KUpnpSecurityManagerBuildVersionNumber );

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::CUpnpSecurityManagerServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerServer::CUpnpSecurityManagerServer()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::~CUpnpSecurityManagerServer
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerServer::~CUpnpSecurityManagerServer()
    {
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSymbianServerBase* CUpnpSecurityManagerServer::NewLC()
    {
    CUpnpSecurityManagerServer* connManager =
            new (ELeave) CUpnpSecurityManagerServer();
    CleanupStack::PushL( connManager );
    connManager->ConstructL();
    return connManager;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerServer::ConstructL()
    {
    BaseConstructL();
    iEngine = CUpnpSecurityManagerEngine::NewL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CUpnpSecurityManagerServer::NewSessionL( const RMessage2& /*aMsg*/) const
    {
    return CUpnpSecurityManagerSession::NewL( *iEngine );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::SupportedVersion
// Gets server supported version.
// -----------------------------------------------------------------------------
//
TVersion CUpnpSecurityManagerServer::SupportedVersion() const
    {
    return KServerSupportedVersion;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerServer::ServerName
// Gets server supported version.
// -----------------------------------------------------------------------------
//
const TDesC& CUpnpSecurityManagerServer::ServerName() const
    {
    return KUpnpSecurityManagerName;
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main entry point.
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CUpnpSecurityManagerServer::StartServer(
        KUpnpSecurityManagerThreadName, CUpnpSecurityManagerServer::NewLC );
    }

// End of File
