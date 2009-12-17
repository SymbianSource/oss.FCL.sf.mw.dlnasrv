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
* Description:      The device object
*
*/






// INCLUDE FILES

#include "upnpavdevice.h"

// CONSTANTS
const TInt KMaxStringLen = 255;
const TInt KBufferGranularity = 100;

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// CONSTANTS
_LIT8( KAudioSupport,        "audio/" );
_LIT8( KImageSupport,        "image/" );
_LIT8( KVideoSupport,        "video/" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVDevice::CUpnpAVDevice
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice::CUpnpAVDevice()
    {    
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUpnpAVDevice::ConstructL()
    {                           
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice* CUpnpAVDevice::NewL()
    {
    CUpnpAVDevice* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice* CUpnpAVDevice::NewL( const CUpnpAVDevice& aDevice )
    {
    CUpnpAVDevice* self = NewLC();
    
    self->CopyFromL( aDevice );

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice* CUpnpAVDevice::NewLC()
    {
    CUpnpAVDevice* self = new( ELeave )
        CUpnpAVDevice;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// --------------------------------------------------------------------------
// CUpnpAVDevice::~CUpnpAVDevice
// Destructor
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice::~CUpnpAVDevice()
    {
    delete iName;
    delete iUuid;            
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::ExternalizeL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::ExternalizeL( RWriteStream& aStream ) const
    {
    __ASSERTD( iUuid, __FILE__, __LINE__ );
    __ASSERTD( iName, __FILE__, __LINE__ );
    
    aStream << *iUuid;
    aStream << *iName;
    aStream.WriteInt32L( (TInt)iDeviceType );
    aStream.WriteInt32L( (TInt)iCopyCapability );
    aStream.WriteInt32L( (TInt)iSearchCapability );
    aStream.WriteInt32L( (TInt)iPauseCapability );
    aStream.WriteInt32L( (TInt)iVolumeCapability );
    aStream.WriteInt32L( (TInt)iMuteCapability );
    aStream.WriteInt32L( (TInt)iAudioMediaCapability );
    aStream.WriteInt32L( (TInt)iImageMediaCapability );
    aStream.WriteInt32L( (TInt)iVideoMediaCapability );
    aStream.WriteInt32L( (TInt)iNextAVTransportUri );
    aStream.WriteInt32L( (TInt)iMaxVolume );
    aStream.WriteInt32L( (TInt)iDlnaCompatible );
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDevice::InternalizeL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::InternalizeL( RReadStream& aStream )
    {
    delete iUuid;
    iUuid = NULL;
    iUuid = HBufC8::NewL( aStream, KMaxStringLen );

    delete iName;
    iName = NULL;
    iName = HBufC8::NewL( aStream, KMaxStringLen );
    
    iDeviceType = (TUpnpAVDeviceType)aStream.ReadInt32L();
    iCopyCapability = (TBool)aStream.ReadInt32L();
    iSearchCapability = (TBool)aStream.ReadInt32L();
    iPauseCapability = (TBool)aStream.ReadInt32L();
    iVolumeCapability = (TBool)aStream.ReadInt32L();
    iMuteCapability = (TBool)aStream.ReadInt32L();
    iAudioMediaCapability = (TBool)aStream.ReadInt32L();
    iImageMediaCapability = (TBool)aStream.ReadInt32L();
    iVideoMediaCapability = (TBool)aStream.ReadInt32L();
    iNextAVTransportUri = (TBool)aStream.ReadInt32L();
    iMaxVolume = (TInt)aStream.ReadInt32L();
    iDlnaCompatible = (TBool)aStream.ReadInt32L();
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::ToDes8L
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C HBufC8* CUpnpAVDevice::ToDes8L() const
    {
    // serialize object
    CBufFlat* tempFlatBuf = CBufFlat::NewL( KBufferGranularity );
    CleanupStack::PushL( tempFlatBuf );

    RBufWriteStream stream( *tempFlatBuf );
    CleanupClosePushL( stream );
    
    stream << *this;
    
    // create heap descriptor
    HBufC8* tempBuf = HBufC8::NewLC( tempFlatBuf->Size() );
    TPtr8 ptr( tempBuf->Des() );
    tempFlatBuf->Read( 0, ptr, tempFlatBuf->Size() );
    
    // clean up
    CleanupStack::Pop( tempBuf );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( tempFlatBuf );
    
    return tempBuf;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::CopyFromL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::CopyFromL( const CUpnpAVDevice& aDevice )
    {
    iDeviceType = aDevice.DeviceType();
    SetFriendlyNameL( aDevice.FriendlyName() );
    SetUuidL( aDevice.Uuid() );
    iCopyCapability = aDevice.CopyCapability();
    iSearchCapability = aDevice.SearchCapability();
    iPauseCapability = aDevice.PauseCapability();
    iVolumeCapability = aDevice.VolumeCapability();
    iMuteCapability = aDevice.MuteCapability();
    iAudioMediaCapability = aDevice.AudioCapability();
    iImageMediaCapability = aDevice.ImageCapability();
    iVideoMediaCapability = aDevice.VideoCapability();
    iNextAVTransportUri = aDevice.NextAVTransportUri();
    iMaxVolume = aDevice.MaxVolume();
    iDlnaCompatible = aDevice.DlnaCompatible();
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetFriendlyNameL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetFriendlyNameL( const TDesC8& aName )
    {
    HBufC8* tempBuf = aName.AllocL();
    delete iName;
    iName = tempBuf;    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDevice::FriendlyName
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C const TDesC8& CUpnpAVDevice::FriendlyName() const
    {
    if( iName )
        {
        return *iName;
        }
    else
        {
        return KNullDesC8;
        }        
    }


// --------------------------------------------------------------------------
// CUpnpAVDevice::SetUuidL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetUuidL( const TDesC8& aUuid )
    {
    HBufC8* tempBuf = aUuid.AllocL();
    delete iUuid;
    iUuid = tempBuf;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::Uuid
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C const TDesC8& CUpnpAVDevice::Uuid() const
    {
    if( iUuid )
        {
        return *iUuid;
        }
    else
        {
        return KNullDesC8;
        }    
    }


// --------------------------------------------------------------------------
// CUpnpAVDevice::SetDeviceType
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetDeviceType( TUpnpAVDeviceType aDeviceType )
    {
    iDeviceType = aDeviceType;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDevice::DeviceType
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C CUpnpAVDevice::TUpnpAVDeviceType CUpnpAVDevice::DeviceType() const
    {
    return iDeviceType;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetCapabilitiesBySupportedMimeTypesL
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetCapabilitiesBySupportedMimeTypesL(
                                const TDesC8& aListOfMimeTypes )
    {
    if( aListOfMimeTypes == KNullDesC8 )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        // Update the audio media capability
        if( aListOfMimeTypes.Find( KAudioSupport ) >= 0 )
            {
            iAudioMediaCapability = ETrue;
            }
        else
            {
            iAudioMediaCapability = EFalse;
            }

        // Update the image media capability
        if( aListOfMimeTypes.Find( KImageSupport ) >= 0 )
            {
            iImageMediaCapability = ETrue;
            }
        else
            {
            iImageMediaCapability = EFalse;
            }

        // Update the video media capability
        if( aListOfMimeTypes.Find( KVideoSupport ) >= 0 )
            {
            iVideoMediaCapability = ETrue;
            }
        else
            {
            iVideoMediaCapability = EFalse;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::LinkOffset
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C TInt CUpnpAVDevice::LinkOffset()
    {
    return _FOFF( CUpnpAVDevice, iSlink );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetCopyCapability
// Sets the copy capability of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetCopyCapability( TBool aCopyCap )
    {
    iCopyCapability = aCopyCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::CopyCapability
// Gets the copy capability of the device
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::CopyCapability() const
    {
    return iCopyCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetSearchCapability
// Sets the search capability of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetSearchCapability( TBool aSearchCap )
    {
    iSearchCapability = aSearchCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SearchCapability
// Gets the search capability of the device
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::SearchCapability() const
    {
    return iSearchCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetPauseCapability
// Sets the pause capability of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetPauseCapability( TBool aPauseCap )
    {
    iPauseCapability = aPauseCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::PauseCapability
// Gets the pause capability of the device
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::PauseCapability() const
    {
    return iPauseCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetVolumeCapability
// Sets the volume capability of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetVolumeCapability( TBool aVolumeCap )
    {
    iVolumeCapability = aVolumeCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::VolumeCapability
// Gets the volume capability of the device
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::VolumeCapability() const
    {
    return iVolumeCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetMuteCapability
// Sets the mute capability of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetMuteCapability( TBool aMuteCap )
    {
    iMuteCapability = aMuteCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::MuteCapability
// Gets the mute capability of the device
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::MuteCapability() const
    {
    return iMuteCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetMaxVolume
// Sets the maximum volume of the device
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetMaxVolume( TInt aVolume )
    {
    iMaxVolume = aVolume;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::MaxVolume
// Gets the maximum volume of the device
// --------------------------------------------------------------------------
EXPORT_C TInt CUpnpAVDevice::MaxVolume() const
    {
    return iMaxVolume;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetNextAVTransportUri
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetNextAVTransportUri( TBool aCap )
    {
    iNextAVTransportUri = aCap;
    __LOG1( "CUpnpAVDevice::SetNextAVTransportUri: iNextAVTransportUri %d",
            iNextAVTransportUri );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::NextAVTransportUri
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::NextAVTransportUri() const
    {
    __LOG1( "CUpnpAVDevice::NextAVTransportUri: iNextAVTransportUri %d",
            iNextAVTransportUri );
    // Do not support NextAVTransportUri for now
    __LOG( "return EFalse" );
    return EFalse;
    }           

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetAudioCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetAudioCapability( TBool aCap )
    {
    iAudioMediaCapability = aCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::AudioCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::AudioCapability() const
    {
    return iAudioMediaCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetImageCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetImageCapability( TBool aCap )
    {
    iImageMediaCapability = aCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::ImageCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::ImageCapability() const
    {
    return iImageMediaCapability;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetVideoCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetVideoCapability( TBool aCap )
    {
    iVideoMediaCapability = aCap;
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::VideoCapability
// See upnpavdevice.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::VideoCapability() const
    {
    return iVideoMediaCapability;     
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetDlnaCompatible
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpAVDevice::SetDlnaCompatible( TBool aDlnaCompatible )
    {
    iDlnaCompatible = aDlnaCompatible;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDevice::DlnaCompatible
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpAVDevice::DlnaCompatible() const
    {
    return iDlnaCompatible;
    }

// End of File

