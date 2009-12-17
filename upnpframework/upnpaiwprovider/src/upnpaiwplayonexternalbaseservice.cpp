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
* Description:  Implementation of the UpnpAiwPlayOnExternalBaseService
*
*/


#include <AiwMenu.h>
#include <AiwCommon.hrh>
#include <aknnotewrappers.h>
#include "upnpaiwengine.h"
#include "upnpaiwplayonexternalbaseservice.h"
#include "upnpdrmfilter.h"

_LIT( KComponentLogfile, "upnpaiwprovider.log" );
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::NewL
// NewL.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalBaseService* CUPnPAiwPlayOnExternalBaseService::NewL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::NewL" );

    CUPnPAiwPlayOnExternalBaseService* self = 
                    new (ELeave) CUPnPAiwPlayOnExternalBaseService;
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::CUPnPAiwPlayOnExternalBaseService
// Constructor.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalBaseService::CUPnPAiwPlayOnExternalBaseService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
CUPnPAiwPlayOnExternalBaseService" );
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::~CUPnPAiwPlayOnExternalBaseService
// Destructor.
// --------------------------------------------------------------------------
CUPnPAiwPlayOnExternalBaseService::~CUPnPAiwPlayOnExternalBaseService()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
~CUPnPAiwPlayOnExternalBaseService" );

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
    
    // Release the DRM filter AO
    delete iActiveDrmFilter;
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::ConstructL
// Second phase constructor.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalBaseService::ConstructL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
ConstructL" );

    // Create the engine and register as an observer for call backs
    iEngine = CUPnPAiwEngine::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::HandleServiceCmdL
// AIW Framework's method for handling service commands
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalBaseService::HandleServiceCmdL( 
                                const TInt& aCmdId,
                                const CAiwGenericParamList& aInParamList,
                                CAiwGenericParamList& aOutParamList,
                                TUint /*aCmdOptions*/,
                                const MAiwNotifyCallback* aCallback )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
HandleServiceCmdL" );

    // Cast the aCallback reference to non-const and store the reference
    iClient = const_cast<MAiwNotifyCallback*>(aCallback);

    // Empty the file name array
    EmptyFileNameArray();

    // Get the file name parameters
    if( aInParamList.Count() > 0 )
        {
        for( TInt index = 0; index < aInParamList.Count(); index++ )
            {
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

    // Process the command only if the command ID matches
    if( aCmdId == KAiwCmdUPnPPlay )
        {
        if( iEngine )
            {
            if( iFileNames.Count() == 1 )
                {

                // Filter out the DRM protected files
                RPointerArray<TDesC16> drmFiles;

                iActiveDrmFilter = CUpnpDrmFilter::NewL( iFileNames,
                            drmFiles );

                iActiveDrmFilter->FilterDrmL();
                
                TInt returnValue = KErrArgument;
                if( iFileNames.Count() == 1 )
                    {
                    // Delete and reallocate the filename of the file that is 
                    // going to be played
                    delete iFileName; iFileName = NULL;
                    iFileName = iFileNames[0]->AllocL();

                    // Register to observe the Engine call backs and then call
                    // Play on UPnP AIW Engine
                    iEngine->SetEngineObserver( this );
                    TRAP( returnValue, iEngine->PlayL( *iFileNames[0] ) );
                    }

                // If there were DRM protected files, return code is
                // KErrAccessDenied
                if( drmFiles.Count() > 0 )
                    {
                    returnValue = KErrAccessDenied;
                    }

                // Add the return value on the out param list
                TAiwGenericParam statusParameter( 
                                    EGenericParamError, 
                                    returnValue );
                aOutParamList.AppendL( statusParameter );

                // Add the drm protected files into the outparameter list
                for( TInt index=0; index<drmFiles.Count(); index++ )
                    {
                    if( drmFiles[index] )
                        {
                        // Create TAIWGenericParam for the filename
                        TFileName fileName( *drmFiles[index] );
                        TAiwGenericParam fileParameter( 
                                            EGenericParamFile, 
                                            fileName );

                        // Append the file filename param on the out list
                        aOutParamList.AppendL( fileParameter );

                        // Clean up
                        // Delete the file name item from the array
                        delete drmFiles[index];
                        drmFiles[index] = NULL;
                        }
                    }

                // Clean up the drm files array
                drmFiles.Reset();
                drmFiles.Close();

                // In case of play, the file item is not removed from the 
                // list if the play operation fails, so in this case the list
                // can be just emptied, no need to append the items on
                // the outparam list.
                EmptyFileNameArray();
                
                delete iActiveDrmFilter;
                iActiveDrmFilter = NULL;
                }
            else
                {
                __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService, \
invalid number of files!" );
                }
            }
        else
            {
            __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService, \
engine is dead!" );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::EmptyFileNameArray
// Empties the file name array (data member variable).
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalBaseService::EmptyFileNameArray()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
HandleServiceCmdL" );

    // delete the list of file names and close the file name array
    for( TInt index=0; index<iFileNames.Count(); index++ )
        {
        delete iFileNames[index];
        iFileNames[index] = NULL;
        }
    iFileNames.Reset();
    }

// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::PlayCompleteL
// Indicates that the play operation is complete.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalBaseService::PlayCompleteL( TInt aStatus )
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
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

            }
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPAiwPlayOnExternalBaseService::ConnectionLostL
// Indicates that the connection with the target UPnP device has 
// been lost.
// --------------------------------------------------------------------------
void CUPnPAiwPlayOnExternalBaseService::ConnectionLostL()
    {
    __LOG( "[UpnpAiwProvider]\t CUPnPAiwPlayOnExternalBaseService::\
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
        if( iEngine )
            {
            iEngine->RemoveEngineObserver();
            }
        }
    }

// End of file
