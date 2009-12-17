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

#include    "mdhclient.h"
#include    "cmserviceobserver.h"
#include    "msdebug.h"

// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
// 
CCmMdhClient* CCmMdhClient::NewL( MCmServiceObserver& aServer )
    {
    CCmMdhClient* self = CCmMdhClient::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;   
    }
    
// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
// 
CCmMdhClient* CCmMdhClient::NewLC( MCmServiceObserver& aServer )
    {    
    CCmMdhClient* self = new ( ELeave ) CCmMdhClient( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;      
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmMdhClient::~CCmMdhClient()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::~CCmMdhClient\n"));
    Cancel();
    iMdhSession.Close();
    LOG(_L("[CmMdh Server]\t CCmMdhClient::~CCmMdhClient end\n"));
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCmMdhClient::CCmMdhClient( MCmServiceObserver& aServer ) : 
    CActive( EPriorityStandard ),
    iServer( aServer ), iState( ECmMdhClientIdle )
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::CCmMdhClient\n"));
    CActiveScheduler::Add( this ); 
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCmMdhClient::ConstructL( )
    {        
    }

// ---------------------------------------------------------------------------
// Harvest
// ---------------------------------------------------------------------------
//
TInt CCmMdhClient::Harvest()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::Harvest\n"));
    
    if ( IsActive() )
        {
        return KErrNotReady; 
        }
    
    TInt errCode = iMdhSession.Connect();
    if (errCode != KErrNone)
        {
        return errCode;
        }
    iMdhSession.SearchMediaservers( iStatus );
    SetActive();
    iState = ECmMdhClientSearching;
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// Stop
// ---------------------------------------------------------------------------
//
void CCmMdhClient::Stop()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::Stop\n"));
	Cancel();
    }	
	
// ---------------------------------------------------------------------------
// Close
// ---------------------------------------------------------------------------
//
void CCmMdhClient::Close()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::Close\n"));
	delete this;
    }	
	
// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CCmMdhClient::DoCancel()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::DoCancel\n"));
    iMdhSession.Cancel();
    iMdhSession.Close();
    iState = ECmMdhClientIdle;
    iServer.ServiceExecuted( ECmServiceHarvest, KErrCancel );
    LOG(_L("[CmMdh Server]\t CCmMdhClient::DoCancel end\n"));
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CCmMdhClient::RunL()
    {
    LOG(_L("[CmMdh Server]\t CCmMdhClient::RunL\n"));
    if ( iState == ECmMdhClientSearching && 
         iStatus == KErrNone )
        {
        iMdhSession.Harvest( iStatus );
        SetActive();
        iState = ECmMdhClientHarvesting;
        }
    else // iState == ECmMdhClientHarvesting or error in search
        {
        iMdhSession.Close();
        iState = ECmMdhClientIdle;
        iServer.ServiceExecuted ( ECmServiceHarvest, iStatus.Int() );
        }
    }

// End of file


    
