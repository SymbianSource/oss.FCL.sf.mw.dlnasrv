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
* Description:  Source file for CUpnpBrowseCommand class.
*
*/


// INCLUDE FILES
#include <upnpbrowsecommand.h>          // CUpnpBrowseCommand
#include "upnpcommand.h"                // CUpnpCommand
#include "upnpcommandmain.h"            // UpnpCommandMain::LoadL

// --------------------------------------------------------------------------
// CUpnpBrowseCommand::NewL
// Creates a new UpnpCommand for Upnp home network browsing purposes.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpBrowseCommand* CUpnpBrowseCommand::NewL()
    {
    // Create new CUpnpBrowseCommand instance
    CUpnpBrowseCommand* self = new (ELeave) CUpnpBrowseCommand();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpBrowseCommand::CUpnpBrowseCommand
// Constructor
// --------------------------------------------------------------------------
//
CUpnpBrowseCommand::CUpnpBrowseCommand()
    {
    // No implementation
    }

// --------------------------------------------------------------------------
// CUpnpBrowseCommand::~CUpnpBrowseCommand
// Destructor
// --------------------------------------------------------------------------
//
CUpnpBrowseCommand::~CUpnpBrowseCommand()
    {
    delete iCommand;
    iCommand = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpBrowseCommand::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpBrowseCommand::ConstructL()
    {
    iCommand = UpnpCommandMain::LoadL( UpnpCommand::ECommandBrowse );
    }

// --------------------------------------------------------------------------
// CUpnpBrowseCommand::BrowseHomeNetworkL
// Allocates Upnp Framework resources, and initiates Upnp home network
// browsing.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpBrowseCommand::BrowseHomeNetworkL()
    {
    // Allocate Upnp Framework resources
    iCommand->AllocateResourcesL();

    // Execute the command (Synchronous. Will not return until the user
    // stops browsing)
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
// CUpnpBrowseCommand::IsAvailableL
// Returns the availability information of the command.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpBrowseCommand::IsAvailableL()
    {
    // create a temporary plugin instance
    // then query command availability.
    TBool available = EFalse;
    TRAP_IGNORE(
        CUpnpCommand* temp = UpnpCommandMain::LoadL( UpnpCommand::ECommandBrowse );
        CleanupStack::PushL( temp );
        available = temp->IsAvailableL();
        CleanupStack::PopAndDestroy( temp );
        );
    return available;
    }

// End of File
