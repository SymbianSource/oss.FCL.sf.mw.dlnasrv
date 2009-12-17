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
* Description:  Source file for CUpnpNoteHandler class.
*
*/


// INCLUDE FILES
// system
#include <bautils.h>                            // BaflUtils
#include <eikenv.h>                             // CEikonEnv
#include <AknWaitDialog.h>                      // CAknWaitDialog
#include <StringLoader.h>                       // StringLoader
#include <aknnotewrappers.h>                    // CAknErrorNote
// upnpframework / common ui
#include "upnpcommonui.h"                       // CUPnPCommonUI
// command internal
#include <upnpcommandresources.rsg>             // UpnpCommand resource file
#include "upnpnotehandler.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"


// CONSTANTS
_LIT( KResFileName,     "\\resource\\upnpcommandresources.rsc" );

// --------------------------------------------------------------------------
// CUpnpNoteHandler::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpNoteHandler* CUpnpNoteHandler::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::NewL" );

    // Create instance
    CUpnpNoteHandler* self = NULL;
    self = new (ELeave) CUpnpNoteHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::CUpnpNoteHandler
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpNoteHandler::CUpnpNoteHandler()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::Constructor" );

    iWaitNote = NULL;
    
    // DRM note will be shown only once
    iDrmNoteShown = EFalse;
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpNoteHandler::~CUpnpNoteHandler()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::Destructor" );

    // If wait note is running, finish it
    if( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->ProcessFinishedL() );
        }

    // Un-load resource file
    if ( iResFileOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ConstructL" );

    // Get the pointer to coe env (owned by Eikon env)
    CEikonEnv* coeEnv = CEikonEnv::Static();
    if( !coeEnv )
        {
        User::Leave( KErrNotSupported );
        }

    // Get the filesession reference from coe env
    RFs& fileSession = coeEnv->FsSession();

    // Load resource file
    TFileName rscFileName( KResFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );

    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        __LOG( "[UpnpCommand]\t Resource file does not exist!" );
        User::Leave( KErrNotFound );
        }

    // Read the resource file offset
    iResFileOffset = coeEnv->AddResourceFileL( rscFileName );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::RunConnectingWaitNote
// Launches the "Connecting" wait note.
// --------------------------------------------------------------------------
//
TInt CUpnpNoteHandler::RunConnectingWaitNote()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::\
RunConnectingWaitNote" );

    TInt status = KErrNone;
    status = RunWaitNote( R_COMMAND_CONNECTING_WAIT_NOTE );
    return status;
    }


// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowDrmNoteL
// Show "DRM protected files not supported." info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowDrmNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowDrmNoteL" );

    if( !iDrmNoteShown )
        {
        iDrmNoteShown = ETrue;
        ShowErrorNoteL( R_COMMAND_DRM_FILE_TEXT );
        }
    else
        {
        __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowDrmNoteL, skipped" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowConnectionLostNoteL
// Show "Connection failed." info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowConnectionLostNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowConnectionLostNoteL" );

    ShowErrorNoteL( R_COMMAND_ERR_CON_FAILED_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowLocalMSStartErrorNoteL
// Show "Connection failed." info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowLocalMSStartErrorNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowLocalMSStartErrorNoteL" );

    // local mediaserver refuses to start.
    // use "connection failed" - this describes the error best.
    ShowErrorNoteL( R_COMMAND_ERR_CON_FAILED_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowCopyInfoNoteL
// Show "%N files copied to %U" info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowCopyInfoNoteL( TInt aCount, 
        const TDesC& aServerName )
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowCopyInfoNoteL" );

    HBufC* infoText = NULL;
    
    if( aCount == 1 )
        {                               
        infoText = StringLoader::LoadLC( 
                R_COMMAND_INFO_COPY_EXT_ONE_TEXT,
                aServerName );
        }
    else
        {
        infoText = StringLoader::LoadLC( 
                R_COMMAND_INFO_COPY_EXT_MANY_TEXT,
                aServerName,
                aCount );
        }

    ShowInfoNoteL( *infoText );
    
    CleanupStack::PopAndDestroy( infoText );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowMoveInfoNoteL
// Show "1 file moved to %U" info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowMoveInfoNoteL( TInt aCount, 
        const TDesC& aServerName )
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowMoveInfoNoteL" );
    // NOTE: defect in UI spec. Missing loc text. Using
    // "N files copied" localisation instead.

    HBufC* infoText = NULL;

    if( aCount == 1 )
        {                                       
        infoText = StringLoader::LoadLC( 
                R_COMMAND_INFO_MOVE_EXT_ONE_TEXT,
                aServerName );
        }
    else
        {
        infoText = StringLoader::LoadLC( 
                R_COMMAND_INFO_MOVE_EXT_MANY_TEXT,
                aServerName,
                aCount );
        }

    ShowInfoNoteL( *infoText );
    
    CleanupStack::PopAndDestroy( infoText );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowCopyErrorNoteL
// Show "Selected device does not support this operation" error note
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowCopyErrorNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowCopyErrorNoteL" );
  
    ShowErrorNoteL( R_COMMAND_ERR_GENERAL_FAILURE_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowMoveErrorNoteL
// Show "Selected device does not support this operation" error note
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowMoveErrorNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowMoveErrorNoteL" );

    ShowErrorNoteL( R_COMMAND_ERR_GENERAL_FAILURE_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowCopyDisconnectionErrorNoteL
// Show "Connection lost, some files may not be copied" error note
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowCopyDisconnectionErrorNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::\
ShowCopyDisconnectionErrorNoteL" );
 
    ShowErrorNoteL( R_COMMAND_ERR_CONN_LOST_COPY_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowMoveDisconnectionErrorNoteL
// Show "Connection lost. Some files might not be moved" error note
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowMoveDisconnectionErrorNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::\
ShowMoveDisconnectionErrorNoteL" );

    ShowErrorNoteL( R_COMMAND_ERR_CONN_LOST_MOVE_TEXT);
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowPlaybackFailedNoteL
// Show "Playback failed on the remote device for unknown error. 
// Try again" info note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowPlaybackFailedNoteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowPlaybackFailedNoteL" );
    
    ShowErrorNoteL( R_COMMAND_ERR_RENDERING_FAILED_UNKNOWN_TEXT );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::RunWaitNote
// Launches the wait note.
// --------------------------------------------------------------------------
//
TInt CUpnpNoteHandler::RunWaitNote( TInt aResource )
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::RunWaitNote" );

    TInt status = KErrNone;
    
    
    // If the note is already showing
    if( iWaitNote )
        {
        status = KErrInUse;
        }
    else
        {
        // Create the wait note
        iWaitNote = new CAknWaitDialog(
            ( REINTERPRET_CAST( CEikDialog**, &iWaitNote ) ), ETrue );
        if( iWaitNote )
            {
            // Execute the wait note
            TBool noteResult = EFalse;
            TRAP( status, noteResult =
                iWaitNote->ExecuteLD(
                aResource ) );
            if( !noteResult )
                {
                status = KErrCancel;
                }

            // Clean up
            iWaitNote = NULL;
            }
        else
            {
            // If we failed to allocate memory for the note
            status = KErrNoMemory;
            }

        }

    return status;
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::CloseWaitNote
// Closes the "Activating sharing" wait note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::CloseWaitNote()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::CloseWaitNote" );

    // If wait note is running, finish it
    if( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->ProcessFinishedL() );
        }
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowInfoNoteL
// Show info note
// --------------------------------------------------------------------------
//    
void CUpnpNoteHandler::ShowInfoNoteL( const TDesC& aText )
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowInfoNoteL" );
    
    CAknInformationNote* infoNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( aText );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowErrorNoteL
// Show error note
// --------------------------------------------------------------------------
//    
void CUpnpNoteHandler::ShowErrorNoteL( TInt aResource )
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ShowInfoNoteL" );

    // Load the string, and show the note
    HBufC* errorText = StringLoader::LoadLC( aResource );
    CAknInformationNote* errorNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ResetDrmNoteCount
// Drm note can shoud be shown only one in operation
// this reset calculator so that note can be shown again
// --------------------------------------------------------------------------
//     
void CUpnpNoteHandler::ResetDrmNoteCount()
    {
    __LOG( "[UpnpCommand]\t CUpnpNoteHandler::ResetDrmNoteCount" );
    iDrmNoteShown = EFalse;
    }

// End of File
