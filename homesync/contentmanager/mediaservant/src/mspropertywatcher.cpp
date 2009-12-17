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
* Description:  CMSPropertyWatcher class implementation
*
*/



//  Include Files
#include <mediaservant.rsg>

#include "mspropertywatcher.h"
#include "msdebug.h"


// ---------------------------------------------------------------------------
// CMSPropertyWatcher::CMSPropertyWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMSPropertyWatcher::CMSPropertyWatcher()
            : CActive(CActive::EPriorityStandard)
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::CMSPropertyWatcher"));
    }


// ---------------------------------------------------------------------------
// CMSPropertyWatcher::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMSPropertyWatcher* CMSPropertyWatcher::NewL()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::NewL"));

    CMSPropertyWatcher* self = CMSPropertyWatcher::NewLC();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMSPropertyWatcher* CMSPropertyWatcher::NewLC()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::NewLC"));

    CMSPropertyWatcher* self = new ( ELeave ) CMSPropertyWatcher();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::ConstructL"));

    // Define new property
    TInt err = RProperty::Define( KCmPropertyCat,
                                  KCmProperty,
                                  RProperty::EByteArray,
                                  sizeof(TCmProgressInfo));
    if (err != KErrAlreadyExists)
    {
    User::LeaveIfError(err);
    }

    User::LeaveIfError(iProgressInfo .Attach(KCmPropertyCat,KCmProperty));
    CActiveScheduler::Add(this);
    }


// Destructor
CMSPropertyWatcher::~CMSPropertyWatcher()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::~CMSPropertyWatcher"));

    Cancel();
    iProgressInfo.Close();
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::StartL
// Starts active object
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::StartL()
    {
    RunL();
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::Stop
// Stops active object
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::Stop()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::RunL
// Called when asyncronous request is ready
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::RunL()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::RunL"));

    // resubscribe before processing new value to prevent missing updates
    iProgressInfo.Subscribe( iStatus );
    SetActive();

    TCmProgressInfo info;

    // initialise values on info to remove compilation warning
    info.iService = ECmServiceNone;             // Service identifier
    info.iTotalItems = KErrNone;                // Total count of items
    info.iProcessedItems = KErrNone;            // Processed items
    info.iItemsToTransferPerService = KErrNone; // Remaining items per service
    
    TPckgBuf< TCmProgressInfo > paramspkg( info );

    // get value
    if ( iProgressInfo.Get(
            KCmPropertyCat,KCmProperty,paramspkg ) == KErrNotFound )
        {
        }
    else
        {
        // inform observer
        iObserver->PropertyChangedL(
                                paramspkg().iService,
                                paramspkg().iTotalItems,
                                paramspkg().iProcessedItems,
                                paramspkg().iItemsToTransferPerService );
        }
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::DoCancel
// Cancels active object
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::DoCancel()
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::DoCancel"));

    iProgressInfo.Cancel();
    }

// ---------------------------------------------------------------------------
// CMSPropertyWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CMSPropertyWatcher::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMSPropertyWatcher::SetObserver
// Set observer
// ---------------------------------------------------------------------------
//
void CMSPropertyWatcher::SetObserver( MMSPropertyObserver* aObserver )
    {
    LOG(_L("[MediaServant]\t CMSPropertyWatcher::SetObserver"));

    iObserver = aObserver;
    }
// End of file
