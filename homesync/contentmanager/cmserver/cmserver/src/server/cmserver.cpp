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
* Description:  Server implementation
*
*/


#include    <e32property.h>
#include    "upnpavdevice.h"
#include    "upnpavcontroller.h"
#include    "upnpavcontrollerfactory.h"

#include    "cmdmmain.h"
#include    "cmfillmanagerfactory.h"
#include    "cmfillmanager.h"
#include    "cmstoremanagerfactory.h"
#include    "cmstoremanager.h"
#include    "cmscheduler.h"
#include    "cmmmmain.h"
#include    "cmfilllistitem.h"
#include    "cmdriveinfo.h"
#include    "cmmediaserverfull.h"
#include    "cmfillrulecontainer.h"
#include    "cmfillrule.h"
#include    "cmsqlpropertycontainer.h"
#include    "cmsettingsfactory.h"
#include	"mmdhclient.h"
#include	"mdhclientfactory.h"
#include    "cmsettings.h"
#include    "cmserver.h"
#include    "cmserversession.h"
#include    "msdebug.h"

// CONSTANTS
const TInt KDefaultSystemUpdateId = 0;  // default system update id
const TInt KArrayGranularity = 16;      // array granularity
const TInt KDriveBufferSize = 4;        // drive letter buffer size
const TInt KDeleteDataBaseItem = 255;   // delete status for FillListId
_LIT( KColon, ":");                     // colon
_LIT( KDoubleBackSlash, "\\");          // double backslash

//Total number of ranges
const TUint KCmServerRangeCount = 2;

//Definition of the ranges of IPC numbers
const TInt KCmServerRanges[KCmServerRangeCount] =
        {
        0,
        34
        };

//Policy to implement for each of the above ranges
const TUint8 KCmServerElementsIndex[KCmServerRangeCount] =
        {
        1, //applies to 1st range
        CPolicyServer::ENotSupported
        };

//Specific capability checks
const CPolicyServer::TPolicyElement KCmServerElements[] =
        {
        {_INIT_SECURITY_POLICY_C3(ECapabilityNetworkServices,
            ECapabilityReadUserData, ECapabilityWriteUserData ),
            CPolicyServer::EFailClient },
        {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices),
            CPolicyServer::EFailClient}
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy KCmServerPolicy =
        {
        //specifies all connect attempts should pass
        CPolicyServer::EAlwaysPass,
        KCmServerRangeCount,
        KCmServerRanges,
        KCmServerElementsIndex,
        KCmServerElements
        };


// ---------------------------------------------------------------------------
// CCmServer::CCmServer
// ---------------------------------------------------------------------------
//
CCmServer::CCmServer()
    : CPolicyServer( CActive::EPriorityHigh, KCmServerPolicy )
    {
    LOG(_L("[Cm Server]\t CCmServer::CCmServer"));
    }

// ---------------------------------------------------------------------------
// CCmServer::ConstructL
// ---------------------------------------------------------------------------
//
void CCmServer::ConstructL()
    {
    LOG(_L("[Cm Server]\t CCmServer::ConstructL"));

    iServerState = ECmServerStateIdle;
    iSessionCount = (TInt*) User::AllocL( sizeof( TInt ) );
    *iSessionCount = 0;

    LOG(_L("[Cm Server]\t Creating settings.."));
    iSettings = CCmSettingsFactory::NewCmSettingsEngineL();

    TCmServiceState cmState( ECmServiceStateDisabled );
    iSettings->GetServiceState( ECmServiceContentManager, cmState );

    TRACE(Print(_L("[Cm Server]\t Cm state is %d\n"), cmState));

    if ( cmState )
        {
        LOG(_L("[Cm Server]\t Creating scheduler.."));
        iScheduler = CCmScheduler::NewL( *this, *iSettings );
        if ( cmState == ECmServiceStateAutomatic )
            {
            iScheduler->SetServiceToExecute( ECmServicePreProcessingStore );
            }
        }

    LOG(_L("[Cm Server]\t Creating db manager"));
    iDbManager = CCmDmMain::NewL();
    LOG(_L("[Cm Server]\t Creating Mde session"));
    iSession = CMdESession::NewL(*this);
    
    RPointerArray<CCmDriveInfo> drives;
    CleanupClosePushL( drives );
    iDbManager->PrepareQueryCmdL( EDrivesQuery );
    iDbManager->QueryDrivesL( drives );
    TRACE(Print(_L("[Cm Server]\t Drives count = %d"),
        drives.Count() ));
    if( drives.Count() )
        {
        SetDriveSelectionStatus( ETrue );
        }
    drives.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &drives );
    
    LOG(_L("[Cm Server]\t Creating memory manager"));
    TBool mmEnabled( EFalse );
    iSettings->GetMemoryManagerStatus( mmEnabled );
    LOG(_L("[Cm Server]\t Creating memory manager"));
    iMemoryManager = CCmMmMain::NewL( *iDbManager );
    iMemoryManager->SetQuotaListenerStateL( mmEnabled );

    LOG(_L("[Cm Server]\t Creating store manager"));
    iStoreManager = CmStoreManagerFactory::NewStoreManagerL( this, *iSession,
        iDbManager, iMemoryManager);

    LOG(_L("[Cm Server]\t Creating fill manager"));
    iFillManager = CmFillManagerFactory::NewFillManagerL(
        this, *iSession, iDbManager, iMemoryManager);

    LOG(_L("[Cm Server]\t Starting server..."));

    StartL( KCmServerName );

    LOG(_L("[Cm Server]\t CCmServer::ConstructL end"));
    }

