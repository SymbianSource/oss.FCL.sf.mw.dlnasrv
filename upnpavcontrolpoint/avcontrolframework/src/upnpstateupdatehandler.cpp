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
* Description:  CUpnpStateUpdateHandler
*
*/


// INCLUDE FILES
#include "upnpstateupdatehandler.h"
#include "upnpavcpstring.h"

// CONSTANTS
using namespace UpnpAVCPStrings;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::CUpnpStateUpdateHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpStateUpdateHandler::CUpnpStateUpdateHandler( 
                                         MUpnpAVControlPointObserver& aAVCPObserver,
                                         CUpnpAVControlPoint& aAVCP )
    :iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
	}

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpStateUpdateHandler::ConstructL() 
	{
	}

// -----------------------------------------------------------------------------
// CSuperDir::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpStateUpdateHandler* CUpnpStateUpdateHandler::NewL(
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP )
    {
	CUpnpStateUpdateHandler* self = new (ELeave) CUpnpStateUpdateHandler( 
        aAVCPObserver, aAVCP );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::~CUpnpStateUpdateHandler
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpStateUpdateHandler::~CUpnpStateUpdateHandler()	
	{
	}

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::ContentDirectoryStateUpdated
// In this function are update events from content directory handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpStateUpdateHandler::ContentDirectoryStateUpdated( CUpnpDevice& aDevice, 
                                                   CUpnpService*& aService)
    {
    CUpnpStateVariable* systemUpdateId = 
        aService->StateVariable( KSystemUpdateID );
    CUpnpStateVariable* containerUpdateIds = 
        aService->StateVariable( KContainerUpdateIDs );
    CUpnpStateVariable* transferIds = 
        aService->StateVariable( KTransferIds );       

    if( !(systemUpdateId || containerUpdateIds || transferIds) )
        {
        return KErrArgument;
        }
        
    //
    if( systemUpdateId )
        {
        TLex8 updateidLex( systemUpdateId->Value() );
        TInt systemUpdate;
        updateidLex.Val( systemUpdate );
        iAVControlPointObserver.CdsUpdateEvent(
            aDevice.Uuid(),
            systemUpdate
            );
        }
    if( containerUpdateIds )
        {
        iAVControlPointObserver.CdsContainerEvent( 
            aDevice.Uuid(),
            containerUpdateIds->Value() 
            );
        }
    if( transferIds )
        {
        iAVControlPointObserver.CdsTransferEvent( 
            aDevice.Uuid(),
            transferIds->Value() 
            );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::StateUpdatedL
// This is the place to start handling state updates
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpStateUpdateHandler::StateUpdated(CUpnpService*& aService)
	{
    TInt err( KErrNone );
	CUpnpDevice& device = aService->Device();
    if (aService->ServiceType().Match( KRenderingControl ) != KErrNotFound )
        {
        err =  RenderingControlStateUpdated( device, aService );
        }
    else if (aService->ServiceType().Match( KConnectionManager ) != KErrNotFound )
        {
        err = ConnectionManagerStateUpdated( device, aService );
        }
    else if (aService->ServiceType().Match( KAVTransport ) != KErrNotFound )
        {
        err = AVTransportStateUpdated( device, aService );        
        }
    else if (aService->ServiceType().Match( KContentDirectory ) != KErrNotFound )
        {
        err = ContentDirectoryStateUpdated( device, aService );        
        }
    else
        {
        err = KErrGeneral;
        }
    return err;

    }
// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::RenderingControlStateUpdated
// In this function are rendering control events are handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpStateUpdateHandler::RenderingControlStateUpdated( CUpnpDevice& aDevice,
                                                        CUpnpService*& aService )
    {
    CUpnpStateVariable* lastChange = aService->StateVariable( KLastChange );
    if( !lastChange )
        {
        return KErrArgument;
        }
    else
        {
        iAVControlPointObserver.RcLastChangeEvent(
            aDevice.Uuid(),
            lastChange->Value() 
            );
        return KErrNone;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::KAVTransportStateUpdatedL
// In this function are updates for mediarenderer handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpStateUpdateHandler::AVTransportStateUpdated( CUpnpDevice& aDevice,
                                                   CUpnpService*& aService )
    {
    CUpnpStateVariable* lastChange = aService->StateVariable( KLastChange );
    if( !lastChange )
        {
        return KErrArgument;
        }
    else
        {
        iAVControlPointObserver.AvtLastChangeEvent(
            aDevice.Uuid(),
            lastChange->Value() 
            );
        return KErrNone;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::ConnectionManagerStateUpdated
// In this function are update events from rendering control handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpStateUpdateHandler::ConnectionManagerStateUpdated(CUpnpDevice& aDevice, 
                                                        CUpnpService*& aService)
    {
    CUpnpStateVariable* source = 
        aService->StateVariable( KSourceProtocolInfo );
    CUpnpStateVariable* sink = 
        aService->StateVariable( KSinkProtocolInfo );
    CUpnpStateVariable* connections = 
        aService->StateVariable( KCurrentConnectionIds );       
    
    if( !(source || sink || connections) )
        {
        return KErrArgument;
        }
        
    if( source )
        {
        iAVControlPointObserver.CmSourceEvent(
            aDevice.Uuid(),
            source->Value()
            );
        }
    if( sink )
        {
        iAVControlPointObserver.CmSinkEvent( 
            aDevice.Uuid(),
            sink->Value() 
            );
        }
    if( connections )
        {
        iAVControlPointObserver.CmConnectionsEvent( 
            aDevice.Uuid(),
            connections->Value() 
            );
        }

    return KErrNone;
    }

