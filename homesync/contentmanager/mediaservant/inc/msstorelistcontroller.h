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
* Description:  StoreListController class definition
*
*/





#ifndef C_MSSTORELISTCONTROLLER_H
#define C_MSSTORELISTCONTROLLER_H

// INCLUDES
#include <e32base.h>
#include "cmcommontypes.h"


// FORWARD DECLARATIONS
class CMSEngine;
class CCmStoreRuleContainer;
class CCmStoreRule;

/**
 * CMSStoreListController class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreListController ): public CBase
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine
         * return pointer to CMSStoreListController
         */
        static CMSStoreListController* NewL( CMSEngine& aMSEngine );

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine
         * return pointer to CMSStoreListController
         */
        static CMSStoreListController* NewLC( CMSEngine& aMSEngine );
        
        /**
         * Destructor.
         */
        virtual ~CMSStoreListController();

        /**
         * Returns store list container
         * @since S60 5.1
         * @return CCmStoreRuleContainer, list container
         */
        CCmStoreRuleContainer* StoreListContainer();

        /**
         * Reads list container and re-creates rule array
         * @since S60 5.1
         */
        void  UpdateListsL();

    private:
    
        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aMSEngine, application engine        
         * @return None
         */
        CMSStoreListController( CMSEngine& aMSEngine );

        /**
         * Performs the second phase construction.
         */
        void ConstructL();

        /**
         * Creates default store rules
         * Since S60 5.1
         * @param None
         * @return None
         */
        void CreateStoreRulesL();

        /**
         * Changes names of store rules
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void ChangeStoreRuleNamesL( void );

    private:

        /**
         * pointer to application engine
         */
        CMSEngine*                  iMSEngine;              // not owned
        /**
         * Fill rules
         */
        CCmStoreRuleContainer*      iStoreListContainer;    // owned
    };


#endif // C_MSSTORELISTCONTROLLER_H


// End of File
