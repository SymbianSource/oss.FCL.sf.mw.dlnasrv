/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the UpnpAiwOpenExternalService
*
*/


#include <AiwMenu.h>
#include <AiwCommon.hrh>
#include <aknnotewrappers.h>
#include "upnpaiwengine.h"
#include "upnpaiwopenexternalservice.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUPnPAiwOpenExternalService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwOpenExternalService* CUPnPAiwOpenExternalService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService::NewL" );

    CUPnPAiwOpenExternalService* self = 
                new (ELeave) CUPnPAiwOpenExternalService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwOpenExternalService::CUPnPAiwOpenExternalService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwOpenExternalService::CUPnPAiwOpenExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService::\
CUPnPAiwOpenExternalService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwOpenExternalService::~CUPnPAiwOpenExternalService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwOpenExternalService::~CUPnPAiwOpenExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService::\
~CUPnPAiwOpenExternalService" );

    // Release the engine instance
    if( iEngine )
        {
        CUPnPAiwEngine::ReleaseInstance();
        iEngine = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwOpenExternalService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwOpenExternalService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService::\
ConstructL" );

    // Create the engine
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwOpenExternalService::HandleServiceCmdL
// AIW Framework's method for handling service commands
// --------------------------------------------------------------------------
void CUPnPAiwOpenExternalService::HandleServiceCmdL( 
                    const TInt& aCmdId,
                    const CAiwGenericParamList& /*aInParamList*/,
                    CAiwGenericParamList& aOutParamList,
                    TUint /*aCmdOptions*/,
                    const MAiwNotifyCallback* /*aCallback*/ )
    {
    // Process the command only if the id matches
    if( aCmdId == KAiwCmdUPnPOpen )
        {
        __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService::\
HandleServiceCmdL" );
 
        TInt returnValue = KErrNone;

        if( iEngine )
            {
            TRAP( returnValue,
                  iEngine->OpenExternalMediaL() );
            }
        else
            {
            returnValue = KErrDied;
            __LOG( "[UpnpAiwProvider]\t CUPnPAiwOpenExternalService, \
engine is dead!" );
            }

        // Add the return value on the out param list
        TAiwGenericParam statusParameter( 
                            EGenericParamError, 
                            returnValue );
        aOutParamList.AppendL( statusParameter );
        }
    }

// End of file
