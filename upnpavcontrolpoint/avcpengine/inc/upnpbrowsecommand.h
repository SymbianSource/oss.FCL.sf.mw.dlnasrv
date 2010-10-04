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
* Description:  Class responsible for browsing in MS 
*
*/


#ifndef C_CUPNPBROWSECOMMAND_H
#define C_CUPNPBROWSECOMMAND_H

//  INCLUDES
#include "upnpcommand.h"
#include "upnpresponsehandler.h"
#include "upnpresolverobserver.h"


// FORWARD DECLARATIONS
class CUpnpPathResolver;

// CLASS DECLARATION
/**
* Class responsible for browsing MS structure.
* It perfroms request related to directory listings, 
* file/directory attributes
*
*  @since Series 60 3.1
*/
class CUpnpBrowseCommand : public CUpnpCommand, public MUpnpResponseHandler, 
                                        public MUpnpResolverObserver,
                                        public MUpnpHttpResponseHandler
	{
	public:	
		/**
		* Two-phased constructor.
		*
		* @param aAVCPManager manager
		* @param aSession server-side session
		* @param aMessage message
		*/	
		static CUpnpBrowseCommand* NewL(CUpnpAVCPManager& aAVCPManager, 
	  	CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Destructor.
		*/	       
		virtual ~CUpnpBrowseCommand();	  
		   
		/**
		* Cleanup Mechanism
		*/
		static void CleanupArray(TAny* aDirs);
			
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
		* From CUpnpCommand set result from previous performed command.
		* It sets result from directory listing.
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
		CUpnpBrowseCommand(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Symbian 2nd phase constructor.
		*/			
		void ConstructL();
		/**
		* Helper function, iterate over list to find object with fiven name.
		* If it find correct object set its attribute to internal member of class.
		*
		* @param aList list infomrmation about object
		* @param aName name of object to find
		* @return ETrue - found, else EFalse
		*/
		TBool SetAttForObject(RArray<TFileInfo>& aList, const TDesC8& aName);
		/**
		* Prepare message with HTTP-HEAD and send it to specific address.
		* 
		* @param aUrl destination address 		
		*/
		void SendHeadRequestL(const TDesC8& aUrl); 

	private:
		// path resolver
		CUpnpPathResolver* iPathResolver;
		// object id taken from path specified by request				
		HBufC8* iObjectId;
		// object name taken from path specified by request						
		HBufC8* iObjectName;
		// start index, value used to not get whole browse response at once
		TInt iStartIndex;
		// structure used for keep information about file/directory attribute
		TFileInfo iAttr;
		// array of directory elements (files/directories)
    	RArray<TFileInfo> iResult;
};

#endif // C_CUPNPBROWSECOMMAND_H
