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
* Description:  Produces cds actions
*
*/


// INCLUDE FILES
#include "upnpcdsactionfactory.h"
#include "upnpavcontrolpoint.h"

#include "upnpavcpstring.h"

// CONSTANTS
using namespace UpnpAVCPStrings;
const TInt KMaxIntLength = 10;

// ============================= LOCAL FUNCTIONS ===============================



// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CUpnpCdsActionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpCdsActionFactory::CUpnpCdsActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
                                     CUpnpAVControlPoint& aAVCP )
:iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCdsActionFactory::ConstructL() 
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCdsActionFactory* CUpnpCdsActionFactory::NewL( 
                                        MUpnpAVControlPointObserver& aAVCPObserver,
                                        CUpnpAVControlPoint& aAVCP )
    {
    CUpnpCdsActionFactory* self = new (ELeave) CUpnpCdsActionFactory( 
        aAVCPObserver, 
        aAVCP );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::~CUpnpCdsActionFactory
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCdsActionFactory::~CUpnpCdsActionFactory()	
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsBrowseActionLC
// Creates a browsing action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsBrowseActionLC(CUpnpAction*& aAction,
                                       CUpnpDevice* aMediaServer,
                                       const TDesC8& aObjectId,
                                       const TDesC8& aBrowseFlag,
                                       const TDesC8& aFilter, 
                                       TInt aStartingIndex,
                                       TInt aRequestedCount,
                                       const TDesC8& aSortCriteria)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC(KBrowse);
   
    if( aAction )
        {
        TBuf8<KMaxIntLength> startingIndexDes;
        startingIndexDes.Num(aStartingIndex);
        TBuf8<KMaxIntLength> requestedCount;
        requestedCount.Num( aRequestedCount );

        aAction->SetArgumentL( KObjectID(), aObjectId );
        aAction->SetArgumentL( KBrowseFlag(), aBrowseFlag ); 
        aAction->SetArgumentL( KFilter(), aFilter ); 
        aAction->SetArgumentL( KStartingIndex(), startingIndexDes ); 
        aAction->SetArgumentL( KRequestedCount(), requestedCount ); 
        aAction->SetArgumentL( KSortCriteria(), aSortCriteria ); 
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsSearchActionLC
// Creates a search action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsSearchActionLC(CUpnpAction*& aAction,
                                       CUpnpDevice* aMediaServer,
                                       const TDesC8& aObjectId,
                                       const TDesC8& aSearchCriteria,
                                       const TDesC8& aFilter, 
                                       TInt aStartingIndex,
                                       TInt aRequestedCount,
                                       const TDesC8& aSortCriteria)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC(KSearch);
    
    if( aAction )
        {
        TBuf8<KMaxIntLength> startingIndexDes;
        startingIndexDes.Num(aStartingIndex);
        TBuf8<KMaxIntLength> requestedCount;
        requestedCount.Num( aRequestedCount );

        aAction->SetArgumentL( KContainerID(), aObjectId );
        aAction->SetArgumentL( KSearchCriteria(), aSearchCriteria ); 
        aAction->SetArgumentL( KFilter(), aFilter ); 
        aAction->SetArgumentL( KStartingIndex(), startingIndexDes ); 
        aAction->SetArgumentL( KRequestedCount(), requestedCount ); 
        aAction->SetArgumentL( KSortCriteria(), aSortCriteria ); 
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsSystemUpdateIdActionLC
// Creates an action for content directory systemUpdateID query.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsSystemUpdateIdActionLC(CUpnpAction*& aAction,
                                                  CUpnpDevice* aMediaServer)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetSystemUpdateID );

    if( aAction )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsSearchCapabilitiesActionLC
// Creates an action for content directory systemUpdateID query.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsSearchCapabilitiesActionLC(CUpnpAction*& aAction,
                                                      CUpnpDevice* aMediaServer)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetSearchCapabilities );

    if( aAction )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsSortCapabilitiesActionLC
// Creates an action for content directory systemUpdateID query.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsSortCapabilitiesActionLC(CUpnpAction*& aAction,
                                                    CUpnpDevice* aMediaServer)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetSortCapabilities );

    if( aAction )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsCreateObjectActionLC
