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
* Description:  Session implementation class for Content Manager
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <s32mem.h>

#include    "cmcommon.h"
#include    "cmfillrulecontainer.h"
#include    "cmfillrule.h"
#include    "cmstorerulecontainer.h"
#include    "cmstorerule.h"
#include    "cmbaselistitem.h"
#include    "cmdmmain.h"
#include    "cmsettingsfactory.h"
#include    "cmsettings.h"
#include    "cmmediaserverfull.h"
#include    "cmsqlpropertycontainer.h"
#include    "cmsqlpropertycollector.h"
#include    "cmdriveinfo.h"
#include    "cmserversession.h"
#include    "cmserver.h"
#include    "msdebug.h"

// data buffer size
const TInt KBufferExpandSize = KKilo;


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CCmSession::CCmSession
// ---------------------------------------------------------------------------
//
CCmSession::CCmSession( CCmDmMain& aDbManager, MCmSettings& aSettings )
    : iDbManager( aDbManager ), iSettings( aSettings )
    {
    }

// ---------------------------------------------------------------------------
// CCmSession::NewL
// ---------------------------------------------------------------------------
//
CCmSession* CCmSession::NewL( CCmDmMain& aDbManager,
                              MCmSettings& aSettings )
    {
    LOG(_L("[Cm Server]\t CCmSession::NewL"));
    CCmSession* self =
        new (ELeave) CCmSession( aDbManager, aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSession::ConstructL
// ---------------------------------------------------------------------------
//
void CCmSession::ConstructL()
    {
    LOG(_L("[Cm Server]\t CCmSession::ConstructL"))
    iCollector = CCmSqlPropertyCollector::NewL();
    }


// ---------------------------------------------------------------------------
// CCmSession::~CCmSession
// ---------------------------------------------------------------------------
//
CCmSession::~CCmSession()
    {
    LOG(_L("[Cm Server]\t CCmSession::~CCmSession"));

    delete iDataBuffer;

    Server().DecrementSessions();

    delete iCollector;
    }



// ---------------------------------------------------------------------------
// CCmSession::Server
// Returns a reference to server
// ---------------------------------------------------------------------------
//
CCmServer& CCmSession::Server()
    {
    LOG(_L("[Cm Server]\t CCmSession::Server"));
    
    return *static_cast<CCmServer*>(const_cast<CServer2*>
        (CSession2::Server()));
    }



// ---------------------------------------------------------------------------
// CCmSession::ServiceL
// Implements CSession2 -derived ServiceL -method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmSession::ServiceL(const RMessage2& aMessage)
    {
    LOG(_L("[Cm Server]\t CCmSession::ServiceL"));

    switch ( aMessage.Function() )
        {
        case ECmExecuteService:
            {
            LOG(_L("[Cm Server]\t ECmExecuteService request"));
            TCmService service = (TCmService)aMessage.Int0();

            TRACE( Print (_L("[Cm Server]\t CCmSession::\
            ServiceL ECmExecuteService service = %d\n"), service ));

            Server().SetCurrentAsyncMsg( aMessage );
            Server().ExecuteServiceL( service );
            break;
            }

        case ECmCancel:
            {
            LOG(_L("[Cm Server]\t ECmCancel request"));
            Server().CancelServiceL();

            aMessage.Complete( KErrNone );
            break;
            }

        case ECmGetServerState:
            {
            LOG(_L("[Cm Server]\t ECmGetServerState request"));
            TPckg<TCmServerState> pckg( Server().ServerState() );
            aMessage.WriteL(0, pckg );
            aMessage.Complete( KErrNone );
            break;
            }

        case ECmGetServiceState:
            {
            LOG(_L("[Cm Server]\t ECmGetServiceState request"));
            TCmService service = static_cast<TCmService>(aMessage.Int0());

            TRACE( Print (_L("[Cm Server]\t CCmSession::\
            ServiceL ECmGetServiceState service = %d\n"), service ));

            TCmServiceState state = ECmServiceStateDisabled;
            TInt err( KErrNone );
            switch ( service )
                {
                case ECmServiceFill:  // fall through
                case ECmServiceStore: // fall through
                case ECmServiceContentManager:
                    {
                    iSettings.GetServiceState( ECmServiceContentManager,
                                               state );
                    if ( state )
                        {
                        TBool autoSync = EFalse;
                        iSettings.GetAutoSync( autoSync );
                        if ( autoSync )
                            {
                            state = ECmServiceStateAutomatic;
                            }
                        else
                            {
                            LOG(_L("[Cm Server]\t ECmGetServiceState \
                            request autosync = disabled"));
                            }
                        }
                    else
                        {
                        LOG(_L("[Cm Server]\t ECmGetServiceState request \
                        state = ECmServiceStateDisabled"));
                        }
                    break;
                    }
                case ECmServiceMemoryManager:
                    {
                    TBool mm = EFalse;
                    iSettings.GetMemoryManagerStatus( mm );
                    state = ECmServiceStateAutomatic;
                    if ( !mm )
                        {
                        state = ECmServiceStateDisabled;
                        }

                    break;
                    }
                default:
                    {
                    err = KErrArgument;
                    break;
                    }
                }
            TPckg<TCmServiceState> pckg( state );
            aMessage.WriteL(1, pckg );
            aMessage.Complete( err );
            break;
            }

        case ECmSetServiceState:
            {
            LOG(_L("[Cm Server]\t ECmSetServiceState request"));

            TCmService service = static_cast<TCmService>(aMessage.Int0());
            TCmServiceState state =
                static_cast<TCmServiceState>(aMessage.Int1());

            TRACE(Print(_L("[Cm Server]\t service %d state %d \n"),
                service, state ));

            Server().SetServiceStateL( service, state );

            aMessage.Complete( KErrNone );
            break;
            }

        case ECmGetItemCount:
            {
            LOG(_L("[Cm Server]\t ECmGetItemCount request"));
            TCmService service = static_cast<TCmService>(aMessage.Int2());

            TRACE( Print (_L("[Cm Server]\t CCmSession::\
            ServiceL ECmGetItemCount service = %d\n"), service ));

            TInt len = aMessage.GetDesLength( 3 );
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmGetItemCount request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            HBufC8* name = HBufC8::NewLC( len );
            TPtr8 ptr( name->Des() );
            aMessage.ReadL( 3, ptr );
            TInt64 size = 0;
            TInt count = 0;

            if ( service == ECmServiceFill )
                {
                size = iDbManager.ListSize(
                    ECmFmFillRule,
                    *name,
                    ECmFilled | ECmToBeFilled | ECmToBeShrinked
                );
                iDbManager.GetFillFileCount(
                    *name,
                    ECmFilled | ECmToBeFilled | ECmToBeShrinked,
                    count
                );
                TRACE(Print(_L("[Cm Server]\t got fill size %ld count %d"),
                    size, count));
                }
            else // ECmServiceStore
                {
                size = iDbManager.ListSize(
                    ECmFmStoreRule,
                    *name,
                    ECmToBeRemoved | ECmToBeShrinked | ECmKeepOnDevice
                );
                iDbManager.GetStoreFileCount(
                    *name,
                    ECmToBeRemoved | ECmToBeShrinked | ECmKeepOnDevice,
                    count
                );
                TRACE(Print(_L("[Cm Server]\t got store size %ld count %d"),
                    size, count));
                }
            CleanupStack::PopAndDestroy( name );
            TPckg<TInt> countPckg( count );
            TPckg<TInt64> sizePckg( size );
            aMessage.WriteL( 0, countPckg );
            aMessage.WriteL( 1, sizePckg );

            aMessage.Complete( KErrNone );
            break;
            }

        case ECmGetTransferInfo:
            {
            LOG(_L("[Cm Server]\t ECmGetTransferInfo request"));
            TCmService service = static_cast<TCmService>(aMessage.Int0());
            TInt count( 0 );
            TInt time( 0 );
            TPckg<TInt> countPckg( count );
            TPckg<TInt> timePckg( time );

            iDbManager.GetTransferInfo(
                service, count, time );

            TRACE(Print(_L("[Cm Server]\t ECmGetTransferInfo \
            count = %d, time = %d"), count, time));

            aMessage.WriteL( 1, countPckg );
            aMessage.WriteL( 2, timePckg );
            aMessage.Complete( KErrNone );
            break;
            }


        case ECmGetDataFields:
            {
            LOG(_L("[Cm Server]\t ECmGetDataFields request"));

            TInt len = aMessage.GetDesLength(0);
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmGetDataFields request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            iCollector->InternalizeL( stream );

            Server().SetCurrentAsyncMsg( aMessage );
            Server().ExecuteServiceL( ECmServiceGetFillFields,
                                      0,
                                      iCollector );
            CleanupStack::PopAndDestroy( 2 );
            break;
            }

        case ECmGetDataFieldSize:
            {
            LOG(_L("[Cm Server]\t ECmGetDataFieldSize request"));

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream propertyStream;
            propertyStream.Open( *iDataBuffer );
            CleanupClosePushL( propertyStream );

            CCmSqlPropertyCollector* propertyCollector =
                Server().PropertyCollector();

            if( !propertyCollector )
                {
                LOG(_L("[Cm Server]\t ECmGetDataFieldSize request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            propertyCollector->ExternalizeL( propertyStream );

            TInt size = iDataBuffer->Size();
            TPckg<TInt> pckg( size );
            aMessage.WriteL(0, pckg );
            CleanupStack::PopAndDestroy( &propertyStream );

            TRACE(Print(_L("[Cm Server]\t buffer size is %d"), size));

            aMessage.Complete( KErrNone );
            break;
            }

        case ECmGetDataBuffer:
            {
            TInt errCode = KErrNone;
            if ( !iDataBuffer )
                {
                LOG(_L("[Cm Server]\t ECmGetDataBuffer \
                databuffer doesn't exist"));
                errCode = KErrNotReady;
                }

            if (errCode == KErrNone )
                {
                aMessage.WriteL(0, iDataBuffer->Ptr(0));
                }
            aMessage.Complete( errCode );
            break;
            }

        case ECmGetFillRulesSize:
            {
            LOG(_L("[Cm Server]\t ECmGetFillRulesSize request"));

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream ruleStream;
            ruleStream.Open( *iDataBuffer );
            CleanupClosePushL( ruleStream );

            CCmFillRuleContainer* ruleContainer =
                Server().FillRuleContainerL();
            CleanupStack::PushL( ruleContainer );
            ruleContainer->ExternalizeL( ruleStream );
            CleanupStack::PopAndDestroy( ruleContainer );

            TInt size = iDataBuffer->Size();
            TPckg<TInt> pckg( size );
            aMessage.WriteL(0, pckg );
            CleanupStack::PopAndDestroy( &ruleStream );

            TRACE(Print(_L("[Cm Server]\t buffer size is %d"), size));

            aMessage.Complete( KErrNone );
            LOG(_L("[Cm Server]\t ECmGetFillRulesSize request end"));

            break;
            }

        case ECmGetStoreItemsSize:
            {
            LOG(_L("[Cm Server]\t ECmGetStoreItemsSize request"));

            TInt len = aMessage.GetDesLength( 0 );
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmGetStoreItemsSize request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            HBufC8* name = HBufC8::NewLC( len );
            TPtr8 ptr( name->Des() );
            aMessage.ReadL(0, ptr);

            iDbManager.PrepareQueryCmdL( EStoreFilesQuery );
            RPointerArray<CCmBaseListItem> items;
            CleanupClosePushL( items );

            iDbManager.QueryStoreFileListL(
                items,
                *name,
                ECmToBeRemoved | ECmToBeShrinked | ECmKeepOnDevice
            );
            
            TInt count = items.Count();

            TRACE(Print(_L("[Cm Server]\t store item array size is %d"),
                count));

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream ruleStream;
            ruleStream.Open( *iDataBuffer );
            CleanupClosePushL( ruleStream );

            for ( TInt i = 0; i < count; i++ )
                {
                items[i]->ExternalizeL( ruleStream );
                }

            CleanupStack::PopAndDestroy( &ruleStream );
            items.ResetAndDestroy();
            CleanupStack::PopAndDestroy( &items );
            CleanupStack::PopAndDestroy( name );

            TInt size = iDataBuffer->Size();

            TPckg<TInt> sizePckg( size );
            aMessage.WriteL( 1, sizePckg );
            TRACE(Print(_L("[Cm Server]\t buffer size is %d"), size));            

            TPckg<TInt> countPckg( count );
            aMessage.WriteL( 2, countPckg );
            TRACE(Print(_L("[Cm Server]\t item count is %d"), count));

            aMessage.Complete( KErrNone );

            LOG(_L("[Cm Server]\t ECmGetStoreItemsSize request end"));

            break;
            }

        case ECmGetFillItemsSize:
            {
            LOG(_L("[Cm Server]\t ECmGetFillItemsSize request"));

            TInt len = aMessage.GetDesLength( 0 );
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmGetFillItemsSize request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            HBufC8* name = HBufC8::NewLC( len );
            TPtr8 ptr( name->Des() );
            aMessage.ReadL(0, ptr);

            iDbManager.PrepareQueryCmdL( EFillFilesQuery );
            RPointerArray<CCmBaseListItem> items;
            CleanupClosePushL( items );

            iDbManager.QueryFillFileListL(
                items,
                *name,
                ECmFilled | ECmToBeFilled | ECmToBeShrinked | ECmLocalCopy
            );
            
            TInt count = items.Count();

            TRACE(Print(_L("[Cm Server]\t list item array size is %d"),
                count));

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream ruleStream;
            ruleStream.Open( *iDataBuffer );
            CleanupClosePushL( ruleStream );

            for ( TInt i = 0; i < count; i++ )
                {
                items[i]->ExternalizeL( ruleStream );
                }

            CleanupStack::PopAndDestroy( &ruleStream );
            items.ResetAndDestroy();
            CleanupStack::PopAndDestroy( &items );
            CleanupStack::PopAndDestroy( name );
            TInt size = iDataBuffer->Size();

            TPckg<TInt> sizePckg( size );
            aMessage.WriteL( 1, sizePckg );
            TRACE(Print(_L("[Cm Server]\t buffer size is %d"), size));            

            TPckg<TInt> countPckg( count );
            aMessage.WriteL( 2, countPckg );
            TRACE(Print(_L("[Cm Server]\t item count is %d"), count));

            aMessage.Complete( KErrNone );

            LOG(_L("[Cm Server]\t ECmGetStoreItemsSize request end"));

            break;
            }

        case ECmSetFillRules:
            {
            LOG(_L("[Cm Server]\t ECmSetFillRules request"));

            TInt len = aMessage.GetDesLength(0);
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmSetFillRules request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            CCmFillRuleContainer* tempContainer =
                CCmFillRuleContainer::NewLC();
            tempContainer->InternalizeL( stream );
            iDbManager.DeleteAllFillRules();
            for ( TInt i = 0; i < tempContainer->FillRuleCount(); i++ )
                {
#ifdef _DEBUG
                TDesC8& name = tempContainer->FillRule( i )->Name();
                TBuf<KMaxFileName> buf;
                buf.Copy(name);
                TRACE(Print(_L("[Cm Server]\t updating fill rule %S"), &buf));
#endif

                if ( tempContainer->FillRule( i )->Selected() == ECmDeleted )
                    {
                    TRACE(Print(_L("[Cm Server]\t skipping deleted rule %S"),
                        &buf));
                    iDbManager.SetFillListStateL(
                        tempContainer->FillRule( i )->ListId(), ECmDeleted );
                    }
                else
                    {
                    TRACE(Print(_L("[Cm Server]\t storing rule %S to db"),
                        &buf));
                    TInt ret = iDbManager.AddFillRuleL(
                        tempContainer->FillRule( i ));
                    TRACE(Print(_L("[Cm Server]\t ret = %d"), ret));
                    }
                }
            aMessage.Complete( KErrNone );
            CleanupStack::PopAndDestroy( 3, buffer );
            break;
            }

        case ECmPreProcessFillList:
            {
            LOG(_L("[Cm Server]\t ECmPreProcessFillList request"));

            TInt len = aMessage.GetDesLength( 0 );
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmPreProcessFillList request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            HBufC8* name = HBufC8::NewLC( len );
            TPtr8 ptr( name->Des() );
            aMessage.ReadL(0, ptr);

            TRACE(Print(_L("[Cm Server]\t list name %S"), name ));

            Server().ExecuteServiceL(
                ECmServicePreProcessingFilllist, 0, name );
            Server().SetCurrentAsyncMsg( aMessage );

            CleanupStack::Pop( name );
            break;
            }

        case ECmGetSyncTime:
            {
            LOG(_L("[Cm Server]\t ECmGetSyncTime request"));
            TTime syncTime;
            TInt err = iDbManager.GetSyncTime( syncTime );
            TRACE( Print(_L("[Cm Server]\t returning time %ld"),
                   syncTime.Int64() ) );

            TPckg<TTime> pckg( syncTime );
            aMessage.WriteL( 0, pckg );
            aMessage.Complete( err );
            break;
            }

        case ECmGetMediaserversSize:
            {
            LOG(_L("[Cm Server]\t ECmGetMediaserversSize request"));

            iDbManager.PrepareQueryCmdL( EMediaServersAllQuery );

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream serverStream;
            serverStream.Open( *iDataBuffer );
            CleanupClosePushL( serverStream );

            TInt ret( KErrNone );
            TInt amount = 0;
            while( KErrNotFound != ret )
                {
                CCmMediaServerFull* server = CCmMediaServerFull::NewLC();
                ret = iDbManager.QueryAllMediaServersL( server );
                if( KErrNotFound != ret )
                    {
                    ++amount;
                    server->ExternalizeL( serverStream );
                    }
                CleanupStack::PopAndDestroy( server );
                }

            TInt size = iDataBuffer->Size();
            TPckg<TInt> pckg( size );
            aMessage.WriteL(0, pckg );

            TPckg<TInt> pckg2( amount );
            aMessage.WriteL(1, pckg2 );

            TRACE(Print(_L("[Cm Server]\t buffer size is %d, amount %d"),
                size, amount));

            aMessage.Complete( KErrNone );
            CleanupStack::PopAndDestroy( &serverStream );
            break;
            }

        case ECmGetStoreRulesSize:
            {
            LOG(_L("[Cm Server]\t ECmGetStoreRulesSize request"));
            RPointerArray<HBufC8> array;
            CleanupClosePushL( array );
            iDbManager.PrepareQueryCmdL( EStoreRuleNamesQuery );
            iDbManager.QueryStoreRuleNamesL( array );
            TRACE(Print(_L("[Cm Server]\t store rule array size is %d"),
                array.Count()));

            CCmStoreRuleContainer* ruleContainer =
                CCmStoreRuleContainer::NewLC();
            for ( TInt i = 0; i < array.Count(); i++ )
                {
                CCmStoreRule* rule = CCmStoreRule::NewLC();
                rule->SetNameL( *(array[i]) );
                iDbManager.PrepareQueryCmdL( EStoreRuleQuery );
                iDbManager.QueryStoreRuleL( rule );
                ruleContainer->AddStoreRuleL( rule );
                CleanupStack::Pop( rule );
                }

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream ruleStream;
            ruleStream.Open( *iDataBuffer );
            CleanupClosePushL( ruleStream );

            ruleContainer->ExternalizeL( ruleStream );
            TInt size = iDataBuffer->Size();
            TPckg<TInt> pckg( size );
            aMessage.WriteL(0, pckg );

            TRACE(Print(_L("[Cm Server]\t buffer size is %d"), size));

            aMessage.Complete( KErrNone );
            CleanupStack::PopAndDestroy( 2, ruleContainer );
            array.Reset();
            CleanupStack::PopAndDestroy( &array );
            LOG(_L("[Cm Server]\t ECmGetStoreRulesSize request end"));

            break;
            }

        case ECmSetStoreRules:
            {
            LOG(_L("[Cm Server]\t ECmSetStoreRules request"));

            TInt len = aMessage.GetDesLength(0);
            TInt totalCount(0);
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmSetStoreRules request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            CCmStoreRuleContainer* tempContainer =
                CCmStoreRuleContainer::NewLC();
            tempContainer->InternalizeL( stream );
            iDbManager.DeleteAllStoreRules();
            for ( TInt i = 0; i < tempContainer->StoreRuleCount(); i++ )
                {
                TDesC8& name = tempContainer->StoreRule( i )->Name();
                TBuf<KMaxFileName> buf;
                buf.Copy(name);
                TRACE(Print(_L("[Cm Server]\t updating store rule %S"),
                    &buf));
                TInt ret = iDbManager.AddStoreRuleL(
                    tempContainer->StoreRule( i ));
                TRACE(Print(_L("[Cm Server]\t ret = %d"), ret));

                if( tempContainer->StoreRule( i )->Selected() )
                    {
                    TInt oneRuleCount(0);
                    iDbManager.GetStoreFileCount(
                           name,
                           ECmToBeRemoved | ECmToBeShrinked | ECmKeepOnDevice,
                           oneRuleCount );
                    totalCount += oneRuleCount;
                    }
                }
            TInt time (0);
            TInt count(0);
            iDbManager.GetTransferInfo( ECmServiceStore, count, time );
            iDbManager.UpdateTransferInfo( ECmServiceStore, totalCount, 
                    time ); 

            aMessage.Complete( KErrNone );

            CleanupStack::PopAndDestroy( 3, buffer );
            break;
            }

        case ECmSetMediaServers:
            {
            LOG(_L("[Cm Server]\t ECmSetMediaServers request"));

            TInt mediaServerCount = aMessage.Int1();
            TInt len = aMessage.GetDesLength(0);
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmSetMediaServers request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));
            TRACE(Print(_L("[Cm Server]\t mediaserver count is %d"),
                mediaServerCount ));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            RPointerArray<CCmMediaServerFull> mediaServers;
            CleanupClosePushL( mediaServers);

            for ( TInt i = 0; i < mediaServerCount; i++ )
                {
                CCmMediaServerFull* mediaServer =
                    CCmMediaServerFull::NewLC();
                mediaServer->InternalizeL( stream );
                mediaServers.AppendL( mediaServer );
                CleanupStack::Pop( mediaServer );
                }
            TInt ret = iDbManager.SetMediaServersL( mediaServers );
            TRACE(Print(_L("[Cm Server]\t mediaservers update ret = %d"),
                ret));

            aMessage.Complete( ret );

            mediaServers.ResetAndDestroy();            
            CleanupStack::PopAndDestroy( 3 ); //buffer, &stream, &mediaServers
            break;
            }

        case ECmSetFillFileStatuses:
            {
            LOG(_L("[Cm Server]\t ECmSetFillFileStatuses request"));

            TInt itemCount = aMessage.Int1();
            TInt len = aMessage.GetDesLength(0);
            if( len <= 0 )
                {
                LOG(_L("[Cm Server]\t ECmSetFillFileStatuses request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));
            TRACE(Print(_L("[Cm Server]\t item count is %d"),
                itemCount ));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            RPointerArray<CCmBaseListItem> items;
            CleanupClosePushL( items );

            for ( TInt i = 0; i < itemCount; i++ )
                {
                CCmBaseListItem* item = CCmBaseListItem::NewLC();
                item->InternalizeL( stream );
                items.AppendL( item );
                CleanupStack::Pop( item );
                }
            TInt ret = iDbManager.UpdateStatusValues( items );
            TRACE(Print(_L("[Cm Server]\t fillfile status update ret = %d"),
                ret));

            aMessage.Complete( ret );

            items.ResetAndDestroy();
            CleanupStack::PopAndDestroy( 3 ); //buffer, &stream, &items
            break;
            }

        case ECmGetAppWizardState:
            {
            LOG(_L("[Cm Server]\t ECmGetAppWizardState request"));

            TInt appWizardState( KErrNotFound );
            TInt err;
            err = iSettings.GetAppWizardInformation( appWizardState );
            TPckg<TInt> pckg( appWizardState );
            aMessage.WriteL(0, pckg );
            aMessage.Complete( err );
            break;
            }

        case ECmSetAppWizardState:
            {
            LOG(_L("[Cm Server]\t ECmSetAppWizardState request"));
            TInt err;
            err = iSettings.SetAppWizardInformation();
            aMessage.Complete( err );

            break;
            }
        case ECmUpdateFillLists:
            {
            LOG(_L("[Cm Server]\t ECmUpdateLists request"));
            Server().ExecuteServiceL(
                ECmServiceUpdateFillLists, 0, NULL );
            Server().SetCurrentAsyncMsg( aMessage );
            break;
            }
        case ECmDeleteMetadata:
            {
            LOG(_L("[Cm Server]\t ECmDeleteMetadata request"));

            Server().ExecuteServiceL(
                ECmServiceDeleteMetadata, 0, NULL );
            Server().SetCurrentAsyncMsg( aMessage );
            break;
            }
        case ECmDeleteDeletedMediaServers:
            {
            TRAPD( err, iDbManager.DeleteDeletedMediaServersL() );
            aMessage.Complete( err );
            break;
            }
        case ECmGetDriveSelectionStatus:
            {
            TInt err( KErrNone );
            TPckg<TBool> pckg( Server().DriveSelectionStatus() );
            aMessage.WriteL( 0, pckg );
            aMessage.Complete( err );
            break;
            }
        case ECmSetDrives:
            {
            LOG(_L("[Cm Server]\t ECmSetDrives request"));

            TInt driveCount = aMessage.Int1();
            TInt len = aMessage.GetDesLength(0);
            if( len < 0 )
                {
                LOG(_L("[Cm Server]\t ECmSetDrives request \
                leaving with KErrArgument"));
                User::Leave( KErrArgument );
                }
            TRACE(Print(_L("[Cm Server]\t buffer length is %d"), len));
            TRACE(Print(_L("[Cm Server]\t drive count is %d"),
                driveCount ));

            RPointerArray<CCmDriveInfo> origDrives;
            CleanupClosePushL( origDrives );
            
            iDbManager.PrepareQueryCmdL( EDrivesQuery );
            TRAPD( ret, iDbManager.QueryDrivesL( origDrives ) );
            TRACE(Print(_L("[Cm Server]\t Query orig drives ret = %d"),
                ret ));

            HBufC8* buffer = HBufC8::NewLC(len);

            TPtr8 ptr(buffer->Des());
            aMessage.ReadL(0, ptr);
            RDesReadStream stream;
            stream.Open(ptr);
            CleanupClosePushL(stream);

            RPointerArray<CCmDriveInfo> drives;
            CleanupClosePushL( drives );

            for ( TInt i = 0; i < driveCount; i++ )
                {
                CCmDriveInfo* drive =
                    CCmDriveInfo::NewLC();
                drive->InternalizeL( stream );
                drives.AppendL( drive );
                }
            // Has drive slots changed or has some mmc taken away
            TBool driveSelected( EFalse );
            for( TInt j = 0; j < origDrives.Count(); j++ )
                {
                for( TInt k = 0; k < drives.Count(); k++ )
                    {
                    if( origDrives[j]->DriveId() == drives[k]->DriveId() )
                        {
                        driveSelected = ETrue;
                        if( origDrives[j]->DriveNumber() !=
                            drives[k]->DriveNumber() )
                            {
                            // update fill list drive numbers and ids
                            iDbManager.UpdateFillListDriveStatusesL(
                                origDrives[j], drives[k], driveSelected );
                            }
                        else
                            {
                            // Do nothing, drive still in same slot as
                            // previously
                            }
                        }
                    }
                if( !driveSelected )
                    {
                    // Drive not selected, update fill file list statuses
                    // Use drive id in update clause
                     iDbManager.UpdateFillListDriveStatusesL(
                        origDrives[j], NULL, driveSelected );
                    }
                driveSelected = EFalse;
                }

            TRAP( ret, iDbManager.SetDrivesL( drives ) );
            TRACE(Print(_L("[Cm Server]\t drives update ret = %d"),
                ret));
            if( drives.Count() )
                {
                Server().SetDriveSelectionStatus( ETrue );
                }
            else
                {
                Server().SetDriveSelectionStatus( EFalse );
                }
            aMessage.Complete( ret );

            CleanupStack::PopAndDestroy( driveCount );
            drives.Reset();
            CleanupStack::PopAndDestroy( &drives );
            CleanupStack::PopAndDestroy( 2, buffer );
            origDrives.ResetAndDestroy();
            CleanupStack::PopAndDestroy( &origDrives );
            // Restart quota listener if needed
            TBool mmEnabled( EFalse );
            iSettings.GetMemoryManagerStatus( mmEnabled );
            Server().SetServiceStateL( ECmServiceMemoryManager,
                (TCmServiceState)mmEnabled );
            break;
            }
        case ECmGetDrivesSize:
            {
            LOG(_L("[Cm Server]\t ECmGetDrivesSize request"));

            iDbManager.PrepareQueryCmdL( EDrivesQuery );

            delete iDataBuffer;
            iDataBuffer = NULL;

            iDataBuffer = CBufFlat::NewL( KBufferExpandSize );
            RBufWriteStream driveStream;
            driveStream.Open( *iDataBuffer );
            CleanupClosePushL( driveStream );

            RPointerArray<CCmDriveInfo> drives;
            CleanupClosePushL( drives );
            iDbManager.QueryDrivesL( drives );
            for( TInt i = 0; i < drives.Count(); i++ )
                {
                drives[i]->ExternalizeL( driveStream );
                }

            TInt size = iDataBuffer->Size();
            TPckg<TInt> pckg( size );
            aMessage.WriteL(0, pckg );

            TPckg<TInt> pckg2( drives.Count() );
            aMessage.WriteL(1, pckg2 );

            TRACE(Print(_L("[Cm Server]\t buffer size is %d, amount %d"),
                size, drives.Count() ));

            aMessage.Complete( KErrNone );
            drives.ResetAndDestroy();
            CleanupStack::PopAndDestroy( &drives );
            CleanupStack::PopAndDestroy( &driveStream );
            break;
            }
        case ECmApplicationExit:
            {
            LOG(_L("[Cm Server]\t ECmApplicationExit request"));
            Server().SetAsyncRequestStateL( EFalse );
            if ( !aMessage.IsNull() )
                {
                aMessage.Complete( KErrNone );
                }
            break;
            }
        case ECmGetFilledFilesSize:
            {
            LOG(_L("[Cm Server]\t ECmGetFilledFilesSize request"));
            TInt id = aMessage.Int1();
            TInt64 size = iDbManager.BytesFilled( id );
            TPckg<TInt64> pckg( size );

            aMessage.WriteL(0, pckg );
            aMessage.Complete( KErrNone );

            break;
            }
        default:
            {
            LOG(_L("[Cm Server]\t default panic with ECmClientBadRequest"));
            // Unknown opcode --> panic client
            PanicClient( aMessage, ECmClientBadRequest );
            break;
            }
        }

    // If ServiceL leaves, default implementation in server framework
    // completes the RMessage2 with the leave code.
    }

// ---------------------------------------------------------------------------
// PanicClient
// Client panic handler
// RMessage2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCmSession::PanicClient( const RMessage2& aMessage, TInt aPanic ) const
    {
    TRACE(Print(_L("[Cm Server]\t PanicClient: Reason = %d\n"), aPanic));
    aMessage.Panic( KCmServerPanic, aPanic );
    }    

// End of File
