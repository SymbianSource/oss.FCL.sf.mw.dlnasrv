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
* Description:  Implementation of server-side session
*
*/


#ifndef C_CUPNPAVCPENGINESESSION_H
#define C_CUPNPAVCPENGINESESSION_H

// INCLUDES
#include <e32base.h>
#include "upnpavcpengine.h"
#include "upnpavcpenginecommon.h"

// FORWARD DECLARATIONS
class CUpnpAVCPManager;
class CUpnpCommand;

// CLASS DECLARATION
/**
* Implementation of server-side session
*
*  @since Series 60 3.1
*/
class CUpnpAVCPEngineSession : public CSession2
	{
	public: // New methods
		/**
		* Two-phased constructor.
		*
		* @param aClient client thread
		* @param aServer server
		* @param aManager AVCP manager
		*/	
		static CUpnpAVCPEngineSession* NewL( const RThread& aClient, 
			CUpnpAVCPEngine& aServer, CUpnpAVCPManager& aManager);
		/**
		* Destructor.
		*/		
		virtual ~CUpnpAVCPEngineSession();

	public: // From CSession
		/**
		* From CSession invoked when requests from client come.
		*/
		void ServiceL( const RMessage2& aMessage );
    
	public:    
		/**
		* Remove command from array of active command
		*/
    void RemoveCommand(CUpnpCommand* aCommand);
    /**
    * Get index of command in array based on command id
    */
    TInt CommandById(TInt aId);

	private: // New methods
		/**
		* C++ default constructor.
		*/	
    CUpnpAVCPEngineSession( const RThread& aClient, 
    					    CUpnpAVCPEngine& aServer,
                            CUpnpAVCPManager& aManager);
		/**
		* Symbian 2nd phase constructor.
		*/                            
    void ConstructL() ;
		/**
		* Panic client thread
		*
		* @param aMessage message to be panic
		* @param aReason panic code
		*/
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;
   
	private: // Data
		// server 
    CUpnpAVCPEngine& iAVCPEngine;
    // manager 
    CUpnpAVCPManager& iAVCPManager;
    // list of active command
    RPointerArray<CUpnpCommand> iCommandList;
    };

#endif //C_CUPNPAVCPENGINESESSION_H