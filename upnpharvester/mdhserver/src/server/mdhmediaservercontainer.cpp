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
* Description:      Metadata Harvester server's mediaserver container component
*
*/







// INCLUDE FILES
#include    <e32property.h>
#include    <f32file.h>

#include    "cmsettingsfactory.h"
#include    "cmsettings.h"
#include    "cmdmmain.h"
#include    "cmmediaserverfull.h"
#include    "cmsearchresponsehash.h"
#include    "cmsettings.h"
#include    <upnpstring.h>
#include    <upnperrors.h>
#include    "mdhconnectionmonitor.h"
#include    "mdhmediaservercontainer.h"
#include    "mdhmetadatacollector.h"
#include    "mdhserver.h"
#include    "msdebug.h"


// CONSTANTS
#ifdef __SERIES60_31__
_LIT8( KAVControlPointFriendlyName,  "AVDevice" );
#endif //__SERIES60_31__
_LIT8( KMediaServer,                 "MediaServer" );
#ifdef _DEBUG
_LIT8( KFriendlyNameTag,             "friendlyName" );
#endif

const TInt KDefaultCaculateSize = 100;
const TInt KDefaultChunkSize = 150;
const TInt KDefaultAddGranularity = 500;

// amount of microseconds in one second
const TInt KMicrosecondsInSecond = 1000000;
// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
// 
CCmMdhMediaserverInfo* CCmMdhMediaserverInfo::NewL( )
    {  
    CCmMdhMediaserverInfo* self = CCmMdhMediaserverInfo::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
//     
CCmMdhMediaserverInfo* CCmMdhMediaserverInfo::NewLC()
    {   
    CCmMdhMediaserverInfo* self = new ( ELeave ) CCmMdhMediaserverInfo( );
    CleanupStack::PushL( self );
    self->ConstructL( );    
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CCmMdhMediaserverInfo::~CCmMdhMediaserverInfo()
    {
    delete iUuid;
    }
    
// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//            
CCmMdhMediaserverInfo::CCmMdhMediaserverInfo( )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//  
void CCmMdhMediaserverInfo::ConstructL( )
    {
                                  
    } 

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
CCmMdhMediaserverContainer* CCmMdhMediaserverContainer::NewL(
    CCmMdhServer& aMdhServer)
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::NewL"));

    CCmMdhMediaserverContainer* self =
        new (ELeave) CCmMdhMediaserverContainer(aMdhServer);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
CCmMdhMediaserverContainer::CCmMdhMediaserverContainer(
    CCmMdhServer& aMdhServer)
    :iMdhServer( aMdhServer )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CCmMdhMediaserverContainer"));
    iAvControlPoint = NULL;
    iMetadataCollector = NULL;
    iTimer = NULL;
    iMediaserverIndex = 0;
    iResponsesPending = 0;
    iTotalItemCount = 0;
    iHarvestErrCode = KErrNone;
    iHarvestActive = EFalse;
    iPropertyItemIndex = 0;
    iProgressInfo.iService = ECmServiceHarvest;
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::ConstructL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::ConstructL"));
    iDbManager = CCmDmMain::NewL();
    iSettings = CCmSettingsFactory::NewCmSettingsEngineL();
    iSettings->GetIapL( iIap );
    TRACE( Print( _L("[CmMdh Server]\t Monitoring iap %d"), iIap ));
    iMdHConMon = CMdHConnectionMonitor::NewL( *this, iIap );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CCmMdhMediaserverContainer::~CCmMdhMediaserverContainer()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             ~CCmMdhMediaserverContainer"));

    TRACE( Print( _L("[CmMdh Server]\t deleting %d media servers"),
        iMediaservers.Count()));

    iMediaservers.ResetAndDestroy();
    iOldMediaServers.ResetAndDestroy();

    LOG(_L("[CmMdh Server]\t deleting settings engine.."));
	if ( iSettings )
	    {
	    iSettings->Close();    
	    }

    LOG(_L("[CmMdh Server]\t deleting db manager.."));
    if ( iDbManager )
        {
        delete iDbManager;    
        }    

    LOG(_L("[CmMdh Server]\t deleting metadata collector.."));
    if ( iMetadataCollector )
        {
        delete iMetadataCollector;
        }

    LOG(_L("[CmMdh Server]\t deleting av control point.."));
    if ( iAvControlPoint )
        {
        delete iAvControlPoint;    
        }

    LOG(_L("[CmMdh Server]\t deleting timer.."));
    if ( iTimer )
        {
        delete iTimer;    
        }
    
    LOG(_L("[CmMdh Server]\t deleting connection monitor.."));
    if ( iMdHConMon )
        {
        delete iMdHConMon;    
        }
    
    LOG(_L("[CmMdh Server]\t All deleted.."));
    }
        
// ---------------------------------------------------------------------------
// Harvest a media server
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::DoHarvestMediaserverL(
    TInt aMediaserverIndex )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             DoHarvestMediaserverL"));
    iMdhServer.SetServerStateL(ECmMdhServerStateHarvesting);
    if ( iMediaservers[aMediaserverIndex]->iMediaserverState
        == ECmMdhMediaserverNotSynchronized)
        {
        iMetadataCollector->HarvestMediaserverL(
            *iMediaservers[aMediaserverIndex]->iUuid );
        iHarvestActive = ETrue;
        }
    else
        {
        LOG(_L("[CmMdh Server]\t No search caps or \
                                 no sync needed, skipping.."));
        HarvestCompleteL( KErrNone );
        }
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             DoHarvestMediaserverL end"));
    }

