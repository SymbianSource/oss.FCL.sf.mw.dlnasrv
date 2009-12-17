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
* Description:  Source file for CUpnpShowTask class.
*
*/


// INCLUDE FILES
// upnp stack api
#include <upnpitem.h>                   // CUpnpItem
#include <upnpobject.h>                 // CUpnpObject (cast)

// upnpframework / avcontroller api
#include "upnpavcontroller.h"           // MUPnPAVController
#include "upnpavrenderingsession.h"     // MUPnPAVRenderingSession
#include "upnpavsessionobserverbase.h"

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h"           // KFilterCommon, KClassVideo
#include "upnpitemresolver.h"           // MUPnPItemResolver
#include "upnpitemresolverobserver.h"   // MUPnPItemResolverObserver
#include "upnpitemresolverfactory.h"    // UPnPItemResolverFactory
#include "upnpitemutility.h"            // UPnPItemUtility::BelongsToClass

// upnpframework / commonui
#include "upnpcommonui.h"

// command internal
#include "upnpfilepipe.h"               // CUpnpFilePipe
#include "upnptaskhandler.h"            // MUpnpTaskHandler
#include "upnptaskresourceallocator.h"  // CUpnpTaskResourceAllocator
#include "upnpimagerenderingengine.h"   // CUpnpImageRenderingEngine
#include "upnpshowtask.h"
#include "upnpcommand.h"
#include "upnpnotehandler.h"            // CUpnpNoteHandler

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"


// --------------------------------------------------------------------------
// CUpnpShowTask::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpTask* CUpnpShowTask::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::NewL" );

    // Create instance
    CUpnpShowTask* self = new (ELeave) CUpnpShowTask();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::CUpnpShowTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpShowTask::CUpnpShowTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::Constructor" );

    // Initialise member variables
    iCommonUI = NULL;
    iPlayingVideo = EFalse;
    iRenderingSession = NULL;
    iVideoRenderingSession = NULL;
    iResourceAllocator = NULL;
    iRenderingEngine = NULL;
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpShowTask::~CUpnpShowTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::Destructor" );

    Cleanup();
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::Cleanup
// --------------------------------------------------------------------------
//
void CUpnpShowTask::Cleanup()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::Cleanup" );

    if ( iRenderingEngine )
        {
        delete iRenderingEngine;
        iRenderingEngine = NULL;
        }       
        
    // delete the resource allocator
    // Local mediaserver and AVController resources will be freed.
    delete iResourceAllocator;
    iResourceAllocator = NULL;

    iRenderingSession = NULL;
    iVideoRenderingSession = NULL;

    delete iCommonUI;
    iCommonUI = NULL;

    __LOG( "[UpnpCommand]\t CUpnpShowTask::Cleanup - end" );
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpShowTask::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::ConstructL" );

    // create common UI
    iCommonUI = CUPnPCommonUI::NewL();

    // create the resource allocator
    iResourceAllocator = CUpnpTaskResourceAllocator::NewL(
        *iCommonUI,
        CUpnpTaskResourceAllocator::EResourceAvController |
        CUpnpTaskResourceAllocator::EResourceLocalMediaServer |
        CUpnpTaskResourceAllocator::EResourceSelectImageRenderer );
        
    iShowPlaybackFailedNote = ETrue;
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::AllocateResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpShowTask::AllocateResourcesL()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::AllocateResourcesL" );

    iResourceAllocator->SetNoteHandlerL( NoteHandler() );
    
    NoteHandler()->ResetDrmNoteCount();
    
    // now allocate!
    iResourceAllocator->AllocateL();

    // start a rendering session
    iRenderingSession =
        &iResourceAllocator->AVController().StartRenderingSessionL(
            iResourceAllocator->SelectedDevice() );

    // create image rendering engine
    iRenderingEngine = CUpnpImageRenderingEngine::NewL(
        iResourceAllocator->AVController(),
        *iRenderingSession,
        *this );
    }



// --------------------------------------------------------------------------
// CUpnpShowTask::ExecuteL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpShowTask::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::ExecuteL" );
    // assert that required resources exist
    __ASSERTD( iRenderingEngine, __FILE__, __LINE__ );


    if( iPlayingVideo )
        {
        // if videoplayerdlg is active, we cannot start rendering new file
        // before user closes the dialog
        __LOG( "[UpnpCommand]\t CUpnpShowTask::ExecuteL\
            video already playing, do nothing" );
        }
    else
        {
        // start rendering process on the engine
        iRenderingEngine->PlayL();
        }
    }


