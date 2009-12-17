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
* Description:  Definition for MediaServant browse view class
*
*/





#ifndef C_MSFILLBROWSEVIEW_H
#define C_MSFILLBROWSEVIEW_H

// INCLUDES
#include <aknlists.h>           // listbox observer
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSFillBrowseContainer;

/**
 * CMSBrowseView view class. Main view is visible when
 * application is launched
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillBrowseView ) : public CMSBaseView
    {
    public:
        /**
         * Constructor.
         */
        CMSFillBrowseView( CMSAppUi& aAppUi );

        /**
         * Symbian default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSFillBrowseView();

    protected: 

// From base class CAknView

        /**
         * From CAknView returns Uid of View
         * See base class definition
         */
        TUid Id() const;

        /**
         * From CAknView activate the view
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
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );

        /**
         * From MEikMenuObserver
         * See base class definition
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);        

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                           iAppUi;             // not owned
        /**
         * view's container
         */
        CMSFillBrowseContainer*             iContainer;         // owned
        /**
         * Marked item indexes
         */
        const CListBoxView::CSelectionIndexArray* iSelections;  // not owned
    };

#endif // C_MSFillBROWSEVIEW_H


// End of File
