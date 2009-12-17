/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Upnp framework's common utility functions
*
*/


// INCLUDE FILES
// System
#include <e32std.h>
#include <upnpstring.h>
#include <apmrec.h>
#include <apgcli.h>
#include <WlanCdbCols.h>            // WLAN_SERVICE
#include <caf/content.h>                // CAF (WM-DRM check)
#include <caf/agent.h>                  // CAF (WM-DRM check)
#include <commdb.h>
#include <in_sock.h> 
#include <pathinfo.h>

// upnp stack api
#include <upnpcontainerlist.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpdevice.h>

// upnpframework / avcontroller helper api
#include <upnpconstantdefs.h>

// utilities internal
#include "upnpfileutilitytypes.h"
#include "upnpcommonutils.h"

_LIT( KComponentLogfile, "upnputilities.txt");
#include "upnplog.h"

// CONSTANTS
_LIT8( KMimeAudio,          "audio" );
_LIT8( KMimeVideo,          "video" );
_LIT8( KMimeImage,          "image" );
_LIT8( KMimePlaylist,       "audio/mpegurl" );

_LIT16( KXHtml16,           ".XHTML" );
_LIT16( KHtml16,            ".HTML" );
_LIT16( KXml16,             ".XML" );
_LIT16( KTxt16,             ".TXT" );
_LIT16( KJpeg16,            ".JPEG" );
_LIT16( KJpg16,             ".JPG" );
_LIT16( KGif16,             ".GIF" );
_LIT16( KPng16,             ".PNG" );
_LIT16( KMpeg16,            ".MPEG" );
_LIT16( KMpg16,             ".MPG" );
_LIT16( KMpeg416,           ".MPEG4" );
_LIT16( KAvi16,             ".AVI" );
_LIT16( KMp316,             ".MP3" );
_LIT16( KAac16,             ".AAC" );
_LIT16( KWma16,             ".WMA" );
_LIT8( KXHtml,              "application/vnd.pwg-xhtml-print" );
_LIT8( KHtml,               "text/html" );
_LIT8( KXml,                "text/xml" );
_LIT8( KTxt,                "text/plain" );
_LIT8( KJpg,                "image/jpeg" );
_LIT8( KGif,                "image/gif" );
_LIT8( KPng,                "image/png" );
_LIT8( KMpeg,               "video/mpeg" );
_LIT8( KMpeg4,              "video/mpeg4" );
_LIT8( KMp3,                "audio/mpeg" );
_LIT8( KAac,                "audio/aac" );
_LIT8( KWma,                "audio/x-ms-wma" );
_LIT8( KAvi,                "video/x-msvideo" );


_LIT(  KLineFeed,           "\r\n");
_LIT8( KHttpDes,            "http://" );
_LIT( KTemp,                "upnpfwtemp");

_LIT8( KUnderScore,         "_" );
_LIT8( KStar,               "*" );
_LIT8( KColon,              ":");
_LIT8( KKorp,               "<" );
_LIT8( KProk,               ">" );
_LIT8( KQuotation,          "\"" );
_LIT8( KDoubleBackSlash,    "\\" );
_LIT8( KSlash,              "/" );
_LIT8( KQuestionMark,       "?" );
_LIT8( KPipe,               "|" );
_LIT8( KTab,                "\t" );
_LIT8( KSpace,              " " );

const TInt KPlaceHolderLength       = 28;
const TInt KBufferLength            = 256;
const TInt KBufferLengthSmall       = 10;
const TInt KCodeSemicolon           = 58;
const TInt KMask                    = 0xFF;
const TInt KCodeDot                 = 46;


_LIT( KUpnpDateString,"%F%Y-%M-%D");
const TInt KMaxDateStringLength = 30;



_LIT8( KAudioSupport,        "audio/" );
_LIT8( KImageSupport,        "image/" );
_LIT8( KVideoSupport,        "video/" );

_LIT8( KImageJpeg,         	"=JPEG_" );
_LIT8( KImagePng,          	"=PNG_" );

