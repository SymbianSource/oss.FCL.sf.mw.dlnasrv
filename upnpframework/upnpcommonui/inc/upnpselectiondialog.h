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
* Description:  Header file for the UPnPSelectionDialog class implementation
*
*/


#ifndef __UPNP_SELECTION_DIALOG_H__
#define __UPNP_SELECTION_DIALOG_H__

// INCLUDES
#include "upnpavdeviceobserver.h"
#include "upnpcommonui.h"

// FORWARD DECLARATIONS
class MUPnPAVController;
class CUpnpAVDeviceList;
/**
* Device selection dialog class of Common UI library
*/
NONSHARABLE_CLASS( CUPnPSelectionDialog ): public CBase, 
                                           public MUPnPAVDeviceObserver
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUPnPSelectionDialog* NewL(MUPnPAVController& aAVControl);
        /**
        * Destructor.
        */
        virtual ~CUPnPSelectionDialog();

    public: // Methods from MUPnPAVDeviceObserver
        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since Series 60 3.1
         * @param CUpnpAVDevice
         * @return None
         */
        void UPnPDeviceDiscovered(const CUpnpAVDevice& aDevice);

        /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since Series 60 3.1
         * @param CUpnpAVDevice
         * @return None
         */
        void UPnPDeviceDisappeared(const CUpnpAVDevice& aDevice);

        /**
         * Notifies that the WLAN connection has been lost. All sessions
         * are now usable and must be closed.
         *
         * @since Series 60 3.1
         * @return None
         */ 
        void WLANConnectionLost();
        
    public:

        /**
         * Starts selection popup
         *
         * @since Series 60 3.1
         * @param CUpnpAVDevice, selected device
         */
        TInt StartPopupL(CUpnpAVDevice& aDevice);

        /**
         * Creates a selection popup.
         *
         * @since Series 60 3.1
         * @param const TDesC&, popup title
         * @param aDeviceType TUPnPDeviceTypesToSearch type of devices to
         *        search
         * @return None
         */
        void CreatePopupL( const TDesC& aTitle,
                           TUPnPDeviceTypesToSearch aDeviceType );
        
        /**
         * Destroy the selection dialog rather than pressing "Cancel" button
         *
         * @since Series 60 3.1
         * @param TInt exiting error 
         * @return None
         */
        void DismissItself( TInt aError );
        
    private:

        /**
        * C++ default constructor.
        */
        CUPnPSelectionDialog(MUPnPAVController& aAVControl);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // New Functions

        /**
        * Loads a possibly skinned icon and adds it to icon array
        * @since Series 60 3.1
        * @param CAknIconArray, array of icons
        * @param MAknsSkinInstance, skin instance
        * @param TDesC, reference to icon file
        * @param TAknsItemID, skinned icon id
        * @param TInt, bitmap id
        * @param TInt, bitmap mask id
        */
        void AppendIconToArrayL(CAknIconArray* aArray,
                                MAknsSkinInstance* aSkin,
                                const TDesC& aMbmFile,
                                const TAknsItemID& aID,
                                TInt aBitmapId,
                                TInt aMaskId);

        /**
        * Loads listbox items
        * @since Series 60 3.1
        */
        void LoadListItemsL(void);

        /**
        * From CAknDialog, handles layout initialization
        * @param const TDesC&, dialog title
        */
        void PreLayoutDynInitL(const TDesC& aTitle);

        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since Series 60 3.1
         * @param CUpnpAVDevice, new device
         * @return None
         */
        void UPnPDeviceDiscoveredL(const CUpnpAVDevice& aDevice);

        /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since Series 60 3.1
         * @param CUpnpAVDevice, disappeared device
         * @return None
         */
        void UPnPDeviceDisappearedL(const CUpnpAVDevice& aDevice);

        /**
         * Checks if a given device matches with the search criteria.
         *
         * @since Series 60 3.1
         * @param aDevice (CUpnpAVDevice*) the device
         * @return TBool
         */
        TBool MatchWithSearchCriteria( CUpnpAVDevice *aDevice );
        
        /**
         * Updates command set of the dialog by the search type of the devices.
         * 
         * @since Series S60 5.1
         * @param none
         * @return none
         */
        void UpdateCommandSetL();

    private:

        CAknPopupList*                              iPopup; //not owned
        CAknSingleGraphicPopupMenuStyleListBox*     iListBox; //owned
        CUpnpAVDeviceList*                          iDeviceArray; //owned
        

        MUPnPAVController&                          iAVControl;
        TUPnPDeviceTypesToSearch                    iTypeOfDevicesToSearch;
        //observer for device appearance
        MUPnPAVDeviceObserver*                      iDeviceObserver;
        
        TInt                                         iExitReason;
    };

#endif // __UPNP_SELECTION_DIALOG_H__

// End of file
