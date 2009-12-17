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
* Description:      dispatches control point indications for clients
*
*/






#ifndef C_CUPNPAVDISPATCHER_H
#define C_CUPNPAVDISPATCHER_H


#include <e32base.h>
#include <upnpavcontrolpointobserver.h>

// FORWARD DECLARATIONS
class CUPnPAVActionInfo;
class CUPnPAVControllerImpl;
class CUpnpAVControllerServer;
// CLASS DECLARATION

/**
*  UPnP AV Controller callback dispatcher
*  
*
*  @lib - 
*  @since Series 60 3.1
*/

class CUPnPAVDispatcher : public CBase,
                          public MUpnpAVControlPointObserver
    {
public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    static CUPnPAVDispatcher* NewLC( CUpnpAVControllerServer& aServer );
    
    /**
    * Two-phased constructor.
    */
    static CUPnPAVDispatcher* NewL( CUpnpAVControllerServer& aServer );    

    /**
    * Destructor.
    */
    virtual ~CUPnPAVDispatcher();

private: // New methods

    /**
    * Constructs the server 
    * @param aPriority CServer2 input parameter
    */
    CUPnPAVDispatcher( CUpnpAVControllerServer& aServer );

    /**
     * Perform the second phase construction of a CUpnpMessageHandler object
     */
    void ConstructL();

protected:

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
    void RcSetVolumeResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr, 
        const TDesC8& aInstance, 
        const TDesC8& aChannel, 
        const TDesC8& aDesiredVolume);
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
    void RcVolumeResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr, 
        const TDesC8& aInstance, 
        const TDesC8& aChannel, 
        const TDesC8& aCurrentVolume); 
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
    void RcSetMuteResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr, 
        const TDesC8& aInstance, 
        const TDesC8& aChannel, 
        const TDesC8& aDesiredMute);
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
    void RcMuteResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr, 
        const TDesC8& aInstance, 
        const TDesC8& aChannel, 
        const TDesC8& aCurrentMute); 


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
    void AvtSetTransportUriResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aCurrentUri,
        const TDesC8& aCurrentUriMetaData);
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
    void AvtSetNextTransportUriResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aNextUri,
        const TDesC8& aNextUriMetaData);
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
    void AvtMediaInfoResponse(
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
        const TDesC8& aWriteStatus);
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
    void AvtGetTransportInfoResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aCurrenTransportState,
        const TDesC8& aCurrentTransportStatus,
        const TDesC8& aCurrentSpeed);
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
    void AvtPositionInfoResponse(
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
        const TDesC8& aAbsCount);
    /**
    * Observer callback for rendering control AV get device
    * capabilities response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aPlayMedia
    * @param aRecMedia 
    * @param aRecQualityModes
    */
    void AvtDeviceCapabilitiesResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aPlayMedia,
        const TDesC8& aRecMedia,
        const TDesC8& aRecQualityMode);
    /**
    * Observer callback for rendering control AV get transport
    * settings response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId
    * @param aInstanceId
    * @param aErr UPnP error code.        
    * @param aPlayMode
    * @param aRecQualityMode
    */
    void AvtTransportSettingsResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aPlayMode,
        const TDesC8& aRecQualityMode);
    /**
    * Observer callback for rendering control AV Stop response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    */
    void AvtStopResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId);
    /**
    * Observer callback for rendering control AV Play response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aSpeed
    */
    void AvtPlayResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aSpeed);
    /**
    * Observer callback for rendering control AV Pause response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    */
    void AvtPauseResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId);
    /**
    * Observer callback for rendering control AV Record response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    */
    void AvtRecordResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId);
    /**
    * Observer callback for rendering control AV Seek response.
    * @since Series 60 2.6
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aUnit
    * @param aTarget
    */
    void AvtSeekResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aUnit,
        const TDesC8& aTarget);
    /**
    * Observer callback for rendering control AV Next response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    */
    void AvtNextResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId);
    /**
    * Observer callback for rendering control AV Previous response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    */
    void AvtPreviousResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId);
    /**
    * Observer callback for rendering control AV setplay mode response.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aNewPlayMode
    */
    void AvtSetPlayModeResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aNewPlayMode);
    /**
    * Observer callback for rendering control AV set record qualityresponse.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aNewRecordQuality
    */
    void AvtSetRecordModeResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aInstanceId,
        const TDesC8& aNewRecordQuality);
    /**
    * Observer callback for Content Directory GetSearchCapabilities function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aSearchCaps
    */
    void CdsSearchCapabilitiesResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSearchCaps);
    /**
    * Observer callback for Content Directory GetSortCapabilities function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aSortCaps
    */
    void CdsSortCapabilitiesResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSortCaps);
    /**
    * Observer callback for Content Directory GetSystemUpdateID function.
    * @since Series 60 2.6
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aSystemUpdateId
    */
    void CdsSystemUpdateIdResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        TInt aSystemUpdateId);
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
    void CdsBrowseResponse(
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
        const TDesC8&  aUpdateID);
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
    void CdsSearchResponse(
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
        const TDesC8& aUpdateID);
    /**
    * Observer callback for Content Directory DestroyObject function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aObjectId
    */
    void CdsDestroyObjectResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aObjectId );
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
    void CdsUpdateObjectResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aObjectId,
        const TDesC8& aCurrentTagValue,
        const TDesC8& aNewTagValue );
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
    void CdsImportResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSourceURI,
        const TDesC8& aDestinationURI,
        const TDesC8& aTransferId );
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
    void CdsExportResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSourceURI,
        const TDesC8& aDestinationURI,
        const TDesC8& aTransferId );
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
    void CdsStopTransferResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aTransferId );
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
    void CdsCTransferProgressResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aTransferId,
        const TDesC8& aTransferStatus,
        const TDesC8& aTransferLength,            
        const TDesC8& aTransferTotal );
    /**
    * Observer callback for Content Directory DeleteResource function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aResourceUri Resource to be deleted.
    */ 
    void CdsDeleteResourceResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aResourceUri );
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
    void CdsCreateReferenceResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aContainerId, 
        const TDesC8& aObjectId, 
        const TDesC8& aNewId );
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
    void CdsCreateObjectResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aContainerID, 
        const TDesC8& aElements, 
        const TDesC8& aObjectID, 
        const TDesC8& aResult );

    /**
    * Observer callback for Connection Manager GetProtocolInfo function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aSource
    * @param aSink
    */ 
    void CmProtocolInfoResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aSource, 
        const TDesC8& aSink );
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
    void CmPrepareResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aRemoteProtocolInfo,
        const TDesC8& aPeerConnectionManager,
        const TDesC8& aPeerConnectionId,
        const TDesC8& aDirection,
        TInt aConnection,
        TInt aTransport,
        TInt aRsc );
    /**
    * Observer callback for Connection Manager ConnectionComplete 
    * function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aConnection
    */         
    void CmComplete(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        TInt aConnection );
    /**
    * Observer callback for Connection Manager GetCurrentConnectionIDs
    * function.
    * @since Series 60 2.6
    * @param aUuid Source device UUID. 
    * @param aSessionId 
    * @param aErr UPnP error code.
    * @param aConnection
    */         
    void CmCurrentConnections(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aConnections);
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
    void CmCurrentInfo(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        TInt rscId, 
        TInt transportId, 
        const TDesC8& aProtocolInfo,
        const TDesC8& aPeerConnectionManager, 
        TInt peerId, 
        const TDesC8& aDirection, 
        const TDesC8& aStatus );

    //*****************************************************************
    //Functions for UPnP event handling
    //*****************************************************************
    /**
    * Content Directory SystemUpdateId event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aSystemUpdateId Device systemUpdateId.
    */
    void CdsUpdateEvent(
            const TDesC8& aUuid,
            TInt aSystemUpdateId
            ) ;
    /**
    * Content Directory ContainerUpdateIDs event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aConteinerIds Device container update ids.
    */
    void CdsContainerEvent(
            const TDesC8& aUuid,
            const TDesC8& aConteinerIds
            );
    /**
    * Content Directory TransferIDs event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aTransferIds Device transfer ids.
    */
    void CdsTransferEvent(
            const TDesC8& aUuid,
            const TDesC8& aTransferIds
            );
    /**
    * Rendering Control LastChange event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aLastChange Device LastChange statevariable.
    */
    void RcLastChangeEvent(
            const TDesC8& aUuid,
            const TDesC8& aLastChange
            );
    /**
    * AV Transport LastChange event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aLastChange Device LastChange statevariable.
    */
    void AvtLastChangeEvent(
            const TDesC8& aUuid,
            const TDesC8& aLastChange
            );
    /**
    * Connection Manager SourceEvent event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aSource Device's source info.
    */
    void CmSourceEvent(
            const TDesC8& aUuid,
            const TDesC8& aSource
            );
    /**
    * Connection Manager SinkEvent event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aSink Device's sink info.
    */
    void CmSinkEvent(
            const TDesC8& aUuid,
            const TDesC8& aSink
            );
    /**
    * Connection Manager ConnectionsEvent event handler.
    * @since Series 60 2.6
    * @param aUuid Device UUID.
    * @param aConnections Devices connections.
    */
    void CmConnectionsEvent(
            const TDesC8& aUuid,
            const TDesC8& aConnections
            );
    //*****************************************************************
    // Device and http functions.
    //*****************************************************************
    /**
    * Handles HTTP messages.
    * @since Series 60 2.6
    * @param aMessage Incoming HTTP message.
    */
    void HttpResponseL(CUpnpHttpMessage* aMessage) ;

    /**
    * Handles UPnP device discoveries.
    * @since Series 60 2.6
    * @param aDevice Device that is discovered.
    */
    void DeviceDiscoveredL(CUpnpDevice* aDevice);

    /**
    * Handles UPnP device disappears.
    * @since Series 60 2.6
    * @param aDevice Device that disappeared.
    */
    void DeviceDisappearedL(CUpnpDevice* aDevice);
    
public: // New methods

    void RegisterL( TInt aSessionId,
        MUpnpAVControlPointObserver& aObserver/*, const TDesC8& aUuid*/ );

    void UnRegister( TInt aSessionId /*, const TDesC8& aUuid*/ );

    void RegisterForEventsL( MUpnpAVControlPointObserver& aObserver,
        const TDesC8& aUuid );

    void UnRegisterEvents( MUpnpAVControlPointObserver& aObserver );

private: // New methods

    MUpnpAVControlPointObserver& FindObserver( TInt aSessionId );   

private:
    
    CUpnpAVControllerServer&            iServer;            

    /** @var iSessionCount the number of session owned by this server */
    RPointerArray<CUPnPAVActionInfo>    iActionInfos;

    //RPointerArray<CUPnPAVActionInfo>    iActionInfosDevice;

    RPointerArray<CUPnPAVActionInfo>    iActionInfosEvent;
    
    };


#endif // C_CUPNPAVDISPATCHER_H