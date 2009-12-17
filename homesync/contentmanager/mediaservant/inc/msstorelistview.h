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
* Description:  CMSStoreListView class definition
*
*/





#ifndef C_MSSTORELISTVIEW_H
#define C_MSSTORELISTVIEW_H

// INCLUDES
#include "msengineobserver.h"
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSStoreListContainer;
//class CMSEngine;

/**
 * CMSStoreListView view class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreListView ): public CMSBaseView,
                         public MMSEngineObserver
    {

    public:

        /**
         * Constructor.
         *
         * @since S60 5.1
         * @param aAppUi, application UI
         * @return None
         */
        CMSStoreListView( CMSAppUi& aAppUi );

        /**
         * Symbian default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSStoreListView();

        /**
         * ListBox focus index
         * @since S60 v3.1
         * @return TInt, focus index
         */
        TInt FocusIndex() const;

        /**
         * Creates and sets navi pane text
         * @since S60 3.2
         */
        void CreateAndSetNaviTextL();

		/**
	*  Returns store list preprocessing state
	* @since S60 5.1
	* @return TBool, ETrue if preprocessing, otherwise EFalse
	*/
	TBool PreProcessing();

    protected:

// From base class CAknView

        /**
         * From CAknView
         * See base class definition
         */
        TUid Id() const;

        /**
         * From CAknView activate the view
         * See base class definition
         */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
         * From CAknView activate the view
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

// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi&                           iAppUi;         // not owned
        /**
         * view's container
         */
        CMSStoreListContainer*              iContainer;     // owned
        /**
         * Focus index
         */
        TInt                                iFocusIndex;
		/**
         * preprocessing state
         */
		TBool								iPreprocessingState;
    };

#endif // C_MSSTORELISTVIEW_H


// End of File
