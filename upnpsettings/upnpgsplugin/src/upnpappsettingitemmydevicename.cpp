/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Implements home network my device name setting item class to
*                home network setting item list.
*
*/






// INCLUDE FILES
#include "upnpappsettingitemmydevicename.h"
#include <aknnotewrappers.h>                // CAknInformationNote
#include <upnpgspluginrsc.rsg>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// UPnPAppSettingItemMyDeviceName::NewL()
// ---------------------------------------------------------
//
UPnPAppSettingItemMyDeviceName* UPnPAppSettingItemMyDeviceName::NewL(
    TInt aIdentifier,
    TDes& aText,
    TBool& aSharingState )
    {
    UPnPAppSettingItemMyDeviceName* self = new (ELeave)
        UPnPAppSettingItemMyDeviceName(
        aIdentifier,
        aText,
        aSharingState );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------
// UPnPAppSettingItemMyDeviceName::UPnPAppSettingItemMyDeviceName()
// ----------------------------------------------------------------
//
UPnPAppSettingItemMyDeviceName::UPnPAppSettingItemMyDeviceName(
    TInt aIdentifier,
    TDes& aText,
    TBool& aSharingState ) :
    CAknTextSettingItem(aIdentifier, aText),
    iSharingState( aSharingState )
    {
    }

// ---------------------------------------------------------
// UPnPAppSettingItemMyDeviceName::ConstructL()
// ---------------------------------------------------------
//
void UPnPAppSettingItemMyDeviceName::ConstructL()
    {
    }

// -----------------------------------------------------------------
// UPnPAppSettingItemMyDeviceName::~UPnPAppSettingItemMyDeviceName()
// -----------------------------------------------------------------
//
UPnPAppSettingItemMyDeviceName::~UPnPAppSettingItemMyDeviceName()
    {
    }

// ---------------------------------------------------------
// UPnPAppSettingItemMyDeviceName::EditItemL()
// ---------------------------------------------------------
//
void UPnPAppSettingItemMyDeviceName::EditItemL(TBool aCalledFromMenu)
    {

    // if sharing is on, only information note is displayed
    if ( iSharingState )
        {
        CAknInformationNote* note = new (ELeave)
            CAknInformationNote;
        HBufC* noteText = CCoeEnv::Static()->AllocReadResourceLC(
            R_QTN_IUPNP_IAP_TURN_SHARING_OFF);
        note->ExecuteLD(*noteText);
        CleanupStack::PopAndDestroy(noteText);
        }
    else
        {
        // normal behaviour
        CAknTextSettingItem::EditItemL(aCalledFromMenu);
        }

    }


// End of File
