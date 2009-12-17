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
#include "upnpcontentsharingao.h"
#include "upnpcontentsharingobserver.h"
#include "upnpsharingcallback.h"
#include "upnpsharingrequest.h"

#include "upnpmetadataobserver.h"
#include "upnpcontainercheckerao.h"
#include "upnpunsharerao.h"

// FORWARD DECLARATIONS

class CUpnpSelectionReader;
class CUpnpContentMetadataUtility;
class CUpnpContentSharingAo;
class CUpnpSharingRequest;
class CUpnpContentServer;
class CUPnPPeriodic;


/**
 *  A class to schedule the active objects in server
 *
 *  @since S60 3.1
 */
class CUpnpContentServerHandler : public CBase,
                                  public MUpnpSharingCallback,
                                  public MUpnpMetadataObserver
    {
    /**
     * Indicates internal state of the CUpnpContentServerHandler
     */
    enum THandlerState
        {
        ESchedulingSharing,
        EWaitingUploads,
        EProcessingUploads,
        ENotActive
        };

    /**
     * Indicates which active object is running
     */
    enum TSharingPhase
        {
        ESharingInActive,
        ECheckDefaults,
        EUnshare,
        EShare
        };

public:

    /**
     * 2-phased constructor.
     */
    static CUpnpContentServerHandler* NewL( CUpnpContentServer* aServer );

    /**
     * C++ destructor.
     */
    virtual ~CUpnpContentServerHandler();

    /**
     * Sets the observer to session class
     * @since S60 3.1
     * @param aObserver Pointer to observer in session class
     */
    void SetContentSharingObserverL( MUpnpContentSharingObserver* aObserver );

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
     * Callback from metadatautility to inform that refresh is completed
     * @since S60 3.1
     */
    void RefreshDoneL();

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

    /**
     * Cancel the current sharing operation
     * @since S60 3.1
     * @param aErr Error code
     */
    void ValidateDefaultContainersL();




private:
    /**
     * C++ constructor.
     * @since S60 3.1
     */
    CUpnpContentServerHandler( CUpnpContentServer* aServer );

    /**
     * The main sharing loop
     * @since S60 3.1
     */
    void DoShareL( );

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
     * Fill the progress info for aType
     * @since S60 3.1
     * @param aArr Array to store progress information
     * @param aType Determines which container id is returned
     */
    void FillProgressInfoL(
        RArray<TUpnpProgressInfo>& aArr,
        const TInt aType );

    /**
     * Set the values of the iImageVideoSharingReq or iMusicSharingReq
     * depending of aType
     * @since S60 3.1
     * @param aMarkedItems The new sharing selections
     * @param aType Type of sharing selections
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
     * Perform CLF refresh in background
     * @param aPtr Pointer to CUpnpContentServerHandler instance
     * @return EFalse
     */
    static TInt RefreshClfL( TAny* aPtr );
    
    
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
    CUpnpContentSharingAo* iAo;

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
     * Used to wait CLF refresh
     */
    CActiveSchedulerWait iWait;

    /**
     * Handlers current state
     */
    TInt iHandlerState;

    /**
     * Currently processed request index
     */
    TInt iBufferPosition;

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
     * The buffer for any sharing request which is not yet scheduled to run
     * owned
     */
    CUpnpSharingRequest* iPendingSharingReq;

    /**
     * Ongoing sharing operation. See TSharingPhase
     */
    TInt iSharingPhase;

    /**
     * Active object checking the file structure of the media server
     */
    CUpnpContainerCheckerAo* iContainerChecker;

    /**
     * Active object handling the unsharing of items and containers
     */
    CUpnpUnsharerAo* iUnsharer;

    /**
     * Storage for ids of default containers
     */
    RArray<TInt> iDefaultContainerIds;

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
    * Flag to indicate if the iContainerChecker is created in
    * staring sharing context
    */
    TBool iStartupCleaning;

    /**
     * The Publish subscribe key to deliver progress infromation about sharing
     */
    RProperty iProgressProperty;

    CUPnPPeriodic* iIdle;
    
    };

#endif // __UPNPCONTENTSERVERHANDLER_H__
