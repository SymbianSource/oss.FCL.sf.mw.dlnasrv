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






#ifndef C_UPNPDEVICEDISCOVERYMESSAGE_H
#define C_UPNPDEVICEDISCOVERYMESSAGE_H

// INDLUDE FILES
#include <e32base.h>
#include "upnpavcontrollerglobals.h"

// FORWARD DECLARATIONS
class CUpnpAVDevice;

/**
 * Used to save CUpnpAVDevice object and device discovery message type.
 *
 * @since Series 60 3.2.3
 */
class CUpnpDeviceDiscoveryMessage : public CBase
{
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor. Instance is left in cleanup stack.
     *
     * @since Series 60 3.2.3
     * @param aDevice pointer to device object.
     * @param aType device discovery message type.
     * @return a CUpnpDeviceDiscoveryMessage instance
     */
    static CUpnpDeviceDiscoveryMessage* NewLC( 
            const CUpnpAVDevice* aDevice,
            TAVControllerDeviceDiscovery aType );
        
    /**
     * Two-phased constructor.
     *
     * @since Series 60 3.2.3
     * @param aDevice pointer to device object.
     * @param aType device discovery message type.
     * @return a CUpnpDeviceDiscoveryMessage instance
     */
    static CUpnpDeviceDiscoveryMessage* NewL( 
            const CUpnpAVDevice* aDevice,
            TAVControllerDeviceDiscovery aType );
        
    /**
     * Destructor
     */
    virtual ~CUpnpDeviceDiscoveryMessage();
    
public:
    
    /**
     * Return link offset (used in linked list)
     * 
     * @since Series 60 3.2.3
     * @return link offset
     */
    static TInt LinkOffset();
    
    /**
     * Return link offset (used in linked list)
     * 
     * @since Series 60 3.2.3
     * @return pointer to device object
     */
    CUpnpAVDevice* Device();
    
    /**
     * Return device discovery message type
     * 
     * @since Series 60 3.2.3
     * @return device discovery message type
     */
    TAVControllerDeviceDiscovery& MsgType();
    
protected: //

    /**
     * Constructor
     */
    CUpnpDeviceDiscoveryMessage();
    
private:
    
    /** @var iSlink  pointer to next CUpnpDeviceDiscoveryMessage object*/
    TSglQueLink iSlink;
    
    /** @var iUpnpAVDevice  pointer to device object*/
    CUpnpAVDevice* iUpnpAVDevice; //Own
    
    /** @var iMsgType device discovery message type */
    TAVControllerDeviceDiscovery iMsgType;
};

#endif /*C_UPNPDEVICEDISCOVERYMESSAGE_H*/
