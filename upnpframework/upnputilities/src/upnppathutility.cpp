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
#include "upnpsettingsengine.h"
#include "upnpconstantdefs.h"
#include <upnpstring.h>
#include <upnpitem.h>
#include <upnpdlnaprotocolinfo.h>
#include <pathinfo.h>

#include "upnppathutility.h"
#include "Upnpcommonutils.h"

// CONSTANTS
_LIT( KYearMonthDayFormat,        "%04d\\%02d\\%02d\\");
_LIT( KTitleExtFormat,            "%S%S");
_LIT( KArtistFormat,              "%S\\");
_LIT( KAlbumFormat,               "%S\\");
_LIT( KSlash,                     "\\");
_LIT( KSlashData,                 "\\Data\\");
_LIT( KUnknown,                   "Unknown");

_LIT( KSeparator,                   ":" );
_LIT( KNullTime,                    "000000" );

const TInt KDateStringLength        = 10;
const TInt KDateTimeStringLength    = 19;
const TInt KMaxDateStringLength     = 30;

// ============================ MEMBER FUNCTIONS ============================

// ---------------------------------------------------------------------------
// CUPnPPathUtility::CUPnPPathUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUPnPPathUtility::CUPnPPathUtility()
    {
    }

// ---------------------------------------------------------------------------
// CUPnPPathUtility::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPPathUtility* CUPnPPathUtility::NewL()
    {
    CUPnPPathUtility* self = CUPnPPathUtility::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CUPnPPathUtility::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPPathUtility* CUPnPPathUtility::NewLC()
    {
    CUPnPPathUtility* self = new( ELeave ) CUPnPPathUtility;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CUPnPPathUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUPnPPathUtility::ConstructL()
    {
    iSettingsEngine = CUPnPSettingsEngine::NewL();
    }
    
// ---------------------------------------------------------------------------
// CUPnPPathUtility::~CUPnPPathUtility()
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPPathUtility::~CUPnPPathUtility()
    {
    delete iSettingsEngine;
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::GetCopyPathDriveL
// Gets the drive for the copy operation
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPathUtility::GetCopyPathDriveL( 
             TDriveNumber& aDrive ) const
    {
    iSettingsEngine->GetCopyLocationDriveL( aDrive );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::GetCopyPathL
// Returns the path of the upnp item to be copied
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUPnPPathUtility::GetCopyPathL(
               const CUpnpItem& aItem,
               const CUpnpElement& aResource,
               TBool aAppendTitleAndExt ) const
    {
    TDriveNumber drive;
    iSettingsEngine->GetCopyLocationDriveL( drive );   
    return GetCreateCopyPathL( aItem, 
                               aResource, 
                               aAppendTitleAndExt,
                               EFalse,
                               drive );
    }   

// --------------------------------------------------------------------------
// CUPnPPathUtility::GetCopyPathL
// Returns the path of the upnp item to be copied
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUPnPPathUtility::GetCopyPathL(
               const CUpnpItem& aItem,
               const CUpnpElement& aResource,
               TBool aAppendTitleAndExt,
               TDriveNumber aDriveNumber ) const
    {
    
    return GetCreateCopyPathL( aItem, 
                               aResource, 
                               aAppendTitleAndExt,
                               EFalse,
                               aDriveNumber );
    }   

// --------------------------------------------------------------------------
// CUPnPPathUtility::CreateCopyPathL
// Returns the path of the upnp item to be copied
// Creates the path if necessary and appends the filename and extension if
// required
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUPnPPathUtility::CreateCopyPathL(
               const CUpnpItem& aItem,
               const CUpnpElement& aResource,
               TBool aAppendTitleAndExt ) const
    {
    TDriveNumber drive;
    iSettingsEngine->GetCopyLocationDriveL( drive );
    return GetCreateCopyPathL( aItem, 
                               aResource, 
                               aAppendTitleAndExt,
                               ETrue,
                               drive );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::CreateCopyPathL
// Returns the path of the upnp item to be copied
// Creates the path if necessary and appends the filename and extension if
// required
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUPnPPathUtility::CreateCopyPathL(
               const CUpnpItem& aItem,
               const CUpnpElement& aResource,
               TBool aAppendTitleAndExt,
               TDriveNumber aDriveNumber ) const
    {
    return GetCreateCopyPathL( aItem, 
                               aResource, 
                               aAppendTitleAndExt,
                               ETrue,
                               aDriveNumber );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::RemoveEmptyFoldersFromCopyPathL
// Removes empty folders from the copy path 
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPPathUtility::RemoveEmptyFoldersFromCopyPathL(
                            const TDesC& aCopyPath )
    {  
    TPtrC basePath;
    const TDesC& soundsPath = PathInfo::SoundsPath();
    const TDesC& videosPath = PathInfo::VideosPath();
    const TDesC& imagesPath = PathInfo::ImagesPath();
    const TDesC& othersPath = PathInfo::OthersPath();
    
    TInt baseLength = 0;
    TInt found = KErrNotFound;    
    if ( KErrNotFound != ( found = aCopyPath.Find( 
                                     soundsPath ) ) )
        {
        baseLength = soundsPath.Length();
        }
    else if ( KErrNotFound != ( found = aCopyPath.Find( 
                                    videosPath ) ) )
        {
        baseLength = videosPath.Length();
        }
    else if ( KErrNotFound != ( found = aCopyPath.Find( 
                                    imagesPath ) ) )
        {
        baseLength = imagesPath.Length();
        }
    else if ( KErrNotFound != ( found = aCopyPath.Find( 
                                    othersPath ) ) )
        {
        baseLength = othersPath.Length();
        }

    if ( KErrNotFound != found )
        {
        TPtrC basePath = aCopyPath.Left( found + baseLength );
        TParse parsePath;
        User::LeaveIfError( parsePath.Set( aCopyPath, NULL, NULL ) );
        //Remove filename and extension before passing the copy path
        RemoveEmptyFoldersL( basePath, parsePath.DriveAndPath() );
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::GetCreateCopyPathL
// Returns the path of the upnp item to be copied
// Creates the path if necessary and appends the filename and extension if
// required
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
HBufC* CUPnPPathUtility::GetCreateCopyPathL(
               const CUpnpItem& aItem,
               const CUpnpElement& aResource,
               TBool aAppendTitleAndExt,
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
        AppendDataL( refPath, KSlash );
        }
    
    // Get the protocolinfo-attribute
    const CUpnpAttribute* pInfo = FindAttributeByName(
            aResource, KAttributeProtocolInfo );
    if ( NULL == pInfo )
        {
        User::Leave( KErrArgument );
        }
    
    CUpnpDlnaProtocolInfo* dlnaInfo =
            CUpnpDlnaProtocolInfo::NewL( pInfo->Value() );
    CleanupStack::PushL( dlnaInfo );
    TUPnPItemType fileType = UPnPCommonUtils::FileTypeByMimeTypeL( 
            dlnaInfo->ThirdField() );
    
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
    if( aAppendTitleAndExt )
        {
        AppendTitleAndExtL( refPath, *dlnaInfo, aItem );
        }
    
    CleanupStack::PopAndDestroy( dlnaInfo );
    CleanupStack::Pop( path );
    
    return path;
    }   

// --------------------------------------------------------------------------
// CUPnPPathUtility::AppendTitleAndExtL
// Appends title and extension to the path.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::AppendTitleAndExtL( 
             TDes& aPath, CUpnpDlnaProtocolInfo& aProtocolInfo, 
             const CUpnpItem& aItem ) const
    {
    HBufC* fileExt = UPnPCommonUtils::FileExtensionByMimeTypeL(
            aProtocolInfo.ThirdField() );
    
    User::LeaveIfNull( fileExt );
    CleanupStack::PushL( fileExt );
    
    HBufC* title16 = UpnpString::ToUnicodeL( aItem.Title() );
    CleanupStack::PushL( title16 );
    HBufC* title16checked =
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( *title16 );
    CleanupStack::PopAndDestroy( title16 );
    
    CheckBufferSpaceL( aPath, 
            title16checked->Length()+fileExt->Length() );
    
    aPath.AppendFormat( KTitleExtFormat(), title16checked, fileExt );
    
    delete title16checked; title16checked = NULL;
    CleanupStack::PopAndDestroy( fileExt );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::AppendYearMonthDayL
// Appends year, month and day to the path.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::AppendYearMonthDayL( 
        TDes& aPath, const CUpnpItem& aItem ) const
    {  
    // Get the date-element
    const CUpnpElement* dateElem = FindElementByName(
            aItem, KElementDate );

    TTime date; date.HomeTime();
    TInt offsetMonthDay = 1;
    // Use date element time instead of current time,
    // if element exist
    if ( dateElem != NULL )
        {
        UPnPDateAsTTimeL( dateElem->Value(), date );
        offsetMonthDay = 0;
        }
    TDateTime ymd = date.DateTime();
    CheckBufferSpaceL( aPath, 11 ); //4(year)+2(month)+2(day)+3(\)
        
    aPath.AppendFormat( KYearMonthDayFormat(), 
            ymd.Year(), 
            ymd.Month() + offsetMonthDay, 
            ymd.Day() + offsetMonthDay );
    
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::AppendArtistAlbumL
// Appends artist and album to the path.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::AppendArtistAlbumL( 
        TDes& aPath, const CUpnpItem& aItem ) const
    {
    
    // Get the artist-element
    const CUpnpElement* artistElem = FindElementByName(
                aItem, KElementArtist );
    if ( NULL != artistElem )
        {
        HBufC* artist = UpnpString::ToUnicodeL( artistElem->Value() );
        CleanupStack::PushL( artist );
        
        HBufC* artistchecked =
            UPnPCommonUtils::ReplaceIllegalDirNameCharactersL( *artist );
        CleanupStack::PopAndDestroy( artist );
        
        CheckBufferSpaceL( aPath, artistchecked->Length()+1 );// 1 for '\'          
        aPath.AppendFormat( KArtistFormat(), artistchecked );
        
        delete artistchecked;
        }
    else
        {
        CheckBufferSpaceL( aPath, KUnknown().Length()+1 );  // 1 for '\'       
        aPath.AppendFormat( KArtistFormat(), &KUnknown() );
        }
    
    // Get the album-element
    const CUpnpElement* albumElem = FindElementByName(
                aItem, KElementAlbum );
    if ( NULL != albumElem )
        {
        HBufC* album = UpnpString::ToUnicodeL( albumElem->Value() );
        CleanupStack::PushL( album );
        
        HBufC* albumchecked =
            UPnPCommonUtils::ReplaceIllegalDirNameCharactersL( *album );
        CleanupStack::PopAndDestroy( album );
        
        CheckBufferSpaceL( aPath, albumchecked->Length()+1 );// 1 for '\'
        aPath.AppendFormat( KAlbumFormat(), albumchecked );
        
        delete albumchecked;
        }
    else
        {
        CheckBufferSpaceL( aPath, KUnknown().Length()+1 );  // 1 for '\'       
        aPath.AppendFormat( KAlbumFormat(), &KUnknown() );
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::AppendDataL
// Appends data to the path's buffer.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::AppendDataL( 
        TDes& aPath, const TDesC& aData ) const
    {  
    CheckBufferSpaceL( aPath, aData );
    aPath.Append( aData );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::CheckBufferSpaceL
// Checks whether the data can be appended to buffer or not.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::CheckBufferSpaceL( 
        const TDes& aPath, const TDesC& aData ) const
    {
    CheckBufferSpaceL( aPath, aData.Length() );
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::CheckBufferSpaceL
// Checks whether the data of the specified length
// can be appended to buffer or not.
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::CheckBufferSpaceL( 
        const TDes& aPath, const TInt& aLength ) const
    {  
    if ( (aPath.Length() + aLength) > aPath.MaxLength() )
        {
        User::Leave( KErrOverflow );
        }
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::FindElementByName
// Finds an element within an object.
//---------------------------------------------------------------------------
const CUpnpElement* CUPnPPathUtility::FindElementByName(
    const CUpnpObject& aObject, const TDesC8& aName ) const
    {
    CUpnpElement* element = NULL;
    const RUPnPElementsArray& array =
        const_cast<CUpnpObject&>(aObject).GetElements();
    for( TInt i = 0; i < array.Count(); i++ )
        {
        if( array[ i ]->Name() == aName )
            {
            element = array[ i ];
            i = array.Count();
            }
        }
    return element;
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::FindAttributeByName
// Finds an attribute within an element.
//---------------------------------------------------------------------------
const CUpnpAttribute* CUPnPPathUtility::FindAttributeByName(
    const CUpnpElement& aElement, const TDesC8& aName ) const
    {
    CUpnpAttribute* attribute = NULL;
    const RUPnPAttributesArray& array =
        const_cast<CUpnpElement&>(aElement).GetAttributes();
    
    for( TInt i = 0; i < array.Count(); i++ )
        {
        
        TBufC8<255> buf(array[ i ]->Name());
        if( array[ i ]->Name() == aName )
            {
            attribute = array[ i ];
            i = array.Count();
            }
        }
    return attribute;
    }

// --------------------------------------------------------------------------
// CUPnPPathUtility::UPnPDateAsTTimeL
// Converts upnp date to TTime object.
//---------------------------------------------------------------------------
void CUPnPPathUtility::UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
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

// --------------------------------------------------------------------------
// CUPnPPathUtility::RemoveEmptyFoldersL
// Removes empty folders from the  path 
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CUPnPPathUtility::RemoveEmptyFoldersL(
        const TDesC& aBasePath, const TDesC& aFullPath )
    {
        
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    
    TParse parsePath;
    User::LeaveIfError( parsePath.Set( aFullPath, NULL, NULL ) );
    while( 0 != aBasePath.Compare( parsePath.DriveAndPath() ) )
        {
        if ( KErrNone != fs.RmDir( parsePath.DriveAndPath() ) ||
             KErrNone != parsePath.PopDir() )
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy(&fs);
     
    }

//  End of File  
