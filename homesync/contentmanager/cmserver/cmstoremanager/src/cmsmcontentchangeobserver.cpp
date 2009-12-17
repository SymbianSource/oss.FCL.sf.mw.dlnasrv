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
* Description:  Handles devices content change events
*
*/


#include "cmsmcontentchangeobserver.h"
#include "cmsmfileprocessingobserver.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::NewL
// ---------------------------------------------------------------------------
// 
CCmSmContentChangeObserver* CCmSmContentChangeObserver::NewL( 
    CMdESession& aSession, 
    MCmSmFileProcessingObserver& aObserver )
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::NewL() start"));    
    CCmSmContentChangeObserver* self = 
        CCmSmContentChangeObserver::NewLC( aSession, aObserver );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmContentChangeObserver* CCmSmContentChangeObserver::NewLC( 
    CMdESession& aSession,
    MCmSmFileProcessingObserver& aObserver )
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::NewLC() start"));     
    CCmSmContentChangeObserver* self = 
        new ( ELeave ) CCmSmContentChangeObserver( aSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::~CCmSmContentChangeObserver
// ---------------------------------------------------------------------------
// 
CCmSmContentChangeObserver::~CCmSmContentChangeObserver()
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::\
        ~CCmSmContentChangeObserver() start"));
    TRAP_IGNORE( iMdSSession.RemoveObjectObserverL( *this ) );
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::\
        ~CCmSmContentChangeObserver() end"));            
    }

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::StartObserversL
// ---------------------------------------------------------------------------
//
void CCmSmContentChangeObserver::StartObserversL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::StartObserversL")); 
    
    iMdSSession.AddObjectObserverL( *this );
    iStarted = ETrue;    
    }

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::IsStarted
// ---------------------------------------------------------------------------
// 
TBool CCmSmContentChangeObserver::IsStarted()
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::IsStarted")); 
    
    return iStarted;
    }

// ---------------------------------------------------------------------------
// CCmSmContentChangeObserver::HandleObjectNotification
// ---------------------------------------------------------------------------
// 
void CCmSmContentChangeObserver::HandleObjectNotification(
                                  CMdESession& /*aSession*/, 
						          TObserverNotificationType /*aType*/,
						          const RArray<TItemId>& /*aObjectIdArray*/)
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::\
    HandleObjectNotification")); 
    
    iObserver.FileProcessingStatus( ECmSmContentChanged );
    }
    
// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//            
CCmSmContentChangeObserver::CCmSmContentChangeObserver( 
    CMdESession& aSession,
    MCmSmFileProcessingObserver& aObserver ) : iMdSSession( aSession ), 
    iObserver( aObserver )
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::\
    CCmSmContentChangeObserver")); 
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmContentChangeObserver::ConstructL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmContentChangeObserver::ConstructL"));
    
    iStarted = EFalse;
    }    

// End of file
