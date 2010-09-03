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
* Description:  Implementation of the class CUPnPAiwEngine.
 *
*/


// INCLUDE FILES
// System
#include <bautils.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <utf.h>

// Upnp stack
#include <upnpitem.h>

// avcontroller api
#include "upnpavcontroller.h"
#include "upnpavcontrollerfactory.h"
#include "upnpavrenderingsession.h"
#include "upnpavsessionbase.h"
#include "upnpavdevice.h"

// avcontroller / avcontroller helper api
#include "upnpitemresolver.h"
#include "upnpitemresolverfactory.h"
#include "upnpitemutility.h"
#include "upnpresourceselector.h"

// upnpframework internal api's
#include "upnpfiletransferengine.h"     // Upnp Fw / UpnpFileTransferEngine
#include "upnpcommonutils.h"            // Upnp Fw / UpnpUtilities

// aiw engine internal
#include <upnpaiwengineresources.rsg>   // Upnp Fw / UpnpAiwEngine
#include "upnpaiwengine.h"                // Upnp Fw / UpnpAiwEngine
#include "upnpaiwtimer.h"               // Upnp Fw / UpnpAiwEngine

// logging
_LIT( KComponentLogfile, "upnpaiwengine.log" );
#include "upnplog.h"

const TInt KSlash = 92;
const TInt KDot = 46;

// CONSTANTS
_LIT( KResFileName, "\\resource\\upnpaiwengineresources.rsc" );
const TInt KAiwTimerIntervalInSeconds = 1;

// --------------------------------------------------------------------------
// CUPnPAiwEngine::NewL
// NewL.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPAiwEngine* CUPnPAiwEngine::NewL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::NewL" );

    CUPnPAiwEngine* self = NULL;

    // If this is the first time we are getting the instance of this
    // singleton class, create the instance and store it into the
    // Thread Local Storage
    if( !Dll::Tls() )
        {
        self = new (ELeave) CUPnPAiwEngine;

        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );

        TInt err = Dll::SetTls( static_cast<TAny*>( self ) );
        if( err != KErrNone )
            {
            delete self;
            self = NULL;
            }
        else
            {
            // Initialise the user counter to zero
            self->InitialiseUserCounter();
            }
        }
    else
        {
        self = static_cast<CUPnPAiwEngine*>( Dll::Tls() );
        }

    // Increase number of users by one
    self->IncreaseUserCounter();
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ReleaseInstance
// Releases the instance.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::ReleaseInstance()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ReleaseInstance" );

    CUPnPAiwEngine* instance = NULL;

    // Get the instance from the TLS if there is an instance
    if( Dll::Tls() )
        {
        instance = static_cast<CUPnPAiwEngine*>( Dll::Tls() );
        }

    // If there still were an instance in the TLS
    if( instance )
        {

        // Decrease the number of instance
        instance->DecreaseUserCounter();

        // If the instance count is now 0, empty the TLS
        if( instance->UserCounter() <= 0 )
            {
            // Empty the TLS
            TInt err = Dll::SetTls( NULL );
            if( err )
                {
                // ignore
                }

            // Delete the instance
            delete instance;
            instance = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CUPnPAiwEngine::CUPnPAiwEngine()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CUPnPAiwEngine" );

    iRenderingSessionInUse = EFalse;
    iVideoPlay = EFalse;
    iFileName = NULL;
    iVideoPlayCancel = EFalse;
    iVideoPlayWait = EFalse;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAiwEngine::~CUPnPAiwEngine()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::~CUPnPAiwEngine" );
    // Delete resolvers
    // Delete for resolvers is done using temporary variables so that we can
    // first nullify the members and then delete the actual objects.
    // This is because local resolver deletion uses active scheduler loops
    // and therefore other asynchronous events may orrur. So we may end
    // up here in Cleanup again, during the resolver is being deleted.
    // if deletion is done the conventional way, the objects get deleted
    // twice, which is not what we want. :-)
    MUPnPItemResolver* tempResolver = iResolver;
    iResolver = NULL;
    delete tempResolver;
    StopTimer();

    // If wait note is shown, finish it
    if (iWaitNoteDialog)
        {
        TRAP_IGNORE( iWaitNoteDialog->ProcessFinishedL() );
        }

    if (iRenderingSession && iAVController)
        {
        TRAP_IGNORE( iAVController->StopRenderingSession(
                        *iRenderingSession ) );
        iRenderingSession = NULL;
        }

    delete iCommonUI;

    // Delete the UPnP AV Controller
    delete iAVController;
    delete iFileName;

    // Un-load resource file
    if (iResFileOffset)
        {
        CEikonEnv::Static()->DeleteResourceFile(iResFileOffset);
        iResFileOffset = 0;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ConstructL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ConstructL" );

    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();

    // Load resource file
    TFileName rscFileName(KResFileName);
    TFileName dllName;
    Dll::FileName(dllName);
    TBuf<2> drive = dllName.Left(2); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive);

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile(fileSession, rscFileName);

    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists(fileSession, rscFileName) )
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, resource file does not \
exist!" );
        User::Leave(KErrNotFound);
        }

    if (iCoeEnv)
        {
        // Read the resource file offset
        iResFileOffset = iCoeEnv->AddResourceFileL(rscFileName);
        }

    // Set initial state of the connection to the UPnPAVController
    iCommonUI = CUPnPCommonUI::NewL();
    iAVController = NULL;
    iRenderingSession = NULL;
    iEngineState = EUPnPEngineNotConnected;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::SetEngineObserver
