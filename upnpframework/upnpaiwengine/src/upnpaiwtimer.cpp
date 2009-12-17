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
* Description:  Implementation of the class CUPnPAiwTimer.
*
*/


// INCLUDES
#include "upnpaiwtimer.h"
#include "upnpaiwtimerobserver.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAiwTimer::NewL
// Construct the timer, add CHeartbeat to Active Scheduler and start it.
// --------------------------------------------------------------------------
//
CUPnPAiwTimer* CUPnPAiwTimer::NewL( TInt aInterval,
                                    MUPnPAiwTimerObserver* aCallback 
                                )
    {
    CUPnPAiwTimer* self = new (ELeave) CUPnPAiwTimer();
    CleanupStack::PushL( self );
    self->ConstructL( aInterval, aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwTimer::CUPnPAiwTimer
// Constructor
// --------------------------------------------------------------------------
//
CUPnPAiwTimer::CUPnPAiwTimer()
    {
    // No implementation
    }

// --------------------------------------------------------------------------
// CUPnPAiwTimer::ConstructL
// Constructs the timer.
// --------------------------------------------------------------------------
//
void CUPnPAiwTimer::ConstructL( TInt aInterval,
                                MUPnPAiwTimerObserver* aCallback )
    {

    // Check the parameters
    if( aInterval <= 0 ||
        !aCallback )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        iInterval = aInterval;
        iCallback = aCallback;
        }

    // Create CHeartbeat object
    iHeartbeat = CHeartbeat::NewL( EPriorityLow );

    iCounter = 0;

    // Start the heartbeat timer (beating exactly on the second)
    iHeartbeat->Start( ETwelveOClock, this );
    }

// --------------------------------------------------------------------------
// CUPnPAiwTimer::~CUPnPAiwTimer()
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPAiwTimer::~CUPnPAiwTimer()
    {
    // Cancel any outstanding request
    if( iHeartbeat )
        {
        iHeartbeat->Cancel();
        delete iHeartbeat;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwTimer::Beat()
// Called when the beat is in sync.
// --------------------------------------------------------------------------
//
void CUPnPAiwTimer::Beat()
    {
    // Increase heartbeat counter
    iCounter++;

    // If interval is reached, do the call back
    if( iCounter == iInterval )
        {
        if( iCallback )
            {
            iCallback->TimerCallback();
            }
        iCounter = 0;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwTimer::Synchronize()
// Called when the beat needs to be syncronized.
// --------------------------------------------------------------------------
//
void CUPnPAiwTimer::Synchronize()
    {
    // not used
    }

//  End of File
