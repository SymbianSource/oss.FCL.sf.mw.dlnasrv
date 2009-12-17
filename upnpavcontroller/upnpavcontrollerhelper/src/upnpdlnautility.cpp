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
* Description:      Utility for working with files in upnp context
*
*/






// INCLUDE FILES
#include <e32std.h>

#include "upnpdlnautility.h"

// CONSTANTS

// LITERALS
// Video mimetypes that can be played on device
_LIT8( KVideoMp4, "video/mp4" );
_LIT8( KVideo3Gpp, "video/3gpp" );

// Audio mimetypes that can be played on device
_LIT8( KAudioMpeg, "audio/mpeg" );
_LIT8( KAudioWma, "audio/x-ms-wma" );
_LIT8( KAudio3Gpp, "audio/3gpp" );
_LIT8( KAudioMp4, "audio/mp4" );
_LIT8( KAudioAdts, "audio/vnd.dlna.adts" );
_LIT8( KAudioWav, "audio/x-wav" );
_LIT8( KAudioXAac, "audio/x-aac" );
_LIT8( KAudioAac, "audio/aac" );
_LIT8( KAudioM4a, "audio/x-m4a" );

// Image mimetypes that can be played on device
_LIT8( KImageJpeg, "image/jpeg" );
_LIT8( KImagePng, "image/png" );
_LIT8( KImageGif, "image/gif" );
_LIT8( KImageBmp, "image/bmp" );

// Video profiles that can be played on device
_LIT8( KAvcMp4BlCif15Aac520, "AVC_MP4_BL_CIF15_AAC_520" );
_LIT8( KMpeg4H2633gppP3L10Amr, "MPEG4_H263_3GPP_P3_L10_AMR" ); 
_LIT8( KMpeg4P23gppSpL0bAmr, "MPEG4_P2_3GPP_SP_L0B_AMR" ); 
_LIT8( KMpeg4P2Mp4AspAac, "MPEG4_P2_MP4_ASP_AAC" ); 
_LIT8( KMpeg4P2Mp4AspHeaac, "MPEG4_P2_MP4_ASP_HEAAC" ); 
_LIT8( KMpeg4P2Mp4AspL4SoAac, "MPEG4_P2_MP4_ASP_L4_SO_AAC" ); 
_LIT8( KMpeg4P2Mp4AspL4SoHeaac, "MPEG4_P2_MP4_ASP_L4_SO_HEAAC" ); 
_LIT8( KMpeg4P2Mp4SpVgaAac, "MPEG4_P2_MP4_SP_VGA_AAC" ); 
_LIT8( KMpeg4P2Mp4SpVgaHeaac, "MPEG4_P2_MP4_SP_VGA_HEAAC" ); 
_LIT8( KMpeg4P2Mp4SpL2Aac, "MPEG4_P2_MP4_SP_L2_AAC" ); 
_LIT8( KMpeg4P2Mp4spAac, "MPEG4_P2_MP4_SP_AAC" );

_LIT8( KMpeg4P23gppSpL0bAac, "MPEG4_P2_3GPP_SP_L0B_AAC");
_LIT8( KAvcMp4BlCif15Aac, "AVC_MP4_BL_CIF15_AAC" ); 
_LIT8( KAvcMp4BlCif15AacLtp, "AVC_MP4_BL_CIF15_AAC_LTP" ); 
_LIT8( KAvcMp4BlCif15AacLtp520, "AVC_MP4_BL_CIF15_AAC_LTP_520" ); 
_LIT8( KAvcMp4BlCif30Aac940, "AVC_MP4_BL_CIF30_AAC_940" ); 
_LIT8( KAvcMp4BlL12Cif15Heaac, "AVC_MP4_BL_L12_CIF15_HEAAC" );
_LIT8( KAvcTSBlCif15Aac, "AVC_TS_BL_CIF15_AAC" );

