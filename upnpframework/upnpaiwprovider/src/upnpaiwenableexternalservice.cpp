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
* Description:  Implementation of the UpnpAiwEnableExternalService
*
*/


#include <AiwMenu.h>
#include <AiwCommon.hrh>
#include <aknnotewrappers.h>
#include <upnpaiwmenuresources.rsg>
#include "upnpaiwengine.h"
#include "upnpaiwenableexternalservice.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// CONSTANTS
_LIT( KResFileName, "\\resource\\upnpaiwmenuresources.rsc" );

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwEnableExternalService* CUPnPAiwEnableExternalService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::NewL" );

    CUPnPAiwEnableExternalService* self = 
                    new (ELeave) CUPnPAiwEnableExternalService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::CUPnPAiwEnableExternalService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwEnableExternalService::CUPnPAiwEnableExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::\
CUPnPAiwEnableExternalService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::~CUPnPAiwEnableExternalService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwEnableExternalService::~CUPnPAiwEnableExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::\
~CUPnPAiwEnableExternalService" );

    // Release the engine instance
    if( iEngine )
        {

        // If the UPnP is enabled, disable it
        TInt engineState = KErrGeneral;
        engineState = iEngine->EngineState();

        if( engineState != EUPnPEngineNotConnected )
            {
            iEngine->DisableExternal();
            }

        CUPnPAiwEngine::ReleaseInstance();
        iEngine = NULL;
        }

    // Set the client call back reference to NULL
    iClient = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwEnableExternalService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::ConstructL" );

    // Create the engine and register as an observer for call backs
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::HandleServiceCmdL
// AIW Framework's method for handling service commands
// --------------------------------------------------------------------------
void CUPnPAiwEnableExternalService::HandleServiceCmdL( 
                const TInt& aCmdId,
                const CAiwGenericParamList& /*aInParamList*/,
                CAiwGenericParamList& aOutParamList,
                TUint /*aCmdOptions*/,
                const MAiwNotifyCallback* aCallback )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::\
HandleServiceCmdL" );

    // Cast the aCallback reference to non-const and store the reference
    iClient = const_cast<MAiwNotifyCallback*>( aCallback );

    if( aCmdId == KAiwCmdUPnPEnableExternal )
        {

        TInt status = KErrNone;

        if( iEngine )
            {
            TInt engineState = KErrGeneral;
            engineState = iEngine->EngineState();

            if( engineState == EUPnPEngineNotConnected )
                {
                TRAP( status, iEngine->EnableExternalL() );
                }
            else
                {
                iEngine->DisableExternal();

                __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService, \
send stopped event." );
                
                // send STOPPED event
                CAiwGenericParamList* emptyParamlist =
                    CAiwGenericParamList::NewLC();
                if( iClient )
                    {
                iClient->HandleNotifyL( KAiwCmdUPnPPlay,
                                        KAiwEventStopped,
                                        *emptyParamlist,
                                        *emptyParamlist );
                    }
                CleanupStack::PopAndDestroy( emptyParamlist );
                emptyParamlist = NULL;
                }
            }
        else
            {
            status = KErrDied;
            __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService, \
engine is dead!" );
            }

        // Add the return value on the out param list
        TAiwGenericParam statusParameter( EGenericParamError, status );
        aOutParamList.AppendL( statusParameter );

        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::InitializeMenuPaneL
// AIW Framework's method for initialising the menu
// --------------------------------------------------------------------------
void CUPnPAiwEnableExternalService::InitializeMenuPaneL( 
                CAiwMenuPane& aMenuPane,
                TInt aIndex,
                TInt /*aCascadeId*/,
                const CAiwGenericParamList& /*aInParamList*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::\
InitializeMenuPaneL" );
    
    // Update the drive letter to the path of the resource file name
    TFileName resFile( KResFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left(2); // Drive letter followed by ':' 
    resFile.Insert( 0, drive );

    // Select the correct menu resource, depending on the state of Provider
    if( iEngine )
        {
        TUPnPEngineState engineState = iEngine->EngineState();
        if( engineState == EUPnPEngineNotConnected || 
            engineState == EUPnPEngineConnectionLost )
            {
            aMenuPane.AddMenuItemsL( resFile,
                                     R_AIW_ENABLE_EXTERNAL_MENU,
                                     KAiwCmdUPnPEnableExternal,
                                     aIndex );
            }
        else
            {
            aMenuPane.AddMenuItemsL( resFile,
                                     R_AIW_DISABLE_EXTERNAL_MENU,
                                     KAiwCmdUPnPEnableExternal,
                                     aIndex );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwEnableExternalService::HandleMenuCmdL
// AIW Framework's method for handling menu commands
// --------------------------------------------------------------------------
void CUPnPAiwEnableExternalService::HandleMenuCmdL( 
                TInt aMenuCmdId,
                const CAiwGenericParamList& /*aInParamList*/,
                CAiwGenericParamList& aOutParamList,
                TUint /*aCmdOptions*/,
                const MAiwNotifyCallback* aCallback )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService::\
HandleMenuCmdL" );

    // Cast the aCallback reference to non-const and store the reference
    iClient = const_cast<MAiwNotifyCallback*>( aCallback );

    TInt status = KErrNone;

    if( iEngine )
        {
        TInt engineState = iEngine->EngineState();

        if( aMenuCmdId == KAiwCmdUPnPEnableExternal )
            {
            if( engineState == EUPnPEngineNotConnected )
                {
                TRAP( status, iEngine->EnableExternalL() );
                }
            else
                {
                iEngine->DisableExternal();

                __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService, \
send stopped event." );

                // send STOPPED event
                CAiwGenericParamList* emptyParamlist =
                    CAiwGenericParamList::NewLC();
                if( iClient )
                    {
                iClient->HandleNotifyL( KAiwCmdUPnPPlay,
                                        KAiwEventStopped,
                                        *emptyParamlist,
                                        *emptyParamlist );
                    }
                CleanupStack::PopAndDestroy( emptyParamlist );
                emptyParamlist = NULL;
                }
            }
        }
    else
        {
        status = KErrDied;
        __LOG( "[UpnpAiwProvider]\t CUPnPAiwEnableExternalService, \
engine is dead!" );
        }

    // Add the return value on the out param list
    TAiwGenericParam statusParameter( EGenericParamError, status );
    aOutParamList.AppendL( statusParameter );
    }

// End of file
