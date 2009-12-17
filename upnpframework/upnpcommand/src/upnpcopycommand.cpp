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
* Description:  Source file for CUpnpCopyCommand class.
*
*/


// INCLUDE FILES
#include <upnpcopycommand.h>            // CUpnpCopyCommand
#include "upnpcommand.h"                // CUpnpCommand
#include "upnpcommandmain.h"            // UpnpCommandMain::LoadL

// --------------------------------------------------------------------------
// CUpnpCopyCommand::NewL
// Creates a new UpnpCommand for file copying purposes.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCopyCommand* CUpnpCopyCommand::NewL()
    {
    // Create new CUpnpCopyCommand instance
    CUpnpCopyCommand* self = new (ELeave) CUpnpCopyCommand();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::CUpnpCopyCommand
// Constructor
// --------------------------------------------------------------------------
//
CUpnpCopyCommand::CUpnpCopyCommand()
    {
    // No implementation
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::~CUpnpCopyCommand
// Destructor
// --------------------------------------------------------------------------
//
CUpnpCopyCommand::~CUpnpCopyCommand()
    {
    delete iCommand;
    iCommand = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpCopyCommand::ConstructL()
    {
    iCommand = UpnpCommandMain::LoadL( UpnpCommand::ECommandCopy );
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::CopyFilesL
// Copies the given list of files to a remote Upnp Media Server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCopyCommand::CopyFilesL( CDesCArrayFlat* aFiles )
    {
    TInt status = KErrNone;

    // Check param
    if( !aFiles ||
        aFiles->Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    // Push the filenames into the file pipe
    for( TInt index=0; index<aFiles->Count(); index++ )
        {
        if( status == KErrNone )
            {
            TRAP( status,
                  iCommand->PushFileL( aFiles->MdcaPoint( index ) ) );
            }
        }

    // If all files were pushed ok
    if( status == KErrNone )
        {
        // Allocate Upnp Fw only for the duration of the command execution
        TRAP( status, iCommand->AllocateResourcesL() );
        if( status == KErrNone )
            {
            // Execute the command
            TRAP( status, iCommand->ExecuteL() );

            // Move the failed files back to client file array
            aFiles->Reset();
            for ( TInt i=0; i<iCommand->FileCount(); ++i )
                {
                TRAP_IGNORE( aFiles->AppendL( iCommand->File( i ) ) );
                }

            // Release Upnp Fw
            iCommand->ReleaseResources();
            }
        }

    // Reset the file pipe
    iCommand->ResetFiles();

    // Reset parameters
    iCommand->ResetParameters();

    // Leave if operation failed
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::CopyPlaylistL
// Copies the given playlist (playlist name + filenames) to a remote Upnp
// Media Server.
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCopyCommand::CopyPlaylistL( const TDesC& aPlaylistName,
                                               CDesCArrayFlat* aFiles )
    {
    // Check playlist name parameter, aFiles will be checked later
    if( aPlaylistName == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // Set the playlist parameter
    iCommand->SetParameterL(
        UpnpCommand::EParamCollectionName, aPlaylistName );

    // Use CopyFilesL to do the copy (and to handle cleanup)
    CopyFilesL( aFiles );
    }

// --------------------------------------------------------------------------
// CUpnpCopyCommand::IsAvailable
// Inline implementation of the IsAvailable method.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpCopyCommand::IsAvailableL()
    {
    // create a temporary plugin instance
    // then query command availability.
    TBool available = EFalse;
    TRAP_IGNORE(
        CUpnpCommand* temp = UpnpCommandMain::LoadL( UpnpCommand::ECommandCopy );
        CleanupStack::PushL( temp );
        available = temp->IsAvailableL();
        CleanupStack::PopAndDestroy( temp );
        );
    return available;
    }

// End of File