_LIT8( KAudioAc3,         	"=AC3" );
_LIT8( KAudioAmr,         	"=AMR_" );
_LIT8( KAudioAtra,         	"=ATRAC3plus" );
_LIT8( KAudioLpcm,         	"=LPCM" );
_LIT8( KAudioMp3,         	"=MP3" );
_LIT8( KAudioAac,         	"=AAC_" );
_LIT8( KAudioHeaac,        	"=HEAAC_" );
_LIT8( KAudioBsac,         	"=BSAC_" );
_LIT8( KAudioWmab,         	"=WMABASE" );
_LIT8( KAudioWmaf,         	"=WMAFULL" );

_LIT8( KAudioWmap,         	"=WMAPRO" );
_LIT8( KVideoMpeg,          "=MPEG" );
_LIT8( KVideoAvc,          	"=AVC_" );
_LIT8( KVideoWmvm,         	"=WMVMED_" );
_LIT8( KVideoVmvh,          "=WMVHIGH_" );
_LIT8( KVideoWmvh,          "=WMVHM_" );
_LIT8( KVideoWmvspll,       "=WMVSPLL_" );
_LIT8( KVideoWmvspml,       "=WMVSPML_" );
// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UPnPCommonUtils::MemoryCardStateL
//---------------------------------------------------------------------------
//
EXPORT_C TBool UPnPCommonUtils::MemoryCardStateL()
    {
    TDriveInfo driveInfo;

    TBool status = EFalse;
    RFs fileServer;
    User::LeaveIfError( fileServer.Connect() );
    CleanupClosePushL( fileServer );
    TInt error = fileServer.Drive( driveInfo, EDriveE );
    CleanupStack::PopAndDestroy( &fileServer );

    //check if memory card is present
    if ( error == KErrNone && driveInfo.iType == EMediaHardDisk )
        {
        status = ETrue;
        }

    return status;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::ResolveMimeTypeL
// Resolves the MIME type of a file. Leaves in error cases.
// --------------------------------------------------------------------------
EXPORT_C HBufC8* UPnPCommonUtils::ResolveMimeTypeL( const TDesC& aFileName )
    {
    HBufC8* mimetype = NULL;

    mimetype = GetMimeTypeByExtensionL( aFileName );

    // If the mime type was not recognized by the extension, try the 
    // platform mime type recognizer
    if( !mimetype )
        {           
        // finding server
        RApaLsSession sess; 
        TInt error = sess.Connect();
    
        // We have FS, File and RApaLs open
        if ( error == KErrNone )
            {
            CleanupClosePushL( sess );
    
            TUid id = TUid::Uid( 0 );
            TDataType type;
    
            error = sess.AppForDocument( aFileName, id, type );
    
            if( error == KErrNone )
                {
                mimetype = HBufC8::NewL( type.Des8().Length() + 
                                         KLineFeed().Length() );
                mimetype->Des().Append( type.Des8() );
                mimetype->Des().Append( KLineFeed() );
                }
                
            // Clean up
            CleanupStack::PopAndDestroy( &sess );
            }
        }

    return mimetype;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::GetMimeTypeByExtensionL
// Resolves the type of a file. Leaves in error cases.
// --------------------------------------------------------------------------
EXPORT_C HBufC8* UPnPCommonUtils::GetMimeTypeByExtensionL(
    const TDesC& aFileName )
    {
    HBufC8* mimetype = NULL;
    HBufC* tempFilename = aFileName.AllocL();
    CleanupStack::PushL( tempFilename );

    // Convert letters to upper case
    tempFilename->Des().UpperCase();

    TParse parse;
    parse.Set( tempFilename->Des(), NULL, NULL );
    TBufC<KMaxName> ext( parse.Ext() );

    CleanupStack::PopAndDestroy( tempFilename );
    tempFilename = NULL;

    // FindC and length calculation is used,
    // because compareC does not work for undefined reason  

    if ( ext.FindC( KHtml16 ) == 0 && 
         ext.Length() == KHtml16().Length() )
        {
        mimetype = HBufC8::NewLC( KHtml().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KHtml() );
        }
    else if ( ext.FindC( KXml16 ) == 0 && 
              ext.Length() == KXml16().Length() )
        {
        mimetype = HBufC8::NewLC( KXml().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KXml() );
        }
    else if ( ext.FindC( KTxt16 ) == 0 && 
              ext.Length() == KTxt16().Length() )
        {
        mimetype = HBufC8::NewLC( KTxt().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KTxt() );
        }
    else if ( ( ext.FindC( KJpg16 ) == 0 && 
                ext.Length() == KJpg16().Length() ) ||
                ( ext.FindC( KJpeg16 ) == 0 && 
                  ext.Length() == KJpeg16().Length() ) )
        {
        mimetype = HBufC8::NewLC( KJpg().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KJpg() );
        }       
    else if ( ext.FindC( KGif16 ) == 0 && 
              ext.Length() == KGif16().Length() )
        {
        mimetype = HBufC8::NewLC( KGif().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KGif() );
        }   
    else if ( ext.FindC( KPng16 ) == 0 && 
              ext.Length() == KPng16().Length() )
        {
        mimetype = HBufC8::NewLC( KPng().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KPng() );
        }
    else if ( ( ext.FindC( KMpg16 ) == 0 &&
                ext.Length() == KMpg16().Length() ) ||
                ( ext.FindC( KMpeg16 ) == 0 &&
                  ext.Length() == KMpeg16().Length() ) )
        {
        mimetype = HBufC8::NewLC( KMpeg().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KMpeg() );
        }
    else if ( ( ext.FindC( KMpeg416 ) == 0 && 
                ext.Length() == KMpeg416().Length() ) )
        {
        mimetype = HBufC8::NewLC( KMpeg4().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KMpeg4() );
        }
    else if ( ext.FindC( KAvi16 ) == 0 && 
              ext.Length() == KAvi16().Length() )
        {
        mimetype = HBufC8::NewLC( KAvi().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KAvi() );
        }
    else if ( ext.FindC( KMp316 ) == 0 && 
              ext.Length() == KMp316().Length() )
        {
        mimetype = HBufC8::NewLC( KMp3().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KMp3() );
        }
    else if ( ext.FindC( KXHtml16 ) == 0 && 
              ext.Length() == KXHtml16().Length() )
        {
        mimetype = HBufC8::NewLC( KXHtml().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KXHtml() );
        }
    else if ( ext.FindC( KAac16 ) == 0 && 
              ext.Length() == KAac16().Length() )
        {
        mimetype = HBufC8::NewLC( KAac().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KAac() );
        }
    else if ( ext.FindC( KWma16 ) == 0 && 
              ext.Length() == KWma16().Length() )
        {
        mimetype = HBufC8::NewLC( KWma().Length() );
        mimetype->Des().Zero();
        mimetype->Des().Append( KWma() );
        }
    else
        {
        // Mime type not recognized. Return NULL.        
        }

    if( mimetype )
        {
        CleanupStack::Pop( mimetype );
        }

    return mimetype;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::ResolveFileTypeL
// Resolves the MIME type of a file. Leaves in error cases.
// --------------------------------------------------------------------------
EXPORT_C TUPnPItemType UPnPCommonUtils::ResolveFileTypeL(
                                                    const TDesC& aFileName )
    {
    TUPnPItemType returnValue = ETypeOther;

    if( aFileName != KNullDesC )
        {

        HBufC8* mimetype = NULL;
        mimetype = ResolveMimeTypeL( aFileName );
        if( mimetype )
            {
            CleanupStack::PushL( mimetype );

            if( mimetype->Des().Find( KMimeImage() ) >= 0 )
                {
                returnValue = ETypeImage;
                }
            else if( mimetype->Des().Find( KMimePlaylist() ) >= 0 )
                {
                returnValue = ETypePlaylist;
                }
            else if( mimetype->Des().Find( KMimeAudio() ) >= 0 )
                {
                returnValue = ETypeAudio;
                }
            else if( mimetype->Des().Find( KMimeVideo() ) >= 0 )
                {
                returnValue = ETypeVideo;
                }
            else
                {
                returnValue = ETypeOther;
                }
    
            // Clean up
            CleanupStack::PopAndDestroy( mimetype );
            mimetype = NULL;
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

//---------------------------------------------------------------------------
// UPnPCommonUtils::ReplacePlaceHolderInURIL
// Replaces IP address and port number placeholders of the RES element's URI
// string of the given item.
//---------------------------------------------------------------------------
//
EXPORT_C void UPnPCommonUtils::ReplacePlaceHolderInURIL( 
                                            CUpnpItem& aItem,
                                            const TInetAddr& aAddress )
    {
    __LOG( "UPnPCommonUtils::ReplacePlaceHolderInURIL" );
    
    const RUPnPElementsArray& elms = aItem.GetElements();
    
    TInt count = elms.Count();
    TInt i = 0;
    for( ; i < count; i++)
        {
        if( elms[ i ]->Name() == KElementRes )
            {
            TPtrC8 ptrC( elms[ i ]->Value() );

            HBufC8* addrBuf = HBufC8::NewLC( KBufferLength );
            addrBuf->Des().Append( KHttpDes );
            TBuf8<KBufferLengthSmall> buf;
            TChar separator( KCodeSemicolon );
            TChar dot( KCodeDot );
            TUint32 addr = aAddress.Address();
            buf.Num( addr >> 24 ); // First part of the ip address
            addrBuf->Des().Append( buf );
            addrBuf->Des().Append( dot );
            buf.Num( ( addr >> 16 ) & KMask ); // Second part of ip address
            addrBuf->Des().Append( buf );
            addrBuf->Des().Append( dot );
            buf.Num( ( addr >> 8 ) & KMask ); // Third part of the ip address
            addrBuf->Des().Append( buf );
            addrBuf->Des().Append( dot );
            buf.Num( addr & KMask ); // last part of the ip address
            addrBuf->Des().Append( buf );
            addrBuf->Des().Append( separator );

            buf.Num( aAddress.Port() ); // port number
            addrBuf->Des().Append( buf );
            
            // Append rest of the uri
            addrBuf->Des().Append( ptrC.Mid( KPlaceHolderLength ) );
            
            __LOG8( *addrBuf );
            
            elms[ i ]->SetValueL( *addrBuf );
                        
            CleanupStack::PopAndDestroy( addrBuf );
            i = count;
            }
        }    
    }
    
//---------------------------------------------------------------------------
// UPnPCommonUtils::ReplaceIllegalFilenameCharactersL
// Replaces the illegal filename characters from the given filename string
// and returns a new fixed filename string. 8bit version.
//---------------------------------------------------------------------------
//
EXPORT_C HBufC8* UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
                                        const TDesC8& aFileName )
    {
    // Note that this implementation is a bit slow since it goes through the 
    // descriptor many times. Only one iteration would be optimal solution.
    HBufC8* buf = HBufC8::NewLC( aFileName.Length() );
    TPtr8 ptr = buf->Des();
    ptr.Copy( aFileName );
    
    UpnpString::StringReplaceInPlaceL( ptr, 
                                       KProk(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KKorp(),
                                       KUnderScore() );        
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KQuotation(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KDoubleBackSlash(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr, 
                                       KSlash(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KStar(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KPipe(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KColon(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KQuestionMark(),
                                       KUnderScore() );
    UpnpString::StringReplaceInPlaceL( ptr,
                                       KTab(),
                                       KUnderScore() );
    CleanupStack::Pop(); // Can't pop by name, name invalid now
    return buf;
    }    

//---------------------------------------------------------------------------
// UPnPCommonUtils::ReplaceIllegalFilenameCharactersL
// Replaces the illegal filename characters from the given filename string
// and returns a new fixed filename string. 16bit version.
//---------------------------------------------------------------------------
//
EXPORT_C HBufC* UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
                                        const TDesC& aFilename )
    {
    HBufC8* convertedParam = NULL;
    convertedParam = UpnpString::FromUnicodeL( aFilename );
    CleanupStack::PushL( convertedParam );
    
    HBufC8* replacedFilename8 = NULL;
    replacedFilename8 = ReplaceIllegalFilenameCharactersL( *convertedParam );
    CleanupStack::PushL( replacedFilename8 );

    HBufC* replacedFilename16 = NULL;
    replacedFilename16 = UpnpString::ToUnicodeL( *replacedFilename8 );
    
    // Clean up
    CleanupStack::PopAndDestroy( replacedFilename8 );
    CleanupStack::PopAndDestroy( convertedParam );
                                            
    return replacedFilename16;
    }    

//---------------------------------------------------------------------------
// UPnPCommonUtils::IsPlaylistContainerL
// Checks if the given CUpnpObject is a playlist container.
//---------------------------------------------------------------------------
//
EXPORT_C TBool UPnPCommonUtils::IsPlaylistContainerL( 
                                        const CUpnpObject& aContainer )
    {
    TBool returnValue = EFalse;
    if( aContainer.ObjectType() != EUPnPContainer ||
        aContainer.ObjectClass() == KNullDesC8 )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        if( aContainer.ObjectClass().CompareF( KClassPlaylist() ) == 0 )
            {
            returnValue = ETrue;
            }        
        }
    return returnValue;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::FindContainerByTitle
// Finds container from CUpnpContainerList by title field
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpContainer* UPnPCommonUtils::FindContainerByTitle(
    const CUpnpContainerList& aList, const TDesC8& aTitle )
    {
    TBool notFound( ETrue );
    TInt index( 0 );
    CUpnpContainer* tmp( NULL );

    while ( index < aList.ObjectCount() && notFound )
        {
        tmp = static_cast< CUpnpContainer* >
            ( aList[ index ] );
        if ( tmp->Title().Compare( aTitle ) == 0 )
            {
            notFound = EFalse;
            }
        else
            {
            tmp = NULL;
            }
        index++;
        }
    return tmp;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::IntToDesC8LC
// Returns IntValue for aObject
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* UPnPCommonUtils::IntToDesC8LC( 
    const TInt& aObject )
    {
    HBufC8* buf = HBufC8::NewLC(12);
    TPtr8 ptrBuf = buf->Des();

    ptrBuf.Num(aObject);

    return buf;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::DesC8ToInt
// Returns IntValue for aObject
// --------------------------------------------------------------------------
//
EXPORT_C TInt UPnPCommonUtils::DesC8ToInt( 
    const TDesC8& aObject )
    {
    TLex8 lex( aObject );

    TInt lValue( KErrArgument );

    lex.Val( lValue );
    return lValue;
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::TTimeToUPnPDateL
// see upnpcommonutils.h
// --------------------------------------------------------------------------
EXPORT_C HBufC* UPnPCommonUtils::TTimeToUPnPDateL( TTime aTime )
    {
    TBuf<KMaxDateStringLength> date;        
    aTime.FormatL( date, KUpnpDateString );

    return date.AllocL();
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::RenameFileL
// see upnpcommonutils.h
// --------------------------------------------------------------------------
EXPORT_C HBufC* UPnPCommonUtils::RenameFileL( const TDesC& aFilePath )
    {
    
    //get the file extension include '.',e.g .jpg
    TInt dot = aFilePath.LocateReverse( KCodeDot );
    HBufC* extension = NULL;
    if( dot > 0 ) //if there is extension
        {
        extension = aFilePath.Right( aFilePath.Length() - dot ).AllocL();
        CleanupStack::PushL( extension );
        }
    else //if there is no extension
        {
        dot = aFilePath.Length();
        }
    // get the file path without extension: http://1.2.3//test
    HBufC* leftString = aFilePath.Left( dot ).AllocL(); 
    CleanupStack::PushL( leftString );
    
    //rename it: http:://1.2.3//test_upnptemp.jpg
    HBufC* name_upnpfwtemp = NULL;
    if( extension )
        {
        name_upnpfwtemp = HBufC::NewL( leftString->Length() + 
                                          KTemp().Length() + 
                                          extension->Length() );    
        }
    else
        {
        name_upnpfwtemp = HBufC::NewL( leftString->Length() + 
                                       KTemp().Length() );
        }
                            
    name_upnpfwtemp->Des().Append( *leftString );
    name_upnpfwtemp->Des().Append( KTemp() );
    
    if( extension )
        {
        name_upnpfwtemp->Des().Append( *extension );
        }
    
    
    CleanupStack::PopAndDestroy( leftString );
    
    if( extension )
        {
        CleanupStack::PopAndDestroy( extension );
        }
    
    
    CleanupStack::PushL( name_upnpfwtemp );
    
    // try to rename the file
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); // connect session
    CleanupClosePushL( fsSession );

    TInt error = fsSession.Rename( aFilePath, *name_upnpfwtemp );

    CleanupStack::PopAndDestroy( &fsSession );
    CleanupStack::Pop( name_upnpfwtemp ); 
    
    if( KErrNone == error )
        {
        return name_upnpfwtemp; //transfer ownership
        }
    else
        {
        delete name_upnpfwtemp;
        return NULL;
        }
    
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::DeleteTempFilesL
// see upnpcommonutils.h
// --------------------------------------------------------------------------
EXPORT_C void UPnPCommonUtils::DeleteTempFilesL()
    {
    __LOG("UPnPCommonUtils::\
DeleteTempFilesL begin");

    RFs fs;
    User::LeaveIfError( fs.Connect() ); // connect session
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );
    _LIT( KAnyChar, "*");
    _LIT( KAnyExtension, "*.*");
    _LIT( KUpnpUploadTempDirectory, "Download\\Media\\temp\\" );

    // clean phone memory
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtrC mcDrive( PathInfo::MemoryCardRootPath() );
    path->Des().Append( mcDrive );
    path->Des().Append( KUpnpUploadDirectory );
    path->Des().Append( KAnyChar );
    path->Des().Append( KTemp );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );
    
    path->Des().Zero();
    path->Des().Append( mcDrive );
    path->Des().Append( KUpnpUploadTempDirectory );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );
    CleanupStack::PopAndDestroy( path );

    // clean memory card
    path = HBufC::NewLC( KMaxFileName );
    TPtrC phoneDrive( PathInfo::PhoneMemoryRootPath() );
    path->Des().Append( phoneDrive );
    path->Des().Append( KUpnpUploadDirectory );
    path->Des().Append( KAnyChar );
    path->Des().Append( KTemp );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );
    
    path->Des().Zero();
    path->Des().Append( phoneDrive );
    path->Des().Append( KUpnpUploadTempDirectory );
    path->Des().Append( KAnyExtension );
    fileMan->Delete( *path );
    CleanupStack::PopAndDestroy( path );

    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy( &fs );

    __LOG("UPnPCommonUtils::\
DeleteTempFilesL end");
    }

// --------------------------------------------------------------------------
// UPnPCommonUtils::IsImageSupported
// see upnpcommonutils.h
// --------------------------------------------------------------------------    
 EXPORT_C TBool UPnPCommonUtils::IsImageSupported( const TDesC8& aProtocolInfo )
	{
	// simple check
	if( aProtocolInfo.Find( KImageSupport ) >= 0 )
		{
		return ETrue;
		}
		
		
	// check mime types	
	if( aProtocolInfo.FindC( KImageJpeg ) > 0 )
		{
		return ETrue;
		}
		
	if( aProtocolInfo.FindC( KImagePng ) > 0 )
		{
		return ETrue;
		}
	
	return EFalse;
	}
	
	
// --------------------------------------------------------------------------
// UPnPCommonUtils::IsAudioSupported
// see upnpcommonutils.h
// --------------------------------------------------------------------------
EXPORT_C TBool UPnPCommonUtils::IsAudioSupported( const TDesC8& aProtocolInfo )
	{
	// simple check
	if( aProtocolInfo.Find( KAudioSupport ) >= 0 )
		{
		return ETrue;
		}
		
	
	// check mime types	
	if( aProtocolInfo.FindC( KAudioAc3 ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioAmr ) > 0 )
		{
		return ETrue;
		}

	if( aProtocolInfo.FindC( KAudioAtra ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioLpcm ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioMp3 ) > 0 )
		{
		return ETrue;
		}

	if( aProtocolInfo.FindC( KAudioAac ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioHeaac ) > 0 )
		{
		return ETrue;
		}

	if( aProtocolInfo.FindC( KAudioBsac ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioWmab ) > 0 )
		{
		return ETrue;
		}
			
	if( aProtocolInfo.FindC( KAudioWmaf ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KAudioWmap ) > 0 )
		{
		return ETrue;
		}
	

	return EFalse;
	}
	
	

	
// --------------------------------------------------------------------------
// UPnPCommonUtils::IsVideoSupported
// see upnpcommonutils.h
// --------------------------------------------------------------------------
EXPORT_C TBool UPnPCommonUtils::IsVideoSupported( const TDesC8& aProtocolInfo )
	{
	// simple check
	if( aProtocolInfo.Find( KVideoSupport ) >= 0 )
		{
		return ETrue;
		}
		
		
	// check mime types		
	if( aProtocolInfo.FindC( KVideoMpeg ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoAvc ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoWmvm ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoVmvh ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoWmvh ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoWmvspll ) > 0 )
		{
		return ETrue;
		}
	
	if( aProtocolInfo.FindC( KVideoWmvspml ) > 0 )
		{
		return ETrue;
		}
	
	return EFalse;
	}
	
// --------------------------------------------------------------------------
// UPnPCommonUtils::FileExtensionByMimeTypeL
// See upnpfileutility.h
//---------------------------------------------------------------------------
//
EXPORT_C HBufC* UPnPCommonUtils::FileExtensionByMimeTypeL(
    const TDesC8& aMimeType )
    {
    HBufC* fileExt = NULL;

    // XHTML
    if( aMimeType.CompareC( KXHtmlMime ) == 0 )
        {
        fileExt = KXHtmlExt().AllocL();
        }
    // HTML
    else if( aMimeType.CompareC( KHtmlMime ) == 0 )
        {
        fileExt = KHtmlExt().AllocL();
        }
    // XML
    else if( aMimeType.CompareC( KXmlMime1 ) == 0 ||
             aMimeType.CompareC( KXmlMime2 ) == 0 ||
             aMimeType.CompareC( KXmlMime3 ) == 0  )
        {
        fileExt = KXmlExt().AllocL();
        }
    // TXT    
    else if( aMimeType.CompareC( KTxtMime )  == 0 ||
             aMimeType.CompareC( KTxtMime2 ) == 0 ||
             aMimeType.CompareC( KTxtMime3 ) == 0 ||
             aMimeType.CompareC( KTxtMime4 ) == 0 ||
             aMimeType.CompareC( KTxtMime5 ) == 0 ||
             aMimeType.CompareC( KTxtMime6 ) == 0  )
        {
        fileExt = KTxtExt().AllocL();
        }
    // JPEG
    else if( aMimeType.CompareC( KJpegMime ) == 0 )
        {
        fileExt = KJpegExt().AllocL();
        }
    // JPG
    else if( aMimeType.CompareC( KJpegMime2 ) == 0 ||
             aMimeType.CompareC( KJpegMime3 ) == 0 ||
             aMimeType.CompareC( KJpegMime4 ) == 0 ||
             aMimeType.CompareC( KJpegMime5 ) == 0  )
        {
        fileExt = KJpgExt().AllocL();
        }
    // GIF    
    else if( aMimeType.CompareC( KGifMime )  == 0 ||
             aMimeType.CompareC( KGifMime2 ) == 0 ||
             aMimeType.CompareC( KGifMime3 ) == 0  )
        {
        fileExt = KGifExt().AllocL();
        }
    // PNG    
    else if( aMimeType.CompareC( KPngMime )  == 0 ||
             aMimeType.CompareC( KPngMime2 ) == 0 ||
             aMimeType.CompareC( KPngMime3 ) == 0  )
        {
        fileExt = KPngExt().AllocL();
        }
    // MPG    
    else if( aMimeType.CompareC( KMpgMime1 )  == 0 ||
             aMimeType.CompareC( KMpgMime2 )  == 0 ||
             aMimeType.CompareC( KMpgMime3 )  == 0 ||
             aMimeType.CompareC( KMpgMime4 )  == 0 ||
             aMimeType.CompareC( KMpgMime5 )  == 0 ||
             aMimeType.CompareC( KMpgMime6 )  == 0 ||
             aMimeType.CompareC( KMpgMime7 )  == 0 ||
             aMimeType.CompareC( KMpgMime10 ) == 0  )
        {
        fileExt = KMpgExt1().AllocL();
        }
    // MPEG4    
    else if( aMimeType.CompareC( KMpeg4Mime )  == 0 )
        {
        fileExt = KMpeg4Ext().AllocL();
        }
    // MP4        
    else if( aMimeType.CompareC( KMp4Mime )  == 0 ||
             aMimeType.CompareC( KMp4Mime2 ) == 0 ||
             aMimeType.CompareC( KMp4Mime3 ) == 0  )
        {
        fileExt = KMp4Ext().AllocL();
        }
    // AVI
    else if( aMimeType.CompareC( KAviMime1 ) == 0 ||
             aMimeType.CompareC( KAviMime2 ) == 0 ||
             aMimeType.CompareC( KAviMime3 ) == 0 ||
             aMimeType.CompareC( KAviMime4 ) == 0 ||
             aMimeType.CompareC( KAviMime5 ) == 0 ||
             aMimeType.CompareC( KAviMime6 ) == 0 ||
             aMimeType.CompareC( KAviMime7 ) == 0 ||
             aMimeType.CompareC( KAviMime8 ) == 0 ||
             aMimeType.CompareC( KAviMime9 ) == 0 )
        {
        fileExt = KAviExt().AllocL();
        }
    // MP3
    else if( aMimeType.CompareC( KMp3Mime1 )  == 0 ||
             aMimeType.CompareC( KMp3Mime2 )  == 0 ||
             aMimeType.CompareC( KMp3Mime3 )  == 0 ||
             aMimeType.CompareC( KMp3Mime4 )  == 0 ||
             aMimeType.CompareC( KMp3Mime5 )  == 0 ||
             aMimeType.CompareC( KMp3Mime6 )  == 0 ||
             aMimeType.CompareC( KMp3Mime7 )  == 0 ||
             aMimeType.CompareC( KMp3Mime8 )  == 0 ||
             aMimeType.CompareC( KMp3Mime9 )  == 0 ||
             aMimeType.CompareC( KMp3Mime10 ) == 0 )
        {
        fileExt = KMp3Ext().AllocL();
        }
    // AAC    
    else if( aMimeType.CompareC( KAacMime )   == 0 ||
             aMimeType.CompareC( KAacMime2 )  == 0 ||
             aMimeType.CompareC( KAacMime3 )  == 0 ||
             aMimeType.CompareC( KAacMime4 )  == 0 )
        {
        fileExt = KAacExt().AllocL();
        }
    //M4a
    else if( aMimeType.CompareC( KM4aMime )   == 0 )
       {
       fileExt = KM4aExt().AllocL();
       }
    // WMA    
    else if( aMimeType.CompareC( KWmaMime )   == 0 ||
             aMimeType.CompareC( KWmaMime2 )  == 0 )
        {
        fileExt = KWmaExt().AllocL();
        }
    // BMP
    else if( aMimeType.CompareC( KBmpMime )  == 0 ||
             aMimeType.CompareC( KBmpMime2 ) == 0 ||
             aMimeType.CompareC( KBmpMime3 ) == 0 ||
             aMimeType.CompareC( KBmpMime4 ) == 0 ||
             aMimeType.CompareC( KBmpMime5 ) == 0 ||
             aMimeType.CompareC( KBmpMime6 ) == 0 ||
             aMimeType.CompareC( KBmpMime7 ) == 0 ||
             aMimeType.CompareC( KBmpMime8 ) == 0 ||
             aMimeType.CompareC( KBmpMime9 ) == 0  )
        {
        fileExt = KBmpExt().AllocL();
        }
    // 3GP
    else if( aMimeType.CompareC( KAudio3gpp ) == 0 ||
             aMimeType.CompareC( KVideo3gpp ) == 0 ) 
        {
        fileExt = K3gpExt().AllocL();
        }
    // AMR
    else if( aMimeType.CompareC( KAudioAMR )  == 0 )
        {
        fileExt = KAmrExt().AllocL();
        }
    // WAV
    else if( aMimeType.CompareC( KAudioWav ) == 0 ||
             aMimeType.CompareC( KAudioXWav ) == 0 ||
             aMimeType.FindC( KAudioL16 ) != KErrNotFound )
        {
        fileExt = KWavExt().AllocL();
        }
    // Real audio    
    else if ( aMimeType.CompareC( KRAMime8 )  == 0 )
        {
        fileExt = KRAExt().AllocL();
        }
    // TTS    
    else if( aMimeType.CompareC( KVideoTts )  == 0 )
        {
        fileExt = KTtsExt().AllocL();
        }
    // WMV    
    else if( aMimeType.CompareC( KWmvMime )  == 0 )
        {
        fileExt = KWmvExt().AllocL();        
        }
    return fileExt;
    }

//---------------------------------------------------------------------------
// UPnPCommonUtils::FixListboxItemTextL
// Removes the tab character of the given text
// and returns a new fixed text string. 8bit version.
//---------------------------------------------------------------------------
//
EXPORT_C HBufC8* UPnPCommonUtils::FixListboxItemTextL( const TDesC8& aText )
    {
    HBufC8* fixedText = HBufC8::NewLC( aText.Length() );
    TPtr8 ptr = fixedText->Des();
    ptr.Copy( aText );
    
    UpnpString::StringReplaceInPlaceL( ptr, 
                                       KTab(),
                                       KSpace() );   
    CleanupStack::Pop(); // Can't pop by name, name invalid now
    return fixedText;
    }

//  End of File
