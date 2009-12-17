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
* Description:  AV control point class
*
*/



#ifndef C_CUPNPAVCONTROLPOINT_H
#define C_CUPNPAVCONTROLPOINT_H


//  INCLUDES
#include "upnpavcontrolpointobserver.h"
#include "upnpcontrolpoint.h"
#include "upnphttpmessage.h"

// FORWARD DECLARATIONS
class CUpnpActionResponseHandler;
class CUpnpStateUpdateHandler;
class CUpnpAvtActionFactory;
class CUpnpCdsActionFactory;
class CUpnpCmActionFactory;
class CUpnpRcActionFactory;
// CLASS DECLARATION

/**
*  Main class of AV Control Point.
*  This class serves as an interface to AVCP library and contais functions for
*  UPnP Action creation and sending as well as receiving Action responses and
*  UPnP events.
*
*  Prior using the CUpnpAVControlPoint class, client application has to create 
*  the network connection. If connection is not up and 
*  running, the CUpnpDispatcherEngine class construction will fail. 
*  See UPnP Interface Design document for instuctions.
*
*  @since Series 60 2.6
*/
class CUpnpAVControlPoint : public CUpnpControlPoint
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpAVControlPoint* NewL(
        MUpnpAVControlPointObserver& aAVControlPointObserver            
        );
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CUpnpAVControlPoint();

