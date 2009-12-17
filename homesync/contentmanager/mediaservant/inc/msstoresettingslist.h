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
* Description:  CMSStoreSettingsList class definition
*
*/


#ifndef C_MSSTORESETTINGSLIST_H
#define C_MSSTORESETTINGSLIST_H

// INCLUDES
#include <aknsettingitemlist.h>
#include "msengineobserver.h"
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class CMSAppUi;
class CCmStoreRuleContainer;
class CCmStoreRule;
class CCmMediaServerFull;
class CMSStoreSettingsView;

// CLASS DECLARATION

/**
 * CMSStoreSettingsList general class.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSStoreSettingsList ): public CAknSettingItemList,
                            public MMSEngineObserver
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1.
         * @param aResourceId : resource ID of setting page.
         * @param aAppUi, application UI
         * @param aView, store settings view
         * @return pointer to CMSStoreSettingsList
         */
        static CMSStoreSettingsList* NewL( TInt aResourceId,
                                           CMSAppUi& aAppUi,
                                           CMSStoreSettingsView& aView );

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1.
         * @param aResourceId : resource ID of setting page.
         * @param aAppUi, application UI
         * @param aView, store settings view
         * @return pointer to CMSStoreSettingsList
         */
        static CMSStoreSettingsList* NewLC( TInt aResourceId,
                                           CMSAppUi& aAppUi,
                                           CMSStoreSettingsView& aView );
                                           
        /**
         * Destructor.
         */
        virtual ~CMSStoreSettingsList();

        /**
         * Opens the selected listbox item
         * @since S60 5.1
         * @param None
         * @return None
         */
        void OpenSelectedListboxItemL();

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
        void  HandleListBoxEventL ( CEikListBox *aListBox,
                                    TListBoxEvent aEventType );

// From base class CCoeControl
        
        /**
         * From CCoeControl
         * See base class definition
         */
        void SizeChanged();

        /**
         * From CoeControl
         * See base class definition
         */
        void HandleResourceChange( TInt aType );

        /**
         * From CoeControl
         * See base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;        

// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

    private:

        /**
         * Performs the second phase construction.
         *
         * @since S60 5.1
         * @param aResourceId, settinglist resource id
         * @return None
         */
        void ConstructL( TInt aResourceId );

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aAppUi, application UI
         * @param aView, store settings view
         * @return None
         */
        CMSStoreSettingsList( CMSAppUi& aAppUi, CMSStoreSettingsView& aView );

        /**
         * Reads rule status
         * @since S60 5.1
         */
        void ReadRuleStatus();

        /**
         * Saves 'keep on phone' setting to rule
         * @since S60 5.1
         */
        void SaveKeepOnPhoneSetting();

        /**
         * Sets title pane text accordin to media type
         * @since S60 5.1
         */
        void SetTitlePaneTextL();

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi&                               iAppUi;         // not owned
        /**
         * dummy text needed by CAknTextSettingItem
         */
        TBuf<KMaxFileName>                      iDummyText;
        /**
         * All store rules
         */
        CCmStoreRuleContainer*             iStoreRuleContainer; // not owned
        /**
         * Store rule
         */
        CCmStoreRule*                           iStoreRule;     // not owned
        /**
         * Keep on phone setting
         */
        TInt                                    iKeepOnPhone;
        /**
         * Media type
         */
        TCmMediaType                            iMediaType;
        /**
         * Wait note
         */
        CAknWaitDialog*                         iWaitNoteDialog;// owned
        /**
         * Parent view
         */
        CMSStoreSettingsView&                   iView;
    };

#endif // C_MSSTORESETTINGSLIST_H

// End of File
