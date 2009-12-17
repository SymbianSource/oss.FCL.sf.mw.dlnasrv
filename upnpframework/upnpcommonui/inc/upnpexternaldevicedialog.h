/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __UPNP_EXTERNAL_DEVICE_DIALOG_H__
#define __UPNP_EXTERNAL_DEVICE_DIALOG_H__

// INCLUDES
#include <aknselectionlist.h>
#include <aknnavide.h> // CAknNavigationDecorator
#include "upnpavdeviceobserver.h"
#include "upnpnavipanecontainer.h"

// FORWARD DECLARATIONS
class CAknIconArray;
class MAknsSkinInstance;
class CAknTitlePane;
class CUPnPCommonUI;
class CAknNavigationDecorator;    
class MUPnPAVController;

class CUpnpItem;
class CUpnpAVDeviceList;
/**
* External device dialog class of Common UI library
*/
NONSHARABLE_CLASS( CUPnPExternalDeviceDialog ) : public CAknDialog,
                                                 public MUPnPAVDeviceObserver
    {
    private:
        /**
        * C++ default constructor
        */
        CUPnPExternalDeviceDialog( MUPnPAVController& aAVControl,
                                   CUPnPCommonUI& aCommonUI );

        /**
        * EPOC default constructor.
        */
        void ConstructL(TInt aMenuResource );


    public: // Methods from MUPnPAVDeviceObserver
        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since Series 60 3.1
         * @param CUpnpDevice
         * @return None
         */
        void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

        /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since Series 60 3.1
         * @param CUpnpDevice
         * @return None
         */
        void UPnPDeviceDisappeared (const CUpnpAVDevice& aDevice );
        
        /**
         * Notifies that the WLAN connection has been lost. All sessions
         * are now usable and must be closed.
         *
         * @since Series 60 3.1
         * @return None
         */ 
        void WLANConnectionLost();
        
    protected:  // Functions from base classes

        /**
        * From CAknDialog, handles layout initialization
        * @since Series 60 3.1
        * @return none
        */
        void PreLayoutDynInitL();
        
        /**
        * From CEikdialog This function is called by the EIKON dialog 
        * framework just before the dialog is activated, after it 
        *has called PreLayoutDynInitL() and the dialog has been sized. 
        * 
        * @param none
        * @since Series 60 3.1
        * @return none
        */
        void PostLayoutDynInitL();

        /**
        * From CAknDialog, handles focus changes of the dialog
        * @since Series 60 3.1
        * @param TInt, pressed button id
        * @return True if ready to close dialog, False otherwise
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CAknDialog, dynamically modifies menu
        * @since Series 60 3.1
        * @param TInt, menu resource
        * @param CEikMenuPane, menu pane resource
        */
        void DynInitMenuPaneL (TInt aResourceId, CEikMenuPane *aMenuPane);

        /**
        * From CAknDialog, handles menu commands
        * @since Series 60 3.1
        * @param TInt, command to be handled
        */
        void ProcessCommandL (TInt aCommand);

        /**
        * From CoeControl
        */
        void HandleResourceChange(TInt aType);


    public:

        /**
        * Two-phased constructor.
        * @param TInt, menu resource used with the dialog
        * @param CUPnPAVControl&, AV controller
        * @param CUPnPCommonUI&, pointer to common ui
        */
        static CUPnPExternalDeviceDialog* NewL(
            TInt aMenuResource,
            MUPnPAVController& aAVControl,
            CUPnPCommonUI& aCommonUI);

        // Destructor
        virtual ~CUPnPExternalDeviceDialog(); // destruct and give statistics


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
        * Display Media Server Names.
        * @since Series 60 3.1
        */
        void DisplayMediaServersL();

        /**
        * Execute Browse dialog
        * @since Series 60 3.1
        * @return dialog return code
        */
        TInt ExecuteBrowseL();
        
        /**
        * Execute AdvFind dialog
        * @since Series 60 3.1
        * @return dialog return code
        */
        TInt ExecuteFindL();
        /**
         * Notifies that a new UPnP device was discovered.
         *
         * @since Series 60 3.1
         * @param CUpnpDevice, new device
         * @return None
         */
        void UPnPDeviceDiscoveredL(const CUpnpAVDevice& aDevice);

        /**
         * Notifies that a UPnP device was dissapeared.
         *
         * @since Series 60 3.1
         * @param CUpnpDevice, disappeared device
         * @return None
         */
        void UPnPDeviceDisappearedL(const CUpnpAVDevice& aDevice);

        /**
        * From CoeControl
        * @param TCoeHelpContext, context of desired help
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;        
        
        /**
         * Updates navi pane animation
         *
         * @since Series 60 3.1
         * @param TBool, animation on/off         
         */
        void UpDateAnimationWindowL(TBool aAnimationState);
        
    

    private:
        CEikColumnListBox*                          iListBox; //not owned
        MUPnPAVController&                          iAVControl;
        
        CUpnpAVDeviceList*                          iDeviceArray; //owned
        CUPnPCommonUI&                              iCommonUI; //not owned
        // title pane
        CAknTitlePane*                              iTitlePane; //not owned
        CAknNavigationDecorator*                    iNaviDecorator; //owned
        // animation timer
        //not owned
        CUPnPNaviPaneContainer*                     iNaviContainer;
        CEikStatusPane*                             iStatusPane; //not owned
        //not owned
        CAknNavigationControlContainer*             iNaviPaneContainer;
        
        MUPnPAVDeviceObserver*                      iPreDeviceObserver;
        TBool                                       iNaviPaneActive;
    };

#endif // __UPNP_EXTERNAL_DEVICE_DIALOG_H__
