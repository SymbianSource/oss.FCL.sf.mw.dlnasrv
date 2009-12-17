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
* Description:  Checks if wlan scan state changes and informs observer of it
*
*/


//  Include Files
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#include "cmscheduler.h"
#include "cmcenrepnotifier.h"
#include "msdebug.h"

// Two-phased constructor.
CCmCenrepNotifier* CCmCenrepNotifier::NewL( CCmScheduler& aScheduler )
    {
    CCmCenrepNotifier* self = new (ELeave) CCmCenrepNotifier( aScheduler );

    CleanupStack::PushL( self );
    CActiveScheduler::Add( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CCmCenrepNotifier::ConstructL()
    {
    // open central repository
    iRepository = CRepository::NewL( KCRUidProfileEngine );
    // assign notification request
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CCmCenrepNotifier::CCmCenrepNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CCmCenrepNotifier::CCmCenrepNotifier( CCmScheduler& aScheduler ) :
    CActive( CActive::EPriorityStandard ), iScheduler( aScheduler )
    {
    LOG(_L("[Cm Scheduler]\t CCmCenrepNotifier::CCmCenrepNotifier()"));
    }

// Destructor
CCmCenrepNotifier::~CCmCenrepNotifier()
    {
    LOG(_L("[Cm Scheduler]\t CCmCenrepNotifier::~CCmCenrepNotifier()"));

    Cancel();
    delete iRepository;
    }

// --------------------------------------------------------------------------
// CCmCenrepNotifier::RequestNotification
// Requests database notification
// --------------------------------------------------------------------------
//
void CCmCenrepNotifier::RequestNotification()
    {
    LOG(_L("[Cm Scheduler]\t CCmCenrepNotifier::RequestNotification()"));
    if ( !IsActive() )
        {
        iRepository->NotifyRequest( KProEngActiveProfile, iStatus );
        SetActive();
        }
    }


// --------------------------------------------------------------------------
// CCmCenrepNotifier::DoCancel
// --------------------------------------------------------------------------
//
void CCmCenrepNotifier::DoCancel()
    {
    // cancel possible requests
    iRepository->NotifyCancelAll();
    }

// ---------------------------------------------------------------------------
// CCmCenrepNotifier::RunL
// Called when change in database occurs
// ---------------------------------------------------------------------------
//
void CCmCenrepNotifier::RunL()
    {
    LOG(_L("[Cm Scheduler]\t CCmCenrepNotifier::RunL()"));

    // Get ID of current profile
    TInt profile( 0 );
    User::LeaveIfError( GetCurrentProfile( profile ) );

    iScheduler.ProfileChangedL( profile );

    // Assign new request
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CCmCenrepNotifier::RunError
// Called if RunL leaves
// --------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CCmCenrepNotifier::RunError( TInt aError )
#else // _DEBUG
TInt CCmCenrepNotifier::RunError( TInt /*aError*/ )
#endif // _DEBUG
    {
    TRACE(Print(_L("[Cm Scheduler]\t \
    CCmCenrepNotifier::RunError errorcode = %d"), aError));

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CCmCenrepNotifier::GetCurrentProfile
// --------------------------------------------------------------------------
//
TInt CCmCenrepNotifier::GetCurrentProfile( TInt& aProfile )
    {
    LOG(_L("[Cm Scheduler]\t CCmCenrepNotifier::GetCurrentProfile()"));

    return iRepository->Get( KProEngActiveProfile, aProfile );
    }
	
// End of file
