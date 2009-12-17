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
* Description:  Header file for the Common UI class
*
*/


#ifndef CUPNPCOMMONUI_H
#define CUPNPCOMMONUI_H

// INCLUDES
#include <eikspmod.h>
#include <eikclb.h>
#include <aknview.h>

// CONSTANTS

// Application UID's needed in code
const TInt KMediaGalleryUID3 = { 0x101F8599 };

enum TUPnPBrowseActionIds
    {
    EUPnPBrowseOpen = 1,
    EUPnPBrowseFind,
    EUPnPBrowseShowExt,
    EUPnPBrowsePlayExt,
    EUPnPBrowseCopy
    };

enum TUPnPBrowseIncomingActionId
    {
    EUPnPSelectContainer = 1,
    EUPnPBrowseFolders
    };

enum TUPnPDialogTitle
    {
    EUPnPSelectDeviceTitle = 1, /* "Select device:" */
    EUPnPSelectFolderTitle,     /* "Select folder:" */
    EUPnPCopyToTitle,           /* "Copy to:" */
    EUPnPMoveToTitle            /* "Move to: */ 
    };

enum TUPnPDeviceTypesToSearch
    {
    EUPnPSearchAllDevices = 1,
    EUPnPSearchAllServerDevices,
    EUPnPSearchServerDevicesWithCopyCapability,
    EUPnPSearchServerDevicesWithSearchCapability,
    EUPnPSearchAllRenderingDevices,
    EUPnPSearchRenderingDevicesWithImageCapability,
    EUPnPSearchRenderingDevicesWithVideoCapability,
    EUPnPSearchRenderingDevicesWithImageAndVideoCapability,
    EUPnPSearchRenderingDevicesWithAudioCapability
    };

enum TUPnPPopUpSoftkey
    {
    EUPnPSoftkeyCopy = 1,
    EUPnPSoftkeyMove,
    EUPnPSoftkeySelect
    };
    
// FORWARD DECLARATIONS
class CAknViewAppUi;
class CUPnPExternalDeviceDialog;
class CUPnPBrowseDialog;
class CUPnPAdvancedFindDialog;
class CUPnPVideoPlayerDlg;
class CUPnPSelectionDialog;
class MUPnPAVController;
class MUPnPAVRenderingSession;
class MUPnPAVBrowsingSession;
class CUPnPPlayListFiller;
class CUPnPMusicAdapter;

class CUpnpObject;
class CUpnpAVDevice;
class CUpnpContainer;




// CLASS DECLARATION

