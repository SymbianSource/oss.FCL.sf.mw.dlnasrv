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
* Description:  Source file for CUpnpFileTransferBaseTask class.
*
*/


// INCLUDE FILES
// system
#include <utf.h>                        // ConvertToUnicodeFromUtf8L

// upnp stack api
#include <upnpitem.h>                   // CUpnpItem
#include <upnpobject.h>                 // CUpnpObject (cast)

// upnp framework / avcontroller api
#include "upnpavcontrollerfactory.h"    // UPnPAVControllerFactory
#include "upnpavcontroller.h"           // MUPnPAVController
#include "upnpavbrowsingsession.h"      // MUPnPAVBrowsingSession
#include "upnpavsessionobserverbase.h"  // MUPnPAVSessionObserverBase
#include "upnpavdevice.h"               // CUpnpAVDevice

// upnp framework / common utils api
#include "upnpcommonutils.h"            // TUPnPItemType
#include "upnpmetadatafetcher.h"        // UPnPMetadataFetcher

// upnp framework / fte
#include "upnpfiletransferengine.h"     // CUpnpFileTransferEngine
// upnp framework / common ui
#include "upnpcommonui.h"               // CUpnpCommonUI

// upnpcommand internal
#include "upnpfilepipe.h"               // CUpnpFilePipe
#include "upnpcommandparameters.h"      // CUpnpCommandParameters
#include "upnptaskhandler.h"            // MUpnpTaskHandler
#include "upnpnotehandler.h"            // CUpnpNoteHandler
#include "upnpcommand.h"                // CUpnpCommand
#include "upnptaskresourceallocator.h"  // CUpnpTaskResourceAllocator
#include "upnpfiletransferbasetask.h"   // CUpnpFileTransferBaseTask


_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"


// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::CUpnpFileTransferBaseTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpFileTransferBaseTask::CUpnpFileTransferBaseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpFileTransferBaseTask::~CUpnpFileTransferBaseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::Destructor" );

    // delete the resource allocator
    // Local mediaserver and AVController resources will be freed.
    delete iResourceAllocator;
    delete iCommonUI;
    }


// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::AllocateFileTransferResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferBaseTask::AllocateFileTransferResourcesL( TInt aMode)
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::AllocateResourcesL" );
    
    // create common UI
    iCommonUI = CUPnPCommonUI::NewL();
    
    // create the resource allocator
    iResourceAllocator = CUpnpTaskResourceAllocator::NewL(
        *iCommonUI, aMode);
        
    iState = EStateConnected; 
    
    iResourceAllocator->SetNoteHandlerL( NoteHandler() );
    
    // now allocate!
    iResourceAllocator->AllocateL();

     // start a browsing session
    iBrowsingSession =
        &iResourceAllocator->AVController().StartBrowsingSessionL(
            iResourceAllocator->SelectedDevice() );
            
    iBrowsingSession->SetObserver( *this );
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::ResourceAllocator
// Executes the task.
// --------------------------------------------------------------------------
//
CUpnpTaskResourceAllocator* CUpnpFileTransferBaseTask::ResourceAllocator()
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::ResourceAllocator" );
 
    return iResourceAllocator;
    }

    
// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::ExecuteFileTransferL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferBaseTask::ExecuteFileTransferL( TBool aRemoveFiles, 
    TInt& aTransferredFiles)
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::ExecuteL" );


    // Leave if command has not been allocated yet
    if( !FilePipe() ||
        !CommandParameters() ||
        !NoteHandler() ||
        !iBrowsingSession ||
        !iResourceAllocator )
        {
        __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::\
ExecuteFileTransferL resources not allocated, leave" );
        User::Leave( KErrNotReady );
        }

    // reset drm-note calculator ( note is shown once per operation )
    NoteHandler()->ResetDrmNoteCount();
    
    TInt status = KErrNone;
    TInt origFileCount = FilePipe()->Count();;    
    TInt failedFileCount = 0;   

    // Initialise and execute the UpnpFileTransferEngine
    CUpnpFileTransferEngine* ftEngine = NULL;
    ftEngine = CUpnpFileTransferEngine::NewL( iBrowsingSession );
    if( status == KErrNone &&
        ftEngine )
        {
        CleanupStack::PushL( ftEngine );
        
        // move operation
        if( aRemoveFiles )
            {
            __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask, move files" );
            ftEngine->MoveLocalFilesToRemoteServerL(
                 &FilePipe()->AsArray() );
            }
        // copy operation
        else
            {  
            // Read the collection name (if any)
            const TDesC& collectionName =
                CommandParameters()->Get( UpnpCommand::EParamCollectionName );

            // If collection name is not set, the operation is file copy
            if( collectionName != KNullDesC )
                {
                __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask,\
 copy playlist" );
                ftEngine->CopyLocalPlaylistToRemoteServerL(
                              collectionName, &FilePipe()->AsArray() );
                }
            // If collection name is set, the operation is playlist copy
            else
                {
                __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask,\
 copy files" );
                ftEngine->CopyLocalFilesToRemoteServerL(
                              &FilePipe()->AsArray() );
                }
            }

        // Clean up
        CleanupStack::PopAndDestroy( ftEngine );
        ftEngine = NULL;
        }

    // calculate number of transferred files based on original file count and
    // items remaining in filepipe (those were NOT transferred)
    failedFileCount = FilePipe()->Count();
    aTransferredFiles = origFileCount - failedFileCount;

    // check if connection was lost during transfer
    if( iState != EStateConnected )
        {
        status = KErrDisconnected;
        }

    __LOG1( "[UpnpCommand]\t CUpnpFileTransferBaseTask::\
ExecuteL done status %d", status );
    
    // If there was an error, leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::MediaServerDisappeared
// Notifies that the Media Renderer we have a session with has disappeared.
// --------------------------------------------------------------------------
//
void CUpnpFileTransferBaseTask::MediaServerDisappeared( 
    TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG1( "[UpnpCommand]\t CUpnpFileTransferBaseTask::\
MediaServerDisappeared aReason %d", aReason );

    // need to know if wlan or server was lost
    if( aReason == MUPnPAVSessionObserverBase::EWLANLost )
        {
        iState = EStateWLANLost;
        }
    else
        {
        iState = EStateMSLost;
        }
    
    }

// --------------------------------------------------------------------------
// CUpnpFileTransferBaseTask::IsWlanActive
// Returns connection state
// --------------------------------------------------------------------------
//
TBool CUpnpFileTransferBaseTask::IsWlanActive()
    {
    __LOG( "[UpnpCommand]\t CUpnpFileTransferBaseTask::IsWlanActive" );

    return (iState != EStateWLANLost);
    }

// End of File
