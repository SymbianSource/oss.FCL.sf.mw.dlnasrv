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
* Description:      Singleton class for main upnp related services
*
*/






// INCLUDES
#include <badesca.h>
#include "upnpavcontrollerfactory.h"
#include "upnpavcontroller.h"
#include "upnpavdevice.h"
#include "upnpavdevicelist.h"
#include "upnprendererselectorobserver.h"
#include "upnpsingleton.h"

// CONSTANTS
const TInt KDefaultGranularity = 5;
const TInt32 KRendererRequestTimeOut = 5000000;

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

// --------------------------------------------------------------------------
// Static members of CUPnPSingleton
// --------------------------------------------------------------------------
//

// reference count
TInt CUPnPSingleton::iInstanceCount = 0;

// reference count
CUPnPSingleton* CUPnPSingleton::iInstance = 0;

// default device index
TInt CUPnPSingleton::iDefaultRendererIndex;


// --------------------------------------------------------------------------
// CUPnPSingleton::GetInstanceL
// singleton constructor
// --------------------------------------------------------------------------
//
CUPnPSingleton* CUPnPSingleton::GetInstanceL()
    {
    if( iInstanceCount == 0 )
        {
        CUPnPSingleton* self = new(ELeave) CUPnPSingleton();
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        iInstance = self;
        }

    ++iInstanceCount;
    return iInstance;
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::CUPnPSingleton
// Default constructor.
// --------------------------------------------------------------------------
//
CUPnPSingleton::CUPnPSingleton()
    {
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::ConstructL
// ConstructL.
// --------------------------------------------------------------------------
//
void CUPnPSingleton::ConstructL()
    {
    // Create timer for observing play time out.
    iPeriodizer = CUPnPMusicPeriodizer::NewL( *this,
        KRendererRequestTimeOut );

    // Create handle to AVController
    iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL();
    iAVController->SetDeviceObserver( *this );
    CacheRendererListL();
    if ( iMediaRenderers.Count() > 0 )
        {
        iSelectorState = EStateReady;
        }
    else
        {
        iSelectorState = EStateWaiting;
        iPeriodizer->Start();
        }

    }

// --------------------------------------------------------------------------
// CUPnPSingleton::LoseInstance
// singleton desctuctor
// --------------------------------------------------------------------------
//
void CUPnPSingleton::LoseInstance( CUPnPSingleton* aInstance )
    {
    if( aInstance != 0 )
        {
        __ASSERTD( aInstance == iInstance,__FILE__, __LINE__ );
        __ASSERTD( iInstanceCount > 0,__FILE__, __LINE__ );
        --iInstanceCount;
        if( iInstanceCount == 0 )
            {
            delete iInstance;
            iInstance = 0;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::~CUPnPSingleton
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPSingleton::~CUPnPSingleton()
    {
    __LOG("CUPnPSingleton::~CUPnPSingleton()");
    iMediaRenderers.ResetAndDestroy();
    delete iPeriodizer;
    if( iAVController )
        {
        iAVController->Release();
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::GetRendererNamesL
// Get renderes from AVController and return the renderer names.
// --------------------------------------------------------------------------
//  
void CUPnPSingleton::GetRendererNamesL(
    MUPnPRendererSelectorObserver& aObserver )
    {
    __ASSERTD( iRendererselectorObserver == 0,__FILE__, __LINE__ );
    iRendererselectorObserver = &aObserver;

    switch( iSelectorState )
        {
        case EStateWaiting:
            {
            __LOG("GetRendererNames: waiting");
            // wait for something to happen
            }
            break;
        case EStateComplete:
            {
            __LOG("GetRendererNames: complete->ready");
            DeliverNamesToObserverL( ETrue );
            iSelectorState = EStateReady;
            }
            break;
        case EStateReady:
            {
            CacheRendererListL();
            if ( iMediaRenderers.Count() > 0 )
                {
                __LOG("GetRendererNames: ready");
                DeliverNamesToObserverL( ETrue );
                }
            else
                {
                __LOG("GetRendererNames: ready->waiting");
                iSelectorState = EStateWaiting;
                iPeriodizer->Start();
                }
            }
            break;
        case EStateError:
            {
            __LOG("GetRendererNames: error");
            DeliverNamesToObserverL( ETrue, KErrDisconnected );
            }
            break;
        default:
            {
            __PANICD(__FILE__, __LINE__ );
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::CancelGetRendererNames
// --------------------------------------------------------------------------
//
void CUPnPSingleton::CancelGetRendererNames()
    {
    // just clear the observer -> no callbacks will occur.
    iRendererselectorObserver = 0;
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::SelectRendererByIndexL
// Select renderer by given index.
// --------------------------------------------------------------------------
//
const CUpnpAVDevice* CUPnPSingleton::SelectRendererByIndexL( TInt aIndex )
    {
     if( iMediaRenderers.Count() <= aIndex || aIndex < 0 )
        {        
        __LOG("CUPnPSingl::SelectRendererByIndexL error: Wrong index");
        User::Leave( KErrArgument );
        }
        
    if ( iRendererselectorObserver != 0 )
        {
        DeliverNamesToObserverL( ETrue );
        }

    if ( !IsAvailable( *iMediaRenderers[aIndex] ) )
        {
        __LOG("CUPnPSingl::SelectRendererByIndexL error: disconnected");
        User::Leave( KErrDisconnected );
        }

    iDefaultRendererIndex = aIndex;

    return iMediaRenderers[aIndex];
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::DefaultDevice
// Return return pointer of current renderer
// --------------------------------------------------------------------------
//
const CUpnpAVDevice* CUPnPSingleton::DefaultDevice()
    {
    if ( iDefaultRendererIndex >= 0 )
        {
        return iMediaRenderers[iDefaultRendererIndex];
        }
    else
        {
        return 0;
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::AVC
// Provides reference to the AVController resource
// --------------------------------------------------------------------------
//
MUPnPAVController& CUPnPSingleton::AVC()
    {
    return *iAVController;
    }

    
// --------------------------------------------------------------------------
// Method from MUPnPAVMediaObserver
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPSingleton::UPnPDeviceDiscovered
// Inform that new renderer device has discovered.
// --------------------------------------------------------------------------
//
void CUPnPSingleton::UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice  )
    {
    // Check if device is media renderer
    if( aDevice.DeviceType() == CUpnpAVDevice::EMediaRenderer 
        && aDevice.AudioCapability() )
        {
        if ( iSelectorState == EStateWaiting ||
             iSelectorState == EStateComplete )
            {
            CUpnpAVDevice* dev = 0;
            TRAP_IGNORE( dev = CUpnpAVDevice::NewL( aDevice ) );
            if( dev )
                {
                iMediaRenderers.AppendL( dev );
                if ( iRendererselectorObserver != 0 )
                    {
                    TRAP_IGNORE( DeliverNamesToObserverL( EFalse ) );
                    }
                }
            }
        }
    else
        {
        // Do nothing with media servers
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::UPnPDeviceDisappeared
// Inform that renderer device has disappeared.
// --------------------------------------------------------------------------
//   
void CUPnPSingleton::UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice )
    {
    // Check if device is media renderer
    if( aDevice.DeviceType() == CUpnpAVDevice::EMediaRenderer 
        && aDevice.AudioCapability() )
        {
        if ( iSelectorState == EStateWaiting ||
             iSelectorState == EStateComplete )
            {
            // In monitoring state. Remove the renderer from cache.
            TInt count = iMediaRenderers.Count();
            for( TInt i = 0; i < count; i++ )
                {
                if( aDevice.Uuid().Compare(
                    iMediaRenderers[i]->Uuid() ) == 0 )
                    {
                    iMediaRenderers.Remove(i);
                    // we removed an renderer from the list so we want to 
                    // continue the loop with the same i.
                    // Could break here if absolutely sure that there are 
                    // no duplicates in iMediaRenderers.
                    count--;
                    i--; 
                    }
                }
            if ( iRendererselectorObserver != 0 )
                {
                TRAP_IGNORE( DeliverNamesToObserverL( EFalse ) );
                }
            }
        else if ( iSelectorState == EStateReady )
            {
            // Device disappears in ready state. Can't remove from cache
            // (because we use indexing to select to renderers)
            // instead mark the renderer unusable.
            __LOG( "Singleton: renderer disappeared in Ready state" );
            TInt count = iMediaRenderers.Count();
            TBool done = EFalse;
            for( TInt i = 0; i < count && !done; i++ )
                {
                if( aDevice.Uuid().Compare(
                    iMediaRenderers[i]->Uuid() ) == 0 )
                    {
                    SetAvailable( *iMediaRenderers[i], EFalse );
                    done = ETrue;
                    }
                }
            }
        }
    else
        {
        // Do nothing with media servers
        }
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::WLANConnectionLost
// Inform that renderer device has disappeared.
// --------------------------------------------------------------------------
// 
void CUPnPSingleton::WLANConnectionLost()
    {
    iMediaRenderers.ResetAndDestroy();
    if ( iSelectorState == EStateWaiting )
        {
        iPeriodizer->Stop();
        }

    iSelectorState = EStateError;

    if ( iRendererselectorObserver != 0 )
        {
        TRAP_IGNORE( DeliverNamesToObserverL( ETrue, KErrDisconnected ) );
        }
    }

// --------------------------------------------------------------------------
// Methods of MUPnPMusicPeriodizerObserver
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPSingleton::HandlePeriod
// Action when timer has expired.
// --------------------------------------------------------------------------
// 
void CUPnPSingleton::HandlePeriod()
    {
    __ASSERTD( iSelectorState == EStateWaiting,__FILE__, __LINE__ );

    if ( iMediaRenderers.Count() > 0 )
        {
        __LOG("GetRendererNames: timeout->complete");
        iSelectorState = EStateComplete;
        if ( iRendererselectorObserver != 0 )
            {
            __LOG("GetRendererNames: complete->ready");
            iSelectorState = EStateReady;
            TRAP_IGNORE( DeliverNamesToObserverL( ETrue ) );
            }
        }
    else
        {
        __LOG("GetRendererNames: timeout->continue timer");
        iPeriodizer->Continue();
        if ( iRendererselectorObserver != 0 )
            {
            TRAP_IGNORE( DeliverNamesToObserverL( ETrue ) );
            }
        }
    }

// --------------------------------------------------------------------------
// Private methods of CUPnPRendererSelector
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPSingleton::CacheRendererListL
// Refreshes the renderer cache from AVController
// --------------------------------------------------------------------------
// 
void CUPnPSingleton::CacheRendererListL()
    {
    // Clean old devices array and device names
    iDefaultRendererIndex = KErrNotFound;

    // Get media renderers
    CUpnpAVDeviceList* devices = iAVController->GetMediaRenderersL();
    CleanupStack::PushL( devices );

    // go through current renderer list
    for ( TInt i = 0; i < iMediaRenderers.Count(); ++i )
        {
        CUpnpAVDevice* renderer = iMediaRenderers[ i ];
        TBool exists = EFalse;
        for ( TInt j = 0; j < devices->Count() && !exists; ++j )
            {
            CUpnpAVDevice* device = (*devices)[j];
            if ( renderer->Uuid() == device->Uuid())
                {
                // renderer still exists.
                delete device;
                devices->Remove( j );
                SetAvailable( *renderer, ETrue );
                --j;
                exists = ETrue;
                }
            }
        if ( !exists )
            {
            // renderer has disappeared
            delete renderer;
            iMediaRenderers.Remove( i );
            --i;
            }
        }

    // Remaining devices are all new. Add them to end of the list.
    while ( devices->Count() > 0 )
        {
         __LOG("CacheRendererListL::Remaining devices are all new.");
        CUpnpAVDevice* device = (*devices)[ 0 ];
        if( device->AudioCapability() )
            {
            iMediaRenderers.AppendL( device );
            }
        devices->Remove( 0 );
        }

    CleanupStack::PopAndDestroy( devices ); 
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::DeliverNamesToObserverL
// Delivers current names in cached list to observer
// --------------------------------------------------------------------------
// 
void CUPnPSingleton::DeliverNamesToObserverL( TBool aCompleted,
    TInt aError )
    {
    __ASSERTD( iRendererselectorObserver != 0,__FILE__, __LINE__ );

    // Create an array for names
    CDesCArrayFlat* deviceNames = new(ELeave) CDesCArrayFlat( 
        KDefaultGranularity );
    CleanupStack::PushL( deviceNames );

    TInt count = iMediaRenderers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        HBufC* buf = HBufC::NewL(
            iMediaRenderers[i]->FriendlyName().Length() );
        CleanupStack::PushL( buf );
        buf->Des().Copy( iMediaRenderers[i]->FriendlyName() );
        deviceNames->AppendL( *buf ); // Transfer ownership
        CleanupStack::PopAndDestroy( buf );
        }

    // Call back current renderer names
    __LOG1( "DeliverNamesToObserver: count=%d", count );
    MUPnPRendererSelectorObserver& obs = *iRendererselectorObserver;
    iRendererselectorObserver = 0;
    obs.HandleSubPlayerNames( deviceNames, aCompleted, aError );

    CleanupStack::PopAndDestroy( deviceNames );
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::IsAvailable
// checks if the device type is renderer
// --------------------------------------------------------------------------
// 
TBool CUPnPSingleton::IsAvailable( const CUpnpAVDevice& aDevice ) const
    {
    return aDevice.DeviceType() == CUpnpAVDevice::EMediaRenderer;
    }

// --------------------------------------------------------------------------
// CUPnPSingleton::SetAvailable
// sets the device type to an illegal unrecognized type
// --------------------------------------------------------------------------
// 
void CUPnPSingleton::SetAvailable(
    CUpnpAVDevice& aDevice, TBool aAvailable ) const
    {
    if ( aAvailable && !IsAvailable( aDevice ) )
        {
        __LOG1( "CUPnPSingleton: renderer available: %S",
            &aDevice.FriendlyName() );
        aDevice.SetDeviceType(
            CUpnpAVDevice::EMediaRenderer );
        }
    else if ( !aAvailable && IsAvailable ( aDevice ) )
        {
        __LOG1( "CUPnPSingleton: renderer not available: %S",
            &aDevice.FriendlyName() );
        aDevice.SetDeviceType(
            (CUpnpAVDevice::TUpnpAVDeviceType)KErrUnknown );
        }
    }
