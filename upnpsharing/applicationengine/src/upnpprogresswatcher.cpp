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
* Description:      CUpnpProgressWatcher class implementation
 *
*/







//  Include Files
#include <e32property.h>
#include <e32debug.h>
#include <upnpstring.h>
#include "upnpprogresswatcher.h"
#include "upnpfilesharingengine.h"

_LIT( KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"


// CONSTANTS
// The maximum numbers of sharingrequests that can be currently received
const TInt KMaxSharingReqs = 2;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::CUpnpProgressWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpProgressWatcher::CUpnpProgressWatcher( CUPnPFileSharingEngine& aEngine )
    : CActive(CActive::EPriorityStandard),
      iEngine( aEngine )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::CUpnpProgressWatcher");
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpProgressWatcher* CUpnpProgressWatcher::NewL( 
    CUPnPFileSharingEngine& aEngine )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::NewL");
    CUpnpProgressWatcher* self = 
        new ( ELeave ) CUpnpProgressWatcher( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::ConstructL()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::ConstructL");

    User::LeaveIfError( 
        iProgressInfo.Attach( KUpnpContentServerCat, ESharingProgress ) );
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::~CUpnpProgressWatcher
// Destructor
// --------------------------------------------------------------------------
//
CUpnpProgressWatcher::~CUpnpProgressWatcher()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::~CUpnpProgressWatcher");

    Cancel();
    iProgressInfo.Close();
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::StartL
// Starts active object
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::StartL()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::StartL begin");

    TUpnpProgress totalProgress;
    TPckgBuf<TUpnpProgress> progressBuf( totalProgress );
    TInt pubErr = iProgressInfo.Get( progressBuf );
    iEngine.SetShareFileComplete( EFalse );
    if ( !pubErr )
        {
        Mem::Copy( &totalProgress, 
                   progressBuf.Ptr(), sizeof( TUpnpProgress ) );

        // update UI
        SetShowCompleteNote( totalProgress );
        TProgressInfos progressArr( KMaxSharingReqs );
        progressArr.Append( totalProgress.iImageVideoProgress );
        progressArr.Append( totalProgress.iMusicProgress );
        CleanupClosePushL( progressArr );
        if ( iEngine.Observer() )
            {
            iEngine.Observer()->HandleSharingProgress( 
                iEngine, progressArr );
            }
        CleanupStack::PopAndDestroy( &progressArr );
        }

    if ( !IsActive() && SharingOngoing( totalProgress ) )
        {
        // start periodical reporting of progress
        RunL();
        }

    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::StartL end");
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::Stop
// Stops active object
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::Stop()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::Stop");
    Cancel();
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::RunL()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::RunL");

    // resubscribe before processing new value to prevent missing updates
    iProgressInfo.Subscribe( iStatus );
    SetActive();

    TUpnpProgress totalProgress;
    TPckgBuf< TUpnpProgress > paramspkg( totalProgress );


    // get value
    if ( iProgressInfo.Get(
             KUpnpContentServerCat, 
             ESharingProgress, 
             paramspkg ) == KErrNotFound )
        {
        // property deleted, do necessary actions here...
        }
    else
        {
        Mem::Copy( &totalProgress, paramspkg.Ptr(), sizeof( TUpnpProgress ) );


        TProgressInfos progressArr( KMaxSharingReqs );

        progressArr.Append( totalProgress.iImageVideoProgress );
        progressArr.Append( totalProgress.iMusicProgress );

        CleanupClosePushL( progressArr );
        if ( totalProgress.iError )
            {
            HandleErrorL( totalProgress );
            }
        else
            {
            if ( iEngine.Observer() )
                {
                // update UI
                iEngine.Observer()->HandleSharingProgress( 
                    iEngine, progressArr );
                }
            if ( progressArr.Count() )
                {
                __LOG2( "ProgressCallbackL: cout:%d progress:%d\n",
                    progressArr.Count(), 
                    progressArr[0].iProgress );
                }
            else
                {
                __LOG( "progressArr empty" );
                }

            SetShowCompleteNote( totalProgress );

            if ( iShowCompleteNote && !SharingOngoing( totalProgress ) )
                {
                // no sharing operations ongoing, stop query
                Cancel();
                // inform UI to show final note
                if ( iEngine.Observer() && iShowCompleteNote )
                    {
                    iShowCompleteNote = EFalse;
                    iEngine.Observer()->HandleSharingDone( 
                        iEngine, KErrNone );
                    }
                }
            }
        CleanupStack::PopAndDestroy( &progressArr );
        }
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::DoCancel()
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::DoCancel");
    iEngine.SetShareFileComplete( ETrue );
    iProgressInfo.Cancel();
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::SetShowCompleteNote
// determine need to show complete note, handle errors
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::SetShowCompleteNote( 
    const TUpnpProgress& aProgress )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::SetShowCompleteNote");

    if( !iShowCompleteNote )
        {
        if( aProgress.iImageVideoProgress.iProgressType != 
            TUpnpProgressInfo::EVisualStatus ||
            aProgress.iMusicProgress.iProgressType != 
            TUpnpProgressInfo::EVisualStatus)
            {
            iShowCompleteNote = ETrue;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::SharingOngoing
// determine need to show complete note, handle errors
// --------------------------------------------------------------------------
//
TBool CUpnpProgressWatcher::SharingOngoing( const TUpnpProgress& aProgress )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::SharingOngoing");
    TBool ret( EFalse );

    if( aProgress.iImageVideoProgress.iProgressType != 
        TUpnpProgressInfo::EVisualStatus ||
        aProgress.iMusicProgress.iProgressType !=
        TUpnpProgressInfo::EVisualStatus)
        {
        ret = ETrue;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpProgressWatcher::HandleErrorL
// determine need to show complete note, handle errors
// --------------------------------------------------------------------------
//
void CUpnpProgressWatcher::HandleErrorL( const TUpnpProgress& aProgress )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpProgressWatcher::HandleIfErrorL");
    Cancel();
    if( aProgress.iError == KErrNoMemory || 
        aProgress.iError == KErrDiskFull )
        {
        // update UI
        TProgressInfos progressArr( KMaxSharingReqs );
        CleanupClosePushL( progressArr );
        progressArr.Append( aProgress.iImageVideoProgress );
        progressArr.Append( aProgress.iMusicProgress );
        
        if ( iEngine.Observer() )
            {
            iEngine.Observer()->HandleSharingProgress( 
                iEngine, progressArr );
            }
        CleanupStack::PopAndDestroy( &progressArr );
        iShowCompleteNote = EFalse;
        User::Leave( aProgress.iError );
        }
    }

// End of file