public: // New functions
    
    /**
    * Service search and getter function.
    * @since Series 60 2.6
    * @param aDevice Target device of search.
    * @param aServiceType Type of searched service.
    * @return Found CUpnpService or NULL if not found.
    */
    IMPORT_C CUpnpService* Service(
        CUpnpDevice* aDevice,
        const TDesC8& aServiceType);

    /**
    * Device getter.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @return Founded device.
    IMPORT_C const CUpnpDevice* Device(const TDesC8& aUuid);       
    */


    /**
    * Creates and sends a SetTransportURI action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aCurrentUri Content URI.
    * @param aCurrentMetaData Content metadata.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtSetTransportUriActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aCurrentUri,
        const TDesC8& aCurrentMetaData);

    /**
    * Creates and sends a SetNextTransportURI action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aNextUri Content URI.
    * @param aNextMetaData Content metadata.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtSetNextTransportUriActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aNextUri,
        const TDesC8& aNextMetaData);

    /**
    * Creates and sends a Seek action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aUnit
    * @param aTarget
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtSeekActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aUnit,
        const TDesC8& aTarget);

    /**
    * Creates and sends a GetMediaInfo action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtMediaInfoActionL(const TDesC8& aUuid,TInt aInstanceId);

    /**
    * Creates and sends a GetTransportInfo action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtTransportInfoActionL(const TDesC8& aUuid,TInt aInstanceId);

    /**
    * Creates and sends a GetPositionInfo action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtPositionInfoActionL(const TDesC8& aUuid,TInt aInstanceId);

    /**
    * Creates and sends a GetDeviceCapabilities action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtDeviceCapabilitiesActionL(const TDesC8& aUuid,TInt aInstanceId);

    /**
    * Creates and sends a GetTransportSetings action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtTransportSettingsActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a Stop action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtStopActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a Play action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aSpeed Speed of playback.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtPlayActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aSpeed);

    /**
    * Creates and sends a Pause action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtPauseActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a Record action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtRecordActionL(const TDesC8& aUuid,TInt aInstanceId);

    /**
    * Creates and sends a Next action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtNextActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a Previous action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtPreviousActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a SetPlayMode action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aPlayMode
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtSetPlayModeActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aPlayMode);

    /**
    * Creates and sends a SetRecordQualityMode action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aRecordMode
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtSetRecordModeActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aRecordMode);

    /**
    * Creates and sends a GetTransportIDs action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt AvtTransportsActionL(const TDesC8& aUuid, TInt aInstanceId);

    /**
    * Creates and sends a Browse action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aObjectId Target's id.
    * @param aBrowseFlag MetaData or DirectChildren.
    * @param aFilter What is returned.
    * @param aStartingIndex First returned item.
    * @param aRequestedCount How manyy items is returned.
    * @param aSortCriteria Sorting order.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsBrowseActionL(
        const TDesC8& aUuid,
        const TDesC8& aObjectId,
        const TDesC8& aBrowseFlag,
        const TDesC8& aFilter, 
        TInt aStartingIndex,
        TInt aRequestedCount,
        const TDesC8& aSortCriteria);

    /**
    * Creates and sends a Search action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aObjectId Target's id.
    * @param aSearchCriteria Search rules.
    * @param aFilter What is returned.
    * @param aStartingIndex First returned item.
    * @param aRequestedCount How manyy items is returned.
    * @param aSortCriteria Sorting order.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsSearchActionL(
        const TDesC8& aUuid,
        const TDesC8& aObjectId,
        const TDesC8& aSearchCriteria,
        const TDesC8& aFilter, 
        TInt aStartingIndex,
        TInt aRequestedCount,
        const TDesC8& aSortCriteria
        );

    /**
    * Creates and sends a GetSystemUpdateID action.
    * @since Series 60 2.6
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsSystemUpdateIdActionL(const TDesC8& aUuid);

    /**
    * Creates and sends a GetSearchCapabilities action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsSearchCapabilitiesActionL(const TDesC8& aUuid);

    /**
    * Creates and sends a GetSortCapabilities action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsSortCapabilitiesActionL(const TDesC8& aUuid);

    /**
    * Creates and sends a CreateObject action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aId Parent container.
    * @param aElements Object in XML form.
    * @return Error code. KErrNone if no errors.
    */                                                                                              
    IMPORT_C TInt CdsCreateObjectActionL(
        const TDesC8& aUuid,
        const TDesC8& aId, 
        const TDesC8& aElements);

    /**
    * Creates and sends a ImportResource action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aSourceUri Source of import.
    * @param aDestinationUri Destination of import.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsImportResourceActionL(
        const TDesC8& aUuid,
        const TDesC8& aSourceUri, 
        const TDesC8& aDestinationUri
        );

    /**
    * Creates and sends a ExportResource action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aSourceUri Source of export.
    * @param aDestinationUri Destination of export.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsExportResourceActionL(
        const TDesC8& aUuid,
        const TDesC8& aSourceUri, 
        const TDesC8& aDestinationUri );

    /**
    * Creates and sends a CreteReference action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aDestinationContainerId Place for reference.
    * @param aSourceObjectId Referred object.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsCreateReferenceActionL(
        const TDesC8& aUuid,
        const TDesC8& aDestinationContainerId, 
        const TDesC8& aSourceObjectId );

    /**
    * Creates and sends a DeleteResource action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aResourceUri Resource to be deleted.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsDeleteResourceActionL(
        const TDesC8& aUuid,
        const TDesC8& aResourceUri );

    /**
    * Creates and sends a DestroyObject action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aObjectId Object to be destroyed.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsDestroyObjectActionL(
        const TDesC8& aUuid,
        const TDesC8& aObjectId );

    /**
    * Creates and sends a StopTransfer action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aTransferId Transfer to be stopped.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsStopTransferActionL(const TDesC8& aUuid, TInt aTransferId );

    /**
    * Creates and sends a GetTransferProgress action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aTransferId Observed transfer.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsTransferProgressActionL(const TDesC8& aUuid, TInt aTransferId );

    /**
    * Creates and sends a SetTransportURI action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aObjectID Updated object.
    * @param aCurrentTagValue Current object XML.
    * @param aNewTagValue New object XML.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CdsUpdateObjectActionL(
        const TDesC8& aUuid,
        const TDesC8& aObjectID,
        const TDesC8& aCurrentTagValue,
        const TDesC8& aNewTagValue );

    /**
    * Creates and sends a GetProtocolInfo action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CmProtocolInfoActionL(const TDesC8& aUuid);

    /**
    * Creates and sends a GetCurrentConnections action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CmCurrentConnectionsActionL(const TDesC8& aUuid);

    /**
    * Creates and sends a PrepareForConnection action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aRemoteProtocolInfo
    * @param aPeerConnectionManager
    * @param aPeerConnectionID
    * @param aDirection
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CmPrepareConnectionActionL(
        const TDesC8& aUuid,
        const TDesC8& aRemoteProtocolInfo,
        const TDesC8& aPeerConnectionManager,
        TInt aPeerConnectionID,
        const TDesC8& aDirection
        );

    /**
    * Creates and sends a ConnectionComplete action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aConnectionId
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CmConnectionCompleteActionL(
        const TDesC8& aUuid,
        TInt aConnectionId
        );

    /**
    * Creates and sends a GetCurrentConnectionInfo action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aConnectionId
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt CmCurrentConnectionInfoActionL(
        const TDesC8& aUuid,
        TInt aConnectionId
        );

    /**
    * Creates and sends a GetVolume action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceID Rendring Instance.
    * @param aChannel Audio channel.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt RcGetVolumetActionL(
        const TDesC8& aUuid,
        TInt aInstanceID,
        const TDesC8& aChannel
        );

    /**
    * Creates and sends a SetVolume action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aChannel Audio channel.
    * @param aVolume
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt RcSetVolumetActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aChannel,
        TInt aVolume
        );

    /**
    * Creates and sends a GetMute action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aChannel Audio channel.
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt RcGetMuteActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aChannel
        );

    /**
    * Creates and sends a SetTransportURI action.
    * @since Series 60 2.6
    * @param aUuid A Universal Unique Identifier of device.
    * @param aInstanceId Rendring Instance.
    * @param aChannel Audio channel.
    * @param aMute
    * @return Error code. KErrNone if no errors.
    */
    IMPORT_C TInt RcSetMuteActionL(
        const TDesC8& aUuid,
        TInt aInstanceId,
        const TDesC8& aChannel,
        const TDesC8& aMute
        );

    /**
     * This function will be invoke if some network event will occure
     * for example IP Address of UPnP changes
     */
    IMPORT_C void NetworkEvent( CUpnpNetworkEventBase* aEvent );

