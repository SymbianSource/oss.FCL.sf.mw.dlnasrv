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
* Description:      Metadata Harvester server's client
*
*/







// INCLUDE FILES

#include    "mdhclientsession.h"
#include    "mdhcommon.h"
#include    "msdebug.h"


// CONSTANTS
// Number of retries to start server
const TInt KServerRetries = 2;

// =========================== LOCAL FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// StartServer
// Starts the CM Metadata Harvester server.
// Returns: TInt error: Error status of the CM Metadata Harvester startup.
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// StartServer
// Creates a new process for the server and starts it up.
// ---------------------------------------------------------------------------
//
static TInt StartServer()
    {
    LOG(_L("[CmMdh Server]\t StartServer\n"));
    
    TInt result( KErrNone );
    // create server - if one of this name does not already exist
    TFindServer findServer( KCmMdhServerName );
    TFullName name;
    if ( findServer.Next( name ) != KErrNone ) // we don't exist already
        {
        
        TRequestStatus status( KRequestPending );
        RProcess server;
        // Create the server process
        result = server.Create( KCmMdhServerExe, KNullDesC );       
        if( result != KErrNone )
            {
            return result;
            }
    
        // Process created successfully
        server.Resume(); // start it going
        server.Rendezvous( status );
        
        // Wait until the completion of the server creation
        // server signals us when it's up
        User::WaitForRequest( status );
        
        
        if( status != KErrNone )
            {
            server.Close();
            return status.Int();
            }
            
        
        // Server created successfully
        server.Close(); // we can close the handle to server process now
        }
    return result;
    }


// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// RCmMdhSession::RCmMdhSession
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
RCmMdhSession::RCmMdhSession()
    {
    }


// ---------------------------------------------------------------------------
// RCmMdhSession::Connect
// Starts and creates a session of CM Metadata Harvester 
// ---------------------------------------------------------------------------
//
TInt RCmMdhSession::Connect()
    {
    TInt tryLoop = 0;
    TInt errCode = 0;

    for ( tryLoop = 0; tryLoop < KServerRetries; tryLoop++ )
        {
        errCode = CreateSession(KCmMdhServerName, 
            TVersion( KCmMdhServerMajor, KCmMdhServerMinor, 
                    KCmMdhServerBuild ) );
        //gives MessageSlots of -1
        //this uses global pool rather than local pool

        TRACE(Print(_L("[CmMdh Server]\t RCmMdhSession::Connect New Session \
                            created with status %d\n"), errCode));
        if( errCode == KErrNotSupported )
            {
            TRACE(Print(_L("[CmMdh Server]\t Version not supported!\n") ));
            return errCode;            
            }
        if ( errCode != KErrNotFound &&  errCode != KErrServerTerminated )
            {
            return errCode;
            }

        errCode = StartServer();

        if ( errCode != KErrNone && errCode != KErrAlreadyExists )
            {
            return errCode;
            }
            
        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// RCmMdhSession::SearchMediaServers
// Searchs for neighborhood media servers
// ---------------------------------------------------------------------------
//
void RCmMdhSession::SearchMediaservers( TRequestStatus& aStatus )
    {
    LOG(_L("[CmMdh Server]\t RCmMdhSession::SearchMediaservers\n"));
    
    SendReceive(ECmMdhSearchMediaservers, TIpcArgs(), aStatus);
    }
    
    
// ---------------------------------------------------------------------------
// RCmMdhSession::Harvest
// Harvests and synchronizes metadata from found mediaservers
// ---------------------------------------------------------------------------
//
void RCmMdhSession::Harvest( TRequestStatus& aStatus )
    {
    LOG(_L("[CmMdh Server]\t RCmMdhSession::SearchMediaServers\n"));
    
    SendReceive(ECmMdhHarvest, TIpcArgs(), aStatus);
    }

// ---------------------------------------------------------------------------
// RCmMdhSession::Cancel
// ---------------------------------------------------------------------------
//
void RCmMdhSession::Cancel()
    {
    LOG(_L("[CmMdh Server]\t RCmMdhSession::Cancel\n"));
    
    SendReceive(ECmMdhCancel, TIpcArgs());
    }


    
