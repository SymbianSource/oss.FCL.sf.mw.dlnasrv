/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


// INCLUDE FILES
#include "upnptransfercontroller.h"
#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydb.h"
#include "upnpfiletransfertimerobserver.h"
#include "upnpfiletransferevent.h"
#include "upnpcontentdirectoryeventobserver.h"
#include "upnpfiletransfer.h"
#include "upnphttpmessage.h"
#include "upnpautodestroyobjectlist.h"
#include "upnpcdutils.h"
#include "upnptransferinterface.h"
#include "upnpstring.h"

// -----------------------------------------------------------------------------
// CUpnpTransferController::CUpnpTransferController
// -----------------------------------------------------------------------------
//
CUpnpTransferController::CUpnpTransferController( CUpnpContentDirectory* aCd ):
    iContentDirectory(aCd)
    {
    
    }
// -----------------------------------------------------------------------------
// CUpnpTransferController::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpTransferController::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpTransferController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpTransferController* CUpnpTransferController::NewL( CUpnpContentDirectory* aCd )
    {
    CUpnpTransferController* self = 
    new (ELeave) CUpnpTransferController( aCd );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpTransferController destructor
// -----------------------------------------------------------------------------
//
CUpnpTransferController::~CUpnpTransferController()
    {
    iFileTransfers.ResetAndDestroy();
        // clear timers
    iStatusTimers.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpTransferController::HttpResponseReceivedL
// -----------------------------------------------------------------------------
//
void CUpnpTransferController::HttpResponseReceivedL(  TInt aSessionId,
                                                    TInt aStatusCode ) 
    {
    TInt transferId = Transfer( aSessionId);
     
    if ( transferId == KErrNotFound )
        {
        return; 
        }

    if( aStatusCode < 0)     
        {   
        if( iFileTransfers[ transferId ]->TransferState() != ECompleted )
            {
            //stopped transfer
            if(aStatusCode == KErrCancel)
                {   
                 iFileTransfers[ transferId ]->SetTransferState( EStopped);
                }
            //error in transfer
            else
                {
                 iFileTransfers[ transferId ]->SetTransferState( EError); 
                }                
                            
            iContentDirectory->GetAutoDestroyObjectList()->TransferFailL( iFileTransfers[ transferId ]->ImportNumber() );
            iContentDirectory->RemoveFileTransferFromTransferIDsStateVariableL( aSessionId );
            // remove the transfer after 30s
            CUpnpFileTransferTimerObserver* timer = 
                        CUpnpFileTransferTimerObserver::NewLC( 
                        this, aSessionId );
            iStatusTimers.AppendL(timer); 
            CleanupStack::Pop(timer);
            
            // send event to MS
            if( aStatusCode == KErrCancel)
                {
                CUpnpFileTransfer::TTransferType type =
                        iFileTransfers[ transferId ]->TransferType();
                    
                    
                iContentDirectory->NotifyTransferStoppedL( aSessionId, type); 
                }
             else
                {
                //transfer with error
                iContentDirectory->NotifyTransferFailL( aSessionId, aStatusCode );     
                }
            
            }                        
        }   
    else if( aStatusCode == EHttpOk )
        {
        CUpnpFileTransferEvent* transferEvent = iFileTransfers[ transferId ]->Event();
        if( iContentDirectory->IsDrmFileL( transferEvent->FilePath() ))
            {
            iFileTransfers[ transferId ]->SetTransferState(EError);
            iContentDirectory->GetAutoDestroyObjectList()->TransferFailL( iFileTransfers[ transferId ]->ImportNumber() );
            
            // send event to MS - AM -???
            iContentDirectory->NotifyTransferFailL( aSessionId, aStatusCode ); 
            }
        else
            {
            iFileTransfers[ transferId ]->SetTransferState(ECompleted);
            
            // checking if transfer was GET. In that case we have to 
            // create res tag for that file to CDS
            if ( iFileTransfers[ transferId ]->TransferType() != CUpnpFileTransfer::EExport )
                {
                // 16-bit -> 8-bit conversion
                HBufC8* filenameBuffer = UpnpString::FromUnicodeL( transferEvent->FilePath() );
                CleanupStack::PushL( filenameBuffer );               
                TPtr8 buffPtr = filenameBuffer->Des();
            
                TInt ret = iContentDirectory->CreateResTagL( aSessionId, 
                                                            buffPtr , 
                                                            iFileTransfers[ transferId ]->ImportNumber() );
                User::LeaveIfError( ret );                                                                
                CleanupStack::PopAndDestroy(filenameBuffer);               
                iContentDirectory->TransferCompleteL( iFileTransfers[ transferId ]->ImportNumber() ,
                                        iFileTransfers[ transferId ]->ObjectId() );
			    }
            
            //send event to MS
            iContentDirectory->NotifyOutgoingTransferL( aSessionId );
            }
             
       
        // remove the transfer after 30s
        CUpnpFileTransferTimerObserver* timer = 
                        CUpnpFileTransferTimerObserver::NewLC( 
                        this, aSessionId );
        iStatusTimers.AppendL(timer); 
        CleanupStack::Pop(timer);
        
        iContentDirectory->RemoveFileTransferFromTransferIDsStateVariableL( aSessionId );       
        }
     else
        {
        //needed?
        //
        }                        
    }


// -----------------------------------------------------------------------------
// CUpnpTransferController::StopTransferResourceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTransferController::StopTransferResourceL(  TInt aId  )
    {
    TInt transferIndex = Transfer( aId ); 
    if( KErrNotFound == transferIndex )
        {
        User::Leave( ENoFileTransfer );
        }
    
    if(iFileTransfers[ transferIndex ]->TransferState() == ::ETransferInProgress)
        {
        CUpnpFileTransfer::TTransferType type = 
                            iFileTransfers[ transferIndex ]->TransferType();
        if(type == CUpnpFileTransfer::EImport)
            {
            iContentDirectory->TransferHandler()->CancelDownload(aId);    
            }
        else
            {
            iContentDirectory->TransferHandler()->CancelUpload(aId);
            }                    
        }
    return EHttpOk;
    
    }

// -----------------------------------------------------------------------------
// CUpnpTransferController::Transfer
// -----------------------------------------------------------------------------
//
TInt CUpnpTransferController::Transfer( TInt aTransferId )
    {
    for( TInt transferIndex(0); 
        transferIndex < iFileTransfers.Count(); 
        transferIndex++ )
        {
        if( iFileTransfers[transferIndex]->TransferId() == aTransferId )
            {
            return transferIndex;
            }
        }
    return KErrNotFound;
    }
             
// -----------------------------------------------------------------------------
// CUpnpTransferController::IsMaxImportExportTransfers
// -----------------------------------------------------------------------------
//
TBool CUpnpTransferController::IsMaxImportExportTransfers()
    {  
    /*
    * This function limits total number of import/export transfers, according to
    * current number of transfers, which are in progress state and pending import/export transfers
    * Note: Any better algorithm can be used as well
    */
    
    TInt numberOfInProgressTransfers = 0;
    
    for( TInt i = 0; i < iFileTransfers.Count(); i++ )
        {
        if( iFileTransfers[i]->TransferState() == ::ETransferInProgress )
            {
            ++numberOfInProgressTransfers;
            }
        }
       
    // Note: "+ 1" because e.g future 4th transfer discovers only 3 previous transfers, 
    // and if KMaxImportExportTransfers = 3, 4th transfer should be forbidden now.        
    
    return ( ( numberOfInProgressTransfers + 1 ) > KMaxImportExportTransfers );   
    }
    
    
    
// -----------------------------------------------------------------------------
// CUpnpTransferController::TransferInternal
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEvent* CUpnpTransferController::RemoveTransferLC( TInt aTransferId )
    {
    for( TInt i = 0; i < iFileTransfers.Count(); i++ )
        {
        if( iFileTransfers[i]->TransferId() == aTransferId )
            {

            CUpnpFileTransferEvent* event = iFileTransfers[i]->RemoveEvent();
            if(event)
                {
                CleanupStack::PushL( event );
                return event;    
                }
            else
                {
                return NULL;
                }
            
            }
        }
     return NULL;
    }


// -----------------------------------------------------------------------------
// CUpnpTransferController::TransferInternal
// -----------------------------------------------------------------------------
//
CUpnpFileTransferEvent* CUpnpTransferController::TransferL( TInt aTransferId )
    {
    for( TInt i = 0; i < iFileTransfers.Count(); i++ )
        {
        if( iFileTransfers[i]->TransferId() == aTransferId )
            {
            CUpnpFileTransferEvent* event = iFileTransfers[i]->Event(); 
            return event;
            }
        }
     return NULL;
    }


// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpTransferController::TransferEvent(
                                    CUpnpFileTransferTimerObserver* aTimer)

    {
    // remove transfer
    for( TInt i = 0; i < iFileTransfers.Count(); i++ )
        {
        if( iFileTransfers[i]->TransferId() == aTimer->TransferId() )
            {
            delete iFileTransfers[i];
            iFileTransfers.Remove(i);
            break;
            }
        }
    
    // remove timer
    for( TInt i = 0; i < iStatusTimers.Count(); i++ )
        {
        if( iStatusTimers[i] == aTimer )
            {
            delete  iStatusTimers[i];
            iStatusTimers.Remove(i);
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddIncomingTransferInternalL
// -----------------------------------------------------------------------------
//   
void CUpnpTransferController::AddIncomingTransferInternalL( CUpnpHttpMessage* aHttpMsg )
    {    
  
    TInt objId = iContentDirectory->ContentDirectoryDb()->GetObjIdByAttrLikeL(KImportUri8,  aHttpMsg->SenderPathFromHeader());    
  
    CUpnpFileTransferEvent* event = CUpnpFileTransferEvent::NewLC();
    event->SetProgressState( CUpnpFileTransferEvent::ECompleted );        
    event->SetDirection( CUpnpFileTransferEvent::EIncoming );        
    event->SetContentDirectoryObjectId( objId );
    event->SetTransferId( aHttpMsg->SessionId() );
    CleanupStack::Pop( event );         
    TInt transferid = Transfer(aHttpMsg->SessionId());          
    if( transferid == KErrNotFound)
        User::Leave(KErrNotFound);
    iFileTransfers[transferid]->SetEvent(event);
    
  

    }


// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferFailL
// -----------------------------------------------------------------------------
//
void CUpnpTransferController::TransferFailL(CUpnpHttpMessage* aHttpMsg)
    {
    TPtrC8 path( aHttpMsg->DestinationPath() );

    if( path.Length() )
        {
        iContentDirectory->GetAutoDestroyObjectList()->TransferFailL( UpnpCdUtils::ResIdFromUriL( path ) );
        }
    TInt transferId = Transfer( aHttpMsg->SessionId() );
    if ( transferId != KErrNotFound )
         {
         if ( !path.Length() )
             {
             iContentDirectory->GetAutoDestroyObjectList()->TransferFailL( iFileTransfers[ transferId ]->ImportNumber() );    
             }
            
         iContentDirectory->RemoveFileTransferFromTransferIDsStateVariableL( aHttpMsg->SessionId() );
         if(iFileTransfers[ transferId ]->TransferState () ==::ETransferInProgress  )
             {
             iFileTransfers[ transferId ]->SetTransferState( EError );
             }
            
            CUpnpFileTransferTimerObserver* timer = 
                        CUpnpFileTransferTimerObserver::NewLC( 
                                            this, aHttpMsg->SessionId() );
            iStatusTimers.AppendL( timer ); 
            CleanupStack::Pop( timer );
        }
    
    // send event to MS
    NotifyTransferFailL( aHttpMsg );    
    }


// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NotifyTransferFailL
// -----------------------------------------------------------------------------
//
void CUpnpTransferController::NotifyTransferFailL( CUpnpHttpMessage* aHttpMsg )
    {    
    CUpnpFileTransferEvent* event = RemoveTransferLC( aHttpMsg->SessionId() );
    if( event )
        {
        event->SetProgressState( CUpnpFileTransferEvent::EFailed );                   
        event->SetErrorCode( aHttpMsg->Error() );
        CleanupStack::Pop( event);
        iContentDirectory->EventObserver()->FileTransferEvent( event );
        }

    }


// End of File
