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
* Description:  CMSMemoryCardMonitor implementation
*
*/

#include <driveinfo.h>

#include "msmemorycardmonitor.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::NewL
// ---------------------------------------------------------------------------
//
CMSMemoryCardMonitor* CMSMemoryCardMonitor::NewL( 
    MMSMemoryCardObserver* aObserver, RFs& aRfs )
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::NewL"));
    
    CMSMemoryCardMonitor* self = CMSMemoryCardMonitor::NewLC( aObserver,
                                                              aRfs );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::NewLC
// ---------------------------------------------------------------------------
//
CMSMemoryCardMonitor* CMSMemoryCardMonitor::NewLC( 
    MMSMemoryCardObserver* aObserver, RFs& aRfs )
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::NewLC"));
    
    CMSMemoryCardMonitor* self = 
        new(ELeave) CMSMemoryCardMonitor( aObserver, aRfs );
    CleanupStack::PushL( self );
    self->ConstructL();
        
    return self;
    }    

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::ConstructL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMSMemoryCardMonitor::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::CMSMemoryCardMonitor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMSMemoryCardMonitor::CMSMemoryCardMonitor( MMSMemoryCardObserver* aObserver,
                                               RFs& aRfs ) 
                                            : CActive( EPriorityIdle ), 
                                              iObserver( aObserver ),
                                              iFsSession( aRfs ),
                                              iMemoryCardExist( EFalse )
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::CMSMemoryCardMonitor"));
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::~CMSMemoryCardMonitor
// Destructor
// ---------------------------------------------------------------------------
//
CMSMemoryCardMonitor::~CMSMemoryCardMonitor()
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::~CMSMemoryCardMonitor"));
    
    Cancel();
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::DoCancel
// cancel monitor
// ---------------------------------------------------------------------------
//
void CMSMemoryCardMonitor::DoCancel()
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::DoCancel() start"));
    iFsSession.NotifyChangeCancel();
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::DoCancel() end"));
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::RunL
// RunL
// ---------------------------------------------------------------------------
//
void CMSMemoryCardMonitor::RunL()
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::RunL() start"));
    TMemoryCardEvent mmcevent( EMemoryCardOthers );
    TBool mmc = MemoryCardExist();

    TRACE(Print(_L("[MediaServant]\t CMSMemoryCardMonitor::Runl %d-->%d\n"),
        iMemoryCardExist, mmc ));

    if( iMemoryCardExist && !mmc )
        {
        mmcevent = EMemoryCardRemove;
        }
    else if( !iMemoryCardExist && mmc )
        {
        mmcevent = EMemoryCardInsert;
        }

    if( mmcevent != EMemoryCardOthers && iObserver )
        {
        iObserver->MemoryCardChangedL( mmcevent );
        }

    StartMonitor();
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::RunError
// RunError
// ---------------------------------------------------------------------------
//
TInt CMSMemoryCardMonitor::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::StartMonitor
// start
// ---------------------------------------------------------------------------
//
void CMSMemoryCardMonitor::StartMonitor()
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::StartMonitor"));
    if ( IsActive() ) 
        {
        Cancel();
        }
    iMemoryCardExist = MemoryCardExist();
    iFsSession.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CMSMemoryCardMonitor::MemoryCardExist
// Get the mmc status
// ---------------------------------------------------------------------------
//
TBool CMSMemoryCardMonitor::MemoryCardExist() const
    {
    LOG(_L("[MediaServant]\t CMSMemoryCardMonitor::MemoryCardExist"));
    
    TBool mmcstatus( EFalse );
    TDriveList list;
    iFsSession.DriveList( list );

    for( TInt driveNumber = 0 ; driveNumber <= EDriveZ ; driveNumber++ )
        {
        if( list[driveNumber] ) // Is memorycard available
            {
            TDriveInfo driveInfo;
            iFsSession.Drive( driveInfo, driveNumber );
            TUint drvStatus( 0 );
            User::LeaveIfError( DriveInfo::GetDriveStatus( iFsSession,
                                                          driveNumber,
                                                          drvStatus ) );
            TInt removableStatus = drvStatus & DriveInfo::EDriveRemovable;

            // Check if drive is memory card
            if( removableStatus && ( driveInfo.iType != EMediaNotPresent ) )
                {
                mmcstatus = ETrue;
                driveNumber = EDriveZ + 1;
                }
            }
        }
    return mmcstatus;
    }

// End of File
