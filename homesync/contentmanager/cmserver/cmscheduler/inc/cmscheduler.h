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
* Description:  Main class in the CM Scheduler component
*
*/


#ifndef _CMSCHEDULER_H
#define _CMSCHEDULER_H

#include <e32base.h>
#include <rconnmon.h>
#include "cmserver.h"
#include "cmsettings.h"

/* Forward declarations. */
class CCmCommsDbNotifier;
class CCmCenrepNotifier;
class CCmSettingsEngine;
class MCmSettings;

/**
 *  CCmScheduler class
 *  Part of CM Scheduler
 *  @lib cmscheduler.lib
 *  @since S60 v5.1
 */
class CCmScheduler : public CActive,
                     public MConnectionMonitorObserver
    {
    private: // Enumerations

    // Scheduler states
    enum TCmSchedulerState
        {
        ECmScStateOffline = 0,  // Don't start processing
        ECmScStateOnline        // Start processing
        };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCmScheduler* NewL( MCmServiceObserver& aServer,
                                   MCmSettings& aSettings );

        /**
         * Destructor.
         */
        virtual ~CCmScheduler();

        /**
         * Enabled connection monitor events
         *
         * @since S60 5.1
         * @param aEnable, ETrue if enabled, EFalse otherwise
         */
        void SetEnabledL( TBool aEnable );

        /**
         * Called when service is executed
         *
         * @since S60 5.1
         * @param aService, executed service
         * @param aErr, error code
         */
        IMPORT_C void ServiceExecuted( TCmService aService, TInt aErr );

        /**
         * Sets service where execution starts
         *
         * @since S60 5.1
         * @param aService, service to be executed
         */
        IMPORT_C void SetServiceToExecute( TCmService aService );

        /**
         * Called by commsdb notifier when wlan scan state is changed
         *
         * @since S60 5.1
         * @param aState, Scan interval
         */
        void WlanScanStateChanged( TInt aState );

        /**
         * Called by cenrep notifier when profile has been changed
         *
         * @since S60 5.1
         * @param aProfile, current profile
         */
        void ProfileChangedL( TInt aProfile );

        /**
         * Sets scheduled service state
         *
         * @since S60 5.1
         * @param TBool, ETrue = running
         *               EFalse = not running
         */
        IMPORT_C void SetScheduledServiceState( TBool aState );


    protected:

    // From base class MConnectionMonitorObserver

        /**
         * From MConnectionMonitorObserver
         * see base class definition
         */
        void EventL( const CConnMonEventBase &aConnMonEvent );

     // From base class CActive

        /**
         * From CActive
         * see base class definition
         */
        void RunL();

        /**
         * From CActive
         * see base class definition
         */
        void DoCancel();

        /**
         * From CActive
         * see base class definition
         */
        TInt RunError( TInt aError );

    private:

        /**
         * Tries to execute service
         *
         * @since S60 5.1
         * @param aService, service to be executed
         */
        void TryToExecuteService( TCmService aService );

        /**
         * Tries to start timer
         *
         * @since S60 5.1
         */
        void TryToStartRetryTimer();

        /**
         * Cancels timer
         *
         * @since S60 5.1
         */
        void CancelTimer();

        /**
         * Handles connection event
         *
         * @since S60 5.1
         * @param aConnMonEvent The event information.
         */
        void HandleConnectionEventL(
                    const CConnMonEventBase& aConnMonEvent );

        /**
         * Checks if client application is running
         *
         * @since S60 5.1
         * @return TBool, ETrue if running, EFalse otherwise
         */
        TBool ApplicationRunning();

        /**
         * Checks if iap is available
         *
         * @since S60 5.1
         * @param aConnMonEvent The event information.
         * @return TBool, ETrue if iap is available, EFalse otherwise
         */
        TBool CheckIapAvailabilityL(
                        const CConnMonEventBase& aConnMonEvent );

	private:

        /**
         * C++ default constructor.
         */
        CCmScheduler( MCmServiceObserver& aServer,
                      MCmSettings& aSettings );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:

        /**
         * Service observer
         */
        MCmServiceObserver&                     iServer;
		
        /**
         * Settings handler
         */
        MCmSettings&                            iSettings;
		
        /**
         * Service state
         */
        TBool                                   iScheduledServiceRunning;
		
        /**
         * Connection monitor state
         */
        TBool                                   iEnabled;
		
        /**
         * Scheduler state
         */
        TCmSchedulerState                       iState;
		
        /**
         * Timer counter
         */
        TInt                                    iTimerCount;
		
        /**
         * Service to be executed
         */
        TCmService                              iServiceToExecute;
		
        /**
         * Harvest execution flag
         */
        TBool                                   iExecuteHarvest;
		
        /**
         * Scheduler started operation
         */
        TBool                                   iSchedulerInitiated;
		
        /**
         * Connection monitor
         */
        RConnectionMonitor                      iConnMon;
		
        /**
         * Timer for service execution
         */
        RTimer                                  iTimer;
		
        /**
         * Comms database notifier
         */
        CCmCommsDbNotifier*                     iNotifier;           // owned
		
        /**
         * Cenral repository notifier
         */
        CCmCenrepNotifier*                      iRepositoryNotifier; // owned
		
        /**
         * Current profile
         */
        TInt                                    iCurrentProfile;
		
        /**
         * Wlan scan state
         */
        TInt                                    iWlanScanState;
		
        /**
         * Store preprocessing flag
         */
        TBool                                   iStorePreprocessed;
		
        /**
         * Last sync time
         */
        TTime                                   iLastSyncTime;
    };


#endif //  _CMSCHEDULER_H
