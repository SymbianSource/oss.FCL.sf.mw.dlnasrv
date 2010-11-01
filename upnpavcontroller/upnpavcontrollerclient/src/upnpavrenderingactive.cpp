/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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





#include <utf.h>

#include "upnpavrenderingactive.h"

#include "upnpavcontrollerclient.h"
#include "upnpavrenderingsessionimpl.h"
#include "upnpavrequest.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

const TInt KPositionInfoSize = 15;
static const TInt KSeekTargetMaxLength = 8; // "00:00:20" -> 8

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
    __LOG1( "CUPnPAVRenderingActive::~CUPnPAVRenderingActive %d", this );
    
    Cancel();
    
    delete iBuffer;
    delete iBuffer2;
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ConstructL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ConstructL()
    {  
    __LOG1( "CUPnPAVRenderingActive::ConstructL %d", this );  
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::RunL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::RunL()
    {
    __LOG1( "CUPnPAVRenderingActive::RunL %d", this );
    
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
            
        case ESeekRelTime:
            {
            SeekRelTimeComplete();
            }
            break;
            
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            }
            break;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::DoCancel
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::DoCancel()
    {
    __LOG1( "CUPnPAVRenderingActive::DoCancel %d", this );
    
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
                       
        case ESeekRelTime:
            {
            iServer.CancelSeekRelTime( iId );
            }
            break;
                       
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            }
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
// CUPnPAVRenderingActive::SetURIL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SetURIL( const TDesC8& aURI,
    const CUpnpItem& aItem )
    {
    __LOG1( "CUPnPAVRenderingActive::SetURIL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetNextURIL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::PlayL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::StopL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::PauseL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetVolumeL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::GetVolumeL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetMuteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::GetMuteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::GetPositionInfoL %d", this );
    
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
// CUPnPAVRenderingActive::SeekRelTimeL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SeekRelTimeL( const TTime& aDesiredTime ) 
    {
    __LOG1( "CUPnPAVRenderingActive::SeekRelTimeL %d", this );
    
    ResetL();
  
    _LIT16( KHourMinSecFormatString, "%:0%*H%:1%T%:2%S%:3" ); //Current value equals to R_QTN_TIME_DURAT_LONG.
    TBuf<KSeekTargetMaxLength> desiredTimeString;
    aDesiredTime.FormatL( desiredTimeString, KHourMinSecFormatString );

    iBuffer = CnvUtfConverter::ConvertFromUnicodeToUtf8L( desiredTimeString );
    iBufferPtr.Set( iBuffer->Des() );
    iPendingOperation = ESeekRelTime;
    iServer.SeekRelTime( iId, iBufferPtr, iStatus );
    SetActive();   
    }

// --------------------------------------------------------------------------
// CUPnPAVRenderingActive::ResetL
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::ResetL()
    {
    __LOG1( "CUPnPAVRenderingActive::ResetL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetURICompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetNextURICompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::PlayCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::StopCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::PauseCompleteL %d", this );

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
    __LOG1( "CUPnPAVRenderingActive::SetVolumeCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::GetVolumeCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::SetMuteCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::GetMuteCompleteL %d", this );
    
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
    __LOG1( "CUPnPAVRenderingActive::PositionInfoCompleteL %d", this );
    
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
// CUPnPAVRenderingActive::SeekRelTimeComplete
// See upnpavrenderingactive.h
// --------------------------------------------------------------------------
void CUPnPAVRenderingActive::SeekRelTimeComplete()
    {
    __LOG1( "CUPnPAVRenderingActive::SeekRelTimeCompleteL %d", this );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerSeekCompleted )
            {
            iObserver->InteractOperationComplete( KErrNone, EUPnPAVSeek );    
            }
        else
            {
            iObserver->InteractOperationComplete( iStatus.Int(),EUPnPAVSeek); 
            }
        }
    }

// end of file
