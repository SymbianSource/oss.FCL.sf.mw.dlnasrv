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
* Description:      Multiselection UI implementation
*
*/






// INCLUDE FILES
#include <AknUtils.h>
#include <aknview.h>
#include <aknViewAppUi.h>
#include <eikspmod.h>
#include <eikclb.h>
#include <featmgr.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknmessagequerydialog.h> //caknmessagequerydialog
#include <aknlistquerydialog.h>  //caknlistquerydialog
#include <es_sock.h> //rsocketserv

#include "upnpfilesharingengine.h"

#include "upnpappmultiselectiondialog.h"
#include "upnpappmultiselectionpopup.h"
#include <cupnpmultiselectionui.rsg>
#include "cupnpmultiselectionui.h"

_LIT( KComponentLogfile, "multiselectionui.txt");
#include "upnplog.h"


// Filename of rsc file
_LIT( KUPnPMultiSelectionRscFile, "\\resource\\CUPnPMultiselectionUi.rsc" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::CUPnPMultiselectionUi
// --------------------------------------------------------------------------
//
CUPnPMultiselectionUi::CUPnPMultiselectionUi()
    {
    __LOG("CUPnPMultiselectionUi::CUPnPMultiselectionUi");
    iFeatureManagerInitialized = EFalse;
    }

// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPMultiselectionUi::ConstructL( 
    THomeConnectMediaType aMediaType,
    CUPnPFileSharingEngine* aFileSharingEngine,
    TInt& /*aVisualSharingState*/,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    TUiStyle aStyle,
    const HBufC* aHeading )
    {  
    __LOG("CUPnPMultiselectionUi::ConstructL");
    
    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();

    
    // Load common ui resource file    
    TFileName rscFileName( KUPnPMultiSelectionRscFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    
    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    if ( aStyle == EDialog )
        {

        iDlg = CUPnPAppMultiselectionDialog::NewL( aMediaType, 
                                                   aFileSharingEngine,
                                                   aSelectionIndexArray );
        }
    else
        {
        iPopup = CUPnPAppMultiselectionPopup::NewL( aMediaType, 
                                                    aFileSharingEngine,
                                                    aSelectionIndexArray,
                                                    aHeading );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPMultiselectionUi* CUPnPMultiselectionUi::NewL(    
    THomeConnectMediaType aMediaType,
    CUPnPFileSharingEngine* aFileSharingEngine,
    TInt& aVisualSharingState,
    CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
    TUiStyle aStyle,
    const HBufC* aHeading )
    {
    __LOG("CUPnPMultiselectionUi::NewL begin");
    
    CUPnPMultiselectionUi* self = new ( ELeave) CUPnPMultiselectionUi();
    CleanupStack::PushL( self );

    self->ConstructL( aMediaType,
                      aFileSharingEngine, 
                      aVisualSharingState,
                      aSelectionIndexArray,
                      aStyle,
                      aHeading );
        
    CleanupStack::Pop();
    __LOG("CUPnPMultiselectionUi::NewL end");
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUPnPMultiselectionUi::~CUPnPMultiselectionUi()
    {
    __LOG("CUPnPMultiselectionUi::~CUPnPMultiselectionUi begin");
    
    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }
    
    // Un-Load resource file
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }
 
    __LOG("CUPnPMultiselectionUi::~CUPnPMultiselectionUi end");
    }
   
// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::ExecuteDlgLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMultiselectionUi::ExecuteDlgLD()
    {
    __LOG("CUPnPMultiselectionUi::ExecuteDlgLD");

    TInt ret(KErrNone);
    ret = iDlg->ExecuteLD( R_UPNP_MULTISELECTION_DIALOG );
    iDlg = NULL;             
    return ret;
    }
    
// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::ExecutePopupLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMultiselectionUi::ExecutePopupLD()
    {
    __LOG("CUPnPMultiselectionUi::ExecutePopupLD");

    TInt ret(KErrNone);
    ret = iPopup->ExecuteLD( R_UPNP_MULTISELECTION_POPUP );
    iPopup = NULL;
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPMultiselectionUi::SetMarkedItems
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPMultiselectionUi::SetMarkedItemsL( CArrayFix<TInt>& 
		                                                       aMarkedItems )
    {
    if( iPopup )
        {
        iPopup->SetMarkedItemsL(aMarkedItems);
        }
    }

// End of File  
