/*
* Copyright (c) 2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of playback time for rendering state machine
*
*/

// INCLUDES
#include "upnprenderingplaytimecalculator.h"

_LIT( KComponentLogfile, "upnprenderingstatemachine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KMicrosecondsInMillisecond = 1000;
const TInt KMillisecondsInSecond = 1000;
const TInt KDurationErrorMargin = 3000; // 3 seconds

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::CUpnpRenderingPlaytimeCalculator
// --------------------------------------------------------------------------
//
CUpnpRenderingPlaytimeCalculator::CUpnpRenderingPlaytimeCalculator()
    {
    iPlaying = EFalse;
    iPaused = EFalse;
    iStartMark = 0;
    iStopMark = 0;
    iPauseMark = 0;
    iPauseTime = 0;
    iOffset = 0;
    iDuration = KErrNotFound;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Start
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::Start()
    {
    __LOG("PlaytimeCalculator: Start()" );
    iStartMark.UniversalTime();
    iPauseTime = 0;
    iPlaying = ETrue;
    iPaused = EFalse;
    iOffset = 0;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Pause
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::Pause()
    {
    __LOG("PlaytimeCalculator: Pause()" );
    if( !iPaused )
        {
        iPauseMark.UniversalTime();
        iPaused = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Resume
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::Resume()
    {
    __LOG("PlaytimeCalculator: Resume()" );
    TTime resumeMark;
    resumeMark.UniversalTime();
    iPauseTime += PausetimeAt( resumeMark );
    iPaused = EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Stop
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::Stop()
    {
    __LOG("PlaytimeCalculator: Stop()" );
    iStopMark.UniversalTime();
    iPlaying = EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::SetDuration
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::SetDuration( TInt aDuration )
    {
    iDuration = aDuration;
    __LOG1("PlaytimeCalculator:SetDuration iDuration %d", iDuration );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Duration
// --------------------------------------------------------------------------
//
TInt CUpnpRenderingPlaytimeCalculator::Duration() const
    {
    __LOG1("PlaytimeCalculator: iDuration %d", iDuration );
    return iDuration;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::AcknowledgePositionInfo
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::AcknowledgePositionInfo(
    TInt aDuration, TInt aPosition )
    {
    __LOG3("PlaytimeCalculator::AcknowledgePositionInfo \
aDuration %d aPosition %d, iPlaying %d ", 
    aDuration, aPosition, iPlaying );
    
    // handle DURATION
    if ( aDuration > 0 )
        {
        if ( iDuration <= 0)
            {
            // we did not know duration before, set it
            iDuration = aDuration;
            }
        else if ( aDuration > iDuration )
            {
            // renderer thinks track is longer. trust longer value!
            // too long is more safe than too short.
            // TOO LONG -> playlist play may discontinue. (annoying)
            // TOO SHORT -> pressing stop may cause track skip and
            //              user can't stop the music (critical)
            iDuration = aDuration;
            }
        }
    // handle POSITION
    if ( iPlaying )
        {
        // calculate playtime now, compare to given position
        // and manipulate offset accordingly
        iOffset = OffsetFromNow( aPosition );
        __LOG1("PlaytimeCalculator: New Offset %d", iOffset );
        }
    else
        {
        // start timer and set position according to ongoing playback
        Start();
        TTimeIntervalSeconds origPos = aPosition / KMillisecondsInSecond;
        iStartMark -= origPos;
        iOffset = OffsetFromNow( aPosition );
        }
    
    __LOG2("PlaytimeCalculator::AcknowledgePositionInfo \
iDuration %d Current Position %d ", iDuration, Position() );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::Position
// --------------------------------------------------------------------------
//
TInt CUpnpRenderingPlaytimeCalculator::Position() const
    {
    TTime tempTime;
    if ( iPlaying )
        {
        __LOG("PlaytimeCalculator: Position() - using current time" );
        // measure time until NOW.
        tempTime.UniversalTime();
        }
    else
        {
        // measure time until last track STOPPED.
        tempTime = iStopMark;
        }        
    return PositionAt( tempTime );
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::IsTrackComplete
// --------------------------------------------------------------------------
//
TBool CUpnpRenderingPlaytimeCalculator::IsTrackComplete() const
    {
    TBool isCompleted = ETrue;
    TInt playtime = Position();
    if ( playtime >= 0 &&
        playtime < iDuration - KDurationErrorMargin )
        {
        // [0 - duration-margin]
        isCompleted= EFalse;
        }
    else if ( playtime >= iDuration - KDurationErrorMargin &&
        playtime <= iDuration + KDurationErrorMargin )
        {
        // [duration-margin - duration+margin]
        isCompleted= ETrue;
        }
    else
        {
        // position either negative or greater than duration ??
        __LOG2("PlaytimeCalculator: WARNING: playtime=%d duration=%d",
            playtime,
            iDuration );
        isCompleted= ETrue;
        }
    return isCompleted;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::IsPlaying
// --------------------------------------------------------------------------
//
TBool CUpnpRenderingPlaytimeCalculator::IsPlaying() const
    {
    return iPlaying;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::RestartAt
// --------------------------------------------------------------------------
//
void CUpnpRenderingPlaytimeCalculator::RestartAt( TInt aNewPosition )
    {
    __LOG("PlaytimeCalculator: RestartAt()" );
    Stop();
    Start();
    // Consider new position to be equivalent to current position
    TTimeIntervalSeconds newPos = 
            aNewPosition / KMillisecondsInSecond;
    iStartMark -= newPos;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::PositionAt
// --------------------------------------------------------------------------
//
TInt CUpnpRenderingPlaytimeCalculator::PositionAt( TTime& aMark ) const
    {
    TTimeIntervalMicroSeconds played =
        aMark.MicroSecondsFrom( iStartMark );
    TInt playtime = ( played.Int64() / KMicrosecondsInMillisecond );
    __LOG1("PlaytimeCalculator: PositionAt played %d", playtime );
    
    playtime -= iPauseTime;
    playtime -= PausetimeAt( aMark );
    playtime += iOffset;
    __LOG3("PlaytimeCalculator: playtime=%d pausetime=%d offset=%d",
        playtime,
        (iPauseTime + PausetimeAt( aMark )),
        iOffset );
    return playtime;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::PausetimeAt
// --------------------------------------------------------------------------
//
TInt CUpnpRenderingPlaytimeCalculator::PausetimeAt( TTime& aMark ) const
    {
    TInt pausetime = 0;
    if ( iPaused )
        {
        TTimeIntervalMicroSeconds latestPause =
            aMark.MicroSecondsFrom( iPauseMark );
        pausetime = ( latestPause.Int64() / KMicrosecondsInMillisecond );
        __LOG1("PlaytimeCalculator: PausetimeAt pause time %d", pausetime );
        }
    return pausetime;
    }

// --------------------------------------------------------------------------
// CUpnpRenderingPlaytimeCalculator::OffsetFromNow
// --------------------------------------------------------------------------
//
TInt CUpnpRenderingPlaytimeCalculator::OffsetFromNow( TInt aPosition ) const
    {
    TTime timeNow;
    timeNow.UniversalTime();
    TInt tempPlaytime = PositionAt( timeNow );
    TInt o = aPosition - tempPlaytime;
    __LOG2("PlaytimeCalculator: renderer position (%d), \
tempPlaytime (%d)", aPosition, tempPlaytime );
    __LOG3("PlaytimeCalculator: old off(%d) current off(%d), \
new offset(%d)", iOffset, o, iOffset+o);
    return iOffset+o;
    }

// end of file
