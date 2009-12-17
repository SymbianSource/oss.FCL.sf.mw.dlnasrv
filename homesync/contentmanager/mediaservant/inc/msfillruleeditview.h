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
* Description:  Definition for MediaServant Fill rule edit view class
*
*/





#ifndef C_MSFILLRULEEDITVIEW_H
#define C_MSFILLRULEEDITVIEW_H

// INCLUDES
#include "msengineobserver.h"
#include "msbaseview.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSEngine;
class CMSFillRuleEditList;

/**
 * CMSFillRuleEditView class.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillRuleEditView ): public CMSBaseView
    {

    public:

        /**
         * Constructor.
         */
        CMSFillRuleEditView( CMSAppUi& aAppUi );

        /**
         * Default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        virtual ~CMSFillRuleEditView();

        /**
         * Function shows a template query
         *
         * @since S60 5.1
         * @param aResourceId, Query id
         * @param aQueryHeading, Query header id
         * @param aRuleNames, Query rule names
         * @param aSelected, Selected rule index
         * @return TInt, returns ETrue, if an item has been selected,
         *                       EFalse, if query has been cancelled
         */
		TBool ShowTemplateQueryL( TInt aResourceId,
                                  TInt aQueryHeading,
                                  CDesCArray* aRuleNames,
                                  TInt &aSelected );

        /**
         * Function shows a query
         *
         * @since S60 5.1
         * @param aResourceId, Query id
         * @return TInt, returns ETrue, if yes has been selected,
         * otherwise EFalse
         */
		TBool ShowQueryL( TInt aResourceId );

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

        /**
         * From MEikMenuObserver
         * See base class definition
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                           iAppUi;         // not owned
        /**
         * Views container class
         */
        CMSFillRuleEditList*                iContainer;     // owned
        /**
         * application engine
         */
        CMSEngine*                          iMSEngine;      // not owned
    };

#endif // C_MSFILLRULEEDITVIEW_H


// End of File
