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
* Description:  Multiselection popup implementation
*
*/


// INCLUDE FILES
#include <avkon.mbg>
#include <AknsUtils.h>
#include <cmsappwizard.rsg>

#include "cmsmultiselectionpopup.h"
#include "msdebug.h"

// Format string for listbox items
_LIT(KItemFormatString, "1\t%S");

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::NewL
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup* CMSMultiselectionPopup::NewL(
    CDesCArrayFlat* aItemArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    const TDesC& aHeading )
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::NewL begin"));

    CMSMultiselectionPopup* self =
                    CMSMultiselectionPopup::NewLC( aItemArray,
                                                   aSelectionIndexArray,
                                                   aHeading );
    CleanupStack::Pop( self );

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::NewL end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::NewLC
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup* CMSMultiselectionPopup::NewLC(
    CDesCArrayFlat* aItemArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    const TDesC& aHeading )
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::NewLC begin"));

    CMSMultiselectionPopup* self =
                        new(ELeave) CMSMultiselectionPopup( NULL,
                                                            aHeading );

    CleanupStack::PushL(self);
    self->ConstructL( aItemArray, aSelectionIndexArray );

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::NewLC end"));
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::ConstructL
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::ConstructL(
    CDesCArrayFlat* aItemArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray )
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
    ConstructL begin"));

    iItemArray = aItemArray;

    iSelectionIndexes = aSelectionIndexArray;

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
    ConstructL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::CMSMultiselectionPopup
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup::CMSMultiselectionPopup(
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    const TDesC& aHeading )
    : CAknListQueryDialog( aSelectionIndexArray ),
      iHeading( aHeading )

    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            CMSMultiselectionPopup"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::~CMSMultiselectionPopup
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup::~CMSMultiselectionPopup()
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
    CMSMultiselectionPopup"));
    }
    
// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::UpdateAndDrawPopup
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::UpdateAndDrawPopupL(
    CDesCArrayFlat* aItemArray )
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            UpdateAndDrawPopupL begin"));

    iItemArray = aItemArray;

    if ( iItemArray->Count() )
        {
        CreateAndSetListboxItemsL();        
        }
               
    Layout();
    SizeChanged();    
    iListBox->DrawNow();

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            UpdateAndDrawPopupL ends"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::AppendIconToArrayL
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId) const
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            AppendIconToArrayL begin"));

    __ASSERT_DEBUG( aArray, User::Leave(KErrArgument) );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    icon->SetBitmapsOwnedExternally(EFalse);

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL(icon);

    aArray->AppendL(icon);

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop(icon);
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            AppendIconToArrayL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::OkToExitL(TInt aButtonId)
// ---------------------------------------------------------------------------
//
TBool CMSMultiselectionPopup::OkToExitL(TInt aButtonId)
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
OkToExitL begin"));

    if ( aButtonId == EAknSoftkeyNext ||  aButtonId == EAknSoftkeyBack )
        {
        LOG(_L("[MSAppWizard]\t CUPnPAppMultiselectionPopup::\
OkToExitL: Next or Back"));

        const CListBoxView::CSelectionIndexArray* selected =
            ListBox()->SelectionIndexes();

        if ( iSelectionIndexes )
            {
            TInt selectCount = selected->Count();
            iSelectionIndexes->Reset();
            for ( TInt i = 0; i < selectCount; i++ )
                {
                iSelectionIndexes->AppendL( selected->At( i ) );
                }
            }
        }

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
OkToExitL end"));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::CreateAndSetListboxItemsL()
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::CreateAndSetListboxItemsL()
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup::\
    CreateAndSetListboxItemsL"));    
    
    TInt itemCount = iItemArray->Count();
    
    CDesCArrayFlat* listItems = new (ELeave) CDesCArrayFlat( 5 );
    CleanupStack::PushL( listItems );
    
    for ( TInt i = 0; i < itemCount; i++)
        {
        TPtrC itemStr = iItemArray->MdcaPoint(i);
        HBufC* item = HBufC::NewLC( itemStr.Size() +
                                    sizeof( KItemFormatString ) );                                
        item->Des().Format( KItemFormatString, &itemStr );             
        
        listItems->AppendL( *item );
        CleanupStack::PopAndDestroy( item );        
        }
    SetItemTextArray( listItems );
    CleanupStack::Pop( listItems );
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::PreLayoutDynInitL()
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::PreLayoutDynInitL()
    {
    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            PreLayoutDynInitL begin"));

    // two icons
    CAknIconArray* icons = new (ELeave) CAknIconArray(2);
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName() );

    AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOn,
                        EMbmAvkonQgn_indi_checkbox_on,
                        EMbmAvkonQgn_indi_checkbox_on_mask );

    AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnPropCheckboxOff,
                        EMbmAvkonQgn_indi_checkbox_off,
                        EMbmAvkonQgn_indi_checkbox_off_mask );


    CAknListQueryDialog::SetIconArrayL(icons);

    CleanupStack::Pop(icons);

    iListBox = ListBox();

    CAknFilteredTextListBoxModel* model = STATIC_CAST(
            CAknFilteredTextListBoxModel*,
            iListBox->Model());

    // create listbox items if any
    if ( iItemArray->Count() )
        {
        CreateAndSetListboxItemsL();        
        }
    
    SetHeaderTextL( iHeading );

    iListBox->ActivateL();
    
    CAknListQueryDialog::PreLayoutDynInitL();

    LOG(_L("[MSAppWizard]\t CMSMultiselectionPopup:: \
            PreLayoutDynInitL end"));
    }

// End of file


