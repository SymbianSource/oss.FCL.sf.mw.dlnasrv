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
* Description:  File sharing interface class
*
*/


#include <e32base.h>

#include "upnpfilesharing.h"
#include "upnpfilesharingbody.h"
#include "upnpitemlist.h"


// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpFileSharing::CUpnpFileSharing()
    {
    }


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
void CUpnpFileSharing::ConstructL()
    {
    iBody = CUpnpFileSharingBody::NewL();
    }


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpFileSharing* CUpnpFileSharing::NewL()
    {
    CUpnpFileSharing* self = new (ELeave) CUpnpFileSharing;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUpnpFileSharing::~CUpnpFileSharing()
    {
    delete iBody;
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharing::UnshareRootL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::DropAllContentL(TRequestStatus& aStatus)
    {
    iBody->UnshareRootL(aStatus);
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::ShareItemL( const TDesC8& aParentId, CUpnpItem& aItem,
                                            TRequestStatus& aStatus )
    {
    iBody->ShareItemL( aParentId, aItem, aStatus );
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::ShareContainerL( const TDesC8& aParentId, 
                      CUpnpContainer& aContainer, TRequestStatus& aStatus )
    {
    iBody->ShareContainerL( aParentId, aContainer, aStatus );
    }
                                
// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::ShareItemListL( const TDesC8& aParentId, 
        CUpnpItemList& aItemList, TInt& aExecStatus, TRequestStatus& aStatus )
    {
    iBody->ShareItemListL(aParentId, aItemList, aExecStatus, aStatus);
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::UnshareItemL( TInt aItemId,
                                TRequestStatus& aStatus)
    {
    iBody->UnshareItemL( aItemId, aStatus );
    }
   
 
// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::UnshareContainerL( TInt aContainerId,
                                                   TRequestStatus& aStatus )
    {
    iBody->UnshareContainerL( aContainerId, aStatus );
    }
 

// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::GetSharedItemL( TInt aItemId,
                                                CUpnpItem& aItem,
                                                TRequestStatus& aStatus )
    {
    iBody->GetSharedItemL( aItemId, aItem, aStatus );
    }


// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::GetSharedContainerL( TInt aContainerId,
                                                 CUpnpContainer& aContainer,
                                                 TRequestStatus& aStatus )
    {
    iBody->GetSharedContainerL( aContainerId, aContainer, aStatus );
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareItemL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::UnshareItemListL( CUpnpItemList& aObjectList,
                                TInt& aExecStatus,
                                TRequestStatus& aStatus)
    {
    iBody->UnshareItemListL(aObjectList, aExecStatus, aStatus);
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharing::GetSharedItemListL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::GetSharedItemListL( TInt aContainerId,
                                CUpnpBrowseCriteria& aBrowseCriteria,
                                CUpnpItemList& aItemList,
                                TInt& aTotalMatches,
                                TRequestStatus& aStatus )
    {
    iBody->GetSharedItemListL(aContainerId, aBrowseCriteria, aItemList, 
                                                    aTotalMatches, aStatus);
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharing::GetSharedContainerListL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::GetSharedContainerListL( TInt aContainerId,
                                CUpnpBrowseCriteria& aBrowseCriteria,
                                CUpnpContainerList& aItem,
                                TInt& aTotalMatches,
                                TRequestStatus& aStatus )
    {
    iBody->GetSharedContainerListL(aContainerId, aBrowseCriteria, aItem, 
                                                    aTotalMatches, aStatus);
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareReferenceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::ShareReferenceL( TInt aContainerId, 
                                            TInt aObjectId, 
                                            CUpnpItem& aItem,
                                            TRequestStatus& aStatus )
    {
    iBody->ShareReferenceL( aContainerId, aObjectId, aItem, aStatus );
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharing::ShareReferenceListL
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpFileSharing::ShareReferenceListL( CUpnpItemList& aItemList,
                                                TInt& aExecStatus,
                                                TRequestStatus& aStatus )
    {
    iBody->ShareReferenceListL(aItemList, aExecStatus, aStatus);    
    }    
                                                
// ---------------------------------------------------------------------------
// CUpnpFileSharing::MediaServerClient
// ---------------------------------------------------------------------------
//    
EXPORT_C RUpnpMediaServerClient& CUpnpFileSharing::MediaServerClient( )
    {
    return iBody->MediaServerClient();
    }

// End of file
