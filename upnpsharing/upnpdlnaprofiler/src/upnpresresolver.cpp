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
* Description:      Implementation file of CUpnpResResolver class. 
*                CUpnpResResolver is a class used for resolving res field 
*                attributes for a given file.
*
*/






// system includes
#include <f32file.h> // RFile
#include <apgcli.h> // RApaLsSession
#include <upnpdlnaprotocolinfo.h> // CUpnpDlnaProtocolInfo
#include <imageconversion.h> // CImageDecoder
#include <caf/caftypes.h> // ContentAccess
#include <3gplibrary/mp4lib.h> // MP4ParseRequestAudioDescription
#include <escapeutils.h> // EscapeUtils::ConvertFromUnicodeToUtf8L

// user includes
#include "upnpresresolver.h"
#include "upnpdlnaprofiler.h"
#include "upnpresparameters.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"

// constants
_LIT8( KHttpGet8, "http-get" );
_LIT8( KStar8, "*" );

const TInt KMicrosecondsInSecond = 1000000;
const TInt KMilliSecondsInSecond = 1000;

// ======== LOCAL FUNCTIONS ========
// NONE

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpResResolver C++ constructor
// --------------------------------------------------------------------------
//
CUpnpResResolver::CUpnpResResolver() : iDuration(0)
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver constructor" );
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpResResolver::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::ConstructL" );
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpResResolver* CUpnpResResolver::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::NewL" );
    CUpnpResResolver* self = CUpnpResResolver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpResResolver* CUpnpResResolver::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::NewLC" );
    CUpnpResResolver* self = new( ELeave ) CUpnpResResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpResResolver destructor
// --------------------------------------------------------------------------
//
CUpnpResResolver::~CUpnpResResolver()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver destructor" );
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::GetResParametersL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpResParameters* CUpnpResResolver::GetResParametersL( 
                                                    const TDesC& aFilename ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetResParametersL" );
    // check that aFileName is reasonable
    if ( aFilename.Length() <= 0 ) 
        {
        User::Leave( KErrArgument );
        }
    
    // Create object that will be returned.
    CUpnpResParameters* retval = CUpnpResParameters::NewLC();

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
    
    // Obtain mimetype of the file
    HBufC8* mimetype = GetMimetypeL( file );
    CleanupStack::PushL( mimetype );
    retval->SetMimetypeL( *mimetype );

    // Obtain file size
    TInt filesize = GetFileSizeL( file );
    retval->SetFileSize( filesize );

    // Obtain duration for video&audio files and resolution for images.
    TSize imageResolution( KErrNotFound, KErrNotFound );
    if ( mimetype->Left( KMimeStartLength ).Compare( KVideo() ) == 0 ) 
        {
        GetVideoDurationL( file );
        retval->SetDurationInSeconds( iDuration.Int64() );
        }
    else if ( mimetype->Left( KMimeStartLength ).Compare( KAudio() ) == 0 ) 
        {
        GetAudioDurationL( aFilename );
        retval->SetDurationInSeconds( 
                                iDuration.Int64() / KMicrosecondsInSecond );
        }
    else if ( mimetype->Left( KMimeStartLength ).Compare( KImage() ) == 0 ) 
        {
        TRAPD( err, imageResolution = GetImageResolutionL( file, 
                                                           *mimetype ) );
        if ( !err ) 
            {
            retval->SetResolution( imageResolution );
            }
        }

    // Construct CUpnpDlnaProtocolInfo object.
    CUpnpDlnaProtocolInfo* protocolInfo = CUpnpDlnaProtocolInfo::NewL(); 
    CleanupStack::PushL( protocolInfo ); 
    protocolInfo->SetFirstFieldL( KHttpGet8() ); 
    protocolInfo->SetSecondFieldL( KStar8() ); 
    protocolInfo->SetThirdFieldL( *mimetype ); 
        
    // Obtain DLNA Profile ID
    CUpnpDlnaProfiler* profiler = CUpnpDlnaProfiler::NewLC();
    HBufC* PnParameter = NULL;
    TRAPD( profileErr, 
           PnParameter = profiler->ProfileForFileL( aFilename,
                                                    file,
                                                    *retval ) );
    CleanupStack::PopAndDestroy( profiler );
    
    if ( PnParameter && !profileErr ) 
        {
        // Set DLNA Profile name (PN parameter)
        CleanupStack::PushL( PnParameter );
        HBufC8* profileName8bit = 
            EscapeUtils::ConvertFromUnicodeToUtf8L( *PnParameter );
        CleanupStack::PushL( profileName8bit );
        protocolInfo->SetPnParameterL( *profileName8bit );
        CleanupStack::PopAndDestroy( profileName8bit );

        // OP parameter is always the same in our CDS.
        protocolInfo->SetOpParameterL( 
                                   UpnpDlnaProtocolInfo::KDEFAULT_DLNA_OP ); 

        // Set Flags according to file type.
        if ( mimetype->Left( KMimeStartLength ).Compare( KVideo() ) == 0 ) 
            {
            protocolInfo->SetFlagsParameterL( 
                           UpnpDlnaProtocolInfo::KDEFAULT_DLNA_FLAGS_AV );
            }
        else if 
           ( mimetype->Left( KMimeStartLength ).Compare( KAudio() ) == 0 ) 
            {
            protocolInfo->SetFlagsParameterL( 
                           UpnpDlnaProtocolInfo::KDEFAULT_DLNA_FLAGS_AUDIO );
            }
        else if 
           ( mimetype->Left( KMimeStartLength ).Compare( KImage() ) == 0 ) 
            {
            protocolInfo->SetFlagsParameterL( 
                           UpnpDlnaProtocolInfo::KDEFAULT_DLNA_FLAGS_IMAGE );
            }
                            
        CleanupStack::PopAndDestroy( PnParameter );
        }

    retval->SetProtocolInfoL( protocolInfo->ProtocolInfoL() );

    // clean up
    CleanupStack::PopAndDestroy( protocolInfo );
    CleanupStack::PopAndDestroy( mimetype );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fsSession );
    CleanupStack::Pop( retval );
    return retval;

    }


