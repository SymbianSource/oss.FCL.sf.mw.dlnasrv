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
* Description:      Implementation of base DLNA profile resolver class for audio 
*                files.
*
*/






// system includes
// NONE

// includes
#include <mdaaudiosampleeditor.h> // CMdaAudioConvertUtility
#include <badesca.h> // CDesC16ArrayFlat
#include "upnpaudiosolverbase.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"

// constants
_LIT( KTargetAudioFile, "c:\\unnecessary.txt" );

// audio mime types
_LIT8( KAudioMpeg, "audio/mpeg" );
_LIT8( KAudio3gpp, "audio/3gpp" );
_LIT8( KAudioMp4, "audio/mp4" );
_LIT8( KAudioL16, "audio/L16" );
_LIT8( KAudioAdts, "audio/vnd.dlna.adts" );
_LIT8( KAudioWma, "audio/x-ms-wma" );

// audio profiles
_LIT( KLpcm, "LPCM" );
_LIT( KMp3, "MP3" );
_LIT( KAac_iso, "AAC_ISO" );
_LIT( KAac_iso_320, "AAC_ISO_320" );
_LIT( KAac_mult5_iso, "AAC_MULT5_ISO" );
_LIT( KWmabase, "WMABASE" );
_LIT( KWmafull, "WMAFULL" );
_LIT( KWmapro, "WMAPRO" );

const TInt KAac_iso_320_max_bitrate = 320000;
const TInt KAac_iso_max_samplerate = 48000;
const TInt KAac_iso_max_bitrate = 576000;

const TInt KWmabase_max_bitrate = 192999;
const TInt KWmafull_max_bitrate = 385000;
const TInt KWmapro_max_bitrate = 1500000;

const TInt KWmabase_max_samplerate = 48000;
const TInt KWmafull_max_samplerate = 48000;
const TInt KWmapro_max_samplerate = 96000;

const TInt KTwo_Channels = 2;
const TInt KSix_Channels = 6;
const TInt KEight_Channels = 8;

// ======== LOCAL FUNCTIONS ========
// NONE

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpAudioSolverBase C++ constructor
// --------------------------------------------------------------------------
//
CUpnpAudioSolverBase::CUpnpAudioSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase Constructor" );
    }

// --------------------------------------------------------------------------
// CUpnpAudioSolverBase::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpAudioSolverBase::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::ConstructL" );
    iWait = new( ELeave ) CActiveSchedulerWait;
    }


// --------------------------------------------------------------------------
// CUpnpAudioSolverBase::NewL
// --------------------------------------------------------------------------
//
CUpnpAudioSolverBase* CUpnpAudioSolverBase::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::NewL" );
    CUpnpAudioSolverBase* self = CUpnpAudioSolverBase::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpAudioSolverBase::NewLC
// --------------------------------------------------------------------------
//
CUpnpAudioSolverBase* CUpnpAudioSolverBase::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::NewLC" );
    CUpnpAudioSolverBase* self = new( ELeave ) CUpnpAudioSolverBase;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpAudioSolverBase destructor
// --------------------------------------------------------------------------
//
CUpnpAudioSolverBase::~CUpnpAudioSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::Destructor" );
    delete iAudioconverter;
    delete iWait;
    }

// --------------------------------------------------------------------------
// From class MUpnpProfiler.
// SupportedProfilesL returns DLNA profiles that are currently supported.
// --------------------------------------------------------------------------
//
TInt CUpnpAudioSolverBase::SupportedProfilesL( 
                                          CDesC16ArrayFlat* aProfiles ) const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
