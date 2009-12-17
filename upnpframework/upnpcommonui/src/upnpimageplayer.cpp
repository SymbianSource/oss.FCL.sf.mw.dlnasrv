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
* Description:  Plays remote images on a renderer
*
*/


// INCLUDE FILES
// System
#include <aknnotewrappers.h>

// upnp stack api
#include <upnpobject.h>
#include <upnpitem.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"            // ResourceFromItemL

// upnpframework / internal api's
#include "upnpcdsreselementutility.h"

// commonui internal
#include "upnpimageplayer.h"
#include "upnpdeviceobserver.h"
#include <upnpcommonui.rsg>

_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVDevice::CUpnpAVDevice
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpImagePlayer::CUpnpImagePlayer( MUPnPAVController& aAVControl,
                                    MUPnPDeviceObserver& aDialog,
                                    CUPnPCommonUI& aCommonUI ):
                                    iAVControl(aAVControl),
                                    iDialog(aDialog),
                                    iCommonUI(aCommonUI)
    {    
    __LOG( "CUpnpImagePlayer::CUpnpImagePlayer" );
    iImageState = EImageIdle;
    iFirstStart = ETrue;
    iAction = CUPnPCommonUI::EUPnPNone;
    __LOG( "CUpnpImagePlayer::CUpnpImagePlayer end" );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::ConstructL()
    {                        
    
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpImagePlayer* CUpnpImagePlayer::NewL( 
                                    MUPnPAVController& aAVControl,
                                    MUPnPDeviceObserver& aDialog,
                                    CUPnPCommonUI& aCommonUI )
    {
    __LOG( "CUpnpImagePlayer::NewL" );
    CUpnpImagePlayer* self = new( ELeave )CUpnpImagePlayer( aAVControl,
                                                          aDialog,
                                                          aCommonUI );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    __LOG( "CUpnpImagePlayer::NewL end" );
    return self;
    }

// Destructor
CUpnpImagePlayer::~CUpnpImagePlayer()
    {   
    __LOG( "CUpnpImagePlayer::~CUpnpImagePlayer" );
    
    Stop();
    delete iUri;
    __LOG( "CUpnpImagePlayer::~CUpnpImagePlayer end" );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetTargetDeviceL
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::SetTargetDeviceL( const CUpnpAVDevice& aTargetDevice )
    {
    __LOG( "CUpnpImagePlayer::SetTargetDeviceL" );
    iTargetDevice = &aTargetDevice;  
    iImageState = EImageIdle;
    
    if( iRendSession )
        {
        iRendSession->RemoveObserver();
        iAVControl.StopRenderingSession( *iRendSession );
        iRendSession = NULL;
        }
    iRendSession = &iAVControl.StartRenderingSessionL( *iTargetDevice );
    iRendSession->SetObserver( *this );
    
    __LOG( "CUpnpImagePlayer::SetTargetDeviceL end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDevice::PlayL
// --------------------------------------------------------------------------
//  
void CUpnpImagePlayer::PlayL( const CUpnpObject& aItem )
    {
    __LOG( "CUpnpImagePlayer::PlayL" );
    
    iAction = CUPnPCommonUI::EUPnPShow;
    iItem = static_cast<const CUpnpItem*>( &aItem );
    delete iUri; iUri = NULL;

    iUri = UPnPItemUtility::ResourceFromItemL( *iItem ).Value().AllocL();
    
    if( iImageState == EImageIdle )
        {
        if( iFirstStart )
            {
            iRendSession->SetURIL( *iUri, *iItem ); //not possible to leave 
            iImageState = EImageInitialising;
            delete iUri; iUri = NULL;    
            }
        else //stopping previously played item
            {
            iRendSession->StopL();
            iImageState = EImageNext;
            }
        }
    else
        {
        iImageState = EImageNext;                
        }

    __LOG( "CUpnpImagePlayer::PlayL end" );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::Stop
// --------------------------------------------------------------------------
//  
void CUpnpImagePlayer::Stop()
    {
    if( iRendSession )
        {
        iImageState = EImageIdle;
        iFirstStart = ETrue;
        TRAP_IGNORE( iRendSession->StopL() );
        iRendSession->RemoveObserver();
        iAVControl.StopRenderingSession( *iRendSession );
        iRendSession = NULL;      
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::InteractOperationComplete
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::InteractOperationComplete( 
              TInt aError,
              TUPnPAVInteractOperation aOperation 
              )
    {
    __LOG2( "CUpnpImagePlayer::InteractOperationComplete: %d, %d",
        aError, aOperation );
    TRAP_IGNORE( InteractOperationCompleteL( aError, aOperation ) );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::InteractOperationCompleteL
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::InteractOperationCompleteL( 
              TInt aError,
              TUPnPAVInteractOperation aOperation 
              )
    {
    iCommonUI.HandleCommonErrorL( aError, NULL );
    
    switch( aOperation )
        {
        case EUPnPAVPlay:
            {
            if( iFirstStart )
                {
                iFirstStart = EFalse;    
                }
            
            if( iImageState == EImageNext ) //if still there is a pending item
                {                           //stop current showing item
                iRendSession->StopL();
                }
            else
                {
                iImageState = EImageIdle;
                }
            }
            break;
        case EUPnPAVStop:
            {
            if( iUri && iImageState == EImageNext )
                {
                iAction = CUPnPCommonUI::EUPnPShow;
                iRendSession->SetURIL( *iUri, *iItem );
                iImageState = EImageInitialising;
                delete iUri; iUri = NULL;    
                }
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetURIResult
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::SetURIResult( TInt aError )
    {
    __LOG1( "CUpnpImagePlayer::SetURIResult: %d", aError );
    
    TRAP_IGNORE( SetURIResultL( aError ) );
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::SetURIResultL
// --------------------------------------------------------------------------
//   
void CUpnpImagePlayer::SetURIResultL( TInt aError )
    {
    iCommonUI.GetUpnpAction( iAction );
    iCommonUI.HandleCommonErrorL( aError, NULL );
    if( iImageState == EImageNext ) //if there is another pending item
        {
        if( iUri )
            {
            iRendSession->SetURIL( *iUri, *iItem ); //not possible to leave
            iImageState = EImageInitialising;
            delete iUri; iUri = NULL;    
            }
        else
            {
            iImageState = EImageIdle;
            iCommonUI.GetUpnpAction( iAction );
            iCommonUI.HandleCommonErrorL( KErrNotFound, NULL );            
            }
        }
    else //if no pending item
        {
        if( KErrNone == aError )
            {
            iRendSession->PlayL(); //not possible to leave
            iImageState = EImageShowing;
            }
        else
            {
            iImageState = EImageIdle;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDevice::PlayL
// --------------------------------------------------------------------------
//
void CUpnpImagePlayer::MediaRendererDisappeared( 
                                 TUPnPDeviceDisconnectedReason aReason )
    {
    if( aReason == EDisconnected )
        {
        iDialog.DeviceDisappeared( KErrSessionClosed );
        }
    
    }
    
// End of File

