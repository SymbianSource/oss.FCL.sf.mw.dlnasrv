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
* Description:  File transfer observer
*
*/


// INCLUDE FILES
#include "upnpfiletransfertimerobserver.h"

// -----------------------------------------------------------------------------
// CFileTransferTimer::CUpnpFileTransferTimerObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpFileTransferTimerObserver::CUpnpFileTransferTimerObserver (
                            MUpnpFileTransferTimerObserver* aCd,
                            TInt aTransferId)
    {
    iCd = aCd;
    iTransferId = aTransferId;
    }
// -----------------------------------------------------------------------------
// CUpnpFileTransferTimerObserver::ConstructL
// Two-phased constructor.
// Initialize variables, set parent service and create XML reader.
// -----------------------------------------------------------------------------
//
void CUpnpFileTransferTimerObserver::ConstructL()
{
    iEventTimer = CUpnpNotifyTimer::NewL( this );
    iEventTimer->After( KFileTransferStorageTime, EFalse );
}

// -----------------------------------------------------------------------------
// CUpnpFileTransferTimerObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpFileTransferTimerObserver* CUpnpFileTransferTimerObserver::NewLC(
                                MUpnpFileTransferTimerObserver* aCd, TInt aTransferId )
{
    CUpnpFileTransferTimerObserver* self = 
    new (ELeave) CUpnpFileTransferTimerObserver( aCd, aTransferId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpFileTransferTimerObserver destructor
// -----------------------------------------------------------------------------
//
CUpnpFileTransferTimerObserver::~CUpnpFileTransferTimerObserver()
{
    if ( iEventTimer )
    {
        iEventTimer->Cancel();
    }
    delete iEventTimer;
}

// -----------------------------------------------------------------------------
// CUpnpFileTransferTimerObserver::TimerEventL
//  implements MUpnpNotifyTimerObserver interface.
// -----------------------------------------------------------------------------
//
void CUpnpFileTransferTimerObserver::TimerEventL( 
                              CUpnpNotifyTimer* aTimer
                              )
{
    if (aTimer == iEventTimer )
    {
       iCd->TransferEvent(this);
    }
}
// -----------------------------------------------------------------------------
// CUpnpFileTransferTimerObserver::TransferId
//  implements MUpnpNotifyTimerObserver interface.
// -----------------------------------------------------------------------------
//
TInt CUpnpFileTransferTimerObserver::TransferId()
{
    return iTransferId;
}

//end of file
