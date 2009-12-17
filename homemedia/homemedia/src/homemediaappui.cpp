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
* Description:      Main Ui class
*
*/






// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>
#include <homemedia.rsg>
#include <akntoolbar.h>

// debug log support
_LIT( KComponentLogfile, "homemedia.txt");
#include "upnplog.h"

#include "homemedia.hrh"
#include "homemediaapplication.h"
#include "homemediaappui.h"
#include "homemediaappview.h"

// --------------------------------------------------------------------------
// CHomeMediaAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CHomeMediaAppUi::ConstructL()
    {
    __LOG("CHomeMediaAppUi::ConstructL begin");
    BaseConstructL( EAknEnableSkin|EAknEnableMSK );

    // Create view object
    iAppView = CHomeMediaAppView::NewL();
    
    // Ownership transferred
    AddViewL(iAppView);
    
    SetDefaultViewL(*iAppView);

    CAknToolbar* toolbar = CurrentFixedToolbar();
    if ( toolbar != NULL )
        {
        toolbar->HideItem( ECmdDummy, ETrue, EFalse );
        toolbar->SetToolbarVisibility( EFalse );
        }

    __LOG("CHomeMediaAppUi::ConstructL end");
    }
// --------------------------------------------------------------------------
// CHomeMediaAppUi::CHomeMediaAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CHomeMediaAppUi::CHomeMediaAppUi()
    {
    // No implementation required
    }
    
// --------------------------------------------------------------------------
// CHomeMediaAppUi::~CHomeMediaAppUi()
// Destructor.
// --------------------------------------------------------------------------
//
CHomeMediaAppUi::~CHomeMediaAppUi()
    {
    }

// --------------------------------------------------------------------------
// CHomeMediaAppUi::HandleCommandL()
// Takes care of command handling.
// --------------------------------------------------------------------------
//
void CHomeMediaAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit: // fall through
            Exit();
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// --------------------------------------------------------------------------
//
void CHomeMediaAppUi::HandleResourceChangeL( TInt aType )
    {
    __LOG1("CHomeMediaAppUi::HandleResourceChangeL aType = %d",aType);
    CAknAppUi::HandleResourceChangeL( aType );
    // *****************************
    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        ((CHomeMediaAppView*) View( iAppView->Id() ) )->
                                                  HandleClientRectChange(  );
        }
    } 

// ---------------------------------------------------------------------------
// HandleForegroundEventL
// handle switches to background and foreground
// ---------------------------------------------------------------------------
void CHomeMediaAppUi::HandleForegroundEventL( TBool aForeground )
    {
    if( aForeground )
        {
        iAppView->RefreshVisibilitySettingItemL();
        }
  
    }
// End of File
