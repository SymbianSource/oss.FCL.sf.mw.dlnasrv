/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the CUPnPAIWTimer class.
*
*/


#ifndef UPNP_AIW_TIMER_H
#define UPNP_AIW_TIMER_H

//  INCLUDES
#include <e32base.h>
#include <e32cons.h>
#include <coecntrl.h>
#include <AknUtils.h>
#include "upnpaiwtimerobserver.h"

// CLASS DECLARATION

/**
* CUPnPAiWTimer class.
*/
class CUPnPAiwTimer : public CBase,
                      public MBeating
    {

    public:

        /**
         * Static NewL method to construct the timer.
         *
         * @since S60 3.0
         * @param aInterval (TInt) Timer interval in seconds
         * @param aCallback (MUPnPAiwTimerObserver*) call back reference 
         * @return CUPnPAVTimer* timer instance
         */
        static CUPnPAiwTimer* NewL( TInt aInterval,
                                    MUPnPAiwTimerObserver* aCallback );

        // Destructor
        virtual ~CUPnPAiwTimer();

    private:

        /**
         * ConstructL.
         *
         * @since S60 3.0
         * @param aInterval (TInt) Timer interval in seconds
         * @param aCallback (MUPnPAiwTimerObserver*) call back reference 
        */
        void ConstructL( TInt aInterval,
                         MUPnPAiwTimerObserver* aCallback );

        /**
         * Constructor.
         *
         * @since S60 3.0
        */
        CUPnPAiwTimer();

    private: // for MBeating

        /**
         * Called when the beat is in sync.
         *
         * @since S60 3.0
         */
        void Beat();

        /**
         * Called when the beat needs to be syncronized.
         *
         * @since S60 3.0
         */
        void Synchronize();

    private: // Data members

        // Heartbeat active object
        CHeartbeat* iHeartbeat;                     // owned

        // Call back pointer
        MUPnPAiwTimerObserver *iCallback;           // not owned

        // Interval in seconds
        TInt iInterval;

        // Heartbeat counter
        TInt iCounter;

    };

#endif // UPNP_AIW_TIMER_H

// End of file
