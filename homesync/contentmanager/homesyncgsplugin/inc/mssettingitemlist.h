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
* Description:  Definition for Setting item list -class
*
*/


#ifndef MSSETTINGITEMLIST_H
#define MSSETTINGITEMLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

#include "cmcommon.h"
#include "msengineobserver.h"
#include "msgs.hrh"

// FORWARD DECLARATIONS
class CMSSettingsView;
class CMSEngine;
class CCmMediaServerFull;


// CLASS DECLARATION

/**
* CMSSSettingItemList general class. This class is used to
* create setting items to several setting item lists.
*/
NONSHARABLE_CLASS( CMSSettingItemList ): public CAknSettingItemList,
                                         public MMSEngineObserver
    {
    public: // Constructor and destructor

        /**
         * NewL
         * 2-phase constructor.
         * @param aResourceId : resource ID of setting page.
         * @param aAppUi : application ui.
         */
        static CMSSettingItemList* NewL( TInt aResourceId,
                                         CMSSettingsView& aView );

        /**
         * NewLC
         * 2-phase constructor.
         * @param aResourceId : resource ID of setting page.
         * @param aAppUi : application ui.
         */
        static CMSSettingItemList* NewLC( TInt aResourceId,
                                         CMSSettingsView& aView );

        /**
         * Constructor.
         */
        CMSSettingItemList( CMSSettingsView& aView );
        
        /**
         * ConstructL
         * Second phase constructor.
         * @param aResourceId : resource ID of setting page.
         * @param aAppUi : application ui
         */
        void ConstructL( TInt aResourceId );

        /**
         * Destructor.
         */
        virtual ~CMSSettingItemList();

        /**
         * Opens the selected listbox item
         *
         * @since S60 5.1
         */
        void OpenSelectedListboxItemL();

        /**
         * Returns MSEngine
         *
         * @since S60 5.1
         * @return CMSEngine, application engine
         */
        CMSEngine* MSEngine();

    protected:

    // From base class CAknSettingItemList

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        CAknSettingItem* CreateSettingItemL( TInt aSettingId );

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void SizeChanged();

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void HandleResourceChange( TInt aType );

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void EditItemL ( TInt aIndex, TBool aCalledFromMenu );

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void HandleListBoxEventL( CEikListBox *aListBox,
                                  TListBoxEvent aEventType );

    // From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * see base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

    // From base class CCoeControl

        /**
         * From CCoeControl
         * see base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;

    private:

        /**
         * Searches item from the list be item ID
         *
         * @since S60 5.1
         * @param aId, item id
         * @return CAknSettingItem, item pointer
         */
        CAknSettingItem* SearchItemById( const TMSSettingItems aId ) const;

        /**
         * Reads current profile from cenrep
         *
         * @since S60 5.1
         * @return TInt, profile
         */
        TInt GetCurrentProfileL() const;

        /**
         * Sets memory selection visibility
         *
         * @since S60 5.1
         */
        void SetMemorySelectionL();

    private:

        /**
         * Reference to AppUi
         */
        CMSEngine*                          iMSEngine;          // owned
        
        /**
         * Parent view
         */
        CMSSettingsView&                    iView;
        
        /**
         * Pointer to listbox
         */
        CEikFormattedCellListBox*           iListBox;           // not owned
        
        /**
         * dummy text needed by text setting items
         */
        TBuf<KMaxFileName>                  iDummyText;
        
        /**
         * Sync selection
         */
        TCmServiceState                     iSync;
        
        /**
         * Memory manager
         */
        TCmServiceState                     iMemoryManager;
        
        /**
         * searched servers list
         */
        RPointerArray<CCmMediaServerFull>*  iStoredServers;     // owned        
    };

#endif // MSSETTINGITEMLIST_H

// End of File
