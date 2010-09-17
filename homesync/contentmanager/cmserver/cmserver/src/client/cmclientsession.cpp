/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Manager server's client
*
*/



// INCLUDE FILES

#include    <s32mem.h>
#include    <badesca.h>

#include    "cmfillrulecontainer.h"
#include    "cmstorerulecontainer.h"
#include    "cmbaselistitem.h"
#include    "cmmediaserverfull.h"
#include    "cmsqlpropertyitem.h"
#include    "cmsqlpropertycontainer.h"
#include    "cmsqlpropertycollector.h"
#include    "cmdriveinfo.h"
#include    "cmactive.h"
#include    "msdebug.h"


// CONSTANTS
// Number of retries to start server
const TInt KServerRetries = 2;
// wizard executed state
const TInt KAppWizardExecuted = 1;

// ---------------------------------------------------------------------------
// StartServer
// Creates a new process for the server and starts it up.
// ---------------------------------------------------------------------------
//
static TInt StartServer()
    {
    LOG(_L("[Cm Server]\t StartServer begin"));

    TInt result( KErrNone );
    // create server - if one of this name does not already exist
    TFindServer findServer( KCmServerName );
    TFullName name;
    if ( findServer.Next( name ) != KErrNone ) // we don't exist already
        {

        TRequestStatus status( KRequestPending );
        RProcess server;
        // Create the server process
        result = server.Create( KCmServerExe, KNullDesC );
        if( result != KErrNone )
            {
            return result;
            }

        // Process created successfully
        server.Resume(); // start it going
        server.Rendezvous( status );

        // Wait until the completion of the server creation
        // server signals us when it's up
        User::WaitForRequest( status );

        if( status != KErrNone )
            {
            server.Close();
            return status.Int();
            }

        // Server created successfully
        server.Close(); // we can close the handle to server process now
        }

    LOG(_L("[Cm Server]\t StartServer end"));

    return result;
    }

// ---------------------------------------------------------------------------
// RContentManager::RContentManager
// ---------------------------------------------------------------------------
//
EXPORT_C RContentManager::RContentManager()
    {
    LOG(_L("[Cm Server]\t RContentManager::RContentManager"));
    
    iClientRequest = NULL;
    iTargetContainer = NULL;
    iBuffer = NULL;
    iAo = NULL;
    }

