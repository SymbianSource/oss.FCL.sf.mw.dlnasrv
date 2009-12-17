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
* Description:      implements a session towards a renderer
*
*/






// INCLUDES
// upnp stack api
#include <upnpitem.h>
#include <upnpobjectlist.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpavcontrollerglobals.h"
#include "upnpavrenderingsessionobserver.h"

// avcontroller internal
#include "upnpaveventactive.h"
#include "upnpavcontrollerclient.h"
#include "upnpavdeviceactive.h"
#include "upnpavrenderingactive.h"
#include "upnpavrenderingsessionimpl.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::CUPnPAVRenderingSessionImpl
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
CUPnPAVRenderingSessionImpl* CUPnPAVRenderingSessionImpl::NewL( 
                                            RUPnPAVControllerClient& aServer, 
                                            const CUpnpAVDevice& aDevice )
    {
    CUPnPAVRenderingSessionImpl* self = 
                        new (ELeave) CUPnPAVRenderingSessionImpl( aServer );
    CleanupStack::PushL( self );
    self->iDevice = CUpnpAVDevice::NewL( aDevice );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::CUPnPAVRenderingSessionImpl
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
CUPnPAVRenderingSessionImpl::CUPnPAVRenderingSessionImpl( 
                                        RUPnPAVControllerClient& aServer ):
    iServer( aServer ),
    iBufferPtr( 0, 0 ),
    iAlive( ETrue )
    {
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::~CUPnPAVRenderingSessionImpl
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
CUPnPAVRenderingSessionImpl::~CUPnPAVRenderingSessionImpl()
    {
    __LOG( "CUPnPAVRenderingSessionImpl::~CUPnPAVRenderingSessionImpl" );
    
    delete iDevice;
    delete iEventActive;
    delete iCommandActive;
    delete iSettingActive;
    delete iDeviceActive;
    delete iBuffer; 

    iServer.DestroyRenderingSession( (TInt)this );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::ConstructL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::ConstructL()
    {
    __LOG( "CUPnPAVRenderingSessionImpl::ConstructL" );
    
    iBuffer = iDevice->Uuid().AllocL();
    iBufferPtr.Set( iBuffer->Des() );
    User::LeaveIfError( iServer.CreateRenderingSession( (TInt)this , 
                                                        iBufferPtr ) );
    iEventActive = CUPnPAVEventActive::NewL( iServer, *this );
    iCommandActive = CUPnPAVRenderingActive::NewL( iServer, (TInt)this );
    iSettingActive = CUPnPAVRenderingActive::NewL( iServer, (TInt)this );
    iDeviceActive = CUPnPAVDeviceActive::NewL( iServer, *this );
    iDeviceActive->StartListening( (TInt)this );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::SetObserver
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::SetObserver( 
                                 MUPnPAVRenderingSessionObserver& aObserver )
    {
    // Set the observer and make an async request to the server to receive
    // device callbacks
    __ASSERTD( !iObserver, __FILE__, __LINE__ );

    iEventActive->StartListeningEvents( (TInt)this );        
        
    iObserver = &aObserver;
    iCommandActive->SetObserver( aObserver );
    iSettingActive->SetObserver( aObserver );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::Observer
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
MUPnPAVRenderingSessionObserver* CUPnPAVRenderingSessionImpl::Observer() const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::RemoveObserver
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::RemoveObserver()
    {
    iObserver = NULL;
    iCommandActive->RemoveObserver();
    iSettingActive->RemoveObserver();    
    iEventActive->Cancel();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::Device
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
const CUpnpAVDevice& CUPnPAVRenderingSessionImpl::Device() const
    {
    return *iDevice;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::ReserveLocalMSServicesL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::ReserveLocalMSServicesL()
    {  
    ResetL();
    
    iCommandActive->ReserveLocalMSServicesL();       
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::CancelReserveLocalMSServicesL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::CancelReserveLocalMSServicesL()
    {
    ResetL();
    
    iCommandActive->CancelReserveLocalMSServicesL();  
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::ReleaseLocalMSServicesL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::ReleaseLocalMSServicesL()
    {  
    ResetL();
    
    iCommandActive->ReleaseLocalMSServicesL();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::SetURIL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::SetURIL( const TDesC8& aURI,
    const CUpnpItem& aItem )
    {
    ResetL();
    
    iCommandActive->SetURIL( aURI, aItem );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::SetNextURIL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::SetNextURIL( const TDesC8& aURI,
    const CUpnpItem& aItem )    
    {
    ResetL();
    
    iCommandActive->SetNextURIL( aURI, aItem );     
    }


// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::PlayL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::PlayL()
    {
    ResetL();
    
    iCommandActive->PlayL();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::StopL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::StopL()
    {
    ResetL();
    
    iCommandActive->StopL(); 
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::PauseL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::PauseL()
    {
    ResetL();
    
    iCommandActive->PauseL();   
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::SetVolumeL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::SetVolumeL( TInt aVolumeLevel )
    {
    ResetL();
    
    iSettingActive->SetVolumeL( aVolumeLevel );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::GetVolumeL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::GetVolumeL()
    {
    ResetL();
    
    iSettingActive->GetVolumeL();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::SetMuteL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::SetMuteL( TBool aMute )
    {
    ResetL();
    
    iSettingActive->SetMuteL( aMute );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::GetMuteL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::GetMuteL()
    {
    ResetL();
    
    iSettingActive->GetMuteL();           
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::GetPositionInfoL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::GetPositionInfoL()
    {
    ResetL();
    
    iSettingActive->GetPositionInfoL();               
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::UPnPDeviceDiscovered
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::UPnPDeviceDiscovered( 
                                        const CUpnpAVDevice& /*aDevice*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::UPnPDeviceDisappeared
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::UPnPDeviceDisappeared( 
                                        const CUpnpAVDevice& /*aDevice*/ )
    {
    __LOG( "CUPnPAVRenderingSessionImpl::UPnPDeviceDisappeared" );
    
    iAlive = EFalse;
    if( iObserver )
        {
        iObserver->MediaRendererDisappeared(
            MUPnPAVSessionObserverBase::EDisconnected );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::WLANConnectionLost
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::WLANConnectionLost()
    {
    __LOG( "CUPnPAVRenderingSessionImpl::WLANConnectionLost" );
    
    iAlive = EFalse;
    if( iObserver )    
        {
        iObserver->MediaRendererDisappeared(
            MUPnPAVSessionObserverBase::EWLANLost );
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::EventReceived
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::EventReceived( TInt aError,
    TUnsolicitedEventC aEvent )
    {
    if( iObserver )
        {
        switch( aEvent.iEvent )
            {
            case EPlay:
                {
                iObserver->InteractOperationComplete( aError,
                    EUPnPAVPlayUser );
                break;
                }
            case EPause:
                {
                iObserver->InteractOperationComplete( aError,
                    EUPnPAVPauseUser );                
                break;
                }
            case EStop:
                {
                iObserver->InteractOperationComplete( aError,
                    EUPnPAVStopUser );                
                break;
                }
            case EVolume:
                {
                iObserver->VolumeResult( aError, aEvent.iValue, EFalse );
                break;
                }
            case EMute:
                {
                iObserver->MuteResult( aError, (TBool)aEvent.iValue,
                    EFalse );
                break;
                }
            default:
                {
                __PANICD( __FILE__, __LINE__ );
                break;
                }    
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingSessionImpl::ResetL
// See upnpavrenderingsessionimpl.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingSessionImpl::ResetL()
    {
    __LOG( "CUPnPAVRenderingSessionImpl::ResetL" );
    
    if( !iAlive )
        {
        __LOG( "ResetL - disconnected, leave " );
        User::Leave( KErrDisconnected );        
        }

    delete iBuffer; iBuffer = NULL;
    }

// end of file
