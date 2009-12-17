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
* Description:  AV Controller UPnP actions handler
*  Version     : %version: tr1ido#1.1.10 % << Don't touch!
*
*/

#include <utf.h>
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpfileuploadsession.h"

#include "cmsmtransferobserver.h"
#include "cmdmmain.h"
#include "cmstorelistitem.h"
#include "cmsqlmainfactory.h"
#include "mcmsqlmain.h"
#include "cmsmtransferengine.h"
#include "msdebug.h"

// CONSTANTS
const TInt KGranularity = 10;
//If once total size bigger than 30MB may cause timeout when uploading
const TInt KCopySizePerTime = 30 * KMega;
//If once total count more than 20 files may cause timeout when uploading
const TInt KCopyCountPerTime = 20;
    
#ifdef _DEBUG
_LIT( KCmStoreManager, "CmStoreManager" ); 
#endif // _DEBUG

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::NewL
// ---------------------------------------------------------------------------
// 
CCmSmTransferEngine* CCmSmTransferEngine::NewL( 
    MCmSmTransferObserver* aObserver, CCmDmMain* aDBMngr, 
    RPointerArray<CCmStoreListItem>& aItems, RArray<TInt>& aItemIds )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::NewL() start"));    
    CCmSmTransferEngine* self = CCmSmTransferEngine::NewLC( aObserver, 
        aDBMngr, aItems, aItemIds );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::NewL() end"));
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmSmTransferEngine::NewLC
// ---------------------------------------------------------------------------
//    
CCmSmTransferEngine* CCmSmTransferEngine::NewLC( 
    MCmSmTransferObserver* aObserver, CCmDmMain* aDBMngr,
    RPointerArray<CCmStoreListItem>& aItems, RArray<TInt>& aItemIds )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::NewLC() start"));    
    CCmSmTransferEngine* self = new ( ELeave ) CCmSmTransferEngine( aObserver,
        aDBMngr, aItems, aItemIds );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::~CCmSmTransferEngine
// ---------------------------------------------------------------------------
// 
CCmSmTransferEngine::~CCmSmTransferEngine()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::~CCmSmTransferEngine()"));
    CancelCopy();
    if( iFilesToBeRemoved )
        {
        iFilesToBeRemoved->Reset();
        delete iFilesToBeRemoved;
        }
    if( iFilesToBeShrinked )
        {
        iFilesToBeShrinked->Reset();
        delete iFilesToBeShrinked;
        }
    iToBeDeleted.Close();        
    iObserver = NULL;
    iDBManager = NULL;   
    iDbWrapper->Close();
    iFSession.Close();
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CCmSmTransferEngine
// ---------------------------------------------------------------------------
//         
CCmSmTransferEngine::CCmSmTransferEngine( MCmSmTransferObserver* aObserver, 
    CCmDmMain* aDBMngr, RPointerArray<CCmStoreListItem>& aItems, 
    RArray<TInt>& aItemIds )
    : iObserver( aObserver ), iDBManager( aDBMngr ), iDevice( NULL ), 
    iItems( aItems ), iItemIds(aItemIds)
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CCmSmTransferEngine"));    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmTransferEngine::ConstructL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::ConstructL"));    
    
    User::LeaveIfError( iFSession.Connect() );
    iFilesToBeRemoved = new ( ELeave ) CDesCArrayFlat( KGranularity );
    iFilesToBeShrinked = new ( ELeave ) CDesCArrayFlat( KGranularity );
    iDbWrapper = CCmSqlMainFactory::NewCmSqlMainL( iFSession );
    }    

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::SetAvController
// ---------------------------------------------------------------------------
// 
void CCmSmTransferEngine::SetAvController( MUPnPAVController* aAVController )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::SetAvController"));    
    
    iAVController = aAVController;
    iUploadSession = NULL;
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::GetMediaServersL
// ---------------------------------------------------------------------------
//     
void CCmSmTransferEngine::GetMediaServersL( 
    CUpnpAVDeviceList*& aDevices )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::GetMediaServersL()"));
    
    aDevices = iAVController->GetMediaServersL();    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::ResetFileArrays
