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
* Description:      implementation for session towards a media server
*
*/







#ifndef C_UPNPBROWSINGSESSION_H
#define C_UPNPBROWSINGSESSION_H

// INCLUDE FILES
#include <e32base.h>
#include "upnpavcontroller.h"
#include <upnpavcontrolpointobserver.h>
#include <upnpmediaserverobserver.h>
#include "upnpavbrowsingsession.h"


// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpObject;
class CUpnpAVRequest;
class CUpnpAVBrowseRequest;
class CUPnPFileSharingActive;
class CUpnpMediaServerNotifier;
class RUpnpMediaServerClient;
class CUpnpAVDeviceExtended;
class CUpnpMediaServerSettings;
class CRepository;
class CUpnpAVControllerServer;
/**
 * Implements server side browsing session functionality. Implements
 * browsing, searching, copying etc.  
 *
 * @since S60 v3.1
 */
class CUPnPBrowsingSession :    public CBase,
                                public MUpnpAVControlPointObserver,
                                public MUpnpMediaServerObserver
    {

private:

    /**
     * Defines the internal state of browsing session
     */
    enum TInternalBrowseState
        {
        ENone,
        EBrowse,
        EDestroyObject,
        ECopyLocal,
        ECreateContainer,
        ECopyToPhone
        };

    enum TInternalCopyState
        {
        EIdle,
        EObjectCreated,
        ECopying
        };

public:

    /**
     * Static 1st phase constructor
     *
     * @param aControlPoint AV Control Point reference
     * @param aClient S60 Media Server session reference
     * @param aDispatcher observer callback dispatcher reference
     * @param aSessionId session id
     * @param aUuid device Uuid
     */
    static CUPnPBrowsingSession* NewL
        (
        RUpnpMediaServerClient& aClient,
        CUpnpAVControllerServer& aServer,
        TInt aSessionId,   
        const TDesC8& aUuid
        );
    
    /**
     * Destructor
     */
    virtual ~CUPnPBrowsingSession();
    
private:

    /**
     * Private constructor
     *
     * @param aControlPoint AV Control Point reference
     * @param aClient S60 Media Server session reference
     * @param aDispatcher observer callback dispatcher reference
     * @param aSessionId session id
     */
    CUPnPBrowsingSession
        (
        RUpnpMediaServerClient& aClient,
        CUpnpAVControllerServer& aServer,
        TInt aSessionId
        );    
    
    /**
     * 2ns phase constructor
     */
    void ConstructL( const TDesC8& aUuid );

protected: // From MUpnpAVControlPointObserver

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
    virtual void AvtSetNextTransportUriResponse(
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
    virtual void CdsCTransferProgressResponse(
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
            );
            
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
    void HttpResponseL( CUpnpHttpMessage* aMessage );

public:

    /**
     * Handles UPnP device discoveries.
     * @since Series 60 2.6
     * @param aDevice Device that is discovered.
     */
    void DeviceDiscoveredL( CUpnpDevice* aDevice );

    /**
     * Handles UPnP device disappears.
     * @since Series 60 2.6
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpDevice* aDevice );    
 
protected: // From CUpnpMediaServerNotifier

    /**
     * Called when Content Directory file transfer is finished
     * either with success or failure.
     * See CUpnpFileTransferEvent class description for details.
     *
     * @since Series 60 3.1
     * @param aEvents Media Server event
     * @return notifier object
     */
    void FileTransferEvent( CUpnpFileTransferEvent *aEvent );
    
    /**
     * Called when Content Directory file transfer is finished
     * and iInternalState == ECopyToPhone
     * @since Series 60 3.2
     * @param aEvent Media Server event
     * @param aError from FileTransferEvent
     */
    void HandleCopyToPhoneEventL( 
        CUpnpFileTransferEvent& aEvent, 
        TInt aError );

    /**
     * Called in case of CUpnpMediaServerNotifier internal error.
     * Should be used for event handling recovery.
     *
     * @since Series 60 3.1
     * @param aObserver event observer
     * @return notifier object
     */
    void NotifierError( TInt aError );
    
public: // New functions

    /**
     * Handles UPnP device disappears.
     *
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Sets local (S60) Media Server Uuid
     *
     * @param aUuid device Uuid
     */
    void SetLocalMSUuidL( const TDesC8& aUuid );

    /**
     * Returns Session Id
     *
     * @return session id
     */
    TInt SessionId() const;
    
    /**
     * Returns device Uuid 
     *
     * @return device Uuid
     */
    const TDesC8& Uuid() const;
    
public:

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetBrowseResponseSizeL( const RMessage2& aMessage );  

    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetBrowseResponseSizeL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetBrowseResponseL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchResponseSizeL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetSearchResponseSizeL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchResponseL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchCapabitiesSizeL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetSearchCapabitiesSizeL();     
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchCapabitiesL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CreateContainerL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelCreateContainerL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void DeleteObjectL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDeleteObjectL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void DeviceDisappearedRequestL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDeviceDisappearedRequestL();
                                   
private:

    /**
     * Parses create object -action response and returns import uri.
     *
     * @param aResponse create object -action response
     * @return import uri (ownership is transferred)
     */
    HBufC8* ParseCreateObjectResponseL( const TDesC8& aResponse );

    /**
     * Parses browse response and checks if the container supports the 
     * media type we are about to copy into it
     *
     * @param aResponse browse response
     */
    void CheckIsCreateObjectSupportedL( const TDesC8& aResponse );

    /**
     * Parses browse response and checks if the container supports the 
     * media type we are about to copy into it. After that it sends
     * a create object -action.
     *
     * @param aResponse browse response
     */
    void CheckAndSendCreateObjectActionL( const TDesC8& aResponse );

    /**
     * Parses browse response and checks if the object can be deleted. If
     * supported, sends destroyobject-action.
     *
     * @param aResponse browse response
     */
    void CheckAndSendDestroyObjectActionL( const TDesC8& aResponse );    
    
    /**
     * Sends a create object -action
     *
     * @param aUuid device uuid
     * @param aContainerId container id
     * @param aResponse elements xml
     */
    void SendCreateObjectActionL( const TDesC8& aUuid,
        const TDesC8& aContainerId, const TDesC8& aResponse );

    /**
     * Sends export resource -action
     */
    void SendExportActionL();
    
    /**
     * Sends import resource -action
     */
    void SendImportActionL();

    /**
     * Resets internal state
     */
    void ResetL();
        
    /**
     * Postprocesses copy operation
     *
     * @param aError error code in copying
     * @param aFinished state of copy (create object/ import/export action)
     */
    void CopyFinished( TInt aError, TBool aFinished ); 
    
    /**
     * Reads an object from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aObj UPnP object
     */
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpObject* aObj );
        
    /**
     * Reads a request from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aReq request
     */
    void ReadReqFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpAVRequest* aReq );
        
    /**
     * Reads a browse request from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aReq browse request
     */
    void ReadBrowseReqFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpAVBrowseRequest* aReq );

    /**
     * Reads a buffer from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @return a buffer (heap descriptor)
     */
    HBufC8* ReadBufFromMessageLC( const RMessage2& aMessage, TInt aSlot );

    /**
     * Sets download settings. Not currently needed, for future use.
     *
     * @param aType download location
     */
    void SetDownloadSettingsL( MUPnPAVBrowsingSession::TMemoryType aType );
    
    /**
     * Restores download settings. Not currently needed, for future use.
     *
     * @return error code
     */
    TInt RestoreDownloadSettings();
    
    /**
     * Checks sharing status. Determines if the copied file is left shared or
     * not.
     *
     * @return error code
     */
    TInt CheckSharingStatus();                      

