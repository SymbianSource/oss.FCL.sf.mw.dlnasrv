/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  MUpnpAVControlPointObserver
*
*/



#ifndef C_MUPNPAVCONTROLPOINTOBSERVER_H
#define C_MUPNPAVCONTROLPOINTOBSERVER_H

//  INCLUDES
#include "upnpdevice.h"

// CLASS DECLARATION

/**
*  Interface class.
*  This class defines a observer interface for AV controlpoint
*
*  @since Series 60 2.6
*/
class MUpnpAVControlPointObserver
        {
    public:  // Constructors and destructor

    public: // New functions
        /**
        * Observer callback for rendering control Set Volume.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aChannel audio channel.
        * @param aDesiredVolume 
        */
        virtual void RcSetVolumeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aDesiredVolume) = 0;
            public: // New functions
        /**
        * Observer callback for rendering control Get Volume.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aChannel audio channel.
        * @param aCurrentVolume 
        */
        virtual void RcVolumeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aCurrentVolume) = 0; 
        /**
        * Observer callback for rendering control Set Mute.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aChannel audio channel.
        * @param aDesiredMute 
        */
        virtual void RcSetMuteResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aDesiredMute) = 0;
        /**
        * Observer callback for rendering control Get Mute.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aChannel audio channel.
        * @param aCurrentMute 
        */
        virtual void RcMuteResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aCurrentMute) = 0; 


        /**
        * Observer callback for rendering control AV set transport uri.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aCurrentUri Current transport URI.
        * @param Current transport URI Metadata. 
        */
        virtual void AvtSetTransportUriResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aCurrentUri,
            const TDesC8& aCurrentUriMetaData) = 0;
        /**
        * Observer callback for rendering control AV set next transport uri.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aCurrentUri Current transport URI.
        * @param Current transport URI Metadata. 
        */
        virtual void AvtSetNextTransportUriResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNextUri,
            const TDesC8& aNextUriMetaData) = 0;
        /**
        * Observer callback for rendering control AV get media info response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aNrTracks Number of tracks.
        * @param aMediaDuration 
        * @param aCurrentUri Current transport URI.
        * @param aCurrentUriMetaData Metadata of current trransport uri. 
        * @param aNextUri Next transport URI.
        * @param aNextUriMetaData Metadata of next trransport uri.
        * @param aPlayMedium
        * @param aRecordMedium
        * @param aWriteStatus
        */
        virtual void AvtMediaInfoResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNrTracks,
            const TDesC8& aMediaDuration,
            const TDesC8& aCurrentUri,
            const TDesC8& aCurrentUriMetaData,
            const TDesC8& aNextUri,
            const TDesC8& aNextUriMetaData,
            const TDesC8& aPlayMedium,
            const TDesC8& aRecordMedium,
            const TDesC8& aWriteStatus) = 0;
        /**
        * Observer callback for rendering control AV get transport info response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId
        * @param aErr UPnP error code.
        * @param aInstance rendering instance.
        * @param aCurrenTransportState
        * @param aCurrentTransportStatus
        * @param aCurrentUri aCurrentSpeed
        */
        virtual void AvtGetTransportInfoResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aCurrenTransportState,
            const TDesC8& aCurrentTransportStatus,
            const TDesC8& aCurrentSpeed) = 0;
        /**
        * Observer callback for rendering control AV get position info response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aTrack rendering instance.
        * @param aTrackDuration 
        * @param aTrackMetaData
        * @param aTrackURI
        * @param aRelTime
        * @param aAbsTime
        * @param aRelCount
        * @param aAbsCount
        */
        virtual void AvtPositionInfoResponse(
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
            const TDesC8& aAbsCount) = 0;
        /**
        * Observer callback for rendering control AV get device capabilities response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aPlayMedia
        * @param aRecMedia 
        * @param aRecQualityModes
        */
        virtual void AvtDeviceCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aPlayMedia,
            const TDesC8& aRecMedia,
            const TDesC8& aRecQualityMode) = 0;
        /**
        * Observer callback for rendering control AV get transport settings response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId
        * @param aInstanceId
        * @param aErr UPnP error code.        
        * @param aPlayMode
        * @param aRecQualityMode
        */
        virtual void AvtTransportSettingsResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aPlayMode,
            const TDesC8& aRecQualityMode) = 0;
        /**
        * Observer callback for rendering control AV Stop response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        */
        virtual void AvtStopResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId) = 0;
        /**
        * Observer callback for rendering control AV Play response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSpeed
        */
        virtual void AvtPlayResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aSpeed) = 0;
        /**
        * Observer callback for rendering control AV Pause response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        */
        virtual void AvtPauseResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId) = 0;
        /**
        * Observer callback for rendering control AV Record response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        */
        virtual void AvtRecordResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId) = 0;
        /**
        * Observer callback for rendering control AV Seek response.
        * @since Series 60 2.6
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aUnit
        * @param aTarget
        */
        virtual void AvtSeekResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aUnit,
            const TDesC8& aTarget) = 0;
        /**
        * Observer callback for rendering control AV Next response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        */
        virtual void AvtNextResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId) = 0;
        /**
        * Observer callback for rendering control AV Previous response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        */
        virtual void AvtPreviousResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId) = 0;
        /**
        * Observer callback for rendering control AV setplay mode response.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aNewPlayMode
        */
        virtual void AvtSetPlayModeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNewPlayMode) = 0;
        /**
        * Observer callback for rendering control AV set record qualityresponse.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aNewRecordQuality
        */
        virtual void AvtSetRecordModeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNewRecordQuality) = 0;
        /**
        * Observer callback for Content Directory GetSearchCapabilities function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSearchCaps
        */
        virtual void CdsSearchCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSearchCaps) = 0;
        /**
        * Observer callback for Content Directory GetSortCapabilities function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSortCaps
        */
        virtual void CdsSortCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSortCaps) = 0;
        /**
        * Observer callback for Content Directory GetSystemUpdateID function.
        * @since Series 60 2.6
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSystemUpdateId
        */
        virtual void CdsSystemUpdateIdResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt aSystemUpdateId) = 0;
        /**
        * Observer callback for Content Directory Browse function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aObjectID
        * @param aBrowseFlag
        * @param aFilter
        * @param aIndex
        * @param arequest
        * @param aSortCriteria
        * @param aResult
        * @param aReturned
        * @param aMatches
        * @param aUpdateID
        */
        virtual void CdsBrowseResponse(
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
            const TDesC8&  aUpdateID) = 0;
        /**
        * Observer callback for Content Directory Search function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aContainerId
        * @param aSearchCriteria
        * @param aFilter
        * @param aIndex
        * @param arequest
        * @param aSortCriteria
        * @param aResult
        * @param aReturned
        * @param aMatches
        * @param aUpdateID
        */
        virtual void CdsSearchResponse(
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
            const TDesC8& aUpdateID) = 0;
        /**
        * Observer callback for Content Directory DestroyObject function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aObjectId
        */
        virtual void CdsDestroyObjectResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aObjectId ) = 0;
        /**
        * Observer callback for Content Directory UpdateObject function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aObjectId
        * @param aCurrentTagValue
        * @param aNewTagValue
        */
        virtual void CdsUpdateObjectResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aObjectId,
            const TDesC8& aCurrentTagValue,
            const TDesC8& aNewTagValue ) = 0;
        /**
        * Observer callback for Content Directory ImportResource function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSourceURI
        * @param aDestinationURI
        * @param aTransferId
        */       
        virtual void CdsImportResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSourceURI,
            const TDesC8& aDestinationURI,
            const TDesC8& aTransferId ) = 0;
        /**
        * Observer callback for Content Directory ExportResource function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSourceURI
        * @param aDestinationURI
        * @param aTransferId
        */       
        virtual void CdsExportResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSourceURI,
            const TDesC8& aDestinationURI,
            const TDesC8& aTransferId ) = 0;
        /**
        * Observer callback for Content Directory StopTransfer function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSourceURI
        * @param aDestinationURI
        * @param aTransferId
        */       
        virtual void CdsStopTransferResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aTransferId ) = 0;
        /**
        * Observer callback for Content Directory GetTransferProgress function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aTransferId
        * @param aTransferStatus
        * @param aTransferTotal
        */         
        virtual void CdsCTransferProgressResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aTransferId,
            const TDesC8& aTransferStatus,
            const TDesC8& aTransferLength,            
            const TDesC8& aTransferTotal ) = 0;
        /**
        * Observer callback for Content Directory DeleteResource function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aResourceUri Resource to be deleted.
        */ 
        virtual void CdsDeleteResourceResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aResourceUri ) = 0;
        /**
        * Observer callback for Content Directory CreateReference function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aContainerId Destination folder.
        * @param aObjectId Source object.
        * @param aNewId Created reference.
        */ 
        virtual void CdsCreateReferenceResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aContainerId, 
            const TDesC8& aObjectId, 
            const TDesC8& aNewId ) = 0;
        /**
        * Observer callback for Content Directory CreateReference function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aContainerID Container in which new object is created.
        * @param aElements Elements for item creation.
        * @param aObjectID New objects ID.
        * @param aResult Result of the action.
        */ 
        virtual void CdsCreateObjectResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aContainerID, 
            const TDesC8& aElements, 
            const TDesC8& aObjectID, 
            const TDesC8& aResult ) = 0;

        /**
        * Observer callback for Connection Manager GetProtocolInfo function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSource
        * @param aSink
        */ 
        virtual void CmProtocolInfoResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSource, 
            const TDesC8& aSink ) = 0;
        /**
        * Observer callback for Connection Manager PrepareForConnection 
        * function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aRemoteProtocolInfo
        * @param aPeerConnectionManager
        * @param aPeerConnectionId
        * @param aDirection
        * @param aConnection
        * @param aTransport
        * @param aRsc
        */        
        virtual void CmPrepareResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aRemoteProtocolInfo,
            const TDesC8& aPeerConnectionManager,
            const TDesC8& aPeerConnectionId,
            const TDesC8& aDirection,
            TInt aConnection,
            TInt aTransport,
            TInt aRsc ) = 0;
        /**
        * Observer callback for Connection Manager ConnectionComplete 
        * function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aConnection
        */         
        virtual void CmComplete(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt aConnection ) = 0;
        /**
        * Observer callback for Connection Manager GetCurrentConnectionIDs
        * function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aConnection
        */         
        virtual void CmCurrentConnections(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aConnections) = 0;
        /**
        * Observer callback for Connection Manager GetCurrentConnectionInfo
        * function.
        * @since Series 60 2.6
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param rscId
        * @param aProtocolInfo
        * @param aPeerConnectionManager
        * @param peerId
        * @param aStatus
        */   
        virtual void CmCurrentInfo(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt rscId, 
            TInt transportId, 
            const TDesC8& aProtocolInfo,
            const TDesC8& aPeerConnectionManager, 
            TInt peerId, 
            const TDesC8& aDirection, 
            const TDesC8& aStatus ) = 0;

        //*****************************************************************
        //Functions for UPnP event handling
        //*****************************************************************
        /**
        * Content Directory SystemUpdateId event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aSystemUpdateId Device systemUpdateId.
        */
        virtual void CdsUpdateEvent(
                const TDesC8& aUuid,
                TInt aSystemUpdateId
                ) = 0;
        /**
        * Content Directory ContainerUpdateIDs event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aConteinerIds Device container update ids.
        */
        virtual void CdsContainerEvent(
                const TDesC8& aUuid,
                const TDesC8& aConteinerIds
                ) = 0;
        /**
        * Content Directory TransferIDs event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aTransferIds Device transfer ids.
        */
        virtual void CdsTransferEvent(
                const TDesC8& aUuid,
                const TDesC8& aTransferIds
                ) = 0;
        /**
        * Rendering Control LastChange event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aLastChange Device LastChange statevariable.
        */
        virtual void RcLastChangeEvent(
                const TDesC8& aUuid,
                const TDesC8& aLastChange
                ) = 0;
        /**
        * AV Transport LastChange event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aLastChange Device LastChange statevariable.
        */
        virtual void AvtLastChangeEvent(
                const TDesC8& aUuid,
                const TDesC8& aLastChange
                ) = 0;
        /**
        * Connection Manager SourceEvent event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aSource Device's source info.
        */
        virtual void CmSourceEvent(
                const TDesC8& aUuid,
                const TDesC8& aSource
                ) = 0;
        /**
        * Connection Manager SinkEvent event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aSink Device's sink info.
        */
        virtual void CmSinkEvent(
                const TDesC8& aUuid,
                const TDesC8& aSink
                ) = 0;
        /**
        * Connection Manager ConnectionsEvent event handler.
        * @since Series 60 2.6
        * @param aUuid Device UUID.
        * @param aConnections Devices connections.
        */
        virtual void CmConnectionsEvent(
                const TDesC8& aUuid,
                const TDesC8& aConnections
                ) = 0;
        //*****************************************************************
        // Device and http functions.
        //*****************************************************************
        /**
        * Handles HTTP messages.
        * @since Series 60 2.6
        * @param aMessage Incoming HTTP message.
        */
        virtual void HttpResponseL(CUpnpHttpMessage* aMessage) = 0;

        /**
        * Handles UPnP device discoveries.
        * @since Series 60 2.6
        * @param aDevice Device that is discovered.
        */
        virtual void DeviceDiscoveredL(CUpnpDevice* aDevice) = 0;

        /**
        * Handles UPnP device disappears.
        * @since Series 60 2.6
        * @param aDevice Device that disappeared.
        */
        virtual void DeviceDisappearedL(CUpnpDevice* aDevice) = 0;
    };

#endif      // C_MUPNPAVCONTROLPOINTOBSERVER_H
            
// End of File
