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
* Description:  Header file for the CUpnpFileTransferEngine class
*
*/


#ifndef UPNP_FILE_TRANSFER_ENGINE_H
#define UPNP_FILE_TRANSFER_ENGINE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpContainer;
class CUpnpItem;
class MUPnPAVBrowsingSession;
class MUPnPAVController;
class CUpnpFileTransferHandler;
class CDesC8ArrayFlat;
class CUpnpUploadHandler;

// CLASS DECLARATION

/**
* CUpnpFileTransferEngine
* The class works as an interface component, providing file transfer features
* for the clients.
*
* @since S60 3.0
*/
class CUpnpFileTransferEngine : public CBase
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * Leaves (KErrArgument) if the provided UpnpAvBrowsingSession
         * pointer is not valid.
         *
         * @since S60 3.0
         * @param aBrowsingSession (MUPnPAVBrowsingSession*) pointer to the
         *        browsing session
         */
         IMPORT_C static CUpnpFileTransferEngine* NewL(
                            MUPnPAVBrowsingSession* aBrowsingSession );

        /**
         * Destructor.
         */
        virtual ~CUpnpFileTransferEngine();

    public: // New methods for transfering content to handset

        /**
         * Copies items (whose object ids are provided) from a remote Upnp
         * Media Server to the handset. The files are stored in the location
         * defined in the settings of the Home Media application.
         *
         * Leaves using e32err.h error codes if operation fails.
         *
         * @since S60 3.2
         * @param aObjectIds (CDesC8ArrayFlat*) list of object IDs to copy
         * @return TInt a number of copied files
         */
        IMPORT_C TInt CopyToHandsetL( CDesC8ArrayFlat* aObjectIds );

    public: // Transfer content from handset to remote UPnP Media Server

       /**
         * Copies local files to a remote media server. DLNA Any Container
         * feature will be used. Removes files from the array which are 
         * succesfully moved.
         *
         * Leaves (KErrArgument) if the provided array of items is empty.
         *
         * @since S60 3.2
         * @param aFileNames (RPointerArray*) pointer to the array of 
         *        file names (including path) of the files to copy
         */
        IMPORT_C void CopyLocalFilesToRemoteServerL(
                                RPointerArray<TDesC16>* aFilePaths );

        /**
         * Moves local files to a remote media server. DLNA Any Container
         * feature will be used. Removes files from the array which are 
         * succesfully moved.
         *
         * Leaves (KErrArgument) if the provided array of files is empty.
         *
         * @since S60 3.0
         * @param aFileNames (RPointerArray*) pointer to the array of 
         *        file names (including path) of the files to move
         */
        IMPORT_C void MoveLocalFilesToRemoteServerL(
                                RPointerArray<TDesC16>* aFilePaths );

        /**
         * Copies a playlist from the local handset to a remote Upnp Media
         * Server.
         *
         * Leaves (KErrArgument) if the provided playlist name is not valid
         * or the provided array of items is empty.
         *
         * @since S60 3.1
         * @param aPlaylistName (const TDesC&) the name of the playlist
         * @param aFileNames (RPointerArray&) reference to the array of 
         *        file names (including path) of the files to copy
         */
        IMPORT_C void CopyLocalPlaylistToRemoteServerL(
                                const TDesC& aPlaylistName,
                                RPointerArray<TDesC16>* aFilePaths );

    public: // Transfer content from remote UPnP Media Server to handset

        /**
         * Copies items from a remote Upnp Media Server to the handset to
         * the location defined in the Home Media application).
         *
         * Leaves (KErrArgument) if the provided array of items is empty.
         *
         * @since S60 3.0
         * @param aItems (RPointerArray&) reference to the array of 
         *        CUpnpItems, the items to copy
         */
        IMPORT_C void CopyRemoteItemsToHandsetL(
                                RPointerArray<CUpnpItem>& aItems );

        /**
         * Copies a container from a remote Upnp Media Server to the location
         * defined in the Home Media application).
         * 
         * Leaves (KErrArgument) if the provided UpnpContainer is not valid.
         *
         * @since S60 3.0
         * @param aSourceContainer (CUpnpContainer*) pointer to the source 
         *        UpnpContainer object
         */
        IMPORT_C void CopyRemoteContainerToHandsetL(
                                CUpnpContainer* aSourceContainer );

        /**
         * Copies a playlist from a remote Upnp Media Server to the location
         * defined in the Home Media application).
         *
         * Leaves (KErrArgument) if the provided UpnpContainer is not valid.
         *
         * @since S60 3.0
         * @param aSourceContainer (CUpnpContainer*) pointer to the source 
         *        UpnpContainer object
         */
        IMPORT_C void CopyRemotePlaylistToHandsetL(
                                CUpnpContainer* aSourceContainer );
    public:

        /**
         * Return the number of items copied from a container
         *
         * @since S60 3.1
         * @param none
         * @return TInt number of items copied
         */
        IMPORT_C TInt ItemCopiedFromContainer();
    
    private: // Construction methods

        // Constructor
        CUpnpFileTransferEngine();

        /**
         * ConstructL.
         * @param aController (MUPnPAVBrowsingSession*) pointer to a valid
         *        UpnpAvController BrowsingsSession
         */
        void ConstructL( MUPnPAVBrowsingSession* aBrowsingSession );

    private: // Data members

        /*
         * Pointer to the file transfer handler. Owned.
         */
        CUpnpFileTransferHandler* iFileTransferHandler;

        /*
         * Pointer to the browsing session which the client provides. Not
         * owned.
         */
        MUPnPAVBrowsingSession* iBrowsingSession;
        CUpnpUploadHandler* iUploadHandler; //owned
    };

#endif // UPNP_FILE_TRANSFER_ENGINE_H

// End of file
