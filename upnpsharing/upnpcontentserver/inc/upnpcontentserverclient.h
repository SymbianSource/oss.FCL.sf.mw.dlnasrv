/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Header for content server client class
*
*/





#ifndef __UPNPCONTENTSERVERCLIENT_H__
#define __UPNPCONTENTSERVERCLIENT_H__

// INCLUDES
#include <e32base.h>
#include <badesca.h>  //CDesC8Array
#include "upnpcontentserverdefs.h"

// CONSTANTS
// The maximum numbers of sharingrequests that can be currently received
const TInt KContentServerMaxSharingReqs( 2 );
const TUid KUpnpContentServerCat={0x20007564};
enum TUpnpContentServerKeys {ESharingProgress};
const TInt KMaxProgressLength( 50 );

/**
 * Helper class to transfer progress information in one struct
 */
class TUpnpProgressInfo
    {
public:

    /**
     * Kind of operaiotn this information is related to
     * images&videos or playlists
     */
    UpnpContentServer::TUpnpMediaType iProgressKind;

    /**
     * Type of progress
     */
    enum TProgressType
        {
        ESharingProgress = 0, //sharing is ongoing for this kind of media
        EVisualStatus         //sharing is not ongoing
        };
    TProgressType iProgressType;

    /**
     * If iProgressType = ESharingProgress: the value 
     * of progress in percents 0-100.
     * If iProgressType = EVisualStatus: status of the selection 
     * ( UpnpContentServer::TUpnpSharingSelection )
     */
    TInt iProgress;
    };

typedef RArray<TUpnpProgressInfo> TProgressInfos;

/**
 * Helper class to contain all progress information
 */
class TUpnpProgress
    {
public:

    TUpnpProgressInfo iImageVideoProgress;

    TUpnpProgressInfo iMusicProgress;

    /**
     * Possible errorcode in sharing
     * 
     */
    TInt iError;
    };

/**
 *  RUpnpContentServerClient class is used to create session and a connection
 *  to UpnpContentServer
 *
 *  @lib upnpcontentserverclient.lib
 *  @since S60 3.1
 */
class RUpnpContentServerClient : public RSessionBase
    {

public:

    /**
     * C++ default constructor.
     */
    IMPORT_C RUpnpContentServerClient();

    /**
     * Creates new session to Upnp Content Server
     * @since S60 3.1
     */
    IMPORT_C void OpenL();

    /**
     * Closes the connection with Upnp Content Server
     * @since S60 3.1
     */
    IMPORT_C void Close();

    /**
     * Starts the media server upload listener
     * Not supported
     * @since S60 3.1
     */
    IMPORT_C void StartUploadListenerL();

    /**
     * Stops the media server upload listener
     * Not supported
     * @since S60 3.1
     */
    IMPORT_C void StopUploadListenerL();

    /**
     * Gets the names of existed albums/playlists
     * @since S60 3.1
     * @param aType Type of media
     * @param aStatus TRequestStatus for async operation
     */
    IMPORT_C void GetSelectionContentL(
        UpnpContentServer::TUpnpMediaType aType,
        TRequestStatus& aStatus );

    /**
     * Ask the result of GetSelectionContentL call
     * Should be used after the GetSelectionContentL has completed.
     * @since S60 3.1
     * @param aIDArray Array to add selection items
     */
    IMPORT_C void GetSelectionContentResultL( CDesCArray& aIDArray );

    /**
     * Sends the selected indexes to server and starts sharing
     * @since S60 3.2
     * @param aType Type of media
     * @param aMarkedItems List of selected indexes
     * @param aStatus TRequestStatus for async operation.
     */
    IMPORT_C void ChangeSharedContentL(
        UpnpContentServer::TUpnpMediaType aType,
        const CArrayFix<TInt>& aMarkedItems,
        TRequestStatus& aStatus );

    /**
     * Refresh the shared content
     * @since S60 3.1
     * @param aType Type of media
     * @param aStatus TRequestStatus for async operation.
     */
    IMPORT_C void RefreshSharedContentL(
        UpnpContentServer::TUpnpMediaType aType,
        TRequestStatus& aStatus );

    /**
     * Gets the users previous selections as list of selected indexes
     * @since S60 3.1
     * @param aType Type of media
     * @param aStatus TRequestStatus for async operation.
     */
    IMPORT_C void GetSelectionIndexesL(
        UpnpContentServer::TUpnpMediaType aType,
        TRequestStatus& aStatus );

    /**
     * Ask the result of GetSelectionIndexesL call
     * @since S60 3.1
     * @param aMarkedItems Array to hold the items
     */
    IMPORT_C void GetSelectionIndexesResultL( 
        CArrayFix<TInt>& aMarkedItems );

    /**
     * Start connection monitor in content server
     * Handles stopping of media server in case of WLAN is lost
     * @since S60 3.1
     * @param aIapId Access point id
     * @param aStatus TRequestStatus for async operation
     */
    IMPORT_C void StartConnectionMonitorL( 
        TInt aIapId,
        TRequestStatus& aStatus );

    /**
     * Stop connection monitor in content server
     * @since S60 3.1
     * @param aStatus TRequestStatus for async operation
     */
    IMPORT_C void StopConnectionMonitorL( TRequestStatus& aStatus );

private:
    /**
     * Free previous receive buffers and allocate new
     * @since S60 3.1
     * @param aSize The size of the buffer
     */
    void PrepareReceiveBuffersL( TInt aSize );

private:
    /**
     * General buffer for sending data
     */
    TPtr8 iSendBufferPtr;

    /**
     * General buffer for sending data
     * Owned
     */
    CBufFlat* iSendBuffer;

    /**
     * General buffer for receiving data 
     */
    TPtr8 iReceiveBufferPtr;

    /**
     * General buffer for receiving data 
     * Owned
     */
    HBufC8* iReceiveBuffer;

    /**
     * Buffer for receiving content for sharing UI
     * separate buffer because of multiple async requests
     */
    TPtr8 iContentBufferPtr;

    /**
     * Buffer for receiving content for sharing UI
     * separate buffer because of multiple async requests
     * Owned
     */
    HBufC8* iContentBuffer;

    };

#endif // __UPNPCONTENTSERVERCLIENT_H__