// ---------------------------------------------------------------------------
// RContentManager::Close
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RContentManager::Close()
    {
    LOG(_L("[Cm Server]\t RContentManager::Close"));
    
    // Base call
    RSessionBase::Close();

    if ( iAo )
        {
        delete iAo; iAo = NULL;
        }

    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::Connect
// Starts and creates a session of Content Manager
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RContentManager::Connect()
    {
    LOG(_L("[Cm Server]\t RContentManager::Connect"));
    
    iCanceled = EFalse;
    
    TInt tryLoop( 0 );
    TInt errCode( KErrNone );

    for ( tryLoop = 0; tryLoop < KServerRetries; tryLoop++ )
        {
        errCode = CreateSession(KCmServerName,
            TVersion( KCmServerMajor, KCmServerMinor,
                    KCmServerBuild ) );
        //gives MessageSlots of -1
        //this uses global pool rather than local pool

        TRACE(Print(_L("[Cm Server]\t RContentManager::Connect New Session \
        created with status %d\n"), errCode));

        if( errCode == KErrNotSupported )
            {
            TRACE(Print(_L("[Cm Server]\t Version not supported!\n")));
            return errCode;
            }
        if ( errCode != KErrNotFound && errCode != KErrServerTerminated)
            {
            return errCode;
            }

        errCode = StartServer();

        TRACE(Print(_L("[Cm Server]\t RContentManager::Connect Start server \
        errCode %d\n"), errCode));

        if ( errCode != KErrNone && errCode != KErrAlreadyExists )
            {
            return errCode;
            }

        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// RContentManager::OperationCompletedL
// ---------------------------------------------------------------------------
//
TInt RContentManager::OperationCompletedL( TInt aErr )
    {
    LOG(_L("[Cm Server]\t RContentManager::OperationCompletedL"));

    TRACE(Print(_L("[Cm Server]\t RContentManager::OperationCompletedL \
    aErr = %d, iCanceled = %d\n"), aErr, iCanceled));

    if ( !aErr && !iCanceled )
        {
        // Items are changed so we don't want to keep old ones
        // Containers cannot be deleted because application
        // looses their address
        for ( TInt i = 0;
              i < (*iTargetContainer)->PropertyContainerCount();
              i++ )
            {
            CCmSqlPropertyContainer* container =
                            (*iTargetContainer)->PropertyContainer( i );
            for ( TInt j = 0; j < container->PropertyItemCount(); j++ )
                {
                container->DeletePropertyItem(j--);
                }
            }

        TInt size( 0 );
        TPckg<TInt> sizePckg( size );
        SendReceive( ECmGetDataFieldSize, TIpcArgs( &sizePckg ));

        if ( size > 0 )
            {
            HBufC8* buffer = HBufC8::NewLC( size );

            TPtr8 bufferPointer = buffer->Des();
            SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );

            RDesReadStream stream;
            stream.Open( bufferPointer );
            CleanupClosePushL( stream );

            // copy items to application side containers
            CopyItemsL( stream );

            CleanupStack::PopAndDestroy( 2, buffer );
            }


        }
    User::RequestComplete( iClientRequest, aErr );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::CopyItemsL
// ---------------------------------------------------------------------------
//
void RContentManager::CopyItemsL( RDesReadStream& aStream )
    {
    LOG(_L("[Cm Server]\t RContentManager::CopyItemsL"));

    CCmSqlPropertyCollector* collector = CCmSqlPropertyCollector::NewLC();
    collector->InternalizeL( aStream );

    // append internalized items to containers
    for ( TInt k = 0; k < collector->PropertyContainerCount(); k++ )
        {
        CCmSqlPropertyContainer* container =
                                collector->PropertyContainer( k );
        CCmSqlPropertyContainer* targetContainer =
                                (*iTargetContainer)->PropertyContainer(k);
        for ( TInt l = 0; l < container->PropertyItemCount(); l++ )
            {
            // copy item
            // must be done so that we can destroy collector
            CCmSqlPropertyItem* item = new ( ELeave )
                            CCmSqlPropertyItem( *container->PropertyItem(l));
            CleanupStack::PushL( item );
            // add copied item
            targetContainer->AddPropertyItemL( item );
            CleanupStack::Pop( item );
            // delete original item
            container->DeletePropertyItem( l );
            // itemcount decreased so index must be updated also
            l--;
            }
        }
    CleanupStack::PopAndDestroy( collector );
    }

// ---------------------------------------------------------------------------
// RContentManager::Cancel
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::Cancel()
    {
    LOG(_L("[Cm Server]\t RContentManager::Cancel\n"));

    // iCanceled is set ETrue if user is closing the application
    // We don't want to cancel asynchronous process

    TRACE(Print(_L("[Cm Server]\t RContentManager::Cancel value is %d\n"),
        iCanceled));
    if ( !iCanceled )
        {
        SendReceive( ECmCancel, TIpcArgs() );
        }

    if ( iAo )
        {
        iAo->Deque();
        delete iAo; iAo = NULL;
        }
        
    LOG(_L("[Cm Server]\t RContentManager::Cancel end\n"));
    }
    
// ---------------------------------------------------------------------------
// RContentManager::ServerState
// ---------------------------------------------------------------------------
EXPORT_C TCmServerState RContentManager::ServerState() const
    {
    LOG(_L("[Cm Server]\t RContentManager::ServerState\n"));

    TCmServerState state;
    TPckg<TCmServerState> pckg( state );
    SendReceive( ECmGetServerState, TIpcArgs( &pckg ) );
    
    return state;
    }

// ---------------------------------------------------------------------------
// RContentManager::SetServiceState
// ---------------------------------------------------------------------------
EXPORT_C TInt RContentManager::SetServiceState( const TCmService aService,
                               const TCmServiceState aNewState )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetServiceState\n"));
    
    TRACE(Print(_L("[Cm Server]\t service %d state %d \n"),
        aService, (TInt)aNewState ));

    SendReceive( ECmSetServiceState, TIpcArgs( aService, aNewState ) );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::ServiceState
// ---------------------------------------------------------------------------
EXPORT_C TCmServiceState RContentManager::ServiceState
    ( const TCmService aService ) const
    {
    LOG(_L("[Cm Server]\t RContentManager::ServiceState\n"));
    
    TCmServiceState state;
    TPckg<TCmServiceState> pckg( state );
    SendReceive( ECmGetServiceState, TIpcArgs( aService, &pckg ) );
    
    return state;
    }

// ---------------------------------------------------------------------------
// RContentManager::ExecuteService
// ---------------------------------------------------------------------------
EXPORT_C TInt RContentManager::ExecuteService( const TCmService aService,
                                               TRequestStatus& aStatus )
    {
    TRACE( Print (_L("[Cm Server]\t RContentManager::ExecuteServiceL \
    aService = %d\n"), aService ));
   
    SendReceive( ECmExecuteService, TIpcArgs( aService ), aStatus );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::GetItemCountsL
// ---------------------------------------------------------------------------

EXPORT_C TInt RContentManager::GetItemCountsL( const TCmService aService,
                                               const TDesC8& aListName,
                                               TInt& aCount,
                                               TInt64& aSize ) const
    {
    TRACE( Print( _L("[Cm Server]\t RContentManager::GetItemCountsL \
    service = %d\n"), aService ));

    if ( aService != ECmServiceFill &&
         aService != ECmServiceStore )
        {
        LOG(_L("[Cm Server]\t RContentManager::ItemCount wrong argument\n"));
        User::Leave( KErrArgument );
        }
    TPckg<TInt> countPckg( aCount );
    TPckg<TInt64> sizePckg( aSize );

    SendReceive( ECmGetItemCount,
        TIpcArgs( &countPckg, &sizePckg, aService, &aListName ) );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Get filled files size
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::GetFilledFilesSize( TInt64& aSize, const TInt aID ) const
    {
    TPckg<TInt64> sizePckg( aSize );
    SendReceive( ECmGetFilledFilesSize, TIpcArgs( &sizePckg, aID ) );
    }

// RContentManager::GetTransferInfo
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::GetTransferInfo( const TCmService aService,
                               TInt& aItemCount,
                               TInt& aTimeInSec ) const
    {
    TRACE( Print( _L("[Cm Server]\t RContentManager::GetTransferInfo \
    service = %d\n"), aService ));
    TPckg<TInt> countPckg( aItemCount );
    TPckg<TInt> timePckg( aTimeInSec );
    SendReceive( ECmGetTransferInfo,
        TIpcArgs( aService, &countPckg, &timePckg ) );
    }

// ---------------------------------------------------------------------------
// RContentManager::GetFilteredMetadataL
// ---------------------------------------------------------------------------
EXPORT_C TInt RContentManager::GetFilteredMetadataL(
                            const TCmService aService,
                            CCmSqlPropertyCollector*& aCollector,
                            TRequestStatus& aStatus )
    {
    TRACE( Print( _L("[Cm Server]\t RContentManager::GetFilteredMetadataL \
    service = %d\n"), aService ));

    if ( aService != ECmServiceFill &&
         aService != ECmServiceStore )
        {
        LOG(_L("[Cm Server]\t RContentManager::GetFilteredMetadataL \
        wrong argument\n"));
        User::Leave( KErrArgument );
        }

    iCanceled = EFalse;

    if ( iAo )
        {
        delete iAo; iAo = NULL;
        }
    iAo = CCmActive::NewL( *this );

    iBuffer = CBufFlat::NewL( KKilo );
    RBufWriteStream stream;
    stream.Open( *iBuffer );
    CleanupClosePushL( stream );
    aCollector->ExternalizeL( stream );
    
    iBufferPointer.Set( iBuffer->Ptr(0) );
    SendReceive( ECmGetDataFields,
                 TIpcArgs( &iBufferPointer ),
                 iAo->iStatus);

    CleanupStack::PopAndDestroy(); // stream

    iAo->RunL();
    
    aStatus = KRequestPending;
    iTargetContainer = &aCollector;

    iClientRequest = &aStatus;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::DeleteMetadataL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::UpdateFillListsL( TRequestStatus& aStatus )
    {
    LOG(_L("[Cm Server]\t RContentManager::UpdateFillListsL\n"));
    SendReceive( ECmUpdateFillLists, aStatus );
    }

// ---------------------------------------------------------------------------
// Deleting metadata harvested from unselected servers
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::DeleteMetadataL(
    TRequestStatus& aStatus )
    {
    LOG(_L("[Cm Server]\t RContentManager::DeleteMetadataL\n"));
    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
    iBuffer = CBufFlat::NewL( KKilo );
    RBufWriteStream stream;
    stream.Open( *iBuffer );
    CleanupClosePushL( stream );

    iBufferPointer.Set( iBuffer->Ptr(0) );
    SendReceive( ECmDeleteMetadata,
                 TIpcArgs( &iBufferPointer ),
                 aStatus );
    CleanupStack::PopAndDestroy( &stream );
    }

// ---------------------------------------------------------------------------
// RContentManager::LastSyncTime
// ---------------------------------------------------------------------------
EXPORT_C TTime RContentManager::LastSyncTime() const
    {
    LOG(_L("[Cm Server]\t RContentManager::LastSyncTime\n"));
    TTime syncTime( 0 );
    TPckg<TTime> timePckg( syncTime );
    SendReceive( ECmGetSyncTime, TIpcArgs( &timePckg ) );
    return syncTime;
    }


// ---------------------------------------------------------------------------
// RContentManager::SetFillFileStatusesL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::SetFillFileStatusesL(
            const RPointerArray<CCmBaseListItem>& aItems )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetFillFileStatusesL\n"));

    CBufFlat* buffer = CBufFlat::NewL( KKilo );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream;
    stream.Open( *buffer );
    CleanupClosePushL( stream );
    for ( TInt i = 0; i < aItems.Count(); i++ )
        {
        aItems[ i ]->ExternalizeL( stream );
        }

    TPtrC8 bufferPointer = buffer->Ptr(0);
    SendReceive( ECmSetFillFileStatuses,
                 TIpcArgs( &bufferPointer, aItems.Count() ) );
    CleanupStack::PopAndDestroy( 2, buffer );
    }

// ---------------------------------------------------------------------------
// RContentManager::AppWizardState
// ---------------------------------------------------------------------------

EXPORT_C TBool RContentManager::AppWizardState() const
    {
    LOG(_L("[Cm Server]\t RContentManager::AppWizardState\n"));

    TInt appWizardState(0);
    TPckg<TInt> wizardStatepckg( appWizardState );
    SendReceive( ECmGetAppWizardState, TIpcArgs( &wizardStatepckg ) );

    TBool returnvalue = EFalse;
    if ( appWizardState == KAppWizardExecuted )
        {
        returnvalue = ETrue;
        }
    return returnvalue;
    }

// ---------------------------------------------------------------------------
// RContentManager::SetAppWizardState
// ---------------------------------------------------------------------------

EXPORT_C TInt RContentManager::SetAppWizardState()
    {
    LOG(_L("[Cm Server]\t RContentManager::SetAppWizardState\n"));

    SendReceive( ECmSetAppWizardState );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::SetFillRulesL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::SetFillRulesL(
    const CCmFillRuleContainer& aFillRules )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetFillRulesL\n"));

#ifdef _DEBUG
    CCmFillRuleContainer* temp = (CCmFillRuleContainer*) &aFillRules;
    TInt count = temp->FillRuleCount();
    TRACE(Print(_L("[Cm Server]\t fill rule count is %d\n"), count));
#endif

    CBufFlat* buffer = CBufFlat::NewL( KKilo );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream;
    stream.Open( *buffer );
    CleanupClosePushL( stream );
    aFillRules.ExternalizeL( stream );
    TPtrC8 bufferPointer = buffer->Ptr(0);
    SendReceive( ECmSetFillRules, TIpcArgs( &bufferPointer ) );
    CleanupStack::PopAndDestroy( 2, buffer );
    }

// ---------------------------------------------------------------------------
// RContentManager::FillRulesL
// ---------------------------------------------------------------------------
EXPORT_C CCmFillRuleContainer* RContentManager::FillRulesL()
    {
    LOG(_L("[Cm Server]\t RContentManager::FillRulesL\n"));
    TInt size( 0 );
    TPckg<TInt> pckg( size );
    SendReceive( ECmGetFillRulesSize, TIpcArgs( &pckg ));
    // for some reason the sendreceive sometimes leaves size as negative.
    // leave if this is the case.
    if ( size < 0 )
        {
        User::Leave( KErrArgument );
        }
    TRACE(Print(_L("[Cm Server]\t RContentManager: buffer size is %d\n"),
        size) );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPointer = buffer->Des();
    SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );

    RDesReadStream stream;
    stream.Open( bufferPointer );
    CleanupClosePushL( stream );
    CCmFillRuleContainer* container = CCmFillRuleContainer::NewLC();
    container->InternalizeL( stream );
    CleanupStack::Pop( container );
    CleanupStack::PopAndDestroy( 2, buffer );
    return container;
    }


// ---------------------------------------------------------------------------
// RContentManager::ListItemsL
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CCmBaseListItem>*
            RContentManager::ListItemsL( const TDesC8& aListName,
                                         TCmService aService )
    {
    LOG(_L("[Cm Server]\t RContentManager::ListItemsL"));
    
#ifdef _DEBUG
    HBufC* listName16 = HBufC::NewLC( aListName.Length() );
    listName16->Des().Copy( aListName );
    TRACE( Print(_L("[Cm Server]\t RContentManager::FillListItemsL \
    list name %S"), listName16 ) );
    CleanupStack::PopAndDestroy( listName16 );
#endif

    TInt size( 0 );
    TInt itemCount( 0 );
    TPckg<TInt> sizePckg( size );
    TPckg<TInt> countPckg( itemCount );

    // Switch statement ensures that SendReceive is not called with other
    // services that ECmGetFillItemsSize and ECmGetStoreItemsSize
    switch ( aService )
        {
        case ECmServiceFill:
            {
            SendReceive( ECmGetFillItemsSize,
                         TIpcArgs( &aListName, &sizePckg, &countPckg ) );

            break;
            }
        case ECmServiceStore:
            {
            SendReceive( ECmGetStoreItemsSize,
                         TIpcArgs( &aListName, &sizePckg, &countPckg ) );
            break;
            }
        default:
            {
            LOG(_L("[Cm Server]\t RContentManager::ListItemsL \
            service not supported"));
            break;
            }

        }

    TRACE( Print( _L("[Cm Server]\t RContentManager::ListItemsL \
    size = %d, itemCount = %d"), size, itemCount ));

    // Sendreceive leaves size or itemCount as negative
    if ( size < 0 || itemCount < 0 )
        {
        User::Leave( KErrArgument );
        }

    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPointer = buffer->Des();
    SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );
    RDesReadStream stream;
    stream.Open( bufferPointer );
    CleanupClosePushL( stream );

    RPointerArray<CCmBaseListItem>* itemArray =
        new (ELeave)RPointerArray<CCmBaseListItem>();
    CleanupStack::PushL( itemArray );
    for( TInt i = 0; i < itemCount; i++ )
        {
        CCmBaseListItem* item = CCmBaseListItem::NewLC();
        item->InternalizeL( stream );
        itemArray->AppendL( item );
        CleanupStack::Pop( item );
        }

    CleanupStack::Pop( itemArray );
    CleanupStack::PopAndDestroy( 2, buffer );
    return itemArray;

    }

// ---------------------------------------------------------------------------
// RContentManager::PreProcessFillList
// ---------------------------------------------------------------------------
EXPORT_C TInt RContentManager::PreProcessFillList( const TDesC8& aListName,
                                                   TRequestStatus& aStatus )
    {
    LOG(_L("[Cm Server]\t RContentManager::PreProcessFillList\n"));

    SendReceive( ECmPreProcessFillList, TIpcArgs( &aListName ), aStatus );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::MediaserversL
// ---------------------------------------------------------------------------

EXPORT_C RPointerArray<CCmMediaServerFull>*
    RContentManager::MediaserversL() const
    {
    LOG(_L("[Cm Server]\t RContentManager::MediaServersL\n"));
    TInt size( 0 );
    TInt amount( 0 );
    TPckg<TInt> pckg( size );
    TPckg<TInt> pckg2( amount );

    SendReceive( ECmGetMediaserversSize, TIpcArgs( &pckg, &pckg2 ) );
    // Sendreceive leaves size or amount as negative
    if ( size < 0 || amount < 0 )
        {
        User::Leave( KErrArgument );
        }
        
    TRACE(Print(_L("[Cm Server]\t RContentManager: \
        buffer size is %d, amount %d\n"), size, amount));

    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPointer = buffer->Des();
    SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );

    RDesReadStream stream;
    stream.Open( bufferPointer );
    CleanupClosePushL( stream );

    RPointerArray<CCmMediaServerFull>* mediaServers =
        new (ELeave) RPointerArray<CCmMediaServerFull>();
    CleanupStack::PushL( mediaServers );

    for ( TInt i = 0; i < amount; i++ )
        {
        CCmMediaServerFull* tempServer = CCmMediaServerFull::NewLC();
        tempServer->InternalizeL( stream );
        mediaServers->Append( tempServer );
        CleanupStack::Pop( tempServer );
        }
    CleanupStack::Pop( mediaServers );
    CleanupStack::PopAndDestroy( 2, buffer );
    return mediaServers;
    }

// ---------------------------------------------------------------------------
// RContentManager::SetMediaserversL
// ---------------------------------------------------------------------------
EXPORT_C TInt RContentManager::SetMediaserversL(
    RPointerArray<CCmMediaServerFull>& aMediaServers )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetMediaserversL\n"));

    CBufFlat* buffer = CBufFlat::NewL( KKilo );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream;
    stream.Open( *buffer );
    CleanupClosePushL( stream );
    for ( TInt i = 0; i < aMediaServers.Count(); i++ )
        {
        aMediaServers[ i ]->ExternalizeL( stream );
        }

    TPtrC8 bufferPointer = buffer->Ptr(0);
    SendReceive( ECmSetMediaServers,
                 TIpcArgs( &bufferPointer, aMediaServers.Count() ) );
    CleanupStack::PopAndDestroy( 2, buffer );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// RContentManager::DeleteDeletedMediaserversL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::DeleteDeletedMediaserversL()
    {
    LOG(_L("[Cm Server]\t RContentManager::DeleteDeletedMediaserversL\n"));
    SendReceive( ECmDeleteDeletedMediaServers );
    }

// ---------------------------------------------------------------------------
// RContentManager::SetStoreRulesL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::SetStoreRulesL(
    const CCmStoreRuleContainer& aStoreRules )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetStoreRulesL\n"));

#ifdef _DEBUG
    CCmStoreRuleContainer* temp = (CCmStoreRuleContainer*) &aStoreRules;
    TInt count = temp->StoreRuleCount();
    TRACE(Print(_L("[Cm Server]\t store rule count is %d\n"), count));
#endif

    CBufFlat* buffer = CBufFlat::NewL( KKilo );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream;
    stream.Open( *buffer );
    CleanupClosePushL( stream );
    aStoreRules.ExternalizeL( stream );
    TPtrC8 bufferPointer = buffer->Ptr(0);
    SendReceive( ECmSetStoreRules, TIpcArgs( &bufferPointer ) );
    CleanupStack::PopAndDestroy( 2, buffer );

    }

