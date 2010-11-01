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
* Description:      Content sharing servers main scheduler function
 *
*/





#include <e32debug.h>
#include <e32property.h>
#include "upnpsharingalgorithmfactory.h"
#include "upnpcdsliteobject.h"
#include "upnpcdsliteobjectarray.h"
#include <mclfitemlistmodel.h>
#include <escapeutils.h>            // ConvertFromUnicodeToUtf8L

#include "upnpcommonutils.h"
#include "upnpcontentserverdefs.h"
#include "upnpcontentserverhandler.h"
#include "upnpselectionreader.h"
#include "upnpcontentserverclient.h"
#include "upnpcontentserver.h"
#include "upnpcontentmetadatautility.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KNoProgress = -1;
const TInt KZeroProgress = 0;
const TInt KArrayGranularity = 10;

_LIT8( KAudio, "audio" );
_LIT8( KVideo, "video" );
_LIT8( KImage, "image" );

using namespace UpnpContentServer;

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::NewL
// 2-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler* CUpnpContentServerHandler::NewL(
    CUpnpContentServer* aServer )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContentServerHandler* self =
        new (ELeave) CUpnpContentServerHandler( aServer );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CUpnpContentServerHandler()
// C++ constructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler::CUpnpContentServerHandler(
    CUpnpContentServer* aServer ) :
    iServer( aServer ),
    iContentSharingObserver( NULL ),
    iOngoingSharingType( KErrNotFound ),
    iErrorToClient( KErrNone ),
    iSharingAlgorithm( NULL )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    ResetPendingRequestInfo();

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ConstructL
// 2nd phase constructor.
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
    static _LIT_SECURITY_POLICY_S0(KThisProcessPolicy,
                                   KUpnpContentServerCat.iUid );

    // define progress property to be a byte array, allocating mem
    TInt err = iProgressProperty.Define( KUpnpContentServerCat,
                                         ESharingProgress,
                                         RProperty::EByteArray,
                                         KAllowAllPolicy,
                                         KThisProcessPolicy,
                                         sizeof( TUpnpProgress ) );
    if ( err != KErrNone )
        {
        __LOG1( "Error: %d", err );
        }
    if ( err!=KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    // Create selection reader
    iReader = CUpnpSelectionReader::NewL();

    // reset progress information
    SetProgressL( KNoProgress );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::InitializeL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::InitializeL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Create MUpnpSharingAlgorithm
    if ( !iSharingAlgorithm )
        {
        iSharingAlgorithm = 
                CUPnPSharingAlgorithmFactory::NewSharingApiL();
        }
    // Create CUpnpContentSharerAo
    if ( !iAo )
        {
        iAo = CUpnpContentSharerAo::NewL( this, iSharingAlgorithm );
        }
    // Create CUpnpContentMetadataUtility
    if ( !iMetadata )
        {
        iMetadata = CUpnpContentMetadataUtility::NewL();
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::~CUpnpContentServerHandler()
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpContentServerHandler::~CUpnpContentServerHandler()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( iSharingAlgorithm )
        {
        iSharingAlgorithm->Close();
        }
    
    delete iAo;
    delete iMetadata;
    delete iReader;
    delete iMediaServer;

    iPendingSharingReqInfo.iMarkedItems.Close();
    delete iMusicSharingReq;
    delete iVisualSharingReq;
    iProgressProperty.Delete( KUpnpContentServerCat, ESharingProgress );
    iProgressProperty.Close();
    iServer = NULL;
        
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetContentSharingObserverL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetContentSharingObserverL(
    MUpnpContentSharingObserver* aObserver )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iContentSharingObserver = aObserver; // can be NULL
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }
// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectionContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::GetSelectionContentL(
    const TInt& aContainerType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // There might be many containers -> 10 is OK for granularity
    CDesCArray* containers = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( containers );

    switch ( aContainerType )
        {
        case EImageAndVideo :
            {
            iReader->FetchCollectionsL( containers );
            break;
            }
        case EPlaylist :
            {
            iReader->FetchPlaylistsL( containers );
            break;
            }
        default :
            {
            CleanupStack::PopAndDestroy( containers );
            __LOG1( "Error: %d", __LINE__ );
            break;
            }
        }
    if ( iContentSharingObserver )
        {
        iContentSharingObserver->CompleteSelectionContentL( *containers );
        }

    CleanupStack::PopAndDestroy( containers );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectionIndexesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::GetSelectionIndexesL(
    RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpSharingRequest* sharingReq( NULL );
    switch ( aType )
        {
        case EImageAndVideo :
            {
            if ( iVisualSharingReq )
                {
                sharingReq = iVisualSharingReq;
                }
            break;
            }
        case EPlaylist :
            {
            if ( iMusicSharingReq )
                {
                sharingReq = iMusicSharingReq;
                }
            break;
            }
        default :
            {
            __LOG1( "Error: %d", __LINE__ );
            break;
            }
        }
    
    if ( sharingReq && 
        sharingReq->iClfIds && 
        sharingReq->iSharingType == EShareMany )
        {
        // There is sharing ongoing, return the buffered selections
        for( TInt i(0); i < sharingReq->iClfIds->MdcaCount(); i++ )
            {
            TInt index = iReader->GetSelectionIndexL( 
                    (TUpnpMediaType)aType, 
                    sharingReq->iClfIds->MdcaPoint( i ) );
            if ( index != KErrNotFound )
                {
                aMarkedItems.AppendL( index );
                }
            }
        }
    else
        {
        if ( !iReader )
            {
            __LOG1( "Error: %d", __LINE__ );
            iReader = CUpnpSelectionReader::NewL();
            }
        iReader->GetSelectionIndexesL( aMarkedItems, (TUpnpMediaType)aType );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ChangeShareContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::ChangeShareContentL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    for( TInt e(0); e<aMarkedItems.Count();e++)
        {
        __LOG2("CUpnpContentServerHandler: received index[%d] = %d",
            e, aMarkedItems[e] );
        }    
    
    if ( iOngoingSharingType != KErrNotFound )
        {
        // sharing process ongoing, check that if selection is changed
        // to ongoing sharing type
        if ( aType == iOngoingSharingType && iAo->SharingOngoing() )
            {
            // save pending request info
            SavePendingRequestInfoL( aMarkedItems, aType );
            // cancel Ao and create new sharing request in callback
            iAo->StopSharing();
            }
        else
            {
            SetSharingRequestL( aMarkedItems, aType );
            }
        }
    else
        {
        // Not active, ie. no sharing ongoing        
        SetSharingRequestL( aMarkedItems, aType );
        iOngoingSharingType = aType;

        // start the sharing
        DoShare();
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::RefreshShareContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::RefreshShareContentL(
    TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( ( aType == EImageAndVideo && iVisualSharingReq ) ||
         ( aType == EPlaylist && iMusicSharingReq ) )
        {
        // no refresh available if sharing is ongoing
        __LOG1( "Error: %d", __LINE__ );
        User::Leave( KErrServerBusy );
        }

    delete iReader;
    iReader = NULL;
    iReader = CUpnpSelectionReader::NewL();

    if ( aType == EImageAndVideo )
        {
        RArray<TInt> markedItems;
        CleanupClosePushL( markedItems );
        
        // Get selected indexes and create new sharing request
        GetSelectionIndexesL( markedItems, aType );
        SetSharingRequestL( markedItems, aType );

        CleanupStack::PopAndDestroy( &markedItems );
        }
    else if ( aType == EPlaylist )
        {
        RArray<TInt> markedItems;
        CleanupClosePushL( markedItems );
        
        // Get selected indexes and create new sharing request
        GetSelectionIndexesL( markedItems, aType );
        SetSharingRequestL( markedItems, aType );
        
        CleanupStack::PopAndDestroy( &markedItems );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    // if no refreshes ongoing, start sharing
    if ( iOngoingSharingType == KErrNotFound )
        {
        iOngoingSharingType = aType;
        DoShare();
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CanStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentServerHandler::CanStop() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( EFalse );
    if ( !( iVisualSharingReq || iMusicSharingReq  ) )
        {
        __LOG("CUpnpContentServerHandler: CanStop(): 1");
        ret = ETrue;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CompleteSharingOperationL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::CompleteSharingOperationL(
    const TInt& aErr, const TInt& aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    switch ( aErr )
        {
        case KErrNone:
            {
            iErrorToClient = KErrNone;
            if ( aType == EImageAndVideo )
                {
                // Reset sharing request object 
                delete iVisualSharingReq;
                iVisualSharingReq = NULL;
                
                // If there is pending music request, process it next 
                if ( iMusicSharingReq )
                    {
                    iOngoingSharingType = EPlaylist;
                    DoShare();
                    }
                else
                    {
                    iOngoingSharingType = KErrNotFound;
                    }
                }
            else if ( aType == EPlaylist )
                {
                // Reset sharing request object 
                delete iMusicSharingReq;
                iMusicSharingReq = NULL;
                
                // If there is pending music request, process it next 
                if ( iVisualSharingReq )
                    {
                    iOngoingSharingType = EImageAndVideo;
                    DoShare();
                    }
                else
                    {
                    iOngoingSharingType = KErrNotFound;
                    }
                }
            else 
                {
                iOngoingSharingType = KErrNotFound;
                }
            break;
            }
        case KErrCancel:
            {
            // User reselected currently ongoing sharing, handle it next
            if ( iPendingSharingReqInfo.iMarkedItems.Count() )
                {
                // create sharing request from pending info
                TRAPD( 
                    err, 
                    SetSharingRequestL( 
                        iPendingSharingReqInfo.iMarkedItems,
                        iPendingSharingReqInfo.iMediaType ) 
                    );
                if ( err )
                    {
                    HandleError( err );
                    }
                else
                    {
                    // no errors, reset the pending request info and start 
                    // sharing
                    ResetPendingRequestInfo();
                    DoShare();
                    }
                }
            else
                {
                iOngoingSharingType = KErrNotFound;
                }
            break;
            }
        default:
            {
            __LOG1( "CompleteSharingOperationL: %d", aErr );
            HandleError( aErr );
            break;
            }
        }
    
    if ( iOngoingSharingType == KErrNotFound )
        {
        // Reset the progress info to no progress.
        TRAP_IGNORE( 
            SetProgressL( KNoProgress )
            );
        if ( CanStop() && iServer && !iContentSharingObserver )
            {
            // client application is exited, no sharing ongoing
            // -> stop the server
            iServer->Stop();
            }
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CancelSharingOperationL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::CancelSharingOperationL(
    const TInt& /*aErr*/ )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetProgressL
// This should be called from active objects owned by handler
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetProgressL( const TInt& aProgress )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // Initilialize music sharing progress information
    TUpnpProgressInfo playlistProg;
    playlistProg.iProgress = KZeroProgress;
    playlistProg.iProgressKind = EPlaylist;
    playlistProg.iProgressType = TUpnpProgressInfo::EVisualStatus;
    
    // Set music sharing progress information
    if ( iMusicSharingReq && aProgress != KNoProgress )
        {
        playlistProg.iProgressType = TUpnpProgressInfo::ESharingProgress;
        if ( iOngoingSharingType == EPlaylist )
            {
            playlistProg.iProgress = aProgress;
            }
        }
    else
        {
        TInt state( EShareNone );
        iReader->GetMusicSharingStateL( state );
        playlistProg.iProgress = state;
        }
    
    // Initilialize visual sharing progress information
    TUpnpProgressInfo visualProg;
    visualProg.iProgress = KZeroProgress;
    visualProg.iProgressKind = EImageAndVideo;
    visualProg.iProgressType = TUpnpProgressInfo::EVisualStatus;
    
    // Set visual sharing progress information
    if ( iVisualSharingReq && aProgress != KNoProgress )
        {
        visualProg.iProgressType = TUpnpProgressInfo::ESharingProgress;
        if ( iOngoingSharingType == EImageAndVideo)
            {
            visualProg.iProgress = aProgress;
            }
        }
    else
        {
        TInt state( EShareNone );
        iReader->GetVisualSharingStateL( state );
        visualProg.iProgress = state;
        }

    // Create total progress
    TUpnpProgress totalProg;
    totalProg.iError = iErrorToClient;
    iErrorToClient = KErrNone;
    
    totalProg.iImageVideoProgress = visualProg;
    totalProg.iMusicProgress = playlistProg;

    // Set the progress property so that Ui updates itself
    TPckgBuf<TUpnpProgress> progressBuf( totalProg );
    TInt err = iProgressProperty.Set( KUpnpContentServerCat,
                                      ESharingProgress,
                                      progressBuf  );
    if ( err != KErrNone )
        {
        __LOG1( "Error: %d", err );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::DoShare
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::DoShare()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    switch( iOngoingSharingType )
        {
        case EImageAndVideo :
            {
            // start sharing and put progress
            iAo->StartSharing( iVisualSharingReq );
            break;
            }
        case EPlaylist :
            {
            // start sharing and put progress
            iAo->StartSharing( iMusicSharingReq );
            break;
            }
        default:
            {
            __LOG1( "Error: %d", __LINE__ );
            break;
            }
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::Cleanup
// Cleanup depending on state
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::Cleanup()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if( iAo->SharingOngoing() )
        {
        iAo->StopSharing();
        }

    delete iVisualSharingReq;
    iVisualSharingReq = NULL;
    delete iMusicSharingReq;
    iMusicSharingReq = NULL;
    ResetPendingRequestInfo();

    // reset state variables
    iOngoingSharingType = KErrNotFound;
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ConnectionLost
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentServerHandler::ConnectionLostL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( EFalse );

    if ( iOngoingSharingType != KErrNotFound )
        {
        ret = ETrue;
        if ( iAo->SharingOngoing() )
            {
            iAo->StopSharing();
            }
        }
    else
        {
        TInt err( iMediaServer->Connect() );
        TInt stat( RUpnpMediaServerClient::EStopped );
        if ( !err )
            {
            err = iMediaServer->Status( stat );
            if ( !err && stat == RUpnpMediaServerClient::EStartedOnline )
                {
                iMediaServer->Stop( RUpnpMediaServerClient::EStopSilent );
                iMediaServer->Close();
                delete iMediaServer;
                iMediaServer = NULL;
                }
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetSharingRequestL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentServerHandler::SetSharingRequestL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Create sharing request
    CreateSharingRequestL( aType, SharingType( aMarkedItems ) );

    // Get user selected filenames to be shared
    CDesCArray* filenamesToBeShared = 
        new (ELeave) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( filenamesToBeShared );
    CDesCArray* clfIds =
        new (ELeave) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( clfIds );

    GetSelectionFilenamesL( 
            aMarkedItems, 
            aType, 
            SharingType( aMarkedItems ), 
            *filenamesToBeShared,
            *clfIds );

    // Get currently shared filenames
    CUpnpCdsLiteObjectArray& currentlySharedContent = 
        iSharingAlgorithm->ReadSharedContent();
        
    // Compare and create unshare/share arrays
    RArray<TFileName>* shareArr = new ( ELeave ) RArray<TFileName>;
    CleanupStack::PushL( shareArr );
    RArray<TFileName>* unshareArr = new ( ELeave ) RArray<TFileName>;
    CleanupStack::PushL( unshareArr );

    CompareCdsToClfL( 
            *filenamesToBeShared, 
            currentlySharedContent,
            *shareArr,
            *unshareArr,
            (TUpnpMediaType)aType );    
    
    // Set sharing request information arrays:
    // shareArr, unshareArr and clfIds ownership transferred.
    SetSharingRequestInfo( 
        aType, 
        shareArr, 
        unshareArr, 
        clfIds );

    // cleanup
    CleanupStack::Pop( unshareArr ); // ownership transferred to shar.request
    CleanupStack::Pop( shareArr );   // ownership transferred to shar.request
    CleanupStack::Pop( clfIds );     // ownership transferred to shar.request
    CleanupStack::PopAndDestroy( filenamesToBeShared );
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::HandleError
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::HandleError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // cleanup if some error occurred while sharing
    if( iOngoingSharingType != KErrNotFound )
        {
        Cleanup();
        iErrorToClient = aError;
        // reset progress information
        TRAP_IGNORE( SetProgressL( KNoProgress ) );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectionFilenamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::GetSelectionFilenamesL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType,
    const TInt aSharingType,
    CDesCArray& aFilenames,
    CDesCArray& aClfIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Collect all the necessary filenames and clfIds depending on 
    // the user selection.
    switch ( aSharingType )
        {
        case EShareAll:
            {
            // All files of the selected playlists/albums should be 
            // shared
            if ( aType == EImageAndVideo )
                {
                GetAllImageAndVideoFilenamesL( aFilenames );
                }
            else if ( aType == EPlaylist )
                {
                GetAllMusicFilenamesL( aFilenames );
                }
            break;
            }
        case EShareMany:
            {
            // Selected some of the playlists/albums
            if ( aType == EImageAndVideo )
                {
                GetSelectedImgAndVideoFilenamesL( 
                    aMarkedItems, 
                    aFilenames,
                    aClfIds );
                }
            else if ( aType == EPlaylist )
                {
                GetSelectedMusicFilenamesL( 
                    aMarkedItems,
                    aFilenames,
                    aClfIds );
                }
            break;
            }
        case EShareNone:
            {
            // No selections, do nothing
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetAllImageAndVideoFilenamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::GetAllImageAndVideoFilenamesL(
    CDesCArray& aFilenames )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Fetch all the image files
    const MCLFItemListModel* imageFiles = iMetadata->ImageFiles();
    if ( imageFiles )
        {
        TInt imageCount = imageFiles->ItemCount();
        __LOG1( "CUpnpContentServerHandler::GetAllImageAndVideoFilenamesL \
                    imageCount=%d", imageCount );
                    
        // Append all image files to aFilenames
        for ( TInt i=0; i < imageCount; i++ )
            {
            // get the filename from clf item
            TPtrC fullFilename;
            imageFiles->Item(i).GetField( 
                ECLFFieldIdFileNameAndPath,
                fullFilename );
            aFilenames.AppendL( fullFilename );
            }    
        }
        
    // Fetch all the video files
    const MCLFItemListModel* videoFiles = iMetadata->VideoFiles();
    if ( videoFiles )
        {
        TInt videoCount = videoFiles->ItemCount();
        __LOG1( "CUpnpContentServerHandler::GetAllImageAndVideoFilenamesL \
                    videoCount=%d", videoCount );
                    
        // Append all video files to aFilenames
        for ( TInt i=0; i < videoCount; i++ )
            {
            // get the filename from clf item
            TPtrC fullFilename;
            videoFiles->Item(i).GetField( 
                ECLFFieldIdFileNameAndPath,
                fullFilename );
            aFilenames.AppendL( fullFilename );
            }
        }
        
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetAllMusicFilenamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::GetAllMusicFilenamesL(
    CDesCArray& aFilenames )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Fetch all the music files...
    const MCLFItemListModel* musicFiles = iMetadata->MusicFiles();
    if ( musicFiles )
        {
        TInt musicCount = musicFiles->ItemCount();
        __LOG1( "CUpnpContentServerHandler::GetAllMusicFilenamesL \
                    musicCount=%d", musicCount );
                    
        // Append all music files to aFilenames
        for ( TInt i=0; i < musicCount; i++ )
            {
            // get the filename from clf item
            TPtrC fullFilename;
            musicFiles->Item(i).GetField( 
                ECLFFieldIdFileNameAndPath,
                fullFilename );
            aFilenames.AppendL( fullFilename );
            }
        }
        
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectedImgAndVideoFilenamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::GetSelectedImgAndVideoFilenamesL(
    const RArray<TInt>& aMarkedItems,
    CDesCArray& aFilenames,
    CDesCArray& aClfIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Go though all the selected collections and fetch collection ids and 
    // items.
    TInt markedCount = aMarkedItems.Count();
    for ( TInt i=0; i < markedCount; i++ )
        {
        CDesCArray* collFilenames = 
            new (ELeave) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( collFilenames );

        // find out collection id and items
        TPtrC collId = iReader->GetCollectionItemsL( 
                            aMarkedItems[i], 
                            *collFilenames );        
        if ( collId.Length() )
            {
            // collection id found, append it to aClfIds
            aClfIds.AppendL( collId );

            // append all collection items to aFilenames
            TInt collFileCount = collFilenames->Count();
            for (TInt j=0; j < collFileCount; j++ )
                {
                aFilenames.AppendL( collFilenames->MdcaPoint( j ) );
                }
            }
        CleanupStack::PopAndDestroy( collFilenames );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::GetSelectedMusicFilenamesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::GetSelectedMusicFilenamesL(
    const RArray<TInt>& aMarkedItems,
    CDesCArray& aFilenames,
    CDesCArray& aClfIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // Go though all the selected playlists and fetch playlist ids and 
    // items.
    TInt markedCount = aMarkedItems.Count();
    for ( TInt i=0; i < markedCount; i++ )
        {
        CDesCArray* plFilenames = 
            new (ELeave) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( plFilenames );

        // find out playlist id and items
        TPtrC plId = iReader->GetPlaylistItemsL( 
                            aMarkedItems[i],
                            *plFilenames );
        
        if ( plId.Length() )
            {
            // playlist id found, append it to aClfIds
            aClfIds.AppendL( plId );

            // append all playlist items to aFilenames
            TInt plFileCount = plFilenames->Count();
            for (TInt j=0; j < plFileCount; j++ )
                {
                aFilenames.AppendL( plFilenames->MdcaPoint( j ) );
                }            
            }

        CleanupStack::PopAndDestroy( plFilenames );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CompareCdsToClfL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::CompareCdsToClfL( 
    CDesCArray& aClfFilePaths, 
    CUpnpCdsLiteObjectArray& aCdsObjects,
    RArray<TFileName>& aToBeSharedFiles,
    RArray<TFileName>& aToBeUnsharedSharedFiles,
    const TUpnpMediaType aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    
    // See if files in the clffilenames can be found from cds structure.
    // if they cna they are already shared. In they can't they need to be 
    // added to the list of "to be shared" items.
    for (TInt i(0); i<aClfFilePaths.MdcaCount(); i++)
        {        
        RBuf8 fileName;
        CleanupClosePushL( fileName );
        fileName.CreateMaxL( aClfFilePaths.MdcaPoint(i).Length() );
        fileName.Copy( aClfFilePaths.MdcaPoint(i) );
        if(  KErrNotFound == aCdsObjects.FindByName( fileName ))
            {
            // Not found cds-structure, so needs to be added to 
            // toBeSharedFiles.
            aToBeSharedFiles.AppendL( aClfFilePaths.MdcaPoint(i) );
            }
        else
            {
            // Shared already
            }    
        CleanupStack::PopAndDestroy( &fileName );
        }
    
    // See if items in cds structure can be found from clffilenames. If 
    // they are found they need to be shared. If they are not found they 
    // need to be added to the list of "to be unshared" items.
    for ( TInt j( 0 ); j<aCdsObjects.Count(); j++ )
        {
        CUpnpCdsLiteObject& cdsObject = aCdsObjects.ObjectAtL( j );
        // resolve filetype in cds
        const TDesC8& itemType = cdsObject.ObjectClass(); 
        
        // compare only right type
        if ( cdsObject.Type() == CUpnpCdsLiteObject::EItem && 
            ( ( ( itemType.Find( KVideo ) != KErrNotFound || 
                itemType.Find( KImage ) != KErrNotFound ) &&
                aType == EImageAndVideo ) || 
            ( itemType.Find( KAudio ) != KErrNotFound && 
                aType == EPlaylist ) ) )
            {
            // get filename
            HBufC* fileName = EscapeUtils::ConvertToUnicodeFromUtf8L( 
                cdsObject.Name() );
            CleanupStack::PushL( fileName );
            
            TInt clfCount = aClfFilePaths.Count();
            TInt clfIndex( KErrNotFound );
            aClfFilePaths.Find( *fileName, clfIndex );
            if ( clfCount < 1 || clfIndex >= clfCount )
                {
                // Must not be shared so add to the toBeUnsharedSharedFiles.
                aToBeUnsharedSharedFiles.AppendL( *fileName );
                }            
            else
                {
                // Sharing wanted to that item
                }
            CleanupStack::PopAndDestroy( fileName );
            }
        }
        
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::CreateSharingRequestL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::CreateSharingRequestL(
    TInt aType, 
    TInt aSharingType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // flag for indicating is it needed to set progress property
    TBool setProgress( EFalse );
    
    // Create the appropriate request depending on the sharing type.    
    if ( aType == EImageAndVideo )
        {
        // if music sharing is not ongoing, set progress
        if ( !iMusicSharingReq || iOngoingSharingType != EPlaylist )
            {
            setProgress = ETrue;
            }
        else
            {
            // force AO to set progress as soon as possible
            iAo->ForceSetProgress();
            }
            
        // create image and video request 
        delete iVisualSharingReq;
        iVisualSharingReq = NULL;
        iVisualSharingReq = CUpnpSharingRequest::NewL(
                                                    (TUpnpMediaType)aType, 
                                                    aSharingType );
        if ( setProgress )
            {
            // set progress so that ui updates itself
            SetProgressL( KZeroProgress );
            }
        }
    else if ( aType == EPlaylist )
        {
        // if visual sharing is not ongoing, set progress
        if ( !iVisualSharingReq || iOngoingSharingType != EImageAndVideo )
            {
            setProgress = ETrue;
            }
        else
            {
            // force AO to set progress as soon as possible
            iAo->ForceSetProgress();
            }
            
        // create music request 
        delete iMusicSharingReq;
        iMusicSharingReq = NULL;
        iMusicSharingReq = CUpnpSharingRequest::NewL(
                                                    (TUpnpMediaType)aType, 
                                                    aSharingType );
        if ( setProgress )
            {
            // set progress so that ui updates itself
            SetProgressL( KZeroProgress );
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SetSharingRequestInfo
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::SetSharingRequestInfo(
    TInt aType, 
    RArray<TFileName>* aShareArray,
    RArray<TFileName>* aUnshareArray,
    CDesCArray* aClfIds )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Sets the appropriate sharing request information arrays.
    // aShareArray, UnshareArray and aClfIds ownership transferred.
    if ( aType == EImageAndVideo && iVisualSharingReq )
        {
        // image and video request 
        iVisualSharingReq->SetSharingRequestInfo(
            aShareArray,
            aUnshareArray,
            aClfIds );
        }
    else if ( aType == EPlaylist && iMusicSharingReq )
        {
        // music request 
        iMusicSharingReq->SetSharingRequestInfo(
            aShareArray,
            aUnshareArray,
            aClfIds );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ResetPendingRequestInfo
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::ResetPendingRequestInfo()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Reset iPendingSharingReqInfo object
    iPendingSharingReqInfo.iMediaType = KErrNotFound;
    iPendingSharingReqInfo.iMarkedItems.Reset();
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::SavePendingRequestInfoL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

void CUpnpContentServerHandler::SavePendingRequestInfoL(
    const RArray<TInt>& aMarkedItems,
    const TInt aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // reset pending request object
    ResetPendingRequestInfo();
    
    // Fill info of pending sharing request
    iPendingSharingReqInfo.iMediaType = aType;
    for ( TInt i = 0; i < aMarkedItems.Count(); i++ )
        {
        iPendingSharingReqInfo.iMarkedItems.AppendL( aMarkedItems[i] );
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentServerHandler::ResolveSharingType
// ( other items are commented in header )
// --------------------------------------------------------------------------
//

TInt CUpnpContentServerHandler::SharingType( 
    const RArray<TInt>& aMarkedItems )
    {
    // Resolve sharing type
    TInt sharingType( EShareMany );
    if ( aMarkedItems.Find(0) != KErrNotFound )
        {
        // share nothing marked
        sharingType = EShareNone;
        }
    else if ( aMarkedItems.Find(1) != KErrNotFound )
        {
        // share all marked
        sharingType = EShareAll;
        }
    return sharingType;
    }
    
// End of file