// Creates an action for creating new content in content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsCreateObjectActionLC(CUpnpAction*& aAction,
                                             CUpnpDevice* aMediaServer,
                                             const TDesC8& aId, 
                                             const TDesC8& aElements)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KCreateObject );

    if( aAction )
        {
        aAction->SetArgumentL( KContainerID, aId );
        aAction->SetArgumentL( KElements, aElements);
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsImportResourceActionLC
// Creates an action for importing content in content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsImportResourceActionLC(CUpnpAction*& aAction,
                                               CUpnpDevice* aMediaServer,
                                               const TDesC8& aSourceUri, 
                                               const TDesC8& aDestinationUri)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KImportResource );

    if( aAction )
        {
        aAction->SetArgumentL( KSourceURI, aSourceUri );
        aAction->SetArgumentL( KDestinationURI, aDestinationUri);
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsExportResourceActionLC
// Creates an action for exporting content in content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsExportResourceActionLC(CUpnpAction*& aAction,
                                               CUpnpDevice* aMediaServer,
                                               const TDesC8& aSourceUri, 
                                               const TDesC8& aDestinationUri)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KExportResource );

    if( aAction )
        {
        aAction->SetArgumentL( KSourceURI, aSourceUri );
        aAction->SetArgumentL( KDestinationURI, aDestinationUri);

        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsCreateReferenceActionLC
// Creates an action for exporting content in content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsCreateReferenceActionLC(
                                        CUpnpAction*& aAction,
                                        CUpnpDevice* aMediaServer,
                                        const TDesC8& aDestinationContainerId, 
                                        const TDesC8& aSourceObjectId)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KCreateReference );

    if( aAction )
        {
        aAction->SetArgumentL( KContainerID, aDestinationContainerId );
        aAction->SetArgumentL( KObjectID, aSourceObjectId);
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsDestroyObjectActionLC
// Creates an action for destroying object from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsDestroyObjectActionLC(CUpnpAction*& aAction,
                                             CUpnpDevice* aMediaServer,
                                             const TDesC8& aObjectId)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KDestroyObject );

    if( aAction )
        {
        aAction->SetArgumentL( KObjectID, aObjectId );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsDeleteResourceActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsDeleteResourceActionLC(CUpnpAction*& aAction,
                                              CUpnpDevice* aMediaServer,
                                              const TDesC8& aResourceUri)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KDeleteResource );

    if( aAction )
        {
        aAction->SetArgumentL( KResourceURI, aResourceUri );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsStopTransferActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsStopTransferActionLC(CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaServer,
                                            TInt aTransferId)
    {
   	TBuf8<KMaxIntLength> transferIdDes;
    transferIdDes.Num(aTransferId);
    
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KStopTransferResource );

    if( aAction )
        {
        aAction->SetArgumentL( KTransferID, transferIdDes );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsTransferProgressActionLC
// Creates an action for deleting resource from content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsTransferProgressActionLC(CUpnpAction*& aAction,
                                                CUpnpDevice* aMediaServer,
                                                TInt aTransferId)
    {
   	TBuf8<KMaxIntLength> transferIdDes;
    transferIdDes.Num(aTransferId);
    
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }
    aAction = service->CreateActionLC( KGetTransferProgress );

    if( aAction )
        {
        aAction->SetArgumentL( KTransferID, transferIdDes );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpCdsActionFactory::CdsUpdateObjectActionLC
// Creates an action for updating objects in content directory.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpCdsActionFactory::CdsUpdateObjectActionLC(
                                            CUpnpAction*& aAction,
                                            CUpnpDevice* aMediaServer,
                                            const TDesC8& aObjectID,
                                            const TDesC8& aCurrentTagValue,
                                            const TDesC8& aNewTagValue)
    {
    CUpnpService* service = iAVCP.Service(aMediaServer, KContentDirectory);
    if( !service )
        {
        return KErrUnknown;
        }

    aAction = service->CreateActionLC( KUpdateObject );

    if( aAction )
        {
   	    aAction->SetArgumentL( KObjectID, aObjectID );
	    aAction->SetArgumentL( KCurrentTagValue, aCurrentTagValue );
	    aAction->SetArgumentL( KNewTagValue, aNewTagValue );
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