// ---------------------------------------------------------------------------
// RContentManager::StoreRulesL
// ---------------------------------------------------------------------------
EXPORT_C CCmStoreRuleContainer* RContentManager::StoreRulesL()
    {
    LOG(_L("[Cm Server]\t RContentManager::StoreRulesL\n"));
    TInt size( 0 );
    TPckg<TInt> pckg( size );
    SendReceive( ECmGetStoreRulesSize, TIpcArgs( &pckg ) );
    // Sendreceive leaves size as negative
    if ( size < 0 )
        {
        User::Leave( KErrArgument );
        }
        
    TRACE(Print(_L("[Cm Server]\t RContentManager: buffer size is %d\n"),
        size));
    
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPointer = buffer->Des();
    SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );

    RDesReadStream stream;
    stream.Open( bufferPointer );
    CleanupClosePushL( stream );
    CCmStoreRuleContainer* container = CCmStoreRuleContainer::NewLC();
    container->InternalizeL( stream );
    CleanupStack::Pop( container );
    CleanupStack::PopAndDestroy( 2, buffer );
    return container;
    }

// ---------------------------------------------------------------------------
// RContentManager::SetDrivesL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::SetDrivesL(
    RPointerArray<CCmDriveInfo> aDrives )
    {
    LOG(_L("[Cm Server]\t RContentManager::SetDrivesL\n"));

    CBufFlat* buffer = CBufFlat::NewL( KKilo );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream;
    stream.Open( *buffer );
    CleanupClosePushL( stream );
    
    for ( TInt i = 0; i < aDrives.Count(); i++ )
        {
        aDrives[ i ]->ExternalizeL( stream );
        }
    
    TPtrC8 bufferPointer = buffer->Ptr(0);
    SendReceive( ECmSetDrives,
                 TIpcArgs( &bufferPointer, aDrives.Count() ) );
    CleanupStack::PopAndDestroy( 2, buffer );
    }

