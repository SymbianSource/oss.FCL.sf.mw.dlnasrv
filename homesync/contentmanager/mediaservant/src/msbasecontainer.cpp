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
* Description:  CMSBaseContainer class implementation
*
*/



// INCLUDE FILES
#include <eikclbd.h>

#include <aknlists.h>
#include <barsread.h>
#include <AknIconArray.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <akncontext.h>

#include "msbasecontainer.h"
#include "msconstants.h"
#include "msdebug.h"

// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CMSBaseContainer::CMSBaseContainer()
// Default constructor.
// --------------------------------------------------------------------------
//
CMSBaseContainer::CMSBaseContainer()
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::CMSBaseContainer"));

    iCoeEnv = CEikonEnv::Static();
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::~CMSBaseContainer()
// Destructor.
// --------------------------------------------------------------------------
//
CMSBaseContainer::~CMSBaseContainer()
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::~CMSBaseContainer"));
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::SetListBoxFromResourceL()
// Sets listbox from resource using ConstructFromResourceL() of
// CEikColumnListBox class.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::SetListBoxFromResourceL(
    CEikColumnListBox* aListBox, const TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::SetListBoxFromResourceL \
    column listbox"));

    if ( aListBox && aResourceId )
        {
        aListBox->SetContainerWindowL( *this );

        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
        aListBox->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // resource stuffs.

        // Creates scrollbar.
        CreateScrollbarL( aListBox );
        }
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::SetListBoxFromResourceL()
// Sets listbox from resource using ConstructFromResourceL() of
// CEikFormattedCellListBox class.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::SetListBoxFromResourceL(
    CEikFormattedCellListBox* aListBox,
    const TInt aResourceId )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::SetListBoxFromResourceL \
    formatted cell listbox"));

    if ( aListBox && aResourceId )
        {
        aListBox->SetContainerWindowL( *this );

        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
        aListBox->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // resource stuffs.

        // Creates scrollbar.
        CreateScrollbarL( aListBox );
        }
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::AppendItemToListBoxArrayL()
// Appends item to the list
// --------------------------------------------------------------------------
//
void CMSBaseContainer::AppendItemToListBoxArrayL( CEikListBox* aListBox,
                                                  CTextListBoxModel* aModel,
                                                  TInt aIconIndex,
                                                  TDesC& aPrimaryText,
                                                  TDesC& aSecondaryText )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::AppendItemToListBoxArrayL"));


    if ( aListBox &&  aModel )
        {
        MDesCArray *itemList = aModel->ItemTextArray();
        CDesCArray *itemArray = static_cast<CDesCArray*>( itemList );

    TBuf<2> iconIndex; // magic: maximum icon count is 99
    iconIndex.AppendNum( aIconIndex );

    TInt newLen = iconIndex.Length() +
                  KSlashT().Length() +
                  (&aPrimaryText ? aPrimaryText.Length() : 0) +
                  KSlashT().Length() +
                  (&aSecondaryText ? aSecondaryText.Length() : 0);

    HBufC* newText = HBufC::NewMaxLC( newLen );

    // Create new item string
    TPtr newTPtr = newText->Des();

    newTPtr.Copy( iconIndex );
    newTPtr.Append( KSlashT );

    if ( &aPrimaryText )
        {
        newTPtr.Append( aPrimaryText );
        }
    newTPtr.Append( KSlashT );

    if ( &aSecondaryText )
        {
        newTPtr.Append( aSecondaryText );
        }

    itemArray->AppendL(newTPtr);
    CleanupStack::PopAndDestroy( newText );

    aListBox->HandleItemAdditionL();
        }
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::AppendItemToListBoxArrayL()
// Appends item to the list
// --------------------------------------------------------------------------
//
void CMSBaseContainer::AppendItemToListBoxArrayL( CEikListBox* aListBox,
                                                  CTextListBoxModel* aModel,
                                                  TInt aIconIndex,
                                                  TDesC& aItemText )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::AppendItemToListBoxArrayL"));


    if ( aListBox &&  aModel )
        {
        MDesCArray *itemList = aModel->ItemTextArray();
        CDesCArray *itemArray = static_cast<CDesCArray*>( itemList );

    TBuf<2> iconIndex; // maximum icon count is 99
    iconIndex.AppendNum( aIconIndex );

    TInt newLen = iconIndex.Length() +
                  KSlashT().Length() +
                  (&aItemText ? aItemText.Length() : 0) +
                  KSlashT().Length() +
                  iconIndex.Length();

    HBufC* newText = HBufC::NewMaxLC( newLen );

    // Create new item string
    TPtr newTPtr = newText->Des();

    newTPtr.Copy( iconIndex );
    newTPtr.Append( KSlashT );

    if ( &aItemText )
        {
        newTPtr.Append( aItemText );
        }
    newTPtr.Append( KSlashT );

    itemArray->AppendL(newTPtr);
    CleanupStack::PopAndDestroy( newText );

    aListBox->HandleItemAdditionL();
        }
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::AppendIconToArrayL()
// Adds icon to iconarray
// --------------------------------------------------------------------------
//
void CMSBaseContainer::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId)
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::AppendIconToArrayL"));

    __ASSERT_DEBUG( aArray, User::Leave(KErrArgument) );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    icon->SetBitmapsOwnedExternally(EFalse);

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL(icon);

    aArray->AppendL(icon);

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop(icon);

    }

// --------------------------------------------------------------------------
// CMSBaseContainer::CreateScrollbarL()
// Creates scrollbar.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::CreateScrollbarL( CEikListBox* aListBox )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::CreateScrollbarL"));

    if ( aListBox )
        {
        // Creates scrollbar.
        aListBox->CreateScrollBarFrameL( ETrue );
        aListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        }
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::UpdateScrollBar()
// Update scroll bar.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::ShowNoteL ( TInt aResource, TInt aError )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::ShowNoteL"));

    // Show error note
    CAknInformationNote* dlg = new(ELeave)CAknInformationNote();
    TBuf<KMaxFileName> errorText;
    iCoeEnv->ReadResourceL(errorText, aResource );
    errorText.AppendNum( aError );
    dlg->ExecuteLD(errorText);
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::SetContextPaneIconL()
// Sets context pane icon.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::SetContextPaneIconL( const CFbsBitmap* aIconBitmap,
                                            const CFbsBitmap* aIconMask  )
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::SetContextPaneIconL"));

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL( statusPane );
    CAknContextPane* contextPane = ( CAknContextPane * )statusPane->
            ControlL( TUid::Uid( EEikStatusPaneUidContext ) );
    CleanupStack::Pop( statusPane );
    contextPane->SetPicture( aIconBitmap, aIconMask );
    }

// --------------------------------------------------------------------------
// CMSBaseContainer::SetDefaultContextPaneIconL()
// Sets default context pane icon.
// --------------------------------------------------------------------------
//
void CMSBaseContainer::SetDefaultContextPaneIconL()
    {
    LOG(_L("[MediaServant]\t CMSBaseContainer::SetDefaultContextPaneIconL"));

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL( statusPane );
    CAknContextPane* contextPane = ( CAknContextPane * )statusPane->
            ControlL( TUid::Uid( EEikStatusPaneUidContext ) );
    CleanupStack::Pop( statusPane );
    contextPane->SetPictureToDefaultL();
    }

// End of File
