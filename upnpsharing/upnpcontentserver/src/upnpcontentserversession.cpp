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
* Description:      CUpnpContentServerSession class implementation
*
*/





#include <e32debug.h>
#include <e32def.h> //CArrayFix
#include <s32mem.h> //RDesReadStream
#include "upnpcontentserverdefs.h"
#include "upnpcontentserversession.h"
#include "upnpcontentserverclient.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

using namespace UpnpContentServer;

// --------------------------------------------------------------------------
// CUpnpContentServerSession::NewL
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerSession* CUpnpContentServerSession::NewL(
    CUpnpContentServer* aServer )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServerSession* self =
        new (ELeave) CUpnpContentServerSession();
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop( self );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::CUpnpContentServerSession
// C++ constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerSession::CUpnpContentServerSession( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// void CUpnpContentServerSession::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::ConstructL( CUpnpContentServer* aServer )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( !aServer )
        {
        User::Leave( KErrArgument );
        }
    iServer = aServer;
    iHandler = iServer->Handler();
    iServer->AddSession();
    if ( iHandler )
        {
        iHandler->SetContentSharingObserverL( this );
        }
    else
        {
        User::Leave( KErrBadHandle );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::~CUpnpContentServerSession
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerSession::~CUpnpContentServerSession()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Sharing is off, client is disconnected, no sharing operation is ongoing
    // -> close the server
    if( iServer && iHandler )
        {
        iServer->RemoveSession();
        if ( iHandler->CanStop() && iServer->CanStop() )
            {
            iServer->Stop();
            __LOG("CUpnpContentServerSession::~CUpnpContentServerSession, \
CActiveScheduler stopped");
            }
        TRAP_IGNORE( iHandler->SetContentSharingObserverL( NULL ) )
        }
    delete iResultBuffer;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::ServiceL
// From CSession2, passes the request forward to DispatchMessageL.
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::ServiceL( const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TRAPD(err,DispatchMessageL(aMessage));
    if ( err != KErrNone )
        {
        __LOG1( "Error: %d", err );
        aMessage.Complete( err );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::DispatchMessageL
// Handles the request from client.
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::DispatchMessageL( const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iHandler->SetContentSharingObserverL( this );
    switch( aMessage.Function() )
        {
    case ERequestSelectionContent :
        {
        GetSelectionContentL( aMessage );
        // completed separately
        }
        break;
    case EGetSelectionIndexes :
        {
        GetSelectionIndexesL( aMessage );
        aMessage.Complete( KErrNone );
        }
        break;
    case EChangeShareContent :
        {
        ChangeShareContentL( aMessage );
        aMessage.Complete( KErrNone );
        }
        break;
    case ERefreshShareContent :
        {
        RefreshShareContentL( aMessage );
        aMessage.Complete( KErrNone );
        }
        break;
    case ERequestProgressInfo :
        {
        RequestProgressInfoL( aMessage );
        aMessage.Complete( KErrNone );
        }
        break;
    case EStartConnectionMonitor :
        {
        StartConnectionMonitorL( aMessage );
        aMessage.Complete( KErrNone );
        }
        break;
    case EStopConnectionMonitor :
        {
        StopConnectionMonitorL();
        aMessage.Complete( KErrNone );
        }
        break;

    default:
        {
        aMessage.Complete( KErrArgument );
        }
        break;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::GetSelectionContentL
// asynchronous
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::GetSelectionContentL(
    const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt len( aMessage.GetDesLength(0) );

    HBufC8* buffer = HBufC8::NewLC( len );
    TPtr8 ptr( buffer->Des() );
    aMessage.ReadL( 0, ptr );
    RDesReadStream stream;
    stream.Open( ptr );
    CleanupClosePushL( stream );

    TInt mediaType( stream.ReadInt32L() );
    iSelectionContentMsg = aMessage;

    // call to server
    iHandler->GetSelectionContentL( mediaType );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::GetSelectionIndexesL
// synchronous
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::GetSelectionIndexesL(
    const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt len( aMessage.GetDesLength(0) );

    RArray<TInt>* selections = new ( ELeave )
        RArray<TInt>( 5 );
    CleanupClosePushL( *selections );
    HBufC8* buffer = HBufC8::NewLC( len );
    TPtr8 ptr( buffer->Des() );
    aMessage.ReadL( 0, ptr );
    RDesReadStream stream;
    stream.Open( ptr );
    CleanupClosePushL( stream );

    TInt mediaType( stream.ReadInt32L() );
    // call to server
    iHandler->GetSelectionIndexesL( *selections, mediaType );

    TInt objectCount( selections->Count() );

    delete iResultBuffer;
    iResultBuffer = NULL;

    iResultBuffer = CBufFlat::NewL( KTintSize + KTintSize*objectCount );
    RBufWriteStream wstream;
    wstream.Open( *iResultBuffer );
    CleanupClosePushL( wstream );

    // append count and ints
    wstream.WriteInt32L( objectCount );
    for( TInt i(0); i < objectCount; i++ )
        {
        wstream.WriteInt32L( selections->operator[]( i ) );
        }

    // Write result back to client
    aMessage.WriteL( 1, iResultBuffer->Ptr(0) );

    CleanupStack::PopAndDestroy( &wstream );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( selections );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::ChangeShareContentL
// Handles the async request from client.
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::ChangeShareContentL(
    const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt len( aMessage.GetDesLength(0) );
    RArray<TInt> selections( len );
    CleanupClosePushL( selections );
    HBufC8* buffer = HBufC8::NewLC( len );
    TPtr8 ptr( buffer->Des() );
    aMessage.ReadL( 0, ptr );
    RDesReadStream stream;
    stream.Open( ptr );
    CleanupClosePushL( stream );

    TInt mediaType( stream.ReadInt32L() );
    TInt objCount( stream.ReadInt32L() );

    // loop the TInt from stream
    for( TInt i(0); i< objCount; i++)
        {
        selections.AppendL( stream.ReadInt32L() );
        }

    iHandler->ChangeShareContentL( selections,  mediaType );
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &selections );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::RequestProgressInfoL
// synchronous
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::RequestProgressInfoL(
    const RMessage2& /*aMessage*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // The progress should be read using pubsub
    // This call will be removed in upnpfw 2.1
    User::Leave( KErrNotSupported );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::RefreshShareContentL
// asynchronous
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::RefreshShareContentL(
    const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt len( aMessage.GetDesLength(0) );
    HBufC8* buffer = HBufC8::NewLC( len );
    TPtr8 ptr( buffer->Des() );
    aMessage.ReadL( 0, ptr );
    RDesReadStream stream;
    stream.Open( ptr );
    CleanupClosePushL( stream );

    TInt mediaType( stream.ReadInt32L() );

    // The sharing and refresh cannot be used simultaneously
    // Using the same place to store message for completion
    //
    iChangeShareContentMsg = aMessage;
    // call to handler.
    iHandler->RefreshShareContentL( mediaType );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::CompleteSelectionContentL
// Completes the async GetSelectionContentL call
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::CompleteSelectionContentL(
    const CDesCArray& aMarkedItems )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt objectCount( aMarkedItems.MdcaCount() );

    delete iResultBuffer;
    iResultBuffer = NULL;

    iResultBuffer = CBufFlat::NewL( KTintSize + KTintSize*objectCount );
    RBufWriteStream wstream;
    wstream.Open( *iResultBuffer );
    CleanupClosePushL( wstream );

    // append count
    wstream.WriteInt32L( objectCount );
    // append descriptors
    for( TInt i(0); i<objectCount; i++ )
        {
        wstream.WriteInt32L( aMarkedItems[ i ].Length() );
        wstream.WriteL( aMarkedItems[ i ] );
        }

    // Write result back to client
    iSelectionContentMsg.WriteL( 1, iResultBuffer->Ptr(0) );
    iSelectionContentMsg.Complete( KErrNone );
    CleanupStack::PopAndDestroy( &wstream );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::StartConnectionMonitorL
// synchronous
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::StartConnectionMonitorL(
    const RMessage2& aMessage )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt len( aMessage.GetDesLength(0) );
    HBufC8* buffer = HBufC8::NewLC( len );
    TPtr8 ptr( buffer->Des() );
    aMessage.ReadL( 0, ptr );
    RDesReadStream stream;
    stream.Open( ptr );
    CleanupClosePushL( stream );

    TInt iapId( stream.ReadInt32L() );

    // call to server
    iServer->RequestConnectionLostL( iapId );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buffer );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerSession::StopConnectionMonitorL
// cancel connection lost
// --------------------------------------------------------------------------
//
void CUpnpContentServerSession::StopConnectionMonitorL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iServer->CancelConnectionLostL();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// End of file
