/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a list of devices with extended information
*
*/






#ifndef C_UPNPDEVICEREPOSITORY_H_
#define C_UPNPDEVICEREPOSITORY_H_

// Include files
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class CUpnpAVDeviceExtended;
class CUpnpDevice;
class CUpnpAVControlPoint;

/**
 * UPnP Device Repository. Stores UPnP devices with additional (protocolinfo)
 * information.
 */
class CUPnPDeviceRepository: public CBase
    {
 
public:

    /**
     * Static 1st phase constructor
     *
     * @param aControlPoint AV Control Point instance
     * @return new instance
     */    
    static CUPnPDeviceRepository* NewL
        (
        CUpnpAVControlPoint& aControlPoint
        );

    /**
     * Destructor
     */    
    virtual ~CUPnPDeviceRepository();
      
private:

    /**
     * Private constructor
     */
    CUPnPDeviceRepository
        (
        CUpnpAVControlPoint& aControlPoint
        );

    /**
     * 2nd phase constructor
     */    
    void ConstructL();    

public: // New functions

    /**
     * Adds a new device to repository
     *
     * @param aDevice UPnP Device
     */    
    void AddDeviceL( CUpnpDevice& aDevice );
    
    /**
     * Adds protoconinfo to a device
     *
     * @param aUuid device uuid
     * @param aSource source protocolinfo
     * @param aSink sink protocolinfo
     * @return UPnP device with protocolinfo
     */    
    CUpnpAVDeviceExtended& AddProtocolInfoL( const TDesC8& aUuid,
        const TDesC8& aSource, const TDesC8& aSink );
        
    /**
     * Removes a device from repository
     *
     * @param aUuid device Uuid
     */    
    void Remove( const TDesC8& aUuid );
    
    /**
     * Finds and return a device
     *
     * @param aUuid device Uuid
     * @return device reference
     */    
    CUpnpAVDeviceExtended& FindDeviceL( const TDesC8& aUuid );
    
    /**
     * Return a list of devices
     *
     * @return list of device
     */    
    const RPointerArray<CUpnpAVDeviceExtended>& DeviceList() const;
    
    /**
     * Subscibe device for events (sends subscribe action)
     *
     * @param aUuid device Uuid
     */    
    void SubscribeDeviceL( const TDesC8& aUuid );     

    /**
     * Unsubscibe device for events (sends unsubscribe action)
     *
     * @param aUuid device Uuid
     */    
    void UnSubscribeDeviceL( const TDesC8& aUuid );
    
    void ConnectionLost();
    
    TBool IsWlanActive(); 
    
private:

    /**
     * Parses devices services (goes through actions and determines what's
     * supported.
     *
     * @param aSource source device
     * @param aTarget target device
     */    
    void ParseDeviceServicesL( CUpnpDevice& aSource,
        CUpnpAVDeviceExtended& aTarget );

   
private:

    CUpnpAVControlPoint&        iControlPoint; // Not own        
   
    RPointerArray<CUpnpAVDeviceExtended> iDevices; // Own
    
    TBool iIsWlanActive;
    };


#endif // C_UPNPDEVICEREPOSITORY_H_