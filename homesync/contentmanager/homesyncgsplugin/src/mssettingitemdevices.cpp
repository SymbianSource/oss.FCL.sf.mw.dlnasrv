/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMSSettingItemDevices class implementation
*
*/


#include <msgspluginsrc.rsg>
#include <StringLoader.h>
#include "upnpavdevice.h"
#include "upnpavcontroller.h" // for MUPnPAVController
#include "upnpavcontrollerfactory.h" // for UPnPAVControllerFactory
#include <AknWaitDialog.h>
#include <utf.h> // for CnvUtfConverter

#include "cmmediaserverfull.h"
#include "cmcommonutils.h"
#include <aknnotewrappers.h>
#include "mssettingitemdevices.h"
#include "msmultiselectionpopup.h"
#include "msengine.h"
#include "upnpavdevicelist.h"
#include "msdebug.h"

// CONSTANTS

// reserved list size
const TInt KItemArrayGranularity = 3;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CMSSettingItemDevices::NewL
// --------------------------------------------------------------------------
//
CMSSettingItemDevices* CMSSettingItemDevices::NewL(
                        TInt aIdentifier,
                        TDes& aText,
                        CMSEngine& aMSEngine,
                        RPointerArray<CCmMediaServerFull>& aStoredServers,
                        TBool aUploadCapabilitySupport )
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::NewL"));

    CMSSettingItemDevices* self = CMSSettingItemDevices::NewLC(
                                          aIdentifier,
                                          aText,
                                          aMSEngine,
                                          aStoredServers,
                                          aUploadCapabilitySupport );

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::NewLC
// --------------------------------------------------------------------------
//
CMSSettingItemDevices* CMSSettingItemDevices::NewLC(
                        TInt aIdentifier,
                        TDes& aText,
                        CMSEngine& aMSEngine,
                        RPointerArray<CCmMediaServerFull>& aStoredServers,
                        TBool aUploadCapabilitySupport )
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::NewLC"));
	
    CMSSettingItemDevices* self = new ( ELeave ) CMSSettingItemDevices(
                                          aIdentifier,
                                          aText,
                                          aMSEngine,
                                          aStoredServers,
                                          aUploadCapabilitySupport );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
	
// --------------------------------------------------------------------------
// CMSSettingItemDevices::ConstructL
// --------------------------------------------------------------------------
//
void CMSSettingItemDevices::ConstructL()
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::ConstructL begins"));

    iSettingText = HBufC::NewL( KMaxFileName );

    iSelectedServers =
        new (ELeave) CArrayFixFlat<TInt>( KItemArrayGranularity );
    
    SetSettingItemTextL();

    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::ConstructL ends"));
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::~~CMSSettingItemDevices()
// --------------------------------------------------------------------------
//
CMSSettingItemDevices::~CMSSettingItemDevices()
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::~CMSSettingItemDevices"));

    delete iSettingText;

    delete iSelectedServers;

    iServerList.Reset();
    iServerList.Close();
    
    if ( iAvController )
        {
        iAvController->RemoveDeviceObserver();
		iAvController->Release();
		iAvController = NULL;
        }

    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::EditItemL( TBool aCalledFromMenu )
