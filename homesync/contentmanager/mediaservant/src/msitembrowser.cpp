/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMSItemBrowser class implementation
*
*/



#include <AknIconArray.h>
#include <mediaservant.rsg>
#include <mediaservant.mbg>
#include <avkon.mbg>
#include <aknnotedialog.h>
#include <StringLoader.h>
#include <e32math.h>

#include "cmbaselistitem.h"
#include "msappui.h"
#include "msitembrowser.h"
#include "msengine.h"
#include "msconstants.h"
#include "msdebug.h"


// --------------------------------------------------------------------------
// CMSItemBrowser::ConstructL()
// --------------------------------------------------------------------------
//
void CMSItemBrowser::ConstructL( TRect aRect )
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::ConstructL"));    

    GetBrowseDataL();

    CreateWindowL();

    iListBox = new (ELeave) CAknDoubleGraphicStyleListBox;

    SetListBoxFromResourceL(iListBox, R_BROWSE_VIEW_LISTBOX);

    // Create icons
    CAknIconArray* icons = new (ELeave) CAknIconArray( EIconCount );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // mark icon
    AppendIconToArrayL( icons, skin,
                        KAvkonMifFileName,
                        KAknsIIDDefault,
                        EMbmAvkonQgn_indi_marked_add,
                        EMbmAvkonQgn_indi_marked_add_mask );

    // image icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_prop_file_image,
                        EMbmMediaservantQgn_prop_file_image_mask );
    // audio icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_prop_file_audio,
                        EMbmMediaservantQgn_prop_file_audio_mask );
    // video icon
    AppendIconToArrayL( icons, skin,
                        KMSMifFileName,
                        KAknsIIDDefault,
                        EMbmMediaservantQgn_prop_file_video,
                        EMbmMediaservantQgn_prop_file_video_mask );


    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(icons);
    CleanupStack::Pop(icons);

    // Container is listbox observer
    iListBox->SetListBoxObserver( this );

    SetRect(aRect);

    // Add data to listbox
    UpdateListBoxDataL();

    ActivateL(); // Activates window. ( Ready to draw )
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::~CMSItemBrowser
// --------------------------------------------------------------------------
CMSItemBrowser::~CMSItemBrowser()
    {
    if ( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        iItemArray->Close();
        delete iItemArray;
        }
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::CountComponentControls()
// Returns number of components.
// --------------------------------------------------------------------------
//
TInt CMSItemBrowser::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::ComponentControl()
// Returns pointer to particular component.
// --------------------------------------------------------------------------
//
CCoeControl* CMSItemBrowser::ComponentControl( TInt /*aIndex*/) const

    {
    return iListBox; // return a pointer to the listbox
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::OfferKeyEventL()
// Handles the key events.
// --------------------------------------------------------------------------
//
TKeyResponse CMSItemBrowser::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::OfferKeyEventL"));

    TKeyResponse response = EKeyWasNotConsumed;

    if ( aType != EEventKey )
        {
        response =  EKeyWasNotConsumed;
        }

    if ( iListBox )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }

    return response;
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::HandleListBoxEventL()
// Handles listbox event.
// --------------------------------------------------------------------------
void CMSItemBrowser::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent /*aEventType*/ )
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::HandleListBoxEventL"));
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::ReadDataFromEngineL()
// Reads data items from engine
// --------------------------------------------------------------------------
//
void CMSItemBrowser::UpdateListBoxDataL()
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::UpdateListBoxDataL"));

    // Listbox items
    CDesCArray* itemArray = static_cast<CDesCArray*>
                                   (iListBox->Model()->ItemTextArray());
    // Clear list
    itemArray->Reset();

    TInt count = iItemArray->Count();

    for (TInt index = 0; index < count; index++)
        {
        TInt iconType;
        CCmBaseListItem* item = (*iItemArray)[index];
        TUint32 fileSize = item->Size();
        HBufC* fileSizeStr = NULL;
        HBufC* primaryText = HBufC::NewLC( KMaxDataTypeLength );
        HBufC* secondaryText = HBufC::NewLC( KMaxDataTypeLength );

        switch ( iMediaType )
            {
            case ECmVideo:
            // fall through
            case ECmOtherVideo:
                {
                iconType = EIconVideo;
                break;
                }
            case ECmImage:
            // fall through
            case ECmOtherImage:
                {
                iconType = EIconImage;
                break;
                }
            case ECmAudio:
                {
                iconType = EIconAudio;

                secondaryText->Des().Copy( item->SecondaryText() );
                break;
                }
            default:
                {
                iconType = EIconVideo;
                break;
                }
            }

        // Set item secondary text
        if ( fileSize < KMega )
            {
            if ( secondaryText->Length() )
                {
                secondaryText->Des().Append( KDelimiter);
                }

            fileSizeStr = StringLoader::LoadLC(
                                  R_MS_FILE_SIZE_KB, fileSize / KKilo );
            }
        else
            {
            if ( secondaryText->Length() )
                {
                secondaryText->Des().Append( KDelimiter );
                }

            TReal size( 0 );
            Math::Round( size, TReal( fileSize ) / KMega, 0 );
            fileSizeStr = StringLoader::LoadLC(
                                  R_MS_FILE_SIZE_MB, size );
            }

        secondaryText->Des().Append( *fileSizeStr );
        if ( fileSizeStr )
            {
            CleanupStack::PopAndDestroy( fileSizeStr );
            }

        // do number conversion
        TPtr ptr = secondaryText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        // add item
        AppendItemToListBoxArrayL(
                            iListBox,
                            iListBox->Model(),
                            iconType, // default icon
                            (*iItemArray)[index]->PrimaryText(),
                            *secondaryText );
        CleanupStack::PopAndDestroy( secondaryText );
        CleanupStack::PopAndDestroy( primaryText );
        }
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::SetCurrentFileStatus
// --------------------------------------------------------------------------
//
void CMSItemBrowser::SetFileStatus( TInt aIndex,
                                           TCmListItemStatus aStatus )
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::SetFileStatus"));

    (*iItemArray)[aIndex]->SetStatus( aStatus );
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::GetCurrentFileStatus
// --------------------------------------------------------------------------
//
TCmListItemStatus CMSItemBrowser::FileStatus( TInt aIndex )
    {
    LOG(_L("[MediaServant]\t CMSItemBrowser::FileStatus"));

    return (*iItemArray)[ aIndex ]->Status();
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::ListBox()
// Returns listbox
// --------------------------------------------------------------------------
//
CEikListBox* CMSItemBrowser::ListBox()
    {
    return iListBox;
    }

// --------------------------------------------------------------------------
// CMSItemBrowser::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CMSItemBrowser::SizeChanged()
    {
    // container control resize code.
    if ( iListBox )
        {
        iListBox->SetRect( Rect() ); // Set rectangle of listbox.
        }
    }
