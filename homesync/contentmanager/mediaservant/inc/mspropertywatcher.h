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
* Description:  CMSPropertyWatcher class defition
*
*/



#ifndef __MSPROPERTYWATCHER_H__
#define __MSPROPERTYWATCHER_H__


// Include Files
#include <e32base.h>
#include <e32property.h>
#include "cmcommon.h"

// FORWARD DECLARATIONS
class MMSPropertyObserver;

// CLASS DECLARATION
/**
 * Active object class for MSEngine
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSPropertyWatcher ): public CActive
    {

    public:

        /**
         * Two-phased constructor.
         */
        static CMSPropertyWatcher* NewL();

        /**
         * Two-phased constructor.
         */
        static CMSPropertyWatcher* NewLC();        

        /**
        * Destructor.
        */
        virtual ~CMSPropertyWatcher();

        /**
         * Sets property observer
         * @since S60 5.1
         * @param aObserver, property observer
         * @return None
         */
        void SetObserver( MMSPropertyObserver* aObserver );

        /**
         * Starts listening property
         * @since S60 5.1
         * @param None
         * @return None
         */
        void StartL();

        /**
         * Stops listening property
         * @since S60 5.1
         * @param None
         * @return None
         */
        void Stop();

    protected:

// From base class CActive

        /**
         * From CActive
         * See base class definition
         */
        virtual void RunL();

        /**
         * From CActive
         * See base class definition
         */
        virtual void DoCancel();

        /**
         * From CActive
         * See base class definition
         */
        TInt RunError( TInt aError );  
        
    private:

        /**
         * Performs the first phase of two phase construction.
         */
        CMSPropertyWatcher();

        /**
         * Performs the second phase construction.
         */
        void ConstructL();


    private:

        /**
         * property
         */
        RProperty                       iProgressInfo;
        /**
         * property observer
         */
        MMSPropertyObserver*            iObserver;      // not owned

    };




/**
 *  Property observer
 *  @since Series 60 3.1
 */
class MMSPropertyObserver
    {
    public:
 
    /**
     * Informs about property changes
     * @param aService, completed service
     * @param aError, error code
     * @since S60 5.1
     */
     virtual void PropertyChangedL( TCmService aService,
                                    TInt aTotalItems,
                                    TInt aProcessedItems,
                                    TInt aItemsToBeProcessed ) = 0;
    };

#endif  // __MSPROPERTYWATCHER_H__

// End of file
