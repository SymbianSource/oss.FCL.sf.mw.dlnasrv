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
* Description:      Setting list implementation for upnp GS plugin
*
*/






// INCLUDE FILES
#include <aknnotewrappers.h>       // CAknInformationNote
#include <pathinfo.h> 
#include <upnpstring.h>

#include "upnpsettingsengine.h"
#include "upnpsettingsengine.hrh"

#include "upnpgsplugin.hrh"
#include "upnpappsettingslist.h"
#include "upnpappsettingitemhomeiap.h"
#include "upnpappsettingitemmydevicename.h"
#include <upnpgspluginrsc.rsg>

#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include "upnpsettingitemmemoryselection.h"

_LIT( KComponentLogfile, "upnpgsplugin.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::CUPnPAppSettingsList
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAppSettingsList::CUPnPAppSettingsList( TBool& aSharingState )
    : iSharingState( aSharingState )
    {
    __LOG("[gsplugin]\tCUPnPAppSettingsList::CUPnPAppSettingsList");
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppSettingsList::ConstructL( TInt aResourceId )
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::ConstructL");

    iName = HBufC8::NewL( KMaxFriendlyNameLength );

    iSettingsEngine = CUPnPSettingsEngine::NewL();
    
    if( aResourceId != 0 )
        {
        CAknSettingItemList::ConstructFromResourceL( aResourceId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAppSettingsList* CUPnPAppSettingsList::NewL( TBool& aSharingState,
                                                  TInt aResourceId )
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::NewL");

    CUPnPAppSettingsList* self = new (ELeave) CUPnPAppSettingsList(
        aSharingState );
    CleanupStack::PushL(self);
    self->ConstructL( aResourceId );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::~CUPnPAppSettingsList
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAppSettingsList::~CUPnPAppSettingsList()
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList:: \
            ~CUPnPAppSettingsList");
    delete iName;
    delete iSettingsEngine;
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::CreateSettingItemL
// Creates list items
// --------------------------------------------------------------------------
//
CAknSettingItem* CUPnPAppSettingsList::CreateSettingItemL( TInt aIdentifier )
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::CreateSettingItemL");

    LoadSettingsL();

    CAknSettingItem* settingItem = NULL;
    switch (aIdentifier)
        {
    case EUPnPGSPluginItemIAP:
        {
        settingItem = UPnPAppSettingItemHomeIAP::NewL( 
                aIdentifier,
                iIapSetting, iIapId, 
                iWapId, 
                iSharingState );
        break;
        }
    case EUPnPGSPluginItemMyDeviceName:
        {
        settingItem = UPnPAppSettingItemMyDeviceName::NewL(
            aIdentifier,
            iFriendlyName,
            iSharingState );
        break;
        }
    case EUPnPGSPluginItemCopyLocation:
        {
        settingItem = CUPnPSettingItemMemorySelection::NewL(
            aIdentifier, iCopyLocationDrive);
        }
        break;

    default:
        {
        __LOG("[gsplugin]\t CUPnPAppSettingsList:: \
                    CreateSettingItemL No such item!");
        break;
        }
        }

    return settingItem;
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::EditItemL
// Called when setting list item is modified
// --------------------------------------------------------------------------
//
void  CUPnPAppSettingsList::EditItemL (TInt aIndex, TBool aCalledFromMenu)
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::EditItemL");
    
    // Validates the access point.
    TBool validiap = EFalse;
    TRAPD(err, validiap = CUPnPSettingsEngine::IsAccessPointValidL( iIapId ) );
    if( err || !validiap )
        {
        iIapId = EUPnPSettingsEngineIAPIdNone;
        iIapSetting = EUPnPSettingsEngineIAPItemNone;
        iWapId = EUPnPSettingsEngineIAPItemNone;
        }

    if ( aIndex != EUPnPGSPluginItemCopyLocation )
        {
        CAknSettingItemList::EditItemL (aIndex, aCalledFromMenu);
        }

    StoreSettingsL();

    switch (aIndex)
        {
    case EUPnPGSPluginItemIAP:
        {
        // if sharing is on, nothing is done
        if ( !iSharingState )
            {
            iSettingsEngine->SetAccessPoint(iIapId);
            iSettingsEngine->SetAccessPointSetting(iIapSetting);
            iSettingsEngine->SetWapId(iWapId);
            }
        break;
        }
    case EUPnPGSPluginItemMyDeviceName:
        {
        // if sharing is on, nothing is done
        if ( !iSharingState )
            {
            HBufC8* tmpStr = NULL;    
            tmpStr = UpnpString::FromUnicodeL( iFriendlyName );
            CleanupStack::PushL( tmpStr );

            User::LeaveIfError(
                iSettingsEngine->SetLocalFriendlyName( tmpStr->Des() ));
            CleanupStack::PopAndDestroy( tmpStr );
            }
        break;
        }
    case EUPnPGSPluginItemCopyLocation:
        {
        // if sharing is on, nothing is done
        if ( !iSharingState )
            {
            CAknSettingItemList::EditItemL ( aIndex, aCalledFromMenu );
            StoreSettingsL();
            iSettingsEngine->SetCopyLocationL( iCopyLocationDrive );
            }
        else
            {
            CAknInformationNote* note = new (ELeave)
                CAknInformationNote;
            HBufC* noteText = CCoeEnv::Static()->AllocReadResourceLC(
                R_QTN_IUPNP_IAP_TURN_SHARING_OFF);
            note->ExecuteLD(*noteText);
            CleanupStack::PopAndDestroy(noteText);
            }

        break;
        }
    default:
        {
        __LOG("[gsplugin]\t CUPnPAppSettingsList::EditItemL \
                    No such item!");
        break;
        }
        }
    StoreSettingsL();
    // load settings to screen
    CAknSettingItemList::LoadSettingsL ();
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::LoadDataFromEngineL
// Loads data from Central Repository to member variables
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void  CUPnPAppSettingsList::LoadDataFromEngineL()
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::\
        LoadDataFromEngineL begin");

    // Get friendly name
    TPtr8 ptr( iName->Des() );
    iSettingsEngine->GetLocalFriendlyName( ptr );

    HBufC* unicodeFriendlyName = UpnpString::ToUnicodeL( ptr );
    CleanupStack::PushL( unicodeFriendlyName );
    iFriendlyName.Copy( unicodeFriendlyName->Des() );
    CleanupStack::PopAndDestroy( unicodeFriendlyName );

    iSettingsEngine->GetAccessPoint( iIapId );
    iSettingsEngine->GetAccessPointSetting(iIapSetting);

    iSettingsEngine->GetWapId(iWapId);

    TInt error( KErrNone );

    TRAP(error, iSettingsEngine->GetCopyLocationDriveL( iCopyLocationDrive ) )

    // Something wrong in getting copy location. It defaults to 
    // drive C and hopes that setting copy location succeeds
    if( error != KErrNone )
        {
        iCopyLocationDrive = EDriveC;
        }
    
    // check if the IAP id is valid
    if ( !CUPnPSettingsEngine::IsAccessPointValidL( iIapId ) )
        {
        // Resets all variables related to iap.
        iIapId = EUPnPSettingsEngineIAPIdNone;
        iIapSetting = EUPnPSettingsEngineIAPItemNone;
        iWapId = EUPnPSettingsEngineIAPItemNone;

        // set iap id
        iSettingsEngine->SetAccessPoint( iIapId );
        // set iap UI setting
        iSettingsEngine->SetAccessPointSetting( iIapSetting );
        // set iap wap id (needed for focus in iap selection component)
        iSettingsEngine->SetWapId( iWapId );
        }

    //load settings to screen
    CAknSettingItemList::LoadSettingsL();
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::HandleResourceChange
// --------------------------------------------------------------------------
//
void CUPnPAppSettingsList::HandleResourceChange( TInt aType )
    {
    __LOG("[gsplugin]\t CUPnPAppSettingsList::\
HandleResourceChange");

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPaneRect );

        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(
            TRect( TPoint( 0, 0 ), mainPaneRect.Size() ),
            AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );

        ListBox()->SetRect( layoutRect.Rect() );
        }

    // Base call
    CAknSettingItemList::HandleResourceChange( aType );
    }

//  End of File
