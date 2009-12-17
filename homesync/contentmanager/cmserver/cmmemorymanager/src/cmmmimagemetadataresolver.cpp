/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image metadata resolver class in Memory manager component
*
*/


#include <e32std.h>
#include <ExifModify.h>
#include <ExifTag.h> 
#include <exifutility.h>
#include <ExifRead.h> 
#include "msdebug.h"
#include "cmmmimagemetadataresolver.h"

const TUint16 KIdFNumber = 0x829D;
const TUint16 KIdFocalLength = 0x920A;
const TUint16 KIdFocalLength35mmFilm = 0xA405;
const TUint16 KIdFocalPlaneResolutionUnit = 0xA210;
const TUint16 KIdFocalPlaneXResolution = 0xA20E;
const TUint16 KIdFocalPlaneYResolution = 0xA20F;
const TUint32 KDefaultPixelsPerResolutionUnitNumerator = 99;
const TUint32 KDefaultPixelsPerResolutionUnitDenominator = 66;
const TUint16 KDefaultYCbCrPositioningCentered = 1;
const TUint16 KDefaultColorSpace_sRGB = 1;

// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::NewL
// ---------------------------------------------------------------------------
//
CCmMmImageMetadataResolver* CCmMmImageMetadataResolver::NewL( 
    RFs& aFileSession )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::NewL() start"));
    CCmMmImageMetadataResolver* self = 
        CCmMmImageMetadataResolver::NewLC( aFileSession );
    CleanupStack::Pop( self );
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::NewLC
// ---------------------------------------------------------------------------
//
CCmMmImageMetadataResolver* CCmMmImageMetadataResolver::NewLC( 
    RFs& aFileSession )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::NewLC() start"));
    CCmMmImageMetadataResolver* self = 
        new ( ELeave ) CCmMmImageMetadataResolver( aFileSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::NewLC() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CCmMmImageMetadataResolver::~CCmMmImageMetadataResolver()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::~CCmMmImageMetadataResolver() start"));
    delete iExifOrig;
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::~CCmMmImageMetadataResolver() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::CaptureOrginalMetadataL
// ---------------------------------------------------------------------------
//
void CCmMmImageMetadataResolver::CaptureOrginalMetadataL( 
    const TDesC& aOrigFileName )
    {
    RFile orig;
    User::LeaveIfError( orig.Open( iFileSession, aOrigFileName, EFileRead ) );    
    CleanupClosePushL( orig );        
    TInt size = 0;
    orig.Size(size);
    delete iExifOrig;
    iExifOrig = NULL;
    iExifOrig = HBufC8::NewL( size );
    TPtr8 bufferDes( iExifOrig->Des() );
    User::LeaveIfError( orig.Read( bufferDes ) );
    CleanupStack::PopAndDestroy();    
    }
    
// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::ResolveMetadataL
// ---------------------------------------------------------------------------
//
void CCmMmImageMetadataResolver::ResolveMetadataL(
    const TDesC& aShrinkedFileName )
    {
    RFile shrinked;
    User::LeaveIfError( 
        shrinked.Open( iFileSession, aShrinkedFileName, EFileRead|EFileWrite ) );
    
    CleanupClosePushL( shrinked );         
    
    // 1. Read Exif image from the file to a buffer...
    TInt sizeS = 0;
    shrinked.Size(sizeS);
    HBufC8* exifShrinked = HBufC8::NewL( sizeS );
    CleanupStack::PushL( exifShrinked );
    TPtr8 bufferDesS( exifShrinked->Des() );
    User::LeaveIfError( shrinked.Read( bufferDesS ) );
    
    CExifModify* modify = NULL;
    CExifRead* read = NULL;
    TRAPD( err1, read = CExifRead::NewL( iExifOrig->Des() ) );
    User::LeaveIfError( err1 );
    // 2. Instantiate Exif modifier in EModify mode...
    TRAPD( err, modify = CExifModify::NewL( exifShrinked->Des() ) );
    if(err == KErrCorrupt)
        {
        modify = CExifModify::NewL( exifShrinked->Des(), CExifModify::ECreate );
        }
    CleanupStack::PushL(modify);
    // If exif info fails, there is nothing we can do
    TRAP_IGNORE( FillExifL( *read, *modify ) );         
    
    // 4. Get the modified Exif image...
    // If zero length descriptor is given instead of exif->Des(), then only the
    // Exif meta data is returned
    TInt pos = 0;
    User::LeaveIfError( shrinked.Seek( ESeekStart, pos ) );    
    HBufC8* modifiedExif = NULL;
    TRAPD( err2, modifiedExif = modify->WriteDataL( exifShrinked->Des() ) );
    /* Process the modified Exif data */
    if( modifiedExif && !err2 )
        {
        shrinked.Write( *modifiedExif, modifiedExif->Des().Length() );
        }    
    
    delete modifiedExif;
    modifiedExif = NULL;

    // 5. Delete the modifier instance...
    CleanupStack::PopAndDestroy( modify );
    CleanupStack::PopAndDestroy( exifShrinked );
    CleanupStack::PopAndDestroy(); 
    delete iExifOrig;
    iExifOrig = NULL; 
    
    }
    
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCmMmImageMetadataResolver::CCmMmImageMetadataResolver( RFs& aFileSession ) 
    : iFileSession( aFileSession )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::CCmMmImageMetadataResolver() start"));
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::CCmMmImageMetadataResolver() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::ConstructL
// ---------------------------------------------------------------------------
//
void CCmMmImageMetadataResolver::ConstructL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::ConstructL() start"));
    LOG(_L("[MEMORY MNGR]\t CCmMmImageMetadataResolver::ConstructL() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmImageMetadataResolver::FillExifL
// ---------------------------------------------------------------------------
//
void CCmMmImageMetadataResolver::FillExifL( CExifRead& aRead, CExifModify& aModify )
    {    
    TRAP_IGNORE( aModify.SetImageDescriptionL( *aRead.GetImageDescriptionL() ) );
    TRAP_IGNORE( aModify.SetMakeL( *aRead.GetMakeL() ) );
    TRAP_IGNORE( aModify.SetModelL( *aRead.GetModelL() ) ); 
    
    TRAP_IGNORE( aModify.SetThumbnailL( *aRead.GetThumbnailL() ) );
    TRAP_IGNORE( aModify.SetTransferFunctionL( *aRead.GetTransferFunctionL() ) );
    TRAP_IGNORE( aModify.SetDateTimeL( *aRead.GetDateTimeL() ) );         
    
    TRAP_IGNORE( aModify.SetSoftwareL( *aRead.GetSoftwareL() ) );
    TRAP_IGNORE( aModify.SetCopyrightL( *aRead.GetCopyrightL() ) );

    aModify.SetXResolutionL(
        KDefaultPixelsPerResolutionUnitNumerator,
        KDefaultPixelsPerResolutionUnitDenominator );
    aModify.SetYResolutionL( 
        KDefaultPixelsPerResolutionUnitNumerator, 
        KDefaultPixelsPerResolutionUnitDenominator );

    aModify.SetYCbCrPositioningL( KDefaultYCbCrPositioningCentered );

    aModify.SetColorSpaceL( KDefaultColorSpace_sRGB );
    
    TUint32 uint32Value = 0;
    TUint16 uint16Value = 0;
    HBufC8* textValue = NULL;
    const CExifTag* exifTagInfo = NULL;
    TInt exifError( KErrNone );
    exifError = aRead.GetPixelXDimension( uint32Value );
    if( exifError == KErrNone )
        { 
        TRAP_IGNORE( aModify.SetPixelXDimensionL( uint32Value ) );
        }
    exifError = aRead.GetPixelYDimension( uint32Value );
    if( exifError == KErrNone )
        { 
        TRAP_IGNORE( aModify.SetPixelYDimensionL( uint32Value ) );
        }
    
    exifError = aRead.GetWhiteBalance( uint16Value );
    if( exifError == KErrNone )
        {
        aModify.SetWhiteBalanceL( uint16Value );
        }
    exifError = aRead.GetFlash( uint16Value );
    if( exifError == KErrNone )
        {
        aModify.SetFlashL( uint16Value );        
        }

    exifError = aRead.GetExposureProgram( uint16Value );
    if( exifError == KErrNone )
        {
        aModify.SetExposureProgramL( uint16Value );
        }


    TRAP( exifError, textValue = aRead.GetUserCommentL() );

    if ( exifError == KErrNone )
        {
        aModify.SetUserCommentL( *textValue );
        }

    TRAP( exifError, textValue = aRead.GetDateTimeOriginalL() );
    if ( exifError == KErrNone )
        {
        aModify.SetDateTimeOriginalL( *textValue );
        }

    TRAP( exifError, textValue = aRead.GetDateTimeDigitizedL() );
    if ( exifError == KErrNone )
        {
        aModify.SetDateTimeDigitizedL( *textValue );
        }

    exifError = aRead.GetOrientation( uint16Value );
    if ( exifError == KErrNone )
        {
        aModify.SetOrientationL( uint16Value );
        }

    exifError = aRead.GetYCbCrPositioning( uint16Value );
    if ( exifError == KErrNone )
        {
        aModify.SetYCbCrPositioningL( uint16Value );
        }

    exifError = aRead.GetResolutionUnit( uint16Value );
    if ( exifError == KErrNone )
        {
        aModify.SetResolutionUnitL( uint16Value );
        }

    TRAP( exifError, textValue = aRead.GetIsoSpeedRatingsL() );
    if ( exifError == KErrNone )
        {
        aModify.SetIsoSpeedRatingsL( *textValue );
        }

    TRAP( exifError, textValue = aRead.GetRelatedSoundFileL() );
    if ( exifError == KErrNone )
        {
        aModify.SetRelatedSoundFileL( *textValue );
        }
                
    TUint32 exifDenominator = 0;
    exifError = aRead.GetExposureTime( uint32Value, exifDenominator );
    if ( exifError == KErrNone )
        {
        aModify.SetExposureTimeL( uint32Value, exifDenominator );
        }
        
    exifDenominator = 0;
    exifError = aRead.GetApertureValue( uint32Value, exifDenominator );
    if ( exifError == KErrNone )
        {
        aModify.SetApertureValueL( uint32Value, exifDenominator );
        }        

    exifError = aRead.GetColorSpace( uint16Value );
    if ( exifError == KErrNone )
        {
        aModify.SetColorSpaceL( uint16Value );
        }

    TInt32 exifExposureB = 0;
    TInt32 exifDenominatorB = 0;
    exifError = aRead.GetExposureBiasValue( exifExposureB, exifDenominatorB );
    if ( exifError == KErrNone )
        {
        aModify.SetExposureBiasValueL( exifExposureB, exifDenominatorB );
        }

    exifError = aRead.GetMeteringMode( uint16Value );
    if ( exifError == KErrNone )
        {               
        aModify.SetMeteringModeL( uint16Value );
        }

    exifDenominator = 0;
    exifError = aRead.GetThumbnailXResolution( uint32Value, exifDenominator );
    if ( exifError == KErrNone )
        {        
        aModify.SetThumbnailXResolutionL( uint32Value, exifDenominator );
        }
    
    exifDenominator = 0;
    exifError = aRead.GetThumbnailYResolution( uint32Value, exifDenominator );
    if ( exifError == KErrNone )
        {        
        aModify.SetThumbnailYResolutionL( uint32Value, exifDenominator );
        } 
       
    exifError = aRead.GetThumbnailResolutionUnit( uint16Value );
    if ( exifError == KErrNone )
        {
        aModify.SetThumbnailResolutionUnitL( uint16Value );
        }


    TInt32 exifShutterSpeedValue = 0;
    TInt32 exifDenominatorS = 0;
    exifError = aRead.GetShutterSpeedValue( exifShutterSpeedValue, exifDenominatorS );
    if ( exifError == KErrNone )
        {
        aModify.SetShutterSpeedValueL( exifShutterSpeedValue, exifDenominatorS );
        }

    TUint8 exifComponent4th( 0 );
    TUint8 exifComponent3rd( 0 );
    TUint8 exifComponent2nd( 0 );
    TUint8 exifComponent1st( 0 );

    exifError = aRead.GetComponentsConfiguration(
    		exifComponent1st, exifComponent2nd, exifComponent3rd, exifComponent4th );
    if ( exifError == KErrNone )
        {
        aModify.SetComponentsConfigurationL(
        		exifComponent1st, exifComponent2nd, 
        		exifComponent3rd, exifComponent4th );
        }
        
    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFNumber ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFNumberBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFNumber, CExifTag::ETagRational, 1 );
        aModify.SetTagL( EIfdExif, tagInfo, exifFNumberBuf );
        }
        

    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFocalLength ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFocalLengthBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFocalLength, CExifTag::ETagRational, 1 );
        TRAP_IGNORE( aModify.SetTagL( EIfdExif, tagInfo, exifFocalLengthBuf ) );
        }
        
    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFocalLength35mmFilm ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFocal35mmBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFocalLength35mmFilm, CExifTag::ETagRational, 1 );
        TRAP_IGNORE( aModify.SetTagL( EIfdExif, tagInfo, exifFocal35mmBuf ) );
        }                

    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFocalPlaneResolutionUnit ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFocalPlaneResolutionUnitBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFocalPlaneResolutionUnit, CExifTag::ETagShort, 1 );
        TRAP_IGNORE( aModify.SetTagL( EIfdExif, tagInfo, exifFocalPlaneResolutionUnitBuf ) );
        }        

    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFocalPlaneXResolution ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFocalPlaneXResBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFocalPlaneXResolution, CExifTag::ETagRational, 1 );
        TRAP_IGNORE( aModify.SetTagL( EIfdExif, tagInfo, exifFocalPlaneXResBuf ) );
        }
        
    TRAP( exifError, exifTagInfo = aRead.GetTagL( EIfdExif, KIdFocalPlaneYResolution ) );
    if ( exifError == KErrNone )
        {
        TPtrC8 exifFocalPlaneYResBuf( exifTagInfo->Data() );
        TExifTagInfo tagInfo( KIdFocalPlaneYResolution, CExifTag::ETagRational, 1 );
        TRAP_IGNORE( aModify.SetTagL( EIfdExif, tagInfo, exifFocalPlaneYResBuf ) );
        }
         
    exifError = aRead.GetLightSource( uint16Value );
    if ( exifError == KErrNone )
        {
        TRAP_IGNORE( aModify.SetLightSourceL( uint16Value ) );
        }
                                                                                                                        
    }

// End of file

