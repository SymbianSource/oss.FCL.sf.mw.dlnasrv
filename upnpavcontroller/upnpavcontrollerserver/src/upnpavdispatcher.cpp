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
* Description:      dispatches control point events to AVController clients
*
*/



// INCLUDE FILES

#include "upnpavdispatcher.h"

#include "upnpavactioninfo.h"
#include "upnpavcontrolpointobserver.h"
#include "upnpavcontrollerserver.h"

_LIT( KComponentLogfile, "CUPnPAVDispatcher.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::CUPnPAVDispatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher::CUPnPAVDispatcher( CUpnpAVControllerServer& aServer ) :
    iServer( aServer )
    {    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::ConstructL()
    {                           
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher* CUPnPAVDispatcher::NewL( CUpnpAVControllerServer& aServer )
    {
    CUPnPAVDispatcher* self = NewLC( aServer);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher* CUPnPAVDispatcher::NewLC(
    CUpnpAVControllerServer& aServer )
    {
    CUPnPAVDispatcher* self = new( ELeave )
        CUPnPAVDispatcher( aServer );   
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// --------------------------------------------------------------------------
// CUPnPAVDispatcher::~CUPnPAVDispatcher
// --------------------------------------------------------------------------
//
CUPnPAVDispatcher::~CUPnPAVDispatcher()
    {
    iActionInfos.ResetAndDestroy(); 
    iActionInfosEvent.ResetAndDestroy();           
    }



// --------------------------------------------------------------------------
// CUPnPAVDispatcher::ActionResponseL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::ActionResponseL(CUpnpAction* aAction )
    {
    __LOG( "CUPnPAVDispatcher::ActionResponseL" );

    MUpnpAVControlPointObserver& obs = FindObserver( aAction->SessionId() );
    if( &obs )
        {
        obs.ActionResponseL( aAction );
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::StateUpdatedL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::StateUpdatedL(CUpnpService* aService)
    {
    __LOG( "CUPnPAVDispatcher::StateUpdatedL" );

    // Forward to each observer
    TInt tempCount = iActionInfosEvent.Count();
    
    //We may have multiple sessions to the same device and 
    //they are all interested about state changes
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( aService->Device().Uuid() == iActionInfosEvent[i]->Uuid() )
            {
            iActionInfosEvent[i]->Observer().StateUpdatedL(aService);
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::HttpResponseL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::HttpResponseL( CUpnpHttpMessage* /*aMessage*/ )
    {
    //NO IMPLEMENTATION NEEDED
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::DeviceDiscoveredL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::DeviceDiscoveredL( CUpnpDevice* aDevice )
    {
    __ASSERT( aDevice, __FILE__, __LINE__  )
    
    iServer.DeviceDiscoveredL( *aDevice );    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::DeviceDisappearedL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::DeviceDisappearedL( CUpnpDevice* aDevice )
    {
    __ASSERT( aDevice, __FILE__, __LINE__  );
    
    iServer.DeviceDisappearedL( *aDevice );    
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::RegisterL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RegisterL( TInt aSessionId,
    MUpnpAVControlPointObserver& aObserver )
    {
    __LOG1( "CUPnPAVDispatcher::RegisterL session: %d", aSessionId );

    CUPnPAVActionInfo* tempInfo = CUPnPAVActionInfo::NewLC();
    tempInfo->SetSessionId( aSessionId );
    tempInfo->SetObserver( aObserver );
    CleanupStack::Pop( tempInfo );
    iActionInfos.AppendL( tempInfo );
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::UnRegister
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::UnRegister( TInt aSessionId
    /*, const TDesC8& aUuid*/ )
    {
    __LOG1( "CUPnPAVDispatcher::UnRegister session: %d", aSessionId );

    TInt tempCount = iActionInfos.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( iActionInfos[ i ]->SessionId() == aSessionId )
            {
            delete iActionInfos[ i ];
            iActionInfos.Remove( i );
            i = tempCount;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::FindObserver
// --------------------------------------------------------------------------
//
MUpnpAVControlPointObserver& CUPnPAVDispatcher::FindObserver(
    TInt aSessionId )
    {
    MUpnpAVControlPointObserver* tempObserver = NULL;
    TInt tempCount = iActionInfos.Count();
    
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( iActionInfos[ i ]->SessionId() == aSessionId )
            {
            tempObserver = &( iActionInfos[ i ]->Observer() );
            i = tempCount;
            }
        }
    return *tempObserver;   
    }

// --------------------------------------------------------------------------
// CUPnPAVDispatcher::RegisterForEventsL
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::RegisterForEventsL(
    MUpnpAVControlPointObserver& aObserver, const TDesC8& aUuid )
    {
    __LOG( "CUPnPAVDispatcher::RegisterForEventsL" );

    TInt tempCount = iActionInfosEvent.Count();
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( &aObserver == &iActionInfosEvent[ i ]->Observer() )
            {
            // Must not let to register twice!
            __PANIC( __FILE__, __LINE__ );
            }
        }        

    CUPnPAVActionInfo* tempInfo = CUPnPAVActionInfo::NewLC();
    tempInfo->SetObserver( aObserver );
    tempInfo->SetUuidL( aUuid );
    CleanupStack::Pop( tempInfo );
    iActionInfosEvent.AppendL( tempInfo );        
    }
    
// --------------------------------------------------------------------------
// CUPnPAVDispatcher::UnRegisterEvents
// --------------------------------------------------------------------------
//
void CUPnPAVDispatcher::UnRegisterEvents(
    MUpnpAVControlPointObserver& aObserver )
    {
    __LOG( "CUPnPAVDispatcher::UnRegisterEvents" );
    
    TInt tempCount = iActionInfosEvent.Count();
    for( TInt i = 0; i < tempCount; i++ )
        {
        if( &aObserver == &iActionInfosEvent[ i ]->Observer() )
            {
            delete iActionInfosEvent[ i ];
            iActionInfosEvent.Remove( i );
            i = tempCount;
            }
        }        
    }


// End of File