// ---------------------------------------------------------------------------
// Adds a media server to database if it doesn't exist there
// ---------------------------------------------------------------------------
TBool CCmMdhMediaserverContainer::AddToDbIfNewL( CUpnpDevice& aMediaserver )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::AddToDbIfNewL"));

    CCmMediaServerFull* tempServer = CCmMediaServerFull::NewLC();
    tempServer->SetUDNL( aMediaserver.Uuid() );
    iDbManager->PrepareQueryCmdL( EMediaServerInfoQyery );
    TInt errCode = iDbManager->QueryMediaServerL( tempServer );
    TBool ret = EFalse;
    
    if ( errCode != KErrNotFound )
        {
        TRACE( Print( _L("[CmMdh Server]\t FillUsage: %d"),
        		tempServer->FillUsage()));
        ret = (TBool)tempServer->FillUsage();
        iAllFound = ETrue;
        for( TInt i = 0 ; i < iOldMediaServers.Count(); i++ )
            {
            if( KErrNotFound != aMediaserver.Uuid().Match(
                iOldMediaServers[i]->MediaServer() ) )
                {
                TRACE( Print( _L("[CmMdh Server]\t In db...")));
                iOldMediaServers[i]->SetFillUsage( EFalse );
                }
            if( iOldMediaServers[i]->FillUsage())
                {
                TRACE( Print( _L("[CmMdh Server]\t All not found")));
                iAllFound = EFalse;
                }
            }
        }
    CleanupStack::PopAndDestroy( tempServer );
    return ret;
    }

// ---------------------------------------------------------------------------
// Decreases pending responses counter and finishes search if ready
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::DecResponsesAndFinishIfReadyL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             DecResponsesAndFinishIfReadyL"));
    if ( !(--iResponsesPending )  )
        {
        if( iAllFound )
	        {
	        DoSearchTimerCompletedL();	
	        }
        }
    }

// ---------------------------------------------------------------------------
// Static timer callback function.
// ---------------------------------------------------------------------------
TInt CCmMdhMediaserverContainer::SearchTimerCompletedL( TAny* aInstance )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             SearchTimerCompleted"));
    return ((CCmMdhMediaserverContainer*)aInstance)
        ->DoSearchTimerCompletedL();
   }

// ---------------------------------------------------------------------------
// Timer callback function.
// ---------------------------------------------------------------------------
TInt CCmMdhMediaserverContainer::DoSearchTimerCompletedL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
        DoSearchTimerCompleted"));
    delete iTimer;
    iTimer = NULL;
    TRACE( Print( _L("[CmMdh Server]\t Found %d media servers"),
        iMediaservers.Count()));

    if ( !iResponsesPending )
        {
        iOldMediaServers.ResetAndDestroy();
        iMdhServer.SetServerStateL(ECmMdhServerStateIdle);
        }
    return KErrNone;
   }

