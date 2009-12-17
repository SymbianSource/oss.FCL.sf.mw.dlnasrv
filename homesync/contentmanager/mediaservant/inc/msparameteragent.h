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
* Description:  CMSParameterAgent class definition
*
*/


#ifndef C_MSPARAMETERAGENT_H
#define C_MSPARAMETERAGENT_H

// INCLUDES
#include <e32base.h>
#include "mediaservant.hrh"

// template parameters
struct TMSTemplate
    {
    TInt templateType; // mediatype
    TInt selectedTemplate; // selected template
    TBuf<KMaxFileName> templateName; // template name
    };

// FORWARD DECLARATIONS

/**
 * CMSParameterAgent class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSParameterAgent ): public CBase
    {

    public:

        /**
         * Constructor.
         */
        CMSParameterAgent();

        /**
         * Destructor.
         */
        virtual ~CMSParameterAgent();

        /**
         * Sets selected rule template
         *
         * @since S60 5.1
         * @param aTemplate, selected template
         * @return None
         */
        void SetRuleTemplate( TMSTemplate aTemplate );

        /**
         * Returns selected rule template
         *
         * @since S60 5.1
         * @return TMSRuleTemplate, selected template
         * @return None
         */
        TMSTemplate GetRuleTemplate() const;

        /**
         * Sets selected index of main view
         *
         * @since S60 5.1
         * @param aIndex, selected listbox item index
         * @return None
         */
        void SetMainViewFocusIndex( TInt aIndex );

        /**
         * Returns focus index of main view.
         *
         * @since S60 5.1
         * @return TInt, focus index of main view
         * @return None
         */
        TInt MainViewFocusIndex() const;

        /**
         * Sets selected index of fill view
         *
         * @since S60 5.1
         * @param aIndex, selected listbox item index
         * @return None
         */
        void SetFillViewFocusIndex( TInt aIndex );

        /**
         * Returns focus index of fill view.
         *
         * @since S60 5.1
         * @return TInt, focus index of fill view
         * @return None
         */
        TInt FillViewFocusIndex() const;

        /**
         * Sets selected index of store view
         *
         * @since S60 5.1
         * @param aIndex, selected listbox item index
         * @return None
         */
        void SetStoreViewFocusIndex( TInt aIndex );

        /**
         * Returns focus index of store view.
         *
         * @since S60 5.1
         * @param None
         * @return TInt, focus index of store view
         */
        TInt StoreViewFocusIndex() const;
        
        /**
         * Sets uid of current view
         *
         * @since S60 5.1
         * @param aViewId, view id
         * @return None
         */
        void SetPreviousViewId( TUid aViewId );

        /**
         * Returns uid of previous view.
         *
         * @since S60 5.1
         * @param None
         * @return TUid, focus index of previous view
         */
        TUid PreviousViewId() const;

        /**
         * Sets browse target
         *
         * @since S60 5.1
         * @param aBrowseTarget, browse target
         * @return None
         */
        void SetBrowseTarget( TMSBrowseTarget aBrowseTarget );

        /**
         * Gets browse target
         *
         * @since S60 5.1
         * @param None
         * @return TMSBrowseTarget, browse target
         */
        TMSBrowseTarget GetBrowseTarget() const;

    private:

        /**
         * previous view focus index
         */
        TInt                                iPreviousFocusIndex;
        /**
         * previous view focus index
         */
        TUid                                iPreviousViewId;
        /**
         * selected template
         */
        TMSTemplate                         iTemplate;
        /**
         * Browse target
         */
        TMSBrowseTarget                     iBrowseTarget;
        /**
         * Main view focus index
         */
        TInt                                iMainViewFocusIndex;
        /**
         * Fill view focus index
         */
        TInt                                iFillViewFocusIndex;
        /**
         * Store view focus index
         */
        TInt                                iStoreViewFocusIndex;
    };

#endif // C_MSPARAMETERAGENT_H


// End of File
