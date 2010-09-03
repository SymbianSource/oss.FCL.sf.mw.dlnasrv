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
* Description:      storage for devices with extended information
*
*/






// INCLUDE FILES
#include "upnpdevicerepository.h"

#include "upnpavdeviceextended.h"

#include <upnpservice.h>
#include <upnpdevice.h>
#include <upnpavcontrolpoint.h>

// CONSTANTS
_LIT8( KSearch,                     "Search" );
_LIT8( KVolume,                     "Volume" );
_LIT8( KCreateObject,               "CreateObject" );
_LIT8( KPause,                      "Pause");
_LIT8( KSetVolume,                  "SetVolume");
_LIT8( KGetVolume,                  "GetVolume");
_LIT8( KGetMute,                    "GetMute");
_LIT8( KSetMute,                    "SetMute");
_LIT8( KMediaServer,                "MediaServer" );
_LIT8( KFriendlyName,               "friendlyName" );
_LIT8( KAVTransportService,         "AVTransport" );
_LIT8( KRenderingControlService,    "RenderingControl" );
_LIT8( KSetNextUri,                 "SetNextAVTransportURI" );
_LIT8( KDestroyObject,              "DestroyObject" );
_LIT8( KDlnaDoc,                    "dlna:X_DLNADOC" );
_LIT8( KDlnaCap,                    "dlna:X_DLNACAP" );
_LIT8( KAudioUpload,                "audio-upload" );
_LIT8( KImageUpload,                "image-upload" );
_LIT8( KVideoUpload,                "av-upload" );
_LIT8( KCreateChildContainer,       "create-child-container" );
_LIT8( KDMS,                        "DMS" );
_LIT8( KDMP,                        "DMP" );
_LIT8( KDMR,                        "DMR" );

