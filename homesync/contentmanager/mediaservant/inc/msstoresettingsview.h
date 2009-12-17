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
* Description:  CMSStoreSettingsView class definition
*
*/





#ifndef C_MSSTORESETTINGSVIEW_H
#define C_MSSTORESETTINGSVIEW_H

// INCLUDES
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSEngine;
class CMSStoreSettingsList;

/**
 * CMSStoreSettingsView class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreSettingsView ) : public CMSBaseView
    {

    public:

        /**
         * Constructor.
         *
         * @since S60 5.1
         * @param aAppUi, application UI
         * @return None
         */
        CMSStoreSettingsView( CMSAppUi& aAppUi );

        /**
         * default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSStoreSettingsView();

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
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
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
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );


    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi&                           iAppUi;         // not owned
        /**
         * Views container class
         */
        CMSStoreSettingsList*               iContainer;     // owned
    };

#endif // C_MSSTORESETTINGSVIEW_H


// End of File
