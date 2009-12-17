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
* Description:  Media Server Client
*
*/

#ifndef C_CUPNPMEDIASERVERCLIENT_H
#define C_CUPNPMEDIASERVERCLIENT_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>
#include "upnpicon.h"
#include "upnpsessionbase.h"

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION

/**
@brief Client-side interface to Media Server

This class provides the client-side interface to the media server session
*/
class RUpnpMediaServerClient : public RUpnpSessionBase
    {

public:
    //Media Server operational status
    enum TServerStatus
        {
        EStartedOnline,
        EStartedOffline,
        EStopped
        };

    enum TServerStopMode
        {
        EStopNormal,
        EStopSilent
        };

public: // exported methods

    IMPORT_C void Close();
    /**
    * Constructs the object
    */
    IMPORT_C RUpnpMediaServerClient();

    /**
    * RMediaServerClient checks if Media Server Engine exists. If not creates new process.
    * During Media Server Engine process initialization the CUpnpMediaServer class is instantiated.
    * When Media Server Engine is created RMediaServerClient creates session to it.
    * Each time session is created the session counter is incremented.
    * Note: During a first start the private directory structure is created and
    *       device/service description files are copied there from Z: drive.
    * @return error code
    */
    IMPORT_C TInt Connect();

    /**
    * Stops the media server.
    * This operation is complementary to Start operation.
    * It destroys all services and device itself.
    * The Media Server process is not terminated, it goes to a idle state.
    *
    * @param aMode one of the following modes:
    *    - EStopNormal - ssdp:bye-bye messages are sent before stop (the same as stop)
    *    - EStopSilent - server is stopped silently without ssdp:bye-bye messages
    *
    * Note: Process is terminated when last session is disconnected and
    *       media Server Engine is stopped.
    *       To disconnect session use: RMediaServerClient::Close() which is inherited
    *       from RSessionBase();
    * @return error code
    */
    IMPORT_C TInt Stop( TServerStopMode aMode );

    /**
    * Stops media server. It equals to the Stop( EStopNormal ).
    *
    * @return error code
    */
    IMPORT_C TInt Stop();

    /**
    * Starts media server - UPnP device.
    * Sets device description values accoriding to Content Directory settings.
    * After that instantiates services needed for Media Server operation:
    * - Content Directory Service
    * - Connection Manager Service
    * Finally sets internal status variable to true (that means server is up and running).
    *
    * Panic occurs if session is not connected.
    * Returns one of the system-wide error codes in case of wrong Central Repository settings:
    * - KUPnPStackIapId
    * - KUpnpMediaServerMediaTypes
    *
    * KErrNotFound – if following Central Repository settings has empty values.
    * - KUpnpMediaServerFriendlyName
    * - KUpnpMediaServerManufacturer
    * - KUpnpMediaServerModelName
    * @return error code
    */
    IMPORT_C TInt Start();

    /**
    * Starts media server in offline mode, Media Server is not visible in UPnP network
    * It instantiate only Content Directory without Upnp framework.
    * This mode is used for local sharing purpose.
    * @return error code
    */
    IMPORT_C TInt StartOffline();

    /**
    * Starts media server (asynchonous version)
    * See Start() description for details
    * @return error code
    */
    IMPORT_C void Start(TRequestStatus& aStatus);

    /**
    * This operation performs restart on CUpnpDevice only.
    * This causes sending "bye-bye" and "alive" notification to the network.
    * In order to activate changes in Central Repository use Stop()/Start()
    * sequence.
    * @return error code
    */
    IMPORT_C TInt Restart();

    /**
    * Gets media server operational status
    * @return error code
    */
    IMPORT_C TInt Status( TInt& aStat );        
    
    /**
    * Gets media server inet address
    * @return error code
    */
    IMPORT_C TInt GetAddress( TInetAddr& aAddr );

    /**
    * Gets media server inet address asynchronously
    * @return error code
    */
    IMPORT_C void GetAddressL( TInetAddr& aAddr, TRequestStatus& aStatus );

    /**
    * Method performs 2 steps:
    * 1. Changes Central Repository KUpnpMediaServerUploadDirectory key value,
    * 2. Forces Media Server to switch to new upload directory.
    *
    * Note: In case of Media Server is not started request status is set to KErrNotReady,
    *       but Central Repository key is changed.
    *       During starting Media Server is initiated with new value.
    *       First param aPath should end with double backslash ('\\').
    *       for example default value equals c:\\Download\\Media\\
    * @param aPath upload directory path
    * @param aStatus request status
    */
    IMPORT_C void SetUploadDirectoryL( const TDesC& aPath, TRequestStatus& aStatus );

    /**
    * Adds new icon to Media Server.
    * Media Server performs the following operations:
    * - copies file from path specified aIcon object
    *   to its private directory,
    * - adds icon element to its device description XML file
    *
    * @param aIcon icon
    * @param aStatus request status
    */
    IMPORT_C void AddIcon( CUpnpIcon* aIcon, TRequestStatus& aStatus);

    /**
    * Removes icon of given url.
    * Media Server performs the following operations:
    * - deletes its private copy of icon file,
    * - deletes respective icon element from description XML flie
    * Usage example:
    *
    * iIcon = CUpnpIcon::NewL(aPath, aWidth, aHeight, aDepth, aMimeType);
    * iDevice.SetIcon( iIcon );
    *
    * @param aUrl icon url from XML description
    * @param aStatus
    */
    IMPORT_C void RemoveIcon( TDes8& aUrl, TRequestStatus& aStatus);

    /**
    * Removes all icons
    * Media Server performs the following operations:
    * - deletes all private copies of icon files,
    * - deletes all icon elements from description XML flie
    *
    * @param aStatus
    */
    IMPORT_C void RemoveIcons( TRequestStatus& aStatus);

    /**
    * Updates icon of given url.
    * Media Server performs the following operations:
    * - replaces its private copy of icon file,
    * - replaces respective icon element from description XML flie
    *   url value remains unchanged
    *
    * @param aUrl icon url from XML description
    * @param aIcon icon
    * @param aStatus
    */
    IMPORT_C void UpdateIcon( TDes8& aUrl, CUpnpIcon* aIcon, TRequestStatus& aStatus);

public: // File sharing related methods

    /**
    * Shares item
    * @param aSend
    * @param aRcvdBufSize
    * @param aStatus request status
    * @return the version
    */
    void ShareItem( TDes8& aSend, TDes8& aRcvdBufSize, TRequestStatus& aStatus);

    /**
    * Gets share item result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void ShareItemResult( TDes8& aResult, TRequestStatus& aStatus);


    /**
    * Shares container
    * @param aStatus request status
    * @return the version
    */
    void ShareContainer( TDes8& aSend, TDes8& aRcvd, TRequestStatus& aStatus);

    /**
    * Gets share container result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void ShareContainerResult( TDes8& aResult, TRequestStatus& aStatus);

    /**
    * Shares item list
    * @param aSend
    * @param aStatus request status
    * @return the version
    */
    void ShareItemList( TDes8& aSend, TDes8& aRcvd, TRequestStatus& aStatus);

    /**
    * Gets share item result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void ShareItemListResult( TDes8& aResult, TDes8& aExecStatus, TRequestStatus& aStatus);

    /**
    * Unshares root
    * @param aStatus request status
    * @return the version
    */
    void UnshareRoot(TRequestStatus& aStatus);

    /**
    * Unshares item
    * @param aStatus request status
    * @return the version
    */
    void UnshareItem(TInt aItemId, TRequestStatus& aStatus);

    /**
    * Unshares container
    * @param aStatus request status
    * @return the version
    */

    void UnshareContainer(TInt aItemId, TRequestStatus& aStatus);
    /**
    * Gets shared item
    * @param aStatus request status
    * @return the version
    */
    void GetSharedItem(TInt aItemId, TDes8& aRcvdBufSize, TRequestStatus& aStatus);

    /**
    * Gets shared item result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void GetSharedItemResult( TDes8& aResult, TRequestStatus& aStatus);

    /**
    * Gets shared container
    * @param aStatus request status
    * @return the version
    */
    void GetSharedContainer(TInt aContainerId, TDes8& aRcvdBufSize, TRequestStatus& aStatus);

    /**
    * Gets shared container result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void GetSharedContainerResult( TDes8& aResult, TRequestStatus& aStatus);

    /**
    * Unshares item list
    * @param aStatus request status
    * @return the version
    */
    void UnshareItemListL(TDes8& aSend, TDes8& aExecStatus, TRequestStatus& aStatus);

    /**
    * Gets shared item list
    * @param aStatus request status
    * @return the version
    */
    void GetSharedItemList(TInt aContainerId, TDes8& aSend, TDes8& aRcvdBufSize,
        TRequestStatus& aStatus);

    /**
    * Gets shared item list result
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void GetSharedItemListResult(TDes8& aResult, TDes8& aTotalMatches,
        TRequestStatus& aStatus);

    /**
    * Gets shared container list
    * @param aStatus request status
    * @return the version
    */
    void GetSharedContainerList(TInt aContainerId, TDes8& aSend, TDes8& aRcvdBufSize,
        TRequestStatus& aStatus);

    /**
    * Gets shared container list results
    * @param aResult buffer for result
    * @param aStatus request status
    * @return the version
    */
    void GetSharedContainerListResult(TDes8& aResult, TDes8& aTotalMatches,
        TRequestStatus& aStatus);

    /**
    * Creates a single reference upon given object
    * @param aSend
    * @param aRcvdBufSize
    * @param aStatus request status
    * @return the version
    */
    void ShareReference( TDes8& aSend, TDes8& aRcvdBufSize, TRequestStatus& aStatus );

    /**
    * Creates some reference items, located at specific container, pointing specific objects
    * @param aSend
    * @param aRcvdBufSize
    * @param aStatus request status
    */
    void ShareReferenceList( TDes8& aSend, TDes8& aRcvdBufSize, TRequestStatus& aStatus );
    
public: // Event handling related methods

    /**
    * Subscribe Media Server events
    * @param aStatus request status
    */
    void SubscribeEvents(TRequestStatus& aStatus);

    /**
    * Unsubscribe Media Server events
    */
    void UnsubscribeEvents();

    /**
    * Requests single events list
    * @param aEventSize size of buffer for events list to be allocated by client
    * @param aStatus request status
    */
    void ListenTransferEvent(TDes8& aEventSize, TRequestStatus& aStatus);

    /**
    * Requests single events list body
    * @param aResult buffer for events list
    * @param aStatus request status
    */
    void GetTransferEventBody(TDes8& aResult, TRequestStatus& aStatus);
    
public: // Notify status change related methods     
   
    /**
    * Notify about media server operational status
    * @param aStatus request status
    */
    void StatusChangeNotify( TRequestStatus& aStatus );
    
    /**
    * Cancel notifycation about media server operational status
    */
    void CancelStatusChangeNotify();

private:

    // holds inet address
    TPckg<TInetAddr>* iAddressPkg;
    };

#endif // C_CUPNPMEDIASERVERCLIENT_H

// End of File
