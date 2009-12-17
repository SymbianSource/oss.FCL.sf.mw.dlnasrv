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
* Description:      AVController client session
*
*/






#ifndef C_CUPNPAVCONTROLLERCLIENT_H
#define C_CUPNPAVCONTROLLERCLIENT_H

// INCLUDES
#include "upnpavcontrollerglobals.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpContainer;
class CUpnpDevice;
class CUpnpObject;
class RFile;

// CLASS DECLARATION
/**
 *  Client-side interface to AV Controller Server
 *
 *  This class provides the client-side interface to the AV Controller
 *  Server session.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class RUPnPAVControllerClient : public RSessionBase
    {
public:

    /**
     * Construct the object
     */
    RUPnPAVControllerClient();

    /**
     * Connects to the server and create a session
     * @return error code
     */
    TInt Connect();
    
    /**
     * Returns the version number
     * @return the version
     */
    TVersion Version() const;

    /**
     * Startup request. Starts up the AV Control Point
     *
     * @param aStatus request status
     */    
    void StartUp( TRequestStatus& aStatus );

    /**
     * Cancels startup.
     *
     * @return error code
     */    
    TInt CancelStartUp();

    /**
     * Device discovery request
     *
     * @param aDiscovered type of device
     * @param aRcvdBufSize size of device
     * @param aStatus request status
     */    
    void DeviceRequest( TDes8& aDiscovered, TDes8& aRcvdBufSize,
        TRequestStatus& aStatus );
        
    /**
     * Cancels device discovery request
     *
     * @return error code
     */    
    TInt CancelDeviceRequest();
    
    /**
     * Gets device
     *
     * @param aDevice device
     * @return error code
     */    
    TInt GetDevice( TDes8& aDevice );
    
    /**
     * Gets device list size
     *
     * @param aType type of devices
     * @param aSize size of the list
     * @return error code
     */    
    TInt GetDeviceListSize( TDes8& aType, TDes8& aSize );
    
    /**
     * Gets device list
     * 
     * @param aList device list
     * @return error code
     */    
    TInt GetDeviceList( TDes8& aList );
    
    /**
     * Release media server
     *
     * @param aId session id
     * @return error code
     */    
    TInt ReleaseMediaServer( TInt aId );
    
    /**
     * Reserve media server
     * 
     * @param aId session id
     * @param aStatus request status
     */    
    void ReserveMediaServer( TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel reserve media server
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelReserveMediaServer( TInt aId );

public: // Rendering Session releated
    
    /**
     * Create a rendering session
     * 
     * @param aId session id
     * @param aUuid device Uuid
     * @return error code
     */    
    TInt CreateRenderingSession( TInt aId, TDes8& aUuid );
    
    /**
     * Destroy a rendering session
     * 
     * @param aId session id
     * @return error code
     */    
    TInt DestroyRenderingSession( TInt aId );
    
    /**
     * AV event request
     *
     * @param aEvent AV Event
     * @param aId session id
     * @param aStatus request status
     */    
    void EventRequest( TDes8& aEvent, TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel event request
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelEventRequest( TInt aId );
    
    /**
     * Set URI
     *
     * @param aId session id
     * @param aRequest request information
     * @param aItem item
     * @param aType type of the item 
     * @param aStatus request status
     */    
    void SetURI( TInt aId, TDes8& aRequest, TDes8& aItem,
                                 TRequestStatus& aStatus );
        
    /**
     * Cancel reserve media server
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelSetURI( TInt aId );
    
    /**
     * Set Next URI
     *
     * @param aId session id
     * @param aRequest request information
     * @param aItem item
     * @param aType type of the item 
     * @param aStatus request status
     */    
    void SetNextURI( TInt aId, TDes8& Request, TDes8& aItem,
                                    TRequestStatus& aStatus );
        
    /**
     * Cancel reserve media server
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelSetNextURI( TInt aId );
    
    /**
     * Send play action
     *
     * @param aId session id
     * @param aStatus request status
     */    
    void Play( TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel play
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelPlay( TInt aId );
    
    /**
     * Send stop action
     *
     * @param aId session id
     * @param aStatus request status
     */    
    void Stop( TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel stop
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelStop( TInt aId );
    
    /**
     * Send pause action
     *
     * @param aId session id
     * @param aStatus request status
     */    
    void Pause( TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel pause
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelPause( TInt aId );
    
    /**
     * Set volume level
     *
     * @param aId session id
     * @param aVolumeLevel volume level
     * @param aRetVolume (set) volume level
     * @param aStatus request status
     */    
    void SetVolume( TInt aId, TInt aVolumeLevel, TDes8& aRetVolume,
        TRequestStatus& aStatus );
        
    /**
     * Cancel set volume
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelSetVolume( TInt aId );
    
    /**
     * Get volume level
     *
     * @param aId session id
     * @param aVolumeLevel volume level
     * @param aStatus request status
     */    
    void GetVolume( TInt aId, TDes8& aVolumeLevel, TRequestStatus& aStatus );
    
    /**
     * Cancel get volume
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelGetVolume( TInt aId );
    
    /**
     * Set mute
     *
     * @param aId session id
     * @param aMute mute
     * @param aRetMute (set) mute
     * @param aStatus request status
     */    
    void SetMute( TInt aId, TBool aMute, TDes8& aRetMute,
        TRequestStatus& aStatus );
    
    /**
     * Cancel set mute
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelSetMute( TInt aId );
    
    /**
     * Get mute
     *
     * @param aId session id
     * @param aMute mute
     * @param aStatus request status
     */    
    void GetMute( TInt aId, TDes8& aMute, TRequestStatus& aStatus );
    
    /**
     * Cancel get mute
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelGetMute( TInt aId );
     
    /**
     * Get position info
     *
     * @param aId session id
     * @param aPosition position info
     * @param aLength lenght info
     * @param aStatus request status
     */    
    void GetPositionInfo( TInt aId, TDes8& aPosition, TDes8& aLength,
        TRequestStatus& aStatus );
        
    /**
     * Cancel get position info
     * 
     * @param aId session id
     * @return error code
     */    
    TInt CancelGetPositionInfo( TInt aId );

public: // Browsing Session releated

    /**
     * Create browsing session
     *
     * @param aId session id
     * @param aUuid device uuid
     * @return error code
     */    
    TInt CreateBrowsingSession( TInt aId, TDes8& aUuid );
    
    /**
     * Destroy browsing session
     *
     * @param aId session id
     * @return error code
     */    
    TInt DestroyBrowsingSession( TInt aId );
    
    /**
     * Get browse response size
     *
     * @param aId session id
     * @param aRequest request information
     * @param aSize response size
     * @param aMatches count of items
     * @param aStatus request status
     */    
    void BrowseResponseSize( TInt aId, TDes8& aRequest, TDes8& aParams,
        TRequestStatus& aStatus );
    
    /**
     * Cancel get browse response size
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelBrowseResponseSize( TInt aId );
    
    /**
     * Get browse response
     *
     * @param aId session id
     * @param aBrowseResponse browse response
     * @return error code
     */    
    TInt BrowseResponse( TInt aId, TDes8& aBrowseResponse,
        TDes8& aUpdateId );

    /**
     * Get search response size
     *
     * @param aId session id
     * @param aRequest request information
     * @param aSize response size
     * @param aMatches count of items
     * @param aStatus request status
     */    
    void SearchResponseSize( TInt aId, TDes8& aRequest, TDes8& aParams,
        TRequestStatus& aStatus );
    
    /**
     * Cancel get search response size
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelSearchResponseSize( TInt aId );
    
    /**
     * Get search response
     *
     * @param aId session id
     * @param aSearchResponse search response
     * @return error code
     */    
    TInt SearchResponse( TInt aId, TDes8& aSearchResponse,
        TDes8& aUpdateId );

    /**
     * Get search capabilities size
     *
     * @param aId session id
     * @param aSize size
     * @param aStatus request status
     */    
    void GetSearchCapabilitiesSize( TInt aId, TDes8& aSize,
        TRequestStatus& aStatus );
    
    /**
     * Cancel get search capabilities size
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelGetSearchCapabilitiesSize( TInt aId );
    
    /**
     * Get search capabilities
     *
     * @param aId session id
     * @param aCapabilitiesResponse search capabilities response
     * @return error code
     */    
    TInt SearchCapabilitiesResponse( TInt aId,
        TDes8& aCapabilitiesResponse );
    
    /**
     * Create a container
     *
     * @param aId session id
     * @param aTitle container title
     * @param aParentId parent id
     * @param aContainerType container type
     * @param aStatus request status
     */    
    void CreateContainer( TInt aId,
                          TDes8& aTitle,
                          TDes8& aParentId,
                          TInt aContainerType,
                          TRequestStatus& aStatus );                          
    
    /**
     * Cancel create container
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelCreateContainer( TInt aId );

    /**
     * Delete object
     *
     * @param aId session id
     * @param aObjectId object id
     * @param aStatus request status
     */    
    void DeleteObject( TInt aId, TDes8& aObjectId, TRequestStatus& aStatus );
    
    /**
     * Cancel delete object
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelDeleteObject( TInt aId );
    
    /**
     * Device disappeared request
     *
     * @param aId session id
     * @param aStatus request status
     */    
    void DeviceDisappearedRequest( TInt aId, TRequestStatus& aStatus );
    
    /**
     * Cancel device disappeared request
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelDeviceDisappearedRequest( TInt aId );    
    
    /**
     * Monitor WLAN connection request
     *
     * @param aStatus request status
     */    
    void MonitorConnection( TRequestStatus& aStatus );
    
    /**
     * Cancel monitor connection
     *
     * @param aId session id
     * @return error code
     */    
    TInt CancelMonitorConnection();

    /**
     * 
     *
     * @param aInUse
     * @return error code
     */    
    TInt MSServicesInUse( TDes8& aInUse );

public: // Download Session releated

    /**
     * Create a download session
     * 
     * @param aId session id
     * @param aUuid device Uuid
     * @return error code
     */    
    TInt CreateDownloadSession( TInt aId, TDes8& aUuid );
    
    /**
     * Destroy a download session
     * 
     * @param aId session id
     * @return error code
     */    
    TInt DestroyDownloadSession( TInt aId );

    TInt StartDownload( TInt aId, TDes8& aFileTransferItem );
    
    TInt StartDownload( TInt aId, TDes8& aFileTransferItem,
        const RFile& aFile );
    
    void GetDownloadEvent( TInt aId, TDes8& aEvent,
        TRequestStatus& aStatus, TDes& aBuffer );
    
    TInt CancelDownloadEvent( TInt aId );
        
    TInt StartTrackingDownloadProgress( TInt aId, TInt aKey );    
    
    TInt CancelDownload( TInt aId, TInt aKey );
    
    TInt CancelAllDownloads( TInt aId );
    
public: // Upload Session releated

    /**
     * Create an upload session
     * 
     * @param aId session id
     * @param aUuid device Uuid
     * @return error code
     */    
    TInt CreateUploadSession( TInt aId, TDes8& aUuid );
    
    /**
     * Destroy an upload session
     * 
     * @param aId session id
     * @return error code
     */    
    TInt DestroyUploadSession( TInt aId );

    TInt StartUpload( TInt aId, TDes8& aFileTransferItem );
    
    void GetUploadEvent( TInt aId, TDes8& aEvent,
        TRequestStatus& aStatus );
    
    TInt CancelUploadEvent( TInt aId );
        
    TInt StartTrackingUploadProgress( TInt aId, TInt aKey );    
    
    TInt CancelUpload( TInt aId, TInt aKey );
    
    TInt CancelAllUploads( TInt aId );
    };

#endif // C_CUPNPAVCONTROLLERCLIENT_H

// End of File