private:

    RUpnpMediaServerClient&     iMediaServer; // Not own

    CUpnpAVControllerServer&    iServer;
    
    TInt                        iSessionId;

    TInt                        iInstanceId;
    
    TInt                        iIPSessionId;

    TInt                        iTransferId;
    
    TBool                       iTransferEventReceived;
    
    TInternalBrowseState        iInternalState;
    
    TInternalCopyState          iCopyState;
    
    RMessage2*                  iActionMessage; // Own
    
    RMessage2*                  iDeviceMessage; // Own
    
    CUpnpAVDeviceExtended*      iDevice; // Own
    
    HBufC8*                     iLocalMediaServerUuid; // Own
    
    HBufC8*                     iRespBuf; // Own
    
    HBufC8*                     iRespBuf2; // Own  
        
    HBufC8*                     iImportURI; // Own
    
    HBufC8*                     iSourceURI; // Own
    
    HBufC8*                     iItemId; // Own
    
    HBufC8*                     iContainerId; // Own
    
    CUpnpItem*                  iSharedItem; // Own  
    
    CUPnPFileSharingActive*     iFileSharing; // Own
      
    CUpnpMediaServerNotifier*   iMediaServerNotifier; // Own
    
    HBufC8*                     iOriginalLocation; // Own
    
    HBufC8*                     iFilePath; // Own
    
    CUpnpMediaServerSettings*   iMSSettings; // Own, For future use
    
    CRepository*                iAppSettings; // Own

    MUPnPAVBrowsingSession::TFileSharing iShareFlag;    
    
    TBool                       iMusic;
    
    TInt                        iAsyncErr;
    
    };

#endif // C_UPNPBROWSINGSESSION_H
