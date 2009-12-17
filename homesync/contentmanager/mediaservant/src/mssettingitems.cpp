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
* Description:  Setting item class implementations
*
*/


#include <aknsettingpage.h>
#include <aknmfnesettingpage.h>
#include <aknsettingitemlist.h>
#include <mediaservant.rsg>
#include "mssettingitems.h"
#include "mediaservant.hrh"

// --------------------------------------------------------------------------
// CMSTextSettingItem::CMSTextSettingItem
// --------------------------------------------------------------------------
//
CMSTextSettingItem::CMSTextSettingItem(  TInt aIdentifier, TDes& aText ) :
    CAknTextSettingItem( aIdentifier, aText )
    {
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::HandleSettingPageEventL
// --------------------------------------------------------------------------
//
void  CMSTextSettingItem::HandleSettingPageEventL(
                                        CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType )
    {
    if ( aEventType == EEventSettingOked )
        {
        iOKPressed = ETrue;
        }
    else
        {
        iOKPressed = EFalse;
        }

    CAknTextSettingItem::HandleSettingPageEventL(
                                            aSettingPage,
                                            aEventType );
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::SettingAccepted
// --------------------------------------------------------------------------
//
TBool CMSTextSettingItem::SettingAccepted()
    {
    return iOKPressed;
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::SetAcceptState
// --------------------------------------------------------------------------
//
void CMSTextSettingItem::SetAcceptState( TBool aState )
    {
    iOKPressed = aState;
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::Text
// --------------------------------------------------------------------------
//
TPtr& CMSTextSettingItem::Text()
    {
    return CAknTextSettingItem::InternalTextPtr();
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::SetText
// --------------------------------------------------------------------------
//
void CMSTextSettingItem::SetText( TDesC& text )
    {
    CAknTextSettingItem::SetExternalText(text);
    }

// --------------------------------------------------------------------------
// CMSEnumeratedTextPopupSettingItem::CMSEnumeratedTextPopupSettingItem
// --------------------------------------------------------------------------
//
CMSEnumeratedTextPopupSettingItem::CMSEnumeratedTextPopupSettingItem(
                                                        TInt aResourceId,
                                                        TInt& aValue ):
    CAknEnumeratedTextPopupSettingItem( aResourceId, aValue )
    {
    }

// --------------------------------------------------------------------------
// CMSEnumeratedTextPopupSettingItem::HandleSettingPageEventL
// --------------------------------------------------------------------------
//
void  CMSEnumeratedTextPopupSettingItem::HandleSettingPageEventL(
                                        CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType )
    {
    if ( aEventType == EEventSettingOked )
        {
        iOKPressed = ETrue;
        }
    else
        {
        iOKPressed = EFalse;
        }

    CAknEnumeratedTextPopupSettingItem::HandleSettingPageEventL(
                                            aSettingPage,
                                            aEventType );
    }

// --------------------------------------------------------------------------
// CMSEnumeratedTextPopupSettingItem::SettingAccepted
// --------------------------------------------------------------------------
//
TBool CMSEnumeratedTextPopupSettingItem::SettingAccepted()
    {
    return iOKPressed;
    }

// --------------------------------------------------------------------------
// CMSEnumeratedTextPopupSettingItem::SetAcceptState
// --------------------------------------------------------------------------
//
void CMSEnumeratedTextPopupSettingItem::SetAcceptState( TBool aState )
    {
    iOKPressed = aState;
    }

// --------------------------------------------------------------------------
// CMSTimeOrDateSettingItem::CMSTimeOrDateSettingItem
// --------------------------------------------------------------------------
//
CMSTimeOrDateSettingItem::CMSTimeOrDateSettingItem( TInt aCMSIdentifier,
                                        TInt aIdentifier,
                                        TAknTimeOrDateSettingItemMode aMode,
                                        TTime& aTime ):
    CAknTimeOrDateSettingItem( aIdentifier, aMode, aTime ),iCMSInternalTime( aTime )
    {
    iCMSIdentifier = aCMSIdentifier;
    }

// --------------------------------------------------------------------------
// CMSTimeOrDateSettingItem::HandleSettingPageEventL
// --------------------------------------------------------------------------
//
void  CMSTimeOrDateSettingItem::HandleSettingPageEventL(
                                        CAknSettingPage* aSettingPage,
                                        TAknSettingPageEvent aEventType )
    {
    if ( aEventType == EEventSettingOked )
        {
        iOKPressed = ETrue;
        // When press OK ,the external time value should transferred to the internal time value immediately
        LoadL();
        }
    else
        {
        iOKPressed = EFalse;
        // When press cancel,externalizes the current setting
        StoreL();
        }

    CAknTimeOrDateSettingItem::HandleSettingPageEventL(
                                            aSettingPage,
                                            aEventType );
    }

// --------------------------------------------------------------------------
// CMSTimeOrDateSettingItem::SettingAccepted
// --------------------------------------------------------------------------
//
TBool CMSTimeOrDateSettingItem::SettingAccepted()
    {
    return iOKPressed;
    }

// --------------------------------------------------------------------------
// CMSTimeOrDateSettingItem::SetAcceptState
// --------------------------------------------------------------------------
//
void CMSTimeOrDateSettingItem::SetAcceptState( TBool aState )
    {
    iOKPressed = aState;
    }

// --------------------------------------------------------------------------
// CMSTimeOrDateSettingItem::EditItemL
// --------------------------------------------------------------------------
//
void CMSTimeOrDateSettingItem::EditItemL( TBool  /*aCalledFromMenu*/ )
    {
    // Access to the setting Name
    TPtrC text = SettingName();
    CAknSettingPage* dlg = KErrNone;

    // Launches the setting page resource file
    dlg = new ( ELeave )CAknDateSettingPage( iCMSIdentifier, iCMSInternalTime );

    // Set the edit page to observer
    SetSettingPage( dlg );
    SettingPage()->SetSettingPageObserver( this );

    if( ProtectionState() == CAknSettingItem::ESettingItemViewOnly )
        SettingPage()->SetEditState( EFalse );

    // Change to EUpdateWhenAccepted flag to avoid the checking every time there is an input to the field
    SettingPage()->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );
    SetSettingPage( 0 );
   }
// End of File
