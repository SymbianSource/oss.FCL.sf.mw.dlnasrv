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
* Description:  Calculates playback time for rendering state machine
*
*/

#ifndef UPNPRENDERINGPLAYTIMECALCULATOR_H
#define UPNPRENDERINGPLAYTIMECALCULATOR_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
* A class that keeps track of playback time by setting a timestamp at given
* play, pause, resume and stop events and maybe also comparing that to given
* fixed progress positions along the way. By doing that we will be able to
* tell how long the track has been playing when a stop signal arrives. This
* is important so that we can tell if it is a natural "track complete" stop
* or an "user interrupt" type of stop.
*/
NONSHARABLE_CLASS( CUpnpRenderingPlaytimeCalculator )
    {
public: // the interface

    /**
     * Default constructor
     */
    CUpnpRenderingPlaytimeCalculator();

    /**
     * Marks START timestamp.
     * Calling Start() will reset the calculator.
     */
    void Start();

    /**
     * Marks PAUSE timestamp.
     * Can be called multiple times, ceach Pause will replace previous mark.
     */
    void Pause();

    /**
     * Marks RESUME timestamp.
     * Can NOT be called multiple times.
     */
    void Resume();

    /**
     * Marks STOP timestamp.
     * Can be called multiple times.
     */
    void Stop();

    /**
     * Sets the track duration as it is represented in the media server.
     * This method should be used to set the initial duration so that
     * the calculator works even if renderer NEVER sends position info.
     * @param aDuration track duration in milliseconds
     */
    void SetDuration( TInt aDuration );

    /**
     * Returns the track duration if it is known, otherwise KErrNotFound.
     */
    TInt Duration() const;

    /**
     * Acknowledges track duration and position info that was received from
     * the renderer. Following things are done here:
     * 1. The renderer's understanding of duration might be different from
     * media server's understanding. If it is, we MAY modify it.
     * 2. The position is compared to the calculated position and if it is
     * different, an offset is calculated and used in future Position calls.
     */
    void AcknowledgePositionInfo( TInt aDuration, TInt aPosition );

    /**
     * Estimates position now, or returns total playtime of previous track.
     */
    TInt Position() const;

    /**
     * returns true if last track track playtime would indicate that it was
     * played back entirely.
     */
    TBool IsTrackComplete() const;
        
    /**
     * Returns true if calculator is in playing state.
     */
    TBool IsPlaying() const;
    
    /**
     * Adjust the start mark according to the new postion; use case is seek
     */
   void RestartAt( TInt aNewPosition );

private:

    /**
     * calculates playtime estimate starting from PLAY mark until given time,
     * tacking in account all time spent in PAUSE and also cinsidering offset.
     */
    TInt PositionAt( TTime& aTime ) const;

    /**
     * calculates time spent in pause
     */
    TInt PausetimeAt( TTime& aTime ) const;
    
    /**
     * calculates the offset from current time and the position
     */
    TInt OffsetFromNow( TInt aPosition ) const;

private:

    // duration of the track
    TInt iDuration;

    // the start timestamp
    TTime iStartMark;

    // the stop timestamp
    TTime iStopMark;

    // the pause mark
    TTime iPauseMark;

    // total time spent in pause
    TInt iPauseTime;

    // the offset to real position
    TInt iOffset;

    // is a track playing
    TBool iPlaying;

    // is track paused
    TBool iPaused;

    };



#endif  // UPNPRENDERINGPLAYTIMECALCULATOR_H

// End of File
