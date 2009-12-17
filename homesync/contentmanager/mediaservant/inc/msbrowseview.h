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





#ifndef C_MSBROWSEVIEW_H
#define C_MSBROWSEVIEW_H

// INCLUDES
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSBrowseContainer;

/**
 * CMSBrowseView view class. Main view is visible when
 * application is launched
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSBrowseView ): public CMSBaseView
    {
    public:
        /**
         * Constructor.
         */
        CMSBrowseView( CMSAppUi& aAppUi );

        /**
         * Symbian default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSBrowseView();

        /**
         * Solves title pane text by media type
         * @since S60 3.2
         * @param aMediaType, mediatype
         */
        void SetTitlePaneTextL( TCmMediaType aMediaType );
        
    protected: 

// From base class CAknView

        /**
         * From CAknView returns Uid of View
         * See base class definition
         */
        TUid Id() const;

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
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );    

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                           iAppUi;         // not owned

        /**
         * view's container
         */
        CMSBrowseContainer*                 iContainer;     // owned
    };

#endif // C_MSBROWSEVIEW_H


// End of File
