/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  handles music playing within UPnP framework
*
*/


#ifndef C_UPNPMUSICADAPTER_H
#define C_UPNPMUSICADAPTER_H

// INCLUDES
#include <e32base.h>
#include <mpxplaybackobserver.h> // base class
#include <mpxcollectionobserver.h> // base class

// FORWARD DECLARATIONS
class MUPnPAVController;
class CUpnpAVDevice;
class CUPnPPlayListFiller;
class MUPnPPlaylistFiller;
class CMPXCollectionPlaylist;
class CEikAppUi;
class CMPXMediaArray;
class MMPXPlaybackUtility;
class MMPXPlayerManager;
class CUpnpAVDevice;
class MMPXCollectionUtility;
class MMPXViewUtility;
class CMPXMedia;
class CUPnPMpxHelper;

// CONSTANT DEFINITIONS
// ** a message to music player to complete an immediate exit
const TInt KMusicPluginMessageExit = 3001;

/**
 * A class providing play back services for playing within UPnP framework.
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPMusicAdapter : public CBase,
                          public MMPXPlaybackObserver,
                          public MMPXPlaybackCallback,
                          public MMPXCollectionObserver
    {

public: // interface methods

    /**
     * 1st phase constructor
     *
     * @param aAVController reference to AVController resource. 
     * @since Series 60 3.1
     * @return a new CUPnPMusicAdapter instance
     */
    IMPORT_C static CUPnPMusicAdapter* NewL(
        MUPnPAVController& aAVController );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    IMPORT_C virtual ~CUPnPMusicAdapter();

    /**
     * Initiates music play in a remote renderer.
     * The method will block until music player exits.
     * Music adapter will take ownership of the filler in all conditions.
     *
     * @param aAppUi the app.ui in case we are displaying a view/dialog
     * @param aFiller Object that contains playlist information
     * @param aRenderer the selected renderer device
     * @return exit status / reason
     * @since Series 60 3.1
     */
    IMPORT_C TInt ExecuteMusicLiteInRemoteL(
        CEikAppUi* aParentUi,
        MUPnPPlaylistFiller* aFiller,
        const CUpnpAVDevice& aRenderer );

    /**
     * Initiates music play in the local device
     * The method will block until music player exits.
     * Music adapter will take ownership of the filler in all conditions.
     *
     * @param aAppUi the app.ui in case we are displaying a view/dialog
     * @param aFiller Object that contains playlist information
     * @return exit status / reason
     * @since Series 60 3.1
     */
    IMPORT_C TInt ExecuteMusicLiteInLocalL(
        CEikAppUi* aAppUi,
        MUPnPPlaylistFiller* aFiller );

    /**
     * Initiates music play in a remote renderer.
     * The method will block until music player exits.
     * Music adapter will take ownership of the filler in all conditions.
     *
     * @param aAppUi the app.ui in case we are displaying a view/dialog
     * @param aFiller Object that contains playlist information
     * @param aRenderer the selected renderer device
     * @return exit status / reason
     * @since Series 60 3.1
     */
    IMPORT_C TInt ExecuteMusicInRemoteL(
        CEikAppUi* aParentUi,
        CUPnPPlayListFiller* aFiller,
        const CUpnpAVDevice& aRenderer );

    /**
     * Initiates music play in the local device
     * The method will block until music player exits.
     * Music adapter will take ownership of the filler in all conditions.
     *
     * @param aAppUi the app.ui in case we are displaying a view/dialog
     * @param aFiller Object that contains playlist information
     * @return exit status / reason
     * @since Series 60 3.1
     */
    IMPORT_C TInt ExecuteMusicInLocalL(
        CEikAppUi* aAppUi,
        CUPnPPlayListFiller* aFiller );

    /**
     * Dismiss the music player UI. The Execute... method will
     * immediately return with the given return code
     *
     * @param aReturnCode Exit code for the execution thread
     * @since Series 60 3.1
     */
    IMPORT_C void Dismiss( TInt aReturnCode = KErrCancel );

public: // component internal methods

    /**
     * Called when playlist fill has completed.
     * Does not pass ownership.
     *
     * @param aFiller the object that has completed its job.
     * @since Series 60 3.1
     */
    void Complete( MUPnPPlaylistFiller* aFiller );

    /**
     * The AVController resource
     * (to be used by the fillers)
     *
     * @return reference to the AVController resource
     * @since Series 60 3.1
     */
    MUPnPAVController& AVController();

    /**
     * The Collection UID used for the playlist.
     * Accessor for the filler
     */
    TUid PlaylistCollectionId();

    
    /**
     * The MPX CollectionUtility.
     * Accessor for the filler
     */
    MMPXCollectionUtility& CollectionUtility();


