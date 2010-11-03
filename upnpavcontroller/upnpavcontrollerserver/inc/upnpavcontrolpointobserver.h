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
* Description:  MUpnpAVControlPointObserver
*
*/



#ifndef MUPNPAVCONTROLPOINTOBSERVER_H
#define MUPNPAVCONTROLPOINTOBSERVER_H

//  INCLUDES
#include "upnpdevice.h"

// FORWARD DECLARATIONS
class CUpnpAction;
class CUpnpHttpMessage;
class CUpnpDevice;

// CLASS DECLARATION

/**
*  Interface class.
*  This class defines a observer interface for AV controlpoint
*/
class MUpnpAVControlPointObserver
        {
    public: // New functions

        /**
        * Handles an action event that occurred in remote device
        * @param aAction object that describes the action
        */
        virtual void ActionResponseL(CUpnpAction* aAction ) = 0;

        /**
        * A service update has been received from network
        * @param aService the service with its new state
        */
        virtual void StateUpdatedL(CUpnpService* aService) = 0;

        /**
        * Handles HTTP messages.
        * @param aMessage Incoming HTTP message.
        */
        virtual void HttpResponseL(CUpnpHttpMessage* aMessage) = 0;

        /**
        * Handles UPnP device discoveries.
        * @param aDevice Device that is discovered.
        */
        virtual void DeviceDiscoveredL(CUpnpDevice* aDevice) = 0;

        /**
        * Handles UPnP device disappears.
        * @param aDevice Device that disappeared.
        */
        virtual void DeviceDisappearedL(CUpnpDevice* aDevice) = 0;

    };

#endif // MUPNPAVCONTROLPOINTOBSERVER_H
            
// End of File
