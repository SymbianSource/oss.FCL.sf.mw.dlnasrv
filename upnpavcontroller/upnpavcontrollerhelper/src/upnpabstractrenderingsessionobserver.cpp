/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      An abstract rendering session observer implementation
*
*/






// INCLUDE FILES
#include "upnpavrenderingsession.h"
#include "upnpavrenderingsessionobserver.h"

#include "upnpabstractrenderingsessionobserver.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS


// METHODS


// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::CUPnPAbstractRenderingSessionObs
//---------------------------------------------------------------------------
EXPORT_C CUPnPAbstractRenderingSessionObserver::
    CUPnPAbstractRenderingSessionObserver()
    {
    }


// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::~CUPnPAbstractRenderingSessionObs
//---------------------------------------------------------------------------
EXPORT_C CUPnPAbstractRenderingSessionObserver::
    ~CUPnPAbstractRenderingSessionObserver()
    {
    DisableSessionObserver();
    }


// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::SetSession
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractRenderingSessionObserver::SetSession(
    MUPnPAVRenderingSession& aHostSession )
    {
    // observer bust be disabled when calling this!
    __ASSERTD( !iObserverEnabled, __FILE__, __LINE__ );
    iObservedSession = &aHostSession;
    }


// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::EnableSessionObserver
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractRenderingSessionObserver::EnableSessionObserver()
    {
    __LOG( "AbstractObserver:EnableSessionObserver()" );
    if ( !iObserverEnabled )
        {
        // take original observer as proxy
        iProxyObserver = iObservedSession->Observer();
        iObservedSession->RemoveObserver();
        iObservedSession->SetObserver( *this );
        iObserverEnabled = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::DisableSessionObserver
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractRenderingSessionObserver::DisableSessionObserver()
    {
    if ( iObserverEnabled )
        {
        __LOG( "AbstractObserver:DisableSessionObserver()" );
        __ASSERTD( iObservedSession != 0, __FILE__, __LINE__ );
        // check we still are still the observer !!!
        if ( iObservedSession->Observer() == this )
            {
            iObservedSession->RemoveObserver();
            if ( iProxyObserver )
                {
                // set proxy observer back
                iObservedSession->SetObserver( *iProxyObserver );
                iProxyObserver = 0;
                }
            }
        else
            {
            __LOG( "Disable: Not disabling - wrong observer!" );
            }
        iObserverEnabled = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::IsSessionObserverEnabled
//---------------------------------------------------------------------------
EXPORT_C TBool CUPnPAbstractRenderingSessionObserver::
    IsSessionObserverEnabled()
    {
    return iObserverEnabled;
    }


// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::VolumeResult
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::VolumeResult(
    TInt aError, TInt aVolumeLevel,
    TBool aActionResponse )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->VolumeResult( aError, aVolumeLevel, aActionResponse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::MuteResult
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::MuteResult(
    TInt aError, TBool aMute,
    TBool aActionResponse )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->MuteResult( aError, aMute, aActionResponse );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::InteractOperationComplete
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::InteractOperationComplete(
    TInt aError, TUPnPAVInteractOperation aOperation )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->InteractOperationComplete( aError, aOperation );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::PositionInfoResult
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::PositionInfoResult( TInt aError,
    const TDesC8& aTrackPosition, const TDesC8& aTrackLength )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->PositionInfoResult( aError,
            aTrackPosition, aTrackLength );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::SetURIResult
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::SetURIResult( TInt aError )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->SetURIResult( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::SetNextURIResult
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::SetNextURIResult( TInt aError )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->SetNextURIResult( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::MediaRendererDisappeared
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::MediaRendererDisappeared(
    TUPnPDeviceDisconnectedReason aReason )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->MediaRendererDisappeared( aReason );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractRenderingSessionObserver::ReserveLocalMSServicesCompleted
//---------------------------------------------------------------------------
void CUPnPAbstractRenderingSessionObserver::ReserveLocalMSServicesCompleted(
    TInt aError )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->ReserveLocalMSServicesCompleted( aError );
        }
    }

