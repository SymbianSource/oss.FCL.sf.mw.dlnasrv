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
* Description:      implements a renderer playback state machine
*
*/

// INCLUDE FILES
// System
#include <in_sock.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// upnp stack api
#include <upnpdevice.h>
#include <upnpservice.h>
#include <upnpstring.h>

// dlnasrv / mediaserver api
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpdlnaprotocolinfo.h>

// dlnasrv / xmlparser api
#include "upnpxmlparser.h"

// dlnasrv / avcontroller api
#include "upnpavcontrollerglobals.h"

// dlnasrv / avcontroller helper api
#include "upnpitemutility.h"
#include "upnpconstantdefs.h" // for upnp-specific stuff

// dlnasrv / internal api
#include "upnpmetadatafetcher.h"
#include "upnpcommonutils.h"
#include "upnpcdsreselementutility.h"
#include "upnpxmleventparser.h"

// dlnasrv / avcontroller internal
#include "upnpavcontrollerserver.h"
#include "upnpavrequest.h"
#include "upnpavdispatcher.h"
#include "upnpaverrorhandler.h"
#include "upnpavdeviceextended.h"
#include "upnpdevicerepository.h"
#include "upnpplaybacksession.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpstrings.h"

using namespace UpnpAVCPStrings;

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

_LIT8( KNormalSpeed,        "1" );
_LIT8( KMasterVolume,       "Master" );
_LIT8( KMuteOn,             "1" );
_LIT8( KMuteOff,            "0" );
_LIT8( KAsterisk,           "*" );
_LIT8( KConnectioMgr,       "urn:upnp-org:serviceId:ConnectionManager" );
_LIT8( KInput,              "Input" );
_LIT8( KDefaultPeerConnectionID, "-1" );

const TInt KDefaultInstanceId   = 0;
const TInt KMaxVolume           = 100;
const TInt KMaxIntLength        = 10;

_LIT8( KNoMedia, "NO_MEDIA_PRESENT" );
_LIT8( KPlaying, "PLAYING" );
_LIT8( KPausedPlayback, "PAUSED_PLAYBACK" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::NewL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
CUPnPPlaybackSession* CUPnPPlaybackSession::NewL
    (
    CUpnpAVControllerServer& aServer,
    TInt aSessionId,
    const TDesC8& aUuid
    )
    {
    CUPnPPlaybackSession* self = new (ELeave) CUPnPPlaybackSession(
        aServer, aSessionId );
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
    CUpnpAVControllerServer& aServer,
    TInt aSessionId
    ):
    iServer( aServer ),
    iSessionId( aSessionId ),
    iAVTInstanceId( KDefaultInstanceId ),
    iRCInstanceId( KDefaultInstanceId ),
    iConnectionId( KErrNotFound ),
    iIPSessionIdCommand( KErrNotFound ),
    iIPSessionIdSetting( KErrNotFound ),
    iEventingActive( EFalse ),
    iPlaybackState( EUninitialized ),
    iMuteState( EUnknown ),
    iVolume( KErrNotFound ),
    iInitialEventReceived(EFalse),
    iPreviousTransportState( CUPnPAVTEvent::ENoMediaPresent )
    {
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::~CUPnPPlaybackSession
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
CUPnPPlaybackSession::~CUPnPPlaybackSession()
    {
    __LOG( "CUPnPPlaybackSession::~CUPnPPlaybackSession" );
    
    if( ( iPlaybackState == EPlaying || iPlaybackState == EPaused ) && 
         !iInitialEventReceived )
        {
        if( iDevice && iServer.DeviceRepository().IsWlanActive() )
            {
            __LOG( "~CUPnPPlaybackSession - \
playback still ongoing, send stop" );    
            TRAP_IGNORE( EmergencyStopL() );
            }
        else
            {
            __LOG( "~CUPnPPlaybackSession - \
playback still ongoing, wlan not active" );    
            }    
        }

    delete iEventMessage;
    delete iSettingMessage;
    delete iCommandMessage;
    delete iDeviceMessage;
    delete iInitialEventMsg;
    
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
    
    delete iPInfoForPrevious;
    delete iCurrentUri;
    delete iCurrentItem;
    
    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }
	delete iUuid;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ConstructL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPPlaybackSession::ConstructL" );
    
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
            
            __ASSERT( !iLocalMediaServerUuid, __FILE__, __LINE__ );
            iLocalMediaServerUuid = devList[i]->Uuid().AllocL();
            }
        if( devList[ i ]->Uuid() == aUuid )
            {
            __ASSERT( !iDevice, __FILE__, __LINE__ );
            iDevice = CUpnpAVDeviceExtended::NewL( *devList[ i ] );
            }
        }        
    if( !iDevice )
        {
        User::Leave( KErrNotFound );
        }
    
    iUuid = HBufC8::NewL( aUuid.Length() );
    TPtr8 uuidPtr( iUuid->Des() );
    uuidPtr.Copy( aUuid );
    
    iTimer = CUPnPAVTimer::NewL( *this, CUPnPAVTimer::ETimerFailSafe );

    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CpDeviceL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
