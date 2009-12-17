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
#include <commdb.h>
#include <d32dbms.h>
#include <WlanCdbCols.h>
#include <wlanmgmtclient.h>
#include "cmcommsdbnotifier.h"
#include "cmscheduler.h"
#include "msdebug.h"

// Two-phased constructor.
CCmCommsDbNotifier* CCmCommsDbNotifier::NewL( CCmScheduler& aScheduler )
    {
    CCmCommsDbNotifier* self = new (ELeave) CCmCommsDbNotifier( aScheduler );

    CleanupStack::PushL( self );
    CActiveScheduler::Add( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CCmCommsDbNotifier::ConstructL()
    {
    // open comms database
    iDb = CCommsDatabase::NewL();
    // assign notification request
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CCmCommsDbNotifier::CCmCommsDbNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CCmCommsDbNotifier::CCmCommsDbNotifier( CCmScheduler& aScheduler ) :
    CActive( CActive::EPriorityStandard )
    {
    LOG(_L("[Cm Scheduler]\t CCmCommsDbNotifier::CCmCommsDbNotifier()"));

    // Scheduler reads scan interval on construction, so we can
    // assume it's off
    // now
    iScanState = 0;
    iScheduler = &aScheduler;
    }

// Destructor
CCmCommsDbNotifier::~CCmCommsDbNotifier()
    {
    LOG(_L("[Cm Scheduler]\t CCmCommsDbNotifier::~CCmCommsDbNotifier()"));

    Cancel();

    delete iDb;
    }

// --------------------------------------------------------------------------
// CCmCommsDbNotifier::RequestNotification
// Requests database notification
// --------------------------------------------------------------------------
//
void CCmCommsDbNotifier::RequestNotification()
    {
    LOG(_L("[Cm Scheduler]\t CCmCommsDbNotifier::RequestNotification()"));
    if ( !IsActive() )
        {
        iDb->RequestNotification( iStatus );
        SetActive();
        }
    }


// --------------------------------------------------------------------------
// CCmCommsDbNotifier::DoCancel
// --------------------------------------------------------------------------
//
void CCmCommsDbNotifier::DoCancel()
    {
    // cancel possible requests
    iDb->CancelRequestNotification();
    }

// ---------------------------------------------------------------------------
// CCmCommsDbNotifier::RunL
// Called when change in database occurs
// ---------------------------------------------------------------------------
//
void CCmCommsDbNotifier::RunL()
    {
    LOG(_L("[Cm Scheduler]\t CCmCommsDbNotifier::RunL()"));

    if ( iScheduler &&
         iStatus.Int() == RDbNotifier::ECommit &&
         HasScanStateChangedL() )
        {
        iScheduler->WlanScanStateChanged( iScanState );
        }

    // Assign new request
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CCmCommsDbNotifier::RunError
// Called if RunL leaves
// --------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CCmCommsDbNotifier::RunError( TInt aError )
#else // _DEBUG
TInt CCmCommsDbNotifier::RunError( TInt /*aError*/ )
#endif // _DEBUG

    {
    TRACE(Print(_L("[Cm Scheduler]\t \
    CCmCommsDbNotifier::RunError errorcode = %d"), aError));

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CCmCommsDbNotifier::HasScanStateChangedL
// Checks if wlan scanning interval has been changed
// --------------------------------------------------------------------------
//
TBool CCmCommsDbNotifier::HasScanStateChangedL()
    {
    LOG(_L("[Cm Scheduler]\t CCmCommsDbNotifier::HasScanStateChangedL"));

    TBool status( EFalse );

    TUint32 scanState = WlanScanStateL();

    if ( iScanState != scanState )
        {
        // value changed
        iScanState = scanState;
        status = ETrue;
        }
    else
        {
        // value not changed
        status = EFalse;
        }

    return status;
    }

// --------------------------------------------------------------------------
// CCmCommsDbNotifier::WlanScanStateL
// Returns state of wlan scan interval setting
// --------------------------------------------------------------------------
//
TUint32 CCmCommsDbNotifier::WlanScanStateL()
    {
    TUint32 scanState( 0 );

#ifndef __WINS__

    // open wlan table
    CCommsDbTableView* view = iDb->OpenViewMatchingUintLC
        (
        TPtrC( WLAN_DEVICE_SETTINGS ),
        TPtrC( WLAN_DEVICE_SETTINGS_TYPE ), KWlanUserSettings
        );

    User::LeaveIfError( view->GotoFirstRecord() );

    // read scanning interval
    view->ReadUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ),
                     scanState );

    TRACE(Print(_L("[Cm Scheduler]\t CCmCommsDbNotifier::prev: %d, \
                scan :%d"), iScanState, scanState ));


    CleanupStack::PopAndDestroy( view );

#endif

    return scanState;
    }

// End of file
