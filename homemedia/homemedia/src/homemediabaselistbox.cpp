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
* Description:      Base listbox class
*
*/






#include <aknViewAppUi.h>
#include <badesca.h>
#include <eiklbx.h> 
#include <homemedia.mbg>
#include <gulicon.h>
#include <eikenv.h>
#include <eikapp.h>
#include <AknsUtils.h> 
#include <AknIconUtils.h> 
#include "homemediaappview.h"
#include "homemediabaselistbox.h"

//CONSTANTS
static const TInt KIconGranularity( 3 );
_LIT( KAknHomeMediaMbmFileName, "\\resource\\apps\\HomeMedia.mif" );

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::CHomeMediaBaseListbox()
// --------------------------------------------------------------------------
//
CHomeMediaBaseListbox::CHomeMediaBaseListbox()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::~CHomeMediaBaseListbox()
// --------------------------------------------------------------------------
//
CHomeMediaBaseListbox::~CHomeMediaBaseListbox()
    {
    delete iListBox;
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::ListBox()
// --------------------------------------------------------------------------
//
CEikListBox& CHomeMediaBaseListbox::ListBox()
    {
    return *iListBox;
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::AddColumnL()
// --------------------------------------------------------------------------
//
void CHomeMediaBaseListbox::AddColumnL( TRefByValue<const TDesC> aFmt,... )
    {
    // pointer to list of arguments 
    VA_LIST args;
    // tells the code where to start the list
    VA_START(args,aFmt);
    HBufC* data = HBufC::NewLC(KMaxFileName);
    // create TOverFlowHandler object in case if there is 
    // huge descriptor passed here
    TOverFlowHandler overFlow;
    // now format descriptor
    data->Des().AppendFormatList( aFmt , args , &overFlow );
    // add item to list
    iListBoxItems->AppendL(*data);
    CleanupStack::PopAndDestroy(data);
    // sets pointer to zero
    VA_END(args);
    // draw listbox
    iListBox->HandleItemAdditionL();
    iListBox->DrawDeferred();
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::SizeChanged()
// Called by framework when the view size is changed.
// --------------------------------------------------------------------------
//
void CHomeMediaBaseListbox::SizeChanged()
    {  
    const TPoint listPosition(0,0);
    iListBox->SetExtent(listPosition, iListBox->MinimumSize() );
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::OfferKeyEventL()
// --------------------------------------------------------------------------
//
TKeyResponse CHomeMediaBaseListbox::OfferKeyEventL(
    const TKeyEvent &aKeyEvent,
    TEventCode aType)
    {
    TKeyResponse ret(EKeyWasNotConsumed);
    if( aType == EEventKey )
        {
        if( aKeyEvent.iCode == EKeyUpArrow 
            || aKeyEvent.iCode == EKeyDownArrow )
            {
            ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
            }
        } 
    return ret;
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::CountComponentControls()
// --------------------------------------------------------------------------
//
TInt CHomeMediaBaseListbox::CountComponentControls() const
    {
    return 1;
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::ComponentControl()
// --------------------------------------------------------------------------
//
CCoeControl* CHomeMediaBaseListbox::ComponentControl(TInt aIndex) const
    {
    CCoeControl* ret = NULL;
    switch( aIndex )
        {
        case 0:
            ret = iListBox;
            break;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::BaseConstructL()
// --------------------------------------------------------------------------
//
void CHomeMediaBaseListbox::BaseConstructL( const TRect& aRect 
                                                ,CEikTextListBox* aListBox )
    {    
    CreateWindowL();

    // take the ownership of aListBox
    iListBox = aListBox;
    
    /** This initializes the C-classes of a newly-created @c CEikTextListBox 
    * from supplied arguments. This function fully constructs the associated 
    * list box model and item drawer for this list box, and then invokes 
    * CEikListBox::ConstructL().
    */
    iListBox->ConstructL(this);
    
    /**
    * Creates an own window for the list box or draws the list box to an old 
    * window defined by the container.
    */
    iListBox->SetContainerWindowL(*this);
    
    // add scrollbars to listbox 
    iListBox->CreateScrollBarFrameL(ETrue); 
    
    /**
    * Sets the visibility state for both the horizontal and the vertical
    * scrollbars. This is used when tiling.
    */
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
    
    // get the pointer to listbox itemTextArray
    iListBoxItems 
            = static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );
    
    //Set icons
    CAknIconArray* icons = new (ELeave) CAknIconArray( KIconGranularity );
    CleanupStack::PushL( icons );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );

    TFileName mbmFileName( KAknHomeMediaMbmFileName );
    TFileName dllName 
             = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );

    // if new static menu item added constant KNumberOfItemsBeforePlugins
    // has to be changed
    AppendIconToArrayL( icons, skin,
                        mbmFileName,
                        KAknsIIDDefault,
                        EMbmHomemediaQgn_prop_upnp_browse_home,
                        EMbmHomemediaQgn_prop_upnp_browse_home_mask );

    AppendIconToArrayL( icons, skin,
                        mbmFileName,
                        KAknsIIDDefault,
                        EMbmHomemediaQgn_prop_upnp_share_sub,
                        EMbmHomemediaQgn_prop_upnp_share_sub_mask );
    
    static_cast<CEikFormattedCellListBox*>(iListBox)->ItemDrawer()->
                                    FormattedCellData()->SetIconArray(icons);
    
    CleanupStack::Pop(icons);
    
    // Set the windows size
    SetRect( aRect );
    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// --------------------------------------------------------------------------
// CHomeMediaBaseListbox::AppendIconToArrayL()
// --------------------------------------------------------------------------
//
void CHomeMediaBaseListbox::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId)
    {
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

// End of file