SupportedProfilesL" );
    TInt retval = KErrNone;
    
    if ( !aProfiles ) 
        {
        // Invalid parameter
        retval = KErrArgument;
        }
    else 
        {
        // append all new profiles recognized by this solver
        // do not allow duplicates
        TInt tempPos = KErrNotFound;
        
        if ( aProfiles->Find( KLpcm(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KLpcm() );
            }
        if ( aProfiles->Find( KMp3(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KMp3() );
            }
        if ( aProfiles->Find( KAac_iso(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KAac_iso() );
            }
        if ( aProfiles->Find( KAac_iso_320(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KAac_iso_320() );
            }
        if ( aProfiles->Find( KAac_mult5_iso(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KAac_mult5_iso() );
            }
        if ( aProfiles->Find( KWmabase(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KWmabase() );
            }
        if ( aProfiles->Find( KWmafull(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KWmafull() );
            }
        if ( aProfiles->Find( KWmapro(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KWmapro() );
            }
        }
        
    return retval;
    }
    
// --------------------------------------------------------------------------
// From class MUpnpProfiler.
// ProfileForFileL is for resolving a DLNA profile of a given file. Besides 
// of file name, also mime type of the file is passed as a parameter in 
// order to avoid re-opening the file.
// --------------------------------------------------------------------------
//
HBufC* CUpnpAudioSolverBase::ProfileForFileL( const TDesC& aFilename,
                                              const TDesC8& aMimetype, 
                                              RFile& /*aFile*/ ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::ProfileForFileL" );
    HBufC* retval = NULL;
    
    if ( aMimetype.Compare( KAudioMpeg() ) == 0 ) 
        {
        // Note that MP3X profile is currently not supported.
        retval = HBufC::NewL( KMp3().Length() );
        retval->Des().Append( KMp3() );
        }
    else if ( aMimetype.Compare( KAudioMp4() ) == 0 ) 
        {
        // obtain bitrate and channel count information from the file
        GetAudioFileInformationL( aFilename );

        retval = DetermineMp4ProfileL();
        }
    else if ( aMimetype.Compare( KAudio3gpp() ) == 0 ) 
        {
        // obtain bitrate and channel count information from the file
        GetAudioFileInformationL( aFilename );
        
        retval = Determine3GppProfileL();
        }
    else if ( aMimetype.Left( KAudioL16().Length() ).Compare( 
                                                     KAudioL16() ) == 0 ) 
        {
        retval = HBufC::NewL( KLpcm().Length() );
        retval->Des().Append( KLpcm() );
        }
    else if ( aMimetype.Compare( KAudioWma() ) == 0 ) 
        {
        // obtain bitrate and channel count information from the file
        GetAudioFileInformationL( aFilename );

        retval = DetermineWmaProfileL();
        }
    else if ( aMimetype.Compare( KAudioAdts() ) == 0 ) 
        {
        // no audio/vnd.dlna.adts profiles currently supported
        // obtain bitrate and channel count information from the file
        //GetAudioFileInformationL( aFilename );
        //retval = DetermineAdtsProfileL();
        }
        
    return retval;
    }

// --------------------------------------------------------------------------
// From class MMdaObjectStateChangeObserver.
// MoscoStateChangeEvent callback is called after a call to 
// CMdaAudioConvertUtility::OpenL has completed.
// --------------------------------------------------------------------------
//
void CUpnpAudioSolverBase::MoscoStateChangeEvent( CBase */*aObject*/, 
                                                  TInt /*aPreviousState*/, 
                                                  TInt /*aCurrentState*/, 
                                                  TInt aErrorCode ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
MoscoStateChangeEvent" );
    if( KErrNone == aErrorCode )
        {
            // get bit rate and channel count of the file. Trap because this 
            // method may not leave. TRAP_IGNORE because there is nothing we
            // can do in case of leave.
            TRAP_IGNORE( iBitrate = iAudioconverter->SourceBitRateL() );
            TRAP_IGNORE( iChannelCount = 
                            iAudioconverter->SourceNumberOfChannelsL() );
            TRAP_IGNORE( iSampleRate = 
                            iAudioconverter->SourceSampleRateL() );
        }
        
    // continue execution in ProfileForFileL-method by stopping 
    // ActiveSchedulerWait
    iWait->AsyncStop();
    }

// --------------------------------------------------------------------------
// GetAudioFileInformationL is for resolving audio file attributes by using 
// CMdaAudioConvertUtility. 
// --------------------------------------------------------------------------
//
TInt CUpnpAudioSolverBase::GetAudioFileInformationL( const TDesC& aFilename )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
GetAudioFileInformationL" );
    TInt retval = KErrNone;
    
    // create audioconverter if it does not exist
    if ( !iAudioconverter ) 
        {
        iAudioconverter = CMdaAudioConvertUtility::NewL( *this );
        }
    
    // Create iWait if it does not exist. Create it here rather than after 
    // OpenL-call so that there will be no problems if somehow OpenL calls 
    // MoscoStateChangeEvent-callback before iWait is created and started.
    if ( !iWait ) 
        {
        iWait = new( ELeave ) CActiveSchedulerWait;
        }

    // Open file specified in aFilename. This is an asynchronic operation. 
    // Calls MoscoStateChangeEvent callback after completed.
    TMdaFileClipLocation sinkFile( KTargetAudioFile() );
    TMdaRawAudioClipFormat  sinkformat;
    iAudioconverter->OpenL( aFilename, &sinkFile, &sinkformat );

    // start CActiveSchedulerWait which is completed in MoscoStateChangeEvent
    iWait->Start();

    // openL + MoscoStateChangeEvent callback executed now
    delete iAudioconverter;
    iAudioconverter = NULL;
    delete iWait;
    iWait = NULL;

    // delete temporary file
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Delete( KTargetAudioFile() ) );
    CleanupStack::PopAndDestroy( &fs );
    
    return retval;
    }

// --------------------------------------------------------------------------
// Determine3GppProfileL is for determining profile for 3gpp audio files
// --------------------------------------------------------------------------
//
HBufC* CUpnpAudioSolverBase::Determine3GppProfileL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
Determine3GppProfileL" );
    HBufC* retval = NULL;
    
    if ( iBitrate <= KAac_iso_320_max_bitrate && 
         iSampleRate <= KAac_iso_max_samplerate && 
         iChannelCount <= KTwo_Channels ) 
        {
        retval = HBufC::NewL( KAac_iso_320().Length() );
        retval->Des().Append( KAac_iso_320() );
        }
    else if ( iSampleRate <= KAac_iso_max_samplerate && 
              iChannelCount <= KTwo_Channels )
        {
        retval = HBufC::NewL( KAac_iso().Length() );
        retval->Des().Append( KAac_iso() ); 
        }
    else if ( iSampleRate <= KAac_iso_max_samplerate && 
              iChannelCount <= KSix_Channels )
        {
        retval = HBufC::NewL( KAac_mult5_iso().Length() );
        retval->Des().Append( KAac_mult5_iso() ); 
        }    
    
    return retval;
    }



// --------------------------------------------------------------------------
// DetermineWmaProfileL is for determining profile for wma files
// --------------------------------------------------------------------------
//
HBufC* CUpnpAudioSolverBase::DetermineWmaProfileL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
DetermineWmaProfileL" );
    HBufC* retval = NULL;

    if ( iBitrate <= KWmabase_max_bitrate && 
         iSampleRate <= KWmabase_max_samplerate && 
         iChannelCount <= KTwo_Channels )
        {
        retval = HBufC::NewL( KWmabase().Length() );
        retval->Des().Append( KWmabase() );   
        }
    else if ( iBitrate <= KWmafull_max_bitrate && 
              iSampleRate <= KWmafull_max_samplerate && 
              iChannelCount <= KTwo_Channels ) 
        {
        retval = HBufC::NewL( KWmafull().Length() );
        retval->Des().Append( KWmafull() );
        }
    else if ( iBitrate <= KWmapro_max_bitrate && 
              iSampleRate <= KWmapro_max_samplerate && 
              iChannelCount <= KEight_Channels ) 
        {
        retval = HBufC::NewL( KWmapro().Length() );
        retval->Des().Append( KWmapro() );
        }
        
    return retval;
}

// --------------------------------------------------------------------------
// DetermineMp4ProfileL is for determining profile for mp4 audio files
// --------------------------------------------------------------------------
//
HBufC* CUpnpAudioSolverBase::DetermineMp4ProfileL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAudioSolverBase::\
DetermineMp4ProfileL" );
    HBufC* retval = NULL;
    
    if ( iBitrate <= KAac_iso_320_max_bitrate && 
         iChannelCount <= KTwo_Channels ) 
        {
        retval = HBufC::NewL( KAac_iso_320().Length() );
        retval->Des().Append( KAac_iso_320() );            
        }
    else if ( iBitrate <= KAac_iso_max_bitrate && 
              iChannelCount <= KTwo_Channels )
        {
        retval = HBufC::NewL( KAac_iso().Length() );
        retval->Des().Append( KAac_iso() ); 
        }
        
    return retval;
}


// end of file
