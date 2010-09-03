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
* Description:      implements a renderer playback state machine
*
*/







// INCLUDE FILES
// System
#include <in_sock.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// upnp stack api
#include <upnpdevice.h>
#include <upnpcontainer.h>
#include <upnpservice.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpstring.h>
#include <upnpmediaserverclient.h>
#include <upnpdlnaprotocolinfo.h>
#include <upnpavcontrolpoint.h>

// upnpframework / xmlparser api
#include "upnpxmlparser.h"

// upnpframework / avcontroller api
#include "upnpavcontrollerglobals.h"

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"
#include "upnpconstantdefs.h" // for upnp-specific stuff

// upnpframework / internal api's
#include "upnpmetadatafetcher.h"
#include "upnpcommonutils.h"
#include "upnpcdsreselementutility.h"
#include "upnpxmleventparser.h"

// avcontroller internal
#include "upnpavcontrollerserver.h"
#include "upnpavrequest.h"
#include "upnpfilesharingactive.h"
#include "upnpavdispatcher.h"
#include "upnpaverrorhandler.h"
#include "upnpavdeviceextended.h"
#include "upnpdevicerepository.h"
#include "upnpplaybacksession.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

_LIT8( KPlaying,            "PLAYING" );
_LIT8( KPaused,             "PAUSED_PLAYBACK" );
_LIT8( KStopped,            "STOPPED" );
_LIT8( KNormalSpeed,        "1" );
_LIT8( KMasterVolume,       "Master" );
_LIT8( KMuteOn,             "1" );
_LIT8( KMuteOff,            "0" );
_LIT8( KAsterisk,           "*" );

const TInt KDefaultInstanceId   = 0;
const TInt KExpectedCount       = 1;
const TInt KMaxVolume           = 100;

