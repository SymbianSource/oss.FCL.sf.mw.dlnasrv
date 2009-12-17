/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      server impl. of session against media server
*
*/






// INCLUDE FILES
// System
#include <f32file.h>

// upnp stack api
#include <upnpmediaserversettings.h>
#include <upnpstring.h>
#include <upnpdlnaprotocolinfo.h>
#include <upnpdlnaprotocolinfocons.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff

// upnpframework / internal api's
#include "upnpcommonutils.h"
#include "httpdownloader.h"

// avcontroller internal
#include "upnpdownloadsession.h"
#include "upnpavdeviceextended.h"
#include "upnpfiletransferitem.h"
#include "upnpavcontrollerserver.h"
#include "upnpaverrorhandler.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// CONSTANTS
const TUint32 KBufferSize       = 0x40000; // 256K
const TInt KParallerTransfers   = 1;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPDownloadSession::NewL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
CUPnPDownloadSession* CUPnPDownloadSession::NewL( CUpnpAVControllerServer&
    aServer, TInt aSessionId, const TDesC8& aUuid )
    {
    __LOG( "CUPnPDownloadSession::NewL" );
    
    CUPnPDownloadSession* self = new (ELeave) CUPnPDownloadSession(
        aServer, aSessionId );
    CleanupStack::PushL( self );    
    self->ConstructL( aUuid );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::CUPnPDownloadSession
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
CUPnPDownloadSession::CUPnPDownloadSession( CUpnpAVControllerServer&
    aServer, TInt aSessionId ) :
    CUPnPFileTransferSessionBase( aServer, aSessionId )
    {
    __LOG( "CUPnPDownloadSession::CUPnPDownloadSession" );
    
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::~CUPnPDownloadSession
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
CUPnPDownloadSession::~CUPnPDownloadSession()
    {
    __LOG( "CUPnPDownloadSession::~CUPnPDownloadSession" );
    
    delete iDownloader;
    
    __LOG( "CUPnPDownloadSession::~CUPnPDownloadSession - end" );
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::ConstructL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPDownloadSession::ConstructL" );
    
    CUPnPFileTransferSessionBase::ConstructL( aUuid );

    iDownloader = CHttpDownloader::NewL( *this, (TUint32)iServer.IAP(),
        KBufferSize, KParallerTransfers );
    }
   
// --------------------------------------------------------------------------
// CUPnPDownloadSession::TransferProgress
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::TransferProgress( TAny* aKey, TInt aBytes,
    TInt aTotalBytes )
    {
    __LOG( "CUPnPDownloadSession::TransferProgress" );
    
    if( iProgressTrackingEnabled )
        {
        TUpnpFileTransferEvent item;
        TInt err = FindItemByKey( item, (TInt)aKey );
        if( err == KErrNone )
            {
            item.iStatus = KErrNone;
            item.iParam2 = aTotalBytes;
            item.iParam1 = aBytes;
            item.iEvent = TUpnpFileTransferEvent::ETransferProgress;

            Complete( item );                            
            }
        else
            {
            __LOG( "TransferProgress - Not enabled!" );
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::ReadyForTransfer
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::ReadyForTransferL( TAny* aKey )
    {
    __LOG( "CUPnPDownloadSession::ReadyForTransfer" );

    TUpnpFileTransferEvent item;
    TInt err = FindItemByKey( item, (TInt)aKey );
    if( err == KErrNone )
        {       
        iDownloader->StartTransferL( aKey );        
    
        item.iEvent = TUpnpFileTransferEvent::ETransferStarted;
        Complete( item );
        }
    else
        {
        __LOG( "ReadyForTransfer - key not found" );

        // Not found
        item.iStatus = KErrGeneral;
        item.iParam2 = 0;
        item.iEvent = TUpnpFileTransferEvent::ETransferCompleted;
        
        Complete( item );
        }
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::TransferCompleted
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::TransferCompleted( TAny* aKey, TInt aStatus )
    {
    __LOG( "CUPnPDownloadSession::TransferCompleted" );
    
    TUpnpFileTransferEvent item;
    TInt index;
    TInt err = FindItemByKey( item, (TInt)aKey, index );
    if( err == KErrNone )
        {
        item.iStatus = UPnPAVErrorHandler::ConvertToSymbianErrorCode( 
                aStatus,
                EUPnPHTTPError );
        item.iEvent = TUpnpFileTransferEvent::ETransferCompleted;
        item.iParam3 = 0;

        const HBufC* path = NULL;
        TRAPD( err, path = iDownloader->GetPropertyL( aKey,
            CHttpTransferBase::ETargetPath ) );

        if( iEventMsg )
            {
            if( err == KErrNone && path )
                {
                // Write path using slot 2
                iEventMsg->Write( 2, *path );                            
                }
            else
                {
                __LOG( "Error in path handling" );
                }    
            }
        else
            {
            if( err == KErrNone && path )
                {
                HBufC* buf = HBufC::New( path->Length() );
                if( buf )
                    {
                    buf->Des().Copy( *path );
                    item.iParam3 = (TAny*)buf;
                    }
                else
                    {
                    __LOG( "Error in path handling" );
                    }    
                }
            }    
        Complete( item );
        
        iTransferItems.Remove( index );      
        }
    else
        {
        // Corresponding key was not found!
        __LOG( "TransferCompleted - key not found" );

        // Not found
        item.iStatus = KErrGeneral;
        item.iParam2 = 0;
        item.iEvent = TUpnpFileTransferEvent::ETransferCompleted;
        
        Complete( item );        
        }
        
    if( aStatus == KErrDisconnected )
        {
        __LOG( "TransferCompleted - Target device lost" );
        
        TInt count = iTransferItems.Count();
        for( TInt i = 0; i < count; i++ )
            {
            iDownloader->CancelTransfer( (TAny*)iTransferItems[i].iKey );
            }
        iTransferItems.Reset();
        iEventQueu.Reset();            
        }
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::StartDownloadL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::StartDownloadL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::StartDownloadL" );
    
    CUpnpFileTransferItem* tmpTransferItem = CUpnpFileTransferItem::NewLC();
        
    ReadTransferItemFromMessageL( aMessage, 1 ,tmpTransferItem );
        
    TUpnpFileTransferEvent transferItem;
    
    if( CheckIfKeyExists( tmpTransferItem->Key() ) )
        {
        User::Leave( KErrInUse );
        }
        
    transferItem.iKey = tmpTransferItem->Key();
    
    HBufC* path = HBufC::NewLC( KMaxPath );
    if( tmpTransferItem->Path() == KNullDesC )
        {
        // Read path from the settings
        CUpnpMediaServerSettings* msSettings =
            CUpnpMediaServerSettings::NewL();
        CleanupStack::PushL( msSettings );
               
        HBufC8* buf = msSettings->GetL(
            UpnpMediaServerSettings::EUploadDirectory );
            
        CleanupStack::PopAndDestroy( msSettings );    
        
        path->Des().Copy( *buf );
        delete buf;
        }
    else
        {
        // Get path from the transfer item
        path->Des().Copy( tmpTransferItem->Path() );
        }    
    
    __LOG8( tmpTransferItem->Title() );
    
    HBufC* title16 = UpnpString::ToUnicodeL( tmpTransferItem->Title() );
    CleanupStack::PushL( title16 );
    
    HBufC* title16checked =
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( *title16 );
    CleanupStack::PopAndDestroy( title16 );
    CleanupStack::PushL( title16checked );
    
    CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
        tmpTransferItem->ProtocolInfo() );
    CleanupStack::PushL( tmpInfo );    
    HBufC* fileExt = UPnPCommonUtils::FileExtensionByMimeTypeL(
        tmpInfo->ThirdField() );  
    path->Des().Append( *title16checked  );
    
    if( fileExt )
        {
        path->Des().Append( *fileExt );
        delete fileExt;        
        }

    iDownloader->InsertFileIntoWaitQueueL( (TAny*)transferItem.iKey,
        *path, tmpTransferItem->Uri() );
        
    SetHeadersL( *tmpInfo, (TAny*)transferItem.iKey );    
    CleanupStack::PopAndDestroy( tmpInfo );    
    CleanupStack::PopAndDestroy( title16checked );
    
    iTransferItems.Append( transferItem );
           
    iDownloader->MoveToTransferQueueL( (TAny*)transferItem.iKey );
    
    CleanupStack::PopAndDestroy( path );
    
    CleanupStack::PopAndDestroy( tmpTransferItem );
    
    aMessage.Complete( KErrNone );        
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::StartDownloadFHL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::StartDownloadFHL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::StartDownloadFHL" );

    RFile fHandle;
    // handle is stored in slots 2 and 3
    User::LeaveIfError( fHandle.AdoptFromClient( aMessage, 2, 3 ) );
    CleanupClosePushL( fHandle );
    
    CUpnpFileTransferItem* tmpTransferItem = CUpnpFileTransferItem::NewLC();
        
    ReadTransferItemFromMessageL( aMessage, 1 ,tmpTransferItem );
        
    TUpnpFileTransferEvent transferItem;
    transferItem.iKey = tmpTransferItem->Key();
    
    if( CheckIfKeyExists( tmpTransferItem->Key() ) )
        {
        User::Leave( KErrInUse );
        }
    
    CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
        tmpTransferItem->ProtocolInfo() );
    CleanupStack::PushL( tmpInfo );     
    
    iDownloader->InsertFileIntoWaitQueueL( (TAny*)transferItem.iKey, fHandle,
        tmpTransferItem->Uri() );
    
    SetHeadersL( *tmpInfo, (TAny*)transferItem.iKey );
    
    CleanupStack::PopAndDestroy( tmpInfo );
        
    iTransferItems.Append( transferItem );

    iDownloader->MoveToTransferQueueL( (TAny*)transferItem.iKey );
        
    CleanupStack::PopAndDestroy( tmpTransferItem );
    
    CleanupStack::PopAndDestroy( &fHandle );
    
    aMessage.Complete( KErrNone );  
    }
    
// --------------------------------------------------------------------------
// CUPnPDownloadSession::CancelDownloadL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::CancelDownloadL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::CancelDownloadL" );
    
    TInt key = aMessage.Int1();

    TInt index;
    if( CheckIfKeyExists( key, index ) )
        {
        iDownloader->CancelTransfer( (TAny*)key );
        aMessage.Complete( KErrNone );
        iTransferItems.Remove( index );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::CancelAllDownloadsL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::CancelAllDownloadsL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::CancelAllDownloadsL" );
    
    TInt count = iTransferItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iDownloader->CancelTransfer( (TAny*)iTransferItems[i].iKey );
        }
    iTransferItems.Reset();
    iEventQueu.Reset();    
    aMessage.Complete( KErrNone );
    }
    
// --------------------------------------------------------------------------
// CUPnPDownloadSession::StartTrackingDownloadProgressL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::StartTrackingDownloadProgressL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::StartTrackingDownloadProgressL" );
    
    TInt key = aMessage.Int1();
    if( CheckIfKeyExists( key ) )
        {
        iDownloader->TrackProgress( (TAny*)key, ETrue );
        iProgressTrackingEnabled = ETrue;        
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPDownloadSession::StopTrackingDownloadProgressL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::StopTrackingDownloadProgressL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::StopTrackingDownloadProgressL" );
    
    TInt key = aMessage.Int1();
    if( CheckIfKeyExists( key ) )
        {
        iDownloader->TrackProgress( (TAny*)key, EFalse );
        iProgressTrackingEnabled = EFalse;        
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::GetDownloadEventL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::GetDownloadEventL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::GetDownloadEventL" );
    
    if( iEventQueu.Count() )
        {
        __LOG1( "GetDownloadEventL, events in queu: %d",
            iEventQueu.Count() );
        
        // There are events in the queu, write the first queued event back
        // to client
        TUpnpFileTransferEvent event = iEventQueu[0];
        iEventQueu.Remove( 0 );
        TPckg<TUpnpFileTransferEvent> resp1( event );
        
        if( event.iEvent ==
            TUpnpFileTransferEvent::ETransferCompleted &&
            event.iParam3 )
            {
            // Write path using slot 2
            __LOG16( *( (HBufC*)event.iParam3 ) );
            aMessage.Write( 2, *( (HBufC*)event.iParam3 ) );
            delete (HBufC*)event.iParam3;
            }
        aMessage.Write( 1, resp1  ); // Write event using slot 1    
        aMessage.Complete( KErrNone );       
        }
    else
        {
        iEventMsg = new (ELeave) RMessage2( aMessage );    
        }
    __LOG( "CUPnPDownloadSession::GetDownloadEventL - end" );        
    }
    
// --------------------------------------------------------------------------
// CUPnPDownloadSession::CancelGetDownloadEventL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::CancelGetDownloadEventL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPDownloadSession::CancelGetDownloadEventL" );
    
    if( iEventMsg )
        {
        iEventMsg->Complete( KErrCancel );
        delete iEventMsg; iEventMsg = NULL;
        }
    aMessage.Complete( KErrNone );
    }

void CUPnPDownloadSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPDownloadSession::DeviceDisappearedL" );
    
    if( aDevice.Uuid().Compare( iDevice->Uuid() ) == 0 )
        {
        TUpnpFileTransferEvent event;
        event.iEvent = TUpnpFileTransferEvent::EDeviceDisconnected;
        
        Complete( event );
        }
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::SetHeadersL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPDownloadSession::SetHeadersL( CUpnpDlnaProtocolInfo& aInfo,
    TAny* aKey )
    {
    __LOG( "CUPnPDownloadSession::SetHeadersL" );
    
    // Check supported transfermodes
    // We MUST use background, if it's supported
    if( aInfo.DlnaFlag( UpnpDlnaProtocolInfo::TM_B_FLAG ) )
        {
        iDownloader->SetHeaderL( aKey, KTransferMode, KBackgroundMode );
        }
    else if( aInfo.DlnaFlag( UpnpDlnaProtocolInfo::TM_S_FLAG ) )
        {
        iDownloader->SetHeaderL( aKey, KTransferMode, KStreamingMode );
        }
    else if( aInfo.DlnaFlag( UpnpDlnaProtocolInfo::TM_I_FLAG ) )
        { 
        iDownloader->SetHeaderL( aKey, KTransferMode, KInteractiveMode );
        }      
    else 
        {
        __LOG( "SetHeadersL - No DLNA flags" );
        
        // Transfermode is not defined by any flag
        // Correction for HALA-7B6FA5
        if( UPnPCommonUtils::IsImageSupported( aInfo.ProtocolInfoL() ) )
            {
            __LOG( "SetHeadersL - Image -> Interactive" );
            // It's an image, we should use Interactive-mode
            iDownloader->SetHeaderL( aKey, KTransferMode, KInteractiveMode );
            }
        else if(
            UPnPCommonUtils::IsAudioSupported( aInfo.ProtocolInfoL() ) ||
            UPnPCommonUtils::IsVideoSupported( aInfo.ProtocolInfoL() ) )
            {
            __LOG( "SetHeadersL - A/V -> Streaming" );
            iDownloader->SetHeaderL( aKey, KTransferMode, KStreamingMode );
            }
        else
            {
            __LOG( "SetHeadersL - Unknown type -> No mode" );
            // It' something other
            }                    
        }        
    }
   
// End of file
