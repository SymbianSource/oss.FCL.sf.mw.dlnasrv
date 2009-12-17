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
#include <aknlists.h>

#include "cmmediaserverfull.h"
#include "msmultiselectionpopup.h"
#include "msconstants.h"
#include "msdebug.h"

// CONSTANTS
const TInt KSoftkeyOk = 63585;

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup* CMSMultiselectionPopup::NewL(
    TBool aUploadCapabilitySupport,
    RPointerArray<CCmMediaServerFull>* aMediaServerArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::NewL begin"));

    CMSMultiselectionPopup* self = CMSMultiselectionPopup::NewLC( 
													aUploadCapabilitySupport,
                                                    aMediaServerArray,
                                                    aSelectionIndexArray );

    CleanupStack::Pop( self );

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::NewL end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup* CMSMultiselectionPopup::NewLC(
    TBool aUploadCapabilitySupport,
    RPointerArray<CCmMediaServerFull>* aMediaServerArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::NewLC begin"));

    CMSMultiselectionPopup* self =
                 new(ELeave) CMSMultiselectionPopup( aUploadCapabilitySupport,
                                                     aMediaServerArray,
                                                     aSelectionIndexArray );

    CleanupStack::PushL(self);
    self->ConstructL();

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::NewLC end"));
    return self;
    }
	
// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::ConstructL begin"));

    for (TInt i=0;i<iServers->Count();i++)
        {
        if ( iUploadCapabilitySupport && (*iServers)[i]->StoreUsage() )
            {
            iPreviousSelection->AppendL(i);
            }
        else if (!iUploadCapabilitySupport && (*iServers)[i]->FillUsage() )
            {
            iPreviousSelection->AppendL(i);
            }
        else
            {
            LOG(_L("[MediaServant]\t CMSMultiselectionPopup::ConstructL \
            server not used"));
            }            
        }

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::ConstructL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::CMSMultiselectionPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup::CMSMultiselectionPopup(
    TBool aUploadCapabilitySupport,   
    RPointerArray<CCmMediaServerFull>* aMediaServerArray,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray )
    : CAknListQueryDialog( aSelectionIndexArray ),    
    iUploadCapabilitySupport ( aUploadCapabilitySupport ),
    iServers( aMediaServerArray ),
    iPreviousSelection( aSelectionIndexArray )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            CMSMultiselectionPopup"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::~CMSMultiselectionPopup
// C++ default destructor.
// ---------------------------------------------------------------------------
//
CMSMultiselectionPopup::~CMSMultiselectionPopup()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            ~CMSMultiselectionPopup"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::UpdateAndDrawPopupL
//
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::UpdateAndDrawPopupL(
                                CCmMediaServerFull* aMediaServer )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            UpdateAndDrawPopupL begin"));

    // Listbox items
    CDesCArray* device_array = static_cast<CDesCArray*>
                     (ListControl()->listbox()->Model()->ItemTextArray());

    HBufC* item = CreateListBoxItemLC( aMediaServer );
    device_array->AppendL( *item );
    CleanupStack::PopAndDestroy( item );

    iListBox->HandleItemAdditionL(); // Update listbox

    Layout();
    SizeChanged();
    DrawNow();

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            UpdateAndDrawPopupL ends"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::HandlePointerEventL
//
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::HandlePointerEventL 
                               ( const TPointerEvent& aPointerEvent )
	{
	CCoeControl::HandlePointerEventL( aPointerEvent );

    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
    	{
		TKeyEvent keyEvent;
		keyEvent.iCode = EKeyOK;
		this->OfferKeyEventL( keyEvent, EEventKey );
	    }
	}

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::AppendIconToArrayL
//
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::AppendIconToArrayL( CAknIconArray* aArray,
                                                 MAknsSkinInstance* aSkin,
                                                 const TDesC& aMbmFile,
                                                 const TAknsItemID& aID,
                                                 TInt aBitmapId,
                                                 TInt aMaskId ) const
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
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

    // ownership transferred
    aArray->AppendL(icon);

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop(icon);
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            AppendIconToArrayL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// ---------------------------------------------------------------------------
//
TBool CMSMultiselectionPopup::OkToExitL( TInt /*aButtonId*/ )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::OkToExitL"));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::AppendMediaServersL
// Appends items to the listbox
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::AppendMediaServersL()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            AppendMediaServersL begin"));

    TInt serverCount = iServers->Count();

    CDesCArray* listBoxArray = static_cast<CDesCArray*>
                   (ListControl()->listbox()->Model()->ItemTextArray());

    for ( TInt i = 0; i < serverCount; i++ )
        {
        CCmMediaServerFull* server = (*iServers)[i];

        HBufC* item = CreateListBoxItemLC( server );

        listBoxArray->AppendL( *item );

        CleanupStack::PopAndDestroy( item );
        }

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            AppendMediaServersL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::PreLayoutDynInitL()
// called by framework before dialog is shown
// ---------------------------------------------------------------------------
//
void CMSMultiselectionPopup::PreLayoutDynInitL()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            PreLayoutDynInitL begin"));

    CAknListQueryDialog::PreLayoutDynInitL();

    CAknIconArray* icons = new (ELeave) CAknIconArray( EIconLast );
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

    AppendIconToArrayL( icons, skin,
                        iconsPath,
                        KAknsIIDQgnIndiNaviArrowRight,
                        EMbmAvkonQgn_indi_navi_arrow_right,
                        EMbmAvkonQgn_indi_navi_arrow_right_mask );

    CAknListQueryDialog::SetIconArrayL( icons );

    CleanupStack::Pop( icons );

    iListBox = ListBox();
    SetOwnershipType( ELbmOwnsItemArray );

    AppendMediaServersL();

    iListBox->ActivateL();

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            PreLayoutDynInitL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::PostLayoutDynInitL()
// Called by framework before dialog is shown.
// Needed because filter is not active until PreLayoutDynInitL().
// ---------------------------------------------------------------------------
//
void  CMSMultiselectionPopup::PostLayoutDynInitL()
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            PostLayoutDynInitL begin"));

    CAknListQueryDialog::PostLayoutDynInitL();
    ListBox()->ActivateL();

    if ( iPreviousSelection && iPreviousSelection->Count() )
        {
        iListBox->SetSelectionIndexesL( iPreviousSelection );
        }

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            PostLayoutDynInitL end"));
    }

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::OfferKeyEventL()
// called by framework when key is pressed
// ---------------------------------------------------------------------------
//
TKeyResponse CMSMultiselectionPopup::OfferKeyEventL (
                                    const TKeyEvent &aKeyEvent,
                                    TEventCode aType )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::OfferKeyEventL begin"));

    TKeyResponse status( EKeyWasNotConsumed );

    // check if user has selected something
    if (aKeyEvent.iCode == EKeyOK)
        {
        status = ListBox()->OfferKeyEventL( aKeyEvent, aType );

        TInt itemNbr = iListBox->CurrentItemIndex();
        if( iListBox->View()->ItemIsSelected( itemNbr ) )
            {
            iPreviousSelection->AppendL( itemNbr );
            }
        else
            {
            TKeyArrayFix sortKey( 0, ECmpTInt );
            TInt itemIndex( 0 );

            if( iPreviousSelection->Find( 
                itemNbr, sortKey, itemIndex ) == 0 )
                {
                // remove selection
                iPreviousSelection->Delete( itemIndex );
                }
            }
        }
    else
        {
        // prevents to mark item if dialog is accepted with OK key
        if ( aKeyEvent.iCode == KSoftkeyOk )
            {
            iListBox->View()->SetDisableRedraw(ETrue);    
            }
           
        status = CAknListQueryDialog::OfferKeyEventL(aKeyEvent,aType);
        }

    LOG(_L("[MediaServant]\t CMSMultiselectionPopup::OfferKeyEventL end"));
    return status;
}

// ---------------------------------------------------------------------------
// CMSMultiselectionPopup::CreateListBoxItemLC()
// Creates listbox item
// ---------------------------------------------------------------------------
//
HBufC* CMSMultiselectionPopup::CreateListBoxItemLC(
                                const CCmMediaServerFull* aServer )
    {
    LOG(_L("[MediaServant]\t CMSMultiselectionPopup:: \
            CreateListBoxItemLC"));

    TBuf<KMaxFileName> serverName;
    serverName.Copy( aServer->MediaServerName().Left( KMaxFileName ) );

    HBufC* item = HBufC::NewLC( serverName.Size() +
            sizeof( KSingleGraphicStyleFormatStringNoTrailIcons() ) );
            item->Des().Format( KSingleGraphicStyleFormatStringNoTrailIcons,
            EUnCheckedBox,
            &serverName );

    return item;
    }

// End of file
