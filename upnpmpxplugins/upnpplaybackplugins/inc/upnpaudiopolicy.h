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
* Description:      Audio policy implementation for upnp remote plugin
*
*/






#ifndef C_UPNPAUDIOPOLICY_H
#define C_UPNPAUDIOPOLICY_H


#include <mmf/server/sounddevice.h> // for MDevSoundObserver

// FORWARD DECLARATIONS
class MUPnPAudioPolicyObserver;
class CMMFBuffer;
class CMMFDevSound;

/**
 * A class that implements audio policy for upnp playback plugin.
 * It registers for DevSound for a vrtual playback handle and is
 * able to receive notifications from MMF. The purpose for this
 * is to pause upnp playback when <br>
 *  - local playback begins <br>
 *  - there is an incoming call <br>
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPAudioPolicy
    : public CBase,
      public MDevSoundObserver
    {
    
private:

    /**
     * State of audiopolicy
     *
     * @since Series 60 3.1
     */   
    enum TAudioPolicyState
        {
        EStateUninitialised, // waiting initialise devSound, can not be used
        EStatePlaying,       // audiopolicy initialized and playback started
        EStateStopped,       // playback stopped
        };

public: // public services

    /**
     * constructor
     * @param aPlaybackMachine the playback states engine
     */
    static CUPnPAudioPolicy* NewL(
        MUPnPAudioPolicyObserver& aObserver );

    /**
     * destructor
     */
    CUPnPAudioPolicy::~CUPnPAudioPolicy();

    /**
     * request for start playback
     */
    void PlayL();

    /**
     * indicate playback stops
     */
    void Stop();

protected: // MDevSoundObserver

    /**
     * see MDevSoundObserver
     */
    void InitializeComplete( TInt aError );

    /**
     * see MDevSoundObserver
     */
    void ToneFinished( TInt aError );

    /**
     * see MDevSoundObserver
     */
    void BufferToBeFilled( CMMFBuffer* aBuffer );

    /**
     * see MDevSoundObserver
     */
    void PlayError( TInt aError );

    /**
     * see MDevSoundObserver
     */
    void BufferToBeEmptied( CMMFBuffer* aBuffer );

    /**
     * see MDevSoundObserver
     */
    void RecordError( TInt aError );

    /**
     * see MDevSoundObserver
     */
    void ConvertError( TInt aError );

    /**
     * see MDevSoundObserver
     */
    void DeviceMessage( TUid aMessageType, const TDesC8& aMsg );

    /**
     * see MDevSoundObserver
     */
    void SendEventToClient( const TMMFEvent& aEvent );

private: // private methods

    /**
     * default constructor
     * @param aPlaybackMachine the playback states engine
     */
    CUPnPAudioPolicy(
        MUPnPAudioPolicyObserver& aObserver );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: // data

    /**
     * the playback states engine
     */
    MUPnPAudioPolicyObserver& iObserver;

    /**
     * DevSound component for MMF access
     */
    CMMFDevSound* iDevSound;
    
    /**
     * Current AudioPolicy state
     */
    TAudioPolicyState iAudioPolicyState;

    };

/**
 * Callback interface for audio policy class
 */
class MUPnPAudioPolicyObserver
    {

public:

    /**
     * A conflict was found between simultaneously playing audio
     * resources. This means that this instance is active and
     * meanwhile another party starts using audio resources in
     * a way that has been defined to be illegal. The observer
     * is expected to take apropriate actions after receiving
     * this event - notifying the playback framework, which will
     * then clear all resources and inform user.
     *
     * @param aError error to be issued to playback framework
     */
    virtual void AudioConflict( TInt aError ) = 0;

    };

#endif // C_UPNPAUDIOPOLICY_H
