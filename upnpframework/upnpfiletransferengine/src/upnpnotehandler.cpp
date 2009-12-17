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
* Description:  Implementation of the CUpnpNoteHandler class
*
*/


// INCLUDES
#include <AknUtils.h>                           // CEikonEnv
#include <bautils.h>                            // BaflUtils
#include <eikprogi.h>                           // CEikProgressInfo
#include <AknWaitDialog.h>                      // CAknWaitDialog
#include <upnpstring.h>                         // UpnpString
#include <upnpfiletransferengineresources.rsg>  // Dialog resources
#include <data_caging_path_literals.hrh>        // KDC_RESOURCE_FILES_DIR
#include <StringLoader.h>                       // StringLoader
#include <aknnotewrappers.h>                    // CAknErrorNote

#include "upnpnotehandler.h"
#include "upnpprogressdialogobserver.h"

_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KResFileName,  "upnpfiletransferengineresources.rsc");
const TInt KProgressInfoInitialValue = 0;


// --------------------------------------------------------------------------
// CUpnpNoteHandler::NewL
// NewL.
// --------------------------------------------------------------------------
//
CUpnpNoteHandler* CUpnpNoteHandler::NewL( 
                                    MUPnPProgressDialogobserver* aObserver )
    {
    __LOG( "[UpnpNoteHandler] CUpnpNoteHandler: NewL" );

    CUpnpNoteHandler* self = NULL;
    self = new (ELeave) CUpnpNoteHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __LOG( "[UpnpNoteHandler] CUpnpNoteHandler: NewL end" );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpNoteHandler::CUpnpNoteHandler( MUPnPProgressDialogobserver* aObserver )
    {
    __LOG( "[UpnpNoteHandler] Constructor" );
    iProgressDialogObserver = aObserver;
    iProgressNoteFinish = EFalse;
    __LOG( "[UpnpNoteHandler] Constructor - end" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpNoteHandler::~CUpnpNoteHandler()
    {
    __LOG( "[UpnpNoteHandler] Destructor" );

    // Un-load resource file
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }
    __LOG( "[UpnpNoteHandler] Destructor end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ConstructL()
    {
    __LOG( "[UpnpNoteHandler] ConstructL" );

    // Leave if UI context is not available
    iCoeEnv = CEikonEnv::Static(); // Not owned
    if( !iCoeEnv )
        {
        User::Leave( KErrNotSupported );
        }

    // Get the file server session handle
    RFs fileSession = iCoeEnv->FsSession(); // Not owned

    // Parse the resource file path
    TParse parse;
    parse.Set( KResFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName rscFileName;
    rscFileName.Append( parse.FullName() );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    
    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );

    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        __LOG( "[UpnpNoteHandler] Resource file does not exist!" );
        User::Leave( KErrNotFound );
        }

    // Read the resource file offset
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );
    __LOG( "[UpnpNoteHandler] ConstructL end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::SetValueL
// Sets the value of the progress note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::SetValue( TInt aProgressValue )
    {
    __LOG1( "[UpnpNoteHandler] SetValueL: %d", aProgressValue );
    __LOG1( "[UpnpNoteHandler] iMaxProgressValue: %d", iMaxProgressValue );
    // Update the progress value
    iCurrentProgressValue = aProgressValue;
    
    if( iCurrentProgressValue >= 0 && 
        iCurrentProgressValue <= iMaxProgressValue )
        {
        // Re-draw the note
        if( iProgressInfo && !iProgressNoteFinish )
            {
            iProgressInfo->SetAndDraw( iCurrentProgressValue );
            }    
        }
    
    __LOG( "[UpnpNoteHandler] SetValueL" );
    }
    
// --------------------------------------------------------------------------
// CUpnpNoteHandler::SetValueL
// Sets the max value of the progress note.
// --------------------------------------------------------------------------
//    
void CUpnpNoteHandler::SetMaxValue( TInt aMaxValue )
    {
    __LOG( "[UpnpNoteHandler] SetMaxValue" );
    iMaxProgressValue = aMaxValue;
    if( iProgressInfo && !iProgressNoteFinish )
        {
        iProgressInfo->SetFinalValue( aMaxValue );        
        }
    __LOG( "[UpnpNoteHandler] SetMaxValue end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::DialogDismissedL
// ProgressDialog call back method. Get's called when a dialog is dismissed.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::DialogDismissedL( TInt aButtonId )
    {
    __LOG( "[UpnpNoteHandler] DialogDismissedL" );
    if( aButtonId == EEikBidCancel )
        {
        iProgressDialogObserver->DialogDismissedL();
        }
    __LOG( "[UpnpNoteHandler] DialogDismissedL end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::RunProgressNoteL
// Initialises and runs the progress note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::RunProgressNoteL( TUpnpProgressNoteType aNoteType )
    {
    __LOG( "[UpnpNoteHandler] RunProgressNoteL()" );

    // Check the status of the note
    if( iProgressNoteDialog )
        {
        User::Leave( KErrInUse );
        }

    // reset the iProgressNote flag
    iProgressNoteFinish = EFalse;

    // Create the note
    iProgressNoteDialog = new (ELeave) CAknProgressDialog(
                          ( REINTERPRET_CAST( CEikDialog**,
                          &iProgressNoteDialog ) ) );

    // Select correct resource
    if( aNoteType == EUpnpMoveProgressNote )
        {
        iProgressNoteDialog->PrepareLC( R_FT_MOVE_PROGRESS_NOTE_DIALOG );
        }
    else
        {
        iProgressNoteDialog->PrepareLC( R_FT_COPY_PROGRESS_NOTE_DIALOG );
        }

    // Set values
    iProgressNoteDialog->SetCallback( this );
    iProgressInfo = iProgressNoteDialog->GetProgressInfoL();
    iCurrentProgressValue = KProgressInfoInitialValue;

    // Draw
    iProgressInfo->SetAndDraw( iCurrentProgressValue );

    // Run the note
    iProgressNoteDialog->RunLD();
    
    __LOG( "[UpnpNoteHandler] RunProgressNoteL() end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::FinishProgressNote
// Finishes the progress note.
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::FinishProgressNote()
    {
    __LOG( "[UpnpNoteHandler] FinishProgressNote()" );
    
    iProgressNoteFinish = ETrue;
    // If the progress note is running, finish it
    if( iProgressNoteDialog )
        {
        TRAP_IGNORE( iProgressNoteDialog->ProcessFinishedL() );
        }
    __LOG( "[UpnpNoteHandler] FinishProgressNote() end" );
    }

// --------------------------------------------------------------------------
// CUpnpNoteHandler::ShowSkippingDRMFilesNote
// Shows 'Skipping DRM protected files.' -note
// --------------------------------------------------------------------------
//
void CUpnpNoteHandler::ShowSkippingDRMFilesNoteL()
    {
    __LOG( "[CUpnpNoteHandler] ShowSkippingDRMFilesNote" );
                   
    // Load the string, and show the note
    HBufC* errorText = StringLoader::LoadLC( 
            R_FT_DRM_FILE_TEXT );
    CAknInformationNote* errorNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    errorNote->ExecuteLD( *errorText );
    CleanupStack::PopAndDestroy( errorText );
    }

// End of file
