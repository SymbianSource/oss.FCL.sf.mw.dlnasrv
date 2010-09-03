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
* Description:      Resolver for remote items
*
*/






// INCLUDE FILES
// upnp stack api's
#include <upnpobject.h>
#include <upnpitem.h>
#include <upnpcontainer.h>
#include <upnpelement.h>
#include <upnpattribute.h>
#include <upnpdlnaprotocolinfo.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsession.h" // browsing session

// upnpframework / avcontroller helper api
#include "upnpresourceselector.h" // MUPnPResourceSelector
#include "upnpitemresolverobserver.h" // observer for this class
#include "upnpitemutility.h" // for ResourceFromItem
#include "upnpconstantdefs.h" // for browsing param: KSortNone

// upnpframework / xml parser api
#include "upnpxmlparser.h" // for xml parsing

// avcontrollerhelper internal
#include "upnpremoteitemresolver.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS


// METHODS

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::NewL
//---------------------------------------------------------------------------
CUPnPRemoteItemResolver* CUPnPRemoteItemResolver::NewL(
    const TDesC8& aItemId,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    {
    CUPnPRemoteItemResolver* self = new (ELeave) CUPnPRemoteItemResolver(
        aItemId, aHostSession, aSelector, aBrowseFilter );
    CleanupStack::PushL( self );
    self->ConstructL( aItemId, aHostSession, aSelector, aBrowseFilter );
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::CUPnPRemoteItemResolver
//---------------------------------------------------------------------------
CUPnPRemoteItemResolver::CUPnPRemoteItemResolver(
    const TDesC8& /*aItemId*/,
    MUPnPAVBrowsingSession& aHostSession,
    MUPnPResourceSelector& aSelector,
    const TDesC8& aBrowseFilter )
    : CUPnPAbstractBrowsingSessionObserver()
    , iBrowsingSession( aHostSession )
    , iSelector( aSelector )
    , iBrowseFilter( aBrowseFilter )
    {
    iState = EStateIdle;
    SetSession( aHostSession );
    }


// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::ConstructL
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::ConstructL(
    const TDesC8& aItemId,
    MUPnPAVBrowsingSession& /*aHostSession*/,
    MUPnPResourceSelector& /*aSelector*/,
    const TDesC8& /*aBrowseFilter*/ )
    {
    iItemId = aItemId.AllocL();
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::~CUPnPRemoteItemResolver
//---------------------------------------------------------------------------
CUPnPRemoteItemResolver::~CUPnPRemoteItemResolver()
    {
    Cleanup();
    delete iItemId;
    iItemId = 0;
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::ResolveL
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::ResolveL(
    MUPnPItemResolverObserver& aObserver )
    {
    __LOG( "RemoteItemResolver:Resolve()" );
    __ASSERTD( iState == EStateIdle, __FILE__, __LINE__ );

    // enable receiving callbacks here
    EnableSessionObserver();

    // change state
    iObserver = &aObserver;
    iState = EStateActive;

    iRecursionDepth = 0;

    iBrowsingSession.BrowseL(
        iItemId->Des(), iBrowseFilter,
        MUPnPAVBrowsingSession::EMetadata,
        0, 1, KSortNone );

    }


// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::Item
//---------------------------------------------------------------------------
const CUpnpItem& CUPnPRemoteItemResolver::Item() const
    {
    __LOG( "RemoteItemResolver:item" );
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERTD( iFirstLevelItem, __FILE__, __LINE__ );

    return *iFirstLevelItem;
    }


// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::Resource
//---------------------------------------------------------------------------
const CUpnpElement& CUPnPRemoteItemResolver::Resource() const
    {
    __LOG( "RemoteItemResolver:Resource" );
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERTD( iResource, __FILE__, __LINE__ );

    return *iResource;
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::BrowseResponse
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::BrowseResponse(
    const TDesC8& aBrowseResponse,
    TInt aError,
    TInt /*aMatches*/,
    TInt /*aTotalCount*/,
    const TDesC8& /*aUpdateId*/ )
    {
    __ASSERTD( iState == EStateActive, __FILE__, __LINE__ );
    __LOG1( "RemoteItemResolver:BrowseResponse(%d)", aError );

    // If the browse succeeded, parse the response and process the result
    if ( aError == KErrNone )
        {
        TRAP( aError, BrowseResponseL( aBrowseResponse ) );
        }

    // It the browse failed, or the parsing/processging of the response
    // failed, do complete.
    if( aError != KErrNone )
        {
        Complete( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::BrowseResponseL
//---------------------------------------------------------------------------
//
void CUPnPRemoteItemResolver::BrowseResponseL( const TDesC8& aBrowseResponse )
    {
    __LOG( "RemoteItemResolver:BrowseResponseL" );

    // Status code
    TInt status = KErrNone;

    // Result array
    RPointerArray<CUpnpObject> array;

    // Create parser
    CUPnPXMLParser* parser = NULL;
    TRAP( status, parser = CUPnPXMLParser::NewL() );

    // If the parser was created succesfully...
    if( status == KErrNone )
        {
        CleanupStack::PushL( parser );

        // Parse the result data, and process the result object
        TRAP( status,
              parser->ParseResultDataL( array, aBrowseResponse );
              if(array.Count()!= 0 )
            	  ProcessResultObjectL( array[0] );
              else
            	  {
            	  //handle error no object
            	  Complete( KErrNotFound );
            	  }
            );

        // Clean up
        CleanupStack::PopAndDestroy( parser );
        parser = NULL;
        }

    // Empty, reset and close the array
    for( TInt i=0; i<array.Count(); i++ )
        {
        delete array[i];
        }
    array.Reset();
    array.Close();

    // If there was an error, forward the leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::ProcessResultObjectL
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::ProcessResultObjectL(
    const CUpnpObject* aResult )
    {
  /*  if ( aResult->ObjectType() != EUPnPItem )
        {
        User::Leave( KErrArgument );
        }

    // copy the item
    CUpnpItem* item = CUpnpItem::NewL();
    CleanupStack::PushL( item );
    item->CopyL( *aResult );

    // select the resource
    __LOG( "RemoteItemResolver:calling SelectResource" );
    const CUpnpElement& res =
        iSelector.SelectResourceL( *item );

    if ( iRecursionDepth == 0 )
        {
        __ASSERTD( !iFirstLevelItem, __FILE__, __LINE__ );
        CleanupStack::Pop( item );
        iFirstLevelItem = item;
        item = 0; // take ownership
        }

    HBufC8* recurringId = 0;


    if ( recurringId )
        {
        // continue recursion !
        ++iRecursionDepth;
        __LOG1( "RemoteItemResolver:ProcessResult - recur %d",
            iRecursionDepth );
        iBrowsingSession.BrowseL(
            recurringId->Des(), iBrowseFilter, 
            MUPnPAVBrowsingSession::EMetadata,
            0, 1, KSortNone );
        CleanupStack::PopAndDestroy( recurringId );
        }
    else
        {
        // finished browsing the item
        __ASSERTD( !iLastLevelItem, __FILE__, __LINE__ );
        __LOG( "RemoteItemResolver:ProcessResult - finished" );
        if ( iRecursionDepth > 0 )
            {
            // pointed object is DIFFERENT from first level object
            iLastLevelItem = CUpnpItem::NewL();
            iLastLevelItem->CopyL( *item );
            }
        iResource = &res;
        Complete( KErrNone );
        }

    if( item )
        {
        CleanupStack::PopAndDestroy( item );
        }*/
    }


// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::Complete
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::Complete( TInt aError )
    {
    __ASSERTD( iState == EStateActive, __FILE__, __LINE__ );

    DisableSessionObserver();
    MUPnPItemResolverObserver& observer = *iObserver;
    iObserver = 0;
    if ( aError == KErrNone )
        {
        iState = EStateReady;
        }
    else
        {
        iState = EStateIdle;
        Cleanup();
        }

    observer.ResolveComplete( *this, aError );
    }

// --------------------------------------------------------------------------
// CUPnPRemoteItemResolver::Cleanup
//---------------------------------------------------------------------------
void CUPnPRemoteItemResolver::Cleanup()
    {
    DisableSessionObserver();

    if ( iState == EStateActive )
        {
        iBrowsingSession.CancelBrowse();
        }

    iObserver = 0;

    delete iFirstLevelItem;
    iFirstLevelItem = 0;

    delete iLastLevelItem;
    iLastLevelItem = 0;

    iResource = 0;

    iState = EStateIdle;
    }


