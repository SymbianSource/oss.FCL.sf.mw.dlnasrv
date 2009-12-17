/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media Server file transfer event list
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "upnpfiletransfereventlist.h"
#include    "upnpmediaserverevent.h"
#include    "upnpfiletransferevent.h"










// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpFileTransferEventList* CUpnpFileTransferEventList::NewL()
    {
    CUpnpFileTransferEventList* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpFileTransferEventList* CUpnpFileTransferEventList::NewLC()
    {
    CUpnpFileTransferEventList* self = new( ELeave ) CUpnpFileTransferEventList;
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::NewL
// -----------------------------------------------------------------------------
//
void CUpnpFileTransferEventList::InternalizeSubTypeL( RReadStream& aStream )
    {
    CUpnpFileTransferEvent* newEvent = CUpnpFileTransferEvent::NewLC();
	newEvent->InternalizeL( aStream );
	iEvents.AppendL( newEvent );   
	CleanupStack::Pop();         
    }


// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::NewL
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEventList::CUpnpFileTransferEventList()
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServerEventList::NewL
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEventList::~CUpnpFileTransferEventList()
    {    
    }



//  End of File

