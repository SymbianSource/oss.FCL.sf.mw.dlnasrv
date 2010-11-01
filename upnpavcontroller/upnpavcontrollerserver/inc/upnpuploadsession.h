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

// INTERNAL INCLUDES
#include "httptransferobserver.h"
#include "upnpfiletransfersessionbase.h"
#include "tupnpfiletransferevent.h"
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CHttpUploader;
class CUPnPResourceHelper;
class CUpnpObject;
class CUpnpAttribute;
class CUpnpDevice;

// CLASS DECLARATION
/**
 * Implements AV Controller server side upload session
 *
 * @since S60 v3.2
 */
class CUPnPUploadSession : public CUPnPFileTransferSessionBase,
                           private MHttpTransferObserver,
                           private MUpnpAVControlPointObserver
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
    * @see MUpnpAVControlPointObserver::ActionResponseL
    */
    void ActionResponseL( CUpnpAction* aAction );
    
    /**
    * @see MUpnpAVControlPointObserver::StateUpdatedL
    */
    void StateUpdatedL( CUpnpService* aService );

    /**
    * @see MUpnpAVControlPointObserver::HttpResponseL
    */
    void HttpResponseL( CUpnpHttpMessage* aMessage );
    
    /**
    * @see MUpnpAVControlPointObserver::DeviceDiscoveredL
    */
    void DeviceDiscoveredL( CUpnpDevice* aDevice );
    
    /**
    * @see MUpnpAVControlPointObserver::DeviceDisappearedL
    */
    void DeviceDisappearedL( CUpnpDevice* aDevice );

private: // network event handling

    /**
     * Handles response for ContentDirectory CreateObject command
     */
    void CdsCreateObjectResponse(
        TInt aErr,
        const TDesC8& aObjectID, 
        const TDesC8& aResult );

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

    /**
     * Sends CreateObject command to remote CDS
     * returns the related HTTP session id
     */
    int CreateObjectL( const TDesC8& aContainerId, const TDesC8& aElements );

    /**
     * Sends DestroyObject command to remote CDS
     * returns the related HTTP session id
     */
    int DestroyObjectL( const TDesC8& aObjectId );

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
     * CUpnpDevice used by AvControlPoint.
     */
    const CUpnpDevice*                      iCpDevice; // Not own.    
        
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

