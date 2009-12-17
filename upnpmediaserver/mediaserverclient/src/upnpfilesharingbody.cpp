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
* Description:  Implementation class for UPnP File Sharing class
*
*/


#include <e32base.h>
#include "upnpcontainer.h"
#include "upnpitem.h"
#include "upnpitemlist.h"
#include "upnpcontainerlist.h"
#include "upnpbrowsecriteria.h"
#include "upnpmediaserverglobals.h"
#include "upnpfilesharingbody.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPMediaServer.txt")
#endif
#include "upnpcustomlog.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpFileSharingBody::CUpnpFileSharingBody()
    : CActive( EPriorityStandard ), iSendBufferPtr(0,0), 
      iReceiveBufferPtr(0,0), iRespBufSizePkg(iRespBufSize)
    {
    }


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iMediaServer.Connect() );
    }


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpFileSharingBody* CUpnpFileSharingBody::NewL()
    {
    CUpnpFileSharingBody* self = new (ELeave) CUpnpFileSharingBody;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUpnpFileSharingBody::~CUpnpFileSharingBody()
    {
    Cancel();
    iMediaServer.Close();
    delete iSendBuffer;
    delete iReceiveBuffer;
    delete iClientExecStatusPkg;
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::LeaveIfBusyL()
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::LeaveIfBusyL()
    {
    // leave if the active object has a request outstanding
    if(IsActive())
        {
        User::Leave( KErrInUse );
        }
    // not active, reset buffers
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::Reset()
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::Reset()
    {
    if(iSendBuffer)
        {
        delete iSendBuffer; 
        iSendBuffer = NULL;
        }
    if(iReceiveBuffer)
        {
        delete iReceiveBuffer;
        iReceiveBuffer = NULL;
        }
     if(iClientExecStatusPkg)
        {
        delete iClientExecStatusPkg;
        iClientExecStatusPkg = NULL;
        }
     iClientStatus = NULL;
     iRespBufSize = 0;
     iPendingAction = ENoAction;
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::UnshareRootL
// ---------------------------------------------------------------------------
//    
void CUpnpFileSharingBody::UnshareRootL(TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EUnshareRoot;

    // issue the request 
    iMediaServer.UnshareRoot( iStatus );
    SetActive();
    }    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::ShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ShareItemL( const TDesC8& aParentId, 
                        CUpnpItem& aItem, TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
    
    // set parent id
    aItem.SetParentIdL(aParentId);
       
    // serialize object
    iSendBuffer = aItem.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    
    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EShareItem;
    iClientItem = &aItem;
    
    // issue the request 
    iMediaServer.ShareItem( iSendBufferPtr,
                            iRespBufSizePkg, 
                            iStatus );
                            
    // set active
    SetActive();
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::ShareContainerL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ShareContainerL( const TDesC8& aParentId, 
                    CUpnpContainer& aContainer, TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
       
    // set parent id
    aContainer.SetParentIdL(aParentId);

    // serialize object
    iSendBuffer = aContainer.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    
    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EShareContainer;
    iClientContainer = &aContainer;
    
    // issue the request 
    iMediaServer.ShareContainer( iSendBufferPtr,
                            iRespBufSizePkg, 
                            iStatus );
                            
    // set active
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::ShareItemListL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ShareItemListL(const TDesC8& aParentId, 
        CUpnpItemList& aItemList, TInt& aExecStatus, TRequestStatus& aStatus)
{
    // check if active and reset variables
    LeaveIfBusyL();
    
    // set parent id
    if( !aItemList.ObjectCount() )
        { // nothing to share
        User::Leave( KErrNotFound );
        }
    aItemList[0]->SetParentIdL(aParentId);

    // serialize object
    iSendBuffer = aItemList.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    iClientExecStatusPkg = new(ELeave)TPckg<TInt>(aExecStatus);

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EShareItemList;
    iClientItemList = &aItemList;
    
    // Issue the request 
    iMediaServer.ShareItemList( iSendBufferPtr,
                            iRespBufSizePkg,
                            iStatus );
    SetActive();
}
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::UnshareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::UnshareItemL( TInt aItemId,
                                TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EUnshareItem;

    // issue the request 
    iMediaServer.UnshareItem( aItemId, iStatus );
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::UnshareContainer
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::UnshareContainerL( TInt aContainerId, 
                                TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EUnshareContainer;

    // issue the request 
    iMediaServer.UnshareContainer( aContainerId, iStatus );
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::UnshareItemListL(CUpnpItemList& aItemList, 
                                TInt& aExecStatus, TRequestStatus& aStatus)
{
    // check if active and reset variables
    LeaveIfBusyL();
    
    // serialize object
    iSendBuffer = aItemList.IdsToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    iClientExecStatusPkg = new(ELeave)TPckg<TInt>(aExecStatus);

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EUnshareItemList;
    iClientItemList = &aItemList;
    
    // Issue the request 
    iMediaServer.UnshareItemListL(  iSendBufferPtr,
                                    *iClientExecStatusPkg,
                                    iStatus );
    SetActive();
}
// ---------------------------------------------------------------------------
// CUpnpFileSharing::GetSharedItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetSharedItemL( TInt aItemId,
                                           CUpnpItem& aItem,
                                           TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
       
    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EGetSharedItem;
    iClientItem = &aItem;
    
    // issue the request 
    iMediaServer.GetSharedItem( aItemId,
                                iRespBufSizePkg, 
                                iStatus );
                            
    // set active
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharing::GetSharedContainerL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetSharedContainerL( TInt aItemId,
                                           CUpnpContainer& aContainer,
                                           TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
       
    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EGetSharedContainer;
    iClientContainer = &aContainer;
    
    // issue the request 
    iMediaServer.GetSharedContainer( aItemId,
                                iRespBufSizePkg, 
                                iStatus );
                            
    // set active
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetSharedItemListL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetSharedItemListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpItemList& aItemList,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
    
    // serialize object
    iSendBuffer = aBrowseCriteria.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    iClientExecStatusPkg = new(ELeave)TPckg<TInt>(aTotalMatches);

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EGetSharedItemList;
    iClientItemList = &aItemList;
    
    // Issue the request 
    iMediaServer.GetSharedItemList( aContainerId, iSendBufferPtr, 
                                    iRespBufSizePkg, iStatus );
    SetActive();
    }    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetSharedContainerListL
// ---------------------------------------------------------------------------
//
 void CUpnpFileSharingBody::GetSharedContainerListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpContainerList& aContainerList,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
    
    // serialize object
    iSendBuffer = aBrowseCriteria.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
    iClientExecStatusPkg = new(ELeave)TPckg<TInt>(aTotalMatches);

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EGetSharedContainerList;
    iClientContainerList = &aContainerList;
    
    // Issue the request 
    iMediaServer.GetSharedContainerList( aContainerId, iSendBufferPtr, 
                                    iRespBufSizePkg, iStatus );
    SetActive();
    }    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::RunL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        HandleError();
        return;
        }
    switch ( iPendingAction )
        {
        case EShareItem:
        case EShareReference:
            {
            GetShareItemResultL();
            break;
            }
        case EShareItemResult:
            {
            FinishShareItemL();
            break;
            }
        case EShareReferenceList:
        case EShareItemList:
            {
            GetShareItemListResultL();
            break;
            }
        case EShareItemListResult:
            {
            FinishShareItemListL();
            break;
            }
        case EShareContainer:
            {
            GetShareContainerResultL();
            break;
            }
        case EShareContainerResult:
            {
            FinishShareContainerL();
            break;
            }
        case EUnshareContainer:
        case EUnshareItem:
            {
            FinishUnshareObject();
            break;
            }
        case EUnshareRoot:
            {
            FinishUnshareRoot();
            break;
            }            
        case EUnshareItemList:
            FinishUnshareItemList();
            break;
        case EGetSharedContainer:
            {
            GetGetSharedContainerResultL();
            break;
            }
        case EGetSharedContainerResult:
            {
            FinishGetSharedContainerL();
            break;
            }
        case EGetSharedItemResult:
            {
            FinishGetSharedItemL();
            break;
            }
        case EGetSharedItem:
            {
            GetGetSharedItemResultL();
            break;
            }
        case EGetSharedItemList:
            {
            GetGetSharedItemListResultL();
            break;
            }
        case EGetSharedItemListResult:
            {
            FinishGetSharedItemListL();
            break;
            }
        case EGetSharedContainerList:
            {
            GetGetSharedContainerListResultL();
            break;
            }
        case EGetSharedContainerListResult:
            {
            FinishGetSharedContainerListL();
            break;
            }
        
        case ENoAction: // Fall through
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::RunError
// ---------------------------------------------------------------------------
//
TInt CUpnpFileSharingBody::RunError( TInt aErr )
    {
    LOGS1("CUpnpFileSharingBody::RunError(%d)", aErr);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::DoCancel
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::DoCancel( )
    {
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::HandleError
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::HandleError()
    {
    User::RequestComplete( iClientStatus, iStatus.Int() );
    Reset();
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishShareItemL()
    {         
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientItem;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetShareItemResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.ShareItemResult(iReceiveBufferPtr, iStatus);
    iPendingAction = EShareItemResult;

    // set active
    SetActive();
    }
           
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishShareContainerL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientContainer;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetShareContainerResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.ShareContainerResult(iReceiveBufferPtr, iStatus);
    iPendingAction = EShareContainerResult;

    // set active
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishUnshareObject
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishUnshareObject()
    {
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishUnshareRoot
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishUnshareRoot()
    {
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    Reset();
    }    
// CUpnpFileSharingBody::FinishUnshareObject
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishUnshareItemList()
    {
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    Reset();
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishShareItemListL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientItemList;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);

    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemListResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetShareItemListResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.ShareItemListResult(iReceiveBufferPtr, *iClientExecStatusPkg, iStatus);
    iPendingAction = EShareItemListResult;

    // set active
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishGetSharedItemListL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishGetSharedItemListL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientItemList;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetGetSharedItemListResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.GetSharedItemListResult( iReceiveBufferPtr, 
                                       *iClientExecStatusPkg, iStatus );
    iPendingAction = EGetSharedItemListResult;

    // set active
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishGetSharedContainerListL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishGetSharedContainerListL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientContainerList;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);

    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareContainerResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetGetSharedContainerListResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.GetSharedContainerListResult( iReceiveBufferPtr, 
                                        *iClientExecStatusPkg, iStatus );
    iPendingAction = EGetSharedContainerListResult;

    // set active
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishGetSharedItemL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientItem;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);

    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetGetSharedItemResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.GetSharedItemResult(iReceiveBufferPtr, iStatus);
    iPendingAction = EGetSharedItemResult;

    // set active
    SetActive();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::FinishShareItemL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::FinishGetSharedContainerL()
    {
    // internalize data
    RDesReadStream stream( iReceiveBufferPtr );
    CleanupClosePushL( stream );
    stream >> *iClientContainer;
    CleanupStack::PopAndDestroy( &stream ); 
 
    // complete client request
    User::RequestComplete(iClientStatus, KErrNone);
    
    // idle state
    Reset();
    }
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::GetShareItemResultL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::GetGetSharedContainerResultL()
    {
    // get result
    iReceiveBuffer = HBufC8::NewL( iRespBufSize );
    iReceiveBufferPtr.Set( iReceiveBuffer->Des() );
    iMediaServer.GetSharedContainerResult(iReceiveBufferPtr, iStatus);
    iPendingAction = EGetSharedContainerResult;

    // set active
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::ShareReferenceL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ShareReferenceL( TInt aContainerId, 
                                    TInt aObjectId,
                                    CUpnpItem& aItem,
                                    TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
    
    // setting 'container id' upon which reference object will be placed
    TBuf8<20> parentIdBuf;
    parentIdBuf.Num( aContainerId );               
    aItem.SetParentIdL( parentIdBuf );    
        
    // setting 'refID' pointing at main object
    TBuf8<20> refIdBuf;
    refIdBuf.Num( aObjectId );               
    aItem.SetRefIdL( refIdBuf );

    // serialize object
    iSendBuffer = aItem.ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());

    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EShareReference;
    iClientItem = &aItem;

    // issue the request 
    iMediaServer.ShareReference( iSendBufferPtr,
                                iRespBufSizePkg, 
                                iStatus );
                            
    // set active
    SetActive();
    }

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::ShareReferenceListL
// ---------------------------------------------------------------------------
//
void CUpnpFileSharingBody::ShareReferenceListL( CUpnpItemList& aItemList,
                                                TInt& aExecStatus,
                                                TRequestStatus& aStatus )
    {
    // check if active and reset variables
    LeaveIfBusyL();
    
    // set parent id
    if( !aItemList.ObjectCount() )
        {    
        // nothing to share
        User::Leave( KErrNotFound );
        }

    // serialize object
    iSendBuffer = aItemList.ToDes8L();
    iSendBufferPtr.Set( iSendBuffer->Des() );
    iClientExecStatusPkg = new( ELeave )TPckg<TInt>( aExecStatus );
    
    // object state
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iPendingAction = EShareReferenceList;
    iClientItemList = &aItemList;
    
    // Issue the request 
    iMediaServer.ShareReferenceList( iSendBufferPtr,
                            iRespBufSizePkg,
                            iStatus );
    SetActive();
    }    

// ---------------------------------------------------------------------------
// CUpnpFileSharingBody::MediaServerClient
// ---------------------------------------------------------------------------
//    
RUpnpMediaServerClient& CUpnpFileSharingBody::MediaServerClient()
    {
    return iMediaServer;
    }

// End of file
