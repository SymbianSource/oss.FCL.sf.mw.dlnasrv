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
* Description:      Resolver for local items
*
*/






// INCLUDE FILES
// upnp stack api's
#include <upnpitem.h>

// upnpframework / avcontroller api
#include "upnpavcontroller.h" // avcontroller service
#include "upnpavbrowsingsession.h" // browsing session
#include "upnpavdevice.h" // device (for creating a session)
#include <upnpfilesharing.h>

// upnpframework / avcontroller helper api
#include "upnpitemresolverfactory.h" // optimisation flags
#include "upnpitemresolverobserver.h" // MUPnPItemResolverObserver
#include "upnpfileutility.h" // IsFileProtected
#include "upnpresourceselector.h" // MUPnPResourceSelector

// upnpframework / internal api's
#include "upnpmetadatafetcher.h" // CreateItemFromFileLC
#include "upnpcommonutils.h" // ReplacePlaceHolderInURIL

// avcontrollerhelper internal
#include "upnplocalitemresolver.h"

#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpitemutility.h" // for GetResElements
#include "upnpsecaccesscontroller.h" // CUpnpSecAccessController
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KCancelWaitMaximum =   4000000;
const TInt KCancelWaitResolution = 500000;
const TInt KUnshareWait =  1000000;

// METHODS

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver:: NewL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver* CUPnPLocalItemResolver::NewL(
    const TDesC& aFilePath,
    MUPnPAVController& aAvController,
    MUPnPResourceSelector& aSelector,
    TInt aOptimisationFlags    )
    {
    CUPnPLocalItemResolver* self =
        new (ELeave )CUPnPLocalItemResolver(
        aAvController, aSelector, aOptimisationFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aFilePath );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::CUPnPLocalItemResolver
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver::CUPnPLocalItemResolver(
    MUPnPAVController& aAvController,
    MUPnPResourceSelector& aSelector,
    TInt aOptimisationFlags )
    : CActive( EPriorityStandard )
    , iAvController( aAvController )
    , iSelector( aSelector )
    {
    CActiveScheduler::Add( this );
    iOptimisationFlags = aOptimisationFlags;
    iState = EStateIdle;
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ConstructL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ConstructL(
    const TDesC& aFilePath )
    {
    __LOG1( "LocalItemResolver:ConstructL() 0x%d", TInt(this) );
    iFilePath = aFilePath.AllocL();
    iFileSharing = CUpnpFileSharing::NewL();    
    iAccessController = CUpnpSecAccessController::NewL();
    iWait = new (ELeave) CActiveSchedulerWait();
    iTimer = CUPnPPeriodic::NewL( CActive::EPriorityHigh );
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::~CUPnPLocalItemResolver
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver::~CUPnPLocalItemResolver()
    {
    __LOG1( "LocalItemResolver destructor 0x%d", TInt(this) );
    
    Cleanup();
    
    if ( iTempSession )
        {
        iTempSession->RemoveObserver();
        iAvController.StopBrowsingSession( *iTempSession );
        }

    delete iFileSharing;
    iFileSharing = NULL;
    
    // cancel any async calls
    if ( IsActive() )
        {
         __LOG( "LocalItemResolver destructor RequestComplete" );
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    
    Cancel();
      
    delete iFilePath;
    iFilePath = NULL;
    
    delete iAccessController;
    iAccessController = NULL;
    
    delete iSharedItem;
    iSharedItem = NULL;
        
    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    
    if( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    delete iWait;
    iWait = NULL;
        
    __LOG( "LocalItemResolver destructor end" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ResolveL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ResolveL(
    MUPnPItemResolverObserver& aObserver )
    {
    __LOG1( "LocalItemResolver:Resolve() 0x%d", TInt(this) );
    __ASSERTD( iState == EStateIdle, __FILE__, __LINE__ );
    iObserver = &aObserver;

    if ( iOptimisationFlags & UPnPItemResolverFactory::EOmitDrmCheck )
        {
        // no need to check DRM ! this branch is empty.
        }
    else
        {
        // check DRM
        if ( UPnPFileUtility::IsFileProtectedL( iFilePath->Des() ) )
            {
            User::Leave( KErrNotSupported );
            }
        }

    // create item metadata
    iState = EStateCreatingItem;
    CUpnpItem* item = UPnPMetadataFetcher::CreateItemFromFileLC(
        iFilePath->Des() );
    CleanupStack::Pop( item );
    __LOG1( "LocalItemResolver:Resolve CreateItemFromFileLC done 0x%d", TInt(this) );
    if( iSharedItem )
        {
        delete iSharedItem;
        iSharedItem = 0;
        }
    iSharedItem = item;

    // state check
    if ( iState == EStateCreatingItem )
        {
        // start local mediaserver
        if ( iOptimisationFlags & UPnPItemResolverFactory::EOmitLocalMSStart )
            {
            // omit mediaserver start - go directly to sharing.
            DoShareL();
            }
        else
            {
            // start a session for local MS keepalive
            CUpnpAVDevice* dummyDevice = CUpnpAVDevice::NewLC();
            dummyDevice->SetUuidL( KNullDesC8 );
            dummyDevice->SetDeviceType(CUpnpAVDevice::EMediaServer);    
            iTempSession = &iAvController.StartBrowsingSessionL( *dummyDevice );
            CleanupStack::PopAndDestroy( dummyDevice );

            iTempSession->SetObserver( *this );
            // request for start local MS
            iState = EStateStartingMS;
            iTempSession->ReserveLocalMSServicesL();
            }
        
        }
    else
        {
        __LOG( "LocalItemResolver: create item interrupted" );
		iState = EStateIdle;
        }
        
    __LOG( "LocalItemResolver:Resolve() END" );
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::DoCancel
// See upnplocalitemresolver.h
// --------------------------------------------------------------------------
void CUPnPLocalItemResolver::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::RunError
// See upnplocalitemresolver.h
// --------------------------------------------------------------------------
TInt CUPnPLocalItemResolver::RunError( TInt aError )
    {
    __LOG1( "CUPnPLocalItemResolver::RunError %d", aError );
    // should never be here.
    __PANICD( __FILE__, __LINE__ );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::RunL
// See upnplocalitemresolver.h
// --------------------------------------------------------------------------
void CUPnPLocalItemResolver::RunL()
    {
    __LOG2( "CUPnPLocalItemResolver::RunL iStatus=%d, iState=%d", 
        iStatus.Int(), iState );


	if ( iState == EStateSharing )
		{
		// If the sharing failed
        if( iStatus.Int() != KErrNone )
            {
            // Deny access to the files listed in res-elements
            SetAccesstoItemResources( *iSharedItem, EFalse );
            }
        
        if( iStatus.Int() == KErrNone )
    		{
    	    // replace the IP address in the URI
    	    __LOG( "LocalItemResolver:replacing IP in URI" );
    	    TInetAddr address;
    	    User::LeaveIfError( iMediaServer.Connect() );
    	    iMediaServer.GetAddress( address );
    	    UPnPCommonUtils::ReplacePlaceHolderInURIL(
    	        *iSharedItem, address );
    	    iMediaServer.Close();

    	    // select the resource
    	    iResource = &iSelector.SelectResourceL( *iSharedItem );
            }
               
        Complete( iStatus.Int() );
		}
		
    else if ( iState == EStateUnsharing )
		{	
		iTimer->Cancel();
		
        // Deny access to the files listed in res-elements
        SetAccesstoItemResources( *iSharedItem, EFalse );
        
        iResource = 0; // NOTE: no deletion !
        iState = EStateIdle;
        
        iWait->AsyncStop();
        }
        
    __LOG( "CUPnPLocalItemResolver::RunL end" );
    }
    
    
// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ReserveLocalMSServicesCompleted
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ReserveLocalMSServicesCompleted(
    TInt aError )
    {
    __LOG( "LocalItemResolver:MSServicesComplete" );
    __ASSERTD( iState == EStateStartingMS, __FILE__, __LINE__ );

    if ( aError == KErrNone )
        {
        TRAPD( err, DoShareL() );
        if( err != KErrNone )
            {
            __LOG1( "LocalItemResolver:MSServicesComplete\
 DoShareL failed %d", err );
            }
        }
    else
        {
        Complete( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::DoShareL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::DoShareL()
    {
    __LOG( "CUPnPLocalItemResolver::DoShareL start" );
    // share the item
    iState = EStateSharing;
    
    // Allow access to the files listed in res-elements
    SetAccesstoItemResources( *iSharedItem, ETrue );
    
    // Share the item
    iFileSharing->ShareItemL( KContainerIdRoot, *iSharedItem, iStatus );
        
    SetActive();
    __LOG( "CUPnPLocalItemResolver::DoShareL end" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::DoUnshareL
// See upnplocalitemresolver.h
// --------------------------------------------------------------------------
void CUPnPLocalItemResolver::DoUnshareL()
    {
    __LOG( "CUPnPLocalItemResolver::DoUnshareL" );

    iState = EStateUnsharing;
    
    TInt id;
    TLex8 idParser( iSharedItem->Id() );
    TInt status = idParser.Val( id );
    if ( status == KErrNone )
        {
        iFileSharing->UnshareItemL( id, iStatus );

        SetActive();
        }
    else
        {
        __LOG1( "CUPnPLocalItemResolver::DoUnshareL leave %d", status );
        User::Leave( status );
        }
        
    __LOG( "CUPnPLocalItemResolver::DoUnshareL - end" );
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Item
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
const CUpnpItem& CUPnPLocalItemResolver::Item() const
    {
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERTD( iSharedItem, __FILE__, __LINE__ );

    return *iSharedItem;
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Resource
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
const CUpnpElement& CUPnPLocalItemResolver::Resource() const
    {
    __ASSERTD( iState == EStateReady, __FILE__, __LINE__ );
    __ASSERTD( iResource, __FILE__, __LINE__ );

    return *iResource;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Complete
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::Complete( TInt aError )
    {
    __LOG1( "LocalItemResolver:Complete() %d", aError );
    __ASSERTD( iState == EStateStartingMS || 
        iState == EStateSharing, __FILE__, __LINE__ );
        
    MUPnPItemResolverObserver& observer = *iObserver;
    iObserver = 0;
    if ( aError == KErrNone )
        {
        iState = EStateReady;
        }
    else
        {
        iState = EStateIdle;
        Cleanup();
        }

    observer.ResolveComplete( *this, aError );
    __LOG( "LocalItemResolver:Complete() END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Cleanup
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::Cleanup()
    {
    __LOG1( "CUPnPLocalItemResolver:Cleanup() iState %d", iState );
    
    iObserver = 0;

    if ( iState == EStateCreatingItem )
        {
        // signal cancel, wait until create item exits
        iState = EStateCancel;
        for ( TInt t = KCancelWaitMaximum;
            t > 0 && iState != EStateIdle;
            t -= KCancelWaitResolution )
            {
            User::After( TTimeIntervalMicroSeconds32(
                KCancelWaitResolution ) );
            }
        }
    else if ( iState == EStateStartingMS )
        {
        if( iTempSession )
            {
            TRAP_IGNORE( iTempSession->CancelReserveLocalMSServicesL() );
            }
        }
    else if ( iState == EStateSharing )
		{
        // do nothing, cannot unshare item if sharing hasn't finished yet.
        __LOG( "CUPnPLocalItemResolver:Cleanup() sharing in progress, do nothing" );
		} 
    else if ( iState == EStateUnsharing )
		{
        // do nothing, unsharing is still in progress. This happens,
        // if we start shutting down while previous cleanup hasn't finished yet.
        __LOG( "CUPnPLocalItemResolver:Cleanup() unsharing in progress, do nothing" );
		}
    else if ( iState == EStateCancel )
		{
        // do nothing, we shouldn't be here
        __LOG( "CUPnPLocalItemResolver:Cleanup() cancelling, shouldn't be here.." );
		}
    else if ( iSharedItem )
        {
        TRAPD( error, DoUnshareL() );
        
        if( error == KErrNone )
            {
            // wait some time.. 
            // if unshare doesn't finish in time, just cancel andcontinue
            __LOG( "CUPnPLocalItemResolver:Cleanup() start timer" );
            iTimer->Start( KUnshareWait, KUnshareWait, TCallBack( TimerExpired, this ) );
            iWait->Start();
            
            iTimer->Cancel();
            }
        else
            {
            __LOG1( "CUPnPLocalItemResolver:Cleanup() unshare failed %d", error );
            }
        }

    __LOG( "CUPnPLocalItemResolver:Cleanup() end" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::SetAccesstoItemResources
// See upnplocalitemresolver.h
// --------------------------------------------------------------------------
void CUPnPLocalItemResolver::SetAccesstoItemResources(
                                                CUpnpItem& aItem,
                                                TBool aAccessAllowed )
    {
    __LOG( "CUPnPLocalItemResolver::SetAccesstoItemResources" );

    // Get all filenames and set the access for those.
    RUPnPElementsArray resElements;
    UPnPItemUtility::GetResElements( aItem, resElements );
    for( TInt i=0; i<resElements.Count(); i++ )
        {
        if( aAccessAllowed )
            {
            iAccessController->AddAllowedFile( resElements[i]->FilePath() );
            }
        else
            {
            iAccessController->RemoveAllowedFile(
                                        resElements[i]->FilePath() );
            }
        }
    resElements.Close();
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::TimerExpired
// See upnplocalitemresolver.h
// -------------------------------------------------------------------------- 
TInt CUPnPLocalItemResolver::TimerExpired( TAny* aArg )
    {
    __LOG( "CUPnPLocalItemResolver::TimerExpired" );
    
    CUPnPLocalItemResolver* resolver =
        (static_cast<CUPnPLocalItemResolver*>( aArg ));
    
    resolver->iTimer->Cancel();
    resolver->iWait->AsyncStop();
    
    __LOG( "CUPnPLocalItemResolver::TimerExpired end" );
    return KErrNone;
    }