// --------------------------------------------------------------------------
// CUpnpResResolver::GetMimetypeL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpResResolver::GetMimetypeL( RFile& aFile ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetMimetypeL" );
    HBufC8* retval = NULL;
    // Create an AppArc server session for mime type recognition 
    RApaLsSession sess;
    User::LeaveIfError( sess.Connect() );
    CleanupClosePushL( sess );

    // Try to get mime type from AppArc server
    TDataRecognitionResult mimeResult;
    User::LeaveIfError( sess.RecognizeData( aFile, mimeResult ) );
    
    // close session handle
    CleanupStack::PopAndDestroy( &sess );

    // Data recognition done. Check results.
    retval = mimeResult.iDataType.Des8().AllocL();
    return retval;
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::GetVideoDurationL
// --------------------------------------------------------------------------
//
void CUpnpResResolver::GetVideoDurationL( RFile& aFile )  
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetVideoDurationL" );
    mp4_u32 videolength = 0;  
    mp4_double framerate = 0; 
    mp4_u32 videotype = 0;    
    mp4_u32 videowidth = 0;   
    mp4_u32 videoheight = 0;  
    mp4_u32 timescale = 0; 

    MP4Handle myMp4Handle;
    
    // try open mp4 file handle
    MP4Err openerr = MP4ParseOpenFileHandle( &myMp4Handle, &aFile );
    switch ( openerr )
        {
        case MP4_OK :
        	{
        	// obtain necessary information from file
            MP4Err requesterr = MP4ParseRequestVideoDescription( 
                                                myMp4Handle, 
                                                &videolength,   
                                                &framerate,     
                                                &videotype,     
                                                &videowidth,    
                                                &videoheight,   
                                                &timescale );
               
            // close mp4 file handle
            MP4Err closeerr = MP4ParseClose( myMp4Handle );
            if ( closeerr || requesterr ) 
                {
                User::Leave( KErrGeneral );
                }
        	}
        	break;
        case MP4_ERROR :
        	{
        	User::Leave( KErrGeneral );
        	}
        	break;
        case MP4_OUT_OF_MEMORY :
        	{
        	User::Leave( KErrNoMemory );
        	}
        	break;
        case MP4_FILE_ERROR :
        	{
        	User::Leave( KErrAccessDenied );
        	}
        	break;
        default :
            User::Leave( KErrGeneral );
            break;
        }
    // videolength in milliseconds
    iDuration = videolength/KMilliSecondsInSecond;
    }
    