// ---------------------------------------------------------------------------
// Handles UPnP device discoveries.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::DeviceDiscoveredL( CUpnpDevice* aDevice )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::DeviceDiscoveredL"));

#ifdef _DEBUG

    HBufC* uuid = UpnpString::ToUnicodeL( aDevice->Uuid() );
    HBufC* name = UpnpString::ToUnicodeL(
                    aDevice->DescriptionProperty( KFriendlyNameTag() ));
    HBufC* type = UpnpString::ToUnicodeL( aDevice->DeviceType() );
    TRACE( Print(_L("[CmMdh Server]\t type: %S uuid: %S, name %S"),
        type, uuid, name ));
    delete uuid;
    delete name;
    delete type;

#endif


    if (!iTimer) // search is no more active
        {
        LOG(_L("[CmMdh Server]\t Search no more active, returning.."));
        return;
        }

    if( aDevice->DeviceType().Find(KMediaServer) != KErrNotFound && 
        !iAllFound )
        {
        LOG(_L("[CmMdh Server]\t Found a media server"));

        if ( AddToDbIfNewL( *aDevice ) )
            {
            ++iResponsesPending;
            iAvControlPoint->CdsSearchCapabilitiesActionL( aDevice->Uuid() );
            }
#ifdef _DEBUG

        else
            {
            LOG(_L("[CmMdh Server]\t not active, skipping.."));
            }
#endif

        }

    }

// ---------------------------------------------------------------------------
// Handles UPnP device disappears.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::DeviceDisappearedL(
    CUpnpDevice* /* aDevice */ )
    {
    LOG( _L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
             DeviceDisappearedL") );
    }

// ---------------------------------------------------------------------------
// Searches UPnP Media servers.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::SearchMediaserversL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             SearchMediaServersL"));

    iResponsesPending = 0;
    iProgressInfo.iTotalItems = 0;
    iProgressInfo.iProcessedItems = 0;

    if ( !iAvControlPoint )
        {
        iSettings->GetIapL( iIap );
        // Create and start (automatic) the AV control point
        TRACE( Print( _L("[CmMdh Server]\t IAP to use is  %d"), iIap));
        LOG( _L("[CmMdh Server]\t Creating AV control point..." ) );

        iAvControlPoint = CUpnpAVControlPoint::NewL( *this );
        }
        
    LOG( _L("[CmMdh Server]\t Creating CPeriodic timer..." ) );
    iTimer = CPeriodic::NewL(EPriorityHigh);

    TInt deviceDiscoveryDelay;
    iSettings->GetDiscoveryDelay( deviceDiscoveryDelay );
    TRACE(Print(_L("[CmMdh Server]\t using device discovery delay %d\n"),
        deviceDiscoveryDelay));

    iTimer->Start(deviceDiscoveryDelay * KMicrosecondsInSecond,
                  0,
                  TCallBack(SearchTimerCompletedL, this));

    iMdhServer.SetServerStateL(ECmMdhServerStateSearching);

    iOldMediaServers.ResetAndDestroy();
    iDbManager->GetMediaServersL( iOldMediaServers );
    
    for ( TInt i = 0; i < iOldMediaServers.Count(); i++ )
        {
        if ( !iOldMediaServers[i]->FillUsage() &&
             iOldMediaServers[i]->SystemUpdateID() != KErrNotFound )
            {
            iOldMediaServers[i]->SetSystemUpdateID( KErrNotFound );
            iDbManager->UpdateMediaServerInfo( iOldMediaServers[i] );
            }
        }
    
    iAllFound = EFalse;
    SendProgressInfo ( 0 );
    }