// --------------------------------------------------------------------------
// CUpnpShowTask::GetMedia
// provide media to be played back
// --------------------------------------------------------------------------
MUPnPItemResolver* CUpnpShowTask::GetMedia()
    {
    __ASSERTD( FilePipe(), __FILE__, __LINE__ );

    MUPnPItemResolver* resolver = 0;

    if ( FilePipe()->Count() > 0 )
        {
        const TDesC& filename = FilePipe()->FileAt( 0 );
        __LOG1( "[UpnpCommand]\t CUpnpShowTask::GetMedia: %S", &filename );
        TRAP_IGNORE (
            resolver =
                UPnPItemResolverFactory::NewLocalItemResolverL(
                    filename,
                    iResourceAllocator->AVController(),
                    iSelector,
                    UPnPItemResolverFactory::EOmitLocalMSStart );
            );
        }

    return resolver;
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::RenderAck
// Callback from image rendering engine.
// --------------------------------------------------------------------------
TInt CUpnpShowTask::RenderAck(
    TInt aError,
    const CUpnpItem* aItem )
    {
    __LOG1( "[UpnpCommand]\t CUpnpShowTask::RenderAck aError %D", aError );
    if ( aError == KErrNotSupported && aItem &&
        UPnPItemUtility::BelongsToClass( *aItem, KClassVideo ) )
        {
        // NOTE!
        // This is a special case. For video playback we use the image
        // rendering engine for resolving, and then it fails with
        // KErrNotSupported, because it is not an image. Now we catch the
        // error here and play the resolved item. Why that complicated ?
        // - to keep image rendering engine for image rendering ONLY
        //   and not to mix video UI stuff in there
        // - to enable image rendering engine reuse in the future
        // - so that we do not have to rewrite the resolving code in
        //   another place, it already exists in image rendering engine.
        aError = PlayVideo( *aItem );
        }

    if ( aError != KErrDisconnected )
        {
        // disconnect message is handled in EngineShutdown
        // other errors are handled here
        if ( aError == KErrNotSupported || aError == KErrPermissionDenied )
            {
            TRAP_IGNORE( NoteHandler()->ShowDrmNoteL() );
            }
        else if( aError != KErrNone && iShowPlaybackFailedNote )
            {
            // note is shown only once per session
            TRAP_IGNORE( NoteHandler()->ShowPlaybackFailedNoteL() );
            iShowPlaybackFailedNote = EFalse;
            }
            
        // inform observer
        CommandEvent( UpnpCommand::EEventComplete, aError );
        }
        __LOG1( "[UpnpCommand]\t CUpnpShowTask::RenderAck end, resp=%d", aError );
    return aError;
    }

// --------------------------------------------------------------------------
// CUpnpShowTask::EngineShutdown
// Callback from image rendering engine.
// --------------------------------------------------------------------------
void CUpnpShowTask::EngineShutdown(
    TInt aError )
    {
    __ASSERTD( iRenderingEngine, __FILE__, __LINE__ );
    
    if ( iPlayingVideo )
        {
        __LOG1( "[UpnpCommand]\t CUpnpShowTask::EngineShutdown(%d)\
while video playing",
            aError );
        TRAP_IGNORE( iCommonUI->DismissDialogL( aError ) );
        }
    else
        {
        __LOG1( "[UpnpCommand]\t CUpnpShowTask::EngineShutdown(%d)",
            aError );
            
        if( aError == KErrDisconnected )
            {
            // check from rendering engine if wlan is active
            // note shown only in device disappeared cases
            if( iRenderingEngine->IsWlanActive() )
                {
                TRAP_IGNORE( NoteHandler()->ShowConnectionLostNoteL() );
                }
            }
            
            
        // Inform the observer
        __ASSERTD( TaskHandler(), __FILE__, __LINE__ );

        CommandEvent( UpnpCommand::EEventComplete, aError);
        }
    __LOG( "[UpnpCommand]\t CUpnpShowTask::EngineShutdown END" );
    }


    
// --------------------------------------------------------------------------
// CUpnpShowTask::PlayVideo
// Launches the video player dialog for playing a video file on remote
// --------------------------------------------------------------------------
TInt CUpnpShowTask::PlayVideo( const CUpnpItem& aItem )
    {
    __LOG( "[UpnpCommand]\t CUpnpShowTask::CUpnpShowTask::PlayVideo start" );
    
    TInt videoPlayerError = KErrNone;
    TInt videoStatus = KErrNone;

    // Update the state
    iPlayingVideo = ETrue;
            
            
    // Launch the video player dialog to render the item.
    TRAP( videoPlayerError,
    
        // start a rendering session ( used only for video playing )
        if( !iVideoRenderingSession )
            {
            iVideoRenderingSession =
                &iResourceAllocator->AVController().StartRenderingSessionL(
                    iResourceAllocator->SelectedDevice() );
            }
        
        // inform observer that we are launching dialog
        __LOG( "[UpnpCommand]\t CUpnpShowTask::CUpnpShowTask::PlayVideo \
launching videoplayerdialog" );
        CommandEvent( UpnpCommand::EEventProgress,
            UpnpCommand::KUpnpCommandStatusStartPlayVideo, EFalse );

        // plays video     
        videoStatus = iCommonUI->ExecuteVideoPlayerL(
             *iVideoRenderingSession, aItem );
        )

    // Update the state
    iPlayingVideo = EFalse;

    // Fix UpnpCommonUi's return value
    if( videoStatus > 0 )
        {
        videoStatus = KErrNone;
        }

    if( videoPlayerError != KErrNone &&
        videoStatus == KErrNone )
        {
        videoStatus = videoPlayerError;
        }

    __LOG( "[UpnpCommand]\t CUpnpShowTask::CUpnpShowTask::PlayVideo end" );

    // Inform the observer, no matter if the playing succeeded or failed.
    return videoStatus;
    }
    

// End of File
