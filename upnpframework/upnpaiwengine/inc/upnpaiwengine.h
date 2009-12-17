/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the CUPnPAiwEngine class.
*
*/


#ifndef UPNP_AIW_ENGINE_H
#define UPNP_AIW_ENGINE_H

//  INCLUDES
// System
#include <e32base.h>
#include <AknProgressDialog.h>

// avcontroller api
#include "upnpavrenderingsessionobserver.h"
#include "upnpavdeviceobserver.h"
#include "upnpitemresolverobserver.h"

// upnpframework / internal api's
#include "upnpcommonui.h"

// aiwengine internal
#include "upnpaiwengineobserver.h"
#include "upnpaiwtimerobserver.h"


// ENUMERATIONS
enum TUPnPEngineState
    {
    EUPnPEngineNotConnected = 1,
    EUPnPEngineConnectionLost,
    EUPnPEngineConnected,
    EUPnPEngineActive,
    EUPnPEngineBrowsingHomeNetwork
    };

enum TUPnPAiwEngineDeviceType
    {
    EUPnPAiwEngineTargetDeviceWithAudioSupport = 1,
    EUPnPAiwEngineTargetDeviceWithImageAndVideoSupport,
    EUPnPAiwEngineTargetDeviceWithImageSupport
    };

enum TUPnPAiwEngineTransferMode
    {
    EAiwEngineCopy = 0,
    EAiwEngineCopyPlaylist,
    EAiwEngineMove
    };    

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CUPnPAiwTimer;
class MUPnPAVController;
class MUPnPAVRenderingSession;
class MUPnPAVSessionBase;

// CLASS DECLARATION

