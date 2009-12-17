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
* Description:  Source file for CUpnpShowCommand class.
*
*/


// INCLUDE FILES
#include <upnpshowcommand.h>            // CUpnpShowCommand
#include <upnpcommandobserver.h>        // CUpnpCommandObserver
#include "upnpcommand.h"                // CUpnpCommand
#include "upnpcommandmain.h"            // UpnpCommandMain::LoadL
#include "upnpcommandcallbackadapter.h" // CUpnpCommandCallbackAdapter

// --------------------------------------------------------------------------
// CUpnpShowCommand::NewL
// Creates a new UpnpCommand for image and video showing purposes.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpShowCommand* CUpnpShowCommand::NewL()
    {
    // Create new CUpnpShowCommand instance
    CUpnpShowCommand* self = new (ELeave) CUpnpShowCommand( NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::NewL
// Creates a new UpnpCommand for image and video showing purposes.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpShowCommand* CUpnpShowCommand::NewL(
                                        MUpnpCommandObserver* aObserver )
    {
    // Check the availability
    if( !IsAvailableL() )
        {
        User::Leave( KErrNotReady );
        }

    // Create new CUpnpShowCommand instance
    CUpnpShowCommand* self = new (ELeave) CUpnpShowCommand( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::CUpnpShowCommand
// Constructor
// --------------------------------------------------------------------------
//
CUpnpShowCommand::CUpnpShowCommand( MUpnpCommandObserver* aObserver)
    {
    iObserver = NULL;
    if( aObserver )
        {
        iObserver = aObserver;
        }
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::~CUpnpShowCommand
// Destructor
// --------------------------------------------------------------------------
//
CUpnpShowCommand::~CUpnpShowCommand()
    {
    delete iCommand;
    iCommand = NULL;
    delete iAdapter;
    iAdapter = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpShowCommand::ConstructL()
    {
    iCommand = UpnpCommandMain::LoadL( UpnpCommand::ECommandShow );

    // Register to observer
    if( iObserver )
        {
        iAdapter = new (ELeave) CUpnpCommandCallbackAdapter( iObserver );
        iCommand->SetObserver( iAdapter );
        }
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::StartShowingL
// Starts showing. Allocates Upnp Framework resources.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpShowCommand::StartShowingL()
    {
    // Allocate Upnp Fw resources
    iCommand->AllocateResourcesL();
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::StopShowingL
// Stops showing. Releases all Upnp Framework resources.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpShowCommand::StopShowingL()
    {
    // Release Upnp Fw resources
    iCommand->ReleaseResources();

    // Reset the file pipe
    iCommand->ResetFiles(); 
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::ShowImageL
// Shows the given images on a remote Upnp Media Rendering device.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpShowCommand::ShowImageL( const TDesC& aFilename )
    {
    // Check param
    if( aFilename == KNullDesC )
        {
        User::Leave( KErrArgument );
        }


    // Push the filename into the parameter pipe
    iCommand->PushFileL( aFilename ); 

    // Execute the command
    iCommand->ExecuteL();
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::ShowVideoL
// Shows the given video on a remote Upnp Media Rendering device.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpShowCommand::ShowVideoL( const TDesC& aFilename )
    {
    // Check param
    if( aFilename == KNullDesC )
        {
        User::Leave( KErrArgument );
        }


    // Push the filename into the file pipe
    iCommand->PushFileL( aFilename ); 

    // Execute the command
    iCommand->ExecuteL(); 
    }

// --------------------------------------------------------------------------
// CUpnpShowCommand::IsAvailableL
// Returns the availability information of the command.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpShowCommand::IsAvailableL()
    {
    // create a temporary plugin instance
    // then query command availability.
    TBool available = EFalse;
    TRAP_IGNORE(
        CUpnpCommand* temp = UpnpCommandMain::LoadL( UpnpCommand::ECommandShow );
        CleanupStack::PushL( temp );
        available = temp->IsAvailableL();
        CleanupStack::PopAndDestroy( temp );
        );
    return available;
    }

// End of File
