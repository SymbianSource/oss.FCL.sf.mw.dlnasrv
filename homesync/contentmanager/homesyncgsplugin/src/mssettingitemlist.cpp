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
* Description:  CMSSettingItemList class implementation
*
*/


#include <msgspluginsrc.rsg>
#include <commdb.h> // for RProperty
#include <StringLoader.h> // for StringLoader
#include <centralrepository.h> // for profile
#include <ProfileEngineSDKCRKeys.h> // for profile
#include <driveinfo.h>

#include "cmsettingsfactory.h"
#include "cmcommonutils.h"
#include "cmsettings.h"
#include "cmdriveinfo.h"
#include "mserv.hlp.hrh"
#include "msengine.h"
#include "mssettingitemlist.h"
#include "mssettingitemmemory.h" // mmc name
#include "mssettingitemdevices.h" // source devices
#include "mssettingsview.h"
#include "msconstants.h"
#include "cmmediaserverfull.h"
#include "msdebug.h"

// CONSTANTS
const TInt KOfflineProfile = 5;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CMSSettingItemList::NewL()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSSettingItemList* CMSSettingItemList::NewL( TInt aResourceId,
                                              CMSSettingsView& aView )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::NewL"));

    CMSSettingItemList* self = CMSSettingItemList::NewLC( aResourceId, 
	                                                      aView );

	CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::NewLC()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSSettingItemList* CMSSettingItemList::NewLC( TInt aResourceId,
                                              CMSSettingsView& aView )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::NewLC"));

    CMSSettingItemList* self = new ( ELeave ) CMSSettingItemList(
                                                aView );
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId );
    return self;
    }
	
// --------------------------------------------------------------------------
// CMSSettingItemList::CMSSettingItemList()
// --------------------------------------------------------------------------
//
CMSSettingItemList::CMSSettingItemList( CMSSettingsView& aView )
    : iView( aView )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::CMSSettingItemList"));
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::~CMSSettingItemList()
// --------------------------------------------------------------------------
//
CMSSettingItemList::~CMSSettingItemList()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::~CMSSettingItemList"));

	if ( iStoredServers ) 
		{
		iStoredServers->ResetAndDestroy();
	    iStoredServers->Close();
	    delete iStoredServers;		
		}

    if ( iMSEngine )
        {
        delete iMSEngine;    
        }    
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::ConstructL()
// Second phase constructor.
// --------------------------------------------------------------------------
//
void CMSSettingItemList::ConstructL( TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::ConstructL"));

    // Create engine
    iMSEngine = CMSEngine::NewL();

    // Set engine observer
    iMSEngine->SetObserver( this );
    // Read memory manager setting
    iMemoryManager =
           iMSEngine->ServiceState( ECmServiceMemoryManager );

    iSync = iMSEngine->ServiceState( ECmServiceContentManager );

    // Get server list
    iStoredServers = iMSEngine->GetMediaServersL();

    CAknSettingItemList::ConstructFromResourceL( aResourceId );

    // sets memory selection setting visibility
    SetMemorySelectionL();

    iListBox = ListBox();
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::OpenSelectedListboxItemL
// --------------------------------------------------------------------------
//
void CMSSettingItemList::OpenSelectedListboxItemL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::OpenSelectedListboxItemL"));

    HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
    }


