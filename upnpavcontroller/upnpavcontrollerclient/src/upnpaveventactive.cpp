/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      AO for observing events within a rendering session
*
*/






#include "upnpaveventactive.h"

#include "upnpavcontrollerclient.h"
#include "upnpavrenderingsessionimpl.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVEventActive::NewL
// See upnpeventactive.h
// --------------------------------------------------------------------------
CUPnPAVEventActive* CUPnPAVEventActive::NewL(
    RUPnPAVControllerClient& aServer,
    CUPnPAVRenderingSessionImpl& aObserver )
    {
    CUPnPAVEventActive* self = new (ELeave) CUPnPAVEventActive(
        aServer, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVEventActive::CUPnPAVEventActive
// See upnpeventactive.h
// --------------------------------------------------------------------------
CUPnPAVEventActive::CUPnPAVEventActive(
    RUPnPAVControllerClient& aServer,
    CUPnPAVRenderingSessionImpl& aObserver ):
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iEventPkg( iEvent ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVEventActive::~CUPnPAVEventActive
// See upnpeventactive.h
// --------------------------------------------------------------------------
CUPnPAVEventActive::~CUPnPAVEventActive()
    {
    __LOG( "CUPnPAVEventActive::~CUPnPAVEventActive" );    
    
    Cancel();
    }

// --------------------------------------------------------------------------
// CUPnPAVEventActive::ConstructL
// See upnpeventactive.h
// --------------------------------------------------------------------------
void CUPnPAVEventActive::ConstructL()
    {
    __LOG( "CUPnPAVEventActive::ConstructL" );    
    }

// --------------------------------------------------------------------------
// CUPnPAVEventActive::RunL
// See upnpeventactive.h
// --------------------------------------------------------------------------
void CUPnPAVEventActive::RunL()
    {
    __LOG2( "CUPnPAVEventActive::RunL, status: %d, event: %d",
        iStatus.Int(), iEvent );
    // An event received
    TInt status = iStatus.Int();
    TUnsolicitedEventC event = iEvent;

    if( status == KErrNone )
        {
        iServer.EventRequest( iEventPkg, iId, iStatus );
        SetActive();        
        }
    
    iObserver.EventReceived( status, event );
    }
    
// --------------------------------------------------------------------------
// CUPnPAVEventActive::DoCancel
// See upnpeventactive.h
// --------------------------------------------------------------------------
void CUPnPAVEventActive::DoCancel()
    {
    iServer.CancelEventRequest( iId );
    }
    
// --------------------------------------------------------------------------
// CUPnPAVEventActive::RunError
// See upnpeventactive.h
// --------------------------------------------------------------------------
TInt CUPnPAVEventActive::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVEventActive::StartListeningEvents
// See upnpeventactive.h
// --------------------------------------------------------------------------
void CUPnPAVEventActive::StartListeningEvents( TInt aId )
    {
    __ASSERTD( !IsActive(), __FILE__, __LINE__ );
    iId = aId;
    iServer.EventRequest( iEventPkg, aId, iStatus );
    SetActive();    
    }

// end of file