// Audio profiles that can be played on device
_LIT8( KAacIso, "AAC_ISO" ); 
_LIT8( KAacIso320, "AAC_ISO_320" );
_LIT8( KAacAdts, "AAC_ADTS" ); 
_LIT8( KAacAdts320, "AAC_ADTS_320" ); 
_LIT8( KAacMult5Iso, "AAC_MULT5_ISO" );
_LIT8( KHeaacL2Iso, "HEAAC_L2_ISO" ); 
_LIT8( KHeaacL2Iso320, "HEAAC_L2_ISO_320" ); 
_LIT8( KHeaacL2Adts, "HEAAC_L2_ADTS" ); 
_LIT8( KHeaacL2Adts320, "HEAAC_L2_ADTS_320" ); 
_LIT8( KHeaacMuLt5Iso, "HEAAC_MULT5_ISO" ); 
_LIT8( KHeaacL3ADTS, "HEAAC_L3_ADTS" ); 
_LIT8( KAmr3gpp, "AMR_3GPP" ); 
_LIT8( KAmrWbplus, "AMR_WBplus" ); 
_LIT8( KMp3, "MP3" );
_LIT8( KMp3x, "MP3X" ); 
_LIT8( KWmaBase, "WMABASE" );
_LIT8( KWmaFull, "WMAFULL" );

// Image profiles that can be played on device
_LIT8( KJpegLrg, "JPEG_LRG" ); 
_LIT8( KJpegMed, "JPEG_MED" ); 
_LIT8( KJpegSm, "JPEG_SM" );
_LIT8( KPngLrg, "PNG_LRG" );
_LIT8( KJpegLrgIco, "JPEG_LRG_ICO" );
_LIT8( KJpegSmIco, "JPEG_SM_ICO" );
_LIT8( KJpegTn, "JPEG_TN" );
_LIT8( KPngTn, "PNG_TN" );
_LIT8( KPngSmIco, "PNG_SM_ICO" );
_LIT8( KPngLrgIco, "PNG_LRG_ICO" );


// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UPnPDlnaUtility::GetSupportedProfilesL
// Returns Supported dlna profiles, Only player mode supported.
// --------------------------------------------------------------------------
EXPORT_C CDesC8Array& UPnPDlnaUtility::GetSupportedProfilesL( 
    const TDlnaMode aMode )
    {
    CDesC8Array* array = new (ELeave) CDesC8ArrayFlat( 10 );
    CleanupStack::PushL( array );
    // At the m
    switch( aMode )
        {         
        case EDMPMode:
            {
            // Video profiles that can be played on device
            array->AppendL( KAvcMp4BlCif15Aac520() );
            array->AppendL( KMpeg4H2633gppP3L10Amr() );
            array->AppendL( KMpeg4P23gppSpL0bAmr() );
            array->AppendL( KMpeg4P2Mp4AspAac() );
            array->AppendL( KMpeg4P2Mp4AspHeaac() );
            array->AppendL( KMpeg4P2Mp4AspL4SoAac() );
            array->AppendL( KMpeg4P2Mp4AspL4SoHeaac() );
            array->AppendL( KMpeg4P2Mp4SpVgaAac() );
            array->AppendL( KMpeg4P2Mp4AspL4SoHeaac() );
            array->AppendL( KMpeg4P2Mp4SpVgaAac() );
            array->AppendL( KMpeg4P2Mp4SpVgaHeaac() );
            array->AppendL( KMpeg4P2Mp4SpL2Aac() );
            array->AppendL( KMpeg4P2Mp4spAac() );
            
            array->AppendL( KMpeg4P23gppSpL0bAac() );
            array->AppendL( KAvcMp4BlCif15Aac() );
            array->AppendL( KAvcMp4BlCif15AacLtp() );
            array->AppendL( KAvcMp4BlCif15AacLtp520() );
            array->AppendL( KAvcMp4BlCif30Aac940() );
            array->AppendL( KAvcMp4BlL12Cif15Heaac() );
            array->AppendL( KAvcTSBlCif15Aac() );            
            
            // Audio profiles that can be played on device
            array->AppendL( KAacIso() );
            array->AppendL( KAacIso320() );
            array->AppendL( KAacAdts() );
            array->AppendL( KAacAdts320() );
            array->AppendL( KAacMult5Iso() );
            array->AppendL( KHeaacL2Iso() );
            array->AppendL( KHeaacL2Iso320() );
            array->AppendL( KHeaacL2Adts() );
            array->AppendL( KHeaacL2Adts320() );
            array->AppendL( KAmr3gpp() );
            array->AppendL( KAmrWbplus() );
            array->AppendL( KMp3() );
            array->AppendL( KMp3x() );
            array->AppendL( KWmaBase() );
            array->AppendL( KWmaFull() );
            
            // Image profiles that can be played on device
            array->AppendL( KJpegLrg() );
            array->AppendL( KJpegMed() );
            array->AppendL( KJpegSm() );            
            array->AppendL( KPngLrg() ); //fix for EFCG-7E2DKU            
            array->AppendL( KJpegLrgIco() );
            array->AppendL( KJpegSmIco() );
            array->AppendL( KJpegTn() );
            array->AppendL( KPngTn() );
            array->AppendL( KPngSmIco() );
            array->AppendL( KPngLrgIco() );

            break;
            }
        case EDMSMode:
        case EDMUMode:      
        case EDMDMode:    
        default:            
            {
            User::Leave( KErrNotSupported );
            break;
            }             
        }
    CleanupStack::Pop( array );        
    return *array;        
    }
    
