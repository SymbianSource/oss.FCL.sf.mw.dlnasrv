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
* Description:      implements a session towards a media server
*
*/






// INCLUDES
// upnp stack api
#include <upnpcontainer.h>
#include <upnpobjectlist.h>
#include <upnpitem.h>
#include <upnpstring.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpavbrowsingsessionobserver.h"

// avcontroller internal
#include "upnpavcontrollerclient.h"
#include "upnpavrequest.h"
#include "upnpavbrowserequest.h"
#include "upnpavdeviceactive.h"
#include "upnpavbrowsingsessionimpl.h"

_LIT( KComponentLogfile, "upnpavcontrollerclient.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::NewL
// Two-phase construction
// --------------------------------------------------------------------------
CUPnPAVBrowsingSessionImpl* CUPnPAVBrowsingSessionImpl::NewL(
    RUPnPAVControllerClient& aServer, const CUpnpAVDevice& aDevice )
    {
    CUPnPAVBrowsingSessionImpl* self = new (ELeave)
        CUPnPAVBrowsingSessionImpl( aServer );
    CleanupStack::PushL( self );
    self->iDevice = CUpnpAVDevice::NewL( aDevice );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CUPnPAVBrowsingSessionImpl
// Constructor
// --------------------------------------------------------------------------
CUPnPAVBrowsingSessionImpl::CUPnPAVBrowsingSessionImpl(
    RUPnPAVControllerClient& aServer ) :
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iBufferPtr( 0, 0 ),
    iBufferPtr2( 0, 0 ),
    iBufferPtr3( 0, 0 ),
    iRespBufSizePkg( iRespBufSize ),
    //iMatchesPkg( iMatches ),
    iRespParamsPkg( iRespParams ),
    iPendingOperation( ENone ),
    iAlive( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::~CUPnPAVBrowsingSessionImpl
// Destructor
// --------------------------------------------------------------------------
CUPnPAVBrowsingSessionImpl::~CUPnPAVBrowsingSessionImpl()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::~CUPnPAVBrowsingSessionImpl" );
    
    delete iBuffer;
    iBuffer = NULL;
    delete iBuffer2;
    iBuffer2 = NULL;
    delete iBuffer3;
    iBuffer3 = NULL;
    
    delete iDeviceActive;
    iDeviceActive = NULL;
    
    //delete iParser;
    delete iDevice;    
    iDevice = NULL;
    
    Cancel();
    if( iMediaServerResourceReserved )
        {
        iServer.ReleaseMediaServer( (TInt)this );
        }    
    iServer.DestroyBrowsingSession( (TInt)this );
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::ConstructL
// Two-phase construction
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::ConstructL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::ConstructL" );
    
    iBuffer = iDevice->Uuid().AllocL();
    iBufferPtr.Set( iBuffer->Des() );
    User::LeaveIfError( iServer.CreateBrowsingSession(
        (TInt)this , iBufferPtr ) );
    //iParser = CUPnPXMLParser::NewL();
    iDeviceActive = CUPnPAVDeviceActive::NewL( iServer, *this );
    iDeviceActive->StartListening( (TInt)this );
    }

void CUPnPAVBrowsingSessionImpl::RunL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::RunL" );
    
    switch( iPendingOperation )
        {
        case EBrowseSize:
            {
            iPendingOperation = ENone;
            BrowseSizeCompletedL();
            }
            break;
            
        case ESearchSize:
            {
            iPendingOperation = ENone;
            SearchSizeCompletedL();
            }
            break;
            
        case EGetSearchCapabilities:
            {
            SearchCapabilitiesCompletedL();
            }
            break;

        case ECreateContainer:
            {
            CreateContainerCompletedL();
            }
            break;

        case EDeleteObject:
            {
            DeleteObjectCompletedL();
            }
            break;

        case EStartMediaServer:
            {
            StartMediaServerCompleteL();
            }
            break;
            
        default:
            __PANICD( __FILE__, __LINE__ );
            break;            
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::DoCancel
// From CActive
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::DoCancel()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::DoCancel" );
    
    switch( iPendingOperation )
        {
        case EBrowseSize:
            {
            iServer.CancelBrowseResponseSize( (TInt)this );
            }
            break;

        case ESearchSize:
            {
            iServer.CancelSearchResponseSize( (TInt)this );
            }
            break;

        case EGetSearchCapabilities:
            {
            iServer.CancelGetSearchCapabilitiesSize( (TInt)this );
            }
            break;

        case ECreateContainer:
            {
            iServer.CancelCreateContainer( (TInt)this );
            }
            break;

        case EDeleteObject:
            {
            iServer.CancelDeleteObject( (TInt)this );
            }
            break;

        case EStartMediaServer:
            {
            iServer.CancelReserveMediaServer( (TInt)this );
            }
            break;          

        default:
            __PANICD( __FILE__, __LINE__ );
            break;            
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::RunError
// From CActive
// --------------------------------------------------------------------------
TInt CUPnPAVBrowsingSessionImpl::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

   
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::SetObserver
// Sets observer
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::SetObserver(
    MUPnPAVBrowsingSessionObserver& aObserver )
    {
    iObserver = &aObserver;
    }
     
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::RemoveObserver
// Removes observer
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::RemoveObserver()
    {
    iObserver = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::Observer
// Returns observer
// --------------------------------------------------------------------------
MUPnPAVBrowsingSessionObserver* CUPnPAVBrowsingSessionImpl::Observer() const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::Device
// Returns used device
// --------------------------------------------------------------------------
const CUpnpAVDevice& CUPnPAVBrowsingSessionImpl::Device() const
    {
    return *iDevice;
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::ReserveLocalMSServicesL
// Reserves local media server
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::ReserveLocalMSServicesL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::ReserveLocalMSServicesL" );
    
    ResetL();
    
    if( iMediaServerResourceReserved )
        {
        if( iObserver )
            {
            iObserver->ReserveLocalMSServicesCompleted( KErrNone );
            }
        }
    else
        {
        iPendingOperation = EStartMediaServer;
        iServer.ReserveMediaServer( (TInt)this, iStatus );
        SetActive();            
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CancelReserveLocalMSServicesL
// Cancel reserve local media server
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::CancelReserveLocalMSServicesL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::CancelReserveLocalMSServicesL" );
    
    if( iPendingOperation == EStartMediaServer )
        {
        Cancel();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::ReleaseLocalMSServicesL
// Release local media server
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::ReleaseLocalMSServicesL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::ReleaseLocalMSServicesL" );
    
    ResetL();
    
    if( iMediaServerResourceReserved )
        {
        iMediaServerResourceReserved = EFalse;
        
        User::LeaveIfError( iServer.ReleaseMediaServer( (TInt)this ) );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::BrowseL
// Browse
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::BrowseL( const TDesC8& aId,
                          const TDesC8& aFilter,
                          TBrowseFlag aBrowseFlag,
                          TInt aStartIndex,
                          TInt aRequestedCount,
                          const TDesC8& aSortCriteria )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::BrowseL" );
    
    ResetL();
    
    CUpnpAVBrowseRequest* request = CUpnpAVBrowseRequest::NewLC();
    request->SetIdL( aId );
    request->SetFilterL( aFilter );
    request->SetBrowseFlag( aBrowseFlag );
    request->SetStartIndex( aStartIndex );
    request->SetRequestedCount( aRequestedCount );
    request->SetSortCriteriaL( aSortCriteria );
    request->SetSearchCriteriaL( KNullDesC8 ); // Not needed in browse
    
    iBuffer = request->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    CleanupStack::PopAndDestroy( request );
    
    iPendingOperation = EBrowseSize;
    
    iServer.BrowseResponseSize( (TInt)this, iBufferPtr,
        iRespParamsPkg, iStatus );
    SetActive();
    }


// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CancelBrowse
// Cancels browse
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::CancelBrowse()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::CancelBrowse" );
    
    if( iPendingOperation == EBrowseSize )
        {
        Cancel();
        }     
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::SearchL
// Search
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::SearchL(   const TDesC8& aId,
                                            const TDesC8& aSearchCriteria,
                                            const TDesC8& aFilter,
                                            TInt aStartIndex,
                                            TInt aRequestedCount,
                                            const TDesC8& aSortCriteria )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::SearchL" );
    
    ResetL();
    
    CUpnpAVBrowseRequest* request = CUpnpAVBrowseRequest::NewLC();
    request->SetIdL( aId );
    request->SetSearchCriteriaL( aSearchCriteria );
    request->SetFilterL( aFilter );
    //request->SetBrowseFlag( aBrowseFlag );
    request->SetStartIndex( aStartIndex );
    request->SetRequestedCount( aRequestedCount );
    request->SetSortCriteriaL( aSortCriteria );
    
    
    iBuffer = request->ToDes8L();
    iBufferPtr.Set( iBuffer->Des() );
    
    CleanupStack::PopAndDestroy( request );
    
    iPendingOperation = ESearchSize;
    
    iServer.SearchResponseSize( (TInt)this, iBufferPtr,
        iRespParamsPkg, iStatus );
    SetActive();    
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CancelSearch
// Cancel search
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::CancelSearch()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::CancelSearch" );
    
    if( iPendingOperation == ESearchSize )
        {
        Cancel();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::GetSearchCapabilitiesL
// Get search capabilities
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::GetSearchCapabilitiesL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::GetSearchCapabilitiesL" );
    
    ResetL();
    
    iPendingOperation = EGetSearchCapabilities;
    iServer.GetSearchCapabilitiesSize( (TInt)this,
        iRespBufSizePkg, iStatus );
    SetActive();
    }                
    
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CreateContainerL
// Create a container
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::CreateContainerL( 
                                            const TDesC8& aTitle,
                                            const TDesC8& aParentId,
                                            TContainerType aContainerType )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::CreateContainerL" );
    
    ResetL();

    iBuffer = aTitle.AllocL();
    iBufferPtr.Set( iBuffer->Des() );
    
    iBuffer2 = aParentId.AllocL();
    iBufferPtr2.Set( iBuffer2->Des() );
    
    iServer.CreateContainer( (TInt)this, 
                              iBufferPtr,
                              iBufferPtr2,
                              (TInt)aContainerType,
                              iStatus );
    iPendingOperation = ECreateContainer;
    SetActive();
    }
 
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::UPnPDeviceDiscovered
// Device discovered
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::UPnPDeviceDiscovered(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    // No implementation needed
    }

  
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::UPnPDeviceDisappeared
// Device disappeared
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::UPnPDeviceDisappeared(
    const CUpnpAVDevice& /*aDevice*/ )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::UPnPDeviceDisappeared" );
    
    iAlive = EFalse;
    if( iObserver )
        {
        iObserver->MediaServerDisappeared( 
            MUPnPAVSessionObserverBase::EDisconnected );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::WLANConnectionLost
// Connection lost
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::WLANConnectionLost()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::WLANConnectionLost" );
    
    iAlive = EFalse;
    if( iObserver )    
        {
        iObserver->MediaServerDisappeared( 
            MUPnPAVSessionObserverBase::EWLANLost );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::DeleteObjectL
// Delete object
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::DeleteObjectL( const TDesC8& aId )
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::DeleteObjectL" );
    
    ResetL();

    iBuffer = aId.AllocL();
    iBufferPtr.Set( iBuffer->Des() );    

    iServer.DeleteObject( (TInt)this, iBufferPtr, iStatus );
    iPendingOperation = EDeleteObject;
    SetActive();   
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::BrowseSizeCompletedL
// Handle browse
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::BrowseSizeCompletedL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::BrowseSizeCompletedL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerGetBrowseResponseSizeCompleted )
            {
            ResetL();
            if( iRespParams.iResponseSize > 0 )
                {
                iBuffer = HBufC8::NewL( iRespParams.iResponseSize );
                iBufferPtr.Set( iBuffer->Des() );
                iBuffer2 = HBufC8::NewL( iRespParams.iUpdateIdSize );
                iBufferPtr2.Set( iBuffer2->Des() );

                TInt err = iServer.BrowseResponse( TInt(this), iBufferPtr,
                    iBufferPtr2 );
                if( err == EAVControllerGetBrowseResponseCompleted )
                    {
                    iObserver->BrowseResponse( *iBuffer, KErrNone,
                        iRespParams.iMatches, iRespParams.iTotalCount,
                        *iBuffer2 );
                    
                    }
                else
                    {
                    iObserver->BrowseResponse( KNullDesC8, err, 0,
                        0, KNullDesC8 );
                    }

                }
            else
                {
                iObserver->BrowseResponse( KNullDesC8, KErrGeneral, 0,
                    0, KNullDesC8 );
                }    
            }
        else
            {
            iObserver->BrowseResponse( KNullDesC8, iStatus.Int(), 0,
                0, KNullDesC8 );

            }
        }           
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::SearchSizeCompletedL
// Handle search
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::SearchSizeCompletedL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::SearchSizeCompletedL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerGetSearchResponseSizeCompleted )
            {
            ResetL();
            if( iRespParams.iResponseSize > 0 )
                {
                iBuffer = HBufC8::NewL( iRespParams.iResponseSize );
                iBufferPtr.Set( iBuffer->Des() );
                iBuffer2 = HBufC8::NewL( iRespParams.iUpdateIdSize );
                iBufferPtr2.Set( iBuffer2->Des() );                
                
                TInt err = iServer.SearchResponse( TInt(this), iBufferPtr,
                    iBufferPtr2 );
                if( err == EAVControllerGetSearchResponseCompleted )
                    {
                    iObserver->SearchResponse( *iBuffer, KErrNone,
                        iRespParams.iMatches, iRespParams.iTotalCount,
                        *iBuffer2 ); 
                    }
                else
                    {
                    iObserver->SearchResponse( KNullDesC8, err, 0,
                        0, KNullDesC8 ); 
                    }                            
                ResetL();
                }
            else
                {
                iObserver->SearchResponse( KNullDesC8, KErrGeneral, 0,
                    0, KNullDesC8 ); 
                }    
            }
        else
            {
            iObserver->SearchResponse( KNullDesC8, iStatus.Int(), 0,
                0, KNullDesC8 ); 
            }
        }            
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::SearchCapabilitiesCompletedL
// Handle search capabilities
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::SearchCapabilitiesCompletedL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::SearchCapabilitiesCompletedL" );
    
    if( iObserver )
        {
        if( iStatus.Int() ==
            EAVControllerGetSearchCapabilitiesSizeCompleted )
            {
            ResetL();
            if( iRespBufSize > 0 )
                {
                iBuffer = HBufC8::NewL( iRespBufSize );
                iBufferPtr.Set( iBuffer->Des() );
                TInt err = iServer.SearchCapabilitiesResponse( TInt(this),
                    iBufferPtr );
                if( err == EAVControllerGetSearchCapabilitiesCompleted )
                    {
                    iObserver->SearchCapabilitiesResponse(
                        KErrNone, *iBuffer );
                    }
                else
                    {
                    iObserver->SearchCapabilitiesResponse(
                        iStatus.Int(), KNullDesC8 );        
                    }                    
                }
            else
                {
                iObserver->SearchCapabilitiesResponse(
                    KErrGeneral, KNullDesC8 );
                }    
            }
        else
            {
 
            iObserver->SearchCapabilitiesResponse(
                iStatus.Int(), KNullDesC8 );   
            }
        }    
    }

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::CreateContainerCompletedL
// Handle CreateContainer
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::CreateContainerCompletedL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::CreateContainerCompletedL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerCreateContainerCompleted )
            {
            HBufC* objectId = HBufC::NewL( iBuffer->Des().Length() );
            objectId->Des().Copy( *iBuffer );
            CleanupStack::PushL( objectId );
            
            HBufC8* objectId8 = UpnpString::FromUnicodeL( *objectId );
            
            CleanupStack::PopAndDestroy( objectId );
            objectId = NULL;
            iObserver->CreateContainerResponse( KErrNone, *objectId8 );
            delete objectId8;
            
            }
        else
            {
            iObserver->CreateContainerResponse( iStatus.Int(), KNullDesC8 );
            } 
        }    
    }
    
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::DeleteObjectCompletedL
// Handle delete object
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::DeleteObjectCompletedL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::DeleteObjectCompletedL" );
    
    if( iObserver )
        {
        if( iStatus.Int() == EAVControllerDeleteObjectCompleted )
            {
            iObserver->DeleteObjectResponse( KErrNone );
            }
        else
            {
            iObserver->DeleteObjectResponse( iStatus.Int() );
            } 
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::ResetL
// Reset
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::ResetL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::ResetL" );
    
    if( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    if( !iAlive )
        {
        User::Leave( KErrDisconnected );
        }
    delete iBuffer; 
    iBuffer = NULL;
    
    delete iBuffer2; 
    iBuffer2 = NULL;
    
    delete iBuffer3; 
    iBuffer3 = NULL;

    iPendingOperation = ENone;
    }    

// --------------------------------------------------------------------------
// CUPnPAVBrowsingSessionImpl::StartMediaServerCompleteL
// Handle start media server
// --------------------------------------------------------------------------
void CUPnPAVBrowsingSessionImpl::StartMediaServerCompleteL()
    {
    __LOG( "CUPnPAVBrowsingSessionImpl::StartMediaServerCompleteL" );
    
    if( iObserver )
        {

        if( iStatus.Int() == EAVControllerStartMediaServerCompleted )
            {
            iMediaServerResourceReserved = ETrue;
            iObserver->ReserveLocalMSServicesCompleted( KErrNone );
            }
        else
            {
            iObserver->ReserveLocalMSServicesCompleted( iStatus.Int() );
            }
        
        }
    }

// end of file
