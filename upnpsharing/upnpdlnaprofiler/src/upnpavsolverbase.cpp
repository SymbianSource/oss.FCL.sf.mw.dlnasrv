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
* Description:      Implementation of base DLNA profile resolver class for av 
*                files.
*
*/






// includes
#include <badesca.h> // CDesC16ArrayFlat
#include <3gplibrary/mp4lib.h>
#include "upnpavsolverbase.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"

// constants
_LIT8( KVideoMp4, "video/mp4" );

_LIT( KMpeg4_p2_mp4_sp_l2_aac,      "MPEG4_P2_MP4_SP_L2_AAC" );
_LIT( KMpeg4_p2_mp4_sp_aac,         "MPEG4_P2_MP4_SP_AAC" );
_LIT( KMpeg4_p2_mp4_sp_vga_aac,     "MPEG4_P2_MP4_SP_VGA_AAC" );
_LIT( KMpeg4_p2_mp4_sp_l5_aac,      "MPEG4_P2_MP4_SP_L5_AAC" );

_LIT( KAvc_mp4_bl_cif15_aac,        "AVC_MP4_BL_CIF15_AAC" );
_LIT( KAvc_mp4_bl_cif15_aac_520,    "AVC_MP4_BL_CIF15_AAC_520" );

const TUint32 KSimpleProfileLevel2 = 0x02;
const TUint32 KSimpleProfileLevel3 = 0x03;
const TUint32 KSimpleProfileLevel4 = 0x04;
const TUint32 KSimpleProfileLevel5 = 0x05;


const mp4_u32 KMaxXResolutionCif = 352;
const mp4_u32 KMaxYResolutionCif = 288;
const mp4_u32 KMaxXResolutionVga = 640;
const mp4_u32 KMaxYResolutionVga = 480;

//const TInt  KBitrateAverageToMaxFactor = 20;
//const mp4_u32 KMaxBitrateCif520 = 520;
//const mp4_u32 KMaxBitrateCif = 384;
//const TUint32 KAdvancedSimpleProfileLevel1 = 0x91;
//const TUint32 KAdvancedSimpleProfileLevel2 = 0x92;
//const TUint32 KAdvancedSimpleProfileLevel3 = 0x93;
//const TUint32 KAdvancedSimpleProfileLevel4 = 0x94;
//const TUint32 KAvcBaseline1_2 = 12;

/*
===========================================================================
Codec-specific details are explained here
(for the part that is currently required by upnp framework)

MPEG4 part2
------------
see ISO/IEC 14496-2, chapter 6.2.2 and annex G
(http://akuvian.org/src/x264/ISO-IEC-14496-2_2001_MPEG4_Visual.pdf.gz)

32 bit: visual_object_sequence_start_code (not interesting)
8 bit: profile_and_level_indication, where:
   0x02 = simple profile, level 2
   0x03 = simple profile, level 3

   0x04 = simple profile, level 4a
   0x05 = simple profile, level 5
   
   0x91 = advanced simple profile, level 1
   0x92 = advanced simple profile, level 2
   0x93 = advanced simple profile, level 3
   0x94 = advanced simple profile, level 4
   
...

MPEG4 part 10 (AVC)
-------------------
see ISO/IEC 14496-15, chapter 5.2.4.1.1
(http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=43178)

8 bit: version (not interesting)
8 bit: AVC profile indication, where
   0x42 = AVC baseline
   0x4D = AVC main
   0x58 = AVC extended
8 bit: compatibility flags (not interesting)
8 bit: AVC level indication, where
   10 = baseline level 1.0
   11 = baseline level 1.1
   12 = baseline level 1.2
   13 = baseline level 1.3
   20 = baseline level 2.0
   21 = baseline level 2.1
   22 = baseline level 2.2
   30 = baseline level 3.0
...

===========================================================================
*/


// --------------------------------------------------------------------------
// CUpnpAvSolverBase C++ constructor
// --------------------------------------------------------------------------
//
CUpnpAvSolverBase::CUpnpAvSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase constructor" );
    }
    
// --------------------------------------------------------------------------
// CUpnpAvSolverBase::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpAvSolverBase::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::ConstructL" );
    }


// --------------------------------------------------------------------------
// CUpnpAvSolverBase::NewL
// --------------------------------------------------------------------------
//
CUpnpAvSolverBase* CUpnpAvSolverBase::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::NewL" );
    CUpnpAvSolverBase* self = CUpnpAvSolverBase::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpAvSolverBase::NewLC
