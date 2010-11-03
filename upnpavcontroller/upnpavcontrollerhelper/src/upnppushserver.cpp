/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Push transport server
*
*/






// INCLUDE FILES
// System
#include <es_sock.h>
#include <in_sock.h>

// upnp stack api's
#include <upnphttpserversession.h>
#include <upnphttpserverruntime.h>
#include <upnphttpserverobserver.h>
#include <upnphttpservertransaction.h>
#include <upnphttpservertransactioncreator.h>
#include <upnphttpmessage.h>

// dlnasrv / mediaserver api
#include <upnpitem.h>
#include "upnpdlnafilter.h"

// dlnasrv / internal api's
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpitemutility.h" // for GetResElements


_LIT( KComponentLogfile, "upnppushserver.txt");
#include "upnplog.h"
#include "upnppushserver.h"

// CONSTANTS
const TUint KMaxItemResourcesCount = 8;
const TUint KMaxBufferSize = 64;
const TUint KMaxUriLength = 256;
const TInt KSlash = 47;  // '/'
const TInt KDot = 46;    // '.'

_LIT8( KHttp,                       "http://" );
_LIT8( KSeparatorcolon,             ":" );
_LIT8( KSeparatorSlash,             "/" );
// GLOBALS


// --------------------------------------------------------------------------
// CUpnpPushServer::ShareL
// 
//---------------------------------------------------------------------------
EXPORT_C void CUpnpPushServer::ShareL(
    TUint aHash,
    CUpnpItem& aItem )
    {
    __LOG( "PushServer::ShareL()" );
    RUPnPElementsArray resources;
    CleanupClosePushL( resources );
    UPnPItemUtility::GetResElements( aItem, resources );
    if ( resources.Count() == 0 )
    	{
        __LOG( "PushServer::ShareL() - no resources" );
        User::Leave(KErrNotFound);
    	}

    CUpnpPushServer* singleton = static_cast<CUpnpPushServer*>( Dll::Tls() );
    if ( !singleton )
        {
        // singleton must be created first
        __LOG( "PushServer::ShareL: creating singleton" );
        singleton = new (ELeave) CUpnpPushServer();
        CleanupStack::PushL( singleton );
        singleton->ConstructL();
        User::LeaveIfError( Dll::SetTls( singleton ) );
        CleanupStack::Pop( singleton );
        }
    
    CUpnpElement* elem = 
    (CUpnpElement*)(UPnPItemUtility::FindElementByName( 
                    aItem, KElementAlbumArtUri() ));
    if( elem )
        {
        singleton->ShareResourceL( aHash, *elem );
        aHash += 1;
        }
    
    for( TUint i(0); i<resources.Count(); ++i )
        {
        singleton->ShareResourceL( aHash+i, *resources[i] );
        }

    CleanupStack::PopAndDestroy( &resources );
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::Unshare
// 
//---------------------------------------------------------------------------
EXPORT_C void CUpnpPushServer::UnshareL(
    TUint aHash )
    {
    CUpnpPushServer* singleton = static_cast<CUpnpPushServer*>( Dll::Tls() );
    if ( singleton == 0 ) return; // singleton is empty, nothing to share.

    // assume item would have max. 8 resources, call unshare for those
    for( TUint i=0; i<KMaxItemResourcesCount; ++i )
        {
        singleton->UnshareResource( aHash+i );
        }

    if ( singleton->EntryCount() == 0 )
        {
        __LOG( "PushServer::Unshare: destroying singleton" );
        Dll::FreeTls();
        delete singleton;
        }
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::UnshareAll
// 
//---------------------------------------------------------------------------
EXPORT_C void CUpnpPushServer::UnshareAllL()
    {
    CUpnpPushServer* singleton = static_cast<CUpnpPushServer*>( Dll::Tls() );
    if ( singleton != 0 )
        {
        __LOG( "PushServer::UnshareAll: destroying singleton" );
        Dll::FreeTls();
        delete singleton;
        }
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::IsRunning
// 
//---------------------------------------------------------------------------
EXPORT_C TBool CUpnpPushServer::IsRunning()
    {
    CUpnpPushServer* singleton = static_cast<CUpnpPushServer*>( Dll::Tls() );
    return (singleton ? ETrue : EFalse);
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::ShareResourceL
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::ShareResourceL(
    TUint aHash,
    CUpnpElement& aResource )
    {
    // catch protocol info if one exists
    const CUpnpAttribute* piAttr = UPnPItemUtility::FindAttributeByName(
        aResource, KAttributeProtocolInfo() );

    // create URI
    TBuf8<KMaxBufferSize> uri;
    TParse parse;
    TInt result = parse.Set( aResource.FilePath(), NULL, NULL );
    User::LeaveIfError(result);
    HashToUri( aHash, parse.Ext(), uri );
    aResource.SetValueL( uri );

    // add to share
    __LOG2( "PushServer::ShareResourceL %d = %S",
        aHash, &aResource.FilePath() );
    __LOG8( uri );
    TUpnpPushEntry entry(aHash, aResource.FilePath(),
						( piAttr ? piAttr->Value() : KNullDesC8 ) );
    iEntries.InsertL( aHash, entry );
    }


// --------------------------------------------------------------------------
// CUpnpPushServer::UnshareResource
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::UnshareResource( TUint aHash )
    {
    iEntries.Remove( aHash );
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::EntryCount
// 
//---------------------------------------------------------------------------
TInt CUpnpPushServer::EntryCount()
    {
    return iEntries.Count();
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::HashToUri
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::HashToUri( TUint aHash, const TDesC& aFileExt, 
    TDes8& aUriBuf )
    {
    TInetAddr addr;
    iHttpServer->GetAddress( addr );
    TBuf<KMaxBufferSize> ip;
    addr.Output( ip );

    aUriBuf.Append( KHttp() );  			// http://
    aUriBuf.Append( ip );            		// http://n.n.n.n
    aUriBuf.Append( KSeparatorcolon() );    // http://n.n.n.n:
    aUriBuf.AppendNum( addr.Port() ); 		// http://n.n.n.n:pppp
    aUriBuf.Append( KSeparatorSlash() );    // http://n.n.n.n:pppp/
    aUriBuf.AppendNum( aHash );       		// http://n.n.n.n:pppp/hash
    aUriBuf.Append( aFileExt );       		// http://n.n.n.n:pppp/hash.ext
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::HashFromUri
// 
//---------------------------------------------------------------------------
TUint CUpnpPushServer::HashFromUri( const TDesC8& aUri )
    {
    TBuf16<KMaxUriLength> uri16;
    uri16.Copy( aUri );
    return HashFromUri( uri16 );
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::HashFromUri
// 
//---------------------------------------------------------------------------
TUint CUpnpPushServer::HashFromUri( const TDesC16& aUri )
    {
    TUint hash = 0;
	TChar slash( KSlash );
    TChar dot( KDot );
    // extract the hash part
    TInt begin = aUri.LocateReverse( slash );
    if ( begin<0 ) 
    	{
    	begin = -1;
    	}
    TInt end = aUri.LocateReverse( dot );
    if ( end<0 ) 
    	{
    	end = aUri.Length();
    	}
    TPtrC16 hashPart = aUri.Mid(
        begin+1, end-begin-1 );
    // convert to a number
    TLex16 hashLex( hashPart );
    if ( hashLex.Val( hash, EDecimal ) != KErrNone )
        {
        // hashcode was not found
        hash = 0;
        }
    __LOG2( "PushServer: HashFromUri: %S -> %d", &aUri, hash );
    return hash;
    }


// --------------------------------------------------------------------------
// CUpnpPushServer::CUpnpPushServer
// 
//---------------------------------------------------------------------------
CUpnpPushServer::CUpnpPushServer()
    :iEntries()
    {
    __LOG( "PushServer::CUpnpPushServer()" );
    
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::ConstructL
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::ConstructL()
    {
    __LOG( "PushServer::ConstructL()" );
    
    iHttpServer = CUpnpHttpServerSession::NewL( 0, *this );
    
    TRAPD( err, iHttpServer->StartL( KPushServerPort ) );
    if( err )
        {
        //fixed port already in use -> start with random port
        iHttpServer->StartL();
        }
    
    iHttpServer->DefaultRuntime().SetCreator( *this );
    iFilter = CUpnpDlnaFilter::NewL( this, NULL );
    
    __LOG( "PushServer::ConstructL() - end" );
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::CUpnpPushServer
// 
//---------------------------------------------------------------------------
CUpnpPushServer::~CUpnpPushServer()
    {
    __LOG( "PushServer::~CUpnpPushServer()" );
    iEntries.Close();
    if(NULL != iHttpServer)
    	{
    	iHttpServer->Stop();
    	}
    delete iHttpServer;
    delete iFilter;
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::NewTransactionL
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::NewTransactionL(
    const TDesC8& aMethod,
    const TDesC8& aUri,
    const TInetAddr& aSender,
    CUpnpHttpServerTransaction*& aTrans )
    {
    TBuf<KMaxBufferSize> ip;
    aSender.Output(ip);
	if ( aMethod == KHttpGet() || aMethod == KHttpHead() )
	    {
        __LOG2( "PushServer: HTTP %S from=%S, launching transaction", &aMethod, 
            &ip );
        iFilter->NewTransactionL( aMethod, aUri, aSender, aTrans );
	    }
    else
        {
        // this method not supported
        __LOG2( "PushServer: unsupported request: %S from=%S", &aMethod, &ip );
        User::Leave( KErrNotSupported );
        }
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::HttpEventLD
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::HttpEventLD( CUpnpHttpMessage* aMessage )
    {
    __LOG1( "QuickShare:HttpEventLD() type=%d", aMessage->Type() );

    delete aMessage;
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::CheckImportUriL
// 
//---------------------------------------------------------------------------
TInt CUpnpPushServer::CheckImportUriL( const TDesC8& /*aImportUri*/ )
    {
    // should never be here
    __PANIC( __FILE__, __LINE__ );
    return 0;
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::GetTitleForUriL
// 
//---------------------------------------------------------------------------
void CUpnpPushServer::GetTitleForUriL( TInt /*aObjectId*/, TPtr& /*aValue*/ )
    {
    // should never be here
    __PANIC( __FILE__, __LINE__ );
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::GetProtocolInfoL
// 
//---------------------------------------------------------------------------
TInt CUpnpPushServer::GetProtocolInfoL( const TDesC8& aContentUri,
    CUpnpDlnaProtocolInfo*& aProtocolInfo )
    {
    aProtocolInfo = 0;
    TInt result = KErrNotFound;
    TUpnpPushEntry* entry = iEntries.Find( HashFromUri( aContentUri ) );
    if ( entry )
        {
        aProtocolInfo = CUpnpDlnaProtocolInfo::NewL( entry->ProtocolInfo() );
        result = KErrNone;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::FindSharedFolderL
// 
//---------------------------------------------------------------------------
TInt CUpnpPushServer::FindSharedFolderL( const TDesC& /*aUrlPath*/,
    const TDesC& aFileName, HBufC*& aFolderPath )
    {
    aFolderPath = 0;
    TInt result = KErrNotFound;
    TUpnpPushEntry* entry = iEntries.Find( HashFromUri( aFileName ) );
    if ( entry )
        {
        aFolderPath = entry->Path().AllocL();
        result = KErrNone;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CUpnpPushServer::GetProtocolInfoByImportUriL
// 
//---------------------------------------------------------------------------
CUpnpDlnaProtocolInfo* CUpnpPushServer::GetProtocolInfoByImportUriL( 
    const TDesC8& /*aImportUri */)
    {
    // should never be here
    __PANIC( __FILE__, __LINE__ );
    return 0;
    }

