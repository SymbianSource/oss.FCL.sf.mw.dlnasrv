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
* Description:      implementation for session towards a media server
*
*/






#ifndef C_UPNPUPLOADSESSION_H
#define C_UPNPUPLOADSESSION_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <upnpavcontrolpointobserver.h>

// INTERNAL INCLUDES
#include "httptransferobserver.h"
#include "upnpfiletransfersessionbase.h"
#include "tupnpfiletransferevent.h"

// FORWARD DECLARATIONS
class CHttpUploader;
class CUpnpFileTransferItem;
class CUPnPResourceHelper;
class CUpnpDlnaProtocolInfo;
class CUpnpObject;
class CUpnpAttribute;

// CLASS DECLARATION
/**
 * Implements AV Controller server side upload session
 *
 * @since S60 v3.2
 */
class CUPnPUploadSession :  public CUPnPFileTransferSessionBase,
                            public MHttpTransferObserver,
                            public MUpnpAVControlPointObserver
    {
        
public:

    /**
     * Static 1st phase constructor
     *
     * @param aServer AV Controller server handle
     * @param aSessionId session id
     * @param aUuid device Uuid
     */
    static CUPnPUploadSession* NewL( CUpnpAVControllerServer& aServer,
        TInt aSessionId, const TDesC8& aUuid );
    
    /**
     * Destructor
     */
    virtual ~CUPnPUploadSession();
    
private:

    /**
     * Private constructor
     *
     * @param aServer AV Controller server handle
     * @param aSessionId session id
     */
    CUPnPUploadSession( CUpnpAVControllerServer& aServer,
        TInt aSessionId );
    
    /**
     * 2ns phase constructor
     *
     * @param aUuid device uuid
     */
    void ConstructL( const TDesC8& aUuid );

private: // From MHttpTransferObserver
    
    /**
     * See httptransferobserver.h
     */
    void TransferProgress( TAny* aKey, TInt aBytes, TInt aTotalBytes );

    /**
     * See httptransferobserver.h
     */
    void ReadyForTransferL( TAny* aKey );

    /**
     * See httptransferobserver.h
     */
    void TransferCompleted( TAny* aKey, TInt aStatus );

private: // From MUpnpAVControlPointObserver

    /**
     * See upnpavcontrolpointobserver.h
     */
    void RcSetVolumeResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/, 
        const TDesC8& /*aInstance*/, 
        const TDesC8& /*aChannel*/, 
        const TDesC8& /*aDesiredVolume*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void RcVolumeResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/, 
        const TDesC8& /*aInstance*/, 
        const TDesC8& /*aChannel*/, 
        const TDesC8& /*aCurrentVolume*/) {}
         
    /**
     * See upnpavcontrolpointobserver.h
     */
    void RcSetMuteResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/, 
        const TDesC8& /*aInstance*/, 
        const TDesC8& /*aChannel*/, 
        const TDesC8& /*aDesiredMute*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void RcMuteResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/, 
        const TDesC8& /*aInstance*/, 
        const TDesC8& /*aChannel*/, 
        const TDesC8& /*aCurrentMute*/) {} 

    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtSetTransportUriResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aCurrentUri*/,
        const TDesC8& /*aCurrentUriMetaData*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    virtual void AvtSetNextTransportUriResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aNextUri*/,
        const TDesC8& /*aNextUriMetaData*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtMediaInfoResponse(
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
        const TDesC8& /*aWriteStatus*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtGetTransportInfoResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aCurrenTransportState*/,
        const TDesC8& /*aCurrentTransportStatus*/,
        const TDesC8& /*aCurrentSpeed*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtPositionInfoResponse(
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
        const TDesC8& /*aAbsCount*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtDeviceCapabilitiesResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aPlayMedia*/,
        const TDesC8& /*aRecMedia*/,
        const TDesC8& /*aRecQualityMode*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtTransportSettingsResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aPlayMode*/,
        const TDesC8& /*aRecQualityMode*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtStopResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtPlayResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aSpeed*/) {}
    
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtPauseResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtRecordResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtSeekResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aUnit*/,
        const TDesC8& /*aTarget*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtNextResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtPreviousResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/) {}
    
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtSetPlayModeResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aNewPlayMode*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtSetRecordModeResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aInstanceId*/,
        const TDesC8& /*aNewRecordQuality*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsSearchCapabilitiesResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aSearchCaps*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsSortCapabilitiesResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aSortCaps*/) {}

    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsSystemUpdateIdResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        TInt /*aSystemUpdateId*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsBrowseResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aObjectID*/,
        const TDesC8& /*aBrowseFlag*/,
        const TDesC8& /*aFilter*/,
        TInt /*aIndex*/,
        TInt /*aRequest*/,
        const TDesC8& /*aSortCriteria*/,
        const TDesC8& /*aResult*/,
        TInt /*aReturned*/,
        TInt /*aMatches*/,
        const TDesC8& /*aUpdateID*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsSearchResponse(
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
        const TDesC8& /*aUpdateID*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsDestroyObjectResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aObjectId*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsUpdateObjectResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aObjectId*/,
        const TDesC8& /*aCurrentTagValue*/,
        const TDesC8& /*aNewTagValue*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsImportResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aSourceURI*/,
        const TDesC8& /*aDestinationURI*/,
        const TDesC8& /*aTransferId*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsExportResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aSourceURI*/,
        const TDesC8& /*aDestinationURI*/,
        const TDesC8& /*aTransferId*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsStopTransferResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aTransferId*/ ) {}
    
    /**
     * See upnpavcontrolpointobserver.h
     */
    virtual void CdsCTransferProgressResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aTransferId*/,
        const TDesC8& /*aTransferStatus*/,
        const TDesC8& /*aTransferLength*/,            
        const TDesC8& /*aTransferTotal*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsDeleteResourceResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aResourceUri*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsCreateReferenceResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aContainerId*/, 
        const TDesC8& /*aObjectId*/, 
        const TDesC8& /*aNewId*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
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
     * See upnpavcontrolpointobserver.h
     */
    void CmProtocolInfoResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aSource*/, 
        const TDesC8& /*aSink*/ ) {}
    
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmPrepareResponse(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aRemoteProtocolInfo*/,
        const TDesC8& /*aPeerConnectionManager*/,
        const TDesC8& /*aPeerConnectionId*/,
        const TDesC8& /*aDirection*/,
        TInt /*aConnection*/,
        TInt /*aTransport*/,
        TInt /*aRsc*/ ) {}
    
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmComplete(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        TInt /*aConnection*/ ) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmCurrentConnections(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        const TDesC8& /*aConnections*/) {}
        
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmCurrentInfo(
        const TDesC8& /*aUuid*/,
        TInt /*aSessionId*/,
        TInt /*aErr*/,
        TInt /*rscId*/, 
        TInt /*transportId*/, 
        const TDesC8& /*aProtocolInfo*/,
        const TDesC8& /*aPeerConnectionManager*/, 
        TInt /*peerId*/, 
        const TDesC8& /*aDirection*/, 
        const TDesC8& /*aStatus*/ ) {}

    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsUpdateEvent(
            const TDesC8& /*aUuid*/,
            TInt /*aSystemUpdateId*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsContainerEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aConteinerIds*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CdsTransferEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aTransferIds*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void RcLastChangeEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aLastChange*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void AvtLastChangeEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aLastChange*/
            ) {}
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmSourceEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aSource*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmSinkEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aSink*/
            ) {}
            
    /**
     * See upnpavcontrolpointobserver.h
     */
    void CmConnectionsEvent(
            const TDesC8& /*aUuid*/,
            const TDesC8& /*aConnections*/
            ) {}

    /**
     * See upnpavcontrolpointobserver.h
     */
    void HttpResponseL( CUpnpHttpMessage* /*aMessage*/ ) {}

