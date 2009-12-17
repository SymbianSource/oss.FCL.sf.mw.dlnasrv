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
* Description:      CUPnPFileSharingEngine class implementation
 *
*/






//  Include Files
#include <upnpcontainer.h>
#include <f32file.h>
#include <upnpstring.h>
#include <s32file.h>

#include <upnpmediaserverclient.h>
#include "upnpfilesharingengine.h"
#include "upnpfilesharingengineao.h"
#include "upnpcontentrequestao.h"
#include "upnpcontentshareao.h"
#include "upnpcontentserverclient.h"
#include "upnpsettingsengine.h"
#include "upnpprogresswatcher.h"
#include "upnpcommonutils.h"
#include "upnpdownloaditemresolver.h"
#include "upnpcontentshareao.h"
#include "upnpconnectionmonitor.h"
#include "upnpiconconfigurator.h"

_LIT( KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::CUPnPFileSharingEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPFileSharingEngine::CUPnPFileSharingEngine()
    {
    iShareFileState = EFalse;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::ConstructL()
    {
    iContentServer.OpenL();
    iAO = new (ELeave) CUPnPFileSharingEngineAO( *this, iContentServer );
    iContentSharer = CUpnpContentShareAO::NewL( *this, iContentServer );    
    UPnPCommonUtils::DeleteTempFilesL();
    CUPnPDownloadItemResolver::DeleteTempDownloadFilesL();
    iContentRequestor = new (ELeave) CUpnpContentRequestAO( 
        *this, iContentServer );

    // create connection monitor, so we can receive wlan lost notifications
    // immideatly and observe connection if application was restarted 
    CUPnPSettingsEngine* settings = CUPnPSettingsEngine::NewLC();
    TInt iap( 0 );
    User::LeaveIfError( settings->GetAccessPoint( iap ) );
    CleanupStack::PopAndDestroy( settings );
    iConnMon = CUPnPConnectionMonitor::NewL( *this, iap );

    //set flag that icons hasn't been set
    CUPnPIconConfigurator::MarkIconsAddedL( EFalse );
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPFileSharingEngine* CUPnPFileSharingEngine::NewL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::NewL begin");
    CUPnPFileSharingEngine* self = CUPnPFileSharingEngine::NewLC();
    CleanupStack::Pop( self );
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::NewL end");
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPFileSharingEngine* CUPnPFileSharingEngine::NewLC()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::NewLC begin");
    CUPnPFileSharingEngine* self = new (ELeave) CUPnPFileSharingEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::NewLC end");
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::~CUPnPFileSharingEngine
// Destructor
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPFileSharingEngine::~CUPnPFileSharingEngine()
    {
    __LOG("[UPNP_ENGINE]\tCUPnPFileSharingEngine::~CUPnPFileSharingEngine\
 begin");

    delete iConnMon;
    delete iAO;
    delete iContentSharer;

    delete iImageSelections;
    delete iMusicSelections;
    delete iContentRequestor;


    iContentServer.Close();
    delete iProgress;

    __LOG("[UPNP_ENGINE]\tCUPnPFileSharingEngine::~CUPnPFileSharingEngine\
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SetObserver
// Set observer
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::SetObserver(
    MUPnPFileSharingEngineObserver* aObserver )
    {
    iObserver = aObserver;
    if ( !iObserver )
        {
        if ( iProgress )
            {
            iProgress->Stop();
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::Observer
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
EXPORT_C MUPnPFileSharingEngineObserver* CUPnPFileSharingEngine::Observer()
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SetSharingState
// Starts/Stops file sharing using UPnP Media server API
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::SetSharingStateL( TBool aState )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SetSharingState \
begin");
    if ( iProgress && iShareFileState )
        {
        iProgress->Stop();
        }
    iAO->SetSharingStateL( aState );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SetSharingState \
end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SharingState
// Gets file sharing status.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUPnPFileSharingEngine::SharingStateL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SharingState \
begin");

    TBool status( EFalse );
    if ( iAO )
        {
        status = iAO->SharingStateL();
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SharingState \
end");
    return status;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::ShareMediaL
// Starts sharing process
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::ShareMediaL(
    THomeConnectMediaType aType )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::ShareMediaL begin");

    switch ( aType )
        {
    case EImageAndVideo :
        {
        iContentSharer->ChangeSharedContentL( 
            UpnpContentServer::TUpnpMediaType( aType ),
            *iImageSelections );
        }
        break;
    case EPlaylist:
        {
        iContentSharer->ChangeSharedContentL( 
            UpnpContentServer::TUpnpMediaType( aType ),
            *iMusicSelections );
        }
        break;
    default :
        User::Leave( KErrArgument );
        break;
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::ShareMediaL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::RefreshSharedMediaL
// Updates shared objects
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::RefreshSharedMediaL( 
    THomeConnectMediaType aType )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RefreshSharedMediaL\
 begin");

    // clear UI cache
    delete iImageSelections;
    iImageSelections = NULL;
    delete iMusicSelections;
    iMusicSelections = NULL;


    iContentSharer->RefreshSharedContentL( 
        UpnpContentServer::TUpnpMediaType( aType ) );
    RequestSharingProgressL();

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RefreshSharedMediaL\
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::RequestSelectionIndexesL
// Gets the selected albums/playlist indexes
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::RequestSelectionIndexesL(
    THomeConnectMediaType aType ) const
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RequestSelectionIndexesL\
 begin");

    iContentRequestor->RequestSelectionIndexesL( 
        UpnpContentServer::TUpnpMediaType( aType ) );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RequestSelectionIndexesL\
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::GetSelectionIndexes
// Gets the selected albums/playlist indexes
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::GetSelectionIndexesL(
    CArrayFix<TInt>& aMarkedItems ) const
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::GetSelectionIndexesL\
 begin");

    iContentRequestor->GetSelectionIndexesL( aMarkedItems );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::GetSelectionIndexesL\
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SetSelectionIndexes
// Sets the user selections
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::SetSelectionIndexesL(
    THomeConnectMediaType aType,
    const CArrayFix<TInt>& aMarkedItems )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SetSelectionIndexesL\
 begin");

    switch ( aType )
        {
    case EImageAndVideo:
        {
        delete iImageSelections;
        iImageSelections = NULL;
        iImageSelections = new (ELeave) CArrayFixFlat<TInt>
            (aMarkedItems.Count());
        CopyTintArrayL( aMarkedItems, *iImageSelections );
        }
        break;
    case EPlaylist:
        {
        delete iMusicSelections;
        iMusicSelections = NULL;
        iMusicSelections = new (ELeave) CArrayFixFlat<TInt>
            (aMarkedItems.Count());
        CopyTintArrayL( aMarkedItems, *iMusicSelections );

        }
        break;
    default:
        User::Leave( KErrArgument );
        break;
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::SetSelectionIndexesL\
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::RequestSelectionContentL
// Requests the album /playlist names
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::RequestSelectionContentL(
    THomeConnectMediaType aType )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RequestSelectionContentL\
 begin");

    iContentRequestor->RequestSelectionContentL( 
        UpnpContentServer::TUpnpMediaType( aType ) );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::RequestSelectionContentL\
 begin");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::GetSelectionContentL
// Get all albums/playlists identifiers from Media gallery
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::GetSelectionContentL(
    CDesCArray& aArray ) const
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine:: \
GetSelectionContentL begin");

    iContentRequestor->GetSelectionContentL( aArray );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine:: \
GetSelectionContentL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::RequestSharingProgressL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::RequestSharingProgressL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
RequestSharingProgressL begin");
    if( !iProgress )
        {
        iProgress = CUpnpProgressWatcher::NewL( *this );
        }
    iProgress->StartL();

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
RequestSharingProgressL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::StartUploadListenerL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::StartUploadListenerL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
StartUploadListenerL begin");
    iContentServer.StartUploadListenerL();
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
StartUploadListenerL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::StopUploadListenerL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPFileSharingEngine::StopUploadListenerL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
StopUploadListenerL begin");
    iContentServer.StopUploadListenerL();
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
StopUploadListenerL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SetConMonState
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::SetConMonStateL( 
    TBool aState, TRequestStatus& aStatus )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
SetConMonState begin");

    delete iConnMon;
    iConnMon = NULL;

    if ( aState )
        {
        // (Re)Start
        CUPnPSettingsEngine* settings = CUPnPSettingsEngine::NewLC();
        TInt iap( KErrNotFound );
        User::LeaveIfError( settings->GetAccessPoint( iap ) );
        iConnMon = CUPnPConnectionMonitor::NewL( *this, iap );
        CleanupStack::PopAndDestroy( settings );
        iContentServer.StartConnectionMonitorL( iap, aStatus );
        }
    else
        {
        iContentServer.StopConnectionMonitorL( aStatus );
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
SetConMonState end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::ConnectionLost
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::ConnectionLost()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
ConnectionLost begin");
    if( iAO->SharingStateL() )
        {
        iAO->ConnectionLost();
        if ( iObserver )
            {
            iObserver->HandleSharingConnectionLost( *this );
            }
        }
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngine::\
ConnectionLost end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::CopyTintArrayL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::CopyTintArrayL( const CArrayFix<TInt>& aSrc,
    CArrayFix<TInt>& aDst ) const
    {
    for ( TInt i(0); i<aSrc.Count();i++ )
        {
        aDst.AppendL(aSrc.At(i));
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::CopyCdescArray
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::CopyCdescArrayL( 
    const CDesCArray& aSrc,
    CDesCArray& aDst ) const
    {
    for ( TInt i(0); i<aSrc.Count();i++ )
        {
        aDst.AppendL( aSrc[ i ] );
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::SetShareFileComplete
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngine::SetShareFileComplete( 
    TBool aShareState )
    { 
    iShareFileState = aShareState; 
    }

// End of file
