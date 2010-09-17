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
* Description:      dispatches control point events to AVController clients
*
*/






// INCLUDE FILES

#include "upnpavdispatcher.h"

#include "upnpavactioninfo.h"
#include "upnpavcontrolpointobserver.h"
//#include "upnpavcontrollerimpl.h"
#include "upnpavcontrollerserver.h"

_LIT( KComponentLogfile, "CUPnPAVDispatcher.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::CUPnPAVDispatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher::CUPnPAVDispatcher( CUpnpAVControllerServer& aServer ) :
    iServer( aServer )
    {    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::ConstructL()
    {                           
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher* CUPnPAVDispatcher::NewL( CUpnpAVControllerServer& aServer )
    {
    CUPnPAVDispatcher* self = NewLC( aServer);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher* CUPnPAVDispatcher::NewLC(
    CUpnpAVControllerServer& aServer )
    {
    CUPnPAVDispatcher* self = new( ELeave )
        CUPnPAVDispatcher( aServer );   
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// Destructor
CUPnPAVDispatcher::~CUPnPAVDispatcher()
    {
    iActionInfos.ResetAndDestroy(); 
//    iActionInfosDevice.ResetAndDestroy();
    iActionInfosEvent.ResetAndDestroy();           
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RcSetVolumeResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& aInstance, 
    const TDesC8& aChannel, 
    const TDesC8& aDesiredVolume)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.RcSetVolumeResponse( aUuid, aSessionId, aErr, aInstance, aChannel,
            aDesiredVolume );        
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RcVolumeResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& aInstance, 
    const TDesC8& aChannel, 
    const TDesC8& aCurrentVolume)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.RcVolumeResponse( aUuid, aSessionId, aErr, aInstance, aChannel,
            aCurrentVolume );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RcSetMuteResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& aInstance, 
    const TDesC8& aChannel, 
    const TDesC8& aDesiredMute)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
    
        obs.RcSetMuteResponse( aUuid, aSessionId, aErr, aInstance, aChannel,
            aDesiredMute );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RcMuteResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr, 
    const TDesC8& aInstance, 
    const TDesC8& aChannel, 
    const TDesC8& aCurrentMute)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
       
        obs.RcMuteResponse( aUuid, aSessionId, aErr, aInstance, aChannel,
            aCurrentMute );
        }
    }


// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtSetTransportUriResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& aCurrentUri,
    const TDesC8& aCurrentUriMetaData)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtSetTransportUriResponse( aUuid, aSessionId, aErr, aInstanceId,
            aCurrentUri, aCurrentUriMetaData );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtSetNextTransportUriResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& aNextUri,
    const TDesC8& aNextUriMetaData )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtSetNextTransportUriResponse( aUuid, aSessionId, aErr,
            aInstanceId, aNextUri, aNextUriMetaData );
        }
    }
  
// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtMediaInfoResponse(
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
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtGetTransportInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aCurrenTransportState*/,
    const TDesC8& /*aCurrentTransportStatus*/,
    const TDesC8& /*aCurrentSpeed*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtPositionInfoResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& aTrack,
    const TDesC8& aTrackDuration,
    const TDesC8& aTrackMetaData,
    const TDesC8& aTrackURI,
    const TDesC8& aRelTime,
    const TDesC8& aAbsTime,
    const TDesC8& aRelCount,
    const TDesC8& aAbsCount)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtPositionInfoResponse( aUuid, aSessionId, aErr,
            aInstanceId, aTrack, aTrackDuration, aTrackMetaData,
            aTrackURI, aRelTime, aAbsTime, aRelCount, aAbsCount );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtDeviceCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMedia*/,
    const TDesC8& /*aRecMedia*/,
    const TDesC8& /*aRecQualityMode*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtTransportSettingsResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMode*/,
    const TDesC8& /*aRecQualityMode*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtStopResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtStopResponse( aUuid, aSessionId, aErr, aInstanceId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtPlayResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId,
    const TDesC8& aSpeed)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtPlayResponse( aUuid, aSessionId, aErr, aInstanceId,
            aSpeed );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtPauseResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aInstanceId)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.AvtPauseResponse( aUuid, aSessionId, aErr, aInstanceId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtRecordResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtSeekResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aUnit*/,
    const TDesC8& /*aTarget*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtNextResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtPreviousResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtSetPlayModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewPlayMode*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtSetRecordModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewRecordQuality*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsSearchCapabilitiesResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aSearchCaps)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsSearchCapabilitiesResponse( aUuid, aSessionId, aErr,
            aSearchCaps );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsSortCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSortCaps*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsSystemUpdateIdResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aSystemUpdateId*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsBrowseResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aObjectID,
    const TDesC8&  aBrowseFlag,
    const TDesC8&  aFilter,
    TInt aIndex,
    TInt aRequest,
    const TDesC8&  aSortCriteria,
    const TDesC8&  aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8&  aUpdateID)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsBrowseResponse( aUuid, aSessionId, aErr, aObjectID,
            aBrowseFlag, aFilter, aIndex, aRequest, aSortCriteria,
            aResult, aReturned, aMatches, aUpdateID );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsSearchResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aContainerId,
    const TDesC8& aSearchCriteria,
    const TDesC8& aFilter,
    TInt aIndex,
    TInt aRequest,
    const TDesC8& aSortCriteria,
    const TDesC8& aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8& aUpdateID)
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsSearchResponse( aUuid, aSessionId, aErr, aContainerId,
            aSearchCriteria, aFilter, aIndex, aRequest, aSortCriteria,
            aResult, aReturned, aMatches, aUpdateID );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsDestroyObjectResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aObjectId )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsDestroyObjectResponse( aUuid, aSessionId, aErr, aObjectId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsUpdateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectId*/,
    const TDesC8& /*aCurrentTagValue*/,
    const TDesC8& /*aNewTagValue*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsImportResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aSourceURI,
    const TDesC8& aDestinationURI,
    const TDesC8& aTransferId )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsImportResponse( aUuid, aSessionId, aErr, aSourceURI,
            aDestinationURI, aTransferId );
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsExportResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aSourceURI,
    const TDesC8& aDestinationURI,
    const TDesC8& aTransferId )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsExportResponse( aUuid, aSessionId, aErr, aSourceURI,
            aDestinationURI, aTransferId );
        }
    
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsStopTransferResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aTransferId )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsStopTransferResponse( aUuid, aSessionId, aErr, aTransferId );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsCTransferProgressResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aTransferId,
    const TDesC8& aTransferStatus,
    const TDesC8& aTransferLength,            
    const TDesC8& aTransferTotal )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsCTransferProgressResponse( aUuid, aSessionId,
            aErr, aTransferId, aTransferStatus, aTransferLength,
            aTransferTotal );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsDeleteResourceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aResourceUri*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsCreateReferenceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerId*/, 
    const TDesC8& /*aObjectId*/, 
    const TDesC8& /*aNewId*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsCreateObjectResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aContainerID, 
    const TDesC8& aElements, 
    const TDesC8& aObjectID, 
    const TDesC8& aResult )
    {
    MUpnpAVControlPointObserver& obs = FindObserver( aSessionId );
    if( &obs )
        {
        obs.CdsCreateObjectResponse( aUuid, aSessionId, aErr, aContainerID,
            aElements, aObjectID, aResult );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmProtocolInfoResponse(
    const TDesC8& aUuid,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& aSource, 
    const TDesC8& aSink )
    {
    iServer.CmProtocolInfoResponse( aUuid, aSessionId, aErr, aSource, aSink );
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmPrepareResponse(
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
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmComplete(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aConnection*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmCurrentConnections(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aConnections*/)
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmCurrentInfo(
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
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsUpdateEvent(
        const TDesC8& /*aUuid*/,
        TInt /*aSystemUpdateId*/
        )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsContainerEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConteinerIds*/
        )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CdsTransferEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aTransferIds*/
        )
    {

    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RcLastChangeEvent(
        const TDesC8& aUuid,
        const TDesC8& aLastChange
        )
    {
    // Forward to each observer
    TInt tempCount = iActionInfosEvent.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( aUuid == iActionInfosEvent[ i ]->Uuid() )
            {
            iActionInfosEvent[ i ]->Observer().RcLastChangeEvent(
                aUuid, aLastChange );            
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::AvtLastChangeEvent(
        const TDesC8& aUuid,
        const TDesC8& aLastChange
        )
    {
    // Forward to each observer
    TInt tempCount = iActionInfosEvent.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( aUuid == iActionInfosEvent[ i ]->Uuid() )
            {
            iActionInfosEvent[ i ]->Observer().AvtLastChangeEvent(
                aUuid, aLastChange );
            }
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmSourceEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSource*/
        )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmSinkEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSink*/
        )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::CmConnectionsEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConnections*/
        )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::HttpResponseL( CUpnpHttpMessage* /*aMessage*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::DeviceDiscoveredL( CUpnpDevice* aDevice )
    {
    __ASSERTD( aDevice, __FILE__, __LINE__  )
    
    if( aDevice )
        {
        iServer.DeviceDiscoveredL( *aDevice );    
        }
    else
        {
        // No can do?
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::DeviceDisappearedL( CUpnpDevice* aDevice )
    {
    __ASSERTD( aDevice, __FILE__, __LINE__  );
    
    if( aDevice )
        {
        iServer.DeviceDisappearedL( *aDevice );    
        }
    else
        {
        // No can do?
        }                
    }

void CUPnPAVDispatcher::RegisterL( TInt aSessionId,
    MUpnpAVControlPointObserver& aObserver/*, const TDesC8& aUuid*/ )
    {
    CUPnPAVActionInfo* tempInfo = CUPnPAVActionInfo::NewLC();
    tempInfo->SetSessionId( aSessionId );
    tempInfo->SetObserver( aObserver );
    //tempInfo->SetUuidL( aUuid );
    CleanupStack::Pop( tempInfo );
    iActionInfos.AppendL( tempInfo );
    }

void CUPnPAVDispatcher::UnRegister( TInt aSessionId
    /*, const TDesC8& aUuid*/ )
    {
    TInt tempCount = iActionInfos.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( iActionInfos[ i ]->SessionId() == aSessionId )
            {
            delete iActionInfos[ i ];
            iActionInfos.Remove( i );
            i = tempCount;
            }
        }
    }

