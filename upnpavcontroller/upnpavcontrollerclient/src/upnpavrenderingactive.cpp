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
* Description:      AO for rendering commands
*
*/






#include "upnpavrenderingactive.h"

#include "upnpavcontrollerclient.h"
#include "upnpavrenderingsessionimpl.h"
#include "upnpavrequest.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

const TInt KPositionInfoSize = 15;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::NewL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
CUPnPAVRenderingActive* CUPnPAVRenderingActive::NewL(
    RUPnPAVControllerClient& aServer, TInt aId )
    {
    CUPnPAVRenderingActive* self = new (ELeave) CUPnPAVRenderingActive(
        aServer, aId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::CUPnPAVRenderingActive
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
CUPnPAVRenderingActive::CUPnPAVRenderingActive(
    RUPnPAVControllerClient& aServer, TInt aId ):
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iId( aId ),
    iBufferPtr( 0, 0 ),
    iBufferPtr2( 0, 0 ),
    iRespBufSizePkg( iRespBufSize )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::~CUPnPAVRenderingActive
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
CUPnPAVRenderingActive::~CUPnPAVRenderingActive()
    {
    __LOG( "CUPnPAVRenderingActive::~CUPnPAVRenderingActive" );
    
    Cancel();
    
    delete iBuffer;
    delete iBuffer2;

    if( iMediaServerResourceReserved )
        {
        iServer.ReleaseMediaServer( iId );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ConstructL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ConstructL()
    {  
    __LOG( "CUPnPAVRenderingActive::ConstructL" );  
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::RunL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::RunL()
    {
    __LOG( "CUPnPAVRenderingActive::RunL" );
    
    switch( iPendingOperation )
        {
        case ESetURI:
            {
            SetURICompleteL();
            }
            break;

        case ESetNextURI:
            {
            SetNextURICompleteL();
            }
            break;

        case EPlay:
            {
            PlayCompleteL();
            }
            break;

        case EStop:
            {
            StopCompleteL();
            }
            break;

        case EPause:
            {
            PauseCompleteL();
            }
            break;

        case ESetVolume:
            {
            SetVolumeCompleteL();
            }
            break;

        case EGetVolume:
            {
            GetVolumeCompleteL();
            }
            break;

        case ESetMute:
            {
            SetMuteCompleteL();
            }
            break;

        case EGetMute:
            {
            GetMuteCompleteL();
            }
            break;

        case EPositionInfo:
            {
            PositionInfoCompleteL();
            }
            break;
            
        case EStartMediaServer:
            {
            StartMediaServerCompleteL();
            }
            break;
            
        default:
            __PANICD( __FILE__, __LINE__ );
            break;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::DoCancel
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::DoCancel()
    {
    __LOG( "CUPnPAVRenderingActive::DoCancel" );
    
    switch( iPendingOperation )
        {
        case ESetURI:
            {
            iServer.CancelSetURI( iId );
            }
            break;

        case ESetNextURI:
            {
            iServer.CancelSetNextURI( iId );
            }
            break;

        case EPlay:
            {
            iServer.CancelPlay( iId );
            }
            break;

        case EStop:
            {
            iServer.CancelStop( iId );
            }
            break;

        case EPause:
            {
            iServer.CancelPause( iId );
            }
            break;

        case ESetVolume:
            {
            iServer.CancelSetVolume( iId );
            }
            break;

        case EGetVolume:
            {
            iServer.CancelGetVolume( iId );
            }
            break;

        case ESetMute:
            {
            iServer.CancelSetMute( iId );
            }
            break;

        case EGetMute:
            {
            iServer.CancelGetMute( iId );
            }
            break;

        case EPositionInfo:
            {
            iServer.CancelGetPositionInfo( iId );
            }
            break;

        case EStartMediaServer:
            {
            iServer.CancelReserveMediaServer( iId );
            }
            break;          
                       
        default:
            __PANICD( __FILE__, __LINE__ );
            break;            
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::RunError
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
TInt CUPnPAVRenderingActive::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetObserver
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetObserver( 
                                 MUPnPAVRenderingSessionObserver& aObserver )
    {
    // Set the observer and make an async request to the server to receive
    // device callbacks
    __ASSERTD( !iObserver, __FILE__, __LINE__ );

        
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::Observer
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
MUPnPAVRenderingSessionObserver* CUPnPAVRenderingActive::Observer() const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::RemoveObserver
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::RemoveObserver()
    {
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ReserveLocalMSServicesL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ReserveLocalMSServicesL()
    {
    __LOG( "CUPnPAVRenderingActive::ReserveLocalMSServicesL" );
    
    ResetL();
    
    if( iMediaServerResourceReserved )
        {
        if( iObserver )
            {
            iObserver->ReserveLocalMSServicesCompleted( KErrNone );
            }
        }
    else
        {
        iPendingOperation = EStartMediaServer;
        iServer.ReserveMediaServer( iId, iStatus );
        SetActive();    
        
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::CancelReserveLocalMSServicesL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::CancelReserveLocalMSServicesL()
    {
    __LOG( "CUPnPAVRenderingActive::CancelReserveLocalMSServicesL" );
    
    if( iPendingOperation == EStartMediaServer )
        {
        Cancel();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ReleaseLocalMSServicesL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ReleaseLocalMSServicesL()
    {
    __LOG( "CUPnPAVRenderingActive::ReleaseLocalMSServicesL" );
    
    ResetL();
    
    if( iMediaServerResourceReserved )
        {
        iMediaServerResourceReserved = EFalse;
        
        User::LeaveIfError( iServer.ReleaseMediaServer( iId ) );        
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetURIL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetURIL( const TDesC8& aURI,
    const CUpnpItem& aItem )
    {
    __LOG( "CUPnPAVRenderingActive::SetURIL" );
    
    ResetL();
        
    CUpnpAVRequest* tempRequest = CUpnpAVRequest::NewLC();
    tempRequest->SetURIL( aURI );
    
    iBuffer = tempRequest->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    iBuffer2 = aItem.ToDes8L();
    iBufferPtr2.Set( iBuffer2->Des() );
    
    CleanupStack::PopAndDestroy( tempRequest );

    iPendingOperation = ESetURI;
        
    iServer.SetURI( iId, iBufferPtr, iBufferPtr2, iStatus );
    SetActive();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetNextURIL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetNextURIL( const TDesC8& aURI,
    const CUpnpItem& aItem )    
    {
    __LOG( "CUPnPAVRenderingActive::SetNextURIL" );
    
    ResetL();
    
    CUpnpAVRequest* tempRequest = CUpnpAVRequest::NewLC();
    tempRequest->SetURIL( aURI );
    
    iBuffer = tempRequest->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );

    iBuffer2 = aItem.ToDes8L();
    iBufferPtr2.Set( iBuffer2->Des() );
    
    CleanupStack::PopAndDestroy( tempRequest );
            
    iPendingOperation = ESetNextURI;
    
    iServer.SetNextURI( iId, iBufferPtr, iBufferPtr2, iStatus );
    SetActive();        
    }


// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::PlayL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::PlayL()
    {
    __LOG( "CUPnPAVRenderingActive::PlayL" );
    
    ResetL();
    iPendingOperation = EPlay;
    iServer.Play( iId, iStatus );
    SetActive();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::StopL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::StopL()
    {
    __LOG( "CUPnPAVRenderingActive::StopL" );
    
    ResetL();
    iPendingOperation = EStop;
    iServer.Stop( iId, iStatus );
    SetActive();    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::PauseL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::PauseL()
    {
    __LOG( "CUPnPAVRenderingActive::PauseL" );
    
    ResetL();
    iPendingOperation = EPause;
    iServer.Pause( iId, iStatus );
    SetActive();    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetVolumeL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetVolumeL( TInt aVolumeLevel )
    {
    __LOG( "CUPnPAVRenderingActive::SetVolumeL" );
    
    ResetL();
    iPendingOperation = ESetVolume;
    iServer.SetVolume( iId, aVolumeLevel, iRespBufSizePkg, iStatus );
    SetActive();
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::GetVolumeL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::GetVolumeL()
    {
    __LOG( "CUPnPAVRenderingActive::GetVolumeL" );
    
    ResetL();
    iPendingOperation = EGetVolume;
    iServer.GetVolume( iId, iRespBufSizePkg, iStatus );
    SetActive();    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetMuteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetMuteL( TBool aMute )
    {
    __LOG( "CUPnPAVRenderingActive::SetMuteL" );
    
    ResetL();
    iPendingOperation = ESetMute;
    iServer.SetMute( iId, aMute, iRespBufSizePkg, iStatus );
    SetActive();        
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::NewL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::GetMuteL()
    {
    __LOG( "CUPnPAVRenderingActive::GetMuteL" );
    
    ResetL();
    iPendingOperation = EGetMute;
    iServer.GetMute( iId, iRespBufSizePkg, iStatus );
    SetActive();            
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::GetPositionInfoL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::GetPositionInfoL()
    {
    __LOG( "CUPnPAVRenderingActive::GetPositionInfoL" );
    
    ResetL();
  
    iBuffer = HBufC8::NewL( KPositionInfoSize );
    iBufferPtr.Set( iBuffer->Des() );
    iBuffer2 = HBufC8::NewL( KPositionInfoSize );
    iBufferPtr2.Set( iBuffer2->Des() );
    iPendingOperation = EPositionInfo;
    iServer.GetPositionInfo( iId, iBufferPtr, iBufferPtr2, iStatus );
    SetActive();                
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ResetL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ResetL()
    {
    __LOG( "CUPnPAVRenderingActive::ResetL" );
    
    if( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }

    delete iBuffer; iBuffer = NULL;
    delete iBuffer2; iBuffer2 = NULL;
    iPendingOperation = ENone;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetURICompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetURICompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::SetURICompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerSetURICompleted )
            {
            iObserver->SetURIResult( KErrNone );    
            }
        else
            {
            iObserver->SetURIResult( iStatus.Int() );    
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetNextURICompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetNextURICompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::SetNextURICompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerSetNextURICompleted )
            {
            iObserver->SetNextURIResult( KErrNone );    
            }
        else
            {
            iObserver->SetNextURIResult( iStatus.Int() );    
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::PlayCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::PlayCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::PlayCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerPlayCompleted )
            {
            iObserver->InteractOperationComplete( KErrNone, EUPnPAVPlay );    
            }
        else
            {
            iObserver->InteractOperationComplete( iStatus.Int(), 
                                                  EUPnPAVPlay ); 
            }
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::StopCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::StopCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::StopCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerStopCompleted )
            {
            iObserver->InteractOperationComplete( KErrNone, EUPnPAVStop );    
            }
        else
            {
            iObserver->InteractOperationComplete( iStatus.Int(), 
                                                  EUPnPAVStop ); 
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::PauseCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::PauseCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::PauseCompleteL" );

    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerPauseCompleted )
            {
            iObserver->InteractOperationComplete( KErrNone,
                                                  EUPnPAVPause );
            }
        else
            {
            iObserver->InteractOperationComplete( iStatus.Int(), 
                                                  EUPnPAVPause );    
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetVolumeCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetVolumeCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::SetVolumeCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerSetVolumeCompleted )
            {
            iObserver->VolumeResult( KErrNone, iRespBufSize, ETrue );    
            }
        else
            {
            iObserver->VolumeResult( iStatus.Int(), 0, ETrue );
            }
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::GetVolumeCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::GetVolumeCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::GetVolumeCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerGetVolumeCompleted )
            {
            iObserver->VolumeResult( KErrNone, iRespBufSize, ETrue );    
            }
        else
            {
            iObserver->VolumeResult( iStatus.Int(), 0, ETrue );
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::SetMuteCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetMuteCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::SetMuteCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerSetMuteCompleted )
            {
            iObserver->MuteResult( KErrNone, (TBool)iRespBufSize, ETrue );    
            }
        else
            {
            iObserver->MuteResult( iStatus.Int(), 0, ETrue );
            }
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::GetMuteCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::GetMuteCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::GetMuteCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerGetMuteCompleted )
            {
            iObserver->MuteResult( KErrNone, iRespBufSize, ETrue );    
            }
        else
            {
            iObserver->MuteResult( iStatus.Int(), 0, ETrue );
            }
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::PositionInfoCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::PositionInfoCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::PositionInfoCompleteL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerPositionInfoCompleted )
            {
            iObserver->PositionInfoResult( KErrNone, *iBuffer2, *iBuffer );  
            }
        else
            {
            iObserver->PositionInfoResult( iStatus.Int(), 
                                           KNullDesC8, 
                                           KNullDesC8 );
            }
        }            
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::StartMediaServerCompleteL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::StartMediaServerCompleteL()
    {
    __LOG( "CUPnPAVRenderingActive::StartMediaServerCompleteL" );
    
    if( iObserver )
        {

        if( iStatus.Int() == EAVControllerStartMediaServerCompleted )
            {
            iMediaServerResourceReserved = ETrue;
            iObserver->ReserveLocalMSServicesCompleted( KErrNone );
            }
        else
            {
            iObserver->ReserveLocalMSServicesCompleted( iStatus.Int() );
            }
        
        }
    }    
// end of file
