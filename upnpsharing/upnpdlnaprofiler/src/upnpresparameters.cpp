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
* Description:      Implementation file of CUpnpResParameters class. 
*                CUpnpResParameters is created to encapsulate information  
*                that is needed in a contentdirectory res element.
*
*/






// system includes

// user includes
#include "upnpresparameters.h"
#include "upnpdlnaprofiler.h"

_LIT( KComponentLogfile, "dlnaprofiler.txt");
#include "upnplog.h"


// constants
const TInt KMaxTintLength = 20; // max. TInt string representation length.
const TInt KMinutesInHour = 60;
const TInt KSecondsInMinute = 60;
const TInt KSecondsInHour = KMinutesInHour * KSecondsInMinute;

const TInt KHourMaxLength = 5;
const TInt KMinutesLength = 2;
const TInt KSecondsLength = 2;

_LIT( KResolutionDivider, "x" );
_LIT( KTimeDivider, ":" );
_LIT( KZero, "0" );

    
// ======== LOCAL FUNCTIONS ========
// NONE

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpResParameters C++ constructor
// --------------------------------------------------------------------------
//
CUpnpResParameters::CUpnpResParameters()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters constructor" );
    }


// --------------------------------------------------------------------------
// CUpnpResParameters::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpResParameters::ConstructL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::ConstructL" );
    }


// --------------------------------------------------------------------------
// CUpnpResParameters::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpResParameters* CUpnpResParameters::NewL()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::NewL" );
    CUpnpResParameters* self = CUpnpResParameters::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpResParameters::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpResParameters* CUpnpResParameters::NewLC()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::NewLC" );
    CUpnpResParameters* self = new( ELeave ) CUpnpResParameters;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpResParameters destructor
// --------------------------------------------------------------------------
//
CUpnpResParameters::~CUpnpResParameters()
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters destructor" );
    delete iProtocolInfo;
    delete iMimetype;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::ProtocolInfo
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpResParameters::ProtocolInfo() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::ProtocolInfo" );

    if ( iProtocolInfo ) 
        {
        return *iProtocolInfo;
        }

    return KNullDesC8();
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::FileSizeL
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpResParameters::FileSizeL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::FileSizeL" );
    HBufC8* retval = NULL;
    
    TBuf<KMaxTintLength> sizeOfFile;
    sizeOfFile.Num( iFileSize );
    
    retval = HBufC8::NewL( sizeOfFile.Length() );
    retval->Des().Append( sizeOfFile );
    
    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::DurationL
//
//   Requirement [7.3.22.1]: The syntax of the res@duration must be 
//   compliant to the following definition:
//     duration = hours ":" minutes ":" seconds
//     hours = 1*5 DIGIT; 0-99999
//     minutes = 2 DIGIT ; 00-59
//     seconds = 2 DIGIT ["." 3 DIGIT] ; 00-59 (.000-.999) 
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpResParameters::DurationL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::DurationL" );
    HBufC8* retval = NULL;

    // Leave if the duration has invalid value
    if( iDurationInSeconds < 0 )
        {
        User::Leave( KErrArgument );
        }

    TInt hour = iDurationInSeconds / KSecondsInHour;
    TInt min = (iDurationInSeconds % KSecondsInHour) / KSecondsInMinute;
    TInt sec = (iDurationInSeconds % KSecondsInHour) % KSecondsInMinute;

    // Leave if the duration is longer than allowed (99999h 99min 99sec)
    if( hour >= 100000 )
        {
        User::Leave( KErrArgument );
        }

    // Convert TInt to TBuf
    TBuf<KHourMaxLength> hours;
    hours.Num( hour );
    hours.Trim();

    TBuf<2> minutes;
    minutes.Num( min );

    TBuf<2> seconds;
    seconds.Num( sec );

    retval = HBufC8::NewL( hours.Length() + 
                           KTimeDivider().Length() + 
                           KMinutesLength +
                           KTimeDivider().Length() + 
                           KSecondsLength );

    retval->Des().Append( hours );
    retval->Des().Append( KTimeDivider() );
    if( min < 10 )
        {
        retval->Des().Append( KZero() );
        }
    retval->Des().Append( minutes );
    retval->Des().Append( KTimeDivider() );
    if( sec < 10 )
        {
        retval->Des().Append( KZero() );
        }
    retval->Des().Append( seconds );

    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::ResolutionL
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpResParameters::ResolutionL() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::ResolutionL" );
    HBufC8* retval = NULL;
    
    TBuf<KMaxTintLength> width;
    width.Num( iResolution.iWidth );

    TBuf<KMaxTintLength> height;
    height.Num( iResolution.iHeight );

    retval = HBufC8::NewL( width.Length() + 
                          KResolutionDivider().Length() + 
                          height.Length() );
                          
    retval->Des().Append( width );
    retval->Des().Append( KResolutionDivider() );
    retval->Des().Append( height );
    
    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::IsImageFile
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpResParameters::IsImageFile() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::IsImageFile" );
    TBool retval = EFalse;

    if ( iMimetype->Left( KMimeStartLength ).Compare( KImage() ) == 0 ) 
        {
        retval = ETrue;
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::IsAudioOrVideoFile
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpResParameters::IsAudioOrVideoFile() const
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::IsAudioOrVideoFile" );
    TBool retval = EFalse;

    if ( iMimetype->Left( KMimeStartLength ).Compare( KVideo() ) == 0 ||
         iMimetype->Left( KMimeStartLength ).Compare( KAudio() ) == 0 ) 
        {
        retval = ETrue;
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::SetProtocolInfoL
// --------------------------------------------------------------------------
//
void CUpnpResParameters::SetProtocolInfoL( const TDesC8& aProtocolInfo ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::SetProtocolInfoL" );
    iProtocolInfo = aProtocolInfo.AllocL();    
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::SetMimetypeL
// --------------------------------------------------------------------------
//  
void CUpnpResParameters::SetMimetypeL( const TDesC8& aMimetype )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::SetMimetypeL" );
    iMimetype = aMimetype.AllocL();
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::SetFileSize
// --------------------------------------------------------------------------
//
void CUpnpResParameters::SetFileSize( TInt aFileSize )
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::SetFileSize" );
    iFileSize = aFileSize;
    }
// --------------------------------------------------------------------------
// CUpnpResParameters::SetResolution
// --------------------------------------------------------------------------
//
void CUpnpResParameters::SetResolution( TSize aResolution ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::SetResolution" );
    iResolution = aResolution;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::SetDurationInSeconds
// --------------------------------------------------------------------------
//
void CUpnpResParameters::SetDurationInSeconds( TInt64 aDuration ) 
    {
    __LOG( "[UPnPDlnaProfiler] CUpnpResParameters::\
SetDurationInSeconds" );
    iDurationInSeconds = aDuration;
    }

// --------------------------------------------------------------------------
// CUpnpResParameters::MimeType
// --------------------------------------------------------------------------
//    
const TDesC8& CUpnpResParameters::MimeType() 
    {
    if ( iMimetype ) 
        {
        return *iMimetype;
        }
    
    return KNullDesC8();
    }
// end of file
