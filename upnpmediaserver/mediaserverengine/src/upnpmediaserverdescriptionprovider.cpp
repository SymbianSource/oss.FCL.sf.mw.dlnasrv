/** @file
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpMediaServerDescriptionProvider implementation
*
*/

#include "upnpmediaserverdescriptionprovider.h"
#include "upnpsecuritymanager.h"
#include "upnpfileutils.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include "upnphttpmessage.h"
#include "upnpdevicedescriptionrequest.h"

// CONSTANTS

// -----------------------------------------------------------------------------
//  TBufC8Hash
//  Hash function for RHashMap.
// -----------------------------------------------------------------------------
TUint32 TBufC8Hash( const TBufC8<KMaxUriLength>& aPtr )
    {
    return DefaultHash::Des8( aPtr );
    }

// -----------------------------------------------------------------------------
//  TBufC8Ident
//  Identity function for RHashMap.
// -----------------------------------------------------------------------------
TBool TBufC8Ident( const TBufC8<KMaxUriLength>& aFirst, 
                   const TBufC8<KMaxUriLength>& aSecond )
    {
    return DefaultIdentity::Des8( aFirst, aSecond );
    }


// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::CUpnpMediaServerDescriptionProvider
//
// -----------------------------------------------------------------------------
//
CUpnpMediaServerDescriptionProvider::CUpnpMediaServerDescriptionProvider()
    : iUriMap( &TBufC8Hash, &TBufC8Ident )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::~CUpnpMediaServerDescriptionProvider
//
// -----------------------------------------------------------------------------
//
CUpnpMediaServerDescriptionProvider::~CUpnpMediaServerDescriptionProvider()
    {
    iFs.Close();
    delete iSecurityManager;
    iUriMap.Close();
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::NewLC
//
// -----------------------------------------------------------------------------
//
CUpnpMediaServerDescriptionProvider* CUpnpMediaServerDescriptionProvider::NewLC()
    {
    CUpnpMediaServerDescriptionProvider* self =
            new (ELeave) CUpnpMediaServerDescriptionProvider();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::NewL
//
// -----------------------------------------------------------------------------
//
CUpnpMediaServerDescriptionProvider* CUpnpMediaServerDescriptionProvider::NewL()
    {
    CUpnpMediaServerDescriptionProvider* self =
            CUpnpMediaServerDescriptionProvider::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::ConstructL
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerDescriptionProvider::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );        
    iSecurityManager = CUpnpSecurityManager::NewInstanceFromUpnpSettingsL();
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::AddUriToFileMapL
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerDescriptionProvider::AddUriToFileMapL( const TDesC8& aUri, 
                                                            const TDesC16& aFile )
    {
    HBufC8* buf = HBufC8::NewLC( aUri.Length() );
    buf->Des().CopyLC( aUri ); // change to lower characters

    iUriMap.InsertL( buf->Des().Left( KMaxUriLength ), aFile );
    
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::RemoveUriToFileMapL
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerDescriptionProvider::RemoveUriToFileMapL( const TDesC8& aUri )
    {
    HBufC8* buf = HBufC8::NewLC( aUri.Length() );
    buf->Des().CopyLC( aUri ); // change to lower characters

    iUriMap.Remove( buf->Des() );
    
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::OnDeviceDescription
//
// -----------------------------------------------------------------------------
//
TInt CUpnpMediaServerDescriptionProvider::OnDeviceDescription( 
                                           CUpnpDeviceDescriptionRequest& aRequest )
    {
    return OnRequest( aRequest );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::OnServiceDescription
//
// -----------------------------------------------------------------------------
//
TInt CUpnpMediaServerDescriptionProvider::OnServiceDescription( 
                                           CUpnpDeviceDescriptionRequest& aRequest )
    {
    return OnRequest( aRequest );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::OnIcon
//
// -----------------------------------------------------------------------------
//
TInt CUpnpMediaServerDescriptionProvider::OnIcon( CUpnpDeviceDescriptionRequest& aRequest )
    {
    return OnRequest( aRequest );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::GetFile
//
// -----------------------------------------------------------------------------
//
TInt CUpnpMediaServerDescriptionProvider::GetFile( const TDesC8& aUri, RFile& aFile )
    {    
    TRAPD( error, GetFileFromUriL( aUri, iFileName ) );
    if ( error != KErrNone )
        {
        return error;
        }
    
    return aFile.Open( iFs, iFileName, EFileShareReadersOnly | EFileRead );  
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::OnRequestL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpMediaServerDescriptionProvider::OnRequest( CUpnpDeviceDescriptionRequest& aRequest )
    {
    TInetAddr addr;
    aRequest.InetAddress( addr );

    TRAPD( error, GetFileFromUriL( aRequest.Uri(), iFileName ) );
    
    if ( KErrNone == error )
        {
        if ( 0 == iFileName.Length() )
            {
            return KErrNotFound;
            }
        else
            {
            TRAP( error, AuthorizeRequestL( iFileName, addr ) );
            }
        }
    
    return error;            
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::ExtractPathAndFileFromUriL
//
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMediaServerDescriptionProvider::ExtractPathAndFileFromUriL( const TDesC8& aUri )
    {
    TPtrC8 fileName;
    TInt parseError;
    TPtrC8 urlPath = UpnpFileUtil::ExtractUrlPath( aUri, fileName, parseError);
    User::LeaveIfError( parseError );
    
    HBufC8* buf = HBufC8::NewLC( urlPath.Length() + fileName.Length() + 
                                 2 * UpnpString::KSlash().Length() );
    buf->Des().Copy( UpnpString::KSlash() );
    buf->Des().Append( urlPath );
    buf->Des().Append( UpnpString::KSlash() );
    buf->Des().Append( fileName );
    
    HBufC8* buf2 = HBufC8::NewL( buf->Length() );
    buf2->Des().CopyLC( buf->Des() );   // convert to lower characters
    CleanupStack::PopAndDestroy( buf );
    
    return buf2;
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::GetFileFromUriL
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerDescriptionProvider::GetFileFromUriL( const TDesC8& aUri, TDes& aPath )
    {
    HBufC8* pathAndFile = ExtractPathAndFileFromUriL( aUri );
    
    TFileName* file = iUriMap.Find( pathAndFile->Des() );
    
    delete pathAndFile;
    pathAndFile = NULL;
    
    if ( !file )
        {
        User::Leave( KErrNotFound );
        }
    
    aPath.Copy( *file );
    }

// -----------------------------------------------------------------------------
//  CUpnpMediaServerDescriptionProvider::AuthorizeRequestL
//
// -----------------------------------------------------------------------------
//
void CUpnpMediaServerDescriptionProvider::AuthorizeRequestL(
    const TDesC& aFileName, const TInetAddr& aSender )
    {
    CUpnpHttpMessage* tempMessage = CUpnpHttpMessage::NewL( aSender );
    CleanupStack::PushL( tempMessage );
    if ( iSecurityManager
            && iSecurityManager->AuthorizeMessage( tempMessage,
                    (TFileName&) aFileName  ) != KErrNone )
        { //not showing why resource is refused
        User::Leave( -EHttpNotFound );
        }
    CleanupStack::PopAndDestroy( tempMessage );
    }

