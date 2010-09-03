/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      CUpnpContentServer class implamentation
 *
*/





#include <e32debug.h>
#include <w32std.h>

#include "upnpcontentserver.h"
#include "upnpcontentserversession.h"
#include "upnpcontentserverdefs.h"
#include "upnpcontentserverhandler.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KUpnpContentServerString, "Upnp content server");

using namespace UpnpContentServer;

// --------------------------------------------------------------------------
// Server's policy
// --------------------------------------------------------------------------

//Total number of ranges
static const TUint KContentServerRangeCount = 1;

//Definition of the ranges of IPC numbers
static const TInt contentServerRanges[KContentServerRangeCount] =
        {
        0
        };

//Policy to implement for each of the above ranges
static const TUint8 contentServerElementsIndex[KContentServerRangeCount] =
        {
        0, //applies to 0th range
        };

//Specific capability checks
static const CPolicyServer::TPolicyElement contentServerElements[] =
        {
        {_INIT_SECURITY_POLICY_C3(ECapabilityReadUserData,
                                  ECapabilityWriteUserData,
                                  ECapabilityNetworkServices),
                                  CPolicyServer::EFailClient}
                                  //policy "0",
                                  //fail call if all capabilities not present
        };

//Package all the above together into a policy
static const CPolicyServer::TPolicy KUpnpContentServerPolicy =
        {
        CPolicyServer::EAlwaysPass, //all connect attempts should pass
        KContentServerRangeCount,
        contentServerRanges,
        contentServerElementsIndex,
        contentServerElements
        };

// --------------------------------------------------------------------------
// E32Main
// main function called by E32
// --------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TRAPD( ret, CUpnpContentServer::LaunchServerL() );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::LaunchServer
// Initializes the process and creates an instance of CUpnpContentServer.
// --------------------------------------------------------------------------
//
TInt CUpnpContentServer::LaunchServerL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // Check server not already started
    TFindServer findHostServer( KUpnpContentServer );

    TInt err( KErrNone );
    TFullName name;
    if ( findHostServer.Next(name) == KErrNone )
        { // found server already
        __LOG1( "Error: %d", __LINE__ );
        err = KErrGeneral;
        }
    if ( !err )
        {
        User::RenameThread( KUpnpContentServer );

        // Create cleanup stack.
        CTrapCleanup* cleanup = CTrapCleanup::New();

        // Construct and install active scheduler.
        CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
        CActiveScheduler::Install( scheduler );

        // Construct server.
        CUpnpContentServer* server = NULL;
        TRAPD( err, server = CUpnpContentServer::NewL() );
        if ( err )
            {
            __LOG1( "Error: %d", err );
            }
        RProcess::Rendezvous( err );

        __LOG("CUpnpContentServer::LaunchServer, Start CActiveScheduler");
        // Start handling requests.
        CActiveScheduler::Start();

        delete server;
        __LOG("CUpnpContentServer::LaunchServer, Server deleted.");
        delete scheduler;
        delete cleanup;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::NewL
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServer* CUpnpContentServer::NewL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServer* self = new (ELeave) CUpnpContentServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    self->StartL( KUpnpContentServer );
    CleanupStack::Pop(); // self
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::CUpnpContentServer()
// C++ constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServer::CUpnpContentServer()
    : CPolicyServer( EPriorityStandard, KUpnpContentServerPolicy,
                     ESharableSessions )
    {
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CUpnpContentServer::ConstructL()
    {
    iHandler = CUpnpContentServerHandler::NewL( this );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::~CUpnpContentServer()
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpContentServer::~CUpnpContentServer()
    {
    delete iHandler;
    delete iContainerIndex;
    delete iConMon;
    delete iIdle;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::Handler
// Returns pointer to CUpnpContentServerHandler
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler* CUpnpContentServer::Handler() const
    {
    return iHandler;
    }


// --------------------------------------------------------------------------
// CUpnpContentServer::NewContainerL
// Deletes  objcet container
// --------------------------------------------------------------------------
//
void CUpnpContentServer::RemoveSession( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iSessionCount--;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::NewContainerL
// Deletes  objcet container
// --------------------------------------------------------------------------
//
void CUpnpContentServer::AddSession()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iSessionCount++;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::CanStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentServer::CanStop() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( EFalse );
    if ( iSessionCount < 1 && !iConMon )
        {
        ret = ETrue;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::Stop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServer::Stop()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
  /*  if ( iIdle )
        {
        iIdle->Cancel();
        }
    else
        {
        TRAPD( err, iIdle = CUPnPPeriodic::NewL( CActive::EPriorityIdle ) );
        __ASSERT( err == KErrNone, __FILE__, __LINE__  );
        }

    iIdle->Start( KShutdownTimeout, 
                  KShutdownTimeout, 
                  TCallBack( Shutdown, this ) );*/
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::Shutdown
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentServer::Shutdown( TAny* aPtr )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServer* server = static_cast<CUpnpContentServer*>(aPtr);
    delete server->iIdle;
    server->iIdle = NULL;
    CActiveScheduler::Stop();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::RequestConnectionLostL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentServer::RequestConnectionLostL(
    const TInt aIapId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iHandler )
        {
        iHandler->ValidateDefaultContainersL();
        }
    TInt err( KErrNone );
    if ( !iConMon )
        {
        iConMon = CUPnPConnectionMonitor::NewL( *this, aIapId );
        iActiveIapId = aIapId;
        }
    else if ( iActiveIapId != aIapId )
        {
        err = KErrInUse;
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::CancelConnectionLostL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServer::CancelConnectionLostL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // Stop connection monitoring
    delete iConMon;
    iConMon = NULL;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::ConnectionLost
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServer::ConnectionLost()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TRAP_IGNORE( iHandler->ConnectionLostL() );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::RunError
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentServer::RunError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( aError )
        {
        __LOG1( "Error: %d", aError );
        }
    if ( aError == KErrBadDescriptor )
        {
        Message().Panic( KUpnpContentServerString, aError );
        }
    else
        {
        Message().Complete( aError );
        }
    ReStart();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentServer::NewSessionL
// from CServer2, creates a new session.
// --------------------------------------------------------------------------
//
CSession2* CUpnpContentServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( iIdle )
        {
        iIdle->Cancel();
        }
    
    TVersion v( KUpnpContentServerVersionMajor,
                KUpnpContentServerVersionMinor,
                0 );
    if( !User::QueryVersionSupported(v,aVersion) )
        {
        User::Leave(KErrNotSupported);
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return CUpnpContentServerSession::NewL( (CUpnpContentServer*)this );
    }

// End of file
