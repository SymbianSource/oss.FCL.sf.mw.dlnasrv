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
* Description:      AO for ob serving device disappeared events per session
*
*/






#include "upnpavdeviceactive.h"

#include "upnpavcontrollerclient.h"
#include "upnpavrenderingsessionimpl.h"
#include "upnpavdevice.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
CUPnPAVDeviceActive* CUPnPAVDeviceActive::NewL( RUPnPAVControllerClient&
    aServer, MUPnPAVDeviceObserver& aObserver )
    {
    CUPnPAVDeviceActive* self = new (ELeave) CUPnPAVDeviceActive( aServer,
        aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
CUPnPAVDeviceActive::CUPnPAVDeviceActive( RUPnPAVControllerClient& aServer,
    MUPnPAVDeviceObserver& aObserver ):
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
CUPnPAVDeviceActive::~CUPnPAVDeviceActive()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
void CUPnPAVDeviceActive::ConstructL()
    {    
    }

// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
void CUPnPAVDeviceActive::RunL()
    {
    CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL(); 
    iObserver.UPnPDeviceDisappeared( *tempDev );
    delete tempDev;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
void CUPnPAVDeviceActive::DoCancel()
    {
    iServer.CancelDeviceDisappearedRequest( iId );
    }
    
// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
TInt CUPnPAVDeviceActive::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CUPnPAVDeviceActive::ConstructL
// See upnpavdeviceactive.h
// --------------------------------------------------------------------------
void CUPnPAVDeviceActive::StartListening( TInt aId )
    {
    __ASSERTD( !IsActive(), __FILE__, __LINE__ );
    iId = aId;
    iServer.DeviceDisappearedRequest( aId, iStatus );
    SetActive();    
    }

// end of file
