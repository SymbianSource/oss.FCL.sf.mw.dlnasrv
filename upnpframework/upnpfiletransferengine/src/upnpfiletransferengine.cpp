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
* Description:  Implementation of the CUpnpFileTransferEngine class
*
*/


// INCLUDE FILES
// System
#include <badesca.h>                            // CDesC8ArrayFlat

// upnp stack api
#include <upnpcontainer.h>                      // CUpnpContainer

// upnpframework / avcontroller api
#include "upnpavbrowsingsession.h"              // MUPnPAVSessionBase

// upnpfiletransferengine internal
#include "upnpfiletransferengine.h"             // CUpnpFileTransferEngine
#include "upnpfiletransferhandler.h"            // CUpnpFileTransferHandler
#include "upnpdownloadhandler.h"                // CUpnpDownloadHandler
#include "upnpuploadhandler.h"                  // CUpnpUploadHandler


_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::NewL
// NewL.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpFileTransferEngine* CUpnpFileTransferEngine::NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[UpnpFileTransferEngine] CUpnpFileTransferEngine: NewL" );

    // Check that the browsing session is valid and has target device set.
    if( !aBrowsingSession )
        {
        User::Leave( KErrArgument );
        }

    CUpnpFileTransferEngine* self = NULL;
    self = new (ELeave) CUpnpFileTransferEngine;
    CleanupStack::PushL( self );
    self->ConstructL( aBrowsingSession );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpFileTransferEngine::CUpnpFileTransferEngine()
    {
    __LOG( "[UpnpFileTransferEngine] Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpFileTransferEngine::~CUpnpFileTransferEngine()
    {
    __LOG( "[UpnpFileTransferEngine] Destructor" );

    // Delete the file transfer handler.
    delete iFileTransferHandler;
    delete iUploadHandler;

    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpFileTransferEngine::ConstructL(
                                MUPnPAVBrowsingSession* aBrowsingSession )
    {
    __LOG( "[UpnpFileTransferEngine] ConstructL" );

    // Check the parameter first
    if( !aBrowsingSession)
        {
        User::Leave( KErrArgument );
        }

    iBrowsingSession = aBrowsingSession; // not owned
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyToHandsetL
// Copies items (whose object ids are provided) from a remote Upnp Media
// Server to the handset.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpFileTransferEngine::CopyToHandsetL(
                                        CDesC8ArrayFlat* aObjectIds )
    {
    __LOG( "[UpnpFileTransferEngine] CopyToHandsetL" );

    // Create the download handler
    CUpnpDownloadHandler* downloadHandler =
                            CUpnpDownloadHandler::NewL( iBrowsingSession );
    CleanupStack::PushL( downloadHandler );

    // Do the download
    downloadHandler->DownloadItemsL( aObjectIds );
    
    TInt fileNumber = downloadHandler->GetNumCopiedFiles();
    // Clean up
    CleanupStack::PopAndDestroy( downloadHandler );
    downloadHandler = NULL;
    return fileNumber;
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyLocalFilesToRemoteServerL
// Copies local files to a remote media server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::CopyLocalFilesToRemoteServerL(
                                RPointerArray<TDesC16>* aFilePaths )
    {
    __LOG( "[UpnpFileTransferEngine] CopyLocalItemsToRemoteServerL" );

    // Create the download handler
    if( !iUploadHandler )
        {
        iUploadHandler = CUpnpUploadHandler::NewL( iBrowsingSession );    
        }
    
    iUploadHandler->UploadItemsL( aFilePaths, EUpnpCopy );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::MoveLocalFilesToRemoteServerL
// Moves local files to a remote media server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::MoveLocalFilesToRemoteServerL(
                                RPointerArray<TDesC16>* aFilePaths )
    {
    __LOG( "[UpnpFileTransferEngine] MoveLocalFilesToRemoteServerL" );

    // Create the download handler
    if( !iUploadHandler )
        {
        iUploadHandler = CUpnpUploadHandler::NewL( iBrowsingSession );    
        }
    
    iUploadHandler->UploadItemsL( aFilePaths, EUpnpMove );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyLocalPlaylistToRemoteServerL
// Copies a playlist from the local handset to a remote Upnp Media Server.
// Deprecated!
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::CopyLocalPlaylistToRemoteServerL(
                                const TDesC& aPlaylistName,
                                RPointerArray<TDesC16>* aFilePaths )
    {
    __LOG( "[UpnpFileTransferEngine] \
CopyLocalPlaylistToRemoteServerL" );

    // Create the download handler
    if( !iUploadHandler )
        {
        iUploadHandler = CUpnpUploadHandler::NewL( iBrowsingSession );    
        }
    
    iUploadHandler->UploadPlayListL( aPlaylistName, aFilePaths, EUpnpCopy );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyRemoteItemsToHandsetL
// Copies items from a remote Upnp Media Server to default location.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::CopyRemoteItemsToHandsetL(
                            RPointerArray<CUpnpItem>& aItems )
    {
    __LOG( "[UpnpFileTransferEngine] CopyRemoteItemsToHandsetL" );

    // Create the file transfer handler, if not yet created
    if( !iFileTransferHandler )
        {
        iFileTransferHandler = CUpnpFileTransferHandler::NewL(
                                                    iBrowsingSession );
        }

    // Forward the method call with the correct transfer mode
    iFileTransferHandler->TransferRemoteItemsToHandsetL(
                                EUpnpCopyRemoteItemsToDefaultLocation,
                                aItems );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyRemoteContainerToHandsetL
// Copies a container from a remote Upnp Media Server to the default (setting
// set in the Home Network Application) location.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::CopyRemoteContainerToHandsetL(
                            CUpnpContainer* aContainer )
    {
    __LOG( "[UpnpFileTransferEngine] CopyRemoteContainerToHandsetL" );

    // Create the file transfer handler, if not yet created
    if( !iFileTransferHandler )
        {
        iFileTransferHandler = CUpnpFileTransferHandler::NewL(
                                                    iBrowsingSession );
        }

    // Forward the method call with the correct transfer mode
    iFileTransferHandler->TransferRemoteContainerToHandsetL(
                            EUpnpCopyRemoteContainerToDefaultLocation,
                            aContainer );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::CopyRemotePlaylistToHandsetL
// Copies a playlist from a remote Upnp Media Server to the default (setting
// set in the Home Network Application) location.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileTransferEngine::CopyRemotePlaylistToHandsetL(
                            CUpnpContainer* aContainer )
    {
    __LOG( "[UpnpFileTransferEngine] CopyRemotePlaylistToHandsetL" );

    // Create the file transfer handler, if not yet created
    if( !iFileTransferHandler )
        {
        iFileTransferHandler = CUpnpFileTransferHandler::NewL(
                                                    iBrowsingSession );
        }

    // Forward the method call with the correct transfer mode
    iFileTransferHandler->TransferRemoteContainerToHandsetL(
                            EUpnpCopyRemotePlaylistToDefaultLocation,
                            aContainer );
    }

 
// --------------------------------------------------------------------------
// CUpnpFileTransferEngine::ItemCopiedFromContainer
// returns a number of items copied from a container
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpFileTransferEngine::ItemCopiedFromContainer()
    {
    TInt returnValue = KErrNotReady;
    if( iFileTransferHandler )
        {
        returnValue = iFileTransferHandler->ItemCopiedFromContainer();
        }
    return returnValue;
    }

// End of file
