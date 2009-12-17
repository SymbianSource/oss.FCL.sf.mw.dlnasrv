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
* Description:      timing services for AVC server
*
*/






// INCLUDE FILES
#include "upnpavtimer.h"
#include "upnpbrowsingsession.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KMillion = 1000000;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAVTimer::NewL
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPAVTimer* CUPnPAVTimer::NewL( MUPnPAVTimerCallback& aObserver,
    TAVTimerType aType )
    {
    CUPnPAVTimer* timer = new(ELeave) CUPnPAVTimer( aObserver, aType );
    CleanupStack::PushL( timer );
    timer->ConstructL();
    CleanupStack::Pop();
    return timer;
    }

// --------------------------------------------------------------------------
// CUPnPAVTimer::~CUPnPAVTimer
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPAVTimer::~CUPnPAVTimer()
    { 
    Cancel();
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTimer::CUPnPAVTimer
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPAVTimer::CUPnPAVTimer( MUPnPAVTimerCallback& aObserver,
    TAVTimerType aType ) :
    CTimer( EPriorityStandard ),
    iObserver( aObserver ),
    iTimerType( aType )
    {
    CActiveScheduler::Add( this );
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTimer::ConstructL
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPAVTimer::ConstructL()
    {
    CTimer::ConstructL();
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTimer::RunL
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPAVTimer::RunL()
    {
    iObserver.UPnPAVTimerCallback( iTimerType );        
    }

// --------------------------------------------------------------------------
// CUPnPAVTimer::Start
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPAVTimer::Start( TInt aIntervalInSecs )
    {
    __ASSERTD( !IsActive(), __FILE__, __LINE__  )
    After( TTimeIntervalMicroSeconds32( aIntervalInSecs * KMillion ) );
    }

// end of file
