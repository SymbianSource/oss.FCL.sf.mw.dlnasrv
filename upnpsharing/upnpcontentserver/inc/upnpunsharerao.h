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
* Description:      CUpnpUnsharerAo class definitions
*
*/





#ifndef __UPNPUNSHARERAO_H__
#define __UPNPUNSHARERAO_H__


// INCLUDE FILES
#include <upnpbrowsecriteria.h>
#include <upnpcontainerlist.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpitemlist.h>
#include <upnpfilesharing.h>
#include <upnpmediaserverclient.h>
#include "upnpcontentserverdefs.h"
#include "upnpsharingcallback.h"

// CLASS DECLARATION
/**
 *  Active object class for unsharing UPnP containers in an incremental way
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CUpnpUnsharerAo ): public CActive
    {
    /**
     * Indicates the ongoing operation of the CUpnpUnsharerAo
     */
    enum THandlerOperations
        {
        EInitialize,
        EQueryContainersFromMediaServer,
        EQueryContainersFromMediaServerResult,
        EQueryItemsFromMediaServer,
        EQueryItemsFromMediaServerResult,
        EUnshareItemList,
        EUnshareContainer,
        EUnshareContainerResult,

        ENotActive
        };

 public: // Constructors and destructor

    /**
     * C++ default constructor.
     * @since S60 3.1
     * @param aClientWait, reference to client wait, for signalling completion
     * @param aPriority, Priority value for this active object
     */
    CUpnpUnsharerAo( MUpnpSharingCallback* aCallback, TInt aPriority );

    /**
     * Destructor.
     */
    virtual ~CUpnpUnsharerAo();

 protected:

    /**
     * Function is called when active request is ready
     * @since S60 3.1
     */
    virtual void RunL();

    /**
     * Cancels active request
     * @since S60 3.1
     */
    virtual void DoCancel();

    /**
     * Handle leaves from RunL function
     * @since S60 3.1
     */
    TInt RunError( TInt aError );

 public: // new functions

    /**
     * Unshares all items and subcontainers from given container recursively
     * Leaves the containers with aContainerId
     * @since S60 3.1
     * @param aContainerId Id of the container to empty
     * @return Error code of the operation
     */
    TInt EmptyContainer( TInt& aContainerId );

    /**
     * Request this active ocject to stop as soon as is possible
     * Stop is indicated to client via callback
     * @since S60 3.1
     * @param aStopType Type of stop operation
     */
    void RequestStop( MUpnpSharingCallback::TSharingStopType aStopType );

    /**
     * Return progress of the current unsharing operation 
     * Progress is shown as procents
     * @since S60 3.2.3
     * @return the progress value
     */
    TInt UnSharingProgress( ) ;

     /**
      * Get total items of the all unsharing operation 
      * @since S60 3.2.3
      * @return a numbers of the unsharing items 
      */
    TInt TotalItemsForUnShare( ) ;
    
 private:

    /**
     * Qyery list of containers from media server
     * @since S60 3.1
     * @param aId Id of the container to query containers from
     */
    void QueryContainersFromMediaServerL( TInt aId );

    /**
     * Handle the result of the container query
     * @since S60 3.1
     */
    void QueryContainersFromMediaServerResultL();

    /**
     * Query items from certain container from media server
     * @since S60 3.1
     * @param aId Id of the container to query items from
     */
    void QueryContainerItemsL( TInt aId );

    /**
     * Handle the result of the container query
     * @since S60 3.1
     */
    void QueryContainerItemsResultL();

    /**
     * Unshare the items in iItemList
     * @since S60 3.1
     */
    void UnshareItemListL();

    /**
     * Unshare the container pointed by iCurrentContainerId
     * @since S60 3.1
     */
    void UnshareContainerL();

    /**
     * Handle the result of container unshare
     * @since S60 3.1
     */
    void UnshareContainerResultL();

    /**
     * Perform allocating of resources needed for processing upload events
     * @since S60 3.1
     * @param aContainerId The id of the initial container
     * @return error code
     */
    TInt InitializeL( TInt& aContainerId );

    /**
     * Creates browse criteria for media server querys
     * @since S60 3.1
     */
    CUpnpBrowseCriteria* CreateBrowseCriteriaLC() const;

    /**
     * Complete own async request
     * @since S60 3.1
     * @param aError Error code to complete request
     */
    void SelfComplete( TInt aError );

    /**
     * Clean the internal variables after error/when finished
     * @since S60 3.1
     */
    void Cleanup();



 private: // data

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
     * Structure used to hold the containers queries from media server
     * In practice it is used to hold "Images& VIdeos" & "Music" & "Uploaded"
     * containers.
     * Ordering from end to beginning
     * owned
     */
    CUpnpContainerList*  iContainerList;

    /**
     * The id of the current container
     */
    TInt iCurrentContainerId;

    /**
     * Array for all container levels used in processing
     * Ordering top-down.
     */
    RPointerArray<CUpnpContainerList> iContainerLists;

    /**
    * Structure used to hold possbile unwanted items when cleaning
    * some folder
    */
    CUpnpItemList* iItemList;

    /**
     * This is started by client if needed. Stopped when profiling is ready
     * not owned.
     */
    MUpnpSharingCallback* iCallback;

    /**
     * The container to process
     */
    TInt iUpContainer;

    /**
     * Total number of items in all operations to media server
     */
    TInt iAllTotalItems;   
        
    /**
     * Total number of items in some operations to media server
     */
    TInt iTotalItems;

    /**
     * Total number of containers received in media server container query
     */ 
    TInt iTotalContainers;

    /**
     * The position when querying containers in segments
     */ 
    TInt iContainerBrowsePosition;

    /**
     * The postion when incrementally processing large number of items
     */
    TInt iExecStatus;

    /**
     * State of the active object used to determine the next operation after
     * RunL completes
     */
    TInt iCurrentOperation;

    /**
     * Indicates if the client has requested stop
     * Value is the stop type
     */
    TInt iStopRequested;

    /**
     * Indicates the last successfully unshared item
     * Not used but required by CUpnpFileSharing API
     */
    TInt iUnsharePosition;
    };

#endif  // __UPNPUNSHARERAO_H__

// End of file
