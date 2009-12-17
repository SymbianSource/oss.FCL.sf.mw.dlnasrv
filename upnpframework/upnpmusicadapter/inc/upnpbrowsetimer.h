/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Prodes timer services
*
*/


#ifndef __UPNPBROWSETIMER_H__
#define __UPNPBROWSETIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MUPnPBrowseTimerObserver;

/**
 * A class that provides periodic notifications.
 *
 * Once started, notifies an abstract interface periodically
 * but only when Continue is called. This avoids situation
 * where previous operation is still in progress when the next
 * notification arrives.
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.2
 */
class CUPnPBrowseTimer : public CTimer
    {

public:

   /**
    * Static constructor
    *
    * @since Series 60 3.2
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    static CUPnPBrowseTimer* NewL(
        MUPnPBrowseTimerObserver& aObserver,
        TInt32 aTimerWavelength );
    
   /**
    * Static constructor
    *
    * @since Series 60 3.2
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    static CUPnPBrowseTimer* NewLC(
        MUPnPBrowseTimerObserver& aObserver,
        TInt32 aTimerWavelength );

    /**
     * Destructor
     *
     * @since Series 60 3.2
     */ 
    virtual ~CUPnPBrowseTimer();

private:

   /**
    * Constructor
    *
    * @since Series 60 3.2
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    CUPnPBrowseTimer(
        MUPnPBrowseTimerObserver& aObserver,
        TInt32 aTimerWavelength );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.2
     */   
    void ConstructL();

public:

    /**
     * Starts the periodizer
     *
     * @since Series 60 3.2
     */
    void Start();

    /**
     * Continues a next step for the periodizer
     *
     * @since Series 60 3.2
     */
    void Continue();

    /**
     * Stops periodizer
     *
     * @since Series 60 3.2
     */
    void Stop();

protected: // personal method

    /**
     * Receives the timer triggering
     *
     * @since Series 60 3.2
     */
    void RunL();


private: // data

    /**
     * Observer of the timer
     */
    MUPnPBrowseTimerObserver& iObserver;

    /**
     * Timer wavelength as milliseconds to wait between notifications
     */
    TInt32 iTimerWavelength;

    };

/**
 * The interface to receive timer notifications
 */
class MUPnPBrowseTimerObserver
    {

public:

   /**
    * Timer has triggered
    *
    * @since Series 60 3.2
    */
    virtual void HandlePeriod() = 0;

    };


#endif // __UPNPBROWSETIMER_H__


