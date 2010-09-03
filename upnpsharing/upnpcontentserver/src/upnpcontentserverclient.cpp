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
* Description:      RUpnpContentServerClient class implementation
*
*/






// INCLUDES
#include <f32file.h>
#include <s32mem.h>

#include "upnpcontentserverclient.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS 
const TInt KDefaultBufferSize( 512 ); //default size for any receice buffer
const TInt KContentBufferSize( 10*1024 ); //size for media content buffer

using namespace UpnpContentServer;

//---------------------------------------------------------------------------
// LaunchServer
// Launches the server
//---------------------------------------------------------------------------
//
TInt LaunchServer()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt res = KErrNone;

    // DLL launch
    RProcess server;
    res = server.Create( KUpnpContentServerNameAndDir, 
                         KNullDesC );

    // Loading failed.
    if ( res == KErrNone )
        {
        server.SetPriority( EPriorityLow );
        TRequestStatus status;
        server.Rendezvous(status);

        if ( status != KRequestPending)
            {
            server.Kill(0);     // abort startup
            server.Close();
            res = KErrGeneral;
            }
        if( !res )
            {
            server.Resume();    // Logon OK - start the server.

            User::WaitForRequest(status);
            server.Close();
            res = status.Int();
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return res;
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::RUpnpContentServerClient
// C++ Constructor
// --------------------------------------------------------------------------
//
EXPORT_C RUpnpContentServerClient::RUpnpContentServerClient()
    : iSendBufferPtr( NULL, 0 ),
      iSendBuffer( NULL ),
      iReceiveBufferPtr( NULL, 0 ),
      iReceiveBuffer( NULL ),
      iContentBufferPtr( NULL, 0 ),
      iContentBuffer( NULL )
    {
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::OpenL
// Creates connection to server
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::OpenL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TVersion version( KUpnpContentServerVersionMajor,
                      KUpnpContentServerVersionMinor,
                      0 );

    TInt res = CreateSession( KUpnpContentServer, version,
                              KDefaultMessageSlots );
    if ( res != KErrNone )
        {
        __LOG1( "Error: %d", res );
        res = LaunchServer();
        if ( res != KErrNone )
            {
            __LOG1( "Error: %d", res );
            }
        User::LeaveIfError( res );
        res = CreateSession( KUpnpContentServer, version,
                             KDefaultMessageSlots );
        if ( res != KErrNone )
            {
            __LOG1( "Error: %d", res );
            }
        ShareAuto();
        }
    User::LeaveIfError( res );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::Close
// close connection to server
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::Close()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // close session
    delete iSendBuffer;
    iSendBuffer = NULL;
    delete iReceiveBuffer;
    iReceiveBuffer = NULL;
    delete iContentBuffer;
    iContentBuffer = NULL;
    RSessionBase::Close();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::StartUploadListenerL
// Starts the media server upload listener
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::StartUploadListenerL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    User::Leave( KErrNotSupported );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::StopUploadListenerL
// Stops the media server upload listener
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::StopUploadListenerL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    User::Leave( KErrNotSupported );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::GetSelectionContentL
// Gets the names of existed albums/playlists
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::GetSelectionContentL(
    UpnpContentServer::TUpnpMediaType aType,
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

 /*   delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = CBufFlat::NewL( KTintSize );

    // Need to externalize a data to buffer to send it with IPC
    RBufWriteStream sendstream;
    sendstream.Open( *iSendBuffer );
    CleanupClosePushL( sendstream );
    sendstream.WriteInt32L( aType );
    iSendBufferPtr.Set( iSendBuffer->Ptr(0) );

    // Prepare receive buffers
    delete iContentBuffer;
    iContentBuffer = NULL;
    iContentBuffer = HBufC8::NewL( KContentBufferSize );
    iContentBufferPtr.Set( iContentBuffer->Des() );

    SendReceive( ERequestSelectionContent,
                 TIpcArgs( &iSendBufferPtr, &iContentBufferPtr ), 
                 aStatus );
    CleanupStack::PopAndDestroy( &sendstream );*/

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::GetSelectionContentResultL
// Ask the result of GetSelectionContentL call
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::GetSelectionContentResultL(
    CDesCArray& aIDArray )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( !iContentBuffer )
        {
        User::Leave( KErrNotReady );
        }

    RDesReadStream stream;
    stream.Open( iContentBufferPtr );
    CleanupClosePushL( stream );
    // leaves -25 if empty stream
    TInt itemCount( 0 );
    TRAP_IGNORE( itemCount = stream.ReadUint32L() );

    for ( TInt i(0); i < itemCount; i++ )
        {
        const TInt desLen( static_cast<TInt>( stream.ReadUint32L() ));
        TBuf<KMaxFileName> item;
        stream.ReadL( item, desLen );
        aIDArray.AppendL( item );
        }

    CleanupStack::PopAndDestroy( &stream );
    delete iContentBuffer;
    iContentBuffer = NULL;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::ChangeSharedContentL
// Sends the selected indexes to server and starts sharing
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::ChangeSharedContentL(
    UpnpContentServer::TUpnpMediaType aType,
    const CArrayFix<TInt>& aMarkedItems,
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt objectCount( aMarkedItems.Count() );
    if ( objectCount < 1 )
        {
        __LOG1( "Error: %d", __LINE__ );
        User::Leave( KErrArgument );
        }

    delete iSendBuffer;
    iSendBuffer = NULL;
    // Calculate correct size for send buffer on basis of the amount
    // of items user has selected
    iSendBuffer = CBufFlat::NewL( objectCount * KTintSize + 2*KTintSize );

    // Need to externalize a data to buffer to send it with IPC
    RBufWriteStream stream;
    stream.Open( *iSendBuffer );
    CleanupClosePushL( stream );
    stream.WriteInt32L( aType );
    stream.WriteInt32L( objectCount );
    for( TInt i(0); i< objectCount; i++ )
        {
        stream.WriteInt32L( aMarkedItems[ i ] );
        }
    iSendBufferPtr.Set( iSendBuffer->Ptr(0) );

    SendReceive( EChangeShareContent,
                 TIpcArgs( &iSendBufferPtr ),
                 aStatus );   
    CleanupStack::PopAndDestroy( &stream );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::RefreshSharedContentL
// Refresh the shared content
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::RefreshSharedContentL(
    UpnpContentServer::TUpnpMediaType aType,
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

 /*   delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = CBufFlat::NewL( KTintSize );

    // Need to externalize a data to buffer to send it with IPC
    RBufWriteStream stream;
    stream.Open( *iSendBuffer );
    CleanupClosePushL( stream );
    stream.WriteInt32L( aType );
    iSendBufferPtr.Set( iSendBuffer->Ptr(0) );

    SendReceive( ERefreshShareContent,
                 TIpcArgs( &iSendBufferPtr ),
                 aStatus );
    CleanupStack::PopAndDestroy( &stream );*/

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::GetSelectionIndexesL
// Gets the users previous selections as list of selected indexes
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::GetSelectionIndexesL(
    UpnpContentServer::TUpnpMediaType aType,
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

  /*  delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = CBufFlat::NewL( KTintSize );

    // Need to externalize a data to buffer to send it with IPC
    RBufWriteStream sendstream;
    sendstream.Open( *iSendBuffer );
    CleanupClosePushL( sendstream );
    sendstream.WriteInt32L( aType );
    iSendBufferPtr.Set( iSendBuffer->Ptr(0) );

    // Prepare receive buffers
    PrepareReceiveBuffersL( KDefaultBufferSize );

    SendReceive( EGetSelectionIndexes,
                 TIpcArgs( &iSendBufferPtr, &iReceiveBufferPtr ),
                 aStatus );
    CleanupStack::PopAndDestroy( &sendstream );*/

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::GetSelectionIndexesResultL
// Ask the result of GetSelectionIndexesL call
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::GetSelectionIndexesResultL(
    CArrayFix<TInt>& aMarkedItems )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( !iReceiveBuffer )
        {
        User::Leave( KErrNotReady );
        }

    RDesReadStream stream;
    stream.Open( iReceiveBufferPtr );
    CleanupClosePushL( stream );

    // leaves -25 if empty stream
    TInt itemCount( 0 );
    TRAP_IGNORE( itemCount = stream.ReadUint32L() );

    for ( TInt i(0); i < itemCount; i++ )
        {
        const TInt val = static_cast<TInt>( stream.ReadUint32L() );
        aMarkedItems.AppendL( val );
        }

    CleanupStack::PopAndDestroy( &stream );
    delete iReceiveBuffer;
    iReceiveBuffer = NULL;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::StartConnectionMonitorL
// Start connection monitor in content server
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::StartConnectionMonitorL( 
    TInt aIapId,
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

   /* delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = CBufFlat::NewL( KTintSize );

    // Need to externalize a data to buffer to send it with IPC
    RBufWriteStream sendstream;
    sendstream.Open( *iSendBuffer );
    CleanupClosePushL( sendstream );
    sendstream.WriteInt32L( aIapId );
    iSendBufferPtr.Set( iSendBuffer->Ptr(0) );

    SendReceive( EStartConnectionMonitor,
                 TIpcArgs( &iSendBufferPtr ),
                 aStatus );
    CleanupStack::PopAndDestroy( &sendstream );*/

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::StopConnectionMonitorL
// Stop connection monitor in content server
// --------------------------------------------------------------------------
//
EXPORT_C void RUpnpContentServerClient::StopConnectionMonitorL( 
    TRequestStatus& aStatus )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    SendReceive( EStopConnectionMonitor, aStatus );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// RUpnpContentServerClient::PrepareReceiveBuffers
// Prepares the receive buffers
// --------------------------------------------------------------------------
//
void RUpnpContentServerClient::PrepareReceiveBuffersL( TInt aSize )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iReceiveBuffer;
    iReceiveBuffer = NULL;
    iReceiveBuffer = HBufC8::NewL( aSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

//End of File