/**
*  CUPnPCommonUI class
*
*  Collection UI class.
*/
class CUPnPCommonUI : public CBase
    {
    public:
    
        enum TUPnPAction
        {
        EUPnPNone = 0,
        EUPnPBrowse,
        EUPnPSearch,
        EUPnPCopy,
        EUPnPShow
        };
    
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUPnPCommonUI* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUPnPCommonUI();

    public: // Business logic methods

        /**
        * Executes device selection dialog
        * 
        * @since Series 60 3.1
        * @param aAVControl (MUPnPAVController&) controller for remote devices
        * @return TInt, Error code
        */
        IMPORT_C TInt ExecuteDeviceDialogL( MUPnPAVController& aAVControl );

        /**
        * Executes browse dialog
        * 
        * @since Series 60 3.1
        * @param aAVControl (MUPnPAVController&) controller for remote devices
        * @return TInt, Error code
        */
        IMPORT_C TInt ExecuteBrowseDialogL( MUPnPAVController& aAVControl,
                                            const CUpnpAVDevice& aDevice );

        /**
        * Executes video player
        * 
        * @since Series 60 3.1
        * @param aRenderingSession (MUPnPAVRenderingSession& )
        *        the rendering session to used for the playback
        * @param aObject (CUpnpObject&) the video item to be played
        * @return TInt, Error code
        */
        IMPORT_C TInt ExecuteVideoPlayerL(
                                MUPnPAVRenderingSession& aRenderingSession,
                                const CUpnpObject& aObject );
        
        /**
        * Executes music player
        * 
        * @since Series 60 3.1
        * @param aAVControl UPnPAVControl reference
        * @param aFiller Object that contains playlist information
        * @param aRenderer the selected renderer device
        * @return TInt error code
        */
        IMPORT_C TInt ExecuteMusicPlayerL( MUPnPAVController& aAVControl,
                                  CUPnPPlayListFiller* aFiller,
                                  const CUpnpAVDevice* aTargetDevice );
                        
        /**
        * Displays a UPnP device selection pop-up dialog.
        *
        * @since Series 60 3.1
        * @param aAVControl (MUPnPAVController&) reference to a 
        *                   CUPnPAVControl
        * @param aDevice (CUpnpAVDevice&) reference to the device
        * @param aType (TUPnPDeviceTypesToSearch), type of device that is
        *        requested
        * @param aTitle (TUPnPDialogTitle) the title for the dialog
        * @return TInt exiting reason
        */
        IMPORT_C TInt SelectDeviceL( MUPnPAVController& aAVControl,
                                     CUpnpAVDevice& aDevice,
                                     TUPnPDeviceTypesToSearch aType,
                                     TUPnPDialogTitle aTitle );

        /**
        * Dismiss dialog
        * when media server disappears
        *
        * @since Series 60 3.1
        * @param aError exit error
        * 
        * @return None
        */                          
        IMPORT_C void DismissDialogL( TInt aError );

        /**
        * Displays an Connection failed error note needed in aiw engine.
        *
        * @since Series 60 3.1
        */
        IMPORT_C void DisplayConnectionErrorNoteL();

        /**
        * Displays an Connection lost error note if copying files fails.
        *
        * @since Series 60 3.2.3
        */
        IMPORT_C void DisplayConnectionLostCopyErrorNoteL();

    public: // Common API internal interface

        /**
        * Executes the Advanced Find Dialog.
        *
        * @since Series 60 3.1
        * @param aAVControl (MUPnPAVController&) reference to AVController
        * @param aBrowsingSession (MUPnPAVBrowsingSession&) reference to the
        *        rendering session
        * @return TInt the status
        */
        TInt ExecuteAdvFindDialogL( MUPnPAVController& aAVControl,
            MUPnPAVBrowsingSession& aBrowsingSession );
        
        /**
        * Display a error message
        *
        * @since Series 60 3.1
        * @param aResource The string from the resource file
        * @param aMaxNumberOfResultsShown The integer filled up in the string
        * @return None
        */
        void DisplayErrorTextL( TInt aResource, 
                                TInt aMaxNumberOfResultsShown );

        /**
        * Displays error note
        *
        * @since Series 60 3.1
        * @param TInt, note resource
        * @return None
        */
        void DisplayErrorTextL( TInt aResource );
        
        /**
         * Displays error note
         *
         * @since Series 60 3.1
         * @param TInt, note resource
         * @param aInfo, note resource
         * @return None
         */
        void DisplayErrorTextL( TInt aResource, const TDesC& aInfo );
        
        
        /**
        * Displays Information text
        *
        * @since Series 60 3.1
        * @param TInt, note resource
        * @param TInt, number of files copied
        * @return None
        */
        void DisplayInfoTextL( TInt aResource,
                               TInt aNumberOfCopy );
        
        /**
        * Displays Information text
        *
        * @since Series 60 3.1
        * @param TInt, note resource
        * @return None
        */                       
        void DisplayInfoTextL( TInt aResource );
                                
        /**
        * Return number of dialogs created in CommonUI
        *
        * @since Series 60 3.1
        * @param none
        * @return any of dialog created
        */                          
        TBool PresenceOfDialog();
        
        /**
        * Handle common error code from other dialogs
        *
        * @since Series 60 3.1
        * @param aError error code
        * @return none
        */                          
        void HandleCommonErrorL( TInt aError,
                                 TInt aNumCopyItem );
        
        /**
        * Get current upnp action
        *
        * @since Series 60 3.1
        * @param aAction current upnp action
        * @return none
        */                         
        void GetUpnpAction( TUPnPAction aAction );
        
    private:

        /**
        * C++ default constructor
        */
        CUPnPCommonUI();

        /**
        * EPOC default constructor.
        */
        void ConstructL();
        
        /**
         * Get Copy location
         * @since Series 60 3.1
         * @param aLocation current copy loaction
         */
        void GetCopyLocationL( TDes& aLocation ) const;

    private: // Data

        CAknViewAppUi*              iAppUi;                     // Not owned
        CUPnPExternalDeviceDialog*  iExternalDeviceSelection;   // Not owned
        CUPnPBrowseDialog*          iBrowseSelection; //not owned
        CUPnPAdvancedFindDialog*    iAdvFindDialog; //not owned
        CUPnPVideoPlayerDlg*        iVideoPlayerDialog; //not owned
        CUPnPSelectionDialog*         iDeviceSelection; //not owned
        TInt                        iResFileOffset;
        CEikonEnv*                  iCoeEnv; //not owned
        CUPnPMusicAdapter*          iMusicAdapter; //owned
        CUPnPCommonUI::TUPnPAction  iAction;
        
        /**
         * Flag the FeatureManager is initialized or not
         */
        TBool                       iFeatureManagerInitialized;
    };

#endif  // CUPNPCOMMONUI_H

// End of File
