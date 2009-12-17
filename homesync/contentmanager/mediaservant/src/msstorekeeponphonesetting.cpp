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
* Description:  CMSStoreKeepOnPhoneSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <aknlistquerydialog.h>

#include "msstorekeeponphonesetting.h"
#include "mediaservant.hrh"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSStoreKeepOnPhoneSetting::~CMSStoreKeepOnPhoneSetting()
// --------------------------------------------------------------------------
//
CMSStoreKeepOnPhoneSetting::~CMSStoreKeepOnPhoneSetting()
    {
    }

// --------------------------------------------------------------------------
// CMSStoreKeepOnPhoneSetting::CMSStoreKeepOnPhoneSetting
// --------------------------------------------------------------------------
//
CMSStoreKeepOnPhoneSetting::CMSStoreKeepOnPhoneSetting(
    TInt aIdentifier,
    TCmMediaType aMediaType,
    TInt& aSelection
     ) :
    CAknEnumeratedTextPopupSettingItem( aIdentifier, aSelection),
    iMediaType ( aMediaType )
    {
    }

// --------------------------------------------------------------------------
// CMSRuleAmountSetting::CompleteConstructionL
// --------------------------------------------------------------------------
//
void CMSStoreKeepOnPhoneSetting::CompleteConstructionL()
    {
    LOG(_L("[MediaServant]\t CMSStoreKeepOnPhoneSetting::\
    CompleteConstructionL"));

    // base class call
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();

    // Get text arrays - defined in resource
    CArrayPtr< CAknEnumeratedText >* resEnumeratedTextArray =
                                                   EnumeratedTextArray ();
    CArrayPtr< HBufC > * resPoppedUpTextArray = PoppedUpTextArray ();

    if ( iMediaType == ECmImage || iMediaType == ECmOtherImage )
        {
        // remove "yes" item
        delete resEnumeratedTextArray->At( EMSKeep );
        resEnumeratedTextArray->Delete( EMSKeep );
        delete resPoppedUpTextArray->At( EMSKeep );
        resPoppedUpTextArray->Delete( EMSKeep );
        }

    else // other type than image
        {
        // remove "keep original size" and "keep screen size" items
        delete resEnumeratedTextArray->At( EMSKeepScreenSize );
        delete resPoppedUpTextArray->At( EMSKeepScreenSize );
        delete resEnumeratedTextArray->At( EMSKeepOriginalSize );
        delete resPoppedUpTextArray->At( EMSKeepOriginalSize );
        // delete 2 items
        resEnumeratedTextArray->Delete( EMSKeepOriginalSize, 2 );
        // delete 2 items
        resPoppedUpTextArray->Delete( EMSKeepOriginalSize, 2 );

        }

    HandleTextArrayUpdateL();
    }

// End of File

