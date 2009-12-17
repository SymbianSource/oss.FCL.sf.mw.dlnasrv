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
* Description:  CMSRuleFileSizeSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <aknsettingitemlist.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include "cmcommontypes.h"
#include "msrulefilesizesetting.h"
#include "mediaservant.hrh"
#include "msdebug.h"


// CONSTANTS
const TInt KArrayGranularity = 2;


// --------------------------------------------------------------------------
// CMSRuleFileSizeSetting::~CMSRuleFileSizeSetting()
// --------------------------------------------------------------------------
//
CMSRuleFileSizeSetting::~CMSRuleFileSizeSetting()
    {
    }

// --------------------------------------------------------------------------
// CMSRuleFileSizeSetting::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSRuleFileSizeSetting::EditItemL( TBool aCalledFromMenu )
    {
    LOG(_L("[MediaServant]\t CMSRuleFileSizeSetting::EditItemL"));

    // Launches setting page
    // Button pressed on setting page will be stored to iSettingPageEvent
    CMSEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);
    StoreL();

    // selected item index
    TInt selected = QueryValue()->CurrentValueIndex();

    if ( SettingAccepted() &&
         selected != EMSAny )
        {
        // Construct Integer query
        CAknNumberQueryDialog* query = CAknNumberQueryDialog::NewL( iValue );

        // set query heading
        if ( iType == EMSMinFileSize)
            {
            HBufC* heading = StringLoader::LoadLC(
                                    R_MS_MIN_FILE_SIZE_PAGE_ITEM_TEXT );
            query->SetPromptL( *heading );
            CleanupStack::PopAndDestroy( heading );
            }
        else // EMSMaxFileSize
            {
            HBufC* heading = StringLoader::LoadLC(
                                    R_MS_MAX_FILE_SIZE_PAGE_ITEM_TEXT );
            query->SetPromptL( *heading );
            CleanupStack::PopAndDestroy( heading );
            }

        // run query
        if ( query->ExecuteLD( R_MSERV_DATA_QUERY ) )
            {
            CArrayPtr< CAknEnumeratedText >* enumeratedTextArray =
                                                    EnumeratedTextArray ();
            // delete object
            delete enumeratedTextArray->At(selected);
            // delete pointer
            enumeratedTextArray->Delete( selected );

            HBufC* itemText = StringLoader::LoadLC( R_MS_FILE_SIZE_ITEM_TEXT,
                                            iValue );

            // do number conversion
            TPtr ptr = itemText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            CAknEnumeratedText* enumeratedText =
                    new (ELeave) CAknEnumeratedText( selected, itemText);
            CleanupStack::PushL( enumeratedText );

            // insert new text to array
            enumeratedTextArray->InsertL( selected, enumeratedText );
            CleanupStack::Pop( 2, itemText );


            UpdateListBoxTextL();
            }
        }
    }

// --------------------------------------------------------------------------
// CMSRuleFileSizeSetting::CMSRuleFileSizeSetting
// --------------------------------------------------------------------------
//
CMSRuleFileSizeSetting::CMSRuleFileSizeSetting(
    TInt aIdentifier,
    TInt aType,
    TInt &aSelection,
    TInt &aValue
     ) :
    CMSEnumeratedTextPopupSettingItem( aIdentifier, aSelection ),
    iType( aType ),
    iValue( aValue )
    {
    }

// --------------------------------------------------------------------------
// CMSRuleFileSizeSetting::CompleteConstructionL
// --------------------------------------------------------------------------
//
void CMSRuleFileSizeSetting::CompleteConstructionL()
    {
    LOG(_L("[MediaServant]\t CMSRuleFileSizeSetting::\
    CompleteConstructionL"));

    CArrayPtr< CAknEnumeratedText > * enumeratedTextArray =
        new (ELeave) CArrayPtrFlat<CAknEnumeratedText>(KArrayGranularity);
    CleanupStack::PushL( enumeratedTextArray );

    CArrayPtr< HBufC > * poppedUpTextArray =
            new (ELeave) CArrayPtrFlat<HBufC> (KArrayGranularity);
    CleanupStack::PushL( poppedUpTextArray );

    // "Any" setting item text
    HBufC* itemText = StringLoader::LoadLC( R_MS_VALUE_ANY_FILE_SIZE );
    CAknEnumeratedText* enumeratedText =
            new (ELeave) CAknEnumeratedText( EMSAny, itemText );
    CleanupStack::PushL( enumeratedText );
    enumeratedTextArray->AppendL( enumeratedText );
    CleanupStack::Pop( 2, itemText );

    // "XX MB" setting item text
    itemText = StringLoader::LoadLC( R_MS_FILE_SIZE_ITEM_TEXT, iValue );

    // do number conversion
    TPtr ptr = itemText->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

    enumeratedText = new (ELeave) CAknEnumeratedText(EMSSetSize, itemText);
    CleanupStack::PushL( enumeratedText );
    enumeratedTextArray->AppendL( enumeratedText );
    CleanupStack::Pop( 2, itemText );

    // "Any" popup text
    HBufC* popupText = StringLoader::LoadLC( R_MS_VALUE_ANY_FILE_SIZE );
    poppedUpTextArray->AppendL(popupText);
    CleanupStack::Pop( popupText );

    if ( iType == EMSMinFileSize )
        {
        // "Set min MB" setting page text
        HBufC* popupText2 =
                StringLoader::LoadLC( R_MS_MIN_FILE_SIZE_PAGE_ITEM_TEXT );
        poppedUpTextArray->AppendL(popupText2);
        CleanupStack::Pop( popupText2 );
        }
    else // EMSMaxFileSize
        {
        // "Set max MB" setting page text
        HBufC* popupText2 = StringLoader::LoadLC(
                                    R_MS_MAX_FILE_SIZE_PAGE_ITEM_TEXT );
        poppedUpTextArray->AppendL(popupText2);
        CleanupStack::Pop( popupText2 );
        }

    // set arrays
    SetEnumeratedTextArrays(enumeratedTextArray, poppedUpTextArray);
    CleanupStack::Pop( 2, enumeratedTextArray );

    HandleTextArrayUpdateL();
    }

// End of File

