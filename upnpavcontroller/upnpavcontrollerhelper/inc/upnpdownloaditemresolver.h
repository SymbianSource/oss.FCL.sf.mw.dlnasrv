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
* Description:      Resolver for downloading remote items
*
*/






#ifndef UPNPDOWNLOADITEMRESOLVER_H
#define UPNPDOWNLOADITEMRESOLVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "upnpfiletransfersessionobserver.h" // base class
#include "upnpitemresolver.h" // base class
#include "upnpitemresolverobserver.h" // base class

// FORWARD DECLARATIONS
class CUPnPSettingsEngine;
class CUPnPRemoteItemResolver;
class MUPnPAVController;
class CUpnpAVDevice;
class MUPnPFileDownloadSession;

// CLASS DECLARATION

/**
* Resolves remote upnp items from a plain item id.
* This resolving is done by executing a upnp metadata browse, or
* possibly several ones of those.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPDownloadItemResolver
    : public CBase
    , public MUPnPItemResolver
    , public MUPnPItemResolverObserver
    , public MUPnPFileTransferSessionObserver
    {
public: // construction/destruction

    /**
     * static constructor
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aAvController, AVController
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     * @return DownloadItemResolver instance 
     */
    static CUPnPDownloadItemResolver* NewL(
        const TDesC8& aItemId,
        MUPnPAVController& aAvController,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );

    /**
     * destructor
     */
    virtual ~CUPnPDownloadItemResolver();

private:

    /**
     * default constructor
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aAvController, AVController
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     */
    CUPnPDownloadItemResolver(
        const TDesC8& aItemId,
        MUPnPAVController& aAvController,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.2
     * @param aItemId, item to be resolved
     * @param aAvController, AVController
     * @param aHostSession, browsing session
     * @param aSelector, resource selector
     * @param aBrowseFilter, filter
     */
    void ConstructL(
        const TDesC8& aItemId,
        MUPnPAVController& aAvController,
        MUPnPAVBrowsingSession& aHostSession,
        MUPnPResourceSelector& aSelector,
        const TDesC8& aBrowseFilter );

public: // the interface

    /**
     * see UPnPItemResolver
     */
    void ResolveL(
        MUPnPItemResolverObserver& aObserver
        );

    /**
     * see UPnPItemResolver
     */
    const CUpnpItem& Item() const;


    /**
     * see UPnPItemResolver
     */
    const CUpnpElement& Resource() const;
    
    /**
     * Delete upnp genereated temporary files from download locations
     * Leave in case of fatal errors
     *
     * @since S60 3.2
     */
    IMPORT_C static void DeleteTempDownloadFilesL();


protected: // MUPnPItemResolverObserver

    /**
     * see MUPnPItemResolverObserver
     */
    void ResolveComplete(
        const MUPnPItemResolver& aResolver, TInt aError );

protected: // MUPnPFileTransferSessionObserver


    /**
     * see MUPnPFileTransferSessionObserver
     */
    void TransferStarted( TInt aKey, TInt aStatus );

    /**
     * see MUPnPFileTransferSessionObserver
     */
    void TransferCompleted( TInt aKey, TInt aStatus,
        const TDesC& aFilePath );

    /**
     * see MUPnPFileTransferSessionObserver
     */
    void TransferProgress( TInt aKey,
        TInt aBytes, TInt aTotalBytes );

    /**
     * see MUPnPFileTransferSessionObserver
     */
    void MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason aReason );

private: // private methods

    /**
     * Starts download when the remote item itself has been
     * resolved using the remote resolver
     */
    void InitiateDownloadL();

    /**
     * Completes ongoing async operation
     */
    void Complete( TInt aError );

    /**
     * Clean up all resources
     */
    void Cleanup();
    
    /**
     * Create temporary file name for file to be download.
     * Format is upnpfwtemp<item_id><file extension>
     * The following characters are replaced with an underscore: 
     * >, <, ", \, /, *, |, * : and ?
     *
     * @return file name. Ownership transferred.
     */
    HBufC* CreateTmpFileNameL();
    
    /**
     * Get selected download location from settings engine.
     *
     * @return download location. Ownership transferred.
     */
    HBufC* GetSelectedDownloadLocationL();
    
    /**
     * Creates RFile
     * @param aFilePath, file name and path
     * @return error code
     */
    TInt CreateRFile( const TDesC& aFilePath );

    /**
     * internal state machinery
     */
    enum TInternalState {
        EStateIdle,         // no data, ready for starting
        EStateResolving,    // using remote resolver to resolve the item
        EStateDownloading,  // Downloading data to local file system
        EStateReady         // Ready, can read data
    };

    /**
    * Check if the audio is DLNA supported in remote to local playback 
    * @since Series 60 3.1
    * @param CUpnpObject item to be checked
    * @return void
    */
    void IsLocallySupportedL( const CUpnpElement& aRes ); 

private: // members

    // the remote resolver is a part of this resolver
    CUPnPRemoteItemResolver* iRemoteResolver;
    
    // Settings engine for get selected copy location (Owned).
    CUPnPSettingsEngine*    iSettingsEngine;

    // avcontroller
    MUPnPAVController& iAvController;

    // the remote device
    CUpnpAVDevice* iRemoteDevice;

    // the local filename
    HBufC* iLocalFile;

    // session for downloading items
    MUPnPFileDownloadSession* iDownloadSession;

    // observer for this utility. Note: NOT OWNED
    MUPnPItemResolverObserver* iObserver;

    // state of the class
    TInternalState iState;
    
    // File server session
    RFs iFsSession;
    
    // Download target
    RFile iFile;
    
    // selector interface to use
    MUPnPResourceSelector& iSelector;
    };


#endif  // UPNPDOWNLOADITEMRESOLVER_H

// End of File
