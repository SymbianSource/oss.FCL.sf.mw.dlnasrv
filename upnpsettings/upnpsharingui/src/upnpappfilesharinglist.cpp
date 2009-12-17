/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      File sharing list implementation for File sharing view
*
*/






// INCLUDE FILES
// system
#include <aknlists.h>
#include <aknPopup.h>
#include <StringLoader.h>
#include <upnpsharingui.rsg>
#include <aknnotewrappers.h>
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>

// internal upnpfw interfaces
#include "upnpsettingsengine.h"
#include "upnpfilesharingengine.h"
#include "cupnpappwizard.h"
#include "upnpcontentserverclient.h" //TUpnpProgressInfo

// this component
#include "upnpsharingui.hrh"
#include "upnpappfilesharinglist.h"

_LIT( KComponentLogfile, "upnpsharingui.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KCDrive,  "C" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::CUPnPAppFileSharingList
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAppFileSharingList::CUPnPAppFileSharingList()
    {
    __LOG("CUPnPAppFileSharingList::CUPnPAppFileSharingList");
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::ConstructL(
                                CUPnPFileSharingEngine* aFileSharingEngine)
    {
    __LOG("CUPnPAppFileSharingList::ConstructL");

    iFileSharingEngine = aFileSharingEngine;
    User::LeaveIfNull( iFileSharingEngine );
    iVisibility = 0;
    if ( iFileSharingEngine->SharingStateL() )
        {
        iVisibility = 1;
        }
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAppFileSharingList* CUPnPAppFileSharingList::NewL(
    CUPnPFileSharingEngine* aFileSharingEngine )
    {
    __LOG("CUPnPAppFileSharingList::NewL");

    CUPnPAppFileSharingList* self = new (ELeave) CUPnPAppFileSharingList( );
    CleanupStack::PushL(self);
    self->ConstructL( aFileSharingEngine );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::~CUPnPAppFileSharingList
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAppFileSharingList::~CUPnPAppFileSharingList()
    {
    __LOG("CUPnPAppFileSharingList::~CUPnPAppFileSharingList");
    // This will kill the possible wait note
    iStartingSharing = EPhaseCompleted;
    delete iWizard;
    iWizard = NULL;
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::CreateSettingItemL
// Creates list items
// --------------------------------------------------------------------------
//
CAknSettingItem* CUPnPAppFileSharingList::CreateSettingItemL(TInt aIdentifier)
    {
    __LOG("CUPnPAppFileSharingList::CreateSettingItemL");

    CAknSettingItem* settingItem = NULL;
    switch (aIdentifier)
        {
        case ESharingUiSettingItemVisibility:
            {
            settingItem =
                new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,
                                                        iVisibility);
            break;
            }

        case ESharingUiSettingItemVisual:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,
                                                           iVisualAlbums);
            break;
            }

        case ESharingUiSettingItemMusic:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,
                                                           iPlayLists);
            break;
            }

        default:
            {
            __LOG("CUPnPAppFileSharingList::CreateSettingItemL \
Unknown setting item!");
            }
        }

    return settingItem;
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::EditItemL
// Called when setting list item is modified
// --------------------------------------------------------------------------
//
void  CUPnPAppFileSharingList::EditItemL (TInt aIndex, 
                                          TBool /*aCalledFromMenu*/)
    {
    __LOG("CUPnPAppFileSharingList::EditItemL");

    LoadDataFromEngineL();

    switch ( aIndex )
        {
        case ESharingUiSettingItemVisibility:
            {
            if ( !iVisibility )
                {
                if ( iIapId != KErrNotFound )
                    {
                    ShowWaitNoteL();
                    }
                else
                    {
                    iWizardRun = ETrue;

                    MUPnPFileSharingEngineObserver* observer = 
                                    iFileSharingEngine->Observer();
                    
                    delete iWizard;
                    iWizard = NULL;
                    iWizard = CUPnPAppWizard::NewL( 
                        KNullDesC, iFileSharingEngine );
                    iWizard->StartL();


                    iFileSharingEngine->SetObserver(observer);
                    iFileSharingEngine->RequestSharingProgressL();

                    LoadDataFromEngineL();
                    }
                }
            else
                {
                ShowWaitNoteL();
                }
            }
            break;

        default:
            {
            __LOG("CUPnPAppFileSharingList::EditItemL No such item!");
            }
        }

    //load settings to screen
    CAknSettingItemList::LoadSettingsL();
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::LoadDataFromEngineL
// Loads data from Central Repository to member variables
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void  CUPnPAppFileSharingList::LoadDataFromEngineL()
    {
    __LOG("CUPnPAppFileSharingList::LoadDataFromEngineL");

    CUPnPSettingsEngine* settingsEngine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( settingsEngine );

    // Read file sharing state
    iVisibility = 0;
    if ( iFileSharingEngine->SharingStateL() )
        {
        iVisibility = 1;
        }

    // Read iap id
    settingsEngine->GetAccessPoint( iIapId );

    //load settings to screen
    CAknSettingItemList::LoadSettingsL ();
    CleanupStack::PopAndDestroy( settingsEngine );
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::HandleSharingStatus
// Function informs when file sharing was enabled or disabled
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::HandleSharingStatus( 
        CUPnPFileSharingEngine& /*aEngine*/, 
        TInt aError, 
        TBool aPhase )
    {
    __LOG("CUPnPAppFileSharingList::HandleSharingStatus begin");

    if ( aPhase )
        {
        __LOG("start sharing");

        if ( aError )
            {
            // change state back to off
            iVisibility = EFalse;

            // If there domtree wasn't able to be written due to disk full
            // situation, send leave to kernel to inform user
            if( aError == KErrDiskFull ||
                aError == KErrNoMemory ||
                aError == KErrInUse )
                {
                // stop the wait note
                iStartingSharing = EPhaseCompleted;
                //show some critical errors using
                //default symbian error notes
                CActiveScheduler::Current()->Error( aError );
                }
            else
                {
                // show error note
                TRAP_IGNORE(
                    HBufC* errorNote = StringLoader::LoadLC(
                        R_SETTINGS_START_SHARING_ERROR_TEXT);
                    CAknErrorNote* note = new (ELeave) CAknErrorNote(ETrue);
                    note->ExecuteLD(*errorNote);
                    CleanupStack::PopAndDestroy() ); //errorNote
                }

            }
        else
            {
            // change secondary text
            iVisibility = ETrue;
            }
        }
    else
        {
        __LOG("stop sharing");

        // If the media server lost, aError will be KErrGeneral or
        // KErrServerTerminated.
        // If there were other errors, stop sharing fails and visibility 
        // will remain true.
        if ( !aError || KErrGeneral == aError || 
                KErrServerTerminated == aError )
            {
            iVisibility = EFalse;
            }

        // Note user not enough memory to change the phone state.
        if ( KErrDiskFull == aError ||
             KErrNoMemory == aError ||
             KErrInUse == aError )
            {
            __LOG1( "Not enough memory to change the \
                    phone state error:%d ", aError  );
           
            //There is nothing to do if leaves
            TRAP_IGNORE(
                HBufC* errorNote = StringLoader::LoadLC(
                    R_SETTINGS_START_SHARING_PHONE_ERROR_TEXT, KCDrive );
                CAknErrorNote* note = new( ELeave ) CAknErrorNote( ETrue );
                note->ExecuteLD( *errorNote );
                CleanupStack::PopAndDestroy( errorNote ) );
            }  
        
        TRAP_IGNORE(
            CAknSettingItemList::EditItemL(
                ESharingUiSettingItemVisibility, EFalse ) );
        }


    iStartingSharing = EPhaseCompleted;
    // Update UI
    TRAP_IGNORE( CAknSettingItemList::LoadSettingsL() );
    DrawDeferred();

    __LOG("CUPnPAppFileSharingList::HandleSharingStatus end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::HandleSharingConnectionLost
// Callback to inform if connection has been lost
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::HandleSharingConnectionLost( 
        CUPnPFileSharingEngine& /*aEngine*/ )
    {
    __LOG("CUPnPAppFileSharingList::HandleSharingConnectionLost begin");

    iVisibility = EFalse;
    TRAPD( err,
        CAknSettingItemList::EditItemL (
            ESharingUiSettingItemVisibility, EFalse);
        LoadSettingsL();
        );
    DrawDeferred();
    if ( err != KErrNone )
        {
        //show some critical errors using
        //default symbian error notes
        CActiveScheduler::Current()->Error( err );
        }

    __LOG("CUPnPAppFileSharingList::HandleSharingConnectionLost end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::HandleResourceChange
// From CAknDialog
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::HandleResourceChange( TInt aType )
    {
    __LOG("CUPnPAppFileSharingList::HandleResourceChange");

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPaneRect );

        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(
            TRect( TPoint( 0, 0 ), mainPaneRect.Size() ),
            AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );

        ListBox()->SetRect( layoutRect.Rect() );
        }

    // Base call
    CAknSettingItemList::HandleResourceChange( aType );
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::SetFilesharingTextsL
// Update the secondary texts in the dialog
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::SetFilesharingTextsL( 
    const RArray<TUpnpProgressInfo>& aProgressInfo )
    {
    __LOG( "[CUPnPAppFileSharingList]\t SetFilesharingTextsL begin" );


    for ( TInt i(0); i < aProgressInfo.Count(); i++ )
        {
        if ( aProgressInfo[ i ].iProgressKind == EImageAndVideo )
            {
            if ( aProgressInfo[ i ].iProgressType == 
                 TUpnpProgressInfo::EVisualStatus )
                {
                SetDefaultSecondaryTextsL( EImageAndVideo, 
                                           aProgressInfo[ i ].iProgress );
                }
            else
                {
                SetProgressSecondaryTextsL( EImageAndVideo, 
                                            aProgressInfo[ i ].iProgress );
                }
            }
        else
            {
            if ( aProgressInfo[ i ].iProgressType == 
                 TUpnpProgressInfo::EVisualStatus )
                {
                SetDefaultSecondaryTextsL( EPlaylist, 
                                           aProgressInfo[ i ].iProgress );
                }
            else
                {
                SetProgressSecondaryTextsL( EPlaylist, 
                                            aProgressInfo[ i ].iProgress );
                }
            }
        }
    if( ListBox() && ListBox()->BottomItemIndex() > 1 )
        {
        CAknSettingItemList::LoadSettingsL();
        DrawDeferred();
        }
    __LOG( "[CUPnPAppFileSharingList]\t SetFilesharingTextsL end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::SetDefaultSecondaryTextsL
// Update the secondary texts in the dialog
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::SetDefaultSecondaryTextsL( 
    const TInt& aMediaType, 
    const TInt& aState )
    {
    __LOG( "[CUPnPAppFileSharingList]\t \
SetDefaultSecondaryTextsL begin" );
    HBufC* secondaryText = NULL;
    switch ( aMediaType )
        {
    case EImageAndVideo :
        {
        if ( aState == EShareAll)
            {
            secondaryText = StringLoader::LoadLC(
                R_VISUAL_ALBUMS_SHARE_MANY_TEXT);
            }

        else if ( aState == EShareNone)
            {
            secondaryText = StringLoader::LoadLC(
                R_VISUAL_ALBUMS_SHARE_NOTHING_TEXT);
            }

        else // there were shared files
            {
            secondaryText = StringLoader::LoadLC(
                R_VISUAL_ALBUMS_SHARE_ONE_TEXT);
            }

        iVisualAlbums.Copy( *secondaryText );
        }
        break;
    case EPlaylist :
        {
        if ( aState == EShareAll)
            {
            secondaryText = StringLoader::LoadLC(R_MUSIC_SHARE_MANY_TEXT);
            }

        else if ( aState == EShareNone)
            {
            secondaryText = StringLoader::LoadLC(
                R_MUSIC_SHARE_NOTHING_TEXT);
            }

        else // there were shared files
            {
            secondaryText = StringLoader::LoadLC(R_MUSIC_SHARE_ONE_TEXT);

            }

        iPlayLists.Copy( *secondaryText );
        }
        break;
    default:
        break;
        }
    CleanupStack::PopAndDestroy( secondaryText );
    __LOG( "[CUPnPAppFileSharingList]\t SetDefaultSecondaryTextsL end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::SetProgressSecondaryTextsL
// Update the secondary texts in the dialog
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::SetProgressSecondaryTextsL( 
    const TInt& aMediaType, 
    const TInt& aProgress )
    {
    __LOG( "[CUPnPAppFileSharingList]\t \
SetProgressSecondaryTextsL begin" );

    HBufC* secondaryText = StringLoader::LoadLC(
            R_SHARINGUI_SECONDARY_IN_PROGRESS, aProgress );

    switch ( aMediaType )
        {
    case EImageAndVideo :
        {
        iVisualAlbums.Copy(*secondaryText);
        }
        break;
    case EPlaylist :
        {
        iPlayLists.Copy(*secondaryText);
        }
        break;
    default:
        break;
        }
    CleanupStack::PopAndDestroy( secondaryText );
    __LOG( "[CUPnPAppFileSharingList]\t SetProgressSecondaryTextsL end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::FocusChanged
// From CAknDialog
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::FocusChanged( 
    TDrawNow  /*aDrawNow*/ )
    {
    __LOG( "[CUPnPAppFileSharingList]\t \
FocusChanged begin" );
    if ( iWizardRun )
        {
        TRAP_IGNORE( iFileSharingEngine->RequestSharingProgressL());
        iWizardRun = EFalse;
        }

    __LOG( "[CUPnPAppFileSharingList]\t FocusChanged end" );
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::ShowWaitNoteL
// Shows standard wait note
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::ShowWaitNoteL()
    {
    __LOG("[CUPnPAppFileSharingList]\t CUPnPAppFileSharingList::\
ShowWaitNoteL begin");
    TInt resId( R_SHARINGUI_STOPPING_FILE_SHARING_NOTE_DIALOG );
    if ( !iVisibility )
        {
        resId = R_SHARINGUI_STARTING_FILE_SHARING_NOTE_DIALOG;
        }

    iStartingSharing = EPhaseNotActive;
    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
    waitNoteWrapper->ExecuteL(
        resId,      // TInt aResId,
        *this,            // MAknBackgroundProcess& aBackgroundProcess
        ETrue);
    CleanupStack::PopAndDestroy();
    __LOG("[CUPnPAppFileSharingList]\t CUPnPAppFileSharingList::\
ShowWaitNoteL end");
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::StepL
// Step done during wait note
// From MAknBackgroundProcess
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::StepL(void)
    {
    if ( iStartingSharing == EPhaseNotActive )
        {
        iStartingSharing = EPhaseProcessing;
        }
    }


// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::IsProcessDone
// Returns ETrue if process finished
// From MAknBackgroundProcess
// --------------------------------------------------------------------------
//
TBool CUPnPAppFileSharingList::IsProcessDone(void) const
    {
    
    TBool ret(EFalse);
    if ( iStartingSharing == EPhaseNotActive )
        {
        if ( !iVisibility )
            {
            TRAPD( err, iFileSharingEngine->SetSharingStateL( ETrue ) );
            if( err != KErrNone )
                {
                return ETrue;
                }
            }
        else
            {
            TRAPD( err, iFileSharingEngine->SetSharingStateL( EFalse ) );
            if( err != KErrNone )
                {
                return ETrue;
                }
            }
        }
    else if ( iStartingSharing == EPhaseCompleted )
        {
        ret = ETrue;
        }
    else
        {
        __LOG("CUPnPAppFileSharingList::IsProcessDone: else branch");
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPAppFileSharingList::UpdateVisibitySettingItemL
// Update the visibity of item ,when the list is switched foregroud.
// --------------------------------------------------------------------------
//
void CUPnPAppFileSharingList::UpdateVisibitySettingItemL()
    {
    CUPnPSettingsEngine* settingsEngine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( settingsEngine );

    // Read file sharing state
    TInt Visibility = 0;
    if ( iFileSharingEngine->SharingStateL() )
         {
         Visibility = 1;
         }
    CleanupStack::PopAndDestroy( settingsEngine );
    
    if( Visibility != iVisibility )
        {
        iVisibility = Visibility;
        CAknSettingItemList::LoadSettingsL();
        DrawDeferred();
        }
    
    }
//  End of File
