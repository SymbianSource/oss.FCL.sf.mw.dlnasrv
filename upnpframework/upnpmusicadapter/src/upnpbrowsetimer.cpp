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


// INCLUDES
#include "upnpbrowsetimer.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPBrowseTimer* CUPnPBrowseTimer::NewL(
    MUPnPBrowseTimerObserver& aObserver,
    TInt32 aTimerWavelength )
    {
    CUPnPBrowseTimer* p = CUPnPBrowseTimer::NewLC(
        aObserver, aTimerWavelength );
    CleanupStack::Pop();
    return p;    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::NewLC
// 1st phase constructor.
// --------------------------------------------------------------------------
//  
CUPnPBrowseTimer* CUPnPBrowseTimer::NewLC(
    MUPnPBrowseTimerObserver& aObserver,
    TInt32 aTimerWavelength )
    {
    
    CUPnPBrowseTimer* p = new(ELeave) CUPnPBrowseTimer( aObserver,
        aTimerWavelength );
    CleanupStack::PushL( p );
    p->ConstructL();
    return p;    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::~CUPnPBrowseTimer
// Desctructor.
// --------------------------------------------------------------------------
//
CUPnPBrowseTimer::~CUPnPBrowseTimer()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::CUPnPBrowseTimer
// Default constructor.
// --------------------------------------------------------------------------
//
CUPnPBrowseTimer::CUPnPBrowseTimer(
    MUPnPBrowseTimerObserver& aObserver,
    TInt32 aTimerWavelength )
    :CTimer( EPriorityStandard ),
    iObserver( aObserver ),
    iTimerWavelength( aTimerWavelength )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPBrowseTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }
 
// --------------------------------------------------------------------------
// Periodizer services
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::Start
// Starts the periodizer.
// --------------------------------------------------------------------------
//
void CUPnPBrowseTimer::Start()
    {
    if ( !IsActive() )
        {
        After( TTimeIntervalMicroSeconds32( iTimerWavelength ) );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::Continue
// Continues a next step for the periodizer.
// --------------------------------------------------------------------------
//
void CUPnPBrowseTimer::Continue()
    {
    After( TTimeIntervalMicroSeconds32( iTimerWavelength ) );
    }

// --------------------------------------------------------------------------
// CUPnPBrowseTimer::RunL
// Receives the timer triggering.
// --------------------------------------------------------------------------
//
void CUPnPBrowseTimer::RunL()
    {
    // deliver the periodic event to the observer
    iObserver.HandlePeriod();
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowseTimer::Stop
// Stops periodizer.
// --------------------------------------------------------------------------
//
void CUPnPBrowseTimer::Stop()
    {
    Cancel();
    }

