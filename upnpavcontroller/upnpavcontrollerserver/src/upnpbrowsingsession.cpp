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
* Description:      server impl. of session against media server
*
*/






// INCLUDE FILES
// System
#include <utf.h> 
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <centralrepository.h>

// upnp stack api
#include <upnpstring.h>
#include <upnpdevice.h>
#include <upnpcontainer.h>
#include <upnpservice.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpmediaserversettings.h>
#include <upnpmediaserverclient.h>
#include <upnpmediaservernotifier.h>
#include <upnpfiletransferevent.h>

#include <upnpavcontrolpoint.h>

// upnpframework / avcontroller api
#include "upnpavrenderingsessionobserver.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpfileutility.h"
#include "upnpitemutility.h"

// upnpframework / xml parser api
#include "upnpxmlparser.h"

// upnpframework / internal api's
#include "upnpcontentservercrkeys.h"
#include "upnpcommonutils.h"
#include "upnpcdsreselementutility.h"
#include "upnpmetadatafetcher.h"

// avcontroller internal
#include "upnpfilesharingactive.h"
#include "upnpavdispatcher.h"
#include "upnpavbrowserequest.h"
#include "upnpavrequest.h"
#include "upnpaverrorhandler.h"
#include "upnpavdeviceextended.h"
#include "upnpdevicerepository.h"
#include "upnpavbrowserespparams.h"
#include "upnpavcontrollerserver.h"
#include "upnpbrowsingsession.h"

