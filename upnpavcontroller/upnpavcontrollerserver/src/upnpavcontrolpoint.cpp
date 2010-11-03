/** @file
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpAVControlPoint
*
*/


// INCLUDE FILES
#include "upnpavcontrolpoint.h"
#include "upnpavcontrolpointobserver.h"

#include <upnpdevice.h>

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt" );
#include "upnplog.h"

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAVControlPoint* CUpnpAVControlPoint::NewL(
            MUpnpAVControlPointObserver& aAVControlPointObserver )
    {
    CUpnpAVControlPoint* self = 
        new (ELeave) CUpnpAVControlPoint( aAVControlPointObserver );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CUpnpAVControlPoint
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpAVControlPoint::CUpnpAVControlPoint(
     MUpnpAVControlPointObserver& aAVControlPointObserver)
                : CUpnpControlPoint(),
                  iAVControlPointObserver(aAVControlPointObserver)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::ConstructL( )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::~CUpnpAVControlPoint
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpAVControlPoint::~CUpnpAVControlPoint()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::Service
// This function returns a pointer to appropriate service instance.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
CUpnpService* CUpnpAVControlPoint::Service( 
        const CUpnpDevice* aDevice,
        const TDesC8& aServiceType )
    {
    __ASSERT( aDevice, __FILE__, __LINE__ );
    
    // CUpnpDevice::ServiceList() is non-const method (for some reason). 
    CUpnpDevice* dev = const_cast<CUpnpDevice*>(aDevice);
    RPointerArray<CUpnpService>& services = dev->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( aServiceType )!= KErrNotFound )
            {
            return services[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CreateActionLC
// Creates new action object.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
CUpnpAction* CUpnpAVControlPoint::CreateActionLC( 
        const CUpnpDevice* aDevice, 
        const TDesC8& aServiceType,
        const TDesC8& aActionName )
    {
    // Tries to find CUpnpService from given device with given
    // service type.
    CUpnpService* service = Service( aDevice, aServiceType );
    if ( !service ) 
        {
        // Service not found.
        User::Leave( KErrUnknown );
        }
    
    // Creates the action with given action name.
    // Note! If service doesn't include action, it return NULL.
    // CUpnpService::CreateActionLC leaves only if creation of 
    // found action leaves.
    CUpnpAction* action = service->CreateActionLC( aActionName );
    if ( !action ) 
        {
        // Action not found.
        User::Leave( KErrGeneral );
        }
    
    return action;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::StartUpL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::StartUpL()
    {
    //_LIT8( KMediaServer, "urn:schemas-upnp-org:device:MediaServer" );
    _LIT8( KMediaRenderer, "urn:schemas-upnp-org:device:MediaRenderer" );

    CDesC8ArrayFlat* targetDeviceTypes = new(ELeave) CDesC8ArrayFlat(1);
    CleanupStack::PushL( targetDeviceTypes );
    // We only have push use case, no need to ask stack to find media servers: 
    //targetDeviceTypes->AppendL( KMediaServer() );
    targetDeviceTypes->AppendL( KMediaRenderer() );    
    CUpnpControlPoint::ConstructL( *targetDeviceTypes );
    CleanupStack::Pop( targetDeviceTypes );
    targetDeviceTypes->Reset();
    delete targetDeviceTypes; 
    targetDeviceTypes = NULL;

    TPtrC8 devicePtr;
    devicePtr.Set( UpnpSSDP::KUPnPRootDevice );
    SearchL( devicePtr );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::DeviceDiscoveredL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::DeviceDiscoveredL( CUpnpDevice* aDevice )
    {
    iAVControlPointObserver.DeviceDiscoveredL( aDevice );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::DeviceDisappearedL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::DeviceDisappearedL( CUpnpDevice* aDevice )
    {    
    iAVControlPointObserver.DeviceDisappearedL(aDevice);
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::ActionResponseReceivedL
// This function ralizes an interface. Functionality is located in separate 
// handler class.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::ActionResponseReceivedL( CUpnpAction* aAction )
    {
    iAVControlPointObserver.ActionResponseL( aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::StateUpdatedL
// This function implements an inteface and forwards request 
// to stateupdate handler.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::StateUpdatedL( CUpnpService* aService )
    {
    iAVControlPointObserver.StateUpdatedL( aService );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::HttpResponseReceivedL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::HttpResponseReceivedL( 
        CUpnpHttpMessage* aMessage )
    {    
    iAVControlPointObserver.HttpResponseL( aMessage );
    }


// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::NetworkEvent
// -----------------------------------------------------------------------------
//
void CUpnpAVControlPoint::NetworkEvent( CUpnpNetworkEventBase* aEvent )
    {
    CUpnpControlPoint::NetworkEvent( aEvent );                
    TRAP_IGNORE( SearchL( UpnpSSDP::KUPnPRootDevice ) );
    }    

//end of file
