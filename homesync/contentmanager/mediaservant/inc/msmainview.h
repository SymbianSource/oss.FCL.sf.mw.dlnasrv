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
* Description:  Definition for MediaServant main view class
*
*/





#ifndef C_MSMAINVIEW_H
#define C_MSMAINVIEW_H

// INCLUDES
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSMainContainer;
class CMSEngine;
class CMSSettingsView;
class CGSPluginInterface;

/**
 * CMSMainView view class. Main view is visible when
 * application is launched
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMainView ): public CMSBaseView
    {
    public:
        /**
         * Constructor.
         */
        CMSMainView( CMSAppUi& aAppUi );

        /**
         * Symbian default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSMainView();

        /**
         * Changes cba according to hinghlighted item index
         * @since S60 3.2
         * @param aHighlightedIndex, highlighted item index
         */
        void SetCbaL( TInt aHighlightedIndex );

        /**
         * ListBox focus index
         * @since S60 5.1
         * @return TInt, focus index
         */
        TInt FocusIndex() const;

        /**
         * Checks if MMC card has been changed
         * @since S60 5.1
         */
        void CheckMemoryCardL();

    protected:

// From base class CAknView

        /**
         * From CAknView
         * See base class definition
         */
        TUid Id() const;

        /**
         * From CAknView
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );

        /**
         * From CAknView
         * See base class definition
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * From CAknView
         * See base class definition
         */
        void DoDeactivate();

// From base class MEikMenuObserver

        /**
         * From MEikMenuObserver
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    private:

        /**
         * Show memory card query and handles selection
         * @since S60 5.1
         * @param RPointerArray<CCmDriveInfo>&, stored drive array
         * @param RPointerArray<CCmDriveInfo>&, device drive array
         * @param TInt, mmc drive index
         */
        void ShowAndHandleMMCQueryL(
                             RPointerArray<CCmDriveInfo>& aStoredArray,
                             RPointerArray<CCmDriveInfo>& aDriveArray,
                             TInt index );

        /**
         * Starts the wizard when run first time, callback for CIdle
         * @since S60 5.1
         * @param aPtr, Pointer to this class
         * @return TBool, EFalse because of one time call
         */
        static TBool InitializeWizardL( TAny* aPtr );

        /**
         * Starts the new memory card dialog when new
         * memory card inserted, callback for CIdle
         * @since S60 5.1
         * @param aPtr, Pointer to this class
         */
        static TInt InitializeMemoryCardL( TAny* aPtr );

        /**
         * Do starts the wizard when run first time
         * @since S60 5.1
         * @return TBool, EFalse because of one time call
         */
        TBool DoInitializeWizardL();

        /**
         * Do starts the new memory card dialog when new
         * memory card inserted
         * @since S60 5.1
         * @return TBool, EFalse because of one time call
         */
        void DoInitializeMemoryCardL();

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                           iAppUi;         // not owned

        /**
         * view's container
         */
        CMSMainContainer*                   iContainer;     // owned

        /**
         * Check MMC indicator
         */
        TBool                               iCheckMMC;
        /**
         * Idle loop to initialize wizard
         */
        CIdle*                              iIdle;

        /**
         * Flag for wizard initialization
         */
        TBool                               iWizardInitialization;
    };

#endif // C_MSMAINVIEW_H


// End of File
