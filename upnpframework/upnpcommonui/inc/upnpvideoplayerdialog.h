/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for UPnP Video player class implementation
*
*/


#ifndef __UPNP_VIDEOPLAYER_DIALOG_H__
#define __UPNP_VIDEOPLAYER_DIALOG_H__

// INCLUDES
#include <aknViewAppUi.h>
#include "upnpavrenderingsessionobserver.h"
#include<remconcoreapitargetobserver.h>    //for MRemConCoreApiTargetObserver

// FORWARD DECLARATIONS
class CCustomCtrlDlgCustomControl;
class MUPnPAVRenderingSession;
class MUPnPAVController;
class CUpnpObject;
class CUpnpAVDevice;
class CUPnPCommonUI;
class MProgressDialogCallback;
class CEikLabel;

class CAknVolumePopup;
class CRemConCoreApiTarget;
class CRemConInterfaceSelector;
class CUPnPPeriodic;

/**
* Video player class of Common UI library
*/
NONSHARABLE_CLASS( CUPnPVideoPlayerDlg ) : public CAknDialog,
                                    public MUPnPAVRenderingSessionObserver,
                                    public MProgressDialogCallback
                                  , public MRemConCoreApiTargetObserver
    {

    public:  // Constructors and destructor

        /**
        * C++ default constructors.
        */
        CUPnPVideoPlayerDlg( MUPnPAVRenderingSession& aRenderingSession,
                             const CUpnpObject& aObject,
                             CUPnPCommonUI& aCommonUI );

        
        /**
        * Two-phased constructors.
        */
        static CUPnPVideoPlayerDlg* NewL(
                            MUPnPAVRenderingSession& aRenderingSession,
                            const CUpnpObject& aObject,
                            CUPnPCommonUI& aCommonUI );

        /**
        * Destructor.
        */
        virtual ~CUPnPVideoPlayerDlg();

    public: // Methods from MUPnPAVRenderingSessionObserver

        /**
         * UPnP AV Controller calls this method to return the result for the
         * 'get volume' request. Parameter contains the volume level of the
         * media renderer device to which the 'get volume' request was sent.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @param aVolumeLevel TInt volume level (between 0 - 100)
         * @param aActionResponse EFalse if caused by pressing hardware key
         *                        ETrue if caused by rendering session
         */
        void VolumeResult( TInt aError, TInt aVolumeLevel, 
                           TBool aActionResponse );

        /**
         * UPnP AV Controller calls this method to indicate that the requested
         * interaction operation (play, stop, etc.) is complete. In other
         * words, the target rendering device has changed it's state
         * accordingly.
         *
         * @since Series 60 3.1
         * @param aErrorCode TInt error code
         * @param aOperation TInt operation (TAVInteractOperation)
         * @return None
         */
        void InteractOperationComplete(
                            TInt aErrorCode,
                            TUPnPAVInteractOperation aOperation );

        /**
         * UPnP AV Controller calls this method to return the result for the
         * 'get mute' request. Parameter contains the state of the mute of the
         * media renderer device to which the 'get mute' request was sent.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @param aMute TBool the state of the mute (ETrue or EFalse)
         * @param aActionResponse EFalse if caused by pressing hardware key
         *                        ETrue if caused by rendering session
         */
        void MuteResult( TInt aError, TBool aMute, TBool aActionResponse );

        /**
         * UPnP AV Controller calls this method as a response to Position
         * Info action. The current position and the total length of the
         * track that is currently playing is returned. The results are in
         * (hh:mm:ss) format.
         *
         * UPnPAVController releases the memory allocated for aTrackPosition
         * and aTrackLength.
         *
         * @since Series 60 3.1
         * @param aStatus TInt error code
         * @param aTrackPosition TDesC8& track position
         * @param aTrackLength TDesC8& track length
         */
        void PositionInfoResult( TInt aError,
                                 const TDesC8& aTrackPosition,
                                 const TDesC8& aTrackLength );

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
         * UPnP AV Controller calls this method to return the result for the
         * 'set next uri' request.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @return None
         */
        void SetNextURIResult( TInt /*aError*/ ){};
       
       /**
         * Notifies that the Media Renderer we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @return None
         */  
        void MediaRendererDisappeared(
            TUPnPDeviceDisconnectedReason aReason );
        
        /**
         * Notifies that the Local Media Server has been put on sharing state
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */
        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ){};
       
    public: // Functions from base classes

        /**
        * CreateCustomControlL
        *
        * Draw this CUPnPVideoPlayerDlg to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * From CAknDialog, handles key events.
        * @param TKeyEvent, Event to handled.
        * @param TEventCode, Type of the key event.
        * @return TKeyResponse, response code (EKeyWasConsumed,
        *         EKeyWasNotConsumed).
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        
    public:  //new function
        
        /**
         * Called from CommonUI to destroy the video dialog itself
         * both in single Media server disappears or WLAN lost case
         * @since Series 60 3.1
         * @param aError exit reason (KErrSessionClosed and
         *                            KErrDisconnected)
         * @return None
         */   
        void DismissItselfL( TInt aError );
        
        /**
         * Handle different errors
         * @since Series 60 3.1
         * @param exiting reason
         * @return none
         */
        void HandleErrorL( TInt aError );
      
    protected: // Functions from base classes

        /**
        * From CoeControl
        * @param None
        * @return None
        */
        void HandleResourceChange(TInt aType);

        /**
        * From CAknDialog
        * @param None
        * @return None
        */
        void SizeChanged();

    protected:  // Functions from base classes

        /**
        * From CEikDialog, handles layout initialization
        * @param None
        * @return None
        */
        void PreLayoutDynInitL();

        /**
        * From CAknDialog, handles focus changes of the dialog
        * @param aButtonId, pressed button id
        * @return True if ready to close dialog, False otherwise
        */
        TBool OkToExitL( TInt aButtonId );

    private:    // New Functions

        /**
        * Updates navi pane
        *
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void UpdateNaviPaneL( );

        /**
        * Check the status pane capabilities.
        *
        * @since Series 60 3.1
        * @param aPaneId Status pane Id.
        * @return indication of success
        */
        TBool CheckStatusPaneCapabilities( const TPaneId& aPaneId );

        /**
        * Set navigation label from descriptor.
        *
        * @since Series 60 3.1
        * @param aText Text to be displayed on the navi label.
        * @return none
        */
        void SetNaviLabelL( const TDesC& aText );

        
        /**
         * UPnP AV Controller calls this method to indicate that the requested
         * interaction operation (play, stop, etc.) is complete. In other
         * words, the target rendering device has changed it's state
         * accordingly.
         *
         * @since Series 60 3.1
         * @param aErrorCode TInt error code
         * @param aOperation TInt operation (TAVInteractOperation)
         * @return None
         */
        void InteractOperationCompleteL(
                            TInt aErrorCode,
                            TUPnPAVInteractOperation aOperation );
                            
        /**
         * UPnP AV Controller calls this method to return the result for the
         * 'set uri' request.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @return None
         */
        void SetURIResultL( TInt aError );
        
        /**
         * For set cba string, encapsulation it in this function.
         *
         * @since Series 60 3.2
         * @param None
         * @return None
         */
        void ContinueAfterPauseL();
                        
    private:

        /**
        * 2nd phase constructors.
        */
        void ConstructL();

        /**
        * Clears dialog title
        *
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void ClearTitleL();

        /**
        * Updates softkey texts
        *
        * @since Series 60 3.1
        * @param TInt, state
        * @return none
        */
        void UpdateSoftkeysL(TInt aState);

        /**
        * Timer callback for volume indicator hiding.
        *
        * @since Series 60 3.1
        * @return TInt, error code
        */
        static TInt HideVolumeIdicator(TAny* aPtr);

        /**
        * Shows volume indicator
        *
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void ShowVolumeIndicatorL();

        /**
        * Timer callback
        *
        * @since Series 60 3.1
        * @param TAny*, caller
        * @return TInt
        */
        static TInt TimerIndicatorL(TAny* aPtr);

        /**
        * Resolves the target UPnP device capabilites
        * @since Series 60 3.1
        * @param None
        * @return none
        */
        void ResolveTargetDeviceCapabilitiesL();
        
        /**
        * Stop the video playback wait note
        * @since Series 60 3.1
        * @param None
        * @return none
        */
        void FinishWaitNoteL();
        
        /**
         * Prepare the video playback
         *
         * @since Series 60 3.1
         * @param None
         * @return None
         */
        void PreparePlayBackL();
        
        /**
         * Start wait note
         *
         * @since Series 60 3.1
         * @param None
         * @return An error code
         */
        TInt StartWaitNoteL();
        
        /**
         * Start displaying timer on the navi pane
         *
         * @since Series 60 3.1
         * @param None
         * @return None
         */
        void StartDisplayTimer();
        
        /**
        * Calls respective observer function
        *
        * @since Series 60 3.1
        * @param none
        * @return none
        */
        void DoChangeVolumeL();
        
        /**
        * Timer callback
        *
        * @since Series 60 3.1
        * @param TAny*, caller
        * @return TInt
        */
        static TInt ChangeVolume( TAny* aPtr );
        
    public: // Call back methods of MAknProgressDialogCallback

        /**
         * ProgressDialog call back method.
         * Get's called when a dialog is dismissed.
         *
         * @since S60 3.1
         * @param aButtonId (TInt) ID of the button pressed
         */
        void DialogDismissedL( TInt aButtonId );
    
    private:// From MRemConCoreApiTargetObserver
        
        
        /**
        * Side volume key API from MRemConCoreApiTargetObserver
        * @since 3.2
        * @see MRemConCoreApiTargetObserver
        */
        virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct );         
    private:    // Data

        enum TUPnPVideoStates
            {
            EUPnPVideoPause = 0x600,
            EUPnPVideoPlay,
            EUPnPVideoStop
            };

        CUPnPPeriodic*                  iTimer; //owned
        CAknNavigationDecorator*        iNaviDecorator; //owned
        CAknNavigationControlContainer* iNaviPane;             // Not owned
        
        MUPnPAVRenderingSession&        iRendSession; //not owned

        const CUpnpObject*              iItem; //not owned
        const CUpnpAVDevice*            iTargetDevice;         // Not owned
        const CUpnpAVDevice*            iSourceDevice;         // Not owned

        HBufC*                          iMinSecFormatString; //owned
        HBufC*                          iHourMinSecFormatString; //owned
        HBufC*                          iTimerNavi;
        TInt64                          iPlaybackPosInSeconds;
        TInt64                          iTotalLengthInSeconds;
        TInt                            iCurrentVolume;
        TInt                            iNewVolume;
        TInt                            iTimerRefresh;
        
        CCustomCtrlDlgCustomControl*    iContainer; //not owned
        
        //Volume adjust indicator
        TBool                           iAdjustingVolume;
        // Target device capabilites
        TBool                           iPauseCapability;
        TBool                           iVolumeCapability;
        // title text before video player
        HBufC*                          iOriginalTitleText; //owned
        
        CUPnPCommonUI&                  iCommonUI;
        
        TBool                           iPause; 
        
        // Flag if the remote renderer is playing and the elapsed time needs
        // be updated. This means that the renderer has started the playback 
        // already and it is not in paused state.
        TBool                           iRendererPlaying;
        
        // The time interval of getting GetPositionInfoL
        TInt                            iGetPositionInfoInterval;
        
        CUPnPCommonUI::TUPnPAction      iAction;
        
        CAknWaitDialog*                 iVideoPlaybackWaitNote; // owned;
        
        TInt                            iExitCode;
        
        //ETrue if volume is up, EFalse if volume is down
        TBool                           iVolumeUp;
           
        TBool                            iVolumeKeyPressHold;
        CUPnPPeriodic*                   iVolumeTimer; // owned
        CAknVolumePopup*                 iVolumePopup; // owned
        
        // owned by iInterfaceSelector
        CRemConCoreApiTarget*            iCoreTarget; 
        CRemConInterfaceSelector*        iInterfaceSelector; // owned
        
    };

