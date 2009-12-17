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
* Description:  CMSMainContainer class implementation
*
*/



#include <AknIconArray.h>
#include <mediaservant.rsg>
#include <mediaservant.mbg>
#include <StringLoader.h>
#include <aknnotewrappers.h>

#include "mserv.hlp.hrh"
#include "cmfillrule.h"
#include "msappui.h"
#include "msmainview.h"
#include "mediaservantuid.h"
#include "msmaincontainer.h"
#include "msengine.h"
#include "msconstants.h"
#include "msdebug.h"

// Constants
const TInt KTimerInterval = 60000000; // 1 min
const TInt KNaviTimerInterval = 2000000; // 2 sec


// --------------------------------------------------------------------------
// CMSMainContainer::CMSMainContainer()
// --------------------------------------------------------------------------
//
CMSMainContainer::CMSMainContainer( CMSAppUi& aAppUi,
                                    CMSMainView& aView )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::CMSMainContainer"));

    iAppUi = &aAppUi;
    iView = &aView;
    iAppWizardRun = EFalse;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::~CMSMainContainer()
// --------------------------------------------------------------------------
//
CMSMainContainer::~CMSMainContainer()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::~CMSMainContainer"));

    if( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    delete iPeriodic;

    if( iNaviTimer )
        {
        iNaviTimer->Cancel();
        }    
    delete iNaviTimer;
    
    if( iUpdateList )
        {
        iUpdateList->Cancel();
        }
    delete iUpdateList;

    if( iMSPropertyWatcher )
        {
        iMSPropertyWatcher->Stop();
        }
    delete iMSPropertyWatcher;

    if( iMemoryCardMonitor )
        {
        iMemoryCardMonitor->Cancel();
        }
    delete iMemoryCardMonitor;

    delete iListBox; // Deletes listbox object.
    }


// --------------------------------------------------------------------------
// CMSMainContainer::ConstructL()
// --------------------------------------------------------------------------
//
void CMSMainContainer::ConstructL( TRect aRect )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::ConstructL"));

    // Create property watcher for progress info
    iMSPropertyWatcher = CMSPropertyWatcher::NewL();
    // view will observe changes in property
    iMSPropertyWatcher->SetObserver(this);

    //create a mmc monitor to capture mmc event
    RFs& systemFs = CEikonEnv::Static()->FsSession();
    iMemoryCardMonitor = CMSMemoryCardMonitor::NewL( this, systemFs );
    iMemoryCardMonitor->StartMonitor();

    CreateWindowL();

    iListBox = new (ELeave) CAknDoubleLargeStyleListBox;

    SetListBoxFromResourceL(iListBox, R_MAIN_VIEW_LISTBOX);

    // Create icons
    CAknIconArray* icons = new (ELeave) CAknIconArray( EIconCount );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName() );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_sync,
                        EMbmMediaservantQgn_mserv_sync_mask );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_sync_stop,
                        EMbmMediaservantQgn_mserv_sync_stop_mask );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_to_home,
                        EMbmMediaservantQgn_mserv_to_home_mask );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_to_home_inactive,
                        EMbmMediaservantQgn_mserv_to_home_inactive_mask );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_from_home,
                        EMbmMediaservantQgn_mserv_from_home_mask );

    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_mserv_from_home_inactive,
                        EMbmMediaservantQgn_mserv_from_home_inactive_mask );

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(icons);

    CleanupStack::Pop(icons);

    // Container is listbox observer
    iListBox->SetListBoxObserver( this );
    // Set engine observer
    iAppUi->MSEngine()->SetObserver( this );

    SetRect(aRect);

    // Check if automatic sync/harvest is ongoing
    TCmServerState state;
    iAppUi->MSEngine()->ServerState( state );
    if ( state == ECmServerStateStoring ||
         state == ECmServerStateFilling ||
         state == ECmServerStateSearchingForServers )
        {
        iSyncRunning = ESyncRunning;
         }

    if ( state == ECmServerStateHarvesting )
        {
        iRefreshRunning = ETrue;
        }

    // From home secondary item text depends list selections
    iFillListSelected = CheckIfFillListSelected();
    
    // Add data to listbox
    UpdateListBoxDataL();

    if ( iSyncRunning != ESyncRunning &&
         iRefreshRunning == EFalse &&
         state != ECmServerStateDeleting )
        {
        // Set last sync time to navi pane
        SetNaviTextLastSyncL();
        }

    // Delete may be in progress so we have to inform user
    if ( state == ECmServerStateDeleting )
        {
        HBufC* naviText = StringLoader::LoadLC( R_MS_PREPROCESSING_TEXT );
        iView->SetNavigationPaneTextL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
        }

    // start listening property
    iMSPropertyWatcher->StartL();

    //Set current item
    iListBox->SetCurrentItemIndex(
                        iAppUi->ParameterAgent()->MainViewFocusIndex() );
    
    // Start timer for last sync time reading
    iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
    iPeriodic->Start( 0, KTimerInterval, TCallBack(TimerCallbackL,this) );

    // create navi text timer - don't start
    iNaviTimer = CPeriodic::NewL( CActive::EPriorityIdle );
    
    iUpdateList = CIdle::NewL( CActive::EPriorityIdle );
    
    ActivateL(); // Activates window. ( Ready to draw )
    }

