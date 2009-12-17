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
* Description:  Produces avt actions
*
*/


// INCLUDE FILES
#include "upnpavtactionfactory.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpstring.h"

// CONSTANTS
using namespace UpnpAVCPStrings;
const TInt KMaxIntLength = 10;

// ============================= LOCAL FUNCTIONS ===============================



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::CUpnpAvtActionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpAvtActionFactory::CUpnpAvtActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
                                     CUpnpAVControlPoint& aAVCP )
:iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAvtActionFactory::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAvtActionFactory* CUpnpAvtActionFactory::NewL( 
                                      MUpnpAVControlPointObserver& aAVCPObserver,
                                      CUpnpAVControlPoint& aAVCP )
    {
    CUpnpAvtActionFactory* self = new (ELeave) CUpnpAvtActionFactory( 
        aAVCPObserver,
        aAVCP );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::~CUpnpAvtActionFactory
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpAvtActionFactory::~CUpnpAvtActionFactory()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtSetTransportUriActionLC
// Creates an action for setting URI for transport.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtSetTransportUriActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId,
                                            const TDesC8& aCurrentUri,
                                            const TDesC8& aCurrentMetaData)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KSetAVTransportURI );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KCurrentURI, aCurrentUri );
        aAction->SetArgumentL( KCurrentURIMetaData, aCurrentMetaData );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtSetNextTransportUriActionLC
// Creates an action for setting URI for next transport.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtSetNextTransportUriActionLC(
                                                   CUpnpAction*& aAction,
                                                    CUpnpDevice* aMediaRenderer,
                                                    TInt aInstanceId,
                                                    const TDesC8& aNextUri,
                                                    const TDesC8& aNextMetaData)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KSetNextAVTransportURI );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KNextURI, aNextUri );
        aAction->SetArgumentL( KNextURIMetaData, aNextMetaData );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtSeekActionLC
// Creates an action for seeking through the resource.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtSeekActionLC(CUpnpAction*& aAction,
                                        CUpnpDevice* aMediaRenderer,
                                        TInt aInstanceId,
                                        const TDesC8& aUnit,
                                        const TDesC8& aTarget)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KSeek );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KUnit, aUnit );
        aAction->SetArgumentL( KTarget, aTarget );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtMediaInfoActionLC
// Creates an action for gettin media info.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtMediaInfoActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KGetMediaInfo );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtTransportInfoActionLC
// Creates an action for gettting transport info.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtTransportInfoActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );
    
    aAction = service->CreateActionLC( KGetTransportInfo );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtPositionInfoActionLC
// Creates an action for getting position info.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtPositionInfoActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KGetPositionInfo );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtDeviceCapabilitiesActionLC
// Creates an action for getting device capabilities.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtDeviceCapabilitiesActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KGetDeviceCapabilities );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtTransportSettingsActionLC
// Creates an action for getting transport settings.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtTransportSettingsActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KGetTransportSettings );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtStopActionLC
// Creates an action for stopping transport.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtStopActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KStop );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtPlayActionLC
// Creates an action for playing.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtPlayActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId,
                                            const TDesC8& aSpeed)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KPlay );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KSpeed, aSpeed );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtPauseActionLC
// Creates an action for pause.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtPauseActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KPause );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtRecordActionLC
// Creates an action for recording.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtRecordActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KRecord );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtNextActionLC
// Creates an action for playing next item.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtNextActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KNext );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtPreviousActionLC
// Creates an action for palying previous item.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtPreviousActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KPrevious );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtSetPlayModeActionLC
// Creates an action for setting playmode.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtSetPlayModeActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId,
                                            const TDesC8& aPlayMode)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KSetPlayMode );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KNewPlayMode, aPlayMode );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtSetRecordModeActionLC
// Creates an action for setting record quality mode.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtSetRecordModeActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId,
                                            const TDesC8& aRecordMode)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KSetRecordQualityMode );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        aAction->SetArgumentL( KNewRecordQualityMode, aRecordMode );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpAvtActionFactory::AvtTransportsActionLC
// Creates an action for getting current transports info.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpAvtActionFactory::AvtTransportsActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaRenderer,
                                            TInt aInstanceId)
    {
    CUpnpService* service = iAVCP.Service(aMediaRenderer, KAVTransport);
    if( !service )
        {
        return KErrUnknown;
        }
    TBuf8<KMaxIntLength> instanceId;
	instanceId.Num( aInstanceId );

    aAction = service->CreateActionLC( KGetCurrentTransportActions );
    if( aAction )
        {
        aAction->SetArgumentL( KInstanceID, instanceId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
//end of file
