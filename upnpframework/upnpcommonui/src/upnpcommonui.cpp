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
* Description:  Implementation of Common UI.
*
*/


// INCLUDE FILES
// System
#include <AknUtils.h>
#include <featmgr.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <pathinfo.h> //PathInfo

// upnp stack api
#include <upnpobject.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"
#include "upnpmediaserversettings.h"

// upnpframework / internal api's
#include "upnpmusicadapter.h"

// commonui internal
#include "upnpcommonui.h"
#include <upnpcommonui.rsg>
#include "upnpexternaldevicedialog.h"
#include "upnpbrowsedialog.h"
#include "upnpvideoplayerdialog.h"
#include "upnpadvfinddialog.h"
#include "upnpselectiondialog.h"

// debug stuff
_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// CONSTANTS
// Filename of rsc file
_LIT( KUPnPCommonUiRscFile, "\\resource\\upnpcommonui.rsc" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPCommonUI::CUPnPCommonUI
// --------------------------------------------------------------------------
//
CUPnPCommonUI::CUPnPCommonUI()
    {
    __LOG( "CUPnPCommonUI::CUPnPCommonUI" );
    iFeatureManagerInitialized = EFalse;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPCommonUI::ConstructL()
    {
    __LOG( "CUPnPCommonUI::ConstructL" );

    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();

    // Load common ui resource file
    TFileName rscFileName( KUPnPCommonUiRscFile );
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

    // Get AppUI pointer
    iAppUi = static_cast<CAknViewAppUi*>( iCoeEnv->EikAppUi() );
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPCommonUI* CUPnPCommonUI::NewL()
    {
    __LOG( "CUPnPCommonUI::NewL" );
    
    CUPnPCommonUI* self = new ( ELeave) CUPnPCommonUI();
    CleanupStack::PushL( self );

    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPCommonUI::~CUPnPCommonUI()
    {
    __LOG( "CUPnPCommonUI::~CUPnPCommonUI" );

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
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ExecuteDeviceDialogL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPCommonUI::ExecuteDeviceDialogL(
    MUPnPAVController& aAVControl )
    {
    __LOG( "CUPnPCommonUI::ExecuteDeviceDialogL" );
    
    TInt returnValue = KErrArgument;

    iExternalDeviceSelection = CUPnPExternalDeviceDialog::NewL(
        R_UPNPCOMMONUI_EXTERNAL_DEVICE_DIALOG_MENUBAR,
        aAVControl,
        *this );

    
    iExternalDeviceSelection->SetMopParent( iAppUi );
    returnValue = iExternalDeviceSelection->ExecuteLD(
        R_UPNPCOMMONUI_EXTERNAL_MEDIA_SELECT_DIALOG );
    iExternalDeviceSelection = NULL;

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ExecuteBrowseDialogL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPCommonUI::ExecuteBrowseDialogL(
    MUPnPAVController& aAVControl,
    const CUpnpAVDevice& aDevice)
    {
    __LOG( "CUPnPCommonUI::ExecuteBrowseDialogL" );


    TInt returnValue = KErrArgument;
    
    TInt error = KErrNone; 


    iBrowseSelection = CUPnPBrowseDialog::NewL(
            R_UPNPCOMMONUI_BROWSE_DIALOG_MENUBAR, aAVControl, aDevice, *this );

    iBrowseSelection->SetMopParent( iAppUi );
    TRAP( error, returnValue =  iBrowseSelection->ExecuteLD( 
                                        R_UPNPCOMMONUI_BROWSE_DIALOG ) );
    iBrowseSelection = NULL;

    if( error != KErrNone )
        {
        returnValue = error;
        }
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ExecuteVideoPlayerL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPCommonUI::ExecuteVideoPlayerL( 
    MUPnPAVRenderingSession& aRenderingSession,
    const CUpnpObject& aObject )
    {
    __LOG( "CUPnPCommonUI::ExecuteVideoPlayerL" );

    TInt returnValue = KErrArgument;

    iVideoPlayerDialog = CUPnPVideoPlayerDlg::NewL( 
              aRenderingSession, aObject, *this );
    iVideoPlayerDialog->SetMopParent( iAppUi );
    TRAPD( error, returnValue = iVideoPlayerDialog->ExecuteLD(
        R_UPNPCOMMONUI_VIDEO_PLAYER_DIALOG ) );
    iVideoPlayerDialog = NULL;
    
    if( error != KErrNone )
        {
        returnValue = error;
        }
        
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ExecuteMusicPlayerL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPCommonUI::ExecuteMusicPlayerL( 
                                   MUPnPAVController& aAVControl,
                                   CUPnPPlayListFiller* aFiller,
                                   const CUpnpAVDevice* aTargetDevice )
    {
    TInt returnValue = KErrArgument;

    iMusicAdapter = CUPnPMusicAdapter::NewL( aAVControl ); 
    TInt error = KErrNone;   
    if( aTargetDevice )
        {
        //play in remote
        TRAP( error, returnValue = iMusicAdapter->ExecuteMusicInRemoteL( 
            iCoeEnv->EikAppUi(),
            aFiller,
            *aTargetDevice ) );
        }
    else 
        {
        //play in local
        TRAP( error, returnValue = iMusicAdapter->ExecuteMusicInLocalL(
             iCoeEnv->EikAppUi(),
             aFiller ) );
        }
    delete iMusicAdapter;
    iMusicAdapter = 0;

    if( error != KErrNone )
        {
        returnValue = error;
        }
    return returnValue;
    }

                  
// --------------------------------------------------------------------------
// CUPnPCommonUI::SelectDeviceL
// Displays a UPnP device selection pop-up dialog.
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPCommonUI::SelectDeviceL( MUPnPAVController& aAVControl,
                                            CUpnpAVDevice& aDevice,
                                            TUPnPDeviceTypesToSearch aType,
                                            TUPnPDialogTitle aTitle )
    {
    __LOG( "CUPnPCommonUI::SelectDeviceL" );


    TInt returnValue = KErrGeneral;

    // Create the selection popup
    delete iDeviceSelection; iDeviceSelection = NULL;
    iDeviceSelection = CUPnPSelectionDialog::NewL( aAVControl );
    
    // Read the title string
    HBufC* title = NULL;
    if( aTitle == EUPnPCopyToTitle )
        {
        title = StringLoader::LoadLC( R_UPNPCOMMONUI_COPY_TO_TEXT );
        } 
    else if( aTitle == EUPnPMoveToTitle )
        {        
        title = StringLoader::LoadLC( R_UPNPCOMMONUI_MOVE_TO_TEXT );
        }
    else
        {
        if( aType == EUPnPSearchRenderingDevicesWithAudioCapability )
            {
            title = StringLoader::LoadLC( R_UPNPCOMMONUI_SELECT_PLAYER_TEXT );
            }
        else
            {
            title = StringLoader::LoadLC( R_UPNPCOMMONUI_SELECT_DEVICE_TEXT );
            }
        }

    if( aType == EUPnPSearchAllDevices ||
        aType == EUPnPSearchAllServerDevices ||
        aType == EUPnPSearchServerDevicesWithCopyCapability ||
        aType == EUPnPSearchServerDevicesWithSearchCapability ||
        aType == EUPnPSearchAllRenderingDevices ||
        aType == EUPnPSearchRenderingDevicesWithImageCapability ||
        aType == EUPnPSearchRenderingDevicesWithVideoCapability ||
        aType == EUPnPSearchRenderingDevicesWithImageAndVideoCapability ||
        aType == EUPnPSearchRenderingDevicesWithAudioCapability )
        {
        iDeviceSelection->CreatePopupL( *title, aType );
        returnValue = iDeviceSelection->StartPopupL( aDevice );
        }
    else
        {
        CleanupStack::PopAndDestroy( title ); 
        title = NULL;
        return KErrNotSupported;
        }
    
     // Clean up
    CleanupStack::PopAndDestroy( title ); 
    title = NULL;
    delete iDeviceSelection; iDeviceSelection = NULL;

    __LOG1( "CUPnPCommonUI::SelectDeviceL: %d", returnValue );
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::DisplayConnectionErrorNote()
// Displays error note R_UPNP_ERROR_CON_TEXT
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPCommonUI::DisplayConnectionErrorNoteL()
    {
    HBufC* errorText = StringLoader::LoadLC( R_UPNPCOMMONUI_ERROR_CON_TEXT );
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::DisplayConnectionLostCopyErrorNoteL()
// Displays error note R_UPNPCOMMONUI_CONN_LOST_COPY_ERROR_TEXT
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPCommonUI::DisplayConnectionLostCopyErrorNoteL()
    {
    __LOG( "CUPnPCommonUI::DisplayConnectionLostCopyErrorNoteL" );
    HBufC* errorText = StringLoader::LoadLC( 
                           R_UPNPCOMMONUI_CONN_LOST_COPY_ERROR_TEXT );
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::ExecuteAdvFindDialogL()
// --------------------------------------------------------------------------
//
TInt CUPnPCommonUI::ExecuteAdvFindDialogL(
                                MUPnPAVController& aAVControl,
                                MUPnPAVBrowsingSession& aBrowsingSession)

    {
    __LOG( "CUPnPCommonUI::ExecuteAdvFindDialogL" );
    
    
    TInt returnValue = KErrArgument;
    TInt error = KErrNone;

    iAdvFindDialog = CUPnPAdvancedFindDialog::NewL(
                            R_UPNPCOMMONUI_ADVANCED_FIND_MAIN_DIALOG,
                            aAVControl,
                            aBrowsingSession,
                            *this );

    iAdvFindDialog->SetMopParent( iAppUi );

    TRAP( error, returnValue = iAdvFindDialog->ExecuteLD(
                            R_UPNPCOMMONUI_ADVANCED_FIND_MAIN_DIALOG ) );
    
    iAdvFindDialog = NULL;
    
    if( KErrNone != error )
        {
        returnValue = error;
        }
    
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::DismissDialogL()
// called only when the media server disconnected or WLAN connection is lost
// to close the dialogs which are running on the top of the mother class
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPCommonUI::DismissDialogL( TInt aError )
    {
    if( iDeviceSelection && KErrSessionClosed == aError )
        {
        iDeviceSelection->DismissItself( aError );
        }
    
    if( iVideoPlayerDialog )
        {
        iVideoPlayerDialog->DismissItselfL( aError ); 
        iVideoPlayerDialog = NULL;
        }
        
    if( iMusicAdapter )
        {
        iMusicAdapter->Dismiss( aError );
        //iMusicAdapter = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::DisplayErrorTextL
// 
// --------------------------------------------------------------------------
void CUPnPCommonUI::DisplayErrorTextL( TInt aResource )
    {
    CAknErrorNote* errorNote = new( ELeave )CAknErrorNote();
    errorNote->ExecuteLD( *StringLoader::LoadLC( aResource ) );
    CleanupStack::PopAndDestroy();  
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::DisplayErrorTextL
// --------------------------------------------------------------------------
// 
void CUPnPCommonUI::DisplayErrorTextL( TInt aResource, const TDesC& aInfo )
    {
    HBufC* errorText = StringLoader::LoadLC( aResource, aInfo );
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DisplayErrorNote
// --------------------------------------------------------------------------
//    
void CUPnPCommonUI::DisplayErrorTextL( TInt aResource,
                                       TInt aMaxNumberOfResultsShown )
    {    
    HBufC* errorText = StringLoader::LoadLC( aResource, 
                                             aMaxNumberOfResultsShown );
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DisplayInfoTextL
// --------------------------------------------------------------------------
//      
void CUPnPCommonUI::DisplayInfoTextL( TInt aResource,
                                      TInt aNumberOfCopy )
    {
    CAknInformationNote* errorNote = 
                                new( ELeave )CAknInformationNote( ETrue );
    errorNote->ExecuteLD( *StringLoader::LoadLC( aResource, 
                                                 aNumberOfCopy ) );
    CleanupStack::PopAndDestroy(); 
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::DisplayInfoTextL
// --------------------------------------------------------------------------
//      
void CUPnPCommonUI::DisplayInfoTextL( TInt aResource )
    {
    CAknInformationNote* errorNote = 
                                new( ELeave )CAknInformationNote( );
    errorNote->ExecuteLD( *StringLoader::LoadLC( aResource ) );
    CleanupStack::PopAndDestroy(); 
    }    
    
// --------------------------------------------------------------------------
// CUPnPBrowseDialog::PresenceOfDialog
// --------------------------------------------------------------------------
//  
TBool CUPnPCommonUI::PresenceOfDialog()
    {
    if( iBrowseSelection || iAdvFindDialog || iDeviceSelection )
        {
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::GetUpnpAction()
// --------------------------------------------------------------------------
//
void CUPnPCommonUI::GetUpnpAction( TUPnPAction aAction )
    {
    iAction = aAction;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseDialog::HandleCommonErrorL
// --------------------------------------------------------------------------
//      
void CUPnPCommonUI::HandleCommonErrorL( TInt aError,
                                        TInt aCopyIndex  )
    {
    
    //specified errors for every action should be handled first
    // common erros last
    __LOG1( "CUPnPCommonUI::HandleCommonErrorL: %d", aError );
    
    if( KErrNone <= aError && ( iAction == CUPnPCommonUI::EUPnPNone ||
        iAction != CUPnPCommonUI::EUPnPCopy ) )
        {
        iAction = EUPnPNone;
        return;
        }
    switch( aError )
        {
        case KErrCancel:
            {
            break;
            }
        case KErrServerBusy:
            {
            DisplayErrorTextL( R_UPNPCOMMONUI_GENERAL_FAILURE_ERROR_TEXT );
            break; 
            }
        case KErrNoMemory:
            {
            DisplayErrorTextL( R_UPNPCOMMONUI_NOMEMORY_TEXT );
            break;
            }
        case KErrDiskFull:
            {
            HBufC* copyLocation = HBufC::NewLC( KMaxFileName );
            TPtr copyLocationPtr( copyLocation->Des() );
            GetCopyLocationL( copyLocationPtr );
            TPtr driveName = copyLocationPtr.LeftTPtr( 2 );
            DisplayErrorTextL( R_UPNPCOMMONUI_DEVICE_MEMORY_LOW, driveName );
            CleanupStack::PopAndDestroy( copyLocation );
            break;
            }
        case KErrSessionClosed:
        case KErrDisconnected:
            {
            if( iAction == CUPnPCommonUI::EUPnPCopy )
                {
                //Connection lost, some files may not be copied
                DisplayErrorTextL( R_UPNPCOMMONUI_CONN_LOST_COPY_ERROR_TEXT );
                }
            else if( KErrSessionClosed == aError &&
                     iAction != CUPnPCommonUI::EUPnPCopy )
                {
                //connection failed
                DisplayErrorTextL( R_UPNPCOMMONUI_ERROR_CON_TEXT );
                }
            delete iBrowseSelection;
            iBrowseSelection = NULL;
            break;
            }
        case KErrNotSupported:
            {
            //Selected device does not support this operation
            DisplayErrorTextL( R_UPNPCOMMONUI_NOT_SUPPORTED_ERROR_TEXT );
            break;
            }
        default:
            {
            if( iAction != EUPnPNone )
                {
                if( iAction == EUPnPCopy )  
                    {
                    if( KErrNone == aError )
                        {
                        if( aCopyIndex == 1) //copying single item OK
                            {
                            DisplayInfoTextL( 
                                    R_UPNPCOMMONUI_INFO_COPY_ONE_TEXT );
                            }
                        else if( aCopyIndex > 1 ) //copying multi items OK
                            {
                            DisplayInfoTextL( 
                                    R_UPNPCOMMONUI_INFO_COPY_MANY_TEXT, 
                                    aCopyIndex );
                            }
                        else
                            {
                            //
                            }    
                        }
                    else
                        {
                        if( KErrNotFound == aError )
                            {
                            if( aCopyIndex ) //copying a container
                                {
                                //"Container does not contain any items."
                                DisplayErrorTextL( 
                                R_UPNPCOMMONUI_NO_ITEMS_TO_PLAY_TEXT );
                                }
                            else
                                {
                                //copy failed
                                DisplayErrorTextL( 
                                R_UPNPCOMMONUI_GENERAL_FAILURE_ERROR_TEXT );
                                }
                            }
                         else //if copying, unkown error
                            {
                            //copy failed
                            DisplayErrorTextL( 
                                R_UPNPCOMMONUI_GENERAL_FAILURE_ERROR_TEXT );
                            }   
                        }
                    }
                else if( iAction == EUPnPShow )
                    {
                    if( KErrNone != aError )
                        {
                        //Playback failed on the remote device for 
                        //unknown error. Try again
                        DisplayErrorTextL( 
                        R_UPNPCOMMONUI_RENDERING_FAILED_UNKNOWN_ERROR_TEXT );
                        }
                    }
                else
                    {
                    if( KErrNone != aError )
                        {
                        //Selected device refused the operation
                        DisplayErrorTextL( 
                            R_UPNPCOMMONUI_GENERAL_FAILURE_ERROR_TEXT );
                        }
                    }
                }//if( iAction != EUPnPNone )
           
            break;        
            }//default:
            
        } // switch( aError )
        
        iAction = EUPnPNone;
    }

// --------------------------------------------------------------------------
// CUPnPCommonUI::GetCopyLocationL
// --------------------------------------------------------------------------
//
void CUPnPCommonUI::GetCopyLocationL( TDes& aLocation ) const
    {
    // Get instance of Server Settings object
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );

    // Get the location setting from ServerSettings
    settings->Get( UpnpMediaServerSettings::EUploadDirectory, aLocation );
        
    CleanupStack::PopAndDestroy( settings );    
    }
// End of File