// --------------------------------------------------------------------------
// CMSMainContainer::CountComponentControls()
// Returns number of components.
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }


// --------------------------------------------------------------------------
// CMSMainContainer::ComponentControl()
// Returns pointer to particular component.
// --------------------------------------------------------------------------
//
CCoeControl* CMSMainContainer::ComponentControl( TInt aIndex ) const

    {
    CCoeControl* retval = NULL;
    switch ( aIndex )
        {
        case 0:
            {
            retval = iListBox; // return a pointer to the listbox
            break;
            }
        default:
            {
            retval = NULL;
            break;
            }
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::OfferKeyEventL()
// Handles the key events.
// --------------------------------------------------------------------------
//
TKeyResponse CMSMainContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::OfferKeyEventL"));

    TKeyResponse response = EKeyWasNotConsumed;

    if ( aType != EEventKey )
        {
        response =  EKeyWasNotConsumed;
        }

    if ( iListBox )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }

    iView->SetCbaL( iListBox->CurrentItemIndex() );

    return response;
    }


// --------------------------------------------------------------------------
// CMSMainContainer::OpenSelectedListboxItemL
// --------------------------------------------------------------------------
//
void CMSMainContainer::OpenSelectedListboxItemL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::OpenSelectedListboxItemL"));
    
    HandleListBoxEventL( NULL, EEventEnterKeyPressed );
    }