//////////////////////////////////////////////////////////////////////////////
//
// -----> CImage (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CImage : public CCoeControl
    {
    public:

        /**
        * C++ default constructor.
        */
        CImage(MUPnPAVRenderingSession& aRendSession);

        //destructor
        virtual ~CImage();

    private: // Methods

        /**
        * Draws the display
        *
        * @param TRect&, Rectangle to be drawn
        * @return none
        */
        void Draw(const TRect& aRect) const;

        /**
        * Draws the display
        *
        * @param TRect&, Rectangle to be drawn
        * @return none
        */
        void DrawL(const TRect& aRect) const;

        /**
        * Draws the label
        *
        * @param TRect&, Rectangle to be drawn
        * @return none
        */
        void DrawLabelL(const TRect& aRect) const;

     private: // Data

        // Data members defined and used by this class.
        MUPnPAVRenderingSession&    iRendSession;
        CFbsBitmap*                 iBitmap; //owned
        CFbsBitmap*                 iMask; //owned
    };

//////////////////////////////////////////////////////////////////////////////
//
// -----> CCustomCtrlDlgCustomControl (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CCustomCtrlDlgCustomControl : public CCoeControl
    {

    public:

        /**
        * Construct From Resource.
        */
        void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * Sets AV controller
        *
        * @since Series 60 3.1
        * @param MUPnPAVRenderingSession&, AV controller
        * @return none
        */
        void SetRenderingSession( MUPnPAVRenderingSession& aAVRendSession );

        //destructor
        virtual ~CCustomCtrlDlgCustomControl();

    private:

        /**
        * Draws the display
        *
        * @param TRect&, Rectangle to be drawn
        * @return none
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CAknDialog
        * @param none
        * @return none
        */
        void SizeChanged();

        /**
        * From CAknDialog
        * @param none
        * @return the number of component controls contained by this control
        */
        TInt CountComponentControls() const;

        /**
        * From CAknDialog
        * @param the index of the control to get
        * @return the component control with an index of aIndex
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

    private:

        // Data members defined and used by this class.
        CImage*                     iImage; //owned
        MUPnPAVRenderingSession*    iRendSession; //not owned
        
    };

#endif // __UPNP_VIDEOPLAYER_DIALOG_H__
