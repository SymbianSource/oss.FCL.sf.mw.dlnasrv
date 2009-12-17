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






// INCLUDES
#include <mmf/common/mmfbase.h> // MMF base classes

#include <S60FourCC.h>
#include <AudioPreference.h>
#include "upnpaudiopolicy.h" // myself

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"
// CONSTANTS

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPAudioPolicy* CUPnPAudioPolicy::NewL(
    MUPnPAudioPolicyObserver& aObserver )
    {
    __LOG( "CUPnPAudioPolicy::NewL 1" );
    CUPnPAudioPolicy* self =
        new(ELeave) CUPnPAudioPolicy( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::CUPnPAudioPolicy
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPAudioPolicy::CUPnPAudioPolicy(
    MUPnPAudioPolicyObserver& aObserver )
    : iObserver( aObserver )
    , iAudioPolicyState( EStateUninitialised )
    {
    }

// --------------------------------------------------------------------------
// CUPnPTrack::ConstructL
// --------------------------------------------------------------------------
//  
void CUPnPAudioPolicy::ConstructL()
    {
    __LOG( "CUPnPAudioPolicy::ConstructL" );
    iDevSound = CMMFDevSound::NewL();
    __LOG( "CUPnPAudioPolicy::ConstructL end" );
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::CUPnPAudioPolicy
// destructor.
// --------------------------------------------------------------------------
//
CUPnPAudioPolicy::~CUPnPAudioPolicy()
    {
    __LOG( "CUPnPAudioPolicy destructor" );
    delete iDevSound;
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::PlayL
// requests for playback start
// --------------------------------------------------------------------------
//
void CUPnPAudioPolicy::PlayL()
    {
    __LOG( "CUPnPAudioPolicy::PlayL()" );
    __LOG1( "CUPnPAudioPolicy::PlayL in state(%d)", TInt(iAudioPolicyState) );

    // Ignore if already on playing state
    if( iAudioPolicyState != EStatePlaying )
        {
        __LOG( "CUPnPAudioPolicy: initialising devsound" );
        TRAPD( err, iDevSound->InitializeL(
            *this, KFourCCCodeNokiaUPnP, EMMFStatePlaying ) );
        if( err != KErrNone )
            {
            __LOG1( "CUPnPAudioPolicy: DevSound initialize err:(%d)",err );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::Stop
// indicates playback will stop
// --------------------------------------------------------------------------
//
void CUPnPAudioPolicy::Stop()
    {
    __LOG( "CUPnPAudioPolicy::Stop()" );
    __LOG1( "CUPnPAudioPolicy::Stop in state(%d)", TInt(iAudioPolicyState) );

    // Ignore if already on stopped state
    if( iAudioPolicyState == EStatePlaying )
        {
        iAudioPolicyState = EStateStopped;
        iDevSound->Stop();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::InitializeComplete
// from MDevSoundObserver
// --------------------------------------------------------------------------
//
void CUPnPAudioPolicy::InitializeComplete( TInt aError )
    {
    __LOG1( "CUPnPAudioPolicy::InitializeComplete(%d)", aError );
    __LOG1( "CUPnPAudioPolicy::InitializeCompl in state(%d)",
        TInt(iAudioPolicyState) );
    if( aError == KErrNone && iAudioPolicyState != EStatePlaying )
        {
        __LOG( "CUPnPAudioPolicy: priority settings" );
        TMMFPrioritySettings mmfPrioSettings;
        mmfPrioSettings.iPriority = KAudioPriorityUPnPRemotePlayback;
        mmfPrioSettings.iPref =
            (TMdaPriorityPreference)KAudioPrefUPnPPlayback;
        mmfPrioSettings.iState = EMMFStatePlaying;
        iDevSound->SetPrioritySettings(mmfPrioSettings);
        
        __LOG( "CUPnPAudioPolicy::InitializeComple calling PlayInitL.." );
        TRAPD( err, iDevSound->PlayInitL() );
        if( err == KErrNone )
            {
            iAudioPolicyState = EStatePlaying;
            }
        else
            {
            __LOG1( "CUPnPAudioPolicy: PlayInitL err:(%d)", err );
            }
        __LOG( "CUPnPAudioPolicy::InitializeComplete - end" );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAudioPolicy::PlayError
// from MDevSoundObserver
// --------------------------------------------------------------------------
//
void CUPnPAudioPolicy::PlayError( TInt aError )
    {
    __LOG1( "CUPnPAudioPolicy::PlayError(%d)", aError );
    __LOG1( "CUPnPAudioPolicylicy::PlayError in state(%d)",
        TInt(iAudioPolicyState) );
    if ( aError == KErrDied || aError == KErrInUse )
        {
        iAudioPolicyState = EStateStopped;
        iObserver.AudioConflict( aError );
        }
    }

// --------------------------------------------------------------------------
// Unused callbacks from MDevSoundObserver
// --------------------------------------------------------------------------
void CUPnPAudioPolicy::ToneFinished( TInt /*aError*/ )
    {
    __LOG( "CUPnPAudioPolicy::ToneFinished()" );
    }
void CUPnPAudioPolicy::BufferToBeFilled( CMMFBuffer* /*aBuffer*/ )
    {
    __LOG( "CUPnPAudioPolicy::BufferToBeFilled()" );
    }
void CUPnPAudioPolicy::DeviceMessage( TUid /*aMessageType*/,
    const TDesC8& /*aMsg*/ )
    {
    __LOG( "CUPnPAudioPolicy::DeviceMessage()" );
    }
void CUPnPAudioPolicy::SendEventToClient( const TMMFEvent& /*aEvent*/ )
    {
    __LOG( "CUPnPAudioPolicy::SendEventToClient()" );
    }
void CUPnPAudioPolicy::RecordError( TInt /*aError*/ )
    {
    __LOG( "CUPnPAudioPolicy::RecordError()" );
    }
void CUPnPAudioPolicy::ConvertError( TInt /*aError*/ )
    {
    __LOG( "CUPnPAudioPolicy::ConvertError()" );
    }
void CUPnPAudioPolicy::BufferToBeEmptied( CMMFBuffer* /*aBuffer*/ )
    {
    __LOG( "CUPnPAudioPolicy::BufferToBeEmptied()" );
    }

