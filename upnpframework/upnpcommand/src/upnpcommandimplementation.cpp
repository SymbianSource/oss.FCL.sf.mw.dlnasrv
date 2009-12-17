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
* Description:  Source file for CUpnpCommandImplementation class.
*
*/


// INCLUDE FILES
// system
#include <centralrepository.h>

// upnpframework / avcontroller helper api
#include "upnpfileutility.h"                    // IsFileProtectedL()

// upnpframework / command api
#include "upnpcommand.h"                        // CUpnpCommand
#include "upnpcommandcallback.h"                // MUpnpCommandCallback

// command internal
#include "upnpcommandimplementation.h"
#include "upnpfilepipe.h"
#include "upnpcommandparameters.h"
#include "upnpnotehandler.h"
#include "upnpshowtask.h"
#include "upnpcopytask.h"
#include "upnpmovetask.h"
#include "upnpbrowsetask.h"
#include "upnprunsetuptask.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// CONSTANTS
const TUid KCRUidUPnPApplication = {0x20009cae};
const TUint32 KUPnPAppAccessPointSetting = 0x00000001;


// --------------------------------------------------------------------------
// CUpnpCommandImplementation::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpCommandImplementation* CUpnpCommandImplementation::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::NewL" );
    
    CUpnpCommandImplementation* self = NULL;
    self = new (ELeave) CUpnpCommandImplementation();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::CUpnpCommandImplementation
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpCommandImplementation::CUpnpCommandImplementation()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::Constructor" );

    // Set command ID to unknown by default. Client is forced to set this
    // (inline code in the NewL method of CUpnpCommand interface class).
    iCommandId = UpnpCommand::ECommandUndefined;

    // By default there is no task
    iTask = NULL;

    // By default the state is IDLE (resources not allocated nor executing)
    iState = UpnpCommand::EStateIdle;

    // DRM note will be shown only once
    iDrmNoteShown = EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::ConstructL
// Second phase construction.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ConstructL" );

    iFilePipe = CUpnpFilePipe::NewL();
    iParameters = CUpnpCommandParameters::NewL();
    iNoteHandler = CUpnpNoteHandler::NewL();
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpCommandImplementation::~CUpnpCommandImplementation()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::Destructor" );

    delete iTask;
    iTask = NULL;
    delete iFilePipe;
    iFilePipe = NULL;
    delete iParameters;
    iParameters = NULL;
    delete iNoteHandler;
    iNoteHandler = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::SetCommandIdL
