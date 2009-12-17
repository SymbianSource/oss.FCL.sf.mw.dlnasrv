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
* Description:  A class that allocates resources for a task
*
*/


// INCLUDE FILES
// upnpframework / avcontroller api
#include "upnpavcontrollerfactory.h"    // UPnPAVControllerFactory
#include "upnpavcontroller.h"           // MUPnPAVController
#include "upnpavdevice.h"               // CUpnpAVDevice
#include "upnpavbrowsingsession.h"      // MUPnPAVBrowsingSession

// upnpframework / common ui
#include "upnpcommonui.h"               // CUPnPCommonUI

// command internal
#include "upnpnotehandler.h"            // CUpnpNoteHandler
#include "upnptaskresourceallocator.h"  // myself

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"


// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::NewL
// --------------------------------------------------------------------------
CUpnpTaskResourceAllocator* CUpnpTaskResourceAllocator::NewL(
    CUPnPCommonUI& aCommonUI,
    TInt aMode )
    {
    CUpnpTaskResourceAllocator* self = new (ELeave)
        CUpnpTaskResourceAllocator( aCommonUI, aMode );

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::CUpnpTaskResourceAllocator
// --------------------------------------------------------------------------
CUpnpTaskResourceAllocator::CUpnpTaskResourceAllocator(
    CUPnPCommonUI& aCommonUI,
    TInt aMode )
    : CAsyncOneShot( EPriorityStandard )
    , iCommonUI( aCommonUI )
    {
    __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator: constructor" );

    iState = EStateIdle;
    iErrorCode = KErrNone;
    iMode = aMode;
    iLocalMSSCompleted = EFalse;

    if ( iMode & EResourceLocalMediaServer ||
         iMode & EResourceSelectDevice )
        {
        // if any of these flags are set, AVCONTROLLER flag is mandatory.
        // assert that.
        __ASSERTD( iMode & EResourceAvController, __FILE__, __LINE__ );
        }

    if ( iMode & EResourceLocalMediaServer )
        {
        // if this flag is set, one of the device selection flags is mandatory.
        __ASSERTD( iMode & EResourceSelectDevice, __FILE__, __LINE__ );
        }

    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::~CUpnpTaskResourceAllocator
// --------------------------------------------------------------------------
CUpnpTaskResourceAllocator::~CUpnpTaskResourceAllocator()
    {
    if ( iState == EStateAllocating )
        {
        __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
        
        SetErrorCode( KErrCancel );
        if ( iNoteHandler )
            {
            iNoteHandler->CloseWaitNote();
            }
        }

    Cleanup();
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::Cleanup
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::Cleanup()
    {
    __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::Cleanup" );

    delete iSelectedDevice;
    iSelectedDevice = 0;
    
    if ( iMediaServerSession )
        {
        // Stop local file sharing (release)
        TRAPD( error, iMediaServerSession->ReleaseLocalMSServicesL() );
        if( error != KErrNone )
            {
            __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::\
ReleaseLocalMSService failed %d", error );
            }
        
        // Stop observing the rendering session
        iMediaServerSession->RemoveObserver();

        // Stop session
        iAVController->StopBrowsingSession( *iMediaServerSession );
        iMediaServerSession = NULL;
        }

    if( iAVController )
        {
        iAVController->RemoveDeviceObserver();
        iAVController->Release(); // Fixes ESLX-7BMJBN
        iAVController = NULL;
        }

    }




// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::AllocateL
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::AllocateL()
    {
    __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::AllocateL" );

    __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
    
    // make CAsyncOneShot to do its trick in parallel
    Call();

    // show progress note in sync.
    iState = EStateAllocating;
    TInt status = iNoteHandler->RunConnectingWaitNote();
    SetErrorCode( status );
    iState = EStateReady;

    // progress note has exited. Monitor errors from the progress note

    // in error situation clean up resources and leave
    if ( iErrorCode != KErrNone )
        {
        iState = EStateError;
        Cleanup();
        User::Leave( iErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::AVController
// --------------------------------------------------------------------------
MUPnPAVController& CUpnpTaskResourceAllocator::AVController()
    {
    __ASSERT( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERT( iMode & EResourceAvController, __FILE__, __LINE__ );

    return *iAVController;
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::SelectedDevice
// --------------------------------------------------------------------------
const CUpnpAVDevice& CUpnpTaskResourceAllocator::SelectedDevice()
    {
    __ASSERT( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERT( iMode & EResourceSelectDevice, __FILE__, __LINE__ );
    __ASSERT( iSelectedDevice, __FILE__, __LINE__ );

    return *iSelectedDevice;
    }


// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::RunL
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::RunL()
    {
    __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL" );

    // make sure we are still on the go
    if ( iState == EStateAllocating )
        {
        // starting of AVController
        StartAvControllerL();
        }
    else
        {
        __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL \
    StartAvControllerL not done in state %d", iState );
        }
    // make sure we are still on the go
    if ( iState == EStateAllocating )
        {
        // select device
        SelectDeviceL();
        }
    else
        {
        __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL \
    SelectDeviceL not done in state %d", iState );
        }
    // make sure we are still on the go
    if ( iState == EStateAllocating )
        {
        // start the local mediaserver
        StartLocalMediaServerL();
        }
    else
        {
        __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL \
    StartLocalMediaServerL not done in state %d", iState );
        }        
    // make sure we are still on the go
    if ( iState == EStateAllocating )
        {
        // if local media server was started but not yet completed,
        // wait for callback
        if ( iMode & EResourceLocalMediaServer
            && !iLocalMSSCompleted )
            {
            __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL \
    waiting for ReserveLocalMSServicesCompleted" );
            iState = EStateWaitingForLMS;
            }
        // otherwise we are done now
        else
            {
            // close the wait note
            __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
            iNoteHandler->CloseWaitNote();
            }
        }
    else
        {
        __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunL \
    CloseWaitNote not done in state %d", iState );
        }
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::RunError
// --------------------------------------------------------------------------
TInt CUpnpTaskResourceAllocator::RunError( TInt aError )
    {
    __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::RunError: %d",
        aError );

    __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
    
    // asynchronous operation leaves. store the error code and make the
    // wait note exit.
    SetErrorCode( aError );
    iNoteHandler->CloseWaitNote();
        
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::StartAvControllerL
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::StartAvControllerL()
    {
    if ( iMode & EResourceAvController )
        {
        __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::\
StartAvControllerL" );

        // Create new UpnpAvController client instance
        iAVController = UPnPAVControllerFactory::NewUPnPAVControllerL();

        // Register as an observer (for WLAN lost notification) 
        iAVController->SetDeviceObserver( *this );
        }
    else
        {
        __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::\
    AvController not started" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::StartLocalMediaServerL
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::StartLocalMediaServerL()
    {
    if ( iMode & EResourceLocalMediaServer )
        {
        __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::\
StartLocalMediaServerL" );

        // create a dummy device
        CUpnpAVDevice* dummyDevice = CUpnpAVDevice::NewLC();
        dummyDevice->SetUuidL( KNullDesC8 );
        dummyDevice->SetDeviceType(CUpnpAVDevice::EMediaServer);    
        // create a session for mediaserver resources keepalive
        iMediaServerSession =
            &iAVController->StartBrowsingSessionL( *dummyDevice );
        iMediaServerSession->SetObserver( *this );
        CleanupStack::PopAndDestroy( dummyDevice );
        // now reserve
        iMediaServerSession->ReserveLocalMSServicesL();
        }
    }


// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::SelectDeviceL
// --------------------------------------------------------------------------
//
void CUpnpTaskResourceAllocator::SelectDeviceL()
    {
    if ( iMode & EResourceSelectDevice )
        {
        __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::SelectDeviceL" );

        TInt status = KErrNone;
        
        // decide device filters and popup title
        TUPnPDeviceTypesToSearch deviceFilter =
            EUPnPSearchAllDevices;
        TUPnPDialogTitle popupTitle = EUPnPSelectDeviceTitle;

        if ( iMode & EResourceSelectImageRenderer )
            {
            deviceFilter = EUPnPSearchRenderingDevicesWithImageCapability;
            }
        else if ( iMode & EResourceSelectCopyServer )
            {
            deviceFilter = EUPnPSearchServerDevicesWithCopyCapability;
            popupTitle = EUPnPCopyToTitle;
            }
        else if ( iMode & EResourceSelectMoveServer )
            {
            deviceFilter = EUPnPSearchServerDevicesWithCopyCapability;
            popupTitle = EUPnPMoveToTitle;
            }
        
        // Launch the device selection dialog
        CUpnpAVDevice* tempDevice = CUpnpAVDevice::NewL();
        CleanupStack::PushL( tempDevice );
        status = iCommonUI.SelectDeviceL(
                  *iAVController,
                  *tempDevice,
                  deviceFilter,
                  popupTitle );

        // Fix UpnpCommonUi return value
        if( status > 0 )
            {
            status = KErrNone;
            }

        if( status != KErrNone )
            {
            // failed
            User::Leave( status );
            }

        CleanupStack::Pop( tempDevice );
        iSelectedDevice = tempDevice;

        }
    }

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::SetErrorCode
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::SetErrorCode( TInt aErrorCode )
    {
    __LOG2( "[UpnpCommand]\t CUpnpTaskResourceAllocator::SetErrorCode \
%d -> %d", iErrorCode, aErrorCode );

    if ( iErrorCode == KErrNone )
        {
        __LOG( "error code changed");
        iErrorCode = aErrorCode;
        } 
    // if user cancelled operation, we don't need other error codes
    // so it's ok to reset old error code
    else if ( aErrorCode == KErrCancel )
        {
        __LOG( "error code reseted");
        iErrorCode = aErrorCode;
        }
    }

// ==========================================================================
// Methods for AVController device observer
// ==========================================================================
    
    
// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::WLANConnectionLost
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::WLANConnectionLost()
    {
    __LOG( "[UpnpCommand]\t CUpnpTaskResourceAllocator::WLANConnectionLost" );

    if ( iState == EStateAllocating || iState == EStateWaitingForLMS )
        {
        __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
        SetErrorCode( KErrDisconnected );
        iNoteHandler->CloseWaitNote();
        iState = EStateError;
        }
    }

void CUpnpTaskResourceAllocator::UPnPDeviceDiscovered(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    }

void CUpnpTaskResourceAllocator::UPnPDeviceDisappeared(
    const CUpnpAVDevice& aDevice )
    {
    __ASSERTD( iNoteHandler, __FILE__, __LINE__ );
    if ( ( iState == EStateAllocating || iState == EStateWaitingForLMS )
         && iSelectedDevice != 0 )
        {
        if ( aDevice.Uuid() == iSelectedDevice->Uuid() )
            {
            __LOG( "[UpnpCommand]\t selected device lost !" );
            SetErrorCode( KErrDisconnected );
            iNoteHandler->CloseWaitNote();
            iState = EStateError;
            }
        }
    }
    
    
// ==========================================================================
// Methods for AVController browsing session observer
// ==========================================================================

// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::ReserveLocalMSServicesCompleted
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::ReserveLocalMSServicesCompleted( TInt aError )
    {
    __LOG1( "[UpnpCommand]\t CUpnpTaskResourceAllocator::\
ReserveLocalMSServicesCompleted: %d", aError );

    if ( iState == EStateAllocating 
        || iState == EStateWaitingForLMS )
        {
        iLocalMSSCompleted = ETrue;
        SetErrorCode( aError );
        if ( aError == KErrNone )
            {
            iLocalMSStarted = ETrue;
            iMediaServerSession->RemoveObserver();
            }
        // allocation done, just waiting for this callback
        // close note and we are done
        if( iState == EStateWaitingForLMS )
            {
            iNoteHandler->CloseWaitNote();
            }
        }
    }


void CUpnpTaskResourceAllocator::BrowseResponse(
    const TDesC8& /*aBrowseResponse*/,
    TInt /*aError*/,
    TInt /*aMatches*/,
    TInt /*aTotalCount*/,
    const TDesC8& /*aUpdateId*/ )
    {
    }

void CUpnpTaskResourceAllocator::SearchResponse( 
    const TDesC8& /*aSearchResponse*/,
    TInt /*aError*/,
    TInt /*aMatches*/,
    TInt /*aTotalCount*/,
    const TDesC8& /*aUpdateId*/ )
    {
    }

void CUpnpTaskResourceAllocator::SearchCapabilitiesResponse( 
    TInt /*aError*/,
    const TDesC8& /*aSearchCapabilities*/ )
    {
    }

void CUpnpTaskResourceAllocator::CreateContainerResponse(
    TInt /*aError*/, 
    const TDesC8& /*aObjectId*/ )
    {
    }

void CUpnpTaskResourceAllocator::DeleteObjectResponse(
    TInt /*aError*/ )
    {
    }
 
void CUpnpTaskResourceAllocator::MediaServerDisappeared(
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    }



// --------------------------------------------------------------------------
// CUpnpTaskResourceAllocator::SetNoteHandlerL
// Sets the pointer to the note handler.
// --------------------------------------------------------------------------
void CUpnpTaskResourceAllocator::SetNoteHandlerL( 
                                        CUpnpNoteHandler* aNoteHandler )
    {
    // Check parameter
    if( !aNoteHandler )
        {
        User::Leave( KErrArgument );
        }

    iNoteHandler = aNoteHandler;
    }
