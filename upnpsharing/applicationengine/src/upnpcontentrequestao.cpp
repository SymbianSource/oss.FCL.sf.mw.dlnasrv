/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUpnpContentRequestAO class implementation
 *
*/






// INCLUDE FILES
// System
#include <s32file.h>
#include <f32file.h>

// upnp stack api
#include <upnpstring.h>

// upnpframework / avcontroller api
#include "upnpavcontrollerfactory.h"

// homeconnect internal
#include "upnpcontentrequestao.h"
#include "upnpfilesharingengine.h"

_LIT( KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::CUpnpContentRequestAO
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpContentRequestAO::CUpnpContentRequestAO( 
    CUPnPFileSharingEngine& aEngine,
    RUpnpContentServerClient& aContentServer )
    : CActive( CActive::EPriorityStandard ),
      iEngine( aEngine ),
      iContentServer( aContentServer )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::~CUpnpContentRequestAO()
// Destructor
// --------------------------------------------------------------------------
//
CUpnpContentRequestAO::~CUpnpContentRequestAO()
    {
    if ( IsActive() )
        {
        __LOG( "CUpnpContentRequestAO destructor: IsActive == TRUE!" );
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    
    Cancel();
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::RequestSelectionContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::RequestSelectionContentL( 
    UpnpContentServer::TUpnpMediaType aType )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::RequestSelectionContentL\
 begin");

    // check media server availibility
    if ( IsActive() ||
         UPnPAVControllerFactory::MSServicesInUse() )
        {
        User::Leave( KErrInUse );
        }
    iContentServer.GetSelectionContentL( aType, iStatus );
    SetActive();
    iState = ERequestContent;
 
    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::RequestSelectionContentL\
 end");
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::GetSelectionContentL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::GetSelectionContentL( CDesCArray& aArray )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::GetSelectionContentL\
 begin");
    
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iContentServer.GetSelectionContentResultL( aArray );

    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::GetSelectionContentL\
 end");
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::RequestSelectionIndexesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::RequestSelectionIndexesL(
    UpnpContentServer::TUpnpMediaType aType )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::RequestSelectionIndexesL\
 begin");

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iContentServer.GetSelectionIndexesL( aType, iStatus );
    SetActive();
    iState = ERequestIndexes;

    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::RequestSelectionIndexesL\
 end");
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::GetSelectionIndexesL
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::GetSelectionIndexesL(
    CArrayFix<TInt>& aCurrentSelection )
    {
    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::GetSelectionIndexesL\
 begin");

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iContentServer.GetSelectionIndexesResultL( aCurrentSelection );

    __LOG("[UPNP_ENGINE]\t CUpnpContentRequestAO::GetSelectionIndexesL\
 end");
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::RunL()
    {
    __LOG1("[UPNP_ENGINE]\t CUpnpContentRequestAO::RunL, error=%d", 
        iStatus.Int());

    TState state( iState );
    iState = ENone;

    if ( iStatus.Int() )
        {
        __LOG1("CUpnpContentRequestAO::RunL: Error %d in iStatus",
            iStatus.Int() );
        }

    if ( iEngine.Observer() )
        {
        if ( state == ERequestContent )
            {
            iEngine.Observer()->HandleSelectionContent( 
                iEngine, iStatus.Int() );
            }
        else if ( state == ERequestIndexes )
            {
            iEngine.Observer()->HandleSelectionIndexes( 
                iEngine, iStatus.Int() ); 
            }
        else
            {
            __PANICD( __FILE__, __LINE__ );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpContentRequestAO::DoCancel
// Cancels the active request
// --------------------------------------------------------------------------
//
void CUpnpContentRequestAO::DoCancel()
    {
    }

// End of file
