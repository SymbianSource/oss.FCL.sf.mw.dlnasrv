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
* Description:  Engine for rendering images remotely
*
*/


#ifndef UPNP_IMAGERENDERINGENGINE_H
#define UPNP_IMAGERENDERINGENGINE_H

// INCLUDES
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h" // base class
#include "upnpitemresolverobserver.h" // base class

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPAVRenderingSession;
class MUpnpImageRenderingEngineObserver;
class MUPnPItemResolver;
class CUPnPPeriodic;

/**
* This class defines the UpnpShowTask used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpImageRenderingEngine
    : public CBase
    , public MUPnPItemResolverObserver
    , public MUPnPAVRenderingSessionObserver
    {

    public: // construction

        /**
         * static constructor
         * @param aAVController avcontroller resource
         * @param aSession the rendering session to work with
         * @param aObserver the client for this engine
         */
        static CUpnpImageRenderingEngine* NewL(
            MUPnPAVController& aAVController,
            MUPnPAVRenderingSession& aSession,
            MUpnpImageRenderingEngineObserver& aObserver);

        /**
         * destructor
         */
        virtual ~CUpnpImageRenderingEngine();

    private: // private part of construction

        /**
         * constructor
         */
        CUpnpImageRenderingEngine(
            MUPnPAVController& aAVController,
            MUPnPAVRenderingSession& aSession,
            MUpnpImageRenderingEngineObserver& aObserver);
        
     
        /**
         * Second phase constructor
         */
        void ConstructL();
        
        
        /**
         * Cleans up used resources
         */
        void Cleanup();

    public: // the interface

        /**
         * Requests to start rendering. This will cause the engine to query
         * the media to render using the callback interface.
         * This method can be called in all states and all conditions
         * and subsequently very fast. The engine will keep track of
         * states and indicate errors etc.
         */
        void PlayL();

        /**
         * Requests to stop rendering. This method can be called in all states.
         */
        void StopL();


    protected: // Call back methods of MUPnPAVRenderingSessionObserver

        /**
         * UPnP AV Controller calls this method to return the result for the
         * 'set uri' request.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @return None
         */
        void SetURIResult( TInt aError );

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
         * Notifies that the Media Renderer we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @param aReason (TUPnPDeviceDisconnectedReason) reason code
         */
        void MediaRendererDisappeared(
            TUPnPDeviceDisconnectedReason aReason );

        // Methods that are not used
        void VolumeResult(
                        TInt /*aError*/,
                        TInt /*aVolumeLevel*/,
                        TBool /*aActionResponse*/) {}
        void MuteResult(
                        TInt /*aError*/,
                        TBool /*aMute*/,
                        TBool /*aActionResponse*/ ) {}
        void PositionInfoResult(
                        TInt /*aError*/,
                        const TDesC8& /*aTrackPosition*/,
                        const TDesC8& /*aTrackLength*/ ) {}
        void SetNextURIResult(
                        TInt /*aError*/ ) {}
            
        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ) {}

    protected: // Methods from MUPnPItemResolverObserver

        /**
         * See UpnpAvControllerHelper API for more info.
         */
        void ResolveComplete(
            const MUPnPItemResolver& aResolver, TInt aError );

    public: // methods for the timer

        /**
         * the timeout callback
         */
        static TInt Timer( TAny* aArg );

        /**
         * handles the timeout internally
         */
        void RunTimerL();

        /**
         * handles errors in the timeout callback body
         */
        TInt RunError( TInt );
        
         /**
         * checks if wlan is active
         */       
        TBool IsWlanActive();
        
    private: // Private methods

        /**
         * Handles the initiation of rendering (SetUri or video player 
         * launching).
         *
         * @since S60 3.2
         */
        void InitiateShowingL();

        /**
         * Handles the start up of the item resolving.
         *
         * @since S60 3.2
         */
        void StartResolvingL();
        
        
        /**
         * Sends an acknowledgement
		 */
		void SendRenderAck( TInt aError );

    private: // Data members

        // avcontroller
        MUPnPAVController&          iAVController;
    
        // the rendering session
        MUPnPAVRenderingSession&    iRenderingSession;

        // The observer interface
        MUpnpImageRenderingEngineObserver&  iObserver;

        // internal states
        enum TRenderingState
            {
            EIdle = 100,            // doing nothing
            EResolvingItem,         // resolving (preparing the item to be played)
            EResolveComplete,       // resolve done succesfully. Starting to play
            EStopping,              // calling Stop (for previoysly playing item)
            ESettingUri,            // calling SetAVTransportURI
            EStartingPlay,          // calling Play
            EPlaying,               // play OK, just displaying the image :-)
            EShuttingDown
            };

        // internal state
        TRenderingState             iState;

        // current resolver. owned.
        MUPnPItemResolver*          iCurrentResolver;

        // Buffered next resolver. Owned.
        MUPnPItemResolver*          iBufferedResolver;

        // flag that indicates a new image is in buffer
        TBool                       iBufferingNewImage;

		// timer support
		CUPnPPeriodic*              iTimer;
		
		TBool                       iWlanActive;

	};


#endif // UPNP_IMAGERENDERINGENGINE_H

// End of File
