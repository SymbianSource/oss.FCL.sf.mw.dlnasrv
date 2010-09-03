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
* Description:      CUpnpContentSharingAo class implementation
 *
*/






//  Include Files
#include <upnpcontainer.h>
#include <upnpfilesharing.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpitemlist.h>
#include <upnpobject.h>
#include <upnpstring.h>
#include <s32file.h>
#include <f32file.h>
#include "upnpcommonutils.h" // FindContainerByTitle
#include "upnpcdsreselementutility.h"
// CLF headers
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItemListModel.h>
#include <CLFContentListing.hrh>

//#include <clfcontentlistingextended.hrh>

#include <MCLFItem.h>

#include "upnpcontentsharingao.h"
#include "upnpcontentmetadatautility.h"
#include "upnpselectionwriter.h"
#include "upnpcontainercheckerao.h"
#include "upnpcontentserverdefs.h"
#include "upnpunsharerao.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// FORWARD DECLARATIONS
class UPnPCommonUtils;

// CONSTANTS

const TInt KUiSelectionOffset = 2;
const TInt KDefaultGranularity( 10 );

// KContentDirCommitAmount should be in sync with KCommitEveryNum in
// upnpcontentdirectoryglobals.h which is not exported
const TInt KContentDirCommitAmount = 10;

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::CUpnpContentSharingAo
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpContentSharingAo::CUpnpContentSharingAo(
    MUpnpSharingCallback* aEngine,
    CUpnpContentMetadataUtility* aMetadata )
    : CActive( CActive::EPriorityStandard )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iEngine = aEngine;
    iMetadataUtility = aMetadata;
    iContainerId = KErrNotFound;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpContentSharingAo* CUpnpContentSharingAo::NewL(
    MUpnpSharingCallback* aEngine,
    CUpnpContentMetadataUtility* aMetadata )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUpnpContentSharingAo* self = new (ELeave)
        CUpnpContentSharingAo( aEngine, aMetadata );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iContainerIdx = 0;
    iObjectIdx = 0;
    iVideoIndex = 0;
    iImageIndex = 0;
    iAllVideosShared = EFalse;
    iAllImagesShared = EFalse;

    CActiveScheduler::Add( this );

    iFileSharing = CUpnpFileSharing::NewL();
    ConnectMediaServer();
    iWriter = CUpnpSelectionWriter::NewL( iContainerType );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// Destructor
CUpnpContentSharingAo::~CUpnpContentSharingAo()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iMediaServer.Close();
    delete iFileSharing;
    iFileSharing = NULL;


    if ( IsActive() )
        {
        __LOG1("CUpnpContentSharingAo destructor IsActive==\
true iSharingState: %d", iSharingState );
        iSharingState = ENotActive;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }

    Cancel();
    iEngine = NULL;
    iContainerIds = NULL;

    delete iTmpContainer;
    delete iSharedContainerIds;
    delete iWriter;
    DeleteListUpnpObject();
    delete iFileNameArr;   
    delete iTmpItem;
    delete iPlaylists;
    delete iCollections;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::RunL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    __LOG8_1( "iStatus", iStatus.Int() );
    if ( iStatus.Int() == KErrCorrupt ||
        iStatus.Int() == KErrDiskFull ||
        iStatus.Int() == KErrNoMemory )
        {
        iSharingState = ENotActive;
        }
    if ( iStopRequested == MUpnpSharingCallback::ESharingPause )
        {
        TInt err = iMediaServer.Stop(
            RUpnpMediaServerClient::EStopSilent );
        __LOG1( "Error: %d", err );
        err = iMediaServer.StartOffline();
        __LOG1( "Error: %d", err );
        
        iStopRequested = MUpnpSharingCallback::ESharingNoStop;
        }

    switch ( iSharingState )
        {
    case EUnshareContainers :
        {
        // Unshare the container structure
        UnshareContainersL();
        }
        break;
    case EShareContainers :
        {
        // Share the container structure
        ShareContainersL();
        }
        break;
    case EShareContainersResult :
        {
        // advance to next container or share items
        ShareContainersResultL();
        }
        break;
    case EShareItems :
        {
        // coming here after all containers are shared
        // then at least once per container
        HandleSharingSubstatesL();
        }
        break;
    case EShareItemsResult :
        {
        // advance to next item or end
        ShareItemsResultL();
        }
        break;
    case EShareAllItems :
        {
        //..coming to here once "All files" is selected and
        // individual albums are already shared
        HandleSharingSubstatesL();
        }
        break;
    case EShareAllItemsResult :
        {
        // advance to next item set or end
        AllItemsResultL();
        }
        break;
    case ENotActive :
        // informs handler that we are done
        {
        TInt err ( iStatus.Int() );
        if ( iStopRequested == MUpnpSharingCallback::ESharingFullStop )
            {
            // normal case
            err = KErrCancel;
            iStopRequested = EFalse;
            }

        TRAP_IGNORE( 
            iEngine->CompleteSharingOperationL( err,
                                                iContainerType ));
        }
        break;

    default :
        {
        __LOG( "RunL - default" );
        }
        break;
        }

    __LOG( "RunL - end" );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::DoCancel()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::RunError