public:

    /**
     * See upnpavcontrolpointobserver.h
     */
    void DeviceDiscoveredL( CUpnpDevice* /*aDevice*/ ) {} 

    /**
     * See upnpavcontrolpointobserver.h
     */
    void DeviceDisappearedL( CUpnpDevice* /*aDevice*/ ) {}
     
public: // New functions

    /**
     * Starts upload
     *
     * @param aMessage message
     */
    void StartUploadL( const RMessage2& aMessage );
    
    /**
     * Cancels upload
     *
     * @param aMessage message
     */
    void CancelUploadL( const RMessage2& aMessage );

    /**
     * Cancels uploads
     *
     * @param aMessage message
     */
    void CancelAllUploadsL( const RMessage2& aMessage );
    
    /**
     * Start tracking progress
     *
     * @param aMessage message
     */
    void StartTrackingUploadProgressL( const RMessage2& aMessage );

    /**
     * Stop tracking progress
     *
     * @param aMessage message
     */
    void StopTrackingUploadProgressL( const RMessage2& aMessage );

    /**
     * Saves a message to receive events
     *
     * @param aMessage message
     */
    void GetUploadEventL( const RMessage2& aMessage );
    
    /**
     * Cancels events
     *
     * @param aMessage message
     */
    void CancelGetUploadEventL( const RMessage2& aMessage );

    /**
     * Handles UPnP device disappears.
     *
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );

private:

    /**
     * Set mandatory transfer headers
     *
     * @param aInfo protocolInfo
     * @param aKey transfer id
     */
    void SetHeadersL( const TDesC8& aInfo, TAny* aKey );
    
    /**
     * Prepares for upload
     *
     * @param aEvent transfer event
     */
    void ReadyForTransferL( TUpnpFileTransferEvent& aEvent );
  
    /**
     * Parsers CreateObject-response
     *
     * @param aResponse CreateObject-response
     * @return importUri
     */
    HBufC8* ParseCreateObjectResponseL( const TDesC8& aResponse );
    
    
    /**
     * Handles CreateObject-response
     *
     * @param aResponse aObjectID object id
     * @param aResult CreateObject-response
     */
    void HandleObjectResponseL( const TDesC8& aObjectID,
        const TDesC8& aResult );
        
    /**
     * Finds and return importUri from an item
     *
     * @param aObject UPnP item
     * @return importUri
     */
    HBufC8* ImportURIFromItemL( const CUpnpObject& aObject );
    
    /**
     * Converts DLNA compliant protocolInfo to UPnP protocolInfo
     *
     * @param aInfo DLNA protolInfo
     */
    void ProtocolInfoToUPnPL( const CUpnpAttribute* aInfo );    
    
    /**
     * Issues active scheduler's wait stop request(with/without callback)
     *
     * @param none
     * @return none
     */
    void StopWait();

    /**
     * Callback for active scheduler's wait stop request
     *
     */
    static TInt SchedulerStoppedCallBack( TAny* aPtr );
    
    /**
     * Does necessary action on active scheduler's wait stop complete
     *
     * @param none
     * @return none
     */
    void DoSchedulerStoppedCallBack();
    
private:

    /**
     * Http Uploader, used to upload files
     *
     * Owned
     */
    CHttpUploader*                          iUploader;
    
    /**
     * Used to synchronize CreateObject-action 
     */
    CActiveSchedulerWait                    iWait;
            
    /**
     * Resource helper array
     *
     * Owned
     */
    RPointerArray<CUPnPResourceHelper>      iResources;
        
    /**
     * Resource index
     */
    TInt                                    iResourceIndex;
    
    /**
     * Stores error (status) code from CreateObject-action
     */
    TInt                                    iAsyncError;
    
    /**
     * Session id for CreateObject-action
     */
    TInt                                    iIPSessionId;
    
    /**
     * Scheduler stop request
     */
    TBool                                   iSchedulerStopped;
    };

#endif // C_UPNPUPLOADSESSION_H

// End of file

