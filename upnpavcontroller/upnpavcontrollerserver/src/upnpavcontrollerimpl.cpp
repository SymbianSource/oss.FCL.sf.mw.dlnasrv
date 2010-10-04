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
* Description:      AVController server side root class
*
*/






// INCLUDE FILES
// upnp stack api
#include <upnpdevice.h>
#include <upnpcontainer.h>
#include <upnpservice.h>
#include <upnpstatevariable.h>
#include <upnpaction.h>
#include <upnpobjectlist.h>
#include <upnpavcontrolpoint.h>

// upnpframework / avcontroller api
#include "upnpavdeviceobserver.h"
#include "upnpavdevicelist.h"

// avcontroller internal
#include "upnpavcontrollerimpl.h"
#include "upnpavcontrollerserver.h"
#include "upnpavdispatcher.h"
#include "upnpavdeviceextended.h"
#include "upnpdevicerepository.h"
#include "upnpaverrorhandler.h"
#include "upnpplaybacksession.h"
#include "upnpbrowsingsession.h"
#include "upnpdevicediscoverymessage.h"

#include "upnpuploadsession.h"
#include "upnpdownloadsession.h"



#include "upnpsecaccesscontroller.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::NewL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
CUPnPAVControllerImpl* CUPnPAVControllerImpl::NewL(
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer )
    {
    CUPnPAVControllerImpl* self = new (ELeave) CUPnPAVControllerImpl( 
        aClient, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CUPnPAVControllerImpl
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
CUPnPAVControllerImpl::CUPnPAVControllerImpl(
    RUpnpMediaServerClient& aClient,
    CUpnpAVControllerServer& aServer ) :
    iMediaServer( aClient ),
    iServer( aServer ),
    iDeviceDiscoveryEnabled( EFalse),
    iDeviceMsgQue( CUpnpDeviceDiscoveryMessage::LinkOffset() ),
    iDeviceMsgQueIter( iDeviceMsgQue )
    {
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::~CUPnPAVControllerImpl
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
CUPnPAVControllerImpl::~CUPnPAVControllerImpl()
    {
    __LOG( "CUPnPAVControllerImpl::~CUPnPAVControllerImpl" );
    
    delete iDeviceRespBuf;
    delete iDeviceListRespBuf;
    
    iPlaybackSessions.ResetAndDestroy();
    iBrowsingSessions.ResetAndDestroy();
    
    iUploadSessions.ResetAndDestroy();
    iDownloadSessions.ResetAndDestroy();
    
    CUpnpDeviceDiscoveryMessage* devMsg = NULL;
    iDeviceMsgQueIter.SetToFirst(); 
    while ( ( devMsg = iDeviceMsgQueIter++ ) != NULL )
        {
        iDeviceMsgQue.Remove( *devMsg );
        delete devMsg;
        };    

    delete iConnectionMsg; iConnectionMsg = NULL;
    delete iDeviceDiscoveryMsg; iDeviceDiscoveryMsg = NULL;        
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::ConstructL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::ConstructL()
    {
    __LOG( "CUPnPAVControllerImpl::ConstructL" );    
       
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::ConnectionLost
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::ConnectionLost()
    {
    if( iConnectionMsg )
        {
        iConnectionMsg->Complete( EAVControllerConnectionLost );
        delete iConnectionMsg; iConnectionMsg = NULL;
        
        }
    else
        {
        // No msg, no can do
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DeviceDiscoveredL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DeviceDiscoveredL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPAVControllerImpl::DeviceDiscoveredL" );

    if( aDevice.DeviceType() == CUpnpAVDevice::EMediaServer &&
        ( aDevice.Local() ) )
        {
        TInt i;
        TInt count = iPlaybackSessions.Count();
        for( i = 0; i < count; i++ )
            {
            iPlaybackSessions[ i ]->SetLocalMSUuidL( aDevice.Uuid() );
            }
        
        count = iBrowsingSessions.Count();
        for( i = 0; i < count; i++ )
            {
            iBrowsingSessions[ i ]->SetLocalMSUuidL( aDevice.Uuid() );
            }

        }
    else if( iDeviceDiscoveryEnabled )
        {
        CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL( aDevice ); 
                             
        if( iDeviceDiscoveryMsg )
            {
            CleanupStack::PushL( tempDev );
            
            delete iDeviceRespBuf; iDeviceRespBuf = NULL;
            iDeviceRespBuf = tempDev->ToDes8L();
            
            CleanupStack::PopAndDestroy( tempDev );
            
            // Write back to the client that a device was discovered
            // of the device and the size
            TPckg<TAVControllerDeviceDiscovery> resp0( EAVDeviceDiscovered );
            TPckg<TInt> resp1( iDeviceRespBuf->Length() );
            
            iDeviceDiscoveryMsg->WriteL( 0, resp0 );
            iDeviceDiscoveryMsg->WriteL( 1, resp1 );
            
            iDeviceDiscoveryMsg->Complete( EAVControllerDeviceCompleted );
            delete iDeviceDiscoveryMsg; iDeviceDiscoveryMsg = NULL;
            }
        else
            {
            // Msg not valid, client is still processing.. 
            //add the discovered device to queue
            CUpnpDeviceDiscoveryMessage* tmpDevMsg = 
                    CUpnpDeviceDiscoveryMessage::NewL( 
                    tempDev, EAVDeviceDiscovered );
            iDeviceMsgQue.AddLast( *tmpDevMsg );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DeviceDisappearedL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPAVControllerImpl::DeviceDisappearedL" );
  
    // Go through browsing and rendering sessions and report if their
    // device or the local MS disappeared
    TBool localMs = EFalse;
    if( aDevice.DeviceType() == CUpnpAVDevice::EMediaServer &&
        ( aDevice.Local() ) )
        {
        __LOG( "CUPnPAVControllerImpl::DeviceDisappearedL\
        - Local MS disappeared!" );
        
         // Local MS disappeared
         localMs = ETrue;
        }
        
    TInt i;
    TInt count = iPlaybackSessions.Count();
    for( i = 0; i < count; i++ )
        {
        if( ( iPlaybackSessions[ i ]->Uuid() == aDevice.Uuid() ) ||
            ( localMs ) )
            {
            iPlaybackSessions[ i ]->DeviceDisappearedL( aDevice );
            }     
        }
    
    count = iBrowsingSessions.Count();
    for( i = 0; i < count; i++ )
        {
        if( ( iBrowsingSessions[ i ]->Uuid() == aDevice.Uuid() ) ||
            ( localMs ) )
            {
            iBrowsingSessions[ i ]->DeviceDisappearedL( aDevice );
            }     
        }
        
    count = iUploadSessions.Count();
    for( i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->Uuid() == aDevice.Uuid() )
            {
            iUploadSessions[ i ]->DeviceDisappearedL( aDevice );
            }     
        }

    count = iDownloadSessions.Count();
    for( i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->Uuid() == aDevice.Uuid() )
            {
            iDownloadSessions[ i ]->DeviceDisappearedL( aDevice );
            }     
        }

    if( iDeviceDiscoveryEnabled )
        {       
        // Create a device (buffer)
        CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL( aDevice );
            
         if( iDeviceDiscoveryMsg )
            {
            CleanupStack::PushL( tempDev );
                   
            delete iDeviceRespBuf; iDeviceRespBuf = NULL;
            iDeviceRespBuf = tempDev->ToDes8L();
            CleanupStack::PopAndDestroy( tempDev );
            
            // Write back to the client that a device was discovered
            // and the size of the device
            TPckg<TAVControllerDeviceDiscovery> resp0(
                EAVDeviceDisappeared );
            TPckg<TInt> resp1( iDeviceRespBuf->Length() );
            
            iDeviceDiscoveryMsg->WriteL( 0, resp0 );
            iDeviceDiscoveryMsg->WriteL( 1, resp1 );
            
            iDeviceDiscoveryMsg->Complete( EAVControllerDeviceCompleted );
            delete iDeviceDiscoveryMsg; iDeviceDiscoveryMsg = NULL;
            }
        else
            {
            // Msg not valid, client is still processing.. 
            //add the disappeared device to queue

            CUpnpDeviceDiscoveryMessage* tmpDevMsg = 
                    CUpnpDeviceDiscoveryMessage::NewL( 
                    tempDev, EAVDeviceDisappeared );
            iDeviceMsgQue.AddLast( *tmpDevMsg );
            }
        }
    }    

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::EnableDeviceDiscoveryL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::EnableDeviceDiscoveryL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPAVControllerImpl::EnableDeviceDiscoveryL" );
    
    __ASSERTD( !iDeviceDiscoveryMsg, __FILE__, __LINE__ );
    
    if( !iDeviceDiscoveryEnabled )
        {
        //iDispatcher.RegisterForDeviceDiscoveryL( *this );
        iDeviceDiscoveryEnabled = ETrue;    
        }
       
    iDeviceDiscoveryMsg = new (ELeave) RMessage2( aMessage );

    // Check queu's
    if( !iDeviceMsgQue.IsEmpty() )
        {
        CUpnpDeviceDiscoveryMessage* devMsg = iDeviceMsgQue.First();
        __ASSERTD( devMsg, __FILE__, __LINE__ );
        CUpnpAVDevice* dev = devMsg->Device();
        TAVControllerDeviceDiscovery devMsgType = devMsg->MsgType();
        DequeDeviceL( *dev, devMsgType );
        iDeviceMsgQue.Remove( *devMsg );
        delete devMsg;
        }
    else
        {
        // Empty else
        }             
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DequeDeviceL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DequeDeviceL( const CUpnpAVDevice& aDevice,
    TAVControllerDeviceDiscovery aType )
    {
    __LOG( "CUPnPAVControllerImpl::DequeDeviceL" );
    
    TPckg<TAVControllerDeviceDiscovery> resp0( aType );
    iDeviceDiscoveryMsg->WriteL( 0, resp0 );
    
    HBufC8* tmp = aDevice.ToDes8L();
    CleanupStack::PushL( tmp );

    TPckg<TInt> resp1( tmp->Length() );            
    iDeviceDiscoveryMsg->WriteL( 1, resp1 );
    
    CleanupStack::Pop( tmp );
     
    delete iDeviceRespBuf; iDeviceRespBuf = NULL;
    iDeviceRespBuf = tmp;
            
    iDeviceDiscoveryMsg->Complete( EAVControllerDeviceCompleted );
    delete iDeviceDiscoveryMsg; iDeviceDiscoveryMsg = NULL;
     

    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetDeviceL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetDeviceL( const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::GetDeviceL, 0x%d", iDeviceRespBuf );
    
    aMessage.WriteL( 0, *iDeviceRespBuf );
    aMessage.Complete( KErrNone );
    delete iDeviceRespBuf; iDeviceRespBuf = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DisableDeviceDiscoveryL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DisableDeviceDiscoveryL(
    /*const RMessage2& aMessage*/ )
    {
    __LOG( "CUPnPAVControllerImpl::DisableDeviceDiscoveryL" );
    
    if( iDeviceDiscoveryEnabled )
        {
        //iDispatcher.UnRegisterDeviceDiscovery( *this );
        iDeviceDiscoveryEnabled = EFalse;    
        }
    
    if( iDeviceDiscoveryMsg )
        { 
        iDeviceDiscoveryMsg->Complete( KErrCancel );
        delete iDeviceDiscoveryMsg; iDeviceDiscoveryMsg = NULL;        
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetDeviceListSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetDeviceListSizeL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPAVControllerImpl::GetDeviceListSizeL" );
    
    // Get the device list from the av control point, create a response buffer
    // and write the size of the list back to the client
    TAVControllerDeviceListType deviceListType;
    TPckg<TAVControllerDeviceListType> resp0( deviceListType );
    aMessage.ReadL( 0, resp0 );
    
    delete iDeviceListRespBuf; iDeviceListRespBuf = NULL;
    
    const RPointerArray<CUpnpAVDeviceExtended>& devList =
        iServer.DeviceRepository().DeviceList();
    
    if( deviceListType == EAVMediaServer )
        {
        CUpnpAVDeviceList* tempList = CUpnpAVDeviceList::NewLC();
                
        TInt count = devList.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( devList[ i ]->DeviceType() == CUpnpAVDevice::EMediaServer )
                {
                if( devList[ i ]->PInfoReceived() &&
                    !devList[ i ]->Local() )
                    {
                    CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL(
                        *devList[i] );
                    CleanupStack::PushL( tempDev );
                    // Ownership of tempDev transferred
                    tempList->AppendDeviceL( *tempDev );
                    CleanupStack::Pop( tempDev );
                    }
                }
            }
        iDeviceListRespBuf = tempList->ToDes8L();
        CleanupStack::PopAndDestroy( tempList );
        }
    else
        {
        CUpnpAVDeviceList* tempList = CUpnpAVDeviceList::NewLC();
                
        TInt count = devList.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( devList[ i ]->DeviceType() == CUpnpAVDevice::EMediaRenderer )
                {
                if( devList[ i ]->PInfoReceived() )
                    {
                    CUpnpAVDevice* tempDev = CUpnpAVDevice::NewL(
                        *devList[i] );
                    // Ownership of tempDev transferred
                    tempList->AppendDeviceL( *tempDev );                    
                    }
                }
            }
        iDeviceListRespBuf = tempList->ToDes8L();
        CleanupStack::PopAndDestroy( tempList ); 
        }    
    
    // Write the size back to the client
    TPckg<TInt> resp1( iDeviceListRespBuf->Length() );
    aMessage.WriteL( 1, resp1 );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetDeviceListL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetDeviceListL( const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::GetDeviceListL, 0x%d",
        iDeviceListRespBuf );
    
    // Write the device list back to the client
    aMessage.WriteL( 0, *iDeviceListRespBuf );
    aMessage.Complete( KErrNone );
    delete iDeviceListRespBuf; iDeviceListRespBuf = NULL;    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CreateRenderingSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CreateRenderingSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::CreateRenderingSessionL, 0x%d",
        aMessage.Int0() );
    
    // Create a rendering session
    TInt id = aMessage.Int0();
    
    // Read the device UID
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr ) );

    CUPnPPlaybackSession* tempPtr = CUPnPPlaybackSession::NewL(
        iMediaServer, iServer, id, *buf );

    CleanupStack::PopAndDestroy( buf );
    iPlaybackSessions.AppendL( tempPtr );
    
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DestroyRenderingSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DestroyRenderingSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::DestroyRenderingSessionL, 0x%d",
        aMessage.Int0() );
    
    // Find and delete a rendering session
    TInt id = aMessage.Int0();
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            delete iPlaybackSessions[ i ];
            iPlaybackSessions.Remove( i );
            found = ETrue;
            i = count;
            }
        }
    if( found )
        {
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::EventRequestL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::EventRequestL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->EventRequestL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelEventRequestL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelEventRequestL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelEventRequestL();
            found = ETrue;
            i = count;
            }
        }
    if( found )
        {
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::SetURIL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::SetURIL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->SetURIL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelSetURIL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelSetURIL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelSetURIL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }            
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::SetNextURIL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::SetNextURIL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->SetNextURIL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }            
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelSetNextURIL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelSetNextURIL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelSetNextURIL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }            
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::PlayL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::PlayL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->PlayL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
   
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelPlayL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelPlayL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelPlayL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::StopL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::StopL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->StopL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
   
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelStopL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelStopL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelStopL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }
   
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::PauseL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::PauseL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->PauseL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelPauseL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelPauseL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelPauseL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::SetVolumeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::SetVolumeL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->SetVolumeL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelSetVolumeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelSetVolumeL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelSetVolumeL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }  
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetVolumeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetVolumeL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->GetVolumeL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetVolumeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetVolumeL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelGetVolumeL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::SetMuteL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::SetMuteL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->SetMuteL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelSetMuteL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelSetMuteL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelSetMuteL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }  
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetMuteL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetMuteL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->GetMuteL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetMuteL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetMuteL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelGetMuteL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetPositionInfoL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetPositionInfoL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->GetPositionInfoL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetPositionInfoL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetPositionInfoL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iPlaybackSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iPlaybackSessions[ i ]->SessionId() == id )
            {
            iPlaybackSessions[ i ]->CancelGetPositionInfoL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CreateBrowsingSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CreateBrowsingSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::CreateBrowsingSessionL, 0x%d",
        aMessage.Int0() );
    
    // Create a rendering session
    TInt id = aMessage.Int0();
    
    // Read the device UID
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr ) );
    
    CUPnPBrowsingSession* tempPtr = CUPnPBrowsingSession::NewL(
        iMediaServer, iServer, id, *buf );

    CleanupStack::PopAndDestroy( buf );
    iBrowsingSessions.AppendL( tempPtr );
    
    aMessage.Complete( KErrNone );    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DestroyBrowsingSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DestroyBrowsingSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::DestroyBrowsingSessionL, 0x%d",
        aMessage.Int0() );
    
    // Find and delete a rendering session
    TInt id = aMessage.Int0();
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            delete iBrowsingSessions[ i ];
            iBrowsingSessions.Remove( i );
            found = ETrue;
            i = count;
            }
        }
    if( found )
        {
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetBrowseResponseSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetBrowseResponseSizeL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetBrowseResponseSizeL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetBrowseResponseSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetBrowseResponseSizeL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelGetBrowseResponseSizeL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetBrowseResponseL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetBrowseResponseL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetBrowseResponseL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetSearchResponseSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetSearchResponseSizeL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetSearchResponseSizeL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetSearchResponseSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetSearchResponseSizeL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelGetSearchResponseSizeL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetSearchResponseL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetSearchResponseL(
    const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetSearchResponseL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }   
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetSearchCapabitiesSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetSearchCapabitiesSizeL(
    const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetSearchCapabitiesSizeL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }       
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelGetSearchCapabitiesSizeL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelGetSearchCapabitiesSizeL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelGetSearchCapabitiesSizeL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::GetSearchCapabitiesL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::GetSearchCapabitiesL( const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->GetSearchCapabitiesL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }           
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CreateContainerL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CreateContainerL( const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CreateContainerL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }               
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelCreateContainerL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelCreateContainerL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelCreateContainerL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }            
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DeleteObjectL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DeleteObjectL( const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->DeleteObjectL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                   
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelDeleteObjectL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelDeleteObjectL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelDeleteObjectL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DeviceDisappearedRequestL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DeviceDisappearedRequestL(
    const RMessage2& aMessage )
    {
     TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->DeviceDisappearedRequestL( aMessage );
            found = ETrue;
            i = count;
            }
        }
        
    if( !found )
        {
        count = iPlaybackSessions.Count();
        for( i = 0; i < count; i++ )
            {
            if( iPlaybackSessions[ i ]->SessionId() == id )
                {
                iPlaybackSessions[ i ]->DeviceDisappearedRequestL(
                    aMessage );
                found = ETrue;
                i = count;
                }
            }
        }                       

    if( !found )
        {  
        aMessage.Complete( KErrNotFound );
        }                       
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelDeviceDisappearedRequestL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelDeviceDisappearedRequestL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iBrowsingSessions.Count();
    TBool found = EFalse;
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( iBrowsingSessions[ i ]->SessionId() == id )
            {
            iBrowsingSessions[ i ]->CancelDeviceDisappearedRequestL();
            found = ETrue;
            i = count;
            aMessage.Complete( KErrNone );
            }
        }
    if( !found )
        {
        count = iPlaybackSessions.Count();
        for( i = 0; i < count; i++ )
            {
            if( iPlaybackSessions[ i ]->SessionId() == id )
                {
                iPlaybackSessions[ i ]->CancelDeviceDisappearedRequestL();
                found = ETrue;
                i = count;
                aMessage.Complete( KErrNone );
                }
            }        
        }                    

    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::MonitorConnectionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::MonitorConnectionL( const RMessage2& aMessage )
    {
    
    if( iConnectionMsg )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        iConnectionMsg = new (ELeave) RMessage2( aMessage );
        }  
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CancelMonitorConnectionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CancelMonitorConnectionL(
    const RMessage2& aMessage )
    {
    
    if( iConnectionMsg )
        {
        iConnectionMsg->Complete( KErrCancel );
        delete iConnectionMsg; iConnectionMsg = NULL;
        }
    aMessage.Complete( KErrNone );       
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::CreateDownloadSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CreateDownloadSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::CreateDownloadSessionL, 0x%d",
        aMessage.Int0() );
    
    // Create a downloading session
    TInt id = aMessage.Int0();
    
    // Read the device UID
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr ) );

    CUPnPDownloadSession* tempPtr = CUPnPDownloadSession::NewL(
        iServer, id, *buf );
    CleanupStack::PopAndDestroy( buf );
    iDownloadSessions.AppendL( tempPtr );
    
    aMessage.Complete( KErrNone );    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DestroyDownloadSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DestroyDownloadSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::DestroyDownloadSessionL, 0x%d",
        aMessage.Int0() );
    
    // Find and delete a downloading session
    TInt id = aMessage.Int0();
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            delete iDownloadSessions[ i ];
            iDownloadSessions.Remove( i );
            found = ETrue;
            i = count;
            }
        }
    if( found )
        {
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }        
    }

