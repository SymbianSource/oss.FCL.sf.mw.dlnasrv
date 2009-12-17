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
* Description:  Base class to connect with server AVCPEngine
*
*/


// INCLUDE FILES
#include <e32math.h>
#include "upnpavcpengineclient.h"

#include <Uri16.h>

// Number of message slots to reserve for this client server session.
const TUint KDefaultMessageSlots = 10;
const TUid KServerUid3 = {0x101F977A};

// Function prototypes
static TInt StartServer( void );
static TInt CreateServerProcess( void );

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineClient::RUpnpAVCPEngineClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RUpnpAVCPEngineClient::RUpnpAVCPEngineClient()
    :RSessionBase()
    {
    }

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineClient::Connect
// Connect to ... session.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineClient::Connect()
    {
    TInt error = ::StartServer();

    if ( KErrNone == error )
        {
        error = CreateSession( KAVCPEngineName,
                               Version(),
                               KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineClient::Version
// Version information.
// -----------------------------------------------------------------------------
//
TVersion RUpnpAVCPEngineClient::Version() const
    {
	return( TVersion( KAVCPEngineMajorVersionNumber,
                      KAVCPEngineMinorVersionNumber,
                      KAVCPEngineBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// StartServer
// Static method to start the server.
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findMessageHandler( KAVCPEngineName );
    TFullName name;

	result = findMessageHandler.Next( name );
	if ( result == KErrNone )
        {
		// Server already running
		return KErrNone;
	    }
    result = CreateServerProcess();
    if ( result != KErrNone )
        {
        return  result;
        }
    return  KErrNone;
    }

// -----------------------------------------------------------------------------
// CreateServerProcess
// Static method to create the server process.
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    TInt result;

	const TUidType serverUid( KNullUid, KNullUid, KServerUid3 );
	RProcess server;

	result = server.Create( KAVCPEngineFilename, KNullDesC, serverUid );
    if ( result != KErrNone )
        {
        return  result;
        }
        
    TRequestStatus stat = KRequestPending;
    server.Rendezvous(stat);
    if ( stat != KRequestPending )
        {
        server.Kill( 0 );       // abort startup
        }
    else
        {
        server.Resume();    // logon OK - start the server
        }
        
    User::WaitForRequest(stat);     // wait for start or death
    result = (server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();    
    server.Close(); 
        
    return result;
    }

//  End of File
