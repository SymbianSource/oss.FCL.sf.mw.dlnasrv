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
* Description:      AV Controller client
*
*/






// INCLUDE FILES
// Sysatem
#include <e32math.h>
#include <s32mem.h>
#include <f32file.h>

// upnpframework / avcontroller api
#include "upnpavcontrollerglobals.h"

// avcontroller internal
#include "upnpavcontrollerclient.h"

// Number of message slots to reserve for this client server session.
const TInt KDefaultMessageSlots = -1;


// Function prototypes
static TInt StartServer( void );
static TInt CreateServerProcess( void ); 

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::RUPnPAVControllerClient
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
RUPnPAVControllerClient::RUPnPAVControllerClient()
    :RSessionBase() 
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::Connect
// Connect to Media Server session. 
// exist
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::Connect()
    {
    TInt error = ::StartServer();

    if ( KErrNone == error )
        {
        error = CreateSession( KAVControllerName,
                               Version(),
                               KDefaultMessageSlots );
        }
    return error;
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::Version
// Version information.
// --------------------------------------------------------------------------
TVersion RUPnPAVControllerClient::Version() const
    {
    return TVersion( KAVControllerMajorVersionNumber,
                     KAVControllerMinorVersionNumber,
                     KAVControllerBuildVersionNumber );
    }


// --------------------------------------------------------------------------
// RUPnPAVControllerClient::StartUp
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::StartUp( TRequestStatus& aStatus )
    {
    TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( EAVControllerStartupRequest, args, aStatus );    
    }


// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelStartUp
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelStartUp()
    {
    TIpcArgs args( TIpcArgs::ENothing );
    return SendReceive( EAVControllerCancelStartupRequest, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DeviceRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::DeviceRequest( TDes8& aDiscovered,
    TDes8& aRcvdBufSize, TRequestStatus& aStatus )
    {
    TIpcArgs args( &aDiscovered, &aRcvdBufSize );
    SendReceive( EAVControllerDeviceRequest, args, aStatus );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelDeviceRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelDeviceRequest()
    {
    TIpcArgs args( TIpcArgs::ENothing );
    return SendReceive( EAVControllerCancelDeviceRequest, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetDevice
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::GetDevice( TDes8& aDevice )
    {
    TIpcArgs args( &aDevice );
    return SendReceive( EAVControllerGetDeviceRequest, args );
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetDeviceListSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::GetDeviceListSize( TDes8& aType, TDes8& aSize )
    {
    TIpcArgs args( &aType, &aSize );
    return SendReceive( EAVControllerGetDeviceListSizeRequest, args );
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetDeviceList
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::GetDeviceList( TDes8& aList )
    {
    TIpcArgs args( &aList );
    return SendReceive( EAVControllerGetDeviceListRequest, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::ReleaseMediaServer
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::ReleaseMediaServer( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerStopMediaServer, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::ReserveMediaServer
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::ReserveMediaServer( TInt aId,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId );
    SendReceive( EAVControllerStartMediaServer, args, aStatus );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelReserveMediaServer
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelReserveMediaServer( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelStartMediaServer, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CreateRenderingSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CreateRenderingSession( TInt aId,
    TDes8& aUuid )
    {
    TIpcArgs args( aId, &aUuid );
    return SendReceive( EAVControllerCreateRenderingSession, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DestroyRenderingSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::DestroyRenderingSession( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerDestroyRenderingSession, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::EventRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::EventRequest( TDes8& aEvent, TInt aId,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aEvent );
    SendReceive( EAVControllerEventRequest, args, aStatus );   
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelEventRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelEventRequest( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelEventRequest, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SetURI
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::SetURI( TInt aId, TDes8& aRequest,
    TDes8& aItem, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aRequest, &aItem );
    
    SendReceive( EAVControllerSetURI, args, aStatus );   
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelSetURI
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelSetURI( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelSetURI, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SetNextURI
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::SetNextURI( TInt aId, TDes8& aRequest,
    TDes8& aItem, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aRequest, &aItem );
    
    SendReceive( EAVControllerSetNextURI, args, aStatus );       
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelSetNextURI
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelSetNextURI( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelSetNextURI, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::Play
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::Play( TInt aId, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId );
    
    SendReceive( EAVControllerPlay, args, aStatus );       
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelPlay
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelPlay( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelPlay, args );
    }
      
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::Stop
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::Stop( TInt aId, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId );
    SendReceive( EAVControllerStop, args, aStatus ); 
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelStop
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelStop( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelStop, args ); 
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::Pause
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::Pause( TInt aId, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId );
    SendReceive( EAVControllerPause, args, aStatus ); 
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelPause
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelPause( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelPause, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SetVolume
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::SetVolume( TInt aId, TInt aVolumeLevel,
    TDes8& aRetVolume, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, aVolumeLevel, &aRetVolume );
    SendReceive( EAVControllerSetVolume, args, aStatus );      
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelSetVolume
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelSetVolume( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelSetVolume, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetVolume
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetVolume( TInt aId, TDes8& aVolumeLevel,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aVolumeLevel );
    SendReceive( EAVControllerGetVolume, args, aStatus );     
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelGetVolume
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelGetVolume( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetVolume, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SetMute
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::SetMute( TInt aId, TBool aMute,
    TDes8& aRetMute, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, (TInt)aMute, &aRetMute );
    SendReceive( EAVControllerSetMute, args, aStatus );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetMute
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetMute( TInt aId, TDes8& aMute,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aMute );
    SendReceive( EAVControllerGetMute, args, aStatus );     
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelSetMute
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelSetMute( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelSetMute, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelGetMute
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelGetMute( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetMute, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetPositionInfo
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetPositionInfo( TInt aId, TDes8& aPosition,
    TDes8& aLength, TRequestStatus& aStatus )    
    {
    TIpcArgs args( aId, &aPosition, &aLength );
    SendReceive( EAVControllerGetPositionInfo, args, aStatus );     
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelGetPositionInfo
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelGetPositionInfo( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetPositionInfo, args ); 
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CreateBrowsingSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CreateBrowsingSession( TInt aId, TDes8& aUuid )
    {
    TIpcArgs args( aId, &aUuid );
    return SendReceive( EAVControllerCreateBrowsingSession, args ); 
    }


// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DestroyBrowsingSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::DestroyBrowsingSession( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerDestroyBrowsingSession, args );     
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::BrowseResponseSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::BrowseResponseSize( TInt aId, TDes8& aRequest,
    TDes8& aParams, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aRequest, &aParams );
    SendReceive( EAVControllerGetBrowseResponseSize, args, aStatus );         
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelBrowseResponseSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelBrowseResponseSize( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetBrowseResponseSize, args );     
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::BrowseResponse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::BrowseResponse( TInt aId,
    TDes8& aBrowseResponse, TDes8& aUpdateId)
    {
    TIpcArgs args( aId, &aBrowseResponse, &aUpdateId );
    return SendReceive( EAVControllerGetBrowseResponse, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SearchResponseSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::SearchResponseSize( TInt aId, TDes8& aRequest,
    TDes8& aParams, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aRequest, &aParams );
    SendReceive( EAVControllerGetSearchResponseSize, args, aStatus );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelSearchResponseSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelSearchResponseSize( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetSearchResponseSize, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SearchResponse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::SearchResponse( TInt aId,
    TDes8& aSearchResponse, TDes8& aUpdateId )
    {
    TIpcArgs args( aId, &aSearchResponse,& aUpdateId );
    return SendReceive( EAVControllerGetSearchResponse, args );             
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::GetSearchCapabilitiesSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetSearchCapabilitiesSize( TInt aId,
    TDes8& aSize, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aSize );
    SendReceive( EAVControllerGetSearchCapabilitiesSize, args, aStatus );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelGetSearchCapabilitiesSize
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelGetSearchCapabilitiesSize( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelGetSearchCapabilitiesSize,
        args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::SearchCapabilitiesResponse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::SearchCapabilitiesResponse( TInt aId,
    TDes8& aCapabilitiesResponse )
    {
    TIpcArgs args( aId, &aCapabilitiesResponse );
    return SendReceive( EAVControllerGetSearchCapabilities, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CreateContainer
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::CreateContainer( TInt aId, TDes8& aTitle,
    TDes8& aParentId, TInt aContainerType, TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aTitle, &aParentId, aContainerType );
    SendReceive( EAVControllerCreateContainer, args, aStatus );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelCreateContainer
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelCreateContainer( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelCreateContainer, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DeleteObject
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::DeleteObject( TInt aId, TDes8& aObjectId,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aObjectId );
    SendReceive( EAVControllerDeleteObject, args, aStatus );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelDeleteObject
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelDeleteObject( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelDeleteObject, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DeviceDisappearedRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::DeviceDisappearedRequest( TInt aId,
    TRequestStatus& aStatus )
    {
    TIpcArgs args( aId );
    SendReceive( EAVControllerDeviceDisappearedRequest, args, aStatus );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelDeviceDisappearedRequest
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelDeviceDisappearedRequest( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelDeviceDisappearedRequest, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MonitorConnection
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::MonitorConnection( TRequestStatus& aStatus )
    {
    TIpcArgs args( TIpcArgs::ENothing );
    SendReceive( EAVControllerMonitorConnection, args, aStatus );        
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CancelMonitorConnection
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelMonitorConnection()
    {
    TIpcArgs args( TIpcArgs::ENothing );
    return SendReceive( EAVControllerCancelMonitorConnection, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::MSServicesInUse( TDes8& aInUse )
    {
    TIpcArgs args( &aInUse );
    return SendReceive( EAVControllerMSServicesInUse, args ); 
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CreateDownloadSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CreateDownloadSession( TInt aId, TDes8& aUuid )
    {
    TIpcArgs args( aId, &aUuid );
    return SendReceive( EAVControllerCreateDownloadSession, args );        
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::DestroyDownloadSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::DestroyDownloadSession( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerDestroyDownloadSession, args );    
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::CreateUploadSession
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CreateUploadSession( TInt aId, TDes8& aUuid )
    {
    TIpcArgs args( aId, &aUuid );
    return SendReceive( EAVControllerCreateUploadSession, args );            
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::DestroyUploadSession( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerDestroyUploadSession, args );        
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::StartDownload( TInt aId,
    TDes8& aFileTransferItem )
    {
    TIpcArgs args( aId, &aFileTransferItem );
    return SendReceive( EAVControllerStartDownload, args );    
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::StartDownload( TInt aId,
    TDes8& aFileTransferItem, const RFile& aFile )
    {
    TIpcArgs args( aId, &aFileTransferItem );
    aFile.TransferToServer( args, 2, 3 );
    return SendReceive( EAVControllerStartDownloadFH, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetDownloadEvent( TInt aId, TDes8& aEvent,
    TRequestStatus& aStatus, TDes& aBuffer )
    {
    TIpcArgs args( aId, &aEvent, &aBuffer );
    SendReceive( EAVControllerGetDownloadEvent, args, aStatus );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelDownloadEvent( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelDownloadEvent, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::StartTrackingDownloadProgress( TInt aId,
    TInt aKey )
    {
    TIpcArgs args( aId, aKey );
    return SendReceive( EAVControllerStartTrackingDownloadProgress, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelDownload( TInt aId, TInt aKey )
    {
    TIpcArgs args( aId, aKey );
    return SendReceive( EAVControllerCancelDownload, args );    
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelAllDownloads( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelAllDownloads, args );
    }

// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::StartUpload( TInt aId, TDes8& aFileTransferItem )
    {
    TIpcArgs args( aId, &aFileTransferItem );
    return SendReceive( EAVControllerStartUpload, args );    
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
void RUPnPAVControllerClient::GetUploadEvent( TInt aId, TDes8& aEvent,
        TRequestStatus& aStatus )
    {
    TIpcArgs args( aId, &aEvent );
    SendReceive( EAVControllerGetUploadEvent, args, aStatus );    
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelUploadEvent( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelUploadEvent, args );    
    }
        
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::StartTrackingUploadProgress( TInt aId, TInt aKey )
    {
    TIpcArgs args( aId, aKey );
    return SendReceive( EAVControllerStartTrackingUploadProgress, args );    
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelUpload( TInt aId, TInt aKey )
    {
    TIpcArgs args( aId, aKey );
    return SendReceive( EAVControllerCancelUpload, args );        
    }
    
// --------------------------------------------------------------------------
// RUPnPAVControllerClient::MSServicesInUse
// See upnpavcontrollerclient.h
// --------------------------------------------------------------------------
TInt RUPnPAVControllerClient::CancelAllUploads( TInt aId )
    {
    TIpcArgs args( aId );
    return SendReceive( EAVControllerCancelAllUploads, args );    
    }

// --------------------------------------------------------------------------
// StartServer
// Static method to start the server.
// --------------------------------------------------------------------------
static TInt StartServer()
    {
    TInt result;

    TFindServer findAVControllerServer( KAVControllerName );
    TFullName name;

    result = findAVControllerServer.Next( name );
    if ( result != KErrNone )
        {
        // server not found - try to create it.
        result = CreateServerProcess();
        }

    return  result;
    }

// --------------------------------------------------------------------------
// CreateServerProcess
// Static method to create the server process.
// --------------------------------------------------------------------------
static TInt CreateServerProcess()
    {
    TInt result;

    const TUidType serverUid( KNullUid, KNullUid, KAVCServerUid3 );
    RProcess server;

    result = server.Create( KAVControllerFilename, KNullDesC, serverUid );
    if ( result != KErrNone )
        {
        return  result;
        }

    TRequestStatus stat = KRequestPending;
    server.Rendezvous(stat);
    if ( stat != KRequestPending )
        {
        server.Kill( 0 );       // abort startup
        }
    else
        {
        server.Resume();    // logon OK - start the server
        }
        
    User::WaitForRequest(stat);     // wait for start or death

    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    if ( server.ExitType() == EExitPanic )
        {
        result = KErrGeneral;
        }
    else
        {
        result = stat.Int();
        }
    
    server.Close();   
    return result;
    }






