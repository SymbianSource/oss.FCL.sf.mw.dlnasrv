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
* Description:  Produces cm actions
*
*/


// INCLUDE FILES
#include "upnpcmactionfactory.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpstring.h"

// CONSTANTS
using namespace UpnpAVCPStrings;
const TInt KMaxIntLength = 10;

// ============================= LOCAL FUNCTIONS ===============================



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CUpnpCmActionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpCmActionFactory::CUpnpCmActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
                                   CUpnpAVControlPoint& aAVCP )
:iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCmActionFactory::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCmActionFactory* CUpnpCmActionFactory::NewL( 
                                      MUpnpAVControlPointObserver& aAVCPObserver,
                                      CUpnpAVControlPoint& aAVCP )
    {
    CUpnpCmActionFactory* self = new (ELeave) CUpnpCmActionFactory( 
        aAVCPObserver,
        aAVCP );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::~CUpnpCmActionFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpCmActionFactory::~CUpnpCmActionFactory()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CmProtocolInfoActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCmActionFactory::CmProtocolInfoActionLC(CUpnpAction*& aAction,
                                              CUpnpDevice* aDevice)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KConnectionManager);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetProtocolInfo );

    if( aAction )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }             
// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CmCurrentConnectionsActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCmActionFactory::CmCurrentConnectionsActionLC(CUpnpAction*& aAction,
                                                    CUpnpDevice* aDevice)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KConnectionManager);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetCurrentConnectionIDs );

    if( aAction )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
 // -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CmPrepareConnectionActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCmActionFactory::CmPrepareConnectionActionLC(
                                           CUpnpAction*& aAction,
                                           CUpnpDevice* aDevice,
                                           const TDesC8& aRemoteProtocolInfo,
                                           const TDesC8& aPeerConnectionManager,
										   TInt aPeerConnectionID,
                                           const TDesC8& aDirection
                                           )
    {
    CUpnpService* service = iAVCP.Service(aDevice, KConnectionManager);
    if( !service )
        {
        return KErrUnknown;
        }
   	TBuf8<KMaxIntLength> conId;
	conId.Num( aPeerConnectionID );
    aAction = service->CreateActionLC( KPrepareForConnection );

    if( aAction )
        {
        aAction->SetArgumentL( KRemoteProtocolInfo(), aRemoteProtocolInfo );
	    aAction->SetArgumentL( KPeerConnectionManager(), aPeerConnectionManager );
	    aAction->SetArgumentL( KPeerConnectionID(), conId );
	    aAction->SetArgumentL( KDirection(), aDirection );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CmConnectionCompleteActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCmActionFactory::CmConnectionCompleteActionLC(CUpnpAction*& aAction, 
                                                    CUpnpDevice* aDevice,
                                                    TInt aConnectionId)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KConnectionManager);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KConnectionComplete );

    if( aAction )
        {
   	    TBuf8<KMaxIntLength> connectionId;
	    connectionId.Num(aConnectionId);
        aAction->SetArgumentL( KConnectionID, connectionId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCmActionFactory::CmCurrentConnectionInfoActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCmActionFactory::CmCurrentConnectionInfoActionLC(CUpnpAction*& aAction, 
                                                    CUpnpDevice* aDevice,
                                                    TInt aConnectionId)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KConnectionManager);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetCurrentConnectionInfo );
    
    if( aAction )
        {
   	    TBuf8<KMaxIntLength> connectionId;
	    connectionId.Num(aConnectionId);
        aAction->SetArgumentL( KConnectionID, connectionId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
