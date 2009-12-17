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
* Description:  Source file for CUpnpBrowseTask class.
*
*/


// INCLUDE FILES
// upnpframework / avcontroller api
#include "upnpavcontrollerfactory.h"    // UPnPAVControllerFactory
#include "upnpavcontroller.h"           // MUPnPAVController

// upnpframework / commonui api's
#include "upnpcommonui.h"

// command internal
#include "upnpfilepipe.h"               // CUpnpFilePipe
#include "upnpcommandparameters.h"      // CUpnpCommandParameters
#include "upnptaskhandler.h"            // MUpnpTaskHandler
#include "upnpnotehandler.h"            // CUpnpNoteHandler
#include "upnpbrowsetask.h"
#include "upnpcommand.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpBrowseTask::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpTask* CUpnpBrowseTask::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::NewL" );

    // Create instance
    CUpnpBrowseTask* self = NULL;
    self = new (ELeave) CUpnpBrowseTask();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    // Cast the object and return
    return (CUpnpTask*)self;
    }

// --------------------------------------------------------------------------
// CUpnpBrowseTask::CUpnpBrowseTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpBrowseTask::CUpnpBrowseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::Constructor" );

    // Initialise member variables
    iAVController = NULL;
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpBrowseTask::~CUpnpBrowseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::Destructor" );
    }

// --------------------------------------------------------------------------
// CUpnpBrowseTask::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpBrowseTask::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpBrowseTask::AllocateResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpBrowseTask::AllocateResourcesL()
    {
     __LOG( "[UpnpCommand]\t CUpnpBrowseTask::AllocateResourcesL" );
 
    // Upnp Fw resources are allocated when the command is executed.
    }

// --------------------------------------------------------------------------
// CUpnpBrowseTask::ExecuteL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpBrowseTask::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::ExecuteL" );

    // Leave if the command has been allocated already
    if( iAVController )
        {
        User::Leave( KErrInUse );
        }

    // Create UpnpAvControllerClient
    iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL();

    TInt status = KErrNone;
    TInt leaveCode = KErrNone;

    // If the improved browse feature is available (introduced in Upnp Fw 2.1)
    // use it, otherwise use the old browse implementation (UpnpCommonUi).
    TRAP( leaveCode,
        CUPnPCommonUI *commonUi = CUPnPCommonUI::NewL();
        CleanupStack::PushL( commonUi );

        status = commonUi->ExecuteDeviceDialogL( *iAVController );

        // Clean up
        CleanupStack::PopAndDestroy( commonUi );
        commonUi = NULL;
        );

    // Show connection lost info note
    if( status == KErrDisconnected )
        {
        TRAP_IGNORE( NoteHandler()->ShowConnectionLostNoteL() );
        }

    // Fix UpnpCommonUi return value
    if( status > 0 )
        {
        status = KErrNone;
        }

    // If UpnpCommonUi leaved, handle the leave code (if necessary)
    if( status == KErrNone &&
        leaveCode != KErrNone )
        {
        status = leaveCode;
        }

    // Clean up
    delete iAVController;
    iAVController = NULL;

    // If there was an error, leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUpnpBrowseTask::WLANConnectionLost
// Notifies that the WLAN connection has been lost. All sessions are now
// usable and must be closed.
// --------------------------------------------------------------------------
void CUpnpBrowseTask::WLANConnectionLost()
    {
    __LOG( "[UpnpCommand]\t CUpnpBrowseTask::WLANConnectionLost" );

    // Inform the client that connection was lost.
    CommandEvent( UpnpCommand::EEventComplete, KErrDisconnected );

    // Call task handler (UpnpCommandImplementation) to destroy this task
    if( TaskHandler() )
        {
        TaskHandler()->DestroyTask();
        }
    }

// End of File