// --------------------------------------------------------------------------
//
void CMSSettingItemDevices::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    LOG( _L("[MSGSPlugin]\t CMSSettingItemDevices::EditItemL") );

    // restore original state
    SetAcceptState( EFalse );

    // Create list for selection
    ReadServerListL();

    // Create Avcontroller and fetch already discovered devices from it.
    CreateAvcAndFetchDevicesL();           
            
    if( !iUserCancelledSearch )
        {
        if ( iUploadCapabilitySupport )
            {
            iDevSelectionDlg = CMSMultiselectionPopup::NewL(
                                    iUploadCapabilitySupport,
                                    &iServerList,
                                    iSelectedServers );
            iDevSelectionDlg->PrepareLC(
                            R_MS_TARGET_DEVICE_SELECTION_DIALOG );
            }
        else
            {
            iDevSelectionDlg = CMSMultiselectionPopup::NewL(
                                    iUploadCapabilitySupport,
                                    &iServerList,
                                    iSelectedServers );           
            iDevSelectionDlg->PrepareLC(
                            R_MS_SOURCE_DEVICE_SELECTION_DIALOG );
            }            

        #ifndef __WINS__
            iDevSelectionDlg->QueryHeading()->SetHeaderAnimationL(
                R_MSGS_ANIMATION_FOR_SELECTION_DIALOG );
        #endif

        TInt returnValue = iDevSelectionDlg->RunLD();            

        iDevSelectionDlg = NULL;
        
        CancelDeviceSearch();

        if( returnValue )
            {
            // User presses OK
            SetAcceptState( ETrue );

            SetServersActivity();
            SetSettingItemTextL();

            iMSEngine.SetMediaServersL( iStoredServers );

            LoadL();
            UpdateListBoxTextL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::SetServersActivity
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::SetServersActivity()
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::SetServersActivity \
    begins"));

    // Set all servers inactive
    for( TInt ind=0; ind < iStoredServers.Count(); ind++ )
        {
        CCmMediaServerFull* server = iStoredServers[ind];

        if ( iUploadCapabilitySupport )
            {
            server->SetStoreUsage( EFalse );
            }
        else
            {            
            server->SetFillUsage( EFalse );
            }
            
        if ( !server->StoreUsage() && !server->FillUsage() )
            {
            server->SetIsActive( EFalse );	
            }
        }

    // Set selected servers active
    if( iSelectedServers )
        {
        // loop trough selected servers
        for ( TInt iy = 0; iy < iSelectedServers->Count(); iy++ )
            {

            // Look for the server in selection array
            TInt selectedIndex = iSelectedServers->At( iy );
            // server points to iStoredServers table
            CCmMediaServerFull* server = iServerList[selectedIndex];
           
            if ( iUploadCapabilitySupport )
                {
                server->SetStoreUsage( ETrue );
                }
            else // fill server
                {
                server->SetFillUsage( ETrue );
                }
            server->SetIsActive( ETrue );
            }
        }

    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::SetServersActivity \
    ends"));
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::CMSSettingItemDevices
// --------------------------------------------------------------------------
//
CMSSettingItemDevices::CMSSettingItemDevices( TInt aIdentifier, TDes& aText,
    CMSEngine& aMSEngine,
    RPointerArray<CCmMediaServerFull>& aStoredServers,
    TBool aUploadCapabilitySupport  ):
    CMSTextSettingItem( aIdentifier, aText ),
    iMSEngine( aMSEngine ),
    iStoredServers( aStoredServers ),
    iUploadCapabilitySupport( aUploadCapabilitySupport )
    {
    LOG( _L("[MSGSPlugin]\t CMSSettingItemDevices::CMSSettingItemDevices") );
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::ReadServerListL
//
// --------------------------------------------------------------------------
//
void CMSSettingItemDevices::ReadServerListL()
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::ReadServerListL"));

    // delete old ones
    iServerList.Reset();

    for ( TInt i = 0; i < iStoredServers.Count(); i++ )
        {
        // Add selected/marked servers to list
        if (iUploadCapabilitySupport && iStoredServers[i]->CopyCapability()
                && iStoredServers[i]->StoreUsage() )
            {
            // Add selected target servers
            iServerList.AppendL(iStoredServers[i]);
            }
        else if (!iUploadCapabilitySupport && iStoredServers[i]->FillUsage() )
            {
            // Add selected source servers
            iServerList.AppendL(iStoredServers[i]);
            }
        else
            {
            LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::ReadServerListL \
            Not selected servers should not show to user"));
            }
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::SetSettingItemTextL
// Sets setting item secondary text according to selected items
// --------------------------------------------------------------------------
//
void CMSSettingItemDevices::SetSettingItemTextL()
    {
    LOG( _L("[MSGSPlugin]\t CMSSettingItemDevices::SetSettingItemTextL") );
    
    // count how many active devices threre are
    TInt count = CountSelectedItems();
    HBufC* itemText = NULL;

    if ( count == 1 )
        {
        // one active found - need to get server name
        for ( TInt index = 0; index < iStoredServers.Count(); index++ )
            {
            CCmMediaServerFull* server = iStoredServers[index];

            if ( ( iUploadCapabilitySupport && server->StoreUsage() ) ||
                 ( !iUploadCapabilitySupport && server->FillUsage() ) )
                {
                itemText = CnvUtfConverter::ConvertToUnicodeFromUtf8L(                                                
                                                server->MediaServerName() );
                CleanupStack::PushL(itemText);

                iSettingText->Des().Copy( itemText->Left( KMaxFileName) );

                CleanupStack::PopAndDestroy( itemText );
                index = iStoredServers.Count(); // break loop                
                }
            else
                {
                LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::\
                SetSettingItemTextL copycapability required but not \
                supported by server"));
                }
            }
        }
    // more than one item selected
    else
        {
        if ( count > 1 )
            {
            itemText = StringLoader::LoadLC( R_MS_ITEM_DEVICES, count );
            }
        else
            {
            itemText = StringLoader::LoadLC( R_MS_NO_SERVER_SELECTED );
            }

        // do number conversion
        TPtr ptr = itemText->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );

        iSettingText->Des().Copy( *itemText );
        CleanupStack::PopAndDestroy( itemText );
        }

     // Set new value
     SetExternalText( *iSettingText );
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::CountSelectedItems
// Counts selected items
// --------------------------------------------------------------------------
//
TInt CMSSettingItemDevices::CountSelectedItems() const
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::CountSelectedItems"));

    TInt count(0);

    if( iStoredServers.Count() )
        {
        TRACE(Print(_L("[MSGSPlugin]\t server COUNT:  = %d"),
            iStoredServers.Count() ));
        for ( TInt index = 0; index < iStoredServers.Count(); index++ )
            {

            CCmMediaServerFull* server = iStoredServers[index];

            if ( iUploadCapabilitySupport && server->StoreUsage() )
                {
                count++;
                }
            else if ( !iUploadCapabilitySupport && server->FillUsage() )
                {
                count++;
                }
            else
                {
                LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::\
                CountSelectedItems server not used"));
                }
            }
        }
    TRACE(Print(_L("[MSGSPlugin]\t active server COUNT:  = %d"),
    count ));
    return count;
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::DialogDismissedL( TInt aButtonId )
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::DialogDismissedL"));

    if( aButtonId != EAknSoftkeyDone )
        {
        LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::\
        DialogDismissedL CANCEL"));
        iUserCancelledSearch = ETrue;
        // cancel server search if it was started
        CancelDeviceSearch();
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::UPnPDeviceDiscoveredL
// Returns discovered device from UPnP AV control point.
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::UPnPDeviceDiscoveredL(
                                    const CUpnpAVDevice& aDevice)
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::UPnPDeviceDiscoveredL"));

    // Check that this is mediaserver
    if ( aDevice.DeviceType() == CUpnpAVDevice::EMediaServer &&
         aDevice.SearchCapability() && aDevice.DlnaCompatible() )
        {
        LOG(_L("[MSGSPlugin]\t Found device is Media server!"));

#ifdef __DEBUG
        HBufC* devName = UpnpString::ToUnicodeL( aDevice.FriendlyName() );
        TRACE(Print(_L("[MSGSPlugin]\t device Name= %S"), devName ));
        delete devName;
#endif
        TPtrC8 deviceUDN = aDevice.Uuid();

        TInt serverCount = iStoredServers.Count();
        TBool newServer( ETrue );
        CCmMediaServerFull* tempServer = NULL;

        // Check if server is already on the list
        for ( TInt i=0; i < serverCount; i++ )
            {
            // Compare server udn
            if ( deviceUDN.Compare(
                    iStoredServers[i]->MediaServer() ) == 0 )
                {
                tempServer = iStoredServers[i];
                // server already on the list
                i = serverCount; // end loop
                newServer = EFalse;
                }
            }

        if ( newServer )
            {
            tempServer = CCmMediaServerFull::NewL();
            CleanupStack::PushL( tempServer );
            tempServer->SetUDNL( aDevice.Uuid() );
            tempServer->SetMediaServerNameL( aDevice.FriendlyName() );
            tempServer->SetCopyCapability( aDevice.CopyCapability() );
            
            // Add the server to found list            
            iStoredServers.Append( tempServer );
            
            CleanupStack::Pop( tempServer );
            }
        // Add unselected servers to show to user
        if ( ( iUploadCapabilitySupport && tempServer->CopyCapability()
              && !tempServer->StoreUsage() )
             || ( !iUploadCapabilitySupport && !tempServer->FillUsage() ) )
            {
            iServerList.AppendL( tempServer );
            if ( iWaitDialog )
                {
                TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
                iWaitDialog = NULL;
                }

            // If devices are searched for the first time
            // the dialog needs to be created in EditItemL.
            // This call is for updating existing dialog.
            if ( iDevSelectionDlg )
                {
                iDevSelectionDlg->UpdateAndDrawPopupL( tempServer );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::UPnPDeviceDiscovered
// Returns discovered device from UPnP AV control point
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::UPnPDeviceDiscovered(
                                    const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::\
             UPnPDeviceDiscovered"));

    TRAP_IGNORE( UPnPDeviceDiscoveredL( aDevice ) );       
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::UPnPDeviceDisappeared
// Returns disappeared device from UPnP AV control point.
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::UPnPDeviceDisappeared(
                                    const CUpnpAVDevice& /*aDevice*/ )
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::\
             UPnPDeviceDisappeared"));
    // do nothing
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::WLANConnectionLost
// Notifies wlan connection lost
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::WLANConnectionLost()
    {
    LOG(_L("[MSGSPlugin]\t CMSSettingItemDevices::WLANConnectionLost"));
    
    TRAP_IGNORE( DialogDismissedL( EAknSoftkeyCancel ) );
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::CreateAvcontrollerL
//
// --------------------------------------------------------------------------
//
TInt CMSSettingItemDevices::CreateAvcontrollerL()
    {
    LOG( _L("[MSGSPlugin]\t CMSSettingItemDevices::CreateAvcontrollerL" ) );

    if( iAvController )
        {
        iAvController->Release();
        iAvController = NULL;
        }

    iAvController = UPnPAVControllerFactory::NewUPnPAVControllerL();

    if( iAvController )
        {
        iAvController->SetDeviceObserver( *this );
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CMSSettingItemDevices::CancelDeviceSearch
//
// --------------------------------------------------------------------------
//
void CMSSettingItemDevices::CancelDeviceSearch()
    {
    LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::CancelDeviceSearch" ) );

    if ( iAvController )
        {
        iAvController->RemoveDeviceObserver();
		iAvController->Release();
		iAvController = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CMSSettingItemDevices::FetchAlreadyDiscoveredDevicesL
// Fetches devices from AVController and calls
// DeviceDiscovered-callback for each of them.
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::FetchAlreadyDiscoveredDevicesL()
    {
    LOG( _L("[MSGSPlugin]\t CMSSettingItemDevices::\
    FetchAlreadyDiscoveredDevicesL") );
    
    // If iAvController is not created, this method is unusable. 
    // Call CreateAvControllerL first.
    User::LeaveIfNull( iAvController );
    
    // Fetch already found devices from AVController.
    CUpnpAVDeviceList* discoveredDevices = 
        iAvController->GetMediaServersL();
    CleanupStack::PushL( discoveredDevices );
        
    if ( discoveredDevices && discoveredDevices->Count() > 0 ) 
        {
        // the AVController already has some devices.
        LOG(_L("[MSGSPlugin]\t \
                CMSSettingItemDevices::FetchAlreadyDiscoveredDevicesL: \
                AVController has already discovered devices" ) );

        // process the already existing devices
        for ( TInt i = 0 ; i < discoveredDevices->Count() ; i++ ) 
            {
            UPnPDeviceDiscovered( 
                *( discoveredDevices->operator[](i) ) );
            }
        }

    // clean up        
    CleanupStack::PopAndDestroy( discoveredDevices );
    discoveredDevices = NULL;
    }
    
// ---------------------------------------------------------------------------
// CMSSettingItemDevices::CreateAvcAndFetchDevicesL
// Creates AVController, fetches devices from it and calls
// DeviceDiscovered-callback for each of them. Starts wait note if no devices 
// are already discovered.
// ---------------------------------------------------------------------------
//
TInt CMSSettingItemDevices::CreateAvcAndFetchDevicesL()
    {
    LOG( _L(
        "[MSGSPlugin]\t CMSSettingItemDevices::CreateAvcAndFetchDevicesL" )
       );
    
    iUserCancelledSearch = EFalse;

    if( iSelectedServers )
        {
        iSelectedServers->Reset();
        delete iSelectedServers;
        iSelectedServers = NULL;
        }
    iSelectedServers =
            new (ELeave) CArrayFixFlat<TInt>( KItemArrayGranularity );

    iCreateAvCState = EPhaseNotActive;
    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL( reinterpret_cast<CBase*>( waitNoteWrapper ) );
    TRAPD( err, waitNoteWrapper->ExecuteL(
              R_MS_WAIT_FOR_CREATEAVC_DIALOG,
              *this,
              ETrue ) );//ETrue = show immediately
    CleanupStack::PopAndDestroy( waitNoteWrapper );


    if ( iAvController )
        {
        // Process devices that AVController has already discovered.
        FetchAlreadyDiscoveredDevicesL();

        // If still no devices discovered, start wait note.
        if ( iServerList.Count() == 0 )
            {
            iWaitDialog = new ( ELeave ) CAknWaitDialog(
            ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ) );
            iWaitDialog->SetCallback( this );
            iWaitDialog->ExecuteLD( R_MS_WAIT_DIALOG );
            LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::\
            CreateAvcAndFetchDevicesL starting wait" ) );
            iWait.Start(); // wait here until first server is found.
            }
        }
    else
        {
        //handle the error
        if( KErrDiskFull == err )
            {
            ShowErrorNoteL( R_MS_GS_MEMORY_FULL );
            }
        iUserCancelledSearch = ETrue; // Don't show selection dialog.
        }

    return err;
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::ShowErrorNoteL
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::ShowErrorNoteL( TInt aTextResource )
    {
    LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::ShowErrorNoteL" ) );
    HBufC* warningText =
                      StringLoader::LoadLC( aTextResource );
    CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( );
    dlg->ExecuteLD( *warningText );
    CleanupStack::PopAndDestroy( warningText );
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::StepL
// ---------------------------------------------------------------------------
//
void CMSSettingItemDevices::StepL()
    {
    LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::StepL" ) );
    if( EPhaseNotActive == iCreateAvCState )
        {
        CreateAvcontrollerL();
        iCreateAvCState = EPhaseCompleted;
        }
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::IsProcessDone
// ---------------------------------------------------------------------------
//
TBool CMSSettingItemDevices::IsProcessDone() const
    {
    LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::IsProcessDone" ) );
    TBool ret( EFalse );
    if ( EPhaseNotActive == iCreateAvCState )
        {
        // try to start avcontroller
        }
    else if ( EPhaseCompleted  == iCreateAvCState )
        {
        ret = ETrue;
        }
    else
        {
        LOG( _L( "[MSGSPlugin]\t CMSSettingItemDevices::IsProcessDone \
        other branch" ) );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMSSettingItemDevices::CycleError
// handles the error
// ---------------------------------------------------------------------------
//
TInt CMSSettingItemDevices::CycleError( TInt aError )
    {
    TRACE( Print( _L( " CMSSettingItemDevices::CycleError \
            aError = %d " ), aError ) );

    return aError;
    }

// End of File