// --------------------------------------------------------------------------
// UPnPDlnaUtility::IsSupportedMimeType
// Returns ETrue if aMime is such a mime type for which playback on device 
// might be supported.
// -------------------------------------------------------------------------- 
EXPORT_C TBool UPnPDlnaUtility::IsSupportedMimeType( const TDesC8& aMime ) 
    {
    TBool retval = EFalse;
    
    if ( aMime.Compare( KVideoMp4() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KVideo3Gpp() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KAudioMpeg() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KAudioWma() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KAudio3Gpp() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KAudioMp4() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KAudioAdts() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KImageJpeg() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aMime.Compare( KImagePng() ) == 0 )
        {
        retval = ETrue;
        } 
    else if ( aMime.Compare ( KAudioWav() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare ( KAudioXAac() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare ( KAudioAac() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare ( KAudioM4a() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare ( KImageGif() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aMime.Compare ( KImageBmp() ) == 0 ) 
        {
        retval = ETrue;
        }

    return retval;
    }
    
// --------------------------------------------------------------------------
// UPnPDlnaUtility::IsSupportedDlnaProfile
// Returns ETrue if aProfile is such a DLNA profile for which playback on 
// device might be supported.
// -------------------------------------------------------------------------- 
EXPORT_C TBool UPnPDlnaUtility::IsSupportedDlnaProfile( 
    const TDesC8& aProfile ) 
    {
    TBool retval = EFalse;
    
    if ( aProfile.Compare( KAvcMp4BlCif15Aac520() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4H2633gppP3L10Amr() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P23gppSpL0bAmr() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4AspAac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4AspHeaac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4AspL4SoAac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4AspL4SoHeaac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4SpVgaAac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4SpVgaHeaac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4SpL2Aac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMpeg4P2Mp4spAac() ) == 0 )
        {
        retval = ETrue;
        }
    else if( aProfile.Compare( KMpeg4P23gppSpL0bAac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcMp4BlCif15Aac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcMp4BlCif15AacLtp() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcMp4BlCif15AacLtp520() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcMp4BlCif30Aac940() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcMp4BlL12Cif15Heaac() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAvcTSBlCif15Aac() ) == 0 )
        {
        retval = ETrue;
        }
     
    else if ( aProfile.Compare( KAacIso() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAacIso320() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAacAdts() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAacAdts320() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAacMult5Iso() ) == 0 ) 
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KHeaacL2Iso() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KHeaacL2Iso320() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KHeaacL2Adts() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KHeaacL2Adts320() ) == 0 )
        {
        retval = ETrue;
        }
	else if ( aProfile.Compare( KHeaacMuLt5Iso() ) == 0 )
        {
        retval = ETrue;
        }
	else if ( aProfile.Compare( KHeaacL3ADTS() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAmr3gpp() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KAmrWbplus() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMp3() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KMp3x() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KWmaBase() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KWmaFull() ) == 0 )
        {
        retval = ETrue;
        }

    else if ( aProfile.Compare( KJpegLrg() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KJpegMed() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KJpegSm() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KPngLrg() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KJpegLrgIco() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KJpegSmIco() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KJpegTn() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KPngTn() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KPngSmIco() ) == 0 )
        {
        retval = ETrue;
        }
    else if ( aProfile.Compare( KPngLrgIco() ) == 0 )
        {
        retval = ETrue;
        }

    return retval;    
    }

// End of file