const TInt KFirstSubscription = 1;

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::NewL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
CUPnPDeviceRepository* CUPnPDeviceRepository::NewL
    (
    CUpnpAVControlPoint& aControlPoint
    )
    {
    CUPnPDeviceRepository* rep= new(ELeave)
        CUPnPDeviceRepository( aControlPoint );
    CleanupStack::PushL( rep );
    rep->ConstructL();
    CleanupStack::Pop();
    return rep;
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::~CUPnPDeviceRepository
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
CUPnPDeviceRepository::~CUPnPDeviceRepository()
    {     
    iDevices.ResetAndDestroy();
    iDevices.Close();
    }
    
// --------------------------------------------------------------------------
// CUPnPDeviceRepository::CUPnPDeviceRepository
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
CUPnPDeviceRepository::CUPnPDeviceRepository
    (
    CUpnpAVControlPoint& aControlPoint
    ):
    iControlPoint( aControlPoint ),
    iIsWlanActive( ETrue )    
    {
    }
    
// --------------------------------------------------------------------------
// CUPnPDeviceRepository::ConstructL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::ConstructL()
    {
    __LOG( "CUPnPDeviceRepository::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::AddDeviceL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::AddDeviceL( CUpnpDevice& aDevice )
    {
    __LOG( "CUPnPDeviceRepository::AddDeviceL" );
        
    CUpnpAVDeviceExtended* dev = CUpnpAVDeviceExtended::NewL();
    CleanupStack::PushL( dev );
    
    // Check if it's a dlna device
    TPtrC8 ptr = aDevice.GetProperty( KDlnaDoc );
    if( ptr.Length() > 0 )
        {
        __LOG( "Dlna compatible device!" );
        // It's a dlna device
        dev->SetDlnaCompatible( ETrue );
        
        if( ptr.FindC( KDMS ) != KErrNotFound )
            {
            dev->SetDLNADeviceType( CUpnpAVDeviceExtended::EDMS );
            }
        else if( ptr.FindC( KDMR ) != KErrNotFound )
            {
            dev->SetDLNADeviceType( CUpnpAVDeviceExtended::EDMR );
            }
        else if( ptr.FindC( KDMP ) != KErrNotFound )
            {
            dev->SetDLNADeviceType( CUpnpAVDeviceExtended::EDMP );
            }        
        
        // Check dlna capabilities
        ptr.Set( aDevice.GetProperty( KDlnaCap ) );
        if( ptr.Find( KAudioUpload ) != KErrNotFound )        
            {
            __LOG( "Audio upload supported!" );
            dev->SetAudioUpload( ETrue );
            }
        if( ptr.Find( KImageUpload ) != KErrNotFound )
            {
            __LOG( "Image upload supported!" );
            dev->SetImageUpload( ETrue );
            }
        if( ptr.Find( KVideoUpload ) != KErrNotFound )
            {
            __LOG( "Video upload supported!" );
            dev->SetVideoUpload( ETrue );
            }
        if( ptr.Find( KCreateChildContainer ) != KErrNotFound )
            {
            __LOG( "Create child container supported!" );
            dev->SetCreateChildContainer( ETrue );
            }        
        }   
        
    if( aDevice.DeviceType().Find( KMediaServer ) != KErrNotFound )
        {
        dev->SetDeviceType( CUpnpAVDevice::EMediaServer );
        }
    else
        {
        dev->SetDeviceType( CUpnpAVDevice::EMediaRenderer );
        }

    dev->SetFriendlyNameL( aDevice.DescriptionProperty( KFriendlyName ) );

    dev->SetUuidL( aDevice.Uuid() );   
    
    dev->SetLocal( aDevice.Local() );
    
    ParseDeviceServicesL( aDevice, *dev );
    CleanupStack::Pop( dev );
    iDevices.AppendL( dev );
    }
    
// --------------------------------------------------------------------------
// CUPnPDeviceRepository::AddProtocolInfoL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended& CUPnPDeviceRepository::AddProtocolInfoL(
    const TDesC8& aUuid, const TDesC8& aSource, const TDesC8& aSink )
    {
    __LOG( "CUPnPDeviceRepository::AddProtocolInfoL" );
    
    // Find the device
    TInt count = iDevices.Count();
    CUpnpAVDeviceExtended* dev = NULL;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDevices[ i ]->Uuid() == aUuid )
            {
            dev = iDevices[ i ];
            i = count;
            }
        }
    if( dev )
        {
        if( dev->DeviceType() == CUpnpAVDevice::EMediaServer )
            {
            dev->SetSourceProtocolInfoL( aSource );
            dev->SetSinkProtocolInfoL( aSink );
            dev->SetCapabilitiesBySupportedMimeTypesL( aSource );
            }
        else
            {
            dev->SetSourceProtocolInfoL( aSource );
            dev->SetSinkProtocolInfoL( aSink );
            dev->SetCapabilitiesBySupportedMimeTypesL( aSink );    
            }
        dev->SetPInfoReceived( ETrue );        
        }
    else
        {
        User::Leave( KErrNotFound );
        }    
    return *dev;
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::Remove
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::Remove( const TDesC8& aUuid )
    {
    __LOG( "CUPnPDeviceRepository::Remove" );
    
    TInt count = iDevices.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDevices[ i ]->Uuid() == aUuid )
            {
            delete iDevices[ i ];
            iDevices.Remove( i );
            i = count;
            }
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::FindDeviceL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended& CUPnPDeviceRepository::FindDeviceL(
    const TDesC8& aUuid )
    {
    __LOG( "CUPnPDeviceRepository::FindDeviceL" );
    
    CUpnpAVDeviceExtended* tmp = NULL;
    TInt count = iDevices.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iDevices[ i ]->Uuid() == aUuid )
            {
            tmp = iDevices[ i ];
            i = count;
            }
        }
    if( !tmp )
        {
        __LOG( "FindDeviceL - not found" );
        
        User::Leave( KErrNotFound );
        }
    return *tmp;    
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::DeviceList
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
const RPointerArray<CUpnpAVDeviceExtended>&
    CUPnPDeviceRepository::DeviceList() const
    {
    __LOG( "CUPnPDeviceRepository::DeviceList" );
    
    return iDevices;
    }
   
