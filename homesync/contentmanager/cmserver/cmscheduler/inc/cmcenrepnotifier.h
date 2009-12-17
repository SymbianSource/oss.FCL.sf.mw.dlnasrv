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
* Description:  central repository notifier class definition
*
*/


#ifndef _CCMCENREPNOTIFIER_H
#define _CCMCENREPNOTIFIER_H

//  Include Files
#include <d32dbms.h>

// FORWARD DECLARATIONS
class CRepository;
class CCmScheduler;


// CLASS DECLARATION

/**
 *  Comms database notifier class for scheduler
 *  @lib cmscheduler.lib
 *  @since S60 5.1
 */
class CCmCenrepNotifier : public CActive
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCmCenrepNotifier* NewL( CCmScheduler& aSheduler );

        /**
         * Destructor.
         */
        virtual ~CCmCenrepNotifier();

        /**
         * Reads current profile from central repository
         *
         * @since S60 5.1
         * @param aProfile, reference of current profile
         * @return TInt, error code
         */
        TInt GetCurrentProfile( TInt& aProfile );

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
        CCmCenrepNotifier( CCmScheduler& aSheduler );
		
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
        CRepository*                    iRepository;    // owned
		
        /**
         * Change observer
         */
        CCmScheduler&                   iScheduler;     // not owned
    };

#endif //  _CCMCENREPNOTIFIER_H