// Sets the observer instance.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::SetEngineObserver(
        MUPnPAiwEngineObserver* aObserver )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::SetEngineObserver" );

    if( aObserver )
        {
        iObserver = aObserver;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::RemoveEngineObserver
// Sets the observer instance.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::RemoveEngineObserver()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::RemoveEngineObserver" );

    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::EngineState
// Gets the state of the UPnP AIW Engine.
// --------------------------------------------------------------------------
//
EXPORT_C TUPnPEngineState CUPnPAiwEngine::EngineState()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::EngineState" );

    return iEngineState;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::OpenExternalMediaL
// Opens the External media UI.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::OpenExternalMediaL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::OpenExternalMediaL" );

    TInt status = KErrNone;

    if( !iAVController &&
            iEngineState == EUPnPEngineNotConnected )
        {
        TRAP( status,
                iAVController =
                UPnPAVControllerFactory::NewUPnPAVControllerL() );

        if( status == KErrNone &&
                iAVController )
            {
            // Update engine state
            iEngineState = EUPnPEngineBrowsingHomeNetwork;
            TInt leaveCode = KErrNone;

            TRAP( leaveCode,
                    status = iCommonUI->ExecuteDeviceDialogL( *iAVController ) );

            // Handle status and leave codes
            if( leaveCode != KErrNone &&
                    status == KErrNone )
                {
                status = leaveCode;
                }

            // Clean up and return the engine state
            delete iAVController;
            iAVController = NULL;
            iEngineState = EUPnPEngineNotConnected;
            }
        else // If the instantiation of UpnpAvController fails

            {
            TRAP_IGNORE( ShowConnectionErrorL() );
            status = KErrCouldNotConnect;
            }
        }
    else // If the UpnpAvController is already instantiated

        {
        status = KErrInUse;
        }

    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::EnableExternalL
// Enables the showing of local media files on a remote renderer device.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::EnableExternalL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::EnableExternalL" );

    TInt status = KErrNone;

    if( !iAVController &&
            iEngineState == EUPnPEngineNotConnected )
        {
        TRAP( status,
                iAVController =
                UPnPAVControllerFactory::NewUPnPAVControllerL() );

        if( status == KErrNone &&
                iAVController )
            {
            iAVController->SetDeviceObserver( *this );
            TRAP( status, StartRenderingSessionL(
                            EUPnPAiwEngineTargetDeviceWithImageSupport ) );

            // If the rendering session was created successfully
            if( status == KErrNone &&
                    iRenderingSession )
                {
                // Start local file sharing
                TRAP( status,
                        StartLocalFileSharingL( iRenderingSession ) );
                    {
                    // If the file sharing failed to start, undo the enable
                    // external by calling disable external.
                    if( status != KErrNone )
                        {
                        DisableExternal();
                        }
                    }
                }
            else
                {
                delete iAVController;
                iAVController = NULL;
                iEngineState = EUPnPEngineNotConnected;
                }
            }
        else // If the instantiation of UpnpAvController fails

            {
            TRAP_IGNORE( ShowConnectionErrorL() );
            status = KErrCouldNotConnect;
            }
        }
    else
        {
        status = KErrInUse;
        }

    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::DisableExternal
// Disables the showing of local media files on a remote renderer device.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::DisableExternal()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::DisableExternal" );

    if( iAVController )
        {
        if( iRenderingSession )
            {
            // If STOP action is required, use the timer to delay the
            // destruction (give some time for AvController to send the STOP
            // action message)
            if( iEngineState == EUPnPEngineActive )
                {
                TRAP_IGNORE( iRenderingSession->StopL() );
                StartTimer();
                }
            else
                {
                // Stop local file sharing
                TRAP_IGNORE( iRenderingSession->ReleaseLocalMSServicesL() );

                // Stop the rendering session
                TRAP_IGNORE( iAVController->StopRenderingSession(
                                *iRenderingSession ) );
                iRenderingSession = NULL;
                }
            }

        // Delete the UPnP AV Controller
        delete iAVController;
        iAVController = NULL;
        }

    // Update engine state
    iRenderingSessionInUse = EFalse;
    iEngineState = EUPnPEngineNotConnected;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::PlayL
// Plays (sends for rendering) local media file using the selected
// UPnP Media renderer.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::PlayL( const TDesC& aFileName )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::PlayL" );

    TInt status = KErrNone;

    if( aFileName != KNullDesC )
        {

        if( iAVController &&
                iRenderingSession &&
                ( iEngineState == EUPnPEngineConnected ||
                        iEngineState == EUPnPEngineActive ) )
            {

            // Check the file type
            TUPnPItemType fileType = ETypeOther;
            TRAPD( mimeError,
                    fileType = UPnPCommonUtils::ResolveFileTypeL(
                            aFileName ) );
            if( mimeError == KErrNone )
                {
                if( fileType == ETypeAudio )
                    {
                    status = KErrNotSupported;
                    if( iObserver )
                        {
                        iObserver->PlayCompleteL( status );
                        }
                    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, \
audio files not supported!" );
                    }
                else if ( fileType == ETypeVideo )
                    {
                    if( iRenderingSessionInUse )
                        {
                        //if the previous operation not finished
                        delete iFileName; iFileName = NULL;
                        iFileName = aFileName.AllocL();
                        iVideoPlayWait = ETrue;
                        }
                    else
                        {
                        TRAP( status, StartVideoPlayL( aFileName ) );
                        }
                    }
                else if ( fileType == ETypeImage )
                    {
                    if( !iVideoPlay ) //if no video is being played

                        {
                        if( iRenderingSessionInUse )
                            {
                            //if the previous operation not finished
                            iVideoPlayWait = EFalse;
                            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, \
iRenderingSessionInUse: ETrue" );
                            delete iFileName;
                            iFileName = NULL;
                            iFileName = aFileName.AllocL();
                            }
                        else
                            {
                            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, \
iRenderingSessionInUse: EFalse" );
                            TRAP( status , CreateUpnpItemAndSetUriL( aFileName ) );
                            if( KErrNone == status )
                                {
                                iRenderingSessionInUse = ETrue;
                                }
                            else
                                {
                                // Failed, notify MG, the image playing is
                                // complete
                                if( iObserver )
                                    {
                                    iObserver->PlayCompleteL( status );
                                    }
                                }
                            }
                        }
                    else
                        {
                        //notify MG, the music playing is complete
                        if( iObserver )
                            {
                            iObserver->PlayCompleteL( status );
                            }
                        }

                    }
                else // Unsupported file

                    {
                    status = KErrNotSupported;
                    }
                }
            else // Failed to resolve MIME type

                {
                status = KErrArgument;
                }
            }
        else // Rendering session not available (Enable External not done)

            {
            status = KErrNotReady;
            }
        }
    else
        {
        status = KErrArgument;
        }

    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::CopyToExternalL
// Copies local files to a remote media server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::CopyToExternalL(
        RPointerArray<TDesC>& aFileNames )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CopyToExternalL" );

    // Check parameters
    if( aFileNames.Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    // Check engine state
    if( iAVController )
        {
        User::Leave( KErrInUse );
        }

    // Create UPnPAVController
    TRAPD( avControllerError,
            iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL() );
    if( avControllerError != KErrNone )
        {
        TRAP_IGNORE( ShowConnectionErrorL() );
        User::Leave( KErrCouldNotConnect );
        }

    // Update engine state
    iEngineState = EUPnPEngineConnected;

    // Do the transfer
    TRAPD( transferError,
            TransferToExternalL( EAiwEngineCopy, aFileNames, KNullDesC ) );

    // Clean up
    delete iAVController;
    iAVController = NULL;

    // Update engine state
    iEngineState = EUPnPEngineNotConnected;

    if( transferError != KErrNone )
        {
        User::Leave( transferError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::CopyToExternalL
// Copies local files to a remote media server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::CopyPlaylistToExternalL(
        const TDesC& aPlaylistName,
        RPointerArray<TDesC>& aFileNames )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CopyPlaylistToExternalL" );

    // Check parameters
    if( aFileNames.Count() <= 0 ||
            aPlaylistName == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // Check engine state
    if( iAVController )
        {
        User::Leave( KErrInUse );
        }

    // Create UPnPAVController
    TRAPD( avControllerError,
            iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL() );
    if( avControllerError != KErrNone )
        {
        TRAP_IGNORE( ShowConnectionErrorL() );
        User::Leave( KErrCouldNotConnect );
        }

    // Update engine state
    iEngineState = EUPnPEngineConnected;

    // Do the transfer
    TRAPD( transferError,
            TransferToExternalL( EAiwEngineCopyPlaylist,
                    aFileNames,
                    aPlaylistName ) );

    // Clean up
    delete iAVController;
    iAVController = NULL;

    // Update engine state
    iEngineState = EUPnPEngineNotConnected;

    if( transferError != KErrNone )
        {
        User::Leave( transferError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::MoveToExternalL
// Moves local files to a remote media server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::MoveToExternalL(
        RPointerArray<TDesC>& aFileNames )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::MoveToExternalL" );

    // Check parameters
    if( aFileNames.Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    // Check engine state
    if( iAVController )
        {
        User::Leave( KErrInUse );
        }

    // Create UPnPAVController
    TRAPD( avControllerError,
            iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL() );
    if( avControllerError != KErrNone )
        {
        TRAP_IGNORE( ShowConnectionErrorL() );
        User::Leave( KErrCouldNotConnect );
        }

    // Update engine state
    iEngineState = EUPnPEngineConnected;

    // Do the transfer
    TRAPD( transferError,
            TransferToExternalL( EAiwEngineMove, aFileNames, KNullDesC ) );

    // Clean up
    delete iAVController;
    iAVController = NULL;

    // Update engine state
    iEngineState = EUPnPEngineNotConnected;

    if( transferError != KErrNone )
        {
        User::Leave( transferError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::TransferToExternalL
// Transfers (copies/moves) local files/playlists to a remote media server.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::TransferToExternalL(TUPnPAiwEngineTransferMode aMode,
        RPointerArray<TDesC>& aFilePaths, const TDesC& aPlaylistName)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::TransferToExternalL" );

    // Parameters were already checked

    // Select the target server device
    // Title should be according to ui spec.
    TUPnPDialogTitle title = EUPnPCopyToTitle;
    if (aMode == EAiwEngineMove)
        {
        title = EUPnPMoveToTitle;
        }
    CUpnpAVDevice* targetDevice = SelectDeviceL(
            EUPnPSearchServerDevicesWithCopyCapability, title);

    CleanupStack::PushL(targetDevice);

    // Create a browsing session
    TInt status = KErrNone;
    MUPnPAVBrowsingSession* browsingSession= NULL;
    TRAP( status, browsingSession =
            &iAVController->StartBrowsingSessionL( *targetDevice ) );

    if (status == KErrNone && browsingSession)
        {
        // Create a file transfer engine instance
        CUpnpFileTransferEngine* ftEngine= NULL;
        TRAP( status, 
                ftEngine = CUpnpFileTransferEngine::NewL( browsingSession ) );
        if ( status == KErrNone && ftEngine )
            {
            CleanupStack::PushL( ftEngine );

            // Update engine state
            iEngineState = EUPnPEngineActive;
            
            // Number of files in the array.
            TInt count = aFilePaths.Count();

            // Do the copy/move
            if ( aMode == EAiwEngineMove )
                {
                TRAP( status, ftEngine->MoveLocalFilesToRemoteServerL(
                                &aFilePaths ) );
                }
            else if ( aMode == EAiwEngineCopyPlaylist)
                {
                TRAP( status, ftEngine->CopyLocalPlaylistToRemoteServerL(
                                aPlaylistName,
                                &aFilePaths ) );
                }
            else
                {
                TRAP( status, ftEngine->CopyLocalFilesToRemoteServerL(
                                &aFilePaths ) );
                }
        
            // Clean up
            // aFilePaths is not updates until ftEngine is deleted!
            CleanupStack::PopAndDestroy( ftEngine );
            ftEngine = NULL;
            
            // Update engine state
            if ( status == KErrNone )
                {
                iEngineState = EUPnPEngineConnected;

                // Shows proper info note.
                //
                // Calculate number of transferred files based on original 
                // file count and items remaining in aFilePaths 
                // (those were NOT transferred)
                ShowTransferInfoNoteL( 
                        aMode, 
                        count - aFilePaths.Count(), 
                        *targetDevice );
                }
            else if (KErrSessionClosed == status || KErrCouldNotConnect == status 
			           || KErrDisconnected == status )
                {
                iEngineState = EUPnPEngineConnectionLost;
                if ( aMode != EAiwEngineMove )
                    {
                    ShowConnectionLostCopyErrorL();
                    }
                else
                    {
                    ShowConnectionErrorL();
                    }
                }
            else if ( KErrNotFound == status )
                {
                 ShowTransferInfoNoteL (
                                       aMode,
                                       0,
                                       *targetDevice );
                }
            else
                {
                iEngineState = EUPnPEngineConnected;
                }
            }

        // Stop browsing session
        iAVController->StopBrowsingSession( *browsingSession );
        }

    // Clean up
    CleanupStack::PopAndDestroy( targetDevice );
    targetDevice = NULL;

    // Get the titles (of the files that failed to be transfered) from
    // the item array and store them in the file name array. 
    TChar slash( KSlash );
    TChar dot( KDot );
    TInt fileCount = aFilePaths.Count();
    RPointerArray<TDesC16> fileTitles;
    for ( TInt index = 0; index < fileCount; index++ )
        {
        TInt offset1 = aFilePaths[index]->LocateReverse( slash );
        TInt offset2 = aFilePaths[index]->LocateReverse( dot );

        HBufC16* fileTitle= NULL;

        if ( KErrNotFound != offset1 && KErrNotFound != offset2 && 
             offset2 > ( offset1 + 1 ) )
            {

            fileTitle = aFilePaths[index]->
            Mid( ( offset1 + 1 ), ( offset2 - offset1 - 1 ) ).Alloc();

            }
        if (fileTitle)
            {
            fileTitles.AppendL(fileTitle);
            }
        }
    aFilePaths.ResetAndDestroy();
    for (TInt index = 0; index < fileTitles.Count(); index++)
        {
        aFilePaths.AppendL(fileTitles[index]);
        }

    fileTitles.Reset();
    fileTitles.Close();

    if (status != KErrNone)
        {
        User::Leave(status);
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StartRenderingSessionL
// Starts a rendering session with UpnpAvController. Queries the target
// rendering device.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::StartRenderingSessionL(TUPnPAiwEngineDeviceType aType)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::StartRenderingSessionL" );

    TInt status = KErrNone;

    if (iAVController)
        {
        if ( !iRenderingSession)
            {
            CUpnpAVDevice* tempDevice = CUpnpAVDevice::NewL();
            CleanupStack::PushL(tempDevice);

            // Launch the device selection dialog

            if (aType == EUPnPAiwEngineTargetDeviceWithAudioSupport)
                {
                status = iCommonUI->SelectDeviceL( *iAVController,
                        *tempDevice,
                        EUPnPSearchRenderingDevicesWithAudioCapability,
                        EUPnPSelectDeviceTitle);
                }
            else
                if (aType
                        == EUPnPAiwEngineTargetDeviceWithImageAndVideoSupport)
                    {
                    status = iCommonUI->SelectDeviceL( *iAVController,
                            *tempDevice,
                            EUPnPSearchRenderingDevicesWithVideoCapability,
                            EUPnPSelectDeviceTitle);
                    }
                else
                    if (aType == EUPnPAiwEngineTargetDeviceWithImageSupport)
                        {
                        status
                                = iCommonUI->SelectDeviceL(
                                        *iAVController,
                                        *tempDevice,
                                        EUPnPSearchRenderingDevicesWithImageCapability,
                                        EUPnPSelectDeviceTitle);
                        }
                    else
                        {
                        status = KErrNotSupported;
                        }

            if (status == KErrNone)
                {
                // Create a rendering session and register to observe the
                // callbacks
                __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, \
creating rendering session." );
                iRenderingSession
                        = &(iAVController->StartRenderingSessionL( *tempDevice) );
                iRenderingSession->SetObserver( *this);
                __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, \
rendering session created." );

                // Update the engine state
                iEngineState = EUPnPEngineConnected;
                }

            // Clean up
            CleanupStack::PopAndDestroy(tempDevice);
            tempDevice = NULL;
            }
        else
            {
            status = KErrInUse;
            }
        }
    else
        {
        status = KErrNotReady;
        }

    if (status != KErrNone)
        {
        User::Leave(status);
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::InteractOperationComplete
// UPnP AV Controller calls this method to indicate that the requested
// interaction operation (play, stop, etc.) is complete.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::InteractOperationComplete(TInt aError,
        TUPnPAVInteractOperation aOperation)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::InteractOperationComplete" );

    if (iObserver && !iVideoPlayCancel)
        {
        // If the play operation is complete (stop) or the play fails, do
        // the call back
        if (aOperation == EUPnPAVPlay || aOperation == EUPnPAVPlayUser
                || aOperation == EUPnPAVStop || aOperation == EUPnPAVStopUser)
            {
            iEngineState = EUPnPEngineConnected;
            TRAP_IGNORE( iObserver->PlayCompleteL( aError ) );
            }
        }
    if (aOperation == EUPnPAVPlay || aOperation == EUPnPAVStop)
        {
        iRenderingSessionInUse = EFalse;
        if (iFileName) // If there is a pending item
            {
            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, pending item" );
            TRAP_IGNORE( InitPlayL() );
            delete iFileName;
            iFileName = NULL;
            iVideoPlayCancel = EFalse; // No need to send StopL
            }
        else
            {
            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, no pending item" );

            if (iVideoPlayCancel)
                {
                __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, stop previous \
video item," );
                iRenderingSessionInUse = ETrue;
                TRAP_IGNORE( iRenderingSession->StopL() );
                iVideoPlayCancel = EFalse; //StopL sent
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::SetURIResult
// UPnP AV Controller calls this method to indicate that ...
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::SetURIResult(TInt aError)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::SetURIResult" );

    iRenderingSessionInUse = EFalse;
    if (aError != KErrNone && iObserver && !iVideoPlayCancel)
        {
        TRAP_IGNORE( iObserver->PlayCompleteL( aError ) );
        }
    else
        if ( !iRenderingSession)
            {
            if (iObserver)
                {
                TRAP_IGNORE( iObserver->PlayCompleteL( KErrNotReady ) );
                }
            }

    if (iFileName) //if there is a pending item
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, pending item" );
        TRAP_IGNORE( InitPlayL() );
        delete iFileName;
        iFileName = NULL;
        iVideoPlayCancel = EFalse; //no need to send StopL        
        }
    else
        if ( !iVideoPlayCancel)
            {
            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, no pending item" );
            __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, no cancellation from \
video play" );
            TRAP( aError, iRenderingSession->PlayL() );
            //if play failed, send the result right way
            if (aError && iObserver)
                {
                TRAP_IGNORE( iObserver->PlayCompleteL( aError ) );
                }
            else
                if (KErrNone == aError)
                    {
                    iRenderingSessionInUse = ETrue;
                    }
            }
        else
            if (iVideoPlayCancel)
                {
                __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, no pending item" );
                __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, stop previous \
video item" );
                TRAP_IGNORE( iRenderingSession->StopL() );
                iRenderingSessionInUse = ETrue;
                iVideoPlayCancel = EFalse; //StopL sent
                }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ReleaseUpnpResourceL
// Release Upnp resouces
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ReleaseUpnpResourceL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ReleaseUpnpResourceL" );

    // Inform the client about the connection lost
    if (iObserver)
        {
        iObserver->ConnectionLostL();
        }

    // Stop the rendering session
    if ( iRenderingSession && iAVController )
        {
        //iRenderingSession->ReleaseLocalMSServicesL();
        iRenderingSession->RemoveObserver();
        iAVController->StopRenderingSession( *iRenderingSession);
        iRenderingSession = NULL;
        }

    // Disconnect UPnPAVController
    delete iAVController;
    iAVController = NULL;

    iRenderingSessionInUse = EFalse;
    // Update engine state
    iEngineState = /*EUPnPEngineConnectionLost;*/EUPnPEngineNotConnected;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::WLANConnectionLost
// UPnP AV Controller calls this method to indicate that ...
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::WLANConnectionLost()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::WLANConnectionLost" );

    iFileSharingError = KErrDisconnected;
    if ( iFileSharingActivationWaitNote )
        {
        TRAP_IGNORE( iFileSharingActivationWaitNote->ProcessFinishedL() );
        }
    TRAP_IGNORE( iCommonUI->DismissDialogL( KErrDisconnected ) );
    TRAP_IGNORE( ReleaseUpnpResourceL() );
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::MediaRendererDisappeared
// Notifies that the Media Renderer we have a session with has disappeared. 
// Session is now unusable and must be closed. 
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::MediaRendererDisappeared(
        TUPnPDeviceDisconnectedReason aReason)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::MediaRendererDisappeared" );

    if (EDisconnected == aReason)
        {
        TRAP_IGNORE( ShowConnectionErrorL() );
        TRAP_IGNORE( ReleaseUpnpResourceL() );
        }

    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::InitialiseUserCounter
// Sets the initial value of the user counter to zero.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::InitialiseUserCounter()
    {
    iNumberOfUsers = 0;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::IncreaseUserCounter
// Increases the value of the user counter by one.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::IncreaseUserCounter()
    {
    iNumberOfUsers++;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::DecreaseUserCounter
// Decreases the value of the user counter by one.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::DecreaseUserCounter()
    {
    iNumberOfUsers--;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::UserCounter
// Gets the value of the user counter.
// --------------------------------------------------------------------------
//
TInt CUPnPAiwEngine::UserCounter() const
    {
    return iNumberOfUsers;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StartTimer()
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::StartTimer()
    {
    // Stop the timer 
    StopTimer();

    // Start the timer 
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, starting timer" );
    TRAPD( err,
            iUPnPAiwTimer = CUPnPAiwTimer::NewL( KAiwTimerIntervalInSeconds,
                    this ) )
    ;
    if (err)
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, timer start failed!" );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StopTimer()
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::StopTimer()
    {
    // Stop the timer if it is running
    if (iUPnPAiwTimer)
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, stopping timer" );
        delete iUPnPAiwTimer;
        iUPnPAiwTimer = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::TimerCallback
// Callback method for the UPnPAiWTimer.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::TimerCallback()
    {
    StopTimer();
    if( iAVController )
        {
        if( iRenderingSession )
            {
            // Stop local file sharing
            TRAP_IGNORE( iRenderingSession->ReleaseLocalMSServicesL() );

            // Stop the rendering session
            TRAP_IGNORE( iAVController->StopRenderingSession(
                            *iRenderingSession ) );
            iRenderingSession = NULL;
            }

        // Disconnect UPnPAVController
        delete iAVController;
        iAVController = NULL;

        // Update engine state
        iEngineState = EUPnPEngineNotConnected;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ShowConnectionErrorL()
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAiwEngine::ShowConnectionErrorL()
    {
    iCommonUI->DisplayConnectionErrorNoteL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ShowConnectionLostCopyErrorL()
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ShowConnectionLostCopyErrorL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ShowConnectionLostErrorL" );
    iCommonUI->DisplayConnectionLostCopyErrorNoteL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ShowTransferInfoNoteL
// Show correct info note after file transfer is completed.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ShowTransferInfoNoteL(
        TUPnPAiwEngineTransferMode aMode,
        TInt aCount, 
        const CUpnpAVDevice& aServerName ) const
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ShowTransferInfoNoteL" );

    // Converts device name into unicode string.
    HBufC* deviceString = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aServerName.FriendlyName() );
    CleanupStack::PushL( deviceString );

    HBufC* infoText= NULL;
    if ( aCount == 1 )
        {
        if ( aMode == EAiwEngineMove )
            {
            // File moved.
            infoText = StringLoader::LoadLC( 
                    R_AIW_INFO_MOVE_EXT_ONE_TEXT, 
                    *deviceString );
            }
        else
            {
            // File copied.
            infoText = StringLoader::LoadLC( 
                    R_AIW_INFO_COPY_EXT_ONE_TEXT,
                    *deviceString );
            }
        }
    else
        {
        if ( aMode == EAiwEngineMove )
            {
            // Files moved.
            infoText = StringLoader::LoadLC(
                    R_AIW_INFO_MOVE_EXT_MANY_TEXT,
                    *deviceString,
                    aCount);
            }
        else
            {
            // Files copied.
            infoText = StringLoader::LoadLC( 
                    R_AIW_INFO_COPY_EXT_MANY_TEXT,
                    *deviceString,
                    aCount );
            }
        }

    // Shows the dialog.
    CAknInformationNote* infoNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *infoText );
    
    CleanupStack::PopAndDestroy( infoText );
    CleanupStack::PopAndDestroy( deviceString );
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ReserveLocalMSServicesCompleted
// Notifies that the Media Server startup has completed.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ReserveLocalMSServicesCompleted(TInt aError)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::\
ReserveLocalMSServicesCompleted" );

    iFileSharingError = aError;
    if (aError == KErrNone)
        {
        iLocalFileSharingActivated = ETrue;
        }
    if (iFileSharingActivationWaitNote)
        {
        TRAP_IGNORE( iFileSharingActivationWaitNote->ProcessFinishedL() );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StartLocalFileSharingL
// Waits for local media server file sharing activation (or user cancel).
// --------------------------------------------------------------------------
void CUPnPAiwEngine::StartLocalFileSharingL(MUPnPAVSessionBase *aSession)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::StartLocalFileSharingL" );

    // Leave if the parameter is not valid
    if ( !aSession)
        {
        User::Leave(KErrArgument);
        }

    // Initialize the sharing error variable
    iFileSharingError = KErrNone;
    iLocalFileSharingActivated = EFalse;

    // Start the local media server
    aSession->ReserveLocalMSServicesL();

    // Instantiate and launch the wait note until the sharing is
    // enabled of user cancels the operation
    if ( !iLocalFileSharingActivated && iFileSharingError == KErrNone)
        {
        // Instantiate and launch the wait note until the sharing is
        // enabled of user cancels the operation
        iFileSharingActivationWaitNote = new(ELeave)CAknWaitDialog(
                ( REINTERPRET_CAST( CEikDialog**,
                                &iFileSharingActivationWaitNote ) ), ETrue );
        iFileSharingActivationWaitNote->SetCallback( this);
        TRAPD( fileSharingActivationError,
                iFileSharingActivationWaitNote->ExecuteLD(
                        R_AIW_ENABLE_SHARING_WAIT_NOTE ) )
        ;

        // Cancel sharing activation is user pressed cancel
        if (iFileSharingError == KErrCancel)
            {
            TRAP_IGNORE( aSession->CancelReserveLocalMSServicesL() );
            }

        // If sharing failed, and the user did not cancel the operations,
        // forward the leave code.
        if (fileSharingActivationError != KErrNone && iFileSharingError
                != KErrCancel)
            {
            iFileSharingError = fileSharingActivationError;
            }
        }

    if (iFileSharingError != KErrNone)
        {
        User::Leave(iFileSharingError);
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::DialogDismissedL
// ProgressDialog call back method. Get's called when a dialog is
// dismissed.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::DialogDismissedL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyCancel)
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, user cancelled the file \
sharing activation" );
        iFileSharingError = KErrCancel;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::DialogDismissedL
// Method for selecting a device.
// --------------------------------------------------------------------------
//
CUpnpAVDevice* CUPnPAiwEngine::SelectDeviceL(
        TUPnPDeviceTypesToSearch aDeviceType, TUPnPDialogTitle aDlgTitle)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::SelectDeviceL" );

    CUpnpAVDevice* device= NULL;

    // Leave if the engine is not in right state
    if ( !iAVController)
        {
        User::Leave(KErrNotReady);
        }
    else
        {
        // Create new UpnpAvDevice instance
        device = CUpnpAVDevice::NewL();
        CleanupStack::PushL(device);

        // Do the device selection
        TInt ret = iCommonUI->SelectDeviceL( *iAVController, *device,
                aDeviceType, aDlgTitle);

        if (ret != KErrNone)
            {
            CleanupStack::PopAndDestroy(device);
            device = NULL;
            User::Leave(ret);
            }
        CleanupStack::Pop(device); // Will be returned from the method
        }

    return device;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StarVideoPlayL
// Method for Playing a local video
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::StartVideoPlayL(const TDesC& aFileName)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::StartVideoPlayL" );

    //check if selected renderer supports playing video
    TInt status = KErrNone;
    if (iRenderingSession->Device().VideoCapability() )
        {
        // Launch the video player dialog to render the
        // item on the selected rendering device.
        MUPnPItemResolver* resolver = CreateResolverLC( aFileName );
        
        // Update engine state
        iEngineState = EUPnPEngineActive;
        iRenderingSessionInUse = ETrue;
        iVideoPlay = ETrue;

        status = iCommonUI->ExecuteVideoPlayerL( *iRenderingSession,
                resolver->Item() );
        iVideoPlay = EFalse;
        iRenderingSessionInUse = EFalse;
        
        if (KErrSessionClosed == status || KErrDisconnected == status)
            {
            if (KErrSessionClosed == status)
                {
                ReleaseUpnpResourceL();
                }
            }
        else
            {
            // Re-register to observe the rendering
            // session and update engine state
            iRenderingSession->RemoveObserver();
            iRenderingSession->SetObserver( *this);
            iEngineState = EUPnPEngineConnected;
            }

        //notify MG, the video playing is complete
        if (iObserver)
            {
            iObserver->PlayCompleteL(status);
            }

        if (KErrCancel == status)
            {
            TRAP_IGNORE( iRenderingSession->StopL() );
            iRenderingSessionInUse = ETrue;
            iVideoPlayCancel = ETrue;
            }
        
        CleanupStack::PopAndDestroy(); // resolver
        }
    else //if not
        {
        status = KErrNotSupported;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::InitPlayL
// Method for Playing a local file
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::InitPlayL()
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::InitPlayL" );

    //if pending an item
    TInt error = KErrNone;

    if (iVideoPlay || iVideoPlayWait)
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, start video play" );
        TRAP_IGNORE( StartVideoPlayL( *iFileName ) );
        iVideoPlayWait = EFalse;
        }
    else
        {
        __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine, start image play" );
        TRAP( error , CreateUpnpItemAndSetUriL( *iFileName ) );
        if (KErrNone == error)
            {
            iRenderingSessionInUse = ETrue;
            }
        else
            {
            iObserver->PlayCompleteL(error);
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::Wait
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::Wait()
    {
    StopWait();
    iWait.Start();
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::StopWait
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::StopWait()
    {
    if (iWait.IsStarted() )
        {
        if (iWait.CanStopNow() )
            {
            iWait.AsyncStop();
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::CreateUpnpItemAndSetUriL
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::CreateUpnpItemAndSetUriL(const TDesC& aFileName)
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CreateUpnpItemAndSetUriL" );
    
    // Creates resolver to iResolver member variable.
    MUPnPItemResolver* resolver = CreateResolverLC( aFileName );
    
    // Sets uri.
    const CUpnpElement& element = UPnPItemUtility::ResourceFromItemL( 
            iResolver->Item() );
    iRenderingSession->SetURIL( element.Value(), iResolver->Item() );
    
    // Cleanup
    CleanupStack::PopAndDestroy(); // resolver
    
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CreateUpnpItemAndSetUriL -END" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::ResolveComplete
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::ResolveComplete(
        const MUPnPItemResolver& /*aResolver*/, 
        TInt aError )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ResolveComplete" );
    
    iResolveResult = aError;
    StopWait();
    
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::ResolveComplete -END" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::CreateResolverLC
// --------------------------------------------------------------------------
//
 MUPnPItemResolver* CUPnPAiwEngine::CreateResolverLC( const TDesC& aFilePath )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CreateResolverLC" );
    
    // If member variable exists, resolving operation is ongoing.
    if ( iResolver )
        {
        User::Leave( KErrInUse );
        }
    
    // Creates resolver
    TUPnPSelectDefaultResource selector;
    iResolver = UPnPItemResolverFactory::NewLocalItemResolverL( 
            aFilePath,
            *iAVController, 
            selector );
    CleanupStack::PushL( TCleanupItem( CleanupResolver, this ) );
    
    // Starts async resolver.
    iResolveResult = KErrNone;
    iResolver->ResolveL( *this );
    Wait();
    
    // If error occured, resolves it in here. It will cleanup resolver.
    User::LeaveIfError( iResolveResult );
    
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CreateResolverLC -END" );
    
    return iResolver;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEngine::CleanupResolver
// Static method to cleanup resolver object.
// --------------------------------------------------------------------------
//
void CUPnPAiwEngine::CleanupResolver( TAny* aAiwEngine )
    {
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CleanupResolver" );
    
    CUPnPAiwEngine* aiwEngine = reinterpret_cast<CUPnPAiwEngine*>( aAiwEngine );
    if ( aiwEngine )
        {
        MUPnPItemResolver* tempResolver = aiwEngine->iResolver;
        aiwEngine->iResolver = NULL;
        delete tempResolver;
        __LOG( "[UpnpAiwEngine]\t\t Resolver cleaned up." );
        }
    
    __LOG( "[UpnpAiwEngine]\t CUPnPAiwEngine::CleanupResolver -END" );
    }

// End of file
