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
* Description:  Class responsible for reading file from MS
*
*/


#ifndef C_CUPNPDOWNLOADCOMMAND_H
#define C_CUPNPDOWNLOADCOMMAND_H

//  INCLUDES
#include "upnpcommand.h"
#include "upnpresponsehandler.h"
#include "upnpresolverobserver.h"

// FORWARD DECLARATIONS
class CUpnpPathResolver;

// CLASS DECLARATION
/**
* Class responsible for downloadind files from MS.
*
*  @since Series 60 3.1
*/
class CUpnpDownloadCommand :    public CUpnpCommand, 
                            public MUpnpResponseHandler,
                            public MUpnpHttpResponseHandler, 
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
		static CUpnpDownloadCommand* NewL(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);	       
		/**
		* Destructor.
		*/			
		virtual ~CUpnpDownloadCommand();	       

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
		* From MUpnpHttpResponseHandler, it is notified when HTTP respone 
		* with relevant session id comes.
		* @param aErrorCode error code
		* @param aMessage response message
		* @since Series 60 3.1
		*/			
		void InterpretL(TInt aErrorCode, CUpnpHttpMessage* aMessage);
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
		CUpnpDownloadCommand(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Symbian 2nd phase constructor.
		*/			
		void ConstructL();

	private:
		// path resolver
		CUpnpPathResolver* iPathResolver;
		// object id taken from path specified by request				
		HBufC8* iObjectId;	
		// path to destinnation file (local file in cache directory)        
    HBufC8* iDestPath;
    // flag indicates that RANGE was used
    TBool iUseRange;
    // structure keeps information about position of file to read
    TFilePosition iPosition;        
	};

#endif //C_CUPNPDOWNLOADCOMMAND_H