// ---------------------------------------------------------------------------
// CCmServer::NewLC
// ---------------------------------------------------------------------------
//
CCmServer* CCmServer::NewLC()
    {
    LOG(_L("[Cm Server]\t CCmServer::NewLC"));
    CCmServer *self = new (ELeave) CCmServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCmServer::~CCmServer
// ---------------------------------------------------------------------------
//
CCmServer::~CCmServer()
    {
    LOG(_L("[Cm Server]\t CCmServer::~CCmServer"));

    delete iDeviceDiscoveryTimer;
    delete iFillRuleContainer;

    if ( iFillManager )
        {
        iFillManager->Close();
        }
    if ( iStoreManager )
        {
        iStoreManager->Close();
        }
    if ( iAvController )
        {
        iAvController->RemoveDeviceObserver();
		iAvController->Release();
		iAvController = NULL;
        }

    iMdhClient->Close();
    delete iPropertyContainer;
    delete iDbManager;
    delete iMemoryManager;
    delete iSessionCount;
    iSettings->Close();
    delete iSession;
    delete iScheduler;
    }


// ---------------------------------------------------------------------------
// CCmServer::NewSessionL
// ---------------------------------------------------------------------------
//
CSession2* CCmServer::NewSessionL( const TVersion& aVersion,
                                   const RMessage2 &/*aMessage*/) const
    {
    LOG(_L("[Cm Server]\t CCmServer::NewSessionL"));
    // check we're the right version
    TVersion version( KCmServerMajor,
                      KCmServerMinor,
                      KCmServerBuild);

    if( !User::QueryVersionSupported( version, aVersion ) )
        {
        LOG(_L("[Cm Server]\t CCmServer::NewSessionL leave with \
        KErrNotSupported"));
        User::Leave( KErrNotSupported );
        }
    ++(*iSessionCount);
    // make new session
    return CCmSession::NewL( *iDbManager, *iSettings );
    }

// ---------------------------------------------------------------------------
// CCmServer::CreateAvcAndTimerL
// ---------------------------------------------------------------------------
//
void CCmServer::CreateAvcAndTimerL()
    {
    LOG(_L("[Cm Server]\t CCmServer::CreateAvcAndTimerL"));
    
    iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL();
    iAvController->SetDeviceObserver( *this );
    
    LOG( _L("[Cm Server]\t Creating CPeriodic timer..." ) );
    
    iDeviceDiscoveryTimer = CPeriodic::NewL(EPriorityHigh);
    TInt deviceDiscoveryDelay( 0 );
    iSettings->GetDiscoveryDelay( deviceDiscoveryDelay );
    
    TRACE(Print(_L("[Cm Server]\t using device discovery delay %d\n"),
        deviceDiscoveryDelay));
    
    iDeviceDiscoveryTimer->Start(deviceDiscoveryDelay * KMega,
                  0,
                  TCallBack(SearchTimerCompletedL,
                  this));
    
    LOG( _L("[Cm Server]\t Updated!" ) );
    
    iMediaServers.ResetAndDestroy();
    
    LOG( _L("[Cm Server]\t Loading media servers from db..." ) );
    
    iDbManager->GetMediaServersL( iMediaServers );
    }

// ---------------------------------------------------------------------------
// CCmServer::LeaveIfNotReadyL
// ---------------------------------------------------------------------------
//
void CCmServer::LeaveIfNotReadyL()
    {
    LOG(_L("[Cm Server]\t CCmServer::LeaveIfNotReadyL"));
    
    if ( ServerState() != ECmServerStateIdle )
        {
        if ( !ServerState() ) // CM disabled
            {
            LOG(_L("[Cm Server]\t CCmServer::LeaveIfNotReadyL \
            leave with KErrNotReady"));
            User::Leave( KErrNotReady );
            }
        // CM busy
        LOG(_L("[Cm Server]\t CCmServer::LeaveIfNotReadyL \
        leave with KErrServerBusy"));
        User::Leave( KErrServerBusy );
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::DeleteUnselectedFillFilesL
// ---------------------------------------------------------------------------
//
void CCmServer::DeleteUnselectedFillFilesL()
    {
    LOG(_L("[Cm Server]\t CCmServer::DeleteUnselectedFillFilesL"));
    
    for ( ; iFillRuleIndex < iFillRuleContainer->FillRuleCount();
            iFillRuleIndex++ )
        {
        CCmFillRule* rule =
            iFillRuleContainer->FillRule( iFillRuleIndex );

        TCmFillRuleStatus status = rule->Selected();
        
        if ( status == ECmUnSelected || status == ECmDeleted )
        	{
        	iDeletingDeletedItems = ETrue;
        	DeleteFilledFilesL();
        	iFillRuleIndex = iFillRuleContainer->FillRuleCount();
        	}
        }
    
    if ( !iDeletingDeletedItems )
    	{
    	// no unselected lists - complete delete request
    	iDeletingDeletedItems = EFalse;
    	ServiceExecuted( ECmServiceDeleteFilledFiles, KErrNone );
    	}
    }

// ---------------------------------------------------------------------------
// CCmServer::DecrementSessions
// ---------------------------------------------------------------------------
//
void CCmServer::DecrementSessions()
    {
    LOG(_L("[Cm Server]\t CCmServer::DecrementSessions"));

    --( *iSessionCount );
    
    if ( (*iSessionCount) <= 0 )
        {
        TCmServiceState cmState;
        iSettings->GetServiceState( ECmServiceContentManager, cmState );
        TBool mmEnabled;
        iSettings->GetMemoryManagerStatus( mmEnabled );

        if ( !cmState && !mmEnabled &&
             ServerState() == ECmServerStateIdle )
             // cm & mm disabled and server idle
            {
            // shutdown server
            LOG(_L("[Cm Server]\t Server & mm disabled and \
                no sessions left, shutting down.."));
            CActiveScheduler::Stop();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::ServerState
// ---------------------------------------------------------------------------
//
TCmServerState CCmServer::ServerState() const
    {
    TRACE( Print( _L("[Cm Server]\t CCmServer::ServerState \
    iServerState = %d"), iServerState ));
    return iServerState;
    }

// ---------------------------------------------------------------------------
// CCmServer::PropertyContainer
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer* CCmServer::PropertyContainer() const
    {
    LOG(_L("[Cm Server]\t CCmServer::PropertyContainer"));
    return iPropertyContainer;
    }

// ---------------------------------------------------------------------------
// CCmServer::PropertyCollector
// ---------------------------------------------------------------------------
//
CCmSqlPropertyCollector* CCmServer::PropertyCollector() const
    {
    LOG(_L("[Cm Server]\t CCmServer::PropertyCollector"));
    return iPropertyCollector;
    }

// ---------------------------------------------------------------------------
// CCmServer::MediaServerArray
// ---------------------------------------------------------------------------
//
RPointerArray<CCmMediaServerFull>& CCmServer::MediaServerArray()
    {
    LOG(_L("[Cm Server]\t CCmServer::MediaServerArray"));
    
    return iMediaServers;
    }

// ---------------------------------------------------------------------------
// CCmServer::UpdateFillPriorities
// ---------------------------------------------------------------------------
//
void CCmServer::UpdateFillPriorities()
    {
    LOG(_L("[Cm Server]\t CCmServer::UpdateFillPriorities"));
    iFillManager->UpdatePriorities();
    }

// ---------------------------------------------------------------------------
// CCmServer::DeletePropertyContainer
// ---------------------------------------------------------------------------
//
void CCmServer::DeletePropertyContainer()
    {
    LOG(_L("[Cm Server]\t CCmServer::DeletePropertyContainer"));
    
    if ( iPropertyContainer )
        {
        delete iPropertyContainer;
        iPropertyContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::DeleteItemsByFillListId
// ---------------------------------------------------------------------------
//
void CCmServer::DeleteItemsByFillListId( TInt aFillListId )
	{
    LOG(_L("[Cm Server]\t CCmServer::DeleteItemsByFillListId"));
    
    TInt error = iDbManager->DeleteItemsByFillListId( aFillListId );
            
    if ( error )
       	{
       	TRACE( Print( _L("[Cm Server]\t CCmServer::DeleteItemsByFillListId \
       	error = %d\n"), error ));
       	}
	}

// ---------------------------------------------------------------------------
// CCmServer::FillRuleContainerL
// ---------------------------------------------------------------------------
//
CCmFillRuleContainer* CCmServer::FillRuleContainerL() const
    {
    LOG(_L("[Cm Server]\t CCmServer::FillRuleContainerL"));

    RPointerArray<HBufC8> array;
    CleanupClosePushL( array );
    iDbManager->PrepareQueryCmdL( EFillRuleNamesQuery );
    iDbManager->QueryFillRuleNamesL( array );
    TRACE(Print(_L("[Cm Server]\t fill rule array size is %d"),
        array.Count()));

    CCmFillRuleContainer* ruleContainer = CCmFillRuleContainer::NewLC();
    for ( TInt i = 0; i < array.Count(); i++ )
        {
        CCmFillRule* rule = CCmFillRule::NewLC();
        rule->SetNameL( *(array[i]) );
        iDbManager->PrepareQueryCmdL( EFillRuleQuery );
        iDbManager->QueryFillRuleL( rule );

        ruleContainer->AddFillRuleL( rule );
        CleanupStack::Pop( rule );
        }
    CleanupStack::Pop( ruleContainer );
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    
    return ruleContainer;
    }

// ---------------------------------------------------------------------------
// CCmServer::ExecuteStoreL
// ---------------------------------------------------------------------------
//
void CCmServer::ExecuteStoreL()
    {
    LOG(_L("[Cm Server]\t CCmServer::ExecuteStoreL"));
    TInt err = KErrNone;
    iServerState = ECmServerStateStoring;
    if ( !iAvController ) // need to create avc and wait for devices
        {
        TRAP( err, CreateAvcAndTimerL() );
        TRACE(Print(_L("[Cm Server]\t CreateAvcAndTimerL err = %d"), err ));
        }
    else // avc already created, start store
        {
        iStoreManager->SetAvController( iAvController );
        TRAP( err, iStoreManager->StartStoreL() );
        }
    if ( err )
        {
        TRACE(Print(_L("[Cm Server]\t Error occurred = %d"), err ));
        ServiceExecuted( ECmServiceStore, err );
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::ExecuteFillL
// ---------------------------------------------------------------------------
//
void CCmServer::ExecuteFillL()
    {
    LOG(_L("[Cm Server]\t CCmServer::ExecuteFillL"));
    if( !iDriveSelectionStatus )
        {
        LOG(_L("[Cm Server]\t No drives selected. Do not start fill!!!"));
        iServerState = ECmServerStateFilling;
        ServiceExecuted( ECmServiceFill, KErrNone );
        }
    else
        {
        TInt err = KErrNone;
        if ( !iAvController ) // need to create avc and wait for devices
            {
            TRAP( err, CreateAvcAndTimerL() );
            }
        else // avc already created, start fill
            {
            iSettings->GetMemoryManagerStatus( iMmState );
            TRACE(Print(_L("[Cm Server]\t Mm status = %d"), (TInt)iMmState ));
            LOG(_L("[Cm Server]\t Disabling quota listener..."));
            iMemoryManager->SetQuotaListenerStateL( EFalse );
            iAvController->RemoveDeviceObserver();
            iFillManager->SetAvController( iAvController );
            TRAP( err, iFillManager->StartFillL() );            
            }
        iServerState = ECmServerStateFilling;
        if ( err )
            {
            ServiceExecuted( ECmServiceFill, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::SetCurrentAsyncMsg
// ---------------------------------------------------------------------------
//
void CCmServer::SetCurrentAsyncMsg( const RMessage2& aMessage )
    {
    LOG(_L("[Cm Server]\t CCmServer::SetCurrentAsyncMsg"));
    if ( !iAsyncRequestOn )
        {
        iCurrentAsyncMsg = aMessage;
        iAsyncRequestOn = ETrue;
        }

    }

// ---------------------------------------------------------------------------
// CCmServer::SearchTimerCompletedL
// ---------------------------------------------------------------------------
//
TInt CCmServer::SearchTimerCompletedL(TAny* aInstance)
    {
    LOG(_L("[Cm Server]\t CCmServer::SearchTimerCompleted"));
    return static_cast<CCmServer*>(aInstance)->DoSearchTimerCompletedL();
   }

// ---------------------------------------------------------------------------
// CCmServer::DoSearchTimerCompletedL
// ---------------------------------------------------------------------------
//
TInt CCmServer::DoSearchTimerCompletedL()
    {
    LOG(_L("[Cm Server]\t CCmServer::DoSearchTimerCompleted"));
    delete iDeviceDiscoveryTimer;
    iDeviceDiscoveryTimer = NULL;

    switch ( ServerState() )
        {
        case ECmServerStateFilling:
            {
            ExecuteFillL();
            break;
            }

        case ECmServerStateStoring:
            {
            ExecuteStoreL();
            break;
            }

        case ECmServerStateSearchingForServers:
            {
            ServiceExecuted( ECmServiceSearchForServers, KErrNone );
            break;
            }
        default:
            {
            LOG(_L("[Cm Server]\t CCmServer::DoSearchTimerCompletedL \
            leave with KErrUnknown"));
            User::Leave( KErrUnknown );
            break;
            }
        }
    return KErrNone;
   }

// ---------------------------------------------------------------------------
// CCmServer::SetServiceStateL
// ---------------------------------------------------------------------------
//
void CCmServer::SetServiceStateL( TCmService aService,
                                  TCmServiceState aState )
    {
    TRACE( Print( _L("[Cm Server]\t CCmServer::SetServiceStateL \
    state = %d"), aState ));

    switch ( aService )
        {
        case ECmServiceFill:  // fall through
        case ECmServiceStore: // fall through
        case ECmServiceContentManager:
        // sync setting
            {
            switch ( aState )
                {
                case ECmServiceStateDisabled:
                    {
                    iSettings->SetAutoSync( EFalse );
                    // disable scheduler
                    if ( iScheduler )
                        {
                        LOG(_L("[Cm Server]\t CCmServer::\
                        SetServiceStateL delete scheduler"));
                        delete iScheduler;
                        iScheduler = NULL;
                        }
                    break;
                    }

                case ECmServiceStateAutomatic:
                    {
                    // enable scheduler
                    if ( !iScheduler )
                        {
                        iScheduler = CCmScheduler::NewL( *this, *iSettings );
                        LOG(_L("[Cm Server]\t CCmServer::\
                        SetServiceStateL create scheduler"));

                        }
                    iScheduler->SetServiceToExecute(
                        ECmServicePreProcessingStore );
                    iSettings->SetAutoSync( ETrue );
                    break;
                    }
                default:
                    {
                    LOG(_L("[Cm Server]\t CCmServer::SetServiceStateL \
                    leave with KErrArgument"));
                    User::Leave( KErrArgument );
                    }
                }
            iSettings->SetServiceState( ECmServiceContentManager,
                                        aState );
            break;
            }
        case ECmServiceMemoryManager:
            {
            TBool enable = (TBool) aState;
            iMemoryManager->SetQuotaListenerStateL( enable );
            iSettings->SetMemoryManagerStatus( enable );
            break;
            }
        default:
            {
            LOG(_L("[Cm Server]\t CCmServer::SetServiceStateL \
            leave with KErrArgument"));
            User::Leave( KErrArgument );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::ExecuteServiceL
// ---------------------------------------------------------------------------
//
void CCmServer::ExecuteServiceL( TCmService aService )
    {
    LOG(_L("[Cm Server]\t CCmServer::ExecuteServiceL"));
    
    ExecuteServiceL( aService, 0, NULL );
    }

// ---------------------------------------------------------------------------
// CCmServer::ExecuteServiceL
// ---------------------------------------------------------------------------
//
void CCmServer::ExecuteServiceL( TCmService aService,
                                 TInt aOptParam1,
                                 TAny* aOptParam2 )
    {
    LOG(_L("[Cm Server]\t CCmServer::ExecuteServiceL"));
    LeaveIfNotReadyL();

#ifdef _DEBUG

    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[Cm Server]\t heap allocsize %d, size %d, cells %d"),
        alloc, size, cells ));

#endif



    switch ( aService )
        {
        case ECmServiceHarvest:
            {
            LOG(_L("[Cm Server]\t ECmServiceHarvest"));

            if ( iMdhClient )
                {
                iMdhClient->Close();
                iMdhClient = NULL;
                }

            iMdhClient = CCmMdhClientFactory::NewCmMdhClientL( *this );
            User::LeaveIfError( iMdhClient->Harvest() );
            iServerState = ECmServerStateHarvesting;
            break;
            }
        case ECmServiceFill:
            {
            LOG(_L("[Cm Server]\t ECmServiceFill"));
            ExecuteFillL();
            break;
            }
        case ECmServiceStore:
            {
            LOG(_L("[Cm Server]\t ECmServiceStore"));

            // just executes deletefilledfiles and
            // sets a flag to start store after that
            iExecuteStoreAfterDelete = ETrue;
            ExecuteServiceL( ECmServiceDeleteFilledFiles );
            break;
            }
        case ECmServicePreProcessingStore:
            {
            LOG(_L("[Cm Server]\t ECmServicePreProcessingStore"));
            iStoreManager->PreProcessLists();
            iServerState = ECmServerStatePreProcessingStore;
            break;
            }

        case ECmServicePreProcessingFill:
            {
            LOG(_L("[Cm Server]\t ECmServicePreProcessingFill"));
            iFillManager->PreProcessListsL();
            iServerState = ECmServerStatePreProcessingFill;
            break;
            }

        case ECmServiceGetStoreFields:
            {
            LOG(_L("[Cm Server]\t ECmServiceGetStoreFields"));

            iServerState = ECmServerStateGettingStoreFields;

            TCmMediaType mediaType =
                static_cast<TCmMediaType> ( aOptParam1 );

            DeletePropertyContainer();
            iPropertyContainer = CCmSqlPropertyContainer::NewL();
            iStoreManager->GetColItemsL( mediaType, *iPropertyContainer );

            break;
            }

        case ECmServiceGetFillFields:
            {
            LOG(_L("[Cm Server]\t ECmServiceGetFillFields"));

            iServerState = ECmServerStateGettingFillFields;

            iPropertyCollector =
                    static_cast<CCmSqlPropertyCollector*> (aOptParam2);

            iFillManager->GetMetadataItemsL( *iPropertyCollector );

            break;
            }
        case ECmServiceSearchForServers:
            {
            LOG(_L("[Cm Server]\t ECmServiceSearchForServers"));

            if ( !iAvController )
                {
                CreateAvcAndTimerL();
                }
            iServerState = ECmServerStateSearchingForServers;
            break;
            }

        case ECmServiceDeleteFilledFiles:
            {
            LOG(_L("[Cm Server]\t ECmServiceDeleteFilledFiles"));

            iServerState = ECmServerStateDeleting;
            iFillRuleContainer = FillRuleContainerL();
            iFillRuleIndex = 0;

            DeleteUnselectedFillFilesL();
            DeleteItemsByFillListId( KDeleteDataBaseItem );
            // this request is completed in DeleteComplete or
            // DeleteUnselectedFillFilesL
            break;
            }

        case ECmServicePreProcessingFilllist:
            {
            LOG(_L("[Cm Server]\t ECmServicePreProcessingFilllist"));

            HBufC8* listName = static_cast<HBufC8*> ( aOptParam2 );
            iFillManager->PreProcessListL( *listName );
            iServerState = ECmServerStatePreProcessingFilllist;
            delete listName;
            break;
            }
        case ECmServiceUpdateFillLists:
            {
            LOG(_L("[Cm Server]\t ECmServiceUpdateFillLists"));
            UpdateFillPriorities();
            iServerState = ECmServerStateUpdatingFillLists;
            break;
            }
        case ECmServiceDeleteMetadata:
            {                   
            LOG(_L("[Cm Server]\t ECmServiceDeleteMetadata"));
            iFillManager->DeleteMetadataL();
            iServerState = ECmServerStateDeletingMetadata;
            break;
            }
        default:

            {
            LOG(_L("[Cm Server]\t CCmServer::ExecuteServiceL \
            leave with KErrArgument"));
            User::Leave( KErrArgument );
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CCmServer::CancelServiceL
// ---------------------------------------------------------------------------
//
void CCmServer::CancelServiceL()
    {
    LOG(_L("[Cm Server]\t CCmServer::CancelServiceL"));
    switch ( ServerState() )
        {
        case ECmServerStateHarvesting:
            {
            iMdhClient->Stop();
            break;
            }

        case ECmServerStateFilling:               // fall through
        case ECmServerStatePreProcessingFill:     // fall through
        case ECmServerStatePreProcessingFilllist: // fall through
        case ECmServerStateGettingFillFields:     // fall through
        case ECmServerStateUpdatingFillLists:     // fall through
        case ECmServerStateDeletingMetadata:
            {
            if ( iDeviceDiscoveryTimer ) // only possible with fill
                {
                ServiceExecuted( ECmServiceFill, KErrCancel );
                }
            else
                {
                iFillManager->CancelOperation();
                }
            break;
            }

        case ECmServerStateStoring:
        case ECmServerStatePreProcessingStore:
        case ECmServerStateGettingStoreFields:

            {
            if ( iDeviceDiscoveryTimer ) // only possible with store
                {
                ServiceExecuted( ECmServiceStore, KErrCancel );
                }
            else
                {
                iStoreManager->CancelOperation();
                }
            break;
            }

        case ECmServerStateSearchingForServers:
            {
            ServiceExecuted( ECmServiceSearchForServers, KErrCancel );
            break;
            }

        case ECmServerStateDeleting:
            {
            iMemoryManager->Cancel();
            if ( iFillRuleContainer )
                {
                delete iFillRuleContainer;
                iFillRuleContainer = NULL;
                }
            ServiceExecuted( ECmServiceDeleteFilledFiles, KErrCancel );
            break;
            }

        case ECmServerStateIdle:
            {
            break;
            }

        default:
            {
            LOG(_L("[Cm Server]\t CCmServer::CancelServiceL \
            leave with KErrNotReady"));
            User::Leave( KErrNotReady );
            break;
            }
        }

    LOG(_L("[Cm Server]\t CCmServer::CancelServiceL end"));
    }

// ---------------------------------------------------------------------------
// CCmServer::DeleteFilledFilesL
// ---------------------------------------------------------------------------
//
void CCmServer::DeleteFilledFilesL()
    {
    LOG(_L("[Cm Server]\t CCmServer::DeleteFilledFilesL"));
    iDbManager->PrepareQueryCmdL(EAllFillFilesStatusQuery);
    CDesCArrayFlat* filesToDelete =
                new (ELeave) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( filesToDelete );

    TInt ret( KErrNone );
    TFileName file;
    RPointerArray<CCmDriveInfo> drives;
    CleanupClosePushL( drives );
    iDbManager->PrepareQueryCmdL( EDrivesQuery );
    TRAP( ret, iDbManager->QueryDrivesL( drives ) );
    TRACE(Print(_L("[Cm Server]\t QueryDrivesL error = %d \n"),
        ret ));
    
    RPointerArray<CCmFillListItem> items;
    CleanupClosePushL( items );
    iDbManager->PrepareQueryCmdL( EAllFillFilesStatusQuery );
    iDbManager->GetAllFillFilesL( items,
        ECmToBeShrinked | ECmToBeFilled | ECmFilled | ECmLocalCopy );

    TRACE(Print(_L("[Cm Server]\t %d total count of list items \n"),
        items.Count()));
    
    RFs fs;
    ret = fs.Connect();
    TRACE(Print(_L("[Cm Server]\t File session connect error = %d \n"),
        ret ));
    
    TLinearOrder<CCmFillListItem> key( CompareById );
    items.Sort( key );
    TInt itemcount = items.Count();
    CCmFillListItem* pi = NULL;
    CCmFillListItem* pj = NULL;
    for ( TInt i = 0; i < itemcount; i++ )
        {
        CleanupStack::PushL( items[i] );
        pi = items[i];
        // if file is on some other list, do not delete
        if( ECmToBeFilled == pi->Status() ||
            ECmToBeShrinked == pi->Status() || 
            ECmLocalCopy == pi->Status() )
            {
            // Do not handle this. Already removed.
            }
        else
            {
            TCmListItemStatus sta( ECmToBeFilled );
            // gets list status where item belogs to
            TInt err = GetStatusByGroupId( pi->ListId(), sta );
            TRACE( Print( _L("[Cm Server]\t GetStatusByGroupId ret = %d\n"),
            err ));
            if( pi->Selected() == ECmUnSelected ||
                pi->Selected() == ECmDeleted )
                {
                //file of unselected, delete
                if( pi->RefId() == 0 )
                    {
                    file.Append( *ResolveAndAppendRootPathL( fs,
                                  pi->DriveNumber() ));
                    file.Append( pi->Path() );
                    TRAPD( error, filesToDelete->InsertIsqL( file ) );
                    if ( error )
                        {
                        TRACE( Print( _L("[Cm Server]\t GetStatusByGroupId \
                        error = %d\n"), error ));
                        }
                    UpdateDriveQuota( drives, *pi );
                    file.Zero();
                    }
                //duplicated file, just set status
                if( ECmUnSelected == pi->Selected() )
                    {
                    pi->SetStatus( sta );
                    }
                }
            else
                {
                 //duplicated file, just set status
                if( pi->RefId() != 0 )
                    {
                    pi->SetStatus( sta );
                    }
                else
                    {
                    TInt j( 0 );
                    for ( j = i + 1; j < itemcount; j++ )
                        {
                        pj = items[j];
                        if( items[i]->DbId() != items[j]->DbId() )
                            {
                            break;
                            }
                        else if ( ECmSelected == pj->Selected() &&
                                  pj->Priority() < pi->Priority() )
                            {
                            file.Append( *ResolveAndAppendRootPathL( fs,
                                          pi->DriveNumber() ) );
                            file.Append( pi->Path() );
                            filesToDelete->InsertIsqL( file );
                            UpdateDriveQuota( drives, *pi );
                            file.Zero();
                            items[i]->SetStatus( sta );
                            break;
                            }
                        }
                    for ( j = i - 1; j >= 0; j-- )
                        {
                        pj = items[j];
                        if( items[i]->DbId() != items[j]->DbId() )
                            {
                            break;
                            }
                        else if ( ECmSelected == pj->Selected() &&
                                  pj->Priority() < pi->Priority() )
                            {
                            file.Append( *ResolveAndAppendRootPathL( fs,
                                          pi->DriveNumber() ) );
                            file.Append( pi->Path() );
                            filesToDelete->InsertIsqL( file );
                            UpdateDriveQuota( drives, *pi );
                            file.Zero();
                            items[i]->SetStatus( sta );
                            break;
                            }
                        }
                    }
                }
            }
        }
    
    filesToDelete->Compress();
    TRACE(Print(_L("[Cm Server]\t %d files to delete\n"),
        filesToDelete->Count()));
    iMemoryManager->SetObserver( *this );
    // delete files asynchronously
    // DeleteCompleteL callback is called when ready
    iMemoryManager->DeleteFilesL( *filesToDelete );
    
    //refresh the fill items
    iDbManager->DeleteFillFiles();
    iDbManager->SetFillFiles( items );
	CleanupStack::Pop( items.Count() );
    items.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &items );

    TRAP( ret, iDbManager->SetDrivesL( drives ) );
    drives.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &drives );
    
    TRACE(Print(_L("[Cm Server]\t Poping filesToDelete \n")));
    CleanupStack::PopAndDestroy( filesToDelete );

    //updates RefIds
    iFillManager->UpdateRefIds();
    iDbManager->DeleteDeletedItemsL();   
    }

// ---------------------------------------------------------------------------
// CCmServer::CreateFileDeleteSQLL
// ---------------------------------------------------------------------------
void CCmServer::CreateFileDeleteSQLL( RPointerArray<CCmFillListItem>& aItems,
                                      RPointerArray<CCmDriveInfo>& aDrives,
                                      CDesCArrayFlat* aFilesToDelete )
    {
    LOG(_L("[Cm Server]\t CCmServer::CCmServer::CreateFileDeleteSQLL"));
    
    TFileName file;
    
    TInt ret( KErrNone );
    RFs fs;
    ret = fs.Connect();   
    TRACE(Print(_L("[Cm Server]\t File session connect error = %d \n"),
        ret ));    

    if ( ret == KErrNone )
        {
        for ( TInt i = 0; i < aItems.Count(); i++ )
            {                        
            // if file is on some other list, do not delete
            if( ECmToBeFilled == aItems[i]->Status() ||
                ECmToBeShrinked == aItems[i]->Status() || 
                ECmLocalCopy == aItems[i]->Status() )
                {
                // Do not handle this. Already removed.
                }
            else
                {
                TCmListItemStatus sta( ECmToBeFilled );
                TInt err = GetStatusByGroupId( aItems[i]->ListId(), sta );
                TRACE( Print( _L("[Cm Server]\t GetStatusByGroupId \
                ret = %d\n"),
                err ));
                if( aItems[i]->Selected() == ECmUnSelected ||
                    aItems[i]->Selected() == ECmDeleted )
                    {
                    //file of unselected, delete
                    if( aItems[i]->RefId() == 0 )
                        {
                        file.Append( *ResolveAndAppendRootPathL( fs,
                            aItems[i]->DriveNumber() ));
                        file.Append( aItems[i]->Path() );
                        TRAPD( error, aFilesToDelete->InsertIsqL( file ) );
                        if ( error )
                            {
                            TRACE( Print( _L("[Cm Server]\t \
                            insertIsql failed with error = %d\n"), error ));
                            }
                        UpdateDriveQuota( aDrives, *aItems[i] );
                        file.Zero();
                        }
                    //duplicated file, just set status
                    if( ECmUnSelected == aItems[i]->Selected() )
                        {
                        aItems[i]->SetStatus( sta );
                        }
                    }
                else
                    {
                     //duplicated file, just set status
                    if( aItems[i]->RefId() != 0 )
                        {
                        aItems[i]->SetStatus( sta );
                        }
                    else
                        {
                        for ( TInt j = 0; j < aItems.Count(); j++ )
                            {
                            //If the prioritys of items with the same Dbid
                            //have changed, should delete files
                            if( aItems[i]->DbId() == aItems[j]->DbId() &&
                                i != j &&
                                ECmSelected == aItems[j]->Selected() &&
                                aItems[j]->Priority() < 
                                aItems[i]->Priority() )
                                {
                                file.Append( *ResolveAndAppendRootPathL( fs,
                                    aItems[i]->DriveNumber() ));
                                file.Append( aItems[i]->Path() );
                                TRAPD( error, aFilesToDelete->InsertIsqL(
                                                                file ) );
                                if ( error )
                                    {
                                    TRACE( Print( _L("[Cm Server]\t \
                                    CCmServer::DeleteFilledFilesL \
                                    error = %d\n"), error ));
                                    }
                                UpdateDriveQuota( aDrives, *aItems[i] );
                                file.Zero();
    
                                //set status and break
                                aItems[i]->SetStatus( sta );
                                j = aItems.Count();
                                }
                            }
                        }
                    }
                }     
            }            
        }
    fs.Close();               
    }
                                   
// ---------------------------------------------------------------------------
// CCmServer::ServiceExecuted
// ---------------------------------------------------------------------------
//
void CCmServer::ServiceExecuted( TCmService aService, TInt aErrCode )
    {
    LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted"));
    TRACE(Print(_L("[Cm Server]\t service %d state %d err %d\n"),
        aService, iServerState, aErrCode ));

#ifdef _DEBUG

    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[Cm Server]\t heap allocsize %d, size %d, cells %d"),
        alloc, size, cells ));

#endif

    if ( iDeviceDiscoveryTimer )
        {
        delete iDeviceDiscoveryTimer;
        iDeviceDiscoveryTimer = NULL;
        }

    if ( aService != ServerState() )
        {
        LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted INTERNAL ERROR"));
        return;
        }

    TBool returnToIdle = ETrue;
    switch ( aService )
        {
        case ECmServiceHarvest:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceHarvest"));

            // Harvest includes fill preprocessing
            iServerState = ECmServerStateIdle;
            TRAPD( error, ExecuteServiceL( ECmServicePreProcessingFill ) );
            if ( error )
                {
                TRACE( Print( _L("[Cm Server]\t CCmServer::\
                ServiceExecuted fill preprocessing error = %d"), error));
                }
            // return from here because processing continues,
            // so we do not complete msg
            return;
            }
        case ECmServiceFill:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceFill"));
            TRACE(Print(_L("[Cm Server]\t Setting mm to state = %d"),
                (TInt)iMmState ));
            TRAPD( error, iMemoryManager->SetQuotaListenerStateL(
                                                        iMmState ) );
            if ( error )
                {
                TRACE( Print( _L("[Cm Server]\t CCmServer::\
                ServiceExecuted ECmServiceFill \
                error = %d"), error));
                }

            if ( aErrCode == KErrNone )
                {
                iDbManager->SetSyncTime();
                }
            // Creation of av controller failed
            if( KErrServerTerminated == aErrCode )
                {
                aErrCode = KErrNone;
                }
            iFillManager->SetAvController( NULL );
            if ( iAvController )
                {
                iAvController->RemoveDeviceObserver();
				iAvController->Release();
				iAvController = NULL;
                LOG(_L("[Cm Server]\t avc deleted"));
                }
            break;
            }
        case ECmServiceStore:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceStore"));

            iStoreManager->SetAvController( NULL );
            if ( aErrCode != KErrNone )
                {
                if ( iAvController )
                    {
                    iAvController->RemoveDeviceObserver();
					iAvController->Release();
					iAvController = NULL;
                    LOG(_L("[Cm Server]\t avc deleted"));
                    }
                }
            break;
            }
        case ECmServicePreProcessingStore:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServicePreProcessingStore"));
            break;
            }

        case ECmServicePreProcessingFill:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
            ECmServicePreProcessingFill"));
            iServerState = ECmServerStateIdle;
            TRAPD( error, ExecuteServiceL( ECmServicePreProcessingStore ) );
            if ( error )
                {
                TRACE( Print( _L("[Cm Server]\t CCmServer::\
                ServiceExecuted ECmServicePreProcessingFill \
                error = %d"), error));
                }
            break;
            }

        case ECmServiceGetStoreFields:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceGetStoreFields"));
            break;
            }

        case ECmServiceGetFillFields:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceGetFillFields"));
            break;
            }

        case ECmServiceSearchForServers:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceSearchForServers"));
            iAvController->RemoveDeviceObserver();
    		iAvController->Release();
    		iAvController = NULL;
    		iServerState = ECmServerStateIdle;
            break;
            }

        case ECmServiceDeleteFilledFiles:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServiceDeleteFilledFiles"));

            // check if store is flagged to be run after delete
            if ( aErrCode == KErrNone && iExecuteStoreAfterDelete )
                {
                iExecuteStoreAfterDelete = EFalse;
                TRAPD( error, ExecuteStoreL() );
                if ( error )
                    {
                    TRACE( Print( _L("[Cm Server]\t CCmServer::\
                    ServiceExecuted ECmServiceDeleteFilledFiles \
                    error = %d"), error));
                    }
                return;
                }
            iExecuteStoreAfterDelete = EFalse;
            break;
            }

        case ECmServicePreProcessingFilllist:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServicePreProcessingFilllist"));
            break;
            }
        case ECmServerStateUpdatingFillLists:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServerStateUpdatingFillLists"));
            break;
            }
        case ECmServerStateDeletingMetadata:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                ECmServerStateDeletingMetadata"));
            break;
            }
        default:
            {
            LOG(_L("[Cm Server]\t CCmServer::ServiceExecuted \
                default"));
            }
        }

    if ( aService == ECmServicePreProcessingFill )
        {
        return;
        }

    if ( !iCurrentAsyncMsg.IsNull() )
        // client-initiated case
        {
        LOG(_L("[Cm Server]\t CCmServer::completing message.."));

        iCurrentAsyncMsg.Complete( aErrCode );

        iAsyncRequestOn = EFalse;

        if ( returnToIdle )
            {
            iServerState = ECmServerStateIdle;
            }
        }

    else // scheduler-initiated case
        {
        if ( returnToIdle )
            {
            iServerState = ECmServerStateIdle;
            }

        iScheduler->ServiceExecuted( aService, aErrCode );
        }


    if ( aService == ECmServiceFill ||
         aService == ECmServicePreProcessingStore )
        {
        TCmProgressInfo progressInfo;
        progressInfo.iService = ECmServiceNone;
        progressInfo.iProcessedItems = 0;
        progressInfo.iItemsToTransferPerService = 0;
        TCmProgressInfoPckg progressPckg( progressInfo );

        TInt err =
            RProperty::Set( KCmPropertyCat, KCmProperty, progressPckg );

        TRACE( Print( _L(
            "[CMSERVER]\t CmServer::ServiceExecuted progresInfo sent"),
            progressInfo.iProcessedItems, progressInfo.iTotalItems));
        }

    TCmServiceState cmState;
    iSettings->GetServiceState( ECmServiceContentManager, cmState );
    TBool mmEnabled;
        iSettings->GetMemoryManagerStatus( mmEnabled );

    if ( !cmState && !mmEnabled && !(*iSessionCount) &&
         !ServerState() == ECmServerStateIdle
         )
        // server & mm disabled and idle, no sessions
        {
        // shutdown server
        LOG(_L("[Cm Server]\t Server & mm disabled and \
            no sessions left, shutting down.."));
        CActiveScheduler::Stop();
        }

    }

// ---------------------------------------------------------------------------
// CCmServer::UPnPDeviceDiscovered
// ---------------------------------------------------------------------------
//
void CCmServer::UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[Cm Server]\t CCmServer::UPnPDeviceDiscovered"));
    CUpnpAVDevice* device = const_cast<CUpnpAVDevice*> ( &aDevice );

    TRACE(Print(_L("[Cm Server]\t serverstate %d devicetype %d\n"),
        iServerState, device->DeviceType() ));
    TBool allFound( EFalse );
    if( device->DeviceType() == CUpnpAVDevice::EMediaServer )
        {
        for( TInt i = 0 ; i < iMediaServers.Count(); i++ )
            {
            if( KErrNotFound !=
                device->Uuid().Match( iMediaServers[i]->MediaServer() ) )
                {
                iMediaServers[i]->SetIsActive( EFalse );
                }
            if( iMediaServers[i]->IsActive() )
                {
                allFound = ETrue;
                }
            }
         }

    if ( ServerState() == ECmServerStateSearchingForServers &&
         device->DeviceType() == CUpnpAVDevice::EMediaServer )
        {
        TRAPD( error, CheckServerL( *device ) );
        if ( error )
            {
            TRACE( Print( _L("[Cm Server]\t CCmServer::\
            UPnPDeviceDiscovered error = %d"), error));
            }
        }

    if( !allFound && iMediaServers.Count() &&
        device->DeviceType() == CUpnpAVDevice::EMediaServer &&
        iDeviceDiscoveryTimer )
        {
        delete iDeviceDiscoveryTimer;
        iDeviceDiscoveryTimer = NULL;
        
        TRACE( Print(
            _L("[Cm Server]\t All Servers found => Start Syncing\n") ) );
        TRAPD( error, DoSearchTimerCompletedL() );
        if ( error )
            {
            TRACE( Print( _L("[Cm Server]\t CCmServer::\
            UPnPDeviceDiscovered searchtimer error = %d"), error));
            }
        iMediaServers.ResetAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::UPnPDeviceDisappeared
// ---------------------------------------------------------------------------
//
void CCmServer::UPnPDeviceDisappeared( const CUpnpAVDevice& /* aDevice */)
    {
    LOG(_L("[Cm Server]\t CCmServer::UPnPDeviceDisappeared"));
    }

// ---------------------------------------------------------------------------
// CCmServer::DeleteCompleteL
// ---------------------------------------------------------------------------
//
void CCmServer::DeleteCompleteL( TInt aErr )
    {
    LOG(_L("[Cm Server]\t CCmServer::DeleteCompleteL"));
    
    iDeletingDeletedItems = EFalse;
    // just complete delete process
    ServiceExecuted( ECmServiceDeleteFilledFiles, aErr );
    }

// ---------------------------------------------------------------------------
// See mdesession.h
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CCmServer::HandleSessionOpened( CMdESession& /*aSession*/,
    TInt aError )
    {
#else
void CCmServer::HandleSessionOpened( CMdESession& /*aSession*/,
    TInt /*aError*/ )
    {
#endif
    TRACE(Print(_L("[Cm Server]\t CCmServer::HandleSessionOpened\
         aError = %d"), aError ));
    }

// ---------------------------------------------------------------------------
// See mdesession.h
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CCmServer::HandleSessionError( CMdESession& /*aSession*/,
    TInt aError )
    {
#else
void CCmServer::HandleSessionError( CMdESession& /*aSession*/,
    TInt /*aError*/ )
    {
#endif
    TRACE(Print(_L("[Cm Server]\t CCmServer::HandleSessionError\
         aError = %d"), aError ));
    }

// ---------------------------------------------------------------------------
// CCmServer::WLANConnectionLost
// ---------------------------------------------------------------------------
//
void CCmServer::WLANConnectionLost()
    {
    LOG(_L("[Cm Server]\t CCmServer::WLANConnectionLost"));
    LOG(_L("[Cm Server]\t Finishing operation..."));

    TInt err( KErrNone );

    TRAP( err,CancelServiceL() );

    if ( err )
        {
        TRACE( Print( _L( "[Cm Server]\t Wlan lost error = %d" ), err ) );
        }
    }

// --------------------------------------------------------------------------
// CCmServer::DriveSelectionStatus
// --------------------------------------------------------------------------
TBool CCmServer::DriveSelectionStatus()
    {
    LOG(_L("[Cm Server]\t CCmServer::DriveSelectionStatus"));
    
    return iDriveSelectionStatus;
    }

// --------------------------------------------------------------------------
// CCmServer::SetDriveSelectionStatus
// --------------------------------------------------------------------------
void CCmServer::SetDriveSelectionStatus( const TBool aDriveSelectionStatus )
    {
    LOG(_L("[Cm Server]\t CCmServer::SetDriveSelectionStatus"));
    
    iDriveSelectionStatus = aDriveSelectionStatus;
    }

// ---------------------------------------------------------------------------
// CCmServer::ResolveAndAppendRootPathL
// ---------------------------------------------------------------------------
//
HBufC* CCmServer::ResolveAndAppendRootPathL( RFs& aFs,
                                             const TInt aDriveNumber )
    {
    LOG(_L("[Cm Server]\t CCmServer::ResolveAndAppendRootPathL"));

    HBufC* buf = HBufC::NewLC( KDriveBufferSize );
    TChar driveChar;
    TInt error = aFs.DriveToChar( aDriveNumber, driveChar );
    if ( !error )
        {
        buf->Des().Append( driveChar );
        buf->Des().Append( KColon );
        }
    buf->Des().Append( KDoubleBackSlash );
    CleanupStack::Pop();
    return buf;
    }

// ---------------------------------------------------------------------------
// CCmServer::UpdateDriveQuota
// ---------------------------------------------------------------------------
//
void CCmServer::UpdateDriveQuota( RPointerArray<CCmDriveInfo>& aDrives,
    CCmFillListItem& aItem )
    {
    LOG(_L("[Cm Server]\t CCmServer::UpdateDriveQuota"));

    for( TInt i = 0; i < aDrives.Count(); i++ )
        {
        if( aDrives[i]->DriveNumber() == aItem.DriveNumber() )
            {
            TInt64 usedQuota( aDrives[i]->UsedDriveQuota() );
            usedQuota -= aItem.Size();
            aDrives[i]->SetUsedDriveQuota( usedQuota );
            i = aDrives.Count();
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmServer::ShrinkCompleteL
// ---------------------------------------------------------------------------
//
void CCmServer::ShrinkCompleteL( TInt /* aErr */)
    {
    LOG(_L("[Cm Server]\t CCmServer::ShrinkCompleteL"));
    }

// ---------------------------------------------------------------------------
// CCmServer::GetStatusByGroupId
// ---------------------------------------------------------------------------
//
TInt CCmServer::GetStatusByGroupId( TUint aListId,
                                    TCmListItemStatus& aStatus )
    {
    LOG(_L("[Cm Server]\t CCmServer::GetStatusByGroupId"));

    TInt result( KErrNotFound );
    TInt count = iFillRuleContainer->FillRuleCount();
    for ( TInt i( 0 ); i < count; i++ )
        {
        CCmFillRule* rule =
            iFillRuleContainer->FillRule( i );
        if ( rule->ListId() == aListId )
            {
            aStatus = rule->Status();
            result = KErrNone;
            i = count;
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CCmServer::CheckServerL
// ---------------------------------------------------------------------------
//
void CCmServer::CheckServerL( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[Cm Server]\t CCmServer::CheckServerL"));

    CCmMediaServerFull* tempServer = CCmMediaServerFull::NewLC();
    tempServer->SetUDNL( aDevice.Uuid() );
    iDbManager->PrepareQueryCmdL( EMediaServerInfoQyery );
    TInt errCode = iDbManager->QueryMediaServerL( tempServer );
    if ( errCode == KErrNotFound )
        {
        LOG(_L("[CmMdh Server]\t not found in DB, adding new.."));
        tempServer->SetMediaServerNameL( aDevice.FriendlyName() );
        tempServer->SetSystemUpdateID( KDefaultSystemUpdateId );
        TTime timeStamp;
        timeStamp.HomeTime();
        tempServer->SetVisibleDate( timeStamp );
        iDbManager->AddMediaServerL( tempServer );

        }
    else
        {
        TRACE(Print(_L("[CmMdh Server]\t CCmServer::CheckServerL \
        error = %d"), errCode ));
        }
    CleanupStack::PopAndDestroy( tempServer );
    }

// ---------------------------------------------------------------------------
// CCmServer::SetAsyncRequestStateL
// ---------------------------------------------------------------------------
//
void CCmServer::SetAsyncRequestStateL( TBool aState )
    {
    LOG(_L("[Cm Server]\t CCmServer::SetAsyncRequestStateL"));

    iAsyncRequestOn = aState;

    if ( !iCurrentAsyncMsg.IsNull() )
        {
        iCurrentAsyncMsg.Complete( KErrNone );
        }

    if ( !iScheduler )
        {
        iScheduler = CCmScheduler::NewL( *this, *iSettings );
        LOG(_L("[Cm Server]\t CCmServer::SetAsyncRequestState\
                 scheduler created"));
        }

    // responsibility of starting services transferred to scheduler
    iScheduler->SetScheduledServiceState( ETrue );
    }

// ---------------------------------------------------------------------------
// CompareById
// For sort. Sort is based on db id
// ---------------------------------------------------------------------------
// 
TInt CCmServer::CompareById( const CCmFillListItem& aItem1,
                             const CCmFillListItem& aItem2 )
    {
    TInt result = 0;

    if ( aItem1.DbId() < aItem2.DbId() )
        {
        result = -1;
        }
    else if ( aItem1.DbId() > aItem2.DbId() )
        {
        result = 1;
        }

    return result;
    }
// End of File
