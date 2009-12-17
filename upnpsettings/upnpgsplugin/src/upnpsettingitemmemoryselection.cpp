/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUPnPSettingItemMemorySelection class implementation
*
*/






#include <upnpgspluginrsc.rsg>
#include <StringLoader.h>
#include <akncheckboxsettingpage.h>
#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>

#include "upnpsettingitemmemoryselection.h"

_LIT( KComponentLogfile, "upnpgsplugin.txt");
#include "upnplog.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUPnPSettingItemMemorySelection::NewL
// --------------------------------------------------------------------------
//
CUPnPSettingItemMemorySelection* CUPnPSettingItemMemorySelection::NewL(
                                TInt aIdentifier,
                                TDriveNumber& aSelectedDrive )
    {
    __LOG("[gsplugin]\t CUPnPSettingItemMemorySelection::NewL");

    CUPnPSettingItemMemorySelection* self =
        new (ELeave) CUPnPSettingItemMemorySelection(
            aIdentifier,
            aSelectedDrive );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPSettingItemMemorySelection::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPSettingItemMemorySelection::ConstructL()
    {
        __LOG("[gsplugin]\t\
CUPnPSettingItemMemorySelection::ConstructL");

    }

// --------------------------------------------------------------------------
// CUPnPSettingItemMemorySelection::~CUPnPSettingItemMemorySelection()
// --------------------------------------------------------------------------
//
CUPnPSettingItemMemorySelection::~CUPnPSettingItemMemorySelection()
    {
        __LOG("[gsplugin]\t\
CUPnPSettingItemMemorySelection::~CUPnPSettingItemMemorySelection");

    }

// --------------------------------------------------------------------------
// CUPnPSettingItemMemorySelection::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CUPnPSettingItemMemorySelection::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    __LOG("[gsplugin]\t CUPnPSettingItemMemorySelection::EditItemL");
    
    CAknMemorySelectionDialogMultiDrive* dlg =
        CAknMemorySelectionDialogMultiDrive::NewL(
            ECFDDialogTypeNormal,
            0,
            EFalse,
            AknCommonDialogsDynMem::EMemoryTypePhone |
            AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
            AknCommonDialogsDynMem::EMemoryTypeMMCExternal 
            );
            
            CleanupStack::PushL(dlg);

            if (dlg->ExecuteL(iInternalData))
            {
                // Call CAknMemorySelectionSettingItemMultiDrive::StoreL()
                StoreL();
            }

            CleanupStack::PopAndDestroy(dlg);
    
    }

// --------------------------------------------------------------------------
// CUPnPSettingItemMemorySelection::CUPnPSettingItemMemorySelection
// --------------------------------------------------------------------------
//
CUPnPSettingItemMemorySelection::CUPnPSettingItemMemorySelection(
    TInt aIdentifier,
    TDriveNumber& aSelectedDrive//,
     ) :
    CAknMemorySelectionSettingItemMultiDrive( aIdentifier, aSelectedDrive )
    {
    }


// End of File

