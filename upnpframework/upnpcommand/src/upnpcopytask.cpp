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
* Description:  Source file for CUpnpCopyTask class.
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

// upnp framework / commonutils api
#include "upnpcommonutils.h"            // TUPnPItemType
#include "upnpmetadatafetcher.h"        // UPnPMetadataFetcher

// upnp framework / filetransferengine api
#include "upnpfiletransferengine.h"     // CUpnpFileTransferEngine

// upnp framework / command internal
#include "upnpfilepipe.h"               // CUpnpFilePipe
#include "upnpcommandparameters.h"      // CUpnpCommandParameters
#include "upnptaskhandler.h"            // MUpnpTaskHandler
#include "upnpnotehandler.h"            // CUpnpNoteHandler
#include "upnpcopytask.h"
#include "upnpcommand.h"
#include "upnptaskresourceallocator.h"  // CUpnpTaskResourceAllocator

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpCopyTask::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpTask* CUpnpCopyTask::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::NewL" );

    // Create instance
    CUpnpCopyTask* self = new (ELeave) CUpnpCopyTask();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCopyTask::CUpnpCopyTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpCopyTask::CUpnpCopyTask()
    : CUpnpFileTransferBaseTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpCopyTask::~CUpnpCopyTask()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::Destructor" );
    }

// --------------------------------------------------------------------------
// CUpnpCopyTask::ConstructL
// Perform the second phase of two phase construction. Reserves the Upnp Fw
// resources (they are released when the task is destroyed).
// --------------------------------------------------------------------------
//
void CUpnpCopyTask::ConstructL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpCopyTask::AllocateResourcesL
// Allocates the Upnp Fw resources.
// --------------------------------------------------------------------------
//
void CUpnpCopyTask::AllocateResourcesL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::AllocateResourcesL" );

    AllocateFileTransferResourcesL(
        CUpnpTaskResourceAllocator::EResourceAvController |
        CUpnpTaskResourceAllocator::EResourceSelectCopyServer );
    }

// --------------------------------------------------------------------------
// CUpnpCopyTask::ExecuteL
// Executes the task.
// --------------------------------------------------------------------------
//
void CUpnpCopyTask::ExecuteL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCopyTask::ExecuteL" );

    if( !ResourceAllocator() )
        {
        __LOG( "[UpnpCommand]\t CUpnpCopyTask::ExecuteL \
resources not allocated, leave" );
        User::Leave( KErrNotReady );
        }

    TInt transferredFiles = 0;

    TRAPD( status, ExecuteFileTransferL
        ( EFalse, transferredFiles ) );
    __LOG1( "[UpnpCommand]\t CUpnpCopyTask::ExecuteL done status %d", status );

    // inform user
    if( status == KErrNone )
        {
        HBufC* deviceString = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            ResourceAllocator()->SelectedDevice().FriendlyName() );
        CleanupStack::PushL( deviceString );
        NoteHandler()->ShowCopyInfoNoteL( 
            transferredFiles,  *deviceString );
        CleanupStack::PopAndDestroy( deviceString );
        }
    else if( status == KErrDisconnected || status == KErrSessionClosed 
            || status == KErrCouldNotConnect )
        {
        // check if wlan is active
        // note shown only in device disappeared cases
        if( IsWlanActive() )
            {
            NoteHandler()->ShowCopyDisconnectionErrorNoteL();
            }
        }                 
    else if( status != KErrCancel )
        {
        // some other generic error EXCEPT cancel
        NoteHandler()->ShowCopyErrorNoteL();
        }

    // If there was an error, leave
    if( status != KErrNone )
        {
        User::Leave( status );
        }
    }


// End of File