MUpnpAVControlPointObserver& CUPnPAVDispatcher::FindObserver(
    TInt aSessionId )
    {
    MUpnpAVControlPointObserver* tempObserver = NULL;
    TInt tempCount = iActionInfos.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( iActionInfos[ i ]->SessionId() == aSessionId )
            {
            tempObserver = &( iActionInfos[ i ]->Observer() );
            i = tempCount;
            }
        }
    return *tempObserver;   
    }

void CUPnPAVDispatcher::RegisterForEventsL(
    MUpnpAVControlPointObserver& aObserver, const TDesC8& aUuid )
    {
    CUPnPAVActionInfo* tempInfo = CUPnPAVActionInfo::NewLC();
    tempInfo->SetObserver( aObserver );
    tempInfo->SetUuidL( aUuid );
    CleanupStack::Pop( tempInfo );
    iActionInfosEvent.AppendL( tempInfo );        
    }
    
void CUPnPAVDispatcher::UnRegisterEvents(
    MUpnpAVControlPointObserver& aObserver )
    {
    TInt tempCount = iActionInfosEvent.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( &aObserver == &iActionInfosEvent[ i ]->Observer() )
            {
            delete iActionInfosEvent[ i ];
            iActionInfosEvent.Remove( i );
            i = tempCount;
            }
        }        
    }


// End of File