void CUPnPAVControllerImpl::StartDownloadL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->StartDownloadL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                       
    }

void CUPnPAVControllerImpl::StartDownloadFHL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->StartDownloadFHL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                       
    }
    
void CUPnPAVControllerImpl::CancelDownloadL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->CancelDownloadL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                           
    }

void CUPnPAVControllerImpl::CancelAllDownloadsL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->CancelAllDownloadsL( aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                           
    }
    
void CUPnPAVControllerImpl::StartTrackingDownloadProgressL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->StartTrackingDownloadProgressL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                           
    }

void CUPnPAVControllerImpl::StopTrackingDownloadProgressL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->StopTrackingDownloadProgressL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                           
    }

void CUPnPAVControllerImpl::GetDownloadEventL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->GetDownloadEventL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                               
    }
    
void CUPnPAVControllerImpl::CancelGetDownloadEventL(
    const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iDownloadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iDownloadSessions[ i ]->SessionId() == id )
            {
            iDownloadSessions[ i ]->CancelGetDownloadEventL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DestroyUploadSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::CreateUploadSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::CreateUploadSessionL, 0x%d",
        aMessage.Int0() );
    
    // Create a downloading session
    TInt id = aMessage.Int0();
    
    // Read the device UID
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( 1, ptr ) );

    CUPnPUploadSession* tempPtr = CUPnPUploadSession::NewL(
        iServer, id, *buf );
    CleanupStack::PopAndDestroy( buf );
    iUploadSessions.AppendL( tempPtr );
    
    aMessage.Complete( KErrNone );        
    }

