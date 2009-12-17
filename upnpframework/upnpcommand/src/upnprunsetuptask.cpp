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
* Description:  Source file for CUpnpRunSetupTask class.
*
*/


// FORWARD DECLARATIONS
class CUpnpIcon;

// INCLUDE FILES
// upnpframework / setup wizard
#include "cupnpappwizard.h"             // CUPnPAppWizard
#include "upnpfilesharingengine.h"      // CUPnPFileSharingEngine
// command internal
#include "upnpnotehandler.h"
#include "upnprunsetuptask.h"
#include "upnpcommand.h"


_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"


// --------------------------------------------------------------------------
// CUpnpRunSetupTask::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpTask* CUpnpRunSetupTask::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::NewL" );

    // Create instance
    CUpnpRunSetupTask* self = NULL;
    self = new (ELeave) CUpnpRunSetupTask();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    // Cast the object and return
    return (CUpnpTask*)self;
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupTask::CUpnpRunSetupTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpRunSetupTask::CUpnpRunSetupTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpRunSetupTask::~CUpnpRunSetupTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::Destructor" );
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupTask::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpRunSetupTask::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupTask::AllocateResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpRunSetupTask::AllocateResourcesL()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::AllocateResourcesL" );

    // Upnp Fw resources are allocated when the command is executed.
    }

// --------------------------------------------------------------------------
// CUpnpRunSetupTask::ExecuteL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpRunSetupTask::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpRunSetupTask::ExecuteL" );

    // Create file sharing engine, it is needed to run the wizard.
    CUPnPFileSharingEngine* sharingEngine = CUPnPFileSharingEngine::NewL();
    CleanupStack::PushL( sharingEngine );

    // Create app wizard (use default text for the first step)
    CUPnPAppWizard* wizard = CUPnPAppWizard::NewL( KNullDesC,
                                                   sharingEngine );
    CleanupStack::PushL( wizard );

    // Run the wizard
    wizard->StartL();

    // Clean up
    CleanupStack::PopAndDestroy( wizard );
    wizard = NULL;
    CleanupStack::PopAndDestroy( sharingEngine );
    sharingEngine = NULL;
    }

// End of File
