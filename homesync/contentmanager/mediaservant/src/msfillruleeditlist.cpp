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
* Description:  CMSFillRuleEditList class implementation
*
*/


#include <mediaservant.rsg>
#include <barsread.h>
#include <akntextsettingpage.h>
#include <StringLoader.h>
#include <akncheckboxsettingpage.h>
#include <e32math.h>
#include <utf.h>

#include "cmsqlpropertycontainer.h"
#include "mserv.hlp.hrh"
#include "cmfillrule.h"
#include "cmfillrulecontainer.h"
#include "cmmediaserverfull.h"
#include "msappui.h"
#include "mediaservantuid.h"
#include "msengine.h"
#include "msconstants.h"
#include "msfillruleeditlist.h"
#include "msfillruleeditview.h"
#include "msruleamountsetting.h"        // Amount
#include "msruleserverssetting.h"       // Servers
#include "msrulemultiselectionsetting.h"// Artist, genre, album, track
#include "msrulefilesizesetting.h"      // File size (min/max)
#include "mediaservant.hrh"
#include "msmetadatacollector.h"
#include "msdebug.h"
#include "msfillcontainer.h"
#include "msfillview.h"

// CONSTANTS
_LIT( KSpace, " ");
const TInt KDateBufferLength = 20;


// --------------------------------------------------------------------------
// CMSFillRuleEditList::NewL()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSFillRuleEditList* CMSFillRuleEditList::NewL(
                CMSAppUi& aAppUi, CMSFillRuleEditView& aView )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::NewL"));

    CMSFillRuleEditList* self = CMSFillRuleEditList::NewLC( aAppUi, aView );        
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::NewLC()
// Two phase constructor.
// --------------------------------------------------------------------------
//
CMSFillRuleEditList* CMSFillRuleEditList::NewLC(
                CMSAppUi& aAppUi, CMSFillRuleEditView& aView )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::NewLC"));

    CMSFillRuleEditList* self = new ( ELeave ) CMSFillRuleEditList();
    CleanupStack::PushL( self );
    self->ConstructL( aAppUi, aView );
        
    return self;
    }
    
// --------------------------------------------------------------------------
// CMSFillRuleEditList::CMSFillRuleEditList()
// --------------------------------------------------------------------------
//
CMSFillRuleEditList::CMSFillRuleEditList()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::CMSFillRuleEditList"));

    iDummyText.Copy(_L(""));
    iMinFileSize = 0;
    iMaxFileSize = 0;
    iMinFileSizeSet = EMSAny;
    iMaxFileSizeSet = EMSAny;
    iFileSize = 0;
    iFileSizeEnum = 0;
    iTrackLengthEnum = 0;
    iNewList = EFalse;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::~CMSFillRuleEditList()
// --------------------------------------------------------------------------
//
CMSFillRuleEditList::~CMSFillRuleEditList()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::~CMSFillRuleEditList"));

    delete iMetadataCollector;

    if ( iServers )
        {
        iServers->ResetAndDestroy();
        iServers->Close();
        delete iServers;
        }

    if ( iServerArray )
        {
        iServerArray->ResetAndDestroy();
        delete iServerArray;
        }

    iVisibleRules.Close();

    iArtistArray.Close();
    iAlbumArray.Close();
    iGenreArray.Close();
    iTrackArray.Close();
    iVideoTitleArray.Close();
    
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::ConstructL()
// Second phase constructor.
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::ConstructL( CMSAppUi& aAppUi,
                                      CMSFillRuleEditView& aView )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::ConstructL"));

    iAppUi = &aAppUi;
    iView = &aView;

    // Set engine observer
    iAppUi->MSEngine()->SetObserver( this );

    // read template
    iRuleTemplate = iAppUi->ParameterAgent()->GetRuleTemplate();

    // Edit existing list
    if ( iRuleTemplate.templateType == EMSCmdEditRule )
        {
        // get selected item index from previous screen
        iSelectedList = iAppUi->ParameterAgent()->FillViewFocusIndex();

        // get selected list
        iRule =(*iAppUi->
               FillRuleController()->FillListArray())[iSelectedList]->List();
        // List media type
        iMediaType = iRule->MediaType();

        // set used template
        iRuleTemplate.selectedTemplate = iRule->TemplateId();

        ReadTemplateRulesL( iRuleTemplate.selectedTemplate );
        // Get values for rules
        GetRuleValuesL();
        }

    else
        {
        iNewList = ETrue;

        // New fill list will be last one - set focus index to it
        iSelectedList =
            iAppUi->FillRuleController()->FillListArray()->Count();
        iAppUi->ParameterAgent()->SetFillViewFocusIndex( iSelectedList );

        // Create new list
        iRule = CCmFillRule::NewL();

        // Read template rules and set default values
        ReadTemplateRulesL( iRuleTemplate.selectedTemplate );

        // Use template name as list name
        iListName.Copy( iRuleTemplate.templateName );
        HBufC8* temp = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                                                            iListName );
        CleanupStack::PushL( temp );
        iRule->SetNameL( *temp );
        CleanupStack::PopAndDestroy( temp );
        // List type is determined by given menu command
        // MediaType is also set here
        SetListType( iRuleTemplate.templateType );
        iRule->SetTemplateId( iRuleTemplate.selectedTemplate );

        //Set rule selected by default
        iRule->SetSelected( ECmSelected );

        // Initialize fill rule status to ECmToBeFilled
        // If rule is image rule, status is re-set
        // according to used template
        iRule->SetStatus( ECmToBeFilled );

        InitializeValues();

        // save new list
        iAppUi->FillRuleController()->AddRuleL( iRule );

        // Get list
        iRule = (*iAppUi->FillRuleController()->
                                    FillListArray())[iSelectedList]->List();
        }
     
     // should filter the vedio array when construct first time, only selected item can be construct
     if( iRule->MediaType() == ECmVideo )
        {
        // reset track array
        iTrackArray.Reset();
        // append the select video title in track array
        TInt count = iVideoTitleArray.Count();
        if( count > 0 )
            {
            for( TInt i=0; i<count; i++ )
                {
                iTrackArray.AppendL( iVideoTitleArray[i] );	
                }
            }
        	
        }
    // Creates metadata collector and starts data fetching
    iMetadataCollector = CMSMetadataCollector::NewL( *iAppUi,
                                                     iMediaType,
                                                     iGenreArray,
                                                     iArtistArray,
                                                     iAlbumArray,
                                                     iTrackArray);

    // get mediaservers and remove not activated
    GetServersAndCheckActivityL();

    CreateServerSettingListL();
    // Construct setting list
    ConstructListAndStartDataReadingL();
    
    iPreviousRuleIdentifier = 0;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::StartDataReadingL
// starts reading data to multiselection lists from server
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::ConstructListAndStartDataReadingL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    ConstructListAndStartDataReadingL"));

    switch ( iMediaType )
        {
        case ECmVideo:
            {
             // construct item list
            CAknSettingItemList::ConstructFromResourceL(
                                            R_MS_VIDEO_FILL_RULES );
            break;
            }
        case ECmImage:
            {
             // construct item list
            CAknSettingItemList::ConstructFromResourceL(
                                            R_MS_IMAGE_FILL_RULES );
            if ( iNewList )
                {
                SaveListValuesL();
                iAppUi->FillRuleController()->SaveRulesL();
                PreprocessListL();
                }
            else
                {
                CreateAndSetNaviPaneTextL();
                }
            break;
            }
        case ECmAudio:
            {
             // construct item list
            CAknSettingItemList::ConstructFromResourceL(
                                            R_MS_AUDIO_FILL_RULES );
            break;
            }
        default:
            {
            break;
            }
        }

    if ( iMediaType == ECmVideo || iMediaType == ECmAudio )
        {
        // Show 'reading data' text on navi pane
        HBufC* naviText = StringLoader::LoadLC(
                                            R_MS_WAIT_GETTING_DATA_TEXT );
        iView->SetNavigationPaneTextL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
        }

    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SaveListValuesL