const CUpnpDevice* CUPnPPlaybackSession::CpDeviceL()
    {
    const CUpnpDevice* device = iServer.ControlPoint().Device( iUuid->Des() );
    if( !device )
        {
		// Control point resets device information once renderer or wlan 
		// connection lost. Avoid panicing by leaving.
        __LOG( "CUPnPPlaybackSession::CpDeviceL - no device, leaving" );
        User::Leave( KErrNotReady );
        }
    return device;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::ActionResponseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::ActionResponseL( CUpnpAction* aAction )
    {
    if ( aAction->Name().Compare( KSetVolume ) == 0 )
        {
        RcSetVolumeResponse(
            aAction->Error(),
            aAction->ArgumentValue( KDesiredVolume ) );
        }
    else if ( aAction->Name().Compare( KGetVolume ) == 0 )
        {
        RcVolumeResponse( 
            aAction->Error(),
            aAction->ArgumentValue( KCurrentVolume ) );
        }
    else if ( aAction->Name().Compare( KSetMute ) == 0 )
        {
        RcSetMuteResponse( 
            aAction->Error(),
            aAction->ArgumentValue( KDesiredMute ) );
        }
    else if ( aAction->Name().Compare( KGetMute ) == 0 )
        {
        RcMuteResponse( 
             aAction->Error(),
             aAction->ArgumentValue( KCurrentMute ) );
        }
    else if ( aAction->Name().Compare( KSetAVTransportURI ) == 0 )
        {
        AvtSetTransportUriResponse(
            aAction->Error() );
        }
    else if ( aAction->Name().Compare( KGetMediaInfo ) == 0 )
        {
        AvtGetMediaInfoResponse(
            aAction->Error(),
            aAction->ArgumentValue( KCurrentURI ) );
        }
    else if ( aAction->Name().Compare( KGetTransportInfo ) == 0 )
        {
        AvtGetTransportInfoResponse(
            aAction->Error(),
            aAction->ArgumentValue( KCurrentTransportState ) );
        }
    else if ( aAction->Name().Compare( KGetPositionInfo ) == 0 )
        {
        AvtPositionInfoResponse(
            aAction->Error(),
            aAction->ArgumentValue( KTrackDuration ),
            aAction->ArgumentValue( KRelTime ) );
        }
    else if ( aAction->Name().Compare( KStop ) == 0 )
        {				
        AvtStopResponse(
            aAction->Error() );
        }
    else if ( aAction->Name().Compare( KPlay ) == 0 )
        {				
        AvtPlayResponse(
            aAction->Error() );
        }
    else if ( aAction->Name().Compare( KPause ) == 0 )
        {		
        AvtPauseResponse(
            aAction->Error() );
        }
    else if ( aAction->Name().Compare( KSeek ) == 0 )
        {	
        AvtSeekResponse(
            aAction->Error() );
        }
    else if (aAction->Name().Compare( KPrepareForConnection ) == 0)
        {
        TLex8 connectionLex1( aAction->ArgumentValue( KConnectionId ) );
        TInt connectionId;
        User::LeaveIfError( connectionLex1.Val( connectionId ) );
        TLex8 transportLex1( aAction->ArgumentValue( KAVTransportId ) );
        TInt transportId;
        User::LeaveIfError( transportLex1.Val( transportId ) );
        TLex8 rscLex3( aAction->ArgumentValue( KRcsID ) );
        TInt rscId;
        User::LeaveIfError( rscLex3.Val( rscId ) );
        CmPrepareResponse(
            aAction->Error(),
            connectionId,
            transportId,
            rscId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::StateUpdatedL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::StateUpdatedL(CUpnpService* aService)
    {
    if (aService->ServiceType().Match( KRenderingControl ) != KErrNotFound )
        {
        CUpnpStateVariable* lastChange = aService->StateVariable( KLastChange );
        if( lastChange ) 
            {
            RcLastChangeEvent(
                lastChange->Value() );
            }
        }
    else if (aService->ServiceType().Match( KAVTransport ) != KErrNotFound )
        {
        CUpnpStateVariable* lastChange = aService->StateVariable( KLastChange );
        if( lastChange )
            {
            AvtLastChangeEvent(
                lastChange->Value() );
            }
        }
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
void CUPnPPlaybackSession::DeviceDisappearedL( CUpnpDevice* /*aDevice*/ )    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcSetVolumeResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcSetVolumeResponse(
    TInt aErr, 
    const TDesC8& aDesiredVolume )
    {
    __LOG1( "CUPnPPlaybackSession::RcSetVolumeResponse: %d", aErr );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
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
    TInt aErr, 
    const TDesC8& aCurrentVolume)
    {
    __LOG1( "CUPnPPlaybackSession::RcVolumeResponse: %d", aErr );    
    
    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
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
    TInt aErr, 
    const TDesC8& aDesiredMute )
    {
    __LOG1( "CUPnPPlaybackSession::RcSetMuteResponse: %d", aErr );    

    iServer.Dispatcher().UnRegister( iIPSessionIdSetting );
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
    TInt aErr, 
    const TDesC8& aCurrentMute )
    {
    __LOG1( "CUPnPPlaybackSession::RcMuteResponse: %d" , aErr );

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
    TInt aErr )
    {
    __LOG1( "CUPnPPlaybackSession::AvtSetTransportUriResponse: %d", aErr );  

    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );

    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPAVTransportError );    

    if( aErr == KErrNone )
        {
        if( iPlaybackState == EStopped )
            {
            __LOG( "CUPnPPlaybackSession::AvtSetTransportUriResponse - \
Already in stopped state" );
            
            // Lastchangeevent came already!                         
            }
        else
            {
            iIPSessionIdCommand = KErrNotFound;
            
            __LOG( "CUPnPPlaybackSession::AvtSetTransportUriResponse - \
Start waiting for CurrentUriMetadata" );
            
            iTimer->Cancel();
            iTimer->Start( KTimerCycle10 );
            }      
        }
    else
        {
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        
        iPlaybackState = EStopped;
        iExpectedEvent = EEventNone;
        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;      
        }                       
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtGetMediaInfoResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
//
void CUPnPPlaybackSession::AvtGetMediaInfoResponse(
    TInt aErr,
    const TDesC8& aCurrentUri )
    {
    // GetMediaInfo action request is issued in playing state when an unexpected 
    // playing event is received.  If the URI in the renderer does not match with 
    // iCurrentUri, it is intepreted as a case where the renderer is being 
    // controlled by some other Control Point and therefore a stop event is propagated.
        
    __LOG1( "CUPnPPlaybackSession::AvtGetMediaInfoResponse %d", aErr );
    
    if ( iCheckForHijackedRenderer )
        {
        __LOG8_1( "CUPnPPlaybackSession::AvtGetMediaInfoResponse: %S",
                &aCurrentUri );

        __LOG8_1( "CUPnPPlaybackSession::AvtGetMediaInfoResponse, iCurrentUri: %S",
                iCurrentUri );

        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;

        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
                EUPnPConnectionManagerError );
            
        if( aErr == KErrNone && iCurrentUri && *iCurrentUri != aCurrentUri )
            {
            // not our URI playing, renderer is hijacked    
            if( iPlaybackState != EStopped )
                {
                // if we're not yet stopped, propagate a stop event    
                __LOG( "CUPnPPlaybackSession::AvtGetMediaInfoResponse -\
unexpected Uri, renderer is being controlled by someone else - propagate a stop event" );      
                iPlaybackState = EHalted;
                TUnsolicitedEventC event;
                event.iEvent = EStop;
                event.iValue = KErrNotReady;
                PropagateEvent( event );                    
                }
            else
                {
                // just ignore as someone else keeps controlling the renderer    
                __LOG( "CUPnPPlaybackSession::AvtGetMediaInfoResponse -\
unexpected Uri, renderer is still being controlled by someone else - ignoring this event" );                      
                }
            }
        
        iCheckForHijackedRenderer = EFalse;
        }            
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtGetTransportInfoResponse
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPPlaybackSession::AvtGetTransportInfoResponse(
    TInt aErr,
    const TDesC8& aCurrenTransportState )
    {
    __LOG1( "CUPnPPlaybackSession::AvtGetTransportInfoResponse %d", aErr );

    __LOG8_1( "CUPnPPlaybackSession::AvtGetTransportInfoResponse: %S",
            &aCurrenTransportState );

    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
 
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPConnectionManagerError );
    
    if( aErr == KErrNone )
        {            
        // Send AVTranportUri-action. If an error occurs (leaves), complete
        // the message with error code. No futher processing can be done
        
        if( aCurrenTransportState == KNoMedia )
            {
            iPlaybackState = ENoMedia;
            }
        else if( aCurrenTransportState == KPlaying )
            {
            iPlaybackState = EPlaying;
            }
        else if( aCurrenTransportState == KPausedPlayback )
            {
            iPlaybackState = EPaused;
            }
        else
            {
            iPlaybackState = EStopped;
            }
        
        TRAP( aErr, SendAVTransportUriActionL() );
        if( aErr != KErrNone )
            {
            __LOG1( "CUPnPPlaybackSession::AvtGetTransportInfoResponse -\
SendAVTransportUriActionL failed with code %d", aErr  );
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;
            }
        }
    else
        {
        // Something wrong with the action. Complete the message with error
        // code. No further processing can be dome.
        __LOG1( "CUPnPPlaybackSession::AvtGetTransportInfoResponse - \
action failed with code %d", aErr  );
        
        delete iCurrentItem; iCurrentItem = NULL;
        delete iCurrentUri; iCurrentUri = NULL;
        
        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;        
        } 
    
    __LOG( "CUPnPPlaybackSession::AvtGetTransportInfoResponse - end" );
          
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPositionInfoResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPositionInfoResponse(
    TInt aErr,
    const TDesC8& aTrackDuration,
    const TDesC8& aRelTime )
    {
    __LOG1( "CUPnPPlaybackSession::AvtPositionInfoResponse: %d", aErr );    

    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    
    if( iCommandMessage )
        {
        TInt err = iCommandMessage->Write( 1, aTrackDuration );
        err = iCommandMessage->Write( 2, aRelTime );
        // Howto handle err?

        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPAVTransportError );    
           
        if( aErr == KErrNone )
            {
            iCommandMessage->Complete( EAVControllerPositionInfoCompleted );
            }
        else
            {
            iCommandMessage->Complete( aErr ); 
            }        
        delete iCommandMessage; iCommandMessage = NULL; 
        }
    else
        {
        __LOG( "AvtPositionInfoResponse - no msg" );    
        }       
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtStopResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtStopResponse(
    TInt aErr )
    {
    __LOG1( "CUPnPPlaybackSession::AvtStopResponse: %d", aErr );    

    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPAVTransportError );        
    
    if( aErr == KErrNone )
        {
		__LOG( "CUPnPPlaybackSession::AvtStopResponse - \
Start waiting for stop event" );
		
		iTimer->Cancel();
		iTimer->Start( KTimerCycle10 );      
        }
    else
        {
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        
        //iWaitingResponse = EFalse;
        iExpectedEvent = EEventNone;
        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;       
        }                
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPlayResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPlayResponse(
    TInt aErr )
    {
    __LOG1( "CUPnPPlaybackSession::AvtPlayResponse: %d", aErr );    
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPAVTransportError );        
    
    if( aErr == KErrNone )
        {
		__LOG( "CUPnPPlaybackSession::AvtPlayResponse - \
Start waiting for play event" );

		iTimer->Cancel();
		iTimer->Start( KTimerCycle10 );      
        }
    else
        {
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        
        //iWaitingResponse = EFalse;
        iExpectedEvent = EEventNone;
        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;
        }                    

    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtPauseResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtPauseResponse(
    TInt aErr )
    {
    __LOG1( "CUPnPPlaybackSession::AvtPauseResponse: %d", aErr );    
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPAVTransportError );        
    
    if( aErr == KErrNone )
        {
		__LOG( "CUPnPPlaybackSession::AvtPauseResponse - \
Start waiting for pause event" );

		iTimer->Cancel();
		iTimer->Start( KTimerCycle10 );
        }
    else
        {
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        
        //iWaitingResponse = EFalse;
        iExpectedEvent = EEventNone;
        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;             
        }                      
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtSeekResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtSeekResponse(
    TInt aErr )
    {
    __LOG1( "CUPnPPlaybackSession::AvtSeekResponse: %d", aErr );

    __ASSERT( iCommandMessage, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPAVTransportError );        
    
    if( aErr == KErrNone )
        {
        iCommandMessage->Complete( EAVControllerSeekCompleted );
        }
    else
        {
        iCommandMessage->Complete( aErr );
        }
    delete iCommandMessage; 
    iCommandMessage = NULL;
    }


// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CmPrepareResponse
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CmPrepareResponse(
    TInt aErr,
    TInt aConnection,
    TInt aTransport,
    TInt aRsc )
    {
    __LOG1( "CUPnPPlaybackSession::CmPrepareResponse %d", aErr );    

    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;
 
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPConnectionManagerError );
    
    if( aErr == KErrNone )
        {
        // FIX IOP problem with RC events with Simple Center
        // Note that this is likely a problem in SC and needs to be verified
        // once there is another device supporting CM:PrepareForConnection
        iRCInstanceId = KDefaultInstanceId; //iRCInstanceId = aRsc; 
        iAVTInstanceId = aTransport;
        iConnectionId = aConnection;    

        TRAP( aErr, SendGetTransportInfoActionL() )
        if( aErr != KErrNone )
            {
            __LOG1( "CUPnPPlaybackSession::CmPrepareResponse - \
SendGetTransportInfoActionL failed with code %d", aErr  );
            iCommandMessage->Complete( aErr );
            delete iCommandMessage; iCommandMessage = NULL;
            }

        __LOG3( "CUPnPPlaybackSession::CmPrepareResponse - \
AVTid = %d, RCid = %d, connectionid = %d", aTransport, aRsc, aConnection );
        }
    else
        {
        // Something wrong with the action. Complete the message with error
        // code. No further processing can be dome.
        __LOG1( "CUPnPPlaybackSession::CmPrepareResponse - \
action failed with code %d", aErr  );

        delete iCurrentItem; iCurrentItem = NULL;
        delete iCurrentUri; iCurrentUri = NULL; 

        iCommandMessage->Complete( aErr );
        delete iCommandMessage; iCommandMessage = NULL;        
        } 
    
    __LOG( "CUPnPPlaybackSession::CmPrepareResponse - end" );
    }


// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RcLastChangeEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RcLastChangeEvent(
    const TDesC8& aLastChange )
    {
    if( iPlaybackState != EHalted )
        {        
        __LOG( "CUPnPPlaybackSession::RcLastChangeEvent" );
        CUPnPAVTEvent* event = NULL;
        TRAPD(  err, event = iEventParser->ParseRcEventDataL( aLastChange,
                iRCInstanceId ) );
                
        if( err == KErrNone )
            {
            TUnsolicitedEventC unsolicitEvent;
            TInt volume = event->Volume();
            TInt mute = event->Mute();
            // check if it Mute state change
             if( EUnknown != (TMuteState)mute  &&  
                     iMuteState != (TMuteState)mute)
                {
                unsolicitEvent.iEvent = EMute;
                unsolicitEvent.iValue = (TInt)mute;
                iMuteState = (TMuteState)mute;
                PropagateEvent( unsolicitEvent );
                }
             // check if it has valid volume information.
             if (volume >= 0 )  
                {
                // Scale the volume level
                // Get device's maximum volume value
                TInt maxVolume = iDevice->MaxVolume();
                // If max volume not KMaxVolume
                if( maxVolume != KMaxVolume )
                    {
                    // Convert volume to match max volume 100
                    TReal tempVolumeLevel = volume;
                    TReal tempMaxVolume = maxVolume;
                    
                    volume = KMaxVolume * 
                                tempVolumeLevel/tempMaxVolume;
                    }            
                if( iVolume != volume)
                    {
                    // State of volume changed, create an event and send it
                    // to the client side
                    unsolicitEvent.iEvent = EVolume;
                    unsolicitEvent.iValue = volume;
                    iVolume = volume;
                    PropagateEvent( unsolicitEvent );                           
                    }
                }
            }
        delete event; 
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::AvtLastChangeEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AvtLastChangeEvent(
    const TDesC8& aLastChange )
    {
    __LOG2( "CUPnPPlaybackSession::AvtLastChangeEvent, pb state %d, exp evt %d", 
        iPlaybackState, iExpectedEvent );

    CUPnPAVTEvent* avtevent = NULL;
    TRAPD( err, avtevent = iEventParser->ParseAvtEventDataL( aLastChange, 
           iAVTInstanceId ) );
    if( err == KErrNone && iPlaybackState != EHalted )
        {
        CUPnPAVTEvent::TTransportState transportState = avtevent->TransportState();
		PropagateState( transportState );
		
        if( avtevent->TransportURI().Length() > 0 )
            {
            AVTransportUriEventReceived( avtevent->TransportURI(),
                avtevent->TransportState() );
            }
        else
            {     
            iInitialEventReceived = EFalse;
            switch( transportState )
                {
                case CUPnPAVTEvent::EPlaying:
                    {
                    PlayingEventReceived();
                    break;
                    }     
                case CUPnPAVTEvent::EStopped:
                    {         
                    StoppedEventReceived();
                    break;
                    }            
                case CUPnPAVTEvent::EPausedPlayback:
                    {
                    PausedEventReceived();
                    break;
                    }
                case CUPnPAVTEvent::ENoMediaPresent:
                    {
                    NoMediaEventReceived();
                    break;
                    }
                default:
                    {
                    __LOG( "CUPnPPlaybackSession::AvtLastChangeEvent - \
TRANSITIONING, IDLE, RECORDING or PAUSED_RECORDING" );    
                    break;
                    }   
                }
            }
        iPreviousTransportState = transportState;
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::AvtLastChangeEvent - We are halted, or \
parsing failed. No can do." );
        }
    delete avtevent;
    __LOG( "CUPnPPlaybackSession::AvtLastChangeEvent" );
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

    __ASSERT( !iEventMessage, __FILE__, __LINE__ );

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
            if( iDevice->SubscriptionCount() )
                {
                __LOG( "CUPnPPlaybackSession::EventRequestL - Subscription \
has been made already, set state to no media." );
                // Some session has already subscribed, so we will not
                // receive the initial state event. Therefore set our state
                // to no media.
                iPlaybackState = ENoMedia;
                }
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
    
    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );
    
    ResetL();
        
    // Read and store the item and URI from the request
    CUpnpAVRequest* tmpRequest = CUpnpAVRequest::NewLC();
    ReadReqFromMessageL( aMessage, 1 ,tmpRequest );
    
    CUpnpItem* tmpItem = CUpnpItem::NewL();
    CleanupStack::PushL( tmpItem );

    ReadObjFromMessageL( aMessage, 2 ,tmpItem );

    HBufC8* tmpUri = tmpRequest->URI().AllocL();
    
    CleanupStack::Pop( tmpItem );
    CleanupStack::PopAndDestroy( tmpRequest );
    
    delete iCurrentItem;
    iCurrentItem = tmpItem;

    delete iCurrentUri;
    iCurrentUri = tmpUri;

    // Get protocolInfo from the item
    const CUpnpElement& res = 
        UPnPItemUtility::ResourceFromItemL( *iCurrentItem );
    const CUpnpAttribute* protocolInfo = 
        UPnPItemUtility::FindAttributeByName( 
        res, KAttributeProtocolInfo );

    // Store the message. We are going to perfom asynchronous operation
    // so it's needed (completed) afterwards
    iCommandMessage = new (ELeave) RMessage2( aMessage );     

    // Check if we have a valid connection (or if it's even needed.
    // Anyway,that does not matter since if no connection, we can used
    // default instance id)    
    if( CheckConnectionL( protocolInfo->Value() ) )
        {
        // Connection ok, continue with sending AvtSetTransportUri-action
        __LOG( "CUPnPPlaybackSession::SetURIL - \
connection ok, or using default" );
        
        SendGetTransportInfoActionL();      
        }
    else
        {
        // Connection needs to be established
        CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
                CpDeviceL(), KConnectionManager, KPrepareForConnection );
        
        action->SetArgumentL( KRemoteProtocolInfo, protocolInfo->Value() );
	    action->SetArgumentL( KPeerConnectionManager, KConnectioMgr );
	    action->SetArgumentL( KPeerConnectionID, KDefaultPeerConnectionID );
	    action->SetArgumentL( KDirection, KInput );

        iServer.ControlPoint().SendL( action );
        CleanupStack::Pop( action );
        if (action->SessionId() < 0) User::Leave( action->SessionId() );
        iIPSessionIdCommand = action->SessionId();
        iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
        }

    // Store protocolinfo, so it can be checked in PrepareConnectionL when
    // a next item is played 
    CUpnpDlnaProtocolInfo* tmpInfo = 
         CUpnpDlnaProtocolInfo::NewL( protocolInfo->Value() );

    delete iPInfoForPrevious;    
    iPInfoForPrevious = tmpInfo;         
 
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
        //iSetUri = EFalse;
        iExpectedEvent = EEventNone;
        iPlaybackState = EStopped;
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
void CUPnPPlaybackSession::SetNextURIL( const RMessage2& /*aMessage*/ )
    {
    __LOG( "CUPnPPlaybackSession::SetNextURIL" );
 
    User::Leave( KErrNotSupported );
 
    __LOG( "CUPnPPlaybackSession::SetNextURIL - end" );
   }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSetNextURIL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSetNextURIL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSetNextURIL" );

    User::Leave( KErrNotSupported );   
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PlayL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PlayL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::PlayL" );

    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    
    if( !(iPlaybackState == EPaused || iPlaybackState == EStopped) )
        {
        User::Leave( KErrNotReady );
        }

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KPlay );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KSpeed, KNormalSpeed );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    iIPSessionIdCommand = action->SessionId();
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    iExpectedEvent = EEventPlaying;
    
    __LOG( "CUPnPPlaybackSession::PlayL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelPlayL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelPlayL()
    {
    __LOG( "CUPnPPlaybackSession::CancelPlayL" );
    
    //__ASSERT( iCommandPending, User::Panic( KPanicText, __LINE__ ) );
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;
        
        iExpectedEvent = EEventNone;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::StopL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::StopL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::StopL" );
    
    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    // state stopped must be check before stopped action. According to
    // AVTransPort specification stop may be requested in any other state
    // except NO_MEDIA_PRESENT
    if( iPlaybackState == ENoMedia )
        {
        User::Leave( KErrNotReady );
        }
    else
        {
        __LOG1( "CUPnPPlaybackSession::StopL - sending stop in state %d",
            iPlaybackState);
        }

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KStop );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );

    iServer.ControlPoint().SendL( action ); //action ownership is transfered
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdCommand = action->SessionId();
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    iCommandMessage = new (ELeave) RMessage2( aMessage );

    iExpectedEvent = EEventStopped;
    __LOG( "CUPnPPlaybackSession::StopL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::EmergencyStopL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::EmergencyStopL()
    {
    __LOG( "CUPnPPlaybackSession::EmergencyStopL" );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KStop );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    __LOG( "CUPnPPlaybackSession::StopL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PropagateState
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PropagateState( 
    CUPnPAVTEvent::TTransportState aTransportState )
    {
    __LOG2( "CUPnPPlaybackSession::PropagateState - curState %d, prevState %d",
        aTransportState, iPreviousTransportState );
        
    if( aTransportState == CUPnPAVTEvent::ETransitioning )
        {
        __LOG( "CUPnPPlaybackSession::PropagateState - \
TRANSITIONING" );
        TUnsolicitedEventC event;
        event.iEvent = ETransition;
        event.iValue = ETransitionEnter; 
        PropagateEvent( event );
        }
    else if( aTransportState != CUPnPAVTEvent::ETransitioning &&
        iPreviousTransportState == CUPnPAVTEvent::ETransitioning )
        {
        __LOG( "CUPnPPlaybackSession::PropagateState - \
TRANSITIONING ended" );    
        TUnsolicitedEventC event;
        event.iEvent = ETransition;
        event.iValue = ETransitionExit; 
        PropagateEvent( event );
        }
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
        
        iExpectedEvent = EEventNone;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PauseL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PauseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::PauseL" );
    
    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();
    
    if( !(iPlaybackState == EPlaying)  )
        {
        User::Leave( KErrNotReady );
        }

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KPause );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdCommand = action->SessionId();
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    iCommandMessage = new (ELeave) RMessage2( aMessage );

    iExpectedEvent = EEventPaused;
    
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
        
        iExpectedEvent = EEventNone;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SetVolumeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SetVolumeL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SetVolumeL" );                
    
    __ASSERT( !iSettingMessage, __FILE__, __LINE__ );

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

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KRenderingControl, KSetVolume );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( KDefaultInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KChannel, KMasterVolume );
    buf.Num( volume );
    action->SetArgumentL( KDesiredVolume, buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdSetting = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
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
    
    __ASSERT( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KRenderingControl, KGetVolume );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( KDefaultInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KChannel, KMasterVolume );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdSetting = action->SessionId();
    
    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
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
    
    __ASSERT( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KRenderingControl, KSetMute );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( KDefaultInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KChannel, KMasterVolume );
    if ( aMessage.Int1() )
        {
        action->SetArgumentL( KDesiredMute, KMuteOn );
        }
    else
        {
        action->SetArgumentL( KDesiredMute, KMuteOff );
        }

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdSetting = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
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
    
    __ASSERT( !iSettingMessage, __FILE__, __LINE__ );

    ResetL();

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KRenderingControl, KGetMute );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( KDefaultInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KChannel, KMasterVolume );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdSetting = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdSetting, *this );
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
    
    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KGetPositionInfo );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdCommand = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelGetPositionInfoL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelGetPositionInfoL()
    {
    __LOG( "CUPnPPlaybackSession::CancelGetPositionInfoL" );                
    
    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SeekRelTimeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SeekRelTimeL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::SeekRelTimeL" );                
    
    __ASSERT( !iCommandMessage, __FILE__, __LINE__ );

    ResetL();

    // Get the seek target value from aMessage parameter
    // create buffer
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr ) );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KSeek );
    
    TBuf8<KMaxIntLength> bufInt;
    bufInt.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, bufInt );
    action->SetArgumentL( KUnit, KRel_Time );
    action->SetArgumentL( KTarget, *buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionIdCommand = action->SessionId();

    CleanupStack::PopAndDestroy( buf );
    
    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    iCommandMessage = new (ELeave) RMessage2( aMessage );
    }
    
// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CancelSeekRelTimeL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::CancelSeekRelTimeL()
    {
    __LOG( "CUPnPPlaybackSession::CancelSeekRelTimeL" );                

    if( iCommandMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iCommandMessage->Complete( KErrCancel );
        delete iCommandMessage; 
        iCommandMessage = NULL;            
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::GetRendererStateL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::GetRendererStateL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPPlaybackSession::GetRendererStateL" );
    
    if( iPlaybackState == EStopped || iPlaybackState == ENoMedia )
        {
        __LOG( "CUPnPPlaybackSession::GetRendererStateL - \
Stopped or no_media" );
        TUnsolicitedEventE event( EStop );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = aMessage.Write( 1, resp1 );
        aMessage.Complete( err );
        }
    else if( iPlaybackState == EPlaying )
        {
        __LOG( "CUPnPPlaybackSession::GetRendererStateL - Playing" );
        TUnsolicitedEventE event( EPlay );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = aMessage.Write( 1, resp1 );
        aMessage.Complete( err );
        }
    else if( iPlaybackState == EPaused )
        {
        __LOG( "CUPnPPlaybackSession::GetRendererStateL - Paused" );
        TUnsolicitedEventE event( EPause );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = aMessage.Write( 1, resp1 );
        aMessage.Complete( err );  
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::GetRendererStateL - Waiting.." );
        // Waiting the initial event
        __ASSERT( !iInitialEventMsg, __FILE__, __LINE__ );
        iTimer->Start( KTimerCycle10 );
        iInitialEventMsg = new (ELeave) RMessage2( aMessage );
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
    
    __ASSERT( !iDeviceMessage, __FILE__, __LINE__ );
    
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
    
    __LOG( "CUPnPPlaybackSession::ValidateProtocolInfoL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CheckConnectionL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
TBool CUPnPPlaybackSession::CheckConnectionL( 
    const TDesC8& aProtocolInfo )
    {
    __LOG( "CUPnPPlaybackSession::CheckConnectionL" );
    
    // 1.   Check that is CM:PrepareForConnection supported. If not, return
    //      true. This means that we don't need to establish a new connection
    //      (we can just used instance id 0)
    // 2.   If PrepareForConnection is supported, check that do have a
    //      connection already (!iPInfoForPrevious). If not, return false
    //      (a new connection is needed). If we have a connection, check that
    //      does the type of content change. If it changes, a new connection
    //      is needed.
    
    if( iDevice->PrepareForConnection() )
        {
        __LOG( "CUPnPPlaybackSession::CheckConnectionL - \
CM:PrepareForConnection supported" );

        if( iPInfoForPrevious && ( iConnectionId != KErrNotFound ) )
            {
            CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
                    aProtocolInfo );
            
            if( tmpInfo->SecondField() == iPInfoForPrevious->SecondField() &&
                tmpInfo->PnParameter() == iPInfoForPrevious->PnParameter() )
                {
                __LOG( "CUPnPPlaybackSession::CheckConnectionL - \
same as previous" );
                delete tmpInfo;
                return ETrue;
                }
            else
                {
                __LOG( "CUPnPPlaybackSession::CheckConnectionL - \
media type changes, a new connection is needed" );
                // Close existing. Ignore error code (we can do nothing
                // useful in error case)
                CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
                        CpDeviceL(), KConnectionManager, KConnectionComplete );
                
                TBuf8<KMaxIntLength> buf;
                buf.Num( iConnectionId );
                action->SetArgumentL( KConnectionID, buf );

                iServer.ControlPoint().SendL( action );
                CleanupStack::Pop( action );

                iConnectionId = KErrNotFound;
                iAVTInstanceId = KDefaultInstanceId;  
                iRCInstanceId = KDefaultInstanceId;
                delete tmpInfo;
                return EFalse;                
               }
            }
        else
            {
            // No existing connection, a new connection is needed
            return EFalse;
            }
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::CheckConnectionL - \
CM:PrepareForConnection not supported" );
        
        // CM:PrepareForConnection is not supported
        return ETrue;
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SendAVTransportUriActionL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SendAVTransportUriActionL()
    {
    __LOG( "CUPnPPlaybackSession::SendAVTransportUriActionL" );
    
    const CUpnpElement& res = 
        UPnPItemUtility::ResourceFromItemL( *iCurrentItem );
    const CUpnpAttribute* protocolInfo = 
        UPnPItemUtility::FindAttributeByName( 
        res, KAttributeProtocolInfo );    
    
    TPtrC8 uri( *iCurrentUri );
    
    if( !iDevice->MatchSinkProtocolInfo( protocolInfo->Value() ) )
        {
        // Did not match, try to find a match
        TRAPD( err, uri.Set( iDevice->FindFirstMatchingInSinkL(
            *iCurrentItem ) ) );
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
           
   if( !iDevice->DlnaCompatible() )
        {        
        // down grade to upnpitem
        RemoveDlnaFlagsFromResElementsL();
        }          
    else if( ( iDevice->DLNADeviceType() != CUpnpAVDeviceExtended::EDMR ) ||
        ( !iDevice->MatchSinkProfileId( protocolInfo->Value() ) && 
           iDevice->MatchSinkMime( protocolInfo->Value()) ) )
        {
        // remove DLNA profile
        RemoveDlnaProfileFromResElementsL();
        }
    
    // Create metadata xml document
    HBufC8* xmlDoc = CUPnPXMLParser::ItemAsXmlLC( *iCurrentItem );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KSetAVTransportURI );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );
    action->SetArgumentL( KCurrentURI, uri );
    action->SetArgumentL( KCurrentURIMetaData, *xmlDoc );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave(action->SessionId());
    iIPSessionIdCommand = action->SessionId();

    CleanupStack::PopAndDestroy( xmlDoc );      
                                      
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );
    
    // Set playback state artificially to no media. This way we can recognize
    // if renderer has responded with CurrentTrackUri
    iPlaybackState = ENoMedia;
    iExpectedEvent = EEventAVTransportUri;
 
    __LOG( "CUPnPPlaybackSession::SendAVTransportUriActionL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RemoveDlnaFlagsFromResElementL
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RemoveDlnaFlagsFromResElementsL()
    {
    __LOG( "CUPnPPlaybackSession::RemoveDlnaFlagsFromResElementL" );
    RUPnPElementsArray& elements = const_cast<RUPnPElementsArray&>(
                                                iCurrentItem->GetElements());

    for( TInt resIndex(0); resIndex < elements.Count(); ++resIndex )
        {
        if( elements[resIndex]->Name() == KElementRes() )
            {
            const RUPnPAttributesArray& array = 
                                    elements[resIndex]->GetAttributes();
            CUpnpElement* elem = CUpnpElement::NewLC( KElementRes() );
            for( TInt i = 0; i < array.Count(); i++ )
                {
                _LIT8( KProtocolInfo, "protocolInfo" );
                _LIT8( KDlnaOrg, "DLNA.ORG" );
                
                if( array[ i ]->Name() == KProtocolInfo() )
                    {
                    // remove dlna stuff from protocolinfo
                    TPtrC8 protValue( array[ i ]->Value() );
                    TInt index = protValue.Find( KDlnaOrg() );
                    __LOG1( "CUPnPPlaybackSession::RemoveDlnaFlagsFromRes\
                    ElementL dlnaflags found protinfo index = %d",index );
                    if( index > 0 )
                        {
                        _LIT8( KWildCard, ":*" );
                        CUpnpAttribute* attribute = CUpnpAttribute::NewLC( 
                                                     array[ i ]->Name() );
                        HBufC8* tmp = HBufC8::NewLC( 
                        protValue.Mid(0, index-1).Length() +
                        KWildCard().Length() );
                        
                        tmp->Des().Copy( protValue.Mid(0, index-1) );
                        tmp->Des().Append( KWildCard() );
                        
                        attribute->SetValueL( *tmp );
                        CleanupStack::PopAndDestroy( tmp );                                                
                        elem->AddAttributeL(attribute);
                        CleanupStack::Pop( attribute );
                        }
                    else
                        {
                        // if item was allready down graded to upnpitem
                        // clean and break from here
                        CleanupStack::PopAndDestroy( elem );
                        elem = NULL;
                        break;
                        }
                    }
                else
                    {
                    CUpnpAttribute* attribute = CUpnpAttribute::NewLC( 
                                                 array[ i ]->Name() );
                    attribute->SetValueL( array[ i ]->Value() );
                    elem->AddAttributeL(attribute);
                    CleanupStack::Pop( attribute );
                    }
                }     
            if( elem )
                {
                elem->SetValueL( elements[resIndex]->Value() );
                CleanupStack::Pop( elem );
                
                if( elements.Insert( elem, resIndex ) )
                    {
                    delete elem;
                    continue;
                    }
                
                delete elements[++resIndex];
                elements.Remove(resIndex);                    

                __LOG( "CUPnPPlaybackSession::RemoveDlnaFlagsFromResElementL\
                res found and replaced protocol info without dlna stuff" );
                }
            }
        }
    __LOG( "CUPnPPlaybackSession::RemoveDlnaFlagsFromResElementL end" );
    }
    
    // --------------------------------------------------------------------------
// CUPnPPlaybackSession::RemoveDlnaProfileFromResElementsL
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RemoveDlnaProfileFromResElementsL()
    {
    __LOG( "CUPnPPlaybackSession::RemoveDlnaProfileFromResElementsL" );
    RUPnPElementsArray& elements = const_cast<RUPnPElementsArray&>(
                                                iCurrentItem->GetElements());

    for( TInt resIndex(0); resIndex < elements.Count(); ++resIndex )
        {
        if( elements[resIndex]->Name() == KElementRes() )
            {
            const RUPnPAttributesArray& array = 
                                    elements[resIndex]->GetAttributes();
            CUpnpElement* elem = CUpnpElement::NewLC( KElementRes() );
            for( TInt i = 0; i < array.Count(); i++ )
                {
                _LIT8( KProtocolInfo, "protocolInfo" );
                _LIT8( KDlnaOrgPn, "DLNA.ORG_PN=" );
                _LIT8( KDlnaOrgPnEnd, ";" );
                
                if( array[ i ]->Name() == KProtocolInfo() )
                    {
                    // remove dlna profile from protocolinfo
                    TPtrC8 protValue( array[ i ]->Value() );
                    TInt length = 0;
                    TInt index = 0;
                    
                    __LOG8_1("CUPnPPlaybackSession::RemoveDlnaProfileFromRes protocol info = %S", &protValue);
                    
                    index = protValue.Find( KDlnaOrgPn() );
                    if (index != KErrNotFound)
                        {
                        length = protValue.Mid(index).Find( KDlnaOrgPnEnd() );
                        }
                    __LOG1( "CUPnPPlaybackSession::RemoveDlnaProfileFromRes ElementsL DLNA profile found protinfo index = %d", index );
                    if( index > 0 && length > 0 )
                        {                        
                        ++length;    
                        
                        CUpnpAttribute* attribute = CUpnpAttribute::NewLC( 
                                                     array[ i ]->Name() );
                        HBufC8* tmp = HBufC8::NewLC( 
                        protValue.Length() - 
                        protValue.Mid( index, length ).Length() );
                        
                        tmp->Des().Copy( protValue.Mid ( 0, index ) );                        
                        tmp->Des().Append( protValue.Mid( index + length ) );
                                              
                        attribute->SetValueL( *tmp );
                        
                        CleanupStack::PopAndDestroy( tmp );                                                
                        elem->AddAttributeL(attribute);
                        CleanupStack::Pop( attribute );
                        }
                    else
                        {
                        // just clean if profile not found
                        CleanupStack::PopAndDestroy( elem );
                        elem = NULL;
                        break;
                        }
                    }
                else
                    {
                    CUpnpAttribute* attribute = CUpnpAttribute::NewLC( 
                                                 array[ i ]->Name() );
                    attribute->SetValueL( array[ i ]->Value() );
                    elem->AddAttributeL(attribute);
                    CleanupStack::Pop( attribute );
                    }
                }     
            if( elem )
                {
                elem->SetValueL( elements[resIndex]->Value() );
                CleanupStack::Pop( elem );
                
                if( elements.Insert( elem, resIndex ) )
                    {
                    delete elem;
                    continue;
                    }
                
                delete elements[++resIndex];
                elements.Remove(resIndex);                    

                __LOG( "CUPnPPlaybackSession::RemoveDlnaProfileFromResElementsL\
                res found and replaced protocol info without dlna profile" );
                }
            }
        }
    __LOG( "CUPnPPlaybackSession::RemoveDlnaProfileFromResElementsL end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SendGetMediaInfoActionL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SendGetMediaInfoActionL()
    {
    __LOG( "CUPnPPlaybackSession::SendGetMediaInfoActionL" );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KGetMediaInfo );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave(action->SessionId());
    iIPSessionIdCommand = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );

    __LOG( "CUPnPPlaybackSession::SendGetMediaInfoActionL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::SendGetTransportInfoActionL
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::SendGetTransportInfoActionL()
    {
    __LOG( "CUPnPPlaybackSession::SendGetTransportInfoActionL" );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            CpDeviceL(), KAVTransport, KGetTransportInfo );
    
    TBuf8<KMaxIntLength> buf;
    buf.Num( iAVTInstanceId );
    action->SetArgumentL( KInstanceID, buf );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave(action->SessionId());
    iIPSessionIdCommand = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionIdCommand, *this );

    __LOG( "CUPnPPlaybackSession::SendGetTransportInfoActionL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PlayingEventReceived
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PlayingEventReceived()
    {
    __LOG( "CUPnPPlaybackSession::PlayingEventReceived" );
    
    if( iExpectedEvent == EEventPlaying )
        {
        // We were expecting a state change from stopped to playing since we
        // sent the play action.        
        __LOG( "CUPnPPlaybackSession::PlayingEventReceived - \
Response to play" );
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iTimer->Cancel();
        // Unregister from the dispatcher. This way we do not get 
        // ::AvtPlayResponse callback (it would be ignored, anyway)        
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;         
        iCommandMessage->Complete( EAVControllerPlayCompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        }
    else if( iPlaybackState == EPaused  )
        {
        // Unsolicited play event from the renderer. This means that device
        // is playing. Propagate the event.
        __LOG( "CUPnPPlaybackSession::PlayingEventReceived - \
Unsolicited play" );
        TUnsolicitedEventC event;
        event.iEvent = EPlay; event.iValue = KErrNone;
        PropagateEvent( event );
        }
    else if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::PlayingEventReceived - \
initial event" );
        iTimer->Cancel();
        TUnsolicitedEventE event( EPlay );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        iInitialEventReceived = ETrue;
        }
    else if( iPlaybackState == EPlaying || iPlaybackState == EStopped)
        {            
        // Unsolicited play event when we already were in playing state
        // or already swithced to stopped state due to previous unsolicited play    
        // - most probably someone else has issued play command, check current URI
        __LOG( "CUPnPPlaybackSession::PlayingEventReceived - \
Unsolicited play event, check if renderer is hijacked" );
            
        TRAPD( aErr, SendGetMediaInfoActionL() )
        if( aErr != KErrNone )
            {
            __LOG1( "CUPnPPlaybackSession::CmPrepareResponse - \
            SendGetMediaInfoActionL failed with code %d", aErr  );             
            }
        else
            {
            iCheckForHijackedRenderer = ETrue;
            }
        }
    
    iPlaybackState = EPlaying;  
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::StoppedEventReceived
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::StoppedEventReceived()
    {
    __LOG( "CUPnPPlaybackSession::StoppedEventReceived" );
        
    if( iExpectedEvent == EEventStopped )
        {
        // We were expecting a state change from playing/paused to stopped
        // since we sent the stop action.
        RespondToStopRequest();
        }
    else if( iPlaybackState == EPlaying ||
             iPlaybackState == EPaused )
        {
        // Unsolicted stop event from the renderer. This means that playback
        // stopped. Propagate the event.
        __LOG( "CUPnPPlaybackSession::StoppedEventReceived - \
Unsolicted stop" );
        TUnsolicitedEventC event;
        event.iEvent = EStop; event.iValue = KErrNone; 
        PropagateEvent( event );
        }
    else if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::StoppedEventReceived - \
initial event" );
        iTimer->Cancel();
        TUnsolicitedEventE event( EStop );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        }
    iPlaybackState = EStopped;
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PausedEventReceived
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PausedEventReceived()
    {
    __LOG( "CUPnPPlaybackSession::PausedEventReceived - \
PauseUser received" );
    
    if( iExpectedEvent == EEventPaused )
        {
        // We were expecting a state change from playing to paused
        // since we sent the pause action.
        __LOG( "CUPnPPlaybackSession::PausedEventReceived - \
Response to pause" ); 
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iTimer->Cancel();
        // Unregister from the dispatcher. This way we do not get 
        // ::AvtPauseResponse callback (it would be ignored, anyway)
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;        
        iCommandMessage->Complete( EAVControllerPauseCompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        }
    else if( iPlaybackState == EPlaying ) 
        {
        // Unsolicted pause event from the renderer. This means that playback
        // paused. Propagate the event.
        __LOG( "CUPnPPlaybackSession::PausedEventReceived - \
Unsolicted pause" );
        TUnsolicitedEventC event;
        event.iEvent = EPause; event.iValue = KErrNone;
        PropagateEvent( event );
        }
    else if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::PausedEventReceived - \
initial event" );
        iTimer->Cancel();
        TUnsolicitedEventE event( EPause );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        iInitialEventReceived = ETrue;
        }     
    iPlaybackState = EPaused; 
    }
void CUPnPPlaybackSession::NoMediaEventReceived()
    {
    __LOG( "CUPnPPlaybackSession::NoMediaEventReceived" );

    if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::NoMediaEventReceived - \
initial event" );
        iTimer->Cancel();
        TUnsolicitedEventE event( EStop );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        }
    else if( iExpectedEvent == EEventStopped )
        {
        // After reconnecting to renderer, a stop command may
        // result to no media present event. 
        RespondToStopRequest();
        }
    
    if( iPlaybackState == EStopped || iPlaybackState == EUninitialized )
        {
        iPlaybackState = ENoMedia;
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::NoMediaEventReceived - \
illegal NO_MEDIA_PRESENT -> ignore" );        
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::CurrentTrackUriEventReceived
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::AVTransportUriEventReceived( const TDesC8& aUri,
    CUPnPAVTEvent::TTransportState aTransportState )
    {
    __LOG8_1( "CUPnPPlaybackSession::AVTransportUriEventReceived, uri: %S",
            &aUri );
  
    if( iExpectedEvent == EEventAVTransportUri )
        {
        // We were expecting to get AVTransportUri from the renderer
        // since we sent the SetAVTransportUri - action
        __LOG( "CUPnPPlaybackSession::AVTransportUriEventReceived - \
complete seturi" );
           
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        // Unregister from the dispatcher. This way we do not get 
        // ::AvtSetUriResponse callback (it would be ignored, anyway)
        iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
        iIPSessionIdCommand = KErrNotFound;
        iTimer->Cancel();
        iCommandMessage->Complete( EAVControllerSetURICompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        iPlaybackState = EStopped;          
        }
    else if( iCurrentUri && ( *iCurrentUri != aUri ) )
        {
        __LOG( "CUPnPPlaybackSession::AVTransportUriEventReceived - \
different AVTransportUri - propagate a stop event" );  
        
        // Uri changes. Some malicous Control Point captured our device
        // or something else is wrong. Propagate a stop event with error
        // code KErrNotReady
        iPlaybackState = EHalted;
        TUnsolicitedEventC event;
        event.iEvent = EStop;
        event.iValue = KErrNotReady;
        PropagateEvent( event );
        }
    else if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::AVTransportUriEventReceived - \
initial event" );
        iTimer->Cancel();
        TUnsolicitedEventE event( EStop );
        iPlaybackState = EStopped;
        if( aTransportState == CUPnPAVTEvent::EPlaying )
            {
            event = EPlay;
            iPlaybackState = EPlaying;
            }
        else if( aTransportState == CUPnPAVTEvent::EPausedPlayback )
            {
            event = EPause;
            iPlaybackState = EPaused;
            }
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        }    
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::PropagateEvent
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::PropagateEvent( TUnsolicitedEventC event )
    {
    __LOG( "CUPnPPlaybackSession::PropagateEvent" );
    
    if( iEventMessage )
        {
        TPckg<TUnsolicitedEventC> resp1( event );
        TInt err = iEventMessage->Write( 1, resp1 );
        iEventMessage->Complete( err ); 
        delete iEventMessage; iEventMessage = NULL;
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::PropagateEvent - add event to queu" );
        iEventQue.Append( event );
        }                                        
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::UPnPAVTimerCallback
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::UPnPAVTimerCallback(
    CUPnPAVTimer::TAVTimerType /*aType*/ )
    {
    __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback" );

    // Fail safe timer expired. Check what event we were expecting
    if( iExpectedEvent == EEventAVTransportUri )
        {   
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - \
CurrentTrackUri" );
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iCommandMessage->Complete( EAVControllerSetURICompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        iPlaybackState = EStopped;                       
        }
    else if( iExpectedEvent == EEventStopped )
        {
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - Stop" );
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iCommandMessage->Complete( EAVControllerStopCompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        iPlaybackState = EStopped;                  
        }
    else if( iExpectedEvent == EEventPlaying )
        {
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - Play" );
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iCommandMessage->Complete( EAVControllerPlayCompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        iPlaybackState = EPlaying;                          
        }
    else if( iExpectedEvent == EEventPaused )
        {
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - Pause" );
        __ASSERT( iCommandMessage, __FILE__, __LINE__ );
        iCommandMessage->Complete( EAVControllerPauseCompleted );
        delete iCommandMessage; iCommandMessage = NULL;
        iExpectedEvent = EEventNone;
        iPlaybackState = EPaused;                          
        }
    else if( iInitialEventMsg )
        {
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - initial event" );
        TUnsolicitedEventE event( EStop );
        TPckg<TUnsolicitedEventE> resp1( event );
        TInt err = iInitialEventMsg->Write( 1, resp1 );
        iInitialEventMsg->Complete( err );        
        delete iInitialEventMsg; iInitialEventMsg = NULL; 
        }
    else
        {
        __LOG( "CUPnPPlaybackSession::UPnPAVTimerCallback - Not Expected!" );
        __PANIC( __FILE__, __LINE__ );
        }
    }

// --------------------------------------------------------------------------
// CUPnPPlaybackSession::RespondToStopRequest
// See upnpplaybacksession.h
// --------------------------------------------------------------------------
void CUPnPPlaybackSession::RespondToStopRequest()
    {
    __LOG( "CUPnPPlaybackSession::RespondToStopRequest - \
Response to stop" );
    __ASSERT( iCommandMessage, __FILE__, __LINE__ );
    iTimer->Cancel();
    // Unregister from the dispatcher. This way we do not get 
    // ::AvtStopResponse callback (it would be ignored, anyway)        
    iServer.Dispatcher().UnRegister( iIPSessionIdCommand );
    iIPSessionIdCommand = KErrNotFound;        
    iCommandMessage->Complete( EAVControllerStopCompleted );
    delete iCommandMessage; iCommandMessage = NULL;
    iExpectedEvent = EEventNone;
    }

// end of file
