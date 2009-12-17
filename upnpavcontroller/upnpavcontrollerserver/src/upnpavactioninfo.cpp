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
* Description:      UpnpAvAction info
*
*/






// INCLUDE FILES

#include "upnpavactioninfo.h"

// CONSTANTS
_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::CUPnPAVActionInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAVActionInfo::CUPnPAVActionInfo()
    {    
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUPnPAVActionInfo::ConstructL()
    {                           
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUPnPAVActionInfo* CUPnPAVActionInfo::NewL()
    {
    CUPnPAVActionInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
CUPnPAVActionInfo* CUPnPAVActionInfo::NewLC()
    {
    CUPnPAVActionInfo* self = new( ELeave )
        CUPnPAVActionInfo;   
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
CUPnPAVActionInfo::~CUPnPAVActionInfo()
    {
    delete iUuid;            
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::SetSessionId
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
void CUPnPAVActionInfo::SetSessionId( TInt aSessionId )
    {
    iSessionId = aSessionId;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::SessionId
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
TInt CUPnPAVActionInfo::SessionId() const
    {
    return iSessionId;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::SetObserver
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
void CUPnPAVActionInfo::SetObserver( MUpnpAVControlPointObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::Observer
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
MUpnpAVControlPointObserver& CUPnPAVActionInfo::Observer() const
    {
    
    __ASSERTD( iObserver, __FILE__, __LINE__ );
    return *iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::SetUuidL
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
void CUPnPAVActionInfo::SetUuidL( const TDesC8& aUuid )
    {
    HBufC8* tempBuf = aUuid.AllocL();
    delete iUuid;
    iUuid = tempBuf;
    }

// --------------------------------------------------------------------------
// CUPnPAVActionInfo::Uuid
// See upnpavcationinfo.h
// --------------------------------------------------------------------------
const TDesC8& CUPnPAVActionInfo::Uuid() const
    {
    if( iUuid )
        {
        return *iUuid;
        }
    else
        {
        return KNullDesC8;
        }    
    }
    
// End of File