// ---------------------------------------------------------------------------
// Harvests media servers
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::HarvestMediaserversL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             HarvestMediaserversL"));

    if ( !iAvControlPoint )
        {
        User::Leave( KErrNotReady );
        }

    if ( !iMediaservers.Count() )
        {
        User::Leave( KErrNotFound );
        }

    TBool harvestNeeded = EFalse;
    for ( TInt i = 0; i < iMediaservers.Count(); i++ )
        {
        if (iMediaservers[i]->iMediaserverState
            == ECmMdhMediaserverNotSynchronized )
            {
            harvestNeeded = ETrue;
            i = iMediaservers.Count(); // break from the loop
            }
        }

    if ( !harvestNeeded )
        {
        User::Leave( KErrAlreadyExists );
        }

    if ( !iMetadataCollector )
        {
        LOG( _L("[CmMdh Server]\t Creating metadata collector.." ) );

        TInt searchCount = KDefaultChunkSize;
        TInt addGranularity = KDefaultAddGranularity;
        iSettings->GetSearchCount( searchCount );
        iSettings->GetAddCount( addGranularity );
        TRACE( Print(
            _L("[CmMdh Server]\t using chunk size %d and addgran %d"),
            searchCount, addGranularity));
        iMetadataCollector =
            CCmMdhMetadataCollector::NewL(
                *this, *iAvControlPoint, searchCount, addGranularity );

        }
    iMediaserverIndex = 0;
    iHarvestErrCode = KErrNone;
    DoHarvestMediaserverL( iMediaserverIndex );

    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             HarvestMediaserversL end"));

    }

// ---------------------------------------------------------------------------
// Harvest completion callback function
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::HarvestCompleteL( TInt aErr )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::HarvestComplete"));
    iHarvestActive = EFalse;
    if ( aErr != KErrNone )
        {
        iHarvestErrCode = aErr;
        }
    else
        {
        TInt newUpdateId =
            iMediaservers[iMediaserverIndex]->iSystemUpdateId;
        if ( newUpdateId ) // was set to 0 if no update needed
            {
            // store changed systemupdateid
            CCmMediaServerFull* tempServer = CCmMediaServerFull::NewLC();
            tempServer->SetUDNL(
                *(iMediaservers[iMediaserverIndex]->iUuid) );
            iDbManager->PrepareQueryCmdL( EMediaServerInfoQyery );
            TInt errCode = iDbManager->QueryMediaServerL( tempServer );
            if ( errCode != KErrNotFound )
                {
                TRACE( Print( _L
                    ("[CmMdh Server]\t Updating systemupdateid to %d.."),
                    newUpdateId ));

                tempServer->SetSystemUpdateID( newUpdateId );
                iDbManager->UpdateMediaServerInfo( tempServer );
                }
            CleanupStack::PopAndDestroy( tempServer );
            }
        }

    if ( ++iMediaserverIndex < iMediaservers.Count() )
        {
        // harvest next
        DoHarvestMediaserverL( iMediaserverIndex );
        }
    else
        {
        // no more media servers
        iMdhServer.SetServerStateL(ECmMdhServerStateIdle, iHarvestErrCode);
        }
    }


// ---------------------------------------------------------------------------
// Cancels search
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CancelSearchL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::CancelSearchL"));
    if (!iTimer) // no search is active
        {
        User::Leave( KErrNotReady );
        }

    iTimer->Cancel();
    iMediaservers.ResetAndDestroy();
    delete iTimer;
    iTimer = NULL;
    iMdhServer.SetServerStateL(ECmMdhServerStateIdle, KErrCancel);
    }

// ---------------------------------------------------------------------------
// Cancels harvest
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CancelHarvestL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::CancelHarvestL"));
    iHarvestActive = EFalse;
    iMetadataCollector->CancelHarvest();
    iMdhServer.SetServerStateL(ECmMdhServerStateIdle, KErrCancel);
    }

// ---------------------------------------------------------------------------
// Gets media server id from database
// ---------------------------------------------------------------------------
TInt CCmMdhMediaserverContainer::IdForMediaServerL( TDesC8& aUuid )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::IdForMediaServerL"));

    iDbManager->PrepareQueryCmdL( EMediaServerIdQuery );

    TInt id = iDbManager->QueryMediaServerId( aUuid );
    TRACE( Print( _L("[CmMdh Server]\t returning id %d"), id));

    return id;
    }

