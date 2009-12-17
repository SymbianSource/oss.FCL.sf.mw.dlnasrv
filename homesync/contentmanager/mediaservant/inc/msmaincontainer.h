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
* Description:  Definition for MediaServant main container class
*
*/


#ifndef C_MSMAINCONTAINER_H
#define C_MSMAINCONTAINER_H

// INCLUDES
#include <aknlists.h>
#include <eiklbo.h>             // listbox observer
#include "cmcommon.h"           // service types
#include "msengineobserver.h"
#include "msbasecontainer.h"
#include "mspropertywatcher.h"
#include "msmemorycardmonitor.h" //mmc event observer

// FORWARD DECLARATIONS
class CAknIconArray;
class CMSAppUi;
class CMSEngine;
class MMSEngineObserver;
class CMSMainView;


/**
 * CMSMainContainer class
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMainContainer ): public CMSBaseContainer,
                         public MEikListBoxObserver,
                         public MMSEngineObserver,
                         public MMSPropertyObserver,
                         public MMSMemoryCardObserver
    {
    // Icon enumerations
    enum TMSListBoxIcons
        {
        EIconSync = 0,
        EIconSyncStop,
        EIconToHome,
        EIconToHomeInactive,
        EIconFromHome,
        EIconFromHomeInactive,
        EIconCount
        };
    // Synchronization states
    enum TMSSyncState
        {
        ESyncNotRunning = 0,
        ESyncRunning,
        ESyncWaiting
        };
    
    // Update steps
    enum TMSUpdateStep
        {
        EFirstStep = 0,
        ESecondStep,
        EThirdStep,
        EForthStep,
        EEndStep
        };
        
    public:
        /**
         * Constructor.
         */
        CMSMainContainer( CMSAppUi& aAppUi,
                          CMSMainView& aView );

        /**
         * Symbian default constructor.
         */
        void ConstructL( TRect aRect );

        /**
         * Destructor.
         */
        virtual ~CMSMainContainer();    

        /**
         * sets manual wizard execution info
         * @param appWizardRun, wizard executed
         * @since S60 5.1
         */
        void SetManualWizardInfo( TBool aAppWizardRun );

        /**
         * Shows refresh indiction on navi pane
         * @since S60 5.1
         */
        void ShowRefreshIndicationL();

        /**
         * sets refresh state
         * @since S60 5.1
         */
        void StopRefreshL();

        /**
         * return highlighted item index
         * @since S60 5.1
         */
        TInt SelectedItemIndex();

        /**
         * Opens the selected listbox item
         * @since S60 5.1
         */
        void OpenSelectedListboxItemL();

        /**
         * Starts asynchronous metadata delete
         * @since S60 5.1
         */
        void StartMetadataDeleteL();
    
        void RunHarvest();
        // From base class MMSMemoryCardObserver
                
                /**
                 * From MMSMemoryCardObserver
                 * See base class definition
                 */
                 void MemoryCardChangedL( TMemoryCardEvent aEvent );        
    protected:
    
// From base class CCoeControl    

        /**
         * From CCoeControl
         * See base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;    

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

        /**
         * From CCoeControl
         * See base class definition
         */
        void HandleResourceChange( TInt aType );

// From base class MEikListBoxObserver

        /**
         * From MEikListBoxObserver
         * See base class definition
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

// From base class MMSPropertyObserver

        /**
         * From MMSPropertyObserver
         * See base class definition
         */
        void PropertyChangedL( TCmService aService,
                               TInt aTotalItems,
                               TInt aProcessedItems,
                               TInt /*aItemsToBeProcessed*/ );
                               
         
    private:

        /**
         * Updates listbox data
         * @since S60 5.1
         */
        void UpdateListBoxDataL();

        /**
         * Starts sync operation
         * @since S60 5.1
         */
        void StartSyncL();

        /**
         * Shows error note
         * @since S60 5.1
         * @param aTextResource, error text resource
         */
        void ShowErrorNoteL( TInt aTextResource );

        /**
         * Shows last synchronization time on navi pane
         * @since S60 5.1
         */
        void SetNaviTextLastSyncL();

        /**
         * Callback for periodic timer
         * @since S60 5.1
         * @param TAny, callback observer
         */
        static TInt TimerCallbackL(TAny* aObject);

        /**
         * Callback for navi pane timer
         * @since S60 5.1
         * @param TAny, callback observer
         */
        static TInt NaviTimerCallbackL(TAny* aObject);        
        
        /**
         * Static callback method for update list box data
         * @since S60 5.1
         * @param TAny, callback observer
         * @return
         */
        static TInt BackgroundUpdateListL( TAny* aObject );
        
        /**
         * Supdate list box data step by step
         * @since S60 5.1
         * @return
         */
        TInt UpdateListByStepL();
        
        /**
         * Checks if fill list is selected
         *
         * @since S60 5.1
         * @param None
         * @return TBool, ETrue if list is selected, EFalse otherwise
         */
        TBool CheckIfFillListSelected();

        /**
         * Shows 'refresh completed' text in navi pane
         *
         * @since S60 5.1
         * @param None
         * @return None
         */        
        void ShowRefreshCompleteTextL();

    private:

        /**
         * Listbox control
         */
        CEikFormattedCellListBox*               iListBox;       // owned

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                               iAppUi;         // not owned

        /**
         * active refresh operation indicator
         */
        TBool                                   iRefreshRunning;

        /**
         * active sync operation indicator
         */
        TMSSyncState                            iSyncRunning;

        /**
         * active preprocess operation indicator
         */
        TBool                                   iPreprocessRunning;

        /**
         * containers view
         */
        CMSMainView*                            iView;           // not owned

        /**
         * Transfer time
         */
        TInt                                    iTransferTime;
        /**
         * Count of items to be stored
         */
        TInt                                    iItemsToBeStored;
        /**
         * Count of items to be filled
         */
        TInt                                    iItemsToBeFilled;
        /**
         * Progress watcher
         */
        CMSPropertyWatcher*                     iMSPropertyWatcher; // owned
        
        /**
         * MemoryCard monitor
         */
        CMSMemoryCardMonitor*                   iMemoryCardMonitor; // owned

        /**
         * Wizard execution flag
         */
        TBool                                   iAppWizardRun;
        
        /**
         * Last sync timer
         */
        CPeriodic*                              iPeriodic;          // Owned
        
        /**
         * Navi pane text timer
         */
        CPeriodic*                              iNaviTimer;         // Owned        
        
        /**
         * Idle time active object, Owend
         */
        CIdle*                                  iUpdateList;        // Owned
        
        /**
         *  the count of requests for refresh UI
         */
        TInt                                    iRequestCt;
        
        /**
         * current step of update list box
         */
        TMSUpdateStep                           iCurStep;
        
        /**
         * Fill list selection status
         */
        TBool                                   iFillListSelected;
    };

#endif // C_MSMAINCONTAINER_H
