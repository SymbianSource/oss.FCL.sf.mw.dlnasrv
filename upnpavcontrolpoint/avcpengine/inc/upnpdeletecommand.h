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
* Description:  Delete file/directory in MS
*
*/


#ifndef C_CUPNPDELETECOMMAND_H
#define C_CUPNPDELETECOMMAND_H

//  INCLUDES
#include "upnpcommand.h"
#include "upnpresponsehandler.h"
#include "upnpresolverobserver.h"

// FORWARD DECLARATIONS
class CUpnpPathResolver;

// CLASS DECLARATION
/**
* Class responsible for deleting files/directories in MS .
*
*  @since Series 60 3.1
*/
class CUpnpDeleteCommand : public CUpnpCommand, public MUpnpResponseHandler, 
                                        public MUpnpResolverObserver
	{
	public:	
		/**
		* Two-phased constructor.
		*
		* @param aAVCPManager manager
		* @param aSession server-side session
		* @param aMessage message
		*/	
		static CUpnpDeleteCommand* NewL(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Destructor.
		*/		       
		virtual ~CUpnpDeleteCommand();	       


	public:	// Method from CUpnpCommand
		/**
		* From CUpnpCommand returns session id.
		* It is also defined in MUpnpResponseHandler, MUpnpHttpResponseHandler
		* @since Series 60 3.1
		*/	
		TInt SessionId();
		/**
		* From CUpnpCommand implement logic for leaves that appear during InterpretL.
		* It is also defined in MUpnpResponseHandler, MUpnpHttpResponseHandler
		* @since Series 60 3.1
		*/	
		void RunError(TInt aErrorCode);		
		/**
		* From CUpnpCommand perfroms operation connected with command
		* @since Series 60 3.1
		*/			
		void ExecuteL();
		/**
		* From CUpnpCommand it is empty implementation.
		* @since Series 60 3.1
		*/				
		void SetResultL(const RMessage2& aMessage);	
		
	public:	// Method from observers	
		/**
		* From MUpnpResponseHandler, it is notified when SOAP respone 
		* with relevant session id comes.
		* @param aErrorCode error code
		* @param aAction action with attached respone
		* @since Series 60 3.1
		*/	
		void InterpretL(TInt aErrorCode, CUpnpAction* aAction);
		/**
		* From MUpnpResolverObserver, it is notified after translation path into id.
		* 
		* @param aErrorCode error code (KErrNone if Ok)
		* @param aIdElement object describing searched element
		* @since Series 60 3.1
		*/		
		void ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement);					
	
	private:
		/**
		* C++ default constructor.
		*/	
		CUpnpDeleteCommand(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Symbian 2nd phase constructor.
		*/				
		void ConstructL();
		
	private:
		// path resolver
		CUpnpPathResolver* iPathResolver;

};

#endif // C_CUPNPDELETECOMMAND_H
