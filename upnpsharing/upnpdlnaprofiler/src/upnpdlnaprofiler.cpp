/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Implementation file of CUpnpDlnaProfiler class that is used
*                for resolving a DLNA profile of a given file.
*
*/






// system includes
#include <f32file.h>     // RFile
#include <apgcli.h>      // RApaLsSession

// user includes
#include "upnpdlnaprofiler.h"
#include "upnpprofiler.h"
#include "upnpaudiosolverbase.h"
#include "upnpavsolverbase.h"
#include "upnpimagesolverbase.h"
#include "upnpresparameters.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"

// constants
// NONE

// ======== LOCAL FUNCTIONS ========
// NONE

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpDlnaProfiler C++ constructor
// --------------------------------------------------------------------------
//
CUpnpDlnaProfiler::CUpnpDlnaProfiler()
    {
    __LOG( "[UPnPDlnaProfiler] Constructor" );
    }


// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpDlnaProfiler::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] ConstructL" );
    CUpnpAudioSolverBase* baseAudioSolver = CUpnpAudioSolverBase::NewL();
    CleanupStack::PushL( baseAudioSolver );
    iAudioSolvers.AppendL( baseAudioSolver );
    CleanupStack::Pop( baseAudioSolver );
    
    CUpnpAvSolverBase* baseAvSolver = CUpnpAvSolverBase::NewL();
    CleanupStack::PushL( baseAvSolver );
    iAvSolvers.AppendL( baseAvSolver );
    CleanupStack::Pop( baseAvSolver );
    
    CUpnpImageSolverBase* baseImageSolver = CUpnpImageSolverBase::NewL();
    CleanupStack::PushL( baseImageSolver );
    iImageSolvers.AppendL( baseImageSolver );
    CleanupStack::Pop( baseImageSolver );
    }


// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpDlnaProfiler* CUpnpDlnaProfiler::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] NewL" );
    CUpnpDlnaProfiler* self = CUpnpDlnaProfiler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpDlnaProfiler* CUpnpDlnaProfiler::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] NewLC" );
    CUpnpDlnaProfiler* self = new( ELeave ) CUpnpDlnaProfiler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpDlnaProfiler destructor
// --------------------------------------------------------------------------
//
CUpnpDlnaProfiler::~CUpnpDlnaProfiler()
    {
    __LOG( "[UPnPDlnaProfiler] Destructor" );
    iAudioSolvers.ResetAndDestroy();
    iAudioSolvers.Close();
    
    iAvSolvers.ResetAndDestroy();
    iAvSolvers.Close();
    
    iImageSolvers.ResetAndDestroy();
    iImageSolvers.Close();
    }

// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::SupportedProfilesL
// Returns DlnaProfiles that are supported with currently loaded solver 
// plugins.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDlnaProfiler::SupportedProfilesL( 
                                          CDesC16ArrayFlat* aProfiles ) const
    {
    __LOG( "[UPnPDlnaProfiler] SupportedProfilesL" );
    TInt retval = KErrNone;

    if ( !aProfiles ) 
        {
        // Invalid parameter
        retval = KErrArgument;
        }
    else 
        {
        // append all audio profiles recognized 
        for ( TInt i( iAudioSolvers.Count() - 1 ); i >= 0 ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iAudioSolvers[i];
            currentProfiler->SupportedProfilesL( aProfiles );
            }
        // append all av profiles recognized 
        for ( TInt i( iAvSolvers.Count() - 1 ); i >= 0 ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iAvSolvers[i];
            currentProfiler->SupportedProfilesL( aProfiles );
            }
        // append all image profiles recognized 
        for ( TInt i( iImageSolvers.Count() - 1 ); i >= 0 ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iImageSolvers[i];
            currentProfiler->SupportedProfilesL( aProfiles );
            }
        }
        
    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::ProfileForFileL
// Resolves the DLNA profile for a file given in aFileName parameter.
// Returns NULL if DLNA profile could not be recognized with currently loaded
// solver plugins.
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUpnpDlnaProfiler::ProfileForFileL( const TDesC& aFilename )
    {
    __LOG( "[UPnPDlnaProfiler] ProfileForFileL" );
    // check that aFileName is reasonable
    if ( aFilename.Length() <= 0 ) 
        {
        User::Leave( KErrArgument );
        }
    
    HBufC* retval = NULL;
    
    // try to open file specified by aFileName
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); // connect session
    CleanupClosePushL( fsSession );
    // without calling ShareProtected() RApaLsSession::RecognizeData 
    // does not work (leaves with KErrBadHandle).
    fsSession.ShareProtected();
    
    RFile file;  
    User::LeaveIfError( file.Open( fsSession, 
                                   aFilename, 
                                   EFileShareReadersOrWriters|EFileRead ) );
    CleanupClosePushL( file );
    
    // Create an AppArc server session for mime type recognition 
    RApaLsSession sess;
    User::LeaveIfError( sess.Connect() );
    CleanupClosePushL( sess );

    // Try to get mime type from AppArc server
    TDataRecognitionResult mimeResult;
    User::LeaveIfError( sess.RecognizeData( file, mimeResult ) );
    
    // close session handle
    CleanupStack::PopAndDestroy( &sess );

    // Data recognition done. Check results.
    HBufC8* mimetype = HBufC8::NewL( mimeResult.iDataType.Des8().Length() );
    mimetype->Des().Append( mimeResult.iDataType.Des8() );
        
    if ( mimetype && mimetype->Length() > 0 ) 
        {
        CleanupStack::PushL( mimetype );
        retval = DlnaProfileFromSolversL( aFilename, *mimetype, file );
        CleanupStack::PopAndDestroy( mimetype );
        }

    // close file and filesession handles
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fsSession );
            
    return retval;
    }
    
// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::ProfileForFileL
// Resolves the DLNA profile for a file given in aFileName parameter.
// Returns NULL if DLNA profile could not be recognized with currently loaded
// solver plugins. Takes RFile and CUpnpResParameters parameters in order to 
// minimize the amount of file opening.
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUpnpDlnaProfiler::ProfileForFileL( 
                                        const TDesC& aFilename, 
                                        RFile& aFile,
                                        CUpnpResParameters& aParameters ) 
    {
    __LOG( "[UPnPDlnaProfiler] ProfileForFileL" );
    // check that aFileName is reasonable
    if ( aFilename.Length() <= 0 || aParameters.MimeType().Length() <= 0 ) 
        {
        User::Leave( KErrArgument );
        }
    
    HBufC* retval = NULL;

    retval = DlnaProfileFromSolversL( aFilename, 
                                      aParameters.MimeType(), 
                                      aFile );
            
    return retval;    
    }
    
// --------------------------------------------------------------------------
// CUpnpDlnaProfiler::DlnaProfileFromSolversL
// Checks the mime type given as parameter and forwards the recognition task 
// to solver plugins corresponding the mimetype.
// --------------------------------------------------------------------------
//
HBufC* CUpnpDlnaProfiler::DlnaProfileFromSolversL( 
                                const TDesC& aFilename, 
                                const TDesC8& aMimetype, 
                                RFile& aFile )
    {
    __LOG( "[UPnPDlnaProfiler] DlnaProfileFromSolversL" );
    HBufC* retval = NULL;

    TPtrC8 mediatype = aMimetype.Left( KMimeStartLength );
    if ( mediatype.Compare( KAudio() ) == 0 ) 
        {
        for ( TInt i( iAudioSolvers.Count() - 1 ); i >= 0 && !retval ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iAudioSolvers[i];
            retval = currentProfiler->ProfileForFileL( aFilename, 
                                                       aMimetype, 
                                                       aFile );
            }
        }
    else if ( mediatype.Compare( KVideo() ) == 0 ) 
        {
        for ( TInt i( iAvSolvers.Count() - 1 ); i >= 0 && !retval ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iAvSolvers[i];
            retval = currentProfiler->ProfileForFileL( aFilename, 
                                                       aMimetype, 
                                                       aFile );
            }
        }
    else if ( mediatype.Compare( KImage() ) == 0 ) 
        {
        for ( TInt i( iImageSolvers.Count() - 1 ); i >= 0 && !retval ; i-- ) 
            {
            MUpnpProfiler* currentProfiler = iImageSolvers[i];
            retval = currentProfiler->ProfileForFileL( aFilename, 
                                                       aMimetype, 
                                                       aFile );
            }
        }
    else 
        {
        retval = NULL;
        }
        
    return retval;
    }
    
// end of file
