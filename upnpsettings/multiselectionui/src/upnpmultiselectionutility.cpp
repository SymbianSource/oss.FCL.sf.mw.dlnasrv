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
* Description:      Multiselection dialog implementation
*
*/






// INCLUDE FILES
#include <StringLoader.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <aknlists.h>
#include <AknsUtils.h>

#include <cupnpmultiselectionui.rsg>
#include "upnpmultiselectionutility.h"
#include "upnpfilesharingengine.h"

_LIT( KComponentLogfile, "multiselectionui.txt");
#include "upnplog.h"


//CONSTANTS

// Format string for listbox items
_LIT( KItemFormatString, "1\t%S" );

const TInt KShareNoneIndex = 0; // listbox index of "do not share"
const TInt KShareAllIndex = 1;  // listbox index of "share all"
const TInt KPredefinedSelections = 2; // Share none and share all

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::CUpnpMultiselectionUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpMultiselectionUtility::CUpnpMultiselectionUtility()
    {
    __LOG("CUpnpMultiselectionUtility::CUpnpMultiselectionUtility");
    iShareNoneSelected = EFalse;
    iShareAllSelected = EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::CUpnpMultiselectionUtility
// C++ default destructor.
// --------------------------------------------------------------------------
//
CUpnpMultiselectionUtility::~CUpnpMultiselectionUtility()
    {
    __LOG("CUpnpMultiselectionUtility::~CUpnpMultiselectionUtility");
    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::CollectSelectedItemsL()
// Collects selected items to member variable for later use
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::CollectSelectedItemsL( 
    CEikListBox* aListBox, TBool aFirstRun )
    {
    __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL begin");

    const CArrayFix<TInt>* indexes = NULL;

    //get the true indexes of marked items
    CAknListBoxFilterItems* filter 
            = static_cast < CAknFilteredTextListBoxModel* >     
                            ( aListBox->Model() )->Filter();
    if ( filter )
        {
        // Filter knows all.
        filter->UpdateSelectionIndexesL();
        indexes = filter->SelectionIndexes();
        }
    else
        {
        // No filter.
        indexes = aListBox->View()->SelectionIndexes();
        }
    
    if ( indexes->Count() > 0 )
        {
        // obtain selection state  
        TShareSelectionState selectionState = 
            ShareSelectionStateFromArray( *indexes );

        // make some decisions based on the tabs selected
        DoSelectionLogicL( aListBox, aFirstRun,
            indexes, filter, selectionState );

        }
    else // no items selected
        {
        __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
No items selected");

        filter->ResetFilteringL();
        aListBox->View()->SelectItemL( KShareNoneIndex );
        }

    // update iShareNoneSelected and iShareAllSelected member variables
    iShareNoneSelected = EFalse;
    iShareAllSelected = EFalse;

    const CArrayFix<TInt>* indexesAfter = NULL;

    if ( filter )
        {
        // Filter knows all.
        filter->UpdateSelectionIndexesL();
        indexesAfter = filter->SelectionIndexes();
        }
    else
        {
        // No filter.
        indexesAfter = aListBox->View()->SelectionIndexes();
        }

    for ( TInt i = 0 ; i < indexesAfter->Count() ; i++ ) 
        {
        if ( indexesAfter->At( i ) == KShareNoneIndex ) 
            {
            iShareNoneSelected = ETrue;
            }
        else if ( indexesAfter->At( i ) == KShareAllIndex ) 
            {
            iShareAllSelected = ETrue;
            }
        }

    __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL end");
    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::DoSelectionLogicL
// Do some decisions based on the selection state
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::DoSelectionLogicL( 
    CEikListBox* aListBox,
    TBool aFirstRun,
    const CArrayFix<TInt>* aIndexes,
    CAknListBoxFilterItems* aFilter,
    TShareSelectionState aSelectionState )
    {
    const TInt arrayGranularity( aIndexes->Count() + 1 );

    // If "do not share" was not selected before and it is now, unselect
    // all except "do not share".
    if ( ( aSelectionState == EShareNoneSelected ||
           aSelectionState == EShareNoneAndShareAllSelected ) && 
         !iShareNoneSelected )
        {
        // Create new selectionindexarray with only "do not share" 
        // selected and assign it to the listbox.
        __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL:\
Do not share selected");
        CListBoxView::CSelectionIndexArray* noneArr = 
            new (ELeave) CArrayFixFlat<TInt>( arrayGranularity );
        CleanupStack::PushL( noneArr );
        noneArr->AppendL( KShareNoneIndex );
        aListBox->SetSelectionIndexesL( noneArr );
        CleanupStack::PopAndDestroy( noneArr );
        }
    // if "share all" was not selected before and it is now, select all
    // except "do not share". 
    // OR if this method is executed first time (from PostLayoutDynInitL)
    // and share all is selected, then we trust that all items should be 
    // selected.  
    else if ( ( ( aSelectionState == EShareAllSelected || 
                  aSelectionState == EShareNoneAndShareAllSelected ) &&
                  !iShareAllSelected ) 
              || aSelectionState == EShareAllSelected && aFirstRun )
        {
        // Create new selectionindexarray with "share all" and all other
        // items except "do not share" selected and assign it to the 
        // listbox.
        __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
All files selected");
        TInt itemCount = aFilter->NonFilteredNumberOfItems();

        CListBoxView::CSelectionIndexArray* allArr = 
            new (ELeave) CArrayFixFlat<TInt>( arrayGranularity );
        CleanupStack::PushL( allArr );
        allArr->AppendL( KShareAllIndex );
        for ( TInt i( KPredefinedSelections ); i < itemCount; i++ )
            {
            allArr->AppendL( i );
            }
        aFilter->ResetFilteringL();
        aListBox->SetSelectionIndexesL( allArr );
        CleanupStack::PopAndDestroy( allArr );
        }
    // if share all selection is removed
    else if ( aSelectionState == EItemsSelected && 
              iShareAllSelected ) 
        {
        // Create new selectionindexarray with only "do not share" 
        // selected and assign it to the listbox.
        __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
Do not share selected");
        CListBoxView::CSelectionIndexArray* noneArr = 
            new (ELeave) CArrayFixFlat<TInt>( arrayGranularity );
        CleanupStack::PushL( noneArr );
        noneArr->AppendL( KShareNoneIndex );
        aListBox->SetSelectionIndexesL( noneArr );
        CleanupStack::PopAndDestroy( noneArr );
        }
    // if only some individual items have been modified
    else if ( aSelectionState == EItemsSelected && 
              !iShareAllSelected &&
              !iShareNoneSelected ) 
        {
        // do nothing. selections are kept as they are
        }
    // if share none is selected with some individual item, 
    // unselect share none
    else if ( aSelectionState == EShareNoneSelected && aIndexes->Count() > 1 ) 
        {
        // Create similar selectionindexarray without "do not share" 
        // selected and assign it to the listbox.
        __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
Do not share selected");

        for( TInt y=0; y < aIndexes->Count(); y++ )
            {
            TInt itemIndex = aIndexes->At(y);
            if ( itemIndex == 0 || itemIndex == 1 )
                {
                aListBox->View()->DeselectItem( 
                    aFilter->VisibleItemIndex(itemIndex) );
                }
            }
        }
    else if ( aSelectionState == EShareAllSelected &&
              aIndexes->Count() < aFilter->NonFilteredNumberOfItems() - 1 ) 
        {
        if ( aIndexes->Count() > 0 ) 
            {
            // all items were selected and some item (not share all) has 
            // been unselected -> remove selection also from share all.
            __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
Do not share selected");

            for( TInt y=0; y < aIndexes->Count(); y++ )
                {
                TInt itemIndex = aIndexes->At(y);
                if ( itemIndex == KShareAllIndex )
                    {
                    aListBox->View()->DeselectItem( 
                        aFilter->VisibleItemIndex( itemIndex ) );
                    }
                }
            
            if ( !aListBox->View()->SelectionIndexes()->Count() ) 
                {
                // Last individual item has been unselected.
                // Create new selectionindexarray with only 
                // "do not share" selected and assign it to the listbox.
                __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
Do not share selected");
                CListBoxView::CSelectionIndexArray* noneArr = 
                    new (ELeave) CArrayFixFlat<TInt>( arrayGranularity );
                CleanupStack::PushL( noneArr );
                noneArr->AppendL( KShareNoneIndex );
                aListBox->SetSelectionIndexesL( noneArr );
                CleanupStack::PopAndDestroy( noneArr );
                }
            }
        else 
            {
            // some item unselected and no other items left except 
            // "share all"

            // Create new selectionindexarray with only "do not share" 
            // selected and assign it to the listbox.
            __LOG("CUpnpMultiselectionUtility::CollectSelectedItemsL: \
Do not share selected");
            CListBoxView::CSelectionIndexArray* noneArr = 
                new (ELeave) CArrayFixFlat<TInt>( arrayGranularity );
            CleanupStack::PushL( noneArr );
            noneArr->AppendL( KShareNoneIndex );
            aListBox->SetSelectionIndexesL( noneArr );
            CleanupStack::PopAndDestroy( noneArr );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::ConvertSelectionsForEngineL
// Convert the selections from UI to engine as "old style selections"
// Has effect only if "Share All" is selected
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::ConvertSelectionsForEngineL( 
    CArrayFix<TInt>& aSelections ) const
    {
    __LOG("CUpnpMultiselectionUtility::ConvertSelectionsForEngineL begin");

    for ( TInt i(0); i < aSelections.Count(); i++ )
        {
        if ( aSelections.At( i ) == KShareAllIndex )
            {
            aSelections.Reset();
            aSelections.AppendL( KShareAllIndex );
            i = aSelections.Count();
            }
        }
    
    __LOG("CUpnpMultiselectionUtility::ConvertSelectionsForEngineL end");
    }


// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::AppendIconToArray
// Load a possibly skinned icon (with mask) and append it to an
// icon array.
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId) const
    {
    __LOG("CUpnpMultiselectionUtility::AppendIconToArrayL begin");

    __ASSERT_DEBUG(aArray != NULL, User::Leave(KErrArgument));

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );

    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop(icon);
    __LOG("CUpnpMultiselectionUtility::AppendIconToArrayL end");

    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::AppendShareAllSelectionL
// Appends "share all files" item to the listbox
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::AppendShareAllSelectionL(
                                    CDesCArray* aListBoxArray ) const
    {
    __LOG("CUpnpMultiselectionUtility::AppendShareAllSelectionL begin");
    HBufC* item = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( item );
    HBufC* item2 = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( item2 );
    HBufC* itemText1 = StringLoader::LoadLC( 
        R_UPNP_MULTISELECTION_SHARE_ALL_TEXT );
    HBufC* itemText2 = StringLoader::LoadLC(
        R_UPNP_MULTISELECTION_DO_NOT_SHARE_TEXT );
    // create item string for the listbox (icon + album name)
    item->Des().Format( KItemFormatString, itemText1 );
    item2->Des().Format( KItemFormatString, itemText2 );
    // append "share all files" -selection to the listbox
    aListBoxArray->AppendL( item2->Des() );
    aListBoxArray->AppendL( item->Des() );

    CleanupStack::PopAndDestroy(2); // stringloader
    CleanupStack::PopAndDestroy(2); // item item2
    __LOG( "CUpnpMultiselectionUtility::AppendShareAllSelectionL end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::SetShareSelectionState
// (Commented in the header)
// --------------------------------------------------------------------------
//
void CUpnpMultiselectionUtility::SetShareSelectionState( 
    TShareSelectionState aSelectionState ) 
    {
    switch ( aSelectionState )
        {
        case EShareNoneSelected:
            {
            iShareNoneSelected = ETrue;
            iShareAllSelected = EFalse;
            break;
            }
        case EShareAllSelected:
            {
            iShareNoneSelected = EFalse;
            iShareAllSelected = ETrue;
            break;
            }
        case ENoShareNoneOrShareAllSelected:
            {
            iShareNoneSelected = EFalse;
            iShareAllSelected = EFalse;
            break;
            }
        default:
            {
            // Should not be reached.
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpMultiselectionUtility::ShareSelectionStateFromArray
// (Commented in the header)
// --------------------------------------------------------------------------
//
TShareSelectionState 
    CUpnpMultiselectionUtility::ShareSelectionStateFromArray( 
    const CArrayFix<TInt>& aIndexes ) const
    {
    TShareSelectionState retval = EUndefined;
    TBool shareNoneFound = EFalse;
    TBool shareAllFound = EFalse;
    TBool itemsSelected = EFalse;
        
    // Check if the indexes array contains do not share or share all.
    for ( TInt i = 0 ; i < aIndexes.Count() ; i++ ) 
        {
        if ( aIndexes.At( i ) == KShareNoneIndex ) 
            {
            shareNoneFound = ETrue;
            }
        else if ( aIndexes.At( i ) == KShareAllIndex )  
            {
            shareAllFound = ETrue;
            }
        }

    // 
    if ( !shareAllFound && !shareNoneFound ) 
        {
        // "do not share" or "share all" are not selected. Check if there are
        // other items selected.
        if ( aIndexes.Count() ) 
            {
            itemsSelected = ETrue;
            }
        }

    // set retval
    if ( itemsSelected ) 
        {
        retval = EItemsSelected;
        }    
    else if ( shareNoneFound && !shareAllFound ) 
        {
        retval = EShareNoneSelected;
        }
    else if ( !shareNoneFound && shareAllFound ) 
        {
        retval = EShareAllSelected;
        }    
    else if ( shareNoneFound && shareAllFound ) 
        {
        retval = EShareNoneAndShareAllSelected;
        }
    else if ( !shareNoneFound && !shareAllFound ) 
        {
        retval = ENoShareNoneOrShareAllSelected;
        }
    
    return retval;
    }


// End of file
