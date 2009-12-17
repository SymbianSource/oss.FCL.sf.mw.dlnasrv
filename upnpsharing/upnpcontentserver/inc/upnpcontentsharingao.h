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
* Description:      file sharing engine active object class defition
 *
*/







#ifndef __UPNPCONTENTSHARINGAO_H__
#define __UPNPCONTENTSHARINGAO_H__


// Include Files
#include <e32base.h>
#include <badesca.h>  //for CDesCArray
#include <upnpmediaserverclient.h>
#include <upnpbrowsecriteria.h>
#include <upnpcontainerlist.h>
#include "upnpplaylistservices.h"
#include "upnpalbumservices.h"
#include "upnpselectionwriter.h"
#include "upnpsharingcallback.h"
#include "upnpcontentserverdefs.h"
#include "upnpcontentmetadatautility.h"

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;
class CUpnpContainer;
class CUpnpObject;
class CUpnpFileSharing;
class CUpnpItemList;
class CUpnpSelectionWriter;
class CUpnpContentMetadataUtility;
class MUpnpSharingCallback;


// CLASS DECLARATION
/**

*  Active object class for file sharing
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CUpnpContentSharingAo ): public CActive
    {
    /**
     * Enumeration for different sharing types
     */
    enum TSharingType
        {
        ESharing,
        EUnsharing
        };

    /**
     * States of this active object
     */
    enum TUPnPSharingState
        {
        ENotActive,
        EShareFiles,
        EStartSharing,
        EStopSharing,
        EUnshareMainContainers,
        EShareMainContainers,
        // 2.0
        EValidateDefaultContainers,
        EGetSharedContainers,
        EGetSharedItems,//8
        EUnshareContainers,
        EUnshareContainersResult,
        EUnshareItems,//11
        EShareDefaultContainers,
        EShareDefaultContainersResult,
        EShareContainers,
        EShareContainersResult,
        EShareItems, //16
        EShareItemsResult,
        EShareAllItems,//18
        EShareAllItemsResult,
        ECompleteSharingRequest,
        EEnumerateSelections
        };

    /**
     * Sub states of this active object
     */
    enum TUpnpSharingSubState
        {
        EIdle,
        EProfileItemList,
        EProfileItemListResult,
        EShareItemList,
        EShareItemListResult
        };

 public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @since S60 3.1
     * @param aEngine Callback to handler
     * @param aMetadata Pointer to CLF API
     */
    static CUpnpContentSharingAo* NewL(
        MUpnpSharingCallback* aEngine,
        CUpnpContentMetadataUtility* aMetadata );

    /**
     * Destructor.
     * @since S60 3.1
     */
    virtual ~CUpnpContentSharingAo();

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
     * Handle leaves from RunL
     * @since S60 3.1
     */
    TInt RunError( TInt aError );

 public: // new functions

    /**
     * Initialize parameters needed in sharing
     * @since S60 3.2
     * @param aContainerType Type of sharing ongoing
     * @param aCurrentSelection Array of album/playlist selections
     * @param aPlaylistIds Ids of collections
     * @param aPlaylistNames names of collections
     * @param aClientStatus Clients TRequestStatus
     */
    void InitializeL(
        TUpnpMediaType aContainerType,
        const RArray<TInt>& aCurrentSelection,
        CDesCArray& aPlaylistIds,
        CDesCArray& aPlaylistNames,
        const TInt aContainerId );

    /**
     * Starts file sharing process
     * @since S60 3.1
     */
    void ShareFiles( );


    /**
     * Return the number of shared items in current selection
     * @since S60 3.1
     * @param aContainerType Type of selections
     * @return TInt Object count
     */
    TInt SelectionObjectCountL(
        const TUpnpMediaType aContainerType );

    /**
     * Return progress of the current sharing operation and type of sharing
     * Progress is shown as procents
     * @since S60 3.1
     * @param aContainerType Type of the sharing requested
     * @return the progress value
     */
    TInt SharingProgress(
        TUpnpMediaType& aContainerType ) const;

    /**
     * From CActive
     * @since S60 3.1
     */
    TBool IsActive() const;

    /**
     * Request this active ocject to stop as soon as is possible
     * Stop is indicated to client via callback
     * @since S60 3.1
     * @param aStopType Type of stop operation
     */
    void RequestStop( MUpnpSharingCallback::TSharingStopType aStopType );


 private:

    /**
     * C++ default constructor.
     * @since S60 3.1
     * @param aEngine Pointer to handler callback
     * @param aMetadata Pointer to CLF API
     */
    CUpnpContentSharingAo( MUpnpSharingCallback* aEngine,
                           CUpnpContentMetadataUtility* aMetadata );

    /**
     * By default Symbian 2nd phase constructor is private.
     * @since S60 3.1
     */
    void ConstructL();

    /**
     * Opens mediaserver and file sharing session
     * @since S60 3.1
     * @return TInt, KErrNone if connection is ok
     *               error code otherwise
     */
    TInt OpenFileSharingSession();

    /**
     * Closes file sharing and mediaserver session
     * @since S60 3.1
     */
    void CloseFileSharingSession();

    /**
     * Create container
     * @since S60 3.1
     * @param aContainerType Type of container to create
     * @param aTitle Title of the container
     */
    CUpnpContainer* CreateContainerL( TUpnpMediaType aContainerType,
                                      const TDesC8& aTitle ) const;

    /**
     * Connects to media server
     * @since S60 3.1
     */
    TInt ConnectMediaServer();

    /**
     * Processes containers to unshare
     * @since S60 3.1
     */
    void UnshareContainersL();

    /**
     * Adjust the value iShareAllSelection on basis of user selections
     * @since S60 3.1
     * @param aSelections The selections received from UI
     */
    void AdjustShareAllSelection(
        const RArray<TInt>& aSelections );

    /**
     * Share the current container, iContainer
     * @since S60 3.1
     */
    void ShareContainersL();

    /**
     * Shares the current set of items, iItemList
     * @since S60 3.1
     */
    void ShareItemsL();


    /**
     * Process the result of the container sharing operation
     * @since S60 3.1
     */
    void ShareContainersResultL();

    /**
     * Process the result of items sharing operation
     * @since S60 3.1
     */
    void ShareItemsResultL();

    /**
     * Share individual items, not in any album/playlist
     * @since S60 3.1
     */
    void ShareAllItemsL();

    /**
     * Process the result of sharing individual items
     * @since S60 3.1
     */
    void AllItemsResultL();

    /**
     * Calculate correct index for indexing CLF on basis of
     * iCurrentSelection and iShareAllSelection
     * Verifies that the container selections user has made are valid against 
     * current CLF / playlist content
     * @since S60 3.1
     * @return Index as TInt, KErrNotFound if the selection is not valid
     */
    TInt GetContainerIndexL();

    /**
     * Complete own request
     * @since S60 3.1
     */
    void SelfComplete( const TInt aError );

    /**
     * Appends image files to iItemList. Starts from index iImageIndex and 
     * appends maximum of KContentDirCommitAmount items. Updates iImageIndex.
     * @since S60 3.1
     */
    void AppendImageFilesToListL();
    
    /**
     * Appends video files to iItemList. Starts from index iVideoIndex and 
     * appends maximum of KContentDirCommitAmount items. Updates iVideoIndex.
     * @since S60 3.1
     */
    void AppendVideoFilesToListL();

    /**
     * Profiles list of items
     * @since S60 3.1
     */
    void ProfileItemListL();

    /**
     * Handles the result of profiling single item
     * @since S60 3.1
     */
    void ProfileItemListResult();

    /**
     * Shares list of UpnpItems
     * @since S60 3.1
     */
    void ShareItemListL( const TInt& aParentId );

    /**
     * Handles the result of ShareItemListL()
     * @since S60 3.1
     */
    void ShareItemListResultL();

    /**
     * Determines next state according to iSharingSubState variable
     * @since S60 3.1
     */
    void HandleSharingSubstatesL();

    /**
     * Find a conatiner name from available containers on basis of id 
     * @since S60 3.1
     * @param aId Id to search
     * @param aPlaylistName Name of playlist pointed by id
     * @param aContainerType Type of the container
     */
    TInt GetContainerNameByIdL( const TDesC& aId, TDes8& aPlaylistName, 
                                const TInt aContainerType );
    
    /**
     * When delete the iItenList Item pointer,
     * also delete the CUpnpObject object the pointer pointed. 
     */
    void DeleteListUpnpObject();
    
 private: // data

    /**
     * Pointer to calling handler
     * not owned
     */
    MUpnpSharingCallback*         iEngine;

    /**
     * main level container index
     */
    TInt                            iContainerIdx;

    /**
     * object level container index
     */
    TInt                            iObjectIdx;

    /**
     * mediaserver session handle
     */
    RUpnpMediaServerClient            iMediaServer;

    /**
     * Media server API to share files
     * owned
     */
    CUpnpFileSharing*                 iFileSharing;

    /**
     * Type of container to be unshared; Music or Images & video
     */
    TUpnpMediaType             iContainerType;

    /**
     * Indicates whether the starting sharing or stopping
     * sharing is ongoing.
     */
    TInt                            iSharingState;

    /**
     * Container holder to use when sharing containers
     * owned
     */
    CUpnpContainer* iTmpContainer;

    /**
     * item holder used during sharing items
     * owned
     */
    CUpnpItem*      iTmpItem;

    /**
     * Structure used to hold multiple items during sharing
     * owned
     */
    CUpnpItemList*                  iItemList;

    /**
     * Indexes relative to queried content in UI
     * Received when starting sharing ShareFilesL
     */
    RArray<TInt> iCurrentSelections;

    /**
     * The IDs of the shared containers will appear here
     * owned
     */
    CArrayFix<TInt>*                    iSharedContainerIds;

    /**
     * Id of the default container to where share
     * Obtained from handler
     */
    TInt iContainerId;

    /**
     * The type of sharing: EShareNone, EShareAll, EShareMany
     */
    TInt iShareAllSelection;

    /**
     * Utility class for CLF
     * owned
     */
    CUpnpContentMetadataUtility*        iMetadataUtility;

    /**
     * The number of succesfully shared items( containers not incl.)
     */
    TInt iSharedContainersCount;

    /**
     * Class handling the updating of the shared items
     * owned
     */
    CUpnpSelectionWriter* iWriter;

    /**
     * Progress of the sharing process
     */
    TInt iSharingProgress;

    /**
     * Are all images already shared?
     */
    TBool iAllImagesShared;
    
    /**
     * Are all videos already shared?
     */
    TBool iAllVideosShared;
    
    /**
     * iImageIndex indicates up to which index is image sharing done so far.
     */
    TInt iImageIndex;

    /**
     * iVideoIndex indicates up to which index is video sharing done so far.
     */
    TInt iVideoIndex;

    /**
     * Possbile states when processing items. See TUpnpSharingSubState
     */
    TInt iSharingSubState;

    /**
     * Index used during profiling
     */
    TInt iProfileInd;

    /**
     * Array holding filenames of items to be shared
     */
    CDesCArray* iFileNameArr;

    /**
     * Flag indicating if there is profiling ongoing
     */
    TBool iProfilerActive;

    /**
     * Flag indicating if there if the destructor is called
     */
    TBool iDeletePending;

    /**
     * Indicates if the client has requested stop
     * Value is the stop type
     */
    TInt iStopRequested;

    /**
     * Handle to MPX interface
     */
    CUPnPPlaylistServices* iPlaylists;

    /**
     * Handle to MPX interface
     */
    CUPnPAlbumServices* iCollections;

    /**
     * List of playlist ids
     * Not owned
     */
    CDesCArray* iContainerIds;
    
    /**
     * List of playlist names
     * Not owned
     */
    CDesCArray* iContainerNames;
    };

#endif  // __UPNPCONTENTSHARINGAO_H__

// End of file