protected:
    
    /** @see MMPXPlaybackObserver */
    void HandlePlaybackMessage(CMPXMessage* aMsg, TInt aErr );

    /** @see MMPXPlaybackCallback */
    void HandlePropertyL( TMPXPlaybackProperty aProperty,
                          TInt aValue, 
                          TInt aError );

    /** @see MMPXPlaybackCallback */
    void HandleSubPlayerNamesL( TUid aPlayer, 
                                const MDesCArray* aSubPlayers,
                                TBool aComplete,
                                TInt aError );

    /** @see MMPXPlaybackCallback */
    void HandleMediaL(const CMPXMedia& aProperties, TInt aError );

    /** see MMPXCollectionMediaObserver */
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
    
    /** see MMPXCollectionObserver */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
    
    /** see MMPXCollectionObserver */
    void HandleOpenL( const CMPXMedia& aEntries, TInt aIndex, 
        TBool aComplete, TInt aError );

    /** @see MMPXPlaybackObserver */
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /** @see MMPXPlaybackObserver */
    void HandleCommandComplete( CMPXCommand* aCommandResult, TInt aError );

private:

    /**
     * Default constructor
     *
     * @param aAVController reference to AVController resource. 
     * @since Series 60 3.1
     */
    CUPnPMusicAdapter( MUPnPAVController& aAVController );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */
    void ConstructL();

    /**
     * Initialises the state before execution
     *
     * @since Series 60 3.1
     */
    void InitState();

    /**
     * Obtains ownership of the filler pointer.
     *
     * @param aFiller the filler object to own
     * @since Series 60 3.1
     */
    void TakeOwnershipOfTheFiller( MUPnPPlaylistFiller* aFiller );
    
    /**
     * Creates play list
     * @since Series 60 3.1
     */
    void CreatePlayListL();

    /**
     * A loop to show the UI
     * @since Series 60 3.1
     */
    void UiShowLoopL();

    /**
     * Commands the UI to be shown in UiShowLoop
     * @since Series 60 3.1
     */
    void DisplayUI();

    /**
     * Causes an internal exit
     * @param aReturnCode the return code for the thread
     * @since Series 60 3.1
     */
    void Exit( TInt aReturnCode );


    /**
     * Locally handles the corresponding callback
     */
    void DoHandlePlaybackMessageL( TInt aEvent, TInt aType, TInt aData );

    /**
     * Locally handles the corresponding callback
     */
    void DoHandleCollectionMessageL( TInt aEvent, TInt aType, TInt aData );

    /**
     * Locally handles the corresponding callback
     */
    void DoHandleSubPlayerNamesL( TUid aPlayer, 
                                const MDesCArray* aSubPlayers,
                                TBool aComplete,
                                TInt aError );

public: // type definitions

    /**
     * the adapter state
     */
    enum TAdapterState
        {
        EStateIdle = 0, // not running
        EStatePreparing, // Prepare phase (No UI)
        EStatePreparingWait, // Preparing, waiting async resp
        EStateInitialising, // Initialising, UI showing
        EStateRunning, // normal run state
        EStateClosing, // UI exiting
        };

    enum TFillerState
        {
        EFillerStateIdle = 0, // created
        EFillerStateFilling, // filling
        EFillerStateComplete // fill complete
        };

private: // data

    /**
     * AVController resource
     */
    MUPnPAVController&          iAVController;

    /**
     * The renderer device we are rendering on
     * NULL, if rendering locally
     * NO OWNERSHIP
     */
    const CUpnpAVDevice*        iTargetDevice;
    
    /**
     * Pointer to host application ui.
     * NO OWNERSHIP
     */
    CEikAppUi*                  iAppUi;

    /**
     * The current playlist filler object
     */
    MUPnPPlaylistFiller*        iCurrentFiller;

    /**
     * Whether the filler is in working state
     */
    TFillerState                iCurrentFillerState;

    /**
     * The current playlist
     */
    CMPXCollectionPlaylist*     iPlaylist;
    
    /**
     * The current temp playlist 
     * Own.
     */
    CMPXMediaArray*             iTrackArray;
    
    /**
     * For playlist creation
     * Own.
     */
    CUPnPMpxHelper*             iMpxHelper;

    /**
     * Collection ID to use when communicating to MPX
     */
    TUid                        iPlaylistCollectionId;

    /**
     * For playback
     * Own.
     */
    MMPXPlaybackUtility*        iPlaybackUtility;
    
    /**
     * For collection
     * Own.
     */
    MMPXCollectionUtility*      iCollectionUtility;
    
    /**
     * View utility.
     */
    MMPXViewUtility*            iViewUtility;

    /**
     * Tells are we using remote playback 
     */
    TBool                       iRemotePlayer;

    /**
     * Exit code
     */
    TInt                        iReturnCode;

    /**
     * Scheduler wait loop
     */
    CActiveSchedulerWait*       iWait;

    /**
     * state of the adapter
     */
    TAdapterState               iState;
    
    /**
     * For media object array
     */
    CMPXMedia*                  iMedia;

    };


#endif // C_UPNPMUSICADAPTER_H