// Cancels active object
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::RunError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG8_1( "RunError = ", aError );
    if ( aError == KErrNoMemory ||
         aError == KErrDiskFull ||
         aError == KErrCorrupt )
        {
        iSharingState = ENotActive;
        }
    if ( iSharingState == EUnshareContainers )
        {
        if ( iShareAllSelection == EShareNone )
            {
            iSharingState = ENotActive;
            TRAP_IGNORE( iWriter->SaveSharingStateL( EShareNone ) );
            }
        else
            {
            iSharingState = EShareContainers;
            }
        }
    else
        {
        iSharingState = ENotActive;
        }
    if ( iSharingState != EEnumerateSelections )
        {
        SelfComplete( aError );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::IsActive()
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpContentSharingAo::IsActive() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TBool ret( CActive::IsActive());
    if ( !ret )
        {
        ret = iProfilerActive;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::HandleSharingSubstatesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::HandleSharingSubstatesL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iSharingSubState == EIdle )
        {
        if ( iSharingState == EShareItems )
            {
            ShareItemsL();
            }
        else if ( iSharingState == EShareAllItems )
            {
            ShareAllItemsL();
            }
        else
            {
            }
        }
    else if ( iSharingSubState == EProfileItemList )
        {
        ProfileItemListL();
        }
    else if ( iSharingSubState == EProfileItemListResult )
        {
        ProfileItemListResult();
        }
    else if ( iSharingSubState == EShareItemList )
        {
        TInt parentId( KErrNotFound );
        if ( iSharingState == EShareItems )
            {
            parentId = iSharedContainerIds->operator[]( iContainerIdx );
            }
        else if ( iSharingState == EShareAllItems )
            {
            parentId = iContainerId;
            }
        else
            {
            }
        ShareItemListL( parentId );
        }
    else if ( iSharingSubState == EShareItemListResult )
        {
        ShareItemListResultL();
        }
    else
        {
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareFilesL
// Starts file sharing
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::InitializeL(
    TUpnpMediaType aContainerType,
    const RArray<TInt>& aCurrentSelection,
    CDesCArray& aPlaylistIds,
    CDesCArray& aPlaylistNames,    
    const TInt aContainerId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Initialize values
    iObjectIdx = 0;
    iContainerIdx = 0;
    iVideoIndex = 0;
    iImageIndex = 0;
    iAllVideosShared = EFalse;
    iAllImagesShared = EFalse;
    iSharingProgress = 0;

    iContainerType = aContainerType;

    delete iWriter;
    iWriter = NULL;
    iWriter = CUpnpSelectionWriter::NewL( iContainerType );

    AdjustShareAllSelection( aCurrentSelection );
    iCurrentSelections = aCurrentSelection;
    iContainerIds = &aPlaylistIds;
    iContainerNames = &aPlaylistNames;
    iContainerId = aContainerId;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareFilesL
// Starts file sharing
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareFiles( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Open file sharing session from mediaserver.
    // Start offline if not started.
    TInt status( RUpnpMediaServerClient::EStopped );
    iMediaServer.Status( status );
    if ( status == RUpnpMediaServerClient::EStopped )
        {
        iMediaServer.StartOffline();
        }
    iSharingState = EUnshareContainers;
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngineAO::CreateContainerL
// creates container
// --------------------------------------------------------------------------
//
CUpnpContainer* CUpnpContentSharingAo::CreateContainerL(
    TUpnpMediaType aContainerType,
    const TDesC8& aTitle ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContainer* cont= CUpnpContainer::NewL();
    CleanupStack::PushL( cont );
    cont->SetParentIdL( KRootContainerIdDesc8 );
    cont->SetTitleL( aTitle );
    switch ( aContainerType )
        {
    case EImageAndVideo :
        {
        cont->SetObjectClassL( KImageVideoContainerClass );
        }
        break;
    case EPlaylist :
        {
        cont->SetObjectClassL( KStorageFolderContainerClass );
        }
        break;
    default :
        {
        __LOG( "CreateContainerL - default");
        }
        break;
        }
    CleanupStack::Pop( cont );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return cont;
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::CloseFileSharingSession
// Closes mediaserver and file sharing session
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::CloseFileSharingSession()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iMediaServer.Close();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::UnshareContainersL
// Unshares objects, this should only be called from RunL
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::UnshareContainersL( )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // All old containers unshred, move to next state
    if ( iShareAllSelection == EShareNone )
        {
        iSharingState = ENotActive;
        iWriter->SaveSharingStateL( EShareNone );
        }
    else
        {
        iSharingState = EShareContainers;
        }

    // All old containers unshred, move to next state
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ConnectMediaServer
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::ConnectMediaServer()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt error = iMediaServer.Connect();
    __LOG8_1( "ConnectMediaServer error %d", error );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return error;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::AdjustShareAllSelection
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::AdjustShareAllSelection(
    const RArray<TInt>& aSelections )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( aSelections.Count() == 1 )
        {

        if ( aSelections[0] == 0 )
            {
            __LOG("CUpnpContentSharingAo::AdjustShareAllSelection: \
EShareNone");
            iShareAllSelection = EShareNone;
            }
        else if ( aSelections[0] == 1 )
            {
            __LOG("CUpnpContentSharingAo::AdjustShareAllSelection: \
EShareAll");
            iShareAllSelection = EShareAll;
            }
        else
            {
            __LOG("CUpnpContentSharingAo::AdjustShareAllSelection: \
EShareMany");
            iShareAllSelection = EShareMany;
            }
        }
    else
        {
        __LOG("CUpnpContentSharingAo::AdjustShareAllSelection: EShareMany");
        iShareAllSelection = EShareMany;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareContainersL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareContainersL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
 /*   iMetadataUtility->ClearPostFiltersL();

    TInt ind( 0 );
    // Theres is still containers to share
    switch ( iContainerType )
        {
    case EImageAndVideo :
        {
        // get the correct index for CLF
        ind = GetContainerIndexL();
        HBufC8* albumName( NULL );
        if (  ind != KErrNotFound )
            {
            albumName = UpnpString::FromUnicodeL( 
                iContainerNames->MdcaPoint( ind ) );
            CleanupStack::PushL( albumName );

            TInt plErr( KErrNone );

                
            if ( !plErr )
                {
                iWriter->AppendItemL( iContainerIds->MdcaPoint( ind ) );

                iTmpContainer = CreateContainerL( iContainerType,
                                                  *albumName );

                iFileSharing->ShareContainerL(
                    *UPnPCommonUtils::IntToDesC8LC ( iContainerId ),
                    *iTmpContainer,
                    iStatus );
                CleanupStack::PopAndDestroy(); //UPnPCommonUtils::IntToDesC8LC
                CleanupStack::PopAndDestroy( albumName );
                SetActive();
                }
            else
                {
                SelfComplete( KErrNotFound );
                }
            }
        else
            {
            SelfComplete( KErrNone );
            }
        }
            break;
    case EPlaylist :
        {

        // get the correct index for CLF
        ind = GetContainerIndexL();
        if ( ind != KErrNotFound )
            {
            HBufC8* playlistName( NULL );
            playlistName = UpnpString::FromUnicodeL( 
                iContainerNames->MdcaPoint( ind ) );
            CleanupStack::PushL( playlistName  );

            TInt plErr( KErrNone );
            if ( !plErr )
                {
                // do the id..
                iWriter->AppendItemL( iContainerIds->MdcaPoint( ind ) );

                // ..then do the name for the container
                iTmpContainer = CreateContainerL( iContainerType,
                                                  *playlistName );

                iFileSharing->ShareContainerL(
                    *UPnPCommonUtils::IntToDesC8LC( iContainerId ),
                    *iTmpContainer,
                    iStatus );

                CleanupStack::PopAndDestroy(); // IntToDesC8LC

                SetActive();
                }
            else
                {
                SelfComplete( KErrNotFound );
                }
            CleanupStack::PopAndDestroy( playlistName );
            }
        else
            {
            SelfComplete( KErrNone );
            }
        }
        break;
    default :
        break;
        }

    iSharingState = EShareContainersResult;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );*/
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareItemsL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareItemsL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    delete iFileNameArr;
    iFileNameArr = NULL;
    iFileNameArr = new (ELeave)
    CDesCArrayFlat( 4 );
    DeleteListUpnpObject();
    iItemList = CUpnpItemList::NewL();
    TInt ind( 0 );


    TInt parentId( 0 );

    switch ( iContainerType )
        {
    case EImageAndVideo :
        {
        if ( !iCollections ) 
            {
            iCollections = CUPnPAlbumServices::NewL();
            }

        // get the correct index for CLF
        ind  = GetContainerIndexL();

        // ..then do the name for the container
        CDesCArray* items = new (ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL( items );
        TInt itemCount( 0 );
        if ( ind < iContainerIds->MdcaCount() && ind != KErrNotFound )
            {
            TPtrC plId( iContainerIds->MdcaPoint( ind ) );
            TRAPD( albErr,  iCollections->OpenAlbumL( plId, 
                                                      *items ));
            delete iCollections;
            iCollections = NULL;

            if( !albErr )
                {
                itemCount = items->MdcaCount();
                }
            }

        if ( itemCount )
            {

            parentId =
                iSharedContainerIds->operator[]( iContainerIdx );
            TInt i = iObjectIdx;

            delete iFileNameArr;
            iFileNameArr = NULL;
            iFileNameArr = new (ELeave)
                CDesCArrayFlat( 4 );
            TBool canAppend = items->MdcaCount();

            while ( i < itemCount && canAppend )
                {
                // process all items in curret collection
                HBufC8* id = UPnPCommonUtils::IntToDesC8LC( parentId );
                TPtrC item( items->MdcaPoint( i ) );
                TRAPD( itemErr,
                       iTmpItem = iMetadataUtility->CreateItemL(
                           item,
                           *id ));
                if ( !itemErr && iTmpItem )
                    {
                    if ( !itemErr )
                        {
                        if( !iMetadataUtility->UpdateMetadataL( 
                            EPhotoFile,
                            iTmpItem,
                            item ))
                            {
                            // The item was not found from prev model
                            if( !iMetadataUtility->UpdateMetadataL( 
                                EVideoFile,
                                iTmpItem,
                                item ) )
                                {
                                iTmpItem->SetObjectClassL( 
                                    KImageItemObjectClass );
                                }
                            }
                        iItemList->AppendObjectL( *iTmpItem );
                        iFileNameArr->AppendL(
                            item );
                        // transfer ownership
                        }
                    else
                        {
                        delete iTmpItem;
                        }
                    iTmpItem = NULL;
                    i++;
                    if ( iItemList->ObjectCount() >
                         KContentDirCommitAmount )
                        {
                        // the list is full, next share it
                        canAppend = EFalse;
                        // save the current position in list for
                        // next run
                        iObjectIdx = i;
                        }
                    }
                else
                    {
                    i++;
                    iTmpItem = NULL;
                    }
                CleanupStack::PopAndDestroy(); // IntToDesC8LC

                }
            }
        CleanupStack::PopAndDestroy( items );     

        if ( iItemList->ObjectCount() <= KContentDirCommitAmount )
            {
            // next time start with new album
            iSharingProgress += iObjectIdx;
            iObjectIdx = 0;
            iMetadataUtility->ClearPostFiltersL();
            }

        if ( iItemList->ObjectCount() )
            {
            iSharingSubState = EProfileItemList;
            SelfComplete( KErrNone );
            }
        else
            {
            // update indexes
            iMetadataUtility->ClearPostFiltersL();
            iSharingSubState = EShareItemListResult;
            SelfComplete( KErrNone );
            }
        }
        break;
    case EPlaylist :
        {
        if ( !iPlaylists ) 
            {
            iPlaylists = CUPnPPlaylistServices::NewL();
            }

        // get the correct index for CLF
        ind  = GetContainerIndexL();

        // ..then do the name for the container
        CDesCArray* items = new (ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL( items );
        TInt itemCount( 0 );
        if ( ind < iContainerIds->MdcaCount() && ind != KErrNotFound )
            {
            TPtrC plId( iContainerIds->MdcaPoint( ind ) );
            TRAPD( plErr, iPlaylists->OpenPlaylistL( plId, 
                                                     *items ) );
            delete iPlaylists;
            iPlaylists = NULL;

            if( !plErr )
                {
                itemCount = items->MdcaCount();
                }
            }

        if ( itemCount )
            {

            parentId =
                iSharedContainerIds->operator[]( iContainerIdx );
            TInt i = iObjectIdx;

            TBool canAppend = items->MdcaCount();

            while ( i < itemCount && canAppend )
                {
                // process all items in curret collection
                HBufC8* id = UPnPCommonUtils::IntToDesC8LC( parentId );
                TPtrC item( items->MdcaPoint( i ) );
                TRAPD( plErr,
                       iTmpItem = iMetadataUtility->CreateItemL(
                           item,
                           *id ));
                if ( !plErr && iTmpItem )
                    {
                    if ( !plErr )
                        {
                        iMetadataUtility->UpdateMetadataL( 
                            EMusicFile,
                            iTmpItem,
                            item );
                        iTmpItem->SetObjectClassL( 
                            KMusicItemObjectClass );
                        iItemList->AppendObjectL( *iTmpItem );
                        iFileNameArr->AppendL(
                            item );
                        // transfer ownership
                        }
                    else
                        {
                        delete iTmpItem;
                        }
                    iTmpItem = NULL;
                    i++;
                    if ( iItemList->ObjectCount() >
                         KContentDirCommitAmount )
                        {
                        // the list is full, next share it
                        canAppend = EFalse;
                        // save the current position in list for
                        // next run
                        iObjectIdx = i;
                        }
                    }
                else
                    {
                    i++;
                    iTmpItem = NULL;
                    }
                CleanupStack::PopAndDestroy(); // IntToDesC8LC

                }
            }
        CleanupStack::PopAndDestroy( items );

        if ( iItemList->ObjectCount() <= KContentDirCommitAmount )
            {
            // next time start with new album
            iSharingProgress += iObjectIdx;
            iObjectIdx = 0;
            iMetadataUtility->ClearPostFiltersL();
            }


        if ( iItemList->ObjectCount() )
            {
            iSharingSubState = EProfileItemList;
            SelfComplete( KErrNone );
            }
        else
            {
            // goto RunL and update indexes
            iMetadataUtility->ClearPostFiltersL();
            iSharingSubState = EShareItemListResult;
            SelfComplete( KErrNone );
            }
        }
        break;
    default :
        break;
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareAllItemsL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareAllItemsL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    delete iFileNameArr;
    iFileNameArr = NULL;
    iFileNameArr = new (ELeave)
        CDesCArrayFlat( 4 );

    DeleteListUpnpObject();
    iItemList = CUpnpItemList::NewL();

    switch ( iContainerType )
        {
    case EImageAndVideo:
        {
        if ( !iAllImagesShared )
            {
            AppendImageFilesToListL();
            }

        // do not share videos if the list is already full
        if ( !iAllVideosShared &&
             iItemList->ObjectCount() <= KContentDirCommitAmount )
            {
            AppendVideoFilesToListL();
            }
        }
        break;
    case EPlaylist:
        {
        const MCLFItemListModel* list(NULL);
        list = &iMetadataUtility->MusicFiles();
        if ( !list )
            {
            User::Leave( KErrArgument );
            }
        TInt musicCount = list->ItemCount();

        if ( iObjectIdx < musicCount )
            {
            // sharing images
            TBool canAppend( list->ItemCount() );
            TInt i( iObjectIdx );

            while ( i < list->ItemCount() && canAppend )
                {
                // process all items in curret collection
                HBufC8* id = UPnPCommonUtils::IntToDesC8LC(
                    iContainerId );
                TRAPD( plErr, iTmpItem = iMetadataUtility->CreateItemL(
                           list->Item( i ),
                           *id ));
                if ( !plErr && iTmpItem )
                    {
                    TPtrC fullFileName;
                    list->Item( i ).GetField( ECLFFieldIdFileNameAndPath,
                                              fullFileName );

                    if ( !plErr )
                        {
                        iMetadataUtility->UpdateMetadataL( 
                            EMusicFile,
                            iTmpItem,
                            fullFileName );
                        iTmpItem->SetObjectClassL( KMusicItemObjectClass );
                        iItemList->AppendObjectL( *iTmpItem );
                        iFileNameArr->AppendL( fullFileName );
                        }
                    else
                        {
                        delete iTmpItem;
                        }
                    iTmpItem = NULL;
                    i++;
                    if ( iItemList->ObjectCount() >
                         KContentDirCommitAmount )
                        {
                        // the list is full, next share it
                        canAppend = EFalse;
                        }
                    }
                else
                    {
                    i++;
                    iTmpItem = NULL;
                    }

                CleanupStack::PopAndDestroy(); // IntToDesC8LC
                }
            // save the current position in list, for next run
            iObjectIdx = i;
            }

        }
        break;
    default :
        break;
        }

    // do not share empty list
    if ( iItemList->ObjectCount() )
        {
        iSharingSubState = EProfileItemList;
        SelfComplete( KErrNone );
        }
    else
        {
        // update indexes
        iSharingSubState = EShareItemListResult;
        SelfComplete( KErrNone );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );

    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ProfileItemListL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ProfileItemListL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( iProfileInd < iItemList->ObjectCount() )
        {
        iProfilerActive = ETrue;

        CUpnpItem*
            tmpItem = static_cast<CUpnpItem*>(
                iItemList->operator[]( iProfileInd ));

        TPtrC fileName = iFileNameArr->MdcaPoint( iProfileInd );

        iSharingSubState = EProfileItemListResult;

        TRAPD( err, UpnpCdsResElementUtility::AddResElementL(
                   *tmpItem,
                   fileName ));
        if ( iDeletePending )
            {
            DeleteListUpnpObject();
            delete iFileNameArr;
            iFileNameArr = NULL;
            }
        else
            {
            SetActive();
            TRequestStatus* stat = &iStatus;
            User::RequestComplete( stat, err );
            }
#ifdef _DEBUG
        HBufC* log = HBufC::NewLC(512);
        log->Des().Append(_L("CUpnpContentSharingAo: Profiling: "));
        log->Des().Append( fileName );
        __LOG16( *log );
        __LOG2( "CUpnpContentSharingAo: Profile index: (%d of %d)", 
                       iProfileInd, 
                       iItemList->ObjectCount());
        CleanupStack::PopAndDestroy( log );
#endif
        iProfilerActive = EFalse;
        }
    else
        {
        // update indexes
        iProfileInd = 0;
        iSharingSubState = EShareItemListResult;
        SelfComplete( KErrNone );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ProfileItemListResult
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ProfileItemListResult()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iStatus.Int() )
        {
        CUpnpItem* tmpItem = static_cast<CUpnpItem*>(
            iItemList->operator[]( iProfileInd ));
        TPtrC8 id( tmpItem->Id() );
        iItemList->RemoveAndDestroy( id );
        
        iFileNameArr->Delete(iProfileInd);  
        }
    else
        {
        iProfileInd++; 
        }

    if ( iProfileInd < iItemList->ObjectCount() )
        {
        iSharingSubState = EProfileItemList;
        }
    else
        {
        iProfileInd = 0;
        delete iFileNameArr;
        iFileNameArr = NULL;
        iSharingSubState = EShareItemList;
        }
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareItemListL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareItemListL( const TInt& aParentId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // do not share empty list
    if ( iItemList && iItemList->ObjectCount() )
        {
        iFileSharing->ShareItemListL(
            *UPnPCommonUtils::IntToDesC8LC( aParentId ),
            *iItemList,
            iSharedContainersCount, iStatus );
        CleanupStack::PopAndDestroy(); // UPnPCommonUtils::IntToDesC8LC
        iSharingSubState = EShareItemListResult;
        SetActive();
        }
    else
        {
        // update indexes
        iSharingSubState = EShareItemListResult;
        SelfComplete( KErrNone );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareItemListResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareItemListResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iTmpItem = NULL;
    if ( iSharingState == EShareItems )
        {
        ShareItemsResultL();
        }
    else if ( iSharingState == EShareAllItems )
        {
        AllItemsResultL();
        }
    iSharingSubState = EIdle;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::AllItemsResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::AllItemsResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt totalItemCount( 0 );

    if ( iItemList )
        {
        // This updates the field needed in UI for progress
        iSharingProgress += iItemList->ObjectCount();
        iEngine->SetProgressL( iSharingProgress );
        DeleteListUpnpObject();
        delete iFileNameArr;
        iFileNameArr = NULL;
        }
    iTmpItem = NULL;

    switch ( iContainerType )
        {
    case EImageAndVideo:
        {
        totalItemCount = iMetadataUtility->ImageFiles().ItemCount() +
            iMetadataUtility->VideoFiles().ItemCount();
        }
        break;
    case EPlaylist :
        {
        totalItemCount = iMetadataUtility->MusicFiles().ItemCount();
        }
        break;
    default :
        break;
        }
    // Fix EYSN-7CWDZ5 error in UPnP FW2.1  
    // Have not changed it in UPnP FW2.5 because the FW 2.5 have 
    // so much change. 
    if ( iObjectIdx < totalItemCount )
        {
        // Continue sharing process since not everything is shared yet.
        iSharingState = EShareAllItems;
        }
    else
        {
        if ( !iWriter )
            {
            User::Leave( KErrGeneral );
            }
        iWriter->SaveSharingStateL( iShareAllSelection );
        iSharingState = ENotActive;

        }

    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareContainersResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::ShareContainersResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );


    if ( !iStatus.Int() )
        {
        // previous container shared OK, save the album name
        iWriter->AppendStatusL( iContainerIdx );
        }

    if ( iTmpContainer )
        {
        TInt newId = UPnPCommonUtils::DesC8ToInt( iTmpContainer->Id() );
        __LOG1("CUpnpContentSharingAo::ShareContainersResultL:\
 id: %d", newId );
        if ( !iSharedContainerIds )
            {
            iSharedContainerIds =
                new (ELeave) CArrayFixFlat<TInt>( 10 );
            }
        iSharedContainerIds->AppendL( newId );
        delete iTmpContainer;
        iTmpContainer = NULL;
        }

    TInt totalContainers( 0 );
    // determine next state
    if ( iShareAllSelection == EShareAll )
        {
        switch ( iContainerType )
            {
            // totalContainers is used as index -> -1
        case EImageAndVideo :
            {
            totalContainers = iContainerIds->MdcaCount() -1;
            }
            break;
        case EPlaylist :
            {
            totalContainers = iContainerIds->MdcaCount() -1;
            }
            break;
        default:
            break;
            }
        }
    else if ( iShareAllSelection == EShareMany )
        {
        totalContainers = iCurrentSelections.Count() -1;
        }

    if ( iContainerIdx < totalContainers )
        {
        iContainerIdx++;
        iSharingState = EShareContainers;
        }
    else
        {
        // all selected containers shared, next share items
        iContainerIdx = 0;
        iSharingState = EShareItems;
        }
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );

    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::ShareItemsResultL
// ( other items are commented in header )
// --------------------------------------------------------------------------
void CUpnpContentSharingAo::ShareItemsResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( !iMetadataUtility )
        {
        User::Leave( KErrGeneral );
        }

    if ( iItemList )
        {
        // This updates the field needed in UI for progress
        iSharingProgress += iItemList->ObjectCount();
        iEngine->SetProgressL( iSharingProgress );
        DeleteListUpnpObject();
        delete iFileNameArr;
        iFileNameArr = NULL;
        }
    iTmpItem = NULL;
    iSharingState = EShareItems;

    TInt totalContainers( 0 );
    // Clear postfilters, they are re-set if needed: ShareItemsL/
    // ShareAllItemsL
    iMetadataUtility->ClearPostFiltersL();
    // determine next state
    if ( iShareAllSelection == EShareAll )
        {
        switch ( iContainerType )
            {
        case EImageAndVideo :
            {
            totalContainers = iContainerIds->MdcaCount() -1;
            }
            break;
        case EPlaylist :
            {
            totalContainers = iContainerIds->MdcaCount() -1;
                
            }
            break;
        default:
            break;
            }
        }
    else if ( iShareAllSelection == EShareMany )
        {
        if( iSharedContainerIds )
            {
            totalContainers = iSharedContainerIds->Count() -1;
            }
        else
            {
            // sharing of all containers failed or the containers under 
            // sharing were deleted
            totalContainers = KErrNotFound;
            }
        }


    if ( !iObjectIdx )
        {

        // all files were shared from current container
        if ( iContainerIdx < totalContainers )
            {
            // more items in the selected..
            iContainerIdx++;
            }
        else
            {
            // all selected containers shared..
            if ( iShareAllSelection == EShareAll )
                {
                // next share individual files
                iSharingState = EShareAllItems;
                }
            else
                {
                // sharing process is done.
                if ( totalContainers == KErrNotFound )
                    {
                    iShareAllSelection = EShareNone;
                    }
                iWriter->SaveSharingStateL( iShareAllSelection );
                iSharingState = ENotActive;
                }
            }
        }

    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::SelectionObjectCountL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::SelectionObjectCountL(
    const TUpnpMediaType aContainerType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt totalCount( 0);
    if ( iShareAllSelection != EShareNone )
        {
        switch ( aContainerType )
            {
        case EImageAndVideo :
            {
            if ( !iCollections ) 
                {
                iCollections = CUPnPAlbumServices::NewL();
                }
            __LOG("albumserv created");
            CDesCArray* collIds = new ( ELeave ) CDesCArrayFlat( 
                KDefaultGranularity );
            CleanupStack::PushL( collIds );
            CDesCArray* collNames = new ( ELeave ) CDesCArrayFlat( 
                KDefaultGranularity );
            CleanupStack::PushL( collNames );
            iCollections->ListAlbumsL( *collIds, *collNames );
            __LOG("albums listed");
            collNames->Reset();

            if ( iShareAllSelection == EShareAll )
                {
                __LOG("EShareAll");
                for( TInt i(0); i<collIds->MdcaCount(); i++ )
                    {
                    TPtrC id( collIds->MdcaPoint( i ) );
                    RDebug::Print(_L("id: %S"), &id );
                    TRAPD( lcode, iCollections->OpenAlbumL( id,
                                                            *collNames ));
                    if( !lcode )
                        {
                        totalCount += collNames->MdcaCount();
                        }
                    else
                        {
                        }
                    collNames->Reset();
                    __LOG("Loop cycle end");
                    }
                __LOG("Loop done");
                // Add individual files count
                const MCLFItemListModel* imgList =
                    &iMetadataUtility->ImageFiles();
                totalCount += imgList->ItemCount();
                const MCLFItemListModel* vidList =
                    &iMetadataUtility->VideoFiles();
                totalCount += vidList->ItemCount();
                __LOG("EShareAll done");
                }
            else if ( iShareAllSelection == EShareMany )
                {
                for( iContainerIdx = 0; 
                     iContainerIdx < iCurrentSelections.Count();
                     iContainerIdx++ )
                    {
                    TInt ind = GetContainerIndexL(); 

                    if ( ind != KErrNotFound )
                        {
                        TPtrC id = iContainerIds->MdcaPoint( ind );
                        TRAPD( openErr, iCollections->OpenAlbumL( 
                                   id, 
                                   *collNames ));
                        if( !openErr )
                            {
                            totalCount += collNames->MdcaCount();
                            collNames->Reset();
                            }
                        }
                    __LOG("end for(..)");
                    }
                iContainerIdx = 0;
                }
            delete iCollections;
            iCollections = NULL;
            __LOG("iCollections deleted");
            CleanupStack::PopAndDestroy( collNames );
            CleanupStack::PopAndDestroy( collIds );
            }
            break;
        case EPlaylist :
            {
            if ( !iPlaylists ) 
                {
                iPlaylists = CUPnPPlaylistServices::NewL();
                }

            CDesCArray* plIds = new ( ELeave ) CDesCArrayFlat( 
                KDefaultGranularity );
            CleanupStack::PushL( plIds );
            CDesCArray* plNames = new ( ELeave ) CDesCArrayFlat( 
                KDefaultGranularity );
            CleanupStack::PushL( plNames );
            iPlaylists->ListPlaylistsL( *plIds, *plNames );
            plNames->Reset();

            if ( iShareAllSelection == EShareAll )
                {
                for( TInt i(0); i<plIds->MdcaCount(); i++ )
                    {
                    TPtrC id( plIds->MdcaPoint( i ) );
                    iPlaylists->OpenPlaylistL( id,
                                               *plNames );

                    totalCount += plNames->MdcaCount();
                    plNames->Reset();
                    }
                // Add individual files count
                const MCLFItemListModel* musicList =
                    &iMetadataUtility->MusicFiles();
                totalCount += musicList->ItemCount();
                }
            else if ( iShareAllSelection == EShareMany )
                {
                for( iContainerIdx = 0; 
                     iContainerIdx < iCurrentSelections.Count();
                     iContainerIdx++ )
                    {
                    TInt ind = GetContainerIndexL(); 

                    if ( ind != KErrNotFound )
                        {
                        TPtrC id = iContainerIds->MdcaPoint( ind );
                        iPlaylists->OpenPlaylistL( id, 
                                                   *plNames );
                        totalCount += plNames->MdcaCount();
                        plNames->Reset();
                        }
                    
                    }
                iContainerIdx = 0;
                }
            delete iPlaylists;
            iPlaylists = NULL;
            CleanupStack::PopAndDestroy( plNames );
            CleanupStack::PopAndDestroy( plIds );
            }
            break;
        default :
            {
            }
            break;
            }
        }
    __LOG1("CUpnpContentSharingAo::SelectionObjectCountL: totalCount: %d",
        totalCount );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return totalCount;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::SharingProgress
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::SharingProgress(
    TUpnpMediaType& aContainerType ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    aContainerType = iContainerType;
    __LOG1("CUpnpContentSharingAo::SharingProgress: iObjectIdx: %d",
        iSharingProgress );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return iSharingProgress;
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::GetContainerIndexL
// Returns IntValue for aObject
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::GetContainerIndexL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt ind( KErrNone );
    if ( iShareAllSelection == EShareAll )
        {
        if( iContainerIds->MdcaCount() )
            {
            ind = iContainerIdx;
            }
        else
            {
            ind = KErrNotFound;
            }
        }
    else if ( iContainerIdx >= iCurrentSelections.Count() ||
        ( iCurrentSelections[ iContainerIdx ]- KUiSelectionOffset 
          >= iContainerIds->MdcaCount() ))
        {
        ind = KErrNotFound;
        }
    else
        {
        if ( iShareAllSelection == EShareMany )
            {
            ind = iCurrentSelections[ iContainerIdx ] 
                - KUiSelectionOffset;
            
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ind; // correct index or KErrNotFound
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::SelfComplete
// Complete own request
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::SelfComplete( const TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iStopRequested )
        {
        iSharingState = ENotActive;
        }
    SetActive();
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aError );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpContentSharingAo::AppendImageFilesToListL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::AppendImageFilesToListL()
    {
    // Obtain list of image files from CLF
    const MCLFItemListModel* list(NULL);
    list = &iMetadataUtility->ImageFiles();
    if ( !list )
        {
        User::Leave( KErrArgument );
        }

    // check if the iImageIndex is still less than the number of images
    //       -> there are still images to be shared
    TInt imageCount = list->ItemCount();
    if ( !imageCount )
        {
        iAllImagesShared = ETrue;
        return;
        }

    if ( iImageIndex < imageCount )
        {

        // Prepare storage for filenames
        // Loop until all images are processed or until we have
        // KContentDirCommitAmount files in the list of objects to be shared.
        while ( !iAllImagesShared &&
                iItemList->ObjectCount() <= KContentDirCommitAmount )
            {
            // Create an item with parent id of Images&Videos container.
            HBufC8* id = UPnPCommonUtils::IntToDesC8LC(
                iContainerId );
            TRAPD( imgErr, iTmpItem = iMetadataUtility->CreateItemL(
                       list->Item( iImageIndex ),
                       *id ) );

            if ( !imgErr && iTmpItem )
                {
                TPtrC fullFileName;
                list->Item( iImageIndex ).GetField( 
                    ECLFFieldIdFileNameAndPath,
                    fullFileName );

                if ( !imgErr && iTmpItem )
                    {
                    // Update item's metadata and append it to iItemList.
                    iMetadataUtility->UpdateMetadataL( 
                        EPhotoFile,
                        iTmpItem,
                        fullFileName );
                    iTmpItem->SetObjectClassL( KImageItemObjectClass );
                    iItemList->AppendObjectL( *iTmpItem );
                    iFileNameArr->AppendL( fullFileName );
                    }
                else
                    {
                    delete iTmpItem;
                    }
                }
            else
                {
                }

            // set iTmpItem to NULL
            iTmpItem = NULL;

            // clean up
            CleanupStack::PopAndDestroy(); // IntToDesC8LC

            // increment counter to process next image.
            iImageIndex++;

            if ( iImageIndex >= imageCount )
                {
                iAllImagesShared = ETrue;
                }
            }

        }
    // update iObjectIdx to keep the progressbar ok.
    iObjectIdx = iImageIndex;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::AppendVideoFilesToListL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::AppendVideoFilesToListL()
    {
    // Obtain list of video files from CLF
    const MCLFItemListModel* list(NULL);
    list = &iMetadataUtility->VideoFiles();
    if ( !list )
        {
        User::Leave( KErrArgument );
        }

    // Obtain the number of video file items in the list.
    TInt videoCount( list->ItemCount() );
    if ( !videoCount )
        {
        iAllVideosShared = ETrue;
        return;
        }

    // Check that there are still videos to be shared.
    if ( iVideoIndex < videoCount )
        {

        if ( !iFileNameArr )
            {
            iFileNameArr = new (ELeave)
                CDesCArrayFlat( 4 );
            }
        // Loop until all videos are processed or until we have
        // KContentDirCommitAmount files in the list of objects to be shared.
        while ( !iAllVideosShared &&
                iItemList->ObjectCount() <= KContentDirCommitAmount )
            {
            // Create an item with parent id of Images&Videos container.
            HBufC8* id = UPnPCommonUtils::IntToDesC8LC(
                iContainerId );
            TRAPD( vidErr, iTmpItem = iMetadataUtility->CreateItemL(
                       list->Item( iVideoIndex ),
                       *id ) );

            if ( !vidErr && iTmpItem )
                {
                TPtrC fullFileName;
                list->Item( iVideoIndex ).GetField( 
                    ECLFFieldIdFileNameAndPath,
                    fullFileName );
               

                if ( !vidErr && iTmpItem )
                    {
                    // Update item's metadata and append it to iItemList.
                    iMetadataUtility->UpdateMetadataL( 
                        EVideoFile,
                        iTmpItem,
                        fullFileName );
                    iTmpItem->SetObjectClassL( KVideoItemObjectClass );
                    iItemList->AppendObjectL( *iTmpItem );
                    iFileNameArr->AppendL( fullFileName );
                    }
                else
                    {
                    delete iTmpItem;
                    iTmpItem = NULL;
                    }
                }
            else
                {
                }

            // set iTmpItem to NULL
            iTmpItem = NULL;

            // clean up
            CleanupStack::PopAndDestroy(); // IntToDesC8LC

            // increment counter to process next video.
            iVideoIndex++;

            if ( iVideoIndex >= videoCount )
                {
                // all videos are processed.
                // Set iAllVideosShared -> ETrue to end the while loop.
                iAllVideosShared = ETrue;
                }
            }
        }

    // update iObjectIdx to keep the progressbar ok.
    // Remember to add iImageIndex here!
    iObjectIdx = iImageIndex + iVideoIndex;
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::RequestStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentSharingAo::RequestStop( 
    MUpnpSharingCallback::TSharingStopType aStopType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iStopRequested = aStopType;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpContentSharingAo::GetPlaylistNameByIdL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpContentSharingAo::GetContainerNameByIdL( const TDesC& aId,
                                                   TDes8& aPlaylistName,
                                                   const TInt aContainerType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt err( KErrNone );
    CDesCArray* newIds = new ( ELeave )CDesCArrayFlat( 4 );
    CleanupStack::PushL(newIds);
    CDesCArray* newNames = new ( ELeave )CDesCArrayFlat( 4 );
    CleanupStack::PushL(newNames);

    if( aContainerType == EImageAndVideo )
        {
        if ( !iCollections ) 
            {
            iCollections = CUPnPAlbumServices::NewL();
            }
        iCollections->ListAlbumsL( *newIds, *newNames );
        delete iCollections;
        iCollections = NULL;
        }
    else if ( aContainerType == EPlaylist )
        {
        if ( !iPlaylists ) 
            {
            iPlaylists = CUPnPPlaylistServices::NewL();
            }
        iPlaylists->ListPlaylistsL( *newIds, *newNames );
        delete iPlaylists;
        iPlaylists = NULL;
        }

    if ( !newIds->MdcaCount() )
        {
        err = KErrGeneral;
        }
    else
        {
        for ( TInt i(0); i<newIds->MdcaCount();i++ )
            {
            if ( aId.Compare( newIds->MdcaPoint( i ) ) == 0 )
                {
                HBufC8* name8 = 
                    UpnpString::FromUnicodeL( newNames->MdcaPoint( i ) );
                CleanupStack::PushL( name8 );
                TPtr8 name8tmp = name8->Des();
                aPlaylistName = name8tmp;
                CleanupStack::PopAndDestroy( name8 );
                }
            }
        }
    CleanupStack::PopAndDestroy( newNames );
    CleanupStack::PopAndDestroy( newIds );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

void CUpnpContentSharingAo::DeleteListUpnpObject()
    {
    if ( iItemList )
        {
        TInt count = iItemList->ObjectCount();
        for ( TInt i = count - 1; i >= 0; --i )
            {
            CUpnpItem* tmpItem = static_cast<CUpnpItem*>(
                iItemList->operator[]( i ));
            TPtrC8 id( tmpItem->Id() );
            iItemList->RemoveAndDestroy( id ); 
            } 
        delete iItemList;
        iItemList = NULL;
        }
    }
// End of file
