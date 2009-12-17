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
* Description:  Definition for MediaServant fill list browse container class
*
*/


#ifndef C_MSFILLBROWSECONTAINER_H
#define C_MSFILLBROWSECONTAINER_H

// INCLUDES
#include <aknlists.h>

#include "msengineobserver.h"
#include "msbasecontainer.h"
#include "msitembrowser.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSFillBrowseView;
class CCmFillRule;


/**
 * CMSFillBrowseContainer class
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillBrowseContainer ): public CMSItemBrowser,
                                             public MMSEngineObserver
    {
    public:
        
        /**
         * Default constructor.
         */
        CMSFillBrowseContainer( CMSAppUi& aAppUi,
                                CMSFillBrowseView& aView );

        /**
         * Destructor.
         */
        virtual ~CMSFillBrowseContainer();

        /**
         * Randomizes list
         * @since S60 5.1
         * @param None
         * @return None
         */
        void RandomizeListL();

        /**
         * Checks if current list has random rule
         * @since S60 5.1
         * @param None         
         * @return TBool, ETrue if random rule exists
         */
        TBool HasListRandomRule();

        /**
         * Sets file statuses
         * @Since S60 5.1
         * @param None
         * @return None
         */
        void SetFileStatusesL();

        /**
         * Sets status of files
         * @param aStatus, file status
         * @return None
         */
        void SetStatusOfFiles( TCmListItemStatus aStatus );

    protected: 
    
// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

// From base class CoeControl
        
        /**
         * From CoeControl
         * See base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;

// From base class CMSItemBrowser    

        /**
         * Gets browse data from server
         * See base class definition
         */
        void GetBrowseDataL();

    private:
    
        /**
         * Detects the KEikDynamicLayoutVariantSwitch
         * message and re-layouts controls from this.
         * @since S60 5.1
         * @param aType, change type
         * @return None
         */
        void HandleResourceChange(TInt aType);

        /**
         * Checks if file status has been changed
         * @Since S60 5.1
         * @param None
         * @return TBool, ETrue if changed, EFalse otherwise
         */
        TBool CheckIfStatusChanged();
        
        /**
         * Creates and sets navi pane text
         * @Since S60 5.1
         * @param aCount, item count
         * @param aSize, list size
         * @return None
         */
        void CreateAndSetNaviPaneTextL( TUint32 aCount, TUint32 aSize );

    private:
        /**
         * selected rule
         */
        CCmFillRule*                        iRule;  // not owned
        /**
         * containers view
         */
        CMSFillBrowseView*                  iView;  // not owned
        /**
         * array of item status
         * list order can't be changed so we don't have to copy
         * whole item
         */
        RArray<TCmListItemStatus>           iStatusArray;

    };



#endif // C_MSFILLBROWSECONTAINER_H