// --------------------------------------------------------------------------
// CMSMainContainer::HandleListBoxEventL()
// Handles listbox event.
// --------------------------------------------------------------------------
void CMSMainContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::HandleListBoxEventL"));

    if ( ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed ) ||
         ( aEventType == MEikListBoxObserver::EEventItemClicked ) )
        {
        TInt selected = iListBox->CurrentItemIndex();

        switch ( selected )
            {
            case EStoreAndFill:
                {
                // refresh is not active
                if ( !iRefreshRunning )
                    {
                    if ( iSyncRunning == ESyncNotRunning )
                        {
                        StartSyncL();
                        }
                    else
                        {
                        // Cancel current asyncronous operation
                        iAppUi->MSEngine()->StopOperation();
                        iSyncRunning = ESyncNotRunning;
                        iView->ClearCurrentNaviPaneText();
                        UpdateListBoxDataL();
                        }
                    }
                 // refresh is ongoing
                 else
                    {
                    if ( iSyncRunning == ESyncWaiting )
                        {
                        iSyncRunning = ESyncNotRunning;
                        }
                    else
                        {
                        iSyncRunning = ESyncWaiting;
                        }
                    UpdateListBoxDataL();
                    }

                break;
                }
            case EStoreToNetwork:
                {
                if ( !iRefreshRunning && 
                     !iSyncRunning && 
                     !iPreprocessRunning )
                    {
                    iAppUi->ChangeViewL( KMSMainViewId,
                                         KMSStoreListViewId );
                    }
                else
                    {
                    TInt noteId = ( iRefreshRunning || iPreprocessRunning ?
                                    R_MS_ACCESS_ERROR_NOTE : 
                                    R_MS_ACCESS_ERROR_NOTE_SYNC );
                    ShowErrorNoteL( noteId );
                    }
                break;
                }
            case EFillToPhone:
                {
                if ( iAppUi->DriveSelectedL() )
                    {
                    if ( !iRefreshRunning &&
                         !iSyncRunning &&
                         !iPreprocessRunning )
                        {
                        iAppUi->ChangeViewL( KMSMainViewId, KMSFillViewId );
                        }
                    else
                        {
                        TInt noteId = ( iRefreshRunning ||
                                        iPreprocessRunning ?
                                        R_MS_ACCESS_ERROR_NOTE : 
                                        R_MS_ACCESS_ERROR_NOTE_SYNC );
                        ShowErrorNoteL( noteId );
                        }
                    }
                else
                    {
                    ShowErrorNoteL( R_MS_INSERT_MEMORY_CARD_TEXT );
                    }
                break;
                }
            default:
                {
                LOG(_L("[MediaServant]\t CMSMainContainer::\
                HandleListBoxEventL no item selected"));
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMSMainContainer::UpdateListBoxDataL()
// Updates listbox texts
// --------------------------------------------------------------------------
//
void CMSMainContainer::UpdateListBoxDataL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::UpdateListBoxDataL"));

    TCmServerState state;
    iAppUi->MSEngine()->ServerState( state );

    CDesCArray* itemArray = static_cast<CDesCArray*>
                                        (iListBox->Model()->ItemTextArray());

    if ( itemArray->Count() )
        {
        // delete all listbox items
        itemArray->Reset();
        }


    // Get file counts and sync time
    TInt itemsToBeStored( 0 );
    TInt itemsToBeFilled( 0 );
    TInt syncTime( 0 );
    TInt syncTimeStore( 0 );
    TInt syncTimeFill( 0 );

    iAppUi->MSEngine()->GetTransferInfo( ECmServiceStore,
                                         itemsToBeStored,
                                         syncTimeStore );

    iAppUi->MSEngine()->GetTransferInfo( ECmServiceFill,
                                         itemsToBeFilled,
                                         syncTimeFill );

    syncTime = syncTimeStore + syncTimeFill;

    // Create item strings
    HBufC* primaryText = NULL;
    HBufC* secondaryText = NULL;
    TInt icon = EIconSync;

    switch ( iSyncRunning )
        {
        default:
        // fall trough
        case ESyncNotRunning:
            {
            primaryText = StringLoader::LoadLC( R_MAIN_SYNC_NOW_TEXT );

            if ( itemsToBeStored > 0 || itemsToBeFilled > 0)
                {
                if ( syncTime/60 > 1 ) // 1 minute
                    {
                    secondaryText = StringLoader::LoadLC(
                                                    R_MS_SYNC_TIME_TEXT,
                                                    syncTime/60 );
                    // do number conversion
                    TPtr ptr = secondaryText->Des();
                    AknTextUtils::
                        DisplayTextLanguageSpecificNumberConversion( ptr );
                    }
                else
                    {
                    secondaryText = StringLoader::LoadLC(
                                               R_MS_SYNC_TIME_ONE_MIN_TEXT );
                    }

                }
            break;
            }
        case ESyncRunning:
            {
            primaryText = StringLoader::LoadLC( R_MAIN_STOP_SYNC_TEXT );
            icon = EIconSyncStop;
            break;
            }
        case ESyncWaiting:
            {
            primaryText = StringLoader::LoadLC( R_MAIN_STOP_SYNC_TEXT );
            secondaryText = StringLoader::LoadLC( R_MS_SYNC_WAITING_TEXT );
            icon = EIconSyncStop;
            break;
            }
        }

    AppendItemToListBoxArrayL( iListBox,
                               iListBox->Model(),
                               icon,
                               *primaryText,
                               *secondaryText );

    if ( secondaryText )
        {
        CleanupStack::PopAndDestroy( secondaryText );
        secondaryText = NULL;
        }

    if ( primaryText )
        {
        CleanupStack::PopAndDestroy( primaryText );
        primaryText = NULL;
        }


    // From Home -item
    primaryText = StringLoader::LoadLC( R_MAIN_FROM_HOME_TEXT );

    TBool driveSelected = iAppUi->DriveSelectedL();

    if ( driveSelected == EFalse )
        {
        secondaryText = StringLoader::LoadLC( R_MS_MEMORY_CARD_NEEDED_TEXT );
        }
    else if ( !iFillListSelected )
        {
        secondaryText = StringLoader::LoadLC( R_MS_LIST_NOTHING_SELECTED );
        }        
    else if ( itemsToBeFilled <= 0 )
        {
        secondaryText = StringLoader::LoadLC( R_MS_ALL_IN_SYNC_TEXT );
        }

    if ( iRefreshRunning ||
         iPreprocessRunning ||
         iSyncRunning == ESyncRunning ||
         driveSelected == EFalse )
        {
        AppendItemToListBoxArrayL( iListBox,
                                   iListBox->Model(),
                                   EIconFromHomeInactive,
                                   *primaryText,
                                   *secondaryText );
        }
    else
        {
        AppendItemToListBoxArrayL( iListBox,
                                   iListBox->Model(),
                                   EIconFromHome,
                                   *primaryText,
                                   *secondaryText );
        }

    if ( secondaryText )
        {
        CleanupStack::PopAndDestroy( secondaryText );
        secondaryText = NULL;
        }

    CleanupStack::PopAndDestroy( primaryText );
    primaryText = NULL;

    // To Home -item
    primaryText = StringLoader::LoadLC( R_MAIN_TO_HOME_TEXT );

    if ( itemsToBeStored < 1 )
        {
        secondaryText = StringLoader::LoadLC( R_MS_ALL_IN_SYNC_TEXT );
        }
    else if ( itemsToBeStored == 1 )
        {
        secondaryText = StringLoader::LoadLC( R_MS_SYNC_ONE_NEW_ITEM_TEXT );
        }
    else // > 1      
        {
        secondaryText = StringLoader::LoadLC( R_MS_SYNC_NEW_ITEMS_TEXT,
                                              itemsToBeStored );
        // do number conversion
        TPtr ptr = secondaryText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
        }

    if ( iRefreshRunning ||
         iPreprocessRunning ||
         iSyncRunning == ESyncRunning )
        {
        AppendItemToListBoxArrayL( iListBox,
                                   iListBox->Model(),
                                   EIconToHomeInactive,
                                   *primaryText,
                                   *secondaryText );
        }
    else
        {
        AppendItemToListBoxArrayL( iListBox,
                                   iListBox->Model(),
                                   EIconToHome,
                                   *primaryText,
                                   *secondaryText );
        }

    if ( secondaryText )
        {
        CleanupStack::PopAndDestroy( secondaryText );
        secondaryText = NULL;
        }

    CleanupStack::PopAndDestroy( primaryText );
    primaryText = NULL;


    iListBox->DrawNow();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSMainContainer::ReadyL( TCmService aService, TInt aError )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::ReadyL"));

    // Clears navi pane
    iView->ClearCurrentNaviPaneText();

    if( aError == KErrNotFound )
        {
        ShowErrorNoteL( R_MS_ERROR_DEVICES_NOT_FOUND );
        }
    else if( aError == KErrGeneral )
        {
        ShowErrorNoteL( R_MS_ERROR_SYNC_INTERRUPTED );
        }
    else if( aError == KErrDiskFull )
        {
        ShowErrorNoteL( R_MS_ERROR_MEMORY_FULL );
        }		
    else if ( aError && aError != KErrCancel )
        {
#ifdef __DEBUG
        ShowNoteL( R_ACTIVE_ERROR_NOTE_TEXT, aError );
#endif        
        iRefreshRunning = EFalse;
        iSyncRunning = ESyncNotRunning;
        }

    switch ( aService )
        {
        case ECmServiceDeleteMetadata:
            {
            iAppUi->MSEngine()->DeleteDeletedMediaserversL();
            // start harvest
            iAppUi->MSEngine()->ScanMediaServersL();
            break;
            }
        case ECmServiceHarvest:
            {
            if( iAppWizardRun )
                {
                LOG(_L("[MediaServant]\t CMSMainContainer::\
                ReadyL, HARVEST (Wizard)"));

                // refreshes also fill rules
                iAppUi->RefreshListsL();
                
                // "From home" secondary item text depends list selections
                // must be read here again because list selections change
                // after wizard run
                iFillListSelected = CheckIfFillListSelected();
                
                if ( iRefreshRunning && iSyncRunning == ESyncWaiting )
                    {
                    StartSyncL();
                    iRefreshRunning = EFalse;
                    }

                iRefreshRunning = EFalse;
                iAppWizardRun = EFalse;
                LOG(_L("[MediaServant]\t CMSMainContainer::ReadyL, \
                START PRE PRO"));
                
                // show navi pane text for 2 seconds
                ShowRefreshCompleteTextL();
                iNaviTimer->Start( KNaviTimerInterval,
                                  KNaviTimerInterval,
                                  TCallBack( NaviTimerCallbackL, this ) );
                break;
                }
            else
                {
                if ( iRefreshRunning && iSyncRunning == ESyncWaiting )
                    {
                    StartSyncL();
                    iRefreshRunning = EFalse;
                    }
                else
                    {
                    iRefreshRunning = EFalse;
                    // refreshes also fill rules and store item
                    iAppUi->RefreshListsL();
                    
                    // show navi pane text for 2 seconds
                    ShowRefreshCompleteTextL();
                    iNaviTimer->Start( KNaviTimerInterval,
                                     KNaviTimerInterval,
                                     TCallBack( NaviTimerCallbackL, this ) );
                    }
                }
            break;
            }

        case ECmServiceFill:
        // fall trough
        case ECmServiceStore:
            {
            iSyncRunning = ESyncNotRunning;
            SetNaviTextLastSyncL();
            iAppUi->RefreshListsL();
            break;
            }

        case ECmServicePreProcessingStore:
            {
            LOG(_L("[MediaServant]\t CMSMainContainer::ReadyL, \
            PRE PRO STORE"));

            if ( iSyncRunning == ESyncWaiting )
                {
                StartSyncL();
                }

            iPreprocessRunning = EFalse;
            iAppUi->RefreshListsL();
            break;
            }
        case ECmServicePreProcessingFill:
            {
            LOG(_L("[MediaServant]\t CMSMainContainer::ReadyL, \
            PRE PRO FILL"));

            // Set last sync time to navi pane
            SetNaviTextLastSyncL();

            iPreprocessRunning = EFalse;
            break;
            }

        case ECmServiceDeleteFilledFiles:
            {
            // Set last sync time to navi pane
            SetNaviTextLastSyncL();
            break;
            }

        default:
            {
            LOG(_L("[MediaServant]\t CMSMainContainer::ReadyL \
            service not found"));
            break;
            }
        }

    UpdateListBoxDataL();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::PropertyChangedL
// --------------------------------------------------------------------------
//
void CMSMainContainer::PropertyChangedL( TCmService aService,
                                    TInt aTotalItems,
                                    TInt aProcessedItems,
                                    TInt /*aItemsToBeProcessed*/ )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::PropertyChangedL"));

    switch ( aService )
        {
        case ECmServiceHarvest:
            {
            // Launching propertywatcher causes unwanted call of observer
            // (server previous state is returned
            // We want to show progress only when refresh is truly running
            if ( iRefreshRunning )
                {
                TInt value( 0 );
                if ( aTotalItems > 0 )
                    {
                    value = (TReal(aProcessedItems)/TReal(aTotalItems))*100;
                    }

                HBufC* progress = StringLoader::LoadLC(
                                   R_MS_NAVI_REFRESHING_TEXT, value );

                // do number conversion
                TPtr ptr = progress->Des();
                AknTextUtils::
                    DisplayTextLanguageSpecificNumberConversion( ptr );

                // update navi pane
                iView->SetNavigationPaneTextL( *progress );
                CleanupStack::PopAndDestroy( progress );
                }
            break;
            }
        case ECmServiceStore:
        // fall through
        case ECmServiceFill:
            {
            if ( aTotalItems > 0 && aProcessedItems > 0 )
                {
                // calculate procentual value
                TInt value = (TReal(aProcessedItems)/TReal(aTotalItems))*100;
                HBufC* progress = StringLoader::LoadLC(
                                   R_MS_NAVI_SYNC_TEXT, value );

                // do number conversion
                TPtr ptr = progress->Des();
                AknTextUtils::
                    DisplayTextLanguageSpecificNumberConversion( ptr );

                // update navi pane
                iView->SetNavigationPaneTextL( *progress );
                CleanupStack::PopAndDestroy( progress );
                }
            break;
            }
        case ECmServiceNone:
            {
            iSyncRunning = ESyncNotRunning;
            iRefreshRunning = EFalse;
            if ( iNaviTimer && !iNaviTimer->IsActive() )
                {
                SetNaviTextLastSyncL();    
                }
            
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSMainContainer::PropertyChangedL \
            service not found"));
            }
        }
    
    if ( iUpdateList && !iUpdateList->IsActive() )
        {
        iCurStep = EFirstStep;
        iRequestCt = 1;
        iUpdateList->Start( TCallBack( BackgroundUpdateListL, this ) );
        }
    else
        {
        iRequestCt++;
        LOG(_L("[MediaServant]\t S.L add to queue"));
        }
    }

// --------------------------------------------------------------------------
// CMSMainContainer::SetManualWizardInfo
// --------------------------------------------------------------------------
//
void CMSMainContainer::SetManualWizardInfo( TBool aAppWizardRun )
    {
    iAppWizardRun = aAppWizardRun;
    }


// --------------------------------------------------------------------------
// CMSMainContainer::ShowRefreshIndicationL
// --------------------------------------------------------------------------
//
void CMSMainContainer::ShowRefreshIndicationL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::ShowRefreshIndicationL"));

    iRefreshRunning = ETrue;
    // Start 0 %
    HBufC* naviText = StringLoader::LoadLC(
                                  R_MS_NAVI_REFRESHING_TEXT, 0 );

    // do number conversion
    TPtr ptr = naviText->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );

    UpdateListBoxDataL();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::StopRefreshL
// --------------------------------------------------------------------------
//
void CMSMainContainer::StopRefreshL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::StopRefreshL"));

    iRefreshRunning = EFalse;
    iSyncRunning = ESyncNotRunning;
    iAppUi->FillRuleController()->UpdateArrayL();
    UpdateListBoxDataL();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::StartSyncL
// --------------------------------------------------------------------------
//
void CMSMainContainer::StartSyncL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::StartSync"));

    iSyncRunning = ESyncRunning;
    TInt error = iAppUi->MSEngine()->ExecuteSyncL();

    if ( !error )
        {
        UpdateListBoxDataL();

        // set progress to 0%
        HBufC* progress = StringLoader::LoadLC(
                                       R_MS_NAVI_SYNC_TEXT, 0 );

        // do number conversion
        TPtr ptr = progress->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        iView->SetNavigationPaneTextL( *progress );
        CleanupStack::PopAndDestroy( progress );
        }
    else
        {
        iSyncRunning = ESyncNotRunning;
        }
    }