// ---------------------------------------------------------------------------
// Sends progress info
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::SendProgressInfo( TInt aProgress )
    {
    TRACE( Print( _L(
        "[CmMdh Server]\t CCmMdhMediaserverContainer::\
                          SendProgressInfo( %d )"),
        aProgress));

#ifdef _DEBUG

    TInt alloc;
    TInt cells = User::Heap().AllocSize( alloc );
    TInt size = User::Heap().Size();
    TRACE(Print(_L("[CmMdh Server]\t heap allocsize %d, size %d, cells %d"),
        alloc, size, cells ));

#endif


    if ( iProgressInfo.iTotalItems > iProgressInfo.iProcessedItems )
        {
        iProgressInfo.iProcessedItems += aProgress;
        TCmProgressInfoPckg progressPckg( iProgressInfo );
        TRACE( Print( _L("[CmMdh Server]\t processed: %d total %d"),
                iProgressInfo.iProcessedItems, iProgressInfo.iTotalItems) );
        
        if ( ( iProgressInfo.iTotalItems <= iProgressInfo.iProcessedItems ) ||
             ( iProgressInfo.iTotalItems > iProgressInfo.iProcessedItems &&
               iProgressInfo.iProcessedItems > 0 &&
               ( iProgressInfo.iProcessedItems - iPropertyItemIndex ) >=
               iProgressInfo.iTotalItems / KDefaultCaculateSize )
           )
            {
            iPropertyItemIndex = iProgressInfo.iProcessedItems;
            TInt err = RProperty::Set( KCmPropertyCat, KCmProperty, 
                                   progressPckg );
            TRACE( Print( _L("[CmMdh Server]\t RProperty::Set returned %d"), 
                   err)); 
            }
        else
            {
            LOG(_L("[CmMdh Server]\t progress small no need to pub&sub"));
            }
        }

    else
        {
        iProgressInfo.iProcessedItems += aProgress;
        LOG(_L("[CmMdh Server]\t Bigger than 100%% or not beginning"));
        }

    }


// Responses

// ---------------------------------------------------------------------------
// Observer callback for Content Directory Search function.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSearchResponse(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& /*aContainerId*/,
        const TDesC8& /*aSearchCriteria*/,
        const TDesC8& /*aFilter*/,
        TInt /*aIndex*/,
        TInt /*aRequest*/,
        const TDesC8& /*aSortCriteria*/,
        const TDesC8& aResult,
        TInt aReturned,
        TInt aMatches,
        const TDesC8& /*aUpdateID*/)
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSearchResponse"));

    TRAPD( err, CdsSearchResponseL( aUuid, 
                                    aSessionId, 
                                    aErr, 
                                    aResult,
                                    aReturned,
                                    aMatches ) );
    if ( err ) 
        {
        TRACE( Print(_L("[CmMdh Server]\t CdsSearchResponseL leaved with err:\
             %d"), err ) );
        }
    }

// ---------------------------------------------------------------------------
// Leaving version of CdsSearchResponseL callback.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSearchResponseL(
        const TDesC8& aUuid,
        TInt aSessionId,
        TInt aErr,
        const TDesC8& aResult,
        TInt aReturned,
        TInt aMatches
        )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSearchResponseL"));
    if ( iHarvestActive )
        {
        iMetadataCollector->CdsSearchResponse(
            aSessionId, aErr, aResult, aReturned, aMatches );
        }
    else if ( iTimer || iResponsesPending ) // media server search active
        {
        if ( aErr != EHttpOk )
            {
            for( TInt i = 0; i < iMediaservers.Count(); i++ )
                {
                if ( *(iMediaservers[i]->iUuid) == aUuid )
                    {
                    LOG(_L("[CmMdh Server]\t search error, \
                            will not sync this.."));
                    iMediaservers[i]->iMediaserverState =
                        ECmMdhMediaserverNoSearchCaps;
                    iMediaservers[i]->iSystemUpdateId = 0;
                    }
                }
            }
        else
            {
            iProgressInfo.iTotalItems += aMatches;
            TRACE( Print( _L("[CmMdh Server]\t total item count \
                                increased to %d"),
            iProgressInfo.iTotalItems));
            if( iAllFound )
                {
                // when all servers are discovered,1 percentage is 
                // set.
                SendProgressInfo( iProgressInfo.iTotalItems / 
                				  KDefaultCaculateSize );
                iProgressInfo.iProcessedItems = 0;
                delete iTimer;
                iTimer = NULL;
                iOldMediaServers.ResetAndDestroy();
                TRACE( Print( _L("[CmMdh Server]\t All servers found!!!")));
                }
            }
        DecResponsesAndFinishIfReadyL();
        }
    }

