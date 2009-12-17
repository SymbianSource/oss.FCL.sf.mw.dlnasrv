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
* Description:  Base class for all commands
*
*/


#ifndef C_CUPNPCOMMAND_H
#define C_CUPNPCOMMAND_H

//  INCLUDES
#include <e32base.h>
#include "upnpavcpenginecommon.h"
#include "upnpmdebug.h"

// FORWARD DECLARATIONS
class CUpnpAVCPManager;
class CUpnpAVCPEngineSession;
class RMessage2;

// CLASS DECLARATION
/**
*  Base abstract class defines common set of methods for all commands
*
*  @since Series 60 3.1
*/ 
class CUpnpCommand : public CBase
	{
	public:  // Constructors and destructor
		/**
		* Two-phased constructor.
		* Based on type of request it creates instance of relevant derived class
		*
		* @param aAVCPManager manager
		* @param aSession server-side session
		* @param aMessage message
		*/
		static CUpnpCommand* NewL(CUpnpAVCPManager& aAVCPManager, 
													CUpnpAVCPEngineSession& aSession, 
													const RMessage2& aMessage);        
		/**
		* Destructor.
		*/
		virtual ~CUpnpCommand();

	public: // New functions    
		/**
		* Function defines operation to perform by derived command
		*/		    
		virtual void ExecuteL() = 0;    
		/**
		* Function set result from previous performed command.
		* Command was performed and sent information about size of result to client-side.
		* Base on command id session find corresponding instance of command and set result.
		*/				    
		virtual void SetResultL(const RMessage2& aMessage) = 0;    
		/**
		* Return command unique id
		*
		* @return command id;
		*/    
		TInt Id();
		/**
		* Return session id
		*
		* @return session id;
		*/  		
		TInt SessionId();
		/**
		* Function defines define default behaviour when leave appear during asynchrounous operation
		*/		    
		virtual void RunError(TInt aErrorCode);  		
		
	protected:
		/**
		* C++ default constructor.
		*/
		CUpnpCommand(CUpnpAVCPManager& aAVCPManager, 
							CUpnpAVCPEngineSession& aSession, 
							const RMessage2& aMessage);
		/**
		* Symbian 2nd phase constructor.
		*/
		void BaseConstructL();        
		/**
		* Read descriptor from message connected with this command
		* @since Series 60 3.1
		* @param aIndex index in message
		* @return read value
		*/
		HBufC8* ReadDes8FromMessageLC(TInt aIndex);      
		/**
		* Read descriptor from message connected with this command
		* @since Series 60 3.1
		* @param aIndex index in message
		* @return read value
		*/		  
		HBufC* ReadDes16FromMessageLC(TInt aIndex);
                
	protected:
    // session id used for registration in CUpnpAVCPManager    
		TInt iSessionId;
		// command id used for match with command requesting for results
		TInt iId;
		// type of request
		TAVCPEngineRqst iType; 
    // AVCP manager used for sending request            
		CUpnpAVCPManager& iAVCPManager;
		// server-side session 
		CUpnpAVCPEngineSession& iSession;
		// processing message
		RMessage2 iMessage;        
	};

#endif      // C_CUPNPCOMMAND_H 
            
// End of File