// Timer to wait until sending the play action after set transport uri.
// For some reason, some equipments can not responce play action 
// immediately after set transport uri, eg. Kiss 1600.  
const TInt KPlayDelayTimerInterval = 1000000;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::NewL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
CUPnPPlaybackSession* CUPnPPlaybackSession::NewL
    (
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer,
    TInt aSessionId,
    const TDesC8& aUuid
    )
    {
    CUPnPPlaybackSession* self = new (ELeave) CUPnPPlaybackSession(
        aClient, aServer, aSessionId );
    CleanupStack::PushL( self );
    self->ConstructL( aUuid );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CUPnPPlaybackSession
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
CUPnPPlaybackSession::CUPnPPlaybackSession
    (
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer,
    TInt aSessionId
    ):
    iServer( aServer ),
    iMediaServer( aClient ),
    iSessionId( aSessionId ),
    iInstanceId( KDefaultInstanceId ),
    iIPSessionIdCommand( KErrNotFound ),
    iIPSessionIdSetting( KErrNotFound ),
    iEventingActive( EFalse ),
    iMuteState( EUnknown ),
    iVolume( -1 )
    {
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::~CUPnPPlaybackSession
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
CUPnPPlaybackSession::~CUPnPPlaybackSession()
    {
    __LOG( "CUPnPPlaybackSession::~CUPnPPlaybackSession" );
    
    if( iPlaybackState == EPlaying || iPlaybackState == EPaused ||
        iPlaybackState == EPlaySent )
        {
        if( iDevice && iServer.DeviceRepository().IsWlanActive() )
            {
            __LOG( "~CUPnPPlaybackSession - \
playback still ongoing, send stop" );    
            TRAP_IGNORE( iServer.ControlPoint().AvtStopActionL( 
                iDevice->Uuid(), iInstanceId ) );            
            }
        else
            {
            __LOG( "~CUPnPPlaybackSession - \
playback still ongoing, wlan not active" );    
            }    
        }
        
    if( iSharedItem && iItemShared && iFileSharing )
        {
        TRAP_IGNORE( iFileSharing->UnShareItemL( iSharedItem->Id() ) );
        }

    if( iNextSharedItem && iNextItemShared && iFileSharing )
        {
        TRAP_IGNORE( iFileSharing->UnShareItemL( iNextSharedItem->Id() ) );
        }
    delete iFileSharing;
        
    //iMediaServer.Close();

    delete iSharedItem;
    delete iNextSharedItem;

    delete iEventMessage;
    delete iSettingMessage;
    delete iCommandMessage;
    delete iDeviceMessage;
    
    delete iLocalMediaServerUuid;
    delete iEventParser;
    
    iEventQue.Reset();
    iEventQue.Close();

    if( iEventingActive && iDevice )
        {
        __LOG( "~CUPnPPlaybackSession - UnSubscribeDeviceL" );
        TRAP_IGNORE( iServer.DeviceRepository().UnSubscribeDeviceL(
            iDevice->Uuid() ) );
        iServer.Dispatcher().UnRegisterEvents( *this );
        iEventingActive = EFalse;            
        }
        
    delete iDevice;        
    
    // delete the playdelay timer
    if( iPlayDelayTimer )
        {
        iPlayDelayTimer->Cancel();
        delete iPlayDelayTimer;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ConstructL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPPlaybackSession::ConstructL" );

    iFileSharing = CUPnPFileSharingActive::NewL();
    
    iEventParser = CUPnPXMLEventParser::NewL();
    
    const RPointerArray<CUpnpAVDeviceExtended>& devList =
        iServer.DeviceRepository().DeviceList();
    TInt count = devList.Count();
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( devList[ i ]->Local() )
            {
            __LOG( "CUPnPPlaybackSession::ConstructL - Local MS found!" );
            
            __ASSERTD( !iLocalMediaServerUuid, __FILE__, __LINE__ );
            iLocalMediaServerUuid = devList[i]->Uuid().AllocL();
            }
        if( devList[ i ]->Uuid() == aUuid )
            {
            __ASSERTD( !iDevice, __FILE__, __LINE__ );
            iDevice = CUpnpAVDeviceExtended::NewL( *devList[ i ] );
            }
        }        
    if( !iDevice )
        {
        User::Leave( KErrNotFound );
        }

    // create the playdelaytimer
    iPlayDelayTimer = CUPnPPeriodic::NewL( CActive::EPriorityStandard );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcSetVolumeResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcSetVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& aDesiredVolume )
    {
    __LOG1( "CUPnPPlaybackSession::RcSetVolumeResponse: %d", aErr );
    
    __ASSERTD( iIPSessionIdSetting == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
    //iSettingPending = EFalse;
    iIPSessionIdSetting = KErrNotFound;
    
    if( iSettingMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPRenderingControlError );    
        
        if( aErr == KErrNone )
            {
            TInt vol;
            TLex8 lex( aDesiredVolume );
            TInt err = lex.Val( vol );
            if(  err == KErrNone )
                {
                TInt maxVolume = iDevice->MaxVolume();
                // If max volume not KMaxVolume
                if( maxVolume != KMaxVolume )
                    {
                    // Convert volume to match max volume 100
                    TReal tempVolumeLevel = vol;
                    TReal tempMaxVolume = maxVolume;
                       
                    vol = KMaxVolume * tempVolumeLevel / tempMaxVolume;
                    }
                iVolume = vol;
                TPckg<TInt> resp2( vol );
                iSettingMessage->Write( 2, resp2 );

                iSettingMessage->Complete( EAVControllerSetVolumeCompleted );
                delete iSettingMessage; iSettingMessage = NULL;       
                }
            else
                {
                iSettingMessage->Complete( err );
                delete iSettingMessage; iSettingMessage = NULL;
                }    
            }
        else
            {
            iSettingMessage->Complete( aErr );
            delete iSettingMessage; iSettingMessage = NULL;
            }                   
        }
    else
        {
        __LOG( "RcSetVolumeResponse - no msg" );
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcVolumeResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& aCurrentVolume)
    {
    __LOG1( "CUPnPPlaybackSession::RcVolumeResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdSetting == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
    //iSettingPending = EFalse;
    iIPSessionIdSetting = KErrNotFound;

    if( iSettingMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPRenderingControlError );
        
        if( aErr == KErrNone )
            {
            TInt vol;
            TLex8 lex( aCurrentVolume );
            TInt err = lex.Val( vol );
            if(  err == KErrNone )
                {
                
               // Get device's maximum volume value
                TInt maxVolume = iDevice->MaxVolume();

                // If max volume not KMaxVolume
                if( maxVolume != KMaxVolume )
                    {
                    // Convert volume to match max volume KMaxVolume
                    TReal tempVolumeLevel = vol;
                    TReal tempMaxVolume = maxVolume;
                    
                    vol = KMaxVolume * tempVolumeLevel / tempMaxVolume;
                    }            
                iVolume = vol;
                TPckg<TInt> resp1( vol );
                iSettingMessage->Write( 1, resp1 );

                iSettingMessage->Complete( EAVControllerGetVolumeCompleted );
                delete iSettingMessage; iSettingMessage = NULL;      
                }
            else
                {
                iSettingMessage->Complete( err );
                delete iSettingMessage; iSettingMessage = NULL;      
                }    
            }
        else
            {
            iSettingMessage->Complete( aErr );
            delete iSettingMessage; iSettingMessage = NULL;      
            }                               
        }
    else
        {
        __LOG( "RcVolumeResponse - no msg" );
        }    

    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcSetMuteResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcSetMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& aDesiredMute )
    {
    __LOG1( "CUPnPPlaybackSession::RcSetMuteResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdSetting == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
    //iSettingPending = EFalse;
    iIPSessionIdSetting = KErrNotFound;
    
    if( iSettingMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPRenderingControlError );    
        
        if( aErr == KErrNone )
            {
            TInt mute( EUnknown );
            TLex8 lex( aDesiredMute );
            TInt err = lex.Val( mute );
            
            // If mute's value isn't ENotMuted or EMuted, 
            // we think the value is incorrect.
            if ( err == KErrNone && mute != ENotMuted && mute != EMuted )
                {
                err = KErrArgument;
                }

            if(  err == KErrNone )
                {
                iMuteState = (TMuteState)mute;
                TPckg<TInt> resp2( mute );
                iSettingMessage->Write( 2, resp2 );

                iSettingMessage->Complete( EAVControllerSetMuteCompleted );
                delete iSettingMessage; iSettingMessage = NULL;            
                }
            else
                {
                iSettingMessage->Complete( err );
                delete iSettingMessage; iSettingMessage = NULL;      
                }    
            }
        else
            {
            iSettingMessage->Complete( aErr );
            delete iSettingMessage; iSettingMessage = NULL;      
            }                           
        }
    else
        {
        __LOG( "RcSetMuteResponse - no msg" );
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcMuteResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& aCurrentMute )
    {
    __LOG1( "CUPnPPlaybackSession::RcMuteResponse: %d" , aErr );
    
    __ASSERTD( iIPSessionIdSetting == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
    iIPSessionIdSetting = KErrNotFound;

    if( iSettingMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPRenderingControlError );
        
        if( aErr == KErrNone )
            {
            TInt mute( EUnknown );
            TLex8 lex( aCurrentMute );
            TInt err = lex.Val( mute );

            // If mute's value isn't ENotMuted or EMuted, 
            // we think the value is incorrect.
            if ( err == KErrNone && mute != ENotMuted && mute != EMuted )
                {
                err = KErrArgument;
                }

            if(  err == KErrNone )
                {
                iMuteState = (TMuteState)mute;
                TPckg<TInt> resp1( mute );
                iSettingMessage->Write( 1, resp1 );

                iSettingMessage->Complete( EAVControllerGetMuteCompleted );
                delete iSettingMessage; iSettingMessage = NULL;
                }
            else
                {
                iSettingMessage->Complete( err );
                delete iSettingMessage; iSettingMessage = NULL;
                }    
            }
        else
            {
            iSettingMessage->Complete( aErr );
            delete iSettingMessage; iSettingMessage = NULL;
            }                                   
        }
    else
        {
        __LOG( "RcMuteResponse - no msg" );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSetTransportUriResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSetTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& /*aCurrentUri*/,
    const TDesC8& /*aCurrentUriMetaData*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtSetTransportUriResponse: %d", aErr );
    
    __ASSERTD( iIPSessionIdCommand == aSessionId, __FILE__, __LINE__ );  
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    TInt temp;
    TLex8 lex( aInstanceId );
    TInt err = lex.Val( temp );
    if( err == KErrNone )
        {
        __LOG1( "AvtSetTransportUriResponse, instance id: %d", temp );
        iInstanceId = temp;
        iPlaybackState = EStopped;
        }
    
    if( iCommandMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );    
        
        if( aErr == KErrNone )
            {
            iCommandMessage->Complete( EAVControllerSetURICompleted );
            delete iCommandMessage; iCommandMessage = NULL;      
            }
        else
            {
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;      
            }                
        }
    else
        {
        __LOG( "AvtSetTransportUriResponse - no msg" );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSetNextTransportUriResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSetNextTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& /*aNextUri*/,
    const TDesC8& /*aNextUriMetaData*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtSetNextTransportUriResponse: %d",
        aErr );
    
    __ASSERTD( iIPSessionIdCommand == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;

    TInt temp;
    TLex8 lex( aInstanceId );
    TInt err = lex.Val( temp );
    if( err == KErrNone )
        {
        __LOG1( "AvtSetNextTransportUriResponse, instance id: %d", temp );
        //iInstanceId = temp;
        }

    if( iCommandMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );
        
        if( aErr == KErrNone )
            {
            iCommandMessage->Complete( EAVControllerSetNextURICompleted );
            delete iCommandMessage; iCommandMessage = NULL;            
            }
        else
            {
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;      
            }            
        }
    else
        {
        __LOG( "AvtSetNextTransportUriResponse - no msg" );
        }    

    }
  
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtMediaInfoResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtMediaInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNrTracks*/,
    const TDesC8& /*aMediaDuration*/,
    const TDesC8& /*aCurrentUri*/,
    const TDesC8& /*aCurrentUriMetaData*/,
    const TDesC8& /*aNextUri*/,
    const TDesC8& /*aNextUriMetaData*/,
    const TDesC8& /*aPlayMedium*/,
    const TDesC8& /*aRecordMedium*/,
    const TDesC8& /*aWriteStatus*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtGetTransportInfoResponse
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPPlaybackSession::AvtGetTransportInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aCurrenTransportState*/,
    const TDesC8& /*aCurrentTransportStatus*/,
    const TDesC8& /*aCurrentSpeed*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPositionInfoResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPositionInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aTrack*/,
    const TDesC8& aTrackDuration,
    const TDesC8& /*aTrackMetaData*/,
    const TDesC8& /*aTrackURI*/,
    const TDesC8& aRelTime,
    const TDesC8& /*aAbsTime*/,
    const TDesC8& /*aRelCount*/,
    const TDesC8& /*aAbsCount*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtPositionInfoResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdSetting == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
    iIPSessionIdSetting = KErrNotFound;
    
    if( iSettingMessage )
        {
        TInt err = iSettingMessage->Write( 1, aTrackDuration );
        err = iSettingMessage->Write( 2, aRelTime );
        // Howto handle err?

        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );    
           
        if( aErr == KErrNone )
            {
            iSettingMessage->Complete( EAVControllerPositionInfoCompleted );
            delete iSettingMessage; iSettingMessage = NULL;      
            }
        else
            {
            iSettingMessage->Complete( aErr ); 
            delete iSettingMessage; iSettingMessage = NULL; 
            }        
        }
    else
        {
        __LOG( "AvtPositionInfoResponse - no msg" );    
        }       
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtDeviceCapabilitiesResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtDeviceCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMedia*/,
    const TDesC8& /*aRecMedia*/,
    const TDesC8& /*aRecQualityMode*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtTransportSettingsResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtTransportSettingsResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMode*/,
    const TDesC8& /*aRecQualityMode*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtStopResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtStopResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aInstanceId*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtStopResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdCommand == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    //iCommandPending = EFalse;
    iIPSessionIdCommand = KErrNotFound;
    
    if( iCommandMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );        
        
        if( aErr == KErrNone )
            {
            iPlaybackState = EStopped;
            iCommandMessage->Complete( EAVControllerStopCompleted );
            delete iCommandMessage; iCommandMessage = NULL;       
            }
        else
            {
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;       
            }                
        }
    else
        {
        __LOG( "AvtStopResponse - no msg" );
        }      
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPlayResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPlayResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aSpeed*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtPlayResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdCommand == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    //iCommandPending = EFalse;
    iIPSessionIdCommand = KErrNotFound;
    
    if( iCommandMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );        
        
        if( aErr == KErrNone )
            {
            iPlaybackState = EPlaying;
            iCommandMessage->Complete( EAVControllerPlayCompleted );
            delete iCommandMessage; iCommandMessage = NULL;             
            }
        else
            {
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;
            }                    
        }
    else
        {
        __LOG( "AvtPlayResponse - no msg" );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPauseResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPauseResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aInstanceId*/)
    {
    __LOG1( "CUPnPPlaybackSession::AvtPauseResponse: %d", aErr );    
    
    __ASSERTD( iIPSessionIdCommand == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    //iCommandPending = EFalse;
    iIPSessionIdCommand = KErrNotFound;
    
    if( iCommandMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );        
        
        if( aErr == KErrNone )
            {
            iPlaybackState = EPaused;
            iCommandMessage->Complete( EAVControllerPauseCompleted );
            delete iCommandMessage; iCommandMessage = NULL;                   
            }
        else
            {
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;             
            }                    
        }
    else
        {
        __LOG( "AvtPauseResponse - no msg" );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtRecordResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtRecordResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSeekResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSeekResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aUnit*/,
    const TDesC8& /*aTarget*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtNextResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtNextResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPreviousResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPreviousResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSetPlayModeResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSetPlayModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewPlayMode*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSetRecordModeResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSetRecordModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewRecordQuality*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsSearchCapabilitiesResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsSearchCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSearchCaps*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsSortCapabilitiesResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsSortCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSortCaps*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsSystemUpdateIdResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsSystemUpdateIdResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aSystemUpdateId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsBrowseResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsBrowseResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectID*/,
    const TDesC8&  /*aBrowseFlag*/,
    const TDesC8&  /*aFilter*/,
    TInt /*aIndex*/,
    TInt /*aRequest*/,
    const TDesC8&  /*aSortCriteria*/,
    const TDesC8&  /*aResult*/,
    TInt /*aReturned*/,
    TInt /*aMatches*/,
    const TDesC8&  /*aUpdateID*/ )
    {
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsSearchResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsSearchResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerId*/,
    const TDesC8& /*aSearchCriteria*/,
    const TDesC8& /*aFilter*/,
    TInt /*aIndex*/,
    TInt /*aRequest*/,
    const TDesC8& /*aSortCriteria*/,
    const TDesC8& /*aResult*/,
    TInt /*aReturned*/,
    TInt /*aMatches*/,
    const TDesC8& /*aUpdateID*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsDestroyObjectResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsDestroyObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectId*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsUpdateObjectResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsUpdateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectId*/,
    const TDesC8& /*aCurrentTagValue*/,
    const TDesC8& /*aNewTagValue*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsImportResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsImportResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& /*aTransferId*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsExportResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsExportResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& /*aTransferId*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsStopTransferResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsStopTransferResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aTransferId*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsCTransferProgressResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsCTransferProgressResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aTransferId*/,
    const TDesC8& /*aTransferStatus*/,
    const TDesC8& /*aTransferLength*/,            
    const TDesC8& /*aTransferTotal*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsDeleteResourceResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsDeleteResourceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aResourceUri*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsCreateReferenceResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsCreateReferenceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerId*/, 
    const TDesC8& /*ObjectId*/,
    const TDesC8& /*aNewId*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsCreateObjectResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsCreateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerID*/, 
    const TDesC8& /*aElements*/, 
    const TDesC8& /*aObjectID*/, 
    const TDesC8& /*aResult*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmProtocolInfoResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmProtocolInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSource*/, 
    const TDesC8& /*aSink*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmPrepareResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmPrepareResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aRemoteProtocolInfo*/,
    const TDesC8& /*aPeerConnectionManager*/,
    const TDesC8& /*aPeerConnectionId*/,
    const TDesC8& /*aDirection*/,
    TInt /*aConnection*/,
    TInt /*aTransport*/,
    TInt /*aRsc*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmComplete
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmComplete(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aConnection*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmCurrentConnections
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmCurrentConnections(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aConnections*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmCurrentInfo
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmCurrentInfo(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*rscId*/, 
    TInt /*transportId*/, 
    const TDesC8& /*aProtocolInfo*/,
    const TDesC8& /*aPeerConnectionManager*/, 
    TInt /*peerId*/, 
    const TDesC8& /*aDirection*/, 
    const TDesC8& /*aStatus*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsUpdateEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsUpdateEvent(
        const TDesC8& /*aUuid*/,
        TInt /*aSystemUpdateId*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsContainerEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsContainerEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConteinerIds*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CdsTransferEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CdsTransferEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aTransferIds*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcLastChangeEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& aLastChange
        )
    {
    // No implementation required
    if( iPlaybackState != EUninitialized )
        {        
        __LOG( "CUPnPPlaybackSession::RcLastChangeEvent" );
        //__LOG8( aLastChange );

        TInt instanceId = -1;
        TInt volume = -1;
        TBool mute = EFalse;
        if( iMuteState == EMuted )
            {
            mute = ETrue;
            }

        TRAPD( err, iEventParser->ParseResultDataL( aLastChange, instanceId,
            volume, mute ) );
        if( err == KErrNone && instanceId == iInstanceId )
            {
            TUnsolicitedEventC unEvent;
            if( iMuteState != (TMuteState)mute )
                {
                // State of mute changed, create an event and send it to
                // the client side
                unEvent.iEvent = EMute;
                unEvent.iValue = (TInt)mute;
                
                // If mute's value isn't ENotMuted or EMuted, 
                // we think the value is incorrect.
                if ( mute != ENotMuted && mute != EMuted )
                    {    
                    err = KErrArgument;
                    }
                else 
                    {
                iMuteState = (TMuteState)mute;
                    }
                if( iEventMessage )
                    {
                    TPckg<TUnsolicitedEventC> resp1( unEvent );
                    TInt error = iEventMessage->Write( 1, resp1 );
                    if ( error == KErrNone )
                        {
                        error = err;
                        }
                    iEventMessage->Complete( error );
                    delete iEventMessage; iEventMessage = NULL;
                    }
                else if ( err == KErrNone )
                    {
                    // If iEventMessage is invalid and mute's value is
                    // right, we will append event to iEventQue.
                    // Else nothing to do.
                    iEventQue.AppendL( unEvent );
                    }                            
                }

            // Scale the volume level
            // Get device's maximum volume value
            TInt maxVolume = iDevice->MaxVolume();
            // If max volume not KMaxVolume
            if( maxVolume != KMaxVolume )
                {
                // Convert volume to match max volume 100
                TReal tempVolumeLevel = volume;
                TReal tempMaxVolume = maxVolume;
                   
                volume = KMaxVolume * tempVolumeLevel / tempMaxVolume;
                }            
            
            if( iVolume != volume && volume >= 0 )
                {
                // State of volume changed, create an event and send it to
                // the client side
                unEvent.iEvent = EVolume;
                unEvent.iValue = volume;
                iVolume = volume;
                if( iEventMessage )
                    {
                    TPckg<TUnsolicitedEventC> resp1( unEvent );
                    TInt err = iEventMessage->Write( 1, resp1 );
                    iEventMessage->Complete( err );
                    delete iEventMessage; iEventMessage = NULL;
                    }
                else
                    {
                    iEventQue.AppendL( unEvent );
                    }                            
                }
            
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtLastChangeEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& aLastChange
        )
    {
    // Is it for this device?
    if( iPlaybackState != EUninitialized )
        {
        __LOG( "CUPnPPlaybackSession::AvtLastChangeEvent" );
        
        TUnsolicitedEventC event;
        if( aLastChange.Find( KPlaying ) >= 0 )
            {
            __LOG( "AvtLastChangeEvent - PlayUser received" );
            event.iEvent = EPlay;                            
            iPlaybackState = EPlaying;
            if( iEventMessage )
                {
                TPckg<TUnsolicitedEventC> resp1( event );
                TInt err = iEventMessage->Write( 1, resp1 );
                iEventMessage->Complete( err ); // Ok to complete with err?
                delete iEventMessage; iEventMessage = NULL;
                }
            else
                {
                __LOG( "AvtLastChangeEvent - appending playuser" );
                iEventQue.AppendL( event );
                }            
            }
        else if( aLastChange.Find( KStopped ) >= 0 &&
                 iPlaybackState != EStopped )
            {
            __LOG( "AvtLastChangeEvent - StopUser received" );
            event.iEvent = EStop;                
            iPlaybackState = EStopped;
            if( iEventMessage )
                {
                TPckg<TUnsolicitedEventC> resp1( event );
                TInt err = iEventMessage->Write( 1, resp1 );
                iEventMessage->Complete( err ); // Ok to complete with err?
                delete iEventMessage; iEventMessage = NULL;
                }
            else
                {
                __LOG( "AvtLastChangeEvent - appending stopuser" );
                iEventQue.AppendL( event );
                }    
            }
        else if( aLastChange.Find( KPaused ) >= 0 &&
                 iPlaybackState != EPaused )
            {
            __LOG( "AvtLastChangeEvent - PauseUser received" );
            event.iEvent = EPause;
            iPlaybackState = EPaused;
            if( iEventMessage )
                {
                __LOG( "CUPnPPlaybackSession::AvtLastChangeEvent" );
                TPckg<TUnsolicitedEventC> resp1( event );
                TInt err = iEventMessage->Write( 1, resp1 );
                iEventMessage->Complete( err ); // Ok to complete with err?
                delete iEventMessage; iEventMessage = NULL;
                }
            else
                {
                __LOG( "AvtLastChangeEvent - appending pauseuser" );
                iEventQue.AppendL( event );
                }
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmSourceEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmSourceEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSource*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmSinkEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmSinkEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSink*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmConnectionsEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmConnectionsEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConnections*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::HttpResponseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::HttpResponseL( CUpnpHttpMessage* /*aMessage*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::DeviceDiscoveredL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::DeviceDiscoveredL( CUpnpDevice* /*aDevice*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::DeviceDisappearedL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::DeviceDisappearedL( CUpnpDevice* /*aDevice*/ )
    {
    // No implementation required                   
    }    

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::DeviceDisappearedL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPPlaybackSession::DeviceDisappearedL" );
   
    if( aDevice.Local() )
        {
        delete iLocalMediaServerUuid; iLocalMediaServerUuid = NULL; 
        }
    else if( iDeviceMessage ) // Target device
        {
        iDeviceMessage->Complete( KErrNone );
        delete iDeviceMessage; iDeviceMessage = NULL;
        }    
    }
 
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetLocalMSUuidL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetLocalMSUuidL( const TDesC8& aUuid )
    {
    HBufC8* tmp = aUuid.AllocL();
    delete iLocalMediaServerUuid;
    iLocalMediaServerUuid = tmp; 
    } 
 
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SessionId
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
TInt CUPnPPlaybackSession::SessionId() const
    {
    return iSessionId;
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::EventRequestL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::EventRequestL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::EventRequestL" );
    
    __ASSERTD( !iEventMessage, __FILE__, __LINE__ );
    
    TInt count = iEventQue.Count(); 
    if( count )
        {
        __LOG( "EventRequestL - events in the que" );
        // Events pending, get the event from que and complete the msg
        TPckg<TUnsolicitedEventC> resp1( iEventQue[ count - 1 ] );
        TInt err = aMessage.Write( 1, resp1 );
        iEventQue.Remove( count - 1 ); 
        aMessage.Complete( err ); // Ok to complete with err?
        }
    else
        {
        __LOG( "EventRequestL - storing the msg" );
        iEventMessage = new (ELeave) RMessage2( aMessage );
        
        if( !iEventingActive )
            {
            __LOG( "EventRequestL - subscribing.." );
            iServer.DeviceRepository().SubscribeDeviceL( iDevice->Uuid() );
            iServer.Dispatcher().RegisterForEventsL( *this,
                iDevice->Uuid() );
            iEventingActive = ETrue;
            }           
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelEventRequestL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelEventRequestL()
    {
    __LOG( "CUPnPPlaybackSession::CancelEventRequestL" );
           
    if( iEventingActive )
        {
        __LOG( "CancelEventRequestL - unsubscribing.." );
        iServer.DeviceRepository().UnSubscribeDeviceL( iDevice->Uuid() );
        iServer.Dispatcher().UnRegisterEvents( *this );
        iEventingActive = EFalse;            
        }
        
    if( iEventMessage )
        {
        __LOG( "CancelEventRequestL - cancelling the msg.." );
        iEventMessage->Complete( KErrCancel );
        delete iEventMessage; iEventMessage = NULL;   
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetURIL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetURIL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SetURIL" );
    
    __ASSERTD( !iCommandMessage, __FILE__, __LINE__ );
    
    ResetL();

    if( iPlaybackState == EPlaying || iPlaybackState == EPaused ||
        iPlaybackState == EPlaySent )
        {
        iServer.ControlPoint().AvtStopActionL( iDevice->Uuid(),
            iInstanceId );
        }
    
    if( iItemShared )
        {
        iFileSharing->UnShareItemL( iSharedItem->Id() );
        iItemShared = EFalse; 
        }

    // Uri is set by providing an item. Convert the item to xml document and
    // send the action

    __LOG( "SetURIL" );
    
    CUpnpAVRequest* tmpRequest = CUpnpAVRequest::NewLC();
    
    ReadReqFromMessageL( aMessage, 1 ,tmpRequest );
    
    CUpnpItem* tmpItem = CUpnpItem::NewL();
    CleanupStack::PushL( tmpItem );
    
    ReadObjFromMessageL( aMessage, 2 ,tmpItem );

    TPtrC8 uri = tmpRequest->URI();

    const CUpnpElement& res = 
        UPnPItemUtility::ResourceFromItemL( *tmpItem );
    const CUpnpAttribute* protocolInfo = 
        UPnPItemUtility::FindAttributeByName( 
        res, KAttributeProtocolInfo );

    if( !iDevice->MatchSinkProtocolInfo( protocolInfo->Value() ) )
        {
        // Did not match, try to find a match
        TRAPD( err, uri.Set( iDevice->FindFirstMatchingInSinkL(
            *tmpItem ) ) );
        if( err == KErrNone )
            {
            // Suitable res-element found!
            __LOG( "Suitable element found!" );
            }
        else if( err == KErrNotSupported )
            {
            // No suitable res-element
            if( iDevice->DLNADeviceType() ==
                CUpnpAVDeviceExtended::EDMR )
                {
                // DLNA content, DLNA device, no match -> leave
                User::Leave( KErrNotSupported );                    
                }
            else
                {
                // Not a dlna device, try to set the uri of
                // original res-element anyways
                }
            }
        else
            {
            // Some error occured
            User::Leave( err );
            }    
        }

    ValidateProtocolInfoL( *protocolInfo );
           
    // Create metadata xml document
    HBufC8* xmlDoc = CUPnPXMLParser::ItemAsXmlLC( *tmpItem );

    iIPSessionIdCommand = iServer.ControlPoint().
        AvtSetTransportUriActionL( iDevice->Uuid(), iInstanceId, uri,
        //KNullDesC8 );
        *xmlDoc );
            
    CleanupStack::PopAndDestroy( xmlDoc );
    CleanupStack::PopAndDestroy( tmpItem );        
    CleanupStack::PopAndDestroy( tmpRequest );      
                         
                
    if( iIPSessionIdCommand > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }
    else
        {
        User::Leave( iIPSessionIdCommand );
        }                             
           
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    // after settransporturi, there is a delay to send play action.
    if ( !iPlayDelayTimer->IsActive() )
        {
        iPlayDelayTimer->Start( 
            0, KPlayDelayTimerInterval, 
            TCallBack( PlayDelayTimeExpired , this ) );
        }
    else
        {
        iPlayDelayTimer->Cancel();
        iPlayDelayTimer->Start( 
            0, KPlayDelayTimerInterval, 
            TCallBack( PlayDelayTimeExpired , this ) );
        }
    __LOG( "CUPnPPlaybackSession::SetURIL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSetURIL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSetURIL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSetURIL" );
    
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;             
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetNextURIL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetNextURIL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SetNextURIL" );
    
    __ASSERTD( !iCommandMessage, __FILE__, __LINE__ );
    
    ResetL();
    
    if( iNextItemShared )
        {
        iFileSharing->UnShareItemL( iNextSharedItem->Id() );
        iNextItemShared = EFalse; 
        }
    
    // Uri is set by providing an item. Convert the item to xml document and
    // send the action
    CUpnpAVRequest* tmpRequest = CUpnpAVRequest::NewLC();
    
    ReadReqFromMessageL( aMessage, 1 ,tmpRequest );
    
    CUpnpItem* tmpItem = CUpnpItem::NewL();
    CleanupStack::PushL( tmpItem );
    
    ReadObjFromMessageL( aMessage, 2 ,tmpItem );

    TPtrC8 uri = tmpRequest->URI();
    
    const CUpnpElement& res = 
        UPnPItemUtility::ResourceFromItemL( *tmpItem );
    const CUpnpAttribute* protocolInfo = 
        UPnPItemUtility::FindAttributeByName( 
        res, KAttributeProtocolInfo );
          
    if( !iDevice->MatchSinkProtocolInfo( protocolInfo->Value() ) )
        {
        // Did not match, try to find a match
        TRAPD( err, uri.Set( iDevice->FindFirstMatchingInSinkL(
            *tmpItem ) ) );
        if( err == KErrNone )
            {
            // Suitable res-element found!
            }
        else if( err == KErrNotSupported )
            {
            // No suitable res-element
            if( iDevice->DLNADeviceType() ==
                CUpnpAVDeviceExtended::EDMR )
                {
                // DLNA content, DLNA device, no match -> leave
                User::Leave( KErrNotSupported );                    
                }
            else
                {
                // Not a dlna device, try to set the uri of
                // original res-element anyways
                }    
            }
        else
            {
            // Some error occured
            User::Leave( err );
            }    
        }

    // Create metadata xml document
    HBufC8* xmlDoc = CUPnPXMLParser::ItemAsXmlLC( *tmpItem );
    
    iIPSessionIdCommand = iServer.ControlPoint().
        AvtSetNextTransportUriActionL( iDevice->Uuid(), iInstanceId, uri,
        *xmlDoc );
            

    CleanupStack::PopAndDestroy( xmlDoc );
    CleanupStack::PopAndDestroy( tmpItem );
    CleanupStack::PopAndDestroy( tmpRequest );          
                
    if( iIPSessionIdCommand > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }
    else
        {
        User::Leave( iIPSessionIdCommand );
        }                             
           
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    __LOG( "CUPnPPlaybackSession::SetNextURIL - end" );
 
   }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSetNextURIL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSetNextURIL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSetNextURIL" );
    
    //__ASSERTD( iCommandPending, User::Panic( KPanicText, __LINE__ ) );
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PlayL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PlayL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::PlayL" );
    if ( !iPlayDelayTimer->IsActive() )
        {
        // timer is not running so some time has passed since subscribing
    __ASSERTD( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    
    iIPSessionIdCommand = iServer.ControlPoint().AvtPlayActionL(
        iDevice->Uuid(), iInstanceId, KNormalSpeed );
    
     if( iIPSessionIdCommand > 0 )
        {
        __LOG( "CUPnPPlaybackSession::PlayL - registering" );
        // Register
        iPlaybackState = EPlaySent;
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }
    else
        {
        User::Leave( iIPSessionIdCommand );
            }

        }
    else // less than KPlayDelayInterval passed since subscribe.
        {
        // issue the play after the timer expires to make sure some HW 
        // renderers are not confused when beginning the playback.
        iPlayRequested = ETrue;
        }

    iCommandMessage = new (ELeave) RMessage2( aMessage );
    __LOG( "CUPnPPlaybackSession::PlayL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelPlayL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelPlayL()
    {
    __LOG( "CUPnPPlaybackSession::CancelPlayL" );
    
    //__ASSERTD( iCommandPending, User::Panic( KPanicText, __LINE__ ) );
    if( iCommandMessage )
        {
        // cancel postponed play
        if ( iPlayDelayTimer->IsActive() )
            {
            iPlayDelayTimer->Cancel();
            }

        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::StopL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::StopL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::StopL" );
    
    __ASSERTD( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    // state stopped must be check before stopped action    
    if( iPlaybackState != EStopped )
        {
        iIPSessionIdCommand = iServer.ControlPoint().AvtStopActionL(
            iDevice->Uuid(), iInstanceId );
        }
    
     if( iIPSessionIdCommand > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }
    else
        {
        User::Leave( iIPSessionIdCommand );
        }        
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    __LOG( "CUPnPPlaybackSession::StopL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelStopL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelStopL()
    {
    __LOG( "CUPnPPlaybackSession::CancelStopL" );
    
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PauseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PauseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::PauseL" );
    
    __ASSERTD( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    
    iIPSessionIdCommand = iServer.ControlPoint().AvtPauseActionL(
        iDevice->Uuid(), iInstanceId );
    
     if( iIPSessionIdCommand > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }
    else
        {
        User::Leave( iIPSessionIdCommand );
        }
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    __LOG( "CUPnPPlaybackSession::PauseL - end" );                
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelPauseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelPauseL()
    {
    __LOG( "CUPnPPlaybackSession::CancelPauseL" );                
    
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetVolumeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetVolumeL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SetVolumeL" );                
    
    __ASSERTD( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();
    
    TInt volume = aMessage.Int1();
    
    TInt maxVolume = iDevice->MaxVolume();

    // If max volume not KMaxVolume
    if( maxVolume != KMaxVolume )
        {
        // Convert volume to match device's max volume
        TReal tempVolumeLevel = volume;
        TReal tempMaxVolume = maxVolume;
        
        volume = tempMaxVolume * tempVolumeLevel / KMaxVolume;
        }
       


    iIPSessionIdSetting = iServer.ControlPoint().RcSetVolumetActionL(
        iDevice->Uuid(), iInstanceId, KMasterVolume, volume );
    
     if( iIPSessionIdSetting > 0 )
        {
        __LOG( "CUPnPPlaybackSession::SetVolumeL - registering" );
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
        }
    else
        {
        User::Leave( iIPSessionIdSetting );
        }
        
    iSettingMessage = new (ELeave) RMessage2( aMessage );                
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSetVolumeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSetVolumeL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSetVolumeL" );                
    
    if( iSettingMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
        iIPSessionIdSetting = KErrNotFound;
        iSettingMessage->Complete( KErrCancel );
        delete iSettingMessage; iSettingMessage = NULL;            
        }
    }    
   
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::GetVolumeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::GetVolumeL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::GetVolumeL" );                
    
    __ASSERTD( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();
    
    iIPSessionIdSetting = iServer.ControlPoint().RcGetVolumetActionL(
        iDevice->Uuid(), iInstanceId, KMasterVolume );
    
     if( iIPSessionIdSetting > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
        }
    else
        {
        User::Leave( iIPSessionIdSetting );
        }
    iSettingMessage = new (ELeave) RMessage2( aMessage );                    
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelGetVolumeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelGetVolumeL()
    {
    __LOG( "CUPnPPlaybackSession::CancelGetVolumeL" );                
    
    if( iSettingMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
        iIPSessionIdSetting = KErrNotFound;
        iSettingMessage->Complete( KErrCancel );
        delete iSettingMessage; iSettingMessage = NULL;            
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetMuteL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetMuteL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SetMuteL" );                
    
    __ASSERTD( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();

    TInt mute = aMessage.Int1();
    if( mute )
        {
        iIPSessionIdSetting = iServer.ControlPoint().RcSetMuteActionL(
            iDevice->Uuid(), iInstanceId, KMasterVolume, KMuteOn );        
        }
    else
        {
        iIPSessionIdSetting = iServer.ControlPoint().RcSetMuteActionL(
            iDevice->Uuid(), iInstanceId, KMasterVolume, KMuteOff );                
        }    
    
     if( iIPSessionIdSetting > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
        }
    else
        {
        User::Leave( iIPSessionIdSetting );
        }
        
    iSettingMessage = new (ELeave) RMessage2( aMessage );                 
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSetMuteL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSetMuteL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSetMuteL" );                
    
    if( iSettingMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
        iIPSessionIdSetting = KErrNotFound;
        iSettingMessage->Complete( KErrCancel );
        delete iSettingMessage; iSettingMessage = NULL;            
        }
    }    
   
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::GetMuteL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::GetMuteL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::GetMuteL" );                
    
    __ASSERTD( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();
    
    iIPSessionIdSetting = iServer.ControlPoint().RcGetMuteActionL(
        iDevice->Uuid(), iInstanceId, KMasterVolume );
    
     if( iIPSessionIdSetting > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
        }
    else
        {
        User::Leave( iIPSessionIdSetting );
        }
    iSettingMessage = new (ELeave) RMessage2( aMessage );                    
    
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelGetMuteL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelGetMuteL()
    {
    __LOG( "CUPnPPlaybackSession::CancelGetMuteL" );                
    
    if( iSettingMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
        iIPSessionIdSetting = KErrNotFound;
        iSettingMessage->Complete( KErrCancel );
        delete iSettingMessage; iSettingMessage = NULL;            
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::GetPositionInfoL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::GetPositionInfoL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::GetPositionInfoL" );                
    
    __ASSERTD( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();

    iIPSessionIdSetting = iServer.ControlPoint().AvtPositionInfoActionL(
        iDevice->Uuid(), iInstanceId );
    
     if( iIPSessionIdSetting > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
        }
    else
        {
        User::Leave( iIPSessionIdSetting );
        }
    iSettingMessage = new (ELeave) RMessage2( aMessage );
    
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelGetPositionInfoL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelGetPositionInfoL()
    {
    __LOG( "CUPnPPlaybackSession::CancelGetPositionInfoL" );                
    
    if( iSettingMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
        iIPSessionIdSetting = KErrNotFound;
        iSettingMessage->Complete( KErrCancel );
        delete iSettingMessage; iSettingMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::DeviceDisappearedRequestL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::DeviceDisappearedRequestL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::DeviceDisappearedRequestL" );
    
    __ASSERTD( !iDeviceMessage, __FILE__, __LINE__ );
    
    iDeviceMessage = new (ELeave ) RMessage2( aMessage );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelDeviceDisappearedRequestL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelDeviceDisappearedRequestL()
    {
    __LOG( "CUPnPPlaybackSession::CancelDeviceDisappearedRequestL" );
    
    if( iDeviceMessage )
        {
        iDeviceMessage->Complete( KErrCancel );
        delete iDeviceMessage; iDeviceMessage = NULL;             
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ParseBrowseResponseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPPlaybackSession::ParseBrowseResponseL( const TDesC8& aResponse )
    {
    __LOG( "CUPnPPlaybackSession::ParseBrowseResponseL" );                
    
    HBufC8* resource = NULL;
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    if( array.Count() == KExpectedCount )
        {
        if( array[ 0 ]->ObjectType() == EUPnPItem )
            {
            CUpnpItem* item = static_cast<CUpnpItem*>( array[ 0 ] );
            resource = UPnPItemUtility::ResourceFromItemL(
                *item ).Value().AllocL();
            CleanupStack::PushL( resource );

            const CUpnpElement& res = 
                UPnPItemUtility::ResourceFromItemL( *item );
            const CUpnpAttribute* protocolInfo = 
                UPnPItemUtility::FindAttributeByName( 
                res, KAttributeProtocolInfo );

            if( !iDevice->MatchSinkProtocolInfo( protocolInfo->Value() ) )
                {
               TPtrC8 uri;
                // Did not match, try to find a match
                TRAPD( err, uri.Set( iDevice->FindFirstMatchingInSinkL(
                    *item ) ) );
                if( err == KErrNone )
                    {
                    // Suitable res-element found!
                    CleanupStack::PopAndDestroy( resource );
                    resource = uri.AllocL();
                    }
                else if( err == KErrNotSupported )
                    {
                    // No suitable res-element
                    if( iDevice->DLNADeviceType() ==
                        CUpnpAVDeviceExtended::EDMR )
                        {
                        // DLNA content, DLNA device, no match -> leave
                        User::Leave( KErrNotSupported );                    
                        }
                    else
                        {
                        // Not a dlna device, try to set the uri of
                        // original res-element anyways
                        CleanupStack::Pop( resource );
                        }    
                    }
                else
                    {
                    // Some error occured
                    User::Leave( err );
                    }    
                }
            }
        else
            {
            User::Leave( KErrGeneral );
            }    
        }
    else
        {
        User::Leave( KErrGeneral );
        }    
        
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( parser );
       
    __LOG( "CUPnPPlaybackSession::ParseBrowseResponseL - end" );
    
    if( !resource )
        {
        User::Leave( KErrGeneral );
        }
        
    return resource;
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::Uuid
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
const TDesC8& CUPnPPlaybackSession::Uuid() const
    {
    if( iDevice )
        {
        return iDevice->Uuid();
        }
    else
        {
        return KNullDesC8;
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::EncodeXmlL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPPlaybackSession::EncodeXmlL( const TDesC8& aResult )
    {
    HBufC8* tmpBuf = aResult.AllocLC();
    HBufC8* result = UpnpString::EncodeXmlStringL( tmpBuf );
    CleanupStack::PopAndDestroy( tmpBuf );
    return result;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::Reset
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ResetL()
    {
    __LOG( "CUPnPPlaybackSession::ResetL" );
    
    if( !iServer.DeviceRepository().IsWlanActive() )    
        {
        __LOG( "Reset - disconnected" );
        User::Leave( KErrDisconnected );
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ReadObjFromMessageL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ReadObjFromMessageL( const RMessage2& aMessage, 
    TInt aSlot, CUpnpObject* aObj )
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aObj;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }    

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ReadReqFromMessageL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ReadReqFromMessageL( const RMessage2& aMessage, 
    TInt aSlot, CUpnpAVRequest* aReq ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aReq;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }    

void CUPnPPlaybackSession::ValidateProtocolInfoL( const CUpnpAttribute&
    aResource )
    {
    __LOG( "CUPnPPlaybackSession::ValidateProtocolInfoL" );
    
    // Whe'd like to modify the original protocolInfo, that's why constness
    // is casted away
    CUpnpAttribute& attr = const_cast<CUpnpAttribute&>( aResource );
    
    // ProtocolInfo-wrapper takes care of 4th field validation, omitting
    // invalid optional parameters
    CUpnpDlnaProtocolInfo* tmpInfo = 
        CUpnpDlnaProtocolInfo::NewL( attr.Value() );
    CleanupStack::PushL( tmpInfo );
    
    tmpInfo->SetSecondFieldL( KAsterisk ); // Second field must be '*'
    
    attr.SetValueL( tmpInfo->ProtocolInfoL() );
        
    CleanupStack::PopAndDestroy( tmpInfo );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SendPlayIfNeededL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SendPlayIfNeededL()
    {
    __LOG( "CUPnPPlaybackSession::SendPlayIfNeededL" );
    if ( iPlayRequested )
        {
        // during the timer was running, there was a play request.
        // handle it here
        iPlayRequested = EFalse; // play request is being handled

        ResetL();

        iIPSessionIdCommand = iServer.ControlPoint().AvtPlayActionL(
            iDevice->Uuid(), iInstanceId, KNormalSpeed );

        if( iIPSessionIdCommand > 0 )
            {
            __LOG( "CUPnPPlaybackSession::SendPlayIfNeededL - registering" );
            // Register
            iPlaybackState = EPlaySent;
            iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
            }
        else
            {
            User::Leave( iIPSessionIdCommand );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PlayDelayTimeExpired
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
TInt CUPnPPlaybackSession::PlayDelayTimeExpired( TAny* aPtr )
    {
    __LOG( "CUPnPPlaybackSession::PlayDelayTimeExpired" );
    TRAPD( err, ( static_cast< CUPnPPlaybackSession* >( aPtr ) )->
           SendPlayIfNeededL() );
    
    if ( err )
        {
        __LOG( "CUPnPPlaybackSession::PlayDelayTimeExpired error" );
        }

    return ETrue;
    }

// end of file
