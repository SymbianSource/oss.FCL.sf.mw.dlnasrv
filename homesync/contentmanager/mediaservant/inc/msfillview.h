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
* Description:  Definition for MediaServant Fill List view class
*
*/





#ifndef C_MSFILLVIEW_H
#define C_MSFILLVIEW_H

// INCLUDES
#include "msengineobserver.h"
#include "msbaseview.h"


// FORWARD DECLARATIONS
class CMSAppUi;
class CMSFillContainer;
class CMSEngine;

/**
 * CMSFillView class.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillView ): public CMSBaseView,
                    public MMSEngineObserver
    {

    public:

        /**
         * Constructor.
         */
        CMSFillView( CMSAppUi& aAppUi );

        /**
         * default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSFillView();

        /**
         * ListBox focus index
         * @since S60 5.1
         * @return TInt, focus index
         */
        TInt FocusIndex() const;

        /**
         * Changes cba according to selection status
         * @since S60 3.2
         * @param aSelected, ETrue if selected
         */
        void SetCbaL( TBool aSelected );

        /**
         * Shows 'updating lists' text on navi pane
         * @Since S60 5.1
         */
        void ShowNaviTextL();

        /**
         * Sets iListUnSelected if selections changed
         * @Since S60 5.1
         */
        void SetListUnselected();

// From base class MEikMenuObserver
        
        /**
         * From MEikMenuObserver
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );
                
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

// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

// From base class MEikMenuObserver
        
        /**
         * From MEikMenuObserver
         * See base class definition
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:

        /**
         * Opens template query
         * @Since S60 5.1
         * @param aCommand, template type
         */
        void ShowTemplateQueryL( TInt aCommand );

        /**
         * Shows delete filled files query
         * @Since S60 5.1
         */
        void DoDeleteQueryL();

        /**
         * Checks if priority or activity has been changed
         * @Since S60 5.1
         */
        void CheckAndHandleChangesL();

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                           iAppUi;         // not owned
        /**
         * Views container
         */
        CMSFillContainer*                   iContainer;     // owned
        /**
         * Focus index
         */
        TInt                                iFocusIndex;
        /**
         * Next view id
         */
        TUid                                iNextView;
        /**
         * flag for list selection changes
         */
        TBool                               iNewListSelected;
        /**
         * flag for list selection changes
         */
        TBool                               iListUnSelected;
    };

#endif // C_MSFILLVIEW_H


// End of File
