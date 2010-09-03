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
* Description:  CUpnpAVCPManager
*
*/


// INCLUDE FILES

#include "upnpavcpmanager.h"
#include "upnpavcpenginehelper.h"
#include "upnppathresolver.h"
#include "upnpresponsehandler.h"
#include "upnpmdebug.h"
#include "upnpavcpenginecommon.h"



// -----------------------------------------------------------------------------
// CUpnpAVCPManager::NewL
// Creates an instance of the implementation.
// -----------------------------------------------------------------------------
CUpnpAVCPManager* CUpnpAVCPManager::NewL()
	{ 
	CUpnpAVCPManager* self = new (ELeave) CUpnpAVCPManager(NULL);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::CUpnpAVCPManager
// First phase construction.
// -----------------------------------------------------------------------------
CUpnpAVCPManager::CUpnpAVCPManager(MUpnpAVControlPointObserver* aObserver):CUpnpAVControlPoint((MUpnpAVControlPointObserver&)aObserver)
	{
    
	}

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::ConstructL
// Second phase construction.
// -----------------------------------------------------------------------------
void CUpnpAVCPManager::ConstructL()
	{	    
    
    _LIT8( KMediaServer, "MediaServer" );
	//CUpnpControlPoint::ConstructL( KMediaServer, value );	
	CUpnpControlPoint::ConstructL( KMediaServer );	
	InitializeCdsActionFactoryL();
        
	// maybe we should look for something different??
	TPtrC8 devicePtr;
    devicePtr.Set( UpnpSSDP::KSearchTargetAll );
    //SSDP search for mediaserver UPnP devices
	DEBUGSTRING(("Search for devices"));    
	SearchL( devicePtr );
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CUpnpAVCPManager::~CUpnpAVCPManager()
	{
	iPendingHandlers.Reset();
	iHttpPendingHandlers.Reset();
	iPathResolvers.ResetAndDestroy();    
	}
	
// -----------------------------------------------------------------------------
// CUpnpAVCPManager::NewCommandId
// return new command id
// -----------------------------------------------------------------------------	
TInt CUpnpAVCPManager::NewCommandId() 
	{
	return ++iLastCommandId;
	}	

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::PathResolver
// return resolver or creates a new one
// -----------------------------------------------------------------------------	
CUpnpPathResolver& CUpnpAVCPManager::PathResolverL(const TDesC8& aUUID, CUpnpAVCPEngineSession* aSession) 
    {
	    
    CUpnpPathResolver* resolver = CUpnpPathResolver::NewL(*this,aUUID);
    CleanupStack::PushL(resolver);

    TIdentityRelation<CUpnpPathResolver> matcher( CUpnpPathResolver::Match );
	TInt idx = iPathResolvers.Find( resolver, matcher ); 
	
		
	if (idx != KErrNotFound)   
    	{
	    CleanupStack::PopAndDestroy(resolver);	
	    resolver = iPathResolvers[idx];
    	}
	else 
    	{	    
	    //subscribe
	    CUpnpDevice* device = const_cast<CUpnpDevice*>(Device(aUUID));
	    if (device) 
	    	{
	        CUpnpService* service  = Service(device, UpnpAVCPEngine::KContentDirectory);
			DEBUGSTRING8(("Subscribe for events"));	        
	        SubscribeL( service );
	    	} 
	    else 
	    	{
	    	User::Leave(KErrNotFound);
	    	}
	    CleanupStack::Pop(resolver);	
	    iPathResolvers.AppendL(resolver);	    
    	}  
    	
    resolver->RegisterSession(aSession);	
    return *resolver;	
    }
    
// -----------------------------------------------------------------------------
// CUpnpAVCPManager::CleanupSession
// cleanup after closed client session
// -----------------------------------------------------------------------------		
void CUpnpAVCPManager::CleanupSessionL( CUpnpAVCPEngineSession* aSession) 
	{
	DEBUGSTRING8(("CleanupSessionL"));	        	
    for (TInt i = 0; i < iPathResolvers.Count(); i++ ) 
    {
        if (iPathResolvers[i]->UnregisterSession(aSession) == 0 ) 
        {
            //unsubscribe
            CUpnpDevice* device = const_cast<CUpnpDevice*>(Device(iPathResolvers[i]->UUID()));
	        if (device) 
	        {
	            CUpnpService* service  = Service(device, UpnpAVCPEngine::KContentDirectory);
	            DEBUGSTRING8(("UnsubscribeL for events"));
	            UnsubscribeL( service );
	        }
	    
            //remove
            delete iPathResolvers[i];
            iPathResolvers.Remove(i);
            i--;
        }
    }    
	}
	
// -----------------------------------------------------------------------------
// CUpnpAVCPManager::RegisterForAction
// register for get action responses
// -----------------------------------------------------------------------------			
void CUpnpAVCPManager::RegisterForAction(MUpnpResponseHandler& aHandler)	 
  {
      iPendingHandlers.AppendL(&aHandler);
  }

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::HandlerBySessionId
// return index of handler for specific session
// -----------------------------------------------------------------------------	
TInt CUpnpAVCPManager::HandlerBySessionId(TInt aSessionId) 
    {
    TInt i = KErrNotFound;

    for ( i=0; i < iPendingHandlers.Count() && iPendingHandlers[i]->SessionId() != aSessionId; i++ )
    {}

    if ( i != iPendingHandlers.Count() )
        {
        return i;
        }
    return KErrNotFound;   
    }
// -----------------------------------------------------------------------------
// CUpnpAVCPManager::HandlerBySessionId
// return index of handler for specific session
// -----------------------------------------------------------------------------	
void CUpnpAVCPManager::RegisterForHttp(MUpnpHttpResponseHandler& aHandler)	 
    {
    iHttpPendingHandlers.AppendL(&aHandler);
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::HandlerBySessionId
// return index of handler for specific session
// -----------------------------------------------------------------------------	    
void CUpnpAVCPManager::UnregisterForHttp(MUpnpHttpResponseHandler& aHandler)	 
    {
    TInt index = iHttpPendingHandlers.Find(&aHandler);
    if (index != KErrNotFound) 
        {
        iHttpPendingHandlers.Remove(index);    
        }        
    }    

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::HandlerBySessionId
// return index of handler for specific session
// -----------------------------------------------------------------------------	
TInt CUpnpAVCPManager::HttpHandlerBySessionId(TInt aSessionId) 
    {
    TInt i = KErrNotFound;

    for ( i=0; i < iHttpPendingHandlers.Count() && iHttpPendingHandlers[i]->SessionId() != aSessionId; i++ )
    {}

    if ( i != iHttpPendingHandlers.Count() )
        {
            return i;
        }
    return KErrNotFound;   
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::HandlerBySessionId
// return index of handler for specific session
// -----------------------------------------------------------------------------	
void CUpnpAVCPManager::ActionResponseReceivedL(CUpnpAction* aAction) 
    {
    TInt sessionId = aAction->SessionId();	
    TInt index = HandlerBySessionId(sessionId);
    if (index != KErrNotFound) 
        {
        MUpnpResponseHandler* handler = iPendingHandlers[index];
        iPendingHandlers.Remove(index);        
        handler->InterpretL(aAction->Error(), aAction);    
        }
    
    }

// -----------------------------------------------------------------------------
// CUpnpAVCPManager::StateUpdatedL
//  Handles UPnP device events.
// -----------------------------------------------------------------------------	
void CUpnpAVCPManager::StateUpdatedL(CUpnpService*  aService) 
    {
    CUpnpPathResolver* resolver = CUpnpPathResolver::NewL(*this,aService->Device().Uuid());
    CleanupStack::PushL(resolver);

    TIdentityRelation<CUpnpPathResolver> matcher( CUpnpPathResolver::Match );
	TInt idx = iPathResolvers.Find( resolver, matcher ); 
	CleanupStack::PopAndDestroy(resolver);	    	    	
	
	if (idx != KErrNotFound)   
    	{	   
	    CUpnpStateVariable* systemUpdateId = aService->StateVariable( UpnpAVCPEngine::KSystemUpdateID );
        if (systemUpdateId) 
            {
            (iPathResolvers[idx])->StateChangedL(systemUpdateId->Value()); 
            }
    	     
    	}
    }

// --------------------------------------------------------------------------
// CUpnpAVCPManager::HttpResponseReceivedL
// Handles HTTP messages.
// --------------------------------------------------------------------------
void CUpnpAVCPManager::HttpResponseReceivedL(CUpnpHttpMessage* aMessage)
    {
    TInt sessionId = aMessage->SessionId();    
	
    TInt index = HttpHandlerBySessionId(sessionId);
    if (index != KErrNotFound) 
        {
        MUpnpHttpResponseHandler* handler = iHttpPendingHandlers[index];        
        handler->InterpretL(aMessage->Error(), aMessage);    
        } 
    else 
    	{
    	index = index;
       	}   
    }
    
// --------------------------------------------------------------------------
// CUpnpAVCPManager::DeviceDiscoveredL
// Handles UPnP device discoveries.
// --------------------------------------------------------------------------
void CUpnpAVCPManager::DeviceDiscoveredL(CUpnpDevice* aDevice)
    {
	TPtrC8 uuid = aDevice->Uuid();    
	TPtrC8 fn = aDevice->DescriptionProperty(KAVCPEngineFriendlyName);    	
	DEBUGSTRING8(("DeviceDiscoveredL %S %S", &uuid, &fn));     
    }

// --------------------------------------------------------------------------
// CUpnpAVCPManager::DeviceDisappearedL(
// Handles UPnP device disappears.
// --------------------------------------------------------------------------
void CUpnpAVCPManager::DeviceDisappearedL(CUpnpDevice* aDevice)
    {
	TPtrC8 uuid = aDevice->Uuid();
	DEBUGSTRING8(("DeviceDiscoveredL %S", &uuid));     

    CUpnpPathResolver* resolver = CUpnpPathResolver::NewL(*this,aDevice->Uuid());
    CleanupStack::PushL(resolver);

    TIdentityRelation<CUpnpPathResolver> matcher( CUpnpPathResolver::Match );
	TInt idx = iPathResolvers.Find( resolver, matcher ); 
	CleanupStack::PopAndDestroy(resolver);	    	    	
	
	if (idx != KErrNotFound)   
		{    	    
		   delete iPathResolvers[idx];   
		   iPathResolvers.Remove(idx);
		}	        
    }
    
// end of file



