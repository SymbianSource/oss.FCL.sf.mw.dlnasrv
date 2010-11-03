/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      
*
*/

// INCLUDES
#include "upnpavtevent.h"

// --------------------------------------------------------------------------
// CUPnPAVTEvent::CUPnPAVTEvent
// See upnpavevent.h for description
// --------------------------------------------------------------------------
CUPnPAVTEvent::CUPnPAVTEvent()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::ConstructL
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::ConstructL()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::NewL
// See upnpavevent.h for description
// --------------------------------------------------------------------------
CUPnPAVTEvent* CUPnPAVTEvent::NewL()
    {
    CUPnPAVTEvent* self = new (ELeave) CUPnPAVTEvent;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::~CUPnPAVTEvent
// See upnpavevent.h for description
// --------------------------------------------------------------------------
CUPnPAVTEvent::~CUPnPAVTEvent()
    {
    delete iTransportURI;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::CloneL
// See upnpavevent.h for description
// --------------------------------------------------------------------------
CUPnPAVTEvent* CUPnPAVTEvent::CloneL( const CUPnPAVTEvent& event )
    {
    CUPnPAVTEvent* tmp = CUPnPAVTEvent::NewL();
    tmp->SetInstanceID( event.InstanceID() );
    tmp->SetMute( event.Mute() );
    tmp->SetTransportState( event.TransportState() );
    tmp->SetTransportURIL( event.TransportURI() );
    tmp->SetVolume( event.Volume() );    
    return tmp;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::InstanceID
// See upnpavevent.h for description
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPAVTEvent::InstanceID() const
    {
    return iInstanceID;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTEvent::Mute
// See upnpavevent.h for description
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPAVTEvent::Mute() const
    {
    return iMute;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::Volume
// See upnpavevent.h for description
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPAVTEvent::Volume() const
    {
    return iVolume;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::TransportState
// See upnpavevent.h for description
// --------------------------------------------------------------------------
EXPORT_C CUPnPAVTEvent::TTransportState
    CUPnPAVTEvent::TransportState() const
    {
    return iTransportState;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::TransportURI
// See upnpavevent.h for description
// --------------------------------------------------------------------------
EXPORT_C const TDesC8& CUPnPAVTEvent::TransportURI() const
    {
    if( iTransportURI )
        {
        return *iTransportURI;
        }
    else
        {
        return KNullDesC8;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetInstanceID
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetInstanceID( TInt aInstanceID )
    {
    iInstanceID = aInstanceID;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetMute
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetMute( TInt aMute )
    {
    iMute = aMute;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetVolume
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetVolume( TInt aVolume )
    {
    iVolume = aVolume;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetTransportState
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetTransportState( TTransportState aTransportState )
    {
    iTransportState = aTransportState;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetTransportState
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetTransportState( const TDesC8& aData )
    {
    // Define AV Transport States
    _LIT8( KStopped,           "STOPPED"           );        
    _LIT8( KPlaying,           "PLAYING"           );
    _LIT8( KTransitioning,     "TRANSITIONING"     );
    _LIT8( KPausedPlayback,    "PAUSED_PLAYBACK"   );
    _LIT8( KPausedRecording,   "PAUSED_RECORDING"  );
    _LIT8( KRecording,         "RECORDING"         );
    _LIT8( KNoMediaPresent,    "NO_MEDIA_PRESENT"  );

    if( !aData.CompareF( KStopped ) )
        {
        iTransportState = EStopped;
        }
    else if( !aData.CompareF( KPlaying ) )
        {
        iTransportState = EPlaying;
        }
    else if( !aData.CompareF( KTransitioning ) )
        {
        iTransportState = ETransitioning;
        }
    else if( !aData.CompareF( KPausedPlayback ) )
        {
        iTransportState = EPausedPlayback;
        }
    else if( !aData.CompareF( KPausedRecording ) )
        {
        iTransportState = EPausedRecording;
        }
    else if( !aData.CompareF( KRecording ) )
        {
        iTransportState = ERecording;
        }
    else if( !aData.CompareF( KNoMediaPresent ) )
        {
        iTransportState = ENoMediaPresent;
        }
    else
        {
        iTransportState = EIdle;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::SetTransportURIL
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::SetTransportURIL( const TDesC8& aTransportURI )
    {
    HBufC8* tmp = aTransportURI.AllocL();
    iTransportURI = tmp;
    }

// --------------------------------------------------------------------------
// CUPnPAVTEvent::Reset
// See upnpavevent.h for description
// --------------------------------------------------------------------------
void CUPnPAVTEvent::Reset()
    {
    iInstanceID = KErrNotFound;
    iMute = KErrNotFound;
    iVolume = KErrNotFound;
    iTransportState = EIdle;
    delete iTransportURI; iTransportURI = NULL;
    }

// End of File
