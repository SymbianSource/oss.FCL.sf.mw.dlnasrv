/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 *  Description : UpnpAuthorizationNote implementation
 *
 */

// INCLUDES
#include <AknGlobalNote.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <avkon.mbg>
#include <barsread.h>
#include <barsc.h>
#include <upnpnotifier.rsg>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

#include "upnpauthorizationnote.h"

_LIT( KComponentLogfile, "upnpsecurity.txt" );
#include "upnplog.h"

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::CUpnpAuthorizationNote
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CUpnpAuthorizationNote::CUpnpAuthorizationNote() :
    CActive(EPriorityStandard) // Standard priority
    {
    // Add to scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAuthorizationNote* CUpnpAuthorizationNote::NewLC()
    {
    CUpnpAuthorizationNote* self = new ( ELeave ) CUpnpAuthorizationNote();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAuthorizationNote* CUpnpAuthorizationNote::NewL()
    {
    CUpnpAuthorizationNote* self = CUpnpAuthorizationNote::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::ConstructL()
    {
    iTimer = CUpnpNotifyTimer::NewL( this );
    RBuf filename;
    GetResourceFilePathLC( filename );
    ReadAuthorisationStringL(filename);
    CleanupStack::PopAndDestroy( &filename );
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::ResourceFilePathLC
// Construct path of resource file
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::GetResourceFilePathLC( RBuf& aResultFilePath )
    {
    const TInt KDrivePrefixLength(2);
    const TInt KPathLength( 
        KDrivePrefixLength + KResourcePath().Length() + KResourceFileName().Length() );
    aResultFilePath.CreateL( KPathLength );
    CleanupClosePushL( aResultFilePath );
    aResultFilePath.Append( RProcess().FileName().Left(KDrivePrefixLength) );
    aResultFilePath.Append( KResourcePath );
    aResultFilePath.Append( KResourceFileName );
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::ReadAuthorisationStringL
// Read authorisation string from given resource file
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::ReadAuthorisationStringL( const TDesC& aFilename )
    {
    ASSERT( NULL == iAuthorizationString );
    
    // open file server session
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Get the exact filename of the resource file
    TFileName rscFileName;
    rscFileName.Copy( aFilename );
    BaflUtils::NearestLanguageFile( fs, rscFileName );
    
    // open resource file
    RResourceFile resourceFile;
    resourceFile.OpenL( fs, rscFileName );
    CleanupClosePushL( resourceFile );
    resourceFile.ConfirmSignatureL();

    // read authorization string
    HBufC8* resourceBuffer =
            resourceFile.AllocReadLC( R_UPNPNOTIFIER_INCOMING_DEVICE_TEXT );
    TResourceReader resourceReader;
    resourceReader.SetBuffer( resourceBuffer );
    TPtrC textdata = resourceReader.ReadTPtrC();
    iAuthorizationString = textdata.AllocL();

    // cleanup
    CleanupStack::PopAndDestroy( resourceBuffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fs );
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::~CUpnpAuthorizationNote
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpAuthorizationNote::~CUpnpAuthorizationNote()
    {
    Cancel(); // Cancel any request, if outstanding
    delete iTimer;
    delete iAuthorizationString;
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::DoCancel
// How to cancel me.
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::DoCancel()
    {
    iTimer->Cancel( );
    TRAP_IGNORE( CancelNoteL( ));
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::CancelNoteL
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::CancelNoteL()
    {
    if ( iNote )
        {
        iNote->CancelNoteL( iNoteId );
        delete iNote;
        iNote = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::ShowNoteL
// Function for making the request.
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::ShowNoteL(
    MUpnpAuthorizationNoteObserver* aObserver )
    {
    ASSERT( !iNote && aObserver );
    iObserver = aObserver;

    iNote = CAknGlobalNote::NewL();
    iNote->SetSoftkeys( R_AVKON_SOFTKEYS_YES_NO );
    iNote->SetGraphic( EMbmAvkonQgn_note_query, EMbmAvkonQgn_note_query_mask );

    iNoteId = iNote->ShowNoteL( iStatus, EAknGlobalInformationNote,
        *iAuthorizationString );

    Cancel(); // Cancel any request, just to be sure
    iTimer->AfterSeconds( KAuthorizationNoteTimeout ); // Set for later
    SetActive(); // Tell scheduler a request is active
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::TimerEventL
// From MUpnpNotifyTimerObserver.
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::TimerEventL( CUpnpNotifyTimer* /*aTimer*/)
    {
    if ( IsActive() )
        {
        // cancelling note will complete outstanding request with EEikBidCancel
        CancelNoteL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::RunL
// From CActive - handle completion.
// -----------------------------------------------------------------------------
//
void CUpnpAuthorizationNote::RunL()
    {
    iTimer->Cancel();

    // always delete the note
    CancelNoteL();

    // handle the result
    if ( iStatus == EAknSoftkeyYes )
        {
        __LOG( "[UpnpSecurity]\t CUpnpAuthorisationNotifier, \
        used authorized the access." );
        iObserver->NoteResponseL( ETrue );
        }
    else
        {
        __LOG( "[UpnpSecurity]\t CUpnpAuthorisationNotifier, \
        used denied the access." );
        iObserver->NoteResponseL( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAuthorizationNote::RunError
// Override to handle leaves from RunL().
// Default implementation causes the active scheduler to panic.
// -----------------------------------------------------------------------------
//
TInt CUpnpAuthorizationNote::RunError( TInt aError )
    {
    __LOG1( "[UpnpSecurity]\t CUpnpAuthorizationNote::RunError %d", aError );
    TRAP_IGNORE( iObserver->NoteResponseL( EFalse ) );
    return KErrNone;
    }

//  End of File
