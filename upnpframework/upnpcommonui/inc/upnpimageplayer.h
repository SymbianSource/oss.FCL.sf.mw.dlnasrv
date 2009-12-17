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
* Description:  Plays remote images on a renderer
*
*/


#ifndef C_CUPNPIMAGEPLAYER_H
#define C_CUPNPIMAGEPLAYER_H


#include <e32base.h>

#include <s32mem.h>
#include "upnpavrenderingsessionobserver.h"
#include "upnpcommonui.h"

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPAVRenderingSession;
class CUpnpAVDevice;
class CUpnpObject;
class MUPnPDeviceObserver;
class CUPnPCommonUI;


// CLASS DECLARATION

/**
*  UPnP AV Controller callback dispatcher
*  
*
*  @lib - 
*  @since Series 60 3.1
*/

NONSHARABLE_CLASS( CUpnpImagePlayer ) : public CBase, 
                                        public MUPnPAVRenderingSessionObserver
    {
    
    private:
        /**
         * Defines the image playing state
         */
         enum TPendingOperation
            {
            EImageIdle = 0,     //when no action on image has been set
            EImageInitialising, //image uri has been set, callback not called
            EImageShowing,     //image play has been set, callback not called
            EImageNext          //another image is acting when the previous
                                //one's action has not been completed
            };

    public:  // Constructors and destructor
            
        static CUpnpImagePlayer* NewL( MUPnPAVController& aAVControl,
                                       MUPnPDeviceObserver& aDialog,
                                       CUPnPCommonUI& aCommonUI );
       
        /**
        * Destructor.
        */
        virtual ~CUpnpImagePlayer();
        
    public:
        
        /**
         * Set the renderer to show the image
         * @since Series 60 3.1
         * @param aTargetDevice the selected renderer to show the image
         * @return none
         */
        void SetTargetDeviceL(const CUpnpAVDevice& aTargetDevice); 
        
        /**
         * Show the image
         * @since Series 60 3.1
         * @param the image to be shown
         * @return none
         */
        void PlayL(const CUpnpObject& aItem);
        
        /**
         * Stop showing the image
         * @since Series 60 3.1
         * @param none;
         * @return none
         */
        void Stop();
    
    
    protected: //Functions from MUPnPAVRenderingSessionObserver
        
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
         * @return None
         */
        void VolumeResult( TInt /*aError*/, TInt /*aVolumeLevel*/, 
                           TBool /*aActionResponse*/  ){};
        
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
         * @return None
         */
        void MuteResult( TInt /*aError*/, TBool /*aMute*/,
                         TBool /*aActionResponse*/ ){};
        
        /**
         * UPnP AV Controller calls this method to indicate that the
         * requested interaction operation (play, stop, etc.) is complete.
         * In other words, the target rendering device has changed it's
         * state accordingly.
         *
         * @since Series 60 3.1
         * @param aErrorCode TInt error code
         * @param aOperation TInt operation (TAVInteractOperation)
         * @return None
         */ 
        void InteractOperationComplete( 
                      TInt aError,
                      TUPnPAVInteractOperation aOperation 
                      );
        
        /**
         * UPnP AV Controller calls this method as a response to Position 
         * Info action. The current position and the total length of the 
         * track that is currently playing is returned. The results are in
         * (hh:mm:ss) format.
         *
         * UPnPAVController releases the memory allocated for aTrackPosition
         * and aTrackLength.
         *
         * @since Series 60 3.0
         * @param aStatus TInt error code
         * @param aTrackPosition TDesC8& track position
         * @param aTrackLength TDesC8& track length
         * @return None
         */
        void PositionInfoResult( 
                      TInt /*aError*/, 
                      const TDesC8& /*aTrackPosition*/, 
                      const TDesC8& /*aTrackLength*/ 
                      ){};
                      
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
        void SetNextURIResult( TInt /*aError*/){};
        
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
    
    private:

        /**
         * C++ default constructor.
         */
         CUpnpImagePlayer( MUPnPAVController& aAVControl,
                           MUPnPDeviceObserver& aDialog,
                           CUPnPCommonUI& aCommonUI );         
        /**
         * By default Symbian 2nd phase constructor is private.
         */
         void ConstructL(); 

    private:
        
        /**
         * UPnP AV Controller calls this method to indicate that the
         * requested interaction operation (play, stop, etc.) is complete.
         * In other words, the target rendering device has changed it's
         * state accordingly.
         *
         * @since Series 60 3.1
         * @param aErrorCode TInt error code
         * @param aOperation TInt operation (TAVInteractOperation)
         * @return None
         */ 
         void InteractOperationCompleteL( 
                      TInt aError,
                      TUPnPAVInteractOperation aOperation 
                      );
        
        /**
         * UPnP AV Controller calls this method to return the result for the
         * 'set uri' request.
         *
         * @since Series 60 3.1
         * @param aError error code
         * @return None
         */
         void SetURIResultL( TInt aError );
        
    private:

        MUPnPAVRenderingSession*            iRendSession; //owned
        MUPnPAVController&                  iAVControl;   //not owned
        const CUpnpAVDevice*                iTargetDevice;    //not owned
        
        TPendingOperation                   iImageState;
        
        HBufC8*                             iUri;  //OWNED
        
        const CUpnpItem*                    iItem;  //not owned
        
        MUPnPDeviceObserver&                iDialog; //not owned
        
        CUPnPCommonUI&                      iCommonUI; //Not owned
        
        TBool                               iFirstStart; 
        
        CUPnPCommonUI::TUPnPAction          iAction;
    };


#endif // C_CUPNPIMAGEPLAYER_H
