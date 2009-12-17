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
* Description:      CUpnpContentShareAO class implementation
*
*/






// INCLUDE FILES

#include "upnpcontentshareao.h"
#include "upnpfilesharingengine.h"

_LIT( KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpContentShareAO::CUpnpContentShareAO
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpContentShareAO::CUpnpContentShareAO( 
    CUPnPFileSharingEngine& aEngine,
    RUpnpContentServerClient& aContentServer ):
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iContentServer( aContentServer )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::ConstructL()
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::ConstructL()
    {    
    __LOG( "CUpnpContentShareAO::ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::NewL
// --------------------------------------------------------------------------
//
CUpnpContentShareAO* CUpnpContentShareAO::NewL( 
        CUPnPFileSharingEngine& aEngine, 
        RUpnpContentServerClient& aContentServer )
    {   
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::NewL begin" );
    CUpnpContentShareAO* self = CUpnpContentShareAO::NewLC(
            aEngine, aContentServer );
    CleanupStack::Pop( self );
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::NewL end" );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::NewLC
// --------------------------------------------------------------------------
//
CUpnpContentShareAO* CUpnpContentShareAO::NewLC( 
        CUPnPFileSharingEngine& aEngine, 
        RUpnpContentServerClient& aContentServer )
    {    
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::NewLC begin" );
    CUpnpContentShareAO* self = new( ELeave )
        CUpnpContentShareAO( aEngine, aContentServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::NewLC end" );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::~CUpnpContentShareAO()
// Destructor
// --------------------------------------------------------------------------
//    
CUpnpContentShareAO::~CUpnpContentShareAO()
    {
    __LOG( "CUpnpContentShareAO::~CUpnpContentShareAO " );

    iQueuedSelections.ResetAndDestroy();
    iQueuedSelections.Close();
    iQueuedTypes.Close();

    Cancel();
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::ChangeSharedContentL()
// Sends the selected indexes to server and starts sharing
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::ChangeSharedContentL(
    UpnpContentServer::TUpnpMediaType aType,
    const CArrayFix<TInt>& aMarkedItems )
    {   
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::\
ChangeSharedContentL begin" );

    if ( !IsActive() )
        {
        __ASSERTD( iQueuedTypes.Count() == 0, __FILE__, __LINE__ );
        iContentServer.ChangeSharedContentL( aType, aMarkedItems, iStatus );
        iStatus = KRequestPending;
        SetActive();       
        }
    else
        {
        //remove all queued requests with this type
        RemoveQueuedObjectByType( aType );
        
        //queue this request
        CArrayFix<TInt>* selections = 
            new ( ELeave ) CArrayFixFlat<TInt>( aMarkedItems.Count() );
        CleanupStack::PushL( selections );
        for ( TInt i ( 0 ); i < aMarkedItems.Count(); i++ )
            {
            selections->AppendL( aMarkedItems.At(i) );
            }
        
        AppendQueuedObjectL( aType, selections );
        CleanupStack::Pop( selections );
        __ASSERTD( iQueuedSelections.Count() == iQueuedTypes.Count(),
                __FILE__, __LINE__ );
        }

    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::\
ChangeSharedContentL end" );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::RefreshSharedContentL()
// Updates shared objects in mediaserver
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::RefreshSharedContentL( 
    UpnpContentServer::TUpnpMediaType aType )
    {
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::\
RefreshSharedContentL begin" );
    if ( !IsActive() )
        {
        __ASSERTD( iQueuedTypes.Count() == 0, __FILE__, __LINE__ );
        iContentServer.RefreshSharedContentL( aType, iStatus );       
        iStatus = KRequestPending;
        SetActive();       
        }
    else
        {
        //remove all queued requests with this type
        RemoveQueuedObjectByType( aType );

        //queue this request
        CArrayFix<TInt>* selections( NULL );
        AppendQueuedObjectL( aType, selections );
        __ASSERTD( iQueuedSelections.Count() == iQueuedTypes.Count(),
                __FILE__, __LINE__ );
        }
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::\
RefreshSharedContentL end" );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::DoCancel
// Cancel the operation
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::DoCancel()
    {    
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::DoCancel" );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::RunL
// Function is called when active request is ready
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::RunL()
    {      
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::RunL begin" );
    __LOG1( "[UPNP_ENGINE]\t\t status: %d", iStatus.Int() );

    TInt err = iStatus.Int();
    if ( err && 
         err != KErrServerBusy )
        {
        // KErrServerBusy is a response to the 
        // CUpnpContentServer::RefreshSharedContentL call. Therefore we must 
        // not leave without cleaning queued objects. 
        // Otherwise next call to CUpnpContentShareAO::RefreshSharedContentL 
        // will panic (in UDEB builds only!!!!). 
        
        // Clears queued objects and leaves.
        iQueuedSelections.ResetAndDestroy();
        iQueuedTypes.Reset();
        User::Leave( err );
        }

    //When get the responce from the content server then change the UI    
    iEngine.RequestSharingProgressL();

    if ( iQueuedTypes.Count() )
        {
        // Get next queued objects
        UpnpContentServer::TUpnpMediaType type = iQueuedTypes[ 0 ];
        CArrayFix<TInt>* selections = iQueuedSelections[ 0 ];
        
        // Clear arrays.
        iQueuedSelections.Remove( 0 );
        iQueuedTypes.Remove( 0 );
        
        if ( selections )
            {
            //share request
            CleanupStack::PushL( selections );
            iContentServer.ChangeSharedContentL( type, *selections, iStatus );
            CleanupStack::PopAndDestroy( selections );
            }
        else
            {
            //refresh request
            iContentServer.RefreshSharedContentL( type, iStatus );       
            }
        
        iStatus = KRequestPending;
        SetActive();       
        }
    __LOG( "[UPNP_ENGINE]\t CUpnpContentShareAO::RunL end" );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::RunError
// Handles a leave occurring in the request completion
// --------------------------------------------------------------------------
//
TInt CUpnpContentShareAO::RunError( TInt aError )
    {    
    __LOG( "CUpnpContentShareAO::RunError" );
    if ( iEngine.Observer() )
        {
        iEngine.Observer()->HandleSharingDone( iEngine, aError );
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::RemoveQueuedObjects
// Removed queued objects by index.
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::RemoveQueuedObject( TInt aIndex )
    {
    // Checks argument in udeb build.
    // USER 130 panic will occure in urel builds if argument is
    // incorrent.
    TInt count = iQueuedTypes.Count();
    __ASSERTD( aIndex >= 0 && aIndex < iQueuedTypes.Count(),
            __FILE__, __LINE__ );
    
    iQueuedTypes.Remove( aIndex );
    if ( iQueuedSelections[ aIndex ] )
        {
        delete iQueuedSelections[ aIndex ];
        }
    iQueuedSelections.Remove( aIndex );
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::RemoveQueuedObjectByType
// Removed queued objects by type.
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::RemoveQueuedObjectByType( 
        UpnpContentServer::TUpnpMediaType aType )
    {
    //remove all queued requests with this type
    for ( TInt i( iQueuedTypes.Count() - 1 ); i >= 0; i-- )
        {
        if ( iQueuedTypes[ i ] == aType )
            {
            RemoveQueuedObject( i );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpContentShareAO::AppendQueuedObjectL
// Appends given objects.
// --------------------------------------------------------------------------
//
void CUpnpContentShareAO::AppendQueuedObjectL( 
        UpnpContentServer::TUpnpMediaType aType, 
        CArrayFix<TInt>* aSelections )
    {
    iQueuedTypes.AppendL( aType );
    TRAPD( err, iQueuedSelections.AppendL( aSelections ) );
    if ( err )
        {
        // Couldn't append array to the iQueuedSelections array.
        // Removes the type and leaves.
        iQueuedTypes.Remove( iQueuedTypes.Count() - 1 );
        User::Leave( err );
        }
    }