// --------------------------------------------------------------------------
// CMSMainContainer::StartMetadataDeleteL
// --------------------------------------------------------------------------
//
void CMSMainContainer::StartMetadataDeleteL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::StartMetadataDeleteL"));

    iAppUi->MSEngine()->DeleteMetadataL();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::SelectedItemIndex
// returns current item index
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::SelectedItemIndex()
    {
    return iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CMSMainContainer::ShowErrorNoteL
// Shows error note
// --------------------------------------------------------------------------
//
void CMSMainContainer::ShowErrorNoteL( TInt aTextResource )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::ShowErrorNoteL"));
    
    HBufC* errorText = StringLoader::LoadLC( aTextResource );
    CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote();
    dlg->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CMSMainContainer::SetNaviTextLastSyncL
// Sets last synchronization time to navi pane
// --------------------------------------------------------------------------
//
void CMSMainContainer::SetNaviTextLastSyncL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::SetNaviTextLastSyncL"));
    
    TTimeIntervalMinutes syncMinutes;
    TTimeIntervalHours syncHours;
    TTimeIntervalDays syncDays;

    TTime lastSync = iAppUi->MSEngine()->LastSyncTime();

    if ( lastSync.Int64() > 0 )
        {
        // Get current time
        TTime time;
        time.HomeTime();

        time.MinutesFrom( lastSync, syncMinutes );
        time.HoursFrom( lastSync, syncHours );
        syncDays = time.DaysFrom( lastSync );


        HBufC* naviText = NULL;

        TInt days = syncDays.Int();
        TInt hours = syncHours.Int();            
        TInt mins = syncMinutes.Int();            
        if ( days > 0 )
            {
            naviText = ( days == 1 ? StringLoader::LoadLC(
                                    R_MS_LAST_SYNC_DAY ) :
                                     StringLoader::LoadLC(
                                    R_MS_LAST_SYNC_DAYS, days ));
            }        
        else if ( hours > 0 )
            {
            naviText = ( hours == 1 ? StringLoader::LoadLC(
                                    R_MS_LAST_SYNC_HOUR ) :
                                     StringLoader::LoadLC(
                                    R_MS_LAST_SYNC_HOURS, hours ));
            }          
        else if ( mins > 0 )
            {
            naviText = ( mins == 1 ? StringLoader::LoadLC(
                                   R_MS_LAST_SYNC_MIN ) :
                                     StringLoader::LoadLC(
                                   R_MS_LAST_SYNC_MINS, mins ));
            }
        else
            {
            // do nothing - leave navi pane empty
            }

        if ( naviText )
            {
            // do number conversion
            TPtr ptr = naviText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            iView->SetNavigationPaneTextL( *naviText );
            CleanupStack::PopAndDestroy( naviText );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSMainContainer::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSMainContainer::SizeChanged()
    {
    // container control resize code.
    if ( iListBox )
        {
        iListBox->SetRect( Rect() ); // Set rectangle of listbox.
        }
    }

// --------------------------------------------------------------------------
// CMSMainContainer::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSMainContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// --------------------------------------------------------------------------
// CMSMainContainer::TimerCallbackL()
// Periodic timer callback
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::TimerCallbackL(TAny* aObject)
    {        
    CMSMainContainer* mainContainer =
                            static_cast<CMSMainContainer*>(aObject);
    TCmServerState state;
    mainContainer->iAppUi->MSEngine()->ServerState( state );
    if ( state == ECmServerStateIdle )
        {
        mainContainer->SetNaviTextLastSyncL();
        }
    return 0;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::NaviTimerCallbackL()
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::NaviTimerCallbackL(TAny* aObject)
    {        
    CMSMainContainer* mainContainer =
                            static_cast<CMSMainContainer*>(aObject);
    TCmServerState state;
    mainContainer->iAppUi->MSEngine()->ServerState( state );
    
    // Clear old text
    mainContainer->iView->ClearCurrentNaviPaneText();
    
    if ( state == ECmServerStateIdle )
        {
        mainContainer->SetNaviTextLastSyncL();
        }
    
    mainContainer->iNaviTimer->Cancel();
    
    return 0;
    }
    
// --------------------------------------------------------------------------
// CMSMainContainer::BackgroundUpdateListL
// Static callback method for update list box data
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::BackgroundUpdateListL( TAny* aObject )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::BackgroundUpdateListL"));
    TInt reslt = ( ( CMSMainContainer* )aObject )->UpdateListByStepL();
    return reslt;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::UpdateListByStepL
// Update list box data steps
// --------------------------------------------------------------------------
//
TInt CMSMainContainer::UpdateListByStepL()
    {
    TRACE(Print(_L("[MediaServant]\t CMSMainContainer:UpdateListByStepL \
        step: %d %d"), iCurStep, iUpdateList->IsActive()));

    // Get file counts and sync time
    static TInt staItemsToBeStored( 0 );
    static TInt staItemsToBeFilled( 0 );
    static TInt staSyncTime( 0 );
    static TInt staSyncTimeStore( 0 );
    static TInt staSyncTimeFill( 0 );
    static TBool staDriveSelected( ETrue );

    switch ( iCurStep )
        {
        case EFirstStep:
            {
            iAppUi->MSEngine()->GetTransferInfo( ECmServiceStore,
                                                 staItemsToBeStored,
                                                 staSyncTimeStore );
            break;
            }
        case ESecondStep:
            {
            iAppUi->MSEngine()->GetTransferInfo( ECmServiceFill,
                                                 staItemsToBeFilled,
                                                 staSyncTimeFill );
            break;
            }
        case EThirdStep:
            {
            staDriveSelected = iAppUi->DriveSelectedL();
            break;
            }
        case EForthStep:
        //fall through
        default:
            {
            TInt icon = EIconSync;
            // Create item strings
            HBufC* primaryText = NULL;
            HBufC* secondaryText = NULL;
            
            CDesCArray* itemArray = static_cast<CDesCArray*>
                                        (iListBox->Model()->ItemTextArray());
            if ( itemArray->Count() )
                {
                // delete all listbox items
               itemArray->Reset();
                }
            staSyncTime = staSyncTimeStore + staSyncTimeFill;
            //home-sync item
            switch ( iSyncRunning )
                {
                default:
                // fall trough
                case ESyncNotRunning:
                    {
                    primaryText = StringLoader::LoadLC(
                                      R_MAIN_SYNC_NOW_TEXT );
                    if ( staItemsToBeStored > 0 || staItemsToBeFilled > 0 )
                        {
                        if ( staSyncTime/60 > 1 ) // 1 minute
                            {
                            secondaryText = StringLoader::LoadLC(
                                                R_MS_SYNC_TIME_TEXT,
                                                staSyncTime/60 );
                            // do number conversion
                            TPtr ptr = secondaryText->Des();
                            AknTextUtils::
                               DisplayTextLanguageSpecificNumberConversion( ptr );
                            }
                        else
                            {
                            secondaryText = StringLoader::LoadLC(
                                                R_MS_SYNC_TIME_ONE_MIN_TEXT );
                            }
                        }
                    break;
                    }
                case ESyncRunning:
                    {
                    primaryText = StringLoader::LoadLC(
                                      R_MAIN_STOP_SYNC_TEXT );
                    icon = EIconSyncStop;
                    break;
                    }
                case ESyncWaiting:
                    {
                    primaryText = StringLoader::LoadLC( 
                                      R_MAIN_STOP_SYNC_TEXT );
                    secondaryText = StringLoader::LoadLC(
                                        R_MS_SYNC_WAITING_TEXT );
                    icon = EIconSyncStop;
                    break;
                    }
                }
            AppendItemToListBoxArrayL( iListBox,
                                       iListBox->Model(),
                                       icon,
                                       *primaryText,
                                       *secondaryText );

            if ( secondaryText )
                {
                CleanupStack::PopAndDestroy( secondaryText );
                secondaryText = NULL;
                }

            if ( primaryText )
                {
                CleanupStack::PopAndDestroy( primaryText );
                primaryText = NULL;
                }
            
            // From Home -item
            primaryText = StringLoader::LoadLC( R_MAIN_FROM_HOME_TEXT );
            
            if ( staDriveSelected == EFalse )
                {
                secondaryText = StringLoader::LoadLC( 
                                    R_MS_MEMORY_CARD_NEEDED_TEXT );
                }
            else if ( !iFillListSelected )
                {
                secondaryText = StringLoader::LoadLC( 
                                    R_MS_LIST_NOTHING_SELECTED );
                }
            else if ( staItemsToBeFilled <= 0 )
                {
                secondaryText = StringLoader::LoadLC( 
                                    R_MS_ALL_IN_SYNC_TEXT );
                }

            if ( iRefreshRunning ||
                 iPreprocessRunning ||
                 iSyncRunning == ESyncRunning ||
                 staDriveSelected == EFalse )
                {
                AppendItemToListBoxArrayL( iListBox,
                                           iListBox->Model(),
                                           EIconFromHomeInactive,
                                           *primaryText,
                                           *secondaryText );
                }
            else
                {
                AppendItemToListBoxArrayL( iListBox,
                                           iListBox->Model(),
                                           EIconFromHome,
                                           *primaryText,
                                           *secondaryText );
                }

            if ( secondaryText )
                {
                CleanupStack::PopAndDestroy( secondaryText );
                secondaryText = NULL;
                }

            CleanupStack::PopAndDestroy( primaryText );
            primaryText = NULL;

            // To Home -item
            primaryText = StringLoader::LoadLC( R_MAIN_TO_HOME_TEXT );
          

            if ( staItemsToBeStored < 1 )
                {
                secondaryText = StringLoader::LoadLC(
                                            R_MS_ALL_IN_SYNC_TEXT );
                }
            else if ( staItemsToBeStored == 1 )
                {
                secondaryText = StringLoader::LoadLC(
                                            R_MS_SYNC_ONE_NEW_ITEM_TEXT );
                }
            else // > 1      
                {
                secondaryText = StringLoader::LoadLC(
                                            R_MS_SYNC_NEW_ITEMS_TEXT,
                                            staItemsToBeStored );
                // do number conversion
                TPtr ptr = secondaryText->Des();
                AknTextUtils::
                    DisplayTextLanguageSpecificNumberConversion( ptr );
                }

            if ( iRefreshRunning ||
                 iPreprocessRunning ||
                 iSyncRunning == ESyncRunning )
                {
                AppendItemToListBoxArrayL( iListBox,
                                           iListBox->Model(),
                                           EIconToHomeInactive,
                                           *primaryText,
                                           *secondaryText );
                }
            else
                {
                AppendItemToListBoxArrayL( iListBox,
                                           iListBox->Model(),
                                           EIconToHome,
                                           *primaryText,
                                           *secondaryText );
                }

            if ( secondaryText )
                {
                CleanupStack::PopAndDestroy( secondaryText );
                secondaryText = NULL;
                }
            CleanupStack::PopAndDestroy( primaryText );
            primaryText = NULL;
            iListBox->DrawNow();
            break;
            }
        }
    TInt theValue = ( TInt )iCurStep;
    theValue++;
    iCurStep = ( TMSUpdateStep )theValue;
    
    //complete one refresh request
    TInt reslt( 0 );
    if( iCurStep == EEndStep )
        {
        iRequestCt--;
        if( iRequestCt )
            {
            iCurStep = EFirstStep;
            }
        //if still have request,continue
        reslt = iRequestCt;
        }
    //not the last step, contiue
    else
        {
        reslt = ETrue;
        }
    return reslt;
    }

// ---------------------------------------------------------------------------
// CMSMainContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSMainContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_MAIN_VIEW;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::MemoryCardChangedL
// Callback to handle mmc insert
// --------------------------------------------------------------------------
//
void CMSMainContainer::MemoryCardChangedL( TMemoryCardEvent aEvent )
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::MemoryCardChangedL start"));
    switch ( aEvent )
        {
        case EMemoryCardInsert:
            {
            if( !iAppUi->DriveSelectedL() )
                {
                iView->CheckMemoryCardL();
                }
            UpdateListBoxDataL();
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSMainContainer::MemoryCardChangedL \
            event not found"));
            break;
            }
        }
    LOG(_L("[MediaServant]\t CMSMainContainer::MemoryCardChangedL end"));
    }

// --------------------------------------------------------------------------
// CMSMainContainer::CheckIfFillListSelected
// --------------------------------------------------------------------------
//
TBool CMSMainContainer::CheckIfFillListSelected()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::CheckIfFillListSelected"));
    
    TBool listSelected( EFalse );
    RPointerArray<CMSFillList>* listArray =
                        iAppUi->FillRuleController()->FillListArray();
    
    for ( TInt i = 0; i < listArray->Count(); i++  )
        {
        CCmFillRule* list = (*listArray)[i]->List();
        if ( list->Selected() == ECmSelected )
            {
            listSelected = ETrue;
            i = listArray->Count(); // break loop
            }
        }
    return listSelected;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::ShowRefreshCompleteTextL
// --------------------------------------------------------------------------
//
void CMSMainContainer::ShowRefreshCompleteTextL()
    {
    LOG(_L("[MediaServant]\t CMSMainContainer::ShowRefreshCompleteTextL"));
    
    HBufC* naviText = StringLoader::LoadLC( R_MS_NAVI_CONTENT_REFRESHED );
    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );    
    }
// End of File

