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
* Description:      Resource helper for upload session
*
*/






// EXTERNAL INCLUDES
#include <upnpitem.h>

// INTERNAL INCLUDES
#include "upnpresourcehelper.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPResourceHelper::NewL
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPResourceHelper* CUPnPResourceHelper::NewL()
    {
    CUPnPResourceHelper* timer = new(ELeave) CUPnPResourceHelper();
    CleanupStack::PushL( timer );
    timer->ConstructL();
    CleanupStack::Pop();
    return timer;
    }

// --------------------------------------------------------------------------
// CUPnPResourceHelper::~CUPnPResourceHelper
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPResourceHelper::~CUPnPResourceHelper()
    { 
    delete iResource;
    delete iItemId;
    delete iItem;
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::CUPnPResourceHelper
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUPnPResourceHelper::CUPnPResourceHelper()
    {
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::ConstructL
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPResourceHelper::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CUPnPResourceHelper::SetResource
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPResourceHelper::SetResource( HBufC* aResource )
    {
    delete iResource;
    iResource = aResource;
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::Resource
// See upnpavtimer.h
// --------------------------------------------------------------------------
HBufC* CUPnPResourceHelper::Resource()
    {
    return iResource;
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::SetItemId
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPResourceHelper::SetItemId( HBufC8* aItemId )
    {
    delete iItemId;
    iItemId = aItemId;
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::ItemId
// See upnpavtimer.h
// --------------------------------------------------------------------------
HBufC8* CUPnPResourceHelper::ItemId()
    {
    return iItemId;
    }

// --------------------------------------------------------------------------
// CUPnPResourceHelper::SetItem
// See upnpavtimer.h
// --------------------------------------------------------------------------
void CUPnPResourceHelper::SetItem( CUpnpItem* aItem )
    {
    delete iItem;
    iItem = aItem;
    }
    
// --------------------------------------------------------------------------
// CUPnPResourceHelper::Item
// See upnpavtimer.h
// --------------------------------------------------------------------------
CUpnpItem* CUPnPResourceHelper::Item()
    {
    return iItem;
    }
    
// end of file