protected:  // Functions from base classes
    
    /**
    * From CUpnpControlPoint State update handler.
    */
    IMPORT_C void StateUpdatedL(CUpnpService* aService);

    /**
    * From CUpnpControlPoint Device discovery handler.
    */
    IMPORT_C void DeviceDiscoveredL(CUpnpDevice* aDevice);

    /**
    * From CUpnpControlPoint Device dissappear handler.
    */
    IMPORT_C void DeviceDisappearedL(CUpnpDevice* aDevice);

    /**
    * From CUpnpControlPoint Action response handler function.
    */
    IMPORT_C void ActionResponseReceivedL(CUpnpAction* aAction);

    /**
    * From CUpnpControlPoint HTTP message handler function.
    */
    IMPORT_C void HttpResponseReceivedL(CUpnpHttpMessage* aMessage);

    /**
    * C++ default constructor.
    */
    IMPORT_C CUpnpAVControlPoint(MUpnpAVControlPointObserver& aAVControlPointObserver);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    IMPORT_C void ConstructL();
    
    /**
    * Initialize CdsActionFactory if is equal NULL 
    */
    IMPORT_C void InitializeCdsActionFactoryL();
	
protected:    // Data
    //AV control point observer (engine)
    MUpnpAVControlPointObserver& iAVControlPointObserver;

    //Action response handler
    CUpnpActionResponseHandler*         iActionResponseHandler;

    // UPnP event handler
    CUpnpStateUpdateHandler*            iStateUpdateHandler;

    // Action factories
    CUpnpAvtActionFactory*              iAvtActionFactory;
    CUpnpCdsActionFactory*              iCdsActionFactory;
    CUpnpCmActionFactory*               iCmActionFactory;
    CUpnpRcActionFactory*               iRcActionFactory;
    };

#endif      // C_CUPNPAVCONTROLPOINT_H
            
// End of File