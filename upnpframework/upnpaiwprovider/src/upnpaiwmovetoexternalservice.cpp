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
* Description:  Implementation of the UpnpAiwMoveToExternalService
*
*/


#include <AiwCommon.hrh>
#include <AiwMenu.h>
#include <aknnotewrappers.h>
#include <upnpaiwmenuresources.rsg>
#include "upnpaiwengine.h"
#include "upnpaiwmovetoexternalservice.h"
#include "upnpdrmfilter.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// CONSTANTS
_LIT( KResFileName, "\\resource\\upnpaiwmenuresources.rsc" );

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwMoveToExternalService* CUPnPAiwMoveToExternalService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::NewL" );

    CUPnPAiwMoveToExternalService* self = 
                new (ELeave) CUPnPAiwMoveToExternalService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::CUPnPAiwMoveToExternalService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwMoveToExternalService::CUPnPAiwMoveToExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
CUPnPAiwMoveToExternalService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::~CUPnPAiwMoveToExternalService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwMoveToExternalService::~CUPnPAiwMoveToExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
~CUPnPAiwMoveToExternalService" );

    // Empty the file name array and close it
    EmptyFileNameArray();
    iFileNames.Close();

    // Release the engine instance
    if( iEngine )
        {
        CUPnPAiwEngine::ReleaseInstance();
        iEngine = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwMoveToExternalService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::ConstructL" );

    // Create the engine and register as an observer for call backs
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::InitializeMenuPaneL
// AIW Framework's method for initialising the menu
// --------------------------------------------------------------------------
void CUPnPAiwMoveToExternalService::InitializeMenuPaneL( 
                CAiwMenuPane& aMenuPane,
                TInt aIndex,
                TInt /*aCascadeId*/,
                const CAiwGenericParamList& /*aInParamList*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
InitializeMenuPaneL" );

    TUPnPEngineState engineState = iEngine->EngineState();
    if( engineState == EUPnPEngineNotConnected ||
        engineState == EUPnPEngineConnectionLost )
        {
        // Update the drive letter to the path of the resource file name
        TFileName resFile( KResFileName );
        TFileName dllName;
        Dll::FileName( dllName );
        TBuf<2> drive = dllName.Left(2); // Drive letter followed by ':' 
        resFile.Insert( 0, drive );
    
        // Add the play menu item to the menu pane
        aMenuPane.AddMenuItemsL( resFile,
                                 R_AIW_MOVE_TO_EXTERNAL_MENU,
                                 KAiwCmdUPnPMove,
                                 aIndex );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::HandleMenuCmdL
// AIW Framework's method for handling menu commands
// --------------------------------------------------------------------------
void CUPnPAiwMoveToExternalService::HandleMenuCmdL( 
                TInt aMenuCmdId,
                const CAiwGenericParamList& aInParamList,
                CAiwGenericParamList& aOutParamList,
                TUint /*aCmdOptions*/,
                const MAiwNotifyCallback* /*aCallback*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
HandleMenuCmdL" );

    // Empty the file name array
    EmptyFileNameArray();

    // Get the file name parameters
    if( aInParamList.Count() > 0 )
        {
        for( TInt index = 0; index < aInParamList.Count(); index++ )
            {
            // Get the filenames
            if ( aInParamList[index].SemanticId() == EGenericParamFile )
                {
                // Store the file name in the array
                HBufC* fileName = HBufC::NewLC( 
                    aInParamList[index].Value().AsDes().Length() );
                fileName->Des().Append( 
                    aInParamList[index].Value().AsDes() );
                iFileNames.AppendL( fileName );
                CleanupStack::Pop( fileName );
                }
            }
        }

    if( aMenuCmdId == KAiwCmdUPnPMove )
        {
        if( iEngine )
            {
            if( iFileNames.Count()>0 )
                {
                // Do the move
                TInt returnValue = KErrArgument;
                TRAP( returnValue,
                      iEngine->MoveToExternalL( iFileNames ) );

                // Do some error translation regarding DRM protected files
                if ( returnValue == KErrNotSupported ||
                    returnValue == KErrPermissionDenied )
                    {
                    returnValue = KErrAccessDenied;
                    }

                // Add the return value on the out param list
                TAiwGenericParam statusParameter( 
                                    EGenericParamError,
                                    returnValue );
                aOutParamList.AppendL( statusParameter );

                // Add the files that FAILED to copy to out param list.
                // finally, reset the array.
                for( TInt index=0; index<iFileNames.Count(); index++ )
                    {
                    if( iFileNames[index] )
                        {
                        // Create TAIWGenericParam for the filename
                        TFileName fileName( *iFileNames[index] );
                        TAiwGenericParam fileParameter( 
                                    EGenericParamFile, 
                                    fileName );

                        // Append the file filename param on the out list
                        aOutParamList.AppendL( fileParameter );
                        }
                    }
                iFileNames.ResetAndDestroy();

                }
            else
                {
                __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService, \
no files!" );
                }
            }
        else
            {
            __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService, \
engine is dead!" );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwMoveToExternalService::EmptyFileNameArray
// Empties the file name array (data member variable).
// --------------------------------------------------------------------------
void CUPnPAiwMoveToExternalService::EmptyFileNameArray()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
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
// CUPnPAiwMoveToExternalService::ConnectionLostL
// Indicates that the connection with the target UPnP device has 
// been lost.
// --------------------------------------------------------------------------
void CUPnPAiwMoveToExternalService::ConnectionLostL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwMoveToExternalService::\
ConnectionLostL" );

    // If there is a client that has registered as an observer
    if( iClient )
        {
        // Create parameter lists
        CAiwGenericParamList* emptyParamlist = CAiwGenericParamList::NewLC();

        TInt eventCode = KAiwEventStopped;

        // Make the call back
        iClient->HandleNotifyL( KAiwCmdUPnPMove,
                                eventCode,
                                *emptyParamlist,
                                *emptyParamlist );

        // Clean up
        CleanupStack::PopAndDestroy( emptyParamlist );
        emptyParamlist = NULL;
        }
    }

// End of file
