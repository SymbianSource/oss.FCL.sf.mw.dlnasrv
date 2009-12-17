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
* Description:  CMSStoreKeepOnPhoneSetting class definition
*
*/


#ifndef C_MSSTOREKEEPONPHONESETTING_H
#define C_MSSTOREKEEPONPHONESETTING_H

// INCLUDES
#include <aknsettingitemlist.h>
#include "cmcommontypes.h"
#include "mediaservant.hrh"

// FORWARD DECLARATIONS
class CMSAppUi;


// CLASS DECLARATION

/**
 * CMSStoreKeepOnPhoneSetting class.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreKeepOnPhoneSetting ):
                        public CAknEnumeratedTextPopupSettingItem
    {
    public:

        /**
         * Constructor.
         *
         * @since S60 5.1
         * @param aSettingPageResourceId, setting page resource
         * @param aMediaType, mediatype
         * @param aSelection, selection index
         * return None
         */
        CMSStoreKeepOnPhoneSetting(
            TInt aSettingPageResourceId,
            TCmMediaType aMediaType,
            TInt& aSelection
            );

        /**
        * Destructor.
        */
        virtual ~CMSStoreKeepOnPhoneSetting();


    protected:
    
// From base class CAknSettingItem

        /**
         * From CAknSettingItem
         * See base class definition
         */
        void CompleteConstructionL();

    private:

        /**
         * media type
         */
        TCmMediaType                            iMediaType;
    };

#endif // C_MSSTOREKEEPONPHONESETTING_H

// End of File