// --------------------------------------------------------------------------
// CUPnPDeviceRepository::SubscribeDeviceL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::SubscribeDeviceL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPDeviceRepository::SubscribeDeviceL" );
    __LOG8( aUuid );
    
    // Find the device and increase subscription count/check if we have
    // subscribed already
    TInt count = iDevices.Count();
    TInt subscriptionCount = KErrNotFound;
    TInt index;
    for( index = 0; index < count; index++ )
        {
        if( aUuid.Compare( iDevices[ index ]->Uuid() ) == 0 )
            {
            subscriptionCount = iDevices[ index ]->IncreaseSubscriptionCount();
            index = count;
            }
        }
    if( subscriptionCount == KFirstSubscription )
        {
        __LOG( "SubscribeDeviceL - First subscription" ); 
        
        // Start subsciption for AVTransport and RenderingControl services
        // Find the device
        const RPointerArray<CUpnpDevice>& devList =
            iControlPoint.DeviceList();
        count = devList.Count();
        for( index = 0; index < count; index++ )
            {
            if( aUuid.Compare( devList[ index ]->Uuid() ) == 0 )
                {
                break;
                }
            }

        // Find the AVTransport service and subscribe
        RPointerArray<CUpnpService>& servList =
            devList[ index ]->ServiceList();
        count = servList.Count();
        CUpnpService* tempService = NULL;
        for( index = 0; index < count; index++ )
            {
            if( servList[ index ]->ServiceType().Find(
                KAVTransportService ) >= 0 )
                {
                tempService = servList[ index ];
                index = count;
                }
            }
        if( tempService && iIsWlanActive )
            {
            // AVTransport service found for the device, subscribe
            __LOG( "SubscribeDeviceL - Subscribe for AVTransport" ); 
            iControlPoint.SubscribeL( tempService ); 
            }
        else
            {     
            // Service not found, can't subscribe
            __LOG( "SubscribeDeviceL - AVTransport service not found" );
            }
            
        tempService = NULL;
        for( index = 0; index < count; index++ )
            {
            if( servList[ index ]->ServiceType().Find(
                KRenderingControlService ) >= 0 )
                {
                tempService = servList[ index ];
                index = count;
                }
            }
        if( tempService && iIsWlanActive )
            {
            // RenderingControl service found for the device, subscribe
            __LOG( "SubscribeDeviceL - Subscribe for RenderingControl" ); 
            iControlPoint.SubscribeL( tempService ); 
            }
        else
            {     
            // Service not found, can't subscribe
            __LOG( "SubscribeDeviceL - RenderingControl service not found" );
            }                    
        }
    else if( subscriptionCount == KErrNotFound ) 
        {
        __LOG( "SubscribeDeviceL - device not found" );
        }
    else
        {
        // Subscribed already, do nothing
        __LOG( "SubscribeDeviceL - Subscription done already, ignoring!" ); 
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPDeviceRepository::UnSubscribeDeviceL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::UnSubscribeDeviceL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPDeviceRepository::UnSubscribeDeviceL" );
    __LOG8( aUuid );
    
    // Find the device and decrease subscription count/check if it's needed
    // to unsubscribe
    TInt count = iDevices.Count();
    TInt subscriptionCount = KErrNotFound;
    TInt index;
    for( index = 0; index < count; index++ )
        {
        if( aUuid.Compare( iDevices[ index ]->Uuid() ) == 0 )
            {
            subscriptionCount = 
                iDevices[ index ]->DecreaseSubscriptionCount();
            index = count;
            }
        }    
    
    if( subscriptionCount == 0 )
        {   
        // Start unsubsciption for AVTransport and RenderingControl services
        // Find the device
        const RPointerArray<CUpnpDevice>& devList =
            iControlPoint.DeviceList();
        count = devList.Count();
        for( index = 0; index < count; index++ )
            {
            if( aUuid.Compare( devList[ index ]->Uuid() ) == 0 )
                {
                break;
                }
            }

        // Find the AVTransport service and unsubscribe
        RPointerArray<CUpnpService>& servList =
            devList[ index ]->ServiceList();
        count = servList.Count();
        CUpnpService* tempService = NULL;
        for( index = 0; index < count; index++ )
            {
            if( servList[ index ]->ServiceType().Find(
                KAVTransportService ) >= 0 )
                {
                tempService = servList[ index ];
                index = count;
                }
            }
        if( tempService && iIsWlanActive )
            {
            // AVTransport service found for the device, unsubscribe
            __LOG( "UnSubscribeDeviceL - UnSubscribe AVTransport" ); 
            iControlPoint.UnsubscribeL( tempService ); 
            }
        else
            {     
            // Service not found, can't unsubscribe
            __LOG( "UnSubscribeDeviceL - AVTransport service not found" );
            }
            
        tempService = NULL;
        for( index = 0; index < count; index++ )
            {
            if( servList[ index ]->ServiceType().Find(
                KRenderingControlService ) >= 0 )
                {
                tempService = servList[ index ];
                index = count;
                }
            }
        if( tempService && iIsWlanActive )
            {
            // RenderingControl service found for the device, subscribe
            __LOG( "UnSubscribeDeviceL - UnSubscribe RenderingControl" ); 
            iControlPoint.UnsubscribeL( tempService ); 
            }
        else
            {     
            // Service not found, can't subscribe
            __LOG( "UnSubscribeDeviceL - RenderingControl service not\
found" );
            }                    
        }
    else if( subscriptionCount == KErrNotFound ) 
        {
        __LOG( "UnSubscribeDeviceL - device not found" );
        }
    else
        {
        // No need to unsubscibe
        }           
    }

