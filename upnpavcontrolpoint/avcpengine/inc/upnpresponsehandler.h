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
* Description:  Observers which receive notifications about responses of SOAP actions and HTTP requests
*
*/


#ifndef C_CUPNPRESPONSEHANDLER_H
#define C_CUPNPRESPONSEHANDLER_H

// FORWARD DECLARATIONS
class CUpnpAction;
class CUpnpHttpMessage;

// CLASS DECLARATION
/**
*  This observer class has to be implemented to receive notification 
*  about response on SOAP action.
*	 The observer is used by CUpnpAVCPManager, it use session id 
*  (taken from SessionId method) for match incoming response with corresponding
*  handler instance. After notify handler, handler is removed from 
*  registered handlers.
*
*  @see CUpnpAVCPManager
*  @since Series 60 3.1
*/
class MUpnpResponseHandler
	{
	public: // New functions 
		/**
		* This callback function is notified when Soap respone
		* with relevant session id (@see SessionId) comes.
		* @param aErrorCode error code
		* @param aMessage response message
		* @since Series 60 3.1
		*/		
		virtual void InterpretL(TInt aErrorCode, CUpnpAction* aAction) = 0;
		/**
		* Function returns session id that identifies on which response handler is waiting
		*
		* @return session id
		*/		
		virtual TInt SessionId() = 0;
		/**
		* This callback function is notified when leave appears during execution of InterpretL
		* @param aErrorCode error code of leave
		*/		
		virtual void RunError(TInt aErrorCode) = 0;
		

	};

/**
*  This observer class has to be implemented to receive notification 
*  about response on HTTP requests.
*	 The observer is used by CUpnpAVCPManager, it use session id 
*  (taken from SessionId method) for match incoming response with corresponding
*  handler instance. After notify handler, handler is not removed from 
*  registered handlers. Handler instance has to use unregister itself.
*
*  @see CUpnpAVCPManager
*  @since Series 60 3.1
*/
class MUpnpHttpResponseHandler
	{
	public: // New functions
		/**
		* This callback function is notified when HTTP respone
		* with relevant session id (@see SessionId) comes.
		* @param aErrorCode error code
		* @param aMessage response message
		* @since Series 60 3.1
		*/	
		virtual void InterpretL(TInt aErrorCode, CUpnpHttpMessage* aAction) = 0;
		/**
		* Function returns session id that identifies on which response handler is waiting
		*
		* @return session id
		*/
		virtual TInt SessionId() = 0;
		/**
		* This callback function is notified when leave appears during execution of InterpretL
		* @param aErrorCode error code of leave
		*/		
		virtual void RunError(TInt aErrorCode) = 0;		
	};
#endif // C_CUPNPRESPONSEHANDLER_H
