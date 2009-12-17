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
* Description:  CMSRuleAmountSetting class implementation
*
*/


#include <mediaservant.rsg>
#include <AknQueryDialog.h>
#include <StringLoader.h>

#include "cmcommontypes.h"
#include "msruleamountsetting.h"
#include "mediaservant.hrh"
#include "msdebug.h"


// CONSTANTS
const TInt KArrayGranularity = 3;


// --------------------------------------------------------------------------
// CMSRuleAmountSetting::~CMSRuleAmountSetting()
// --------------------------------------------------------------------------
//
CMSRuleAmountSetting::~CMSRuleAmountSetting()
    {
    }

// --------------------------------------------------------------------------
// CMSRuleAmountSetting::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSRuleAmountSetting::EditItemL( TBool aCalledFromMenu )
    {
    LOG(_L("[MediaServant]\t CMSRuleAmountSetting::EditItemL"));

    // Launches setting page
    CMSEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);
    StoreL();

    // selected item index
    TInt selected = QueryValue()->CurrentValueIndex();

    if ( SettingAccepted() &&
         selected != EUnlimited )
        {
        // Construct Integer query
        CAknNumberQueryDialog* query = CAknNumberQueryDialog::NewL( iValue );

        // set query heading
        if ( selected == EPieces)
            {
            HBufC* heading =
                    StringLoader::LoadLC( R_MS_AMOUNT_QUERY_HEADING );
            query->SetPromptL( *heading );
            CleanupStack::PopAndDestroy( heading );
            }
        else // EMbits
            {
            HBufC* heading =
                    StringLoader::LoadLC( R_MS_SIZE_QUERY_HEADING );
            query->SetPromptL( *heading );
            CleanupStack::PopAndDestroy( heading );
            }

        // run query
        if ( query->ExecuteLD( R_MSERV_DATA_QUERY ) )
            {
            HBufC* itemText = NULL;

            CArrayPtr< CAknEnumeratedText >* enumeratedTextArray =
                                                    EnumeratedTextArray ();
            // delete object
            delete (*enumeratedTextArray)[selected];
            // delete pointer
            enumeratedTextArray->Delete( selected );

            if ( selected == EPieces )
                {

                switch ( iValue )
                    {
                    case 0: // 0 items
                        {
                        itemText =
                         StringLoader::LoadLC( R_MS_VALUE_AMOUNT_0_ITEMS );
                        break;
                        }
                    case 1: // 1 item
                        {
                        itemText =
                         StringLoader::LoadLC( R_MS_VALUE_AMOUNT_ONE_ITEM );
                        break;
                        }
                    default: // many items
                        {
                        itemText = StringLoader::LoadLC(
                                             R_MS_VALUE_AMOUNT_COUNT_NUMBER,
                                             iValue );
                        break;
                        }
                    }
                }
            else // EMbits
                {
                itemText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_MB_NUMBER,
                                                iValue );
                }

            // do number conversion
            TPtr ptr = itemText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            CAknEnumeratedText* enumeratedText =
                        new (ELeave) CAknEnumeratedText( selected, itemText);
            CleanupStack::PushL( enumeratedText );
            // insert new text to array
            enumeratedTextArray->InsertL( selected, enumeratedText );
            CleanupStack::Pop( enumeratedText );
            if ( itemText )
                {
                CleanupStack::Pop( itemText );
                }

            UpdateListBoxTextL();
            }
        else
            {
			EditItemL( aCalledFromMenu );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSRuleAmountSetting::CMSRuleAmountSetting
// --------------------------------------------------------------------------
//
CMSRuleAmountSetting::CMSRuleAmountSetting(
    TInt aIdentifier,
    TInt &aLimitType,
    TInt &aValue
     ) :
    CMSEnumeratedTextPopupSettingItem( aIdentifier, aLimitType ),
    iLimitType( aLimitType ),
    iValue( aValue )
    {
    }

// --------------------------------------------------------------------------
// CMSRuleAmountSetting::CompleteConstructionL
// --------------------------------------------------------------------------
//
void CMSRuleAmountSetting::CompleteConstructionL()
    {
    LOG(_L("[MediaServant]\t CMSRuleAmountSetting::CompleteConstructionL"));

    CArrayPtr< CAknEnumeratedText > * enumeratedTextArray =
        new (ELeave) CArrayPtrFlat<CAknEnumeratedText>(KArrayGranularity);
    CleanupStack::PushL( enumeratedTextArray );

    CArrayPtr< HBufC > * poppedUpTextArray =
                new (ELeave) CArrayPtrFlat<HBufC> (KArrayGranularity);
    CleanupStack::PushL( poppedUpTextArray );


    // "UNLIMITED" setting item text
    HBufC* itemText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_UNLIMITED );
    CAknEnumeratedText* enumeratedText =
                new (ELeave) CAknEnumeratedText( EUnlimited, itemText );
    CleanupStack::PushL( enumeratedText );
    enumeratedTextArray->AppendL( enumeratedText );
    CleanupStack::Pop( 2, itemText );

    // "XX items" setting item text
    switch( iValue )
        {
        case 0: // 0 items
            {
            itemText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_0_ITEMS );
            break;
            }
        case 1: // 1 item
            {
            itemText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_ONE_ITEM );
            break;
            }
        default: // many items
            {
            itemText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_COUNT_NUMBER,
                                             iValue);
            // do number conversion
            TPtr ptr = itemText->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

            break;
            }
        }

    enumeratedText = new (ELeave) CAknEnumeratedText(EPieces, itemText);
    CleanupStack::PushL( enumeratedText );
    enumeratedTextArray->AppendL( enumeratedText );
    CleanupStack::Pop( 2, itemText );

    // "XX MB" setting item text
    itemText = StringLoader::LoadLC(
                            R_MS_VALUE_AMOUNT_MB_NUMBER, iValue );
    // do number conversion
    TPtr ptr = itemText->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

    enumeratedText = new (ELeave) CAknEnumeratedText(EMbits, itemText);
    CleanupStack::PushL( enumeratedText );
    enumeratedTextArray->AppendL( enumeratedText );
    CleanupStack::Pop( 2, itemText );

    // "UNLIMITED" popup text
    HBufC* popupText = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_UNLIMITED );
    poppedUpTextArray->AppendL(popupText);
    CleanupStack::Pop( popupText );

    // "SET MB" popup text
    HBufC* popupText2 = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_COUNT_PAGE );
    poppedUpTextArray->AppendL(popupText2);
    CleanupStack::Pop( popupText2 );

    // "SET COUNT" popup text
    HBufC* popupText3 = StringLoader::LoadLC( R_MS_VALUE_AMOUNT_MB_PAGE );
    poppedUpTextArray->AppendL(popupText3);
    CleanupStack::Pop( popupText3 );

    // set arrays
    SetEnumeratedTextArrays(enumeratedTextArray, poppedUpTextArray);
    CleanupStack::Pop( 2, enumeratedTextArray );

    HandleTextArrayUpdateL();
    }

// End of File