// --------------------------------------------------------------------------
// CMSSettingItemList::HandleListBoxEventL()
// --------------------------------------------------------------------------
//
void CMSSettingItemList::HandleListBoxEventL( CEikListBox *aListBox,
                                              TListBoxEvent aEventType)
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::HandleListBoxEventL"));

    CAknSettingItemList::HandleListBoxEventL( aListBox, aEventType );

    // UI selection is stored to actual settings variable
    StoreSettingsL();

    TInt listBoxItemIndex = iListBox->CurrentItemIndex();
    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt realIndex = itemArray->ItemIndexFromVisibleIndex(
                                                      listBoxItemIndex );

    if ( realIndex == EMSSettingsSync )
        {
        iMSEngine->SetServiceState( ECmServiceContentManager,
                                             iSync );
        // inform plugin about changed setting
        TCmProgressInfo info;
        info.iProcessedItems = 0,
        info.iTotalItems = 0;
        info.iService = ECmServiceNone;

        TCmProgressInfoPckg transferInfoPckg( info );

        RProperty::Set( KCmPropertyCat, KCmProperty,
        transferInfoPckg );

        // we aren't allowed to set wlan scanning in offline mode
        if ( GetCurrentProfileL() != KOfflineProfile )
            {
            // Set wlan scanning parameters
            TInt interval = ( iSync == ECmServiceStateDisabled ?
                    KWlanScanNetworkNever : KWlanScanNetworkInterval60 );

            CmCommonUtils::SetWlanScanL( interval );
            }
        }

    else if ( realIndex == EMSMemoryManager )
        {
        iMSEngine->SetServiceState( ECmServiceMemoryManager,
                                             iMemoryManager );
        }
    else
        {
        LOG(_L("[MediaServant]\t CMSSettingItemList::\
        HandleListBoxEventL no such item"));
        }
    }

