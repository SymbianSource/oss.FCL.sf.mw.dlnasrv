/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation class for UPnP File Sharing class
*
*/
#include "upnpmediaserverstatuswatcher.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpMediaServerStatusWatcher* CUpnpMediaServerStatusWatcher::NewL( 
        MUpnpMediaServerStatusObserver& aObserver )
    {
    CUpnpMediaServerStatusWatcher *self = new ( ELeave ) 
                                     CUpnpMediaServerStatusWatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpMediaServerStatusWatcher* CUpnpMediaServerStatusWatcher::NewL( 
        RUpnpMediaServerClient& aClient, MUpnpMediaServerStatusObserver& aObserver )
    {
    CUpnpMediaServerStatusWatcher *self = new ( ELeave )
                                     CUpnpMediaServerStatusWatcher( aClient, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//                                 
CUpnpMediaServerStatusWatcher::~CUpnpMediaServerStatusWatcher()   
    {
    Cancel();
    if ( iSessionOwner )
        {
        iMediaServerClient.Close();
        }
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpMediaServerStatusWatcher::CUpnpMediaServerStatusWatcher( 
    RUpnpMediaServerClient& aClient, MUpnpMediaServerStatusObserver& aObserver )
    : CActive( CActive::EPriorityStandard ), iMediaServerClient( aClient ),
      iStatusEventObserver( aObserver )
    {    
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpMediaServerStatusWatcher::CUpnpMediaServerStatusWatcher( 
    MUpnpMediaServerStatusObserver& aObserver )
    : CActive( CActive::EPriorityStandard ), iSessionOwner( ETrue ),
      iStatusEventObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerStatusWatcher::ConstructL()
    {
    if ( iSessionOwner )
        {
        User::LeaveIfError( iMediaServerClient.Connect() );
        }
    User::LeaveIfError( iMediaServerClient.Status( iMediaServerStatus ) ); 
    CActiveScheduler::Add( this );
    Subscribe();
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerStatusWatcher::Subscribe
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpMediaServerStatusWatcher::Subscribe()
    {
    if ( !IsActive() )
        {
        iMediaServerClient.StatusChangeNotify( iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerStatusWatcher::DispatchResponseStatusEvent
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerStatusWatcher::DispatchResponseStatusEvent( TInt aStatus )
    {
    if ( iMediaServerStatus != aStatus )
        {
        iMediaServerStatus = aStatus;
        iStatusEventObserver.StatusChanged( 
            static_cast<MUpnpMediaServerStatusObserver::TServerState>( iMediaServerStatus ) );
        }    
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerStatusWatcher::RunL
// ---------------------------------------------------------------------------
//
void CUpnpMediaServerStatusWatcher::RunL()
    {
    if ( iStatus >= KErrNone )
        {
        Subscribe();
        TInt newStatus;
        TInt error  = iMediaServerClient.Status( newStatus );
        if ( error != KErrNone )
            {
            iStatusEventObserver.SubscribeError( error );
            return;
            }
        DispatchResponseStatusEvent( newStatus );        
        }
    else
        {
        iStatusEventObserver.SubscribeError( iStatus.Int() );
        }    
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerStatusWatcher::RunError
// ---------------------------------------------------------------------------
//    
TInt CUpnpMediaServerStatusWatcher::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerStatusWatcher::DoCancel
// ---------------------------------------------------------------------------
//        
void CUpnpMediaServerStatusWatcher::DoCancel()
    {
    iMediaServerClient.CancelStatusChangeNotify();
    }    

// End of file
