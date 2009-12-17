/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main functions to start the Content Manager server
*
*/


// INCLUDE FILES
#include "cmserver.h"
#include "msdebug.h"


// LOCAL FUNCTION PROTOTYPES
void RunServerL();

// ---------------------------------------------------------------------------
// E32Main
//
// Module entry point
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    LOG(_L("[Cm Server]\t E32Main"));
    
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt error = KErrNoMemory;
    if ( cleanup )
        {
        TRAP( error, RunServerL() );
        delete cleanup;
        }
    __UHEAP_MARKEND;
    
    return error;
    }

// ---------------------------------------------------------------------------
// RunServerL
// 
// Constructs Active Scheduler and starts the server up & running
// ---------------------------------------------------------------------------
//
void RunServerL()
    {
    LOG(_L("[Cm Server]\t RunServerL"));
    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Create server
    CCmServer* server = CCmServer::NewLC();

    // Initialisation complete, now signal the client
    User::LeaveIfError( RThread().RenameMe( KCmServerName ) );
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    CActiveScheduler::Start();

    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( scheduler );
    }
    
// End of File
