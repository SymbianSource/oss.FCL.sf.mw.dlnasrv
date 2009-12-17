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
* Description:  Definition for MediaServant Source devices setting item
*
*/


#ifndef C_MSSETTINGITEMSOURCEDEV_H
#define C_MSSETTINGITEMSOURCEDEV_H

// INCLUDES
#include <AknProgressDialog.h>
#include "upnpavdeviceobserver.h"
#include <AknWaitNoteWrapper.h> //MAknBackgroundProcess
#include "mssettingitems.h"

// FORWARD DECLARATIONS
class CMSMultiselectionPopup;
class MUPnPAVController;
class CMSEngine;

/**
 * CMSSettingItemDevices class.
 *
 * Class searches devices and shows them to user
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSSettingItemDevices ): public CMSTextSettingItem,
                             public MAknBackgroundProcess,
                             public MProgressDialogCallback,
                             public MUPnPAVDeviceObserver
    {

    public: // Constructor and destructor

        /**
         * NewL
         * 2-phase constructor.
         *
         * @since S60 v5.1
         * @param aIdentifier, setting item identifier
         * @param aText, setting item text
         * @param aMSEngine, application engine
         * @param aStoredServers, stored servers
         * @param aUploadCapabilitySupport, upload capability support
         * @return CMSSettingItemDevices*, the new CMSSettingItemDevices object
         */
        static CMSSettingItemDevices* NewL(
                           TInt aIdentifier,
                           TDes& aText,
                           CMSEngine& aMSEngine,
                           RPointerArray<CCmMediaServerFull>& aStoredServers,
                           TBool aUploadCapabilitySupport = EFalse );

        /**
         * NewLC
         * 2-phase constructor.
         *
         * @since S60 v5.1
         * @param aIdentifier, setting item identifier
         * @param aText, setting item text
         * @param aMSEngine, application engine
         * @param aStoredServers, stored servers
         * @param aUploadCapabilitySupport, upload capability support
         * @return CMSSettingItemDevices*, the new CMSSettingItemDevices object
         */
        static CMSSettingItemDevices* NewLC(
                           TInt aIdentifier,
                           TDes& aText,
                           CMSEngine& aMSEngine,
                           RPointerArray<CCmMediaServerFull>& aStoredServers,
                           TBool aUploadCapabilitySupport = EFalse );

        /**
         * Destructor.
         */
        virtual ~CMSSettingItemDevices();


    public:// from MAknBackgroundProcess

        /**
         * Completes one cycle of the process.
         */
        void StepL();

        /**
         * Return true when the process is done.
         */
        TBool IsProcessDone() const;

        /**
         * Handles error occurred in StepL.
         */
        TInt CycleError( TInt aError );

    protected: 
    
    // From base class CAknTextSettingItem

        /**
         * From CAknTextSettingItem
         * See base class definition
         */         
        void EditItemL( TBool aCalledFromMenu );
    
    // From base class MProgressDialogCallback         
       
       /**
        * From MProgressDialogCallback
        * See base class definition
        */
        void DialogDismissedL( TInt aButtonId );
    
    // From base class MUPnPAVDeviceObserver

        /**
         * From MUPnPAVDeviceObserver
         * See base class definition
         */
        void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

        /**
         * From MUPnPAVDeviceObserver
         * See base class definition
         */
        void UPnPDeviceDiscoveredL( const CUpnpAVDevice& aDevice );

        /**
         * From MUPnPAVDeviceObserver
         * See base class definition
         */
        void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );

        /**
         * From MUPnPAVDeviceObserver
         * See base class definition
         */
        void WLANConnectionLost();         

    private:

        /**
         * Shows error note
         * @since S60 5.1
         * @param aTextResource, error text resource
         */
        void ShowErrorNoteL( TInt aTextResource );

        /**
         * Sets servers activity
         * @since S60 5.1
         */
        void SetServersActivity();

        /**
         * Reads source devices and set item text
         * @since S60 5.1
         */
        void ReadServerListL();

        /**
         * Sets correct text for settings page
         * @since S60 5.1
         */
        void SetSettingItemTextL();

        /**
         * Returns number of selected servers in active servers list.
         * @since S60 5.1
         * @return number of selected servers
         */
        TInt CountSelectedItems() const;

        /**
         * Creates AV controller.
         * @since S60 5.1
         * @return operation success. KErrNone if succesfull
         */
        TInt CreateAvcontrollerL();

        /**
         * Cancels devices search operation.
         * @since S60 5.1
         */
        void CancelDeviceSearch();

        /**
         * Fetches devices from AVController and calls 
         * DeviceDiscovered-callback for each of them.
         * @since S60 5.1
         */
        void FetchAlreadyDiscoveredDevicesL();

        /**
         * Creates AVController, fetches devices from it and calls 
         * DeviceDiscovered-callback for each of them. Starts wait note if no 
         * devices are already discovered.
         *
         * @since S60 5.1
         * @return TInt system wide error code. KErrNone if successful.
         */
        TInt CreateAvcAndFetchDevicesL();        

        /**
         * Constructor.
         */
        CMSSettingItemDevices( TInt aIdentifier,
                               TDes& aText,
                               CMSEngine& aMSEngine,
                               RPointerArray<CCmMediaServerFull>& aStoredServers,
                               TBool aUploadCapabilitySupport );

        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();

    private:

        /**
         * The internal phases of wait note when creating AvC
         */
        enum TCreateAvCState
            {
            EPhaseNotActive,
            EPhaseProcessing,
            EPhaseCompleted
            };

        /**
         * Id list of selected servers
         */
        CArrayFix<TInt>*                    iSelectedServers;   // owned
        
        /**
         * Flag for indicating user cancel
         */
        TBool                               iUserCancelledSearch;
        
        /**
         * Device selection dialog
         */
        CMSMultiselectionPopup*             iDevSelectionDlg;   // owned
        
        /**
         * application engine
         */
        CMSEngine&                          iMSEngine;
        
        /**
         * Servers to be shown to user
         */
        RPointerArray<CCmMediaServerFull>   iServerList;        // not owned

        /**
         * searched servers list
         */
        RPointerArray<CCmMediaServerFull>&  iStoredServers;     // not owned
                
        /**
         * Indicates if upload capability is needed
         */
        TBool                               iUploadCapabilitySupport;

        /**
         * Server name string (external set)
         */
        HBufC*                              iSettingText;       // ownded
        
        /**
         * Wait dialog shown on device search
         */
        CAknWaitDialog*                     iWaitDialog;        // owned
        
        /**
         * Wait scheduler
         */
        CActiveSchedulerWait                iWait;
        
        /**
         * AV controller for device search
         */
        MUPnPAVController*                  iAvController;          // owned

        /**
         * Current state of creating AvC
         */
        TCreateAvCState                     iCreateAvCState;

    };

#endif // C_MSSETTINGITEMSOURCEDEV_H

// End of File
