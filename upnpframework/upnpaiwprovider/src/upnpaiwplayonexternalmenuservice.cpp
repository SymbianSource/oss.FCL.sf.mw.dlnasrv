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
* Description:  Implementation of the UpnpAiwPlayOnExternalMenuService
*
*/


#include <AiwMenu.h>
#include <AiwCommon.hrh>
#include <aknnotewrappers.h>
#include "upnpaiwengine.h"
#include "upnpaiwplayonexternalmenuservice.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalMenuService* CUPnPAiwPlayOnExternalMenuService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::NewL" );

    CUPnPAiwPlayOnExternalMenuService* self = 
        new (ELeave) CUPnPAiwPlayOnExternalMenuService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::CUPnPAiwPlayOnExternalMenuService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalMenuService::CUPnPAiwPlayOnExternalMenuService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
CUPnPAiwPlayOnExternalMenuService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::~CUPnPAiwPlayOnExternalMenuService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalMenuService::~CUPnPAiwPlayOnExternalMenuService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
~CUPnPAiwPlayOnExternalMenuService" );

    // Empty the file name array and close it
    EmptyFileNameArray();
    iFileNames.Close();

    // Delete the filename of the file that is played
    delete iFileName;
    iFileName = NULL;

    // Set the client call back reference to NULL
    iClient = NULL;

    // Release the engine instance
    if( iEngine )
        {
        CUPnPAiwEngine::ReleaseInstance();
        iEngine = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
ConstructL" );

    // Create the engine and register as an observer for call backs
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::InitializeMenuPaneL
// AIW Framework's method for initialising the menu
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::InitializeMenuPaneL(
                              CAiwMenuPane& /*aMenuPane*/,
                              TInt /*aIndex*/,
                              TInt /*aCascadeId*/,
                              const CAiwGenericParamList& /*aInParamList*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
InitializeMenuPaneL" );

    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService, \
supported anymore (after Upnp Fw 2.0)!" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::HandleMenuCmdL
// AIW Framework's method for handling menu commands
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::HandleMenuCmdL( 
                            TInt /*aMenuCmdId*/,
                            const CAiwGenericParamList& /*aInParamList*/,
                            CAiwGenericParamList& /*aOutParamList*/,
                            TUint /*aCmdOptions*/,
                            const MAiwNotifyCallback* /*aCallback*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
HandleMenuCmdL" );

    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService, \
supported anymore (after Upnp Fw 2.0)!" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::EmptyFileNameArray
// Empties the file name array (data member variable).
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::EmptyFileNameArray()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
EmptyFileNameArray" );

    // delete the list of file names and close the file name array
    for( TInt index=0; index<iFileNames.Count(); index++ )
        {
        delete iFileNames[index];
        iFileNames[index] = NULL;
        }
    iFileNames.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::PlayCompleteL
// Indicates that the play operation is complete.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::PlayCompleteL( TInt aStatus )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
PlayCompleteL" );

    // If there is a client that has registered as an observer
    if( iClient )
        {
        if( iFileName )
            {

            // Create parameter lists
            CAiwGenericParamList* eventParamList =
                                    CAiwGenericParamList::NewLC();
            CAiwGenericParamList* inParamList = 
                                    CAiwGenericParamList::NewLC();


            // Create TAIWGenericParam for the filename
            TFileName fileName( *iFileName );
            TAiwGenericParam fileParameter( EGenericParamFile, fileName );

            // Append the file filename param on the out list
            eventParamList->AppendL( fileParameter );

            // Convert the error code into AIW event code
            TInt eventCode;
            if( aStatus == KErrNone )
                {
                eventCode = KAiwEventCompleted;
                }
            else
                {
                eventCode = KAiwEventError;
                }

            // Make the call back
            iClient->HandleNotifyL( KAiwCmdUPnPPlay,
                                    eventCode,
                                    *eventParamList,
                                    *inParamList );

            // Clean up
            CleanupStack::PopAndDestroy( inParamList );
            inParamList = NULL;
            CleanupStack::PopAndDestroy( eventParamList );
            eventParamList = NULL;
            delete iFileName;
            iFileName = NULL;

            // Stop observing the AIW Engine
            if( iEngine )
                {
                iEngine->RemoveEngineObserver();
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalMenuService::ConnectionLostL
// Indicates that the connection with the target UPnP device has 
// been lost.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalMenuService::ConnectionLostL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalMenuService::\
ConnectionLostL" );

    // If there is a client that has registered as an observer
    if( iClient )
        {
        // Create parameter lists
        CAiwGenericParamList* emptyParamlist = CAiwGenericParamList::NewLC();

        TInt eventCode = KAiwEventStopped;

        // Make the call back
        iClient->HandleNotifyL( KAiwCmdUPnPPlay,
                                eventCode,
                                *emptyParamlist,
                                *emptyParamlist );

        // Clean up
        CleanupStack::PopAndDestroy( emptyParamlist );
        emptyParamlist = NULL;
        }
    }

// End of file