// --------------------------------------------------------------------------
// CUpnpResResolver::GetAudioDurationL
// --------------------------------------------------------------------------
//
void CUpnpResResolver::GetAudioDurationL( const TDesC& aFilename ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetAudioDurationL" );

    // create audioplayer if it does not exist
    if ( !iAudioplayer ) 
        {
        iAudioplayer = CMdaAudioPlayerUtility::NewL( *this );
        }

    // Create iWait if it does not exist. Create it here rather than after 
    // OpenL-call so that there will be no problems if somehow OpenFileL 
    // calls MapcInitComplete-callback before iWait is created and started.
    if ( !iWait ) 
        {
        iWait = new( ELeave ) CActiveSchedulerWait;
        }

    // Open file specified in aFilename. This is an asynchronic operation. 
    // Calls MapcInitComplete callback after completed.
    iAudioplayer->OpenFileL( aFilename );
    
    // start CActiveSchedulerWait which is completed in MapcInitComplete
    iWait->Start();

    // clean up
    delete iAudioplayer;
    iAudioplayer = NULL;
    
    delete iWait;
    iWait = NULL;
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::GetFileSizeL
// --------------------------------------------------------------------------
//
TInt CUpnpResResolver::GetFileSizeL( RFile& aFile ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetFileSizeL" );
    TInt retval = KErrNone;
    User::LeaveIfError( aFile.Size( retval ) );
    return retval;
    }


// --------------------------------------------------------------------------
// CUpnpResResolver::GetImageResolutionL
// --------------------------------------------------------------------------
//  
TSize CUpnpResResolver::GetImageResolutionL( RFile& aFile, 
                                             const TDesC8& aMimetype )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetImageResolutionL" );
    CImageDecoder* imageDecoder = NULL;
    TRAPD( createError, imageDecoder = CImageDecoder::FileNewL(
                                                    aFile,
                                                    aMimetype, 
                                                    ContentAccess::EPeek ) );

    if ( createError ) 
        {
        __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::GetImageResolutionL \
CreateError" );
        User::Leave( createError );
        }
                                                              
    CleanupStack::PushL( imageDecoder );
    TSize imageResolution = imageDecoder->FrameInfo().iOverallSizeInPixels;
    CleanupStack::PopAndDestroy( imageDecoder );
    
    return imageResolution;
    }
    
// --------------------------------------------------------------------------
// From class MMdaAudioPlayerCallback.
// MapcPlayComplete callback (not used)
// --------------------------------------------------------------------------
//
void CUpnpResResolver::MapcPlayComplete( TInt /*aError*/ ) 
    {
    //this callback is not needed
    }

// --------------------------------------------------------------------------
// From class MMdaAudioPlayerCallback.
// MapcInitComplete callback is called after a call to 
// CMdaAudioConvertUtility::OpenL has completed.
// --------------------------------------------------------------------------
//
void CUpnpResResolver::MapcInitComplete( 
                                TInt aError, 
                                const TTimeIntervalMicroSeconds& aDuration ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResResolver::MapcInitComplete" );
 
    // if opening was successful, save duration to member variable
    if( KErrNone == aError )
        {
        iDuration = aDuration;
        }

    // continue execution in GetAudioDurationL-method by stopping 
    // ActiveSchedulerWait
    iWait->AsyncStop();
    }

// end of file