void CUPnPDeviceRepository::ConnectionLost()
    {
    __LOG( "CUPnPDeviceRepository::ConnectionLost" );
    iIsWlanActive = EFalse;
    iDevices.ResetAndDestroy();
    }

TBool CUPnPDeviceRepository::IsWlanActive()
    {
    __LOG1( "CUPnPDeviceRepository::IsWlanActive, %d", (TInt)iIsWlanActive );
    return iIsWlanActive;
    }

// --------------------------------------------------------------------------
// CUPnPDeviceRepository::ParseDeviceServicesL
// See upnpdevicerepository.h
// --------------------------------------------------------------------------
void CUPnPDeviceRepository::ParseDeviceServicesL( CUpnpDevice& aSource,
    CUpnpAVDeviceExtended& aTarget )
    {
    __LOG( "CUPnPDeviceRepository::ParseDeviceServicesL" );
    
    TBool createObjectSupported = EFalse;
    TBool getMuteSupported = EFalse;
    TBool setMuteSupported = EFalse;
    TBool getVolumeSupported = EFalse;
    TBool setVolumeSupported = EFalse;

    RPointerArray<CUpnpService>services = aSource.ServiceList();

    TInt i;
    TInt count = services.Count(); 
    for( i = 0; i < count; i++ )
        {
        // Get max volume if it exists
        CUpnpStateVariable* volumeState = 
                services[ i ]->StateVariable( KVolume );

        // If volume info found, save it to the device
        if( volumeState )
            {
            TInt maxVolume = volumeState->MaxValue();
            // If max volume not defined, it is set to 100
            if( maxVolume == KErrNotFound )
                {
                maxVolume = 100;
                }
            aTarget.SetMaxVolume( maxVolume );
            }

        // Get the actions
        RPointerArray<CUpnpAction> actions;
        services[ i ]->GetActionList( actions );

        // Go through the action elements
        TInt j;
        TInt count2 = actions.Count();
        for( j = 0; j < count2; j++ )
            {
            TDesC8& actionName = actions[ j ]->Name();

            if( actionName != KNullDesC8 )
                {
                if( actionName.Find( KCreateObject ) >= 0 )
                    {
                    createObjectSupported = ETrue;
                    }
                if( actionName.Compare( KSearch ) == 0 )
                    {
                    aTarget.SetSearchCapability( ETrue );
                    }
                if( actionName.Find( KPause ) >= 0 )
                    {
                    aTarget.SetPauseCapability( ETrue );
                    }
                if( actionName.Find( KGetVolume ) >= 0 )
                    {
                    getVolumeSupported = ETrue;
                    }
                if( actionName.Find( KSetVolume ) >= 0 )
                    {
                    setVolumeSupported = ETrue;
                    }
                if( actionName.Find( KGetMute ) >= 0 )
                    {
                    getMuteSupported = ETrue;
                    }
                if( actionName.Find( KSetMute ) >= 0 )
                    {
                    setMuteSupported = ETrue;
                    }
                if( actionName.Find( KSetNextUri ) >= 0 )
                    {
                    aTarget.SetNextAVTransportUri( ETrue );
                    }
                if( actionName.Find( KDestroyObject ) >= 0 )
                    {
                    aTarget.SetDestroyObject( ETrue );
                    }                    
                }
            }
        }

    // Set copy support
    if( createObjectSupported )
        {
        aTarget.SetCopyCapability( ETrue );
        }

    // Set volume support
    if( getVolumeSupported &&
        setVolumeSupported )
        {
        aTarget.SetVolumeCapability( ETrue );
        }

    // Set mute support
    if( getMuteSupported &&
        setMuteSupported )
        {
        aTarget.SetMuteCapability( ETrue );
        }
    }        

// end of file
