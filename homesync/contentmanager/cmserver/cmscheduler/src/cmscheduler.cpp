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
* Description:  Content Manager server's scheduler component
*
*/


// INCLUDE FILES
#include <apgtask.h>

#include "cmsettings.h"
#include "cmcommonutils.h"
#include "cmscheduler.h"
#include "mediaservantuid.h"
#include "cmcommsdbnotifier.h"
#include "cmcenrepnotifier.h"
#include "msdebug.h"

// WLAN scanning interval [s]
const TInt
    KTimerInterval = 30 * 60 * 1000 * 1000; // 30 minutes in microseconds
const TInt KTimerCount = 2; // KTimerCount*KTimerInterval = 1 hour
const TUid KMediaServantUid = { KMediaServantUID3 }; // application uid
const TInt KOfflineProfile = 5; // offline profile identifier

// ========================== MEMBER FUNCTIONS ===============================

// Two-phased constructor.
EXPORT_C CCmScheduler* CCmScheduler::NewL(
    MCmServiceObserver& aServer, MCmSettings& aSettings)
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::NewL"));
    CCmScheduler* self = new ( ELeave )
        CCmScheduler( aServer, aSettings );
    self->ConstructL();
    return self;
    }

// Destructor
CCmScheduler::~CCmScheduler()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::~CCmScheduler"));

    Cancel();
    delete iNotifier;
    delete iRepositoryNotifier;
    iConnMon.CancelNotifications();
    iConnMon.Close();
    iTimer.Close();
    }

// --------------------------------------------------------------------------
// CCmScheduler::CCmScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CCmScheduler::CCmScheduler(
    MCmServiceObserver& aServer, MCmSettings& aSettings)
    : CActive( EPriorityStandard ),
    iServer( aServer ), iSettings( aSettings ),
    iScheduledServiceRunning( EFalse ), iEnabled( ETrue ),
    iState( ECmScStateOffline ), iTimerCount( 0 ),
    iServiceToExecute( ECmServicePreProcessingStore ),
    iExecuteHarvest( ETrue ), iSchedulerInitiated( EFalse )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::CCmScheduler()"));
    }

// --------------------------------------------------------------------------
// CCmScheduler::ConstructL()
// --------------------------------------------------------------------------
//
void CCmScheduler::ConstructL()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::ConstructL"));

    // Ensure that iap is set. Otherwise scheduler cannot be used
#ifdef __DEBUG
    TInt iap( 0 );
    User::LeaveIfError( iSettings.GetIapL( iap ) );
    TRACE( Print( _L("[Cm Scheduler]\t IAP to use is  %d"), iap));
#endif

    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    iEnabled = EFalse;

    iRepositoryNotifier = CCmCenrepNotifier::NewL( *this );
    User::LeaveIfError( iRepositoryNotifier->GetCurrentProfile(
                        iCurrentProfile ) );

    // create commsdb notifier
    TRAPD( error, iNotifier = CCmCommsDbNotifier::NewL( *this ) );

    if ( !error )
        {
        iWlanScanState = iNotifier->WlanScanStateL();
        }
    else
        {
        TRACE( Print( _L("[Cm Scheduler]\t CCmScheduler::ConstructL \
        error = %d"), error));
        }

    // check that mc is not in offline mode
    if ( iCurrentProfile != KOfflineProfile )
        {
        iState = ECmScStateOnline;
        if ( error == KErrNone )
            {
            // We assume that scan is off.
            // State is checked and if it is on wlan events can be used.
            if ( iNotifier->HasScanStateChangedL() )
                {
                LOG(_L("[Cm Scheduler]\t CCmScheduler::ConstructL() \
                    Scanning state changed on"));
                // scanning is on
                SetEnabledL( ETrue );
                }

            else
                {
                LOG(_L("[Cm Scheduler]\t CCmScheduler::ConstructL() \
                    Scanning state is off - starting timer"));
                // scanning is off - use timer
                TryToStartRetryTimer();
                }
            }
        else
            {
            LOG(_L("[Cm Scheduler]\t CCmScheduler::ConstructL() \
                Notifier creation failed"));
            // if notifier cannot be created we start the timer
            TryToStartRetryTimer();
            }
        }
    else
        {
        LOG(_L("[Cm Scheduler]\t CCmScheduler::ConstructL() \
            profile = offline"));
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::EventL()
// Called when an event arrives to connection monitor
// --------------------------------------------------------------------------
//
void CCmScheduler::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    TRACE(Print(_L("[Cm Scheduler]\t CCmScheduler::EventL() \
        eventtype = %d, connection id = %d"),
        aConnMonEvent.EventType(), aConnMonEvent.ConnectionId()));

    TTime currentTime;
    currentTime.HomeTime();

    TTimeIntervalHours interval;
    currentTime.HoursFrom( iLastSyncTime, interval );

    TRACE(Print(_L("[Cm Scheduler]\t CCmScheduler::EventL() \
	    hours from last sync = %d"), interval.Int()));

    if ( interval.Int() >= 1 && !ApplicationRunning() )
        {
        HandleConnectionEventL( aConnMonEvent );
        }

    }

