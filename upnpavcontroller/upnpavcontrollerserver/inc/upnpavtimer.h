/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      provides timing services for AVController server
*
*/






#ifndef C_UPNPAVTIMER_H_
#define C_UPNPAVTIMER_H_

// Include files
#include <e32base.h>

const TInt KTimerCycle3         = 3;
const TInt KTimerCycle10        = 10;
const TInt KTimerCycle30        = 30;
const TInt KTimerCycle300       = 300;

// Forward declarations
class CUPnPBrowsingSession;
class MUPnPAVTimerCallback;

/**
 * Timer class. Used as a server (AVC and S60 Media Server) shutdown timer.
 *
 */
class CUPnPAVTimer: public CTimer
    {

public:

    /**
     * Timer type defination
     */    
    enum TAVTimerType
        {
        ETimerServerShutdown = 0,
        ETimerMediaServer
        };
 
public:

    /**
     * Static 1st phase constructor
     * @param aObserver Observer
     * @param aType timer type
     * @return A new timer instance
     */    
    static CUPnPAVTimer* NewL( MUPnPAVTimerCallback& aObserver,
        TAVTimerType aType );

    /**
     * Destructor
     */    
    virtual ~CUPnPAVTimer();
    
    
private:

    /**
     * Private constructor
     * @param aObserver Observer
     * @param aType timer type
     */
    CUPnPAVTimer( MUPnPAVTimerCallback& aObserver, TAVTimerType aType );
    
    /**
     * 2nd phase construct
     */
    void ConstructL();
    
protected: // From CTimer (CActive)

    /**
     * See e32base.h
     */
    void RunL();

public: // New functions

    /**
     * Starts the timer
     * @param aType Type of timing interval
     */
    void Start( TInt aIntervalInSecs );
        
private:
    
    MUPnPAVTimerCallback& iObserver; // Not Own
    
    TAVTimerType iTimerType;
    
    };

/**
 * Timer observer
 *
 */
class MUPnPAVTimerCallback
    {
public:

    /**
     * Called when timer expires
     *
     * @param aType time-out type
     */
    virtual void UPnPAVTimerCallback( CUPnPAVTimer::TAVTimerType aType ) = 0;
    
    };

#endif // C_UPNPAVTIMER_H_