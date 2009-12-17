/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the Setup wizard class
*
*/




#ifndef CMSAPPWIZARD
#define CMSAPPWIZARD

// INCLUDES
#include <aknmessagequerydialog.h>
#include <AknWaitDialog.h>
#include "upnpavdeviceobserver.h"


// FORWARD DECLARATIONS
class CCmMediaServerFull;
class CMSEngine;
class CMSSelectionStorage;
class CMSMultiselectionPopup;
class MUPnPAVController;

// CLASS DECLARATION

/**
  * CMSAppWizard class
  * Class implementing the functionality of the setup wizard
  *
  * @since S60 5.1
  *
  * @lib msappwizard.lib
  */
NONSHARABLE_CLASS( CMSAppWizard ) : public CBase,
                                    public MUPnPAVDeviceObserver,
                                    public MProgressDialogCallback
    {

    // Wizard steps
    enum TWizardStep
        {
        EStep1 = 0,
        EStep2,
        EStep3,
        EStep4,
        EStep5,
        EStep6,
        EStep7,
        EStep8,
        EStep9,
        EStep10,
        EStep11,
        EStep12,
        ELastStep,
        EWizardEnd,
        ECancelWizard
        };

    public:     // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine
         * @return CMSAppWizard pointer
         */
        IMPORT_C static CMSAppWizard* NewL( CMSEngine& aMSEngine );

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine
         * @return CMSAppWizard pointer
         */
        IMPORT_C static CMSAppWizard* NewLC( CMSEngine& aMSEngine );        

        /**
         * Destructor.
         */
        virtual ~CMSAppWizard();


    public:

        /**
         * Starts wizard
         * @since S60 5.1
         * @return TInt, sk button, 0 if cancelled
         */
        IMPORT_C TInt StartL();



    public: // Methods from MProgressDialogCallback

        /**
         * Handles the situation when the dialog
         * is dismissed.
         *
         * @since S60 5.1
         * @param aButtonId The identifier of the button, with which the
         *                  dialog was dismissed.
         * @return None
         */
        void DialogDismissedL( TInt aButtonId );


    private:

        /**
         * Show the information note with OK BACK buttons
         *
         * @since S60 5.1
         * @param aMain, resource ID
         * @param aHeading, heading text
         * @return keycode of the button user has pressed.
         */
        TInt ShowInfoStepL( TInt aMain, TPtr aHeading );

        /**
         * Show the UI dialog with selection list
         *
         * @since S60 5.1
         * @param aMain, resource ID
         * @param aHeading, heading text
         * @return keycode of the button user has pressed.
         */
        TInt ShowListStepL( TInt aMain, TPtr aHeading );

        /**
         * Shows the step for setting shared content
         *
         * @since S60 5.1
         * @param aItemArray, item array
         * @param aResultArray, selected indexes
         * @param aHeading, heading text
         * @return keycode of the button user has pressed.
         */
        TInt ShowMultiselectionStepL( CDesCArrayFlat& aItemArray,
                                      CArrayFix<TInt>& aResultArray,
                                      TPtr aHeading );

        /**
         * Determines the next wizard step
         *
         * @since S60 5.1
         * @param aSelection, forward / backward
         */
        void SetNextStep( const TInt aDirection );

        /**
         * Cancels device search operation.
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void CancelDeviceSearch();

        /**
         * Creates AV controller.
         *
         * @since S60 5.1
         * @param None
         * @return operation success. KErrNone if succesfull
         */
        TInt CreateAvControllerL();

        /**
         * Updates multiselection dialog content.
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void UpdateDevicePopupDlgL();

        /**
         * Show the UI dialog with selection list
         *
         * @since S60 5.1
         * @param aServer, server name
         * @param aServerIndex, server array index
         * @return comparison result.
         */
        TInt MatchFoundServer( TDesC8& aServer, TInt &aServerIndex );

        /**
         * Sets server state to deleted if needed
         * And updates server name
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void CheckAndSetServerStateL();

        /**
         * Fetches devices from AVController and calls 
         * DeviceDiscovered-callback for each of them.
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void FetchAlreadyDiscoveredDevicesL();
        
        /**
         * Close wait dialog
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void CloseWaitDialog();
        
        /**
         * Creates multiselection store list items according
         * to default store rule
         *
         * @since S60 5.1
         * @param None
         * @return CDesCArrayFlat, item array
         */
        CDesCArrayFlat* CreateMultiselectionStoreListItemsLC();

        /**
         * Creates multiselection store list items according
         * to UI spec string
         *
         * @since S60 5.1
         * @param None
         * @return CDesCArrayFlat, item array
         */
        CDesCArrayFlat* CreateMultiselectionUIStoreListStringLC();


        /**
         * Creates server name list for device selection dialog
         *
         * @since S60 5.1
         * @param aServerArray, server list
         * @return None
         */        
        void CreateServerNameListL(
                    RPointerArray<CCmMediaServerFull>& aServerArray);

        /**
         * Creates multiselection fill list items
         *
         * @since S60 5.1
         * @param None
         * @return CDesCArrayFlat, item array
         */        
        CDesCArrayFlat* CreateMultiselectionFillListItemsLC();

        /**
         * Shows wait note until first device appears
         *
         * @since S60 5.1
         * @param None
         * @return None
         */        
        TInt ShowWaitNoteL();

        /**
         * Shows info note
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void ShowInfoNoteL( TInt aBodyTextResource );
        
        /**
         * Adds device name to device name list
         *
         * @since S60 5.1
         * @param aDevice, upnp device
         * @return None         
         */
        void AddDeviceNameL( const CUpnpAVDevice& aDevice );

    public: // Functions from base classes

        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since S60 5.1
         * @param aDevice, upnp device
         * @return None
         */
        void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since S60 5.1
         * @param aDevice, upnp device
         * @return None
         */     
        void UPnPDeviceDiscoveredL( const CUpnpAVDevice& aDevice );

        /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since S60 5.1
         * @param aDevice, upnp device
         * @return None
         */
        void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );

                /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since S60 5.1
         * @param aDevice, upnp device
         * @return None
         */
        void UPnPDeviceDisappearedL( const CUpnpAVDevice& aDevice );

        /**
         * Notifies lost in WLAN connection
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void WLANConnectionLost();

        /**
         * Sets system update id to zero
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void ClearServersSystemUpdateId();

    private:

        /**
         * Performs the second phase construction.
         */
        void ConstructL();

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine
         * @return None
         */
        CMSAppWizard( CMSEngine& aMSEngine );

    private:

        /**
         * Resource file offset
         */
        TInt                                    iResFileOffset;
        /**
         * Control environment for file server access
         */
        CEikonEnv*                              iCoeEnv;
        /**
         * The position of the wizard
         */
        TInt                                    iStep;
        /**
         * Flag for indicating device search cancel
         */
        TBool                                   iUserCancelledSearch;
        /**
         * selection popup for category and device selection
         */
        CMSMultiselectionPopup*                 iMultiSelectionDlg; // owned
        /**
         * wait dialog
         */
        CAknWaitDialog*                         iWaitDialog;        // owned
        /**
         * wait sheduler
         */
        CActiveSchedulerWait                    iWait;              // owned
        /**
         * list of new servers
         */
        RPointerArray<CCmMediaServerFull>       iMediaServers;      // owned
        /**
         * list of previously stored servers
         */
        RPointerArray<CCmMediaServerFull>*      iOriginalServers;   // owned
        /**
         * list of upload capable servers
         */
        RPointerArray<CCmMediaServerFull>       iStoreServers;      // owned        
        /**
         * List of server names
         */
        CDesCArrayFlat*                         iDeviceNameArray;   // owned
        /**
         * The storage for the user selections
         */
        CMSSelectionStorage*                    iSelections;        // owned        
        /**
         * application engine
         */
        CMSEngine*                              iMSEngine;      // not owned  
        /**
         * Step count
         */
        TInt                                    iMaxSteps;
        /**
         * AV controller
         */
        MUPnPAVController*                      iAvController;      // owned
        /**
         * File server session
         */
        RFs                                     iFileSession;
        /**
         * found drives
         */
        RPointerArray<CCmDriveInfo>             iDriveArray;        // owned
        /**
         * Selected drives indexes
         */
        RArray<TInt>                            iDriveIndexes;
        /**
         * Flag for wlan lost 
         */
        TBool                                   iWLanLost;
        /**
         * Upload capable server found -flag
         */
        TBool                                   iUploadServerFound;
    };

#endif  // CMSAPPWIZARD

// End of File