// Sets the command ID.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::SetCommandIdL(
    UpnpCommand::TUpnpCommandId aCommandId )
    {
    __LOG1( "[UpnpCommand]\t CUpnpCommandImplementation::SetCommandIdL %d",
        TInt( aCommandId ) );

    // command ID can only be set ONCE !
    __ASSERTD( iCommandId == UpnpCommand::ECommandUndefined,
        __FILE__, __LINE__ );
    if( iCommandId != UpnpCommand::ECommandUndefined )
        {
        User::Leave( KErrServerBusy );
        }

    iCommandId = aCommandId;

    // If the command is to Show images & video, re-create the file pipe.
    // In this case we are using a file pipe of a limited size (1)
    if( iCommandId == UpnpCommand::ECommandShow )
        {
        delete iFilePipe;
        iFilePipe = NULL; // In case the following method leaves
        iFilePipe = CUpnpFilePipe::NewL( 1 );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::SetObserver
// Sets the callback interface
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::SetObserver(
    MUpnpCommandCallback* aCallback )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::SetObserver" );

    // Set the observer
    iParameters->SetObserver( aCallback );
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::IsAvailableL
// Checks if a command is available for execution
// --------------------------------------------------------------------------
//
TBool CUpnpCommandImplementation::IsAvailableL()
    {
    return IsAvailableL( iCommandId );
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::IsAvailableL
// Checks if a command is available for execution
// --------------------------------------------------------------------------
//
TBool CUpnpCommandImplementation::IsAvailableL(
    UpnpCommand::TUpnpCommandId aCommandId )
    {
    switch( aCommandId )
        {
        case UpnpCommand::ECommandShow: // flow through
        case UpnpCommand::ECommandCopy: // flow through
        case UpnpCommand::ECommandMove: // flow through
        case UpnpCommand::ECommandBrowse:
            {
            // available if upnp is configured
            return IsUpnpConfiguredL();
            }
        case UpnpCommand::ECommandSetup:
            {
            // setup is always available
            return ETrue;
            }
        default:
            __PANICD( __FILE__, __LINE__ );
        }

    return EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::AllocateResourcesL
// Allocates the Upnp Framework resources.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::AllocateResourcesL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::AllocateResourcesL" );

    // Leave if a task is already going (resources allocated)
    if( iTask )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Instantiate a task according to the command ID
    if( iCommandId == UpnpCommand::ECommandShow )
        {
        iTask = CUpnpShowTask::NewL();
        }
    else if( iCommandId == UpnpCommand::ECommandCopy )
        {
        iTask = CUpnpCopyTask::NewL();
        }
    else if( iCommandId == UpnpCommand::ECommandMove )
        {
        iTask = CUpnpMoveTask::NewL();
        }
    else if( iCommandId == UpnpCommand::ECommandBrowse )
        {
        iTask = CUpnpBrowseTask::NewL();
        }
    else if( iCommandId == UpnpCommand::ECommandSetup )
        {
        iTask = CUpnpRunSetupTask::NewL();
        }
    else
        {
        __PANICD( __FILE__, __LINE__ );
        User::Leave( KErrNotSupported );
        }

    // Set the pointers for the task (using base class CUpnpTask methods)
    iTask->SetTaskHandlerL( this );
    iTask->SetCommandParametersL( iParameters );
    iTask->SetFilePipeL( iFilePipe );
    iTask->SetNoteHandlerL( iNoteHandler );

    // Allocate the resources
    TRAPD( allocateError, iTask->AllocateResourcesL() );
    
    // If allocating resources failed, delete the task and forward the
    // leave code.
    if( allocateError != KErrNone )
        {
        __LOG1( "[UpnpCommand]\t CUpnpCommandImplementation::AllocateResourcesL \
failed %d", allocateError );
        // show note only if operation was not cancelled by user
        if( allocateError != KErrCancel )
            {
            TRAP_IGNORE( iNoteHandler->ShowConnectionLostNoteL() );
            }
            
        delete iTask;
        iTask = NULL;
        iState = UpnpCommand::EStateIdle;
        User::Leave( allocateError );
        }

    // Update the state
    iState = UpnpCommand::EStateAllocated;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::ReleaseResources
// Releases the Upnp Framework resources.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::ReleaseResources()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ReleaseResources" );

    // Destroy the task
    if( iTask )
        {
        delete iTask;
        iTask = NULL;
        }

    // Update the state
    iState = UpnpCommand::EStateIdle;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::ExecuteL
// Executes the command.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ExecuteL" );

    // Allocates Upnp Fw resources if not yet allocated
    if( !iTask )
        {
	    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ExecuteL task deleted, leave" );
        User::Leave( KErrNotReady );
        }

    // Update the state
    iState = UpnpCommand::EStateExecuting;

    // Execute the task
    TInt status = KErrNone;
    TRAP( status, iTask->ExecuteL() );

    // Update the state
    iState = UpnpCommand::EStateAllocated;

    // If operation failed, leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::SetParameterL
// Set a parameter.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::SetParameterL(
    UpnpCommand::TUpnpParameterType aParamType,
    const TDesC& aParamValue )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::SetParameterL" );
    __ASSERTD( iParameters!=0, __FILE__, __LINE__ );

    iParameters->SetL( aParamType, aParamValue );
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::Parameter
// --------------------------------------------------------------------------
//
const TDesC& CUpnpCommandImplementation::Parameter(
    UpnpCommand::TUpnpParameterType aParamType )
    {
    __ASSERTD( iParameters!=0, __FILE__, __LINE__ );
    return iParameters->Get( aParamType );
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::ResetParameters
// Resets parameteres.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::ResetParameters()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ResetParameters" );

    // Reset parameters
    iParameters->Reset();
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::PushFileL
// Pushes one file into the file pipe.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::PushFileL( const TDesC& aFilename )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::PushFileL" );
        
    // Push it in the file pipe
    iFilePipe->PushL( aFilename );
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::CountFiles
// --------------------------------------------------------------------------
//
TInt CUpnpCommandImplementation::FileCount()
    {
    return iFilePipe->Count();
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::File
// Pops oldest file from the file pipe.
// --------------------------------------------------------------------------
//
const TDesC& CUpnpCommandImplementation::File( TInt aIndex )
    {
    return iFilePipe->FileAt( aIndex );
    }


// --------------------------------------------------------------------------
// CUpnpCommandImplementation::ResetFiles
// Resets files.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::ResetFiles()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::ResetFiles" );

    // Reset files
    iFilePipe->Reset();
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::State
// Returns the state of the command.
// --------------------------------------------------------------------------
//
UpnpCommand::TUpnpCommandState CUpnpCommandImplementation::State()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::State" );

    return iState;
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::DestroyTask
// Destroys the ongoing task.
// --------------------------------------------------------------------------
//
void CUpnpCommandImplementation::DestroyTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandImplementation::DestroyTask" );

    // Release resources (delete the ongoing UpnpTask).
    ReleaseResources();
    }

// --------------------------------------------------------------------------
// CUpnpCommandImplementation::IsUpnpConfigured
// --------------------------------------------------------------------------
//
TBool CUpnpCommandImplementation::IsUpnpConfiguredL()
    {
    TBool returnValue = EFalse;
    // Access the Upnp Fw central repository key
    CRepository* repository = NULL;
    TRAPD( error, repository = CRepository::NewL( KCRUidUPnPApplication ) );

    if ( error == KErrNone )
        {
        // Read the IAP setting
        TInt iapDefined = KErrNotFound;
        TInt getError = repository->Get( KUPnPAppAccessPointSetting,
                                         iapDefined );
    
        delete repository;
    
        // Define the return value (min. valid IAP Id value is 1,
        // 0=None selected)
        if( getError == KErrNone &&
            iapDefined > 0 )
            {
            returnValue = ETrue;
            }
        }
    
    return returnValue;
    }

// End of File
