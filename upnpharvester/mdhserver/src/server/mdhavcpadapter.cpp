/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      AvControlPoint adapter helper class implementation
*
*/






#include "mdhavcpadapter.h"

// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
// 
CCmMdhAvControlPointAdapter* CCmMdhAvControlPointAdapter::NewL()
    {
    CCmMdhAvControlPointAdapter* self = 
        CCmMdhAvControlPointAdapter::NewLC( );
    CleanupStack::Pop( self );
    return self;      
    }

// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
//     
CCmMdhAvControlPointAdapter* CCmMdhAvControlPointAdapter::NewLC()
    {
    CCmMdhAvControlPointAdapter* self = 
        new ( ELeave ) CCmMdhAvControlPointAdapter( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }
   
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//     
CCmMdhAvControlPointAdapter::~CCmMdhAvControlPointAdapter()
    {
    
    }
  
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmMdhAvControlPointAdapter::CCmMdhAvControlPointAdapter()
    {
    
    }   

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//         
void CCmMdhAvControlPointAdapter::ConstructL()
    {
    
    }  

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//          
void CCmMdhAvControlPointAdapter::RcSetVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aDesiredVolume*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::RcVolumeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aCurrentVolume*/) { /*nothing*/ } 

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::RcSetMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aDesiredMute*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::RcMuteResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/, 
    const TDesC8& /*aInstance*/, 
    const TDesC8& /*aChannel*/, 
    const TDesC8& /*aCurrentMute*/) { /*nothing*/ } 

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtSetTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aCurrentUri*/,
    const TDesC8& /*aCurrentUriMetaData*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtSetNextTransportUriResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNextUri*/,
    const TDesC8& /*aNextUriMetaData*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtMediaInfoResponse(
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
    const TDesC8& /*aWriteStatus*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtGetTransportInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aCurrenTransportState*/,
    const TDesC8& /*aCurrentTransportStatus*/,
    const TDesC8& /*aCurrentSpeed*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtPositionInfoResponse(
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
    const TDesC8& /*aAbsCount*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtDeviceCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMedia*/,
    const TDesC8& /*aRecMedia*/,
    const TDesC8& /*aRecQualityMode*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtTransportSettingsResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aPlayMode*/,
    const TDesC8& /*aRecQualityMode*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtStopResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtPlayResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aSpeed*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtPauseResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtRecordResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtSeekResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aUnit*/,
    const TDesC8& /*aTarget*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtNextResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/) { /*nothing*/ }
/**
* Observer callback for rendering control AV Previous response.
* @since Series 60 2.0
* @param aUuid Source device UUID. 
* @param aSessionId 
* @param aErr UPnP error code.
*/
void CCmMdhAvControlPointAdapter::AvtPreviousResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtSetPlayModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewPlayMode*/) { /*nothing*/ }
/**
* Observer callback for rendering control AV set record qualityresponse.
* @since Series 60 2.0
* @param aUuid Source device UUID. 
* @param aSessionId 
* @param aErr UPnP error code.
* @param aNewRecordQuality
*/
void CCmMdhAvControlPointAdapter::AvtSetRecordModeResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aNewRecordQuality*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtCurrentTransportActionsResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aInstanceId*/,
    const TDesC8& /*aActions*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsSearchCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSearchCaps*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsSortCapabilitiesResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSortCaps*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsSystemUpdateIdResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aSystemUpdateId*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsBrowseResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectID*/,
    const TDesC8& /* aBrowseFlag*/,
    const TDesC8& /* aFilter*/,
    TInt /*aIndex*/,
    TInt /*aRequest*/,
    const TDesC8& /* aSortCriteria*/,
    const TDesC8& /* aResult*/,
    TInt /*aReturned*/,
    TInt /*aMatches*/,
    const TDesC8& /* aUpdateID*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsSearchResponse(
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
    const TDesC8& /*aUpdateID*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsDestroyObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectId */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsUpdateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aObjectId*/,
    const TDesC8& /*aCurrentTagValue*/,
    const TDesC8& /*aNewTagValue */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//       
void CCmMdhAvControlPointAdapter::CdsImportResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& /*aTransferId */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//       
void CCmMdhAvControlPointAdapter::CdsExportResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSourceURI*/,
    const TDesC8& /*aDestinationURI*/,
    const TDesC8& /*aTransferId */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//        
void CCmMdhAvControlPointAdapter::CdsStopTransferResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aTransferId */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//         
void CCmMdhAvControlPointAdapter::CdsCTransferProgressResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aTransferId*/,
    const TDesC8& /*aTransferStatus*/,
    const TDesC8& /*aTransferLength*/,            
    const TDesC8& /*aTransferTotal */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//  
void CCmMdhAvControlPointAdapter::CdsDeleteResourceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aResourceUri */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//  
void CCmMdhAvControlPointAdapter::CdsCreateReferenceResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerId*/, 
    const TDesC8& /*aObjectId*/, 
    const TDesC8& /*aNewId */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//  
void CCmMdhAvControlPointAdapter::CdsCreateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aContainerID*/, 
    const TDesC8& /*aElements*/, 
    const TDesC8& /*aObjectID*/, 
    const TDesC8& /*aResult */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//  
void CCmMdhAvControlPointAdapter::CmProtocolInfoResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aSource*/, 
    const TDesC8& /*aSink */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//         
void CCmMdhAvControlPointAdapter::CmPrepareResponse(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aRemoteProtocolInfo*/,
    const TDesC8& /*aPeerConnectionManager*/,
    const TDesC8& /*aPeerConnectionId*/,
    const TDesC8& /*aDirection*/,
    TInt /*aConnection*/,
    TInt /*aTransport*/,
    TInt /*aRsc */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//          
void CCmMdhAvControlPointAdapter::CmComplete(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*aConnection */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//          
void CCmMdhAvControlPointAdapter::CmCurrentConnections(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    const TDesC8& /*aConnections*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
//    
void CCmMdhAvControlPointAdapter::CmCurrentInfo(
    const TDesC8& /*aUuid*/,
    TInt /*aSessionId*/,
    TInt /*aErr*/,
    TInt /*rscId*/, 
    TInt /*transportId*/, 
    const TDesC8& /*aProtocolInfo*/,
    const TDesC8& /*aPeerConnectionManager*/, 
    TInt /*peerId*/, 
    const TDesC8& /*aDirection*/, 
    const TDesC8& /*aStatus */) { /*nothing*/ }

//*****************************************************************
//Functions for UPnP event handling
//*****************************************************************
// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsUpdateEvent(
        const TDesC8& /*aUuid*/,
        TInt /*aSystemUpdateId
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsContainerEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConteinerIds
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CdsTransferEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aTransferIds
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::RcLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aLastChange
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::AvtLastChangeEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aLastChange
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CmSourceEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSource
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CmSinkEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aSink
        */) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::CmConnectionsEvent(
        const TDesC8& /*aUuid*/,
        const TDesC8& /*aConnections
        */) { /*nothing*/ }

//*****************************************************************
// Device and http functions.
//*****************************************************************
// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::HttpResponseL(
    CUpnpHttpMessage* /*aMessage*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::DeviceDiscoveredL(
    CUpnpDevice* /*aDevice*/) { /*nothing*/ }

// ---------------------------------------------------------------------------
// See upnpavcontrolpointobserver.h
// ---------------------------------------------------------------------------
// 
void CCmMdhAvControlPointAdapter::DeviceDisappearedL(
    CUpnpDevice* /*aDevice*/) { /*nothing*/ }
            
// End of File
