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
* Description:      Implementation of base DLNA profile resolver class for image 
*                files.
*
*/






// system includes
#include <imageconversion.h>
#include <caf/caftypes.h> // ContentAccess
#include <badesca.h> // CDesC16ArrayFlat

// user includes
#include "upnpimagesolverbase.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"

// constants
_LIT8( KImageJpeg, "image/jpeg" );
_LIT( KJpeg_sm, "JPEG_SM" );
_LIT( KJpeg_med, "JPEG_MED" );
_LIT( KJpeg_lrg, "JPEG_LRG" );

_LIT( KJpeg_tn, "JPEG_TN" );
_LIT( KJpeg_lrg_ico, "JPEG_LRG_ICO" );
_LIT( KJpeg_sm_ico, "JPEG_SM_ICO" );

_LIT8( KImagePng, "image/png" );
_LIT( KPngLrg, "PNG_LRG" );


const TInt KJpeg_smico_max_width = 48;
const TInt KJpeg_smico_max_height = 48;
const TInt KJpeg_lrgico_max_width = 120;
const TInt KJpeg_lrgico_max_height = 120;
const TInt KJpeg_sm_max_width = 640;
const TInt KJpeg_sm_max_height = 480;
const TInt KJpeg_med_max_width = 1024;
const TInt KJpeg_med_max_height = 768;
const TInt KJpeg_lrg_max_width = 4096;
const TInt KJpeg_lrg_max_height = 4096;
const TInt KPng_lrg_max_width = 4096;
const TInt KPng_lrg_max_height = 4096;

// ======== LOCAL FUNCTIONS ========
// NONE

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpImageSolverBase C++ constructor
// --------------------------------------------------------------------------
//
CUpnpImageSolverBase::CUpnpImageSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase constructor" );
    }
    
// --------------------------------------------------------------------------
// CUpnpImageSolverBase::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpImageSolverBase::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase::ConstructL" );
    }


// --------------------------------------------------------------------------
// CUpnpImageSolverBase::NewL
// --------------------------------------------------------------------------
//
CUpnpImageSolverBase* CUpnpImageSolverBase::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase::NewL" );
    CUpnpImageSolverBase* self = CUpnpImageSolverBase::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpImageSolverBase::NewLC
// --------------------------------------------------------------------------
//
CUpnpImageSolverBase* CUpnpImageSolverBase::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase::NewLC" );
    CUpnpImageSolverBase* self = new( ELeave ) CUpnpImageSolverBase;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpImageSolverBase destructor
// --------------------------------------------------------------------------
//
CUpnpImageSolverBase::~CUpnpImageSolverBase()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase destructor" );
    }

// --------------------------------------------------------------------------
// From class MUpnpProfiler.
// SupportedProfilesL returns DLNA profiles that are currently supported.
// --------------------------------------------------------------------------
//
TInt CUpnpImageSolverBase::SupportedProfilesL( 
                                        CDesC16ArrayFlat* aProfiles ) const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase::\
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
        
        if ( aProfiles->Find( KJpeg_sm(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_sm() );
            }
        if ( aProfiles->Find( KJpeg_med(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_med() );
            }
        if ( aProfiles->Find( KJpeg_lrg(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_lrg() );
            }
        if ( aProfiles->Find( KPngLrg(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KPngLrg() );
            }
        if ( aProfiles->Find( KJpeg_tn(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_tn() );
            }            
        if ( aProfiles->Find( KJpeg_lrg_ico(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_lrg_ico() );
            }            
        if ( aProfiles->Find( KJpeg_sm_ico(), tempPos, ECmpFolded ) ) 
            {
            aProfiles->AppendL( KJpeg_sm_ico() );
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
HBufC* CUpnpImageSolverBase::ProfileForFileL( const TDesC& /*aFilename*/, 
                                              const TDesC8& aMimetype, 
                                              RFile& aFile )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpImageSolverBase::ProfileForFileL" );
    HBufC* retval = NULL;
       

    CImageDecoder* imageDecoder = CImageDecoder::FileNewL(
                                                    aFile,
                                                    aMimetype,
                                                    ContentAccess::EPeek );
    
    CleanupStack::PushL( imageDecoder );

    TSize imageResolution = imageDecoder->FrameInfo().iOverallSizeInPixels;

    if ( aMimetype.Compare( KImageJpeg() ) == 0 ) 
        {        
        // assuming here that thumbnails are not supported and that we do 
        // not use device icons or at least we do not resolve DLNA profile 
        // of device icons with this component.

        if ( imageResolution.iWidth <= KJpeg_smico_max_width && 
             imageResolution.iHeight <= KJpeg_smico_max_height ) 
            {
            retval = HBufC::NewL( KJpeg_sm_ico().Length() );
            retval->Des().Append( KJpeg_sm_ico() );
            }
        else if ( imageResolution.iWidth <= KJpeg_lrgico_max_width && 
             imageResolution.iHeight <= KJpeg_lrgico_max_height ) 
            {
            retval = HBufC::NewL( KJpeg_lrg_ico().Length() );
            retval->Des().Append( KJpeg_lrg_ico() );
            }
        else if ( imageResolution.iWidth <= KJpeg_sm_max_width && 
             imageResolution.iHeight <= KJpeg_sm_max_height ) 
            {
            retval = HBufC::NewL( KJpeg_sm().Length() );
            retval->Des().Append( KJpeg_sm() );
            }
        else if ( imageResolution.iWidth <= KJpeg_med_max_width && 
                  imageResolution.iHeight <= KJpeg_med_max_height )
            {
            retval = HBufC::NewL( KJpeg_med().Length() );
            retval->Des().Append( KJpeg_med() );
            }
        else if ( imageResolution.iWidth <= KJpeg_lrg_max_width && 
                  imageResolution.iHeight <= KJpeg_lrg_max_height )
            {
            retval = HBufC::NewL( KJpeg_lrg().Length() );
            retval->Des().Append( KJpeg_lrg() );
            }

        }
    else if ( aMimetype.Compare( KImagePng() ) == 0 ) 
        {
        // assuming here that thumbnails are not supported and that we do 
        // not use device icons or at least we do not resolve DLNA profile 
        // of device icons with this component. This leads to profiling all
        // png-files as PNG_LRG in case their size is less than 4096x4096.
        if ( imageResolution.iWidth <= KPng_lrg_max_width && 
             imageResolution.iHeight <= KPng_lrg_max_height )
            {
            retval = HBufC::NewL( KPngLrg().Length() );
            retval->Des().Append( KPngLrg() );
            }
        }
        
    // clean up
    CleanupStack::PopAndDestroy( imageDecoder );
      
    return retval;
    }

// end of file
