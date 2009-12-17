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
* Description:      device data class
*
*/






#ifndef C_CUPNPAVDEVICE_H
#define C_CUPNPAVDEVICE_H


#include <e32base.h>
//#include <s32strm.h>
#include <s32mem.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  UPnP AV Device class
 *  
 *  UPnP AV Device class, represents a UPnP Media Server or Renderer.
 *
 *  @lib - 
 *  @since Series 60 3.1
 */
class CUpnpAVDevice : public CBase
    {
public:

    /**
     * Enumeration to define the type of a device
     */
    enum TUpnpAVDeviceType
        {
        EMediaServer,
        EMediaRenderer
        };    
    
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor. Instance is left in cleanup stack.
     *
     * @return a device instance
     */
    IMPORT_C static CUpnpAVDevice* NewLC();
    
    /**
     * Two-phased constructor.
     *
     * @return a device instance
     */
    IMPORT_C static CUpnpAVDevice* NewL();
    
    /**
     * Two-phased constructor.
     *
     * @param aDevice a device to copy information from
     * @return a device instance
     */
    IMPORT_C static CUpnpAVDevice* NewL( const CUpnpAVDevice& aDevice );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpAVDevice();
    
public:

    /**
     * Externalizes device information to stream.
     * Leaves in case of errors.
     * @since Series 60 3.0
     * @param reference to RWriteStream
     * @return none
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes device information from stream.
     * Leaves in case of errors.
     * @since Series 60 3.0
     * @param reference to RReadStream
     * @return none
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
     * Externalizes device information to a heap descriptor
     *
     * @return externalized device
     */
    IMPORT_C HBufC8* ToDes8L() const;

    /**
     * Overwrites all existing data in this device and replaces it
     * with data copied from given device.
     * @since Series 60 3.1
     * @param aDevice object to copy attributes from
     */
    IMPORT_C void CopyFromL( const CUpnpAVDevice& aDevice );

protected: //

    /**
     * Constructor
     */
    IMPORT_C CUpnpAVDevice();

    /**
     * Perform the second phase construction
     */
    void ConstructL();

public: // New methods

    /**
     * Sets the friendly name of the device
     * 
     * @param aName name
     */
    IMPORT_C void SetFriendlyNameL( const TDesC8& aName );
    
    /**
     * Return friendly name
     * 
     * @return friendly name
     */
    IMPORT_C const TDesC8& FriendlyName() const;
    
    /**
     * Sets the Uuid of the device
     * 
     * @param aUuid Uuid
     */
    IMPORT_C void SetUuidL( const TDesC8& aUuid );
    
    /**
     * Return Uuid
     * 
     * @return Uuid
     */
    IMPORT_C const TDesC8& Uuid() const;
    
    /**
     * Sets the device type (Media Server/Renderer)
     * 
     * @param aDeviceType device type
     */
    IMPORT_C void SetDeviceType( TUpnpAVDeviceType aDeviceType );
    
    /**
     * Return device type
     * 
     * @return device type
     */
    IMPORT_C TUpnpAVDeviceType DeviceType() const;
            
    /**
     * Return link offset (used in linked list)
     * 
     * @return link offset
     */
    IMPORT_C static TInt LinkOffset();         

public: // UPnP media server device capability getters and setters 

    /**
     * Sets the copy capability of the device
     * 
     * @param TBool the copy capability
     * @return None
     */
    IMPORT_C void SetCopyCapability( TBool aCopyCap );

    /**
     * Gets the copy capability of the device
     * 
     * @param None
     * @return TBool the copy capability
     */
    IMPORT_C TBool CopyCapability() const;

    /**
     * Sets the search capability of the device
     * 
     * @param TBool the search capability
     * @return None
     */
    IMPORT_C void SetSearchCapability( TBool aSearchCap );

    /**
     * Gets the search capability of the device
     * 
     * @param None
     * @return TBool the search capability
     */
    IMPORT_C TBool SearchCapability() const;

public: // UPnP rendering device capability getters and setters 

    /**
     * Sets the pause capability of the device
     * 
     * @param TBool the pause capability
     * @return None
     */
    IMPORT_C void SetPauseCapability( TBool aPauseCap );

    /**
     * Gets the pause capability of the device
     * 
     * @param None
     * @return TBool the pause capability
     */
    IMPORT_C TBool PauseCapability() const;

    /**
     * Sets the volume capability of the device
     * 
     * @param TBool the volume capability
     * @return None
     */
    IMPORT_C void SetVolumeCapability( TBool aVolumeCap );

    /**
     * Gets the volume capability of the device
     * 
     * @param None
     * @return TBool the volume capability
     */
    IMPORT_C TBool VolumeCapability() const;

    /**
     * Sets the mute capability of the device
     * 
     * @param TBool the mute capability
     * @return None
     */
    IMPORT_C void SetMuteCapability( TBool aMuteCap );

    /**
     * Gets the mute capability of the device
     * 
     * @param None
     * @return TBool the mute capability
     */
    IMPORT_C TBool MuteCapability() const;

    /**
     * Sets the maximum volume of the device
     * 
     * @param TInt the maximum volume
     * @return None
     */
    IMPORT_C void SetMaxVolume( TInt aVolume );

    /**
     * Gets the maximum volume of the device
     * 
     * @return TInt the maximum volume
     */
    IMPORT_C TInt MaxVolume() const;
    
    /**
     * Sets the SetNextAVTransportUri capability of the device
     * 
     * @param aCap (TBool)
     * @return None
     */
    IMPORT_C void SetNextAVTransportUri( TBool aCap );

    /**
     * Returns the SetNextAVTransportUri capability of the device
     * 
     * @param None
     * @return TBool ETrue if SetNextAVTrasportUri action is supported
     */
    IMPORT_C TBool NextAVTransportUri() const;           

public: // UPnP media rendering capability getters and setters 

    IMPORT_C void SetCapabilitiesBySupportedMimeTypesL(
        const TDesC8& aListOfMimeTypes );

    /**
     * Sets audio capability of the device
     * 
     * @param aCap (TBool)
     * @return None
     */
    IMPORT_C void SetAudioCapability( TBool aCap );

    /**
     * Gets the audio media capability of the device
     * 
     * @param None
     * @return TBool the audio media capability
     */
    IMPORT_C TBool AudioCapability() const;

    /**
     * Sets image capability of the device
     * 
     * @param aCap (TBool)
     * @return None
     */
    IMPORT_C void SetImageCapability( TBool aCap );

    /**
     * Gets the image media capability of the device
     * 
     * @param None
     * @return TBool the image media capability
     */
    IMPORT_C TBool ImageCapability() const;

    /**
     * Sets video capability of the device
     * 
     * @param aCap (TBool)
     * @return None
     */
    IMPORT_C void SetVideoCapability( TBool aCap );

    /**
     * Gets the video media capability of the device
     * 
     * @param None
     * @return TBool the video media capability
     */
    IMPORT_C TBool VideoCapability() const;

public: // DLNA releated

    /**
     * Sets if the device is Dlna compatible
     * 
     * @param aDlnaCompatible
     */    
    IMPORT_C void SetDlnaCompatible( TBool aDlnaCompatible );
    
    /**
     * Return if the device is Dlna compatible
     * 
     * @return ETrue if the device is dlna compatible
     */    
    IMPORT_C TBool DlnaCompatible() const;

protected:
    
    TSglQueLink iSlink;
    
    HBufC8*     iUuid; // Owned
    
    HBufC8*     iName; // Owned
    
    TUpnpAVDeviceType iDeviceType;
    
    TBool iCopyCapability;
    
    TBool iSearchCapability;
    
    TBool iPauseCapability;
    
    TBool iVolumeCapability;
    
    TBool iMuteCapability;
    
    TBool iAudioMediaCapability;
    
    TBool iImageMediaCapability;
    
    TBool iVideoMediaCapability;
    
    TBool iNextAVTransportUri;

    TInt iMaxVolume;            
    
    TBool iDlnaCompatible;            
    };


#endif // C_CUPNPAVDEVICE_H