// ---------------------------------------------------------------------------
// Observer callback for Content Directory SearchCapabilities function.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSearchCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt /* aSessionId */,
            TInt aErr,
            const TDesC8& aSearchCaps )
    {

    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSearchCapabilitiesResponse"));

    TRAPD( err, CdsSearchCapabilitiesResponseL( aUuid, 
                                                aErr, 
                                                aSearchCaps ) );
    if ( err ) 
        {
        TRACE( Print(_L("[CmMdh Server]\t CdsSearchCapabilitiesResponseL \
            leaved with err: %d"), err ) );
        }
    }

// ---------------------------------------------------------------------------
//  Leaving version of CdsSystemUpdateIdResponse callback.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSearchCapabilitiesResponseL(
            const TDesC8& aUuid,
            TInt aErr,
            const TDesC8& aSearchCaps )
    {

    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSearchCapabilitiesResponseL"));

#ifdef _DEBUG

    HBufC* sc = UpnpString::ToUnicodeL( aSearchCaps );
    TRACE( Print(_L("[CmMdh Server]\t err: %d caps: %S"),
        aErr, sc ));
    delete sc;

#endif


    HBufC8* uuid = HBufC8::NewL( aUuid.Length() );
    uuid->Des().Copy( aUuid );
    CleanupStack::PushL( uuid );
    CCmMdhMediaserverInfo* info = CCmMdhMediaserverInfo::NewL( );
    CleanupStack::PushL( info );
    info->iUuid = uuid;
    iMediaservers.AppendL( info );

    if ( aErr == EHttpOk && aSearchCaps.Length() ) // has search capabilities
        {
        info->iMediaserverState = ECmMdhMediaserverNotSynchronized;
        iAvControlPoint->CdsSystemUpdateIdActionL( aUuid );
        }
    else
        {
        DecResponsesAndFinishIfReadyL();
        info->iSystemUpdateId = 0;
        info->iMediaserverState = ECmMdhMediaserverNoSearchCaps;
        }

    CleanupStack::Pop( info );
    CleanupStack::Pop( uuid );
    }


// ---------------------------------------------------------------------------
// Observer callback for Content Directory GetSystemUpdateID function.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSystemUpdateIdResponse(
            const TDesC8& aUuid,
            TInt /* aSessionId */,
            TInt aErr,
            TInt aSystemUpdateId )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSystemUpdateIdResponse"));

    TRACE( Print(_L("[CmMdh Server]\t err: %d suid: %d"),
        aErr, aSystemUpdateId ));
    
    TRAPD( err, CdsSystemUpdateIdResponseL( aUuid, 
                                            aErr, 
                                            aSystemUpdateId ) );
    if ( err ) 
        {
        TRACE( Print(_L("[CmMdh Server]\t CdsSystemUpdateIdResponseL leaved \
            with err: %d"), err ) );
        }
    }