// ---------------------------------------------------------------------------
// RContentManager::GetDrivesL
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::GetDrivesL(
    RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG(_L("[Cm Server]\t RContentManager::GetDrivesL\n"));
    TInt size( 0 );
    TInt amount( 0 );
    TPckg<TInt> pckg( size );
    TPckg<TInt> pckg2( amount );
    SendReceive( ECmGetDrivesSize, TIpcArgs( &pckg, &pckg2 ) );
    // Sendreceive leaves size or amount as negative
    if ( size < 0 || amount < 0 )
        {
        User::Leave( KErrArgument );
        }
        
    TRACE(Print(_L("[Cm Server]\t RContentManager: \
        buffer size is %d, amount %d\n"), size, amount));
    
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPointer = buffer->Des();
    SendReceive( ECmGetDataBuffer, TIpcArgs( &bufferPointer ) );

    RDesReadStream stream;
    stream.Open( bufferPointer );
    CleanupClosePushL( stream );

    for ( TInt i = 0; i < amount; i++ )
        {
        CCmDriveInfo* tempDrive = CCmDriveInfo::NewLC();
        tempDrive->InternalizeL( stream );
        aDrives.AppendL( tempDrive );
        CleanupStack::Pop( tempDrive );
        }
    CleanupStack::PopAndDestroy( 2, buffer );
    }

// ---------------------------------------------------------------------------
// RContentManager::DriveSelectionStatus
// ---------------------------------------------------------------------------
EXPORT_C TBool RContentManager::DriveSelectionStatus()
    {
    LOG(_L("[Cm Server]\t RContentManager: DriveSelectionStatus\n"));

    TBool status( EFalse );
    TPckg<TBool> pckg( status );

    SendReceive( ECmGetDriveSelectionStatus, TIpcArgs( &pckg ) );
    return status;
    }

// ---------------------------------------------------------------------------
// RContentManager::ApplicationExit
// ---------------------------------------------------------------------------
EXPORT_C void RContentManager::ApplicationExit()
    {
    LOG(_L("[Cm Server]\t RContentManager: ApplicationExit\n"));
    iCanceled = ETrue;

    SendReceive( ECmApplicationExit, TIpcArgs() );
    }
    
// End of file
