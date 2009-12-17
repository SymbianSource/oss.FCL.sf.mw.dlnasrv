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
* Description:  Comms database notifier class definition
*
*/


#ifndef _CCMCOMMSDBNOTIFIER_H
#define _CCMCOMMSDBNOTIFIER_H

//  Include Files
#include <d32dbms.h>

// FORWARD DECLARATIONS
class CCommsDatabase;
class CCmScheduler;

// CLASS DECLARATION

/**
 *  Comms database notifier class for scheduler
 *  @lib cmscheduler.lib
 *  @since S60 5.1
 */
class CCmCommsDbNotifier : public CActive
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCmCommsDbNotifier* NewL( CCmScheduler& aSheduler );

        /**
         * Destructor.
         */
        virtual ~CCmCommsDbNotifier();

        /**
         * Checks wlan scan state
         *
         * @since S60 5.1
         * @return TBool, ETrue if value changed, EFalse otherwise
         */
        TBool HasScanStateChangedL();

        /**
         * Gets wlan scan interval setting value
         *
         * @since S60 5.1
         * @return TUint32, scan interval
         */
        TUint32 WlanScanStateL();

    protected: // From CActive
	
        /**
         * Called when request completes
         *
         * @since S60 5.1
         */
        void RunL();
		
        /**
         * Called when request is cancelled
         *
         * @since S60 5.1
         */
        void DoCancel();
		
        /**
         * Called when RunL leaves
         *
         * @since S60 5.1
         */
        TInt RunError(TInt aError);

    private: // Constructors

        /**
         * Default constructor.
         */
        CCmCommsDbNotifier( CCmScheduler& aSheduler );
		
        /**
         * Two-phased constructor.
         */
        void ConstructL();

        /**
         * Initiates asyncronous notification request to database
         *
         * @since S60 5.1
         */
        void RequestNotification();

    private:

        /**
         * Database to be monitored
         */
        CCommsDatabase*                 iDb;    // owned
		
        /**
         * Change observer
         */
        CCmScheduler*                   iScheduler;  // not owned
		
        /**
         * Previous scan state
         */
        TUint32                         iScanState;
    };

#endif //  _CCMCOMMSDBNOTIFIER_H