// --------------------------------------------------------------------------
// CAknSettingItem* CMSSettingItemList::CreateSettingItemL(TInt aIdentifier)
// Takes care of creating actual setting items.
// --------------------------------------------------------------------------
//
CAknSettingItem* CMSSettingItemList::CreateSettingItemL( TInt aIdentifier )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::CreateSettingItemL"));

    CAknSettingItem* settingItem = NULL;

    // setting item is created according to aIdentifier
    switch ( aIdentifier )
        {
        case EMSSettingsSync:
            {
            LOG(_L("[MediaServant]\t List Item: EMSSettingsSync"));
            settingItem = new (ELeave ) CAknEnumeratedTextPopupSettingItem(
                                         aIdentifier,
                                         reinterpret_cast<TInt&> (iSync) );
            break;
            }
        
        case EMSSettingsSourceDevices:
            {
            LOG(_L("[MediaServant]\t List Item: EMSSettingsSourceDevices"));
            settingItem = CMSSettingItemDevices::NewL (
                                            aIdentifier,
                                            iDummyText,
                                            *iMSEngine,
                                            *iStoredServers );
            break;
            }

        case EMSSettingsTargetDevices:
            {
            LOG(_L("[MediaServant]\t List Item: EMSSettingsTargetDevices"));
            settingItem = CMSSettingItemDevices::NewL (
                                            aIdentifier,
                                            iDummyText,
                                            *iMSEngine,
                                            *iStoredServers,
                                            ETrue );
            break;
            }
            
        case EMSSettingsMemory:
            {
            LOG(_L("[MediaServant]\t List Item: EMSSettingsMMC"));
            settingItem = CMSSettingItemMemory::NewL (
                                            aIdentifier,
                                            iDummyText,
                                            *iMSEngine,                                            
                                            *this );
            break;
            }

        case EMSMemoryManager:
            {
            LOG(_L("[MediaServant]\t List Item: EMSMemoryManager"));
            settingItem = new ( ELeave ) CAknBinaryPopupSettingItem(
                                  aIdentifier,
                                  reinterpret_cast<TBool&> (iMemoryManager) );
            break;
            }

        default:
            {
            LOG(_L("[MediaServant]\t No list item found"));
            break;
            }
        }
    return settingItem;
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::EditItemL()
// Called by framework when item is opened
// --------------------------------------------------------------------------
//
void  CMSSettingItemList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );

    if ( iListBox->CurrentItemIndex() == EMSSettingsSourceDevices )
        {
        CMSSettingItemDevices* item = static_cast<CMSSettingItemDevices*>
                               (SearchItemById( EMSSettingsSourceDevices ));

        if ( item->SettingAccepted() )
            {
            TCmServerState state( ECmServerStateIdle );
            // List of unselected servers
            RPointerArray<CCmMediaServerFull> unselectedServers;

            iMSEngine->ServerState( state );
            if ( state != ECmServerStateIdle )
                {
                iMSEngine->StopOperation();
                }
            iView.ClearCurrentNaviPaneText();

            iMSEngine->DeleteMetadataL();

            HBufC* naviText = StringLoader::LoadLC(
                                 R_MS_PREPROCESSING_TEXT );
            iView.SetNavigationPaneTextL( *naviText );
            CleanupStack::PopAndDestroy( naviText );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::SearchItemById
// Searches item from the itemarray by given ID
// --------------------------------------------------------------------------
//
CAknSettingItem* CMSSettingItemList::
                SearchItemById( const TMSSettingItems aId ) const
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::SearchItemById"));

    CAknSettingItem* item = NULL;
    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt count = itemArray->Count();

    for ( TInt index = 0; index < count; index++ )
        {
        if ( itemArray->At( index )->Identifier() == aId )
            {
            item = itemArray->At( index );
            // end loop
            index = count;
            }
        }
    return item;
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSSettingItemList::SizeChanged()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::SizeChanged"));

    ListBox()->SetRect( Rect() ); // Set rectangle of listbox.
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSSettingItemList::HandleResourceChange( TInt aType )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::HandleResourceChange"));

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView.ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
#ifdef __DEBUG
void CMSSettingItemList::ReadyL( TCmService aService, TInt aError )
#else
void CMSSettingItemList::ReadyL( TCmService aService, TInt /*aError*/ )
#endif
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::ReadyL"));
#ifdef __DEBUG
    TRACE(Print(_L("[MediaServant]\t CMSSettingItemList::\
    ReadyL error %d"), aError));
#endif


    switch ( aService )
        {
        case ECmServiceDeleteMetadata:
            {
            iMSEngine->ExecuteFillPreProcessingL();
            break;
            }
        case ECmServicePreProcessingFill:
            {
            iView.ClearCurrentNaviPaneText();
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemList::MSEngine()
// --------------------------------------------------------------------------
//
CMSEngine* CMSSettingItemList::MSEngine()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::MSEngine"));

    return iMSEngine;
    }

// --------------------------------------------------------------------------
// CMSMainContainer::GetCurrentProfileL()
// Reads current profile
// --------------------------------------------------------------------------
//
TInt CMSSettingItemList::GetCurrentProfileL() const
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::GetCurrentProfileL"));

    TInt profile( 0 );
    CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
    repository->Get( KProEngActiveProfile, profile );
    CleanupStack::PopAndDestroy( repository );

    return profile;
    }


// ---------------------------------------------------------------------------
// CMSSettingItemList::SetMemorySelectionL
// ---------------------------------------------------------------------------
//
void CMSSettingItemList::SetMemorySelectionL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::SetMemorySelectionL"));
    
    TBool memoryCardFound( EFalse );
    RPointerArray<CCmDriveInfo> driveArray;
    CleanupClosePushL( driveArray );
    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();

    // get available drives
    settings->DriveListL( ControlEnv()->FsSession(), driveArray );
	settings->Close();
    CleanupStack::Pop(); // settings

    TInt driveCount = driveArray.Count();
    for ( TInt i = 0; i < driveCount; i++ )
        {
        if ( driveArray[i]->DriveType() == DriveInfo::EDriveRemovable )
            {
            memoryCardFound = ETrue;
            i = driveCount; // break loop
            }
        }
    if ( !memoryCardFound )
        {
        CAknSettingItem* item = SearchItemById( EMSSettingsMemory );
        item->SetHidden( ETrue);
        // visibility changed
        HandleChangeInItemArrayOrVisibilityL();
        }

    driveArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &driveArray );
    }

// ---------------------------------------------------------------------------
// CMSSettingItemList::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSSettingItemList::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSSettingItemList::GetHelpContext"));

    aContext.iMajor = KUidMediaServant;
    aContext.iContext = KMSERV_HLP_SYNC_SETTINGS;
    }

// End of File