// Saves list values from visible setting items
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SaveListValuesL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveListValuesL"));

    CAknSettingItemArray* itemArray = SettingItemArray();
    for ( TInt index  = 0; index < itemArray->Count(); index++ )
        {
        CAknSettingItem* item = itemArray->At( index );
        if ( !item->IsHidden() )
            {
            SaveValueL( item );
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::PreprocessListL
// starts list preprocess and shows info on navi pane
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::PreprocessListL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::PreprocessListL"));

    iAppUi->MSEngine()->PreProcessFillListL( iRule->Name() );
    HBufC* naviText = StringLoader::LoadLC( R_MS_PREPROCESSING_TEXT );
    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SaveValueL
// Save values to rule container
// Only values from visible rules are stored
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SaveValueL( CAknSettingItem* aItem )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveValueL"));

    if ( !aItem->IsHidden() )
        {
        switch ( aItem->Identifier() )
            {
            case EMSListName:
                {
                // Name is always visible - set rule name
                HBufC8* listName =
                    CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                       SettingItemArray()->At(EMSListName)->SettingTextL() );
                CleanupStack::PushL( listName );
                iRule->SetNameL( *listName );
                CleanupStack::PopAndDestroy( listName );
                break;
                }
            case EMSShrinkImages:
                {
                // Set shrinking status for images
                SetShrinkingStatus();
                break;
                }
            case EMSAmount:
                {
                // set amount of files
                iRule->SetAmount( iAmount );
                // set limit type
                iRule->SetLimitType( iLimitType );
                break;
                }
            case EMSStartingWith:
                {
                iRule->SetMethod(
                            static_cast<TCmFillMethod> (iStartWith) );
                break;
                }
            case EMSDateFrom:
                {
                DeleteRule( ECmDate, ECmGreater );

                TInt ruleIndex = iRule->AddRuleL( ECmDate,
                                                  ECmGreater );
                HBufC8* str = HBufC8::NewL( KDateBufferLength );
                CleanupStack::PushL( str );
                str->Des().AppendNum(iDateFrom.Int64());
                iRule->AddRuleParamL( ruleIndex, *str );
                CleanupStack::PopAndDestroy( str );
                break;
                }
            case EMSDateUntil:
                {
                DeleteRule( ECmDate, ECmSmaller );

                TInt ruleIndex = iRule->AddRuleL( ECmDate,
                                                  ECmSmaller );
                HBufC8* str = HBufC8::NewL( KDateBufferLength );
                CleanupStack::PushL( str );
                str->Des().AppendNum(iDateUntil.Int64());
                iRule->AddRuleParamL( ruleIndex, *str );
                CleanupStack::PopAndDestroy( str );

                break;
                }
            case EMSServers:
                {
                HandleServerSelectionL();
                break;
                }
            case EMSFreeText:
                {
                DeleteRule( ECmFreeText );
                if ( iFreeText.Size() )
                    {
                    TInt ruleIndex = iRule->AddRuleL( ECmFreeText,
                                                      ECmConsistOf );
                    HBufC8* str = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                                                                 iFreeText );
                    CleanupStack::PushL( str );
                    iRule->AddRuleParamL( ruleIndex, *str );
                    CleanupStack::PopAndDestroy( str );
                    }
                break;
                }
            case EMSTitleName:
                {
                DeleteRule( ECmTitle );

                if ( iVideoTitleArray.Count() > 0 )
                    {
                    AddAdditionalRuleL( iVideoTitleArray,
                                        ECmTitle,
                                        ECmEquals );
                    }
                break;
                }
            case EMSMinFileSize:
                {
                DeleteRule( ECmFileSize, ECmGreater );
                if ( iMinFileSizeSet == EMSSetSize )
                    {
                    AddAdditionalRuleL( ECmFileSize,
                                        ECmGreater,
                                        iMinFileSize*KMega );
                    }
                break;
                }
            case EMSMaxFileSize:
                {
                DeleteRule( ECmFileSize, ECmSmaller );
                if ( iMaxFileSizeSet == EMSSetSize )
                    {
                    AddAdditionalRuleL( ECmFileSize,
                                        ECmSmaller,
                                        iMaxFileSize*KMega );
                    }
                break;
                }
             case EMSGenre:
                {
                DeleteRule( ECmGenre );

                if ( iGenreArray.Count() > 0 )
                    {
                    AddAdditionalRuleL( iGenreArray,
                                        ECmGenre,
                                        ECmEquals );
                    }
                break;
                }
             case EMSArtist:
                {
                DeleteRule( ECmArtist );

                if ( iArtistArray.Count() > 0 )
                    {
                    AddAdditionalRuleL( iArtistArray,
                                        ECmArtist,
                                        ECmEquals );
                    }
                break;
                }
             case EMSAlbum:
                {
                DeleteRule( ECmAlbum );

                if ( iAlbumArray.Count() > 0 )
                    {
                    AddAdditionalRuleL( iAlbumArray,
                                        ECmAlbum,
                                        ECmEquals );
                    }
                break;
                }
            case EMSTrack:
                {
                DeleteRule( ECmTitle );

                if ( iTrackArray.Count() > 0 )
                    {
                    AddAdditionalRuleL( iTrackArray,
                                        ECmTitle,
                                        ECmEquals );
                    }
                break;
                }
            case EMSTrackLength:
                {
                DeleteRule( ECmDuration );
                if ( iTrackLengthEnum )
                    {
                    SaveTrackDurationL();
                    }
                break;
                }
            case EMSFileSize:
                {
                DeleteRule( ECmFileSize );
                if ( iFileSizeEnum )
                    {
                    SaveTrackFileSizeL();
                    }
                break;
                }
            default:
                {
                LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveValueL \
                no such rule"));
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CAknSettingItem* CMSFillRuleEditList::CreateSettingItemL(TInt aIdentifier)
// Takes care of creating actual setting items.
// --------------------------------------------------------------------------
//
CAknSettingItem* CMSFillRuleEditList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* settingItem = NULL;

    // setting item is created according to aIdentifier
    switch ( aIdentifier )
        {
        case EMSListName:
            {
            settingItem = new (ELeave ) CMSTextSettingItem(
                                        aIdentifier,
                                        iListName );
            break;
            }
        case EMSShrinkImages:
            {
            settingItem = new (ELeave ) CMSEnumeratedTextPopupSettingItem(
                                  aIdentifier,
                                  reinterpret_cast<TInt&> (iShrinkImages) );
            break;
            }

        case EMSAmount:
            {
            settingItem = new (ELeave ) CMSRuleAmountSetting(
                                        aIdentifier,
                                        reinterpret_cast<TInt&> (iLimitType),
                                        iAmount );
            break;
            }

        case EMSStartingWith:
            {
            settingItem = new (ELeave ) CMSEnumeratedTextPopupSettingItem(
                                        aIdentifier,
                                        iStartWith );
            break;
            }

        case EMSDateFrom:
            {
            settingItem = new (ELeave ) CMSTimeOrDateSettingItem(
                                        R_MS_FILL_RULE_DATE_FROM_SETTING_PAGE,
                                        aIdentifier,
                                        CAknTimeOrDateSettingItem::EDate,
                                        iDateFrom );
            break;
            }

        case EMSDateUntil:
            {
            settingItem = new (ELeave ) CMSTimeOrDateSettingItem(
                                        R_MS_FILL_RULE_DATE_UNTIL_SETTING_PAGE,
                                        aIdentifier,
                                        CAknTimeOrDateSettingItem::EDate,
                                        iDateUntil );
            break;
            }

        case EMSServers:
            {
            settingItem = CMSRuleServersSetting::NewL(
                                                aIdentifier,
                                                *iServerArray,
                                                iSettingText,
                                                R_MS_VALUE_LIST_ANY_SOURCE_DEVICE);
            break;
            }

        case EMSFreeText:
            {
            settingItem = new (ELeave ) CMSTextSettingItem(
                                        aIdentifier,
                                        iFreeText );
            break;
            }
        case EMSTitleName:
            {
            settingItem = CMSRuleMultiselectionSetting::NewL(
                                                aIdentifier,
                                                R_MS_RULE_NAME_SETTING,
                                                R_MS_VALUE_ANY_NAME,
                                                R_MS_VALUE_LIST_ANY_NAME,
                                                iMetadataCollector->Tracks(),
                                                &iVideoTitleArray,
                                                R_MS_RULE_TITLE_ITEM_TEXT,
                                                iSettingText );
            break;
            }

        case EMSMinFileSize:
            {
            settingItem = new (ELeave ) CMSRuleFileSizeSetting(
                                        aIdentifier,
                                        EMSMinFileSize,
                                        iMinFileSizeSet,
                                        iMinFileSize );
            break;
            }

        case EMSMaxFileSize:
            {
            settingItem = new (ELeave ) CMSRuleFileSizeSetting(
                                        aIdentifier,
                                        EMSMaxFileSize,
                                        iMaxFileSizeSet,
                                        iMaxFileSize );
            break;
            }

        case EMSGenre:
            {


            settingItem = CMSRuleMultiselectionSetting::NewL(
                                                aIdentifier,
                                                R_MS_RULE_GENRE_SETTING,
                                                R_MS_VALUE_ANY_GENRE,
                                                R_MS_VALUE_LIST_ANY_GENRE,
                                                iMetadataCollector->Genres(),
                                                &iGenreArray,
                                                R_MS_RULE_GENRE_ITEM_TEXT,
                                                iSettingText );
            break;
            }

        case EMSArtist:
            {
            settingItem = CMSRuleMultiselectionSetting::NewL(
                                                aIdentifier,
                                                R_MS_RULE_ARTIST_SETTING,
                                                R_MS_VALUE_ANY_ARTIST,
                                                R_MS_VALUE_LIST_ANY_ARTIST,
                                                iMetadataCollector->Artists(),
                                                &iArtistArray,
                                                R_MS_RULE_ARTIST_ITEM_TEXT,
                                                iSettingText );
            break;
            }

        case EMSAlbum:
            {
            settingItem = CMSRuleMultiselectionSetting::NewL(
                                                aIdentifier,
                                                R_MS_RULE_ALBUM_SETTING,
                                                R_MS_VALUE_ANY_ALBUM,
                                                R_MS_VALUE_LIST_ANY_ALBUM,
                                                iMetadataCollector->Albums(),
                                                &iAlbumArray,
                                                R_MS_RULE_ALBUM_ITEM_TEXT,
                                                iSettingText );
            break;
            }

        case EMSTrack:
            {
            settingItem = CMSRuleMultiselectionSetting::NewL(
                                                aIdentifier,
                                                R_MS_RULE_TRACK_SETTING,
                                                R_MS_VALUE_ANY_TRACK,
                                                R_MS_VALUE_LIST_ANY_TRACK,
                                                iMetadataCollector->Tracks(),
                                                &iTrackArray,
                                                R_MS_RULE_TRACK_ITEM_TEXT,
                                                iSettingText );
            break;
            }

        case EMSTrackLength:
            {
            settingItem = new (ELeave ) CMSEnumeratedTextPopupSettingItem(
                                        aIdentifier,
                                        iTrackLengthEnum );
            break;
            }

         case EMSFileSize:
            {
            settingItem = new (ELeave ) CMSEnumeratedTextPopupSettingItem(
                                        aIdentifier,
                                        iFileSizeEnum );
            break;
            }

        case EMSAddMoreRules:
            {
            settingItem = new (ELeave ) CMSTextSettingItem(
                                        aIdentifier,
                                        iDummyText );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::CreateSettingItemL\
            item not found"));            
            break;
            }
        }
    return settingItem;
    }


// --------------------------------------------------------------------------
// CMSFillRuleEditList::OpenSelectedListboxItemL
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::OpenSelectedListboxItemL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    OpenSelectedListboxItemL"));

    HandleListBoxEventL( ListBox(), EEventEnterKeyPressed );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::HandleListBoxEventL()
// Handles listbox events
// --------------------------------------------------------------------------
//
void  CMSFillRuleEditList::HandleListBoxEventL ( CEikListBox *aListBox,
                                                 TListBoxEvent aEventType)
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::HandleListBoxEventL"));

    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt currentItem = ListBox()->CurrentItemIndex();
    TInt realIndex = itemArray->ItemIndexFromVisibleIndex(currentItem);
    CAknSettingItem* item= (*itemArray)[realIndex];
	
	// Event type for non-touch UI
	if ( aEventType == EEventEnterKeyPressed )
		{
  		if ( item->Identifier() == EMSAddMoreRules )
	        {
	        AddFillRuleL();
	        }
	    else
        	{
        	CAknSettingItemList::HandleListBoxEventL ( aListBox, aEventType );
        	}
		}
	// Event type for touch UI
	else if ( aEventType == EEventItemClicked )
		{
	    // show add fill rule query if last item "Add more rules" selected
	    if ( item->Identifier() == EMSAddMoreRules &&
	         iPreviousRuleIdentifier == EMSAddMoreRules )
     	   {
	        AddFillRuleL();
	        }
    	else
	        {
	        CAknSettingItemList::HandleListBoxEventL ( aListBox, aEventType );
    	    }
		iPreviousRuleIdentifier = item->Identifier();
	    // values need to be stored
		StoreSettingsL();
		}
    else if ( item->Identifier() != EMSAddMoreRules )
    	{
		CAknSettingItemList::HandleListBoxEventL ( aListBox, aEventType );
    	}
    
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::EditItemL()
// Called by framework when item is opened
// --------------------------------------------------------------------------
//
void  CMSFillRuleEditList::EditItemL (TInt aIndex, TBool aCalledFromMenu)
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::EditItemL"));

    // cancel ongoing operation
    iView->CancelAsyncOperation( iAppUi );
    iState = EMSIdle;

    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );

    // get the item information after update
    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt currentItem = ListBox()->CurrentItemIndex();
    TInt realIndex = itemArray->ItemIndexFromVisibleIndex(currentItem);
    CAknSettingItem* item= (*itemArray)[realIndex];

    // check if list name is modified
    if ( item->Identifier() == EMSListName )
        {
        HandleListNameL( item );
        }

    if ( CheckSettingPageAcceptance( item ) )
        {
        // store new value
        StoreSettingsL();
        // Save value to rule
        SaveValueL( item );
        // save rule and process it
        SaveAndPreprocesslistL( item );
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::HandleListNameL
// Handles edit list name -case
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::HandleListNameL( CAknSettingItem* aItem )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::HandleListNameL"));

    CMSTextSettingItem* settingItem =
                            static_cast<CMSTextSettingItem*>( aItem );

    TPtr searchStr = settingItem->Text();

    // Deletes leading and trailing whitespace characters
    searchStr.Trim();

    HBufC* originalSearchStr =
                CnvUtfConverter::ConvertToUnicodeFromUtf8L( iRule->Name() );
    CleanupStack::PushL( originalSearchStr );

    // whether they are same or not
    if ( *originalSearchStr != searchStr )
        {
        // if there are same names in other fill rules
        TBool isSame = CheckSameNameL( searchStr );

        if ( isSame )
            {
            TInt postfix = 0;
            TPtr itemText = searchStr;

            // decide which number is added to the end of the name
            for ( ; ; ++postfix )
                {
                HBufC* sameStr = HBufC::NewL( KMaxFileName );
                CleanupStack::PushL( sameStr );
                sameStr->Des().Append( itemText );

                // if the name does not add number to the end is unique so keep the original name
                if ( 0 != postfix)
                    {
                    sameStr->Des().Append( KSpace );
                    sameStr->Des().AppendNum( postfix );
                    // do number conversion
                    TPtr ptr = sameStr->Des();
                    AknTextUtils::
                     DisplayTextLanguageSpecificNumberConversion( ptr );
                    }
                // must modify name with append a number
                if ( !CheckSameNameL( *sameStr ) || ( *sameStr == *originalSearchStr ) )
                    {
                    CleanupStack::PopAndDestroy( sameStr );
                    break;
                    }
                CleanupStack::PopAndDestroy( sameStr );
                }

            // do not need append a number
            if ( 0 != postfix)
                {
                itemText.Append( KSpace );
                itemText.AppendNum( postfix );

                // do number conversion
                AknTextUtils::
                    DisplayTextLanguageSpecificNumberConversion( itemText );
                
                iListName.Copy( itemText );
                settingItem->LoadL();

                aItem->UpdateListBoxTextL();
                }
            }
        }
    else
        {
        settingItem->LoadL();
        }
    CleanupStack::PopAndDestroy( originalSearchStr );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::CheckSameNameL
// check the name is same or not
// --------------------------------------------------------------------------
//
TBool CMSFillRuleEditList::CheckSameNameL( const TDesC& aName ) const
    {
    RPointerArray<CMSFillList>* iFillListArray = 
                iAppUi->FillRuleController()->FillListArray();

    // fill rule name array is not empty
    if ( iFillListArray->Count() > 0 )
        {
        for ( TInt index = 0; index < iFillListArray->Count(); ++index )
            {
            // Get fill rule name and put it to the listbox
            CCmFillRule* rule = (*iFillListArray)[index]->List();
            HBufC* listName =
                CnvUtfConverter::ConvertToUnicodeFromUtf8L( rule->Name() );
            CleanupStack::PushL( listName );
            // whether there is a same name fill rule
            if ( aName == *listName )
                {
                CleanupStack::PopAndDestroy( listName );
                return ETrue;
                }
            CleanupStack::PopAndDestroy( listName );
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SaveAndPreprocesslistL
// Saves modified list and preprocesses it
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SaveAndPreprocesslistL( CAknSettingItem* aItem )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    SaveAndPreprocesslistL"));

    // save rules
    iAppUi->FillRuleController()->SaveRulesL();
    // we'll lost iRule after update...
    iAppUi->FillRuleController()->UpdateArrayL();
    // ...so get it again
    iRule = (*iAppUi->FillRuleController()->
                    FillListArray())[iSelectedList]->List();

    // if multiselectionitem is edited start data reading after preprocess
   if ( aItem->Identifier() == EMSGenre ||
        aItem->Identifier() == EMSArtist ||
        aItem->Identifier() == EMSAlbum ||
        aItem->Identifier() == EMSTrack||
        aItem->Identifier() == EMSTitleName)
        {
        iState = EMSFetchMetadata;
        }

    // if other item than name is changed preprocessing is needed
    if ( aItem->Identifier() != EMSListName )
        {
        PreprocessListL();
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::ActivateL
// Asks setting page selection status from setting item
// --------------------------------------------------------------------------
//
TBool CMSFillRuleEditList::CheckSettingPageAcceptance(
                                            CAknSettingItem* aItem )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    CheckSettingPageAcceptance"));

    TBool status( EFalse );

    switch ( aItem->Identifier() )
        {
        case EMSShrinkImages:
        // fall through
        case EMSAmount:
        // fall through
        case EMSStartingWith:
        // fall through
        case EMSMinFileSize:
        // fall through
        case EMSMaxFileSize:
        // fall through
        case EMSTrackLength:
        // fall through
        case EMSFileSize:
            {
            CMSEnumeratedTextPopupSettingItem* item =
                    static_cast<CMSEnumeratedTextPopupSettingItem*>(aItem);
            status = item->SettingAccepted();
            break;
            }
        case EMSListName:
        // fall through
        case EMSServers:
        // fall through
        case EMSFreeText:
        // fall through
        case EMSTitleName:
        // fall through
        case EMSGenre:
        // fall through
        case EMSArtist:
        // fall through
        case EMSAlbum:
        // fall through
        case EMSTrack:

            {
            CMSTextSettingItem* item =
                    static_cast<CMSTextSettingItem*>(aItem);
            status = item->SettingAccepted();
            break;
            }

        case EMSDateFrom:
        // fall through
        case EMSDateUntil:
            {
            CMSTimeOrDateSettingItem* item =
                    static_cast<CMSTimeOrDateSettingItem*>(aItem);
            status = item->SettingAccepted();
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
            CheckSettingPageAcceptance no such item"));
            break;
            }
        }

    return status;
    }
// --------------------------------------------------------------------------
// CMSFillRuleEditList::ActivateL
// Activates setting list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::ActivateL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::ActivateL"));

    // hide all items by default
    // index starts from '1' because list name is always visible
    // index stops to count-1 because "add more rules" is always visible
    for (TInt index = 1 ;index < SettingItemArray()->Count()-1; index++)
        {
        SettingItemArray()->At( index )->SetHidden(ETrue);
        }

    CAknSettingItemArray* itemArray = SettingItemArray();
    // set template rules visible
    for (TInt idx = 0; idx < iVisibleRules.Count(); idx++)
        {
        CAknSettingItem* item = SearchItemById( iVisibleRules[idx] );
        if ( item )
            {
            item->SetHidden( EFalse);
            }
        }
    // Show also additional rules if user has changed the default value
    DisplayModifiedAdditionalRules();
    // visibility of some items may have changed
    HandleChangeInItemArrayOrVisibilityL();

    CAknSettingItemList::ActivateL();
    }

// --------------------------------------------------------------------------
// void CMSFillRuleEditList::ReadTemplateRulesL
// reads fill rule name
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::ReadTemplateRulesL( TInt aTemplate )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::ReadTemplateRulesL"));

    TResourceReader rsReader;

    switch ( aTemplate )
        {
        case EMSDefaultImageList:
            {
            iCoeEnv->CreateResourceReaderLC(rsReader,
                            R_MS_DEFAULT_IMAGE_LIST_RULES);
            break;
            }
        case EMSDefaultVideoList:
            {
            iCoeEnv->CreateResourceReaderLC(rsReader,
                            R_MS_DEFAULT_VIDEO_LIST_RULES);
            break;
            }
        case EMSDefaultMusicList:
            {
            iCoeEnv->CreateResourceReaderLC(rsReader,
                            R_MS_DEFAULT_MUSIC_LIST_RULES);
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::ReadTemplateRulesL\
             rule not found"));
            break;
            }
        }

    // get rule count of the template
    TInt ruleCount( rsReader.ReadInt16() );

    // List name is always visible -> index starts from EMSListName+1
    for ( TInt index = EMSListName+1; index <= ruleCount; index++ )
        {
        // get rule ID
        TMSFillEditItems ruleId =
                    static_cast<TMSFillEditItems>(rsReader.ReadInt16());
        // get value
        TInt value = rsReader.ReadInt16();
        // get addional value
        TInt addValue = rsReader.ReadInt16();
        if ( ruleId == EMSAmount )
            {
            iLimitType = static_cast<TCmLimitType> (addValue);
            }
        // Remember IDs of template rules - rules will be set
        // visible later
        iVisibleRules.AppendL( ruleId );

        // set value of rule
        SetDefaultValue( ruleId, value );
        }

    CleanupStack::PopAndDestroy();//reader
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SetDefaultValues
// Sets default value defined by template
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SetDefaultValue( TInt aId, TInt aValue )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SetDefaultValue"));

    switch ( aId )
        {
        case EMSShrinkImages:
            {
            iShrinkImages = static_cast<TMSShrinkImages> (aValue);
            break;
            }
        case EMSAmount:
            {
            iAmount = aValue;
            break;
            }
        case EMSStartingWith:
            {
            iStartWith = aValue;
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
            SetDefaultValues item not found"));
            }
        }
    }
// --------------------------------------------------------------------------
// CMSFillRuleEditList::AddFillRuleL
// Sets rule visibility
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::AddFillRuleL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::AddFillRuleL"));

    TResourceReader reader;

    switch ( iMediaType )
        {
        case ECmImage:
            {
            // read image templates
            iCoeEnv->CreateResourceReaderLC( reader, R_MS_IMAGE_RULE_LIST );
            break;
            }
        case ECmVideo:
            {
            // read video templates
            iCoeEnv->CreateResourceReaderLC( reader, R_MS_VIDEO_RULE_LIST );
            break;
            }
        case ECmAudio:
            {
            // read music templates
            iCoeEnv->CreateResourceReaderLC( reader, R_MS_MUSIC_RULE_LIST );
            break;
            }
        default:
            {
            // command not found
            return;
            }
        }

    // read rule count
    TInt ruleCount( reader.ReadInt16() );

    CDesCArray* ruleNames = new ( ELeave ) CDesCArrayFlat( ruleCount );
    CleanupStack::PushL( ruleNames );

    // array for template ID's
    RArray<TInt> indexArray;

    // fill name array
    for ( TInt index = 0; index < ruleCount; index++ )
        {
        // read rule ID
        TMSFillEditItems value =
                    static_cast<TMSFillEditItems>( reader.ReadInt16() );
        // read rule name
        TPtrC text = reader.ReadTPtrC();

        // add only hidden items to the list
        if ( SearchItemById( value )->IsHidden() )
            {
            indexArray.AppendL( value );
            ruleNames->AppendL( text );
            }
        }

    TInt selectedRule( 0 );

    if ( iView->ShowTemplateQueryL( R_MSERV_TEMPLATE_QUERY, 
                                    R_MS_ADD_RULE_QUERY_HEADING,
                                    ruleNames,
                                    selectedRule ) )
        {
        // search item
        for ( TInt idx = 0; idx < SettingItemArray()->Count(); idx++ )
            {
            CAknSettingItem* item = SettingItemArray()->At( idx );
            if ( item->Identifier() == indexArray[selectedRule] )
                {
                // Show confirmation query for "From" and for "Until"
                if ( item->Identifier() == EMSDateFrom ||
                     item->Identifier() == EMSDateUntil )
                	{
    				if ( iView->ShowQueryL( R_MSERV_VIDEO_RULE_QUERY ) )
        				{
                		// set item visible
                		item->SetHidden( EFalse );
        				}
                	}
	            else
	                {
	           		item->SetHidden( EFalse );
	                }
                }
            }
        HandleChangeInItemArrayOrVisibilityL();
        }
    indexArray.Close();
    CleanupStack::PopAndDestroy( ruleNames );
    CleanupStack::PopAndDestroy(); // resource stuff
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SetListType
// Sets list type based on given command
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SetListType( TInt aType )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SetListType"));

    switch ( aType )
        {
        case EMSCmdNewImageList:
            {
            iRule->SetMediaType( ECmImage );
            iMediaType = ECmImage;
            break;
            }
        case EMSCmdNewVideoList:
            {
            iRule->SetMediaType( ECmVideo );
            iMediaType = ECmVideo;
            break;
            }
        case EMSCmdNewMusicList:
            {
            iRule->SetMediaType( ECmAudio );
            iMediaType = ECmAudio;
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::SetListType \
            type not set"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::GetRuleValuesL
// Loads rule values for the list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::GetRuleValuesL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::GetRuleValuesL"));

    HBufC* listName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                iRule->Name() );
    CleanupStack::PushL( listName );
    iListName.Copy( *listName );
    CleanupStack::PopAndDestroy( listName );

    // get limit type
    iLimitType = iRule->LimitType();

    // get amount
    iAmount = iRule->Amount();

    if ( iRule->Status() == ECmToBeShrinked )
        {
        iShrinkImages = EMSShrinkScreenSize;

        }
    else
        {
        iShrinkImages = EMSShrinkingOff;
        }

    // get start with
    iStartWith = iRule->Method();

    InitializeValues();

    // Read additional rules
    TInt ruleCount = iRule->RuleCount();
    // Cycle trough all rules
    for ( TInt ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++ )
        {
        TCmMetadataField metaData;
        TCmOperatorType operatorType;
        TInt paramCount(0);

        iRule->RuleL( ruleIndex, &metaData, &operatorType, &paramCount );


        // Cycle trough all params
        for ( TInt paramIndex = 0; paramIndex < paramCount; paramIndex++ )
            {
            TPtrC8 strParam;
            iRule->RuleParamL( ruleIndex, paramIndex, &strParam );

            // read index value
            TInt ruleParam( 0 );
            iRule->RuleParamL( ruleIndex, paramIndex, ruleParam );

            switch ( metaData )
                {
                case ECmArtist:
                    {
                    iArtistArray.AppendL( ruleParam );
                    break;
                    }
                case ECmAlbum:
                    {
                    iAlbumArray.AppendL( ruleParam );
                    break;
                    }
                case ECmGenre:
                    {
                    iGenreArray.AppendL( ruleParam );
                    break;
                    }
                case ECmTitle:
                    {
                    if ( iMediaType == ECmAudio )
                        {
                        iTrackArray.AppendL( ruleParam );
                        }
                    else // video
                        {
                        iVideoTitleArray.AppendL( ruleParam );
                        }

                    break;
                    }
                case ECmDuration:
                    {
                    TLex8 lex( strParam );
                    if ( operatorType == ECmGreater )
                        {
                        lex.Val( iMinTrackLength );
                        }
                    else if ( operatorType == ECmSmaller )
                        {
                        lex.Val( iMaxTrackLength );
                        }
                    break;
                    }
                case ECmDate:
                    {
                    TInt64 value;
                    TLex8 lex(strParam);
                    lex.Val(value);

                    if ( operatorType == ECmGreater )
                        {
                        iDateFrom = TTime( value );
                        }
                    if ( operatorType == ECmSmaller )
                        {
                        iDateUntil = TTime( value );
                        }
                    break;
                    }
                case ECmFileSize:
                    {
                    TLex8 lex( strParam );
                    if ( operatorType == ECmGreater )
                        {
                        lex.Val( iMinFileSize );
                        iMinFileSize /= KMega;
                        // not needed in audio case
                        iMinFileSizeSet = EMSSetSize;
                        }
                    else if ( operatorType == ECmSmaller )
                        {
                        lex.Val( iMaxFileSize );
                        iMaxFileSize /= KMega;
                        // not needed in audio case
                        iMaxFileSizeSet = EMSSetSize;
                        }
                    break;
                    }
                case ECmBitrate:
                    {
                    break;
                    }
                case ECmResolution:
                    {
                    break;
                    }
                case ECmFreeText:
                    {
                    HBufC* listName =
                        CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                                            strParam );
                    CleanupStack::PushL( listName );
                    iFreeText.Copy( *listName );
                    CleanupStack::PopAndDestroy( listName );
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }

    if  ( iMediaType == ECmAudio )
        {
        SetAudioFileSizeSelection();
        SetAudioFileDurationSelection();
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::InitializeValues
// Initializes values of rules
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::InitializeValues()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::InitializeValues"));

    // set date
    iDateFrom.Set(KDefaultFromDate);
    // set date
    iDateUntil.HomeTime();
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::DeleteRule
// Deletes rule from the list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::DeleteRule( TCmMetadataField aField )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::DeleteRule"));

    iRule->DeleteRule( aField );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::DeleteRule
// Deletes rule from the list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::DeleteRule( TCmMetadataField aField,
                                      TCmOperatorType aType  )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::DeleteRule"));

    iRule->DeleteRule( aField, aType );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::AddAdditionalRuleL
// Adds additional rules to the list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::AddAdditionalRuleL( RArray<TInt>& aArray,
                                              TCmMetadataField aField,
                                              TCmOperatorType aOperator )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::AddAdditionalRuleL"));

    TInt ruleIndex = iRule->AddRuleL( aField, aOperator );
    for ( TInt idx = 0; idx < aArray.Count(); idx++)
        {
        iRule->AddRuleParamL( ruleIndex, aArray[ idx ] );
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::AppendItemToListL
// Appends item to the list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::AppendItemToListL( RArray<TInt>* aArray,
                                          TInt aParam )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::AppendItemToListL"));

    aArray->AppendL( aParam );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::AddAdditionalRuleL
// sets numeric type rule
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::AddAdditionalRuleL( TCmMetadataField aMetadataField,
                                              TCmOperatorType aOperator,
                                              TInt aValue )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::AddAdditionalRuleL"));

    TInt ruleIndex = iRule->AddRuleL( aMetadataField, aOperator );
    HBufC8* str = HBufC8::NewLC( 10 );
    str->Des().Format(_L8("%d"), aValue );
    iRule->AddRuleParamL( ruleIndex, *str );
    CleanupStack::PopAndDestroy( str );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SaveTrackFileSizeL
// Saves track file size
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SaveTrackFileSizeL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveTrackFileSizeL"));

    switch ( iFileSizeEnum )
        {
        case EMSFileSizeLess_1_mb:
            {
            AddAdditionalRuleL( ECmFileSize, ECmSmaller, KMSSize_1_MB );
            break;
            }
        case EMSFileSize_15_mb:
            {
            AddAdditionalRuleL( ECmFileSize, ECmGreater, KMSSize_1_MB );
            AddAdditionalRuleL( ECmFileSize, ECmSmaller, KMSSize_5_MB );
            break;
            }
        case EMSFileSize_510_mb:
            {
            AddAdditionalRuleL( ECmFileSize, ECmGreater, KMSSize_5_MB );
            AddAdditionalRuleL( ECmFileSize, ECmSmaller, KMSSize_10_MB );
            break;
            }
        case EMSFileSize_1050_mb:
            {
            AddAdditionalRuleL( ECmFileSize, ECmGreater, KMSSize_10_MB );
            AddAdditionalRuleL( ECmFileSize, ECmSmaller, KMSSize_50_MB );
            break;
            }
        case EMSFileSizeMore_10:
            {
            AddAdditionalRuleL( ECmFileSize, ECmGreater, KMSSize_50_MB );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
            SetAudioFileSizeSelection enum not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SetAudioFileSizeSelection
// set selection for audio file size
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SetAudioFileSizeSelection()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    SetAudioFileSizeSelection"));

    if ( ( KMega * iMinFileSize ) == KMSSize_0_MB &&
        ( KMega * iMaxFileSize ) == KMSSize_1_MB )
        {
        iFileSizeEnum = EMSFileSizeLess_1_mb;
        }
    else if ( ( KMega * iMinFileSize ) == KMSSize_1_MB &&
              ( KMega * iMaxFileSize ) == KMSSize_5_MB )
        {
        iFileSizeEnum = EMSFileSize_15_mb;
        }
    else if ( ( KMega * iMinFileSize ) == KMSSize_5_MB &&
              ( KMega * iMaxFileSize ) == KMSSize_10_MB )
        {
        iFileSizeEnum = EMSFileSize_510_mb;
        }
    else if ( ( KMega * iMinFileSize ) == KMSSize_10_MB &&
              ( KMega * iMaxFileSize ) == KMSSize_50_MB )
        {
        iFileSizeEnum = EMSFileSize_1050_mb;
        }
    else if ( ( KMega * iMinFileSize ) == KMSSize_50_MB &&
              ( KMega * iMaxFileSize ) == KMSSize_0_MB )
        {
        iFileSizeEnum = EMSFileSizeMore_10;
        }
    else
        {
        iFileSizeEnum = EMSFileSizeAny;
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SaveTrackDurationL
// Saves track duration
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SaveTrackDurationL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveTrackDurationL"));

    switch ( iTrackLengthEnum)
        {
        case EMSTrackLenLess_1:
            {
            AddAdditionalRuleL( ECmDuration, ECmSmaller,
                                KMSTrackLength_1_min );
            break;
            }
        case EMSTrackLen_16:
            {
            AddAdditionalRuleL( ECmDuration, ECmGreater,
                                KMSTrackLength_1_min );
            AddAdditionalRuleL( ECmDuration, ECmSmaller,
                                KMSTrackLength_6_min );
            break;
            }
        case EMSTrackLen_68:
            {
            AddAdditionalRuleL( ECmDuration, ECmGreater,
                                KMSTrackLength_6_min );
            AddAdditionalRuleL( ECmDuration, ECmSmaller,
                                KMSTrackLength_8_min );
            break;
            }
        case EMSTrackLen_830:
            {
            AddAdditionalRuleL( ECmDuration, ECmGreater,
                                KMSTrackLength_8_min );
            AddAdditionalRuleL( ECmDuration, ECmSmaller,
                                KMSTrackLength_30_min );
            break;
            }
        case EMSTrackLenMore_4:
            {
            AddAdditionalRuleL( ECmDuration, ECmGreater,
                                KMSTrackLength_30_min );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::SaveTrackDurationL\
            enum not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SetAudioFileDurationSelection
// set selection for audio file duration
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SetAudioFileDurationSelection()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    SetAudioFileDurationSelection"));

    if ( iMinTrackLength == KMSTrackLength_0_min &&
         iMaxTrackLength == KMSTrackLength_1_min )
        {
        iTrackLengthEnum = EMSTrackLenLess_1;
        }
    else if ( iMinTrackLength == KMSTrackLength_1_min &&
              iMaxTrackLength == KMSTrackLength_6_min )
        {
        iTrackLengthEnum = EMSTrackLen_16;
        }
    else if ( iMinTrackLength == KMSTrackLength_6_min &&
              iMaxTrackLength == KMSTrackLength_8_min )
        {
        iTrackLengthEnum = EMSTrackLen_68;
        }
    else if ( iMinTrackLength == KMSTrackLength_8_min &&
              iMaxTrackLength == KMSTrackLength_30_min )
        {
        iTrackLengthEnum = EMSTrackLen_830;
        }
    else if ( iMinTrackLength == KMSTrackLength_30_min &&
              iMaxTrackLength == KMSTrackLength_0_min )
        {
        iTrackLengthEnum = EMSTrackLenMore_4;
        }
    else
        {
        iTrackLengthEnum = EMSTrackLenAny;
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::DisplayModifiedAdditionalRules
// Displays additional rule if user has set value on it.
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::DisplayModifiedAdditionalRules()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    DisplayModifiedAdditionalRules"));

    CAknSettingItemArray* itemArray = SettingItemArray();

    // loop trough rules
    for ( TInt index = 0; index < itemArray->Count(); index++ )
        {
        CAknSettingItem* item= itemArray->At( index );
        if ( item->IsHidden() )
            {
            switch ( item->Identifier() )
                {
                case EMSFreeText:
                    {
                    item->SetSettingPageFlags(
                                CAknTextSettingPage::EZeroLengthAllowed );
                    if ( iFreeText.Size() )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                case EMSServers:
                    {
                    if ( iRule->MediaServerCount() )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                case EMSDateFrom:
                    {
                    if ( iDateFrom != TTime( KDefaultFromDate ) )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                case EMSDateUntil:
                    {
                    TTime currentDate;
                    currentDate.HomeTime();
					
					if ( iDateUntil.DateTime().Day() != currentDate.DateTime().Day() || 
					     iDateUntil.DateTime().Month() != currentDate.DateTime().Month() || 
					     iDateUntil.DateTime().Year() != currentDate.DateTime().Year() )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                case EMSTrackLength:
                    {
                    if ( iTrackLengthEnum )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                case EMSFileSize:
                    {
                    if ( iFileSizeEnum )
                        {
                        item->SetHidden( EFalse );
                        }
                    break;
                    }
                default:
                    {
                    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
                    SetAudioFileSizeSelection item not found"));
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SearchItemById
// Searches item from the itemarray by given ID
// --------------------------------------------------------------------------
//
CAknSettingItem* CMSFillRuleEditList::SearchItemById( TMSFillEditItems aId )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SearchItemById"));

    CAknSettingItem* item = NULL;
    CAknSettingItemArray* itemArray = SettingItemArray();

    for ( TInt index = 0; index < itemArray->Count(); index++ )
        {
        if ( itemArray->At( index )->Identifier() == aId )
            {
            item = itemArray->At( index );
            index = itemArray->Count(); // break out from the loop
            }
        }
    return item;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SetShrinkingStatus
// Sets shrinking status to fill list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SetShrinkingStatus()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::SetShrinkingStatus"));

    switch ( iShrinkImages )
        {
        case EMSShrinkScreenSize:
            {
            iRule->SetStatus( ECmToBeShrinked );
            break;
            }
        case EMSShrinkingOff:
            {
            iRule->SetStatus( ECmToBeFilled );
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
            SetShrinkingStatus shrink status not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::ReadyL()
// engine observer callback
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::ReadyL( TCmService aService, TInt /*aError*/ )
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::ReadyL"));

    switch ( aService )
        {
        case ECmServicePreProcessingFilllist:
            {
            // we'll lost iRule after update...
            iAppUi->FillRuleController()->UpdateArrayL();
            // ...so get it again
            iRule = (*iAppUi->FillRuleController()->
                            FillListArray())[iSelectedList]->List();
            // show file info
            CreateAndSetNaviPaneTextL();

            if ( iState == EMSFetchMetadata )
                {
                if( iRule->MediaType() == ECmVideo )
	                {
	                // reset track array
	                iTrackArray.Reset();
	                // append the select video title in track array
	                TInt count = iVideoTitleArray.Count();
	                if( count > 0 )
		                {
		                for( TInt i=0; i<count; i++ )
			                {
			                iTrackArray.AppendL( iVideoTitleArray[i] );	
			                }
		                }
	                	
	                }
                iMetadataCollector->GetFilteredMetadataL();

                // Show 'reading data' text on navi pane
                HBufC* naviText = StringLoader::LoadLC(
                                            R_MS_WAIT_GETTING_DATA_TEXT );
                iView->SetNavigationPaneTextL( *naviText );
                CleanupStack::PopAndDestroy( naviText );

                }
            iState = EMSIdle;
            break;
            }
        case ECmServiceFill:
            {
            UpdateSettingItemsL();
            if ( iNewList )
                {
                SaveListValuesL();
                iAppUi->FillRuleController()->SaveRulesL();
                PreprocessListL();
                }
            else
                {
                // show item counts
                CreateAndSetNaviPaneTextL();
                }

            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMSFillRuleEditList::ReadyL \
            service not found"));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::UpdateSettingItemsL()
// updates items in multiselection settings
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::UpdateSettingItemsL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::UpdateSettingItemsL"));

    // update setting item text
    CAknSettingItem* item = NULL;
    if ( iMediaType == ECmVideo )
        {
         item = SearchItemById( EMSTitleName );
        }
    else
        {
         item = SearchItemById( EMSTrack );
        }

    CMSRuleMultiselectionSetting* settingItem =
                        static_cast<CMSRuleMultiselectionSetting*> (item);
        if ( item )
        {
        settingItem->CreateItemListL();
        //DrawDeferred();
        }

    if ( iMediaType == ECmAudio )
        {
        // update setting item text
        item = SearchItemById( EMSArtist );
         settingItem =
                     static_cast<CMSRuleMultiselectionSetting*> (item);
        if ( item )
            {
            settingItem->CreateItemListL();
            //DrawDeferred();
            }

        // update setting item text
        item = SearchItemById( EMSGenre );
        settingItem = static_cast<CMSRuleMultiselectionSetting*> (item);
        if ( item )
            {
            settingItem->CreateItemListL();
            //DrawDeferred();
            }
        // update setting item text
        item = SearchItemById( EMSAlbum );
        settingItem = static_cast<CMSRuleMultiselectionSetting*> (item);
            if ( item )
            {
            settingItem->CreateItemListL();
            }
        }
    DrawDeferred();
    }

// Sets navi pane text
// --------------------------------------------------------------------------
// CMSFillRuleEditList::CreateAndSetNaviPaneTextL()
// Sets navi pane text
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::CreateAndSetNaviPaneTextL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    CreateAndSetNaviPaneTextL"));

    RPointerArray<CMSFillList>* fillListArray =
                        iAppUi->FillRuleController()->FillListArray();

    // Set navi pane text
    // magic: only two numbers
    CArrayFix<TInt>* infoArray = new ( ELeave ) CArrayFixFlat<TInt>(2);
    CleanupStack::PushL( infoArray );

    TReal size( 0 );
    Math::Round( size,
                 TReal((*fillListArray)[iSelectedList]->ListSize()) / KMega,
                 0 );
    TUint32 count = (*fillListArray)[iSelectedList]->ItemCount();

    HBufC* naviText = NULL;

    switch ( count )
        {
        case 0:
            {
            naviText = StringLoader::LoadLC(
                        R_MS_RULE_NAVI_0_ITEMS, size );
            break;
            }
        case 1:
            {
            naviText = StringLoader::LoadLC(
                        R_MS_RULE_NAVI_1_ITEM, size );
            break;
            }
        default:
            {
            infoArray->AppendL( size );
            infoArray->AppendL( count );
            naviText = StringLoader::LoadLC(
                        R_MS_RULE_NAVI_MANY_ITEMS, *infoArray );
            break;
            }
        }

    // do number conversion
    TPtr ptr = naviText->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

    iView->SetNavigationPaneTextL( *naviText );
    CleanupStack::PopAndDestroy( naviText );
    CleanupStack::PopAndDestroy( infoArray );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::RemoveCurrentRule
// Hides current rule
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::RemoveCurrentRuleL()
    {
    LOG( _L( "[MediaServant]\t CMSFillRuleEditList::RemoveCurrentRuleL" ) );

    CAknSettingItemArray* itemArray = SettingItemArray();

    TInt currentItem = ListBox()->CurrentItemIndex();
    TInt realIndex = itemArray->ItemIndexFromVisibleIndex(currentItem);

    CAknSettingItem* item= (*itemArray)[realIndex];

    // free text must be deleted here because user can't enter empty text
    switch ( item->Identifier() )
        {
        case EMSFreeText:
            {
            DeleteRule( ECmFreeText );
            iFreeText.Zero();
            item->LoadL();
            break;
            }
        case EMSServers:
            {
            DeleteAllServersFromRuleL();
            break;
            }
        case EMSDateFrom:
            {
            DeleteRule( ECmDate );
		    iDateFrom.Set( KDefaultFromDate );
            break;
            }
        case EMSDateUntil:
            {
            DeleteRule( ECmDate );
    		iDateUntil.HomeTime();
            break;
            }
        case EMSTrackLength:
            {
            DeleteRule( ECmDuration );
            break;
            }
        case EMSFileSize:
            {
            DeleteRule( ECmFileSize );
            break;
            }
        default:
            {
            LOG( _L( "[MediaServant]\t CMSFillRuleEditList::\
            RemoveCurrentRuleL no such item" ) );
            break;
            }
        }

    // Menu command is shown only hidden items
    item->SetHidden( ETrue );
    // visibility changed
    HandleChangeInItemArrayOrVisibilityL();
    // cancel ongoing operation
    iView->CancelAsyncOperation( iAppUi );
    // set idle state
    iState = EMSIdle;
    // update list info
    SaveAndPreprocesslistL( item );
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::IsCurrentRuleAdditional
// Tests if current rule is additional
// --------------------------------------------------------------------------
//
TBool CMSFillRuleEditList::IsCurrentRuleAdditional()
    {
    LOG( _L( "[MediaServant]\t CMSFillRuleEditList::\
    IsCurrentRuleAdditional" ) );

    TBool isAdditional = EFalse;

    TInt currentItem = ListBox()->CurrentItemIndex();

    CAknSettingItemArray* itemArray = SettingItemArray();
    TInt realIndex = itemArray->ItemIndexFromVisibleIndex(currentItem);
    CAknSettingItem* item= (*itemArray)[realIndex];

    TInt id = item->Identifier();

    if ( id == EMSServers ||
         id == EMSFreeText ||
         id == EMSTrackLength ||
         id == EMSFileSize )
        {
        isAdditional = ETrue;
        }
    // From and Until are additional rules in Video list    
    else if ( iMediaType == ECmVideo && 
              ( id == EMSDateFrom || id == EMSDateUntil ) )
    	{
    	isAdditional = ETrue;
    	}
    
    return isAdditional;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::ListMediaType
// Returns media type of the list
// --------------------------------------------------------------------------
//
TCmMediaType CMSFillRuleEditList::ListMediaType()
    {
    return iMediaType;
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::SizeChanged()
    {
    ListBox()->SetRect( Rect() ); // Set rectangle of listbox.
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::HandleResourceChange
// Handles layout change
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }

    CCoeControl::HandleResourceChange( aType );
    }

// --------------------------------------------------------------------------
// CMSRuleServersSetting::CreateServerSettingListL
// Creates selection item list
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::CreateServerSettingListL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    CreateServerSettingListL"));

    if ( iServerArray )
        {
        iServerArray->ResetAndDestroy();
        delete iServerArray;
        iServerArray = NULL;
        }

    iServerArray =
            new ( ELeave ) CSelectionItemList( 8 );

    TBuf8<KMaxFileName> itemText;
    TInt selectedItemCount( 0 );

    TInt serverCount(0);
    if ( iServers )
        {
        serverCount = iServers->Count();
        }

    // Cycle trough all servers
    for ( TInt idx = 0; idx < serverCount; idx++ )
        {
        // get server
        CCmMediaServerFull* server = (*iServers)[idx];

        // Add all servers to the list
        HBufC* devName =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                                    server->MediaServerName() );
        CleanupStack::PushL( devName );
        CSelectableItem* item = new ( ELeave ) CSelectableItem(
                                                    *devName, EFalse);

        CleanupStack::PushL( item );
        // item must be constructed
        item->ConstructL();

        // count of servers set to rule
        TInt ruleServerCount = iRule->MediaServerCount();

        // Cycle trough all servers
        for (TInt index = 0; index < ruleServerCount; index++ )
            {
            TInt serverId( 0 );
            iRule->MediaServerL( index, serverId );

            if ( serverId == server->DbId() ) // compare IDs
                {
                // found - break this loop
                index = ruleServerCount;
                // found - mark selected
                item->SetSelectionStatus( ETrue );
                selectedItemCount++;
                }
            }
        // append item to list
        iServerArray->AppendL(item);
        CleanupStack::Pop( item );
        CleanupStack::PopAndDestroy( devName );
        }

    // Create and append default item and put it first
    HBufC* itemName = StringLoader::LoadLC( R_MS_VALUE_ANY_SOURCE_DEVICE );
    CSelectableItem* item = NULL;
    if ( selectedItemCount )
        {
        item = new ( ELeave ) CSelectableItem( *itemName, EFalse );
        }
    else
        {
        item = new ( ELeave ) CSelectableItem( *itemName, ETrue );
        }

    CleanupStack::PushL( item );
    item->ConstructL();
    iServerArray->InsertL(0, item);
    CleanupStack::Pop( item );
    CleanupStack::PopAndDestroy( itemName );

    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::HandleServerSelectionL
// Adds selected servers from the list to the rule
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::HandleServerSelectionL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    HandleServerSelectionL"));

    DeleteAllServersFromRuleL();

    // Check if first item ("Any") selected
    if ( (*iServerArray)[0]->SelectionStatus() == EFalse )
       {
       for ( TInt index = 1; index < iServerArray->Count(); index++ )
            {
            CSelectableItem* item = (*iServerArray)[index];
            TBool selected = item->SelectionStatus();

            CCmMediaServerFull* server = (*iServers)[index-1];

            // add server
            if ( item->SelectionStatus() )
                {
                iRule->AddMediaServerL( server->DbId() );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::DeleteAllServersFromRuleL
// Deletes all servers from rule
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::DeleteAllServersFromRuleL()
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::\
    DeleteAllServersFromRuleL"));

    // count of servers set to rule
    TInt ruleServerCount = iRule->MediaServerCount();

    for ( TInt index = ruleServerCount-1; index >= 0; index-- )
        {
        TInt serverId( 0 );
        iRule->MediaServerL( index , serverId );
        iRule->DeleteMediaServer( serverId );
        }
    }

// --------------------------------------------------------------------------
// CMSFillRuleEditList::GetServersAndCheckActivityL
// Gets mediaservers and removes not active servers
// --------------------------------------------------------------------------
//
void CMSFillRuleEditList::GetServersAndCheckActivityL()
    {
    iServers = iAppUi->MSEngine()->GetMediaServersL();

    for ( TInt i = 0; i < iServers->Count() ; i++ )
        {
        CCmMediaServerFull* server = (*iServers)[i];
        // remove server if it is not tagged as fill server
        if ( !server->FillUsage() )
            {
            delete server;
            iServers->Remove(i);
            i--;
            }
        }
    iServers->Compress();
    }

// ---------------------------------------------------------------------------
// CMSFillRuleEditList::GetHelpContext
// ---------------------------------------------------------------------------
//
void CMSFillRuleEditList::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOG(_L("[MediaServant]\t CMSFillRuleEditList::GetHelpContext"));

    aContext.iMajor = TUid::Uid(KMediaServantUID3);
    aContext.iContext = KMSERV_HLP_EDIT_RULES;
    }

// End of File