// CONSTANTS
_LIT8( KDirectChildren,         "BrowseDirectChildren" );
_LIT8( KMetaData,               "BrowseMetadata" );
_LIT8( KImportUri,              "importUri" );
_LIT8( KCreateClass,            "upnp:createClass" );
_LIT8( KBrowseMetadata,         "BrowseMetadata" );
const TInt KDefaultInstanceId   = 0;
const TInt KExpectedCount       = 1;

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::NewL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession* CUPnPBrowsingSession::NewL
    (
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer,
    TInt aSessionId,
    const TDesC8& aUuid
    )
    {
    CUPnPBrowsingSession* self = new (ELeave) CUPnPBrowsingSession(
        aClient, aServer, aSessionId );
    CleanupStack::PushL( self );    
    
    self->ConstructL( aUuid );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CUPnPBrowsingSession
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession::CUPnPBrowsingSession
    (
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer,
    TInt aSessionId
    ):
    iMediaServer( aClient ),
    iServer( aServer ),
    iSessionId( aSessionId ),
    
    iInstanceId( KDefaultInstanceId ),
    iIPSessionId( KErrNotFound ),

    iTransferId( KErrNotFound ),

    iTransferEventReceived( EFalse ),
    iInternalState( ENone ),
    iCopyState( EIdle )
    {
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::~CUPnPBrowsingSession
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession::~CUPnPBrowsingSession()
    {
    iIPSessionId = KErrNotFound;
    
    delete iDevice;
    delete iRespBuf;
    delete iRespBuf2;
    
    delete iImportURI;
    delete iSourceURI;
    
    delete iItemId;
    
    delete iContainerId;
    
    delete iSharedItem;
    
    delete iMediaServerNotifier;
    
    delete iFileSharing;      
    
    delete iLocalMediaServerUuid; 
    
    delete iOriginalLocation;
    
    delete iFilePath;
    
    delete iMSSettings; 
    delete iAppSettings;
    
    delete iActionMessage;
    delete iDeviceMessage;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ConstructL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPBrowsingSession::ConstructL" );
    
    iMediaServerNotifier = CUpnpMediaServerNotifier::NewL( this );
    iFileSharing = CUPnPFileSharingActive::NewL();

    // Get the local Media Server Uuid, if available
    const RPointerArray<CUpnpAVDeviceExtended>& devList =
        iServer.DeviceRepository().DeviceList();
    TInt count = devList.Count();
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( devList[ i ]->Local() )
            {
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
        if( aUuid == KNullDesC8 ) // Fix to enable AV Controller helper usage
            {
            iDevice = CUpnpAVDeviceExtended::NewL();
            }
        else
            {
            User::Leave( KErrNotFound );    
            }    
        }
        
    iMSSettings = CUpnpMediaServerSettings::NewL();
    iAppSettings = CRepository::NewL( KCrUidUpnpContentserver );   
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RcSetVolumeResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::RcSetVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aDesiredVolume*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RcVolumeResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::RcVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aCurrentVolume*/)
    {
    // No implementation required                           
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RcSetMuteResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::RcSetMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aDesiredMute*/ )
    {
    // No implementation required   
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RcMuteResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::RcMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aCurrentMute*/ )
    {
    // No implementation required                      
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtSetTransportUriResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtSetTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aCurrentUri*/,
    const TDesC8& /*aCurrentUriMetaData*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtSetNextTransportUriResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtSetNextTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNextUri*/,
    const TDesC8& /*aNextUriMetaData*/)
    {
    // No implementation required        
    }
  
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtMediaInfoResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtMediaInfoResponse(
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
// CUPnPBrowsingSession::AvtGetTransportInfoResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtGetTransportInfoResponse(
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
// CUPnPBrowsingSession::AvtPositionInfoResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtPositionInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aTrack*/,
    const TDesC8& /*aTrackDuration*/,
    const TDesC8& /*aTrackMetaData*/,
    const TDesC8& /*aTrackURI*/,
    const TDesC8& /*aRelTime*/,
    const TDesC8& /*aAbsTime*/,
    const TDesC8& /*aRelCount*/,
    const TDesC8& /*aAbsCount*/)
    {
    // No implementation required                
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtDeviceCapabilitiesResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtDeviceCapabilitiesResponse(
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
// CUPnPBrowsingSession::AvtTransportSettingsResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtTransportSettingsResponse(
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
// CUPnPBrowsingSession::AvtStopResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtStopResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtPlayResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtPlayResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aSpeed*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtPauseResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtPauseResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtRecordResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtRecordResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtSeekResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtSeekResponse(
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
// CUPnPBrowsingSession::AvtNextResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtNextResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtPreviousResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtPreviousResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtSetPlayModeResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtSetPlayModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewPlayMode*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtSetRecordModeResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtSetRecordModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewRecordQuality*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSearchCapabilitiesResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSearchCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aSearchCaps)
    {
    __LOG1( "CUPnPBrowsingSession::CdsSearchCapabilitiesResponse: %d",
        aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    //iActionPending = EFalse;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            if(  aSearchCaps != KNullDesC8 )
                {
                delete iRespBuf; iRespBuf = NULL;
                iRespBuf = aSearchCaps.Alloc();
                if( iRespBuf )
                    {
                    TPckg<TInt> resp1( aSearchCaps.Length() );
                    iActionMessage->Write( 1, resp1 );
                    }
                else
                    {
                    TPckg<TInt> resp1( 0 );
                    iActionMessage->Write( 1, resp1 );
                    }
                
                iActionMessage->Complete(
                    EAVControllerGetSearchCapabilitiesSizeCompleted );      
                delete iActionMessage; iActionMessage = NULL;
                }
            else
                {
                TPckg<TInt> resp1( 0 );
                iActionMessage->Write( 1, resp1 );

                iActionMessage->Complete(
                    EAVControllerGetSearchCapabilitiesSizeCompleted );
                delete iActionMessage; iActionMessage = NULL;
                }    
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            }        
        }
    else
        {
        __LOG( "CdsSearchCapabilitiesResponse - no msg" );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSortCapabilitiesResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSortCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSortCaps*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSystemUpdateIdResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSystemUpdateIdResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aSystemUpdateId*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsBrowseResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsBrowseResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aObjectID*/,
    const TDesC8&  aBrowseFlag,
    const TDesC8&  /*aFilter*/,
    TInt /*aIndex*/,
    TInt /*aRequest*/,
    const TDesC8&  /*aSortCriteria*/,
    const TDesC8&  aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8&  aUpdateID )
    {
    __LOG1( "CUPnPBrowsingSession::CdsBrowseResponse: %d", aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    if( iActionMessage )
        {
        if (  aBrowseFlag.CompareF( KBrowseMetadata ) == 0 && aReturned == 0 
                && aErr != KErrCouldNotConnect && aErr != KErrHostUnreach )
            {
            aErr = ENoSuchObject;  //the file not exist;
            }
        
        if ( aErr != KErrCouldNotConnect && aErr != KErrHostUnreach )
            {
            aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );
            __LOG1( "CUPnPBrowsingSession::CdsBrowseResponse:001 %d", aErr );
            }    
    
        if( aErr == KErrNone )
            {
            if(  aResult != KNullDesC8 )
                {
                
                if( iInternalState == ECopyToPhone )
                    {
                    TRAP( aErr, SendCreateObjectActionL(
                        *iLocalMediaServerUuid, KContainerIdAny, aResult ) );
                    if( aErr )
                        {
                        iInternalState = ENone;
                        iActionMessage->Complete( aErr );
                        delete iActionMessage; iActionMessage = NULL;
                        }                    
                    }
                else if( iInternalState == ECopyLocal )
                    {
                    TRAP( aErr, CheckAndSendCreateObjectActionL( aResult ) );
                    if( aErr )
                        {
                        iInternalState = ENone;
                        iActionMessage->Complete( aErr );
                        delete iActionMessage; iActionMessage = NULL;
                        }
                    }
                else if( iInternalState == EDestroyObject )
                    {
                    TRAP( aErr, CheckAndSendDestroyObjectActionL( aResult) );
                    if( aErr )    
                        {
                        iInternalState = ENone;
                        iActionMessage->Complete( aErr );
                        delete iActionMessage; iActionMessage = NULL;
                        }
                    }    
                else // Browse
                    {
                    delete iRespBuf; iRespBuf = NULL;
                    delete iRespBuf2; iRespBuf2 = NULL;
                    iRespBuf = aResult.Alloc();
                    iRespBuf2 = aUpdateID.Alloc();
                         
                    if( iRespBuf && iRespBuf2 )
                        {
                        TUpnpAVBrowseRespParams params;
                        TPckg<TUpnpAVBrowseRespParams> resp2( params );
                        params.iMatches = aReturned ;
                        params.iTotalCount = aMatches;
                        params.iResponseSize = aResult.Length();
                        params.iUpdateIdSize = aUpdateID.Length();
                        iActionMessage->Write( 2, resp2 );
                        iActionMessage->Complete( 
                            EAVControllerGetBrowseResponseSizeCompleted );
                        }
                    else
                        {
                        delete iRespBuf; iRespBuf = NULL;
                        delete iRespBuf2; iRespBuf2 = NULL;
                        iActionMessage->Complete( KErrNoMemory );
                        }
                    iInternalState = ENone;
                    delete iActionMessage; iActionMessage = NULL;      
                    }    
                }
            else
                {
                if( iInternalState == EBrowse )
                    {
                    TUpnpAVBrowseRespParams params;
                    TPckg<TUpnpAVBrowseRespParams> resp2( params );
                    params.iMatches = 0;
                    params.iTotalCount = 0;
                    params.iResponseSize = 0;
                    params.iUpdateIdSize = 0;
                    iActionMessage->Write( 2, resp2 );

                    iInternalState = ENone;
                    iActionMessage->Complete(
                        EAVControllerGetBrowseResponseSizeCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                else
                    {
                    iInternalState = ENone;
                    iActionMessage->Complete( aErr );
                    delete iActionMessage; iActionMessage = NULL;
                    }        
                }    
            }
        else
            {
            iInternalState = ENone;
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            if ( KErrCouldNotConnect == aErr || KErrHostUnreach == aErr )
                {                 
                iServer.DeviceDisappearedL( aUuid );
                }
            }            
        }
    else
        {
        __LOG( "CdsBrowseResponse - no msg" );
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSearchResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSearchResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aContainerId*/,
    const TDesC8& /*aSearchCriteria*/,
    const TDesC8& /*aFilter*/,
    TInt /*aIndex*/,
    TInt /*aRequest*/,
    const TDesC8& /*aSortCriteria*/,
    const TDesC8& aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8& aUpdateID )
    {
    __LOG1( "CUPnPBrowsingSession::CdsSearchResponse: %d", aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    //iActionPending = EFalse;
    iIPSessionId = KErrNotFound;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            if(  aResult != KNullDesC8 )
                {
                delete iRespBuf; iRespBuf = NULL;
                delete iRespBuf2; iRespBuf2 = NULL;
                iRespBuf = aResult.Alloc();
                iRespBuf2 = aUpdateID.Alloc();

                if( iRespBuf && iRespBuf2 )
                    {
                    TUpnpAVBrowseRespParams params;
                    TPckg<TUpnpAVBrowseRespParams> resp2( params );
                    params.iMatches = aReturned ;
                    params.iTotalCount = aMatches;
                    params.iResponseSize = aResult.Length();
                    params.iUpdateIdSize = aUpdateID.Length();
                    iActionMessage->Write( 2, resp2 );                    
                    iActionMessage->Complete(
                         EAVControllerGetSearchResponseSizeCompleted );
                    }
                else
                    {
                    delete iRespBuf; iRespBuf = NULL;
                    delete iRespBuf2; iRespBuf2 = NULL;
                    iActionMessage->Complete( KErrNoMemory );
                    }
                delete iActionMessage; iActionMessage = NULL;       
                }
            else
                {
                TUpnpAVBrowseRespParams params;
                TPckg<TUpnpAVBrowseRespParams> resp2( params );
                params.iMatches = 0;
                params.iTotalCount = 0;
                params.iResponseSize = 0;
                params.iUpdateIdSize = 0;
                iActionMessage->Write( 2, resp2 );                    

                iActionMessage->Complete(
                    EAVControllerGetSearchResponseSizeCompleted );
                delete iActionMessage; iActionMessage = NULL;       
                }    
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;       
            }                                     
        }
    else
        {
        __LOG( "CdsSearchResponse - no msg" );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsDestroyObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsDestroyObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aObjectId*/ )
    {
    __LOG1( "CUPnPBrowsingSession::CdsDestroyObjectResponse: %d", aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    iInternalState = ENone;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            iActionMessage->Complete( EAVControllerDeleteObjectCompleted );
            delete iActionMessage; iActionMessage = NULL;
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            }            
        }
    else
        {
        __LOG( "CdsDestroyObjectResponse - no msg" );
        }      
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsUpdateObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsUpdateObjectResponse(
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
// CUPnPBrowsingSession::CdsImportResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsImportResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& aTransferId )
    {
    __LOG1( "CUPnPBrowsingSession::CdsImportResponse: %d", aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPContentDirectoryError );    
    
    if( aErr == KErrNone )
        {
        
        TLex8 lex( aTransferId );
        aErr = lex.Val( iTransferId );
        
        CopyFinished( aErr, EFalse );
        }
    else
        {
        CopyFinished( aErr, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsExportResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsExportResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& aTransferId )
    {
    __LOG1( "CUPnPBrowsingSession::CdsExportResponse: %d", aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPContentDirectoryError );    
    
    if( aErr == KErrNone )
        {
        TLex8 lex( aTransferId );
        aErr = lex.Val( iTransferId );        

        CopyFinished( aErr, EFalse );            
        }
    else
        {
                    
        CopyFinished( aErr, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsStopTransferResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsStopTransferResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aTransferId*/ )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsCTransferProgressResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsCTransferProgressResponse(
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
// CUPnPBrowsingSession::CdsDeleteResourceResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsDeleteResourceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aResourceUri*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsCreateReferenceResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsCreateReferenceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerId*/, 
    const TDesC8& /*aObjectId*/, 
    const TDesC8& /*aNewId*/ )
    {
    // No implementation required            
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsCreateObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsCreateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aContainerID*/, 
    const TDesC8& /*aElements*/, 
    const TDesC8& aObjectID, 
    const TDesC8& aResult )
    {
    __LOG1( "CUPnPBrowsingSession::CdsCreateObjectResponse: %d" , aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    //iActionPending = EFalse;
    iIPSessionId = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPContentDirectoryError );
        
    if( aErr == KErrNone )
        {
        if( iInternalState == ECopyLocal ||
            iInternalState == ECopyToPhone )
            {      
            delete iImportURI; iImportURI = NULL;
            TRAP( aErr, iImportURI = ParseCreateObjectResponseL( aResult ) );
            if( aErr == KErrNone )
                {
                if( iSourceURI )
                    {
                    if( iInternalState == ECopyLocal )
                        {
                        // Export from the local Media Server to the
                        // Remote Media Server
                        delete iItemId;
                        iItemId = aObjectID.Alloc(); // Null ok at this point
                        
                        TRAP( aErr, SendExportActionL() );
                        if( aErr )
                            {
                            CopyFinished( aErr, EFalse );
                            }                                          
                        }
                    else // iInternalState == ECopyToPhone
                        {
                        TRAP( aErr, SendImportActionL() );
                        if( aErr )
                            {
                            CopyFinished( aErr, EFalse );
                            }
                        }        
                    }
                else
                    {
                    CopyFinished( KErrGeneral, EFalse );
                    }                
                }
            else
                {
                CopyFinished( aErr, EFalse );
                } 
            }
        else // Create container
            {
            if( iActionMessage )
                {
                HBufC8* objectID = HBufC8::New( aObjectID.Length() );
                if( objectID )
                    {
                    objectID->Des().Copy( aObjectID );
                    iActionMessage->Write( 1, *objectID );
                    iActionMessage->Complete(
                        EAVControllerCreateContainerCompleted );
                    delete objectID;                
                    }
                else
                    {
                    iActionMessage->Write( 1, KNullDesC8 );
                    iActionMessage->Complete( KErrNoMemory );
                    }    
                delete iActionMessage; iActionMessage = NULL;
                }
            iInternalState = ENone;                                
            }                
        }
    else
        {
        if( iInternalState == ECopyLocal ||
            iInternalState == ECopyToPhone )
            {
            CopyFinished( aErr, EFalse );
            }
        else
            {
            // Create container failed
            iInternalState = ENone;
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;                    
            }
        }                                       
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmProtocolInfoResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmProtocolInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSource*/, 
    const TDesC8& /*aSink*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmPrepareResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmPrepareResponse(
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
// CUPnPBrowsingSession::CmComplete
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmComplete(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aConnection*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmCurrentConnections
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmCurrentConnections(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aConnections*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmCurrentInfo
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmCurrentInfo(
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
// CUPnPBrowsingSession::CdsUpdateEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsUpdateEvent(
        const TDesC8& /*aUuid*/,
        TInt /*aSystemUpdateId*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsContainerEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsContainerEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConteinerIds*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsTransferEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsTransferEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aTransferIds*/
        )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RcLastChangeEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::RcLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aLastChange*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::AvtLastChangeEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::AvtLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aLastChange*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmSourceEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmSourceEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSource*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmSinkEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmSinkEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSink*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CmConnectionsEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CmConnectionsEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConnections*/
        )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::HttpResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::HttpResponseL( CUpnpHttpMessage* /*aMessage*/ )
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDiscoveredL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDiscoveredL( CUpnpDevice* /*aDevice*/ )
    {
    // No implementation required            
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedL( CUpnpDevice* /*aDevice*/ )
    {
    // No implementation required                  
    }    

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::FileTransferEvent
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::FileTransferEvent(
    CUpnpFileTransferEvent *aEvent )
    {
    __ASSERTD( aEvent, __FILE__, __LINE__ );
    
    __LOG1( "CUPnPBrowsingSession::FileTransferEvent, %d",
        aEvent->ErrorCode() );
    
    TInt err = UPnPAVErrorHandler::ConvertToSymbianErrorCode(
        aEvent->ErrorCode(), EUPnPContentDirectoryError );    

    if( aEvent->TransferId() == iTransferId )
        {
        iTransferId = KErrNotFound;            
        if( iInternalState == ECopyLocal )
            {
            if( iActionMessage )
                {
                CopyFinished( err, ETrue );    
                }
            else
                {
                // Msg not received, set the flag instead
                iAsyncErr = err;
                iTransferEventReceived = ETrue;
                }                  
            }
        else if( iInternalState == ECopyToPhone )
            {                  
            TRAPD( err, HandleCopyToPhoneEventL( *aEvent, err ) );
            if ( err )
                {
                 __LOG1( "CUPnPBrowsingSession::FileTransferEvent, %d",
                         err );
                }                                          
            }
        }

    delete aEvent;        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::HandleCopyToPhoneEventL
// Handle CopyToPhoneEvent,and all leave function will move 
// to this function
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::HandleCopyToPhoneEventL( 
         CUpnpFileTransferEvent& aEvent,
         TInt aError )
    {
    HBufC8* filepath = HBufC8::NewL( KMaxPath );
    CleanupStack::PushL( filepath );
    
    HBufC8* eventpath = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
        aEvent.FilePath() );
    filepath->Des().Copy( *eventpath );
    delete eventpath;
    eventpath = NULL;

    if ( iActionMessage )
        {
        TInt res = iActionMessage->Write( 1, *filepath );
        if ( res )
            {
            __LOG1( "CUPnPBrowsingSession::HandleCopyToPhoneEventL, %d",
                    res );
            }
        // clean up
        CleanupStack::PopAndDestroy( filepath );
        }
    else
        {
        iFilePath = filepath;
        
        // clean up
        CleanupStack::Pop( filepath );        
        }

    if ( iActionMessage )
        {
        CopyFinished( aError, ETrue );
        }
    else
        {
        // Msg not received yet, set the flag instead
        iAsyncErr = aError;
        iTransferEventReceived = ETrue;
        }  
    
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::NotifierError
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::NotifierError( TInt aError )
    {
    __LOG( "CUPnPBrowsingSession::NotifierError" );
    
    if( iActionMessage )
        {
        if( iInternalState == ECopyLocal ||
            iInternalState == ECopyToPhone )
            {
            CopyFinished( aError, ETrue );
            }
        }
    else
        {
        // Msg not received yet, set the flag instead
        iAsyncErr = aError;
        iTransferEventReceived = ETrue;        
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPBrowsingSession::DeviceDisappearedL" );
    
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
// CUPnPBrowsingSession::SetLocalMSUuidL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SetLocalMSUuidL( const TDesC8& aUuid )
    {
    HBufC8* tmp = aUuid.AllocL();
    delete iLocalMediaServerUuid;
    iLocalMediaServerUuid = tmp; 
    }
 
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SessionId
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
TInt CUPnPBrowsingSession::SessionId() const
    {
    return iSessionId;
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::Uuid
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
const TDesC8& CUPnPBrowsingSession::Uuid() const
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
// CUPnPBrowsingSession::GetBrowseResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetBrowseResponseSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetBrowseResponseSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();
        
    CUpnpAVBrowseRequest* tmpRequest = CUpnpAVBrowseRequest::NewLC();
    
    ReadBrowseReqFromMessageL( aMessage, 1, tmpRequest );
        
    if( tmpRequest->BrowseFlag() == MUPnPAVBrowsingSession::EDirectChildren )
        {
        iIPSessionId = iServer.ControlPoint().CdsBrowseActionL(
                iDevice->Uuid(),
                tmpRequest->Id(),
                KDirectChildren,
                tmpRequest->Filter(), 
                tmpRequest->StartIndex(),
                tmpRequest->RequestedCount(),
                tmpRequest->SortCriteria() );        
        }
    else
        {
        iIPSessionId = iServer.ControlPoint().CdsBrowseActionL(
                iDevice->Uuid(),
                tmpRequest->Id(),
                KMetaData,
                tmpRequest->Filter(), 
                tmpRequest->StartIndex(),
                tmpRequest->RequestedCount(),
                tmpRequest->SortCriteria() );                
        }    
    

    CleanupStack::PopAndDestroy( tmpRequest );
   
     if( iIPSessionId > 0 )
        {
        // Register
        iInternalState = EBrowse;
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        }
    else
        {
        User::Leave( iIPSessionId );
        }
        
    iActionMessage = new (ELeave) RMessage2( aMessage );    
    }
 

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetBrowseResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetBrowseResponseSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetBrowseResponseSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;         
        }
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetBrowseResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetBrowseResponseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetBrowseResponseL" );
    
   __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    iIPSessionId = KErrNotFound;
    if( iRespBuf && iRespBuf2 )
        {
        aMessage.WriteL( 1, *iRespBuf );
        aMessage.WriteL( 2, *iRespBuf2 );
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        aMessage.Complete( EAVControllerGetBrowseResponseCompleted );    
        }
    else
        {
        //Memory allocaton failed
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        User::Leave( KErrNoMemory );
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchResponseSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchResponseSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();
    
    
    CUpnpAVBrowseRequest* tmpRequest = CUpnpAVBrowseRequest::NewLC();
    
    ReadBrowseReqFromMessageL( aMessage, 1, tmpRequest );
  
    iIPSessionId = iServer.ControlPoint().CdsSearchActionL(
        iDevice->Uuid(),
        tmpRequest->Id(),
        tmpRequest->SearchCriteria(),
        tmpRequest->Filter(), 
        tmpRequest->StartIndex(),
        tmpRequest->RequestedCount(),
        tmpRequest->SortCriteria() );                

    CleanupStack::PopAndDestroy( tmpRequest );
    
     if( iIPSessionId > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        }
    else
        {
        User::Leave( iIPSessionId );
        }
    iActionMessage = new (ELeave) RMessage2( aMessage );      
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetSearchResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetSearchResponseSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetSearchResponseSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;               
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchResponseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchResponseL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    iIPSessionId = KErrNotFound;
    if( iRespBuf && iRespBuf2 )
        {
        aMessage.WriteL( 1, *iRespBuf );
        aMessage.WriteL( 2, *iRespBuf2 );
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        aMessage.Complete( EAVControllerGetSearchResponseCompleted );    
        }
    else
        {
        //Memory allocaton failed
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        User::Leave( KErrNoMemory );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchCapabitiesSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchCapabitiesSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchCapabitiesSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();
  
    iIPSessionId = iServer.ControlPoint().CdsSearchCapabilitiesActionL(
        iDevice->Uuid() );
    
    if( iIPSessionId > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        }
    else
        {
        User::Leave( iIPSessionId );
        }
    iActionMessage = new (ELeave) RMessage2( aMessage );    
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;                   
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchCapabitiesL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchCapabitiesL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchCapabitiesL" );
    
    iIPSessionId = KErrNotFound;
    
    aMessage.WriteL( 1, *iRespBuf );
    aMessage.Complete( EAVControllerGetSearchCapabilitiesCompleted );    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CreateContainerL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CreateContainerL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::CreateContainerL" );       
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );
    
    ResetL();
        
    // Title
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* tempTitle = HBufC8::NewLC( len );
    TPtr8 ptr( tempTitle->Des() );
    aMessage.ReadL( 1, ptr );

    // Container ID
    len = aMessage.GetDesMaxLength( 2 );
    HBufC8* tempId = HBufC8::NewLC( len );
    ptr.Set( tempId->Des() );
    aMessage.ReadL( 2, ptr );
    
    if( iDevice->DlnaCompatible() && !iDevice->CreateChildContainer()
        && *tempId != KContainerIdAny )
        {
        // The device is DLNA compatible and does not support creation
        // of a child container
        User::Leave( KErrNotSupported );
        }
            
    // Container type
    MUPnPAVBrowsingSession::TContainerType type =
            (MUPnPAVBrowsingSession::TContainerType)aMessage.Int3();

    // Create a container object
    CUpnpContainer* tmpContainer = CUpnpContainer::NewL();
    CleanupStack::PushL( tmpContainer );
    
    // Set the title and the parent ID
    tmpContainer->SetTitleL( *tempTitle );
    tmpContainer->SetParentIdL( *tempId );
    
    // Set the object type
    if( type == MUPnPAVBrowsingSession::EPlaylistContainer )
        {
        tmpContainer->SetObjectClassL( KClassPlaylist() );
        }
    else
        {
        tmpContainer->SetObjectClassL( KClassStorage() );
        }

    HBufC8* xmlDoc = CUPnPXMLParser::ContainerToXmlLC( *tmpContainer );  

    iIPSessionId = iServer.ControlPoint().CdsCreateObjectActionL(
        iDevice->Uuid(), *tempId, *xmlDoc );


    CleanupStack::PopAndDestroy( xmlDoc );
    CleanupStack::PopAndDestroy( tmpContainer );
    CleanupStack::PopAndDestroy( tempId );
    CleanupStack::PopAndDestroy( tempTitle );
   
     if( iIPSessionId > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        iInternalState = ECreateContainer; 
        }
    else
        {
        User::Leave( iIPSessionId );
        }
    iActionMessage = new (ELeave) RMessage2( aMessage );         
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelCreateContainerL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelCreateContainerL()
    {
    __LOG( "CUPnPBrowsingSession::CancelCreateContainerL" );       
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;        
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeleteObjectL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeleteObjectL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::DeleteObjectL" );       
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );
    
    ResetL();
    
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* tempId = HBufC8::NewLC( len );
    TPtr8 ptr( tempId->Des() );
    aMessage.ReadL( 1, ptr );
    
    CleanupStack::Pop( tempId );
    delete iItemId;
    iItemId = tempId;
    
            
    iIPSessionId = iServer.ControlPoint().CdsBrowseActionL(
        iDevice->Uuid(), *iItemId, KMetaData, KFilterCommon, 0, 1,
        KNullDesC8 );                
    
    if( iIPSessionId > 0 )
        {
        // Register
        iInternalState = EDestroyObject;
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        }
    else
        {
        User::Leave( iIPSessionId );
        }
    iActionMessage = new (ELeave) RMessage2( aMessage );   
    }
        
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelDeleteObjectL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelDeleteObjectL()
    {
    __LOG( "CUPnPBrowsingSession::CancelDeleteObjectL" );       
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );    
        delete iActionMessage; iActionMessage = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedRequestL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedRequestL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::DeviceDisappearedRequestL" );       
    
    __ASSERTD( !iDeviceMessage, __FILE__, __LINE__ );
    
    iDeviceMessage = new (ELeave) RMessage2( aMessage );
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelDeviceDisappearedRequestL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelDeviceDisappearedRequestL()
    {
    __LOG( "CUPnPBrowsingSession::CancelDeviceDisappearedRequestL" );       
    
    if( iDeviceMessage )
        {
        iDeviceMessage->Complete( KErrCancel );    
        delete iDeviceMessage; iDeviceMessage = NULL;    
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ParseCreateObjectResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPBrowsingSession::ParseCreateObjectResponseL(
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::ParseCreateObjectResponseL" );          
    
    HBufC8* importURI = NULL;
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    if( array.Count() == KExpectedCount )
        {
        if( array[ 0 ]->ObjectType() == EUPnPItem )
            {
            HBufC8* tmp = array[ 0 ]->Id().AllocL(); 
            delete iItemId;
            iItemId = tmp;
            
            if( array[ 0 ]->ObjectClass().Find( KClassAudio )
                != KErrNotFound )
                {
                iMusic = ETrue;
                }
            
            // Get the res-elements
            RUPnPElementsArray elArray;
            CleanupClosePushL( elArray );
            UPnPItemUtility::GetResElements( *array[ 0 ], elArray );
            
            // Find the import uri            
            for( TInt i = 0; i < elArray.Count(); i++ )
                {
                const CUpnpAttribute* attribute = NULL;
                TRAPD( err, attribute =
                    &UPnPItemUtility::FindAttributeByNameL(
                    *elArray[ i ], KImportUri ) );
                if( err == KErrNone )
                    {
                    // import uri found!
                    i = elArray.Count();
                    importURI = attribute->Value().AllocL();
                    }
                }
                        
            CleanupStack::PopAndDestroy( &elArray );
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
          
    if( !importURI )
        {
        User::Leave( KErrGeneral );
        }
    
    if( !UpnpCdsResElementUtility::IsUriAbsolute( *importURI ) )
        {
        // Import uri is not absolute
        delete importURI; importURI = NULL;
        User::Leave( KErrGeneral );
        }
      
    return importURI;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CheckIsCreateObjectSupportedL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CheckIsCreateObjectSupportedL(
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::CheckIsCreateObjectSupportedL" );
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    if( array.Count() == KExpectedCount )
        {
        if( array[ 0 ]->ObjectType() == EUPnPContainer )
            {
            // Try to get upnp:createClass elements
            const CUpnpElement* elem = UPnPItemUtility::FindElementByName(
                *array[ 0 ], KCreateClass );

            if ( !elem )
                {
                // No createClass elements, copy not supported
                User::Leave( KErrNotSupported );
                }

            RUPnPElementsArray resultArray;
            CleanupClosePushL( resultArray );
            UPnPItemUtility::GetResElements( *array[ 0 ], resultArray );
            TInt count = resultArray.Count();

            for( TInt i = 0; i < count; i++ )
                {
                if( iSharedItem->ObjectClass().Find( KClassAudio ) !=
                    KErrNotFound )
                    {
                    // We are going to create a music item, check that the
                    // target container supports that
                    if( resultArray[ i ]->Value().Find( KClassAudio ) ==
                    KErrNotFound )
                        {
                        User::Leave( KErrNotSupported );
                        }
                    }
                else if( iSharedItem->ObjectClass().Find( KClassImage ) !=
                    KErrNotFound )
                    {
                    // We are going to create an image item, check that the
                    // target container supports that
                    if( resultArray[ i ]->Value().Find( KClassImage ) ==
                    KErrNotFound )
                        {
                        User::Leave( KErrNotSupported );
                        }                    
                    }
                else if( iSharedItem->ObjectClass().Find( KClassVideo ) !=
                    KErrNotFound )
                    {
                    // We are going to create a video item, check that the
                    // target container supports that
                    if( resultArray[ i ]->Value().Find( KClassVideo ) ==
                    KErrNotFound )
                        {
                        User::Leave( KErrNotSupported );
                        }                                        
                    }
                else
                    {
                    // Unknown object class, leave
                    User::Leave( KErrNotSupported );
                    }      
                }                            
            CleanupStack::PopAndDestroy( &resultArray );
            }
        else
            {
            User::Leave( KErrNotSupported );
            }            
        }
    else
        {
        User::Leave( KErrGeneral );
        }    
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( parser );              
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL(
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL" );
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    if( array.Count() == KExpectedCount )
        {
        if( array[ 0 ]->Restricted() )
            {
            User::Leave( KErrArgument );
            }
        else
            {
            // Not restricted, ok to destroy
            TInt sessionId = iServer.ControlPoint().CdsDestroyObjectActionL(
                iDevice->Uuid(), *iItemId );
            if( sessionId > 0 )
                {
                iServer.Dispatcher().RegisterL( sessionId, *this );
                iIPSessionId = sessionId;                   
                }
            else
                {
                User::Leave( sessionId );
                }                
            }    
        }
    else
        {
        User::Leave( KErrGeneral );
        }    
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( parser );    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CheckAndSendCreateObjectActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CheckAndSendCreateObjectActionL( 
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::CheckAndSendCreateObjectActionL" );
      
    CheckIsCreateObjectSupportedL( aResponse );
    
    HBufC8* xmlDoc = CUPnPXMLParser::XmlForCreateObjectLC( *iSharedItem );
    SendCreateObjectActionL( iDevice->Uuid(), *iContainerId, *xmlDoc );
    CleanupStack::PopAndDestroy( xmlDoc );   
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SendCreateObjectActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SendCreateObjectActionL( const TDesC8& aUUid,
    const TDesC8& aContainerId, const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::SendCreateObjectActionL" );
    
    TInt sessionId = iServer.ControlPoint().CdsCreateObjectActionL( aUUid,
        aContainerId, aResponse );
    
    if( sessionId > 0 )
        {
        iServer.Dispatcher().RegisterL( sessionId, *this );
        iIPSessionId = sessionId;                   
        }
    else
        {
        User::Leave( sessionId );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SendExportActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SendExportActionL()
    {
    __LOG( "CUPnPBrowsingSession::SendExportActionL" );
    //__LOG8( *iSourceURI );
    //__LOG8( *iImportURI );
    
    HBufC8* tempSourceUri = UpnpString::EncodeXmlStringL( 
                                            iSourceURI );
    delete iSourceURI;
    iSourceURI = tempSourceUri;
    tempSourceUri = NULL;
    
    TInt sessionId = iServer.ControlPoint().CdsExportResourceActionL(
        *iLocalMediaServerUuid, *iSourceURI, *iImportURI );
    if( sessionId > 0 )
        {
        iServer.Dispatcher().RegisterL( sessionId, *this );
        iIPSessionId = sessionId;
        }
    else
        {
        User::Leave( sessionId );
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SendImportActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SendImportActionL()
    {
    __LOG( "CUPnPBrowsingSession::SendImportActionL" );
    //__LOG8( *iSourceURI );
    //__LOG8( *iImportURI );

    HBufC8* tempSourceUri = UpnpString::EncodeXmlStringL( 
                                            iSourceURI );
    delete iSourceURI;
    iSourceURI = tempSourceUri;
    tempSourceUri = NULL;
    
    TInt sessionId = iServer.ControlPoint().CdsImportResourceActionL(
        *iLocalMediaServerUuid, *iSourceURI, *iImportURI );
    if( sessionId > 0 )
        {
        iServer.Dispatcher().RegisterL( sessionId, *this );
        iIPSessionId = sessionId;
        }
    else
        {
        User::Leave( sessionId );
        }    
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CopyFinished
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CopyFinished( TInt aError, TBool aFinished )
    {
    __LOG( "CUPnPBrowsingSession::CopyFinished" );
          
    if( iInternalState == ECopyLocal ) // Local to remote copy
        {
        if( aError == KErrNone )
            {
            if( aFinished )
                {
                // Local to remote copy completed successfully!

                if( iSharedItem )
                    {
                    // Remove shared item
                    TRAP_IGNORE( iFileSharing->UnShareItemL(
                        iSharedItem->Id() ) );    
                    delete iSharedItem; iSharedItem = NULL;        
                    }

                iInternalState = ENone;
                if( iActionMessage )
                    {
                    iActionMessage->Complete(
                        EAVControllerCopyLocalItemFinishCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                }
            else
                {
                // First phase of copy completed (export action succeeded)
                if( iActionMessage )
                    {
                    iActionMessage->Complete(
                        EAVControllerCopyLocalItemStartCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                }                
            }
        else // Error occured, cleanup.
            {
            if( iSharedItem )
                {
                // Remove shared item     
                TRAP_IGNORE( iFileSharing->UnShareItemL(
                    iSharedItem->Id() ) );    
                }
            delete iSharedItem; iSharedItem = NULL;        
            if( iItemId )
                {
                // Destroy object from the remote media server                
                TRAP_IGNORE( iServer.ControlPoint().CdsDestroyObjectActionL( 
                    iDevice->Uuid(), *iItemId ) );
                delete iItemId; iItemId = NULL;
                }
            
            iInternalState = ENone;
            if( iActionMessage )
                {
                iActionMessage->Complete( aError );
                delete iActionMessage; iActionMessage = NULL;    
                }
            
            }    
        }
    else if( iInternalState == ECopyToPhone ) // Remote to local copy
        {        
        if( aError == KErrNone )
            {
            if( aFinished )
                {
                // Remote to local copy successful, check sharing status
                CheckSharingStatus();
                
                // Restore original download settings
                RestoreDownloadSettings(); // ignore error
                                
                iInternalState = ENone;
                if( iActionMessage )
                    {
                    iActionMessage->Complete(
                        EAVControllerCopyToPhoneFinishCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                }
            else
                {
                // First phase of copy completed (import action succeeded)
                if( iActionMessage )
                    {
                    iActionMessage->Complete(
                        EAVControllerCopyToPhoneStartCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                }    
            }
        else // Error occured, cleanup.
            {
            // Restore original download settings
            RestoreDownloadSettings(); // ignore error

            if( iItemId )
                {
                // Destroy object from the local media server
                TRAP_IGNORE( iServer.ControlPoint().CdsDestroyObjectActionL( 
                    *iLocalMediaServerUuid, *iItemId ) );
                delete iItemId; iItemId = NULL;
                }

            iInternalState = ENone;
            if( iActionMessage )
                {
                iActionMessage->Complete( aError );
                delete iActionMessage; iActionMessage = NULL;
                }
            }        
        }
    else
        {      
        __PANICD( __FILE__, __LINE__ );     
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ResetL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ResetL()
    {
    __LOG( "CUPnPBrowsingSession::ResetL" );
    
    iIPSessionId = KErrNotFound;
    
    if( !iServer.DeviceRepository().IsWlanActive() )    
        {
        __LOG( "Reset - disconnected" );
        User::Leave( KErrDisconnected );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadObjFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadObjFromMessageL( const RMessage2& aMessage, 
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
// CUPnPBrowsingSession::ReadReqFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadReqFromMessageL( const RMessage2& aMessage, 
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

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadBrowseReqFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadBrowseReqFromMessageL(
    const RMessage2& aMessage, TInt aSlot, CUpnpAVBrowseRequest* aReq ) 
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
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadBufFromMessageLC
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPBrowsingSession::ReadBufFromMessageLC(
    const RMessage2& aMessage, TInt aSlot ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );        
    return buf;
    }    

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SetDownloadSettingsL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SetDownloadSettingsL(
    MUPnPAVBrowsingSession::TMemoryType aType )
    {
    if( aType == MUPnPAVBrowsingSession::EDefault )
        {
        // Downloading to default location is the only supported target
        }
    else
        {
        User::Leave( KErrNotSupported );
        }    
    
    // EMemoryCard, ERAMDrive or EPhone are not supported, but the code is
    // left in place for possible future use
    /*    
    // Read the original download location and store it
    HBufC8* buf = iMSSettings->GetL(
        UpnpMediaServerSettings::EUploadDirectory );
    delete iOriginalLocation;
    iOriginalLocation = buf;
   
    // Set the new download location
    if( aType == MUPnPAVBrowsingSession::EMemoryCard )
        {
        User::LeaveIfError( iMSSettings->SetL(
            UpnpMediaServerSettings::EUploadDirectory,
            KDownloadMemoryCard ) );
        }
    else if( aType == MUPnPAVBrowsingSession::ERAMDrive )
        {
        User::LeaveIfError( iMSSettings->SetL(
            UpnpMediaServerSettings::EUploadDirectory,
            KDownloadRAMDrive ) );        
        }
    else if( aType == MUPnPAVBrowsingSession::EPhone )
        {
        User::LeaveIfError( iMSSettings->SetL(
            UpnpMediaServerSettings::EUploadDirectory,
            KDownloadPhoneMemory ) );        
        }
    else
        {
        // Default, no change needed
        }
    */    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::RestoreDownloadSettings
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
TInt CUPnPBrowsingSession::RestoreDownloadSettings()
    {
    // Restore the download settings
    TInt err = KErrNone;
    
    if( iOriginalLocation && iShareFlag !=
        MUPnPAVBrowsingSession::EDefault )
        {
        TRAP( err, err = iMSSettings->SetL(
            UpnpMediaServerSettings::EUploadDirectory,
            *iOriginalLocation ) );        
        }
    else
        {
        err = KErrGeneral;
        } 
            
    return err;    
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CheckSharingStatus
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
TInt CUPnPBrowsingSession::CheckSharingStatus()
    {
    TInt err = KErrNone;

    if( iShareFlag == MUPnPAVBrowsingSession::EFileIsNotShared )
        {
        // Unshare the item
        if( iItemId )
            {
            TRAP( err, iFileSharing->UnShareItemL( *iItemId ) );
            delete iItemId; iItemId = NULL;
            }
        }
    else if( MUPnPAVBrowsingSession::EShareBySettings )
        {
        // Check settings to determine should we keep the item shared or not
        TInt share = 0;
        if( iMusic ) // It's a music item
            {       
            err = iAppSettings->Get( KUPnPAppShareAllMusicFiles, share );
            }
        else // Image or video item
            {
            err = iAppSettings->Get( KUPnPAppShareAllVisualFiles, share );
            }    
        
        if( err == KErrNone && !share )
            {
            if( iItemId )
                {
                TRAP( err, iFileSharing->UnShareItemL( *iItemId ) );
                delete iItemId; iItemId = NULL;
                }            
            }
        }
    else
        {
        // File is shared already, do nothing
        }    

    return err;
    }
    
// End of file
