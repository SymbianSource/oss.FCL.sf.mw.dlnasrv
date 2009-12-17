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
* Description:      This module handlers the cleaning and creating of the 
*                filesystem to mediaserver
*
*/





#ifndef __UPNPCONTAINERCHECKERAO_H__
#define __UPNPCONTAINERCHECKERAO_H__

// INCLUDES
#include <e32base.h>

#include <upnpbrowsecriteria.h>
#include <upnpcontainerlist.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpitemlist.h>
#include <upnpfilesharing.h>
#include <upnpmediaserverclient.h>
#include "upnpcontentserverdefs.h"
#include "upnpsharingcallback.h"

/**
 *  A class to check, create  and clean the default filesystem in Media server
 *
 *  @since S60 3.1
 */
class CUpnpContainerCheckerAo : public CActive
    {
    /**
     * Indicates the ongoing operation of the CUpnpContainerCheckerAo
     */
    enum THandlerOperations
        {
        EInitialize,
        EQueryDefaultContainers,
        EUnshareItemList,
        EQueryDefaultContainersResult,
        EQueryItemFromMediaServer,
        EQueryItemFromMediaServerResult,
        
        EUnshareContainer,
        EUnshareContainerResult,

        EShareContainer,
        EShareContainerResult,
        ENotActive
        };

    /**
     * Indicates the current state of the handler
     */
    enum THandlerState
        {
        EResolveDefaultContainerIds,
        ECleanFileStructure,
        EShareDefaultContainers,
        EEmptyUploadedContainer,
        EEmptyUploadedContainerItems,
        EEmptyRootContainerItems,
        EUnshareExtraContainers,
        EUnshareExtraItems,
        EIdle
        };

    /**
     * Indicates the current state of the handler
     */
    enum TErrorState
        {
        ENoError,
        EDropAllContent,
        EDropAllContentResult
        };

    /**
     * The operating modes
     */
    enum TOperationMode
        {
        EValidateContainers,
        ECheckDefaultContainers
        };

public:
   /**
    * The folder checking operations
    */
    enum TContainerPosition
        {
        EImageAndVideo = 0,
        EMusic,
        EUploaded
        };

    /**
    * C++ constructor.
    * @since S60 3.1
    * @param aWait, the client side wait
    */
    CUpnpContainerCheckerAo( MUpnpSharingCallback* aCallback );

    /**
    * C++ destructor.
    */
    virtual ~CUpnpContainerCheckerAo();

    /**
     * Creates the default containers if they do not exist,
     * empties root container and cleans uploaded container
     * @since S60 3.1
     * @return Errorcode of the operation start
     */
    TInt ValidateContainerStructureL( RArray<TInt>* aIds );

    /**
     * Creates the default containers if they do not exist
     * Add ids of the default containers to array
     * @since S60 3.1
     * @param aIds The container Ids, indexed using TContainerPosition type
     * @return Errorcode of the operation start
     */
    TInt CheckDefaultContainersL( RArray<TInt>* aIds );

    /**
     * Request this active ocject to stop as soon as is possible
     * Stop is indicated to client via callback
     * @since S60 3.1
     * @param aStopType Type of stop operation
     */
    void RequestStop( MUpnpSharingCallback::TSharingStopType aStopType );


private:

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
     * The active object main loop
     * @since S60 3.1
     */
    void RunL();

    /**
     * The active object cancel function
     * @since S60 3.1
     */
    void DoCancel();

    /**
     * Perform cleanup in case RunL leaves
     * @since S60 3.1
     * @return error code
     */
    TInt RunError( TInt aError );

    /**
     * Perform allocating of resources needed for processing upload events
     * @since S60 3.1
     * @return error code
     */
    TInt InitializeL();

    /**
     * Query default container structure ids from media server
     * @since S60 3.1
     */
    void QueryDefaultContainerIdsL();

    /**
     * Handle the query result
     * @since S60 3.1
     */
    void QueryDefaultContainerIdsResultL();

    /**
     * Handle the item query result
     * @since S60 3.1
     */
    void QueryItemFromCdsResultL();

    /**
     * Unshare the items in iItemList
     * @since S60 3.1
     */
    void UnshareItemListL();

    /**
     * Share container on basis of ID
     * @since S60 3.1
     */
    void ShareContainerL( TInt aPos );

    /**
     * Handle the sharing operation result
     * @since S60 3.1
     */
    void ShareContainerResult( );

    /**
     * Unshares container under processing
     * @since S60 3.1
     */
    void UnshareContainerL();

    /**
     * Handle result of the unshare container operation
     * @since S60 3.1
     */
    void UnshareContainerResultL();

    /**
     * Query containers in root level
     * @since S60 3.1
     */
    void QueryContainerItemsL();

    /**
     * Create browse criteria for media server query
     * @since S60 3.1
     * @return pointer to CUpnpBrowseCriteria instance
     */
    CUpnpBrowseCriteria* CreateBrowseCriteriaLC() const;

    /**
     * Clean allocated resources for possible next run
     * @since S60 3.1
     */
    void Cleanup();

    /**
     * Complete own request
     * @since S60 3.1
     * @param aError Errorcode to use in completion
     */
    void SelfComplete( TInt aError );



private:

    /**
     * Media server API to share files
     * owned
     */
    CUpnpFileSharing* iFileSharing;

    /**
     * Media server session handle
     */
    RUpnpMediaServerClient iMediaServer;

    /**
     * Used to hold the upper ao
     * Stopped when this class has nothing to do. Not owned.
     */
    MUpnpSharingCallback* iCallback;

    /**
     * Hanlders current state
     */
    TInt iState;

    /**
     * Handlers current operation
     */
    TInt iCurrentOperation;

    /**
     * Id of the Images& Videos container
     */
    TInt iImageVideoContainerId;

    /**
     * Id of the Music container
     */
    TInt iMusicContainerId;

    /**
     * Id of the Upload container
     */
    TInt iUploadContainerId;

    /**
     * Array of shared container IDs
     */
    RArray<TInt> iSharedContainerIds;

    /**
     * The currently processed container
     * owned
     */
    CUpnpContainer* iTmpContainer;

    /**
     * Structure used to hold the containers queries from media server
     * In practice it is used to hold "Images& VIdeos" & "Music" & "Uploaded"
     * containers.
     * owned
     */
    CUpnpContainerList*  iContainerList;
    
    /**
    * Structure used to hold possbile unwanted items when cleaning 
    * some folder
    * owned
    */
    CUpnpItemList* iItemList;
    
    /**
     * Holds the position of the container under processing
     */
    TInt iContainerPosition;

    /**
     * The number of items in item query
     */
    TInt iTotalMatches;

    /**
     * The position when unsharing list of items
     */
    TInt iExecStatus;

    /**
     * Operation mode as defined by TOperationMode
     */
    TInt iOperationMode;

    /**
     * Pointer to client structure to contain the container ids
     * Not owned
     */
    RArray<TInt>* iClientIds;

    /**
     * Indicates if the client has requested stop
     * Value is the stop type
     */
    TInt iStopRequested;

    /**
     * State variable which informs if there is need to do
     * error handling
     */
    TInt iRecoverError;
    
    /**
     * The number of matches returned by media server
     * Not used but required by CUpnpFileSharing API
     */
     TInt iMatchCount;

    /**
     * temporary storage for object count when querying items from media
     * server
     */
    TInt iObjectCount;
    };


#endif // __UPNPCONTAINERCHECKERAO_H__
