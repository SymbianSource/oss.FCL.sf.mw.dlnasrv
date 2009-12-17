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
* Description:  Resolve path(FS) into id (UPnP)
*
*/


#ifndef C_CUPNPRESOLVEHANDLER_H
#define C_CUPNPRESOLVEHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "upnpresponsehandler.h"
#include "upnpresolverobserver.h"

// FORWARD DECLARATIONS
class CUpnpPathResolver;
class CUpnpAVCPManager;

// CLASS DECLARATION
/**
* Class responsible for translating path into id.
*
*  @since Series 60 3.1
*/
class CUpnpResolveHandler: public CBase, public MUpnpResponseHandler
	{	
	public:
		/**
		* Two-phased constructor.
		*
		* @param aAVCPManager manager
		* @param aResolver path resolver
		* @param aObserver observer to nofity
		*/		
		static CUpnpResolveHandler* NewL(CUpnpAVCPManager& aAVCPManager,
			CUpnpPathResolver& aResolver, MUpnpResolverObserver& aObserver);	
		/**
		* Destructor.
		*/			
		virtual ~CUpnpResolveHandler();
		/**
		* Request for tranlating given path in to id.
		* Method parses path into element and tries to match.
		* Inform use about result by observer,
		*
		* @param aPath path
		*/
		void ResolveL(const TDesC8& aPath);
		
	public:	// Method from observers	
		/**
		* From MUpnpResponseHandler returns session id.
		* It is also defined in MUpnpResponseHandler, MUpnpHttpResponseHandler
		* @since Series 60 3.1
		*/	
		TInt SessionId();
		/**
		* From MUpnpResponseHandler, it is notified when SOAP respone 
		* with relevant session id comes.
		* @param aErrorCode error code
		* @param aAction action with attached respone
		* @since Series 60 3.1
		*/	
		void InterpretL(TInt aErrorCode, CUpnpAction* aAction);
		/**
		* From MUpnpResponseHandler, it is notified when leave appears 
		* during InterpretL
		* @param aErrorCode error code
		* @since Series 60 3.1
		*/
		void RunError(TInt aErrorCode);
				
	private:
		/**
		* C++ default constructor.
		*/	
		CUpnpResolveHandler(CUpnpAVCPManager& aAVCPManager,
			CUpnpPathResolver& aResolver, MUpnpResolverObserver& aObserver);
			
		/**
		* Symbian 2nd phase constructor.
		*/					
		void ConstructL();
		/**
		* Check next element in path, if it is the last notify observer.
		* If matching elemrnt wasn't found inform observer.
		*
		* @param aSendAction indicate whether use browse action 
		*	if not found or inform used about error.
		*/		
		void CheckNextElementL(TBool aSendAction = ETrue);    
		
	private:
		// manager
  	CUpnpAVCPManager& iAVCPManager;	
  	// resolver instance
		CUpnpPathResolver& iResolver;
		// observer
		MUpnpResolverObserver& iObserver;
		// array of path elements
		RPointerArray<TPtrC8> iPathElements;	
		// session id of last send SOAP action
		TInt iSessionId; 	
		// original pathname
		HBufC8* iPath;
		// last matched object id
		HBufC8* iLastId;
		// index of path elements, last checked
		TInt iPathIndex;	
		// index used to no get whole browse result
		TInt iStartIndex;			
		
};

#endif // C_CUPNPRESOLVEHANDLER_H
