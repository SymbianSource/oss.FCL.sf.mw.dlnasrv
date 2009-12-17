/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Advanced find dialog component source code
*
*/



// INCLUDE FILES
// System
#include <StringLoader.h>
#include <upnpstring.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsession.h"
#include "upnpavdevice.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"

// common ui internal
#include "upnpcommonui.h"
#include <upnpcommonui.rsg>
#include "upnpadvfinddialog.h"
#include "upnpadvfindlist.h"

// debug stuff
_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KLength = 100;

// ============================ MEMBER FUNCTIONS ============================
// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindDialog::ConstructL(TInt aMenuResource)
    {
    __LOG( "CUPnPAdvancedFindDialog::ConstructL" );
    CAknDialog::ConstructL(aMenuResource);
    
    iOriginalTitleText = HBufC16::NewL( KMaxFileName );
    }


// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::CUPnPAdvancedFindDialog
// default constructor
// --------------------------------------------------------------------------
CUPnPAdvancedFindDialog::CUPnPAdvancedFindDialog(
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession,
                                CUPnPCommonUI& aCommonUI ):
                                iAVControl(aAVControl),
                                iBrowseSession(aBrowsingSession),
                                iCommonUI(aCommonUI)

    {
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAdvancedFindDialog* CUPnPAdvancedFindDialog::NewL(
                                TInt aMenuResource,
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession,
                                CUPnPCommonUI& aCommonUI )
    {    
    __LOG( "CUPnPAdvancedFindDialog::NewL" );

    CUPnPAdvancedFindDialog* self = new ( ELeave ) CUPnPAdvancedFindDialog(
        aAVControl, aBrowsingSession, aCommonUI );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuResource );
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CUPnPAdvancedFindDialog::~CUPnPAdvancedFindDialog()
    {
    __LOG( "CUPnPAdvancedFindDialog destructor" );
    // Set original title pane text
    if( iOriginalTitleText && iTitlePane )
        {
        TRAP_IGNORE( iTitlePane->SetTextL( *iOriginalTitleText ) );
        delete iOriginalTitleText;    
        }
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::OkToExitL( TInt aButtonId )
// called by framework when trying to close the dialog
// --------------------------------------------------------------------------
//
TBool CUPnPAdvancedFindDialog::OkToExitL( TInt aButtonId )
    {
    __LOG( "CUPnPAdvancedFindDialog::OkToExitL" );
    TInt error = KErrNone;
    TBool retval = EFalse;
    
    if ( aButtonId == EAknSoftkeyOk )
        {
        iSettingsList->SetMopParent( this );
        error = iSettingsList->MakeQueryL();
        
        iTitlePane = (CAknTitlePane*)iStatusPane->ControlL(
                                    TUid::Uid( EEikStatusPaneUidTitle ) );
        SetTitleL();
        TryExitL( error );
        }
    else if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        }
    else if ( aButtonId == EAknSoftkeyBack || 
              aButtonId == KErrSessionClosed ||
              aButtonId == KErrDisconnected ||
              aButtonId == EAknCmdExit ||
              aButtonId == EEikCmdExit )
        {
        retval = ETrue; //back to previous dialog
        }
    else if( aButtonId == EAknSoftkeyOpen )
        {
        TKeyEvent tmpEvent;
        tmpEvent.iCode = EKeyOK;
        tmpEvent.iModifiers = 0;
        tmpEvent.iRepeats = 0;
        tmpEvent.iScanCode = 0;
        TEventCode eventType = EEventKey;
        OfferKeyEventL( tmpEvent, eventType );
        }
    return retval;
    }

// ---------------------------------------------------------
// CUPnPAdvancedFindDialog::PreLayoutDynInitL()
// called by framework before dialog is shown 
// ---------------------------------------------------------
//
void CUPnPAdvancedFindDialog::PreLayoutDynInitL()
    {
    __LOG( "CUPnPAdvancedFindDialog::PreLayoutDynInitL" );
    iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL( 
                                    TUid::Uid( EEikStatusPaneUidTitle ) );
    
    // Take backup of title text
    iOriginalTitleText->Des().Copy( *iTitlePane->Text() );
    // set dialog title
    SetTitleL();
    }
    

// ---------------------------------------------------------
// CUPnPAdvancedFindDialog::ProcessCommandL()
// ---------------------------------------------------------
//    
void CUPnPAdvancedFindDialog::ProcessCommandL ( TInt aCommand )
    {
    __LOG( "CUPnPAdvancedFindDialog::ProcessCommandL" );
    HideMenu();    
    CAknDialog::ProcessCommandL( aCommand );
    }
  
// ---------------------------------------------------------
// CUPnPAdvancedFindDialog::OfferKeyEventL()
// called by framework when key is pressed 
// ---------------------------------------------------------
//
TKeyResponse CUPnPAdvancedFindDialog::OfferKeyEventL(
                                                const TKeyEvent &aKeyEvent,
                                                TEventCode aType )
    {
    __LOG( "CUPnPAdvancedFindDialog::OfferKeyEventL" );
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }
    switch ( aKeyEvent.iCode )
        {
        case EKeyDownArrow: // flow through
        case EKeyUpArrow:
            {
            iSettingsList->OfferKeyEventL( aKeyEvent, aType );
            break;
            }
        case EKeyOK:
            {
            TInt selected_item = iSettingsList->ListBox()->CurrentItemIndex();
            iSettingsList->EditItemL( selected_item, ETrue );
            if( !iClose )
                {
                iSettingsList->HandleChangeInItemArrayOrVisibilityL();    
                }
            
            break;
            }       
        default:
            {
            CAknDialog::OfferKeyEventL( aKeyEvent, aType );
            break;    
            }
        }

    return EKeyWasConsumed;
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::CreateCustomControlL
// --------------------------------------------------------------------------
//
SEikControlInfo CUPnPAdvancedFindDialog::CreateCustomControlL(
                                                        TInt aControlType )
    {
    __LOG( "CUPnPAdvancedFindDialog::CreateCustomControlL" );
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    switch ( aControlType )
        {
        case KAknCtLastControlId:
            {            
            iSettingsList = CUPnPAdvancedFindList::NewL( iAVControl, 
                                                         iBrowseSession,
                                                         *this,
                                                         iCommonUI );
            controlInfo.iControl = iSettingsList;          
            break;
            }
        default:
            {
            break;
            }
        }    
    return controlInfo;
    }        

// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::DismissItselfL
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindDialog::DismissItselfL( TInt aError )
    {
    iClose = ETrue;
    if( iSettingsList )
        {
        //if setting page is not open, close it, otherwise, do nothing
        if( !iSettingsList->IsSettingPageOpen() )
            {
            TryExitL( aError );
            }
        }
    else
        {
        TryExitL( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindDialog::SetTitleL
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindDialog::SetTitleL()
    {
    const CUpnpAVDevice* device = &( iBrowseSession.Device() );
    
    // Get device friendly name and replace illegal characters.
    HBufC8* tmpfriendlyname = 
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
        ( ( CUpnpAVDevice* )device )->FriendlyName() );    
    
    CleanupStack::PushL( tmpfriendlyname );
    TPtrC8 friendlyname = *tmpfriendlyname;
        
    HBufC* titleString = UpnpString::ToUnicodeL( 
        friendlyname.Left( KLength ) );
    CleanupStack::PushL( titleString );
    
    iTitlePane->SetTextL( *titleString );
    CleanupStack::PopAndDestroy( titleString );
    CleanupStack::PopAndDestroy( tmpfriendlyname );
    }
// End of file
