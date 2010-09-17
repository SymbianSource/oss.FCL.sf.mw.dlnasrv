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
* Description:  CMSSettingItemMemory class implementation
*
*/


#include <msgspluginsrc.rsg>
#include <StringLoader.h> // StringLoader
#include <akncheckboxsettingpage.h> // CSelectionItemList
#include <driveinfo.h>

#include "cmsettingsfactory.h"
#include "cmsettings.h"
#include "cmdriveinfo.h"
#include "mssettingitemmemory.h"
#include "mssettingitemlist.h"
#include "msconstants.h"
#include "msengine.h"
#include "msdebug.h"

// CONSTANTS
const TInt KArrayGranularity = 2; // memory list size

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CMSSettingItemMemory::NewL
// --------------------------------------------------------------------------
//
CMSSettingItemMemory* CMSSettingItemMemory::NewL(
                                TInt aIdentifier,
                                TDes& aText,
                                CMSEngine& aMSEngine,
                                CMSSettingItemList& aParent )
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::NewL"));

    CMSSettingItemMemory* self = CMSSettingItemMemory::NewLC(
                                                    aIdentifier,
                                                    aText,
                                                    aMSEngine,
                                                    aParent );

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::NewLC
// --------------------------------------------------------------------------
//
CMSSettingItemMemory* CMSSettingItemMemory::NewLC(
                                TInt aIdentifier,
                                TDes& aText,
                                CMSEngine& aMSEngine,
                                CMSSettingItemList& aParent )
    {
    LOG(_L("[MediaServant]\t CMSStoreServersSetting::NewLC"));

    CMSSettingItemMemory* self = new (ELeave) CMSSettingItemMemory(
                                                    aIdentifier,
                                                    aText,
                                                    aMSEngine,
                                                    aParent );

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

	
// --------------------------------------------------------------------------
// CMSSettingItemMemory::ConstructL
// --------------------------------------------------------------------------
//
void CMSSettingItemMemory::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::ConstructL"));

    iItemArray = new ( ELeave ) CSelectionItemList( KArrayGranularity );
    // Create selection list
    CreateSelectionListL();
    // set setting item secondary text
    SetSettingItemTextL();
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::~CMSSettingItemMemory()
// --------------------------------------------------------------------------
//
CMSSettingItemMemory::~CMSSettingItemMemory()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::~CMSSettingItemMemory"));

    if ( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        }

    iDriveArray.ResetAndDestroy();
    iDriveArray.Close();
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSSettingItemMemory::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::EditItemL"));

    if ( iItemArray->Count() )
        {
        CAknSettingPage* dlg = new ( ELeave ) CAknCheckBoxSettingPage(
                R_MS_MEMORYSELECTION_SETTING, iItemArray );

        //store item before launch setting page
        CSelectionItemList* tempItemArray = new ( ELeave )
                                CSelectionItemList( KArrayGranularity );
        CleanupStack::PushL( tempItemArray );
        for ( TInt i = 0; i < iItemArray->Count(); i++ )
            {
            HBufC* memoryName = ( *iItemArray )[i]->ItemText().AllocLC();
            TBool selected = ( *iItemArray )[i]->SelectionStatus();
            CSelectableItem* tempitem = new ( ELeave )
                                CSelectableItem( *memoryName, selected );
            CleanupStack::PushL( tempitem );
            tempitem->ConstructL();
            tempItemArray->AppendL( tempitem );  // ownership transferred
            CleanupStack::Pop( tempitem );
            CleanupStack::PopAndDestroy( memoryName );
            }

        // launch setting page
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            LOG(_L("[MediaServant]\t CMSSettingItemMemory::\
            EditItemL  Press Ok to exit"));
            SetStorageInformationL();
            SetSettingItemTextL();
            UpdateListBoxTextL();
            }
        else
            {
            //restore item after press cancel key to exit setting page
            LOG(_L("[MediaServant]\t CMSSettingItemMemory::\
            EditItemL  Press Cancel to exit"));
            if( iItemArray )
                {
                iItemArray->ResetAndDestroy();
                }

            for ( TInt i = 0; i < tempItemArray->Count(); i++ )
                {
                iItemArray->AppendL( ( *tempItemArray )[i] );
                //ownership transferred
                }
            }

        CleanupStack::PopAndDestroy( tempItemArray );
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::SetSettingItemTextL
// Sets setting item secondary text according to selected items
// --------------------------------------------------------------------------
//
void CMSSettingItemMemory::SetSettingItemTextL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::SetSettingItemTextL"));

    TInt count(0);

    // count selected items
    for ( TInt i = 0; i < iItemArray->Count(); i++ )
        {
        if ( (*iItemArray)[i]->SelectionStatus() )
            {
            count++;
            }
        }

    HBufC* itemText = NULL;
    // Check if there is no selected items
    if ( count == 0 )
        {
        itemText = StringLoader::LoadLC( R_MS_NO_MEMORY_IN_USE );
        }
    // Check if only one item is selected
    else if ( count == 1 )
        {
        TInt itemCount = iItemArray->Count();
        for ( TInt index = 0; index < itemCount; index++ )
            {
            CSelectableItem* item = (*iItemArray)[ index ];
            if ( item->SelectionStatus() )
                {
                itemText = item->ItemText().AllocLC();
                index = itemCount; // break loop
                }
            }
        }
    // more than one item selected
    else
        {
        itemText = StringLoader::LoadLC( R_MS_NOF_MEMORIES_IN_USE, count );
        // do number conversion
        TPtr ptr = itemText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
        }

    if ( itemText )
        {
        // Set new value
        SetExternalText( *itemText );
        LoadL();
        CleanupStack::PopAndDestroy( itemText );
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::CMSSettingItemMemory
// --------------------------------------------------------------------------
//
CMSSettingItemMemory::CMSSettingItemMemory(
    TInt aIdentifier,
    TDes& aText,
    CMSEngine& aMSEngine,
    CMSSettingItemList& aParent
     ) :
    CAknTextSettingItem( aIdentifier, aText ),
    iMSEngine( aMSEngine ),
    iParent( aParent )
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::CMSSettingItemMemory"));
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::CreateSelectionListL
// Creates selection list using drivelist and stored drivelit as input
// --------------------------------------------------------------------------
//
void CMSSettingItemMemory::CreateSelectionListL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::CreateSelectionListL"));

    RFs fileSession = iParent.ControlEnv()->FsSession();

    MCmSettings* settings = CCmSettingsFactory::NewCmSettingsEngineLC();
    settings->DriveListL( fileSession, iDriveArray );

    RPointerArray<CCmDriveInfo> storedDrives;
    CleanupClosePushL( storedDrives );
    iMSEngine.GetDrivesL( storedDrives );

    for ( TInt i = 0; i< iDriveArray.Count(); i++ )
        {
        TDesC* memoryName = NULL;

        if ( iDriveArray[i]->DriveName().Length() )
            {
            memoryName = iDriveArray[i]->DriveName().AllocLC();
            }
        else
            {
            // use default name if drive has no name
            memoryName = StringLoader::LoadLC(
                                R_MS_SETTINGS_DEFAULT_MEMORY_NAME );
            }

        TBool selected( EFalse );
        for ( TInt j = 0; j < storedDrives.Count(); j++)
            {
            if ( storedDrives[j]->DriveId() == iDriveArray[i]->DriveId() )
                {
                selected = ETrue;
                }
            }
        CSelectableItem* item = new ( ELeave ) CSelectableItem( *memoryName,
                                                                selected );
        CleanupStack::PushL( item );
        item->ConstructL();
        iItemArray->AppendL( item ); // ownership transferred
        CleanupStack::Pop( item );
        CleanupStack::PopAndDestroy( memoryName );
        }

    storedDrives.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &storedDrives );
	settings->Close();
    CleanupStack::Pop(); // settings
    }

