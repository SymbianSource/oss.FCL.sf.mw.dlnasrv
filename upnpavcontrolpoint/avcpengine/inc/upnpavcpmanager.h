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
* Description:  Class comunicates with MSs
*
*/


#ifndef C_CUPNPAVCPMANAGER_H
#define C_CUPNPAVCPMANAGER_H

// INCLUDES
#include "upnpavcontrolpoint.h"

// FORWARD DECLARATIONS
class MUpnpResponseHandler;
class MUpnpHttpResponseHandler;
class CUpnpPathResolver;
class CUpnpAVCPEngineSession;

// CLASS DECLARATION
/**
* Class implements (derive CUpnpAVControlPoint).
* It provides way to send SOAP action, HTTP request, 
* receive notification about responses also support eventing phase.
* Additionally class keeps path resolvers for each MS (if it is needed).
*
*  @since Series 60 3.1
*/
class CUpnpAVCPManager : public CUpnpAVControlPoint
	{	
	public:
		/**
		* Two-phased constructor.
		*/		
		static CUpnpAVCPManager* NewL();
		/**
		* Destructor.
		*/		        
		~CUpnpAVCPManager();
 		/**
		* Get unique command id
		* 
		* @return command id
		*/		
		TInt NewCommandId();
 		/**
 		* Register a observer for receive notification about SOAP responses
 		*
 		* @param aHandler class implemented observer
 		*/
		void RegisterForAction(MUpnpResponseHandler& aHandler);
 		/**
 		* Register a observer for receive notification about HTTP responses
 		*
 		* @param aHandler class implemented observer
 		*/        
		void RegisterForHttp(MUpnpHttpResponseHandler& aHandler);        
 		/**
 		* Unregister a observer for receive notification about HTTP responses
 		*
 		* @param aHandler class implemented observer
 		*/        
		void UnregisterForHttp(MUpnpHttpResponseHandler& aHandler);                
	    /**
	    * Return path resolver.
	    * If path resolver was created, return instance, otherwise create a new one, 
	    * subsribe for events.
	    * Parameter aSession is used for managing life time of resolver. 
	    * if all session used resolver has been closed, resolver is doestoyed.
	    *
	    * @param aUUID UDN of MS
	    * aSession server-side session
	    */            
		CUpnpPathResolver& PathResolverL(const TDesC8& aUUID, CUpnpAVCPEngineSession* aSession);
	    /**
	    * Perform cleanup after closing server-side session.
	    * it checks whether it was the last session used resolver and
	    * destroy resolver if it was true.
	    */    
		void CleanupSessionL( CUpnpAVCPEngineSession* aSession);        
		           
	private: 	// Method from CUpnpAVControlPoint
		/**
		* From CUpnpAVControlPoint handles HTTP messages.
		* @since Series 60 2.0
		* @param aMessage Incoming HTTP message.
		*/
		void HttpResponseReceivedL(CUpnpHttpMessage* aMessage);
		/**
		* From CUpnpAVControlPoint handles actions' responses.
		* @since Series 60 2.0
		* @param aMessage Incoming HTTP message.
		*/
		void ActionResponseReceivedL(CUpnpAction* aAction);        
		/**
		* From CUpnpAVControlPoint handles events (as a response for SUBSCRIBE).
		* @since Series 60 2.0
		* @param aMessage Incoming HTTP message.
		*/        
		void StateUpdatedL(CUpnpService*  aService);
		/**
		* From CUpnpAVControlPoint handles UPnP device discoveries.
		* @since Series 60 2.0
		* @param aDevice Device that is discovered.
		*/
		void DeviceDiscoveredL(CUpnpDevice* aDevice);
		/**
		* From CUpnpAVControlPoint handles UPnP device disappears.
		* @since Series 60 2.0
		* @param aDevice Device that disappeared.
		*/
		void DeviceDisappearedL(CUpnpDevice* aDevice);

	private:
		/**
		* C++ default constructor.
		*/	
		CUpnpAVCPManager(MUpnpAVControlPointObserver* aObserver);
		/**
		* Symbian 2nd phase constructor.
		*/		        
		void ConstructL();       
    /**
    * Return index of action handler for specific session id
    * 
    * @return index if found, else KErrNotFound
    */    
		TInt HandlerBySessionId(TInt aSessionId);
    /**
    * Return index of HTTP handler for specific session id
    * 
    * @return index if found, else KErrNotFound
    */            
		TInt HttpHandlerBySessionId(TInt aSessionId);   
		                          
	private: // Data
		// last command id
		TInt iLastCommandId;
		// array of observers registered for SOAP action
		RPointerArray<MUpnpResponseHandler> iPendingHandlers;
		// array of observer registered for HTTP response
		RPointerArray<MUpnpHttpResponseHandler> iHttpPendingHandlers;
		// array of path resolvers
		RPointerArray<CUpnpPathResolver> iPathResolvers;
    };

#endif  //C_CUPNPAVCPMANAGER_H

