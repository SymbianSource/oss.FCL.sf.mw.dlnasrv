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
* Description:  Produces rc actions
*
*/


// INCLUDE FILES
#include "upnprcactionfactory.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpstring.h"

// CONSTANTS
using namespace UpnpAVCPStrings;
const TInt KMaxIntLength = 10;

// ============================= LOCAL FUNCTIONS ===============================



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpRcActionFactory::CUpnpRcActionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpRcActionFactory::CUpnpRcActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
                                   CUpnpAVControlPoint& aAVCP )
:iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpRcActionFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpRcActionFactory::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// CSuperDir::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpRcActionFactory* CUpnpRcActionFactory::NewL(
                                      MUpnpAVControlPointObserver& aAVCPObserver,
                                      CUpnpAVControlPoint& aAVCP )
    {
    CUpnpRcActionFactory* self = new (ELeave) CUpnpRcActionFactory( 
        aAVCPObserver,
        aAVCP );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpRcActionFactory::~CUpnpRcActionFactory
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpRcActionFactory::~CUpnpRcActionFactory()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpRcActionFactory::RcGetVolumeActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpRcActionFactory::RcGetVolumeActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aDevice,
                                            TInt aInstanceID,
                                            const TDesC8& aChannel)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KRenderingControl);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceID );

    aAction = service->CreateActionLC( KGetVolume );
    
    if ( aAction )
        {
        aAction->SetArgumentL( KInstanceID(), instanceId );
	    aAction->SetArgumentL( KChannel(), aChannel );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpRcActionFactory::RcSetVolumeActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpRcActionFactory::RcSetVolumeActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aDevice,
                                            TInt aInstanceID,
                                            const TDesC8& aChannel,
                                            TInt aVolume)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KRenderingControl);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceID );
	TBuf8<KMaxIntLength> volume;
	volume.Num( aVolume );

    aAction = service->CreateActionLC( KSetVolume );
    
    if ( aAction )
        {
        aAction->SetArgumentL( KInstanceID(), instanceId );
        aAction->SetArgumentL( KChannel(), aChannel );
        aAction->SetArgumentL( KDesiredVolume(), volume );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::RcGetMuteActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpRcActionFactory::RcGetMuteActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aDevice,
                                            TInt aInstanceID,
                                            const TDesC8& aChannel)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KRenderingControl);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceID );

    aAction = service->CreateActionLC( KGetMute );

    if ( aAction )
        {
        aAction->SetArgumentL( KInstanceID(), instanceId );
        aAction->SetArgumentL( KChannel(), aChannel );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::RcSetMuteActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpRcActionFactory::RcSetMuteActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aDevice,
                                            TInt aInstanceID,
                                            const TDesC8& aChannel,
                                            const TDesC8& aMute)
    {
    CUpnpService* service = iAVCP.Service(aDevice, KRenderingControl);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceID );

    aAction = service->CreateActionLC( KSetMute );
	
    if ( aAction )
	    {
        aAction->SetArgumentL( KInstanceID(), instanceId );
        aAction->SetArgumentL( KChannel(), aChannel );
        aAction->SetArgumentL( KDesiredMute(), aMute );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