// ---------------------------------------------------------------------------
// Leaving version of CdsSystemUpdateIdResponse callback.
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::CdsSystemUpdateIdResponseL(
            const TDesC8& aUuid,
            TInt aErr,
            TInt aSystemUpdateId )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             CdsSystemUpdateIdResponseL"));

    TRACE( Print(_L("[CmMdh Server]\t err: %d suid: %d"),
        aErr, aSystemUpdateId ));
    for( TInt i = 0; i < iMediaservers.Count(); i++ )
        {
        if ( *(iMediaservers[i]->iUuid) == aUuid )

            {
            LOG(_L("[CmMdh Server]\t checking \
                     media server's systemupdateid.."));

            TInt errCode = KErrNotFound;
            if ( aErr == EHttpOk )
                {
                CCmMediaServerFull* tempServer = CCmMediaServerFull::NewLC();
                tempServer->SetUDNL( aUuid );
                iDbManager->PrepareQueryCmdL( EMediaServerInfoQyery );
                errCode = iDbManager->QueryMediaServerL( tempServer );
                if ( errCode != KErrNotFound )
                    {
                    TRACE( Print( _L
                        ("[CmMdh Server]\t returned id %d, old id %d"),
                        aSystemUpdateId, tempServer->SystemUpdateID() ));


                    if ( aSystemUpdateId == tempServer->SystemUpdateID() )
                        {
                        LOG(_L("[CmMdh Server]\t systemupdateid \
                            match, no sync needed.."));
                        iMediaservers[i]->iMediaserverState =
                            ECmMdhMediaserverSynchronized;
                        iMediaservers[i]->iSystemUpdateId = 0;
                        DecResponsesAndFinishIfReadyL();
                        }
                    else
                        {
                        iMediaservers[i]->iSystemUpdateId = aSystemUpdateId;
                        errCode = iAvControlPoint->CdsSearchActionL( aUuid,
                                          KRootContainerId,
                                          KSearchCriteria,
                                          KSearchFilter,
                                          0,
                                          1,
                                          KSortCriteria );
                        TRACE( Print( _L("[CmMdh Server]\t search \
                            request returned %d"), errCode));
                        }
                    }
                CleanupStack::PopAndDestroy( tempServer );
                }
            if ( aErr != EHttpOk || errCode < 0 )
                {
                LOG(_L("[CmMdh Server]\t error with systemupdate \
                    id response, not syncing this.."));
                iMediaservers[i]->iMediaserverState =
                    ECmMdhMediaserverSynchronized;
                iMediaservers[i]->iSystemUpdateId = 0;
                DecResponsesAndFinishIfReadyL();
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// Observer callback for upnp utilities
// Called when wlan connection is lost
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::ConnectionLost()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::\
                             ConnectionLost"));
    iHarvestActive = EFalse;
    if( iMetadataCollector )
        {
        iMetadataCollector->CancelHarvest();
        }
    TRAP_IGNORE( iMdhServer.SetServerStateL( ECmMdhServerStateIdle, 
                                             KErrDisconnected ) );
    }

// ---------------------------------------------------------------------------
// CCmMdhMediaserverContainer::AddHashValueForResponseL
// ---------------------------------------------------------------------------        
TInt CCmMdhMediaserverContainer::AddHashValueForResponseL(
                                                          TInt aMediaServerId, 
                                                          TInt aSearchIndex, 
                                                          TInt aItemCount, 
                                                          const TDesC8& aHash ) 
    {
    CCmSearchResponseHash* newHash = CCmSearchResponseHash::NewL( aSearchIndex,
                                                                  aItemCount,
                                                                  aHash );
    CleanupStack::PushL( newHash );
    TInt errCode = iDbManager->AddHashValue( aMediaServerId, newHash );
    CleanupStack::PopAndDestroy( newHash );  
    return errCode;                                                                
    }
    
// ---------------------------------------------------------------------------
// Gets media server hash values
// ---------------------------------------------------------------------------
void CCmMdhMediaserverContainer::HashValuesForMediaServerL( TInt aMediaServerId,
    RPointerArray<CCmSearchResponseHash>& aHashValues )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::HashValuesForMediaServerL"));

    iDbManager->PrepareQueryCmdL( EHashValueQuery );
    
    TInt err = iDbManager->QueryHashValuesL( aMediaServerId, aHashValues );

    return;
    }

// ---------------------------------------------------------------------------
// Deletes old hash values
// ---------------------------------------------------------------------------    
void CCmMdhMediaserverContainer::DeleteOldHashValuesL( const TInt aMediaserverId,
    const TInt aSearchIndex )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhMediaserverContainer::DeleteOldHashValuesL"));
    iDbManager->DeleteHashValuesL( aMediaserverId, aSearchIndex );
    }
    
// End of file

    
