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
* Description:      An abstract browsing session observer implementation
*
*/






// INCLUDE FILES
#include "upnpavbrowsingsession.h"
#include "upnpavbrowsingsessionobserver.h"

#include "upnpabstractbrowsingsessionobserver.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"


// CONSTANTS


// METHODS


// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::CUPnPAbstractBrowsingSessionObserver
//---------------------------------------------------------------------------
EXPORT_C CUPnPAbstractBrowsingSessionObserver::
    CUPnPAbstractBrowsingSessionObserver()
    {
    }


// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::~CUPnPAbstractBrowsingSessionObserver
//---------------------------------------------------------------------------
EXPORT_C CUPnPAbstractBrowsingSessionObserver::
    ~CUPnPAbstractBrowsingSessionObserver()
    {
    DisableSessionObserver();
    }


// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::SetSession
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractBrowsingSessionObserver::SetSession(
    MUPnPAVBrowsingSession& aHostSession )
    {
    // observer bust be disabled when calling this!
    __ASSERTD( !iObserverEnabled, __FILE__, __LINE__ );
    iObservedSession = &aHostSession;
    }



// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::EnableSessionObserver
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractBrowsingSessionObserver::EnableSessionObserver()
    {
    __LOG( "AbstractObserver:EnableSessionObserver()" );
    if ( !iObserverEnabled )
        {
        // take original observer as proxy
        iProxyObserver = iObservedSession->Observer();
        iObservedSession->RemoveObserver();
        iObservedSession->SetObserver( *this );
        iObserverEnabled = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::DisableSessionObserver
//---------------------------------------------------------------------------
EXPORT_C void CUPnPAbstractBrowsingSessionObserver::DisableSessionObserver()
    {
    if ( iObserverEnabled )
        {
        __LOG( "AbstractObserver:DisableSessionObserver()" );
        __ASSERTD( iObservedSession != 0, __FILE__, __LINE__ );
        // check we still are still the observer !!!
        if ( iObservedSession->Observer() == this )
            {
            iObservedSession->RemoveObserver();
            if ( iProxyObserver )
                {
                // set proxy observer back
                iObservedSession->SetObserver( *iProxyObserver );
                iProxyObserver = 0;
                }
            }
        else
            {
            __LOG( "Disable: Not disabling - wrong observer!" );
            }
        iObserverEnabled = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::IsSessionObserverEnabled
//---------------------------------------------------------------------------
EXPORT_C TBool CUPnPAbstractBrowsingSessionObserver::IsSessionObserverEnabled()
    {
    return iObserverEnabled;
    }


// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::BrowseResponse
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::BrowseResponse(
    const TDesC8& aBrowseResponse,
    TInt aError,
    TInt aMatches,
    TInt aTotalCount,
    const TDesC8& aUpdateId )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->BrowseResponse( aBrowseResponse, aError,
            aMatches, aTotalCount, aUpdateId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::SearchResponse
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::SearchResponse( 
    const TDesC8& aSearchResponse,
    TInt aError,
    TInt aMatches,
    TInt aTotalCount,
    const TDesC8& aUpdateId )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->SearchResponse( aSearchResponse, aError,
            aMatches, aTotalCount, aUpdateId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::SearchCapabilitiesResponse
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::SearchCapabilitiesResponse( 
    TInt aError,
    const TDesC8& aSearchCapabilities )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->SearchCapabilitiesResponse(
            aError, aSearchCapabilities );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::CreateContainerResponse
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::CreateContainerResponse(
    TInt aError, 
    const TDesC8& aObjectId )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->CreateContainerResponse( aError, aObjectId );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::DeleteObjectResponse
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::DeleteObjectResponse(
    TInt aError )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->DeleteObjectResponse( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::ReserveLocalMSServicesCompleted
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::
    ReserveLocalMSServicesCompleted( TInt aError )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->ReserveLocalMSServicesCompleted( aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAbstractBrowsingSessionObserver::MediaServerDisappeared
//---------------------------------------------------------------------------
void CUPnPAbstractBrowsingSessionObserver::MediaServerDisappeared(
    TUPnPDeviceDisconnectedReason aReason )
    {
    __ASSERTD( iObserverEnabled, __FILE__, __LINE__ );
    if ( iProxyObserver )
        {
        iProxyObserver->MediaServerDisappeared( aReason );
        }
    }


