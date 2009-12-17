/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  handles music playing within UPnP framework
*
*/


// INCLUDE FILES
// MPX framework api
#include <mpxcommonframeworkdefs.h>
#include <mpxcollectionplaylist.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxplaybackmessage.h>
#include <mpxviewutility.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h"
#include "upnpavbrowsingsession.h"
#include "upnpavdevice.h"

// musicadapter internal
#include "upnpmpxhelper.h"
#include "upnpplaylistfiller.h"
#include "upnpmusicadapter.h"

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// constants
_LIT16( KTempPlaylistTitle, "TempPlaylist" );
const TUid KRemotePlayerUid = { 0x200075D8 }; // Remote player UID
const TUid KLocalPlayerUid = { 0x200075D9 };  // Local player UID
const TUid KHomeConnectUid = { 0x10208A0A };  // Home Connect app UID
const TUid KPlaybackDialogUid = { 0x101FFCBD }; // UPnPPlayback dialog UID

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPMusicAdapter* CUPnPMusicAdapter::NewL( 
MUPnPAVController& aAVController )
    {
    __LOG( "CUPnPMusicAdapter::NewL" );
    
    CUPnPMusicAdapter* self = new(ELeave) CUPnPMusicAdapter( aAVController );
    __LOG1( "CUPnPMusicAdapter: self = %x", self );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::CUPnPMusicAdapter
// Default constructor.
// --------------------------------------------------------------------------
//
CUPnPMusicAdapter::CUPnPMusicAdapter( MUPnPAVController& aAVController )
    : iAVController( aAVController )
    {
    iTrackArray = 0;
    iPlaylist = 0;
    iState = EStateIdle;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::ConstructL
// 1st phase constructor.
// --------------------------------------------------------------------------
// 
void CUPnPMusicAdapter::ConstructL()
    {
    __LOG( "CUPnPMusicAdapter::ConstructL" );
    
    // Create playbackutility for playback comands
    iPlaybackUtility = MMPXPlaybackUtility::NewL( KHomeConnectUid, this );
    __LOG1( "iPlaybackUtility = %x", iPlaybackUtility );

    // Create collectionutility 
    iCollectionUtility = MMPXCollectionUtility::NewL( this, 
    KHomeConnectUid );
    __LOG1( "iCollectionUtility = %x", iCollectionUtility );
            
    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();
    __LOG1( "iViewUtility = %x", iViewUtility );
    
    // internal MPX helper for some advanced operations
    iMpxHelper = CUPnPMpxHelper::NewL( KHomeConnectUid );

    __LOG( "CUPnPMusicAdapter::ConstructL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::~CUPnPMusicAdapter
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPMusicAdapter::~CUPnPMusicAdapter()
    {
    __LOG( "CUPnPMusicAdapter::~CUPnPMusicAdapter()" );

    if ( iCurrentFillerState == EFillerStateFilling )
        {
        iCurrentFiller->CancelFill();
        }

    delete iMedia;
    delete iCurrentFiller;    
    delete iTrackArray;    
    delete iPlaylist;
    delete iWait;
    delete iMpxHelper;

    if( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        iPlaybackUtility = 0;
        }
        
        
    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = 0;
        }
    
    if( iViewUtility )
        {
        iViewUtility->Close();
        iViewUtility = 0;
        }
    __LOG( "CUPnPMusicAdapter::~CUPnPMusicAdapter - End" );
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::ExecuteMusicLiteInRemoteL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMusicAdapter::ExecuteMusicLiteInRemoteL(
    CEikAppUi* aAppUi,
    MUPnPPlaylistFiller* aFiller,
    const CUpnpAVDevice& aRenderer )
    {
    __LOG( "CUPnPMusicAdapter::ExecuteMusicLiteInRemoteL" );

    InitState();
    iAppUi = aAppUi;     
    iRemotePlayer = ETrue;

    TakeOwnershipOfTheFiller( aFiller );
    iTargetDevice = &aRenderer;

    // Create and fill playlist
    CreatePlayListL();

    // show the UI
    UiShowLoopL();
 
     __LOG1( "CUPnPMusicAdapter::ExecuteMusicInRemoteL returns %d",
        iReturnCode );
    return iReturnCode;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::ExecuteMusicLiteInLocalL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMusicAdapter::ExecuteMusicLiteInLocalL(
    CEikAppUi* aAppUi,
    MUPnPPlaylistFiller* aFiller )
    {
    __LOG( "CUPnPMusicAdapter::ExecuteMusicLiteInLocalL" );

    InitState();
    iAppUi = aAppUi;
    iRemotePlayer = EFalse;
    
    TakeOwnershipOfTheFiller( aFiller );
    iTargetDevice = 0;
    
    // Create and fill playlist
    CreatePlayListL();

    // show the UI
    UiShowLoopL();

    __LOG1( "CUPnPMusicAdapter::ExecuteMusicInLocalL returns %d",
        iReturnCode );
    return iReturnCode;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::ExecuteMusicInRemoteL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMusicAdapter::ExecuteMusicInRemoteL(
    CEikAppUi* aAppUi,
    CUPnPPlayListFiller* aFiller,
    const CUpnpAVDevice& aRenderer )
    {
    __LOG( "CUPnPMusicAdapter::ExecuteMusicInRemoteL" );

    InitState();
    iAppUi = aAppUi;     
    iRemotePlayer = ETrue;

    TakeOwnershipOfTheFiller( aFiller );
    iTargetDevice = &aRenderer;

    // Create and fill playlist
    CreatePlayListL();

    // show the UI
    UiShowLoopL();
 
     __LOG1( "CUPnPMusicAdapter::ExecuteMusicInRemoteL returns %d",
        iReturnCode );
    return iReturnCode;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::ExecuteMusicInLocalL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPMusicAdapter::ExecuteMusicInLocalL(
    CEikAppUi* aAppUi,
    CUPnPPlayListFiller* aFiller )
    {
    __LOG( "CUPnPMusicAdapter::ExecuteMusicInLocalL" );

    InitState();
    iAppUi = aAppUi;
    iRemotePlayer = EFalse;
    
    TakeOwnershipOfTheFiller( aFiller );
    iTargetDevice = 0;
    
    // Create and fill playlist
    CreatePlayListL();

    // show the UI
    UiShowLoopL();

    __LOG1( "CUPnPMusicAdapter::ExecuteMusicInLocalL returns %d",
        iReturnCode );
    return iReturnCode;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::Dismiss
// Dismiss the music player UI
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPMusicAdapter::Dismiss( TInt aReturnCode )
    {
    Exit( aReturnCode );
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::UiShowLoop
// Wait to show the UI
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::UiShowLoopL()
    {
    // ----------------------------------------
    // PART 1 - wait part
    // ----------------------------------------
    if ( iState == EStatePreparing )
        {
        // Not yet ready to go. Wait until signalled to proceed
        iWait = new (ELeave) CActiveSchedulerWait();
        __LOG( "UiShowLoopL: wait..." );
        iState = EStatePreparingWait;
        iWait->Start();
        delete iWait;
        iWait = 0;
        }
    else 
        {
        __LOG( "UiShowLoopL: no need to wait" );
        }

    // ----------------------------------------
    // PART 2 - action part
    // ----------------------------------------
    if ( iState == EStateInitialising )
        {
        __LOG( "Activate playback view" );  
        iViewUtility->ActivateViewL( KPlaybackDialogUid );
        iState = EStateClosing;
        }
    else if ( iState == EStateClosing )
        {
        // Exit signalled before UI was shown
        }
    else
        {
        // illegal state
        __PANICD( __FILE__, __LINE__ );
        }
    iState = EStateIdle;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::DisplayUI
// Commands the UI to show
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::DisplayUI()
    {
    __LOG1( "CUPnPMusicAdapter::DisplayUI() in state %d",
        iState );
    if ( iState == EStatePreparing )
        {
        // UiShowLoop can show UI directly - no wait
        iState = EStateInitialising;
        }
    else if ( iState == EStatePreparingWait )
        {
        // release wait loop in UiShowLoop
        __ASSERTD( iWait != 0, __FILE__, __LINE__ );
        iState = EStateInitialising;
        iWait->AsyncStop();
        }
    else
        {
        // should never be here
        __PANICD( __FILE__, __LINE__ );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::Exit
// Performs cleanup and releases the execute thread
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::Exit( TInt aReturnCode )
    {
    __LOG2( "CUPnPMusicAdapter::Exit(%d) in state %d",
        aReturnCode, iState );
    if ( iState == EStatePreparing )
        {
        // change state - UiShowLoop will exit immediately
        iReturnCode = aReturnCode;
        iState = EStateClosing;
        }
    if ( iState == EStatePreparingWait )
        {
        // release wait loop in UiShowLoop
        __ASSERTD( iWait != 0, __FILE__, __LINE__ );
        iReturnCode = aReturnCode;
        iState = EStateClosing;
        iWait->AsyncStop();
        }
    else if ( iState == EStateInitialising ||
        iState == EStateRunning )
        {
        __ASSERTD( iViewUtility != 0, __FILE__, __LINE__ );
        iReturnCode = aReturnCode;
        iState = EStateClosing;
        // Send a close command to plugin with this error code.
        // Plugin will notify that it is not available,
        // and then the player UI will exit
        TRAP_IGNORE( iPlaybackUtility->CommandL( 
            EPbCmdClose, KMusicPluginMessageExit ) );
        }
    else if ( iState == EStateClosing )
        {
        // do nothing - already in exit process
        }
    else
        {
        // should never be here
        __PANICD( __FILE__, __LINE__ );
        }
    }


// --------------------------------------------------------------------------
// CUPnPMusicAdapter::CreatePlayList
// --------------------------------------------------------------------------
//    
void CUPnPMusicAdapter::CreatePlayListL()
    {
    __LOG( "CUPnPMusicAdapter::CreatePlayList" );
    __ASSERTD( iState == EStateIdle, __FILE__, __LINE__ );
    iState = EStatePreparing;

    // Get collection Uid
    iPlaylistCollectionId = KInMemoryCollection;
    iPlaylistCollectionId = iCollectionUtility->Collection()
        .CollectionIDL( iPlaylistCollectionId );

    // Create and fill the playlist
    iTrackArray = CMPXMediaArray::NewL();
    __LOG( "Filling object list" );
    iCurrentFiller->FillL( *this, *iTrackArray );
    if ( iCurrentFillerState == EFillerStateIdle )
        {
        iCurrentFillerState = EFillerStateFilling;
        }

    iMedia = CMPXMedia::NewL();
    // set collection id
    iMedia->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, iPlaylistCollectionId );
    // Set type
    iMedia->SetTObjectValueL<TMPXGeneralType>( 
        KMPXMediaGeneralType, EMPXItem );
    // Set category
    iMedia->SetTObjectValueL<TMPXGeneralCategory>( 
        KMPXMediaGeneralCategory, EMPXPlaylist );
    // Set title
    iMedia->SetTextValueL( 
        KMPXMediaGeneralTitle, KTempPlaylistTitle );
    // set contents
    iMedia->SetCObjectValueL( 
        KMPXMediaArrayContents, iTrackArray );  
    // set size
    __LOG1( "Set Array count=%d", iCurrentFiller->PlaylistSize() );
    iMedia->SetTObjectValueL( 
        KMPXMediaArrayCount,  iCurrentFiller->PlaylistSize() );

    __LOG( "Create collection playlist" );
    // Create collection playlist
    iMpxHelper->AddAndOpenL( KHomeConnectUid, *iMedia );

    __LOG( "CUPnPMusicAdapter::CreatePlayList - end" );
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleCollectionMessage
// From MMPXCollectionObserver.
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandleCollectionMessage(CMPXMessage* aMsg, 
    TInt aErr )
    {
    __LOG1( "CUPnPMusicAdapter::HandleCollectionMessage: error %d", aErr );
    if( aMsg && aErr == KErrNone )
        {        
        TInt event = KErrNotSupported;
        TInt type = KErrNotSupported;
        TInt data = KErrNotSupported;
        
        if( aMsg->IsSupported( KMPXMessageGeneralEvent ) )
            {
            event = *aMsg->Value<TInt>( KMPXMessageGeneralEvent );
            }
        if( aMsg->IsSupported( KMPXMessageGeneralType ) )
            {
            type = *aMsg->Value<TInt>( KMPXMessageGeneralType );
            }
        if( aMsg->IsSupported( KMPXMessageGeneralData ) )
            {
            data = *aMsg->Value<TInt>( KMPXMessageGeneralData );
            }

        TRAPD( error, DoHandleCollectionMessageL( event, type, data ) );
        if ( error != KErrNone )
            {
            __LOG1( "DoHandleCollectionMessageL: leave with %d", error );
            Exit( error );
            }
        }
    else
        {
        Exit( aErr );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::DoHandleCollectionMessageL
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::DoHandleCollectionMessageL(
    TInt aEvent, TInt /*aType*/, TInt /*aData*/ )
    {
    __LOG( "CUPnPMusicAdapter::DoHandleCollectionMessageL" );

    switch ( aEvent )
        {
        case TMPXCollectionMessage::EPathChanged:
            {
            __LOG( "HandleCollectionMessageL: EPathChanged" );

            if ( iState == EStatePreparing || iState == EStatePreparingWait )
                {
                __LOG( "Pick up playlist from collection" );
                CMPXCollectionPath* path = 
                iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( path );
                // select track with given index
                __LOG1( "Set selected index = %d",
                    iCurrentFiller->SelectedIndex() );
                path->Set( iCurrentFiller->SelectedIndex() );

                __LOG( "Ccreating new playlist" );
                delete iPlaylist;
                iPlaylist = NULL;
                iPlaylist = CMPXCollectionPlaylist::NewL( *path );
                __LOG1( "iPlaylist = %x", iPlaylist );
                iPlaylist->SetEmbeddedPlaylist( ETrue );
                CleanupStack::PopAndDestroy( path );

                // Disable repeat and shuffle modes
                iPlaylist->SetRepeatEnabled( EFalse );
                iPlaylist->SetShuffleEnabledL( EFalse );

                // Get subplayers if remote playback
                if( iRemotePlayer )
                    {
                    __LOG( "calling SubPlayerNamesL" );
                    iPlaybackUtility->PlayerManager().SubPlayerNamesL( *this,
                        KRemotePlayerUid );
                    }
                else // Local playback. Get subplayer is no needed.
                    {
                    __LOG( "calling SelectPlayerL(local)" );
                    iPlaybackUtility->PlayerManager().SelectPlayerL(
                        KLocalPlayerUid );
                    __LOG("calling InitL");
                    iPlaybackUtility->InitL( *iPlaylist, ETrue );          
                    }
                }
            }
            break;
        default:
            {
            } 
            break;
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleSubPlayerNamesL
// From MMPXPlaybackCallback 
// --------------------------------------------------------------------------
//  
void CUPnPMusicAdapter::HandleSubPlayerNamesL( TUid aPlayer, 
    const MDesCArray* aSubPlayers, TBool aComplete, TInt aError )
    {
    TRAPD( error, DoHandleSubPlayerNamesL(
        aPlayer, aSubPlayers, aComplete, aError ) );
    if ( error != KErrNone )
        {
        __LOG1( "HandleSubPlayerNamesL: leave with %d", error );
        Exit( error );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::DoHandleSubPlayerNamesL
// --------------------------------------------------------------------------
//  
void CUPnPMusicAdapter::DoHandleSubPlayerNamesL( TUid /*aPlayer*/, 
    const MDesCArray* aSubPlayers, TBool /*aComplete*/, TInt aError )
    {
    __LOG( "CUPnPMusicAdapter::HandleSubPlayerNamesL" );

    if( aError == KErrNone)
        {
        // Find out Subplayer index
        TInt count = aSubPlayers->MdcaCount();
        TInt subplayerIndex = KErrNotFound; 
            
        TBuf<256> id16;
        id16.Copy( iTargetDevice->FriendlyName() );
                
        for( TInt i = 0; i < count; i++ )
            {
            // If subplayer match.
            if( !aSubPlayers->MdcaPoint(i).Compare( id16 ) )
                {
                subplayerIndex = i;
                break;
                }
            }   
            
        if( subplayerIndex != KErrNotFound )
            {
            __LOG1( "calling SelectSubPlayerL(%d)", subplayerIndex );
            iPlaybackUtility->PlayerManager().SelectSubPlayerL(
            KRemotePlayerUid, subplayerIndex );
                    
            iTargetDevice = 0; 
                                                        
            __LOG( "calling InitL" );
            iPlaybackUtility->InitL( *iPlaylist, ETrue );             
            }
       else
            {
            __LOG( "HandleSubPlayerNamesL: subplayer not found! " );
            Exit( KErrNotFound );
            } 
        }
    else // Error with SubPlayerNamesL
        {
        __LOG1( "HandleSubPlayerNamesL: Error=%d", aError );
        Exit( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandlePlaybackMessage
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandlePlaybackMessage(CMPXMessage* aMsg,
    TInt aErr )
    {
    __LOG1( "HandlePlaybackMessage: error %d", aErr );
    if( aMsg && aErr == KErrNone )
        {
        if (aMsg->IsSupported(KMPXMessageGeneralId))
            {
            TInt id = static_cast<TInt>(
                    *aMsg->Value<TMPXMessageId>(KMPXMessageGeneralId));
            TInt error(KErrNone);
            if ( id==KMPXMessageGeneral )
                {                    
                TInt event = KErrNotSupported;
                TInt type = KErrNotSupported;
                TInt data = KErrNotSupported;
                
                if( aMsg->IsSupported( KMPXMessageGeneralEvent ) )
                    {
                    event = *aMsg->Value<TInt>( KMPXMessageGeneralEvent );
                    }
                if( aMsg->IsSupported( KMPXMessageGeneralType ) )
                    {
                    type = *aMsg->Value<TInt>( KMPXMessageGeneralType );
                    }
                if( aMsg->IsSupported( KMPXMessageGeneralData ) )
                    {
                    data = *aMsg->Value<TInt>( KMPXMessageGeneralData );
                    }    
                    
                TRAP( error, DoHandlePlaybackMessageL( event, type, data ) );
                }
            if ( error != KErrNone )
                {
                __LOG1( "DoHandlePlaybackMessage: leave with %d", error );
                Exit( error );
                }
            } // else dummy message
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::DoHandlePlaybackMessageL
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::DoHandlePlaybackMessageL( 
    TInt aEvent, TInt /*aType*/, TInt /*aData*/ )
    {
    __LOG( "CUPnPMusicAdapter::DoHandlePlaybackMessageL" );

    switch ( aEvent )
        {
        case TMPXPlaybackMessage::EPlayerChanged:
            {
            if ( iState == EStatePreparing ||
                iState == EStatePreparingWait )
                {
                DisplayUI();
                }
            }   
            break;                  
        case TMPXPlaybackMessage::EInitializeComplete:
            {
            if ( iState == EStateInitialising )
                {
                __LOG( "Calling CommandL( EPbCmdPlay )" );
                iState = EStateRunning;
                iPlaybackUtility->CommandL( EPbCmdPlay );
                }
            }
            break;
        default:
            {
            break;
            }
        }        
    }


// --------------------------------------------------------------------------
// Unused callbacks here
// --------------------------------------------------------------------------

    
// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandlePropertyL
// From MMPXPlaybackCallback 
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandlePropertyL( 
    TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/, TInt /*aError*/ )
    {
    __LOG( "CUPnPMusicAdapter::HandlePropertyL - Not used." );
    // Not used.
    }


// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleMediaL
// From MMPXPlaybackCallback 
// --------------------------------------------------------------------------
// 
void CUPnPMusicAdapter::HandleMediaL( 
    const CMPXMedia& /*aProperties*/, TInt /*aError*/ )
    {
    __LOG( "CUPnPMusicAdapter::HandleMediaL - Not used" );
    // Not used.
    }



// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleCollectionMediaL
// From MMPXCollectionObserver.
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandleCollectionMediaL(
    const CMPXMedia& /*aMedia*/, 
    TInt /*aError*/ )
    {
    __LOG( "CUPnPMusicAdapter::HandleCollectionMediaL - Not used!" );
    // Not used
    }


// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleOpenL
// From MMPXCollectionObserver.
// Handles the collection entries being opened.
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandleOpenL(
    const CMPXMedia& /* aEntries */,
    TInt /* aIndex */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    __LOG( "CUPnPMusicAdapter::HandleOpenL - not used!" );
    // Not used
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleOpenL
// From MMPXCollectionObserver.
// Handles the item being opened.
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    __LOG( "CUPnPMusicAdapter::HandleOpenL - not used!" );
    // Not used
    }
 
 // --------------------------------------------------------------------------
// CUPnPMusicAdapter::HandleEmbeddedOpenL
// From MMPXCHelperEmbeddedOpenObserver
// Handles errors from that occured in embedded mode
// --------------------------------------------------------------------------
//  
void CUPnPMusicAdapter::HandleCommandComplete(
    CMPXCommand* aCommandResult, TInt /*aError*/ )
    {
     __LOG( "CUPnPMusicAdapter::HandleCommandComplete - not used!" );
     delete aCommandResult;
    // Not used
    }
 
// --------------------------------------------------------------------------
// CUPnPMusicAdapter::InitState
// initialises the state before execution
// --------------------------------------------------------------------------
// 
void CUPnPMusicAdapter::InitState()
    {
    iReturnCode = KErrNone;
    if ( iState != EStateIdle )
        {
        // dispose the wait object (just in case)
        delete iWait;
        iWait = 0;
        // dispose temporary class members
        delete iTrackArray;
        iTrackArray = 0;
        delete iMedia;
        iMedia = 0;
        delete iPlaylist;
        iPlaylist = 0;
        // reset state
        iState = EStateIdle;
        }
    if ( !iViewUtility )
        {
        TInt error = KErrNone;
        TRAP( error, iViewUtility = MMPXViewUtility::UtilityL() );
	    if( error != KErrNone )
	        {
	        __LOG( "InitState: Creating viewutility leaves" );
	        Exit( error );
	        }
	    __LOG1( "iViewUtility = %x", iViewUtility );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::TakeOwnershipOfTheFiller
// this one should be pretty self-explanatory.
// --------------------------------------------------------------------------
// 
void CUPnPMusicAdapter::TakeOwnershipOfTheFiller(
    MUPnPPlaylistFiller* aFiller )
    {
    if ( iCurrentFiller != 0 )
        {
        delete iCurrentFiller;
        iCurrentFiller = 0;
        }
    iCurrentFiller = aFiller;
    iCurrentFillerState = EFillerStateIdle;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::Complete
// Default constructor.
// --------------------------------------------------------------------------
//
void CUPnPMusicAdapter::Complete( MUPnPPlaylistFiller* aFiller )
    {
    __LOG( "CUPnPMusicAdapter::Complete" );
    if ( aFiller == iCurrentFiller )
        {
        iCurrentFillerState = EFillerStateComplete;
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::AVController
// Returns the shared AVController resource
// --------------------------------------------------------------------------
//    
MUPnPAVController& CUPnPMusicAdapter::AVController()
    {
    return iAVController;
    }

// --------------------------------------------------------------------------
// CUPnPMusicAdapter::CollectionUiHelper
// --------------------------------------------------------------------------
//
TUid CUPnPMusicAdapter::PlaylistCollectionId()
    {
    return iPlaylistCollectionId;
    }

    
// --------------------------------------------------------------------------
// CUPnPMusicAdapter::CollectionUtility
// --------------------------------------------------------------------------
//
MMPXCollectionUtility& CUPnPMusicAdapter::CollectionUtility()
    {
    return *iCollectionUtility;
    }


