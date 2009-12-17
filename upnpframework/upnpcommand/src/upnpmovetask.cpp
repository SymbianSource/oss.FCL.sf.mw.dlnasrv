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
* Description:  Source file for CUpnpMoveTask class.
*
*/


// INCLUDE FILES
// System
#include <utf.h>                        // ConvertToUnicodeFromUtf8L

// upnp stack api
#include <upnpitem.h>                   // CUpnpItem
#include <upnpobject.h>                 // CUpnpObject (cast)

// upnp framework / avcontroller api
#include "upnpavcontroller.h"           // MUPnPAVController
#include "upnpavcontrollerfactory.h"    // UPnPAVControllerFactory
#include "upnpavbrowsingsession.h"      // MUPnPAVBrowsingSession
#include "upnpavsessionbase.h"          // ReserveLocalMSServicesCompleted()
#include "upnpavdevice.h"               // CUpnpAVDevice

// upnp framework / internal api's
#include "upnpcommonutils.h"            // TUPnPItemType
#include "upnpmetadatafetcher.h"        // UPnPMetadataFetcher
#include "upnpfiletransferengine.h"     // CUpnpFileTransferEngine

// upnp framework / command internal
#include "upnpfilepipe.h"               // CUpnpFilePipe
#include "upnpcommandparameters.h"      // CUpnpCommandParameters
#include "upnptaskhandler.h"            // MUpnpTaskHandler
#include "upnpnotehandler.h"            // CUpnpNoteHandler
#include "upnpmovetask.h"
#include "upnpcommand.h"
#include "upnptaskresourceallocator.h"  // CUpnpTaskResourceAllocator

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpMoveTask::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpTask* CUpnpMoveTask::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::NewL" );

    // Create instance
    CUpnpMoveTask* self = new (ELeave) CUpnpMoveTask();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpMoveTask::CUpnpMoveTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpMoveTask::CUpnpMoveTask()
    : CUpnpFileTransferBaseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpMoveTask::~CUpnpMoveTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::Destructor" );
    }

// --------------------------------------------------------------------------
// CUpnpMoveTask::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpMoveTask::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpMoveTask::AllocateResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpMoveTask::AllocateResourcesL()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::AllocateResourcesL" );

    AllocateFileTransferResourcesL(
        CUpnpTaskResourceAllocator::EResourceAvController |
        CUpnpTaskResourceAllocator::EResourceSelectMoveServer );
    }

// --------------------------------------------------------------------------
// CUpnpMoveTask::ExecuteL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpMoveTask::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpMoveTask::ExecuteL" );

    if( !ResourceAllocator() )
        {
        __LOG( "[UpnpCommand]\t CUpnpCopyTask::ExecuteL \
resources not allocated, leave" );
        User::Leave( KErrNotReady );
        }

    TInt transferredFiles = 0;
        
    TRAPD( status, ExecuteFileTransferL
        ( ETrue, transferredFiles ) );
    __LOG1( "[UpnpCommand]\t CUpnpMoveTask::ExecuteL done status %d", status );

    // inform user
    if( status == KErrNone )
        {
        HBufC* deviceString = CnvUtfConverter::ConvertToUnicodeFromUtf8L( 
            ResourceAllocator()->SelectedDevice().FriendlyName() );
        CleanupStack::PushL( deviceString );
        NoteHandler()->ShowMoveInfoNoteL( 
            transferredFiles,  *deviceString );
        CleanupStack::PopAndDestroy( deviceString );
        }
    else if( status == KErrNotSupported )
        {
        NoteHandler()->ShowDrmNoteL();
        }
    else if( status == KErrDisconnected || status == KErrSessionClosed )
        {
        // check from if wlan is active
        // note shown only in device disappeared cases
        if( IsWlanActive() )
            {
            NoteHandler()->ShowMoveDisconnectionErrorNoteL();
            }
        }                   
    else if( status != KErrCancel )
        {
        // some other generic error EXCEPT cancel
        NoteHandler()->ShowMoveErrorNoteL();
        }

    // If there was an error, leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }


// End of File
