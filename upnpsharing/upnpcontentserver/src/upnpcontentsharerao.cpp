/*
 * Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:      CUpnpContentSharerAo class implementation
 *
 */
//  Include Files

#include "upnpcontentsharerao.h"
#include "upnpcontentserverdefs.h"
#include "upnpselectionwriter.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// FORWARD DECLARATIONS
// CONSTANTS

const TInt KPercent = 100;

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::CUpnpContentSharerAo
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpContentSharerAo::CUpnpContentSharerAo( 
    MUpnpSharingCallback* aEngine,
    MUpnpSharingAlgorithm* aSharingAlgorithm ) 
    :
    CActive(CActive::EPriorityIdle),
    iSharingRequest( NULL )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iSharingEngine = aEngine;
    iSharingAlgorithm = aSharingAlgorithm;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentSharerAo* CUpnpContentSharerAo::NewL( 
        MUpnpSharingCallback* aEngine,
        MUpnpSharingAlgorithm* aSharingAlgorithm )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUpnpContentSharerAo* self = 
        new (ELeave) CUpnpContentSharerAo( aEngine, aSharingAlgorithm );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    CActiveScheduler::Add(this);

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::~CUpnpContentSharerAo
// Destructor
// --------------------------------------------------------------------------
//
CUpnpContentSharerAo::~CUpnpContentSharerAo()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    Cancel();
    if ( iIdle )
        {
        delete iIdle;
        iIdle = NULL;
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//

void CUpnpContentSharerAo::RunL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    __LOG8_1( "CUpnpContentSharerAo::RunL, iStatus=%d", iStatus.Int() );
    
    if ( iCancel )
        {
        // if Cancel wanted, then put iError as KErrCancel
        iError = KErrCancel;
        }    
    else if ( iStatus.Int() == KErrCorrupt || 
            iStatus.Int() == KErrDiskFull || 
            iStatus.Int() == KErrNoMemory )
        {
        // if writing sharing status failed or diskspace is full or no 
        // memory, then stop the process 
        iError = iStatus.Int();
        }
               
    if( !iError && 
        ( iShareIndex < iSharingRequest->iShareArr->Count() ||
          iUnshareIndex < iSharingRequest->iUnshareArr->Count() ) )
        {
        // handle next single file share or unshare
        TInt err = DoSingleOperation();
        
        // set progress so that Ui knows the current sharing progress.
        SetProgressL(); 
        
        //start another round
        TRequestStatus* status = &iStatus; 
        User::RequestComplete( status, err );
        SetActive();
        }
    else
        { 
        //we are done, process complete using CIdle
        if ( iIdle )
            {
            delete iIdle;
            iIdle = NULL;
            }
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        iIdle->Start( TCallBack( CompleteL, this ) );
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::DoSingleOperation
// Handles one file sharing or unsharing
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharerAo::DoSingleOperation()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt err = KErrNone;
    
    // first we go through the whole share list and then unsharing 
    if ( iShareIndex < iSharingRequest->iShareArr->Count() )
        {
        
        TFileName fileName = (*(iSharingRequest->iShareArr))[iShareIndex];
        TRAP( err, iSharingAlgorithm->ShareFileL( fileName ) );
        iShareIndex++;
        if (err != KErrNone)
            {
            // Sharing failed for some reason, not doing break here.
            // Just log and try next one.
            __LOG8_1( "Sharing item error: ", err );
            }
        }
    
    // share list done, next process the unshare list one by one 
    if ( iShareIndex == iSharingRequest->iShareArr->Count() &&
            iUnshareIndex < iSharingRequest->iUnshareArr->Count() )
        {
        TFileName fileName = 
            (*(iSharingRequest->iUnshareArr))[iUnshareIndex];
        TRAP( err, iSharingAlgorithm->UnshareFileL( fileName ) );
        iUnshareIndex++;
        if (err != KErrNone)
            {
            // Unsharing failed for some reason, not doing break here.
            // Just log and try next one.
            __LOG8_1( "Unsharing item error: ", err );
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }
// --------------------------------------------------------------------------
// CUpnpContentSharerAo::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::DoCancel()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // do nothing
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::RunError
// Cancels active object
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharerAo::RunError(TInt aError)
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG8_1( "RunError = ", aError );
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::StartSharing
// Starts file sharing
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::StartSharing( 
    const CUpnpSharingRequest* aSharingRequest )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( IsActive() )
        {
        Cancel();
        }
    
    // initialize values for variables
    iSharingRequest = aSharingRequest;
    iCancel = EFalse;
    iError = KErrNone;
    iShareIndex = 0;
    iUnshareIndex = 0;
    iCurrentProgressPercent = 0;
    
    TInt err( KErrNone );
    if ( iSharingRequest->iShareArr && iSharingRequest->iUnshareArr )
        {
        __LOG2( "CUpnpContentSharerAo::StartSharing \
            shareArr.Count=%d, unshareArr.Count=%d", 
            iSharingRequest->iShareArr->Count(),
            iSharingRequest->iUnshareArr->Count() );

        // write the sharing status to cenrep and clf ids to file
        TRAP( err, WriteSharingStatusL() );

        // if writing sharing status failed, map all errors to KErrCorrupt
        // so that RunL handles the error
        if ( err )
            {
            err = KErrCorrupt;
            }
        }
    else
        {
        err = KErrCorrupt;
        }

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, err );

    SetActive();
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::SetProgressL
// Sets progress to engine
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::SetProgressL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt progress(0);
    
    // Summarize total count of shared and unshared files
    TInt totalCount = iSharingRequest->iShareArr->Count() + 
                            iSharingRequest->iUnshareArr->Count();
    // Find out how many of the files are processed already 
    TInt currentCount = iShareIndex + iUnshareIndex;
    
    // No div by zero
    if( totalCount > 0 )
        {
        // calculate percent for sharing progress
        progress = currentCount * KPercent / totalCount ;
        }
    
    if ( iCurrentProgressPercent != progress )
        {
        // Set the progress information
        iCurrentProgressPercent = progress;
        iSharingEngine->SetProgressL( progress );
        }
    else
        {
        // progress percent not changed, no need to set it
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::WriteSharingStatusL
// Writes the sharing status to cenrep and files
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::WriteSharingStatusL()
    {
    __LOG8_1( "%s start.", __PRETTY_FUNCTION__ );

    // Create selection writer
    CUpnpSelectionWriter* writer = 
            CUpnpSelectionWriter::NewL( iSharingRequest->iMediaType );
    CleanupStack::PushL( writer );
    
    // if sharing request is EShareMany, append clf ids to writer
    if( iSharingRequest->iSharingType == EShareMany )
        {
        if ( iSharingRequest->iClfIds )
            {
            for ( TInt i(0); i < iSharingRequest->iClfIds->MdcaCount(); i++ )
                {
                TPtrC collId = iSharingRequest->iClfIds->MdcaPoint(i);
                writer->AppendItemL( collId );
                }
            }
        else
            {
            // no clf ids, leave
            User::Leave( KErrCorrupt );
            }
        }
    
    // write sharing status to cenrep and/or file
    writer->SaveSharingStateL( iSharingRequest->iSharingType );
    
    CleanupStack::PopAndDestroy( writer );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::SharingOngoing
// Checks whether sharing is ongoing or not.
// --------------------------------------------------------------------------
//
TBool CUpnpContentSharerAo::SharingOngoing()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret(EFalse); 
    if ( iSharingRequest && 
        iSharingRequest->iShareArr &&
        iSharingRequest->iUnshareArr )
        {
        TInt totalCount = iSharingRequest->iShareArr->Count() + 
                                iSharingRequest->iUnshareArr->Count();
        TInt currentCount = iShareIndex + iUnshareIndex;
        if ( currentCount < totalCount )
            {
            // sharing still ongoing
            ret = ETrue;
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::StopSharing
// Stops the sharing.
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::StopSharing()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iCancel = ETrue;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }
    
// --------------------------------------------------------------------------
// CUpnpContentSharerAo::CompleteL
// Operation complete.
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharerAo::CompleteL( TAny* aPtr )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUpnpContentSharerAo* sharer = 
        static_cast<CUpnpContentSharerAo*>( aPtr );
    // callback for operation completed.
    sharer->iSharingEngine->CompleteSharingOperationL( sharer->iError, 
        sharer->iSharingRequest->iMediaType );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharerAo::ForceSetProgress
// Forces itself to set progress in next file operation
// --------------------------------------------------------------------------
//
void CUpnpContentSharerAo::ForceSetProgress()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // let's change the value of the current sharing progress so that 
    // the progress property is set in next file sharing operation.
    iCurrentProgressPercent--;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }
    
// End of file
