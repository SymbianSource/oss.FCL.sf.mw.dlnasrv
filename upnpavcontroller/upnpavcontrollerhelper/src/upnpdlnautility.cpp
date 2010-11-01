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
#include <e32des8.h> 
#include "upnpdlnautility.h"

// CONSTANTS

#define CHARTOVOID(chr) (const void*)chr

/**
 * pointer array for mime types 
 */
const void* const KMimeTypes[]=
    {
    CHARTOVOID("video/mp4"),
    CHARTOVOID("video/3gpp"),
    CHARTOVOID("audio/mpeg"),
    CHARTOVOID("audio/x-ms-wma"),
    CHARTOVOID("audio/3gpp"),
    CHARTOVOID("audio/mp4"),
    CHARTOVOID("audio/vnd.dlna.adts"),
    CHARTOVOID("audio/x-wav"),
    CHARTOVOID("audio/x-aac"),
    CHARTOVOID("audio/aac"),
    CHARTOVOID("audio/x-m4a"),
    CHARTOVOID("image/jpeg"),
    CHARTOVOID("image/png"),
    CHARTOVOID("image/gif"),
    CHARTOVOID("image/bmp")
    };

/**
 * pointer array for dlna types 
 */
const void* const KDlnaTypes[]=
    {
    CHARTOVOID( "AVC_MP4_BL_CIF15_AAC_520" ),
    CHARTOVOID( "MPEG4_H263_3GPP_P3_L10_AMR" ), 
    CHARTOVOID( "MPEG4_P2_3GPP_SP_L0B_AMR" ), 
    CHARTOVOID( "MPEG4_P2_MP4_ASP_AAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_ASP_HEAAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_ASP_L4_SO_AAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_ASP_L4_SO_HEAAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_SP_VGA_AAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_SP_VGA_HEAAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_SP_L2_AAC" ), 
    CHARTOVOID( "MPEG4_P2_MP4_SP_AAC" ),    
    CHARTOVOID( "MPEG4_P2_3GPP_SP_L0B_AAC"),
    CHARTOVOID( "AVC_MP4_BL_CIF15_AAC" ), 
    CHARTOVOID( "AVC_MP4_BL_CIF15_AAC_LTP" ), 
    CHARTOVOID( "AVC_MP4_BL_CIF15_AAC_LTP_520" ), 
    CHARTOVOID( "AVC_MP4_BL_CIF30_AAC_940" ), 
    CHARTOVOID( "AVC_MP4_BL_L12_CIF15_HEAAC" ),
    CHARTOVOID( "AVC_TS_BL_CIF15_AAC" ),   
    CHARTOVOID( "AVC_MP4_MP_HD_720p_AAC" ),
    CHARTOVOID( "AVC_MP4_HP_HD_AAC" ),    
    // Audio profiles that can be played on device 
    CHARTOVOID( "AAC_ISO" ), 
    CHARTOVOID( "AAC_ISO_320" ),
    CHARTOVOID( "AAC_ADTS" ), 
    CHARTOVOID( "AAC_ADTS_320" ), 
    CHARTOVOID( "AAC_MULT5_ISO" ),
    CHARTOVOID( "HEAAC_L2_ISO" ), 
    CHARTOVOID( "HEAAC_L2_ISO_320" ), 
    CHARTOVOID( "HEAAC_L2_ADTS" ), 
    CHARTOVOID( "HEAAC_L2_ADTS_320" ), 
    CHARTOVOID( "HEAAC_MULT5_ISO" ), 
    CHARTOVOID( "HEAAC_L3_ADTS" ), 
    CHARTOVOID( "AMR_3GPP" ), 
    CHARTOVOID( "AMR_WBplus" ), 
    CHARTOVOID( "MP3" ),
    CHARTOVOID( "MP3X" ), 
    CHARTOVOID( "WMABASE" ),
    CHARTOVOID( "WMAFULL" ),    
    // Image profiles that can be played on device 
    CHARTOVOID( "JPEG_LRG" ), 
    CHARTOVOID( "JPEG_MED" ), 
    CHARTOVOID( "JPEG_SM" ),
    CHARTOVOID( "PNG_LRG" ),
    CHARTOVOID( "JPEG_LRG_ICO" ),
    CHARTOVOID( "JPEG_SM_ICO" ),
    CHARTOVOID( "JPEG_TN" ),
    CHARTOVOID( "PNG_TN" ),
    CHARTOVOID( "PNG_SM_ICO" ),
    CHARTOVOID( "PNG_LRG_ICO" )
    };

const TStringTable UPnPDlnaUtility::iMimeTypes = 
    {
    15, 
    KMimeTypes, 
    EFalse
    };

const TStringTable UPnPDlnaUtility::iDlnaTypes = 
    {
    47, 
    KDlnaTypes, 
    EFalse
    };

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
            
            for ( TInt i(0) ; i < UPnPDlnaUtility::iDlnaTypes.iCount ; ++i )
                {
                array->AppendL( TPtrC8( 
                (const TUint8*)(UPnPDlnaUtility::iDlnaTypes.iTable[i]) ) );
                }
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
    return UPnPDlnaUtility::IsSupported(iMimeTypes,aMime);
    }
    
// --------------------------------------------------------------------------
// UPnPDlnaUtility::IsSupportedDlnaProfile
// Returns ETrue if aProfile is such a DLNA profile for which playback on 
// device might be supported.
// -------------------------------------------------------------------------- 
EXPORT_C TBool UPnPDlnaUtility::IsSupportedDlnaProfile( 
    const TDesC8& aProfile ) 
    {
    return UPnPDlnaUtility::IsSupported(iDlnaTypes,aProfile);  
    }

//---------------------------------------------------------------------------
// IsSupported
//---------------------------------------------------------------------------
TBool UPnPDlnaUtility::IsSupported( const TStringTable& aTable 
                                  , const TDesC8& aType )
    {
    TBool ret( EFalse );
    for ( TInt i(0) ; i < aTable.iCount ; ++i )
        {
        if ( aType == TPtrC8( (const TUint8*)(aTable.iTable[i]) ) )
            {
            ret = ETrue;
            i = aTable.iCount;
            }
        }
    return ret;
    }

// End of file