// --------------------------------------------------------------------------
// CCmScheduler::HandleConnectionEventL()
// Handles connection event
// --------------------------------------------------------------------------
//
void CCmScheduler::HandleConnectionEventL(
                            const CConnMonEventBase& aConnMonEvent )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::HandleConnectionEventL() \
	    HandleConnectionEventL"));

    if ( aConnMonEvent.EventType() == EConnMonIapAvailabilityChange )
        {
        LOG(_L("[Cm Scheduler]\t CCmScheduler::HandleConnectionEventL() \
            EConnMonIapAvailabilityChange"));

        TCmSchedulerState newState = ECmScStateOffline;

        if ( CheckIapAvailabilityL( aConnMonEvent ) )
            {
            newState = ECmScStateOnline;
            }

        if ( newState != iState )
            {
            iState = newState;

            if ( newState && !iScheduledServiceRunning )
                {
                LOG(_L("[Cm Scheduler]\t State changed to online,\
                    starting operation.."));
                iSchedulerInitiated = ETrue;// do also harvest
                TRACE( Print( _L("[Cm Scheduler]\t HandleConnectionEventL \
                    iSchedulerInitiated = %d"), iSchedulerInitiated));
                iExecuteHarvest = ETrue;
                TRACE( Print( _L("[Cm Scheduler]\t HandleConnectionEventL \
                    iExecuteHarvest = %d"), iExecuteHarvest));
                TryToExecuteService( iServiceToExecute );
                }
            else
                {
                LOG(_L("[Cm Scheduler]\t State changed to offline,\
                    canceling timer.."));
                CancelTimer();
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::RunL()
// Called when the timer elapses
// --------------------------------------------------------------------------
//
void CCmScheduler::RunL()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::RunL()"));
    TRACE( Print( _L("[Cm Scheduler]\t status %d"), iStatus.Int() ));
    TRACE( Print( _L("[Cm Scheduler]\t state %d"), iState ));

    if ( KErrNone == iStatus.Int() &&
         iState == ECmScStateOnline &&
         !iScheduledServiceRunning )
        {
        if ( ++iTimerCount >= KTimerCount && !ApplicationRunning() )
            {
            LOG(_L("[Cm Scheduler]\t Online timer passed, \
                starting operation.."));
            iSchedulerInitiated = ETrue;// do also harvest
            TRACE( Print( _L("[Cm Scheduler]\t RunL \
                iSchedulerInitiated = %d"), iSchedulerInitiated));
            iExecuteHarvest = ETrue;
            TRACE( Print( _L("[Cm Scheduler]\t RunL \
                iExecuteHarvest = %d"), iExecuteHarvest));
            TryToExecuteService( iServiceToExecute );
            }
        else
            {
            iTimer.After( iStatus,
                TTimeIntervalMicroSeconds32( KTimerInterval ) );
            SetActive();
            }
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::TryToExecuteService()
// Starts process if application is not running
// --------------------------------------------------------------------------
//
void CCmScheduler::TryToExecuteService( TCmService aService )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::TryToExecuteService()"));
    TInt err = KErrNone;

    TRAP( err, iServer.ExecuteServiceL( aService ));
    iScheduledServiceRunning = !err;

    if ( !err && aService == ECmServicePreProcessingStore )
        {
        iStorePreprocessed = EFalse;
        }

    TRACE( Print( _L("[Cm Scheduler]\t ExecuteServiceL err: %d"), err));

    if ( err )
        {
        TryToStartRetryTimer();
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::TryToStartRetryTimer()
// Starts timer if not already active
// --------------------------------------------------------------------------
//
void CCmScheduler::TryToStartRetryTimer()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::TryToStartRetryTimer()"));

    if ( !IsActive() )
        {
        LOG(_L("[Cm Scheduler]\t starting one hour resync timer.."));

        CancelTimer();

        iTimer.After(
                iStatus, TTimeIntervalMicroSeconds32( KTimerInterval ));
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::CancelTimer()
// Cancels timer
// --------------------------------------------------------------------------
//
void CCmScheduler::CancelTimer()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::CancelTimer"));

    iTimer.Cancel();
    iTimerCount = 0;
    }

// --------------------------------------------------------------------------
// CCmScheduler::SetEnabledL()
// Enabled/disabled connection monitor events
// --------------------------------------------------------------------------
//
EXPORT_C void CCmScheduler::SetEnabledL( TBool aEnable )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::SetEnabled()"));
    if ( iEnabled != aEnable )
        {
        iEnabled = aEnable;
        if ( iEnabled )
            {
            LOG(_L("[Cm Scheduler]\t enabling scheduler.."));
            LOG(_L("[Cm Scheduler]\t starting to listen wlan events.."));
            User::LeaveIfError( iConnMon.ConnectL() );
            iConnMon.NotifyEventL( *this );

            }
         else // iConnMon is connected when entering here...
            {
            LOG(_L("[Cm Scheduler]\t disabling scheduler.."));

            iConnMon.CancelNotifications();
            iConnMon.Close();

            }
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::SetServiceToExecute
// Sets service to be executed
// --------------------------------------------------------------------------
//
EXPORT_C void CCmScheduler::SetServiceToExecute( TCmService aService )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::SetServiceToExecute()"));
    iServiceToExecute = aService;
    }

// --------------------------------------------------------------------------
// CCmScheduler::ServiceExecuted
// Called when service is ready
// --------------------------------------------------------------------------
//
EXPORT_C void CCmScheduler::ServiceExecuted( TCmService aService, TInt aErr )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::ServiceExecuted()"));
    TRACE( Print( _L("[Cm Scheduler]\t service %d err %d"),
        aService, aErr));

    if ( !iScheduledServiceRunning )
        {
        LOG(_L("[Cm Scheduler]\t No scheduled service running, returning"));
        return;
        }

    iScheduledServiceRunning = EFalse;
    TBool startRetryTimer( EFalse );
    if ( aErr )
        {
        startRetryTimer = ETrue;
        }
    else
        {
        switch ( aService )
            {
            case ECmServicePreProcessingStore:
                {
                if ( iStorePreprocessed == EFalse )
                    {
                    iStorePreprocessed = ETrue;
                    TryToExecuteService( ECmServiceStore );
                    }
                else
                    {
                    iLastSyncTime.HomeTime();
                    startRetryTimer = ETrue;
                    iSchedulerInitiated = EFalse;
                    }
                break;
                }
            case ECmServiceStore:
                {
                // execute fill
                TryToExecuteService( ECmServiceFill );
                break;
                }

            case ECmServiceFill:
                {
                if ( iExecuteHarvest )
                    {
                    TRACE( Print( _L("[Cm Scheduler]\t serviceexecuted \
                        harvest %d schedulerinitiated %d"),
                        iExecuteHarvest, iSchedulerInitiated));
                    // execute harvest
                    TryToExecuteService( ECmServiceHarvest );
                    }
                else
                    {
                    LOG(_L("[Cm Scheduler]\t CCmScheduler::ServiceExecuted \
                        ExecuteHarvest = EFalse"));
                    startRetryTimer = ETrue;
                    iLastSyncTime.HomeTime();
                    iSchedulerInitiated = EFalse;
                    TRACE( Print( _L("[Cm Scheduler]\t ServiceExecuted \
                         iSchedulerInitiated = %d"), iSchedulerInitiated));
                    }
                break;
                }

            case ECmServiceHarvest:
            // fall through
            case ECmServicePreProcessingFill:
                {
                iSchedulerInitiated = EFalse;
                TRACE( Print( _L("[Cm Scheduler]\t ServiceExecuted \
                    iSchedulerInitiated = %d"), iSchedulerInitiated));
                startRetryTimer = ETrue;
                iLastSyncTime.HomeTime();
                break;
                }

            default:
                {
                LOG(_L("[Cm Scheduler]\t default"));
                break;
                }
            }

        }
    if ( startRetryTimer )
        {
        TryToStartRetryTimer();
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::DoCancel
// Called by framework when timer is cancelled
// --------------------------------------------------------------------------
//
void CCmScheduler::DoCancel()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::DoCancel()"));

    CancelTimer();
    }

// --------------------------------------------------------------------------
// CCmScheduler::WlanScanStateChanged
// starts/disables timer and connection monitor events depending on state
// of wlan scanning interval
// --------------------------------------------------------------------------
//
void CCmScheduler::WlanScanStateChanged( TInt aState )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::WlanScanStateChanged()"));

    iWlanScanState = aState;

    if ( iCurrentProfile != KOfflineProfile )
        {
        if ( aState == KWlanScanNetworkNever )
            {
            TRAP_IGNORE( SetEnabledL( EFalse ) );
            // state is set online so we can try start process
            iState = ECmScStateOnline;
            TryToStartRetryTimer();
            }
        else
            {
            // cancel timer
            CancelTimer();

            TRAP_IGNORE( SetEnabledL( ETrue ) );
            }
        }
    }


// --------------------------------------------------------------------------
// CCmScheduler::ProfileChangedL
// --------------------------------------------------------------------------
//
void CCmScheduler::ProfileChangedL( TInt aProfile )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::ProfileChangedL()"));

    if ( aProfile == KOfflineProfile )
        {
        LOG(_L("[Cm Scheduler]\t CCmScheduler::ProfileChangedL() \
            offline"));

        iCurrentProfile = aProfile;

        // disable wlan scanning
        CmCommonUtils::SetWlanScanL( KWlanScanNetworkNever );

        // stop receiving events
        SetEnabledL( EFalse );
        // cancel timer
        CancelTimer();
        }
    else if ( iCurrentProfile == KOfflineProfile )
        {
        LOG(_L("[Cm Scheduler]\t CCmScheduler::ProfileChangedL() \
            online"));

        iCurrentProfile = aProfile;

        // enable wlan scanning
        CmCommonUtils::SetWlanScanL( KWlanScanNetworkInterval60 );

        if ( iWlanScanState )
            {
            SetEnabledL( ETrue );
            }
        else
            {
            TryToStartRetryTimer();
            }
        }
    }

// --------------------------------------------------------------------------
// CCmScheduler::SetScheduledServiceState
// --------------------------------------------------------------------------
//
EXPORT_C void CCmScheduler::SetScheduledServiceState( TBool aState )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::SetScheduledServiceState()"));

    iScheduledServiceRunning = aState;
    // we don't want to make second round
    iStorePreprocessed = ETrue;
    // This function is called when application transfers responsibility
    // to scheduler. In this case we don't want to do harvest.
    if ( !iSchedulerInitiated )
        {
        LOG(_L("[Cm Scheduler]\t CCmScheduler::SetScheduledServiceState() \
            iSchedulerInitiated = EFalse"));
        iExecuteHarvest = EFalse;
        TRACE( Print( _L("[Cm Scheduler]\t SetScheduledServiceState \
            iExecuteHarvest = %d"), iExecuteHarvest));
        }

    }

// --------------------------------------------------------------------------
// CCmScheduler::RunError
// --------------------------------------------------------------------------
//
TInt CCmScheduler::RunError( TInt aError )
    {
    TRACE( Print( _L("[Cm Scheduler]\t CCmScheduler::RunError \
        error = %d"), aError));

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CCmScheduler::ApplicationRunning
// --------------------------------------------------------------------------
//
TBool CCmScheduler::ApplicationRunning()
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::ApplicationRunning()"));

    TBool mediaServantRunning( EFalse );
    RWsSession wsSession;
    TInt err = wsSession.Connect();
    if ( !err )
        {
        TApaTaskList taskList( wsSession );
        mediaServantRunning = taskList.FindApp( KMediaServantUid ).Exists();
        wsSession.Close();
        }
    return mediaServantRunning;
    }

// --------------------------------------------------------------------------
// CCmScheduler::CheckIapAvailabilityL()
// --------------------------------------------------------------------------
//
TBool CCmScheduler::CheckIapAvailabilityL(
                            const CConnMonEventBase& aConnMonEvent )
    {
    LOG(_L("[Cm Scheduler]\t CCmScheduler::CheckIapAvailabilityL()"));

    TBool iapFound( EFalse );
    TInt accessPoint( -1 );
    User::LeaveIfError( iSettings.GetIapL( accessPoint ) );
    TRACE( Print( _L("[Cm Scheduler]\t \
        IAP to use is  %d"), accessPoint));

    CConnMonIapAvailabilityChange* eventIap =
                    ( CConnMonIapAvailabilityChange* ) &aConnMonEvent;

    TConnMonIapInfo iaps = eventIap->IapAvailability();

    for ( TInt i = 0; i < iaps.iCount; i++ )
        {
        // Compare available IAPs to our IAP
        TRACE( Print( _L("[Cm Scheduler]\t CONNMON iap: %d"),
            iaps.iIap[i].iIapId));
        if ( accessPoint == iaps.iIap[i].iIapId )
            {
            LOG(_L("[Cm Scheduler]\t FOUND CORRECT IAP!"));
            iapFound = ETrue;
            i = iaps.iCount;
            }
        }
    return iapFound;
    }

// End of File
