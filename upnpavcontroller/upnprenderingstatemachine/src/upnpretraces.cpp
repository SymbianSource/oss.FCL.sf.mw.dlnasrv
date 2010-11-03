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
* Description:  Implementation of upnprenderingstatemachine trace helper functions
*
*/

// INCLUDES
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h" // TUPnPAVInteractOperation
#include "upnprenderingstatemachineconstants.h"
#include "upnpretraces.h"

// CONSTANTS


// ======== MEMBER FUNCTIONS ========

const TUint16* StateName( TInt aState )
    {
    switch ( aState )
        {
        case Upnp::EOffSync:   return (TUint16*)L"OffSync";
        case Upnp::EBusy:      return (TUint16*)L"Busy";
        case Upnp::EStopped:   return (TUint16*)L"Stopped";
        case Upnp::EPlaying:   return (TUint16*)L"Playing";
        case Upnp::EPaused:    return (TUint16*)L"Paused";
        case Upnp::EDead:      return (TUint16*)L"Dead";
        case Upnp::EBuffering: return (TUint16*)L"Buffering";
        default:               return (TUint16*)L"State?";
        }
    }

const TUint16* CommandName( TInt aCommand )
    {
    switch ( aCommand )
        {
        case Upnp::ENone:      return (TUint16*)L"None";
        case Upnp::ESync:      return (TUint16*)L"Sync";
        case Upnp::EPlay:      return (TUint16*)L"Play";
        case Upnp::EStop:      return (TUint16*)L"Stop";
        case Upnp::EClose:     return (TUint16*)L"Close";
        case Upnp::EPause:     return (TUint16*)L"Pause";
        case Upnp::EResume:    return (TUint16*)L"Resume";
        case Upnp::ERestart:   return (TUint16*)L"Restart";
        case Upnp::ECalibrate: return (TUint16*)L"Calibrate";
        case Upnp::ESeek:      return (TUint16*)L"Seek";
        case Upnp::ENext:      return (TUint16*)L"Next";
        case Upnp::EPrev:      return (TUint16*)L"Prev";
        case Upnp::EBack:      return (TUint16*)L"Back";
        case Upnp::EJump:      return (TUint16*)L"Jump";
        case Upnp::ESetUri:    return (TUint16*)L"SetUri";
        default:               return (TUint16*)L"Cmd?";
        }
    }

const TUint16* PropertyName( TInt aProperty )
    {
    switch ( aProperty )
        {
        case Upnp::EVolume:       return (TUint16*)L"Volume";
        case Upnp::EMute:         return (TUint16*)L"Mute";
        case Upnp::EMediaDuration:return (TUint16*)L"MediaDuration";
        case Upnp::EPosition:     return (TUint16*)L"Position";
        case Upnp::EOptions:      return (TUint16*)L"Options";
        case Upnp::ECurrentIndex: return (TUint16*)L"CurrentIndex";
        case Upnp::ECurrentState: return (TUint16*)L"CurrentState";
        default:                  return (TUint16*)L"Property?";
        }
    }

const TUint16* InteractOperationName( TInt aInteractOperation )
    {
    switch ( aInteractOperation )
        {
        case EUPnPAVPlay:      return (TUint16*)L"UPnPAVPlay";
        case EUPnPAVPause:     return (TUint16*)L"UPnPAVPause";
        case EUPnPAVStop:      return (TUint16*)L"UPnPAVStop";
        case EUPnPAVPlayUser:  return (TUint16*)L"UPnPAVPlayUser";
        case EUPnPAVPauseUser: return (TUint16*)L"UPnPAVPauseUser";
        case EUPnPAVStopUser:  return (TUint16*)L"UPnPAVStopUser";
        default:               return (TUint16*)L"InteractOperation?";
        }
    }