// --------------------------------------------------------------------------
//
CUpnpAvSolverBase* CUpnpAvSolverBase::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::NewLC" );
    CUpnpAvSolverBase* self = new( ELeave ) CUpnpAvSolverBase;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpAvSolverBase destructor
// --------------------------------------------------------------------------
//
CUpnpAvSolverBase::~CUpnpAvSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase destructor" );
    }

// --------------------------------------------------------------------------
// From class MUpnpProfiler.
// SupportedProfilesL returns DLNA profiles that are currently supported.
// --------------------------------------------------------------------------
//
TInt CUpnpAvSolverBase::SupportedProfilesL( 
                                        CDesC16ArrayFlat* aProfiles ) const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::SupportedProfilesL" );
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

        if ( aProfiles->Find( KMpeg4_p2_mp4_sp_l2_aac(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KMpeg4_p2_mp4_sp_l2_aac() );
            }
        if ( aProfiles->Find( KMpeg4_p2_mp4_sp_aac(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KMpeg4_p2_mp4_sp_aac() );
            }
        if ( aProfiles->Find( KMpeg4_p2_mp4_sp_vga_aac(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KMpeg4_p2_mp4_sp_vga_aac() );
            }
        if ( aProfiles->Find( KMpeg4_p2_mp4_sp_l5_aac(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KMpeg4_p2_mp4_sp_l5_aac() );
            }
        if ( aProfiles->Find( KAvc_mp4_bl_cif15_aac(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KAvc_mp4_bl_cif15_aac() );
            }
        if ( aProfiles->Find( KAvc_mp4_bl_cif15_aac_520(), 
                              tempPos, 
                              ECmpFolded ) ) 
            {
            aProfiles->AppendL( KAvc_mp4_bl_cif15_aac_520() );
            }
        }

    return retval;
    }
    
// --------------------------------------------------------------------------
// From class MUpnpProfiler.
// ProfileForFileL is for resolving a DLNA profile of a given file. Besides 
// of file name, also mime type of the file is passed as a parameter in order
// to avoid re-opening the file.
// --------------------------------------------------------------------------
//
HBufC* CUpnpAvSolverBase::ProfileForFileL( const TDesC& /*aFilename*/,
                                           const TDesC8& aMimetype, 
                                           RFile& aFile )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::\
ProfileForFileL" );
    HBufC* retval = NULL;


    
    if ( aMimetype.Compare( KVideoMp4() ) == 0 ) 
        {
        GetVideoFileInformationL( aFile );



        if ( iVideoType == MP4_TYPE_MPEG4_VIDEO &&
             iAudioType == MP4_TYPE_MPEG4_AUDIO )
            {
            // MPEG4 level 2

            TUint32 level = iCodecInfo.iData[1] & 0x000000FF;
            
            __LOG1( "[CUpnpAvSolverBase] CUpnpAvSolverBase::\
            GetVideoFileInformationL level %d", level );

            if ( level == KSimpleProfileLevel2 )
                {
                // Simple profile level 2
                retval = HBufC::NewL( KMpeg4_p2_mp4_sp_l2_aac().Length() );
                retval->Des().Append( KMpeg4_p2_mp4_sp_l2_aac() ); 
                }
            else if ( level == KSimpleProfileLevel3 )
                {
                // Simple profile level 3
                if ( iVideoResolutionX <= KMaxXResolutionCif &&
                    iVideoResolutionY <= KMaxYResolutionCif )
                    {
                    // resolution below CIF standard
                    retval = HBufC::NewL( KMpeg4_p2_mp4_sp_aac().Length() );
                    retval->Des().Append( KMpeg4_p2_mp4_sp_aac() ); 
                    }
                else if ( iVideoResolutionX <= KMaxXResolutionVga &&
                    iVideoResolutionY <= KMaxYResolutionVga )
                    {
                    // resolution below VGA standard
                    retval = HBufC::NewL(
                        KMpeg4_p2_mp4_sp_vga_aac().Length() );
                    retval->Des().Append( KMpeg4_p2_mp4_sp_vga_aac() ); 
                    }
                }

            else if ( level == KSimpleProfileLevel4 )
                {
                // Simple profile level 4a
                retval = HBufC::NewL( KMpeg4_p2_mp4_sp_vga_aac().Length() );
                retval->Des().Append( KMpeg4_p2_mp4_sp_vga_aac() ); 
                }
            
            else if ( level == KSimpleProfileLevel5 )
                {
                // Simple profile level 5
                retval = HBufC::NewL( KMpeg4_p2_mp4_sp_l5_aac().Length() );
                retval->Des().Append( KMpeg4_p2_mp4_sp_l5_aac() ); 
                }                
                
/*

Not needed yet:

            else if ( level == KAdvancedSimpleProfileLevel1 ||
                level == KAdvancedSimpleProfileLevel2 ||
                level == KAdvancedSimpleProfileLevel3 ||
                level == KAdvancedSimpleProfileLevel4 )
                {
                // Advanced simple profile (levels 1-4)
                retval = HBufC::NewL( KMpeg4_p2_mp4_sp_l5_aac().Length() );
                retval->Des().Append( KMpeg4_p2_mp4_sp_l5_aac() ); 
                }
*/
                
            }
            

/*
  Temporary solution, based on AVC video resolution. 
  iLevel should be used instead, see above Mp4 P2.
*/
        else if ( iVideoType == MP4_TYPE_AVC_PROFILE_BASELINE &&
            iAudioType == MP4_TYPE_MPEG4_AUDIO )
            {
            // MPEG4 part 10 (AVC)
            
            	if (iVideoResolutionX <= KMaxXResolutionCif && 
            		iVideoResolutionY <= KMaxYResolutionCif )
            	{
                    retval = HBufC::NewL( KAvc_mp4_bl_cif15_aac_520().Length() );
                    retval->Des().Append( KAvc_mp4_bl_cif15_aac_520() );            		
            	}
        	
            }
            
            

        }
           
    return retval;
    }


// --------------------------------------------------------------------------
// GetVideoFileInformationL is for resolving audio file attributes by using 
// CMdaAudioConvertUtility. 
// --------------------------------------------------------------------------
//
TInt CUpnpAvSolverBase::GetVideoFileInformationL( RFile& aFile )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpAvSolverBase::\
GetVideoFileInformationL" );
    TInt retval = KErrNone;

    // video description
    mp4_u32 videolength;
    mp4_u32 videotimescale;
    // audio params
    mp4_u32 audiolength = 0;
    mp4_u8 audioframes = 0;
    mp4_u32 audiotimescale = 0;
    mp4_u32 audiobitrate = 0;
    // stream params
    mp4_u32 streamsize = 0;

    // reset old values 
    iStreamAverageBitrate = 0;

    MP4Handle myMp4Handle;
    
    // try open mp4 file handle
    MP4Err openerr = MP4ParseOpenFileHandle( &myMp4Handle, &aFile );
    if ( openerr == MP4_OK )
        {
        MP4Err requesterr = MP4ParseRequestVideoDescription(
                                            myMp4Handle,
                                            &videolength,
                                            &iVideoFramerate,
                                            &iVideoType,
                                            &iVideoResolutionX,
                                            &iVideoResolutionY,
                                            &videotimescale );
        if ( requesterr != MP4_OK )
            {
            MP4ParseClose( myMp4Handle );
            User::Leave( KErrGeneral );
            }


        requesterr = MP4ParseRequestAudioDescription( 
                                            myMp4Handle, 
                                            &audiolength, 
                                            &iAudioType,
                                            &audioframes, 
                                            &audiotimescale, 
                                            &audiobitrate );
        if ( requesterr != MP4_OK )
            {
            MP4ParseClose( myMp4Handle );
            User::Leave( KErrGeneral );
            }

        requesterr = MP4ParseRequestStreamDescription(
                                            myMp4Handle, 
                                            &streamsize, 
                                            &iStreamAverageBitrate );


        if ( requesterr != MP4_OK )
            {
            MP4ParseClose( myMp4Handle );
            User::Leave( KErrGeneral );
            }

        TMP4DecoderSpecificInfo iDecoderInfo;
        mp4_u32 decoderInfoSize;

        requesterr = MP4ParseReadVideoDecoderSpecificInfo(
                                            myMp4Handle,
                                            (mp4_u8*)&iDecoderInfo,
                                            sizeof( iDecoderInfo ),
                                            &decoderInfoSize );
       

		iCodecInfo = iDecoderInfo;
        // close mp4 file handle
        MP4Err closeerr = MP4ParseClose( myMp4Handle );
        if ( closeerr ) 
            {
            User::Leave( KErrGeneral );
            }
        }
    else 
        {
        // can not open file
        User::Leave( KErrBadHandle );
        }
    
    return retval;
    }

    
// end of file