/**
*  CUPnPAiwEngine
*  The class works as an engine component between the UPnP AIW, UPnP Common 
*  UI components and UPnPAVController components.
*  @since S60 3.0
*/
class CUPnPAiwEngine : public CBase,
                       public MUPnPAVRenderingSessionObserver,
                       public MUPnPAiwTimerObserver,
                       public MProgressDialogCallback,
                       public MUPnPAVDeviceObserver,
                       public MUPnPItemResolverObserver
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CUPnPAiwEngine* NewL();

        /**
         * Releases the instance.
         *
         * @since S60 3.0
         */
        IMPORT_C static void ReleaseInstance();
        
        /**
         * Destructor.
         */
        virtual ~CUPnPAiwEngine();

    public: // Methods from MUPnPAVDeviceObserver

        // Unnecessary callback methods
        void UPnPDeviceDiscovered( const CUpnpAVDevice& /*aDevice*/ ) {}
        void UPnPDeviceDisappeared (const CUpnpAVDevice& /*aDevice*/ ) {}

        /**
         * Notifies that the WLAN connection has been lost. All sessions
         * are now usable and must be closed.
         *
         * @since Series 60 3.1
         * @return None
         */ 
        void WLANConnectionLost();
        
    public: // UPnP Aiw Engine call back

        /**
         * Sets the observer instance.
         *
         * @since S60 3.0
         * @param aObserver (MUPnPAiwEngineObserver*) observer instance
         */
        IMPORT_C void SetEngineObserver( MUPnPAiwEngineObserver* aObserver );

        /**
         * Removes the observer instance.
         *
         * @since S60 3.0
         */
        IMPORT_C void RemoveEngineObserver();

    public:

        /**
         * Returns the state of the UPnP AIW Engine.
         *
         * @since S60 3.0
         * @return TUPnPEngineState State of the engine
         */
        IMPORT_C TUPnPEngineState EngineState();

        /**
         * Opens the External media UI.
         *
         * Leaves if the UpnpAiwEngine is already in use, if the
         * UpnpAvController instantiation fails, or if the device selection/
         * browse dialog leaves.
         *
         * @since S60 3.0
         */
        IMPORT_C void OpenExternalMediaL();

        /**
         * Enables the showing of local media files on a remote renderer 
         * device.
         *
         * Leaves if the UpnpAiwEngine is already in use, if the
         * UpnpAvController instantiation fails, or if the device selection
         * dialog leaves.
         *
         * @since S60 3.0
         */
        IMPORT_C void EnableExternalL();

        /**
         * Disables the showing of local media files on remote renderer 
         * device.
         *
         * @since S60 3.0
         */
        IMPORT_C void DisableExternal();

        /**
         * Copies local files to a remote media server.
         *
         * @since S60 3.0
         * @param aFileNames (RPointerArray<TDesC>&) Reference to an array
         *        of TDesC objects containing the list of file names. All 
         *        files that are successfully copied are removed from the
         *        list.
         */
        IMPORT_C void CopyToExternalL( RPointerArray<TDesC>& aFileNames );

        /**
         * Copies a local playlist to a remote media server.
         *
         * @since S60 3.0
         * @param aFileNames (RPointerArray<TDesC>&) Reference to an array
         *        of TDesC objects containing the list of file names. All
         *        files that are successfully copied are removed from the 
         *        list.
         * @param aPlaylistName (const TDesC&) name of the playlist
         */
        IMPORT_C void CopyPlaylistToExternalL(
                                    const TDesC& aPlaylistName,
                                    RPointerArray<TDesC>& aFileNames );

        /**
         * Moves local files to a remote media server.
         *
         * @since S60 3.0
         * @param aFileNames (RPointerArray<TDesC>&) Reference to an array
         *        of TDesC objects containing the list of file names. All
         *        files that are successfully moved removed from the list.
         */
        IMPORT_C void MoveToExternalL( RPointerArray<TDesC>& aFileNames );

        /**
         * Plays (sends for rendering) local media file using the selected
         * UPnP Media renderer. Only image and video files are supported
         * after Upnp Fw 2.0.
         *
         * Leaves if the given filename is not valid, file is not a media
         * file, if the UpnpAiwEngine is not ready (enable external to done),
         * or if the bearer or remote upnp device is lost.
         *
         * @since S60 3.0
         * @param aFileName (const TDesC&) reference to the file name of
         *        the media file.
         */
        IMPORT_C void PlayL( const TDesC& aFileName );

        /**
         * Displays connection error if creating of control point fails.
         *
         * @since S60 3.1
         */
        IMPORT_C void ShowConnectionErrorL();

    public: // Methods to update the singleton reference counter

        /**
         * Sets the initial value of the user counter to zero.
         *
         * @since S60 3.0
         */
        void InitialiseUserCounter();

        /**
         * Increases the value of the user counter.
         *
         * @since S60 3.0
         */
        void IncreaseUserCounter();

        /**
         * Decreases the value of the user counter.
         *
         * @since S60 3.0
         */
        void DecreaseUserCounter();

        /**
         * Gets the value of the user counter.
         *
         * @since S60 3.0
         * @return TInt
         */
        TInt UserCounter() const;

        /**
         * Displays connection lost error if copying files fails.
         *
         * @since S60 3.2.3
         */
        void ShowConnectionLostCopyErrorL();

    private:

        /**
         * Transfers (copies/moves) local files/playlist to a remote media
         * server.
         *
         * Leaves if the given parameters are not valid, if the UpnpAiwEngine
         * is already in use, if user cancels the operation, or if the bearer
         * or remote upnp device is lost.
         *
         * @since S60 3.0
         * @param aMode (TUPnPAiwEngineTransferMode) transfer mode
         * @param aFileNames (RPointerArray<TDesC>&) Reference to an array
         *        of TDesC objects containing the list of file names. All
         *        files that are successfully copied are removed from the
         *        list.
         * @param aPlaylistName (const TDesC&) name of the playlist
         */
        void TransferToExternalL( TUPnPAiwEngineTransferMode aMode,
                                  RPointerArray<TDesC>& aFilePaths,
                                  const TDesC& aPlaylistName );

        /**
         * Starts a rendering session with UpnpAvController. Queries the
         * target rendering device.
         *
         * Leaves if any instantiation fails or used cancels the device
         * selection.
         *
         * @since S60 3.0
         * @param aDeviceType (TUPnPAiwEngineDeviceType) type of the device
         */
         void StartRenderingSessionL( TUPnPAiwEngineDeviceType aDeviceType );

        /**
         * Handles the UPnP device disappeared call back method.
         *
         * @since S60 3.0
         * @param aDevice (const CUpnpAVDevice&) pointer to the device info
         */
        void HandleUPnPDeviceDisappearedL( const CUpnpAVDevice& aDevice );

        /**
         * Waits for local media server file sharing activation (or user 
         * cancel).
         *
         * @since S60 3.1
         * @param aSession (MUPnPAVSessionBase*) pointer to the AVController
         *        session.
         */
        void StartLocalFileSharingL( MUPnPAVSessionBase *aSession );

    public: // Call back methods of MUPnPAVRenderingSessionObserver

        // Unused callback methods
        void VolumeResult( TInt /*aError*/,
                           TInt /*aVolumeLevel*/,
                           TBool /*aActionResponse*/ ) {}
        void MuteResult( TInt /*aError*/,
                         TBool /*aMute*/,
                         TBool /*aActionResponse*/ ) {}
        void PositionInfoResult( TInt /*aError*/,
                                 const TDesC8& /*aTrackPosition*/,
                                 const TDesC8& /*aTrackLength*/ ) {}
        void SetNextURIResult( TInt /*aError*/ ) {}

        /**
         * UPnP AV Controller calls this method to indicate that the requested
         * interaction operation (play, stop, etc.) is complete. In other
         * words, the target rendering device has changed it's state 
         * accordingly.
         *
         * @since Series 60 3.1
         * @param aError (TInt) error code
         * @param aOperation (TAVInteractOperation) operation Id
         */
        void InteractOperationComplete( TInt aError,
                                        TUPnPAVInteractOperation aOperation );

        /**
         * UPnP AV Controller calls this method as a response to SetUri
         * action.
         *
         * @since Series 60 3.1
         * @param aError (TInt) error code
         */
        void SetURIResult( TInt aError );

        /**
         * Notifies that the Media Renderer we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @param aReason (TUPnPDeviceDisconnectedReason) reason code
         */
        void MediaRendererDisappeared(
                            TUPnPDeviceDisconnectedReason aReason );

    public: // Call back methods of MUPnPAVSessionObserverBase
    
         /**
         * Notifies that the Media Server startup has completed. 
         *
         * @since Series 60 3.1
         * @return aError (TInt) error code
         */        
        void ReserveLocalMSServicesCompleted( TInt aError );

    public: // Call back methods of MAknProgressDialogCallback

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.0
         * @param aButtonId (TInt) ID of the button pressed
         */
        void DialogDismissedL( TInt aButtonId );

    public: // Timer related methods

        /**
         * Starts the UPnPAiWTimer.
         *
         * @since S60 3.0
         */
        void StartTimer();

        /**
         * Stops the UPnPAiWTimer.
         *
         * @since S60 3.0
         */
        void StopTimer();

        /**
         * Callback method for the UPnPAiwTimer.
         *
         * @since S60 3.0
         */
        void TimerCallback();

    private: // Helper methods

        /**
         * Method for selecting a device.
         *
         * Leaves if the engine is not ready (UpnpAvController not 
         * instantiated), if the user cancels the operation, or if the bearer
         * is lost during the device selection.
         *
         * @since S60 3.1
         * @param aDeviceType (TUPnPDeviceTypesToSearch) type of device
         * @param aDlgTitle title used in device selection dialog
         * @return CUpnpAVDevice* selected upnp av device.
         */
        CUpnpAVDevice* SelectDeviceL( 
                TUPnPDeviceTypesToSearch aDeviceType,
                TUPnPDialogTitle aDlgTitle );
        
        /**
         * Method for releasing upnp resource.
         *
         * @since S60 3.1
         * @param none
         * @return none
         */
        void ReleaseUpnpResourceL();
        
        /**
         * Method for playing a local file
         *
         * @since S60 3.1
         * @param none
         * @return none
         */
        void InitPlayL();
        
        /**
         * Method for playing a local video
         *
         * @since S60 3.1
         * @param aFilePath file path
         * @return none
         */
        void StartVideoPlayL( const TDesC& aFileName );
        
        /**
         * Shows info note after the file transfer has been completed.
         * 
         * @since S60 5.1
         * @param aMode Type of file transfer (copy or move)
         * @param aCount Number of files copied to server
         * @param aDevice Device files were copied to
         * @return none
         */
        void ShowTransferInfoNoteL( 
                TUPnPAiwEngineTransferMode aMode,
                TInt aCount, 
                const CUpnpAVDevice& aServerName ) const;

        /**
         * creates upnpitem using helper resolve local item
         * 
         * @since S60 5.1
         * @param aFileName 
         */
        void CreateUpnpItemAndSetUriL( const TDesC& aFileName );
        
        /**
         * Helper method to create MUPnPItemResolver.
         * 
         * @since S60 5.1
         * @param aFilePath file path
         * @return Pointer to resolver instance that is put to cleanupstack.
         */
        MUPnPItemResolver* CreateResolverLC( const TDesC& aFilePath );
        
        /**
         * Static cleanup function for MUPnPItemResolver.
         * Method makes sure that MUPnPItemResolver is properly cleaned up.
         * 
         * @since S60 5.1
         * @param aResolver TAny pointer to CUPnPAiwEngine object.
         */
        static void CleanupResolver( TAny* aAiwEngine );

        /**
         * starts asynchronous CActiveSchedulerWait
         */
        void Wait();
        
        /**
         * stops asynchronous CActiveSchedulerWait
         */
        void StopWait();
        
    private: // Construction methods

        // Constructor
        CUPnPAiwEngine();

        /**
         * ConstructL.
         */
        void ConstructL();
        
    private: // from MUPnPItemResolverObserver
        
        /**
         * see MUPnPItemResolverObserver
         */
        void ResolveComplete( 
                const MUPnPItemResolver& aResolver,
                TInt aError );

    private: // Data members

        // State of the engine
        TUPnPEngineState iEngineState;

        // Reference to the AV Controller ECom plug-in
        MUPnPAVController* iAVController;                   // owned

        // Reference to the AIW Provider instance
        MUPnPAiwEngineObserver* iObserver;                  // not owned

        // File sharing activation wait note and related member vars
        CAknWaitDialog* iFileSharingActivationWaitNote;     // owned;
        TBool iLocalFileSharingActivated;
        TInt iFileSharingError;

        // CoeEnv and the resource offset (needed when loading and
        // unloading resources)
        CEikonEnv* iCoeEnv;                                 // not owned
        TInt iResFileOffset;

        // Wait note dialog
        CAknWaitDialog* iWaitNoteDialog;                    // owned

        // Timer
        CUPnPAiwTimer* iUPnPAiwTimer;                       // owned

        MUPnPAVRenderingSession* iRenderingSession;         // owned
        
        CUPnPCommonUI* iCommonUI;                            //owned 
        HBufC16*       iFileName;               

        TBool   iRenderingSessionInUse;
        TBool   iVideoPlay;
        TBool    iVideoPlayCancel;
        TBool   iVideoPlayWait;
    private: // Singleton data members

         // Counter, which maintains the number of users of this singleton
        TInt iNumberOfUsers;
        
        /**
         * For async operations
         */
        CActiveSchedulerWait             iWait;
        
        MUPnPItemResolver*               iResolver;
        
        TInt                             iResolveResult;
    };

#endif // UPNP_AIW_ENGINE_H

// End of file