// ---------------------------------------------------------------------------
// 
void CCmSmTransferEngine::ResetFileArrays()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::ResetFileArrays"));
    
    if( iFilesToBeRemoved )
        {
        iFilesToBeRemoved->Reset();
        }
    if( iFilesToBeShrinked )
        {
        iFilesToBeShrinked->Reset();
        }    
    iToBeDeleted.Reset();
    iToBeDeleted.Close();      
    }
    
// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CancelCopy
// ---------------------------------------------------------------------------
//        
void CCmSmTransferEngine::CancelCopy()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CancelCopy()"));
    
    if ( iAVController )
        {
        iAVController->StopUploadSession( *iUploadSession );
        iUploadSession = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CopyFilesL
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::CopyFilesL( TUint8 aDevId, 
    CUpnpAVDevice* aDevice )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CopyFilesL"));
    
    if( iAVController )
        {
        iAVController->StopUploadSession( *iUploadSession );
        iUploadSession = NULL;
        }
    iDevId = aDevId;    
    iIndex = iItemIds.Count() - 1;    
    iDevice = aDevice;
    ResetFileArrays();   
    iTransferredData = KErrNone; 
	iTotalTransferredData = KErrNone;
	
    if( aDevice )
        {
        ConnectedToDeviceL( *aDevice );
        CopySingleFileL();        
        }
    else
        {
        LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CopyFilesL \
        aDevice == NULL"));
        FinishCopy();
        }        
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::DataAmount
// ---------------------------------------------------------------------------
//
TInt CCmSmTransferEngine::DataAmount() const
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::DataAmount"));
    
    return iTotalTransferredData;
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::FilesToBeRemoved
// ---------------------------------------------------------------------------
//
CDesCArray& CCmSmTransferEngine::FilesToBeRemoved()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::FilesToBeRemoved"));
    
    /** Files to be removed */
    return *iFilesToBeRemoved;
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::DeleteToBeRemoved
// ---------------------------------------------------------------------------
//    
void CCmSmTransferEngine::DeleteToBeRemoved()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::DeleteToBeRemoved"));
    
    iFilesToBeRemoved->Reset();    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::FilesToBeShrinked
// ---------------------------------------------------------------------------
//        
CDesCArray& CCmSmTransferEngine::FilesToBeShrinked()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::FilesToBeShrinked"));
    
    /** Files to be shrinked */
    return *iFilesToBeShrinked;        
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::DeleteToBeShrinked
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::DeleteToBeShrinked()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::DeleteToBeShrinked"));
    
    iFilesToBeShrinked->Reset();    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::ToBeDeleted
// ---------------------------------------------------------------------------
//
RArray<TInt>& CCmSmTransferEngine::ToBeDeleted()
    {
    TRACE(Print(_L("[STORE MNGR] CCmSmTransferEngine::DeleteToBeShrinked \
    To be deleted count = %d"), 
        iToBeDeleted.Count() ));
    return iToBeDeleted;
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::TransferStarted
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CCmSmTransferEngine::TransferStarted( TInt aKey, TInt aStatus )
    {
#else
void CCmSmTransferEngine::TransferStarted( TInt /*aKey*/, TInt /*aStatus*/ )
    {
#endif        
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::TransferStarted()"));
    TRACE(Print( _L("[STORE MNGR] Started transferin file aKey = %D"),aKey));
    TRACE(Print( _L("[STORE MNGR] Started transferin file aStatus = %D"),
        aStatus ));
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::TransferCompleted
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::TransferCompleted( TInt aKey, TInt aStatus,
        const TDesC& /*aFilePath*/ )
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmTransferEngine::TransferCompleted() \
     aStatus %d"), aStatus ));     
    TRACE(Print( _L("[STORE MNGR] Transfered file = %d"), aKey ));    
    
    iObserver->TransferStatus( ECmSmFileTransferred );
    TInt err( KErrNone );
    if( !aStatus )
        {
        TTime time;
        time.HomeTime();
        if( iItems[ iItemIds[ aKey ] ] )
            {
            TRAP( err, AddStoredL( iItems[iItemIds[aKey]]->Path(), 
                iDevice->Uuid(), time ) ); 
            TRACE(Print( _L("[STORE MNGR] AddStoredL err = %d"), err ));                            
            }
        TRAP( err, HandleItemStatusL( aKey ) );
        TRACE(Print( _L("[STORE MNGR] HandleItemStatusL err = %d"), err )); 
        }    
    TInt index( iTransferQueue.FindInOrder( aKey ) );
    if( KErrNotFound != index )
        {
        iTransferQueue.Remove( index );
        if( !iTransferQueue.Count() )
            {
            LOG( _L("[STORE MNGR] Transfer successfully completed"));
            LOG( _L("[STORE MNGR] Queue empty!"));
            iObserver->TransferStatus( ECmSmTransferCompleted );             
            iTransferQueue.Reset();          
            }        
        }
    else
        {
        LOG( _L("[STORE MNGR] Transfer successfully completed"));
        iObserver->TransferStatus( ECmSmTransferCompleted );             
        }
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::TransferProgress
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::TransferProgress( TInt /*aKey*/, TInt aBytes,
        TInt aTotalBytes )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::TransferProgress"));
    
    iTransferredData = ( aBytes / KKilo ); // Convert to kilobytes
    
    // Check if file has been transferred completely
    if ( aBytes == aTotalBytes )
    	{
    	iTotalTransferredData += ( aTotalBytes / KKilo );
    	}
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::MediaServerDisappeared
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::MediaServerDisappeared( 
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    TRACE(Print( _L("[STORE MNGR] CCmSmTransferEngine::\
    MediaServerDisappeared WLANLost") ));
    CancelCopy();        
    iObserver->TransferStatus( ECmSmWlanLost );        
    DeleteToBeRemoved();
    DeleteToBeShrinked();
    iToBeDeleted.Reset();
    iToBeDeleted.Close();  
    }
               
// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CancelOperation
// ---------------------------------------------------------------------------
//    
void CCmSmTransferEngine::CancelOperation()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CancelOperation"));
    
    CancelCopy();
    DeleteToBeRemoved();
    DeleteToBeShrinked();
    iToBeDeleted.Reset();
    iToBeDeleted.Close();    
    iObserver->TransferStatus(ECmSmTransferCanceled);
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CopySingleFileL
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::CopySingleFileL()
    {
    LOG( _L("[STORE MNGR] COTSTransferEngine::CopySingleFileL"));
    
    /** File transfer started */
    iTransferStarted.HomeTime();    
    // Files left in queue
    iTransferQueue.Reset();
    iCopyStatus = ECmSmCopyNever;
    //start uploading
    CopyFilesOnceL();

    if( iItems.Count() == KErrNone )
        {
        iCopyStatus = ECmSmCopySuccess;
        LOG( _L("[STORE MNGR] Transfer successfully completed"));
        __ASSERT_DEBUG(( iIndex + 1 == 0 ),\
            User::Panic( KCmStoreManager, ECmSmTransferQueueMessedUp ));
        FinishCopy();
        }
    if( ECmSmCopyNever == iCopyStatus )
        {
        FinishCopy();
        }
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::ContinueCopyFilesL
// ---------------------------------------------------------------------------
//  
void CCmSmTransferEngine::ContinueCopyFilesL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::ContinueCopyFilesL"));
    
    CopyFilesOnceL();

    if ( iIndex == KErrNotFound && ECmSmCopyContinued == iCopyStatus )
        {
        iCopyStatus = ECmSmCopyNever; 
        }
    if( ECmSmCopyNever == iCopyStatus )
        {
        FinishCopy();    
        }
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::CopyFilesOnceL
// ---------------------------------------------------------------------------
//  
void CCmSmTransferEngine::CopyFilesOnceL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::CopyFilesOnceL"));
    
    TInt copySize( 0 );
    TInt copyCt( 0 );
    if( iItemIds.Count() )
        {
        while( 0 <= iIndex && 
               copySize < KCopySizePerTime &&
               copyCt < KCopyCountPerTime )
            {
            if( iItemIds[iIndex] < iItems.Count() )
                {
                TFileName tempFileName;
                tempFileName.Copy( iItems[iItemIds[iIndex]]->Path() );
                TRACE(Print( _L("[STORE MNGR] Filename S.L 16-bit = %S,%d"), 
                    &tempFileName, iUploadSession ));
                if( !IsDuplicateL( iIndex ) )
                    {
                    TRAPD( err, iUploadSession->StartUploadL( 
                        tempFileName, iIndex ) );
                        
                    if( !err )
                        {
                        iCopyStatus = ECmSmCopySuccess;
                        }
                    
                    TRAP( err, 
                        iUploadSession->StartTrackingProgressL( iIndex ) );                
                    tempFileName.Zero();
                    if( !err )
                        {
                        iTransferQueue.InsertInOrder( iIndex );
                        }
                    copySize += iItems[iItemIds[iIndex]]->Size();
                    copyCt++;
                    }
                else
                    {
                    LOG( _L("[STORE MNGR] File already \
                    on server!Skip file!"));

                    TTime time;
                    time.HomeTime();
                    AddStoredL( iItems[iItemIds[iIndex]]->Path(), 
                                    iDevice->Uuid(), time ); 
                    HandleItemStatusL( iIndex );                      
                    }
                }
            iIndex--;
            if( ( copySize >= KCopySizePerTime ||
                  copyCt >= KCopyCountPerTime ) &&
                  iIndex != KErrNotFound )
                {
                if( ECmSmCopySuccess != iCopyStatus )
                    {
                    iCopyStatus = ECmSmCopyContinued;
                    }
                iObserver->TransferStatus( ECmSmTransferContinued );
                }
            }        
        }
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::FinishCopy
// ---------------------------------------------------------------------------
//  
void CCmSmTransferEngine::FinishCopy()
    {
    LOG( _L("[STORE MNGR] CCmSmTransferEngine::FinishCopy \
    No files to be transfered!"));
    iObserver->TransferStatus( ECmSmTransferCompleted );             
    CancelCopy();
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::ConnectedToDeviceL
// ---------------------------------------------------------------------------
//    
void CCmSmTransferEngine::ConnectedToDeviceL( const CUpnpAVDevice& aDevice )
    {
    LOG( _L("[STORE MNGR] CCmSmTransferEngine::ConnectedToDeviceL"));
    
    CancelCopy();
    iUploadSession = &iAVController->StartUploadSessionL( aDevice );
    iUploadSession->SetObserver( *this );
#ifdef _DEBUG
    CUpnpAVDevice& device = const_cast< CUpnpAVDevice& >( aDevice );                    
    HBufC* data16bitBuf = HBufC::NewLC( device.FriendlyName().Length() );
    TPtr data16bitPtr = data16bitBuf->Des();
    TInt conversionError = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8( data16bitPtr, 
            device.FriendlyName() );
            
    TRACE(Print( _L("[STORE MNGR] conversionError = %d"), conversionError ));            
    TRACE(Print( _L("[STORE MNGR] Target device name: %S"), 
        &data16bitPtr ));
    
    CleanupStack::PopAndDestroy( data16bitBuf );
    
    data16bitBuf = HBufC::NewLC( device.FriendlyName().Length() );
    
    data16bitPtr = data16bitBuf->Des();
    conversionError = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8( data16bitPtr, 
            device.FriendlyName() );
    TRACE(Print( _L("[STORE MNGR] conversionError = %d"), conversionError ));             
    TRACE(Print( _L("[STORE MNGR] Targer device id: %S"), 
        &data16bitPtr ));                
                                                
    CleanupStack::PopAndDestroy( data16bitBuf );    
#endif

    LOG( _L("[STORE MNGR] ****** Connected to target device ******"));
    LOG( _L("[STORE MNGR] CCmSmTransferEngine::ConnectedToDeviceL end"));
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::AddStoredL
// ---------------------------------------------------------------------------
// 
void CCmSmTransferEngine::AddStoredL( const TDesC& aFileName, 
    const TDesC8& aUDN, TTime aTimeStamp )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::AddStoredL"));
    
    iDBManager->AddStoredL( aFileName, aUDN, aTimeStamp );    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::HandleItemStatusL
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::HandleItemStatusL( const TInt aKey )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::HandleItemStatusL"));
    
    RArray<TCmListItemStatus> statusValues; // ownership transferred  
    RArray<TInt> devIds; // ownership transferred

    TInt index( KErrNotFound );
    TCmListItemStatus status( ECmStored );    
    
    if( iItemIds[aKey] < iItems.Count() )
        {
        devIds = iItems[iItemIds[aKey]]->DevIds();    
        statusValues = iItems[iItemIds[aKey]]->StatusValues();
        index = devIds.Find( iDevId );
        }
    
    if( KErrNotFound != index )
        {
        status = statusValues[index];
        }
    TInt countOfStatusVal( KErrNone );
    TRACE(Print( _L("[STORE MNGR] Status = %d"), 
        (TInt)status ));    
    TRACE(Print( _L("[STORE MNGR] statusValues.Count() = %d"), 
        statusValues.Count() ));    
    for( TInt i = 0 ; i < statusValues.Count(); i++ )
        {
        if( statusValues[i] == status )
            {
            countOfStatusVal++;
            }
        }
     
    TRACE(Print( _L("[STORE MNGR] countOfStatusVal = %d"), 
        countOfStatusVal ));
    if( iItemIds[aKey] < iItems.Count() )
        {
        switch( status )
            {
            case ECmToBeRemoved:
                {
                LOG(_L("[STORE MNGR]\t To be removed"));
                if (  2 > countOfStatusVal )
                    {
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    // Let's mark item to be deleted from the db
                    iToBeDeleted.InsertInOrder( iItemIds[aKey] );
                    iFilesToBeRemoved->AppendL( 
                        iItems[iItemIds[aKey]]->Path() );
                    }
                else
                    {
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    }    
                break;
                }
            case ECmToBeShrinked:
                {
                LOG(_L("[STORE MNGR]\t To be shrinked"));
                if ( 2 > countOfStatusVal)
                    {            
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    // Let's mark item to be deleted from the db
                    iToBeDeleted.InsertInOrder( iItemIds[aKey] );
                    iFilesToBeShrinked->AppendL( 
                        iItems[iItemIds[aKey]]->Path() );
                    }
                else
                    {
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    }                
                break;
                }        
            case ECmKeepOnDevice:
                {
                LOG(_L("[STORE MNGR]\t Keep On Device"));
                if( 2 > countOfStatusVal )
                    {
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    // Let's mark item to be deleted from the db
                    iToBeDeleted.InsertInOrder( iItemIds[aKey] );
                    }
                else
                    {
                    // If no action needed just change status
                    iItems[iItemIds[aKey]]->
                        UpdateFileStatusL( iDevId, ECmStored );
                    }    
                break;
                }
            case ECmStored:
                {
                LOG(_L("[STORE MNGR]\t already stored"));
                break;
                }
            default:
                {
                LOG(_L("[STORE MNGR]\t Wrong status!!!"));
                TRACE(Print( _L("[STORE MNGR] Status = %d"), (TInt)status ));
                User::Invariant();
                break;
                }
            } 
        UpdateStoreTransferStatus( ECmStored,
                                   iItems[iItemIds[aKey]]->DbId(),
                                   iDevId );
        }       
    }    

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::UpdateStoreTransferStatus
// ---------------------------------------------------------------------------
//
void CCmSmTransferEngine::UpdateStoreTransferStatus( TUint aStatus, 
    TInt64 aFid, TInt64 aMsId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::UpdateStoreTransferStatus"));
    
    iDBManager->UpdateStoreTransferStatus( aStatus, aFid, aMsId );
    }
    
// ---------------------------------------------------------------------------
// CCmSmTransferEngine::GetArrayIndex
// ---------------------------------------------------------------------------
//
TInt CCmSmTransferEngine::GetArrayIndex( TInt aKey )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::GetArrayIndex"));
    
    TInt index( 0 );
    for( TInt i = 0; i < iItemIds.Count(); i++ )
        {
        if( iItems[iItemIds[i]]->DbId() == aKey )
            {
            index = i;
            // End loop
            i = iItemIds.Count();
            }
        }
    return index;    
    }

// ---------------------------------------------------------------------------
// CCmSmTransferEngine::IsDuplicateL
// ---------------------------------------------------------------------------
// 
TBool CCmSmTransferEngine::IsDuplicateL( const TInt aIndex )
    {
    LOG(_L("[STORE MNGR]\t CCmSmTransferEngine::IsDuplicateL"));
    
    return iDbWrapper->ExistsL( *iItems[iItemIds[aIndex]], iDevId );
    }
    
// End of file


