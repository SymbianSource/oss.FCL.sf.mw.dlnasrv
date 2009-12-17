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
* Description:  Definition for MediaServant browse container class
*
*/


#ifndef C_MSBROWSECONTAINER_H
#define C_MSBROWSECONTAINER_H

// INCLUDES
#include <aknlists.h>

#include "msbasecontainer.h"
#include "msitembrowser.h"

// FORWARD DECLARATIONS

class CMSAppUi;
class CMSBrowseView;


/**
 * CMSBrowseContainer class
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSBrowseContainer ): public CMSItemBrowser
    {
public:
    /**
     * Constructor.
     */
    CMSBrowseContainer( CMSAppUi& aAppUi, CMSBrowseView& aView );

    /**
     * Destructor.
     */
    virtual ~CMSBrowseContainer();
    
protected:

// From base class CoeControl
    
    /**
     * From CoeControl
     * See base class definition
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
     * From CoeControl
     * See base class definition
     */
    void HandleResourceChange(TInt aType);

// From base class CMSItemBrowser

        /**
         * From CMSItemBrowser
         * See base class definition
         */
        void GetBrowseDataL();

private:

    /**
     * containers view
     */
    CMSBrowseView*                          iView;  // not owned

    };

#endif // C_MSBROWSECONTAINER_H