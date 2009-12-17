/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
* Description:  Utility class to get path related info for the media files
*/

// INCLUDE FILES
#include <bautils.h>    
#include <upnpstring.h>
#include <upnpdlnaprotocolinfo.h>
#include <pathinfo.h>
#include <centralrepository.h>

#include "upnpmspathutility.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpcdutils.h"
#include "upnpdominterface.h"

// CONSTANTS
_LIT( KYearMonthDayFormat,        "%04d\\%02d\\%02d\\");
_LIT( KArtistFormat,              "%S\\");
_LIT( KAlbumFormat,               "%S\\");
_LIT( KBackSlash,                 "\\");
_LIT( KSlashData,                 "\\Data\\");
_LIT( KUnknown,                   "Unknown");

_LIT( KSeparator,                 ":" );
_LIT( KNullTime,                  "000000" );

_LIT8( KMimeAudio,                "audio" );
_LIT8( KMimeVideo,                "video" );
_LIT8( KMimeImage,                "image" );
_LIT8( KMimePlaylist,             "audio/mpegurl" );


const TUid KCRUidMediaServer   = { 0x101F978F };
const TInt KDateStringLength        = 10;
const TInt KDateTimeStringLength    = 19;
const TInt KMaxDateStringLength     = 30;

// ============================ MEMBER FUNCTIONS ============================

// ---------------------------------------------------------------------------
// CUPnPMSPathUtility::CUPnPMSPathUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUPnPMSPathUtility::CUPnPMSPathUtility()
    {
    }

