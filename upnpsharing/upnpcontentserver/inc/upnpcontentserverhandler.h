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
* Description:      The header of content server main scheduler class
 *
*/





#ifndef __UPNPCONTENTSERVERHANDLER_H__
#define __UPNPCONTENTSERVERHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <e32property.h>

#include "upnpcontentserverdefs.h"
#include "upnpcontentserverclient.h"
#include "upnpcontentsharingobserver.h"
#include "upnpsharingcallback.h"
#include "upnpsharingrequest.h"
#include "upnpmediaserverclient.h"
#include "upnpcontentsharerao.h"

// FORWARD DECLARATIONS
class CUpnpSelectionReader;
class CUpnpContentMetadataUtility;
class CUpnpSharingRequest;
class CUpnpContentServer;
class CUpnpCdsLiteObjectArray;


/**
 * Helper class for storing pending request
 */
class TUpnpPendingSharingRequest
    {
public:

    /**
     * Kind of operation this information is related to
     * images&videos or playlists
     */
    TInt iMediaType;

    /**
     * Array containing requested sharing items.
     */
    RArray<TInt> iMarkedItems;

    };

/**
 *  A class to schedule the active objects in server
 *
 *  @since S60 3.1
 */
class CUpnpContentServerHandler : public CBase,
                                  public MUpnpSharingCallback
    {

public:

    /**
     * 2-phased constructor.
     */
    static CUpnpContentServerHandler* NewL(
        CUpnpContentServer* aServer );

    /**
     * Perform initialization of the handler
     * @since S60 5.2
     */
    void InitializeL();

    /**
     * C++ destructor.
     */
    virtual ~CUpnpContentServerHandler();

    /**
     * Sets the observer to session class
     * @since S60 3.1
     * @param aObserver Pointer to observer in session class
     */
    void SetContentSharingObserverL( 
        MUpnpContentSharingObserver* aObserver );

    /**
     * Start the media server upload listener, leave if error
     * @since S60 3.1
     */
    void StartUploadListenerL();

    /**
     * Stop the media server upload listener, leave if error
     * @since S60 3.1
     */
    void StopUploadListenerL();

    /**
     * Get the strings for the UI
     * @since S60 3.1
     * @param aContainerType Type of the content requested
     */
    void GetSelectionContentL( const TInt& aContainerType );

    /**
     * Get the selected items for the UI
     * @since S60 3.1
     * @param aMarkedItems Array of previous sharing selections
     * @param aType Type of selections to request
     */
    void GetSelectionIndexesL( RArray<TInt>& aMarkedItems,
                               const TInt aType );

    /**
     * Start sharing
     * @since S60 3.1
     * @param aMarkedItems The new sharing selections
     * @param aType Type of sharing selections
     */
    void ChangeShareContentL( const RArray<TInt>& aMarkedItems,
                              const TInt aType );

    /**
     * First read the selections then refresh.
     * Leaves with KErrServerBusy if sharing is ongoing
     * @since S60 3.1
     * @param aType Type of refresh requested
     */
    void RefreshShareContentL(
        TInt aType );

    /**
     * Determines if it is possible to stop the server
     * @since S60 3.1
     */
    TBool CanStop() const;

    /**
     * Switch media server offline, change internal states accordingly
     * @since S60 3.1
     */
    TBool ConnectionLostL();

    // from MUpnpSharingCallback
    //

    /**
     * The CUpnpContentSharingAo uses this to indicate it has done sharing
     * @since S60 3.1
     * @param aErr Error code
     * @param aType Type of sharing completed
     */
    void CompleteSharingOperationL(
        const TInt& aErr, const TInt& aType );
    /**
     * Cancel the current sharing operation
     * @since S60 3.1
     * @param aErr Error code
     */
    void CancelSharingOperationL(
        const TInt& aErr );

    /**
     * Update the progress PubSub key
     * @since S60 3.1
     * @param aProgress Progress to update
     */
    void SetProgressL(
        const TInt& aProgress );

private:
    /**
     * C++ constructor.
     * @since S60 3.1
     */
    CUpnpContentServerHandler( CUpnpContentServer* aServer );

    /**
     * The main sharing loop
     * @since S60 5.2
     */
    void DoShare( );

    /**
     * Cleanup  resources, also state variables are cleaned
     * @since S60 3.1
     */
    void Cleanup();

    /**
     * 2nd phase constructor.
     * @since S60 3.1
     */
    void ConstructL();

    /**
     * Get id of the default container determined by aType
     * @since S60 3.1
     * @param aType Determines which container id is returned
     */
    TInt GetContainerId( const TInt aType ) const;

    /**
     * Set the values of the iImageVideoSharingReq or iMusicSharingReq
     * depending of aType
     * @since S60 3.1
     * @param aMarkedItems The new sharing selections
     * @param aType Type of sharing selections
     * @param aPendingRequest Pending request or not
     */
    void SetSharingRequestL(
        const RArray<TInt>& aMarkedItems,
        const TInt aType );

    /**
     * Handle errors from active objects
     * @since S60 3.1
     * @param aError Error code
     */
    void HandleError( TInt aError );
    
    /**
     * Get filenames of currently selected playlists/albums
     * @since S60 5.2
     * @param aMarkedItems The new sharing selections
     * @param aType Type of sharing selections (EImageAndVideo 
     * or EPlaylist)
     * @param aSharingType Type of sharing (EShareNone, EShareAll,
     * EShareMany)
     * @param aFilenames Array containing filenames
     * @param aClfIds Array containing collection ids
     */
    void GetSelectionFilenamesL( const RArray<TInt>& aMarkedItems,
        const TInt aType,
        const TInt aSharingType,
        CDesCArray& aFilenames,
        CDesCArray& aClfIds );

    /**
     * Get filenames of all image and video files
     * @since S60 5.2
     * @param aFilenames Array containing filenames
     */
    void GetAllImageAndVideoFilenamesL(
        CDesCArray& aFilenames );

    /**
     * Get filenames of all music files
     * @since S60 5.2
     * @param aFilenames Array containing filenames
     */
    void GetAllMusicFilenamesL(
        CDesCArray& aFilenames );

    /**
     * Get filenames of currently selected albums
     * @since S60 5.2
     * @param aMarkedItems The new sharing selections
     * @param aFilenames Array containing filenames
     * @param aClfIds Array containing collection ids
     */
    void GetSelectedImgAndVideoFilenamesL(
        const RArray<TInt>& aMarkedItems,
        CDesCArray& aFilenames,
        CDesCArray& aClfIds );

    /**
     * Get filenames of currently selected playlists
     * @since S60 5.2
     * @param aMarkedItems The new sharing selections
     * @param aFilenames Array containing filenames
     * @param aClfIds Array containing playlist ids
     */
    void GetSelectedMusicFilenamesL(
        const RArray<TInt>& aMarkedItems,
        CDesCArray& aFilenames,
        CDesCArray& aClfIds );
    
    /**
     * Resolves files to be shared and unshared lists
     * @since S60 5.2
     * @param aClfFilePaths Array containing clf file paths
     * @param aCdsObjects Array containing cds objects
     * @param aToBeSharedFiles Array containing files to be shared
     * @param aToBeUnsharedSharedFiles Array containing files to be unshared
     * @param aType Type of sharing selections (EImageAndVideo 
     * or EPlaylist
     */
    void CompareCdsToClfL( 
        CDesCArray& aClfFilePaths, 
        CUpnpCdsLiteObjectArray& aCdsObjects,
        RArray<TFileName>& aToBeSharedFiles,
        RArray<TFileName>& aToBeUnsharedSharedFiles,
        const UpnpContentServer::TUpnpMediaType aType );
    
    /**
     * Creates sharing request
     * @since S60 5.2
     * @param aType Type of sharing selections (EImageAndVideo 
     * or EPlaylist)
     * @param aSharingType Type of sharing (EShareNone, EShareAll,
     * EShareMany)
     */
    void CreateSharingRequestL(
        TInt aType, 
        TInt aSharingType );

    /**
     * Sets sharing request information
     * @since S60 5.2
     * @param aType Type of sharing selections (EImageAndVideo 
     * or EPlaylist)
     * @param aShareArray Array containing filenames to be shared
     * @param aUnshareArray Array containing filenames to be unshared
     * @param aClfIds Array containing collection ids
     */
    void SetSharingRequestInfo(
        TInt aType, 
        RArray<TFileName>* aShareArray,
        RArray<TFileName>* aUnshareArray,
        CDesCArray* aClfIds );

    /**
     * Resets the pending request info
     * @since S60 5.2
     */
    void ResetPendingRequestInfo();

    /**
     * Saves the pending request info
     * @since S60 5.2
     * @param aMarkedItems The new sharing selections
     * @param aType Type of sharing selections (EImageAndVideo 
     * or EPlaylist)
     */
    void SavePendingRequestInfoL(
        const RArray<TInt>& aMarkedItems,
        const TInt aType );
        
    /**
     * Resolves the wanted sharing type based on the marked items
     * @since S60 5.2
     * @param aMarkedItems The new sharing selections
     * @return  Sharing type (EShareNone, EShareAll, EShareMany)
     */
    TInt SharingType( const RArray<TInt>& aMarkedItems );
    
private:
    /**
     * Pointer to server process, used for stopping it
     * Not owned
     */
    CUpnpContentServer* iServer;

    /**
     * Pointer to CLF interface
     * owned
     */
    CUpnpContentMetadataUtility* iMetadata;

    /**
     * The sharing engine
     * owned
     */
    CUpnpContentSharerAo* iAo;

    /**
     * Pointer to corresponding session observer for returning results
     * not owned
     */
    MUpnpContentSharingObserver* iContentSharingObserver;

    /**
     * Gets the albums/playlists to UI
     * owned
     */
    CUpnpSelectionReader* iReader;

    /**
     * Currently processed media type, KErrNotFound if no sharing 
     * ongoing
     */
    TInt iOngoingSharingType;

    /**
     * The buffer for image and video sharing requests
     * owned
     */
    CUpnpSharingRequest* iVisualSharingReq;

    /**
     * The buffer for music sharing requests
     * owned
     */
    CUpnpSharingRequest* iMusicSharingReq;

    /**
     * The array for any sharing request which is not yet scheduled 
     * to run.
     */
    TUpnpPendingSharingRequest iPendingSharingReqInfo;

    /**
     * MediaServer Handle
     * Owned
     */
    RUpnpMediaServerClient* iMediaServer;

    /**
     * Error code, reset after transferred to client
     */
    TInt iErrorToClient;

    /**
     * The Publish subscribe key to deliver progress infromation about 
     * sharing
     */
    RProperty iProgressProperty;

    /**
     * Sharing algorithm for handling sharing and unsharing operations
     * Owned
     */
    MUpnpSharingAlgorithm* iSharingAlgorithm;
    
    };

#endif // __UPNPCONTENTSERVERHANDLER_H__

// End of file
