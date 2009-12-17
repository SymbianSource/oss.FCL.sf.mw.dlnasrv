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






// INCLUDES
#include "upnpmusicperiodizer.h"

const TInt KPlaybackInfoTimeOutEnd = 2000000; // 2s wait check whether 
    //the playing is ended
// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPMusicPeriodizer* CUPnPMusicPeriodizer::NewL(
    MUPnPMusicPeriodizerObserver& aObserver,
    TInt32 aTimerWavelength )
    {
    CUPnPMusicPeriodizer* p = CUPnPMusicPeriodizer::NewLC(
        aObserver, aTimerWavelength );
    CleanupStack::Pop();
    return p;    
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::NewLC
// 1st phase constructor.
// --------------------------------------------------------------------------
//  
CUPnPMusicPeriodizer* CUPnPMusicPeriodizer::NewLC(
    MUPnPMusicPeriodizerObserver& aObserver,
    TInt32 aTimerWavelength )
    {
    
    CUPnPMusicPeriodizer* p = new(ELeave) CUPnPMusicPeriodizer( aObserver,
        aTimerWavelength );
    CleanupStack::PushL( p );
    p->ConstructL();
    return p;    
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::~CUPnPMusicPeriodizer
// Desctructor.
// --------------------------------------------------------------------------
//
CUPnPMusicPeriodizer::~CUPnPMusicPeriodizer()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::CUPnPMusicPeriodizer
// Default constructor.
// --------------------------------------------------------------------------
//
CUPnPMusicPeriodizer::CUPnPMusicPeriodizer(
    MUPnPMusicPeriodizerObserver& aObserver,
    TInt32 aTimerWavelength )
    :CTimer( EPriorityStandard ),
    iObserver( aObserver ),
    iTimerWavelength( aTimerWavelength )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPMusicPeriodizer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }
 
// --------------------------------------------------------------------------
// Periodizer services
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::Start
// Starts the periodizer.
// --------------------------------------------------------------------------
//
void CUPnPMusicPeriodizer::Start()
    {
    if ( !IsActive() )
        {
        After( TTimeIntervalMicroSeconds32( iTimerWavelength ) );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::Continue
// Continues a next step for the periodizer.
// --------------------------------------------------------------------------
//
void CUPnPMusicPeriodizer::Continue()
    {
    After( TTimeIntervalMicroSeconds32( iTimerWavelength ) );
    }

// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::RunL
// Receives the timer triggering.
// --------------------------------------------------------------------------
//
void CUPnPMusicPeriodizer::RunL()
    {
    // deliver the periodic event to the observer
    if ( iTimerWavelength == KPlaybackInfoTimeOutEnd )
        {
        iObserver.HandlePeriodForEnd();
        }
    else
        {
        iObserver.HandlePeriod();
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPMusicPeriodizer::Stop
// Stops periodizer.
// --------------------------------------------------------------------------
//
void CUPnPMusicPeriodizer::Stop()
    {
    Cancel();
    }