// ---------------------------------------------------------------------------
// CUPnPMSPathUtility::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CUPnPMSPathUtility* CUPnPMSPathUtility::NewL()
    {
    CUPnPMSPathUtility* self = CUPnPMSPathUtility::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CUPnPMSPathUtility::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CUPnPMSPathUtility* CUPnPMSPathUtility::NewLC()
    {
    CUPnPMSPathUtility* self = new( ELeave ) CUPnPMSPathUtility;
    CleanupStack::PushL( self );
    self->ConstructL( KCRUidMediaServer );
       return self;
    }

// ---------------------------------------------------------------------------
// CUPnPMSPathUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUPnPMSPathUtility::ConstructL(TUid aRepositoryUid)
    {
    iRepository = CRepository::NewL(aRepositoryUid);
    }
    
// ---------------------------------------------------------------------------
// CUPnPMSPathUtility::~CUPnPMSPathUtility()
// Destructor
// ---------------------------------------------------------------------------
//
CUPnPMSPathUtility::~CUPnPMSPathUtility()
    {
    delete iRepository;
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::GetCopyPathDriveL
// Gets the drive for the copy operation
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::GetCopyPathDriveL( 
             TDriveNumber& aDrive ) const
    {
    HBufC* location = HBufC::NewL(KMaxFileName);
    CleanupStack::PushL( location );

    TPtr locPtr = location->Des();
    
    // Get the location setting from ServerSettings
    iRepository->Get( EUploadDirectory, locPtr );

    // Check to see if drive is ready.
    TVolumeInfo volInfo;
    TDriveUnit driveUnit = TDriveUnit( locPtr );
    
    RFs fileServer;
    User::LeaveIfError( fileServer.Connect() );
    CleanupClosePushL( fileServer );
    User::LeaveIfError( fileServer.Volume( volInfo, driveUnit ) );
    CleanupStack::PopAndDestroy( &fileServer );

    aDrive = (TDriveNumber)((TInt)driveUnit);

    CleanupStack::PopAndDestroy( location );
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::GetCopyPathL
// Returns the path of the upnp item to be copied
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
HBufC* CUPnPMSPathUtility::GetCopyPathL(
                        const TXmlEngElement& aItem,
                        CUpnpDlnaProtocolInfo& aProInfo ) const
    {
    TDriveNumber drive;
    GetCopyPathDriveL( drive );   
    return GetCreateCopyPathL( aItem, 
                               aProInfo, 
                               EFalse,
                               drive );
    }   

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::CreateCopyPathL
// Returns the path of the upnp item to be copied
// Creates the path if necessary and appends the filename and extension if
// required
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
HBufC* CUPnPMSPathUtility::CreateCopyPathL(
                const TXmlEngElement& aItem,
                CUpnpDlnaProtocolInfo& aProInfo ) const
    {
    TDriveNumber drive;
    GetCopyPathDriveL( drive );
    return GetCreateCopyPathL( aItem, 
                               aProInfo, 
                               ETrue,
                               drive );
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::GetCreateCopyPathL
// Returns the path of the upnp item to be copied
// Creates the path if necessary and appends the filename and extension if
// required
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
HBufC* CUPnPMSPathUtility::GetCreateCopyPathL(
               const TXmlEngElement& aItem,
               CUpnpDlnaProtocolInfo& aProInfo,
               TBool aCreatePath,
               TDriveNumber aDriveNumber ) const
    {
       
    HBufC* path = HBufC::NewLC( KMaxPath );
    TPtr refPath = path->Des();
    
    TDriveUnit driveUnit = TDriveUnit( aDriveNumber );
    AppendDataL( refPath, driveUnit.Name() );
    if ( EDriveC == driveUnit )
        {
        //C:\\Data\\(Images/Videos/Sounds)....
        AppendDataL( refPath, KSlashData );
        }
    else
        {
        //\\(Images/Videos/Sounds)....
        AppendDataL( refPath, KBackSlash );
        }
    
    // Get the protocolinfo-attribute
    TUPnPItemType fileType = FileTypeByMimeTypeL( 
            aProInfo.ThirdField() );
    
    switch( fileType )
        {
        case ETypeAudio:
            {
            AppendDataL( refPath, PathInfo::SoundsPath() );
            AppendArtistAlbumL( refPath, aItem );
            break;
            }
        case ETypeVideo:
            {
            AppendDataL( refPath, PathInfo::VideosPath() );
            AppendYearMonthDayL( refPath, aItem );
            break;
            }
        case ETypeImage:
            {
            AppendDataL( refPath, PathInfo::ImagesPath() );
            AppendYearMonthDayL( refPath, aItem );
            break;
            }
        case ETypePlaylist:
        case ETypeOther:
        default:
            {
            AppendDataL( refPath, PathInfo::OthersPath() );
            }                
        }
    if( aCreatePath )
        {
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL(fs);
        BaflUtils::EnsurePathExistsL( fs, refPath );
        CleanupStack::PopAndDestroy(&fs);
        }
    CleanupStack::Pop( path );
    
    return path;
    }   

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::FileTypeByMimeTypeL
// Get the file type based on MIME type of the file. Leaves in error cases.
// --------------------------------------------------------------------------
CUPnPMSPathUtility::TUPnPItemType CUPnPMSPathUtility::FileTypeByMimeTypeL(
        const TDesC8& aMimeType ) const
    {
    TUPnPItemType returnValue = ETypeOther;

    if( aMimeType != KNullDesC8 )
        {
        if( aMimeType.Find( KMimeImage() ) >= 0 )
            {
            returnValue = ETypeImage;
            }
        else if( aMimeType.Find( KMimePlaylist() ) >= 0 )
            {
            returnValue = ETypePlaylist;
            }
        else if( aMimeType.Find( KMimeAudio() ) >= 0 )
            {
            returnValue = ETypeAudio;
            }
        else if( aMimeType.Find( KMimeVideo() ) >= 0 )
            {
            returnValue = ETypeVideo;
            }
        else
            {
            returnValue = ETypeOther;
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::AppendYearMonthDayL
// Appends year, month and day to the path.
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::AppendYearMonthDayL( 
        TDes& aPath, const TXmlEngElement& aItem ) const
    {  
    // Get the date-element
    TXmlEngElement dateElem;
    UpnpDomInterface::GetElementL( aItem, dateElem, KDate8 );
    TTime date; date.HomeTime();
    if ( !dateElem.IsNull( ) )
        {
        UPnPDateAsTTimeL( dateElem.Value(), date );
        }     
    
    TDateTime ymd = date.DateTime();
    CheckBufferSpaceL( aPath, 11 ); //4(year)+2(month)+2(day)+3(\)
    
    aPath.AppendFormat( KYearMonthDayFormat(), 
            ymd.Year(), ymd.Month()+1, ymd.Day()+1 );
    
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::AppendArtistAlbumL
// Appends artist and album to the path.
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::AppendArtistAlbumL( 
        TDes& aPath, const TXmlEngElement& aItem ) const
    {
    
    // Get the artist-element
    TXmlEngElement artistElem;
    UpnpDomInterface::GetElementL( aItem, artistElem, KArtist );
    if ( !artistElem.IsNull( )  )
        {
        HBufC* artist = UpnpString::ToUnicodeL( artistElem.Value() );
        CleanupStack::PushL( artist );
        
        TPtr ptrArtist = artist->Des();
        UpnpCdUtils::ValidateFilePath( ptrArtist );
        
        CheckBufferSpaceL( aPath, artist->Length()+1 );// 1 for '\'          
        aPath.AppendFormat( KArtistFormat(), artist );
        
        CleanupStack::PopAndDestroy( artist );
        }
    else
        {
        CheckBufferSpaceL( aPath, KUnknown().Length()+1 );  // 1 for '\'       
        aPath.AppendFormat( KArtistFormat(), &KUnknown() );
        }
    
    // Get the album-element
    TXmlEngElement albumElem;
    UpnpDomInterface::GetElementL( aItem, albumElem, KAlbum );
    if ( !albumElem.IsNull( ) )
        {
        HBufC* album = UpnpString::ToUnicodeL( albumElem.Value() );
        CleanupStack::PushL( album );
        
        TPtr ptrAlbum = album->Des();
        UpnpCdUtils::ValidateFilePath( ptrAlbum );
        
        CheckBufferSpaceL( aPath, album->Length()+1 );// 1 for '\'
        aPath.AppendFormat( KAlbumFormat(), album );
        CleanupStack::PopAndDestroy( album );
        }
    else
        {
        CheckBufferSpaceL( aPath, KUnknown().Length()+1 );  // 1 for '\'       
        aPath.AppendFormat( KAlbumFormat(), &KUnknown() );
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::AppendDataL
// Appends data to the path's buffer.
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::AppendDataL( 
        TDes& aPath, const TDesC& aData ) const
    {  
    CheckBufferSpaceL( aPath, aData );
    aPath.Append( aData );
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::CheckBufferSpaceL
// Checks whether the data can be appended to buffer or not.
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::CheckBufferSpaceL( 
        const TDes& aPath, const TDesC& aData ) const
    {
    CheckBufferSpaceL( aPath, aData.Length() );
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::CheckBufferSpaceL
// Checks whether the data of the specified length
// can be appended to buffer or not.
// --------------------------------------------------------------------------
//
void CUPnPMSPathUtility::CheckBufferSpaceL( 
        const TDes& aPath, const TInt& aLength ) const
    {  
    if ( (aPath.Length() + aLength) > aPath.MaxLength() )
        {
        User::Leave( KErrOverflow );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMSPathUtility::UPnPDateAsTTimeL
// Converts upnp date to TTime object.
//---------------------------------------------------------------------------
void CUPnPMSPathUtility::UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
    TTime& aTime ) const
    {
    // This method is capable of handling the most common dc:date formats:
    // CCYY-MM-DD and CCYY-MM-DDThh:mm:ss
    // Rest of the dc:date formats are handled as well, but they might not
    // be converted precisely
    
    TBuf<KMaxDateStringLength> formatDateString;
    HBufC* dateString = HBufC::NewL( aUpnpDate.Length() );
    dateString->Des().Copy( aUpnpDate );

    if( aUpnpDate.Length() >= KDateStringLength )
        {
        // CCYY-MM-DD --> CCYYMMDD
        formatDateString.Copy( dateString->Des().Left( 4 ) ); // Year
        formatDateString.Append( dateString->Des().Mid( 5,2 ) ); // Month
        formatDateString.Append( dateString->Des().Mid( 8,2 ) ); // Day        

        if( aUpnpDate.Length() >= KDateTimeStringLength )
            {
            // hh:mm:ss --> hhmmss
            formatDateString.Append( KSeparator );
            // Hours
            formatDateString.Append( dateString->Des().Mid( 11, 2 ) ); 
            // Minutes
            formatDateString.Append( dateString->Des().Mid( 14, 2 ) );
            // Seconds 
            formatDateString.Append( dateString->Des().Mid( 17, 2 ) ); 
            }
        else
            {
            // hh:mm:ss --> 000000
            formatDateString.Append( KSeparator );
            formatDateString.Append( KNullTime );
            }
        }
    delete dateString;
    
    User::LeaveIfError( aTime.Set( formatDateString ) );
    }

//  End of File  
