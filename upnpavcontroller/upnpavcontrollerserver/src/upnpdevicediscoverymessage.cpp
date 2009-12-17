/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Device message class
*
*/






// include files
#include <e32base.h>

// avcontroller internal
#include "upnpdevicediscoverymessage.h"
#include "upnpavdevice.h"

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::CUpnpDeviceDiscoveryMessage
// constructor
// --------------------------------------------------------------------------
CUpnpDeviceDiscoveryMessage::CUpnpDeviceDiscoveryMessage()
    {
    // none implement
    }

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpDeviceDiscoveryMessage* CUpnpDeviceDiscoveryMessage::NewL(
        const CUpnpAVDevice* aDevice, TAVControllerDeviceDiscovery aType )
    {
    CUpnpDeviceDiscoveryMessage* self = NewLC( aDevice, aType );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpDeviceDiscoveryMessage* CUpnpDeviceDiscoveryMessage::NewLC( 
        const CUpnpAVDevice* aDevice, TAVControllerDeviceDiscovery aType )
    {
    CUpnpDeviceDiscoveryMessage* self = new( ELeave )
            CUpnpDeviceDiscoveryMessage;    
    CleanupStack::PushL( self );
    self->iUpnpAVDevice = ( CUpnpAVDevice* )aDevice;
    self->iMsgType = aType;
    return self;
    }
    
// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::~CUpnpDeviceDiscoveryMessage
// Destructor
// --------------------------------------------------------------------------
CUpnpDeviceDiscoveryMessage::~CUpnpDeviceDiscoveryMessage()
    {
    delete iUpnpAVDevice;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::LinkOffset
// See upnpdevicediscoverymessage.h
// --------------------------------------------------------------------------
TInt CUpnpDeviceDiscoveryMessage::LinkOffset()
    {
    return _FOFF( CUpnpDeviceDiscoveryMessage, iSlink );
    }

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::Device
// See upnpdevicediscoverymessage.h
// --------------------------------------------------------------------------
CUpnpAVDevice* CUpnpDeviceDiscoveryMessage::Device()
    {
    return iUpnpAVDevice;
    }

// --------------------------------------------------------------------------
// CUpnpDeviceDiscoveryMessage::MsgType
// See upnpdevicediscoverymessage.h
// --------------------------------------------------------------------------
TAVControllerDeviceDiscovery& CUpnpDeviceDiscoveryMessage::MsgType()
    {
    return iMsgType;
    }