// --------------------------------------------------------------------------
// CMSSettingItemMemory::SetStorageInformationL
// Sets drive information and quota
// --------------------------------------------------------------------------
//
void CMSSettingItemMemory::SetStorageInformationL()
    {
    LOG(_L("[MediaServant]\t CMSSettingItemMemory::SetStorageInformationL"));

    RPointerArray<CCmDriveInfo> activeDrives;
    for ( TInt i = 0; i < iItemArray->Count(); i++ )
        {
        if ( (*iItemArray)[i]->SelectionStatus() )
            {
            TInt64 quota(0);
            if ( iDriveArray[i]->DriveType() & DriveInfo::EDriveRemovable )
                {
                LOG(_L("[MediaServant]\t CMSSettingItemMemory::\
                SetStorageInformationL set memory card quota"));
                // memory card
                quota =
                    ( iDriveArray[i]->DriveSize() * KMMCDefaultQuota ) / 100;
                }
            else
                {
                LOG(_L("[MediaServant]\t CMSSettingItemMemory::\
                SetStorageInformationL internal memory quota"));
                // internal hd
                quota =
                    ( iDriveArray[i]->DriveSize() * KHDDefaultQuota ) / 100;

                }
            // add quota to drive info
            iDriveArray[i]->SetDriveQuota( quota );
            // add selected drive to array
            activeDrives.AppendL( iDriveArray[i] );
            }
        }

    // store drive information
    TRACE(Print(_L("[MediaServant]\t CMSSettingItemMMemory::\
        SetStorageInformationL drivecount = %d"), activeDrives.Count()));
    iMSEngine.SetDrivesL( activeDrives );
    activeDrives.Reset();
    activeDrives.Close();
    }
    
// End of File
