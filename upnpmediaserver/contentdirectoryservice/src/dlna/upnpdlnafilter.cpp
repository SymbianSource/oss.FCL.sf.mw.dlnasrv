/** @file
 * Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  CUpnpDlnaFilter implementation.
 *
 */
// INCLUDES
#include <bautils.h> 
#include "upnpdlnafilter.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include "upnpcontentdirectorydatafinder.h"
#include "upnphttpmessage.h"
#include "upnpdlnaprotocolinfo.h"
#include "upnpfileutils.h"
#include "upnpsecuritymanager.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnphttpfilereceivetransaction.h"
#include "upnphttpfileservetransaction.h"
#include "upnpdlnafilterheaders.h"
#include "upnpcommonupnplits.h"
#include "upnpdlnacorelation.h"

// CONSTANTS
_LIT8( KDlnaFilter, "DLNA");
_LIT8( KIpPortPlaceholder8, "___.___.___.___:_____" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::CUpnpDlnaFilter
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilter::CUpnpDlnaFilter( MUpnpContentDirectoryDataFinder* aFinder,
    CUpnpSecurityManager* aSecurityManager ) :
    iCdDataFinder( aFinder ), iSecurityManager( aSecurityManager )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::~CUpnpDlnaFilter
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilter::~CUpnpDlnaFilter()
    {
    iFs.Close();
    delete iProtocolInfo;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilter* CUpnpDlnaFilter::NewLC(
    MUpnpContentDirectoryDataFinder* aFinder,
    CUpnpSecurityManager* aSecurityManager )
    {
    CUpnpDlnaFilter* self = new (ELeave) CUpnpDlnaFilter( aFinder,
        aSecurityManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilter* CUpnpDlnaFilter::NewL(
    MUpnpContentDirectoryDataFinder* aFinder,
    CUpnpSecurityManager* aSecurityManager )
    {
    CUpnpDlnaFilter* self =
            CUpnpDlnaFilter::NewLC( aFinder, aSecurityManager );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::ConstructL
// EPOC default constructor for performing 2nd stage construction.
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilter::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::SecurityManager
// 
// -----------------------------------------------------------------------------
//
CUpnpSecurityManager* CUpnpDlnaFilter::SecurityManager()
    {
    return iSecurityManager;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::CheckImportUriL
// Checks if specified URI exists in database and returns object id for
//      given URI or KErrNotFound if URI is no registered in database.
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::CheckImportUriL( TDesC8& aImportUri )
    {
    TInt lastPosOfSlash = aImportUri.LocateReverse( '/' );
    if ( lastPosOfSlash < 0 )
        {
        return KErrGeneral;
        }
    HBufC8* path = aImportUri.AllocLC();
    TInt result = KErrNone;
    result = iCdDataFinder->CheckImportUriL( path->Des() );
    CleanupStack::PopAndDestroy( path );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::Get3rdFieldFromCdL
// Find protocolInfo by contentUri (Not by importUri) and extract 3rd field,
//      using ContentDirectory.
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpDlnaFilter::ThirdFieldFromCdL( const TDesC8& aContentUri )
    {
    HBufC8* result = NULL;
    CUpnpDlnaProtocolInfo* protocolInfo = ProtocolInfoL( aContentUri );
    if ( protocolInfo )
        {
        TPtrC8 thirdField = protocolInfo->ThirdField();
        if ( thirdField != KNullDesC8() )
            {
            result = thirdField.AllocL();
            }
        }
    delete protocolInfo;
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::ProtocolInfoL
// 
// -----------------------------------------------------------------------------
//
CUpnpDlnaProtocolInfo* CUpnpDlnaFilter::ProtocolInfoL( const TDesC8& aContentUri )
    {
    //  "http:/:/" prefix is added to content path to provide backward 
    // compatibility with old testing tools
    HBufC8* fullContentUri = HBufC8::NewL( UpnpHTTP::KHTTPUrl().Length()
            + KIpPortPlaceholder8().Length() + aContentUri.Length() );
    CleanupStack::PushL( fullContentUri );
    
    fullContentUri->Des().Append( UpnpHTTP::KHTTPUrl );
    fullContentUri->Des().Append( KIpPortPlaceholder8 );
    fullContentUri->Des().Append( aContentUri );
    
    CUpnpDlnaProtocolInfo* protocolInfo = NULL;
    if ( iCdDataFinder->GetProtocolInfoL( *fullContentUri, protocolInfo ) != KErrNone )
        {
        delete protocolInfo;
        protocolInfo = NULL;
        }
    CleanupStack::PopAndDestroy( fullContentUri );
    return protocolInfo;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::GetMediaFileNameL
// Gets name of file with content for given object's id.
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilter::GetMediaFileNameL( TInt aObjectId, TPtr& aFileName )
    {
    if ( aObjectId <= KErrNone )
        {
        return;
        }
    iCdDataFinder->GetTitleForUriL( aObjectId, aFileName );
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::FindSharedFolderDBL
// Find a folder shared from DB (ContentDirectory).
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::FindSharedFolderDBL( const TDesC8& aUrlPath,
    const TDesC8& aFileName, HBufC8*& aSystemPath )
    {
    HBufC* unicodeSharedFolder = NULL;
    TInt result = KErrNone;
    HBufC16* urlPathUnicode = UpnpString::ToUnicodeL( aUrlPath );
    CleanupStack::PushL( urlPathUnicode );
    HBufC16* fileNameUnicode = UpnpString::ToUnicodeL( aFileName );
    CleanupStack::PushL( fileNameUnicode );

    result = iCdDataFinder->FindSharedFolderL( *urlPathUnicode,
        *fileNameUnicode, unicodeSharedFolder );
    if ( unicodeSharedFolder )
        {
        CleanupStack::PushL( unicodeSharedFolder );
        HBufC8* sharedFolder =
                UpnpString::FromUnicodeL( *unicodeSharedFolder );
        CleanupStack::PopAndDestroy( unicodeSharedFolder );
        aSystemPath = sharedFolder;
        }

    CleanupStack::PopAndDestroy( fileNameUnicode );
    CleanupStack::PopAndDestroy( urlPathUnicode );
    return result;
    }

// -----------------------------------------------------------------------------
//  CUpnpDlnaFilter::CheckDLNAPostCorrelationsL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::CheckDLNAPostCorrelationsL( CUpnpHttpFileReceiveTransaction& aTransaction )
    {
    TPtrC8 contentURI = aTransaction.SenderUri();
    HBufC8* decodedContentURI = HBufC8::NewLC( contentURI.Length() );
    TPtr8 ptrDecodedContentURI = decodedContentURI->Des();
    ptrDecodedContentURI.Copy( contentURI );
    UpnpString::ReplaceHttpCharacters( ptrDecodedContentURI );

    TBool streamingSupport = 0;
    TBool interactiveSupport = 0;

    CUpnpDlnaProtocolInfo* protocolInfo = NULL;
    TRAPD( error, protocolInfo =
            iCdDataFinder->GetProtocolInfoByImportUriL( *decodedContentURI ) );
    CleanupStack::PopAndDestroy( decodedContentURI );
    if ( error )
        {
        if ( error == ERestrictedObject || error == ERestrictedParentObject )
            {
            return -EHttpForbidden;
            }
        else if ( error == ENoSuchObject )
            {
            return -EHttpNotFound;
            }
        else
            {
            return -EHttpBadRequest;
            }
        }
    CleanupStack::PushL( protocolInfo );
    if ( protocolInfo->FourthField().Find( KDlnaFilter ) == KErrNotFound )
        {
        aTransaction.FilterHeaders().RemoveHeaderL( UpnpDLNA::KHdrTransferMode );
        CleanupStack::PopAndDestroy( protocolInfo );
        return KErrNone;
        }
    else
        {
        streamingSupport = protocolInfo->DlnaFlag(
            UpnpDlnaProtocolInfo::TM_S_FLAG );
        interactiveSupport = protocolInfo->DlnaFlag(
            UpnpDlnaProtocolInfo::TM_I_FLAG );
        CleanupStack::PopAndDestroy( protocolInfo );
        }

    TDesC8& transferMode = aTransaction.FilterHeaders().QueryHeader(
                           UpnpDLNA::KHdrTransferMode );

    if ( transferMode.Length() > 0 )
        {
        if ( (transferMode.CompareC( UpnpDLNA::KTransferModeStreaming ) == 0
                && !streamingSupport) || (transferMode.CompareC(
            UpnpDLNA::KTransferModeInteractive ) == 0 && !interactiveSupport) )
            {
            return -EHttpNotAcceptable;
            }

        if ( transferMode.CompareC( UpnpDLNA::KTransferModeStreaming ) != 0
                && transferMode.CompareC( UpnpDLNA::KTransferModeInteractive )
                        != 0 && transferMode.CompareC(
            UpnpDLNA::KTransferModeBackground ) != 0 )
            {
            return -EHttpBadRequest;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
//  CUpnpDlnaFilter::CheckDLNACorrelations
//
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::CheckDLNACorrelationsL( CUpnpHttpFileServeTransaction& aTransaction )
    {
    TPtrC8 contentURI = aTransaction.SenderUri() ;
    HBufC8* decodedContentURI = HBufC8::NewL( contentURI.Length() );
    TPtr8 ptrDecodedContentURI = decodedContentURI->Des();
    ptrDecodedContentURI.Copy( contentURI );
    UpnpString::ReplaceHttpCharacters( ptrDecodedContentURI );
    CleanupStack::PushL( decodedContentURI );
    delete iProtocolInfo;
    iProtocolInfo = NULL;
    iProtocolInfo = ProtocolInfoL( *decodedContentURI );
    CleanupStack::PopAndDestroy( decodedContentURI );    
    TPtrC8 fourthField( KNullDesC8 );
    if ( iProtocolInfo )
        {
        fourthField.Set( iProtocolInfo->FourthField() );
        }

    if ( fourthField.Find( KDlnaFilter ) == KErrNotFound )
        {
        aTransaction.FilterHeaders().RemoveHeaderL( UpnpDLNA::KHdrTransferMode );
        return KErrNone;
        }

    TUpnpDlnaCorelation dlnaCorelation;
    dlnaCorelation.iFourthField.Set( fourthField );

    TInt corelationError = CheckCorelationL( aTransaction, dlnaCorelation );
    if ( corelationError != KErrNone )
        {
        return corelationError;
        }
    corelationError = CheckTransferModeL( aTransaction, dlnaCorelation );
    if ( corelationError != KErrNone )
        {
        return corelationError;
        }    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::CheckCorelationL
// -----------------------------------------------------------------------------
//    
TInt CUpnpDlnaFilter::CheckCorelationL(  CUpnpHttpFileServeTransaction& aTransaction,
                                         TUpnpDlnaCorelation& aDlnaCorelation )
    {
    //-------------Checking DLNA correlations, response with HTTPerror if some problem occurs
    // DLNA v0.75, 7.3.31.1 checking Operations Parameter    
    if (
    // We don't support timeSeek and playSpeed at the moment, so according to point 7.4.71.1 (ver 1.5 rev 0.96)
    ((aTransaction.QueryRequestHeader( UpnpDLNA::KHdrTimeSeekRange ) != KNullDesC8()
            || aTransaction.QueryRequestHeader( UpnpDLNA::KHdrPlaySpeed )
                    != KNullDesC8()) &&
    // 7.4.71.2 - range takes precedence
                    aTransaction.QueryRequestHeader( UpnpHTTP::KHdrRange ) == KNullDesC8())
            ||
            //
            // or if request mode is Streaming and
            (aTransaction.QueryRequestHeader( UpnpDLNA::KHdrTransferMode ). CompareC(
                UpnpDLNA::KTransferModeStreaming ) == 0 && (
            // if we have no protocolInfo (because for example there is no cdDataFinder )
                    !iProtocolInfo ||
                    // Streaming is not supported for this content type
                            iProtocolInfo -> DlnaFlag(
                                UpnpDlnaProtocolInfo::TM_S_FLAG ) == EFalse)) )
        {
        // we respond with 406 error code - Not Acceptable.
        return -EHttpNotAcceptable;
        }

    aDlnaCorelation.iStreamingSupport = EFalse;
    aDlnaCorelation.iInteractiveSupport = EFalse;
    aDlnaCorelation.iBackgrondSupport = EFalse;
    aDlnaCorelation.iGetContentFeaturesExist = EFalse;
    aDlnaCorelation.iGetContentFeaturesIsOK = ETrue;
    
    // We can only check for getcontentFeaturesExist if we have cdDataFinder ( protocolInfo is not null )
    if ( iProtocolInfo )
        {
        aDlnaCorelation.iBackgrondSupport = 1;
        aDlnaCorelation.iStreamingSupport = iProtocolInfo->DlnaFlag(
            UpnpDlnaProtocolInfo::TM_S_FLAG );
        aDlnaCorelation.iInteractiveSupport = iProtocolInfo->DlnaFlag(
            UpnpDlnaProtocolInfo::TM_I_FLAG );
        if( aTransaction.QueryRequestHeader( UpnpDLNA::KHdrGetcontentFeatures() ) != KNullDesC8() )
            {
            aDlnaCorelation.iGetContentFeaturesExist = ETrue;
            }
        if( aDlnaCorelation.iGetContentFeaturesExist )
            {
            TDesC8& cntHeaderValue = aTransaction.QueryRequestHeader(
                                 UpnpDLNA::KHdrGetcontentFeatures() );
            if ( cntHeaderValue != UpnpDLNA::KHdrGetcontentFeaturesValue() )
                {
                aDlnaCorelation.iGetContentFeaturesIsOK = EFalse;
                }
            }
        }
    
    // Append contentFeatures.dlna.org
    if ( aDlnaCorelation.iGetContentFeaturesExist )
        {
        if ( aDlnaCorelation.iFourthField.Length() > 0 )
            {
            aTransaction.FilterHeaders().AddHeaderL( UpnpDLNA::KHdrContentFeatures,
                                                     aDlnaCorelation.iFourthField );
            }
        }
    return KErrNone;    
    }        
        
// -----------------------------------------------------------------------------
// CUpnpHttpServer::CheckTransferMode
// -----------------------------------------------------------------------------
//        
TInt CUpnpDlnaFilter::CheckTransferModeL( CUpnpHttpFileServeTransaction& aTransaction,
                                         TUpnpDlnaCorelation& aDlnaCorelation )
    {
     TDesC8& transferMode = aTransaction.QueryRequestHeader( UpnpDLNA::KHdrTransferMode );
    // Check if requested transfer mode is handled
    if (
    // if client requested for transfer is not empty
    (transferMode.Length() > 0 && (
    // and if client requested for transfer is different than Background, Streaming or Interactive mode, reply with 400 error
            (transferMode.CompareC( UpnpDLNA::KTransferModeStreaming ) != 0
                    && transferMode.CompareC(
                        UpnpDLNA::KTransferModeInteractive ) != 0
                    && transferMode.CompareC(
                        UpnpDLNA::KTransferModeBackground ) != 0) ||
            // If Background or Interactive mode was requested, check if request doesn;t contain forbidden headers
                    ((transferMode.CompareC(
                        UpnpDLNA::KTransferModeBackground ) == 0
                            || transferMode.CompareC(
                                UpnpDLNA::KTransferModeInteractive ) == 0)
                            && (aTransaction.QueryRequestHeader(
                                UpnpDLNA::KHdrTimeSeekRange ) != KNullDesC8()
                                    || aTransaction.QueryRequestHeader(
                                        UpnpDLNA::KHdrPlaySpeed )
                                            != KNullDesC8()
                                    || aTransaction.QueryRequestHeader(
                                        UpnpDLNA::KHdrRealTimeInfo )
                                            != KNullDesC8())))) || (
    
        aTransaction.QueryRequestHeader( UpnpHTTP::KHdrRange ) == KNullDesC8()
                && transferMode.Length() == 0 && (aTransaction.QueryRequestHeader(
            UpnpDLNA::KHdrTimeSeekRange ) != KNullDesC8()
                || aTransaction.QueryRequestHeader( UpnpDLNA::KHdrPlaySpeed )
                        != KNullDesC8() || aTransaction.QueryRequestHeader(
            UpnpDLNA::KHdrRealTimeInfo ) != KNullDesC8()))
                || !aDlnaCorelation.iGetContentFeaturesIsOK
    
        )
            {
            return -EHttpBadRequest ;
            }
    
    return AppendCorelationHeadersL( aTransaction, aDlnaCorelation, transferMode );    
    }    

// -----------------------------------------------------------------------------
// CUpnpHttpServer::AppendCorelationHeaders
// -----------------------------------------------------------------------------
//        
TInt CUpnpDlnaFilter::AppendCorelationHeadersL( CUpnpHttpFileServeTransaction& aTransaction,
                             TUpnpDlnaCorelation& aDlnaCorelation, TDesC8& aTransferMode )
    {
    if ( aTransferMode.Length() > 0 && ((aTransferMode.CompareC(
        UpnpDLNA::KTransferModeStreaming ) == 0 && !aDlnaCorelation.iStreamingSupport)
            || (aTransferMode.CompareC( UpnpDLNA::KTransferModeInteractive )
                    == 0 && !aDlnaCorelation.iInteractiveSupport)) )
        {
        return -EHttpNotAcceptable ;
        }
    else if ( aTransferMode.Length() <= 0 )
        {
        if ( aDlnaCorelation.iStreamingSupport )
            {
            aTransaction.FilterHeaders().AddHeaderL( UpnpDLNA::KHdrTransferMode,
                    UpnpDLNA::KTransferModeStreaming );
            }
        else if ( aDlnaCorelation.iInteractiveSupport )
            {
            aTransaction.FilterHeaders().AddHeaderL( UpnpDLNA::KHdrTransferMode,
                UpnpDLNA::KTransferModeInteractive );
            }
        else if ( aDlnaCorelation.iBackgrondSupport )
            {
            aTransaction.FilterHeaders().AddHeaderL( UpnpDLNA::KHdrTransferMode,
                UpnpDLNA::KTransferModeBackground );
            }
        }
     return KErrNone;   
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::FormatPathL
//
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilter::FormatPathL( CUpnpHttpFileServeTransaction *aTransaction, TDes &aPath )
    {
    LOGS( "%i, CUpnpHttpSession::FormatPathL " );

    TPtrC8 contentURI = aTransaction->SenderUri();

    HBufC8* decodedContentURI = HBufC8::NewL( contentURI.Length() );
    TPtr8 ptrDecodedContentURI = decodedContentURI->Des();
    ptrDecodedContentURI.Copy( contentURI );
    UpnpString::ReplaceHttpCharacters( ptrDecodedContentURI );
    CleanupStack::PushL( decodedContentURI );

    //extracting URLpath (so removing IP, port )
    TPtrC8 fileName;
    TInt parseError( KErrNone );
    TPtrC8 urlPath = UpnpFileUtil::ExtractUrlPath( ptrDecodedContentURI,
        fileName, parseError );
    if ( parseError )
        {
        User::Leave( -EHttpBadRequest );
        }

    HBufC8* sharedFolder = NULL;
    TInt error = FindSharedFolderDBL( urlPath, fileName, sharedFolder );
    CleanupStack::PushL( sharedFolder );            
    if ( error == KErrNotFound || !sharedFolder )
        {
        User::Leave( -EHttpNotFound );
        }
    
    //sharedFolder includes file name  
    ASSERT( (*sharedFolder).Mid( (*sharedFolder).Length()
            - UpnpString::KDoubleBackSlash().Length() )
            != UpnpString::KDoubleBackSlash() );

    HBufC* path16 = UpnpString::ToUnicodeL( *sharedFolder );
    aPath.Copy( *path16 );
    delete path16;
    
    CleanupStack::PopAndDestroy( sharedFolder );
    CleanupStack::PopAndDestroy( decodedContentURI );
    }


// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::GetContentTypeL()
// Retrieves a mime type from the third field of the protocol info read from CD
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::GetContentTypeL( CUpnpHttpFileServeTransaction &aTransaction,
    HBufC8*& aMime, const TDesC16& aFilename )
    {
    TInt error = KErrNone;

    TParse parse;
    parse.Set( aFilename, NULL, NULL );
    TBufC16<KMaxName> ext( parse.Ext() );
    //XML mime type has to be set seperately
    if ( ext.FindC( KXml16 ) == 0 && ext.Length() == KXml16().Length() )
        {
        // Extension says that's XML but we check content to be sure and get encoding
        _LIT8( KXmlUtf8, "text/xml; charset=\"utf-8\"" );
        aMime = HBufC8::NewL( KXmlUtf8().Length() );
        aMime->Des().Zero();
        aMime->Des().Append( KXmlUtf8() );
        return error;
        }

    //decoding content URI
    TPtrC8 contentURI = aTransaction.SenderUri();
    HBufC8* decodedContentURI = HBufC8::NewL( contentURI.Length() );
    TPtr8 ptrDecodedContentURI = decodedContentURI->Des();
    ptrDecodedContentURI.Copy( contentURI );
    UpnpString::ReplaceHttpCharacters( ptrDecodedContentURI );
    CleanupStack::PushL( decodedContentURI );

    //getting 3rd field
    aMime = ThirdFieldFromCdL( *decodedContentURI );
    CleanupStack::PopAndDestroy( decodedContentURI );
    // asterick as mime type is not good - so error is KErrNotFound
    _LIT8( KAseriskType, "*" );
    if ( (NULL == aMime) || ((*aMime) == KAseriskType()) )
        {
        delete aMime;
        aMime = NULL;
        error = KErrNotFound;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::DetermineDownloadPathL
//
// -----------------------------------------------------------------------------
//
HBufC* CUpnpDlnaFilter::DetermineDownloadPathL(
    CUpnpHttpFileReceiveTransaction& aTransaction )
    {
    TPtrC8 path = aTransaction.SenderUri();
    TInt parseError( KErrNone );
    TPtrC8 urlPath = UpnpFileUtil::ExtractUrlPath( path, parseError );
    if ( parseError )
        {
        User::Leave( EHttpBadRequest );
        }

    // if importURI is bad then doesn't check shared folder just return error.
    TInt result = CheckImportUriL( path );
    if ( KErrNone == result )
        {
        return KNullDesC().AllocL();
        }
    if ( result < KErrNone && result != KErrGeneral )
        {
        return NULL;
        }
    HBufC* fileName = HBufC::NewLC( KMaxFileName );
    TPtr fileNamePtr( fileName->Des() );
    GetMediaFileNameL( result, fileNamePtr );
    
    HBufC8* sharedFolder = NULL;
    FindSharedFolderDBL( urlPath, KNullDesC8, sharedFolder );
    CleanupStack::PushL( sharedFolder );
    
    HBufC* folder = UpnpString::ToUnicodeL( *sharedFolder );
    CleanupStack::PushL( folder );

    if ( fileName->Length() == 0)
       {
       _LIT(KNoDcTitle, "no_dc_title");
       fileName->Des().Copy(KNoDcTitle);
       }
    
    HBufC* resultFileName = HBufC::NewL( folder->Length() + fileName->Length() );   
    resultFileName->Des().Zero();
    resultFileName->Des().Append( *folder );
    resultFileName->Des().Append( *fileName );
    
    CleanupStack::PopAndDestroy( folder );
    CleanupStack::PopAndDestroy( sharedFolder );
    CleanupStack::PopAndDestroy( fileName );
    
    CleanupStack::PushL( resultFileName );        
    HBufC* uniqueFileName = MakeFileNameUniqueL(*resultFileName, iFs );
    CleanupStack::PopAndDestroy( resultFileName );
        
    return uniqueFileName;
    }
        
// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::PrepareHeaderL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::PrepareHeaderL( CUpnpHttpFileServeTransaction& aTransaction )
    {    
    HBufC8* mimetype = NULL;
    HBufC16* fileName = aTransaction.PathWithNewMethodL();
    CleanupStack::PushL( fileName );
    
    if ( GetContentTypeL( aTransaction, mimetype, *fileName ) != KErrNone )
        {
        //getting mime type from Symbian or by the extension
        mimetype = UpnpFileUtil::GetMimeTypeForFileL( *fileName );
        }
    CleanupStack::PushL( mimetype );
    aTransaction.AddResponseHeaderL( UpnpGENA::KContentType(), *mimetype );
    CleanupStack::PopAndDestroy( mimetype );
    
    // Checks if all DLNA correlations are ok and adds proper headers
    // only for GET response

    TInt dlnaCorrelationsError = CheckDLNACorrelationsL( aTransaction );
    if ( dlnaCorrelationsError < KErrNone )
        {
        CleanupStack::PopAndDestroy( fileName );
        return dlnaCorrelationsError;
        }

    
    AddHeaderIfNotEmptyL( UpnpDLNA::KHdrContentFeatures(), aTransaction );
    
        // 7.4.42.2 HTTP Server Endpoints that transfer Non-Cacheable Content using · HTTP/1.0, and· GET responses.
        // These devices must prevent intermediate caching by including among the HTTP response headers
        // the directive:· Pragma:  no-cache
    aTransaction.AddResponseHeaderL( UpnpHTTP::KHdrPragma(),
                                     UpnpHTTP::KNoCache() );   
    aTransaction.AddResponseHeaderL( UpnpHTTP::KHdrCacheControl(),
                                     UpnpHTTP::KNoCache() );        


    // If Accept-Language header is present
    if ( aTransaction.QueryRequestHeader( UpnpHTTP::KHdrAcceptLanguage() ).Length() > 0 )
        {
        aTransaction.AddResponseHeaderL( UpnpHTTP::KHdrContentLanguage(),
                                         UpnpHTTP::KLanguageEn() );        
        }

    // Transfer Mode
    // If Transfer Mode header is present
    AddHeaderIfNotEmptyL( UpnpDLNA::KHdrTransferMode, aTransaction );

    CleanupStack::PopAndDestroy( fileName );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//  CUpnpDlnaFilter::AddHeaderIfNotEmptyL
//
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilter::AddHeaderIfNotEmptyL( const TDesC8& aHeaderName, 
    CUpnpHttpFileServeTransaction& aTransaction )
    {
    if ( aTransaction.FilterHeaders().QueryHeader( aHeaderName ).Length() > 0 )
        {
        aTransaction.AddResponseHeaderL( aHeaderName,
            aTransaction.FilterHeaders().QueryHeader( aHeaderName ) );            
        }
    }

// -----------------------------------------------------------------------------
//  CUpnpDlnaFilter::NewTransactionL
//
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilter::NewTransactionL( const TDesC8& aMethod, const TDesC8& aUri, 
    const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans )
    {
    if ( aMethod == KHttpPost() )
        {
        aResultTrans = CUpnpHttpFileReceiveTransaction::NewL( *this, aSender, aUri );
        }
    else
        {
        aResultTrans = CUpnpHttpFileServeTransaction::NewL( *this, aSender, aUri );
        }
    }

// -----------------------------------------------------------------------------
//  CUpnpDlnaFilter::AuthorizeRequestL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpDlnaFilter::AuthorizeRequestL(
    const TDesC& aFileName, const TInetAddr& aSender )
    {
    TInt result( KErrNone );
    CUpnpHttpMessage* tempMessage = CUpnpHttpMessage::NewL( aSender );
    CleanupStack::PushL( tempMessage );
    if ( SecurityManager()
            && SecurityManager()->AuthorizeMessage( tempMessage,
                    (TFileName&) aFileName  ) != KErrNone )
        { //not showing why resource is refused
        result = -EHttpNotFound;
        }
    CleanupStack::PopAndDestroy( tempMessage );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::FileSession
// -----------------------------------------------------------------------------
//
RFs& CUpnpDlnaFilter::FileSession()
    {
    return iFs;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::PreparePostfixToMakeFileUniqueL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpDlnaFilter::PreparePostfixToMakeFileUniqueL( const TDesC& aFilename, RFs& aFs )
    {
    _LIT( KUnderScore, "_" );
    const TInt KDotLength = 1;  // when aFileName is without extension

    HBufC16* newFile = HBufC16::NewLC( aFilename.Length() + KUnderScore().Length() 
                                       + UpnpString::KMaxTUintLength + KDotLength );

    TParse parse;
    parse.Set( aFilename, NULL, NULL );
    TPtrC fileName = parse.Name();
    TPtrC fileExt = parse.Ext();
    TPtrC filePath = parse.DriveAndPath();

    TUint64 postfixNumber = 1;

    do
        {
        TBuf<UpnpString::KMaxTUintLength + 1> buf;
        buf.AppendNum( postfixNumber++ );
        if ( buf.Length() > UpnpString::KMaxTUintLength )
            {
            User::Leave( KErrOverflow );
            }
        newFile->Des().Copy( filePath );
        newFile->Des().Append( fileName );
        newFile->Des().Append( KUnderScore() );
        newFile->Des().Append( buf );
        newFile->Des().Append( fileExt );
        }
    while ( BaflUtils::FileExists( aFs, newFile->Des() ) );

    CleanupStack::Pop( newFile );
    return newFile;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::PrepareBaseFileNameL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpDlnaFilter::PrepareBaseFileNameL( const TDesC& aFilename, RFs& aFs )
    {
    HBufC* fileToServe;
    if ( aFilename.LocateReverse( '\\' ) == aFilename.Length() - 1 )
        {
        _LIT( KNoName0, "noName_0" );
        const TInt KZeroPostfixLen = 2;  //_0
        
        fileToServe = HBufC::NewL( aFilename.Length() + KNoName0().Length() );
        fileToServe->Des().Copy( aFilename );
        fileToServe->Des().Append( KNoName0() );
        
        if ( BaflUtils::FileExists( aFs, *fileToServe ) )
            {
            fileToServe->Des().Delete( fileToServe->Length() - KZeroPostfixLen, KZeroPostfixLen );
            }
        }
    else
        {
        fileToServe = HBufC::NewL( aFilename.Length() );
        fileToServe->Des().Copy( aFilename );
        }

    if ( BaflUtils::FileExists( aFs, *fileToServe ) )
        {
        CleanupStack::PushL( fileToServe );
        HBufC* newFileName = PreparePostfixToMakeFileUniqueL( *fileToServe, aFs );
        CleanupStack::PopAndDestroy( fileToServe );
        fileToServe = newFileName;
        }
    return fileToServe;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDlnaFilter::MakeFileNameUniqueL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpDlnaFilter::MakeFileNameUniqueL( const TDesC& aFilename, RFs& aFs )
    {
    HBufC* fileToServe = PrepareBaseFileNameL( aFilename, aFs );
    
    TPtrC questionmark;
    questionmark.Set( *fileToServe );
    TInt lastSlash = questionmark.LocateReverse( '\\' );
    TInt lastQuestionMark = questionmark.LocateReverse( '?' );
    if ( lastQuestionMark != KErrNotFound && lastSlash < lastQuestionMark )
        {
        CleanupStack::PushL( fileToServe );
        questionmark.Set( questionmark.Left( lastQuestionMark ) );

        // now setting new name for file        
        HBufC* newFileName; //necessary not to loose current fileToServe
        newFileName = HBufC::NewL( questionmark.Length() );
        newFileName->Des().Copy( questionmark  );
        CleanupStack::PopAndDestroy( fileToServe );
        fileToServe = newFileName;
        }
    
    TPtrC path;
    path.Set( *fileToServe );
    TInt lastPosOfSlash = 0;
    TInt posOfBackSlash = path.Find( KDoubleBackSlash );
    
    while ( posOfBackSlash != KErrNotFound )
        {
        path.Set( path.Mid( posOfBackSlash + 1 ) );
        lastPosOfSlash = lastPosOfSlash + posOfBackSlash + 1;
        posOfBackSlash = path.Find( KDoubleBackSlash );
        }
    
    if ( lastPosOfSlash > 0 )
        {
        path.Set( *fileToServe );
        path.Set( path.Left( lastPosOfSlash ) );
        }
        
    return fileToServe;
    }


//  End of File
