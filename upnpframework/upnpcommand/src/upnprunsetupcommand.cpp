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
* Description:  Source file for CUpnpRunSetupCommand class.
*
*/


// INCLUDE FILES
#include <upnprunsetupcommand.h>        // CUpnpRunSetupCommand
#include "upnpcommand.h"                // CUpnpCommand
#include "upnpcommandmain.h"            // UpnpCommandMain::LoadL

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::NewL
// Creates a new UpnpCommand for running the Upnp Fw's setup wizard.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRunSetupCommand* CUpnpRunSetupCommand::NewL()
    {
    // Create new CUpnpRunSetupCommand instance
    CUpnpRunSetupCommand* self = new (ELeave) CUpnpRunSetupCommand();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::CUpnpRunSetupCommand
// Constructor
// --------------------------------------------------------------------------
//
CUpnpRunSetupCommand::CUpnpRunSetupCommand()
    {
    // No implementation
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::~CUpnpRunSetupCommand
// Destructor
// --------------------------------------------------------------------------
//
CUpnpRunSetupCommand::~CUpnpRunSetupCommand()
    {
    delete iCommand;
    iCommand = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpRunSetupCommand::ConstructL()
    {
    iCommand = UpnpCommandMain::LoadL( UpnpCommand::ECommandSetup );
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::BrowseHomeNetworkL
// Allocates Upnp Framework resources, and starts up the Upnp Fw's setup
// wizard.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRunSetupCommand::RunSetupL()
    {
    // Allocate Upnp Framework resources
    iCommand->AllocateResourcesL();

    // Execute the command
    TRAPD( executeError, iCommand->ExecuteL() );

    // Release Upnp Framework resources
    iCommand->ReleaseResources(); 

    // Leave if operation failed
    if( executeError != KErrNone )
        {
        User::Leave( executeError );
        }
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupCommand::IsAvailableL
// Returns the availability information of the command.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpRunSetupCommand::IsAvailableL()
    {
    // create a temporary plugin instance
    // then query command availability.
    TBool available = EFalse;
    TRAP_IGNORE(
        CUpnpCommand* temp = UpnpCommandMain::LoadL( UpnpCommand::ECommandSetup );
        CleanupStack::PushL( temp );
        available = temp->IsAvailableL();
        CleanupStack::PopAndDestroy( temp );
        );
    return available;
    }

// End of File
