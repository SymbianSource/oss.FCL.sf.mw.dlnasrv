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
* Description:      server impl. of session against media server
*
*/






// INCLUDE FILES
// System
#include <mmf/common/mmfcontrollerpluginresolver.h>

// upnp stack api
#include <upnpitem.h>
#include <upnpattribute.h>
#include <upnpstring.h>
#include <upnpdlnaprotocolinfo.h>
#include <upnpavcontrolpoint.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpitemutility.h" // FindElementByNameL, GetResElements

// upnpframework / xmlparser api
#include "upnpxmlparser.h"

// upnpframework / internal api's
#include "upnpcdsreselementutility.h"
#include "httpuploader.h"
#include "upnpmetadatafetcher.h"
#include "upnpcommonutils.h"

// INTERNAL INCLUDES
#include "upnpuploadsession.h"
#include "upnpfiletransferitem.h"
#include "upnpavcontrollerserver.h"
#include "upnpfiletransferitem.h"
#include "upnpavdeviceextended.h"
#include "upnpavdispatcher.h"
#include "upnpaverrorhandler.h"
#include "upnpresourcehelper.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// CONSTANTS
_LIT8( KImportUri,              "importUri" );
_LIT8( KAsterisk,               "*" );
const TUint32 KBufferSize       = 0x40000; // 256K
const TInt KParallerTransfers   = 1;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPUploadSession::NewL
// See upnpuploadsession.h
// --------------------------------------------------------------------------
CUPnPUploadSession* CUPnPUploadSession::NewL( CUpnpAVControllerServer&
    aServer, TInt aSessionId, const TDesC8& aUuid )
    {
    CUPnPUploadSession* self = new (ELeave) CUPnPUploadSession(
        aServer, aSessionId );
    CleanupStack::PushL( self );    
    self->ConstructL( aUuid );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::CUPnPUploadSession
// See upnpuploadsession.h
// --------------------------------------------------------------------------
CUPnPUploadSession::CUPnPUploadSession( CUpnpAVControllerServer& aServer,
    TInt aSessionId ) :
    CUPnPFileTransferSessionBase( aServer, aSessionId ),
    iResourceIndex( KErrNotFound ),
    iAsyncError( KErrNone ),
    iIPSessionId( KErrNotFound ),
    iSchedulerStopped( ETrue )
    {
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::~CUPnPUploadSession
// See upnpuploadsession.h
// --------------------------------------------------------------------------
CUPnPUploadSession::~CUPnPUploadSession()
    {
    __LOG( "CUPnPUploadSession::~CUPnPUploadSession" );

    delete iUploader;
    iResources.ResetAndDestroy();
    
    __LOG( "CUPnPUploadSession::~CUPnPUploadSession - end" );
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::ConstructL
// See upnpuploadsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPUploadSession::ConstructL" );
    
    CUPnPFileTransferSessionBase::ConstructL( aUuid );

    iUploader = CHttpUploader::NewL( *this, (TUint32)iServer.IAP(),
        KBufferSize, KParallerTransfers );
    }

// --------------------------------------------------------------------------
// CUPnPDownloadSession::TransferProgress
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::TransferProgress( TAny* aKey, TInt aBytes,
    TInt aTotalBytes )
    {
    __LOG( "CUPnPUploadSession::TransferProgress" );
    
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
// CUPnPUploadSession::ReadyForTransfer
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::ReadyForTransferL( TAny* aKey )
    {
    __LOG( "CUPnPUploadSession::ReadyForTransferL" );
    
    TUpnpFileTransferEvent event;
    if( FindItemByKey( event, (TInt)aKey, iResourceIndex ) == KErrNone )
        {
        ReadyForTransferL( event );
        
        event.iEvent = TUpnpFileTransferEvent::ETransferStarted;
        Complete( event );
        }
    else
        {
        __LOG( "ReadyForTransfer - Key not found" );
        
        // Not found
        event.iStatus = KErrGeneral;
        event.iParam2 = 0;
        event.iEvent = TUpnpFileTransferEvent::ETransferCompleted;
        
        Complete( event );
        }
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::TransferCompleted
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::TransferCompleted( TAny* aKey, TInt aStatus )
    {
    __LOG2( "CUPnPUploadSession::TransferCompleted, key = %d, status = %d",
            (TInt)aKey, aStatus );
    
    TUpnpFileTransferEvent item;
    TInt index;
    TInt err = FindItemByKey( item, (TInt)aKey, index );
    if( err == KErrNone )
        {
        item.iStatus = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aStatus,
        EUPnPHTTPError );
        
        item.iEvent = TUpnpFileTransferEvent::ETransferCompleted;
        Complete( item );
                
        iTransferItems.Remove( index );
        delete iResources[ index ];
        iResources.Remove( index );
        }
    else
        {
        __LOG( "TransferCompleted - Key not found" );
        
        // Not found
        if( aStatus == KErrCancel )
            {
            item.iStatus = aStatus;
            }
        else
            {
            item.iStatus = KErrGeneral;    
            }    
        
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
            iUploader->CancelTransfer( (TAny*)iTransferItems[i].iKey );

            if( iResources[ i ]->ItemId() )
                {
                // Send destroyobject
                __LOG( "CancelAllUploadsL - sending destroyobject" );
                TRAP_IGNORE( iServer.ControlPoint().CdsDestroyObjectActionL(
                    iDevice->Uuid(), *iResources[ i ]->ItemId() ) );  
                }
            }
        iResources.ResetAndDestroy();    
        iTransferItems.Reset();
        }      
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::CdsCreateObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::CdsCreateObjectResponse(
    const TDesC8& /*aUuid*/,
    TInt aSessionId,
    TInt aErr,
    const TDesC8& /*aContainerID*/, 
    const TDesC8& /*aElements*/, 
    const TDesC8& aObjectID, 
    const TDesC8& aResult )
    {
    __LOG1( "CUPnPUploadSession::CdsCreateObjectResponse: %d" , aErr );
    
    __ASSERTD( iIPSessionId == aSessionId, __FILE__, __LINE__ );
    
    __ASSERTD( iTransferItems.Count() == iResources.Count(), __FILE__, 
               __LINE__ );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPContentDirectoryError );

    if( aErr == KErrNone )
        {
        TRAP( aErr, HandleObjectResponseL( aObjectID, aResult ) );
        }        
    
    iAsyncError = aErr;
    StopWait();
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::StartUploadL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::StartUploadL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPUploadSession::StartUploadL" );
    
    __ASSERTD( iTransferItems.Count() == iResources.Count(), __FILE__, 
               __LINE__ );
    
    CUpnpFileTransferItem* tmpTransferItem = CUpnpFileTransferItem::NewLC();
        
    ReadTransferItemFromMessageL( aMessage, 1 ,tmpTransferItem );
        
    if( CheckIfKeyExists( tmpTransferItem->Key() ) )
        {
        User::Leave( KErrInUse );
        }
    
    TUpnpFileTransferEvent transferItem;        
    transferItem.iKey = tmpTransferItem->Key();
            
    CUpnpItem* tmpUpnpItem = UPnPMetadataFetcher::CreateItemFromFileLC( 
        tmpTransferItem->Path() );
        
    tmpUpnpItem->SetIdL( KNullDesC8 );
    tmpUpnpItem->SetParentIdL( KContainerIdAny );
    tmpUpnpItem->SetRestricted( EFalse );

    const CUpnpElement& resEl = UPnPItemUtility::FindElementByNameL(
        *tmpUpnpItem, KElementRes );

    const CUpnpAttribute* pInfo = UPnPItemUtility::FindAttributeByName(
        resEl, KAttributeProtocolInfo );  
      
    if( iDevice->DlnaCompatible() )
        {
        // Dlna device, additional checks required
        __LOG( "StartUploadL - DLNA device" );
        
        TPtrC8 objectClass = tmpUpnpItem->ObjectClass(); 
        if( objectClass.Find( KClassAudio ) != KErrNotFound &&
            iDevice->AudioUpload() )
            {
            // Trying to upload audio and it's supported
            }
        else if( objectClass.Find( KClassImage ) != KErrNotFound &&
            iDevice->ImageUpload() )
            {
            // Trying to upload images and it's supported
            }
        else if(  objectClass.Find( KClassVideo ) != KErrNotFound &&
            iDevice->VideoUpload() )
            {
            // Trying to upload video and it's supported
            }
        else
            {
            // Unknown media type, or copy not supported
            __LOG( "StartUploadL - Unknown media type, or copy not \
supported" );
            User::Leave( KErrNotSupported );
            }    

            if( iDevice->ValidateTransfer( pInfo->Value() ) )
                {
                __LOG( "StartUploadL - Match by protocolInfo succeeded" );
                // Continue with DLNA upload
                // ProtocolInfo is in DLNA format
                }
            else
                {
                __LOG( "StartUploadL - Match by protocolInfo failed, upload \
as UPnP content" );
                // Continue with UPnP upload
                // Check the protocolInfo
                ProtocolInfoToUPnPL( pInfo );
                }        
        }
    else
        {
        __LOG( "StartUploadL - UPnP Device -> UPnP upload" );
        // Continue with UPnP upload
        // Check the protocolInfo
        ProtocolInfoToUPnPL( pInfo );        
        }
        
    iUploader->InsertFileIntoWaitQueueL( (TAny*)transferItem.iKey,
        tmpTransferItem->Path(), KNullDesC8 );
    
    SetHeadersL( pInfo->Value(), (TAny*)transferItem.iKey );
    
    iTransferItems.Append( transferItem );
    
    CUPnPResourceHelper* helper = CUPnPResourceHelper::NewL();
    CleanupStack::Pop( tmpUpnpItem );
    CleanupStack::PushL( helper );
    helper->SetItem( tmpUpnpItem ); // Transfer ownership
    iResources.AppendL( helper );
    
    CleanupStack::Pop( helper );
    
    iUploader->MoveToTransferQueueL( (TAny*)transferItem.iKey );
    
    CleanupStack::PopAndDestroy( tmpTransferItem );
    
    aMessage.Complete( KErrNone );
    
    __LOG( "CUPnPUploadSession::StartUploadL - end " );    
    }
    
// --------------------------------------------------------------------------
// CUPnPUploadSession::CancelUploadL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::CancelUploadL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPUploadSession::CancelUploadL" );          
    
    __ASSERTD( iTransferItems.Count() == iResources.Count(), __FILE__,
               __LINE__ );
    
    TInt key = aMessage.Int1();

    TInt index;
    if( CheckIfKeyExists( key, index ) )
        {
        iUploader->CancelTransfer( (TAny*)key );
        aMessage.Complete( KErrNone );
        iTransferItems.Remove( index );
        if( iResources[ index ]->ItemId() )
            {
            // Send destroyobject
            iServer.ControlPoint().CdsDestroyObjectActionL( iDevice->Uuid(),
                *iResources[ index ]->ItemId() );
            }
        delete iResources[index ];
        iResources.Remove( index );   
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }        
    
    __LOG( "CUPnPUploadSession::CancelUploadL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::CancelAllUploadsL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::CancelAllUploadsL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPUploadSession::CancelAllUploadsL" );          
    
    __ASSERTD( iTransferItems.Count() == iResources.Count(), __FILE__,
               __LINE__ );
        
    TInt count = iTransferItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iUploader->CancelTransfer( (TAny*)iTransferItems[i].iKey );

        if( iResources[ i ]->ItemId() )
            {
            // Send destroyobject
            __LOG( "CancelAllUploadsL - sending destroyobject" );
            iServer.ControlPoint().CdsDestroyObjectActionL( iDevice->Uuid(),
                *iResources[ i ]->ItemId() );  
            }
        }
    iResources.ResetAndDestroy();
    iTransferItems.Reset();
    iEventQueu.Reset();

    // Scheduler loop started, stop it and ignore createobject-resp
    __LOG( "CancelAllUploadsL - Un register" );
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    iAsyncError = KErrCancel;

    __LOG( "CancelAllUploadsL - Stop scheduler loop" );
    StopWait();

    aMessage.Complete( KErrNone );

    __LOG( "CUPnPUploadSession::CancelAllUploadsL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPUploadSession::StartTrackingUploadProgressL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::StartTrackingUploadProgressL( const RMessage2&
    aMessage )
    {
    __LOG( "CUPnPUploadSession::StartTrackingUploadProgressL" );
    
    TInt key = aMessage.Int1();
    if( CheckIfKeyExists( key ) )
        {
        iUploader->TrackProgress( (TAny*)key, ETrue );
        iProgressTrackingEnabled = ETrue;
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::StopTrackingUploadProgressL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::StopTrackingUploadProgressL( const RMessage2&
    aMessage )
    {
    __LOG( "CUPnPUploadSession::StopTrackingUploadProgressL" );
    
    TInt key = aMessage.Int1();
    if( CheckIfKeyExists( key ) )
        {
        iUploader->TrackProgress( (TAny*)key, EFalse );
        iProgressTrackingEnabled = EFalse;        
        aMessage.Complete( KErrNone );
        }
    else
        {
        aMessage.Complete( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::GetUploadEventL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::GetUploadEventL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPUploadSession::GetUploadEventL" );
    
    if( iEventQueu.Count() )
        {
        // There are events in the queu, write the first queued event back
        // to client
        __LOG1( "GetUploadEventL - events in queu, count = %d",
            iEventQueu.Count() );
        TPckg<TUpnpFileTransferEvent> resp1( iEventQueu[0] );
        aMessage.Write( 1, resp1  ); // Write response using slot 1
        aMessage.Complete( KErrNone );
        
        iEventQueu.Remove( 0 );
        }
    else
        {
        __ASSERTD( !iEventMsg, __FILE__, __LINE__ );
        __LOG( "GetUploadEventL - Store msg" );
        iEventMsg = new (ELeave) RMessage2( aMessage );
        } 
    __LOG( "CUPnPUploadSession::GetUploadEventL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPUploadSession::CancelGetUploadEventL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::CancelGetUploadEventL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPUploadSession::CancelGetUploadEventL" );          
    
    if( iEventMsg )
        {
        iEventMsg->Complete( KErrCancel );
        delete iEventMsg; iEventMsg = NULL;
        }
    aMessage.Complete( KErrNone );    
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::DeviceDisappearedL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPUploadSession::DeviceDisappearedL" );

    if( aDevice.Uuid().Compare( iDevice->Uuid() ) == 0 )
        {
        TUpnpFileTransferEvent event;
        event.iEvent = TUpnpFileTransferEvent::EDeviceDisconnected;

        Complete( event );
        }
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::SetHeadersL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::SetHeadersL( const TDesC8& aInfo, TAny* aKey )
    {
    CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL( aInfo );
    CleanupStack::PushL( tmpInfo );

    // Set the content-type header
    iUploader->SetHeaderL( aKey, KContentType,
        tmpInfo->ThirdField() );
    
    // Set the expect header
    iUploader->SetHeaderL( aKey, KExpectHeader, K100Continue );

    CleanupStack::PopAndDestroy( tmpInfo );            
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::ReadyForTransferL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::ReadyForTransferL( TUpnpFileTransferEvent& aEvent )
    {
    __LOG( "CUPnPUploadSession::ReadyForTransferL" );
    
    if ( !iSchedulerStopped || iWait.IsStarted() )
        {
        __LOG1( "CUPnPUploadSession::ReadyForTransferL\
        IsStarted = %d", (TInt)iWait.IsStarted() );
        __LOG1( "CUPnPUploadSession::ReadyForTransferL\
        iSchedulerStopped = %d", (TInt)iSchedulerStopped );
        User::Leave( KErrNotReady );
        }

    __ASSERTD( iTransferItems.Count() == iResources.Count(), __FILE__, 
               __LINE__ );
    
    CUpnpItem* tmpUpnpItem = iResources[ iResourceIndex ]->Item();
    if( !tmpUpnpItem )
        {
        User::Leave( KErrCorrupt );
        }
    
    HBufC8* xmlDoc = CUPnPXMLParser::XmlForCreateObjectLC(
        *tmpUpnpItem );
    
    iIPSessionId = iServer.ControlPoint().CdsCreateObjectActionL(
        iDevice->Uuid(), KContainerIdAny, *xmlDoc );

    if( iIPSessionId > 0 )
        {
        // Register
        iServer.Dispatcher().RegisterL( iIPSessionId, *this );
        }
    else
        {
        User::Leave( iIPSessionId );
        }                             
    
    CleanupStack::PopAndDestroy( xmlDoc );
    
    iWait.Start();
    
    __LOG1( "CUPnPUploadSession::ReadyForTransferL\
        iAsyncError = %d", iAsyncError );

    User::LeaveIfError( iAsyncError );

    const CUpnpElement& resEl = UPnPItemUtility::FindElementByNameL(
        *iResources[ iResourceIndex ]->Item(), KElementRes );

    const CUpnpAttribute* pInfo = UPnPItemUtility::FindAttributeByName(
        resEl, KAttributeProtocolInfo );  

    CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
        pInfo->Value() );
    CleanupStack::PushL( tmpInfo );      
    
    // Set DLNA specific headers    
    if( tmpInfo->PnParameter().Length() )
        {
        // DLNA content, set the content-features header
        iUploader->SetHeaderL( (TAny*)aEvent.iKey, KContentFeatures,
            tmpInfo->FourthField() );        

        // Check if background mode is available and use it if it is
        if( tmpInfo->DlnaFlag( UpnpDlnaProtocolInfo::TM_B_FLAG ) )
            {
            __LOG( "ReadyForTransferL - Use background mode" );
            
            iUploader->SetHeaderL( (TAny*)aEvent.iKey, KTransferMode,
                KBackgroundMode );  
            }
        else
            {
            // Background is not available for some reason.
            // Check if media specific transfer mode is available
            if( UPnPCommonUtils::IsImageSupported( tmpInfo->ProtocolInfoL() ) )
                {
                // Uploading image, alternative for background is interactive
                if( tmpInfo->DlnaFlag( UpnpDlnaProtocolInfo::TM_I_FLAG ) )
                    {
                    __LOG( "ReadyForTransferL - Use interactive mode" );
                    iUploader->SetHeaderL( (TAny*)aEvent.iKey, KTransferMode,
                        KInteractiveMode );
                    }
                else
                    {
                    // Target DMS does not support appropriate transfer mode
                    __LOG( "ReadyForTransferL - No appropriate mode" );
                    // Still, give a try and use background
                    iUploader->SetHeaderL( (TAny*)aEvent.iKey, KTransferMode,
                        KBackgroundMode );  
                    }            
                }
            else if( UPnPCommonUtils::IsAudioSupported(
                        tmpInfo->ProtocolInfoL() ) ||
                    UPnPCommonUtils::IsVideoSupported(
                        tmpInfo->ProtocolInfoL() ) )
                {
                // Uploading A/V, alternative for background is streaming
                if( tmpInfo->DlnaFlag( UpnpDlnaProtocolInfo::TM_S_FLAG ) )
                    {
                    __LOG( "ReadyForTransferL - Use streaming mode" );
                    iUploader->SetHeaderL( (TAny*)aEvent.iKey, KTransferMode,
                    KStreamingMode );
                    }
                else
                    {
                    // Target DMS does not support appropriate transfer mode
                    __LOG( "ReadyForTransferL - No appropriate mode" );
                    // Still, give a try and use background
                    iUploader->SetHeaderL( (TAny*)aEvent.iKey, KTransferMode,
                        KBackgroundMode );  
                    }                 
                }
            else
                {
                __LOG( "ReadyForTransferL - Unknown media type" );
                }    
                        
            }            

        }                
    
    CleanupStack::PopAndDestroy( tmpInfo ); 
    
    iUploader->SetPropertyL( (TAny*)aEvent.iKey,
        CHttpTransferBase::ETargetURI,
        *iResources[ iResourceIndex ]->Resource() );
            
    iUploader->StartTransferL( (TAny*)aEvent.iKey );
        
    __LOG( "CUPnPUploadSession::ReadyForTransferL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::ParseCreateObjectResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::HandleObjectResponseL( const TDesC8& aObjectID,
    const TDesC8& aResult )
    {
    __LOG( "CUPnPUploadSession::HandleObjectResponseL" );          
    
    HBufC8* importUri = ParseCreateObjectResponseL( aResult );
    CleanupStack::PushL( importUri );

    HBufC8* tmpUri = UpnpString::EncodeXmlStringL( importUri );
    CleanupStack::PopAndDestroy( importUri );
    CleanupStack::PushL( tmpUri );
        
    HBufC* tmpUri16 = UpnpString::ToUnicodeL( *tmpUri );
    CleanupStack::PopAndDestroy( tmpUri );
          
    iResources[ iResourceIndex ]->SetResource( tmpUri16 );
        
    HBufC8* tmpID = aObjectID.AllocL();
    iResources[ iResourceIndex ]->SetItemId( tmpID );
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::ParseCreateObjectResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPUploadSession::ParseCreateObjectResponseL(
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPUploadSession::ParseCreateObjectResponseL" );          
    
    HBufC8* importURI = NULL;
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    TInt count = array.Count();
    
    // Lets find the first item with an import URI and use that
    for( TInt i = 0; i < count; i++ )
        {
        importURI = ImportURIFromItemL( *array[ i ] );
        if( importURI )
            {
             // Transfer ownership, casting is safe
             // (array[ 0 ]->ObjectType() == EUPnPItem)
            CUpnpItem* item = static_cast<CUpnpItem*>( array[ i ] );
            iResources[ iResourceIndex ]->SetItem( item );
            array.Remove( i );
            i = count;
            }
        }
    if( !importURI ) // no suitable items, leave
        {
        User::Leave( KErrGeneral );
        }
            
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( parser );  
              
    if( !UpnpCdsResElementUtility::IsUriAbsolute( *importURI ) )
        {
        // Import uri is not absolute, upload not supported. leave.
        delete importURI; importURI = NULL;
        User::Leave( KErrGeneral );
        }
      
    return importURI;
    }       

// --------------------------------------------------------------------------
// CUPnPUploadSession::ImportURIFromItemL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPUploadSession::ImportURIFromItemL( const CUpnpObject& aObject )
    {
    HBufC8* importURI = NULL;
    if( aObject.ObjectType() == EUPnPItem )
        {
        // Get the res-elements
        RUPnPElementsArray elArray;
        CleanupClosePushL( elArray );
        UPnPItemUtility::GetResElements( aObject, elArray );
        
        // Find the import uri 
        TInt count = elArray.Count();           
        for( TInt i = 0; i < count; i++ )
            {
            const CUpnpAttribute* attribute = NULL;
            attribute = &UPnPItemUtility::FindAttributeByNameL(
                *elArray[ i ], KImportUri );
            if( attribute )
                {
                // import uri found
                i = count;
                importURI = attribute->Value().AllocL();
                }
            }            
        CleanupStack::PopAndDestroy( &elArray );        
        }
    return importURI;
    }

// --------------------------------------------------------------------------
// CUPnPUploadSession::ProtocolInfoToUPnPL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPUploadSession::ProtocolInfoToUPnPL( const CUpnpAttribute* aInfo )
    {
    __LOG( "CUPnPUploadSession::ProtocolInfoToUPnPL" );
    
    CUpnpDlnaProtocolInfo* destInfo =
        CUpnpDlnaProtocolInfo::NewL( aInfo->Value() );
    CleanupStack::PushL( destInfo );
    
    if( destInfo->PnParameter().Length() )
        {
        __LOG( "ProtocolInfoToUPnPL - Convert to UPnP" );
        
        CUpnpProtocolInfo* targetInfo = CUpnpProtocolInfo::NewL();
        CleanupStack::PushL( targetInfo );
        
        targetInfo->SetFirstFieldL( destInfo->FirstField() );
        targetInfo->SetSecondFieldL( KAsterisk );
        targetInfo->SetThirdFieldL( destInfo->ThirdField() );
        targetInfo->SetFourthFieldL( KAsterisk );
        
        // A bit dirty const cast.. but makes implementation much easier
        CUpnpAttribute* bute = const_cast<CUpnpAttribute*>( aInfo );
        bute->SetValueL( targetInfo->ProtocolInfoL() );
        
        CleanupStack::PopAndDestroy( targetInfo );
        }
    else
        {
        __LOG( "ProtocolInfoToUPnPL - UPnP content -> do nothing" );
        // Not DLNA content, do nothing
        }    
        
    CleanupStack::PopAndDestroy( destInfo );
    }

//---------------------------------------------------------------------------
// CUPnPUploadSession::StopWait()
// See upnpbrowsingsession.h
//---------------------------------------------------------------------------
//
void CUPnPUploadSession::StopWait()
    {
    __LOG( "ProtocolInfoToUPnPL::StopWait" );
    if ( iWait.IsStarted() )
        {
        if ( iWait.CanStopNow() )
            {
            __LOG( "ProtocolInfoToUPnPL::StopWait - stop" );
            iWait.AsyncStop();
            iSchedulerStopped = ETrue;
            }
        else
            {
            __LOG( "CUPnPUploadSession::StopWait\
- not able to stop now, use callback" );
            iSchedulerStopped = EFalse;
            iWait.AsyncStop( TCallBack(
                                SchedulerStoppedCallBack,
                                this ) );
            }
        }
    }

//---------------------------------------------------------------------------
// CUPnPUploadSession::SchedulerStoppedCallBack()
// See upnpbrowsingsession.h
//---------------------------------------------------------------------------
//
TInt CUPnPUploadSession::SchedulerStoppedCallBack( TAny* aPtr )
    {
    CUPnPUploadSession* self = 
        static_cast<CUPnPUploadSession*>( aPtr );
    self->DoSchedulerStoppedCallBack();
    return 0;
    }

//---------------------------------------------------------------------------
// CUPnPUploadSession::DoSchedulerStoppedCallBack()
// See upnpbrowsingsession.h
//---------------------------------------------------------------------------
//
void CUPnPUploadSession::DoSchedulerStoppedCallBack()
    {
    __LOG( "ProtocolInfoToUPnPL::DoSchedulerStoppedCallBack" );
    iSchedulerStopped = ETrue;
    }

// End of file
