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
* Description:  Constants used in upnprenderingstatemachine
*
*/

#ifndef C_UPNPRENDERINGSTATEMACHINECONSTANTS_H
#define C_UPNPRENDERINGSTATEMACHINECONSTANTS_H

/**
 * Constants enumerations for the Upnp API
 * especially those needed in the rendering engine part.
 */
namespace Upnp
    {

    /**
     * Commands
     */
    enum TCommand
        {
        ENone     = 0x0000, // no command
        ESync     = 0x0001, // get in sync with the renderer
        EPlay     = 0x0002, // requests to start playing
           // (in RenderingStateMachine aMedia indicates the media to play)
        EStop     = 0x0004, // requests to stop playback (fast to play again)
        EClose    = 0x0008, // requests to close playback (a permanent stop)
        EPause    = 0x0010, // request to pause
        EResume   = 0x0020, // request to resume from pause
        ERestart  = 0x0040, // start track from beginning
        ECalibrate= 0x0080, // request to calibrate the position counter from
                            // renderer
        ESeek     = 0x0100, // seek (aParameter = position in ms)
        ENext     = 0x0200, // skip to next track
        EPrev     = 0x0400, // skip to previous track
        EBack     = 0x0800, // skip to previous track, or track start if
                            // current position is less than 3 seconds
        EJump     = 0x1000,  // Start playing given track 
                            // (aParameter = track index)
        ESetUri   = 0x2000  // set uri
        };

    /**
     * Properties
     */
    enum TProperty
        {
        EVolume   = 0x01,  // the volume property
        EMute     = 0x02,  // the mute property
        EMediaDuration = 0x04, // current track duration (ms) - read only
        EPosition = 0x08,  // current track position (ms) - see ESeek command
        EOptions  = 0x10,  // rendering options (see TRenderingOptions)
        ECurrentIndex = 0x20, // Index of item currently playing.
            // Note: can only be set when NOT playing.
            // Note2: changes in the value are reported via
            // CurrentIndexChanged()
        ECurrentState = 0x40 // the state of the engine (TState) - read only
        };

    /**
     * Generic rendering state
     */
    enum TState
        {
        EOffSync   = 0x0001, // not synchronised with renderer yet
        EBusy      = 0x0010, // renderer is busy doing something else
        EDead      = 0x0020, // unrecoverable error state
        EStopped   = 0x0100, // renderer is idle
        EBuffering = 0x1000, // media playback is being started in the 
                             // renderer
        EPlaying   = 0x2000, // renderer is playing media
        EPaused    = 0x4000, // renderer is in pause state
        // -------- masks ---------
        EStateMaskInSync    = 0xFFF0, // any state where synchronised to 
                                      // renderer
        EStateMaskActive    = 0xFF00, // any state where can be used
        EStateMaskRendering = 0XF000, // any state where some kind of output
                                      // is going on
        };
    
    /**
     * extra information in RenderingStateMachine provided with state indicates
     * reason for state transition
     */
    enum TStateInfo
        {
        ENoMedia = 0,
            // no media initialised (typically the initial state)
        EStopRequested = 1,
            // stop was requested (by this engine or by external party)
        ETrackCompleted = 2,
            // track was fully completed
        EPositionChanged = 3
        };
    
    /**
     * Mode in which the position is changing.
     * This includes information of trick modes.
     * this type is internal to rendering engine
     */
    enum TPositionMode
        {
        EPlayingNormally = 0,
            // playback is going on normally
        ESeekingForward = 1,
            // fast seeking forward. New position will be ackowledged soon.
        ESeekingBackward = 2,
            // fast seeking backward New position will be ackowledged soon.
        EBufferingData = 3,
            // renderer is buffering data. Will return to PlayingNormally
            // soon.
        };
    
    /**
     * Rendering options. Use this to optimise the engine to your
     * specific needs.
     * this type is internal to rendering engine.
     * 
     * NOTE: instructions for flag use on the rendering engine API:
     * to turn on a flag:
     *   engine->SetPropertyL( Upnp::EOptions,
     *     engine->Property( Upnp::EOptions ) | EFlag );
     * to turn off a flag:
     *   engine->SetPropertyL( Upnp::EOptions,
     *     engine->Property( Upnp::EOptions ) & ~EFlag );
     */
    enum TRenderingOptions
        {
        EPlaylistMode = 0x0001,
            // play content in playlist mode, progress automatically.
            // for images this means a timed slideshow.
            // if flag disabled, only manual skip is supported.
            // (this flag is on by default)
        EAutoCalibratePosition = 0x0002,
            // calibrate and update track position automatically.
            // If position changed, sends PropertyChanged callback.
            // (this flag is on by default)
        EVolumeSupport = 0x0004,
            // request support for volume in the renderer
            // Clear this flag to disable all volume usage and avoid one
            // unnecessary network operation. Clearing the flag would be
            // only useful if you only have pure image source.
            // (this flag is on by default)
        ESafetyTimer = 0x0008,
            // attaches a safety timer for all skip commands
            // (next,prev,back,jump) so that client can safely call them
            // directly from UI as often as required. Commands are only
            // executed after the safety period. having a burst of 10
            // "Next" commands will be converted internally to a
            // single "Jump(current+10)" operation.
        EConvertVolumeZeroToMute = 0x0100,
            // convert client VOLUME 0 request to MUTE and back
            // (this flag is on by default)
        };
    }

#endif // C_UPNPRENDERINGSTATEMACHINECONSTANTS_H

