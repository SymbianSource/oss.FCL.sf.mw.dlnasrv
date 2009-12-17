/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      main callback interface for AVController client
*
*/






#ifndef M_UPNPAVDEVICEOBSERVER_H
#define M_UPNPAVDEVICEOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS
class CUpnpAVDevice;

/**
* Defines the response interface for the device control operations of the
* UPnP AV Controller.
*
* @since Series 60 3.1
*/
class MUPnPAVDeviceObserver
    {

public:

    /**
     * Notifies that a new UPnP device was discovered.
     *
     * UPnPAVController releases the memory allocated for aDevice
     * after calling the callback method.
     *
     * @since Series 60 3.1
     * @param aDevice CUpnpDevice object
     * @return None
     */
    virtual void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice ) = 0;

    /**
     * Notifies that a UPnP device was disappeared.
     *
     * UPnPAVController releases the memory allocated for aDevice
     * after calling the callback method.
     *
     * @since Series 60 3.1
     * @param aDevice CUpnpDevice object
     * @return None
     */
    virtual void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice ) = 0;

    /**
     * Notifies that the WLAN connection has been lost. All sessions
     * are now usable and must be closed.
     *
     * @since Series 60 3.1
     * @return None
     */        
    virtual void WLANConnectionLost() = 0;       

    };

#endif // M_UPNPAVDEVICEOBSERVER_H

// End of File