// --------------------------------------------------------------------------
// CUPnPAVControllerImpl::DestroyUploadSessionL
// See upnpavcontrollerimpl.h
// --------------------------------------------------------------------------
void CUPnPAVControllerImpl::DestroyUploadSessionL(
    const RMessage2& aMessage )
    {
    __LOG1( "CUPnPAVControllerImpl::DestroyUploadSessionL, 0x%d",
        aMessage.Int0() );
    
    // Find and delete an uploading session
    TInt id = aMessage.Int0();
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            delete iUploadSessions[ i ];
            iUploadSessions.Remove( i );
            found = ETrue;
            i = count;
            }
        }
    if( found )
        {
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }        
    }

void CUPnPAVControllerImpl::StartUploadL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->StartUploadL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }        
    }
    
void CUPnPAVControllerImpl::CancelUploadL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->CancelUploadL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }            
    }

void CUPnPAVControllerImpl::CancelAllUploadsL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->CancelAllUploadsL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                
    }
    
void CUPnPAVControllerImpl::StartTrackingUploadProgressL( const RMessage2&
    aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->StartTrackingUploadProgressL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                    
    }

void CUPnPAVControllerImpl::StopTrackingUploadProgressL( const RMessage2&
    aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->StopTrackingUploadProgressL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                        
    }

void CUPnPAVControllerImpl::GetUploadEventL( const RMessage2& aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->GetUploadEventL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                            
    }
    
void CUPnPAVControllerImpl::CancelGetUploadEventL( const RMessage2&
    aMessage )
    {
    TInt id = aMessage.Int0();
    
    // Find the session
    TInt count = iUploadSessions.Count();
    TBool found = EFalse;
    for( TInt i = 0; i < count; i++ )
        {
        if( iUploadSessions[ i ]->SessionId() == id )
            {
            iUploadSessions[ i ]->CancelGetUploadEventL(
                aMessage );
            found = ETrue;
            i = count;
            }
        }
    if( !found )
        {
        aMessage.Complete( KErrNotFound );
        }                                
    }

// end of file
