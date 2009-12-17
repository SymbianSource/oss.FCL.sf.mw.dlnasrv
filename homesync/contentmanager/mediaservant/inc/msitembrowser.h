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
* Description:  Definition for MediaServant item browser base class
*
*/



#ifndef C_MSITEMBROWSER_H
#define C_MSITEMBROWSER_H

// INCLUDES
#include <aknlists.h>
#include "cmcommon.h"           // service types
#include "cmcommontypes.h"
#include "msbasecontainer.h"

// FORWARD DECLARATIONS
class CAknIconArray;
class CMSAppUi;
class CCmBaseListItem;

/**
 * CMSItemBrowser class
 * @since S60 5.1
 */
class CMSItemBrowser :   public CMSBaseContainer,
                         public MEikListBoxObserver
    {
    // Icon enumerations
    enum TMSListBoxIcons
        {
        EMarkIcon = 0,
        EIconImage,
        EIconAudio,
        EIconVideo,
        EIconCount
        };

    public:

        /**
         * Symbian default constructor.
         */
        void ConstructL( TRect aRect );

        /**
         * Destructor
         */
        virtual ~CMSItemBrowser();    

        /**
         * Sets current file status
         * @since S60 5.1
         * @param aIndex, item index
         * @param TCmListItemStatus, file status
         */
        void SetFileStatus( TInt aIndex, TCmListItemStatus aStatus );

        /**
         * Gets current file status
         * @since S60 5.1
         * @param aIndex, item index
         * @return TCmListItemStatus, file status
         */
        TCmListItemStatus FileStatus( TInt aIndex );

        /**
         * Return pointer to listbox
         * @since S60 5.1
         * @return CEikListbox, listbox
         */
        CEikListBox* ListBox();

    protected:

        /**
         * Updates listbox data
         * @since S60 5.1
         */
        void UpdateListBoxDataL();

        /**
         * Gets browse data from server
         * @since S60 5.1
         */
        virtual void GetBrowseDataL() = 0;        

// From base class CCoeControl

        /**
         * From CCoeControl
         * See base class definition
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * From CCoeControl
         * See base class definition
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl
         * See base class definition
         */
        void SizeChanged();

        /**
         * From CCoeControl
         * See base class definition
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

// From base class MEikListBoxObserver

        /**
         * From MEikListBoxObserver
         * See base class definition
         */
        void HandleListBoxEventL(
                CEikListBox* aListBox,
                TListBoxEvent aEventType );

    protected:

        /**
         * Listbox control
         */
        CEikFormattedCellListBox*               iListBox;       // owned
        /**
         * pointer to AppUi object
         */
        CMSAppUi*                               iAppUi;         // not owned
        /**
         * Item array
         */
        RPointerArray<CCmBaseListItem>*         iItemArray;
        /**
         * Internal array for listbox item texts
         */
        CDesCArrayFlat*                         iInternalItemArray;
        /**
         * Next item index for thumbnail loading
         */
        TInt                                    iNextItemIndex;
        /**
         * Icon count for thumbnails
         */
        TInt                                    iIconCount;
        /**
         * Mediatype of selected rule
         */
        TCmMediaType                            iMediaType;
    };

#endif // C_MSITEMBROWSER_H