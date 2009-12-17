/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      implementation of a server session
*
*/






// INCLUDE FILES
#include "upnpavcontrollersession.h"
#include "upnpavcontrollerserver.pan"

#include "upnpavcontrollerserver.h"
#include "upnpavcontrollerimpl.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::CUpnpAVControllerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpAVControllerSession::CUpnpAVControllerSession(
    CUpnpAVControllerServer& aServer):
    CSession2(),
    iAVControllerServer( aServer )
    {
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpAVControllerSession::ConstructL()
    {
    __LOG( "CUpnpAVControllerSession::ConstructL" );
    
    iAVControllerServer.IncrementSessions();
    
    iAVController = CUPnPAVControllerImpl::NewL
        (
        iAVControllerServer.MediaServer(),
        iAVControllerServer
        );
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::~CUpnpAVControllerSession
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpAVControllerSession::~CUpnpAVControllerSession()
    {
    iAVControllerServer.DecrementSessions();        
    
    delete iAVController;
    
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::ServiceL
// Handle client requests.
// --------------------------------------------------------------------------
//
void CUpnpAVControllerSession::ServiceL( const RMessage2& aMessage )
    {   
         
    switch ( aMessage.Function() )
        {
        case EAVControllerStartupRequest:
            iAVControllerServer.StartUpL();
            aMessage.Complete( KErrNone );
            break;
        
        case EAVControllerCancelStartupRequest:
            iAVControllerServer.CancelStartUp();
            aMessage.Complete( KErrNone );
            break;        
        
        case EAVControllerDeviceRequest:           
            iAVController->EnableDeviceDiscoveryL( aMessage );
            break;
            
        case EAVControllerGetDeviceRequest:
            iAVController->GetDeviceL( aMessage );
            break;                      

        case EAVControllerCancelDeviceRequest:
            iAVController->DisableDeviceDiscoveryL();
            aMessage.Complete( KErrNone );
            break;                      
        
        case EAVControllerGetDeviceListSizeRequest:
            iAVController->GetDeviceListSizeL( aMessage );
            break;

        case EAVControllerGetDeviceListRequest:
            iAVController->GetDeviceListL( aMessage );
            break;

        case EAVControllerCreateRenderingSession:
            iAVController->CreateRenderingSessionL( aMessage );
            break;

        case EAVControllerDestroyRenderingSession:
            iAVController->DestroyRenderingSessionL( aMessage );
            break;
            
        case EAVControllerEventRequest:
            iAVController->EventRequestL( aMessage );
            break;
            
        case EAVControllerCancelEventRequest:
            iAVController->CancelEventRequestL( aMessage );
            break;

        case EAVControllerSetURI:
            iAVController->SetURIL( aMessage );
            break;
            
        case EAVControllerCancelSetURI:
            iAVController->CancelSetURIL( aMessage );
            break;
    
        case EAVControllerSetNextURI:
            iAVController->SetNextURIL( aMessage );
            break;
            
        case EAVControllerCancelSetNextURI:
            iAVController->CancelSetNextURIL( aMessage );
            break;

        case EAVControllerPlay:
            iAVController->PlayL( aMessage );
            break;
    
        case EAVControllerCancelPlay:
            iAVController->CancelPlayL( aMessage );
            break;
            
        case EAVControllerStop:
            iAVController->StopL( aMessage );
            break;
    
        case EAVControllerCancelStop:
            iAVController->CancelStopL( aMessage );
            break;
            
        case EAVControllerPause:
            iAVController->PauseL( aMessage );
            break;
            
        case EAVControllerCancelPause:
            iAVController->CancelPauseL( aMessage );
            break;
    
        case EAVControllerSetVolume:
            iAVController->SetVolumeL( aMessage );
            break;

        case EAVControllerCancelSetVolume:
            iAVController->CancelSetVolumeL( aMessage );
            break;
    
        case EAVControllerGetVolume:
            iAVController->GetVolumeL( aMessage );
            break;
    
        case EAVControllerCancelGetVolume:
            iAVController->CancelGetVolumeL( aMessage );
            break;
    
        case EAVControllerSetMute:
            iAVController->SetMuteL( aMessage );
            break;

        case EAVControllerCancelSetMute:
            iAVController->CancelSetMuteL( aMessage );
            break;
    
        case EAVControllerGetMute:
            iAVController->GetMuteL( aMessage );
            break;
            
        case EAVControllerCancelGetMute:
            iAVController->CancelGetMuteL( aMessage );
            break;
            
        case EAVControllerGetPositionInfo:
            iAVController->GetPositionInfoL( aMessage );
            break;
            
        case EAVControllerCancelGetPositionInfo:
            iAVController->CancelGetPositionInfoL( aMessage );
            break;
            
        case EAVControllerCreateBrowsingSession:
            iAVController->CreateBrowsingSessionL( aMessage );
            break;
            
        case EAVControllerDestroyBrowsingSession:
            iAVController->DestroyBrowsingSessionL( aMessage );
            break;

        case EAVControllerGetBrowseResponseSize:
            iAVController->GetBrowseResponseSizeL( aMessage );
            break;
        
        case EAVControllerCancelGetBrowseResponseSize:
            iAVController->CancelGetBrowseResponseSizeL( aMessage );
            break;
        
        case EAVControllerGetBrowseResponse:
            iAVController->GetBrowseResponseL( aMessage );
            break;
                    
        case EAVControllerGetSearchResponseSize:
            iAVController->GetSearchResponseSizeL( aMessage );
            break;
            
        case EAVControllerCancelGetSearchResponseSize:
            iAVController->CancelGetSearchResponseSizeL( aMessage );
            break;
        
        case EAVControllerGetSearchResponse:
            iAVController->GetSearchResponseL( aMessage );
            break;
                    
        case EAVControllerGetSearchCapabilitiesSize:
            iAVController->GetSearchCapabitiesSizeL( aMessage );
            break;    
                            
        case EAVControllerCancelGetSearchCapabilitiesSize:
            iAVController->CancelGetSearchCapabitiesSizeL( aMessage );
            break;
            
        case EAVControllerGetSearchCapabilities:
            iAVController->GetSearchCapabitiesL( aMessage );
            break;

        case EAVControllerCreateContainer:
            iAVController->CreateContainerL( aMessage );
            break;
        
        case EAVControllerCancelCreateContainer:
            iAVController->CancelCreateContainerL( aMessage );
            break;  

        case EAVControllerDeleteObject:
            iAVController->DeleteObjectL( aMessage );
            break;
            
        case EAVControllerCancelDeleteObject:
            iAVController->CancelDeleteObjectL( aMessage );
            break;
            
        case EAVControllerDeviceDisappearedRequest:
            iAVController->DeviceDisappearedRequestL( aMessage );
            break;
            
        case EAVControllerCancelDeviceDisappearedRequest:
            iAVController->CancelDeviceDisappearedRequestL( aMessage );
            break;              

        case EAVControllerStartMediaServer:
            iAVControllerServer.StartMediaServerL( aMessage );
            break;
            
        case EAVControllerCancelStartMediaServer:
            iAVControllerServer.CancelStartMediaServerL( aMessage );
            break;

        case EAVControllerStopMediaServer:
            iAVControllerServer.StopMediaServerL( aMessage );
            break;
            
        case EAVControllerMonitorConnection:
            iAVController->MonitorConnectionL( aMessage );
            break;

        case EAVControllerCancelMonitorConnection:
            iAVController->CancelMonitorConnectionL( aMessage );
            break;
        
        case EAVControllerMSServicesInUse:
            iAVControllerServer.MSServicesInUse( aMessage );
            break;
            
        case EAVControllerCreateDownloadSession:
            iAVController->CreateDownloadSessionL( aMessage );
            break;
            
        case EAVControllerCreateUploadSession:
            iAVController->CreateUploadSessionL( aMessage );       
            break;
        
        case EAVControllerDestroyDownloadSession:
            iAVController->DestroyDownloadSessionL( aMessage );
            break;
        
        case EAVControllerDestroyUploadSession:
            iAVController->DestroyUploadSessionL( aMessage );
            break;
        
        case EAVControllerStartDownload:
            iAVController->StartDownloadL( aMessage );
            break;
        
        case EAVControllerCancelDownload:
            iAVController->CancelDownloadL( aMessage );
            break;
        
        case EAVControllerCancelAllDownloads:
            iAVController->CancelAllDownloadsL( aMessage );
            break;
        
        case EAVControllerStartUpload:
            iAVController->StartUploadL( aMessage );
            break;
        
        case EAVControllerCancelUpload:
            iAVController->CancelUploadL( aMessage );
            break;
        
        case EAVControllerCancelAllUploads:
            iAVController->CancelAllUploadsL( aMessage );
            break;
        
        case EAVControllerStartTrackingUploadProgress:
            iAVController->StartTrackingUploadProgressL( aMessage );
            break;
        
        case EAVControllerStartTrackingDownloadProgress:
            iAVController->StartTrackingDownloadProgressL( aMessage );
            break;
        
        case EAVControllerGetDownloadEvent:
            iAVController->GetDownloadEventL( aMessage );
            break;
        
        case EAVControllerCancelDownloadEvent:
            iAVController->CancelGetDownloadEventL( aMessage );
            break;
        
        case EAVControllerGetUploadEvent:
            iAVController->GetUploadEventL( aMessage );
            break;
        
        case EAVControllerCancelUploadEvent:
            iAVController->CancelGetUploadEventL( aMessage );
            break;
        
        case EAVControllerStartDownloadFH:
            iAVController->StartDownloadFHL( aMessage );
            break;

        default:
            PanicClient( aMessage, EAVControllerServerBadRequest );
            break;
        }
    }


void CUpnpAVControllerSession::DeviceDiscoveredL(
    CUpnpAVDeviceExtended& aDevice )
    {
    iAVController->DeviceDiscoveredL( aDevice );
    }

 
void CUpnpAVControllerSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    iAVController->DeviceDisappearedL( aDevice );
    }


void CUpnpAVControllerSession::ConnectionLost()
    {
    iAVController->ConnectionLost();
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::PanicClient
// Panic client.
// --------------------------------------------------------------------------
//
void CUpnpAVControllerSession::PanicClient( const RMessage2 &aMessage,
    TInt aPanic ) const
    {
     // Note: this panics the client thread, not server
    aMessage.Panic( KAVControllerName, aPanic );
    }
// --------------------------------------------------------------------------
// CUpnpAVControllerSession::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpAVControllerSession* CUpnpAVControllerSession::NewL(
    CUpnpAVControllerServer& aServer )
    {
    CUpnpAVControllerSession* self =
        CUpnpAVControllerSession::NewLC(aServer); 
    CleanupStack::Pop( self ) ;
    return self ;
    }

// --------------------------------------------------------------------------
// CUpnpAVControllerSession::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpAVControllerSession* CUpnpAVControllerSession::NewLC( 
    CUpnpAVControllerServer& aServer )
    {
    CUpnpAVControllerSession* self =
        new (ELeave) CUpnpAVControllerSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL() ;
    return self ;
    }

// End of File



