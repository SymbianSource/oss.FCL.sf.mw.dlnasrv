/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition for MediaServant base view class
*
*/



#ifndef __MSBASEVIEW_H__
#define __MSBASEVIEW_H__

// INCLUDES
#include <aknview.h>


// FORWARD DECLARATIONS
class CAknTitlePane;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CMSAppUi;

// CLASS DECLARATION

/**
* CMSBaseView  base view class.
*/
NONSHARABLE_CLASS( CMSBaseView ) : public CAknView
    {

    protected: // Constructors and destructor

        /**
        * CMSBaseView
        * Default constructor.
        */
        CMSBaseView();

        /**
        * ~CMSBaseView
        * Destructor.
        */
        virtual ~CMSBaseView();

    public: // New functions

        /**
        * SetTitlePaneTextL
        * Sets text by resource ID in title pane.
        * @since S60 3.1
        * @param aResourceId The ID of outline number to displayed next.
        */
        void SetTitlePaneTextL( const TInt aResourceId );

        /**
        * SetTitlePaneTextL
        * Sets text by resource ID in title pane.
        * @since S60 3.1
        * @param aText, title text
        */
        void SetTitlePaneTextL( const TDesC& aText );

        /**
        * SetNavigationPaneTextL
        * Sets Navigation pane text.
        * @since S60 3.1
        * @param aText, text to be shown
        */
        void SetNavigationPaneTextL( const TDesC& aText );

        /**
        * ClearCurrentNaviPaneText
        * Clears current navi pane text
        * @since S60 3.1
        */
        void ClearCurrentNaviPaneText();

        /**
         * Cancels async operation
         * @Since S60 3.1
         * @param aAppUi, pointer to cmsappui class
         */
        void CancelAsyncOperation( CMSAppUi* aAppUi );

    private:

        /**
        * iNaviPane
        */
        CAknNavigationControlContainer* iNaviPane;          // not owned
        /**
        * navi decorator
        */
        CAknNavigationDecorator*    iNaviDecorator;
    };

#endif // __MSBASEVIEW_H__

// End of File
