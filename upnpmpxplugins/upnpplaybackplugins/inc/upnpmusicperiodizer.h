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
* Description:      Periodic timer support resource
*
*/






#ifndef __UPNPMUSICPERIODIZER_H__
#define __UPNPMUSICPERIODIZER_H__

// INCLUDES
#include <e32base.h>
#include "upnpmusicperiodizer.h"

// FORWARD DECLARATIONS
class MUPnPMusicPeriodizerObserver;

/**
 * A class that provides periodic notifications.
 *
 * Once started, notifies an abstract interface periodically
 * but only when Continue is called. This avoids situation
 * where previous operation is still in progress when the next
 * notification arrives.
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPMusicPeriodizer : public CTimer
    {

public:

   /**
    * Static constructor
    *
    * @since Series 60 3.1
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    static CUPnPMusicPeriodizer* NewL(
        MUPnPMusicPeriodizerObserver& aObserver,
        TInt32 aTimerWavelength );
    
   /**
    * Static constructor
    *
    * @since Series 60 3.1
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    static CUPnPMusicPeriodizer* NewLC(
        MUPnPMusicPeriodizerObserver& aObserver,
        TInt32 aTimerWavelength );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */ 
    virtual ~CUPnPMusicPeriodizer();

private:

   /**
    * Constructor
    *
    * @since Series 60 3.1
    * @param aObserver the party to be notified on timer trigger
    * @param aTimerWavelength the timer delay in microseconds
    */
    CUPnPMusicPeriodizer(
        MUPnPMusicPeriodizerObserver& aObserver,
        TInt32 aTimerWavelength );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */   
    void ConstructL();

public:

    /**
     * Starts the periodizer
     *
     * @since Series 60 3.1
     */
    void Start();

    /**
     * Continues a next step for the periodizer
     *
     * @since Series 60 3.1
     */
    void Continue();

    /**
     * Stops periodizer
     *
     * @since Series 60 3.1
     */
    void Stop();

protected: // personal method

    /**
     * Receives the timer triggering
     *
     * @since Series 60 3.1
     */
    void RunL();


private: // data

    /**
     * Observer of the timer
     */
    MUPnPMusicPeriodizerObserver& iObserver;

    /**
     * Timer wavelength as milliseconds to wait between notifications
     */
    TInt32 iTimerWavelength;

    };

/**
 * The interface to receive timer notifications
 */
class MUPnPMusicPeriodizerObserver
    {

public:

   /**
    * Timer has triggered
    *
    * @since Series 60 3.1
    */
    virtual void HandlePeriod() = 0;
    
   /**
    * Timer has triggered
    *
    * @since Series 60 3.2.3
    */
    virtual void HandlePeriodForEnd(){}

    };


#endif // __UPNPMUSICPERIODIZER_H__

