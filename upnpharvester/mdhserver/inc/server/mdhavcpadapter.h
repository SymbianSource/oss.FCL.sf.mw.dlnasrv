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
* Description:      AvControlPoint adapter helper class
*
*/






#ifndef CMMDHAVCPADAPTER
#define CMMDHAVCPADAPTER

//  INCLUDES
#include "upnpavcontrolpointobserver.h"

// CLASS DECLARATION

/**
*  Interface class.
*  This class implements a observer interface for AV controlpoint
*
*  @since S60 3.1
*/
class CCmMdhAvControlPointAdapter: public CBase, 
                                   public MUpnpAVControlPointObserver
        {
    public:  // Constructors and destructor

       /**
        * Two-phased constructor.
        */
        static CCmMdhAvControlPointAdapter* NewL();

       /**
        * Two-phased constructor.
        */        
        static CCmMdhAvControlPointAdapter* NewLC();
        
       /**
        * Destructor
        */        
        virtual ~CCmMdhAvControlPointAdapter();
        
    protected:
    
       /**
        * ConstructL
        */    
        void ConstructL();
        
       /**
        * Default constructor
        */        
        CCmMdhAvControlPointAdapter();
        
        
    public: // Functions from base classes
    
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void RcSetVolumeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aDesiredVolume);
            public: // New functions
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void RcVolumeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aCurrentVolume); 
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void RcSetMuteResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aDesiredMute);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void RcMuteResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr, 
            const TDesC8& aInstance, 
            const TDesC8& aChannel, 
            const TDesC8& aCurrentMute); 
       
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtSetTransportUriResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aCurrentUri,
            const TDesC8& aCurrentUriMetaData);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtSetNextTransportUriResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNextUri,
            const TDesC8& aNextUriMetaData);
        
        /**
         * See upnpavcontrolpointobserver.h
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
            const TDesC8& aWriteStatus);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtGetTransportInfoResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aCurrenTransportState,
            const TDesC8& aCurrentTransportStatus,
            const TDesC8& aCurrentSpeed);
        
        /**
         * See upnpavcontrolpointobserver.h
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
            const TDesC8& aAbsCount);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtDeviceCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aPlayMedia,
            const TDesC8& aRecMedia,
            const TDesC8& aRecQualityMode);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtTransportSettingsResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aPlayMode,
            const TDesC8& aRecQualityMode);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtStopResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtPlayResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aSpeed);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtPauseResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtRecordResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtSeekResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aUnit,
            const TDesC8& aTarget);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtNextResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtPreviousResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtSetPlayModeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNewPlayMode);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtSetRecordModeResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aNewRecordQuality);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtCurrentTransportActionsResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aInstanceId,
            const TDesC8& aActions);
            
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsSearchCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSearchCaps);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsSortCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSortCaps);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsSystemUpdateIdResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt aSystemUpdateId);

        /**
         * See upnpavcontrolpointobserver.h
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
            const TDesC8&  aUpdateID);

        /**
         * See upnpavcontrolpointobserver.h
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
            const TDesC8& aUpdateID);
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsDestroyObjectResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aObjectId );
        
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsUpdateObjectResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aObjectId,
            const TDesC8& aCurrentTagValue,
            const TDesC8& aNewTagValue );

        /**
         * See upnpavcontrolpointobserver.h
         */       
        virtual void CdsImportResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSourceURI,
            const TDesC8& aDestinationURI,
            const TDesC8& aTransferId );

        /**
         * See upnpavcontrolpointobserver.h
         */      
        virtual void CdsExportResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSourceURI,
            const TDesC8& aDestinationURI,
            const TDesC8& aTransferId );

        /**
         * See upnpavcontrolpointobserver.h
         */      
        virtual void CdsStopTransferResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aTransferId );

        /**
         * See upnpavcontrolpointobserver.h
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
         * See upnpavcontrolpointobserver.h
         */ 
        virtual void CdsDeleteResourceResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aResourceUri );

        /**
         * See upnpavcontrolpointobserver.h
         */ 
        virtual void CdsCreateReferenceResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aContainerId, 
            const TDesC8& aObjectId, 
            const TDesC8& aNewId );

        /**
         * See upnpavcontrolpointobserver.h
         */ 
        virtual void CdsCreateObjectResponse(
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
        virtual void CmProtocolInfoResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSource, 
            const TDesC8& aSink );

        /**
         * See upnpavcontrolpointobserver.h
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
            TInt aRsc );

        /**
         * See upnpavcontrolpointobserver.h
         */        
        virtual void CmComplete(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt aConnection );

        /**
         * See upnpavcontrolpointobserver.h
         */         
        virtual void CmCurrentConnections(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aConnections);

        /**
         * See upnpavcontrolpointobserver.h
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
            const TDesC8& aStatus );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsUpdateEvent(
                const TDesC8& aUuid,
                TInt aSystemUpdateId
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsContainerEvent(
                const TDesC8& aUuid,
                const TDesC8& aConteinerIds
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CdsTransferEvent(
                const TDesC8& aUuid,
                const TDesC8& aTransferIds
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void RcLastChangeEvent(
                const TDesC8& aUuid,
                const TDesC8& aLastChange
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void AvtLastChangeEvent(
                const TDesC8& aUuid,
                const TDesC8& aLastChange
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CmSourceEvent(
                const TDesC8& aUuid,
                const TDesC8& aSource
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CmSinkEvent(
                const TDesC8& aUuid,
                const TDesC8& aSink
                );

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void CmConnectionsEvent(
                const TDesC8& aUuid,
                const TDesC8& aConnections
                );
        //*****************************************************************
        // Device and http functions.
        //*****************************************************************
        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void HttpResponseL(CUpnpHttpMessage* aMessage);

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void DeviceDiscoveredL(CUpnpDevice* aDevice);

        /**
         * See upnpavcontrolpointobserver.h
         */
        virtual void DeviceDisappearedL(CUpnpDevice* aDevice);
    };

#endif      // CMMDHAVCPADAPTER   
            
// End of File