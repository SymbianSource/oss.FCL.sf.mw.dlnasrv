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
* Description:  Implementation of the UpnpAiwCopyToExternalService
*
*/


#include <AiwCommon.hrh>
#include <AiwMenu.h>
#include <aknnotewrappers.h>
#include <upnpaiwmenuresources.rsg>
#include "upnpaiwengine.h"
#include "upnpaiwcopytoexternalservice.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// CONSTANTS
_LIT( KResFileName, "\\resource\\upnpaiwmenuresources.rsc" );

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwCopyToExternalService* CUPnPAiwCopyToExternalService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::NewL" );

    CUPnPAiwCopyToExternalService* self = 
                    new (ELeave) CUPnPAiwCopyToExternalService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::CUPnPAiwMoveToExternalService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwCopyToExternalService::CUPnPAiwCopyToExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
CUPnPAiwCopyToExternalService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::~CUPnPAiwCopyToExternalService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwCopyToExternalService::~CUPnPAiwCopyToExternalService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
~CUPnPAiwCopyToExternalService" );

    // Empty the file name array and close it
    EmptyFileNameArray();
    iFileNames.Close();

    // Delete the playlist name (if not yet deleted)
    delete iPlaylistName;

    // Release the engine instance
    if( iEngine )
        {
        CUPnPAiwEngine::ReleaseInstance();
        iEngine = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwCopyToExternalService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
ConstructL" );

    // Create the engine and register as an observer for call backs
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::InitializeMenuPaneL
// AIW Framework's method for initialising the menu
// --------------------------------------------------------------------------
void CUPnPAiwCopyToExternalService::InitializeMenuPaneL( 
                CAiwMenuPane& aMenuPane,
                TInt aIndex,
                TInt /*aCascadeId*/,
                const CAiwGenericParamList& /*aInParamList*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
InitializeMenuPaneL" );

    // Update the drive letter to the path of the resource file name
    TUPnPEngineState engineState = iEngine->EngineState();
    if( engineState == EUPnPEngineNotConnected ||
        engineState == EUPnPEngineConnectionLost )
        {

        TFileName resFile( KResFileName );
        TFileName dllName;
        Dll::FileName( dllName );
        TBuf<2> drive = dllName.Left(2); // Drive letter followed by ':' 
        resFile.Insert( 0, drive );
    
        // Add the play menu item to the menu pane
        aMenuPane.AddMenuItemsL( resFile,
                                 R_AIW_COPY_TO_EXTERNAL_MENU,
                                 KAiwCmdUPnPCopy,
                                 aIndex );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::HandleMenuCmdL
// AIW Framework's method for handling menu commands
// --------------------------------------------------------------------------
void CUPnPAiwCopyToExternalService::HandleMenuCmdL( 
                TInt aMenuCmdId,
                const CAiwGenericParamList& aInParamList,
                CAiwGenericParamList& aOutParamList,
                TUint /*aCmdOptions*/,
                const MAiwNotifyCallback* /*aCallback*/ )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
HandleMenuCmdL" );

    // Empty the file name array
    EmptyFileNameArray();

    // The playlist name (if we are doing a playlist copy)
    delete iPlaylistName;
    iPlaylistName = NULL;

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
                fileName->Des().Append( aInParamList[index].Value().AsDes() );
                iFileNames.AppendL( fileName );
                CleanupStack::Pop( fileName );
                }
            // Get the playlist name if it is not set already (only the first
            // playlist name will be used)
            if( aInParamList[index].SemanticId() == EGenericParamAlbumName &&
                !iPlaylistName )
                {
                iPlaylistName = HBufC::NewL( 
                        aInParamList[index].Value().AsDes().Length() );
                iPlaylistName->Des().Append( 
                        aInParamList[index].Value().AsDes() );
                }
            }
        }

    if( aMenuCmdId == KAiwCmdUPnPCopy )
        {
        if( iEngine )
            {
            if( iFileNames.Count()>0 )
                {
                // Do the copy
                TInt returnValue = KErrArgument;
                if( iPlaylistName == NULL )
                    {
                    // Normal copy
                    TRAP( returnValue,
                          iEngine->CopyToExternalL( iFileNames ) );
                    }
                else
                    {
                    // Playlist copy
                    TRAP( returnValue,
                          iEngine->CopyPlaylistToExternalL(
                                                    *iPlaylistName,
                                                    iFileNames ) );
                    }

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
                __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService, \
no files!" );
                }
            }
        else
            {
            __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService, \
engine is dead!" );
            }
        }

    // Clean up
    delete iPlaylistName;
    iPlaylistName = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::EmptyFileNameArray
// Empties the file name array (data member variable).
// --------------------------------------------------------------------------
void CUPnPAiwCopyToExternalService::EmptyFileNameArray()
    {
    // delete the list of file names and close the file name array
    for( TInt index=0;index< iFileNames.Count();index++ )
        {
        delete iFileNames[index];
        iFileNames[index] = NULL;
        }
    iFileNames.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPAiwCopyToExternalService::ConnectionLostL
// Indicates that the connection with the target UPnP device has
// been lost.
// --------------------------------------------------------------------------
void CUPnPAiwCopyToExternalService::ConnectionLostL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwCopyToExternalService::\
ConnectionLostL" );

    // If there is a client that has registered as an observer
    if( iClient )
        {
        // Create parameter lists
        CAiwGenericParamList* emptyParamlist = CAiwGenericParamList::NewLC();

        TInt eventCode = KAiwEventStopped;

        // Make the call back
        iClient->HandleNotifyL( KAiwCmdUPnPCopy,
                                eventCode,
                                *emptyParamlist,
                                *emptyParamlist );

        // Clean up
        CleanupStack::PopAndDestroy( emptyParamlist );
        emptyParamlist = NULL;
        }
    }

// End of file
